#include "worldServer.h"
#include "User.h"
#include "IniFile.h"
//-------------------------------------------------------------------------------------------------
// Callback Functions
//-------------------------------------------------------------------------------------------------
NetworkObject* CreateAcceptedObject() {	return new User; }
VOID DestroyAcceptedObject( NetworkObject *pNetworkObject ) { delete pNetworkObject; }
VOID DestroyConnectedObject( NetworkObject *pNetworkObject ) {delete pNetworkObject; }

worldServer::worldServer(void)
{
	m_pIOCPServer = new IOCPServer;
}
worldServer::~worldServer(void)
{
	printf( "Server is terminated...\n" );
	m_pIOCPServer->Shutdown();

	delete m_pIOCPServer;
	m_pIOCPServer = nullptr;
}

bool worldServer::start()
{
	IOHANDLER_DESC desc;

	desc.dwIoHandlerKey				= IniFile::get_instance().GetInt("IOHANDLER_DESC.IoHandlerKey");
	desc.dwMaxAcceptSession			= IniFile::get_instance().GetInt("IOHANDLER_DESC.MaxAcceptSession");
	desc.dwMaxConnectSession		= IniFile::get_instance().GetInt("IOHANDLER_DESC.MaxConnectSession");
	desc.dwSendBufferSize			= IniFile::get_instance().GetInt("IOHANDLER_DESC.SendBufferSize");
	desc.dwRecvBufferSize			= IniFile::get_instance().GetInt("IOHANDLER_DESC.RecvBufferSize");
	desc.dwTimeOut					= IniFile::get_instance().GetInt("IOHANDLER_DESC.TimeOut");
	desc.dwMaxPacketSize			= IniFile::get_instance().GetInt("IOHANDLER_DESC.MaxPacketSize");
	desc.dwNumberOfIoThreads		= IniFile::get_instance().GetInt("IOHANDLER_DESC.NumberOfIoThreads");
	desc.dwNumberOfAcceptThreads	= IniFile::get_instance().GetInt("IOHANDLER_DESC.NumberOfAcceptThreads");
	desc.dwNumberOfConnectThreads	= IniFile::get_instance().GetInt("IOHANDLER_DESC.NumberOfConnectThreads");
	desc.fnCreateAcceptedObject		= CreateAcceptedObject;
	desc.fnDestroyAcceptedObject	= DestroyAcceptedObject;
	desc.fnDestroyConnectedObject	= DestroyConnectedObject;


	if( !m_pIOCPServer->Init( &desc, 1 ) )
	{
		printf( "IOCP Server init fail\n" );
		return false;
	}

	if( !m_pIOCPServer->StartListen(desc.dwIoHandlerKey, "", 11111 ) )
	{
		printf( "IOCP Server start listen\n" );
		return false;
	}
	return true;
}

void worldServer::update()
{
	m_pIOCPServer->Update();
}