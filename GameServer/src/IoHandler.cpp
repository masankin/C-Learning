#include <winsock2.h>
#include <assert.h>
#include "IoHandler.h"
#include "Acceptor.h"
#include "Connector.h"
#include "SessionList.h"
#include "SessionPool.h"
#include "net_msg.h"
#include "Session.h"
#include "SendBuffer.h"
#include "RecvBuffer.h"
#include "IOCPServer.h"
#include "NetworkObject.h"


//=============================================================================================================================
/**
	@remarks
			I/O completion线程处理
	@param	param
			IOCP 服务器指针
*/
//=============================================================================================================================
unsigned __stdcall io_thread( LPVOID param )
{
	IoHandler *pIoHandler = (IoHandler*)param;

	BOOL			bSuccess = FALSE;
	DWORD			dwIoSize = 0;

	Session			*pSession = NULL;
	OVERLAPPEDEX	*pOverlappedEx = NULL;
	
	try
	{
		while( 1 )
		{
			bSuccess = GetQueuedCompletionStatus( pIoHandler->m_hIOCP, &dwIoSize, (LPDWORD)&pSession,(LPOVERLAPPED*)&pOverlappedEx, INFINITE );

			// 如果会话为NULL,强制退出 
			if( pSession == NULL ) break;

			// GetQueuedCompletionStatus() 返回失败就移除该会话。啊 角菩牢 版快 趣篮 技记 力芭
			if( !bSuccess )
			{
				if( GetLastError() == ERROR_NETNAME_DELETED )
				{
					// 移除关闭的SOCKET
					//printf("[REMOVE][%d] GetLastError() == ERROR_NETNAME_DELETED\n", (int)pSession->GetSocket());
					pSession->Remove();
				}
				continue;
			}


			if( pOverlappedEx->dwOperationType != DISCONNECT_POSTED && dwIoSize == 0 )
			{
				//printf("[REMOVE][%d] dwIoSize == 0\n", (int)pSession->GetSocket());
				pSession->Remove();
				continue;
			}

			// Completion 处理
			switch( pOverlappedEx->dwOperationType )
			{
			case SEND_POSTED:
				pSession->GetSendBuffer()->Completion( dwIoSize );
				break;

			case RECV_POSTED:			
				pSession->GetRecvBuffer()->Completion( dwIoSize );
				// 下一个 Recv	
				if( !pSession->PreRecv() )	
				{
					//printf("[REMOVE][%d] pSession->PreRecv()\n", (int)pSession->GetSocket());
					pSession->Remove();
				}
				break;

			case DISCONNECT_POSTED:
				pSession->PreAccept( pIoHandler->m_pAcceptor->GetListenSocket() );
				break;
			}
		}

	}
	catch(...)
	{
		int i =0;
		i++;
	}
	
	

	return 0;
}

IoHandler::IoHandler()
{
	m_pAcceptSessionPool		= NULL;
	m_pConnectSessionPool		= NULL;
	m_pAcceptor					= NULL;
	m_pConnector				= NULL;
	m_pActiveSessionList		= NULL;
	m_pAcceptedSessionList		= NULL;
	m_pConnectSuccessList		= NULL;
	m_pConnectFailList			= NULL;
	m_pTempList					= NULL;
	m_numActiveSessions			= 0;
	m_bShutdown					= FALSE;
	for( DWORD i = 0; i < MAX_IO_WORKER_THREAD; ++i ) m_hIoThread[i] = NULL;

	m_fnCreateAcceptedObject	= NULL;
	m_fnDestroyAcceptedObject	= NULL;
	m_fnDestroyConnectedObject	= NULL;
}

IoHandler::~IoHandler()
{
	if( !m_bShutdown ) Shutdown();

	for( DWORD i = 0; i < m_numIoThreads; ++i )
	{
		if( m_hIoThread[i] ) CloseHandle( m_hIoThread[i] );
	}

	if( m_pAcceptSessionPool )		delete m_pAcceptSessionPool;
	if( m_pConnectSessionPool )		delete m_pConnectSessionPool;
	if( m_pAcceptor )				delete m_pAcceptor;
	if( m_pConnector )				delete m_pConnector;
	if( m_pActiveSessionList )		delete m_pActiveSessionList;
	if( m_pAcceptedSessionList )	delete m_pAcceptedSessionList;
	if( m_pConnectSuccessList )		delete m_pConnectSuccessList;
	if( m_pConnectFailList )		delete m_pConnectFailList;
	if( m_pTempList )				delete m_pTempList;
}

VOID IoHandler::Init( IOCPServer *pIOCPServer, LPIOHANDLER_DESC lpDesc )
{
	m_pIOCPServer	= pIOCPServer;

	// 回调函数指针是否合法
	assert( !IsBadReadPtr( lpDesc->fnCreateAcceptedObject, sizeof(lpDesc->fnCreateAcceptedObject) ) );
	assert( !IsBadReadPtr( lpDesc->fnDestroyAcceptedObject, sizeof(lpDesc->fnDestroyAcceptedObject) ) );
	assert( !IsBadReadPtr( lpDesc->fnDestroyConnectedObject, sizeof(lpDesc->fnDestroyConnectedObject) ) );
	
	// 保存回调函数指针
	m_fnCreateAcceptedObject		= lpDesc->fnCreateAcceptedObject;
	m_fnDestroyAcceptedObject		= lpDesc->fnDestroyAcceptedObject;
	m_fnDestroyConnectedObject		= lpDesc->fnDestroyConnectedObject;

	m_dwKey				= lpDesc->dwIoHandlerKey;
	
	// 创建Session
	m_pActiveSessionList	= new SessionList;
	m_pAcceptedSessionList	= new SessionList;
	m_pConnectSuccessList	= new SessionList;
	m_pConnectFailList		= new SessionList;
	m_pTempList				= new SessionList;

	// 最大连接数
	m_dwMaxAcceptSession	= lpDesc->dwMaxAcceptSession;

	// Generate the session pool
	m_pAcceptSessionPool	= new SessionPool( lpDesc->dwMaxAcceptSession + EXTRA_ACCEPTEX_NUM,
											   lpDesc->dwSendBufferSize,
											   lpDesc->dwRecvBufferSize,
											   lpDesc->dwMaxPacketSize,
											   lpDesc->dwTimeOut,
											   1,
											   TRUE );

	m_pConnectSessionPool	= new SessionPool( lpDesc->dwMaxConnectSession,
											   lpDesc->dwSendBufferSize,
											   lpDesc->dwRecvBufferSize,
											   lpDesc->dwMaxPacketSize,
											   lpDesc->dwTimeOut,
											   m_pAcceptSessionPool->GetLength() + 1,
											   FALSE );

	
	// 设置包的最大长度
	assert( lpDesc->dwMaxPacketSize > sizeof(PACKET_HEADER) );
	m_dwMaxPacketSize =	lpDesc->dwMaxPacketSize;
  
	// 创建 IOCP 
	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );

	unsigned threadID;

	//IO worker thread creation
	assert( lpDesc->dwNumberOfIoThreads <= MAX_IO_WORKER_THREAD );
	m_numIoThreads = lpDesc->dwNumberOfIoThreads;
	for( DWORD i = 0; i < m_numIoThreads; ++i )
	{
		m_hIoThread[i] = (HANDLE)_beginthreadex( NULL, 0, io_thread, (LPVOID)this, 0, &threadID );
	}

	// 创建 Acceptor
	m_pAcceptor		= new Acceptor;
	m_pAcceptor->Init( this, lpDesc->dwNumberOfAcceptThreads );

	// 创建Connector	
	m_pConnector	= new Connector;
	m_pConnector->Init( this, lpDesc->dwNumberOfConnectThreads );
}

DWORD IoHandler::Connect( NetworkObject *pNetworkObject, char *pszIP, WORD wPort )
{
	// 如果SOCKET已经和 NetworkObject 绑定，直接返回。
	if( pNetworkObject->m_pSession != NULL ) return 0;

	//创建SOCKET
	SOCKET socket = WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );

	// 设置地址
	SOCKADDR_IN addr;

	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= inet_addr( pszIP );
	addr.sin_port			= htons( wPort );

	// 申请创建 Session
	Session *pSession = AllocConnectSession();
	assert( pSession != NULL && "不能够和Session建立连接，请核实 dwMaxConnectSession " );

	// 设置Session的SOCKET和地址
	pSession->SetSocket( socket );
	pSession->SetSockAddr( addr );

	// Session和NetworkObject进行绑定
	assert( pNetworkObject != NULL );
	pSession->BindNetworkObject( pNetworkObject );

	m_pConnector->Connect( pSession );

	return pSession->GetIndex();
}

//=============================================================================================================================
/**
	@remarks
			创建SOCKET,监听
	@retval	BOOL
			如果创建SOCKET失败，返回FALSE.
	@param	pIP
			使用的IP地址
	@param	wPort
			端口号
*/
//=============================================================================================================================
BOOL IoHandler::StartListen( char *pIP, WORD wPort )
{
	assert( m_dwMaxAcceptSession > 0 );

	// 如果成功处理，返回TRUE
	if( IsListening() ) return TRUE;

	if( !m_pAcceptor->StartListen( pIP, wPort ) )
	{
		printf( "Listen socket creation failed.\n" );
		return FALSE;
	}

	Session *pSession;

	while( pSession = AllocAcceptSession() )
	{
		pSession->PreAccept( m_pAcceptor->GetListenSocket() );
	}

	return TRUE;
}

BOOL IoHandler::IsListening()
{
	return m_pAcceptor->IsListening();
}

//=============================================================================================================================
/**
	@remarks
			所有的会话都是通过会话池进行操作的。
*/
//=============================================================================================================================

Session* IoHandler::AllocAcceptSession()
{
	return m_pAcceptSessionPool->Alloc();
}

Session* IoHandler::AllocConnectSession()
{
	return m_pConnectSessionPool->Alloc();
}

VOID IoHandler::FreeSession( Session *pSession )
{
	//printf("[FreeSession][%d]\n", (int)pSession->GetSocket());
	pSession->CloseSocket();
	pSession->Init();
	if( pSession->IsAcceptSocket() )
	{
		m_pAcceptSessionPool->Free( pSession );
	}
	else
	{
		m_pConnectSessionPool->Free( pSession );
	}
}

VOID IoHandler::ReuseSession( Session *pSession )
{
	//printf("[ReuseSession][%d]\n", (int)pSession->GetSocket());
	pSession->Reuse( m_hIOCP );
}

//=============================================================================================================================
/**
	@remarks
			成功访问会话。处理会话列表。
	@par
			注册会话，ConnectSuccessList中的Session调用OnConnect(TRUE) 会添加Session 到ActiveSessionList中。
*/
//=============================================================================================================================
VOID IoHandler::ProcessConnectSuccessList()
{
	SESSION_LIST_ITER		it;
	Session					*pSession;

	
	// 成功建立连接的临时会话列表，
	m_pConnectSuccessList->Lock();
	m_pTempList->splice( m_pTempList->end(), *m_pConnectSuccessList );
	m_pConnectSuccessList->Unlock();

	for( it = m_pTempList->begin(); it != m_pTempList->end(); )
	{
		pSession = *it;

		// IOCP句柄注册
		CreateIoCompletionPort( (HANDLE)pSession->GetSocket(), m_hIOCP, (ULONG_PTR)pSession, 0 );

		if( pSession->PreRecv() )
		{
			pSession->OnConnect( TRUE );
			++it;
		}
		else
		{
			m_pTempList->erase( it++);

			//释放会话，返回到会话池
			FreeSession( pSession );

			pSession->OnConnect( FALSE );
		}
	}

	if( !m_pTempList->empty() )
	{
		
		m_numActiveSessions += (DWORD)m_pTempList->size();

		//添加成功连接 到   ActiveSessionList中
		m_pActiveSessionList->Lock();
		m_pActiveSessionList->splice( m_pActiveSessionList->end(), *m_pTempList );
		m_pActiveSessionList->Unlock();
	}
}

//=============================================================================================================================
/**
	@remarks
			处理失败会话池中的连接
	@par
			会话调用OnConnect(FALSE)会引起这个  ConnectFailList   的变化。
*/
//=============================================================================================================================
VOID IoHandler::ProcessConnectFailList()
{
	SESSION_LIST_ITER		it;
	Session					*pSession;

	m_pConnectFailList->Lock();

	for( it = m_pConnectFailList->begin(); it != m_pConnectFailList->end(); ++it )
	{
		pSession = *it;

		// 释放内存到会话池
		FreeSession( pSession );

		pSession->OnConnect( FALSE );
	}

	m_pConnectFailList->clear();
	m_pConnectFailList->Unlock();
}

//=============================================================================================================================
/**
	@remarks
			Registration for the sessions, and IOCP TempSessionList Move ActiveSessionList OnAccpet () calls ..
			已经注册的TempSessionList中的会话，在调用OnAccpet () 之后，  转移到  ActiveSessionList
*/
//=============================================================================================================================
VOID IoHandler::ProcessAcceptedSessionList()
{
	SESSION_LIST_ITER		it;
	Session					*pSession;

	// 把已经接受的会话移送到临时会话列表中
	m_pAcceptedSessionList->Lock();
	m_pTempList->splice( m_pTempList->end(), *m_pAcceptedSessionList );
	m_pAcceptedSessionList->Unlock();

	// 处理成功连接的会话
	for( it = m_pTempList->begin(); it != m_pTempList->end();)
	{
		pSession = *it;

		// 如果连接数大于最大连接数，失败
		if( m_numActiveSessions >= m_dwMaxAcceptSession )
		{
			printf( "connection full! no available accept socket!\n" );
			m_pTempList->erase( it-- );
			ReuseSession( pSession );
			continue;
		}

		// IOCP handles the registration
		CreateIoCompletionPort( (HANDLE)pSession->GetSocket(), m_hIOCP, (ULONG_PTR)pSession, 0 );

		// 如果处理Recv 失败
		if( !pSession->PreRecv() )
		{
			m_pTempList->erase( it++);
			ReuseSession( pSession );
			continue;
		}
		else
		{
			++it;
		}

		//--------------------------------
		// 成功处理已经连接的会话
		//--------------------------------

		// 创建Network 
		NetworkObject *pNetworkObject = m_fnCreateAcceptedObject();
		assert( pNetworkObject );

		
		pSession->BindNetworkObject( pNetworkObject );

		// 初始化并连接访问NetworkObject 
		pSession->OnAccept();

		
		++m_numActiveSessions;
	}

	if( !m_pTempList->empty() )
	{
		// 如果成功连接就添加到ActiveSessionList 中
		m_pActiveSessionList->Lock();
		m_pActiveSessionList->splice( m_pActiveSessionList->begin(), *m_pTempList );
		m_pActiveSessionList->Unlock();
	}
}

//=============================================================================================================================
/**
	@remarks
			处理 Accept or connect 的活动会话，并做接收包的处理。
*/
//=============================================================================================================================
VOID IoHandler::ProcessActiveSessionList()
{
	SESSION_LIST_ITER	it;
	Session				*pSession;

	for( it = m_pActiveSessionList->begin(); it != m_pActiveSessionList->end(); ++it )
	{
		pSession = *it;

		if( pSession->ShouldBeRemoved() ) continue;
		
		if( pSession->HasDisconnectOrdered() )
		{
			//处理会话，清除发送缓冲
			if( pSession->GetSendBuffer()->GetLength() == 0 )
			{
				pSession->Remove();
			}
		}
		else
		{
			if( pSession->IsAcceptSocket() && pSession->IsOnIdle() )
			{
				pSession->OnLogString( "Idle Session断开连接." );
				pSession->Remove();
				continue;
			}

			// 处理接收缓冲
			if( !pSession->ProcessRecvdPacket( m_dwMaxPacketSize ) )
			{
				pSession->Remove();
			}
		}
	}
}

//=============================================================================================================================
/**
	@remarks
			不发包超过超时时间就剔除该会话
*/
//=============================================================================================================================
VOID IoHandler::KickDeadSessions()
{
	SESSION_LIST_ITER	it;
	Session				*pSession;

	// Removed from the session and removed the temporary list ActiveSessionList
	// 从ActiveSessionList移除并转移搭配临时会话列表中.
	m_pActiveSessionList->Lock();
	for( it = m_pActiveSessionList->begin(); it != m_pActiveSessionList->end();)
	{
		pSession = *it;

		if( pSession->ShouldBeRemoved() )
		{
			m_pActiveSessionList->erase( it++ );
			m_pTempList->push_back( pSession );
		}
		else
		{
			++it;
		}
	}
	m_pActiveSessionList->Unlock();

	// 移除会话
	for( it = m_pTempList->begin(); it != m_pTempList->end(); ++it )
	{
		Session *pSession = *it;

	
		--m_numActiveSessions;

		NetworkObject *pNetworkObject = pSession->GetNetworkObject();

		// Unbind Network objects
		pSession->UnbindNetworkObject();

		//Callback function to call free will before the session. May request a callback in the Connect 
		// If you reuse the socket for the session to accept the risk of re-accept, connect, the pool is returned for the session.
		if( pSession->IsAcceptSocket() && !m_bShutdown )	
			ReuseSession( pSession );
		else												
			FreeSession( pSession );

		// NetworkObject断开连接
		pNetworkObject->OnDisconnect();

		// 请求销毁 网络对象		
		if( pSession->IsAcceptSocket() )	m_fnDestroyAcceptedObject( pNetworkObject );						
		else								m_fnDestroyConnectedObject( pNetworkObject );			
	}

	m_pTempList->clear();
}

VOID IoHandler::ProcessSend()
{
	SESSION_LIST_ITER	it;
	Session				*pSession;

	// ActiveSessionList 中的会话发送包
	m_pActiveSessionList->Lock();
	for( it = m_pActiveSessionList->begin(); it != m_pActiveSessionList->end(); ++it )
	{
		pSession = *it;

		if( pSession->ShouldBeRemoved() ) continue;

		if( pSession->PreSend() == FALSE ) 
		{
			//printf("[REMOVE][%d] pSession->PreSend() == FALSE\n", (int)pSession->GetSocket());
			pSession->Remove();
		}
	}
	m_pActiveSessionList->Unlock();
}

//=============================================================================================================================
/**
	@remarks
			移除所有会话。
*/
//=============================================================================================================================
VOID IoHandler::KickAllSessions()
{
	SESSION_LIST_ITER	it;

	for( it = m_pActiveSessionList->begin(); it != m_pActiveSessionList->end(); ++it )
	{
		(*it)->Remove();
	}
}

//=============================================================================================================================
/**
	@remarks
			更新函数，更新ActiveSessionList，ConnectSuccessList，ConnectFailList，KickDeadSessions
	@par
			该更新函数定期调用。
*/
//=============================================================================================================================
VOID IoHandler::Update()
{
	ProcessActiveSessionList();

	if( !m_pAcceptedSessionList->empty() )
	{
		ProcessAcceptedSessionList();
	}

	if( !m_pConnectSuccessList->empty() )
	{
		ProcessConnectSuccessList();
	}

	if( !m_pConnectFailList->empty() )
	{
		ProcessConnectFailList();
	}

	KickDeadSessions();
}


VOID IoHandler::Shutdown()
{
	m_bShutdown = TRUE;

	KickAllSessions();

	ProcessActiveSessionList();

	KickDeadSessions();

	m_pAcceptor->Shutdown();

	m_pConnector->Shutdown();

	// IOCP 线程退出。
	for( DWORD i = 0; i < m_numIoThreads; ++i )
	{
		PostQueuedCompletionStatus( m_hIOCP, 0, 0, NULL );
	}

	// IO 等待，直到所有的工作线程都返回。
	WaitForMultipleObjects( m_numIoThreads, m_hIoThread, TRUE, INFINITE );
}