#pragma once
#include "NetworkObject.h"
#include "net_msg.h"
class User : public NetworkObject
{
protected:
	virtual VOID	OnAccept( DWORD dwNetworkIndex ) {};
	virtual VOID	OnDisconnect() {};
	virtual	VOID	OnRecv( BYTE *pMsg, WORD wSize ) ;
	virtual VOID	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex ) {}
public:
	void nc_playerAuthSession(tagMsg* m);
};
