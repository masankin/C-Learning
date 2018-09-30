#include "PolicyManager.h"
#include "battle/AI/basic/BaseState.h"
#include "battle/AI/basic/BaseAction.h"
#include "battle/Build.h"
#include "game/utils/Comm.h"

#include "HelperFunc.h"
#include "cocos2d.h"
USING_NS_CC;

PolicyManager::PolicyManager()
{
	init();
}

PolicyManager::~PolicyManager()
{

}

bool PolicyManager::init()
{

	// state
	registerStateFactory(EST_Build_CanUpgrade, new StateUpgradeBuildFactory());
	registerStateFactory(EST_Build_ProductRate, new StateProductRateFactory());
	registerStateFactory(EST_Camp_MorePeople, new CampPeopleMoreNFactory());
	registerStateFactory(EST_Build_HoldRate, new HoldPeopleMoreNFactory());
	registerStateFactory(EST_Builds_BeAttN, new BuildInNBeAttackFactory());
	registerStateFactory(EST_N_NeutralBuild, new NeutralBuildInNFactory());
	registerStateFactory(EST_N_MorePeople, new PeopleMoreEnemyNFactory());
	registerStateFactory(EST_N_Builds, new HaveBuildMoreNFactory());
	registerStateFactory(EST_Battle_Other, new OAttONFactory());
	registerStateFactory(EST_D_Monster, new OccuMonsterFactory());
	registerStateFactory(EST_D_Builds, new EneBuildsMoreNFactory());
	registerStateFactory(EST_D_MorePeople, new EnePeopleMoreNFactory());
	registerStateFactory(EST_D_LessPeople, new EnePeopleLessNFactory());
	registerStateFactory(EST_NO_BeAttack, new BuildsNoAttackFactory());
	registerStateFactory(EST_Build_PeopleNum, new BuildPeopleNumNFactory());
	registerStateFactory(EST_PeopleCountInN, new PeopleCountInNFactory());
	registerStateFactory(EST_HaveXXbuffer, new HaveXXbufferFactory());
	registerStateFactory(EST_MoraleGtN, new MoraleGtNFactory());
	registerStateFactory(EST_OwnerNotDamage, new OwnerNotDamageFactory());
	registerStateFactory(EST_OwnerCampPeople, new OwnerCampPeopleFactory());
	registerStateFactory(EST_RunTransformCommand, new RunTransformCommandFactory());
	registerStateFactory(EST_CountDownSurplus, new CountDownSurplusFactory());
	registerStateFactory(EST_BuildSoldierNum, new BuildSoldierNumFactory());
	registerStateFactory(EST_EnemySubDstSoldierNum, new EnemySubDstSoldierNumFactory());
	registerStateFactory(EST_EnemySubFriendSoldierNum, new EnemySubFriendSoldierNumFactory());
	registerStateFactory(EST_EnemySubOtherSoldierNum, new EnemySubOtherSoldierNumFactory());
	registerStateFactory(EST_EnemyBuildAfterSendNum, new EnemyBuildAfterSendNumFactory());
	registerStateFactory(EST_EnemyBuildAfterReceiveNum, new EnemyBuildAfterReceiveNumFactory());
	registerStateFactory(EST_SelfBuildLevel, new SelfBuildLevelFactory());
	registerStateFactory(EST_BuildTypeNum, new BuildTypeNumFactory());
	registerStateFactory(EST_TimeCompareLeft, new TimeCompareFactory());
	registerStateFactory(EST_SoldierNumCompareALL, new SoldierNumCompareALLFactory());
	registerStateFactory(EST_SildierNumOrBuildNum, new SildierNumOrBuildNumFactory());
	registerStateFactory(EST_PlayerPropCount, new PlayerPropCountFactory());

	// action
	registerActionFactory(EAT_DO_NOTHING, new ActionDoNothingFactory());
	registerActionFactory(EAT_ALL_ATTACK, new ActionAllAttackFactory());
	registerActionFactory(EAT_BUILD_UPGRADE, new ActionBuildUpgradeFactory());
	registerActionFactory(EAT_SHIFT1, new ActionShiftNumFactory());
	registerActionFactory(EAT_SHIFT2, new ActionShiftRateFactory());                                               
	registerActionFactory(EAT_ATTACK1, new ActionAttackNumFactory());
	registerActionFactory(EAT_ATTACK2, new ActionAttackRateFactory());
	registerActionFactory(EAT_HELP1, new ActionHelpNumFactory());
	registerActionFactory(EAT_HELP2, new ActionHelpRateFactory());
	registerActionFactory(EAT_MONSTER_ATTACK1, new ActionMonsterNumFactory());
	registerActionFactory(EAT_MONSTER_ATTACK2, new ActionMonsterRateFactory());
	registerActionFactory(EAT_ATTACK_RANGE, new ActionAttackRangeFactory());
	registerActionFactory(EAT_ATTACK_NEAREST, new ActionAttackNearestFactory());
	registerActionFactory(EAT_ATTACK_RATE_RANGE, new ActionAttackRateRangeFactory());
	registerActionFactory(EAT_ATTACK_RATE_NEAREST, new ActionAttackRateNearestFactory());
	registerActionFactory(EAT_PROP_SILDIER, new ActionPropSildierFactory());
	registerActionFactory(EAT_PROP_SILDIER_TYPE, new ActionPropSildierTypeFactory());
	registerActionFactory(EAT_PROP_BOOM, new ActionProBoomFactory());
	registerActionFactory(EAT_PROP_ICE, new ActionProIceFactory());
	registerActionFactory(EAT_PROP_OIL, new ActionProOilFactory());
	return true;
}

void PolicyManager::registerAIPolicy(const Policy* pPolicy)
{

}

void PolicyManager::unregisterAIPolicy(const Policy* pPolicy)
{

}

void PolicyManager::registerStateFactory(eStateTag etag, StateFactory* pFactory)
{
	if (!pFactory)
	{
		CCLOG("[error]PolicyManager::registerStateFactory etag:%d pFactory==NULL!", etag);
		return;
	}

	StateFactoryList::iterator it = mStateFactoryList.find(etag);
	if (it == mStateFactoryList.end())
	{
		mStateFactoryList.insert(std::make_pair(etag, pFactory));
	}
}

void PolicyManager::unregisterStateFactory(eStateTag etag)
{
	StateFactoryList::iterator it = mStateFactoryList.find(etag);
	if (it != mStateFactoryList.end())
	{
		mStateFactoryList.erase(it);
	}
}

StateFactory* PolicyManager::getStateFactory(eStateTag etag)
{
	StateFactoryList::iterator it = mStateFactoryList.find(etag);
	if (it != mStateFactoryList.end())
	{
		return it->second;
	}
	return NULL;
}

bool PolicyManager::checkStates(vector<BaseState*>stateVec)
{
	if (stateVec.empty())
		return false;
	
	for (auto it: stateVec)
	{
	 	bool b = it->checkState();
		if (!b)
			return false;
	}
	return true;
}

bool PolicyManager::doBuildAIState(vector<BaseState*> stateVec, int iTag)
{
	if (stateVec.empty())
		return false;

	bool stateRet = checkStates(stateVec);
	BaseState *state = stateVec[0];
	if (stateRet)
	{
		if (!state->getIsLimitTime())
			state->setIsLimitTime(true);
	}

	//if (!state->getIsLimitTime())
	//{
	//	bool stateRet = checkStates(stateVec);
	//	if (stateRet)
	//	{
	//		state->setIsLimitTime(true);
	//	}
	//}

	if (state->getCando())
	{
		state->setTime(0);
		state->setIsLimitTime(false);
		state->setCando(false);
		bool ret = doAction(stateVec, iTag);
		return ret;
	}

	return false;
}

void PolicyManager::registerActionFactory(eActionTag etag, ActionFactory* pFactory)
{
	if (!pFactory)
	{
		CCLOG("[error]PolicyManager::registerActionFactory etag:%d pFactory==NULL!", etag);
		return;
	}

	ActionFactoryList::iterator it = mActionFactoryList.find(etag);
	if (it == mActionFactoryList.end())
	{
		mActionFactoryList.insert(std::make_pair(etag, pFactory));
	}
}

void PolicyManager::unregisterActionFactory(eActionTag etag)
{
	ActionFactoryList::iterator it = mActionFactoryList.find(etag);
	if (it != mActionFactoryList.end())
	{
		mActionFactoryList.erase(it);
	}
}

ActionFactory* PolicyManager::getActionFactory(eActionTag etag)
{
	ActionFactoryList::iterator it = mActionFactoryList.find(etag);
	if (it != mActionFactoryList.end())
	{
		return it->second;
	}
	return NULL;
}

bool PolicyManager::doAction(vector<BaseState*> stateVec, int iTag)
{
	// 获取action id和参数
	const StateAction* pStateAct = sgTableManager()->getStateActionByID(iTag);
	if (!pStateAct)
	{
		CCLOG("[error]PolicyManager::doBuildAIState getStateActionByID eTag:%d error!", iTag);
		return false;
	}
	std::string szAction = "";
	const int listSize = pStateAct->actionList.size();
	if (listSize > 0)
	{
		//随机取一条AI行为
		int r = rand_0_1() * listSize;
		if (r < listSize)
			szAction = pStateAct->actionList[r];
		else
		{
			CCLOG("[error]PolicyManager::doBuildAIState rand error");
			return false;
		}
	}
	else
	{
		CCLOG("[error]PolicyManager::doBuildAIState Action size error");
		return false;
	}

	//解析行为 
	std::vector<std::string>tempStr = splitStr(szAction, "#");
	if (tempStr.size() >= 2)
	{
		//取最后一个#的值，因为策划说行为要有随机值，这么蛋疼的需求
		std::string &str = tempStr.at(tempStr.size() - 1);
		std::vector<std::string>tempStr1 = splitStr(str, "&");
		if (tempStr1.size() == 2)
		{
			int r = random(atoi(tempStr1[0].c_str()), atoi(tempStr1[1].c_str()));
			str = StringUtils::format("%d", r);
		}
	}

	//把随机的值重新整理
	std::string actionStr;
	for (auto s : tempStr)
		actionStr += (s + "\t");

	HelperFunc::replaceAllCharacters(actionStr, "|", "\t");
	std::stringstream ssAction(actionStr);
	int act;
	ssAction >> act;

	ActionFactory *actionFactory = getActionFactory((eActionTag)act);
	if (actionFactory == NULL)
	{
		CCLOG("[error]PolicyManager::doBuildAIState Action no find");
		return false;
	}

	BaseAction * baseAction = actionFactory->create(ssAction);
	if (baseAction == NULL)
	{
		CCLOG("[error]PolicyManager::doBuildAIState BaseAction no create");
		return false;
	}

	//状态的建筑ID回传给行为。又是一个蛋疼的处理
	for (auto it : stateVec)
	{
		if (it->getOutputBuildings().size() != 0)
		{
			baseAction->setInputBuildings(it->getOutputBuildings());
			break;
		}
	}

	baseAction->setTargetBuild(stateVec[0]->getTargetBuild());

	bool bActionRet = baseAction->doAction();
	CC_SAFE_DELETE(baseAction);
	return bActionRet;
}

