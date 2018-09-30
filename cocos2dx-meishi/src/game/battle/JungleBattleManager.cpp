#include "JungleBattleManager.h"
#include "BattleControl.h"
#include "Map/MapHandler.h"
#include "TimeCalculator.h"
#include <random>
#include "EnvSet.h"
#define  WAIT_TIME 2.0f
JungleBattleInfo::JungleBattleInfo()
{
	m_FormationID = 0;
	m_SrcID = 0;
	m_DestID = 0;
}

JungleBattleInfo::~JungleBattleInfo()
{
	 
}

JungleBattleManager* JungleBattleManager::m_Instance = NULL;
JungleBattleManager* JungleBattleManager::getInstance()
{
	if(m_Instance == NULL)
	{
		m_Instance = new JungleBattleManager();
		m_Instance->init();
	}
	return m_Instance;
}

void JungleBattleManager::cleanJungleBattleManager()
{
	if(m_Instance != NULL)
	{
		delete m_Instance; 
		m_Instance = NULL;
	}
}

JungleBattleManager::JungleBattleManager()
{
	m_RefreshTime = 0;
	m_MaxBuildCount = 1;
	m_JungleBuildCount = 0; 
	sgMessageManager()->regisiterMessageHandler(EMT_JUNGLE, this);
}

JungleBattleManager::~JungleBattleManager()
{
	sgMessageManager()->removeMessageHandler(this);
	JungleBattleInfoVec::iterator tIter = m_JungleBattleInfoVec.begin();
	for(; tIter != m_JungleBattleInfoVec.end(); tIter++)
	{
		JungleBattleInfo* tData = *tIter;
		if(tData != NULL)
		{
			delete tData;
			tData = NULL;
		}
	}
	m_JungleBattleInfoVec.clear();
}

bool JungleBattleManager::init()
{
	bool isRet =false;
	do 
	{
		isRet = true;
	} while (0);
	return isRet;
}

void JungleBattleManager::update(float delta)
{ 
	if(AlvinTest::alvin_closeJungle() == true)
	{
		return;
	}
	if(m_JungleBuildMap.size() < m_MaxBuildCount)
	{
		if(m_WaitRefreshTime >= m_RefreshTime)
		{
			for(int i = 0; i < m_RefreshCount && m_JungleBuildMap.size() < m_MaxBuildCount; i++)
			{
				m_WaitRefreshTime = 0.0f;
				int tTotalCount = 0;
				int tLastWeight = 0; 
				JungleDataVec tDataVec1;
				JungleDataVec::iterator tJungleDataVecIter = m_JungleDataVec.begin(); 
				for(; tJungleDataVecIter != m_JungleDataVec.end(); tJungleDataVecIter++)
				{
					JungleData* tData = *tJungleDataVecIter;
					//去除重复buildID的野怪
					//去除重复locationID的位置
					int tBuildID = tData->getBuildID();
					int tLocationID = tData->getLocationID();
					if(m_JungleBuildMap.find(tBuildID) == m_JungleBuildMap.end() 
						&& m_JungleBuildMap2.find(tLocationID) == m_JungleBuildMap2.end())
					{	  
						tTotalCount += tData->getRefreshWeight(); 
						tDataVec1.push_back(tData); 
					}
				}
				std::uniform_int_distribution<unsigned> u(0,tTotalCount);
				std::default_random_engine e(utils::gettime()); 
				unsigned int tRandomValue = u(e);

				tJungleDataVecIter = tDataVec1.begin();
				for(; tJungleDataVecIter != tDataVec1.end(); tJungleDataVecIter++)
				{
					JungleData* tData = *tJungleDataVecIter; 
					tData->setRandRangeStar(tLastWeight);
					tData->setRandRangeEnd(tData->getRandRangeStar() + tData->getRefreshWeight());
					tLastWeight = tData->getRandRangeEnd();
					if(tRandomValue >= tData->getRandRangeStar() && (tRandomValue < tData->getRandRangeStar() + tData->getRefreshWeight()))
					{
						//CCLOG("create jungleBuild ### buildID is %d, locationID is %d", tData->getBuildID(), tData->getLocationID()); 
						m_JungleBuildMap.insert(std::make_pair(tData->getBuildID(), tData));
						m_JungleBuildMap2.insert(std::make_pair(tData->getLocationID(), tData));  
						createJungleBuild(tData->getBuildID(), tData->getLocationID(), tData->getBuffID());	
					}
				} 
			} 
		}
		else
		{
			m_WaitRefreshTime += delta;
		}
	}
	
}

void JungleBattleManager::createJungleBuild(int buildID, int locationID, int buffID)
{  
	int destroyID = 0;
	if(m_ClearJungleQuence.size() > 0)
	{
		destroyID = m_ClearJungleQuence.front();
		m_ClearJungleQuence.pop();
	} 
	char szParam1[64] = { 0, };
	sprintf(szParam1, "%d %d %d %d %d %d %d %d", destroyID, 0, buffID, 0, 0, buildID, locationID, E_JUNGLE_CREATE);
	sgMessageManager()->sendMessage(EMT_JUNGLE, szParam1);
}

void JungleBattleManager::addJungleBuildBuff(int camp, int jungleBuildID)
{ 
	JungleDataVec::iterator tIter = m_JungleDataVec.begin();
	for(; tIter != m_JungleDataVec.end(); tIter++)
	{
		JungleData* tData = *tIter;
		if(tData->getBuildID() == jungleBuildID)
		{
			char tParam[32] = {0};
			sprintf(tParam, "%d %d",tData->getBuffID(),camp);
			sgMessageManager()->sendMessage(EMT_BUFF, tParam); 
			continue;
		}
	}  
}

void JungleBattleManager::returnFromJungleBuild(int srcID, int destID, int srcCamp, int destCamp)
{
	char szParam1[64] = { 0, };
	sprintf(szParam1, "%d %d %d %d %d %d %d %d", srcID, destID, srcCamp, destCamp, 0, 0, 0,E_JUNGLE_MOVE);
	sgMessageManager()->sendMessage(EMT_JUNGLE, szParam1); 
}

void JungleBattleManager::closeJungleBattleInfo(int srcID, int srcCamp, int destCamp, int buildID, int locationID)
{
	char szParam1[64] = { 0, };
	sprintf(szParam1, "%d %d %d %d %d %d %d %d", srcID, 0, srcCamp, destCamp, 0, buildID, locationID, E_JUNGLE_CLOSE);
	sgMessageManager()->sendMessage(EMT_JUNGLE, szParam1); 
}

void JungleBattleManager::clearJungleBuildRefresh(int buildID, int locationID)
{
	JungleBuildMap::iterator tIter2 = m_JungleBuildMap.find(buildID);
	if(tIter2 != m_JungleBuildMap.end())
	{ 
		m_JungleBuildMap.erase(tIter2);
	}

	JungleBuildMap::iterator tIter3 = m_JungleBuildMap2.find(locationID);
	if(tIter3 != m_JungleBuildMap2.end())
	{ 
		m_JungleBuildMap2.erase(tIter3);
	}
	m_WaitRefreshTime = 0;
	
}

void JungleBattleManager::onReceiveMassage(GameMessage * message)
{ 
	if(EMT_JUNGLE == message->getMessageType())
	{
		MessageJungle* tMessage = dynamic_cast<MessageJungle* >(message); 
		if(tMessage->getState() == E_JUNGLE_ATTACK)
		{
			addJungleBattleInfo(
				tMessage->getFormationID(), 
				tMessage->getSrcID(), 
				tMessage->getDestID());
		}
		else if(tMessage->getState() == E_JUNGLE_SUCCESS)
		{ 
			//给阵营添加buff
			addJungleBuildBuff(tMessage->getDestCamp(), tMessage->getJungleBuildID()); 
		}
		else if(tMessage->getState() == E_JUNGLE_FAILED)
		{ 
			removeJungleBattleInfo(
				tMessage->getFormationID(), 
				tMessage->getSrcID(), 
				tMessage->getDestID(),  
				tMessage->getSrcCamp(), 
				tMessage->getDestCamp(), 
				tMessage->getJungleBuildID(), 
				tMessage->getJungleLocationID());
		}
		else if(tMessage->getState() == E_JUNGLE_RETURN)
		{
			//到达打野建筑的士兵返回原来建筑
			returnFromJungleBuild(
				tMessage->getSrcID(), 
				tMessage->getDestID(), 
				tMessage->getSrcCamp(), 
				tMessage->getDestCamp());
			//移除返回阵营的攻击标识
			removeJungleBattleInfo(tMessage->getFormationID(), 
				tMessage->getSrcID(), 
				tMessage->getDestID(), 
				tMessage->getSrcCamp(), 
				tMessage->getDestCamp(), 
				tMessage->getJungleBuildID(), 
				tMessage->getJungleLocationID());  
		}
		else if(tMessage->getState() == E_JUNGLE_REFRESH)
		{
			//刷新打野建筑
		} 
		else if(tMessage->getState() == E_JUNGLE_CLEAR)
		{
			clearJungleBuildRefresh(
				tMessage->getJungleBuildID(), 
				tMessage->getJungleLocationID());
		}
	} 
}

void JungleBattleManager::addJungleBattleInfo(int fromationID, int srcID, int destID)
{
	JungleBattleInfo* tInfo = new JungleBattleInfo();
	tInfo->setFormationID(fromationID);
	tInfo->setSrcID(srcID);
	tInfo->setDestID(destID);
	m_JungleBattleInfoVec.push_back(tInfo);
	
	JungleBuildRefMap::iterator tIter = m_JungleBuildRefMap.find(destID);
	if(tIter != m_JungleBuildRefMap.end())
	{
		tIter->second = tIter->second + 1;
	}
	else
	{
		m_JungleBuildRefMap.insert(std::make_pair(destID, 1));
	}
}

void JungleBattleManager::removeJungleBattleInfo(int fromationID,int srcID, int destID, int srcCamp, int destCamp, int buildID, int locationID)
{
	JungleBattleInfoVec::iterator iter = m_JungleBattleInfoVec.begin();
	int tResult = 0;
	for(; iter != m_JungleBattleInfoVec.end(); iter++)
	{
		JungleBattleInfo* tInfo = *iter;
		//返回需要相反的目标ID
		if(tInfo->getFormationID() == fromationID && tInfo->getSrcID() == destID && tInfo->getDestID() == srcID)
		{			
			m_JungleBattleInfoVec.erase(iter);
			delete tInfo;
			tInfo = NULL; 
			JungleBuildRefMap::iterator tIter = m_JungleBuildRefMap.find(srcID);
			if(tIter != m_JungleBuildRefMap.end())
			{
				tIter->second = tIter->second - 1; 
			}
			break;
		}
	} 
	JungleBuildRefMap::iterator tIter = m_JungleBuildRefMap.find(srcID);
	if(tIter != m_JungleBuildRefMap.end())
	{
		if(tIter->second == 0)
		{
			closeJungleBattleInfo(srcID, srcCamp, destCamp, buildID, locationID);
			m_ClearJungleQuence.push(buildID);
			m_JungleBuildRefMap.erase(tIter);
		}
	}
	
}


void JungleBattleManager::clearJungleBattleData(int gateID)
{ 
	m_MaxBuildCount = 1;
	m_JungleBuildCount = 0;
	JungleBuildRefMap::iterator iter1;; 

	m_JungleBuildRefMap.clear();


	JungleBattleInfoVec::iterator iter2;
	for(iter2 = m_JungleBattleInfoVec.begin(); iter2 != m_JungleBattleInfoVec.end(); iter2++)
	{
		JungleBattleInfo* tInfo = *iter2; 
		delete tInfo;
		tInfo = NULL; 
	}
	m_JungleBattleInfoVec.clear();

	m_JungleDataVec.clear();
	m_JungleBuildMap.clear();
	m_JungleBuildMap2.clear();

	m_JungleDataVec = sgTableManager()->getJungleDataVecByID(gateID);
	if(m_JungleDataVec.size() > 0)
	{
		m_MaxBuildCount = m_JungleDataVec.at(0)->getTotalCount();
		m_RefreshCount = m_JungleDataVec.at(0)->getRefreshCount();
		m_RefreshTime = m_JungleDataVec.at(0)->getRefreshTime();
	} 
	else
	{
		m_MaxBuildCount = 0;
		m_RefreshCount = 0;
		m_RefreshTime = 0;
	}
	//测试数据
	//m_MaxBuildCount = 4;
	m_WaitRefreshTime = 0.0f; 
	 
	while(m_ClearJungleQuence.empty() == false)
	{
		m_ClearJungleQuence.pop();
	} 
	
}


