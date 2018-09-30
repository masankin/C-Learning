#include <winsock2.h>
#include <conio.h>
#include <iostream>
#include <assert.h>
#include <map>
#include "IOCPServer.h"
#include "IoHandler.h"
#include "SessionList.h"


//=============================================================================================================================
/**
	@remarks
			瞳맡묏鱗窟넋櫓琦뻔깁저삔뺐，랙箇관。
	@param	param
			IOCPServer 寧濾
*/
//=============================================================================================================================
unsigned __stdcall send_thread( LPVOID param )
{
	IOCPServer *pIOCPServer = (IOCPServer*)param;
	
	IOHANDLER_MAP_ITER	it;
	
	while( !pIOCPServer->m_bShutdown )
	{
		Sleep( 10 );

		for( it = pIOCPServer->m_mapIoHandlers.begin(); it != pIOCPServer->m_mapIoHandlers.end(); ++it )
		{
			it->second->ProcessSend();
		}
	}

	return 0;
}

IOCPServer::IOCPServer()
{
	m_bShutdown				= FALSE;
	m_hSendThread			= NULL;
}

IOCPServer::~IOCPServer()
{
	if( !m_bShutdown )		Shutdown();

	if( m_hSendThread )		CloseHandle( m_hSendThread );
	
	WSACleanup();
}

//=============================================================================================================================
/**
	@remarks
			서버를 초기화한다.
	@param	lpDesc
			Server 초기화에 필요한 정보를 담은 구조체의 포인터
	@retval	BOOL
			윈속 초기화에 실패하거나 리슨 소켓 생성에 실패하면 FALSE를 리턴한다.
*/
//=============================================================================================================================
BOOL IOCPServer::Init( LPIOHANDLER_DESC lpDesc, DWORD dwNumberofIoHandlers )
{
	//貢쭹놓迦뺏
	if( !InitWinsock() )	return FALSE;

	for( DWORD i = 0; i < dwNumberofIoHandlers; ++i )
	{
		CreateIoHandler( lpDesc + i );
	}

	// 눼쉔랙箇 묏鱗窟넋
	unsigned threadID;
	m_hSendThread = (HANDLE)_beginthreadex( NULL, 0, send_thread, (LPVOID)this, 0, &threadID );

	return TRUE;
}

VOID IOCPServer::CreateIoHandler( LPIOHANDLER_DESC lpDesc )
{
	IoHandler *pIoHandler = new IoHandler;

	pIoHandler->Init( this, lpDesc );

	m_mapIoHandlers.insert( IOHANDLER_MAP_PAIR( pIoHandler->GetKey(), pIoHandler ) );
}


VOID IOCPServer::Shutdown()
{
	m_bShutdown = TRUE;

	// Send 워커 쓰레드가 종료될 때까지 기다린다.
	WaitForSingleObject( m_hSendThread, INFINITE );

	// IoHandler들을 종료시키고 포인터를 삭제한다.
	IOHANDLER_MAP_ITER		it;
	IoHandler				*pIoHandler;

	for( it = m_mapIoHandlers.begin(); it != m_mapIoHandlers.end(); ++it )
	{
		pIoHandler = it->second;
		pIoHandler->Shutdown();
		delete pIoHandler;
	}
	m_mapIoHandlers.clear();
}

//=============================================================================================================================
/**
	@remarks
			Accept된 세션과 Connect된 세션들을 IOCP에 등록하면서 활성 리스트로 옮기고
			모든 세션의 받기 버퍼를 검사하여 패킷을 처리하는 등의 작업을 한다.
*/
//=============================================================================================================================
VOID IOCPServer::Update()
{
	IOHANDLER_MAP_ITER it;

	for( it = m_mapIoHandlers.begin(); it != m_mapIoHandlers.end(); ++it )
	{
		it->second->Update();
	}
}

BOOL IOCPServer::StartListen( DWORD dwIoHandlerKey, char *pIP, WORD wPort )
{
	IOHANDLER_MAP_ITER it = m_mapIoHandlers.find( dwIoHandlerKey );

	assert( it != m_mapIoHandlers.end() );

	return it->second->StartListen( pIP, wPort );
}

//=============================================================================================================================
/**
	@remarks
			- connect 쓰레드를 사용하여 접속을 시도한다.
	@param	dwIoHandlerKey
			- 접속을 시도하기 위한 세션을 가지고 있는 I/O 핸들러의 키
			- connection Success 또는 실패시 OnConnect( BOOL )이 호출된다.
	@param	szIP
			접속할 IP
	@param	wPort
			접속할 포트
	@retval	DWORD
			접속을 시도할 세션의 인덱스
*/
//=============================================================================================================================
DWORD IOCPServer::Connect( DWORD dwIoHandlerKey, NetworkObject *pNetworkObject, char *pszIP, WORD wPort )
{
	if( pNetworkObject == NULL ) return 0;

	IOHANDLER_MAP_ITER it = m_mapIoHandlers.find( dwIoHandlerKey );

	assert( it != m_mapIoHandlers.end() );

	return it->second->Connect( pNetworkObject, pszIP, wPort );
}

BOOL IOCPServer::IsListening( DWORD dwIoHandlerKey )
{
	IOHANDLER_MAP_ITER it = m_mapIoHandlers.find( dwIoHandlerKey );

	assert( it != m_mapIoHandlers.end() );

	return it->second->IsListening();
}

DWORD IOCPServer::GetNumberOfConnections( DWORD dwIoHandlerKey )
{
	IOHANDLER_MAP_ITER it = m_mapIoHandlers.find( dwIoHandlerKey );

	assert( it != m_mapIoHandlers.end() );

	return it->second->GetNumberOfConnections();
}

//=============================================================================================================================
/**
	@remarks
			윈속을 초기화한다.
*/
//=============================================================================================================================
BOOL IOCPServer::InitWinsock()
{
	WSADATA wsaData;

	WSAStartup( MAKEWORD( 2, 2 ), &wsaData );

	if( wsaData.wVersion != MAKEWORD( 2, 2 ) )
	{
		WSACleanup();

		return FALSE;
	}
	
	return TRUE;
}
