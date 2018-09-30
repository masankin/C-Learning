#include <winsock2.h>
#include <windows.h>
#include <assert.h>
#include "Connector.h"
#include "Session.h"
#include "SessionList.h"
#include "IoHandler.h"

//=============================================================================================================================
/**
	@remarks
			connect를 시도하는 쓰레드
	@par
			connect Success시 세션을 ConnSuccessList에 추가하고 실패시 ConnFailureList에 추가한다.
*/
//=============================================================================================================================
unsigned __stdcall connect_thread( LPVOID param )
{
	Connector	*pClass = (Connector*)param;
	Session		*pSession;

	while( !pClass->m_bShutdown )
	{
		DWORD dwRet = WaitForMultipleObjects( 2, pClass->m_hEvent, FALSE, INFINITE );

		if( dwRet - WAIT_OBJECT_0 == 0 )
		{
			// 커넥트 이벤트
			while( !pClass->m_pConnectingList->empty() )
			{
				// connection 대기 큐에서 세션을 하나 꺼낸다.
				pClass->m_pConnectingList->Lock();				
				pSession = pClass->m_pConnectingList->front();
				pClass->m_pConnectingList->pop_front();
				pClass->m_pConnectingList->Unlock();

				// connection 시도
				int err = connect( pSession->GetSocket(), (SOCKADDR*)( pSession->GetSockAddr() ), sizeof(SOCKADDR_IN) );
								   
				if( err == SOCKET_ERROR )
				{
					// connection 실패
					pClass->m_pIoHandler->m_pConnectFailList->Lock();
					pClass->m_pIoHandler->m_pConnectFailList->push_back( pSession );
					pClass->m_pIoHandler->m_pConnectFailList->Unlock();
				}
				else
				{
					// connection Success
					pClass->m_pIoHandler->m_pConnectSuccessList->Lock();
					pClass->m_pIoHandler->m_pConnectSuccessList->push_back( pSession );
					pClass->m_pIoHandler->m_pConnectSuccessList->Unlock();
				}
			}
		}
		else if( dwRet - WAIT_OBJECT_0 == 1 )
		{
			// 쓰레드 종료 이벤트
			break;
		}
	}

	return 0;
}

Connector::Connector(void)
{
	m_pConnectingList = NULL;
	m_bShutdown		= FALSE;
	for( DWORD i = 0; i < MAX_CONNECT_THREAD; ++i ) m_hThread[i] = INVALID_HANDLE_VALUE;
	m_hEvent[0]		= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hEvent[1]		= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_numThreads	= 0;
}

Connector::~Connector(void)
{
	if( !m_bShutdown ) Shutdown();

	for( DWORD i = 0; i < m_numThreads; ++i )
	{
		if( m_hThread[i] != INVALID_HANDLE_VALUE ) CloseHandle( m_hThread[i] );
	}

	CloseHandle( m_hEvent[0] );
	CloseHandle( m_hEvent[1] );

	if( m_pConnectingList	)	delete m_pConnectingList;
}

//=============================================================================================================================
/**
	@remarks
			connect에 Success시 호출되며 그 세션을 ConnSuccessList에 추가한다.
	@param	pServer
			서버의 포인터
*/
//=============================================================================================================================
VOID Connector::Init( IoHandler *pIoHandler, DWORD dwNumberOfThreads )
{
	assert( dwNumberOfThreads <= MAX_CONNECT_THREAD );

	m_pIoHandler		= pIoHandler;
	m_numThreads		= dwNumberOfThreads;

	// 접속을 위해 대기할 세션 큐 생성
	if( m_pConnectingList ) delete m_pConnectingList;
	m_pConnectingList = new SessionList;

	// 워커 쓰레드들을 생성한다.
	unsigned threadID;

	for( DWORD i = 0; i < m_numThreads; ++i )
	{
		m_hThread[i] = (HANDLE)_beginthreadex( NULL, 0, connect_thread, (LPVOID)this, 0, &threadID );
	}
}

//=============================================================================================================================
/**
	@remarks
			connect 워커 쓰레드를 생성하고 접속을 시도한다.
	@param	pSession
			커넥트를 시도하기 위한 세션의 포인터
	@param	szIP
			접속을 시도할 IP
	@param	wPort
			접속할 포트
*/
//=============================================================================================================================
VOID Connector::Connect( Session *pSession )
{
	// 세션을 connection 대기 큐에 추가한다.
	m_pConnectingList->Lock();
	m_pConnectingList->push_back( pSession );
	m_pConnectingList->Unlock();

	// 워커 쓰레드에 커넥트 이벤트를 보낸다.
	SetEvent( m_hEvent[0] );
}


//=============================================================================================================================
/**
	@remarks
			connect 워커 쓰레드를 종료시킨다.
*/
//=============================================================================================================================
VOID Connector::Shutdown()
{
	m_bShutdown = TRUE;

	// 쓰레드 종료 이벤트
	for( DWORD i = 0; i < m_numThreads; ++i ) SetEvent( m_hEvent[1] );

	// 모든 워커 쓰레드가 종료될 때까지 기다린다.
	WaitForMultipleObjects( m_numThreads, m_hThread, TRUE, INFINITE );
}