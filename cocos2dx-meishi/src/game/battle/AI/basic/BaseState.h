#pragma once

#include "AIState.h"
#include "battle/Build.h"


class BaseState:public Ref
{
public:

	BaseState() :mTime(0), mLimitTime(0), misLimit(false), mCanDo(false){}
	virtual ~BaseState(){}
	
	virtual bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag() = 0;
	virtual bool checkState() = 0;

	void	setTargetBuild(BaseBuild* pBuild);
	BaseBuild * getTargetBuild();
	eCampType getOwnCamp();

	bool checkCamps(const int srcCamp, const int dstCamp, int relation);

	void setOutputBuildings(const std::vector<int>& builds) {
		m_outputBuildings.resize(builds.size());
		m_outputBuildings.assign(builds.begin(), builds.end());
		for (const auto& iter : m_outputBuildings)
		{
			CCLOG("output buildId:%d", iter);
		}
	}
	const std::vector<int>& getOutputBuildings(){ return m_outputBuildings; }

protected:
	int mTargetBuildId;

	int param1;
	int param2;
	int param3;

	std::vector<int> m_outputBuildings;
	CC_SYNTHESIZE(int, mTime, Time);          //当前时间
	CC_SYNTHESIZE(int, mLimitTime, LimitTime);//限制时间
	CC_SYNTHESIZE(bool, misLimit, IsLimitTime);//是否限制
	CC_SYNTHESIZE(bool, mCanDo, Cando);			//是否执行
};

class StateFactory
{
public:
	virtual BaseState* create(std::stringstream& szparams) = 0;
};

#define DEFINE_STATE_FACTORY(_NAME_)	\
class _NAME_##Factory : public StateFactory	\
{\
public:	\
	virtual BaseState* create(std::stringstream& szparams){\
	_NAME_* pret = new _NAME_();	\
	if (pret && pret->init(szparams)){\
		return pret;	\
	}	\
		return NULL;\
	}	\
};


// 建筑是否可升级
class StateUpgradeBuild : public BaseState
{
public:

	virtual eStateTag getStateTag(){ return EST_Build_CanUpgrade; }
	virtual bool checkState();
};

DEFINE_STATE_FACTORY(StateUpgradeBuild);

// 建筑兵力达到生产上限的百分比
class StateProductRate : public BaseState
{
public:

	virtual eStateTag getStateTag(){ return EST_Build_ProductRate; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(StateProductRate);

// 己方总兵力达到
class CampPeopleMoreN : public BaseState
{
public:

	virtual eStateTag getStateTag(){ return EST_Camp_MorePeople; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(CampPeopleMoreN);

// 建筑兵力达到容纳上限的百分比
class HoldPeopleMoreN : public BaseState
{
public:

	virtual eStateTag getStateTag(){ return EST_Build_HoldRate; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(HoldPeopleMoreN);

// 距离N格己方建筑受到攻击时
class BuildInNBeAttack : public BaseState
{
public:

	virtual eStateTag getStateTag(){ return EST_Builds_BeAttN; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(BuildInNBeAttack);

// 距离N格存在有中立阵营时
class NeutralBuildInN : public BaseState
{
public:

	virtual eStateTag getStateTag(){ return EST_N_NeutralBuild; }
	virtual bool checkState(); 
	
};
DEFINE_STATE_FACTORY(NeutralBuildInN);

// 己方兵力超过敌方兵力N, 多方混战则超过最多的为N
class PeopleMoreEnemyN : public BaseState
{
public:

	virtual eStateTag getStateTag(){ return EST_N_MorePeople; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(PeopleMoreEnemyN);


// 己方占领的建筑数量大于N
class HaveBuildMoreN : public BaseState
{
public:
	virtual bool init(std::stringstream& ssparams);

	virtual eStateTag getStateTag(){ return EST_N_Builds; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(HaveBuildMoreN);

// 当非友方出兵攻打第三方时
class OAttON : public BaseState
{
public:

	virtual eStateTag getStateTag(){ return EST_Battle_Other; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(OAttON);

// 附近生成打野点
class OccuMonster : public BaseState
{
public:

	virtual eStateTag getStateTag(){ return EST_D_Monster; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(OccuMonster);


// 距离若干格存在的敌方建筑大于N
class EneBuildsMoreN : public BaseState
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_D_Builds; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(EneBuildsMoreN);


// 距离若干格存在的敌方建筑兵力大于N
class EnePeopleMoreN : public BaseState
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_D_MorePeople; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(EnePeopleMoreN);

// 距离若干格存在的敌方建筑兵力小于N
class EnePeopleLessN : public BaseState
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_D_LessPeople; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(EnePeopleLessN);

// 所有己方建筑没有受到任何攻击
class BuildsNoAttack : public BaseState
{
public:

	virtual eStateTag getStateTag(){ return EST_NO_BeAttack; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(BuildsNoAttack);


// 当前建筑内的兵力达到指定值
class BuildPeopleNumN : public BaseState
{
public:

	virtual eStateTag getStateTag(){ return EST_Build_PeopleNum; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(BuildPeopleNumN);

// N格范围内存在兵力少于M的建筑
class PeopleCountInN : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_PeopleCountInN; }
	virtual bool checkState();

private:
	int param4;
};
DEFINE_STATE_FACTORY(PeopleCountInN);

//己方士气大于M
class HaveXXbuffer : public BaseState
{
public:
	virtual eStateTag getStateTag(){ return EST_HaveXXbuffer; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(HaveXXbuffer);

//己方士气大于M
class MoraleGtN : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_MoraleGtN; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(MoraleGtN);

//自身建筑没有收到伤害	
class OwnerNotDamage : public BaseState
{
public:
	virtual eStateTag getStateTag(){ return EST_OwnerNotDamage; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(OwnerNotDamage);

//己方总兵力低于M
class OwnerCampPeople : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_OwnerCampPeople; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(OwnerCampPeople);



//指定建筑发出救援指令
class RunTransformCommand : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_RunTransformCommand; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(RunTransformCommand);

//剩余时间少于M秒
class CountDownSurplus : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_CountDownSurplus; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(CountDownSurplus);


//EST_BuildSoldierNum,	//指定建筑兵力
//EST_EnemySubDstSoldierNum,	//敌方向本建筑派兵数与驻兵数的差距
//EST_EnemySubFriendSoldierNum, //敌方向友方建筑派兵数与驻兵数的差距
//EST_EnemySubOtherSoldierNum, //敌方向第三方派兵数与驻兵数的差距
//EST_EnemyBuildAfterSendNum, //敌方派兵之后的数量
//EST_EnemyBuildAfterReceiveNum, //敌方受到之后的数量
//
//指定建筑兵力
class BuildSoldierNum : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_BuildSoldierNum; }
	virtual bool checkState();
private:
	int param4;
};
DEFINE_STATE_FACTORY(BuildSoldierNum);


class EnemySubDstSoldierNum : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_EnemySubDstSoldierNum; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(EnemySubDstSoldierNum);


class EnemySubFriendSoldierNum : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_EnemySubFriendSoldierNum; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(EnemySubFriendSoldierNum);

class EnemySubOtherSoldierNum : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_EnemySubOtherSoldierNum; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(EnemySubOtherSoldierNum);

class EnemyBuildAfterSendNum : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_EnemyBuildAfterSendNum; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(EnemyBuildAfterSendNum);

class EnemyBuildAfterReceiveNum : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_EnemyBuildAfterReceiveNum; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(EnemyBuildAfterReceiveNum);



//自身建筑等级
class SelfBuildLevel : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_SelfBuildLevel; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(SelfBuildLevel);



//己方占领的M（兵营、炮塔、研究所）的数量R（小于、大于等于）N
class BuildTypeNum : public BaseState
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStateTag getStateTag(){ return EST_BuildTypeNum; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(BuildTypeNum);


class TimeCompare :public BaseState
{
public:
	bool init(std::stringstream & ssparams);
	virtual eStateTag getStateTag(){ return EST_TimeCompareLeft; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(TimeCompare);

class SoldierNumCompareALL :public BaseState
{
public:
	bool init(std::stringstream & ssparams);
	virtual eStateTag getStateTag(){ return EST_SoldierNumCompareALL; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(SoldierNumCompareALL);


class SildierNumOrBuildNum :public BaseState
{
public:
	bool init(std::stringstream & ssparams);
	virtual eStateTag getStateTag(){ return EST_SildierNumOrBuildNum; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(SildierNumOrBuildNum);

class PlayerPropCount :public BaseState
{
public:
	bool init(std::stringstream & ssparams);
	virtual eStateTag getStateTag(){ return EST_PlayerPropCount; }
	virtual bool checkState();
};
DEFINE_STATE_FACTORY(PlayerPropCount);
