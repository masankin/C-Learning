
#include "Assistant.h"
#include "AsstActiveSkill.h"
#include "CCLuaEngine.h"
#include "lua/LuaFuncHelper.h"
#include "battle/Camp.h"
#include "battle/BattleControl.h"


Assistant * Assistant::createWithLuaData(Camp* owner)
{
	auto* pRet = new Assistant();
	if (pRet && pRet->initWithLuaData(owner))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

Assistant::Assistant() : mId(0)
, mLevel(0)
, mQuality(0)
, mOwner(nullptr)
, mView(nullptr)
, mCompoentContainer(nullptr)
, mAlive(false)
{

}

Assistant::~Assistant()
{
	this->cleanActiveSkillCastQueue();

	if (mCompoentContainer)
	{
		for (auto itr = mCompoentContainer->begin(); itr != mCompoentContainer->end(); ++itr)
		{
			auto pCom = itr->second;
			if (pCom)
			{
				pCom->release();
				pCom = nullptr;
			}
		}
		mCompoentContainer->clear();
		delete mCompoentContainer;
		mCompoentContainer = nullptr;
	}

	if (mView)
	{
		mView->release();
		mView = nullptr;
	}
}

bool Assistant::initWithLuaData(Camp* pOwner)
{
	if (nullptr == pOwner)
	{
		CCLOG("[error]Assistant::initWithLuaData : owner is null");
		return false;
	}

	int campType = pOwner->getCampType();
	if (campType < eCampType::ECAMP_TYPE_Player || campType > eCampType::ECAMP_TYPE_Player4)
	{
		CCLOG("[trace]Assistant::initWithLuaData : Glayph is disable for this camp[%d]", campType);
		return false;
	}

	auto luaFuncHandler = sgLuaFuncHandlerManager()->getHandler(LuaFuncHandlerManager::ModuleType::kCustom, (int)LuaFuncHandlerManager::CustomHandlerType::kGetAssistantData);
	if (0 == luaFuncHandler)
	{
		CCLOG("[error]Assistant::initWithLuaData : No register lua handler, camp[%d]", campType);
		return false;
	}

	LuaEngine* scriptEngine = dynamic_cast<LuaEngine*>(ScriptEngineManager::getInstance()->getScriptEngine());
	if (nullptr == scriptEngine)
	{
		CCLOG("[error]Assistant::initWithLuaData : Script engine is null");
		return false;
	}

	LuaStack* stack = scriptEngine->getLuaStack();
	if (nullptr == stack)
	{
		CCLOG("[error]Assistant::initWithLuaData : Lua stack is null");
		return false;
	}

	mOwner = pOwner;

	// Essential component
	this->addComponent(AsstActiveSkillComponent::create());
	auto pASCom = dynamic_cast<AsstActiveSkillComponent*>(this->getComponent(ASST_COMNAME_ACTIVE_SKILL));
	if (pASCom)
		pASCom->setSkillEventCallback(std::bind(&Assistant::onActiveSkillEvent, this, std::placeholders::_1, std::placeholders::_2));

	// Load from lua
	auto tolua_S = stack->getLuaState();

	stack->pushInt(campType);

	LuaStatePrint(tolua_S);
	auto rlt = stack->executeFunction(luaFuncHandler, 1, 1, std::bind(&Assistant::onLuaGetBtlAsstComplete, this, std::placeholders::_1, std::placeholders::_2));
	LuaStatePrint(tolua_S);

	if (rlt == 0)
		lua_settop(tolua_S, 0);
	else
		lua_pop(tolua_S, 1);

	if (0 == mId)
	{
		CCLOG("[trace]Assistant::initWithLuaData : Camp has no assistant now, camp = %d", campType);
		return false;
	}

	// Create view node
	mView = AssistantView::create(mAniFile);
	if (nullptr == mView)
	{
		CCLOG("[error]Assistant::initWithLuaData : View create fail, filename = %s", mAniFile.c_str());
		return false;
	}

	mView->setAniEventCallback(std::bind(&Assistant::onAniEvent, this, std::placeholders::_1));
	mView->retain();
	mView->onIdle();
	mAlive = true;

	return true;
}

void Assistant::onLuaGetBtlAsstComplete(lua_State* tolua_S, int argc)
{
	LuaStatePrint(tolua_S);
	if (!lua_istable(tolua_S, -1))
		return;

	lua_pushnil(tolua_S);
	LuaStatePrint(tolua_S);
	while (lua_next(tolua_S, -2))
	{
		LuaStatePrint(tolua_S);
		std::string key;
		if (lua_isstring(tolua_S, -2))
			key = lua_tostring(tolua_S, -2);

		if (key == "id")
		{
			if (lua_isnumber(tolua_S, -1))
				mId = lua_tointeger(tolua_S, -1);
		}
		else if (key == "level")
		{
			if (lua_isnumber(tolua_S, -1))
				mLevel = lua_tointeger(tolua_S, -1);
		}
		else if (key == "quality")
		{
			if (lua_isnumber(tolua_S, -1))
				mQuality = lua_tointeger(tolua_S, -1);
		}
		else if (key == "aniFile")
		{
			if (lua_isstring(tolua_S, -2))
				mAniFile = lua_tostring(tolua_S, -1);
		}
		else if (key == "activeSkills")
		{
			if (lua_istable(tolua_S, -1))
			{
				auto pBaseData = AsstActiveSkillData::create();
				if (pBaseData)
				{
					std::string skillKey;
					lua_pushnil(tolua_S);
					while (lua_next(tolua_S, -2))
					{
						LuaStatePrint(tolua_S);
						if (lua_isstring(tolua_S, -2))
							skillKey = lua_tostring(tolua_S, -2);

						if (skillKey == "id")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mId = lua_tointeger(tolua_S, -1);
						}
						else if (skillKey == "aniFile")
						{
							if (lua_isstring(tolua_S, -2))
								pBaseData->mName = lua_tostring(tolua_S, -1);
						}
						else if (skillKey == "iconFile")
						{
							if (lua_isstring(tolua_S, -2))
								pBaseData->mIconFile = lua_tostring(tolua_S, -1);
						}
						else if (skillKey == "effFile")
						{
							if (lua_isstring(tolua_S, -2))
								pBaseData->mEffectfile = lua_tostring(tolua_S, -1);
						}
						else if (skillKey == "type")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mType = (AsstActiveSkill::Type)lua_tointeger(tolua_S, -1);
						}
						else if (skillKey == "castType")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mCastType = (AsstActiveSkill::CastType)lua_tointeger(tolua_S, -1);
						}
						else if (skillKey == "coolTime")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mCoolTime = lua_tonumber(tolua_S, -1);
						}
						else if (skillKey == "delayTime")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mDelayTime = lua_tonumber(tolua_S, -1);
						}
						else if (skillKey == "continualTime")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mContinualTime = lua_tonumber(tolua_S, -1);
						}
						else if (skillKey == "intervalTime")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mIntervalTime = lua_tonumber(tolua_S, -1);
						}
						else if (skillKey == "target")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mTarget = (AsstActiveSkill::Target)lua_tointeger(tolua_S, -1);
						}
						else if (skillKey == "range")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mRange = (AsstActiveSkill::Range)lua_tointeger(tolua_S, -1);
						}
						else if (skillKey == "rgMin")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mRgMin = lua_tointeger(tolua_S, -1);
						}
						else if (skillKey == "rgMax")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mRgMax = lua_tointeger(tolua_S, -1);
						}
						else if (skillKey == "operator")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mOperator = (AsstActiveSkill::Operator)lua_tointeger(tolua_S, -1);
						}
						else if (skillKey == "numRule")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mNumRule = (AsstActiveSkill::NumRule)lua_tointeger(tolua_S, -1);
						}
						else if (skillKey == "numType")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mNumType = (AsstActiveSkill::NumType)lua_tointeger(tolua_S, -1);
						}
						else if (skillKey == "initNum")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mInitNum = lua_tointeger(tolua_S, -1);
						}
						else if (skillKey == "numRange")
						{
							if (lua_isnumber(tolua_S, -1))
								pBaseData->mNumRange = lua_tointeger(tolua_S, -1);
						}
						lua_pop(tolua_S, 1);
					}

					this->addActiveSkill(pBaseData);
				}
			}
		}
		lua_pop(tolua_S, 1);
	}
}

void Assistant::run()
{
	if (mCompoentContainer)
	{
		for (auto itr = mCompoentContainer->begin(); itr != mCompoentContainer->end(); ++itr)
		{
			auto pCom = itr->second;
			if (pCom)
				pCom->run();
		}
	}
}

void Assistant::update(float dt)
{
	if (mAlive == true && mCompoentContainer)
	{
		for (auto itr = mCompoentContainer->begin(); itr != mCompoentContainer->end(); ++itr)
		{
			auto pCom = itr->second;
			if (pCom)
				pCom->update(dt);
		}
	}
}

void Assistant::clean()
{
	this->cleanActiveSkillCastQueue();

	if (mCompoentContainer)
	{
		for (auto itr = mCompoentContainer->begin(); itr != mCompoentContainer->end(); ++itr)
		{
			auto pCom = itr->second;
			if (pCom)
				pCom->clean();
		}
	}
	mAlive = false;
}

void Assistant::stop()
{
	if (mView)
	{
		mView->stopAnimtion();
		mView->setVisible(false);
	}
	mAlive = false;
}

bool Assistant::addComponent(AsstComponent* pComponent)
{
	if (nullptr == pComponent)
	{
		CCLOG("[error]Assistant::addCompoent : Argument must be non-nil");
		return false;
	}

	if (nullptr != pComponent->getOwner())
	{
		CCLOG("[error]Assistant::addCompoent : Component already added. It can't be added again");
		return false;
	}

	// lazy init
	if (mCompoentContainer == nullptr)
	{
		mCompoentContainer = new Assistant::ComponentContainer();
	}

	if (mCompoentContainer->end() != mCompoentContainer->find(pComponent->getName()))
	{
		CCLOG("[error]Assistant::addCompoent : Component already cached. It can't be added again");
		return false;
	}

	pComponent->setOwner(this);
	pComponent->retain();
	mCompoentContainer->insert(std::make_pair(pComponent->getName(), pComponent));

	return true;
}

bool Assistant::removeComponent(const std::string& comName)
{
	if (nullptr == mCompoentContainer)
		return false;

	auto findItr = mCompoentContainer->find(comName);
	if (mCompoentContainer->end() == findItr)
	{
		return false;
	}

	auto pComponent = findItr->second;
	if (pComponent)
	{
		pComponent->setOwner(nullptr);
		pComponent->release();

		mCompoentContainer->erase(findItr);
		return true;
	}

	return false;
}

AsstComponent* Assistant::getComponent(const std::string& comName) const
{
	if (nullptr == mCompoentContainer)
		return nullptr;

	auto findItr = mCompoentContainer->find(comName);
	if (mCompoentContainer->end() == findItr)
	{
		return nullptr;
	}
	else
	{
		return findItr->second;
	}
}

unsigned int Assistant::getId() const
{
	return mId;
}


AssistantView* Assistant::getView() const
{
	return mView;
}

eCampType Assistant::getCampType() const
{
	if (mOwner)
		return mOwner->getCampType();

	CCLOG("[error]Assistant::getCampType : mOwner is null");

	return ECAMP_TYPE_Invalid;
}

bool Assistant::addActiveSkill(AsstActiveSkillData* pSkillData)
{
	if (nullptr == pSkillData)
	{
		CCLOG("[error]Assistant::addActiveSkill : Skill data is null");
		return false;
	}

	auto pASCom = dynamic_cast<AsstActiveSkillComponent*>(this->getComponent(ASST_COMNAME_ACTIVE_SKILL));
	if (pASCom)
		pASCom->addSkill(pSkillData);

	return true;
}

void Assistant::castActiveSkill(unsigned int skillId, std::vector<int>& randomList)
{
	auto pASCom = dynamic_cast<AsstActiveSkillComponent*>(this->getComponent(ASST_COMNAME_ACTIVE_SKILL));
	if (pASCom && !pASCom->checkSkill(skillId))
	{
		pASCom->coolSkill(skillId);
		return;
	}

	ActiveSkillCastInfo* pInfo = new ActiveSkillCastInfo();
	if (nullptr == pInfo)
	{
		CCLOG("[error]Assistant::castSkill : Queue ActiveSkillCastInfo fail, camp = %d, skillId = %d", mOwner->getCampType(), skillId);
		return;
	}

	pInfo->skillId = skillId;
	pInfo->randomList.swap(randomList);
	mASCastQueue.push_back(pInfo);

	if (mView && !mView->isCastingSkill())
		mView->onCastSkill();
}

void Assistant::cleanActiveSkillCastQueue()
{
	for (auto pASCastInfo : mASCastQueue)
	{
		if (pASCastInfo)
		{
			delete pASCastInfo;
			pASCastInfo = nullptr;
		}
	}
	mASCastQueue.clear();
}

void Assistant::onActiveSkillEvent(AsstActiveSkillComponent::SkillEvent eventType, unsigned int skillId)
{
	switch (eventType)
	{
	case AsstActiveSkillComponent::SkillEvent::kCDBeg:
		break;
	case AsstActiveSkillComponent::SkillEvent::kCDEnd:
		break;
	case AsstActiveSkillComponent::SkillEvent::kCastBeg:
		break;
	case AsstActiveSkillComponent::SkillEvent::kCastIng:
		break;
	case AsstActiveSkillComponent::SkillEvent::kCastEnd:
		break;
	default:
		break;
	}
}

void Assistant::onAniEvent(AssistantView::AniEvent eventType)
{
	switch (eventType)
	{
	case AssistantView::kCastSkillStart:
	{

	}
	break;

	case AssistantView::kCastSkill:
	{
		auto pCastInfo = mASCastQueue.front();
		mASCastQueue.pop_front();
		if (pCastInfo)
		{
			auto pASCom = dynamic_cast<AsstActiveSkillComponent*>(this->getComponent(ASST_COMNAME_ACTIVE_SKILL));
			if (pASCom)
				pASCom->castSkill(pCastInfo->skillId, pCastInfo->randomList);

			delete pCastInfo;
			pCastInfo = nullptr;
		}
	}
	break;

	case AssistantView::kCastSkillEnd:
	{
		if (!mASCastQueue.empty())
		{
			mView->onCastSkill();
		}
	}
	break;
	default:
		break;
	}
}
