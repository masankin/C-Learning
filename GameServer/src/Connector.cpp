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
			connect�� �õ��ϴ� ������
	@par
			connect Success�� ������ ConnSuccessList�� �߰��ϰ� ���н� ConnFailureList�� �߰��Ѵ�.
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
			// Ŀ��Ʈ �̺�Ʈ
			while( !pClass->m_pConnectingList->empty() )
			{
				// connection ��� ť���� ������ �ϳ� ������.
				pClass->m_pConnectingList->Lock();				
				pSession = pClass->m_pConnectingList->front();
				pClass->m_pConnectingList->pop_front();
				pClass->m_pConnectingList->Unlock();

				// connection �õ�
				int err = connect( pSession->GetSocket(), (SOCKADDR*)( pSession->GetSockAddr() ), sizeof(SOCKADDR_IN) );
								   
				if( err == SOCKET_ERROR )
				{
					// connection ����
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
			// ������ ���� �̺�Ʈ
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
			connect�� Success�� ȣ��Ǹ� �� ������ ConnSuccessList�� �߰��Ѵ�.
	@param	pServer
			������ ������
*/
//=============================================================================================================================
VOID Connector::Init( IoHandler *pIoHandler, DWORD dwNumberOfThreads )
{
	assert( dwNumberOfThreads <= MAX_CONNECT_THREAD );

	m_pIoHandler		= pIoHandler;
	m_numThreads		= dwNumberOfThreads;

	// ������ ���� ����� ���� ť ����
	if( m_pConnectingList ) delete m_pConnectingList;
	m_pConnectingList = new SessionList;

	// ��Ŀ ��������� �����Ѵ�.
	unsigned threadID;

	for( DWORD i = 0; i < m_numThreads; ++i )
	{
		m_hThread[i] = (HANDLE)_beginthreadex( NULL, 0, connect_thread, (LPVOID)this, 0, &threadID );
	}
}

//=============================================================================================================================
/**
	@remarks
			connect ��Ŀ �����带 �����ϰ� ������ �õ��Ѵ�.
	@param	pSession
			Ŀ��Ʈ�� �õ��ϱ� ���� ������ ������
	@param	szIP
			������ �õ��� IP
	@param	wPort
			������ ��Ʈ
*/
//=============================================================================================================================
VOID Connector::Connect( Session *pSession )
{
	// ������ connection ��� ť�� �߰��Ѵ�.
	m_pConnectingList->Lock();
	m_pConnectingList->push_back( pSession );
	m_pConnectingList->Unlock();

	// ��Ŀ �����忡 Ŀ��Ʈ �̺�Ʈ�� ������.
	SetEvent( m_hEvent[0] );
}


//=============================================================================================================================
/**
	@remarks
			connect ��Ŀ �����带 �����Ų��.
*/
//=============================================================================================================================
VOID Connector::Shutdown()
{
	m_bShutdown = TRUE;

	// ������ ���� �̺�Ʈ
	for( DWORD i = 0; i < m_numThreads; ++i ) SetEvent( m_hEvent[1] );

	// ��� ��Ŀ �����尡 ����� ������ ��ٸ���.
	WaitForMultipleObjects( m_numThreads, m_hThread, TRUE, INFINITE );
}