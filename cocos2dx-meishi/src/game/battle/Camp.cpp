#include "Camp.h"
#include "Build.h"
#include "AI/AI.h"
#include "Constant.h"
#include "game/prop/PropsControl.h"
#include "TimeCalculator.h"
//#include "game/battle/Formation.h"
#include "game/battle/Formation/FormationV2.h"
#include "game/battle/BattleControl.h"
#include "BuildView.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"
#include "Solider.h"
#include "buff/BuffManager.h"
#include "battle/BattleRecordsManager.h"

#include "battle/Glyph/GlyphBattleManager.h"
#include "battle/Glyph/GlyphCampAgent.h"
#include "battle/Glyph/GlyphData.h"

#include "Assistant/Assistant.h"

Camp::Camp() : mAICom(NULL)
, mResScore(0.0f)
, mFormationMoveSpeed(NORMAL_SOLDIER_SPEED)
, mLive(true)
, mKillPeople(0)
, mProductPeople(0)
, mDeadPeople(0)
, mHasBuild(true)
, mGlyphAgent(nullptr)
, mAssistant(nullptr)
, mOccupyBuild(0)
, mAsstAtkAddition(0)
, mAsstSpeedAddition(0)
{

}

Camp::~Camp()
{
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); it++)
		delete it->second;
	mBuildList.clear();

	mFormationList.clear();

	if (mAICom)
		delete mAICom;
	
	if (mGlyphAgent)
	{
		sgBattleControl()->getGlyphBattleMgr()->unregistAgent(mGlyphAgent);
		mGlyphAgent->release();
		mGlyphAgent = nullptr;
	}

	if (mAssistant)
	{
		sgBattleControl()->getAsstBattleCtrl()->unregistAssistant(mAssistant);
		mAssistant->release();
		mAssistant = nullptr;
	}
}

bool Camp::init(eCampType etype)
{
	setCampType(etype);

	if (!sgBattleControl()->getIsPvp()
		&& (etype == ECAMP_TYPE_AI || etype == ECAMP_TYPE_AI2 || etype == ECAMP_TYPE_AI3))
	{
		mAICom = AI::create(etype);
	}

	mGlyphAgent = GlyphCampAgent::create(this);
	if (mGlyphAgent)
	{
		mGlyphAgent->retain();
		sgBattleControl()->getGlyphBattleMgr()->registAgent(mGlyphAgent);
	}

	return true;
}

void Camp::update(float dt)
{
	if (mAICom)
		mAICom->update(dt);
	MapBulidList::iterator bIt = mBuildList.begin();
	for (; bIt != mBuildList.end(); bIt++)
	{
		bIt->second->update();
	}

	for (Vect_Formation::iterator it = mFormationList.begin(); it != mFormationList.end();)
	{
		FormationV2* pFormation = it->second;
		if (nullptr == pFormation)
		{
			it = mFormationList.erase(it);
			continue;
		}

		if (pFormation->checkProducer(dt))
		{
			BaseBuild* pBaseBuild = this->getBuild(pFormation->getSrcBuildUid());

			if (nullptr != pBaseBuild)
			{
				int needCount = pFormation->getExpectedCount() - pFormation->getProductedCount();
				if (needCount < 0)
					needCount = 0;
				int reqCount = MIN(needCount, FormationV2::RowMaxCount);
				//CCLOG("[Evix]Producing %d", reqCount);
				int count = pBaseBuild->requestPeople(reqCount);
				if (count >= FormationV2::RowMaxCount)
				{
					pFormation->updateProducer(dt, count);
				}
				else
				{
					if (count > 0)
						pFormation->updateProducer(dt, count);
					pFormation->closeProducer();
				}

				if (pFormation->getSrcCamp() != pBaseBuild->getCamp())
					pFormation->closeProducer();
			}
			else
			{
				pFormation->closeProducer();
			}

			if (pFormation->checkProducerOver())
				pFormation->doProductOver();
		}

		pFormation->setSpeed(sgBuffManager()->getBuffMoveSpeed(getCampType(), pFormation->getBaseSpeed()));
		pFormation->update(dt);

		if (pFormation->checkOver())
		{
			pFormation->doOver();
			it = mFormationList.erase(it);
			continue;
		}

		++it;
	}
}

void Camp::load()
{
	if (mGlyphAgent)
		mGlyphAgent->load();

	if (mAssistant)
	{
		mAssistant->release();
		mAssistant = nullptr;
	}

	mAssistant = Assistant::createWithLuaData(this);
	if (mAssistant)
	{
		mAssistant->retain();
		sgBattleControl()->getAsstBattleCtrl()->registAssistant(mAssistant);
	}

}

Camp* Camp::createCamp(eCampType eType)
{
	Camp* pCamp = new Camp();
	if (pCamp && pCamp->init(eType))
	{
		return pCamp;
	}
	return NULL;
}

void Camp::addBuild(BuildInfo* pInfo)
{
	CCLOG("[trace] Camp::addBuild camp:%d type:%d", pInfo->camp, pInfo->type);

	BaseBuild* pBuild = BaseBuild::createBuild(*pInfo);
	if (!pBuild)
		return;

	int key = pBuild->getKeyID();
	if (mBuildList.find(key) == mBuildList.end())
	{
		mBuildList.insert(std::make_pair(key, pBuild));
		CCLOG("[trace] Camp::addBuild insert success.Build key:%d", key);
	}
	else
	{
		CC_SAFE_DELETE(pBuild);
		CCLOG("[error] Camp::addBuild insert failed.");
	}
}

void Camp::updateBuildInfoById(int id, BuildInfo* pInfo)
{
	addBuild(pInfo);
}

BaseBuild* Camp::getBuild(int key)
{
	MapBulidList::iterator it = mBuildList.find(key);
	if (it != mBuildList.end())
	{
		return it->second;
	}
	return NULL;
}

const MapBulidList& Camp::getBuildList()
{
	return mBuildList;
}

BaseBuild* Camp::getBuildByPos(Vec2 pt)
{
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); it++)
	{
		if (it->second->isEnable()
			&& it->second->boundingBox().containsPoint(pt))
		{
			return it->second;
		}
	}

	return NULL;
}

int Camp::getBuildNumByType(eBulidType buildType)
{
	int count = 0;
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); it++)
	{
		if (buildType == it->second->getBuildType())
			count = count + 1;
	}
	return count;
}

int Camp::getCampMaxProduct()
{
	int total = 0;
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); ++it)
	{
		total += it->second->getMaxProduct();
	}
	return total;
}

int Camp::getCampCurProduct()
{
	int total = 0;
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); ++it)
	{
		total += it->second->getOwnPeople();
	}
	return total;
}

int Camp::getInstituteNum()
{
	int total = 0;
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); ++it)
	{
		if (it->second->getBuildInfo().type == EBULID_TYPE_INSISTUTE)
			total++;
	}
	return total;
}


void Camp::occupyBuild(BaseBuild* pBuild, bool isDirty /* = true */)
{
	if (!pBuild)
		return;

	eCampType temp = pBuild->getBuildInfo().camp;
	if (temp != ECAMP_TYPE_Creeps)
	{
		pBuild->mBuildInfo.express = (eExpressType)(mCampType);
		if (!sgBattleRecordsManager()->getIsBeganPlayBack())
		{
			mOccupyBuild++;
		}
	}

	pBuild->mBuildInfo.camp = this->mCampType;

	if (pBuild->isCurUpgrade())
		pBuild->cancelUpgrade();

	pBuild->doDegrade();
	pBuild->resetFormationSendingList();

	mBuildList.insert(std::make_pair(pBuild->getKeyID(), pBuild));
	if (isDirty)
	{
		pBuild->setDirtyTag(EBDIR_CAMEP);
	}

	ActionStatus*  pStatu = pBuild->getActionStatu();
	if (pStatu)
	{
		pStatu->reset();
	}

	std::string sGlyphTip;

	auto pGlyph = getGlyph(Glyph::E_GLYPH_PPL_OCCUPY);
	if (pGlyph)
	{
		float fValue = 0.0f;

		if (pGlyph->isAffectedByBuildLevel())
		{
			fValue = pGlyph->getValue() * pGlyph->getDataBuildAffectAttr(pBuild->getLevel()) / 100.0f;
		}
		else
		{
			fValue = pGlyph->getValue();
		}

		pBuild->addMinusOwnPeople(fValue);
		sGlyphTip = StringUtils::format(pGlyph->getDataBattleTip().c_str(), (int)fValue);
		addProductPeople(fValue);
	}

	//占领提示信息
	//加入战斗提示消息发送 by edward 2015-09-23
	if (pBuild->getEnableBattleTip() && pBuild->getCamp() == sgBattleControl()->getOwnCamp())
	{
		char tPositionParam[256] = { 0, };
		if (pBuild->getBuildType() == EBULID_TYPE_BARRACK)
		{
			sprintf(tPositionParam, "%d %f %f", E_OCCUPY_BARRACK, pBuild->getBuildView()->getPositionX(), pBuild->getBuildView()->getPositionY() + 30, sGlyphTip.c_str());
		}
		else if (pBuild->getBuildType() == EBULID_TYPE_TURRET || pBuild->getBuildType() == EBUILD_TYPE_STABLES)
		{
			sprintf(tPositionParam, "%d %f %f", E_OCCUPY_TURRET, pBuild->getBuildView()->getPositionX(), pBuild->getBuildView()->getPositionY() + 80, sGlyphTip.c_str());
		}
		else if (pBuild->getBuildType() == EBULID_TYPE_INSISTUTE)
		{
			sprintf(tPositionParam, "%d %f %f", E_OCCUPY_INSTITUTE, pBuild->getBuildView()->getPositionX(), pBuild->getBuildView()->getPositionY() + 30, sGlyphTip.c_str());
		}
		sgMessageManager()->sendMessage(EMT_BATTLE_TIP, tPositionParam);

		pBuild->setEnableBattleTip(false);
		sgTimeCalculator()->createTimeCalcultor(StringUtils::format("battleTip_%s", pBuild->getKeyString().c_str()), 2.0f);
	}
}

void Camp::removeBuild(int BuildId,bool checkDead /* = true */)
{
	MapBulidList::iterator it = mBuildList.find(BuildId);

	if (it != mBuildList.end())
	{
		mBuildList.erase(it);
	}

	if (mBuildList.size() <= 0 && checkDead)
	{
		if (sgBattleControl()->getIsPvp())
		{
			if (sgUserObj()->getIsHost())
			{
				//TODO:send dead
				sgPvpControl()->getPvPProxy()->sendDeadCamp(getCampType());
				mHasBuild = false;
			}
		}
		else
		{
			if (mAssistant)
			{
				mAssistant->stop();
			}
			mHasBuild = false;
		}
	}
}

void Camp::insertBuild(BaseBuild* pBuild)
{
	if (pBuild && pBuild->getCamp() == mCampType)
	{
		MapBulidList::iterator it = mBuildList.find(pBuild->getKeyID());

		if (it == mBuildList.end())
		{
			mBuildList.insert(std::make_pair(pBuild->getKeyID(), pBuild));
		}
	}
}

void Camp::pushFormation(FormationV2* pForm)
{
	if (nullptr == pForm)
		return;

	mFormationList.insert(pForm->getId(), pForm);
}

int Camp::getAllUnit()
{
	if (!mLive) return 0;
	

	int content = 0;
	for (auto e : mBuildList)
	{
		content += e.second->mOwnPeople;
		//if (it.second->mBulidInfo.type == EBULID_TYPE_PRODUCT)
	}

	int content1 = 0;
	//TODO:计算mFormation 中士兵数量
	for (auto e : mFormationList)
	{
		content1 += e.second->getAliveEntityCount();
	}
	//if(getCampType() == ECAMP_TYPE_Player)
	//CCLOG("[trace]  Camp::getAllUnit content1 is %d", content1);
	content += content1;
	return content;
}

bool Camp::checkBeAttackWithTower(BaseBuild* pAttack)
{
	// 	Vect_Formation::iterator it = mFormationList.begin();
	// 	for (; it != mFormationList.end(); it++)
	// 	{
	// 		if (it->second->checkBeAttack(pAttack))
	// 		{
	// 			return true;
	// 		}
	// 	}
	return false;
}


void Camp::removeSoliderWithRect(const Rect& rect)
{
	// 	Vect_Formation::iterator it = mFormationList.begin();
	// 	for (; it != mFormationList.end(); ++it)
	// 	{
	// 		it->second->removeSoliderWithRect(rect);
	// 	}
	// 	sgPropsControl()->setCdTime(PROP_TYPE_COOKING);
}


Solider* Camp::getCanBeAttackSoliser(const Point& worldPos, float radiusA, float radiusB)
{
	auto filter = [](Solider* pSolider)->bool{

		if (pSolider->checkStatus(Solider::Status::kDead))
			return false;

		if (pSolider->checkStatus(Solider::Status::kSpotted))
			return false;

		return true;
	};

	Solider * pRet = nullptr;
	for (Vect_Formation::iterator it = mFormationList.begin(); it != mFormationList.end(); ++it)
	{
		FormationV2* pFormation = it->second;
		if (nullptr == pFormation)
			continue;

		pRet = pFormation->getEntityInEllipseRange(worldPos, radiusA, radiusB, filter);
		if (pRet)
			break;
	}

	return pRet;
}


Solider* Camp::getAliveSoliderInEllipse(const Point& worldPos, float radiusA, float radiusB)
{
	auto filter = [](Solider* pSolider)->bool{

		if (pSolider->checkStatus(Solider::Status::kDead))
			return false;

		return true;
	};

	Solider * pRet = nullptr;
	for (Vect_Formation::iterator it = mFormationList.begin(); it != mFormationList.end(); ++it)
	{
		FormationV2* pFormation = it->second;
		if (nullptr == pFormation)
			continue;

		pRet = pFormation->getEntityInEllipseRange(worldPos, radiusA, radiusB, filter);
		if (pRet)
			break;
	}

	return pRet;
}


void Camp::getAliveSoliderListInEllipse(const Point& worldPos, float radiusA, float radiusB, std::vector<Solider*> & outList)
{
	auto filter = [](Solider* pSolider)->bool{

		if (pSolider->checkStatus(Solider::Status::kDead))
			return false;

		return true;
	};

	for (Vect_Formation::iterator it = mFormationList.begin(); it != mFormationList.end(); ++it)
	{
		FormationV2* pFormation = it->second;
		if (nullptr == pFormation)
			continue;

		pFormation->getEntityListInEllipseRange(worldPos, radiusA, radiusB, outList, filter);
	}
}


void Camp::changeFormationTar(BaseBuild* pOld, BaseBuild* pNew)
{
	// 	Vect_Formation::iterator it = mFormationList.begin();
	// 	for (; it != mFormationList.end();++it)
	// 	{
	// 		it->second->changeTarget(pOld, pNew);
	// 	}
}


void Camp::pauseAllFormation(float time /* = FLT_MAX */)
{
	for (auto it = mFormationList.begin(); it != mFormationList.end(); ++it)
	{
		FormationV2* formation = it->second;
		if (formation)
			formation->pauseByTimer(time);
	}
}


void Camp::resumeAllFormation()
{
	for (auto it = mFormationList.begin(); it != mFormationList.end(); ++it)
	{
		FormationV2* formation = it->second;
		if (formation)
			formation->resumeByTimer();
	}
}



void Camp::pauseFormatiom(int fid, float time /* = FLT_MAX */)
{
	auto it = mFormationList.find(fid);
	if (mFormationList.end() != it)
	{
		FormationV2* formation = it->second;
		if (formation)
		{
			formation->pauseByTimer(time);
		}
	}
}


void Camp::resumeFormatiom(int fid)
{
	auto it = mFormationList.find(fid);
	if (mFormationList.end() != it)
	{
		FormationV2* formation = it->second;
		if (formation)
		{
			formation->resumeByTimer();
		}
	}
}


void Camp::freezeAllFormation(float time)
{
	for (auto it = mFormationList.begin(); it != mFormationList.end(); ++it)
	{
		FormationV2* formation = it->second;
		if (formation)
			formation->doFreeze(time);
	}
}


void Camp::setFormationMoveSpeed(float val)
{ 
	if (mFormationMoveSpeed == val)
		return;

	mFormationMoveSpeed = val;
// 	for (auto it = mFormationList.begin(); it != mFormationList.end(); ++it)
// 	{
// 		FormationV2* formation = it->second;
// 		if (formation)
// 			formation->setSpeed(mFormationMoveSpeed);
// 	}

}



float Camp::getFormationMoveSpeed() const
{
	float speed =  sgBuffManager()->getBuffMoveSpeed(getCampType(), mFormationMoveSpeed);
	return speed;
}

void Camp::addAi()
{
	if (!mAICom && getCampType() != sgBattleControl()->getOwnCamp())
	{
		mAICom = AI::create(getCampType());
	}
}

void Camp::removeAi()
{
	if (mAICom)
	{
		delete mAICom;
		mAICom = nullptr;
	}
}

void Camp::changeToNeutral(Camp* pCamp)
{
	if (pCamp == nullptr) return;



	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); it++)
	{
		it->second->s2cCamp(eCampType::ECAMP_TYPE_Neutral);
		if (pCamp != this)
			pCamp->insertBuild(it->second);
	}

	if (pCamp != this)
		mBuildList.clear();

	mLive = false;

}

void Camp::getCurAttCampList(std::set<int>& camplist)
{
	camplist.clear();

	// 	Vect_Formation::iterator it = mFormationList.begin();
	// 	for (; it != mFormationList.end(); ++it)
	// 	{
	// 		eCampType etype = it->second->getTargetCamp();
	// 
	// 		camplist.insert((int)etype);
	// 	}
}

void Camp::gradeSolider()
{
	auto campType = mCampType;
	int level = MIN(4, this->getInstituteNum());

	for (auto it = mFormationList.begin(); it != mFormationList.end(); ++it)
	{
		auto pFormation = it->second;
		if (pFormation)
			pFormation->doGradeSoliders(level);
	}
}

bool Camp::checkIsMax()
{
	if (mBuildList.size() <= 0)
		return false;

	int iMax = getCampMaxProduct();
	int iPeopel = getAllUnit();

	return iPeopel >= iMax;
}
 

FormationV2* Camp::getFormationById(int id)
{
	auto it = mFormationList.find(id);
	if (mFormationList.end() != it)
	{
		FormationV2* formation = it->second;
		if (formation)
		{
			return formation;
		}
	}

	return nullptr;
}

void Camp::addKillPeople(float num)
{
	mKillPeople += num;
}

void Camp::addProductPeople(unsigned int num)
{
	mProductPeople += num;
}

void Camp::addDeadPeople(float num)
{
	mDeadPeople += num;
}

Glyph* Camp::getGlyph(Glyph::eGlyphType type)
{
	if (mGlyphAgent)
		return mGlyphAgent->getGlyph(type);

	return nullptr;
}

void Camp::addGlyphInitAddPeople()
{
	Glyph* pGlyph = getGlyph(Glyph::E_GLYPH_PPL_INIT);
	if (pGlyph == nullptr)
		return;

	int iNum = pGlyph->getDataViceAttr();
	int fValue = pGlyph->getValueAsAddition();

	if (iNum > mBuildList.size())
		iNum = mBuildList.size();

	for (MapBulidList::iterator it = mBuildList.begin(); iNum > 0 && it != mBuildList.end(); it++)
	{
		it->second->addMinusOwnPeople(fValue);
		iNum--;
	}
}

bool Camp::checkIsAI()
{
	if (sgBattleControl()->getIsPvp())
	{
		if (sgPvpControl()->checkIsAI(mCampType))
			return true;

	}else if (mCampType == ECAMP_TYPE_AI ||
		mCampType == ECAMP_TYPE_AI2 ||
		mCampType == ECAMP_TYPE_AI3)
	{
		return true;
	}

	return false;
}

BaseBuild* Camp::getBiludByMinBuild(eBulidType buildType)
{
	MapBulidList::iterator it = mBuildList.begin();
	BaseBuild * build = NULL;
	if (it != mBuildList.end())
	{
		build = it->second;
		it++;
	}
		
	for (; it != mBuildList.end(); it++)
	{
		if (buildType == it->second->getBuildType())
		{
			if (it->second->getOwnPeople() < build->getOwnPeople())
				build = it->second;
		}
	}
	return build;
}

float Camp::getAsstAtkAddition()
{
	return mAsstAtkAddition;
}

void Camp::setAsstAtkAddition(float add)
{
	mAsstAtkAddition = add;
}

float Camp::getAsstSpeedAddition()
{
	return mAsstSpeedAddition;
}

void Camp::setAsstSpeedAddition(float add)
{
	mAsstSpeedAddition = add;
}

int Camp::getAtkLevel()		{ return mGlyphAgent->getAtkLevel(); }
int Camp::getDefLevel()		{ return mGlyphAgent->getDefLevel(); }
int Camp::getSpdLevel()		{ return mGlyphAgent->getSpdLevel(); }
int Camp::getPplLevel()		{ return mGlyphAgent->getPplLevel(); }
int Camp::getMrlLevel()		{ return mGlyphAgent->getMrlLevel(); }
