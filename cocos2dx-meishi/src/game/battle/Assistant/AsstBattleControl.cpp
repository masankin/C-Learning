#include "AsstBattleControl.h"
#include "Assistant.h"
#include "AsstEvent.h"
#include "AssistantConstant.h"
#include "battle/Camp.h"
#include "battle/BattleControl.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"

USING_NS_CC;

AsstBattleControl::AsstBattleControl()
{
	CCLOG("[trace]AsstBattleControl::AsstBattleControl");

	cocos2d::EventListenerCustom* pListener = nullptr;

	auto pDispatcher = Director::getInstance()->getEventDispatcher();

	pListener = pDispatcher->addCustomEventListener(AsstEvent::ActiveSkillReq, CC_CALLBACK_1(AsstBattleControl::onRequestActiveSkill, this));
	mListenerList.push_back(pListener);
}

AsstBattleControl::~AsstBattleControl()
{
	this->clean();

	auto pDispatcher = Director::getInstance()->getEventDispatcher();
	for (auto pListener : mListenerList)
	{
		pDispatcher->removeEventListener(pListener);
	}

	CCLOG("[trace]AsstBattleControl::~AsstBattleControl");
}

void AsstBattleControl::clean()
{
	CCLOG("[trace]AsstBattleManager::clean");

	for (auto &campPair : mCampAsstList)
	{
		auto pAsst = campPair.second;
		if (pAsst)
		{
			pAsst->release();
			pAsst = nullptr;
		}
	}
	mCampAsstList.clear();
}

void AsstBattleControl::update(float dt)
{
	for (auto &campPair : mCampAsstList)
	{
		auto asst = campPair.second;
		if (asst)
			asst->update(dt);
	}
}

void AsstBattleControl::run()
{
	for (auto &campPair : mCampAsstList)
	{
		auto asst = campPair.second;
		if (asst)
			asst->run();
	}
}

bool AsstBattleControl::registAssistant(Assistant* asst)
{
	if (asst != nullptr && asst->getCampType() != ECAMP_TYPE_Invalid)
	{
		auto it = mCampAsstList.find(asst->getCampType());
		if (it == mCampAsstList.end())
		{
			CCLOG("[trace]AsstBattleManager::registAssistant : camp = %d, id = %d", asst->getCampType(), asst->getId());

			//asst->addComponent(AsstActiveSkillComponent::create());

			asst->retain();
			mCampAsstList.insert(std::make_pair(asst->getCampType(), asst));
		}
		else
		{
			CCLOG("[warning]AsstBattleManager::registAssistant : Already cached camp = %d", asst->getCampType());
		}
	}
	else
	{
		CCLOG("[error]AsstBattleManager::registAssistant : Assistant is null or camp is invalid");
		return false;
	}

	return true;
}


void AsstBattleControl::unregistAssistant(Assistant* asst)
{
	if (asst != nullptr)
	{
		auto it = mCampAsstList.find(asst->getCampType());
		if (it != mCampAsstList.end() && it->second == asst)
		{
			CCLOG("[trace]AsstBattleManager::unregistAssistant : camp = %d, id = %d", asst->getCampType(), asst->getId());

			//asst->removeComponent(ASST_COMNAME_ACTIVE_SKILL);

			asst->release();
			mCampAsstList.erase(it);
			asst = nullptr;
		}
		else
		{
			CCLOG("[warning]AsstBattleManager::unregistAssistant : can't found assistant camp = %d ", asst->getCampType());
		}
	}
	else
	{
		CCLOG("[warning]AsstBattleManager::unregistAssistant  assistant = null ");
	}
}

Assistant* AsstBattleControl::getAssistant(eCampType campType)
{
	auto it = mCampAsstList.find(campType);
	if (it != mCampAsstList.end())
		return it->second;
	return nullptr;
}

void AsstBattleControl::castActiveSkill(eCampType campType, unsigned int skillId, std::vector<int>& randomList)
{
	auto pAsst = this->getAssistant(campType);
	if (nullptr == pAsst)
	{
		CCLOG("[error]AsstBattleControl::castActiveSkill : Assistant no exist, camp = %d, skillId = %d", campType, skillId);
		return;
	}

	pAsst->castActiveSkill(skillId, randomList);
}

void AsstBattleControl::onRequestActiveSkill(EventCustom* customData)
{
	if (nullptr == customData)
	{
		CCLOG("[trace]AsstBattleControl::onRequestActiveSkill : CustomData is null");
		return;
	}

	auto pData = (AsstEvent::ActiveSkillRequestData*)customData->getUserData();
	if (nullptr == pData)
	{
		CCLOG("[trace]AsstBattleControl::onRequestActiveSkill : Userdata is null");
		return;
	}

	if (sgBattleControl()->getIsPvp())
	{
		CCLOG("[trace]AsstBattleControl::onRequestActiveSkill : Request skill[%d] by net ", pData->skillID);

		sgPvpControl()->getPvPProxy()->sendAsstSkill(pData->campType, pData->skillID, pData->randomCount);
	}
	else
	{
		CCLOG("[trace]AsstBattleControl::onRequestActiveSkill : Request skill[%d] by local ", pData->skillID);

		std::vector<int> randomList;
		for (int i = 0; i < pData->randomCount; ++i)
			randomList.push_back(cocos2d::random(1, ASST_RANDOM_LIMIT));

		this->castActiveSkill((eCampType)pData->campType, pData->skillID, randomList);
	}
}
