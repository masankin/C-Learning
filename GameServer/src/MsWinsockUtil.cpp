//---------------------------------------------------------------------------------------------------
/*
�ļ�����MsWinsockUtil.cpp
ʱ�䣺2009.4.5
����:Saint Jack(�޸�)
�汾��1.0

������
	δ��ȡ����ָ��͵��ú�������ֱ������mswsock..lib��ֱ�ӵ���AcceptEx���������Ǻܴ�ģ�
	��ΪAcceptEx ʵ�����Ǵ�����Winsock2�ṹ��ϵ֮��ġ�ÿ��Ӧ�ó������ڷ����ṩ����
	��mswsock֮�ϣ�����AcceptExʱ����Ҫ��ͨ��WSAIoctl��ȡ�ú���ָ�롣���Ҫ���������
	Ӱ�����ܵĲ�����Ӧ�ó��������ֱ�Ӵӷ����ṩ��ͨ��WSAIoctl�Ȼ�ȡ��ЩAPIs��ָ�롣  
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