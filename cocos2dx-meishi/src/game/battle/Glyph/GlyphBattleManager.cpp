#include "GlyphBattleManager.h"
#include "GlyphCampAgent.h"
#include "TimeCalculator.h"
#include "data/TableManager.h"

/**
 * GlyphBattleManager implementation
 */

const std::string GlyphBattleManager::TimeKeySpirit("GlyphBattleManager_Spirit");

GlyphBattleManager::GlyphBattleManager() :mSpirit(0.0f)
, mSpiritInc(0.0f)
{

}


GlyphBattleManager::~GlyphBattleManager()
{
	this->clean();
}


void GlyphBattleManager::load(int checkptId)
{
	if (checkptId <1130)
	{
		return;
	}
	CCLOG("GlyphBattleManager::load : Glyph loading lua data, checkpt id : %d", checkptId);

	// move load to camp
	/*for (CampGlyphList::iterator it = mCampGlyphList.begin(); it != mCampGlyphList.end(); it++)
	{
		it->second->load();
	}*/

	auto pCheckptData = sgTableManager()->getCheckPointByID(checkptId);
	if (pCheckptData)
	{
		mSpiritInc = pCheckptData->spirit;
	}

	if (mSpiritInc > 0.0f)
		sgTimeCalculator()->createTimeCalcultor(GlyphBattleManager::TimeKeySpirit, 1.0f);
	else
		CCLOG("[warn]GlyphBattleManager::load : spirit increse value is 0");
}


void GlyphBattleManager::update(float dt)
{
	for (auto &campGlyphPair : mCampGlyphList)
	{
		auto agent = campGlyphPair.second;
		if (agent)
			agent->update(dt);
	}

	if (sgTimeCalculator()->getTimeLeft(GlyphBattleManager::TimeKeySpirit) == 0.0f)
	{
		mSpirit += mSpiritInc / 100.0f;
		if (mSpirit >= 100.0f)
			mSpirit = 100.0f;

		for (auto &campGlyphPair : mCampGlyphList)
		{
			auto agent = campGlyphPair.second;
			if (agent)
				agent->onSpiritChange(mSpirit);
		}

		if (mSpirit < 100.0f)
			sgTimeCalculator()->resetTimeCalcultor(GlyphBattleManager::TimeKeySpirit);
		else
			sgTimeCalculator()->removeTimeCalcultor(GlyphBattleManager::TimeKeySpirit);
	}
}


void GlyphBattleManager::clean()
{
	CCLOG("[trace]GlyphBattleManager::clean");

	sgTimeCalculator()->removeTimeCalcultor(GlyphBattleManager::TimeKeySpirit);

	mSpirit = 0.0f;
	mSpiritInc = 0.0f;

	for (CampGlyphList::iterator it = mCampGlyphList.begin(); it != mCampGlyphList.end(); it++)
	{
		it->second->clean();
	}
	mCampGlyphList.clear();

}


bool GlyphBattleManager::registAgent(GlyphCampAgent* agent) 
{
	if (agent != nullptr && agent->getCamp() != ECAMP_TYPE_Invalid)
	{
		CampGlyphList::iterator it = mCampGlyphList.find(agent->getCamp());
		if (it == mCampGlyphList.end())
		{
			mCampGlyphList.insert(std::make_pair(agent->getCamp(), agent));
		}
		else
		{
			CCLOG("[warning]GlyphBattleManager::registAgent : already cached camp = %d", agent->getCamp());
		}
	}
	else
	{
		CCLOG("[error]GlyphBattleManager::registAgent : agent is null or camp is invalid");
		return false;
	}

	return true;
}


void GlyphBattleManager::unregistAgent(GlyphCampAgent* agent)
{
	if (agent != nullptr)
	{
		CampGlyphList::iterator it = mCampGlyphList.find(agent->getCamp());
		if (it != mCampGlyphList.end() && it->second == agent)
		{
			it->second->clean();
			mCampGlyphList.erase(it);
		}
		else
		{
			CCLOG("[warning]GlyphBattleManager::unregistAgent  can't found  GlyphCampAgent camp = %d ", agent->getCamp());
		}
	}
	else
	{
		CCLOG("[warning]GlyphBattleManager::unregistAgent  agent = null ");
	}
}



Glyph* GlyphBattleManager::getCampGlyph(eCampType camp, Glyph::eGlyphType type) 
{
	CampGlyphList::iterator it = mCampGlyphList.find(camp);
	if (it != mCampGlyphList.end())
	{
		return it->second->getGlyph(type);
	}
	
	//CCLOG("[trace]GlyphBattleManager::getCampGlyph : can't found  camp = %d and type = %d ", camp , type);
    return nullptr;
}
