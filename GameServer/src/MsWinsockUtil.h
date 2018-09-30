#ifndef _MSWINSOCKUTIL_H_
#define _MSWINSOCKUTIL_H_

class MsWinsockUtil
{
public:
	static void	LoadExtensionFunction( SOCKET ActiveSocket );		//dll���� �Լ� ������ ����(�� Ȱ��ȭ�� Socket����)

	static LPFN_ACCEPTEX				m_lpfnAccepteEx;					//AcceptEx �Լ� ������
	static LPFN_TRANSMITFILE			m_lpfnTransmitFile;					//TransMitFile �Լ� ������ 
	static LPFN_GETACCEPTEXSOCKADDRS	m_lpfnGetAcceptExSockAddrs;			//GetAcceptExSockaddrs �Լ� ������
	static LPFN_DISCONNECTEX			m_lpfnDisconnectEx;					//DisconnectEx �Լ� ������.

private:
	static bool LoadExtensionFunction( SOCKET ActiveSocket,	GUID FunctionID, void **ppFunc);
};

#endif