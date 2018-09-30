#pragma once

#include "BaseProxy.h"
using namespace fgame;

class ActProxy :public BaseProxy
{
public:
	ActProxy(void);
	~ActProxy(void);

	virtual void DoRsp(NetPack& stRsp);

	void sendQueryActLogin();
	void sendGetActLogin();

	void sendGetOpenActivity();
	void sendGetActivityData();
	void sendReceiveActivityReward(int _type);
	void sendBuyActNewerGift(int _id);

	void sendGetMail(int getNum);
	void sendDelMail(std::vector<int>);

	fgame::CSSevenDaySignListResp& getSevenDaySiginList();
private:
	fgame::CSSevenDaySignListResp mSevenDaySiginList;
};

