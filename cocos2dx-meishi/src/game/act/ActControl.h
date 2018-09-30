#pragma once
#include "ActPanel.h"
#include "ActProxy.h"
#include "ActLoginPanel.h"
#include "Constant.h"

class ActControl
{
public:
	ActControl(void);
	~ActControl(void);

	CC_SYNTHESIZE(ActPanel*, mActPanel, ActPanel);
	ActProxy* getActProxy();

	void sendQueryActLogin();
	void sendGetActLogin();

	void doRueryActLogin(const fgame::CSSevenDaySignListResp&);
	void doGetActLogin(const fgame::CSSevenDaySignResp&);

	std::map<int,bool>* getActTypeEff() { return &mActTypeEff; }

	void doRechargeCheck();

	bool getCurrRechargeData(int type);


	bool checkCurrActCanGet();

	bool checkActCanGetByType(int type);

	void setActCanGetByType(int type,bool _get);
	
	void sendGetActNewerCharge();
	void doGetActNewerCharge();
	
	void clearActTypeEff();

	void updateFirstChargeView();

	void setIsNewYearIconOpen(bool isOpen);
	void setIsValentineIconOpen(bool isOpen);
private:
	bool isSameDay(int day);
private:
	ActProxy mActProxy;
	ActLoginPanel* mActLoginPanel;
	CC_SYNTHESIZE(bool ,mIsCanAutoShow, isCanAutoShow);


	//充值信息
	std::map<int,bool> mActTypeEff;//活动显示标签
	CC_SYNTHESIZE(int ,mRechargeCount, RechargeCount);//累计充值
	CC_SYNTHESIZE(int ,mRechargeId, RechargeId);//累计ID

	CC_SYNTHESIZE(int ,mDailyCount, DailyCount);//每日累计充值
	CC_SYNTHESIZE(int ,mDailyId, DailyId);//每日ID

	CC_SYNTHESIZE(int ,mFirstId, FirstId);//首充ID
	CC_SYNTHESIZE(bool, mIsShowFirstCharge, IsShowFirstCharge); // 是否显示首冲

	CC_SYNTHESIZE(bool, mIsNewerGiftOpened, IsNewerGiftOpened);//首充ID

	CC_SYNTHESIZE(bool, mIsChristmasActOpen, IsChristmasActOpen); // 新年活动
	
	CC_SYNTHESIZE(bool, mIsTopRankActOpen, IsTopRankActOpen); // 新年活动

	std::vector<int> mDelMailVec;

	CC_SYNTHESIZE(int, mQueryLoginDay, QueryLoginDay);;//上次查询签到时间day

	CC_SYNTHESIZE(bool,mBtnRefresh,BtnRefresh);
};
#define sgActControl() Singleton<ActControl>::Get()

