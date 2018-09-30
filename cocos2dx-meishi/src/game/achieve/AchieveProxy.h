#pragma once
#include "BaseProxy.h"

class AchieveProxy: public BaseProxy
{
public:
	AchieveProxy(void);
	~AchieveProxy(void);

	void DoRsp(NetPack& stRsp);
	bool fault(const fgame::CSErrorMsg &err);

	void sendQueryAchievementStatus();
	void sendGetAchievementAward(int id);

	void sendReceiveAchievementRewardReq(int id);
};

