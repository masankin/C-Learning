#pragma once

#include "BaseProxy.h"

class RankingProxy : public BaseProxy
{
public:
	RankingProxy();
	~RankingProxy();

	virtual void DoRsp(NetPack& stRsp);

	void sendRankingListReq(eRankingGroupType groupID, int topNSize);
	void sendPlayerRankInfoReq(unsigned int uin);
	void sendPlayerPvpResultReq();
	void sendSelfRankingListReq();
	void sendRankWeekAwardInfoReq();
	void sendRankWeekAwardReq();
};

