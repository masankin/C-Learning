#pragma once

#include "Message.h"

class BaseMode : public MessageHandler
{
public:

	BaseMode(){}
	virtual ~BaseMode(){}

	virtual bool init();
	virtual void update();

	virtual void onReceiveMassage(GameMessage * message);
	virtual eBalanceTag getModeType() = 0;
	virtual void	start(int cp);
	virtual void	doFail();
	virtual void	doSuccess();

	//@return 是否已经结算
	virtual bool	doPvpFail();
	//@return 是否已经结算
	virtual bool	doPvpSuccess();

	int getCheckPointIndex() const { return mCheckPt; }

	bool hasTimeLimit();
	float getTimeLeft();

	void clearData();
	void starGameTime();

protected:

	void		getBuildListSize();
	void		getFormationListSize();


	int		mEneBuildSize;
	int		mEneFormationSize;

	int		mMineBuildSize;
	int		mMineFormationSize;

	int		mCheckPt;
};

// 歼灭模式
class ModeDestory : public BaseMode
{
public:
	virtual bool init();
	virtual void onReceiveMassage(GameMessage * message);
	virtual eBalanceTag getModeType(){ return BALANCE_TAG_DESTORY; }

	//@return 是否已经结算
	virtual bool	doPvpFail();
	//@return 是否已经结算
	virtual bool	doPvpSuccess();

protected:
	 
	void	getTargetBuildsSize();
	std::vector<int>	TagBuildList;
	int m_PlayerOccupyTags;
	int m_NonPlayerOccupyTags;
	std::map<int, eCampType>	TagBuildMap;
	bool mOneOccupy;
	eCampType mOwnCamp;
};



// 占领模式
class ModeOccupy : public BaseMode
{
public:
	virtual bool init();
	virtual void onReceiveMassage(GameMessage * message);
	virtual eBalanceTag getModeType(){ return BALANCE_TAG_OCCUPY; }

	//@return 是否已经结算
	virtual bool	doPvpFail();
	//@return 是否已经结算
	virtual bool	doPvpSuccess();

protected:

	void	getTargetBuildsSize();
	std::vector<int>	TagBuildList;
	bool mOneOccupy;
	eCampType mOwnCamp;
};


//  生存模式
class ModeSurvival : public BaseMode
{
public:
	virtual bool init();
	virtual void update();
	virtual void onReceiveMassage(GameMessage * message);
	virtual eBalanceTag getModeType(){ return BALANCE_TAG_SURVIVAL; }
};

//资源模式
class ModeResoure :public BaseMode
{
public:
	virtual bool init();
	virtual void update();
	virtual void onReceiveMassage(GameMessage * message);
	virtual eBalanceTag getModeType(){ return BALANCE_TAG_RESOURCE; }
	virtual void	doFail();


protected:
	void	getTargetBuildsSize();
	void	doEndMode(eCampType eSucc);
	// 地图中的积分建筑
	std::map<int, std::string>	ScoreBuildList;
};

//守护模式
class  ModeGuard :public BaseMode
{
public:
	virtual bool init();
	virtual void update();
	virtual void onReceiveMassage(GameMessage * message);
	virtual eBalanceTag getModeType(){ return BALANCE_TAG_GUARD; }
protected:

	void	getTargetBuildsSize();
	std::vector<int>	TagBuildList;
};
