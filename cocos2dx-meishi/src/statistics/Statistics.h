
/*
统计类
*/
#pragma once

#include "MessageManager.h"
#include "Singleton.h"



class DataEyeAgent;
class TalkingDataAgent;

class Statistics : public MessageHandler
{
public:
	Statistics();
	virtual ~Statistics();

	bool init();

protected:

	virtual void onReceiveMassage(GameMessage * message) override;

	void doCheckPointStart(MessageCheckPointStart * pMsg);
	void doCheckPointOver(MessageCheckPointEnd* pMsg);
	void doChannelLogin(MessageChannelLogin* pMsg);
	void doChannelLogout(MessageChannelLogout* pMsg);
	void doOpenUI(MessageOpenUI* pMsg);
	void doGuideStepOver(MessageGuideStepOver* pMsg);
	void doRMBBuy(MessageShopPayComplent* pMsg);
	void doItemOperation(MessageItemOperation* pMsg);
	void doMoneyChange(MessageMoneyChange* pMsg);
	void doPVPInfo(MessagePVPInfo* pMsg);
	void doGameLoginFlow(MessageGameLoginFlow* pMsg);

private:
	DataEyeAgent* mDataEyeAgent;
	TalkingDataAgent* mTalkingDataAgent;

};

#define sgStatistic() Singleton<Statistics>::Get()
