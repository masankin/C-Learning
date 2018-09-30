//---------------------------------------------------------------------------------------------------
/*
文件名：MsWinsockUtil.cpp
时间：2009.4.5
作者:Saint Jack(修改)
版本：1.0

描述：
	未获取函数指针就调用函数（如直接连接mswsock..lib并直接调用AcceptEx）的消耗是很大的，
	因为AcceptEx 实际上是存在于Winsock2结构体系之外的。每次应用程序常试在服务提供层上
	（mswsock之上）调用AcceptEx时，都要先通过WSAIoctl获取该函数指针。如果要避免这个很
	影响性能的操作，应用程序最好是直接从服务提供层通过WSAIoctl先获取这些APIs的指针。  
*/
//---------------------------------------------------------------------------------------------------



#include <winsock2.h>
#include <mswsock.h>
#include "MsWinsockUtil.h"

void MsWinsockUtil::LoadExtensionFunction( SOCKET ActiveSocket )
{
	//Import AcceptEx function (dll in ..)
	GUID acceptex_guid = WSAID_ACCEPTEX;
	LoadExtensionFunction( ActiveSocket, acceptex_guid, (void**) &m_lpfnAccepteEx);

	//Import TransmitFile function (dll in ..)
	GUID transmitfile_guid = WSAID_TRANSMITFILE;
	LoadExtensionFunction( ActiveSocket, transmitfile_guid, (void**) &m_lpfnTransmitFile);

	//Import GetAcceptExSockaddrs function
	GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	LoadExtensionFunction( ActiveSocket, guidGetAcceptExSockaddrs, (void**) &m_lpfnGetAcceptExSockAddrs);

	//Import DisconnectEx function
	GUID guidDisconnectEx = WSAID_DISCONNECTEX;
	LoadExtensionFunction( ActiveSocket, guidDisconnectEx, (void**) &m_lpfnDisconnectEx );
}

bool MsWinsockUtil::LoadExtensionFunction( SOCKET ActiveSocket,	GUID FunctionID, void **ppFunc )
{
	DWORD	dwBytes = 0;

	if (0 != WSAIoctl(
		ActiveSocket, 
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&FunctionID,
		sizeof(GUID),
		ppFunc,
		sizeof(void *),
		&dwBytes,
		0,
		0))
	{
		return false;
	}

	return true;
}

LPFN_ACCEPTEX				MsWinsockUtil::m_lpfnAccepteEx				= NULL;
LPFN_TRANSMITFILE			MsWinsockUtil::m_lpfnTransmitFile			= NULL;
LPFN_GETACCEPTEXSOCKADDRS	MsWinsockUtil::m_lpfnGetAcceptExSockAddrs	= NULL;
LPFN_DISCONNECTEX			MsWinsockUtil::m_lpfnDisconnectEx			= NULL;