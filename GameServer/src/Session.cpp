#include <iostream>
#include <assert.h>
#include <winsock2.h>
#include <mswsock.h>
#include "Session.h"
#include "SessionPool.h"
#include "SendBuffer.h"
#include "RecvBuffer.h"
#include "net_msg.h"
#include "NetworkObject.h"
#include "MsWinsockUtil.h"

using namespace std;

//=============================================================================================================================
/**
	@remarks
			Session构造函数.\n
			创建发送缓冲，接收缓冲区，设置超时时间。
	@param	dwSendBufferSize
			发送缓冲区大小
	@param	dwRecvBufferSize
			接收缓冲区大小
	@param	dwTimeOut
			如果在超时时间范围内，进来的消息不能访问这个关闭的连接。
			如果这个值是0，不要挂起这个连接，也不要发送消息。
*/
//=============================================================================================================================
Session::Session( DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize, DWORD dwTimeOut )
{
	m_pSendBuffer = new SendBuffer;
	m_pSendBuffer->Create( dwSendBufferSize, dwMaxPacketSize );

	m_pRecvBuffer = new RecvBuffer;
	m_pRecvBuffer->Create( dwRecvBufferSize, dwMaxPacketSize );

	m_dwTimeOut			= dwTimeOut;
	m_socket			= INVALID_SOCKET;
	m_bAcceptSocket		= FALSE;
}

Session::~Session()
{
	CloseSocket();

	if( m_pSendBuffer )		delete m_pSendBuffer;
	if( m_pRecvBuffer )		delete m_pRecvBuffer;
}

//=============================================================================================================================
/**
	@remarks
			初始化Session
	@param	socket
			给Session赋SOCKET句柄
	@param	sockaddr
			SOCKET地址赋予会话
	@retval	BOOL
			如果传入的SOCKET句柄非法，返回FALSE
*/
//=============================================================================================================================
VOID Session::Init()
{
	m_pSendBuffer->Clear();
	m_pRecvBuffer->Clear();

	ResetKillFlag();

	m_bDisconnectOrdered = FALSE;
}

VOID Session::Reuse( HANDLE hIOCP )
{
	//断开已经在使用的连接
	ZeroMemory( &m_disconnectIoData, sizeof(OVERLAPPEDEX) );
	m_disconnectIoData.dwOperationType	= DISCONNECT_POSTED;

	//如果  TransmitFile失败，就要使用IOCP的强制方式
	if( !MsWinsockUtil::m_lpfnTransmitFile( m_socket, 0, 0, 0, &m_disconnectIoData, 0, TF_DISCONNECT | TF_REUSE_SOCKET )
		&& WSAGetLastError() != WSA_IO_PENDING )
	{		
		CloseSocket();		
		PostQueuedCompletionStatus( hIOCP, 0, (ULONG_PTR)this, &m_disconnectIoData );
	}
}

//=============================================================================================================================
/**
	@remarks
			写入到发送缓冲区.
	@param	pMsg
			包指针
	@param	wSize
			除去数据包 包头之外的大小
	@retval	BOOL
			如果成功写入缓冲区就返回成功，发送缓冲区写满就返回FALSE.
*/
//=============================================================================================================================
bool Session::Send( BYTE *pMsg, DWORD dwSize )//  [7/15/2009 哈达]
{
	PACKET_HEADER header;

	// 设置头大小
	header.size = (WORD)dwSize;
	
	// Write to the Send buffer
	if( m_pSendBuffer->Write( &header, pMsg ) == FALSE )
	{
		OnLogString( "发送缓冲区满，断开连接." );
		Remove();
		return false;
	}

	return true;
}

bool Session::SendEx( DWORD dwNumberOfMessages, BYTE **ppMsg, WORD *pwSize )
{
	assert( !IsBadReadPtr( ppMsg, sizeof(ppMsg) * dwNumberOfMessages ) );
	assert( !IsBadReadPtr( pwSize, sizeof(pwSize) * dwNumberOfMessages ) );

	PACKET_HEADER header;

	// 设置头大小
	header.size = 0;
	DWORD i;
	for( i = 0; i < dwNumberOfMessages; ++i )
	{
		header.size += pwSize[i];
	}

	// 写发送缓冲区(HEADER)
	if( !m_pSendBuffer->Write( (BYTE*)&header, sizeof(PACKET_HEADER) ) )
	{
		OnLogString( "发送缓冲区满，断开连接." );
		Remove();
		return false;
	}

	// 写发送缓冲区(dwNumberOfMessages one message)
	for( i = 0; i < dwNumberOfMessages; ++i )
	{
		if( !m_pSendBuffer->Write( ppMsg[i], pwSize[i] ) )
		{
			OnLogString( "发送缓冲区满，断开连接." );
			Remove();
			return false;
		}
	}

	return true;
}

BOOL Session::ProcessRecvdPacket( DWORD dwMaxPacketSize )
{
	BYTE			*pPacket;
	PACKET_HEADER	*pHeader;

	// 从缓冲区取出数据，然后处理.
	while( pPacket = GetRecvBuffer()->GetFirstPacketPtr() )
	{
		pHeader = (PACKET_HEADER*)pPacket;

		if( pHeader->size <= 0 )
		{
			OnLogString( "包最小限制出错! size(%d) category(%d) protocol(%d)\n",
				pHeader->size, *( pPacket + sizeof(PACKET_HEADER) ), *( pPacket + sizeof(PACKET_HEADER) + 1 ) );
			return FALSE;
		}

		if( pHeader->size + sizeof(PACKET_HEADER) > dwMaxPacketSize )
		{
			OnLogString( "包大小超出限制 : Cur[%d]/Max[%d] Category[%d] Protocol[%d]", 
				pHeader->size + sizeof(PACKET_HEADER), dwMaxPacketSize, *( pPacket + sizeof(PACKET_HEADER) ), *( pPacket + sizeof(PACKET_HEADER) + 1 ) );

			OnLogString( "超出包大小 (前一个包): Category[%d] Protocol[%d] Size[%d]", 
				m_iCategory, m_iProtocol, m_iSize );
			return FALSE;
		}

		m_pNetworkObject->OnRecv( pPacket + sizeof(PACKET_HEADER), pHeader->size );

		GetRecvBuffer()->RemoveFirstPacket( sizeof(PACKET_HEADER) + pHeader->size );

		ResetTimeOut();

		m_iCategory = *( pPacket + sizeof(PACKET_HEADER) );
		m_iProtocol = *( pPacket + sizeof(PACKET_HEADER) + 1 );
		m_iSize = pHeader->size + sizeof(PACKET_HEADER);
	}

	return TRUE;
}

//=============================================================================================================================
/**
	@remarks
			创建 send 到IOCP.
	@retval BOOL
			- 发送缓冲区为空或者发送成功，返回为TRUE.
			- 发送失败，返回FALSE.
*/
//=============================================================================================================================
BOOL Session::PreSend()
{
	WSABUF wsabuf;

	if( GetSendBuffer()->GetSendParam( (BYTE**)&wsabuf.buf, (int&)wsabuf.len ) == FALSE )
	{
		return TRUE;
	}

	ZeroMemory( &m_sendIoData, sizeof(OVERLAPPEDEX) );

	m_sendIoData.dwOperationType = SEND_POSTED;
	
	int ret = WSASend( GetSocket(), &wsabuf, 1, &m_sendIoData.dwIoSize, m_sendIoData.dwFlags, &m_sendIoData, NULL );

	if( ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING )
	{
		return FALSE;
	}

	return TRUE;
}

//=============================================================================================================================
/**
	@remarks
			创建 recv 到IOCP.
	@retval BOOL
			recv失败，返回FALSE.
*/
//=============================================================================================================================
BOOL Session::PreRecv()
{
	WSABUF wsabuf;

	m_pRecvBuffer->GetRecvParam( (BYTE**)&wsabuf.buf, (int&)wsabuf.len );

	ZeroMemory( &m_recvIoData, sizeof(OVERLAPPEDEX) );

	m_recvIoData.dwOperationType = RECV_POSTED;

	int ret = WSARecv( GetSocket(), &wsabuf, 1, &m_recvIoData.dwIoSize, &m_recvIoData.dwFlags, &m_recvIoData, NULL ); 

	if( ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Session::PreAccept( SOCKET listenSocket )
{
	DWORD	dwBytes		= 0;
	int		nLen		= 0;
	BYTE	*pRecvBuf	= NULL;

	if( GetSocket() == INVALID_SOCKET )
	{
		SetSocket( CreateSocket() );
	}

	Init();

	//在Session中获取Recv 指针.
	GetRecvBuffer()->GetRecvParam( &pRecvBuf, nLen );

	//创建AcceptEx
	ZeroMemory( &m_acceptIoData, sizeof(OVERLAPPEDEX) );
	m_acceptIoData.dwOperationType	= ACCEPT_POSTED;
	m_acceptIoData.pSession			= this;				

	

	//添加AcceptEx SOCKET
	int nRet = 	MsWinsockUtil::m_lpfnAccepteEx(	listenSocket, 
												GetSocket(), 
												pRecvBuf, 
												0, 
												sizeof(SOCKADDR_IN) + 16,
												sizeof(SOCKADDR_IN) + 16,
												&dwBytes,
												&m_acceptIoData );

	if( nRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING ) 
	{
		return FALSE;
	}

	return TRUE;
}

SOCKET Session::CreateSocket() 
{
	int nRet		= 0;
	int nZero		= 0;

	SOCKET newSocket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED ); 

	if( newSocket == INVALID_SOCKET ) 
	{
		return newSocket;
	}

	/*
	nZero	= 0;
	nRet	= setsockopt( newSocket, SOL_SOCKET, SO_SNDBUF, (char *)&nZero, sizeof(nZero) );

	if( nRet == SOCKET_ERROR ) 
	{
		return INVALID_SOCKET;
	}
	*/

	return newSocket;
}

VOID Session::BindNetworkObject( NetworkObject *pNetworkObject )
{
	m_pNetworkObject = pNetworkObject;
	pNetworkObject->SetSession( this );
}

VOID Session::UnbindNetworkObject()
{
	if( m_pNetworkObject == NULL )
	{
		return;
	}
	m_pNetworkObject->SetSession( NULL );

	m_pNetworkObject = NULL;
}

VOID Session::OnAccept()
{
	ResetKillFlag();

	ResetTimeOut();

	m_pNetworkObject->OnAccept( GetIndex() );

	//printf("[Session::OnAccept][%d] m_bRemove=%d\n", (int)GetSocket(), (int)m_bRemove);
}

VOID Session::OnConnect( BOOL bSuccess )
{
	Init();

	NetworkObject *pNetworkObject = m_pNetworkObject;

	if( !bSuccess )
	{		
		UnbindNetworkObject();
	}

	pNetworkObject->OnConnect( bSuccess, GetIndex() );
}

VOID Session::OnLogString( char *pszLog, ... )
{
	if( !m_pNetworkObject ) return;

	char		szBuffer[512] = "";
	va_list		pArguments;

	va_start( pArguments, pszLog );
	vsprintf_s( szBuffer, pszLog, pArguments );
	va_end( pArguments );

	printf( "%s(%s)\n", szBuffer, GetIP() );

	m_pNetworkObject->OnLogString( szBuffer );
}

VOID Session::Disconnect( BOOL bGracefulDisconnect )
{ 
	if( bGracefulDisconnect ) 
	{ 
		//printf("[REMOVE][%d] bGracefulDisconnect\n", (int)GetSocket()); 
		Remove(); 
	} 
	else 
	{
		m_bDisconnectOrdered = TRUE; 
	}
}

//////////////////////////////////////////////////////////////////////////
/*
		PostQueuedCompletionStatus函数，向每个工作者线程都发送—个特殊的完成数据包。该函数会指示每个线程都“立即结束并退出”.下面是PostQueuedCompletionStatus函数的定义： 
		BOOL PostQueuedCompletionStatus( 
										HANDLE CompletlonPort, 
										DW0RD  dwNumberOfBytesTrlansferred, 
										DWORD  dwCompletlonKey, 
										LPOVERLAPPED lpoverlapped, 

										); 

		其中，CompletionPort参数指定想向其发送一个完成数据包的完成端口对象。而就dwNumberOfBytesTransferred,dwCompletionKey和lpOverlapped这三个参数来说.每—个都允许我们指定—个值,直接传递给GetQueuedCompletionStatus函数中对应的参数。这样—来。—个工作者线程收到传递过来的三个GetQueuedCompletionStatus函数参数后，便可根据由这三个参数的某一个设置的特殊值，决定何时应该退出。例如,可用dwCompletionPort参数传递0值,而—个工作者线程会将其解释成中止指令。一旦所有工作者线程都已关闭,便可使用CloseHandle函数,关闭完成端口。最终安全退出程序。 
		PostQueuedCompletionStatus函数提供了一种方式来与线程池中的所有线程进行通信。如，当用户终止服务应用程序时，我们想要所有线程都完全利索地退出。但是如果各线程还在等待完成端口而又没有已完成的I/O 请求，那么它们将无法被唤醒。 
		通过为线程池中的每个线程都调用一次PostQueuedCompletionStatus，我们可以将它们都唤醒。每个线程会对GetQueuedCompletionStatus的返回值进行检查，如果发现应用程序正在终止，那么它们就可以进行清理工作并正常地退出。 
 */
//////////////////////////////////////////////////////////////////////////