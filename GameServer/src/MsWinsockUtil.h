#ifndef _MSWINSOCKUTIL_H_
#define _MSWINSOCKUTIL_H_

class MsWinsockUtil
{
public:
	static void	LoadExtensionFunction( SOCKET ActiveSocket );		//dll에서 함수 포인터 얻어옴(꼭 활성화된 Socket으로)

	static LPFN_ACCEPTEX				m_lpfnAccepteEx;					//AcceptEx 함수 포인터
	static LPFN_TRANSMITFILE			m_lpfnTransmitFile;					//TransMitFile 함수 포인터 
	static LPFN_GETACCEPTEXSOCKADDRS	m_lpfnGetAcceptExSockAddrs;			//GetAcceptExSockaddrs 함수 포인터
	static LPFN_DISCONNECTEX			m_lpfnDisconnectEx;					//DisconnectEx 함수 포인터.

private:
	static bool LoadExtensionFunction( SOCKET ActiveSocket,	GUID FunctionID, void **ppFunc);
};

#endif