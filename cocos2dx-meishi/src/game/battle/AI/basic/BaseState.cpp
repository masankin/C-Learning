#include <sstream>
#include "BaseState.h"
#include "battle/Build.h"
#include "battle/BattleControl.h"
#include "battle/Camp.h"
#include "battle/MoraleManager.h"
#include "balance/BalanceControl.h"
#include "battle/Formation/FormationV2.h"
#include "buff/BuffManager.h"
#include "GamePrecedure.h"
#include "pvp/PvpControl.h"
bool BaseState::init(std::stringstream& ssparams)
{
	ssparams >> param1;


	//std::stringstream ssParams(szState);

	//int id;\
	//ssParams >> id >> param1;
	return true;
}

void BaseState::setTargetBuild(BaseBuild* pBuild)
{
	if (pBuild)
		mTargetBuildId = pBuild->getKeyID();
	else
		mTargetBuildId = 0;
}

BaseBuild* BaseState::getTargetBuild()
{
	return sgBattleControl()->getTargetBuild(mTargetBuildId);
}

eCampType BaseState::getOwnCamp()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return eCampType::ECAMP_TYPE_Invalid;

	return pTargetBuild->getBuildInfo().camp;
}

bool BaseState::checkCamps(const int srcCamp, const int dstCamp, int relation)
{
	//T(1、2、3：友方、敌方、中立)
	if (relation == 1)
	{
		if (srcCamp == dstCamp)
		{
			return true;
		}
	}
	else if (relation == 2)
	{
		if (srcCamp != dstCamp && dstCamp != ECAMP_TYPE_Neutral)
		{
			return true;
		}
	}
	else if (relation == 3)
	{
		if (dstCamp == ECAMP_TYPE_Neutral)
			return true;
	}

	return false;
}


bool StateUpgradeBuild::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	if (pTargetBuild->isCurUpgrade())
		return false;

	return pTargetBuild->checkCanUpgrade();
}

bool StateProductRate::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	float nowHave = pTargetBuild->getOwnPeople();
	float maxProd = pTargetBuild->getMaxProduct();

	if (nowHave / maxProd >= (float)param1 / 100)
		return true;
	return false;
}

bool CampPeopleMoreN::checkState()
{
	Camp* pCamp = sgBattleControl()->getTargetCamp(getOwnCamp());
	if (!pCamp)
		return false;

	int totalPeople = pCamp->getCampCurProduct();
	if (totalPeople >= param1)
		return true;
	return false;
}

bool HoldPeopleMoreN::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	float nowHave = pTargetBuild->getOwnPeople();
	float maxHold = pTargetBuild->getMaxHold();

	if (nowHave / maxHold >= (float)param1 / 100)
		return true;
	return false;
}

bool BuildInNBeAttack::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	Vec2 curPos = pTargetBuild->getPosition();
	int dis = param1 * TILED_SIZE;

	Camp* pCamp = sgBattleControl()->getTargetCamp(pTargetBuild->getCamp());
	if (!pCamp)
		return false;

	//const MapBulidList& buildlist = pCamp->getBuildList();
	//MapBulidList::const_iterator it = buildlist.find(pTargetBuild->getKeyID());
	//if (it != buildlist.end() && it->second->checkCurBeAttack() && 
	//	it->second->getPosition().getDistance(curBuild) <= dis)
	//{
	//	return true;
	//}

	//上面的ai只执行一次啊
	//modify by weihua.cui
	const MapBulidList& buildlist = pCamp->getBuildList();
	for (const auto& iter : buildlist)
	{
		if (iter.first != pTargetBuild->getKeyID() && iter.second->getPosition().getDistance(curPos) <= dis)
		{
			std::vector<FormationV2*> vecFormation;
			sgBattleControl()->getApproachingBuildFormations(iter.second->getKeyID(), vecFormation);
			size_t totalEnemy = 0;
			for (size_t i = 0; i < vecFormation.size(); i++)
			{
				FormationV2* v2 = vecFormation[i];
				if (v2->getSrcCamp() != v2->getDstCamp())
				{
					totalEnemy += v2->getExpectedCount();
				}
			}
			if ((int)totalEnemy>iter.second->getOwnPeople())
			{
				return true;
			}
		}
	}
	return false;
}

bool NeutralBuildInN::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	MapBulidList buildlist;
	sgBattleControl()->getTargetBuildList(ECAMP_TYPE_Neutral, buildlist);
	if (buildlist.empty())
		return false;

	Vec2 curBuild = pTargetBuild->getPosition();
	int dis = param1 * TILED_SIZE;

	MapBulidList::iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		if (it->second->getPosition().getDistance(curBuild) <= dis)
		{
			return true;
		}
	} 
	return false;
}
 

bool PeopleMoreEnemyN::checkState()
{
	Camp* pPlayerCamp = sgBattleControl()->getTargetCamp(sgBattleControl()->getOwnCamp());
	Camp* pEnemyCamp = sgBattleControl()->getTargetCamp(getOwnCamp());

	if (!pPlayerCamp || !pEnemyCamp)
		return false;

	if (pEnemyCamp->getCampCurProduct() - pPlayerCamp->getCampCurProduct() > param1)
		return true;

	return false;
}

bool HaveBuildMoreN::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2;
	return true;
}

bool HaveBuildMoreN::checkState()
{
	Camp* pPlayerCamp = sgBattleControl()->getTargetCamp(getOwnCamp());
	if (param1 == 1)
	{
		if (!pPlayerCamp || (int)pPlayerCamp->getBuildList().size() < param2)
			return true;
	}
	else if (param1 == 2)
	{
		if (!pPlayerCamp || (int)pPlayerCamp->getBuildList().size() >= param2)
			return true;
		else
			return false;
	}
	else
	{
		return false;
	}
	return false;
}

bool OAttON::checkState()
{
	return sgBattleControl()->getIfOtherTwoInBattle(getOwnCamp());
}

bool OccuMonster::checkState()
{ 
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	MapBulidList creepsBuildList;
	sgBattleControl()->getTargetActivedBuildList(ECAMP_TYPE_Creeps, creepsBuildList); 
	if (creepsBuildList.empty())
		return false;

	Vec2 curBuild = pTargetBuild->getPosition();
	int dis = param1 * TILED_SIZE;

	MapBulidList::iterator it = creepsBuildList.begin();
	for (; it != creepsBuildList.end(); ++it)
	{
		if (/*it->second->getBuildInfo().locationID != 0
			&&*/ it->second->isEnable()
			&& it->second->getPosition().getDistance(curBuild) <= dis)
		{
			std::vector<int> vecBuilds;
			vecBuilds.resize(1); 
			vecBuilds[0] = it->second->getKeyID();
			this->setOutputBuildings(vecBuilds);
			return true;
		}
	}
	return false;
}

bool EneBuildsMoreN::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	MapBulidList buildlist;
	sgBattleControl()->getEnemyBuildList(pTargetBuild->getCamp(), buildlist);
	if (buildlist.empty())
		return false;

	Vec2 curBuild = pTargetBuild->getPosition();
	int dis = param1 * TILED_SIZE;

	int total = 0;
	MapBulidList::iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		if (it->second->isEnable() && it->second->getPosition().getDistance(curBuild) <= dis)
		{
			total++;
		}
	}

	if (total > param2)
		return true;

	return false;
}

bool EneBuildsMoreN::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2;

	return true;
}

bool EnePeopleMoreN::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	MapBulidList buildlist;
	sgBattleControl()->getEnemyBuildList(pTargetBuild->getCamp(), buildlist);
	if (buildlist.empty())
		return false;

	Vec2 curBuild = pTargetBuild->getPosition();
	int dis = param1 * TILED_SIZE;

	int total = 0;
	MapBulidList::iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		if (it->second->isEnable() && it->second->getPosition().getDistance(curBuild) <= dis)
		{
			total += it->second->getOwnPeople();
		}
	}

	if (total > param2)
		return true;

	return false;
}

bool EnePeopleMoreN::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2;

	return true;
}

bool EnePeopleLessN::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	MapBulidList buildlist;
	sgBattleControl()->getEnemyBuildList(pTargetBuild->getCamp(), buildlist);
	if (buildlist.empty())
		return false;

	Vec2 curBuild = pTargetBuild->getPosition();
	int dis = param1 * TILED_SIZE;

	int total = 0;
	MapBulidList::iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		if (it->second->isEnable() && it->second->getPosition().getDistance(curBuild) <= dis)
		{
			total += it->second->getOwnPeople();
		}
	}

	if (total < param2)
		return true;

	return false;
}

bool EnePeopleLessN::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2;

	return true;
}

bool BuildsNoAttack::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	Camp* pCamp = sgBattleControl()->getTargetCamp(pTargetBuild->getCamp());
	if (!pCamp)
		return false;

	const MapBulidList& buildlist = pCamp->getBuildList();
	MapBulidList::const_iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		if (it->second->checkCurBeAttack() == true)
			return false;
	}
	return true;
}

bool BuildPeopleNumN::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	if (pTargetBuild->getOwnPeople() >= param1)
		return true;
	return false;
}

bool PeopleCountInN::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2 >> param3 >> param4;

	return true;
}

bool PeopleCountInN::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	//范围
	Vec2 curBuild = pTargetBuild->getPosition();
	int dis = param2 * TILED_SIZE;

	//目标建筑列表
	MapBulidList buildlist;
	//??? ECAMP_TYPE_Player
	//sgBattleControl()->getTargetBuildList(ECAMP_TYPE_Player, buildlist);
	buildlist = sgBattleControl()->getBuildList();
	MapBulidList tmpBuildList;



	std::vector<int> vecIdx;
	vecIdx.resize(1);
	vecIdx[0] = -1;
	int absSub = -1;

	for (MapBulidList::iterator iter = buildlist.begin(); iter != buildlist.end(); iter++)
	{	
		if (param1 == 1)
		{
			//CCLOG("--buildId:%d--dis:%f, people:%d", iter->first,iter->second->getPosition().getDistance(curBuild), iter->second->getOwnPeople());
			if (iter->second->isEnable()
				&& checkCamps(pTargetBuild->getCamp(), iter->second->getCamp(), param4)
				&& iter->second->getOwnPeople() < param3
				&& iter->second->getPosition().getDistance(curBuild) <= dis
				&& iter->second->getKeyID() != pTargetBuild->getKeyID())
			{
				int sub = iter->second->getOwnPeople() - param3;
				if (std::abs(sub) > absSub)
				{
					vecIdx[0] = iter->second->getKeyID();
					absSub = std::abs(sub);
				}
			}
		}
		else if (param1 == 2)
		{
			if (iter->second->isEnable()
				&& checkCamps(pTargetBuild->getCamp(), iter->second->getCamp(), param4)
				&& iter->second->getOwnPeople() >= param3
				&& iter->second->getPosition().getDistance(curBuild) <= dis)
			{
				int sub = iter->second->getOwnPeople() - param3;
				if (std::abs(sub) > absSub)
				{
					vecIdx[0] = iter->second->getKeyID();
					absSub = std::abs(sub);
				}
			}
		}
	}
	if (vecIdx[0] != -1)
	{
		this->setOutputBuildings(vecIdx);
		return true;
	}
	return false;
}

bool HaveXXbuffer::checkState()
{	
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	return sgBuffManager()->getExistBuff(pTargetBuild->getCamp(), param1);
}

bool MoraleGtN::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2;
	return true;
}

bool MoraleGtN::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	//己方士气
	float morale = sgMoraleManager()->getMoraleValueByCamp(pTargetBuild->getCamp());
	CCLOG("moral0:%f", sgMoraleManager()->getMoraleValueByCamp((eCampType)0));
	CCLOG("moral1:%f", sgMoraleManager()->getMoraleValueByCamp((eCampType)1));
	CCLOG("moral2:%f", sgMoraleManager()->getMoraleValueByCamp((eCampType)2));

	if (param1 == 1)
	{
		if (morale < param2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (param1 == 2)
	{
		if (morale >= param2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool OwnerNotDamage::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	return !pTargetBuild->checkCurBeAttack();
}


bool OwnerCampPeople::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2;
	return true;
}

bool OwnerCampPeople::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	MapBulidList buildlist;
	sgBattleControl()->getTargetBuildList(pTargetBuild->getCamp(), buildlist);
	if (buildlist.empty())
		return false;

	int total = 0;
	MapBulidList::iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		total += it->second->getOwnPeople();
	}

	if (param1 == 1)
	{
		if (total < param2)
		{
			return true;
		}
	}
	else if (param1 == 2)
	{
		if (total >= param2)
		{
			return true;
		}
	}

	return false;
}

bool RunTransformCommand::init(std::stringstream& ssparams)
{
	ssparams >> param1;
	return true;
}

bool RunTransformCommand::checkState()
{
	BaseBuild* build = sgBattleControl()->getTargetBuildByIns(param1);

	std::vector<FormationV2*> vecFormation;
	sgBattleControl()->getApproachingBuildFormations(param1, vecFormation);
	size_t totalEnemy = 0;
	for (size_t i = 0; i < vecFormation.size(); i++)
	{
		FormationV2* v2 = vecFormation[i];
		if (v2->getDstCamp() != v2->getSrcCamp())
		{
			totalEnemy += v2->getExpectedCount();
		}
	}
	if ( (int)totalEnemy > build->getOwnPeople())
	{
		return true;
	}
	return false;
}


bool CountDownSurplus::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2;
	return true;
}


bool CountDownSurplus::checkState()
{
	//sgBalanceControl()->getMode()->getModeType();
	float leftTime = sgBalanceControl()->getTimeLeft();
	if (param1 == 1)
	{
		if (leftTime != -1 && leftTime < param2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (param2 == 2)
	{
		if (leftTime != -1 && leftTime >= param2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}


bool BuildSoldierNum::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2 >> param3 >> param4;
	return true;
}

bool BuildSoldierNum::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	BaseBuild* build = sgBattleControl()->getTargetBuildByIns(param1);
	if (build) 
	{
		if (param2 == 1)
		{
			if (build->isEnable() && checkCamps(pTargetBuild->getCamp(), build->getCamp(), param4) && build->getOwnPeople() < param3)
			{
				return true;
			}
		}
		else if (param2 == 2)
		{
			if (build->isEnable() && checkCamps(pTargetBuild->getCamp(), build->getCamp(), param4) && build->getOwnPeople() >= param3)
			{
				return true;
			}
		}
	}
	return false;
}

bool EnemySubDstSoldierNum::init(std::stringstream& ssparms)
{
	ssparms >> param1 >> param2;
	return true;
}

bool EnemySubDstSoldierNum::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	std::vector<FormationV2*> vecFormation;
	sgBattleControl()->getApproachingBuildFormations(pTargetBuild->getKeyID(), vecFormation);
	size_t total = 0;
	for (size_t i = 0; i < vecFormation.size(); i++)
	{
		FormationV2* v2 = vecFormation[i];
		total += v2->getExpectedCount();
	}
	int sub = (int)total - pTargetBuild->getOwnPeople();
	if (param1 == 1)
	{
		if (total > 0 && sub < param2)
		{
			return true;
		}
	}
	else if (param1 == 2)
	{
		if (total >0 && sub >= param2)
		{
			return true;
		}
	}
	return false;
}


bool EnemySubFriendSoldierNum::init(std::stringstream& ssparms)
{
	ssparms >> param1 >> param2 >> param3;
	return true;
}

bool EnemySubFriendSoldierNum::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	MapBulidList buildlist;
	sgBattleControl()->getTargetBuildList(pTargetBuild->getCamp(), buildlist);

	int dis = param1 * TILED_SIZE;
	Vec2 curBuild = pTargetBuild->getPosition();

	std::vector<int> vecIdx;
	vecIdx.resize(1);
	vecIdx[0] = -1;
	int maxSub = -9999;
	for (const auto& iter : buildlist)
	{
		if (iter.second->getPosition().getDistance(curBuild) <= dis)
		{
			std::vector<FormationV2*> vecFormation;
			sgBattleControl()->getApproachingBuildFormations(iter.second->getKeyID(), vecFormation);
			size_t totalEnemy = 0;
			for (size_t i = 0; i < vecFormation.size(); i++)
			{
				FormationV2* v2 = vecFormation[i];
				if (v2->getSrcCamp() != v2->getDstCamp())
				{
					totalEnemy += v2->getExpectedCount();
				}
			}
			BaseBuild* build = sgBattleControl()->getTargetBuildByIns(iter.second->getKeyID());
			int sub = (int)totalEnemy - build->getOwnPeople();
			if (param2 == 1)
			{
				if (totalEnemy > 0 && sub < param3)
				{
					if (sub > maxSub)
					{
						vecIdx[0] = iter.second->getKeyID();
						maxSub = sub;
					}
				}
			}
			else if (param2 == 2)
			{
				if (totalEnemy >0 && sub >= param3)
				{
					if (sub > maxSub)
					{
						vecIdx[0] = iter.second->getKeyID();
						maxSub = sub;
					}
				}
			}

		}
	}
	if (vecIdx[0] != -1)
	{
		this->setOutputBuildings(vecIdx);
		return true;
	}
	return false;
}


bool EnemySubOtherSoldierNum::init(std::stringstream& ssparms)
{
	ssparms >> param1 >> param2 >> param3;
	return true;
}

bool EnemySubOtherSoldierNum::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	MapBulidList buildlist = sgBattleControl()->getBuildList();

	int dis = param1 * TILED_SIZE;
	Vec2 curBuild = pTargetBuild->getPosition();

	std::vector<int> vecIdx;
	vecIdx.resize(1);
	vecIdx[0] = -1;
	int minAbs = 999999;
	for (const auto& iter : buildlist)
	{
		if (iter.second->isEnable() && pTargetBuild->getCamp() != iter.second->getCamp() && iter.second->getPosition().getDistance(curBuild) <= dis)
		{
			std::vector<FormationV2*> vecFormation;
			sgBattleControl()->getApproachingBuildFormations(iter.second->getKeyID(), vecFormation);
			size_t total = 0;
			for (size_t i = 0; i < vecFormation.size(); i++)
			{
				FormationV2* v2 = vecFormation[i];
				if (v2->getSrcCamp() != v2->getCurDstCamp() && v2->getCurDstCamp() != pTargetBuild->getCamp())
				{
					total += v2->getExpectedCount();
				}
			}
			BaseBuild* build = sgBattleControl()->getTargetBuildByIns(iter.second->getKeyID());
			int sub = (int)total - build->getOwnPeople();
			if (param2 == 1)
			{
				if (total > 0 && sub < param3)
				{
					if (minAbs > std::abs(sub))
					{
						minAbs = std::abs(sub);
						vecIdx[0] = iter.second->getKeyID();
					}
				}
			}
			else if (param2 == 2)
			{
				if (total >0 && sub >= param3)
				{
					if (minAbs > std::abs(sub))
					{
						minAbs = std::abs(sub);
						vecIdx[0] = iter.second->getKeyID();
					}
				}
			}

		}
	}
	if (vecIdx[0] != -1)
	{
		this->setOutputBuildings(vecIdx);
		return true;
	}
	return false;
}



bool EnemyBuildAfterSendNum::init(std::stringstream& ssparms)
{
	ssparms >> param1 >> param2 >> param3;
	return true;
}

bool EnemyBuildAfterSendNum::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	int dis = param1 * TILED_SIZE;
	Vec2 curPos = pTargetBuild->getPosition();


	std::vector<int> vecIdx;
	vecIdx.resize(1);
	vecIdx[0] = -1;
	int minSoldier = 999999;

	MapBulidList buildlist = sgBattleControl()->getBuildList();
	for (const auto& iter : buildlist)
	{
		//1. 阵营  2.是否在攻击 3.距离
		if (iter.second->getCamp() != pTargetBuild->getCamp())
		{
			if (iter.second->checkAttacking()
				&& iter.second->getPosition().getDistance(curPos) < dis)
			{
				if (param2 == 1)
				{
					if (iter.second->getOwnPeople() < param3)
					{
						if (iter.second->getOwnPeople() < minSoldier)
						{
							vecIdx[0] = iter.second->getKeyID();
							minSoldier = iter.second->getOwnPeople();
						}
					}
				}
				else if (param2 == 2)
				{
					if (iter.second->getOwnPeople() >= param3)
					{
						if (iter.second->getOwnPeople() < minSoldier)
						{
							vecIdx[0] = iter.second->getKeyID();
							minSoldier = iter.second->getOwnPeople();
						}
					}
				}
			}
		}
	}
	if (vecIdx[0] != -1)
	{
		this->setOutputBuildings(vecIdx);
		return true;
	}
	return false;
}

bool EnemyBuildAfterReceiveNum::init(std::stringstream& ssparms)
{
	// param1 range范围 param2 大于小于 param3 兵力数量
	ssparms >> param1 >> param2 >> param3;
	return true;
}

bool EnemyBuildAfterReceiveNum::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	int dis = param1 * TILED_SIZE;
	Vec2 curPos = pTargetBuild->getPosition();


	std::vector<int> vecIdx;
	vecIdx.resize(1);
	vecIdx[0] = -1;
	int minSoldier = 9999;

	MapBulidList buildlist = sgBattleControl()->getBuildList();
	for (const auto& iter : buildlist)
	{
		//1. 敌方阵营
		if (iter.second->getCamp() != pTargetBuild->getCamp())
		{
			// 2.距离
			if (iter.second->getPosition().getDistance(curPos) < dis)
			{
				// 3.
				std::vector<FormationV2*> vecFormation;
				sgBattleControl()->getApproachingBuildFormations(iter.second->getKeyID(), vecFormation);
				size_t totalReceive = 0;
				for (size_t i = 0; i < vecFormation.size(); i++)
				{
					FormationV2* v2 = vecFormation[i];
					if (v2->getSrcCamp() == v2->getCurDstCamp())
					{
						totalReceive += v2->getExpectedCount();
					}
				}
				int afterReceive = (int)totalReceive + iter.second->getOwnPeople();
				if (param2 == 1)
				{

					if (totalReceive >0 && afterReceive < param3)
					{
						if (afterReceive < minSoldier)
						{
							minSoldier = afterReceive;
							vecIdx[0] = iter.second->getKeyID();
						}
					}
				}
				else if (param2 == 2)
				{
					if (totalReceive >0 && afterReceive >= param3)
					{
						if (afterReceive < minSoldier)
						{
							minSoldier = afterReceive;
							vecIdx[0] = iter.second->getKeyID();
						}
					}
				}
			}
		}
	}
	if (vecIdx[0] != -1)
	{
		this->setOutputBuildings(vecIdx);
		return true;
	}
	return false;
}




bool SelfBuildLevel::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2;
	return true;
}

bool SelfBuildLevel::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	if (param1 == 1)
	{
		if (pTargetBuild->getLevel() < param2)
			return true;
	}
	else if (param1 == 2)
	{
		if (pTargetBuild->getLevel() >= param2)
			return true;
	}
	
	return false;
}

//30#M#R#N  M：1~3（1 兵营，2 炮塔，3 研究所），R：1~2，N：1~99
bool BuildTypeNum::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2 >> param3;
	return true;
}

bool BuildTypeNum::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	Camp* pCamp = sgBattleControl()->getTargetCamp(pTargetBuild->getCamp());
	if (!pCamp)
		return false;
	if (param1 <1 || param1 > 3)
		return false;

	if (param2 == 1)
	{
		if (pCamp->getBuildNumByType((eBulidType)param1) < param3)
			return true;
	}
	else if (param2 == 2)
	{
		if (pCamp->getBuildNumByType((eBulidType)param1) >= param3)
			return true;
	}
	return false;
}

bool TimeCompare::init(std::stringstream & ssparams)
{
	ssparams >> param1 >> param2;
	const CheckPoint* pAttr = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);
	if (pAttr)
		param2 = pAttr->timeLimit - param2;
	
	return true;
}

bool TimeCompare::checkState()
{
	float leftTime = sgBalanceControl()->getTimeLeft();
	if (leftTime == -1)
		return false;

	if (param2 < 0)
		return false;

	if (param1 == 1)
		return param2 < leftTime ;
	else if (param1 == 2)
		return  param2 >= leftTime;

	return false;
}


bool SoldierNumCompareALL::init(std::stringstream & ssparams)
{
	ssparams >> param1 >> param2;
	return true;
}

bool SoldierNumCompareALL::checkState()
{
	auto pTargetBuild = this->getTargetBuild();
	if (nullptr == pTargetBuild)
		return false;

	if (param2 < 0 || !pTargetBuild)
		return false;
	
	Camp* pCamp = sgBattleControl()->getTargetCamp(pTargetBuild->getCamp());
	if (!pCamp)
		return false;

	int  cur = pCamp->getAllUnit();
	float  max = pCamp->getCampMaxProduct();
	int  ratio = cur / max * 100.f;

	if (param1 == 1)
		return param2 > ratio;
	else if (param1 == 2)
		return param2 <= ratio;

	return false;
}

bool SildierNumOrBuildNum::init(std::stringstream & ssparams)
{
	ssparams >> param1 >> param2 >> param3;
	return true;
}

bool SildierNumOrBuildNum::checkState()
{
	//33#N#R#M
	//N : 1 - 总兵力，2 - 总建筑数量
	//R : 1 - 小于，2 - 大于等于
	//	结果与M比较
	int enemyNum = 0;
	if (param1 == 1)
	{
		int tempNum = sgBattleControl()->getEnemyBuildsMaxPeople(getOwnCamp());
		if (tempNum == 0)
			return false;
		enemyNum = tempNum;
	}
	else if (param1 == 2)
	{
		MapBulidList buildList;
		sgBattleControl()->getEnemyBuildsMaxPeople(getOwnCamp(), buildList);
		if (buildList.empty())
			return false;
		enemyNum = buildList.size();
	}
		
	if (param2 == 1)
		return enemyNum > param3;
	else if (param2 == 2)
		return enemyNum <= param3;

	return false;
}


bool PlayerPropCount::init(std::stringstream & ssparams)
{
	ssparams >> param1 >> param2 >> param3;
	return true;
}

bool PlayerPropCount::checkState()
{
	int propId = param1;
	int targetPropCount = param3;
	int maxTakeCount = 0;

	//自己的道具数据
	auto selfPorp = sgUserObj()->getPropList();
	for (map<int, int>::iterator iter = selfPorp.begin(); iter != selfPorp.end(); iter++)
	{
		if (iter->first == propId && iter->second > maxTakeCount)
		{
			maxTakeCount = iter->second;
		}
	}

	//对手的道具数据
	fgame::CSPvpMatchResp* matchRsp = sgPvpControl()->getMatchRsp();
 
	for (int i = 0; i < matchRsp->playinfo_size(); i++)
	{
		const fgame::CSPlayInfo& playerInfo = matchRsp->playinfo(i);

		for (int j = 0; j < playerInfo.propdata_size(); j++)
		{
			auto propdata = playerInfo.propdata(j);

			if (propdata.itemid() == propId && propdata.count() > maxTakeCount)
			{
				maxTakeCount = propdata.count();
			}
		}
	}

	if (param2 == 1 && maxTakeCount < targetPropCount)
	{
		//	<
		return true;
	}
	else if (param2 == 2 && maxTakeCount >= targetPropCount)
	{
		//	>=
		return true;
	}

	return false;
}
