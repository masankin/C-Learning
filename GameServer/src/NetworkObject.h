#pragma once

#include <windows.h>

class Session;

//-------------------------------------------------------------------------------------------------
/// NetworkObject
//	- fnCreateAcceptedObject �ݹ� �Լ����� �� ��� ��ӹ��� ����� �ν��Ͻ���
//	  ��ȯ�ϸ� ��Ʈ�� ����� �ϴ� Session ���� ���ε� �ȴ�.
//	- 4���� ���� �Լ�(OnAccept, OnDisconnect, OnRecv, OnConnect)�� ������ �ؼ� ����Ѵ�.
//-------------------------------------------------------------------------------------------------
class NetworkObject
{
	friend class Session;
	friend class IoHandler;

public:
	NetworkObject();
	virtual ~NetworkObject();

	VOID			Disconnect( BOOL bGracefulDisconnect = TRUE );
	bool			Send( BYTE *pMsg, DWORD dwSize );//  [7/15/2009 ����]
	bool			SendEx( DWORD dwNumberOfMessages, BYTE **ppMsg, WORD *pwSize );
	VOID			Redirect( NetworkObject *pNetworkObject );
	char*			GetIP();


private:
	virtual VOID	OnAccept( DWORD dwNetworkIndex ) {}
	virtual VOID	OnDisconnect() {}
	virtual	VOID	OnRecv( BYTE *pMsg, WORD wSize ) = 0;
	virtual VOID	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex ) {}
	virtual VOID	OnLogString( char *pszLog ) {}

	inline VOID		SetSession( Session *pSession ) { m_pSession = pSession; }

	Session			*m_pSession;
};
