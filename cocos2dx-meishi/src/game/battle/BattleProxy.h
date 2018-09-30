#pragma once

#include "BaseProxy.h"

class BattleProxy : public BaseProxy
{
public:

	BattleProxy();
	~BattleProxy();

	virtual void DoRsp(NetPack& stRsp);


};
