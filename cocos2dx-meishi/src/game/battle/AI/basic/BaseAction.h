#pragma once

#include <vector>
#include "AIState.h"

class BaseBuild;
class BaseAction
{
public:
	BaseAction(){}
	virtual ~BaseAction(){}

	virtual bool init(std::stringstream& ssparams){ return true; }
	virtual bool doAction() = 0;
	virtual eActionTag getActionTag() = 0;

	//enemy 1.友方 2.敌方 3.中立 
	bool checkCamps(const int srcCamp, const int dstCamp, int relation);
	void getNearBuildings(int keyId, int relation,  std::vector<BaseBuild*>& vecBuild);

	void	setTargetBuild(BaseBuild* pBuild){ mTargetBuild = pBuild; }

	void	setInputBuildings(const std::vector<int >& builds) {
		m_inBuildingIds.resize(builds.size());
		m_inBuildingIds.assign(builds.begin(), builds.end());
	}
	const std::vector<int>& getInputBuildings() { return m_inBuildingIds; }

protected:
	BaseBuild*	mTargetBuild;

	std::vector<int> m_inBuildingIds;

};

class ActionFactory
{
public:
	virtual BaseAction* create(std::stringstream& ssparams) = 0;
};

// 定义ai action工厂
#define DEFINE_ACTION_FACTORY(_NAME_)	\
class _NAME_##Factory : public ActionFactory	\
{\
public:	\
	virtual BaseAction* create(std::stringstream& ssparams){\
_NAME_* pret = new _NAME_();	\
if (pret && pret->init(ssparams)){\
return pret;}\
return NULL; \
}\
};

// 按兵不动
class ActionDoNothing : public BaseAction
{
public:

	virtual eActionTag getActionTag(){ return EAT_DO_NOTHING; }
	virtual bool doAction(){ return  true; }


};

DEFINE_ACTION_FACTORY(ActionDoNothing);


// 全军出动
class ActionAllAttack : public BaseAction
{
public:

	virtual eActionTag getActionTag(){ return EAT_ALL_ATTACK; }
	virtual bool doAction();
};

DEFINE_ACTION_FACTORY(ActionAllAttack);

// 建筑升级
class ActionBuildUpgrade : public BaseAction
{
public:

	virtual eActionTag getActionTag(){ return EAT_BUILD_UPGRADE; }
	virtual bool doAction();
};

DEFINE_ACTION_FACTORY(ActionBuildUpgrade);

// 按数量屯兵
class ActionShiftNum : public BaseAction
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eActionTag getActionTag(){ return EAT_SHIFT1; }
	virtual bool doAction();


protected:

	int getRuleBuildIns();
	// 可能的攻击建筑id
	std::vector<int> vParam;
	// 屯兵比例或数量
	int	mShiftParam;
};

DEFINE_ACTION_FACTORY(ActionShiftNum);

// 按百分比屯兵
class ActionShiftRate : public ActionShiftNum
{
public:
	virtual eActionTag getActionTag(){ return EAT_SHIFT2; }
	virtual bool doAction();
};

DEFINE_ACTION_FACTORY(ActionShiftRate);

// 按数量进攻
class ActionAttackNum : public BaseAction
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eActionTag getActionTag(){ return EAT_ATTACK1; }
	virtual bool doAction();

protected:

	int getRuleBuildIns();

	int param[6];
};

DEFINE_ACTION_FACTORY(ActionAttackNum);

// 按百分比进攻
class ActionAttackRate : public ActionAttackNum
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eActionTag getActionTag(){ return EAT_ATTACK2; }
	virtual bool doAction();

};

DEFINE_ACTION_FACTORY(ActionAttackRate);

// 按数量救援
class ActionHelpNum : public BaseAction
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eActionTag getActionTag(){ return EAT_HELP1; }
	virtual bool doAction();

protected:
	// 最近受攻击的建筑
	// @type 0-比例 1-个数
	// @numLimit 救援建筑的兵力达到numLimit才执行
	int getRuleBuildKey(int type, int limit);

	int param1;
	int param2;		// 救援建筑的兵力参数，可能为兵数量或比例
};

DEFINE_ACTION_FACTORY(ActionHelpNum);

// 按百分比救援
class ActionHelpRate : public ActionHelpNum
{
public:
	virtual eActionTag getActionTag(){ return EAT_HELP2; }
	virtual bool doAction();
};

DEFINE_ACTION_FACTORY(ActionHelpRate);

// 打野 按数量派兵
class ActionMonsterNum : public ActionAttackNum
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eActionTag getActionTag(){ return EAT_MONSTER_ATTACK1; }
	virtual bool doAction();

protected:

	int param1;
};

DEFINE_ACTION_FACTORY(ActionMonsterNum);


class ActionMonsterRate: public ActionAttackNum
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eActionTag getActionTag(){ return EAT_MONSTER_ATTACK2; }
	virtual bool doAction();
};
DEFINE_ACTION_FACTORY(ActionMonsterRate);

//
class ActionAttackRange : public BaseAction
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eActionTag getActionTag(){ return EAT_ATTACK_RANGE; }
	virtual bool doAction();

protected:
	virtual BaseBuild* getMinimalBuild();
	int param1;
	int param2;	
};
DEFINE_ACTION_FACTORY(ActionAttackRange);

class ActionAttackNearest : public BaseAction
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eActionTag getActionTag(){ return EAT_ATTACK_NEAREST; }
	virtual bool doAction();

protected:
	virtual BaseBuild* getNearestBuild();
	int param1;
};
DEFINE_ACTION_FACTORY(ActionAttackNearest);


class ActionAttackRateRange : public ActionAttackRange
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eActionTag getActionTag(){ return EAT_ATTACK_RATE_RANGE; }
	virtual bool doAction();

protected:
	int param1;
	int param2;
};
DEFINE_ACTION_FACTORY(ActionAttackRateRange);

class ActionAttackRateNearest : public ActionAttackNearest
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eActionTag getActionTag(){ return EAT_ATTACK_RATE_NEAREST; }
	virtual bool doAction();

protected:
	int param1;
};
DEFINE_ACTION_FACTORY(ActionAttackRateNearest);



class ActionPropSildier : public BaseAction
{
public:
	bool init(std::stringstream& ssparams);
	virtual eActionTag getActionTag(){ return EAT_PROP_SILDIER; }
	virtual bool doAction();
	virtual BaseBuild* getBuildId();
protected:
	int param1;
};
DEFINE_ACTION_FACTORY(ActionPropSildier);

class ActionPropSildierType : public ActionPropSildier
{
public:
	virtual eActionTag getActionTag(){ return EAT_PROP_SILDIER_TYPE; }
	BaseBuild* getBuildId();
};
DEFINE_ACTION_FACTORY(ActionPropSildierType);

class ActionProBoom :public BaseAction
{
public:
	virtual eActionTag getActionTag(){ return EAT_PROP_BOOM; }
	virtual bool doAction();
};
DEFINE_ACTION_FACTORY(ActionProBoom);

class ActionProIce :public BaseAction
{
public:
	virtual eActionTag getActionTag(){ return EAT_PROP_ICE; }
	virtual bool doAction();
};
DEFINE_ACTION_FACTORY(ActionProIce);

class ActionProOil :public BaseAction
{
public:
	virtual eActionTag getActionTag(){ return EAT_PROP_OIL; }
	virtual bool doAction();
};
DEFINE_ACTION_FACTORY(ActionProOil);
