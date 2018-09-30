#pragma once


#include "BaseProxy.h"


class WinnerProxy : public BaseProxy
{
public:

	WinnerProxy();
	~WinnerProxy();

	virtual void DoRsp(NetPack& stRsp);
	virtual bool fault(const fgame::CSErrorMsg &err);

	void sendGetRqst();

	void sendWinnerValueRqst();

	void sendActGetRqst(int actId,int subId);

	void sendWinnerListRqst();

};
