#pragma once
#include "cocos2d.h"
#include "Constant.h"
#include "BaseProxy.h"

class CrazyWeekProxy : public BaseProxy
{
public:
	CrazyWeekProxy();
	~CrazyWeekProxy();

	virtual void DoRsp(NetPack& stRsp);
	virtual bool fault(const fgame::CSErrorMsg &err);

	void sendGetCrazyWeekInfoReq();
	void sendGetRewardReq(int rewardId = 1);
};

class ActCrazyWeekControl
{
public:
	ActCrazyWeekControl();
	virtual ~ActCrazyWeekControl();

	// 主界面显示活动周开始
	void	showCrazyWeekIcon(bool flag = false);
	// 任务完成显示红点
	void	showCrazyWeekTip(bool flag = false);

	// 请求狂欢周数据
	void	sendGetCrazyWeekInfoReq();
	// 领取狂欢周奖励
	void	sendGetRewardReq();

	// 同步狂欢周数据
	void	syncCrazyWeekInfo(const fgame::CSActivityCrazyDtlsResp &dtlsResp);
	// 更新狂欢周数据（服务器主动下发）
	void	updateCrazyWeekInfo(const fgame::CSActivityCrazyUsableResp &usableResp);
	//	获取狂欢周奖励
	void	doReward(const fgame::CSActivityCrazyUsageResp &usageResp);

	CC_SYNTHESIZE(int,	mCurrDay, Currday);
	CC_SYNTHESIZE(int,	mRewardId, RewardId);
	CC_SYNTHESIZE(int,	mCrazyPoint, CrazyPoint);
	CC_SYNTHESIZE(int,	mExtraHonourPoint, ExtraHonourPoint);
	CC_SYNTHESIZE(int,  mConditionCompleted, ConditionCompleted);
	CC_SYNTHESIZE(int,  mSubConditionCompleted, SubConditionCompleted);
	CC_SYNTHESIZE(int,  mRewardState, RewardState);   // 0 不可领， 1 可领， 2 已领
	CC_SYNTHESIZE(std::string, mTimeString, TimeString);
	CC_SYNTHESIZE(bool, mIsCrazyWeekOpen, IsCrazyWeekOpen);
protected:
	CrazyWeekProxy mCrazyWeekProxy;

private:
	//int		mCurrDay;
	//int		mRewardId;
	//int		mCrazyPoint;
	//int		mExtraHonourPoint;
	//bool		mConditionCompleted;
	//bool		mSubConditionCompleted;
	//bool		mIsGetReward;
};

#define sgActCrazyWeekControl() Singleton<ActCrazyWeekControl>::Get()
