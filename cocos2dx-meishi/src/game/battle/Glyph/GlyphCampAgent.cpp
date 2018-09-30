#include "CCLuaEngine.h"
#include "lua/LuaFuncHelper.h"
#include "GlyphData.h"
#include "GlyphCampAgent.h"
#include "GlyphBattleManager.h"
#include "battle/BattleControl.h"
#include "battle/Camp.h"
#include "TimeCalculator.h"

/**
 * GlyphCampAgent implementation
 */

GlyphCampAgent* GlyphCampAgent::create(Camp* owner)
{
	GlyphCampAgent* pRet = new GlyphCampAgent();
	if (pRet && pRet->init(owner))
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


GlyphCampAgent::GlyphCampAgent() :mOwner(nullptr)
, mAtkLvCounter(0)
, mDefLvCounter(0)
, mSpdLvCounter(0)
, mPplLvCounter(0)
, mMrlLvCounter(0)
{

}


GlyphCampAgent::~GlyphCampAgent()
{

}

bool GlyphCampAgent::init(Camp* owner)
{
	if (nullptr == owner)
	{
		CCLOG("[error]GlyphCampAgent::init : owner is null");
		return false;
	}

	mOwner = owner;

	return true;
}


void GlyphCampAgent::clean()
{
	for (auto &timerPair : mIntervalTimerList)
	{
		sgTimeCalculator()->removeTimeCalcultor(timerPair.first);
	}
	mIntervalTimerList.clear();

	for (GlyphList::iterator it = mGlyphList.begin(); it != mGlyphList.end(); it++)
	{
		CC_SAFE_RELEASE(it->second);
	}
	mGlyphList.clear();
}



bool GlyphCampAgent::load() 
{
	auto luaFuncHandler = sgLuaFuncHandlerManager()->getHandler(LuaFuncHandlerManager::ModuleType::kCustom, (int)LuaFuncHandlerManager::CustomHandlerType::kGetGlyphData);
	if (0 == luaFuncHandler)
	{
		CCLOG("[error]GlyphCampAgent::load : No register lua handler, camp[%d]", this->getCamp());
		return false;
	}

	LuaEngine* scriptEngine = dynamic_cast<LuaEngine*>(ScriptEngineManager::getInstance()->getScriptEngine());
	if (nullptr == scriptEngine)
	{
		CCLOG("[error]GlyphCampAgent::load : Script engine is null");
		return false;
	}
	
	LuaStack* stack = scriptEngine->getLuaStack();
	if (nullptr == stack)
	{
		CCLOG("[error]GlyphCampAgent::load : Lua stack is null");
		return false;
	}
	
	int campType = (int)this->getCamp();
	if (campType < eCampType::ECAMP_TYPE_Player || campType > eCampType::ECAMP_TYPE_Player4)
	{
		CCLOG("[trace]GlyphCampAgent::load : Glayph is disable for this camp[%d]", campType);
		return true;
	}

	auto tolua_S = stack->getLuaState();

	for (int id = Glyph::eGlyphType::E_GLYPH_ATK_BEG + 1; id < Glyph::eGlyphType::E_GLYPH_ATK_END; ++id)
	{
		stack->pushInt(id);
		stack->pushInt(campType);

		LuaStatePrint(tolua_S);
		auto rlt = stack->executeFunction(luaFuncHandler, 2, 1, std::bind(&GlyphCampAgent::onLuaGetGlyphComplete, this, id, std::ref(mAtkLvCounter), std::placeholders::_1, std::placeholders::_2));
		LuaStatePrint(tolua_S);

		if (rlt == 0)
			lua_settop(tolua_S, 0);
		else
			lua_pop(tolua_S, 1);
	}

	for (int id = Glyph::eGlyphType::E_GLYPH_DEF_BEG + 1; id < Glyph::eGlyphType::E_GLYPH_DEF_END; ++id)
	{
		stack->pushInt(id);
		stack->pushInt(campType);

		LuaStatePrint(tolua_S);
		auto rlt = stack->executeFunction(luaFuncHandler, 2, 1, std::bind(&GlyphCampAgent::onLuaGetGlyphComplete, this, id, std::ref(mDefLvCounter), std::placeholders::_1, std::placeholders::_2));
		LuaStatePrint(tolua_S);

		if (rlt == 0)
			lua_settop(tolua_S, 0);
		else
			lua_pop(tolua_S, 1);
	}

	for (int id = Glyph::eGlyphType::E_GLYPH_SPD_BEG + 1; id < Glyph::eGlyphType::E_GLYPH_SPD_END; ++id)
	{
		stack->pushInt(id);
		stack->pushInt(campType);

		LuaStatePrint(tolua_S);
		auto rlt = stack->executeFunction(luaFuncHandler, 2, 1, std::bind(&GlyphCampAgent::onLuaGetGlyphComplete, this, id, std::ref(mSpdLvCounter), std::placeholders::_1, std::placeholders::_2));
		LuaStatePrint(tolua_S);

		if (rlt == 0)
			lua_settop(tolua_S, 0);
		else
			lua_pop(tolua_S, 1);
	}

	for (int id = Glyph::eGlyphType::E_GLYPH_PPL_BEG + 1; id < Glyph::eGlyphType::E_GLYPH_PPL_END; ++id)
	{
		stack->pushInt(id);
		stack->pushInt(campType);

		LuaStatePrint(tolua_S);
		auto rlt = stack->executeFunction(luaFuncHandler, 2, 1, std::bind(&GlyphCampAgent::onLuaGetGlyphComplete, this, id, std::ref(mPplLvCounter), std::placeholders::_1, std::placeholders::_2));
		LuaStatePrint(tolua_S);

		if (rlt == 0)
			lua_settop(tolua_S, 0);
		else
			lua_pop(tolua_S, 1);
	}

	for (int id = Glyph::eGlyphType::E_GLYPH_MRL_BEG + 1; id < Glyph::eGlyphType::E_GLYPH_MRL_END; ++id)
	{
		stack->pushInt(id);
		stack->pushInt(campType);

		LuaStatePrint(tolua_S);
		auto rlt = stack->executeFunction(luaFuncHandler, 2, 1, std::bind(&GlyphCampAgent::onLuaGetGlyphComplete, this, id, std::ref(mMrlLvCounter), std::placeholders::_1, std::placeholders::_2));
		LuaStatePrint(tolua_S);

		if (rlt == 0)
			lua_settop(tolua_S, 0);
		else
			lua_pop(tolua_S, 1);
	}

	return true;
}


void GlyphCampAgent::update(float dt)
{
	//for (GlyphList::iterator it = mGlyphList.begin(); it != mGlyphList.end(); it++)
	//{
		//CC_SAFE_RELEASE(it->second);
	//}

	for (auto &timerPair : mIntervalTimerList)
	{
		if (sgTimeCalculator()->getTimeLeft(timerPair.first) == 0)
		{
			auto pGlyph = this->getGlyph(timerPair.second);
			if (pGlyph)
			{
				pGlyph->onIntervalTimeout(this->getCamp());
			}

			sgTimeCalculator()->resetTimeCalcultor(timerPair.first);
		}
	}
}


void GlyphCampAgent::onLuaGetGlyphComplete(int glyphId, int &lvCounter, lua_State* tolua_S, int argc)
{
	LuaStatePrint(tolua_S);
	if (lua_istable(tolua_S, -1))
	{
		auto pBaseData = GlyphBaseData::create();
		if (nullptr == pBaseData)
			return;

		lua_pushnil(tolua_S);
		LuaStatePrint(tolua_S);
		while (lua_next(tolua_S, -2))
		{
			LuaStatePrint(tolua_S);
			std::string key;
			if (lua_isstring(tolua_S, -2))
				key = lua_tostring(tolua_S, -2);

			if (key == "mainAttr")
			{
				if (lua_isnumber(tolua_S, -1))
					pBaseData->mMainAttr = lua_tonumber(tolua_S, -1) / 100;
			}
			else if (key == "sideAttr")
			{
				if (lua_isnumber(tolua_S, -1))
					pBaseData->mViceAttr = lua_tonumber(tolua_S, -1);
			}
			else if (key == "soul")
			{
				if (lua_isnumber(tolua_S, -1))
				{
					auto val = lua_tonumber(tolua_S, -1);
					if (val != 0)
						pBaseData->mIsSpiritAffected = true;
					else
						pBaseData->mIsSpiritAffected = false;
				}
			}
			else if (key == "buildLevel")
			{
				if (lua_istable(tolua_S, -1))
				{
					lua_pushnil(tolua_S);
					while (lua_next(tolua_S, -2))
					{
						if (lua_isnumber(tolua_S, -1))
							pBaseData->mBuildLevelAffectedAttrs.push_back(lua_tonumber(tolua_S, -1));
						lua_pop(tolua_S, 1);
					}
				}
			}
			else if (key == "battleTip")
			{
				if (lua_isstring(tolua_S, -2))
					pBaseData->mBattleTip = lua_tostring(tolua_S, -1);
			}
			else if (key == "glyphLevel")
			{
				if (lua_isnumber(tolua_S, -1))
					pBaseData->mLevel = lua_tointeger(tolua_S, -1);
			}
			lua_pop(tolua_S, 1);
		}

		auto glyphType = (Glyph::eGlyphType)glyphId;
		auto pGlyph = Glyph::create(glyphType, pBaseData);
		if (nullptr == pGlyph)
		{
			CCLOG("[warning]GlyphCampAgent::onLuaGetGlyphComplete : Create glyph failed, glyph type is %d", glyphId);
			return;
		}
		else
		{
			pGlyph->retain();
			mGlyphList.emplace(std::make_pair(glyphType, pGlyph));

			lvCounter += pBaseData->mLevel;

			auto timerKey = StringUtils::format("GlyphCampAgent_%d_%d", (int)this->getCamp(), glyphId);
			switch (glyphType)
			{
			case Glyph::E_GLYPH_MIN:
				break;
			case Glyph::E_GLYPH_ATK_POWER:
				break;
			case Glyph::E_GLYPH_ATK_TIME:
			{
				sgTimeCalculator()->createTimeCalcultor(timerKey, pGlyph->getDataViceAttr());
				mIntervalTimerList.emplace(std::make_pair(timerKey, glyphType));
			}
			break;
			case Glyph::E_GLYPH_ATK_INSTITUTE:
				break;
			case Glyph::E_GLYPH_ATK_KILL:
				break;
			case Glyph::E_GLYPH_DEF_DEFENSE:
				break;
			case Glyph::E_GLYPH_DEF_TIME:
			{
				sgTimeCalculator()->createTimeCalcultor(timerKey, pGlyph->getDataViceAttr());
				mIntervalTimerList.emplace(std::make_pair(timerKey, glyphType));
			}
			break;
			case Glyph::E_GLYPH_DEF_INSTITUTE:
				break;
			case Glyph::E_GLYPH_DEF_DEAD:
				break;
			case Glyph::E_GLYPH_SPD_MOVE:
				break;
			case Glyph::E_GLYPH_SPD_UP:
				break;
			case Glyph::E_GLYPH_SPD_PRODUCE:
				break;
			case Glyph::E_GLYPH_SPD_CREEPS:
				break;
			case Glyph::E_GLYPH_SPD_ASSAULT:
				break;
			case Glyph::E_GLYPH_SPD_ASSIST:
				break;
			case Glyph::E_GLYPH_PPL_INIT:
				break;
			case Glyph::E_GLYPH_PPL_UP:
				break;
			case Glyph::E_GLYPH_PPL_OCCUPY:
				break;
			case Glyph::E_GLYPH_PPL_PRODUCE:
				break;
			case Glyph::E_GLYPH_MRL_ADD:
				break;
			case Glyph::E_GLYPH_MRL_TIME:
			{
				sgTimeCalculator()->createTimeCalcultor(timerKey, pGlyph->getDataViceAttr());
				mIntervalTimerList.emplace(std::make_pair(timerKey, glyphType));
			}
			break;
			case Glyph::E_GLYPH_MAX:
				break;
			default:
				break;
			}
		}
	}
}


void GlyphCampAgent::onSpiritChange(float spirit)
{
	for (auto & glyphPair : mGlyphList)
	{
		auto pGlyph = glyphPair.second;
		if (pGlyph)
		{
			pGlyph->onSpiritChange(spirit);
		}
	}
}


eCampType GlyphCampAgent::getCamp() 
{
	if (mOwner)
		return mOwner->getCampType();
	
	CCLOG("[error]GlyphCampAgent::getCamp : mOwner is null");

	return ECAMP_TYPE_Invalid;
}


Glyph* GlyphCampAgent::getGlyph(Glyph::eGlyphType type) 
{
	GlyphList::iterator it = mGlyphList.find(type);
	if (it != mGlyphList.end())
	{
		return it->second;
	}

	//CCLOG("[error]GlyphCampAgent::getGlyph  can't found  type = %d ", type);
    return nullptr;
}
