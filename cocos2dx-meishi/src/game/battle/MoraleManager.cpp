#include "MoraleManager.h"
#include "MessageManager.h"
#include "core/TimeCalculator.h"
#include "GamePrecedure.h" 
#include "BattleControl.h"
#include "Glyph/GlyphData.h"
#include "Glyph/GlyphBattleManager.h"
#include "guide/GuideManager.h"
#define REDUCE_VALUE -1.0f
#define REDUCE_WAITTIME 1.0f 
#define MORALE_KEY "Morale_%d"

MoraleManager::MoraleManager()
{
	m_MoraleValueMap.clear();
	sgMessageManager()->regisiterMessageHandler(EMT_MORALE, this); 
}

MoraleManager::~MoraleManager()
{
	sgMessageManager()->removeMessageHandler(this);
} 

void MoraleManager::cleanMoraleManager()
{
	MoraleValueMap::iterator iter = m_MoraleValueMap.begin();
	for(; iter != m_MoraleValueMap.end(); iter++)
	{
		MoraleRefreshInfo* pInfo = iter->second;
		if(pInfo)
		{
			delete pInfo;
			pInfo = NULL;
		}
	}
	m_MoraleValueMap.clear();
}

void MoraleManager::clearMorale()
{
	MoraleValueMap::iterator iter = m_MoraleValueMap.begin();
	for(; iter != m_MoraleValueMap.end(); iter++)
	{
		MoraleRefreshInfo* pInfo = iter->second;
		if(pInfo)
		{
			pInfo->m_MoraleLevel = 0;
			pInfo->m_MoraleValue = 0;
			pInfo->m_ResetFlag = true;
			pInfo->m_RefreshTime = REDUCE_WAITTIME;
		}
	}
	m_MoraleValueMap.clear(); 
} 

void MoraleManager::update(float delta)
{ 
	updateCampMorale();
}
 
float MoraleManager::getMoraleValueByCamp(eCampType camp)
{	
	CCLOG("map size  %d  ", m_MoraleValueMap.size());
	MoraleValueMap::iterator iter = m_MoraleValueMap.find(camp);

	if (iter != m_MoraleValueMap.end())
	{
		return iter->second->m_MoraleValue;
	}
	else
	{
		return -1;
	} 
	
}

void MoraleManager::onReceiveMassage(GameMessage * message)
{
	if(message->getMessageType() == EMT_MORALE)
	{ 
		MessageMorale* tMessage = dynamic_cast<MessageMorale* >(message);
		if(tMessage != NULL)
		{
			eCampType camp = (eCampType)tMessage->getCampType(); 
			if(camp == ECAMP_TYPE_Creeps || camp == ECAMP_TYPE_Neutral || camp ==  ECAMP_TYPE_Invalid)
			{
				return;
			}
			
			int computeType = tMessage->getComputeType();
			float value = tMessage->getMoraleValue();
			addMorale(camp, value, computeType);  
		}
	} 
}

void MoraleManager::registMoraleMessage(eCampType camp, float value, int computeType, int srcID)
{ 	
	if(camp == ECAMP_TYPE_Creeps || camp == ECAMP_TYPE_Neutral || camp ==  ECAMP_TYPE_Invalid)
	{
		return;
	}  
	mSrcID = srcID;
	addMorale(camp, value, computeType);  
	
}

void MoraleManager::unRegistMoraleMessage()
{

}
 

void MoraleManager::addMorale(eCampType camp, float value, int type)
{
	if(addMoraleValue(camp, value, type) == false)
	{
		createMoraleValue(camp, value, type);
	}  
	if(value >= 10)
	{
		float curValue = getMoraleValueByCamp(camp);
		onShowMoraleEffect(camp, curValue);
	}
} 

void MoraleManager::createMoraleValue(eCampType camp, float value, int type)
{
	float addValue = 0;
	if(E_MORALECOMPUTE_NORMAL == type)
	{ 
		addValue = value;
	}
	else if(E_MORALECOMPUTE_DEFENCE == type)
	{
		const MoraleDefenceData* pData = sgTableManager()->getMoraleDefenceDataByRange(0);
		float tAddValue = 0.0f;
		if(NULL != pData)
		{
			addValue = pData->getAddDefenceMoral() * value;
		}
	}
	MoraleRefreshInfo* pInfo = new MoraleRefreshInfo();
	pInfo->m_MoraleLevel = 0;
	pInfo->m_ResetFlag = true;
	pInfo->m_MoraleValue = addValue;
	pInfo->m_RefreshTime = REDUCE_WAITTIME;
	m_MoraleValueMap.insert(std::make_pair(camp, pInfo));
	createMoraleTimer(camp, pInfo->m_RefreshTime);
	onShowMorale(camp, pInfo->m_MoraleValue);
}

bool MoraleManager::addMoraleValue(eCampType camp, float value, int type)
{ 
	if (value > 0 && type != E_MORALECOMPUTE_GLYPH)//符文加属性
	{
		auto pGlphy = sgBattleControl()->getGlyphBattleMgr()->getCampGlyph(camp, Glyph::E_GLYPH_MRL_ADD);
		if (pGlphy)
		{
			value += pGlphy->getValueAsPercent() * value;
		}
	}

	float addValue = 0;
	if (E_MORALECOMPUTE_NORMAL == type || E_MORALECOMPUTE_GLYPH == type)
	{
		addValue = value;
	}
	else if(E_MORALECOMPUTE_DEFENCE == type)
	{
		MoraleValueMap::iterator iter = m_MoraleValueMap.find(camp);
		if(iter == m_MoraleValueMap.end()) 
			return false;
		const MoraleDefenceData* pData = sgTableManager()->getMoraleDefenceDataByRange(iter->second->m_MoraleValue);
		float tAddValue = 0.0f;
		if(NULL != pData)
		{
			addValue = pData->getAddDefenceMoral() * value;
		}
	} 
	MoraleValueMap::iterator iter = m_MoraleValueMap.find(camp);
	if(iter != m_MoraleValueMap.end())
	{
		MoraleRefreshInfo* pInfo = iter->second;
		pInfo->m_ResetFlag = true;
		pInfo->m_MoraleValue += addValue;
		if(pInfo->m_MoraleValue <= 0.0f)
		{
			pInfo->m_MoraleValue = 0.0f;
			pInfo->m_ResetFlag = false;
		}
		if(pInfo->m_MoraleValue >= 100.0f)
		{
			pInfo->m_MoraleValue = 100.0f;
			//pInfo->m_ResetFlag = false;
		} 
		int level = sgTableManager()->getMoraleLevel(pInfo->m_MoraleValue);
		if(level == 0)
		{
			removeMoraleBuff(camp);
		}
		else
		{
			if(level != pInfo->m_MoraleLevel)
			{
				removeMoraleBuff(camp);
				addMoraleBuff(camp, pInfo->m_MoraleValue);
			} 
		}
		pInfo->m_MoraleLevel = level;
		pInfo->m_RefreshTime = REDUCE_WAITTIME;
		if(pInfo->m_ResetFlag == true)
		{
			createMoraleTimer(camp, pInfo->m_RefreshTime); 
		}  
		else
		{
			std::string key = CCString::createWithFormat(MORALE_KEY, camp)->getCString();
			sgTimeCalculator()->removeTimeCalcultor(key); 
		} 
		onShowMorale(camp, pInfo->m_MoraleValue);
		return true;
	}	
	return false;
}

void MoraleManager::addMoraleBuff(eCampType camp, float value)
{ 
	//当buff叠加等级变化的时候才发送buff消息
	MoraleBuffDataVec buffDataVec = sgTableManager()->getMoraleBuffDataVecByRange(value);
	MoraleBuffDataVec::iterator iter = buffDataVec.begin();
	//循环增益buff加成项
	for(; iter != buffDataVec.end(); iter++)
	{ 
		int tBuffID = (*iter)->getBuffID(); 
		char tParam[32] = {0};
		sprintf(tParam, "%d %d %d", tBuffID, camp, 0);
		sgMessageManager()->sendMessage(EMT_BUFF, tParam); 
	}  
}

void MoraleManager::removeMoraleBuff(eCampType camp)
{
	char tParam[32] = {0};
	sprintf(tParam, "%d %d %d", 1020, camp, -1);
	sgMessageManager()->sendMessage(EMT_BUFF, tParam); 
}

void MoraleManager::onShowMorale(eCampType camp, float value)
{
	char tShowParam[32] = { 0, };
	sprintf(tShowParam, "%d %f %d", camp, value, 0);
	sgMessageManager()->sendMessage(EMT_MORALESHOW, tShowParam);   
 
}

void MoraleManager::onShowMoraleEffect(eCampType camp, float value)
{
	char tShowParam[32] = { 0, };
	sprintf(tShowParam, "%d %f %d", camp, value, mSrcID);
	sgMessageManager()->sendMessage(EMT_MORALESHOW, tShowParam);   
}

void MoraleManager::createMoraleTimer(eCampType camp, float time)
{
	std::string key = CCString::createWithFormat(MORALE_KEY, camp)->getCString();
	if(sgTimeCalculator()->hasKey(key) == true)
	{
		sgTimeCalculator()->removeTimeCalcultor(key);
	}
	sgTimeCalculator()->createTimeCalcultor(key, time);
}

void MoraleManager::updateCampMorale()
{
	MoraleValueMap::iterator iter = m_MoraleValueMap.begin();
	for(; iter != m_MoraleValueMap.end(); iter++)
	{
		updateMorale(iter->first);
	}
}

void MoraleManager::updateMorale(eCampType camp)
{
	std::string key = CCString::createWithFormat(MORALE_KEY, camp)->getCString();
	if(sgTimeCalculator()->hasKey(key) == false)
		return;
	float time = sgTimeCalculator()->getTimeLeft(key);
	if(time <= 0.0f)
	{
		addMoraleValue(camp, REDUCE_VALUE, E_MORALECOMPUTE_NORMAL);

		auto pGlyph = sgBattleControl()->getGlyphBattleMgr()->getCampGlyph(camp, Glyph::E_GLYPH_MRL_TIME);
		if (pGlyph)
		{
			addMoraleValue(camp, pGlyph->getValue(), E_MORALECOMPUTE_GLYPH);
		}
	}
}

 


