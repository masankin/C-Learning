#include "CreepsManager.h"
#include "pvp/PvpProxy.h"
#include "pvp/PvpControl.h"
#include "battle/BattleControl.h"
#include "core/TimeCalculator.h"
#include "battle/BattleRecordsManager.h"

/************************************************************************/
/*               CreepsManager::CreepsPointInfo                         */
/************************************************************************/
CreepsManager::CreepsPointInfo::CreepsPointInfo(const CreepsPoint* pData, const RefreshCallBack& callback) : mData(pData)
, mCallback(callback)
, mHittedCreepsID(0)
{	
}


CreepsManager::CreepsPointInfo::~CreepsPointInfo()
{
	mCallback = nullptr;
	mTimerKeyName.clear();
	mCreepsList.clear();
	mHittedCreepsID = 0;
}


bool CreepsManager::CreepsPointInfo::init()
{
	if (nullptr == mData)
		return false;

	mTimerKeyName = StringUtils::format("CreepsManager_Point%d_Refresh", mData->getID());

	const std::vector<int> & vCreepsList = mData->getCreepsList();
	for (auto & iCreepsID : vCreepsList)
	{
		const Creeps* pCreeps = sgTableManager()->getCreeps(iCreepsID);
		if (pCreeps)
			mCreepsList.push_back(pCreeps);
	}

	return true;
}


/************************************************************************/
/*                     CreepsManager::GroupInfo                         */
/************************************************************************/
CreepsManager::GroupInfo::GroupInfo() : mGroupId(0)
{

}


CreepsManager::GroupInfo::~GroupInfo()
{
	mGroupId = 0;
	mTimerKey.clear();
	mCreepsPtList.clear();
}



/************************************************************************/
/*                            CreepsManager                             */
/************************************************************************/
CreepsManager::CreepsManager()
{

}


CreepsManager::~CreepsManager()
{
	this->cleanup();
}


void CreepsManager::cleanup()
{
	for (auto itr = mGroupList.begin(); itr != mGroupList.end(); ++itr)
	{
		GroupInfo* pInfo = itr->second;
		if (pInfo)
		{
			sgTimeCalculator()->removeTimeCalcultor(pInfo->mTimerKey);
			delete pInfo;
			pInfo = nullptr;
		}
	}
	mGroupList.clear();

	for (auto itr = mCreepsPointList.begin(); itr != mCreepsPointList.end(); ++itr)
	{
		CreepsPointInfo* pInfo = itr->second;
		if (pInfo)
		{
			//sgTimeCalculator()->removeTimeCalcultor(pInfo->mTimerKeyName);
			delete pInfo;
			pInfo = nullptr;
		}
		
	}
	mCreepsPointList.clear();

	mExistingCreepsList.clear();
}


void CreepsManager::update(float dt)
{
	/*for (auto it = mCreepsPointList.begin(); it != mCreepsPointList.end(); ++it)
	{
		CreepsPointInfo* pInfo = it->second;
		if (nullptr == pInfo)
			continue;

		if (sgTimeCalculator()->getTimeLeft(pInfo->mTimerKeyName) == 0)
		{
			if (this->refreshCreepsPoint(pInfo))
			{
				sgTimeCalculator()->removeTimeCalcultor(pInfo->mTimerKeyName);
			}
			else
			{
				sgTimeCalculator()->resetTimeCalcultor(pInfo->mTimerKeyName);
			}
		}
	}*/

	for (auto itr = mGroupList.begin(); itr != mGroupList.end(); ++itr)
	{
		GroupInfo* pInfo = itr->second;
		if (nullptr == pInfo)
			continue;

		if (sgTimeCalculator()->getTimeLeft(pInfo->mTimerKey) == 0)
		{
			this->refreshGroup(pInfo);
			sgTimeCalculator()->resetTimeCalcultor(pInfo->mTimerKey);

		}
	}
}

void CreepsManager::creepsBorn(int creepsId, int hittedCreepsId)
{

	auto itrCreepsPoint = mCreepsPointList.find(creepsId);
	if (mCreepsPointList.end() == itrCreepsPoint)
		return;

	CreepsPointInfo* pPointInfo = itrCreepsPoint->second;
	if (nullptr == pPointInfo)
	{
		CCLOG("[warn]CreepsManager::syncPvpCreepsInfo : Haven't creeps point[%d]", hittedCreepsId);
		return;
	}

	const std::vector<const Creeps*> & vecCreeps = pPointInfo->mCreepsList;
	const Creeps* pHitCreeps = nullptr;

	for (unsigned int index = 0; index < vecCreeps.size(); ++index)
	{
		const Creeps* pCreeps = vecCreeps[index];
		if (pCreeps && pCreeps->getID() == hittedCreepsId)
			pHitCreeps = pCreeps;
	}

	if (nullptr == pHitCreeps)
	{
		CCLOG("[warn]CreepsManager::syncPvpCreepsInfo : Haven't creeps[%d]", hittedCreepsId);
		return;
	}

	if (!pPointInfo->mCallback(pHitCreeps))
		return;

	this->addCreepsToExistingList(pHitCreeps->getID());
	pPointInfo->mHittedCreepsID = pHitCreeps->getID();

	
}

CreepsManager::CreepsPointInfo* CreepsManager::getCreepsPoint(int id) const
{
	auto itFind = mCreepsPointList.find(id);
	if (itFind != mCreepsPointList.end())
		return itFind->second;

	return nullptr;
}


void CreepsManager::addCreepsPoint(int id, const RefreshCallBack& callback)
{
	if (nullptr != this->getCreepsPoint(id))
	{
		CCLOG("[error]CreepsManager::addCreeps: have same creeps point.");
		return;
	}

	const CreepsPoint * pCreepsPoint = sgTableManager()->getCreepsPoint(id);
	if (nullptr == pCreepsPoint)
	{
		CCLOG("[]CreepsManager::addCreeps, no creeps point data.");
		return;
	}

	CreepsPointInfo* pInfo = new CreepsPointInfo(pCreepsPoint, callback);
	if (nullptr == pInfo)
	{
		CCLOG("[error]CreepsManager::addCreepsPoint: allocate CreepsPointInfo failed.");
		return;
		
	}

	if (!pInfo->init())
	{
		CCLOG("[error]CreepsManager::addCreepsPoint: CreepsPointInfo init failed.");
		delete pInfo;
		pInfo = nullptr;
		return;
	}


	//sgTimeCalculator()->createTimeCalcultor(pInfo->mTimerKeyName, pInfo->mData->getInterval());
	mCreepsPointList.insert(std::make_pair(id, pInfo));

	int groupId = pInfo->mData->getGroup();
	float refreshTime = pInfo->mData->getInterval();

	GroupInfo* pGroupInfo = nullptr;
	auto groupItr = mGroupList.find(groupId);
	if (groupItr != mGroupList.end())
	{
		pGroupInfo = groupItr->second;
		if (pGroupInfo)
			pGroupInfo->mCreepsPtList.push_back(pInfo);
	}
	else
	{
		pGroupInfo = new GroupInfo();
		if (pGroupInfo)
		{
			pGroupInfo->mGroupId = groupId;
			pGroupInfo->mCreepsPtList.push_back(pInfo);
			pGroupInfo->mTimerKey = StringUtils::format("CreepsManager_Group%d_Refresh", groupId);

			sgTimeCalculator()->createTimeCalcultor(pGroupInfo->mTimerKey, refreshTime);

			mGroupList.insert(std::make_pair(groupId, pGroupInfo));
		}
		else
		{
			CCLOG("[error]CreepsManager::addCreepsPoint: allocate GroupInfo failed.");
		}
	}
}


bool CreepsManager::refreshGroup(GroupInfo* pInfo)
{
	if (nullptr == pInfo)
		return false;

	if (this->getExistCreepsCount() >= 2)
	{
		CCLOG("[trace]CreepsManager::refreshGroup : too mush creeps.");
		return false;
	}

	auto & vecCreepsPoint = pInfo->mCreepsPtList;
	float fRandomRange = 0.0f;
	std::vector<CreepsPointInfo*> validCreepsPointList;
	for (unsigned int index = 0; index < vecCreepsPoint.size(); ++index)
	{
		CreepsPointInfo * pCreepsPt = vecCreepsPoint[index];
		if (pCreepsPt 
			&& pCreepsPt->mData 
			&& 0 == pCreepsPt->mHittedCreepsID
			&& sgTimeCalculator()->getTimeLeft(pCreepsPt->mTimerKeyName) <= 0)
		{
			fRandomRange += pCreepsPt->mData->getProb();
			validCreepsPointList.push_back(pCreepsPt);
		}
	}

	if (validCreepsPointList.empty())
	{
		CCLOG("[trace]CreepsManager::refreshGroup : random list is empty.");
		return false;
	}

	float fHit = cocos2d::random(0.0f, fRandomRange);

	CreepsPointInfo *pHitCreepsPt = nullptr;
	for (unsigned int index = 0; index < validCreepsPointList.size(); ++index)
	{
		CreepsPointInfo* pCreepsPt = validCreepsPointList[index];
		if (pCreepsPt)
		{
			if (pCreepsPt->mData->getProb() < fHit)
			{
				fHit -= pCreepsPt->mData->getProb();
			}
			else
			{
				pHitCreepsPt = pCreepsPt;
				break;
			}
		}
	}

	if (nullptr == pHitCreepsPt)
	{
		CCLOG("[trace]CreepsManager::refreshGroup : creeps point no hit.");
		return false;
	}

	return this->refreshCreepsPoint(pHitCreepsPt);
}


bool CreepsManager::refreshCreepsPoint(CreepsPointInfo* pPointInfo)
{
	if (sgBattleRecordsManager()->getIsPlayback())
	{
		return false;
	}

	if (nullptr == pPointInfo)
		return false;

	if (pPointInfo->mHittedCreepsID > 0)
		return false;

	/*if (this->getExistCreepsCount() >= 2)
	{
		CCLOG("[trace]CreepsManager::refreshCreepsPoint : too mush creeps.");
		return false;
	}*/

	/*float fPointHit = cocos2d::random(0.0f, 100.0f);
	if (fPointHit > pPointInfo->mData->getProb())
	{
		CCLOG("[trace]CreepsManager::refreshCreepsPoint : point not hit.");
		return false;
	}*/

	const std::vector<const Creeps*> & vecCreeps = pPointInfo->mCreepsList;
	float fRandomRange = 0.0f;
	for (unsigned int index = 0; index < vecCreeps.size(); ++index)
	{
		const Creeps* pCreeps = vecCreeps[index];
		if (pCreeps)
			fRandomRange += pCreeps->getProb();
	}


	unsigned int uHitIndex = vecCreeps.size();
	float fCreepHit = cocos2d::random(0.0f, fRandomRange);
	for (unsigned int index = 0; index < vecCreeps.size(); ++index)
	{
		const Creeps* pCreeps = vecCreeps[index];
		if (pCreeps)
		{
			if (pCreeps->getProb() < fCreepHit)
			{
				fCreepHit -= pCreeps->getProb();
			}
			else
			{
				uHitIndex = index;
				break;
			}
		}
	}

	if (vecCreeps.size() == uHitIndex)
	{
		CCLOG("[trace]CreepsManager::refreshCreepsPoint : creeps no hit.");
		return false;
	}

	const Creeps* pHitCreeps = vecCreeps[uHitIndex];
	if (this->isCreepsExist(pHitCreeps->getID()))
	{
		CCLOG("[trace]CreepsManager::refreshCreepsPointn : creeps existed.");
		return false;
	}

	if (!pPointInfo->mCallback)
	{
		CCLOG("[warn]CreepsManager::refreshCreepsPoint : no callback.");
		return false;
	}

	this->addCreepsToExistingList(pHitCreeps->getID());
	pPointInfo->mHittedCreepsID = pHitCreeps->getID();

	if (sgBattleControl()->getIsPvp())
	{

		if (sgUserObj()->getIsHost())
		{
			int id = pPointInfo->mData->getID();
			int dataId = pHitCreeps->getID();
			sgPvpControl()->getPvPProxy()->sendAppearCreep(id, dataId);
		}

	}
	else
	{
		if (!pPointInfo->mCallback(pHitCreeps))
		{
			this->removeCreepsFromExistingList(pHitCreeps->getID());
			pPointInfo->mHittedCreepsID = 0;

			return false;
		}

		if (!sgBattleRecordsManager()->getIsPlayback())
		{
			char buff[64];
			memset(buff, 0, 64);
			sprintf(buff, "%d %d %d", pPointInfo->mData->getID(), pHitCreeps->getID(), uHitIndex);

			sgMessageManager()->sendMessage(EMT_RECORD_CREEPS_BORN, buff);
		}	
	}
	
	return true;
}


void CreepsManager::syncPvpCreepsInfo(int creepsPtId, int creepsId)
{
	auto itrCreepsPoint = mCreepsPointList.find(creepsPtId);
	if (mCreepsPointList.end() == itrCreepsPoint)
		return;

	CreepsPointInfo* pPointInfo = itrCreepsPoint->second;
	if (nullptr == pPointInfo)
	{
		CCLOG("[warn]CreepsManager::syncPvpCreepsInfo : Haven't creeps point[%d]", creepsId);
		return;
	}

	const std::vector<const Creeps*> & vecCreeps = pPointInfo->mCreepsList;
	const Creeps* pHitCreeps = nullptr;
	for (unsigned int index = 0; index < vecCreeps.size(); ++index)
	{
		const Creeps* pCreeps = vecCreeps[index];
		if (pCreeps && pCreeps->getID() == creepsId)
			pHitCreeps = pCreeps;
	}

	if (nullptr == pHitCreeps)
	{
		CCLOG("[warn]CreepsManager::syncPvpCreepsInfo : Haven't creeps[%d]", creepsId);
		return;
	}

	if (!pPointInfo->mCallback(pHitCreeps))
	{
		this->removeCreepsFromExistingList(pHitCreeps->getID());
		pPointInfo->mHittedCreepsID = 0;

		return;
	}
}


void CreepsManager::occupyCreepPoint(int id)
{
	CreepsPointInfo* pInfo = this->getCreepsPoint(id);
	if (nullptr == pInfo)
	{
		CCLOG("[error]CreepsManager::defeatCreepPoint : Creeps point doesn't exist.");
		return;
	}

	this->removeCreepsFromExistingList(pInfo->mHittedCreepsID);
	pInfo->mHittedCreepsID = 0;

	bool isHost = false;
	if (sgBattleControl()->getIsPvp())
	{
		if (sgUserObj()->getIsHost())
			isHost = true;
	}
	else
	{
		isHost = true;
	}

	if (isHost)
	{
		if (sgTimeCalculator()->hasKey(pInfo->mTimerKeyName))
			sgTimeCalculator()->resetTimeCalcultor(pInfo->mTimerKeyName);
		else
			sgTimeCalculator()->createTimeCalcultor(pInfo->mTimerKeyName, pInfo->mData->getInterval());
	}
}


void CreepsManager::resetCreepPoint(int id)
{
	CreepsPointInfo* pInfo = this->getCreepsPoint(id);
	if (nullptr == pInfo)
	{
		CCLOG("[error]CreepsManager::resetCreepPoint : Creeps point doesn't exist.");
		return;
	}

	this->removeCreepsFromExistingList(pInfo->mHittedCreepsID);
	pInfo->mHittedCreepsID = 0;

	//sgTimeCalculator()->createTimeCalcultor(pInfo->mTimerKeyName, pInfo->mData->getInterval());
}


bool CreepsManager::isCreepsExist(int creepsId) const
{
	auto itFind = mExistingCreepsList.find(creepsId);
	if (mExistingCreepsList.end() != itFind)
		return true;
	else
		return false;
}


int CreepsManager::getExistCreepsCount() const
{
	return mExistingCreepsList.size();
}


void CreepsManager::addCreepsToExistingList(int creepsId)
{
	mExistingCreepsList.insert(creepsId);
}


void CreepsManager::removeCreepsFromExistingList(int creepsId)
{
	mExistingCreepsList.erase(creepsId);
}
