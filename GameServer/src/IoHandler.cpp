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
			I/O completion�̴߳���
	@param	param
			IOCP ������ָ��
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

			// ����ỰΪNULL,ǿ���˳� 
			if( pSession == NULL ) break;

			// GetQueuedCompletionStatus() ����ʧ�ܾ��Ƴ��ûỰ���� ������ ��� Ȥ�� ���� ����
			if( !bSuccess )
			{
				if( GetLastError() == ERROR_NETNAME_DELETED )
				{
					// �Ƴ��رյ�SOCKET
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

			// Completion ����
			switch( pOverlappedEx->dwOperationType )
			{
			case SEND_POSTED:
				pSession->GetSendBuffer()->Completion( dwIoSize );
				break;

			case RECV_POSTED:			
				pSession->GetRecvBuffer()->Completion( dwIoSize );
				// ��һ�� Recv	
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

	// �ص�����ָ���Ƿ�Ϸ�
	assert( !IsBadReadPtr( lpDesc->fnCreateAcceptedObject, sizeof(lpDesc->fnCreateAcceptedObject) ) );
	assert( !IsBadReadPtr( lpDesc->fnDestroyAcceptedObject, sizeof(lpDesc->fnDestroyAcceptedObject) ) );
	assert( !IsBadReadPtr( lpDesc->fnDestroyConnectedObject, sizeof(lpDesc->fnDestroyConnectedObject) ) );
	
	// ����ص�����ָ��
	m_fnCreateAcceptedObject		= lpDesc->fnCreateAcceptedObject;
	m_fnDestroyAcceptedObject		= lpDesc->fnDestroyAcceptedObject;
	m_fnDestroyConnectedObject		= lpDesc->fnDestroyConnectedObject;

	m_dwKey				= lpDesc->dwIoHandlerKey;
	
	// ����Session
	m_pActiveSessionList	= new SessionList;
	m_pAcceptedSessionList	= new SessionList;
	m_pConnectSuccessList	= new SessionList;
	m_pConnectFailList		= new SessionList;
	m_pTempList				= new SessionList;

	// ���������
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

	
	// ���ð�����󳤶�
	assert( lpDesc->dwMaxPacketSize > sizeof(PACKET_HEADER) );
	m_dwMaxPacketSize =	lpDesc->dwMaxPacketSize;
  
	// ���� IOCP 
	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );

	unsigned threadID;

	//IO worker thread creation
	assert( lpDesc->dwNumberOfIoThreads <= MAX_IO_WORKER_THREAD );
	m_numIoThreads = lpDesc->dwNumberOfIoThreads;
	for( DWORD i = 0; i < m_numIoThreads; ++i )
	{
		m_hIoThread[i] = (HANDLE)_beginthreadex( NULL, 0, io_thread, (LPVOID)this, 0, &threadID );
	}

	// ���� Acceptor
	m_pAcceptor		= new Acceptor;
	m_pAcceptor->Init( this, lpDesc->dwNumberOfAcceptThreads );

	// ����Connector	
	m_pConnector	= new Connector;
	m_pConnector->Init( this, lpDesc->dwNumberOfConnectThreads );
}

DWORD IoHandler::Connect( NetworkObject *pNetworkObject, char *pszIP, WORD wPort )
{
	// ���SOCKET�Ѿ��� NetworkObject �󶨣�ֱ�ӷ��ء�
	if( pNetworkObject->m_pSession != NULL ) return 0;

	//����SOCKET
	SOCKET socket = WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );

	// ���õ�ַ
	SOCKADDR_IN addr;

	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= inet_addr( pszIP );
	addr.sin_port			= htons( wPort );

	// ���봴�� Session
	Session *pSession = AllocConnectSession();
	assert( pSession != NULL && "���ܹ���Session�������ӣ����ʵ dwMaxConnectSession " );

	// ����Session��SOCKET�͵�ַ
	pSession->SetSocket( socket );
	pSession->SetSockAddr( addr );

	// Session��NetworkObject���а�
	assert( pNetworkObject != NULL );
	pSession->BindNetworkObject( pNetworkObject );

	m_pConnector->Connect( pSession );

	return pSession->GetIndex();
}

//=============================================================================================================================
/**
	@remarks
			����SOCKET,����
	@retval	BOOL
			�������SOCKETʧ�ܣ�����FALSE.
	@param	pIP
			ʹ�õ�IP��ַ
	@param	wPort
			�˿ں�
*/
//=============================================================================================================================
BOOL IoHandler::StartListen( char *pIP, WORD wPort )
{
	assert( m_dwMaxAcceptSession > 0 );

	// ����ɹ���������TRUE
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
			���еĻỰ����ͨ���Ự�ؽ��в����ġ�
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
			�ɹ����ʻỰ������Ự�б�
	@par
			ע��Ự��ConnectSuccessList�е�Session����OnConnect(TRUE) �����Session ��ActiveSessionList�С�
*/
//=============================================================================================================================
VOID IoHandler::ProcessConnectSuccessList()
{
	SESSION_LIST_ITER		it;
	Session					*pSession;

	
	// �ɹ��������ӵ���ʱ�Ự�б�
	m_pConnectSuccessList->Lock();
	m_pTempList->splice( m_pTempList->end(), *m_pConnectSuccessList );
	m_pConnectSuccessList->Unlock();

	for( it = m_pTempList->begin(); it != m_pTempList->end(); )
	{
		pSession = *it;

		// IOCP���ע��
		CreateIoCompletionPort( (HANDLE)pSession->GetSocket(), m_hIOCP, (ULONG_PTR)pSession, 0 );

		if( pSession->PreRecv() )
		{
			pSession->OnConnect( TRUE );
			++it;
		}
		else
		{
			m_pTempList->erase( it++);

			//�ͷŻỰ�����ص��Ự��
			FreeSession( pSession );

			pSession->OnConnect( FALSE );
		}
	}

	if( !m_pTempList->empty() )
	{
		
		m_numActiveSessions += (DWORD)m_pTempList->size();

		//��ӳɹ����� ��   ActiveSessionList��
		m_pActiveSessionList->Lock();
		m_pActiveSessionList->splice( m_pActiveSessionList->end(), *m_pTempList );
		m_pActiveSessionList->Unlock();
	}
}

//=============================================================================================================================
/**
	@remarks
			����ʧ�ܻỰ���е�����
	@par
			�Ự����OnConnect(FALSE)���������  ConnectFailList   �ı仯��
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

		// �ͷ��ڴ浽�Ự��
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
			�Ѿ�ע���TempSessionList�еĻỰ���ڵ���OnAccpet () ֮��  ת�Ƶ�  ActiveSessionList
*/
//=============================================================================================================================
VOID IoHandler::ProcessAcceptedSessionList()
{
	SESSION_LIST_ITER		it;
	Session					*pSession;

	// ���Ѿ����ܵĻỰ���͵���ʱ�Ự�б���
	m_pAcceptedSessionList->Lock();
	m_pTempList->splice( m_pTempList->end(), *m_pAcceptedSessionList );
	m_pAcceptedSessionList->Unlock();

	// ����ɹ����ӵĻỰ
	for( it = m_pTempList->begin(); it != m_pTempList->end();)
	{
		pSession = *it;

		// ������������������������ʧ��
		if( m_numActiveSessions >= m_dwMaxAcceptSession )
		{
			printf( "connection full! no available accept socket!\n" );
			m_pTempList->erase( it-- );
			ReuseSession( pSession );
			continue;
		}

		// IOCP handles the registration
		CreateIoCompletionPort( (HANDLE)pSession->GetSocket(), m_hIOCP, (ULONG_PTR)pSession, 0 );

		// �������Recv ʧ��
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
		// �ɹ������Ѿ����ӵĻỰ
		//--------------------------------

		// ����Network 
		NetworkObject *pNetworkObject = m_fnCreateAcceptedObject();
		assert( pNetworkObject );

		
		pSession->BindNetworkObject( pNetworkObject );

		// ��ʼ�������ӷ���NetworkObject 
		pSession->OnAccept();

		
		++m_numActiveSessions;
	}

	if( !m_pTempList->empty() )
	{
		// ����ɹ����Ӿ���ӵ�ActiveSessionList ��
		m_pActiveSessionList->Lock();
		m_pActiveSessionList->splice( m_pActiveSessionList->begin(), *m_pTempList );
		m_pActiveSessionList->Unlock();
	}
}

//=============================================================================================================================
/**
	@remarks
			���� Accept or connect �Ļ�Ự���������հ��Ĵ���
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
			//����Ự��������ͻ���
			if( pSession->GetSendBuffer()->GetLength() == 0 )
			{
				pSession->Remove();
			}
		}
		else
		{
			if( pSession->IsAcceptSocket() && pSession->IsOnIdle() )
			{
				pSession->OnLogString( "Idle Session�Ͽ�����." );
				pSession->Remove();
				continue;
			}

			// ������ջ���
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
			������������ʱʱ����޳��ûỰ
*/
//=============================================================================================================================
VOID IoHandler::KickDeadSessions()
{
	SESSION_LIST_ITER	it;
	Session				*pSession;

	// Removed from the session and removed the temporary list ActiveSessionList
	// ��ActiveSessionList�Ƴ���ת�ƴ�����ʱ�Ự�б���.
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

	// �Ƴ��Ự
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

		// NetworkObject�Ͽ�����
		pNetworkObject->OnDisconnect();

		// �������� �������		
		if( pSession->IsAcceptSocket() )	m_fnDestroyAcceptedObject( pNetworkObject );						
		else								m_fnDestroyConnectedObject( pNetworkObject );			
	}

	m_pTempList->clear();
}

VOID IoHandler::ProcessSend()
{
	SESSION_LIST_ITER	it;
	Session				*pSession;

	// ActiveSessionList �еĻỰ���Ͱ�
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
			�Ƴ����лỰ��
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
			���º���������ActiveSessionList��ConnectSuccessList��ConnectFailList��KickDeadSessions
	@par
			�ø��º������ڵ��á�
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

	// IOCP �߳��˳���
	for( DWORD i = 0; i < m_numIoThreads; ++i )
	{
		PostQueuedCompletionStatus( m_hIOCP, 0, 0, NULL );
	}

	// IO �ȴ���ֱ�����еĹ����̶߳����ء�
	WaitForMultipleObjects( m_numIoThreads, m_hIoThread, TRUE, INFINITE );
}