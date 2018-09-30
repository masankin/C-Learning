//---------------------------------------------------------------------------------------------------
/*
�ļ�����Acceptor.cpp
�������������û����ӡ�
*/
//---------------------------------------------------------------------------------------------------

#include <winsock2.h>
#include <mswsock.h>
#include <iostream>
#include <cassert>
#include "Acceptor.h"
#include "IoHandler.h"
#include "IOCPServer.h"
#include "Session.h"
#include "RecvBuffer.h"
#include "SessionList.h"
#include "SessionPool.h"
#include "MsWinsockUtil.h"

//=============================================================================================================================
/**
	@remarks
			�򿪹����̣߳����Ϊ�����̡߳�
*/
//=============================================================================================================================
unsigned __stdcall accept_thread( LPVOID param )
{
	Acceptor		*pAcceptor = (Acceptor*)param;
	IoHandler		*pIoHandler = pAcceptor->m_pIoHandler;

	BOOL			bSuccess = FALSE;
	DWORD			dwIoSize = 0;
	OVERLAPPEDEX	*pOverlappedEx = NULL;
	VOID			*pDummy = NULL;

	BYTE			*pRecvBuf = NULL;
	int				nLen;

	SOCKADDR		*lpLocalSockaddr = NULL;
	SOCKADDR		*lpRemoteSockaddr = NULL;
	int				nLocalSockaddrLen = 0;
	int				nRemoteSockaddrLen = 0;

	int				err;

	while(TRUE)
	{
		bSuccess = GetQueuedCompletionStatus( pAcceptor->m_hIOCP, &dwIoSize, (LPDWORD)&pDummy,
											  (LPOVERLAPPED*)&pOverlappedEx, INFINITE );

		// If the shutdown command thread effect
		if( pDummy == NULL )	break;
		
		if( !bSuccess )
		{
			err = WSAGetLastError();

			if( err == ERROR_OPERATION_ABORTED )
			{
				// At the end of the socket to socket AcceptEx riseun hooked up to a session that raised about ERROR_OPERATION_ABORTED
				delete pOverlappedEx->pSession;
				continue;
			}
			else if( err != ERROR_NETNAME_DELETED )
			{
				// AcceptEx the error in the ERROR_NETNAME_DELETED or (other than the errors that continue)
				continue;
			}
		}

		switch( pOverlappedEx->dwOperationType ) 
		{
		case ACCEPT_POSTED:
			pOverlappedEx->pSession->GetRecvBuffer()->GetRecvParam( &pRecvBuf, nLen );

			// SockAddr the coming
			MsWinsockUtil::m_lpfnGetAcceptExSockAddrs( pRecvBuf,
													   0,
													   sizeof(SOCKADDR_IN) + 16,
													   sizeof(SOCKADDR_IN) + 16,
													   &lpLocalSockaddr,
													   &nLocalSockaddrLen,
													   &lpRemoteSockaddr,
													   &nRemoteSockaddrLen );

			pOverlappedEx->pSession->SetSockAddr( (*(SOCKADDR_IN*)lpRemoteSockaddr) );

			//��ʼ���Ự����ӵ�PreAcceptedLists.
			pIoHandler->m_pAcceptedSessionList->Lock();
			pIoHandler->m_pAcceptedSessionList->push_back( pOverlappedEx->pSession );
			pIoHandler->m_pAcceptedSessionList->Unlock();
			break;
		}
	}

	return 0;
}


Acceptor::Acceptor()
{
	m_listenSocket	= INVALID_SOCKET;
	m_hIOCP			= INVALID_HANDLE_VALUE;
	for( DWORD i = 0; i < MAX_ACCEPT_THREAD; ++i ) m_hThread[i] = INVALID_HANDLE_VALUE;
	m_numThreads	= 0;
}

Acceptor::~Acceptor()
{
	Shutdown();

	if( m_hIOCP != INVALID_HANDLE_VALUE )			CloseHandle( m_hIOCP );
	for( DWORD i = 0; i < m_numThreads; ++i )
	{
		if( m_hThread[i] != INVALID_HANDLE_VALUE ) CloseHandle( m_hThread[i] );
	}
}

VOID Acceptor::Init( IoHandler *pIoHandler, DWORD dwNumberOfThreads )
{
	assert( dwNumberOfThreads <= MAX_ACCEPT_THREAD );

	m_pIoHandler	= pIoHandler;
	m_numThreads	= dwNumberOfThreads;

	// Accept ����IOCP 
	m_hIOCP		= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );

	// ����, �����߳�
	unsigned threadID;
	for( DWORD i = 0; i < m_numThreads; ++i )
	{
		m_hThread[i] = (HANDLE)_beginthreadex( NULL, 0, accept_thread, (LPVOID)this, 0/*CREATE_SUSPENDED*/, &threadID );
	}
}

//=============================================================================================================================
/**
	@remarks
			��ʼ����
	@param	pIP
			����IP��ַ
	@param	wPort
			����PORT
	@retval	BOOL
			���ʧ�ܷ���FALSE
*/
//=============================================================================================================================
BOOL Acceptor::StartListen( char *pIP, WORD wPort )
{	
	//����SOCKET
	if( !CreateListenSocket( pIP, wPort ) ) return FALSE;

	// ע��IOCP��SOCKET
	CreateIoCompletionPort( (HANDLE)m_listenSocket, m_hIOCP, 1, 0 );

	//װ��DLL (AcceptEx, TransmitFile)
	MsWinsockUtil::LoadExtensionFunction( m_listenSocket );

	return TRUE;
}

//=============================================================================================================================
/**
	@remarks
			����SOCKET������SOCKET��ַ.
	@retval	BOOL
			�������SOCKETʧ�ܣ�����FALSE
			(IP and port will might overlap.)
*/
//=============================================================================================================================
BOOL Acceptor::CreateListenSocket( char *pIP, WORD wPort )
{
	// ����Ѿ�������SOCKET�ˣ�ֱ�ӷ���ʧ�ܡ�
	if( m_listenSocket != INVALID_SOCKET )
	{
		return FALSE;
	}

	// ����SOCKET
	m_listenSocket = WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );

	if( m_listenSocket == INVALID_SOCKET )
	{
		return FALSE;
	}

	//��յ�ַ
	memset( &m_sockaddr, 0, sizeof( m_sockaddr ) );

	m_sockaddr.sin_family		= AF_INET;
	m_sockaddr.sin_addr.s_addr	= ( pIP == NULL || strlen( pIP ) == 0 ) ? htonl(INADDR_ANY) : inet_addr(pIP);
	m_sockaddr.sin_port			= htons( wPort );

	// socket��
	int err = bind( m_listenSocket, (SOCKADDR *)&m_sockaddr, sizeof(m_sockaddr) );

	if( err == SOCKET_ERROR )
	{
		closesocket( m_listenSocket );
		return FALSE;
	}

	// ��ʼ����
	err = listen( m_listenSocket, SOMAXCONN );

	if( err == SOCKET_ERROR )
	{
		closesocket( m_listenSocket );
		return FALSE;
	}

	return TRUE;
}

//=============================================================================================================================
/**
	@remarks
			��������߳�.
*/
//=============================================================================================================================
VOID Acceptor::SuspendListenThread()
{
	for( DWORD i = 0; i < m_numThreads; ++i )
	{
		assert( m_hThread[i] != INVALID_HANDLE_VALUE );

		SuspendThread( m_hThread[i] );
	}
}

//=============================================================================================================================
/**
	@remarks
			�ָ�����ļ����̡߳�
*/
//=============================================================================================================================
VOID Acceptor::ResumeListenThread()
{
	for( DWORD i = 0; i < m_numThreads; ++i )
	{
		assert( m_hThread != INVALID_HANDLE_VALUE );

		ResumeThread( m_hThread[i] );
	}
}

//=============================================================================================================================
/**
	@remarks
			���������߳�
*/
//=============================================================================================================================
VOID Acceptor::Shutdown()
{
	// �ر�SOCKET
	if( m_listenSocket != INVALID_SOCKET )		closesocket( m_listenSocket );

	// acceptor �߳��˳���ͨ������PostQueuedCompletionStatus����֪ͨ
	for( DWORD i = 0; i < m_numThreads; ++i )	PostQueuedCompletionStatus( m_hIOCP, 0, 0, NULL );

	// Waits until all worker threads.
	WaitForMultipleObjects( m_numThreads, m_hThread, TRUE, INFINITE );
}
