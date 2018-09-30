#include "AsstActiveSkillComponent.h"
#include "Assistant.h"
#include "AsstActiveSkill.h"
#include "AssistantConstant.h"
#include "LazyTimer.h"
#include "AsstEvent.h"

const float AsstActiveSkillComponent::DelayInterval = 0.7f;
const float AsstActiveSkillComponent::DelayCastProb = 20.0f;

AsstActiveSkillComponent* AsstActiveSkillComponent::create()
{
	auto pRet = new AsstActiveSkillComponent();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

AsstActiveSkillComponent::AsstActiveSkillComponent() : AsstComponent()
, mTimer(nullptr)
{
	mName = ASST_COMNAME_ACTIVE_SKILL;
}

AsstActiveSkillComponent::~AsstActiveSkillComponent()
{
	this->clean();

	if (mTimer)
	{
		mTimer->release();
		mTimer = nullptr;
	}
}

bool AsstActiveSkillComponent::init()
{
	if (!AsstComponent::init())
		return false;

	mTimer = LazyTimer::create();
	if (nullptr == mTimer)
	{
		CCLOG("[error]AsstActiveSkillCompoent::init : Timer init failed");
		return false;
	}

	mTimer->retain();

	return true;
}

void AsstActiveSkillComponent::clean()
{
	CCLOG("[trace]AsstActiveSkillCompoent::clean");

	for (auto &skillPair : mSkillContainer)
	{
		auto pSkill = skillPair.second;
		if (pSkill)
		{
			this->stopSkillAllTimer(pSkill);
			pSkill->setCaster(nullptr);

			pSkill->release();
			pSkill = nullptr;
		}
	}
	mSkillContainer.clear();
}

void AsstActiveSkillComponent::update(float dt)
{
	mTimer->update(dt);
}

void AsstActiveSkillComponent::setOwner(Assistant *pOwner)
{
	CCLOG("[trace]AsstActiveSkillCompoent::setOwner : Owner = %p", pOwner);

	for (auto &skillPair : mSkillContainer)
	{
		auto pSkill = skillPair.second;
		if (pSkill)
		{
			this->stopSkillAllTimer(pSkill);
			pSkill->setCaster(pOwner);
		}
	}

	mOnSkillEventCallback = nullptr;

	AsstComponent::setOwner(pOwner);
}

AsstActiveSkill* AsstActiveSkillComponent::getSkill(unsigned int skillId) const
{
	AsstActiveSkill* pSkill = nullptr;
	auto findItr = mSkillContainer.find(skillId);
	if (findItr != mSkillContainer.end())
	{
		pSkill = findItr->second;
	}
	return pSkill;
}

AsstActiveSkill* AsstActiveSkillComponent::createSkill(AsstActiveSkillData* pData)
{
	if (nullptr == mOwner)
	{
		CCLOG("[error]AsstActiveSkillCompoent::createSkill : No owner");
		return nullptr;
	}

	if (nullptr == pData)
	{
		CCLOG("[error]AsstActiveSkillCompoent::addSkill : Skill's basic data is null.");
		return nullptr;
	}

	AsstActiveSkill* pSkill = nullptr;
	switch (pData->mType)
	{
	case AsstActiveSkill::Type::kNone:			break;
	case AsstActiveSkill::Type::kTroop:		pSkill = new AASTroop();		break;
	case AsstActiveSkill::Type::kTurret:	pSkill = new AASTurret();		break;
	case AsstActiveSkill::Type::kMoveSpeed:		break;
	case AsstActiveSkill::Type::kTroopView:	pSkill = new AASTroopView();	break;
	case AsstActiveSkill::Type::kAtk:		pSkill = new AASAtk();			break;
	case AsstActiveSkill::Type::kDef:			break;
	}

	if (pSkill && pSkill->init(pData))
	{
		pSkill->setCaster(mOwner);
		pSkill->autorelease();
	}
	else
	{
		delete pSkill;
		pSkill = nullptr;
	}

	return pSkill;
}

bool AsstActiveSkillComponent::addSkill(AsstActiveSkillData* pSkillBase)
{
	if (nullptr == mOwner)
	{
		CCLOG("[error]AsstActiveSkillCompoent::addSkill : No owner");
		return false;
	}

	if (nullptr == pSkillBase)
	{
		CCLOG("[error]AsstActiveSkillCompoent::addSkill : Skill's basic data is null.");
		return false;
	}

	if (mSkillContainer.end() != mSkillContainer.find(pSkillBase->mId))
	{
		CCLOG("[error]AsstActiveSkillCompoent::addSkill : Skill has cached, skill id is %d.", pSkillBase->mId);
		return false;
	}

	auto pSkill = this->createSkill(pSkillBase);
	if (nullptr == pSkill)
	{
		CCLOG("[error]AsstActiveSkillCompoent::addSkill : Skill create failed, owner = %d, skillid = %d.", mOwner->getId(), pSkillBase->mId);
		return false;
	}

	pSkill->setCaster(mOwner);

	pSkill->retain();
	mSkillContainer.insert(std::make_pair(pSkill->getId(), pSkill));
	
	return true;
}

void AsstActiveSkillComponent::sendSkillEvent(SkillEvent eventType, unsigned int skillId)
{
	if (mOnSkillEventCallback)
		mOnSkillEventCallback(eventType, skillId);
}

void AsstActiveSkillComponent::requestSkill(unsigned int skillId)
{
	this->requestSkill(this->getSkill(skillId));
}

void AsstActiveSkillComponent::requestSkill(AsstActiveSkill* pSkill)
{
	if (nullptr == pSkill)
	{
		CCLOG("[error]AsstActiveSkillCompoent::requestSkill : Skill is null ");
		return;
	}

// 	if (sgBattleControl()->getIsPvp())
// 	{
// 		CCLOG("[trace]AsstActiveSkillCompoent::requestSkill : Request skill[%d] by net ", pSkill->getId());
// 
// 		sgPvpControl()->getPvPProxy()->sendAsstSkill(mOwner->getCampType(), pSkill->getId(), pSkill->getRandomCount());
// 	}
// 	else
// 	{
// 		CCLOG("[trace]AsstActiveSkillCompoent::requestSkill : Request skill[%d] by local ", pSkill->getId());
// 
// 		std::vector<int> randomList;
// 		for (int i = 0; i < pSkill->getRandomCount(); ++i)
// 			randomList.push_back(cocos2d::random(1, 10000));
// 		pSkill->setRandoms(randomList);
// 		this->castSkill(pSkill);
// 	}

	AsstEvent::ActiveSkillRequestData eventData;
	eventData.campType = mOwner->getCampType();
	eventData.skillID = pSkill->getId();
	eventData.randomCount = pSkill->getRandomCount();

	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(AsstEvent::ActiveSkillReq, &eventData);
}

bool AsstActiveSkillComponent::checkSkill(unsigned skillId) const
{
	auto pSkill = this->getSkill(skillId);
	if (nullptr == pSkill)
	{
		CCLOG("[error]AsstActiveSkillCompoent::castSkill : Skill is null ");
		return false;
	}

	return pSkill->check();
}

void AsstActiveSkillComponent::coolSkill(unsigned skillId)
{
	this->coolSkill(this->getSkill(skillId));
}

void AsstActiveSkillComponent::coolSkill(AsstActiveSkill* pSkill)
{
	if (nullptr == pSkill)
	{
		CCLOG("[error]AsstActiveSkillCompoent::coolSkill : Skill is null ");
		return;
	}

	auto pBase = pSkill->getBaseData();
	if (nullptr == pBase)
	{
		CCLOG("[error]AsstActiveSkillCompoent::coolSkill : Skill's base is null ");
		return;
	}

	mTimer->startTimer(pSkill->getCoolTimerKey(), pBase->mCoolTime, std::bind(&AsstActiveSkillComponent::onCoolTimeout, this, std::placeholders::_1, pSkill->getId()));
	this->sendSkillEvent(SkillEvent::kCDBeg, pSkill->getId());
}

void AsstActiveSkillComponent::castSkill(unsigned int skillId, std::vector<int>& randomList)
{
	this->castSkill(this->getSkill(skillId), randomList);
}

void AsstActiveSkillComponent::castSkill(AsstActiveSkill* pSkill, std::vector<int>& randomList)
{
	if (nullptr == pSkill)
	{
		CCLOG("[error]AsstActiveSkillCompoent::castSkill : Skill is null ");
		return;
	}

	auto pBase = pSkill->getBaseData();
	if (nullptr == pBase)
	{
		CCLOG("[error]AsstActiveSkillCompoent::castSkill : Skill's base is null ");
		return;
	}

	pSkill->setRandoms(randomList);

	pSkill->castBeg();

	this->sendSkillEvent(SkillEvent::kCastBeg, pSkill->getId());

	if (pBase->mContinualTime > 0)
	{
		mTimer->startTimer(pSkill->getContinualTimerKey(), pBase->mContinualTime, std::bind(&AsstActiveSkillComponent::onContinualTimeout, this, std::placeholders::_1, pSkill->getId()));
		if (pBase->mIntervalTime > 0)
			mTimer->startTimer(pSkill->getIntervalTimerKey(), pBase->mIntervalTime, std::bind(&AsstActiveSkillComponent::onIntervalTimeout, this, std::placeholders::_1, pSkill->getId()));
	}
	else
	{
		pSkill->castEnd();
		
		this->sendSkillEvent(SkillEvent::kCastEnd, pSkill->getId());

		this->coolSkill(pSkill);
	}
}

void AsstActiveSkillComponent::stopSkillAllTimer(unsigned int skillId)
{
	this->stopSkillAllTimer(this->getSkill(skillId));	
}

void AsstActiveSkillComponent::stopSkillAllTimer(AsstActiveSkill* pSkill)
{
	if (pSkill)
	{
		mTimer->stopTimer(pSkill->getCoolTimerKey());
		mTimer->stopTimer(pSkill->getDelayTimerKey());
		mTimer->stopTimer(pSkill->getDelayIntervalTimerKey());
		mTimer->stopTimer(pSkill->getContinualTimerKey());
		mTimer->stopTimer(pSkill->getIntervalTimerKey());
	}
}

void AsstActiveSkillComponent::run()
{
	CCLOG("[trace]AsstActiveSkillCompoent::run : Run");

	for (auto &skillPair : mSkillContainer)
	{
		auto pSkill = skillPair.second;
		this->coolSkill(pSkill);
	}
}

void AsstActiveSkillComponent::setSkillEventCallback(const SkillEventCallback& callback)
{
	mOnSkillEventCallback = callback;
}

void AsstActiveSkillComponent::onCoolTimeout(const std::string& key, unsigned int skillId)
{
	mTimer->stopTimer(key);

	auto pSkill = this->getSkill(skillId);
	if (nullptr == pSkill)
	{
		CCLOG("[warning]AsstActiveSkillCompoent::onCoolTimeout : Skill[%d] not found ", skillId);
		return;
	}

	if (pSkill->getCoolTimerKey() != key)
	{
		CCLOG("[warning]AsstActiveSkillCompoent::onCoolTimeout : Skill[%d] may be changed caster or Caster may be changed camp, new key = %s, old key = %s ", skillId, pSkill->getCoolTimerKey().c_str(), key.c_str());
		return;
	}

	auto pBase = pSkill->getBaseData();
	if (nullptr == pBase)
	{
		CCLOG("[error]AsstActiveSkillCompoent::onCoolTimeout : Skill's base is null ");
		return;
	}

	this->sendSkillEvent(SkillEvent::kCDEnd, skillId);

	if (pBase->mCastType == AsstActiveSkill::CastType::kAuto)
	{
		mTimer->startTimer(pSkill->getDelayTimerKey(), pSkill->getBaseData()->mDelayTime, std::bind(&AsstActiveSkillComponent::onDelayTimeout, this, std::placeholders::_1, pSkill->getId()));
		mTimer->startTimer(pSkill->getDelayIntervalTimerKey(), AsstActiveSkillComponent::DelayInterval, std::bind(&AsstActiveSkillComponent::onDelayIntervalTimeout, this, std::placeholders::_1, pSkill->getId()));
	}
}

void AsstActiveSkillComponent::onDelayIntervalTimeout(const std::string& key, unsigned int skillId)
{
	auto pSkill = this->getSkill(skillId);
	if (nullptr == pSkill)
	{
		CCLOG("[warning]AsstActiveSkillCompoent::onDelayIntervalTimeout : Skill[%d] not found ", skillId);
		mTimer->stopTimer(key);
		return;
	}

	if (pSkill->getDelayIntervalTimerKey() != key)
	{
		CCLOG("[warning]AsstActiveSkillCompoent::onDelayIntervalTimeout : Skill[%d] may be changed caster or Caster may be changed camp, new key = %s, old key = %s ", skillId, pSkill->getDelayIntervalTimerKey().c_str(), key.c_str());
		mTimer->stopTimer(key);
		return;
	}

	if (cocos2d::random(1.0f, 100.0f) < AsstActiveSkillComponent::DelayCastProb)
	{
		mTimer->stopTimer(pSkill->getDelayTimerKey());
		mTimer->stopTimer(key);
		this->requestSkill(skillId);
	}
	else
	{
		mTimer->resetTimer(key);
	}
}

void AsstActiveSkillComponent::onDelayTimeout(const std::string& key, unsigned int skillId)
{
	mTimer->stopTimer(key);

	auto pSkill = this->getSkill(skillId);
	if (nullptr == pSkill)
	{
		CCLOG("[warning]AsstActiveSkillCompoent::onDelayTimeout : Skill[%d] not found ", skillId);
		return;
	}

	if (pSkill->getDelayTimerKey() != key)
	{
		CCLOG("[warning]AsstActiveSkillCompoent::onDelayTimeout : Skill[%d] may be changed caster or Caster may be changed camp, new key = %s, old key = %s ", skillId, pSkill->getDelayIntervalTimerKey().c_str(), key.c_str());
		return;
	}

	mTimer->stopTimer(pSkill->getDelayIntervalTimerKey());

	this->requestSkill(skillId);
}

void AsstActiveSkillComponent::onIntervalTimeout(const std::string& key, unsigned int skillId)
{
	auto pSkill = this->getSkill(skillId);
	if (nullptr == pSkill)
	{
		CCLOG("[warning]AsstActiveSkillCompoent::onIntervalTimeout : Skill[%d] not found ", skillId);
		mTimer->stopTimer(key);
		return;
	}

	if (pSkill->getIntervalTimerKey() != key)
	{
		CCLOG("[warning]AsstActiveSkillCompoent::onIntervalTimeout : Skill[%d] may be changed caster or Caster may be changed camp, new key = %s, old key = %s ", skillId, pSkill->getIntervalTimerKey().c_str(), key.c_str());
		mTimer->stopTimer(key);
		return;
	}

	pSkill->castIng();

	this->sendSkillEvent(SkillEvent::kCastIng, skillId);

	mTimer->resetTimer(key);
}

void AsstActiveSkillComponent::onContinualTimeout(const std::string& key, unsigned int skillId)
{
	mTimer->stopTimer(key);
	auto pSkill = this->getSkill(skillId);
	if (nullptr == pSkill)
	{
		CCLOG("[warning]AsstActiveSkillCompoent::onContinualTimeout : Skill[%d] not found ", skillId);
		return;
	}

	if (pSkill->getContinualTimerKey() != key)
	{
		CCLOG("[warning]AsstActiveSkillCompoent::onContinualTimeout : Skill[%d] may be changed caster or Caster may be changed camp, new key = %s, old key = %s ", skillId, pSkill->getContinualTimerKey().c_str(), key.c_str());
		return;
	}

	auto pBase = pSkill->getBaseData();
	if (nullptr == pBase)
	{
		CCLOG("[error]AsstActiveSkillCompoent::onCoolTimeout : Skill's base is null ");
		return;
	}

	mTimer->stopTimer(pSkill->getIntervalTimerKey());

	pSkill->castEnd();

	this->sendSkillEvent(SkillEvent::kCastEnd, skillId);

	this->coolSkill(pSkill);
}
