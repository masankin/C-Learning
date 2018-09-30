#pragma once


#include "BaseProxy.h"


class PropProxy : public BaseProxy
{
public:

	PropProxy();
	~PropProxy();

	virtual void DoRsp(NetPack& stRsp);
	virtual bool fault(const fgame::CSErrorMsg &err);

	void sendBuyPropRqst(int itemid, int price, int count);
	void sendUsePropRqst(int itemid, int count);

	void sendOpenChestRqst(int itemid, int count);
};
