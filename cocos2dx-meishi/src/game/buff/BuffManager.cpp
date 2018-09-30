
#include "BuffManager.h"
#include "MessageManager.h"
#include "battle/BattleControl.h"
#include "EnvSet.h"
#include "TimeCalculator.h"
#include "battle/Camp.h"
#include "battle/Build.h"
#include "core/HelperFunc.h"
#include "GamePrecedure.h"
#define  BUFFKEY "Buff_%d_%d"  
#define  BUFFTIMERKEY "Buff_%d_%d_%d"
BuffNode::BuffNode()
{
	resetValue();
}

void BuffNode::resetValue()
{
	attackSpeed = 0.0f;
	moveSpeed = 0.0f;
	attack = 0.0f;
	defence = 0.0f;
	productSpeed = 0.0f;
	bool  peopleFull = false ;
	attackSpeedRate = 0.0f;
	moveSpeedRate = 0.0f;
	attackRate = 0.0f;
	defenceRate = 0.0f;
	productSpeedRate = 0.0f;
}


int BuffManager::mBuffKey = 0;
BuffManager::BuffManager()
{
	sgMessageManager()->regisiterMessageHandler(EMT_BUFF, this); 
} 

void BuffManager::clearBuff()
{
	CampBuffListMap::iterator iter = m_CampBuffListMap.begin();
	for(;iter != m_CampBuffListMap.end(); iter++)
	{
		BuffIDMap* idMap = iter->second;
		idMap->clear(); 
	}
	CampBuffValueMap::iterator valueIter = m_CampBuffValueMap.begin();
	for(; valueIter != m_CampBuffValueMap.end(); valueIter++)
	{
		initCampBuffValue(valueIter->second);
	}
	
}

int BuffManager::createBuffKey()
{
	return mBuffKey++;
}

void BuffManager::update(float delta)
{ 
	//if(AlvinTest::alvin_closeBuff() == true){return;}
	//遍历所有阵营的buffList,移除失效的buff
	CampBuffListMap::iterator iter = m_CampBuffListMap.begin();
	for(; iter != m_CampBuffListMap.end(); iter++)
	{
		BuffIDMap* idMap = iter->second;
		BuffIDMap::iterator idIter = idMap->begin();
		for(; idIter != idMap->end(); idIter++)
		{
			int keyID = idIter->first;
			int buffID = idIter->second; 
			if(checkBuffActive(iter->first, keyID, buffID) == false)
			{ 
				resetBuff(iter->first, keyID, buffID);
			} 
		} 
	}
}

const BuffIDMap* BuffManager::getBuffListByCamp(eCampType camp)
{
	  CampBuffListMap::iterator iter = m_CampBuffListMap.find(camp);
	  if(iter != m_CampBuffListMap.end())
	  {
		  return iter->second;
	  }
	  return NULL;
}

void BuffManager::onReceiveMassage(GameMessage * message)
{
	if(message->getMessageType() == EMT_BUFF)
	{
		MessageBuff* tMessage = dynamic_cast<MessageBuff* >(message);
		int tBuffID = tMessage->getID(); 
		eCampType type = (eCampType)tMessage->getCampType();
		
		int buffSrc = tMessage->getBuffSrc(); 
		if(buffSrc == -1)
		{
			removeCurBuff(type); 
		}
		addBuff(type, tBuffID);
		showBuffEffect(type, tBuffID, buffSrc); 
	}
} 
 

void BuffManager::addBuff(eCampType camp, int buffID)
{   
	recordBuff(camp, buffID);
	resetCampBuff(camp);
	addCampBuff(camp, buffID, 1); 
} 

void BuffManager::recordBuff(eCampType camp, int buffID)
{ 
	int newKey = createBuffKey();
	const BuffData* pNewData = sgTableManager()->getBuffDataByID(buffID);
	if(NULL == pNewData)
	{
		CCLOG("[error]BuffManager::recordBuff pNewData is NULL");
	}
	CampBuffListMap::iterator iter = m_CampBuffListMap.find(camp);
	if(iter != m_CampBuffListMap.end())
	{
		 BuffIDMap* idMap = iter->second;
		 BuffIDMap::iterator idIter = idMap->begin();
		 for(; idIter != idMap->end(); idIter++)
		 {
			 const BuffData* pData = sgTableManager()->getBuffDataByID(idIter->second);
			 if(NULL == pData)
				 continue;
			 eMutexType mutexType = (eMutexType)pNewData->m_mutex;
			 switch (mutexType)
			 {
			 case E_NONE_MUTEX: 
				break;
			 case E_TYPE_MUTEX:  
				 if(pData->m_BuffType == pNewData->m_BuffType)
				 {
					 idIter->second = 0;
					 removeBuffTimer(camp, idIter->first, idIter->second);
				 }
				 break;
			 case E_ID_MUTEX:  
				 if(pData->m_ID == pNewData->m_ID)
				 {
					 idIter->second = 0;
					 removeBuffTimer(camp, idIter->first, idIter->second);
				 }
				 break;
			 case E_ALL_MUTEX:
				idIter->second = 0;
				removeBuffTimer(camp, idIter->first, idIter->second);
				 break;
			 default:
				 break;
			 } 
		 }
		 idMap->insert(std::make_pair(newKey, buffID));
	}
	else
	{
		 BuffIDMap* idMap = new BuffIDMap();
		 idMap->insert(std::make_pair(newKey, buffID));
		 m_CampBuffListMap.insert(std::make_pair(camp, idMap));
	} 
	const BuffData* pData = sgTableManager()->getBuffDataByID(buffID);
	createBuffTimer(camp, newKey, buffID, pData->m_CDTime);
}

void BuffManager::removeBuff(eCampType camp, int buffID)
{
	CampBuffValueMap::iterator iter = m_CampBuffValueMap.find(camp);
	if(iter != m_CampBuffValueMap.end())
	{
		updateCampBuffValue(camp, iter->second, buffID, -1); 
	}
	
}

void BuffManager::resetCampBuff(eCampType camp)
{
	
	BuffNode* pNode = NULL;
	CampBuffValueMap::iterator valueIter = m_CampBuffValueMap.find(camp);
	if(valueIter == m_CampBuffValueMap.end())
	{
		pNode = new BuffNode(); 
		m_CampBuffValueMap.insert(std::make_pair(camp, pNode)); 
	}  
	else
	{
		pNode = valueIter->second;
		
	} 
	initCampBuffValue(pNode);
}


void BuffManager::addCampBuff(eCampType camp, int buffID, int addType)
{
	 CampBuffValueMap::iterator valueIter = m_CampBuffValueMap.find(camp);
	 if(valueIter != m_CampBuffValueMap.end())
	 {
		 CampBuffListMap::iterator iter = m_CampBuffListMap.find(camp);
		 BuffIDMap* pBuffIDMap = iter->second;
		 BuffIDMap::iterator idIter = pBuffIDMap->begin();
		 for (; idIter !=  pBuffIDMap->end(); idIter ++)
		 { 
// 			 if(camp == ECAMP_TYPE_AI)
// 			 {
// 				 CCLOG("[trace]1.BuffManager::getBuffMoveSpeed buffID is %d and moveRate is %f defRate is %f", buffID, valueIter->second->moveSpeedRate, valueIter->second->defenceRate);
// 			 }
			 updateCampBuffValue(camp,valueIter->second, idIter->second, addType); 
// 			 if(camp == ECAMP_TYPE_AI)
// 			 {
// 				 CCLOG("[trace]2.BuffManager::getBuffMoveSpeed buffID is %d and moveRate is %f defRate is %f", buffID, valueIter->second->moveSpeedRate, valueIter->second->defenceRate);
// 			 }
		 }  
	 }  	 
}

void BuffManager::initCampBuffValue(BuffNode* buffNode)
{
	buffNode->resetValue();
}


void BuffManager::updateCampBuffValue(eCampType camp, BuffNode* buffNode, int buffID, int addType)
{
	if(buffID == 0)
		return;
	const BuffData* pData = sgTableManager()->getBuffDataByID(buffID); 
	float addRate = pData->m_AddRate;
	if(addType < 0)
	{
		addRate = 0;
	}
	switch (pData->m_BuffType)
	{
	case E_Buff_MoveSpeed:
		buffNode->moveSpeed = getAddBuffValue(buffNode->moveSpeed, pData->m_AddValue, addType);
		buffNode->moveSpeedRate = getResetBuffValue(addRate);
		break;
	case E_Buff_AttackSpeed:
		buffNode->attackSpeed = getAddBuffValue(buffNode->attackSpeed, pData->m_AddValue, addType);
		buffNode->attackSpeedRate = getResetBuffValue(addRate);
		break;
	case E_Buff_Defence:
		buffNode->defence = getAddBuffValue(buffNode->defence, pData->m_AddValue, addType);
		buffNode->defenceRate = getResetBuffValue(addRate);
		break;
	case E_Buff_FillPeople:
		if(addType > 0)
		{
			buffNode->peopleFull = getResetBuffValue(1);
			setCampPeopleToFull(camp, buffID); 
		} 
		break;
	case E_Buff_ProduceSpeed:
		buffNode->productSpeed = getAddBuffValue(buffNode->productSpeed, pData->m_AddValue, addType);
		buffNode->productSpeedRate = getResetBuffValue(addRate);
		break;
	case E_Buff_Attack:
		buffNode->attack = getAddBuffValue(buffNode->attack, pData->m_AddValue, addType);
		buffNode->attackRate = getResetBuffValue(addRate);
		break;
	default:
		break;
	}
}

float BuffManager::getAddBuffValue(float srcValue, float addValue, int addType)
{
	float value = srcValue +  addValue * addType;;
	if(value < 0.0001f)
	{
		value = 0.0f;
	}
	return value;
}

float BuffManager::getResetBuffValue(float setValue)
{
	return setValue;
}


void BuffManager::createBuffTimer(eCampType camp, int buffID, float time)
{
	std::string key = CCString::createWithFormat(BUFFKEY, camp, buffID)->getCString(); 
	if(sgTimeCalculator()->hasKey(key) == true)
	{
		sgTimeCalculator()->removeTimeCalcultor(key); 
	}
	sgTimeCalculator()->createTimeCalcultor(key,time); 
}
 
void BuffManager::createBuffTimer(eCampType camp, int keyID, int buffID, float time)
{  
	std::string key = CCString::createWithFormat(BUFFTIMERKEY,camp, keyID, buffID)->getCString(); 
	if(sgTimeCalculator()->hasKey(key) == true)
	{
		sgTimeCalculator()->removeTimeCalcultor(key); 
	}
	sgTimeCalculator()->createTimeCalcultor(key,time); 
}

void BuffManager::removeBuffTimer(eCampType camp, int keyID, int buffID)
{
	std::string key = CCString::createWithFormat(BUFFTIMERKEY,camp, keyID, buffID)->getCString(); 
	if(sgTimeCalculator()->hasKey(key) == true)
	{
		sgTimeCalculator()->removeTimeCalcultor(key); 
	} 
}

bool BuffManager::checkBuffActive(eCampType camp, int keyID, int buffID)
{
	std::string key = CCString::createWithFormat(BUFFTIMERKEY, keyID, camp, buffID)->getCString(); 
	if(sgTimeCalculator()->hasKey(key) == false)
		return true;
	float time = sgTimeCalculator()->getTimeLeft(key);
	if(time <= 0.0f)
	{
		return false;
	}
	return true;
}

void BuffManager::showBuffEffect(eCampType camp, int buffID, int buffSrc)
{ 
	if(sgGamePrecedure()->getCurMapInfo().checkpt < 1120 || buffID/1000 > 0)
		return;
	const BuffData* pData = sgTableManager()->getBuffDataByID(buffID);
	if(pData->m_BuffType == E_Buff_FillPeople)
		return;
	//发送相关消息用于显示
	char tParam[128] = {0};
	sprintf(tParam, "%d %d %f %d",  camp, pData->m_BuffType, pData->m_CDTime, buffSrc);
	sgMessageManager()->sendMessage(EMT_BUFFSHOW, tParam); 
}


void BuffManager::resetBuff(eCampType camp, int keyID, int buffID)
{ 
	removeBuffTimer(camp, keyID, buffID);
	CampBuffListMap::iterator iter = m_CampBuffListMap.find(camp);
	if(iter != m_CampBuffListMap.end())
	{
		BuffIDMap::iterator iditer = iter->second->find(keyID);
		iditer->second = 0;
		addBuff(camp, buffID);
	}
}

bool BuffManager::checkBuffIDExist(eCampType camp, int buffID)
{
	CampBuffListMap::iterator iter = m_CampBuffListMap.find(camp);
	if(iter != m_CampBuffListMap.end())
	{
		BuffIDMap::iterator iditer = iter->second->begin();
		for(; iditer != iter->second->end(); iditer++)
		{
			if(iditer->second == buffID)
			{
				return true;
			}
		} 
	}
	return false;
}


void BuffManager::removeCurBuff(eCampType camp)
{
	CampBuffListMap::iterator iter = m_CampBuffListMap.find(camp);
	if(iter != m_CampBuffListMap.end())
	{
		BuffIDMap::iterator iditer = iter->second->begin();
		for(; iditer != iter->second->end(); iditer++)
		{
			int id = iditer->second;
			if(id/1000 > 0)
			{
				iditer->second = 0; 
				removeBuffTimer(camp, iditer->first, iditer->second);
			} 
		}
	}
	//removeCurBuffView(camp);
}

void BuffManager::removeCurBuffView(eCampType type)
{
	sgBattleControl()->clearCampBuffShow(type);
}
 float BuffManager::getBuffAttackSpeed(eCampType camp, float baseValue)
 {
	 CampBuffValueMap::iterator iter = m_CampBuffValueMap.find(camp);
	 if(iter != m_CampBuffValueMap.end())
	 {
		  float finalValue = (baseValue + iter->second->attackSpeed) * (1.0f + iter->second->attackSpeedRate);
		  return finalValue;
	 } 
	 return baseValue;
 }

 float BuffManager::getBuffMoveSpeed(eCampType camp, float baseValue)
 {
	 CampBuffValueMap::iterator iter = m_CampBuffValueMap.find(camp);
	 if(iter != m_CampBuffValueMap.end())
	 {
		 float finalValue = (baseValue + iter->second->moveSpeed) * (1.0f + iter->second->moveSpeedRate);
		
		 return finalValue;
	 } 
	 return baseValue;
 }

 float BuffManager::getBuffAttack(eCampType camp, float baseValue)
 {
	 CampBuffValueMap::iterator iter = m_CampBuffValueMap.find(camp);
	 if(iter != m_CampBuffValueMap.end())
	 {
		 float finalValue = (baseValue + iter->second->attack) * (1.0f + iter->second->attackRate);
		 return finalValue;
	 } 
	 return baseValue;
 }

 float BuffManager::getBuffProduceSpeed(eCampType camp, float baseValue)
 {
	 CampBuffValueMap::iterator iter = m_CampBuffValueMap.find(camp);
	 if(iter != m_CampBuffValueMap.end())
	 {
		 float finalValue = (baseValue + iter->second->productSpeed) * (1.0f + iter->second->productSpeedRate);
		 return finalValue;
	 } 
	 return baseValue;
 }


 float BuffManager::getBuffDefence(eCampType camp, float baseValue)
 {
	 CampBuffValueMap::iterator iter = m_CampBuffValueMap.find(camp);
	 if(iter != m_CampBuffValueMap.end())
	 {
		 float finalValue = (baseValue + iter->second->defence) * (1.0f + iter->second->defenceRate);
		 return finalValue;
	 } 
	 return baseValue;
 }

 void BuffManager::setCampPeopleToFull(eCampType camp, int buffID)
 {
	 const BuffData* pBuffData = sgTableManager()->getBuffDataByID(buffID);
	 if (!pBuffData)
		 return;

	 std::vector<int> buildIDVec;
	 //
	 Camp* pCamp = sgBattleControl()->getTargetCamp(camp); 
	 const MapBulidList buildList =  pCamp->getBuildList();
	 MapBulidList::const_iterator iter = buildList.begin();
	 for(; iter != buildList.end(); iter++)
	 {
		 buildIDVec.push_back(iter->first);
	 }

	 int randTimes = pBuffData->m_targetNum; 
	 for (int i = 0; i < randTimes && buildIDVec.size() >= randTimes; i++)
	 {
		 int _rand = HelperFunc::getRandomValule(0, buildIDVec.size() - 1);
		 int buildID = buildIDVec.at(_rand);
		 BaseBuild* pBuild = pCamp->getBuild(buildID);
		 if(NULL != pBuild)
		 {
			 pBuild->doFullPeopel(); 
		 }
		 std::vector<int>::iterator it = buildIDVec.begin();
		 buildIDVec.erase(it + _rand);
	 } 
 }

 bool BuffManager::getExistBuff(eCampType camp, int buffID)
 {
	CampBuffListMap::iterator iter = m_CampBuffListMap.find(camp);
	if(iter != m_CampBuffListMap.end())
	{
		BuffIDMap* idMap = iter->second;
		BuffIDMap::iterator idIter = idMap->find(buffID);
		if(idIter != idMap->end() && idIter->second > 0)
		{
			return true;
		}
		return false;
	}
	return false;
 }
 
