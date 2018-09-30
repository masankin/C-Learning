#pragma once

#include <windows.h>

class Session;

//-------------------------------------------------------------------------------------------------
/// NetworkObject
//	- fnCreateAcceptedObject 콜백 함수에서 이 잚깎를 상속받은 잚깎의 인스턴스를
//	  반환하면 네트웍 기능을 하는 Session 잚깎와 바인드 된다.
//	- 4개의 가상 함수(OnAccept, OnDisconnect, OnRecv, OnConnect)를 재정의 해서 사용한다.
//-------------------------------------------------------------------------------------------------
class NetworkObject
{
	friend class Session;
	friend class IoHandler;

public:
	NetworkObject();
	virtual ~NetworkObject();

	VOID			Disconnect( BOOL bGracefulDisconnect = TRUE );
	bool			Send( BYTE *pMsg, DWORD dwSize );//  [7/15/2009 벗댐]
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
