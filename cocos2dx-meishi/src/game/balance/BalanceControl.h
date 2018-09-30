/*
* 通关、评星规则
*/
#pragma once

#include "Singleton.h"
#include "Constant.h"
#include "message.h"

class BaseMode;
class BaseStar;
class StarFactory;

class BalanceControl : public MessageHandler
{
public:
	typedef std::map<int, BaseStar*>	StarList;
	typedef std::map<eBalanceScore, int> ScoreList;

public:

	BalanceControl();
	~BalanceControl();

	void	update();
	void	startCheckPoint(int cp);
	// @bReturn 游戏内点击返回，放弃比赛
	void	doCheckPtEnd(bool bSuccess, bool bReturn = false);
	BaseMode* getMode() const { return mCurMode; }

	float getTimeLeft() const;

	// 获取当前关卡得到的星星数
	int		getStarNum();

	const StarList& getStarList();

	void clean();

	void	initCheckPtStarList(int cp);

	virtual void onReceiveMassage(GameMessage * message);

	BaseStar* getTargetStar(int tag);

	void starGameTime();

	void addScore(eBalanceScore type, int scroe);
	int getScore(eBalanceScore type);
	int getTimeScore();
	int getPropScore();

	int getGameScore();

	long long getBattleTime();

protected:

	// 关卡失败时，设置所有评星未达成
	void	doCheckPtFail();

	// 评星结束，评星存储
	void	doSaveStars();

	BaseMode*	mCurMode;
	// 当前关卡是否胜利
	bool	mCurSuccess;

	// 通关模式
	typedef std::map<eBalanceTag, BaseMode*> BalanceModeList;
	BalanceModeList mModeList;


	// 评星规则
	typedef std::map<int, StarFactory*> StarFactoryList;
	StarFactoryList mStarFactoryList;

	
	StarList	mCPStarList;	// 当前关卡评星列表
	ScoreList	mScoreList;

private:
	long long mStartTime;
	int mCurCheckPoint;
};

#define sgBalanceControl() Singleton<BalanceControl>::Get()
