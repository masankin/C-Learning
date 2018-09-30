#include "BaseAction.h"
#include "battle/Build.h"
#include "battle/Camp.h"
#include "battle/BattleControl.h"
#include "battle/Formation/FormationV2.h"
#include "prop/PropsControl.h"
bool BaseAction::checkCamps(const int srcCamp, const int dstCamp, int relation)
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


void BaseAction::getNearBuildings(int keyId, int relation , std::vector<BaseBuild*>& vecBuildings)
{
	BaseBuild* pOwnBuild = sgBattleControl()->getTargetBuild(keyId);
	if (!pOwnBuild)
	{
		CCLOG("[error]getNearestBuild key:%d!", keyId);
		return ;
	}
	Vec2 curBuildPos = mTargetBuild->getPosition();
	MapBulidList buildList = sgBattleControl()->getBuildList();
	MapBulidList::iterator it = buildList.begin();
	for (; it != buildList.end(); ++it)
	{
		if (/*it->second->getBuildInfo().locationID != 0
			&&*/ it->second->isEnable()
			&& checkCamps(pOwnBuild->getCamp(), it->second->getCamp(), relation))
		{
			vecBuildings.push_back(it->second);
		}
	}

	std::sort(std::begin(vecBuildings), std::end(vecBuildings), [&curBuildPos](BaseBuild* build1, BaseBuild* build2)
	{
		return  build1->getPosition().getDistance(curBuildPos) < build2->getPosition().getDistance(curBuildPos);
	});
}

bool ActionAllAttack::doAction()
{
	Camp* pOwn = sgBattleControl()->getTargetCamp(mTargetBuild->getCamp());
	if (!pOwn)
		return false;

	//BaseBuild* pNearTarget = sgBattleControl()->getNearestBuild(mTargetBuild->getKeyID(), true);
	//if (!pNearTarget)
	//	return false;

	std::vector<BaseBuild* > vecBuild;
	getNearBuildings(mTargetBuild->getKeyID(), 2, vecBuild);
	if (vecBuild.empty())
	{
		return false;
	}
	const MapBulidList& buildlist = pOwn->getBuildList();
	MapBulidList::const_iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		char params[128] = { 0, };
		int randomTarget = RandomHelper::random_int(0, vecBuild.size()>3? 2:(int)(vecBuild.size()-1));
		BaseBuild* build = vecBuild[randomTarget];
		if (build == nullptr)
		{
			continue;
		}
		sprintf(params, "%d %d %d %d %d", it->second->getKeyID(), build->getKeyID(), FormationV2::NewId(), 0, 80);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
	}
	return true;
}


bool ActionBuildUpgrade::doAction()
{
	// 建筑升级
	char params[64] = { 0, };
	sprintf(params, "%d", mTargetBuild->getKeyID());
	sgMessageManager()->sendMessage(EMT_UPGRADE, params);
	return true;
}

bool ActionShiftNum::doAction()
{
	int randIns = 0;
	if (m_inBuildingIds.size() != 0)
	{
		randIns = m_inBuildingIds[0];
	}
	else
	{
		randIns = getRuleBuildIns();
	}

	BaseBuild* pRandBuild = sgBattleControl()->getTargetBuildByIns(randIns);
	if (pRandBuild)
	{
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", mTargetBuild->getKeyID(), pRandBuild->getKeyID(), FormationV2::NewId(), 1, mShiftParam);
		if (pRandBuild->getCamp() != mTargetBuild->getCamp())
		{
			return false;
		}
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}

bool ActionShiftNum::init(std::stringstream& ssparams)
{
	for (int i = 0; i != 6; ++i)
	{
		int iParam = 0;
		ssparams >> iParam;

		vParam.push_back(iParam);
	}

	std::vector<int>::iterator it = vParam.begin();
	for (; it != vParam.end();)
	{
		if (*it == 0)
		{
			it = vParam.erase(it);
		}
		else{
			it++;
		}
	}

	mShiftParam = vParam.back();
	vParam.pop_back();

	return true;
}

int ActionShiftNum::getRuleBuildIns()
{
	int randIns = 0;
	//筛选出己方阵营并按照兵力由小到大排序
	eCampType eOwnCamp = mTargetBuild->getCamp();
	std::vector<int> tmpVec(vParam.begin(), vParam.end());
	vParam.clear();

	for (std::vector<int>::iterator iter = tmpVec.begin(); iter != tmpVec.end(); iter++)
	{
		BaseBuild* pBuildIter = sgBattleControl()->getTargetBuildByIns(*iter);
		if (!pBuildIter || pBuildIter->getCamp() != eOwnCamp)
		{
			continue;
		}

		if (!vParam.empty())
		{
			//CCLOG("people  %d  ", sgBattleControl()->getTargetBuildByIns(tmpVec.at(0))->getOwnPeople());
			//CCLOG("people  %d  ", sgBattleControl()->getTargetBuildByIns(*iter)->getOwnPeople());
			BaseBuild* pBuildZero = sgBattleControl()->getTargetBuildByIns(tmpVec.at(0));
			if (!pBuildZero)
				continue;

			if (pBuildZero->getOwnPeople() > pBuildIter->getOwnPeople())
			{
				vParam.insert(vParam.begin(), *iter);
			}
			else
			{
				vParam.push_back(*iter);
			}
		}
		else
		{
			vParam.push_back(*iter);
		}
	}

	//获得兵力最小，并且相等兵力的建筑数量
	int meetEnmeyCount = 1;
	for (size_t i = 0; i < vParam.size(); i++)
	{
		if (i + 1 < vParam.size())
		{
			BaseBuild* pBuild1 = sgBattleControl()->getTargetBuildByIns(vParam.at(i));
			BaseBuild* pBuild2 = sgBattleControl()->getTargetBuildByIns(vParam.at(i + 1));
			if (!pBuild1 || !pBuild2)
				continue;

			int peopleCount1 = pBuild1->getOwnPeople();
			int peopleCount2 = pBuild2->getOwnPeople();

			if (peopleCount1 == peopleCount2)
			{
				meetEnmeyCount += 1;
			}
			else
			{
				break;
			}
		}
	}

	if (vParam.empty())
	{
		return -1;
	}
	
	if (vParam.size() == 1)
		randIns = vParam.at(0);
	else
	{
		//randIns = vParam.at(rand() % meetEnmeyCount);
		randIns = vParam.at(rand() % meetEnmeyCount);
	}
	return randIns;
}

bool ActionShiftRate::doAction()
{
	int randIns = 0;
	if (m_inBuildingIds.size() != 0)
	{
		randIns = m_inBuildingIds[0];
	}
	else
	{
		randIns = getRuleBuildIns();
	}
	BaseBuild* pRandBuild = sgBattleControl()->getTargetBuildByIns(randIns);
	if (pRandBuild)
	{
		if (pRandBuild->getCamp() != mTargetBuild->getCamp())
		{
			return false;
		}
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", mTargetBuild->getKeyID(), pRandBuild->getKeyID(), FormationV2::NewId(), 0, mShiftParam);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}

bool ActionAttackNum::init(std::stringstream& ssparams)
{
	for (int i = 0; i != 6; ++i)
		param[i] = -1;

	for (int i = 0; i != 6; ++i)
		ssparams >> param[i];
	return true;
}

bool ActionAttackNum::doAction()
{
	int randIns = 0;
	if (m_inBuildingIds.size() != 0)
	{
		randIns = m_inBuildingIds[0];
	}
	else
	{
		randIns = getRuleBuildIns();
	}

	BaseBuild* pRandBuild = sgBattleControl()->getTargetBuildByIns(randIns);
	if (pRandBuild)
	{
		int para = -1;

		for (int i = 5; i != -1;--i)
		{
			if (param[i] != -1)
			{
				para = param[i];
				break;
			}
		}

		if (para == -1)
		{
			CCLOG("[error] ActionAttackNum::doAction get param error!");
			return false;
		}

		char params[128] = { 0, };
		//指定为敌方
		if (mTargetBuild->getCamp() == pRandBuild->getCamp())
		{
			return false;
		}
		sprintf(params, "%d %d %d %d %d", mTargetBuild->getKeyID(), pRandBuild->getKeyID(), FormationV2::NewId(), 1, para);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}

int ActionAttackNum::getRuleBuildIns()
{
	bool bIgnoreLast = false;
	std::vector<int> BuildInsKeyList;
	for (int i = 5; i != -1; --i)
	{
		// 过滤最后一个出兵比例或者个数的参数
		if (param[i] != -1)
		{
			if (bIgnoreLast == true)
			{
				BuildInsKeyList.push_back(param[i]);
			}
			else
			{
				bIgnoreLast = true;
			}
		}
	}

	eCampType eOwnCamp = mTargetBuild->getCamp();

	// 过滤出敌方建筑，优先判定攻击
	std::vector<int> enemyInsKeys;
	std::vector<int>::iterator it = BuildInsKeyList.begin();
	for (; it != BuildInsKeyList.end();)
	{
		int ins = *it;
		
		BaseBuild* pBuild = sgBattleControl()->getTargetBuildByIns(ins);

		if (!pBuild || !pBuild->isEnable())
		{
			it = BuildInsKeyList.erase(it);
			continue;
		}
		else
			it++;

		if (pBuild->getCamp() != eOwnCamp)
		{	
			//顺便按照兵力由小到大排序
			if (!enemyInsKeys.empty())
			{
				int ins2 = enemyInsKeys.at(0);

				BaseBuild* pBuild2 = sgBattleControl()->getTargetBuildByIns(ins2);
				if (!pBuild2)
					return -1;

				if (pBuild2->getOwnPeople() > pBuild->getOwnPeople())
				{
					enemyInsKeys.insert(enemyInsKeys.begin(), ins);
				}
				else
				{
					enemyInsKeys.push_back(ins);
				}
			}
			else
			{
				enemyInsKeys.push_back(ins);
			}
		}
	}

	int randIns = 0;
	if (!enemyInsKeys.empty())
	{	
		//获得兵力最小，并且相等兵力的敌人的数量
		int meetEnmeyCount = 1;
		for (size_t i = 0; i < enemyInsKeys.size(); i++)
		{
			if ( i + 1 < enemyInsKeys.size() )
			{	
				BaseBuild* pBuild1 = sgBattleControl()->getTargetBuildByIns(enemyInsKeys.at(i));
				BaseBuild* pBuild2 = sgBattleControl()->getTargetBuildByIns(enemyInsKeys.at(i + 1));
				if (!pBuild1 || !pBuild2)
					continue;

				int peopleCount1 = pBuild1->getOwnPeople();
				int peopleCount2 = pBuild2->getOwnPeople();

				if (peopleCount1 == peopleCount2)
				{
					meetEnmeyCount += 1;
				}
				else
				{
					break;
				}
			}
		}

		if (enemyInsKeys.size() == 1)
		{
			randIns = enemyInsKeys.at(0);
		}
		else
		{
			randIns = enemyInsKeys.at(rand() % meetEnmeyCount);
		}
	}
	else
	{
		if (BuildInsKeyList.empty())
		{
			CCLOG("[fault]ActionAttackNum::getRuleBuildIns id:%d params error!", getActionTag());
			return -1;
		}

		if (BuildInsKeyList.size() == 1)
			randIns = BuildInsKeyList.at(0);
		else
		{
			randIns = BuildInsKeyList.at(rand() % BuildInsKeyList.size());
		}
	}
	return randIns;
}

bool ActionAttackRate::init(std::stringstream& ssparams)
{
	return ActionAttackNum::init(ssparams);
}

bool ActionAttackRate::doAction()
{
	int randIns = 0;
	if (m_inBuildingIds.size() != 0)
	{
		randIns = m_inBuildingIds[0];
	}
	else
	{
		randIns = getRuleBuildIns();
	}


	BaseBuild* pRandBuild = sgBattleControl()->getTargetBuildByIns(randIns);
	if (pRandBuild)
	{
		int para = -1;

		for (int i = 5; i != -1; --i)
		{
			if (param[i] != -1)
			{
				para = param[i];
				break;
			}
		}

		if (para == -1)
		{
			CCLOG("[error] ActionAttackNum::doAction get param error!");
			return false;
		}

		//指定为敌方
		if (mTargetBuild->getCamp() == pRandBuild->getCamp())
		{
			return false;
		}
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", mTargetBuild->getKeyID(), pRandBuild->getKeyID(), FormationV2::NewId(), 0, para);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}

bool ActionHelpNum::init(std::stringstream& ssparams)
{
	ssparams >> param1 >> param2;
	return true;
}

bool ActionHelpNum::doAction()
{
	int key = getRuleBuildKey(1, param2);

	BaseBuild* pBuild = sgBattleControl()->getTargetBuild(key);
	if (pBuild)
	{
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", pBuild->getKeyID(), mTargetBuild->getKeyID(), FormationV2::NewId(), 1, param1);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}

int ActionHelpNum::getRuleBuildKey(int type, int limit)
{
	Vec2 curBuild = mTargetBuild->boundingBox().origin;
	int dis = 80 * TILED_SIZE;

	Camp* pCamp = sgBattleControl()->getTargetCamp(mTargetBuild->getCamp());
	if (!pCamp)
		return 0;

	int iRetKey = 0;
	const MapBulidList& buildlist = pCamp->getBuildList();
	MapBulidList::const_iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		int needPeop = limit;
		if (type == 0)	// 如果是达到生产上限百分比的要求
		{
			int key = it->second->getBuildInfo().id;
			const BuildAttr* pAttr = sgTableManager()->getBuildAttr(key);
			if (pAttr)
				needPeop = pAttr->proLimit * 1.0f * limit / 100;
		}

		int _dis = it->second->boundingBox().origin.getDistance(curBuild);
		if (it->second->checkCurBeAttack() == false &&
			_dis <= dis &&
			it->second->getOwnPeople() >= needPeop)
		{
			dis = _dis;
			iRetKey = it->second->getKeyID();
		}
	}
	return iRetKey;
}

bool ActionHelpRate::doAction()
{
	int key = getRuleBuildKey(0, param2);
	CCLOG("[trace]ActionHelpRate::doAction helpBuild:%d!", key);

	BaseBuild* pBuild = sgBattleControl()->getTargetBuild(key);
	if (pBuild && pBuild->getKeyID() != mTargetBuild->getKeyID())
	{
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", pBuild->getKeyID(), mTargetBuild->getKeyID(), FormationV2::NewId(), 0, param1);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}

bool ActionMonsterNum::init(std::stringstream& ssparams)
{
	return ActionAttackNum::init(ssparams);
}

bool ActionMonsterNum::doAction()
{
	int randIns = 0;
	if (m_inBuildingIds.size() != 0)
	{
		randIns = m_inBuildingIds[0];
	}
	else
	{
		randIns = getRuleBuildIns();
	}
	BaseBuild* pRandBuild = sgBattleControl()->getTargetBuildByIns(randIns);
	if (!pRandBuild 
		/*|| pRandBuild->getBuildInfo().locationID == 0*/
		|| !pRandBuild->isEnable())
	{
		return false;
	}

	if (pRandBuild)
	{
		int para = -1;

		for (int i = 5; i >= 0;--i)
		{
			if (param[i] != -1)
			{
				para = param[i];
				break;
			}
		}

		if (para == -1)
		{
			CCLOG("[error]ActionMonsterNum::doAction: get param error!");
			return false;
		}

		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", mTargetBuild->getKeyID(), pRandBuild->getKeyID(), FormationV2::NewId(), 1, para);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}

///////////////////////////////////////
bool ActionMonsterRate::init(std::stringstream& ssparams)
{
	return ActionAttackNum::init(ssparams);
}


bool ActionMonsterRate::doAction()
{
	int randIns = 0;
	if (m_inBuildingIds.size() != 0)
	{
		randIns = m_inBuildingIds[0];
	}
	else
	{
		randIns = getRuleBuildIns();
	}

	BaseBuild* pRandBuild = sgBattleControl()->getTargetBuildByIns(randIns);
	if (nullptr == pRandBuild
		/*|| pRandBuild->getBuildInfo().locationID == 0 */
		|| !pRandBuild->isEnable() )
	{
		return false;
	}
	if (pRandBuild)
	{
		int para = -1;

		for (int i = 5; i != -1; --i)
		{
			if (param[i] != -1)
			{
				para = param[i];
				break;
			}
		}

		if (para == -1)
		{
			CCLOG("[error] ActionAttackNum::doAction get param error!");
			return false;
		}

		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", mTargetBuild->getKeyID(), pRandBuild->getKeyID(), FormationV2::NewId(), 0, para);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}

bool ActionAttackRange::init(std::stringstream& ssparams)
{
	// parmam1N格  parmam2  数量
	ssparams >> param1 >> param2;
	return true;
}



BaseBuild* ActionAttackRange::getMinimalBuild()
{
	Vec2 curPos = mTargetBuild->getPosition();
	int dis = param1 * TILED_SIZE;
	BaseBuild* minimal = nullptr;
	for (const auto& iter : sgBattleControl()->getBuildList())
	{
		if (!iter.second->isEnable())
			continue;
		// 正营不相同并且在范围内
		if (mTargetBuild->getCamp() != iter.second->getCamp()
			&& (iter.second->getPosition().getDistance(curPos) <= dis))
		{
			if (minimal == nullptr || minimal->getOwnPeople() >= iter.second->getOwnPeople())
			{
				minimal = iter.second;
			}
		}
	}
	return minimal;
}

bool ActionAttackRange::doAction()
{
	//Vec2 curPos = mTargetBuild->getPosition();
	//int dis = param1 * TILED_SIZE;

	//BaseBuild* minimal = nullptr;
	////从敌对阵营中找出最小的
	//for (const auto& iter : sgBattleControl()->getBuildList())
	//{
	//	// 正营不相同并且在范围内
	//	if (mTargetBuild->getCamp() != iter.second->getCamp() 
	//		&& (iter.second->getPosition().getDistance(curPos) <= dis))
	//	{
	//		if (minimal == nullptr ||  minimal->getOwnPeople() >= iter.second->getOwnPeople())
	//		{
	//			minimal = iter.second;
	//		}
	//	}
	//}

	BaseBuild* minimal = getMinimalBuild();
	if (minimal)
	{
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", mTargetBuild->getKeyID(), minimal->getKeyID(), FormationV2::NewId(), 1, param2);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}

bool ActionAttackNearest::init(std::stringstream& ssparams)
{
	ssparams >> param1;
	return true;
}

BaseBuild* ActionAttackNearest::getNearestBuild()
{
	Vec2 curPos = mTargetBuild->getPosition();
	BaseBuild* nearest = nullptr;
	//从敌对阵营中找出最小的
	for (const auto& iter : sgBattleControl()->getBuildList())
	{
		if (!iter.second->isEnable())
			continue;
		// 正营不相同并且在范围内
		if (mTargetBuild->getCamp() != iter.second->getCamp())
		{
			if (nearest == nullptr || nearest->getPosition().getDistance(curPos) >= iter.second->getPosition().getDistance(curPos))
			{
				nearest = iter.second;
			}
		}
	}
	return nearest;
}

bool ActionAttackNearest::doAction()
{
	//Vec2 curPos = mTargetBuild->getPosition();
	//BaseBuild* nearest = nullptr;
	////从敌对阵营中找出最小的
	//for (const auto& iter : sgBattleControl()->getBuildList())
	//{
	//	// 正营不相同并且在范围内
	//	if (mTargetBuild->getCamp() != iter.second->getCamp())
	//	{
	//		if (nearest == nullptr || nearest->getPosition().getDistance(curPos) >= iter.second->getPosition().getDistance(curPos))
	//		{
	//			nearest = iter.second;
	//		}
	//	}
	//}

	BaseBuild* nearest = getNearestBuild();
	if (nearest)
	{
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", mTargetBuild->getKeyID(), nearest->getKeyID(), FormationV2::NewId(), 1, param1);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}



bool ActionAttackRateRange::init(std::stringstream& ssparams)
{
	// parmam1N格  parmam2  数量
	ssparams >> param1 >> param2;
	return true;
}


bool ActionAttackRateRange::doAction()
{
	BaseBuild* minimal = getMinimalBuild();
	if (minimal)
	{
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", mTargetBuild->getKeyID(), minimal->getKeyID(), FormationV2::NewId(), 0, param2);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}

bool ActionAttackRateNearest::init(std::stringstream& ssparams)
{
	ssparams >> param1;
	return true;
}

bool ActionAttackRateNearest::doAction()
{
	BaseBuild* nearest = getNearestBuild();
	if (nearest)
	{
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", mTargetBuild->getKeyID(), nearest->getKeyID(), FormationV2::NewId(), 0, param1);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		return true;
	}
	return false;
}

bool ActionPropSildier::init(std::stringstream& ssparams)
{
	ssparams >> param1;
	return true;
}

bool ActionPropSildier::doAction()
{

	BaseBuild *build = getBuildId();
	if (build != NULL)
	{
		sgPropsControl()->doFullPeopol(build->mBuildID,build->getCamp());
		return  true;
	}
	return false;
}

BaseBuild* ActionPropSildier::getBuildId()
{
	int buildId;
	if (param1 == 0)
	{
		if (m_inBuildingIds.empty())
			return NULL;
		buildId = m_inBuildingIds.at(0);
	}
	else
		buildId = param1;
	
	return sgBattleControl()->getTargetBuild(buildId);

}

BaseBuild* ActionPropSildierType::getBuildId()
{
	Camp *camp = sgBattleControl()->getTargetCamp( mTargetBuild->getCamp());
	BaseBuild * build = camp->getBiludByMinBuild((eBulidType)param1);
	return build;
}

bool ActionProBoom::doAction()
{
	FormationV2 *formation = sgBattleControl()->getEnemyMaxPeopleFormation(mTargetBuild->getCamp());
	if (formation == NULL)
		return false;
	
	if (formation->getPosition() == Point::ZERO)
		return false;

	sgPropsControl()->doCookingSolider(formation->getPosition(), mTargetBuild->getCamp());
	return true;
}

bool ActionProIce::doAction()
{
	sgPropsControl()->setProp(10007);
	sgPropsControl()->doFreeze( mTargetBuild->getCamp());
	return true;
}

bool ActionProOil::doAction()
{
	FormationV2 *formation = sgBattleControl()->getEnemyMaxPeopleFormation(mTargetBuild->getCamp());
	if (formation == NULL)
		return false;

	if (formation->getPosition() == Point::ZERO)
		return false;

	sgPropsControl()->doOil(formation->getPosition(), mTargetBuild->getCamp());
	return true;
}
