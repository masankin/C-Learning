
#include "BattleControl.h"
#include "Map/MapHandler.h"
#include "TimeCalculator.h"
#include "battle/Build.h"
#include "battle/Camp.h"
#include "HelperFunc.h"

//#include "Formation.h"
#include "Formation/FormationV2.h"

#include "game/battle/Solider.h"
#include "BuildView.h"
#include "balance/BalanceControl.h"
#include "game/ui/HpSlider.h"
#include "GamePrecedure.h"
#include "game/prop/PropsControl.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"
#include "net/UserObj.h"
#include "battle/JungleBattleManager.h"
#include "battle/MoraleManager.h"
#include "buff/BuffManager.h" 
#include "ranking/RankingControl.h"
#include "guide/GuideManager.h"
#include "battle/Glyph/GlyphBattleManager.h"
#include "EnvSet.h" 

#include "SoundManager.h"
#include "ui/dialogs/GenerlDialog.h"
#include "GamePrecedure.h"

#include "battle/BattleRecordsManager.h"
#include "lua/LuaFuncHelper.h"
#include "core/ResManager.h"
//#define __TEST_SOLDIER_COUNT__ 
BattleControl::BattleControl() :
mPercent(100), isBorn(0),
mIsPvp(false), mOwnCamp(ECAMP_TYPE_Player), m_bCanPlayWalkSound(true), mTimeElapsed(0.0f), mBattleView(NULL)
{
}

BattleControl::~BattleControl()
{
	sgMessageManager()->removeMessageHandler(this);
}

void BattleControl::cleanData()
{
	sgMessageManager()->removeMessageHandler(this);
	CC_SAFE_RELEASE_NULL(mBattleView);
	sgResManager()->removeAramaturFromeParent();
}

void BattleControl::init()
{
	m_vFormationIdlist.clear();
	if (!mBattleView)
	{
		mBattleView = BattleView::create();
		CC_SAFE_RETAIN(mBattleView);
	}
	sgMessageManager()->regisiterMessageHandler(EMT_MOVE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_TRANSFORM, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PRODUCT, this);
	sgMessageManager()->regisiterMessageHandler(EMT_UPGRADE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_FIGHT, this);
	sgMessageManager()->regisiterMessageHandler(EMT_TOWER_ATTACK, this);

	sgMessageManager()->regisiterMessageHandler(EMT_PROP_CHANGE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_CHANGEOK, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_FULLPEOP, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_FREEZE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_COOKING, this);

	sgMessageManager()->regisiterMessageHandler(EMT_JUNGLE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_MORALESHOW, this);
	sgMessageManager()->regisiterMessageHandler(EMT_REDUCEPEOPLE, this);


	sgMessageManager()->regisiterMessageHandler(EMT_ROW_DEPART, this);
	sgMessageManager()->regisiterMessageHandler(EMT_FORMATION_OVER, this);

	sgMessageManager()->regisiterMessageHandler(EMT_BUFFSHOW, this);
	sgMessageManager()->regisiterMessageHandler(EMT_GUIDE_TIPS, this);

	sgMessageManager()->regisiterMessageHandler(EMT_PROP_INVISIBILITY, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_Oil, this);

	sgMessageManager()->regisiterMessageHandler(EMT_RECORD_EXECUTE_TURRET_UPDATE, this);
}

void BattleControl::destroy()
{

}

void BattleControl::update(float dt)
{
	if (mEnableGameRun == false)
		return;

	if (!m_bCanPlayWalkSound && sgTimeCalculator()->getTimeLeft("soldier_walk_sound") == 0)
	{
		m_bCanPlayWalkSound = true;
	}

	if (isBorn)
	{
		for (CampList::iterator it = mCampList.begin(); it != mCampList.end(); ++it)
		{
			it->second->update(dt);
		}

		mCreepsMgr.update(dt);

		mGlyphBattleMgr.update(dt);

		mAsstBattleCtrl.update(dt);

		sgBalanceControl()->update();
		sgPropsControl()->update();

		if (mIsPvp && !sgPvpControl()->getIsEnd() && sgTimeCalculator()->getTimeLeft("pvp_update") == 0)
		{
			sgPvpControl()->getPvPProxy()->sendMap(mBuildList);
			sgTimeCalculator()->resetTimeCalcultor("pvp_update");
		}

		updateOil();
	}

	sgPvpControl()->update(dt);

	mBattleView->updateUI(dt);

	//更新所有建筑的updateTime函数，放在这里是为了解决攻击野怪的音效停不下来的bug
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); ++it)
	{
		it->second->updateTime(dt);
	}
	//mBattleView.setTimeLeft(sgBalanceControl()->getTimeLeft());
	mBattleView->refreshSliderPercent();

	updateCampBuffShow(); 
	 
	sgMoraleManager()->update(dt);
	sgBuffManager()->update(dt);

	//移除特效
	if (m_vFormationIdlist.size() > 0)
	{
		for (auto it = m_vFormationIdlist.begin(); it != m_vFormationIdlist.end(); ++it)
		{
			int id = *it;
			if (sgTimeCalculator()->getTimeLeft(StringUtils::format("att_ice_%d", id)) == 0)
			{
				
				FormationV2 * fomrmat=  getFormationById(id);
				if (fomrmat != nullptr)
				{
					fomrmat->setSpeedScale(1);
				}
				sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("att_ice_%d", id));
				m_vFormationIdlist.erase(it);
				break;
			}
		}
	}
	
}

const CampList& BattleControl::getCampList()
{
	return mCampList;
}

void BattleControl::loadMapData(int mapID)
{
	const CampList& camps = sgMapHandler()->getCampList();
	mCampList = camps;

	createBuildList();

	if (getIsPvp())
		setOwnCamp(sgPvpControl()->getMyCampType());

	mBattleView->load();
	mEnableGameRun = true;

	mGlyphBattleMgr.load(mapID);

	for (auto & campPair : mCampList)
	{
		auto pCamp = campPair.second;
		if (pCamp)
			pCamp->load();
	}
	
	//JungleBattleManager::getInstance()->clearJungleBattleData(mapID);
	sgMoraleManager()->clearMorale();
	sgBuffManager()->clearBuff();

	for (CampList::iterator cIt = mCampList.begin(); cIt != mCampList.end(); ++cIt)
	{
		cIt->second->addGlyphInitAddPeople();
	}

	mBattleTipsList.clear();
}

void BattleControl::doCampBuildUpgrade(int buildID)
{
	mBattleView->doCampBuildUpgrade(buildID);
}

const BuildInfo& BattleControl::getCampBuildInfo(eCampType eType, int buildID)
{
	const BaseBuild* pBuild = getCampBaseBuild(eType, buildID);
	if (!pBuild)
	{
		static BuildInfo temp;
		CCLOG("[error] BattleControl::getCampBuildInfo find build:%d failed.", buildID);
		return temp;
	}

	return pBuild->getBuildInfo();
}

const BaseBuild* BattleControl::getCampBaseBuild(eCampType eType, int buildID)
{
	CampList::iterator it = mCampList.find(eType);
	if (it == mCampList.end())
	{
		CCLOG("[error] BattleControl::getCampBaseBuild find camp:%d failed.", (int)eType);
		return NULL;
	}

	return it->second->getBuild(buildID);
}

GlyphBattleManager* BattleControl::getGlyphBattleMgr()
{
	return &mGlyphBattleMgr;
}

AsstBattleControl* BattleControl::getAsstBattleCtrl()
{
	return &mAsstBattleCtrl;
}

void BattleControl::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_MOVE)
	{
		MessageMove* pMes = dynamic_cast<MessageMove*>(message);

		doBuildSoliderMove(message);
	}
	else if (message->getMessageType() == EMT_TRANSFORM)
	{
		doBuildSoliderTrans(message);
	}
	else if (message->getMessageType() == EMT_FIGHT)
	{
		doBuildSoliderFight(message);
	}
	else if (message->getMessageType() == EMT_UPGRADE)
	{
		doBuildUpgrade(message);

	}
	else if (message->getMessageType() == EMT_TOWER_ATTACK)
	{
		doBuildAttack(message);
	}
	else if (message->getMessageType() == EMT_PROP_CHANGE)
	{
		doBuildChange(message);
	}
	else if (message->getMessageType() == EMT_PROP_CHANGEOK)
	{
		MessageBuildChangeOk* pMes = dynamic_cast<MessageBuildChangeOk*>(message);
		if (pMes)
		{
			doChangeBuildType(pMes->getBuild(), pMes->getChangeType(),pMes->getisNew());
		}
	}
	else if (EMT_PROP_FULLPEOP == message->getMessageType())
	{	
		sgSoundManager()->playMusicEffect(SOUND_PROP_FULLPEOP);
		doBuildFullPeopel(message);
	}
	else if (EMT_PROP_FREEZE == message->getMessageType())
	{
		doBuildFreeze(message);
	}
	else if (EMT_PROP_COOKING == message->getMessageType())
	{
		sgSoundManager()->playMusicEffect(SOUND_PROP_COOKING);

		MessageCooking* pMes = dynamic_cast<MessageCooking*>(message);
		if (pMes)
		{
			mBattleView->playPropAni(PROP_TYPE_COOKING, Point(pMes->x, pMes->y));
			mBattleView->playCookBottom(Point(pMes->x, pMes->y));

		}
	}
	else if (EMT_PROP_Oil == message->getMessageType())
	{

		doOil(message);
	}
	else if (EMT_PROP_INVISIBILITY == message->getMessageType())
	{
		sgSoundManager()->playMusicEffect(SOUND_PROP_INVISIBILITY);
		doInvisibility(message);
	}
	else if (EMT_JUNGLE == message->getMessageType())
	{
		doJungleMessage(message);
	}
	else if (EMT_MORALESHOW == message->getMessageType())
	{
		if (AlvinTest::alvin_testMorale() == true)
		{
			doShowMorale(message);
		}
	}
	else if (EMT_REDUCEPEOPLE == message->getMessageType())
	{
		doReducePeople(message);
	}
	else if(message->getMessageType() == EMT_BUFFSHOW)
	{
		doCampBuffShow(message);

		
	} 
	else if (EMT_ROW_DEPART == message->getMessageType())
	{
		doBuildSoliderDepart(message);
	}
	else if (EMT_FORMATION_OVER == message->getMessageType())
	{
		doFormationOver(message);
	}
	else if(EMT_GUIDE_TIPS == message->getMessageType())
	{
		MessageGuideTips* pMsg = dynamic_cast<MessageGuideTips* >(message);
		doGuideExtraAction(pMsg);
	}
	else if (EMT_RECORD_EXECUTE_TURRET_UPDATE == message->getMessageType())
	{
		doSetTurretStep(message);
	}

}

Camp* BattleControl::getTargetCamp(eCampType etype)
{
	CampList::iterator it = mCampList.find(etype);
	if (it == mCampList.end())
	{
		CCLOG("[warning] BattleControl::getTargetCamp etype:%d.", (int)etype);
		return NULL;
	}

	return it->second;
}

void BattleControl::getEnemyCamp(CampList& camplist)
{
	camplist.clear();

	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); ++it)
	{
		eCampType eType = it->second->getCampType();
		if (eType != sgBattleControl()->getOwnCamp() && eType != ECAMP_TYPE_Neutral)
			camplist.insert(std::make_pair(eType, it->second));
	}
}

void BattleControl::getEnemyCamp(CampList& camplist, eCampType campType)
{
	camplist.clear();

	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); ++it)
	{
		eCampType eType = it->second->getCampType();
		if (eType != campType && eType != ECAMP_TYPE_Neutral)
			camplist.insert(std::make_pair(eType, it->second));
	}
}



FormationV2 * BattleControl::getEnemyMaxPeopleFormation(eCampType campType)
{
	CampList campList;
	sgBattleControl()->getEnemyCamp(campList, campType);

	if (campList.empty())
		return NULL;

	FormationV2 *temp = NULL;
	for (auto it : campList)
	{
		Vect_Formation formationList = it.second->getFormationList();
		if (formationList.empty())
			continue;

		auto formationIt = formationList.begin();
		if (formationIt != formationList.end())
		{
			temp = formationIt->second;
			formationIt++;
		}

		for (; formationIt != formationList.end(); formationIt++)
		{
			if (formationIt->second->getAliveEntityCount() > temp->getAliveEntityCount())
				temp = formationIt->second;
		}
	}
	return temp;
}


bool BattleControl::getHasNeutralCamp()
{
	CampList::iterator it = mCampList.find(ECAMP_TYPE_Neutral);
	if (it == mCampList.end())
		return false;
	return true;
}

int BattleControl::getValidCampSize()
{
	int iRet = 0;
	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); ++it)
	{
		eCampType ecamp = it->second->getCampType();
		if (ecamp != ECAMP_TYPE_Neutral && ecamp != ECAMP_TYPE_Creeps)
			iRet++;
	}
	return iRet;
}

void BattleControl::createBuildList()
{
	CCLOG("[trace] BattleControl::createBuildList.");

	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); ++it)
	{
		const MapBulidList& buildlist = it->second->getBuildList();

		MapBulidList::const_iterator bt = buildlist.begin();
		for (; bt != buildlist.end(); ++bt)
		{
			mBuildList.insert(std::make_pair(bt->first, bt->second));
		}
	}
}

BaseBuild* BattleControl::getTargetBuild(int key)
{
	MapBulidList::iterator it = mBuildList.find(key);
	if (it == mBuildList.end())
	{
		CCLOG("[warn] BattleControl::getTargetBuild key:%d!", key);
		return NULL;
	}
	return it->second;
}

BaseBuild* BattleControl::getTargetBuildByIns(int insKey)
{
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); ++it)
	{
		if (it->second->getBuildInfo().insID == insKey)
			return it->second;
	}
	CCLOG("[error]getTargetBuildByIns no build insKey:%d!", insKey);
	return NULL;
}

BaseBuild* BattleControl::getNearestBuild(int key, bool bEnemy)
{
	BaseBuild* pOwnBuild = getTargetBuild(key);
	if (!pOwnBuild)
	{
		CCLOG("[error]getNearestBuild key:%d!", key);
		return nullptr;
	}

	eCampType eOwnCamp = pOwnBuild->getCamp();
	Vec2 OwnPt = pOwnBuild->boundingBox().origin;

	int tempDis = TILED_SIZE * 80;
	BaseBuild* pRet = NULL;
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); ++it)
	{
		if (bEnemy)
		{
			if (it->second->getCamp() == eOwnCamp)
				continue;
		}
		else{
			if (it->second->getCamp() != eOwnCamp)
				continue;
		}

		int dis = it->second->boundingBox().origin.getDistance(OwnPt);
		if (dis < tempDis)
		{
			tempDis = dis;
			pRet = it->second;
		}
	}

	if (pRet)
		CCLOG("[trace]getNearestBuild find build:%d nearest to build:%d, relation:%d!", pRet->getKeyID(), key, bEnemy == true ? 1 : 0);
	else
		CCLOG("[trace]getNearestBuild find no build nearest to build:%d, relation:%d!", key, bEnemy == true ? 1 : 0);
	return pRet;
}

BaseBuild* BattleControl::getTargetBuild(Vec2 pt)
{
	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); ++it)
	{
		if (it->first == ECAMP_TYPE_Creeps)
		{
			continue;
		}
		BaseBuild* pBuild = it->second->getBuildByPos(pt);
		if (pBuild)
			return pBuild;
	}

	CCLOG("[error] BattleControl::getTargetBuild pt:(%f, %f)!", pt.x, pt.y);
	return NULL;
}

BaseBuild* BattleControl::getTargetBuildTouch(Vec2 pt)
{
	BaseBuild* pBuild = nullptr;
	float length = 150;
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); ++it)
	{
		Vec2 m_pos = pt - it->second->getPosition();
		float len = m_pos.length();

		if (len < 150 && len < length
			&& it->second->isEnable()
			/*&& it->second->getBuildInfo().locationID != 0*/)
		{
			length = len;
			pBuild = it->second;
		}
	}

	return pBuild;
}

int BattleControl::getCampInstituteNum(eCampType eType)
{
	int total = 0;
	CampList::iterator it = mCampList.find(eType);
	if (it == mCampList.end())
	{
		total += it->second->getInstituteNum();
	}
	return total;
}

void BattleControl::doBuildSoliderMove(GameMessage* message)
{
	MessageMove* pMes = dynamic_cast<MessageMove*>(message);
	if (nullptr == pMes)
		return;

	BaseBuild* pSrcBuild = getTargetBuild(pMes->getFromBuild());
	BaseBuild* pDstBuild = getTargetBuild(pMes->getToBuild());
	if (nullptr == pSrcBuild || nullptr == pDstBuild)
		return;

	if (pDstBuild->getInviInfo() != nullptr && !pDstBuild->getInviInfo()->mObtain)
		return;
	if (pSrcBuild->getInviInfo() != nullptr && !pSrcBuild->getInviInfo()->mSend)
		return;

	//CampList::iterator cIt = mCampList.find(static_cast<eCampType>(pSrcBuild->getCamp()));
	//if (cIt == mCampList.end())
	//	return;

	Camp * pSrcCamp = this->getTargetCamp(static_cast<eCampType>(pSrcBuild->getCamp()));
	Camp * pDstCamp = this->getTargetCamp(static_cast<eCampType>(pDstBuild->getCamp()));
	if (nullptr == pSrcCamp || nullptr == pDstCamp)
		return;

	//if (pSrcCamp->getCampType() != this->getOwnCamp())
	//	return;

	Vec2 posSrc = pSrcBuild->getPosition();
	Vec2 posDst = pDstBuild->getPosition();

	MapObject* pSrcMapObj = pSrcBuild->getBuildInfo().mapObj;
	MapObject* pDstMapObj = pDstBuild->getBuildInfo().mapObj;
	if (nullptr == pSrcMapObj || nullptr == pDstMapObj)
		return;


	int soliderCount = 0;
	if (pMes->getNumType() == 0)
		soliderCount = (int)(pSrcBuild->getOwnPeople() *  pMes->getNumValue() / 100.0f);
	else
		soliderCount = pMes->getNumValue();

	if (soliderCount <= 0)
		return;

	bool hasFormation = false;
	auto formationID = pSrcBuild->getOnWayFormation(pDstBuild->getKeyID());
	FormationV2* pFormation = nullptr;
	if (formationID != 0)
	{
		pFormation = this->getFormationById(formationID);
		if (pFormation && soliderCount > 0)
		{
			pFormation->addExpectedCount(soliderCount);
			hasFormation = true;
		}
	}
	else
	{
		formationID = pMes->getFormationId();
		pFormation = FormationV2::create(formationID, soliderCount, pSrcMapObj->getUid(), pDstMapObj->getUid());
		if (pFormation)
		{
			pSrcBuild->addSendingFormation(pDstBuild->getKeyID(), pFormation->getId());
			pSrcCamp->pushFormation(pFormation);

			float basicSpeed = pSrcBuild->getMoveSpeed() + pSrcCamp->getAsstSpeedAddition();
			float speed = basicSpeed + pSrcCamp->getAsstSpeedAddition();

			Glyph* pGlyph = nullptr;
			pGlyph = pSrcCamp->getGlyph(Glyph::E_GLYPH_SPD_MOVE);
			if (nullptr != pGlyph)
			{
				speed += pGlyph->getValueAsAddition();
			}

			pGlyph = pSrcCamp->getGlyph(Glyph::E_GLYPH_SPD_CREEPS);
			if (nullptr != pGlyph)
			{
				if (EBULID_TYPE_CREEPS == pDstBuild->getBuildType())
					speed += pGlyph->getValueAsAddition();
			}

			pGlyph = pSrcCamp->getGlyph(Glyph::E_GLYPH_SPD_ASSAULT);
			if (nullptr != pGlyph)
			{
				//CampList campList;
				//int EnemyCa
				//this->getEnemyCamp(campList, pSrcBuild->getCamp());

				//if (pSrcBuild->getCamp() != pDstBuild->getCamp() && pDstCamp->getAllUnit() < pGlyph->())
				//	speed += pGlyph->getValueAsAddition();
			}

			pGlyph = pSrcCamp->getGlyph(Glyph::E_GLYPH_SPD_ASSIST);
			if (nullptr != pGlyph)
			{
				//if (pSrcBuild->getCamp() == pDstBuild->getCamp() && pSrcCamp->getAllUnit() < pGlyph->())
				//	speed += pGlyph->getValueAsAddition();
			}

			pFormation->setBaseSpeed(speed);

			if (pSrcBuild->getBuildType() == EBUILD_TYPE_STABLES)
			{
				pFormation->setAttribType(FormationV2::AttribType::Speed);
			}
			else
			{
				pFormation->setAttribType(FormationV2::AttribType::Normal);
			}

			pFormation->setScrBuildUid(pSrcBuild->getKeyID());
			pFormation->setDstBuildUid(pDstBuild->getKeyID());

			//CCLOG("[Leon]BattleControl::doBuildSoliderMove : [id:%d] Src-%d, Dst-%d", pFormation->getId(), pSrcBuild->getCamp(), pDstBuild->getCamp());

			//pFormation->setSrcCamp(pSrcBuild->getCamp());
			//pFormation->setDstCamp(pDstBuild->getCamp());

			pFormation->setSrcCamp(pSrcBuild->getCamp());
			pFormation->setDstCamp(pDstBuild->getCamp());

			pFormation->enterView(mBattleView, EBVZ_FORMATION);

			eCampType owncamp = this->getOwnCamp();
			if (pDstBuild->getCamp() == owncamp
				&& pSrcBuild->getCamp() != owncamp)
				pDstBuild->getBuildView()->callSOS(true);

			if (pDstBuild->getBuildType() == EBULID_TYPE_CREEPS)
			{
				CreepsBuild * pCreepsBuild = dynamic_cast<CreepsBuild*>(pDstBuild);
				if (pCreepsBuild)
					pCreepsBuild->recordOnWayFormation(formationID, pSrcCamp->getCampType(), pSrcBuild->getKeyID());
			}

			ActionStatus*  pStatu = pSrcBuild->getActionStatu();
			if (pStatu)
			{
				pStatu->reset();
			}
		}
	}


	if (pFormation)
	{
		if (pSrcCamp->getCampType() == this->getOwnCamp())
		{
			if (pSrcBuild->getBuildType() == EBUILD_TYPE_STABLES)	//马厩
			{
				//随机3种马厩出发音效
				int soundIndex = cocos2d::random(0, 4);
				if (soundIndex >= 3)
				{
					soundIndex = 0;
				}

				sgSoundManager()->delayPlayEffect(eMusicType(SOUND_SOLDIER_STABLES_WALK_1 + soundIndex), 0.5);
			}
			else
			{
				//随机3种出发音效
				int soundIndex = cocos2d::random(0, 4);
				if (soundIndex >= 3)
				{
					soundIndex = 0;
				}

				sgSoundManager()->delayPlayEffect(eMusicType(SOUND_SOLDIER_WALK_1 + soundIndex), 0.5);
			}
		}

		//播动画
		if (pSrcBuild->getBuildType() == EBULID_TYPE_BARRACK)
		{
			if (posSrc.x > posDst.x)
				pSrcBuild->getBuildView()->playAni(BUILD_ANI_OUTPUT_STAR);
			else
				pSrcBuild->getBuildView()->playAni(BUILD_ANI_OUTPUT_STAR_RIGHT);
		}


		if (!pMes->getIsNet() && mIsPvp && pFormation)
		{
			sgPvpControl()->getPvPProxy()->sendFight(pMes->getFromBuild(), pMes->getToBuild(), soliderCount, formationID);
			if (!hasFormation)
				pFormation->pauseByTimer(PVP_FRO_WAIT_ITEM); 
		}
		doAttackGuide(pSrcBuild, pDstBuild);
	}
}
 
void BattleControl::doGuideExtraAction(MessageGuideTips* pMsg)
{  
	if (pMsg->getFunctionID() == E_GuideFuc_Drag
		|| pMsg->getFunctionID() == E_GuideFuc_GP2BV)
	{
		int loc = 0;
		if (pMsg->getFunctionParam() == E_GUIDELOC_USEPROP)
			mBattleView->activeGuide(pMsg->getFunctionParam());
		else
			mBattleView->activeGuide();
	} 
	
}

void BattleControl::doAttackGuide(BaseBuild* pSrcBuild, BaseBuild* pDstBuild)
{ 
	eGuideLocationType toBattleViewLoc = E_GUIDELOC_BEGIN;
	eGuideLocationType toGamePageLoc = E_GUIDELOC_BEGIN;
	float activeTime = 1.0f;
	auto scheduler = Director::getInstance()->getScheduler();

	//玩家遭到敌人进攻触发相关的引导
	if (pSrcBuild->getCamp() == ECAMP_TYPE_AI && pDstBuild->getCamp() == ECAMP_TYPE_Player)
	{
		toBattleViewLoc = E_GUIDELOC_E_ATK_P1;
		toGamePageLoc = E_GUIDELOC_E_ATK_P2;
		activeTime = 2.0f;
	}
	//中立遭到玩家进攻触发相关的引导
	else if (pSrcBuild->getCamp() == ECAMP_TYPE_Player && pDstBuild->getCamp() == ECAMP_TYPE_Neutral)
	{
		toBattleViewLoc = E_GUIDELOC_P_ATK_N; 
		activeTime = 2.0f;
	}
	//中立遭到敌人进攻触发相关的引导
	else if (pSrcBuild->getCamp() == ECAMP_TYPE_AI && pDstBuild->getCamp() == ECAMP_TYPE_Neutral)
	{
		toBattleViewLoc = E_GUIDELOC_E_ATK_N;
		activeTime = 2.0f;
	}
	//玩家防守自己建筑触发相关的引导
	else if (pSrcBuild->getCamp() == ECAMP_TYPE_Player && pDstBuild->getCamp() == ECAMP_TYPE_Player)
	{
		toBattleViewLoc = E_GUIDELOC_P_ASS_P;
		activeTime = 2.0f;
	}
	//敌人遭到玩家进攻触发相关的引导
	else if (pSrcBuild->getCamp() == ECAMP_TYPE_Player && pDstBuild->getCamp() == ECAMP_TYPE_AI)
	{
		toBattleViewLoc = E_GUIDELOC_P_ATK_E;
		activeTime = 2.0f;
	}

	if (toBattleViewLoc != E_GUIDELOC_BEGIN)
	{ 
		if (!sgGuideManager()->checkGuideTypeFinished(toBattleViewLoc))
		{ 
			scheduler->schedule(std::bind(&BattleControl::delayActiveGuide, this,
				std::placeholders::_1, (int)toBattleViewLoc),
				this, activeTime, 0, 0.0f, false,
				"BattleControl::delayActiveGuide");
		}
		
	}
	if (toGamePageLoc != E_GUIDELOC_BEGIN)
	{
		if (!sgGuideManager()->checkGuideTypeFinished(toGamePageLoc))
		{
			scheduler->schedule(std::bind(&BattleControl::delayActiveGamePageGuide, this,
				std::placeholders::_1, (int)toGamePageLoc),
				this, activeTime, 0, 0.0f, false,
				"BattleControl::delayActiveGamePageGuide");
		} 
		
	}
}

void BattleControl::delayActiveGuide(float dt, int location)
{
	mBattleView->activeGuide(location);
}

void BattleControl::delayActiveGamePageGuide(float dt, int location)
{
	sgMessageManager()->sendMessage(EMT_GAMESTART, StringUtils::format("%d", location));
}

void BattleControl::doSetTurretStep(GameMessage * message)
{
	MessageRecordExecuteTurretUpdate * pMes = dynamic_cast<MessageRecordExecuteTurretUpdate*>(message);

	Turret* pBuild = dynamic_cast<Turret*>(sgBattleControl()->getTargetBuild(pMes->getBuildId()));

	if (pBuild)
	{
		pBuild->setStep(pMes->getStep());
		pBuild->getBuildView()->showChooseLevel(false);
		pBuild->setIsleveling(true);
		pBuild->Upgrade(true);
	}
}

void BattleControl::doBuildUpgrade(GameMessage* message)
{

	MessageUpgrade* pMes = dynamic_cast<MessageUpgrade*>(message);
	if (pMes)
	{

		MapBulidList::iterator it = mBuildList.find(pMes->getBuild());
		if (it != mBuildList.end())
		{
			it->second->doUpgrade();
		}
	}
}

void BattleControl::doBuildSoliderFight(GameMessage* message)
{
	MessageFight* pMes = dynamic_cast<MessageFight*>(message);
	if (pMes)
	{
		BaseBuild* pDestBuild = getTargetBuild(pMes->getBuild());
		BaseBuild* pSrcBuild = getTargetBuild(pMes->getBuild2());

		if (nullptr == pDestBuild || nullptr == pSrcBuild)
		{
			CCLOG("[error] BattleControl::doBuildSoliderFight : build can't be found. Def[%d] : %p, Atk[%d] : %p", pMes->getBuild(), pDestBuild, pMes->getBuild2(), pSrcBuild);
			return;
		}

		Camp * pAtkCamp = nullptr;
		Camp * pDefCamp = nullptr;

		CampList::iterator attackIt = mCampList.find(pMes->getAttactCamp());
		if (mCampList.end() != attackIt)
			pAtkCamp = attackIt->second;

		CampList::iterator defenseIt = mCampList.find(pMes->getDefenseCamp());
		if (mCampList.end() != defenseIt)
			pDefCamp = defenseIt->second;

		if (nullptr == pAtkCamp || nullptr == pDefCamp)
		{
			CCLOG("[error] BattleControl::doBuildSoliderFight : Camp can't be found. Def : %d, Atk : %d", pDefCamp->getCampType(), pAtkCamp->getCampType());
			return;
		}


		if (pDestBuild->mBeHurtSoundId == 0)
		{
			pDestBuild->mBeHurtSoundId = sgSoundManager()->playMusicEffect(SOUND_SOLDIER_ATTACK, true);
			pDestBuild->mElapsedTime = 0.0f;
		}

		bool bRet = false;
		if (pDestBuild->getBuildType() != EBULID_TYPE_CREEPS)
		{
			if (pDestBuild->doFight(pAtkCamp->getCampType(), pMes->getAttactNum()) == true)
			{
				if (pAtkCamp->isHasBuild())
				{
					pAtkCamp->occupyBuild(pDestBuild, true);
					pDefCamp->removeBuild(pDestBuild->getKeyID());
				}
				else
				{
					Camp* pNeutral = getTargetCamp(ECAMP_TYPE_Neutral);
					if (pNeutral)
					{
						pNeutral->occupyBuild(pDestBuild, true);
					}
					if (pDefCamp->getCampType() != ECAMP_TYPE_Neutral)
					{
						pDefCamp->removeBuild(pDestBuild->getKeyID());
					}
				}
				

				if (pDestBuild->getBuildType() == EBULID_TYPE_INSISTUTE)
				{
					pDefCamp->gradeSolider();
					pAtkCamp->gradeSolider();
				} 
				bRet = true;

				char szParam[64] = { 0 }; 
				sprintf(szParam, "%d %d %d", pDestBuild->getKeyID(), (int)pDefCamp->getCampType(), (int)pAtkCamp->getCampType());
				sgMessageManager()->sendMessage(EMT_BUILD_DESTORY, szParam);

				sgMoraleManager()->registMoraleMessage(pAtkCamp->getCampType(), pDestBuild->getOccupyBuildMoraleValue(), E_MORALECOMPUTE_NORMAL, pDestBuild->getKeyID());
				sgMoraleManager()->registMoraleMessage(pDefCamp->getCampType(), -1 * pSrcBuild->getLostBuildMoraleValue(), E_MORALECOMPUTE_NORMAL, pSrcBuild->getKeyID());
 
				getBattleView()->activeGuide(E_GUIDELOC_OCU2);
			} 

			pDestBuild->getBuildView()->playAni(BUILD_ANI_BEHURT);
		}
		else
		{
			CreepsBuild* pCreepsBuild = dynamic_cast<CreepsBuild*>(pDestBuild);
			if (pCreepsBuild && pCreepsBuild->doDefend(pAtkCamp->getCampType(), pDefCamp->getCampType(), pMes->getAttactNum(), pMes->getFormationID()))
			{
				if (pCreepsBuild->checkOccupy())
				{
					int buffID = pCreepsBuild->takeBuffID();

					// do buff
					sgMessageManager()->sendMessage(EMT_BUFF, StringUtils::format("%d %d %d", buffID, pAtkCamp->getCampType(), pCreepsBuild->getBuildInfo().insID));

					char szParam[64] = { 0 }; 
					sprintf(szParam, "%d %d %d", pDestBuild->getKeyID(), (int)pDefCamp->getCampType(), (int)pAtkCamp->getCampType());
					sgMessageManager()->sendMessage(EMT_BUILD_DESTORY, szParam);

					sgMoraleManager()->registMoraleMessage(pAtkCamp->getCampType(), pDestBuild->getOccupyBuildMoraleValue(), E_MORALECOMPUTE_NORMAL, pDestBuild->getKeyID()); 

					pCreepsBuild->doOccupy();
				}
				bRet = true;
			}

			if (pDestBuild->isEnable())
				pDestBuild->getBuildView()->playAni(BUILD_ANI_BEHURT);
		}

		if (bRet)
		{


		}

	}
}

void BattleControl::doBuildSoliderTrans(GameMessage* message)
{
	MessageTransform* pMes = dynamic_cast<MessageTransform*>(message);
	if (pMes)
	{
		BaseBuild* pBuild = getTargetBuild(pMes->getBuild());
		BaseBuild* pBuild2 = getTargetBuild(pMes->getBuild2());
		if (pBuild)
		 
		{  
			pBuild->getIntoBuild(pBuild2->getBuildType(), pBuild2->getKeyID(), pMes->getTransformNum());

			ActionStatus* pStatu = pBuild->getActionStatu();
			if (pStatu)
			{
				pStatu->change(pBuild->getCamp(), pBuild->getOwnPeople());
			}

			if (!sgGuideManager()->checkGuideTypeFinished(E_GUIDELOC_SOLDIERNUM) && pBuild->getCamp() == ECAMP_TYPE_Player)
			{
				mBattleView->activeGuide(E_GUIDELOC_SOLDIERNUM, pBuild->getKeyID(), pBuild->getOwnPeople());
			}
		}
	}
}

void BattleControl::doChangeBuildType(int buildId, eBulidType type,bool isNew)
{
	BaseBuild* pBuild = getTargetBuild(buildId);
	if (pBuild)
	{

		BaseBuild* pNewBuild = pBuild->copyWithType(type);
		if (!pNewBuild)
			return;

		CampList::iterator it = mCampList.find(pBuild->getCamp());
		if (it != mCampList.end())
		{
			it->second->removeBuild(pBuild->getKeyID(),false);
			it->second->occupyBuild(pNewBuild, false);

			if (pNewBuild->getBuildType() == EBULID_TYPE_INSISTUTE)
			{
				it->second->gradeSolider();
			}
		}

		MapBulidList::iterator bIt = mBuildList.find(pBuild->getKeyID());
		if (bIt != mBuildList.end())
		{
			bIt->second = pNewBuild;
		}

		it = mCampList.begin();
		for (; it != mCampList.end(); it++)
		{
			it->second->changeFormationTar(pBuild, pNewBuild);
		}

		pNewBuild->setDirtyTag(EBDIR_CAMEP);
		pNewBuild->setDirtyTag(EBDIR_TYPE);
		pBuild->getBuildView()->playAni(BUILD_ANI_EXCHANGE_END);
		pNewBuild->getBuildView()->resetBubbleNode(pNewBuild);

		delete pBuild;

		mBattleView->onTouchCancelled(nullptr, nullptr);
	}
}

void BattleControl::doBuildFullPeopel(GameMessage* message)
{
	MessageFullPeopel* pMes = dynamic_cast<MessageFullPeopel*>(message);
	if (pMes)
	{

		BaseBuild* pBuild = getTargetBuild(pMes->getBuildId());
		if (pBuild && pBuild->getCamp() == pMes->getCamp())
		{
			if (pBuild->doFullPeopel())
			{
				pBuild->getBuildView()->addAddPeopel();// playAni(BUILD_ANI_ADDPEOPOL);
				if (pMes->getCamp() == getOwnCamp())
				{
					sgPropsControl()->setCdTime(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_ADDPEOPEL));
				}
			}

		}
		//else if (pBuild && pBuild->getCamp() == ECAMP_TYPE_Creeps)
		//{
		//	if (pBuild->doFullPeopel())
		//	{
		//		pBuild->getBuildView()->playAni(BUILD_ANI_ADDPEOPOL);
		//		sgPropsControl()->setCdTime(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_ADDPEOPEL));
		//	}
		//	else
		//	{
		//		if (pMes->getCamp() == getOwnCamp())
		//			sgPropsControl()->showNoticeText(sgTextHelper(TEXT_COMMTIPS_PROP_TEXT1));
		//	}
		//}
	}
}

void BattleControl::doBuildFreeze(GameMessage* message)
{
	int freezeNum = 0;
	for (CampList::iterator campIt = mCampList.begin(); campIt != mCampList.end(); campIt++)
	{
		Camp* pCamp = campIt->second;
		if (nullptr == pCamp)
			continue;

		float time = 0;

		const PropInfo * pInfo = sgTableManager()->getPropInfoById(sgPropsControl()->getCurPropId());
		if (pInfo)
		{
			time = pInfo->duration;
			//pCamp->pauseAllFormation(time);
			pCamp->freezeAllFormation(time);
		}
		else
		{
			time = 0.3f;
			//pCamp->pauseAllFormation(0.3f);
			pCamp->freezeAllFormation(time);
		}


		const Vect_Formation& formationList = pCamp->getFormationList();
		for (auto formationIt = formationList.begin(); formationIt != formationList.end(); ++formationIt)
		{
			FormationV2* formation = formationIt->second;
			if (nullptr == formation)
				continue;

			if (formation->getSrcCamp() != getOwnCamp())
			{
				freezeNum += formation->getAliveEntityCount();
			}

			mBattleView->playFreezeAni(formation->getPosition(), time);
		}
	}

	if (mIsPvp)
	{
		mBattleView->playPropAni(PROP_TYPE_FREEZE);
	}

	auto pScoreInfo = sgTableManager()->getScoreInfo(eBalanceScore::SCORE_FREEZE);
	if (pScoreInfo)
	{
		int score = pScoreInfo->fightScore * freezeNum;
		sgBalanceControl()->addScore(eBalanceScore::SCORE_FREEZE, score);
	}
}

void BattleControl::doCallCook()
{
	auto  pInfo = sgTableManager()->getPropInfoById(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_COOKING));
	
	Vec2 pos = sgPropsControl()->getTouchPos();
	std::vector<Solider*> soliderList;
	for (CampList::iterator it = mCampList.begin(); it != mCampList.end(); it++)
	{
		Camp* pCamp = it->second;
		if (nullptr == pCamp)
			continue;
		//pCamp->removeSoliderWithRect(Rect(pMes->x - 100, pMes->y - 100, 200, 200));
		pCamp->getAliveSoliderListInEllipse(pos, 200.0f, 200.0f, soliderList);
	}

	std::random_shuffle(soliderList.begin(), soliderList.end());

	const int min = (int)pInfo->duration / 100;
	const int max = (int)pInfo->duration % 100;

	int rand = cocos2d::random(min, max) * 0.01f * soliderList.size();
	int enemyNum = 0;

	for (int i = 0; i < soliderList.size();i++)  
	{
		if (i <= rand)
		{
			Solider* pSolider = soliderList.at(i);
			if (pSolider)
			{
				pSolider->doDead();
				if (pSolider->getSrcCamp() != getOwnCamp())
				{
					enemyNum++;
				}
			}
		}
	}

	auto pMove1 = MoveBy::create(0.03, Vec2(20, 0));
	auto pMove2 = MoveBy::create(0.03, Vec2(0, 20));

	auto pAction = Spawn::create(Sequence::createWithTwoActions(pMove1, pMove1->reverse()),
		Sequence::createWithTwoActions(pMove2, pMove2->reverse()), nullptr);

	mBattleView->runAction(Repeat::create(pAction, 10));

	auto pScoreInfo = sgTableManager()->getScoreInfo(eBalanceScore::SCORE_BOMB);
	if (pInfo)
	{
		int score = pScoreInfo->fightScore * enemyNum;
		sgBalanceControl()->addScore(eBalanceScore::SCORE_BOMB, score);
	}

	//mBattleView.playCookBottom(pos);
}

void BattleControl::doInvisibility(GameMessage* message)
{
	MessageInvisibility* pMes = dynamic_cast<MessageInvisibility*>(message);
	if (pMes)
	{
		eCampType useCamp = (eCampType)pMes->camp;
		BaseBuild* pBuild = getTargetBuild(pMes->buildId);
		const PropInfo*  pInfo = sgTableManager()->getPropInfoById(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_INVISIBILITY_CLOAK));

		if (pBuild && pInfo)
		{
			
			InvisibilityInfo* pInviInfo = new InvisibilityInfo();
			pInviInfo->formatParams(pInfo->params);
			pBuild->setInviInfo(pInviInfo);

			pBuild->getBuildView()->playInvisibilityEffect();
			sgTimeCalculator()->createTimeCalcultor(StringUtils::format("build_Invisibility_%d", pBuild->getKeyID()), pInfo->duration);

			if (useCamp == sgBattleControl()->getOwnCamp())
			{
				sgPropsControl()->setCdTime(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_INVISIBILITY_CLOAK));
			}
		}


	}

}

void BattleControl::doOil(GameMessage* message)
{
	MessageOil* pMes = dynamic_cast<MessageOil*>(message);
	if (pMes)
	{
		const PropInfo* pInfo = sgTableManager()->getPropInfoById(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_CHAIN_OIL));

		if (pInfo)
		{
			OilPoint point;
			point.mId = pMes->camp;
			point.mPos = Vec2(pMes->x, pMes->y);
			std::vector<int> range = HelperFunc::tokenize_2int(pInfo->range, "+");
			if (range.size() >= 2)
			{
				point.mRadiusA = range.at(0);
				point.mRadiusB = range.at(1);
			}
			point.mSpeed = atoi(pInfo->params.c_str()) * 0.01;
			pushOilPoint(point);

			mBattleView->addOilEffect(point.mId, point.mPos);
			sgTimeCalculator()->createTimeCalcultor(StringUtils::format("oil_%d",point.mId), pInfo->duration);
		}
	}
}

void BattleControl::doGamePause(bool stop)
{ 
	if (!sgGamePrecedure()->getEnterGame())
		return;
	if (stop)
		sgSoundManager()->stopAllMusicEffects();
	sgGamePrecedure()->enableGameRunning(!stop);
	mEnableGameRun = !stop;

	std::string str = (stop == true)?"1":"0";
	sgMessageManager()->sendMessage(EMT_LITTLETIPS, str);
}

void BattleControl::doJungleMessage(GameMessage* message)
{
	MessageJungle* tMessage = dynamic_cast<MessageJungle*>(message);
	if (tMessage->getState() == E_JUNGLE_CREATE)
	{
		createJungleBuild(tMessage->getSrcID(), tMessage->getJungleBuildID(), tMessage->getJungleLocationID(), tMessage->getSrcCamp());
	}
	else if (tMessage->getState() == E_JUNGLE_MOVE)
	{
		returnFromJungleBuild(tMessage->getSrcID(), tMessage->getDestID());
	}
	else if (tMessage->getState() == E_JUNGLE_CLOSE)
	{
		closeJungleBuild(tMessage->getSrcID(), tMessage->getSrcCamp(), tMessage->getDestCamp(), tMessage->getJungleBuildID(), tMessage->getJungleLocationID());
	}
}

void BattleControl::doShowMorale(GameMessage* message)
{
	MessageMoraleShow* tMessage = dynamic_cast<MessageMoraleShow*>(message);
	if (NULL != tMessage)
	{
		mBattleView->showMorale((eCampType)tMessage->getCampType(), tMessage->getMoraleValue());
	}
}

void BattleControl::doReducePeople(GameMessage* message)
{
	MessageReducePeople* tMessage = dynamic_cast<MessageReducePeople*>(message);
	if (NULL != tMessage)
	{
		int tBuildID = tMessage->m_BuildID;
		int tNum = tMessage->m_Num;
		eCampType tType = (eCampType)tMessage->m_Camp;
		MapBulidList::iterator tIter = mBuildList.find(tBuildID);
		if (tIter != mBuildList.end())
		{
			BaseBuild* tBuild = tIter->second;
			tBuild->addMinusOwnPeople(tNum);
		}
	}
}


void BattleControl::doCampBuffShow(GameMessage* message)
{ 
	MessageBuffShow* pMsg = dynamic_cast<MessageBuffShow* >(message);
	eCampType camp = (eCampType)pMsg->getCampType();
	eBuffType buffType = (eBuffType)pMsg->getID();
	int buffSrc = pMsg->getBuffSrc();
	float time = pMsg->getCDTime();
	startCampBuffShow(camp, buffType, time);
	if(buffSrc != 0 && buffSrc != -1)
	{
		addBuffTips(camp, buffType, buffSrc);
	}
}



//用于对士兵绕开障碍物
void BattleControl::CheckAndAddBuild(Solider* soldier, BaseBuild* srcBuild, BaseBuild* tarBuild, float speed)
{
	Vec2 soldierPos = soldier->getPosition();
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); it++)
	{
		BaseBuild* pBuild = it->second;
		if (nullptr == pBuild)
			continue;

		if (nullptr == pBuild->getBuildInfo().mapObj)
			continue;


		Vec2 buildPos = (pBuild->getBuildInfo().mapObj)->getPosition();
		Size buildSize = (pBuild->getBuildInfo().mapObj)->getSize();

		float distanceSq = soldierPos.getDistanceSq(buildPos);
		float defenceRange = (buildSize.width * buildSize.width + buildSize.height * buildSize.height) / 4;
		if (pBuild != srcBuild
			&& pBuild != tarBuild
			&& distanceSq <= defenceRange
			/*&& pBuild->getBuildInfo().locationID != 0*/)
		{
			soldier->addWillCheckBuild(it->second);
			//sgMapWorld()->drawLine(Vec2(0,0),Vec2(ft->second->getBuildInfo().x,ft->second->getBuildInfo().y),Color4F::RED);
		}

	}

	soldier->addF(speed);
	soldier->checkSrcAndTarBuild(srcBuild, tarBuild, speed);
}

bool BattleControl::checkBuildBeAttack(int buildKey, eCampType campType)
{

	for (CampList::iterator it = mCampList.begin(); it != mCampList.end(); ++it)
	{
		Camp* pCamp = it->second;
		if (nullptr == pCamp)
			continue;

		if (pCamp->getCampType() == campType)
			continue;

		const Vect_Formation& formations = it->second->getFormationList();

		for (auto itr = formations.begin(); itr != formations.end(); ++itr)
		{
			auto pFormation = itr->second;
			if (nullptr == pFormation)
				continue;

			if (buildKey == pFormation->getDstBuildUid())
				return true;
		}
	}
	return false;
}

void BattleControl::getTargetBuildList(eCampType eType, MapBulidList& buildList)
{
	buildList.clear();
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); ++it)
	{
		if (it->second->getCamp() == eType)
		{
			buildList.insert(std::make_pair(it->second->getKeyID(), it->second));
		}
	}
}

void BattleControl::getTargetActivedBuildList(eCampType eType, MapBulidList& buildList)
{
	buildList.clear();
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); ++it)
	{
		if (it->second->getCamp() == eType
			/*&& it->second->getBuildInfo().locationID != 0*/
			&& it->second->isEnable() == true)
		{
			buildList.insert(std::make_pair(it->second->getKeyID(), it->second));
		}
	}
}

void BattleControl::doBuildAttack(GameMessage* message)
{
	MessageBuildAttact* pMes = dynamic_cast<MessageBuildAttact*>(message);
	if (nullptr == pMes)
		return;

	BaseBuild* pAtkBuild = getTargetBuild(pMes->getBuild());
	if (nullptr == pAtkBuild)
		return;

	if (pAtkBuild->getBuildType() != EBULID_TYPE_TURRET)
		return;

	const BuildAttr* pBuildAttr = sgTableManager()->getBuildAttr(pAtkBuild->getAttrID());
	if (nullptr == pBuildAttr)
		return;

	Size winSize = CCDirector::getInstance()->getWinSize();
	float rangeWidth = pBuildAttr->attRadius;
	float rangeHeight = rangeWidth * winSize.height / winSize.width;

	const Point& atkPos = pAtkBuild->getPosition();

	// 找到被攻击的士兵
	Solider* pHitSolider = nullptr;
	for (CampList::iterator it = mCampList.begin(); it != mCampList.end(); it++)
	{
		Camp * pCamp = it->second;
		if (pCamp->getCampType() == pAtkBuild->getCamp())
			continue;

		pHitSolider = pCamp->getCanBeAttackSoliser(atkPos, rangeWidth, rangeHeight);
		if (pHitSolider)
			break;
	}

	if (nullptr == pHitSolider)
		return;

	// 炮塔攻击动作
	pAtkBuild->getBuildView()->playAni(BUILD_ANI_TURRET_ATTACK);
	sgSoundManager()->playMusicEffect(SOUND_BUILD_ATTACK);

	// 在战斗场景添加子弹
	Point soliderPos = std::move(pHitSolider->getPosition());
	bool bNormalAttack = false;

	if (pAtkBuild->getLevel() > 5)
	{
		
		Turret* pTempAtkBuild = dynamic_cast<Turret*>(pAtkBuild);
		pTempAtkBuild->setAtkCount(pTempAtkBuild->getAtkCount() + 1);
		if (pTempAtkBuild->getAtkCount() == pBuildAttr->attCount)//攻击四次才生效
		{
			bNormalAttack = true;
			pTempAtkBuild->setAtkCount(0);
			if (pAtkBuild->getLevel() == 6)
			{
				//Aoe
				mBattleView->playBombShootAni(atkPos, soliderPos, CC_CALLBACK_0(BattleControl::doBombAoeHitSolider, this, pHitSolider, soliderPos,pTempAtkBuild->getCamp()));
			}
			else if (pAtkBuild->getLevel() == 7)
			{
				//减速
				int formationID = pHitSolider->getFormationId();
				mBattleView->playBombShootAni(atkPos, soliderPos, CC_CALLBACK_0(BattleControl::doBombIceHitSolider, this, formationID, soliderPos));
			}
		}
	}
	//普通炮塔攻击
	if (!bNormalAttack)
	{
		mBattleView->playBombShootAni(atkPos, soliderPos, CC_CALLBACK_0(BattleControl::doBombHitSolider, this, pHitSolider, soliderPos));
		pHitSolider->doSpotted();
		pHitSolider->retain();
	}



	//炮塔攻击增加士气
	sgMoraleManager()->registMoraleMessage(pAtkBuild->getCamp(), 1.0f, E_MORALECOMPUTE_DEFENCE);

}

void BattleControl::doBuildChange(GameMessage* message)
{
	//暂时没有转换
	MessageBuildChange* pMes = dynamic_cast<MessageBuildChange*>(message);
	if (pMes)
	{
		/*BaseBuild* pBuild = getTargetBuild(pMes->getBuild());
		const PropInfo* pProp = sgTableManager()->getPropInfoById(pMes->getPropId());

		if (pBuild && pProp != nullptr && PROP_TYPE_EXCHANGE_B <= pProp->type && pProp->type <= PROP_TYPE_EXCHANGE_T)
		{
			if (pBuild->getCamp() != (eCampType)pMes->getCamp())
				return;

			eBulidType type;
			if (pProp->type == PROP_TYPE_EXCHANGE_B)
				type = eBulidType::EBULID_TYPE_BARRACK;
			else if (pProp->type == PROP_TYPE_EXCHANGE_T)
				type = eBulidType::EBULID_TYPE_TURRET;
			else if (pProp->type == PROP_TYPE_EXCHANGE_I)
				type = eBulidType::EBULID_TYPE_INSISTUTE;

			if (pBuild->getBuildType() == type)
				return;



			std::string key = pMes->getNew() ? StringUtils::format("exchange_%s", pBuild->getKeyString().c_str()) : StringUtils::format("change_%s", pBuild->getKeyString().c_str());
			if (!sgTimeCalculator()->hasKey(key))
			{

				pBuild->getBuildView()->playAni(BUILD_ANI_EXCHANGE);
				sgTimeCalculator()->createTimeCalcultor(key, pProp->duration);
				sgSoundManager()->playMusicEffect(SOUND_BUILD_CHANGE_TYPE);


				if (pMes->getNew())
				{
					sgPropsControl()->setCdTime(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_EXCHANGE));
				}

			}
		}*/


		const PropInfo* pInfo = sgTableManager()->getPropInfoById(pMes->getPropId());
		BaseBuild* pBuild = getTargetBuild(pMes->getBuild());

		if (pInfo && pBuild)
		{
			bool isNew = pMes->getNew();//是否是综合转换卷

			eBulidType type;
			if (pInfo->type == PROP_TYPE_EXCHANGE_B)
				type = eBulidType::EBULID_TYPE_BARRACK;
			else if (pInfo->type == PROP_TYPE_EXCHANGE_T)
				type = eBulidType::EBULID_TYPE_TURRET;
			else if (pInfo->type == PROP_TYPE_EXCHANGE_I)
				type = eBulidType::EBULID_TYPE_INSISTUTE;

			std::string key = isNew ? StringUtils::format("exchange_%s_%d", pBuild->getKeyString().c_str(), type) : StringUtils::format("change_%s_%d", pBuild->getKeyString().c_str(), type);
			if (!sgTimeCalculator()->hasKey(key))
			{

				pBuild->getBuildView()->playAni(BUILD_ANI_EXCHANGE);
				sgTimeCalculator()->createTimeCalcultor(key, pInfo->duration);
				sgSoundManager()->playMusicEffect(SOUND_BUILD_CHANGE_TYPE);

				if (pMes->getCamp() == sgBattleControl()->getOwnCamp())
				{
					if (isNew)
					{
						sgPropsControl()->setCdTime(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_EXCHANGE));
					}
					else
					{
						sgPropsControl()->setCdTime(pMes->getPropId());
					}

				}
			}
		}

	}

	
}

void BattleControl::getEnemyBuildList(eCampType eCamp, MapBulidList& buildList)
{
	buildList.clear();
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); ++it)
	{
		eCampType eType = it->second->getCamp();
		if (eType != eCamp && eType != ECAMP_TYPE_Neutral && eType != ECAMP_TYPE_Creeps)
		{
			buildList.insert(std::make_pair(it->second->getKeyID(), it->second));
		}
	}
}

void BattleControl::getEnemyBuildsMaxPeople(eCampType eCamp, MapBulidList& buildList)
{
	buildList.clear();

	int campPeople = 0;
	eCampType eSltcamp;

	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); ++it)
	{
		// 只获取对立阵营
		if (it->first == ECAMP_TYPE_Neutral || it->first == eCamp)
			continue;

		// 获取人口最大的阵营
		if (campPeople == 0 ||
			(campPeople > 0 && it->second->getAllUnit() > campPeople))
		{
			eSltcamp = it->first;
			campPeople = it->second->getAllUnit();
		}
	}

	it = mCampList.find(eSltcamp);
	if (it != mCampList.end())
	{
		buildList = it->second->getBuildList();
	}
}

int BattleControl::getEnemyBuildsMaxPeople(eCampType eCamp)
{

	int campPeople = 0;
	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); ++it)
	{
		// 只获取对立阵营
		if (it->first == ECAMP_TYPE_Neutral || it->first == eCamp)
			continue;

		// 获取人口最大的阵营
		if (campPeople == 0 ||
			(campPeople > 0 && it->second->getAllUnit() > campPeople))
		{
			campPeople = it->second->getAllUnit();
		}
	}
	return campPeople;
}

void BattleControl::cleanGame()
{
	if (mIsPvp)
	{
		sgPropsControl()->clean();
		sgPropsControl()->initSaveProp();

		if (!sgPvpControl()->getIsEnd())
		{
			sgPvpControl()->getPvPProxy()->sendLeave();
			//sgRankingControl()->requestPlayerRankingInfo();
		}
	}

	
	setOwnCamp(ECAMP_TYPE_Player);
	sgPvpControl()->clean();
	mCreepsMgr.cleanup();
	mGlyphBattleMgr.clean();
	mAsstBattleCtrl.clean();

	mBattleView->clean();
	mCampList.clear();
	mBuildList.clear();
	mBuffTipsVec.clear();
	mOilPointList.clear();
	isBorn = false;
	mIsPvp = false;
	m_bCanPlayWalkSound = true;
	clearCampBuffShow();

	mBattleTipsList.clear();
	cleanData();

}

void BattleControl::enableTimeCalculate(bool start)
{
	if (start == true)
	{
		sgTimeCalculator()->resume();
	}
	else
	{
		sgTimeCalculator()->pause();
	}
}
void BattleControl::resolvePvpCamp()
{
	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); ++it)
	{
		 

	}
}

void BattleControl::initCampBuffView(Node* topNode, eCampType camp)
{
	std::string key = "mBuff%d";
	BuffIconNodeVec nodeVec;
	for(int i = 0; i < 3; i++)
	{
		BuffIconNode* pNode = new BuffIconNode(); 
		pNode->setCampType(camp);
		nodeVec.push_back(pNode);
		Node* pPosNode =dynamic_cast<Node* > (topNode->getChildByName(StringUtils::format(key.c_str(), i+1)));
		if(NULL != pPosNode)
		{
			pNode->setPosition(pPosNode->getPosition());
			topNode->addChild(pNode->getBuffView(), 2);
		}
	} 
	mCampBuffNodeMap.insert(std::make_pair(camp, nodeVec));

	//定义刷新tips时间定时器
	 sgTimeCalculator()->createTimeCalcultor("BuffTipsTime", 1.7f);
}


void BattleControl::startCampBuffShow(eCampType camp, eBuffType buffType, float time)
{
	CampBuffNodeMap::iterator campBuffIter = mCampBuffNodeMap.find(camp);
	if(campBuffIter != mCampBuffNodeMap.end())
	{
		BuffIconNodeVec vec = campBuffIter->second;
		int index = 0;
		for(int i = 0; i < vec.size(); i++)
		{ 
			BuffIconNode* pNode = vec.at(i);
			if(pNode->getLock() == false)
			{
				index++;
				pNode->start(camp, buffType, time);
				break;
			}
			if(pNode->getBuffType() == buffType)
			{
				index++;
				pNode->start(camp, buffType, time);
				break;
			}
		}
		if(index == 0)
		{
			vec.at(index)->start(camp, buffType, time);
		}

	} 
}

void BattleControl::addBuffTips(eCampType camp, eBuffType buffType, int insID)
{ 
	if(getOwnCamp() != camp)
		return;
	mBattleView->addBuffTips(buffType, insID);
}
void BattleControl::updateCampBuffShow()
{
	CampBuffNodeMap::iterator campBuffIter = mCampBuffNodeMap.begin();
	for(; campBuffIter != mCampBuffNodeMap.end(); campBuffIter++)
	{
		BuffIconNodeVec::iterator iter = campBuffIter->second.begin();
		for(; iter != campBuffIter->second.end(); iter++)
		{
			BuffIconNode* pNode = (*iter);
			if(pNode->getLock() == true)
			{
				pNode->update();
			}
		} 
	}
	updateBuffTips();
}

void BattleControl::updateBuffTips()
{
	float time = sgTimeCalculator()->getTimeLeft("BuffTipsTime");
	if(time <= 0.0f)
	{
		sgTimeCalculator()->removeTimeCalcultor("BuffTipsTime");
		
		std::vector<std::string >::iterator iter = mBuffTipsVec.begin();
		if(iter != mBuffTipsVec.end())
		{ 
			sgMessageManager()->sendMessage(EMT_BATTLE_TIP, (*iter).c_str());
			mBuffTipsVec.erase(iter); 
		}
		sgTimeCalculator()->createTimeCalcultor("BuffTipsTime", 1.7f); 
	}
}


void BattleControl::clearCampBuffShow()
{
	CampBuffNodeMap::iterator campBuffIter = mCampBuffNodeMap.begin();
	for(; campBuffIter != mCampBuffNodeMap.end(); campBuffIter++)
	{ 
		for (BuffIconNodeVec::iterator iter = campBuffIter->second.begin(); iter != campBuffIter->second.end(); iter++)
		{
			BuffIconNode* pNode = (*iter);
			if (NULL != pNode)
			{ 
				delete pNode;
				pNode = NULL; 
			}
		}
		campBuffIter->second.clear();
	}
	mCampBuffNodeMap.clear();
}

void BattleControl::clearCampBuffShow(eCampType camp)
{
	CampBuffNodeMap::iterator campBuffIter = mCampBuffNodeMap.find(camp);
	if(campBuffIter !=  mCampBuffNodeMap.end())
	{
		BuffIconNodeVec::iterator iter = campBuffIter->second.begin();
		for(; iter != campBuffIter->second.end(); iter++)
		{
			BuffIconNode* pNode = (*iter);
			if(NULL != pNode)
			{
				pNode->clearNode();
			}
		}
	} 
	mCampBuffNodeMap.erase(campBuffIter->first);
}

void BattleControl::s2cUpdateBuild(int level, int build, int type, int camp, int num, std::set<int>& fList, bool reconn)
{
	if (sgUserObj()->getIsHost())
	{
		sgTimeCalculator()->removeTimeCalcultor("pvp_update");
		removeCampAi();
		sgUserObj()->setIsHost(false);

		CCLOG("[trace]BattleControl::s2cUpdateBuild  remove pvp_update i am  not host");
	}

	BaseBuild* pBuild = getTargetBuild(build);
	if (pBuild)
	{
		if (reconn)
		{
			CampList::iterator rIt = mCampList.find(pBuild->getCamp());
			CampList::iterator cIt = mCampList.find((eCampType)camp);

			//if (pBuild->getCamp() != camp 
			//	&& rIt != mCampList.end() 
			//	&& cIt != mCampList.end() 
			//	&& !(mOwnCamp == cIt->first 
			//	&& sgPvpControl()->getIsDead()))
			//{
			//	rIt->second->removeBuild(pBuild->getKeyID());
			//	pBuild->s2cCamp(camp);
			//	cIt->second->insertBuild(pBuild);
			//}

			rIt->second->removeBuild(pBuild->getKeyID(),false);
			pBuild->s2cCamp(camp);
			cIt->second->insertBuild(pBuild);

			pBuild->s2cType(type);
			pBuild->s2cNum(num);
			pBuild->s2cLevel(level);
			pBuild->s2cFormationList(fList);
		}
		else
		{
			std::set<int> mList = pBuild->getTarFormationIdList();
			std::set<int> diff;//(fList.size() + mList.size());
			std::set<int>::iterator sIt;
			std::set_difference(fList.begin(), fList.end(), mList.begin(), mList.end(), inserter(diff, diff.begin()));

			if (diff.size() <= 0)
			{
				if (!pBuild->isComeIngFormation() && !pBuild->checkStatus(BuildStatus_Sending_Solider))
				{
					if (pBuild->getCamp() != camp)
					{
						CampList::iterator rIt = mCampList.find(pBuild->getCamp());
						CampList::iterator cIt = mCampList.find((eCampType)camp);

						if (rIt != mCampList.end() && cIt != mCampList.end()  && !(mOwnCamp == cIt->first && sgPvpControl()->getIsDead()))
						{
							rIt->second->removeBuild(pBuild->getKeyID(),false);
							pBuild->s2cCamp(camp);
							cIt->second->insertBuild(pBuild);
						}

					}

					pBuild->s2cType(type);
					pBuild->s2cNum(num);
					pBuild->s2cLevel(level);

				}

			}
			else
			{

				//Director::getInstance()->pushUserLogString(StringUtils::format("[leon]: diff Formation List size = %d \n ",diff.size()));
				for (sIt = diff.begin(); sIt != diff.end(); sIt++)
				{
					pBuild->addDiffList(*sIt);
					//Director::getInstance()->pushUserLogString(StringUtils::format("[leon]: diff formation id = %d \n ", *sIt));
				}
			}
		}
	
	}

}

bool BattleControl::isPvpAndHost()
{

	bool flag = sgUserObj()->getIsHost();
	return mIsPvp && flag;
}

int BattleControl::checkEnmyNum()
{
	int num = 0;
	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); it++)
	{
		if (mOwnCamp != it->first && it->first != ECAMP_TYPE_Neutral && it->first != ECAMP_TYPE_Creeps  && it->second->getBuildList().size() > 0)
		{
			num++;
		}
	}

	return num;

}

eCampType BattleControl::getPVPWinCamp()
{
	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); it++)
	{
		if (mOwnCamp != it->first && it->first != ECAMP_TYPE_Neutral && it->second->getBuildList().size() > 0 && it->first != ECAMP_TYPE_Creeps)
		{
			return it->first;
		}
	}

	if (EnvSet::leon())
	{
		GenerlDialog::show("void BaseMode::doFail()", "getPVPWinCamp ");
	}

	return ECAMP_TYPE_Invalid;
}

void BattleControl::checkAddPvpAi()
{
	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); it++)
	{
		if (isPvpAndHost() && sgBattleControl()->getOwnCamp() != it->first && sgPvpControl()->checkIsAI(it->first))
		{
			it->second->addAi();
		}
	}
}


void BattleControl::removeCampAi()
{
	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); it++)
	{
		it->second->removeAi();
	}
}

void BattleControl::changeToNeutral(eCampType type)
{
	CampList::iterator it = mCampList.find(type);
	CampList::iterator nIt = mCampList.find(ECAMP_TYPE_Neutral);
	if (it != mCampList.end())
	{
		if (nIt != mCampList.end())
			it->second->changeToNeutral(nIt->second);
		else{

			Camp* pCamp = it->second;

			pCamp->setCampType(ECAMP_TYPE_Neutral);
			pCamp->changeToNeutral(pCamp);
			mCampList.erase(type);
			mCampList.insert(std::make_pair(pCamp->getCampType(), pCamp));

			CCLOG("[erro] changeToNeutral :no ECAMP_TYPE_Neutral");
		}


		sgPvpControl()->getPvPProxy()->sendMap(mBuildList);

		char szParam[32] = { 0, };
		sprintf(szParam, "%d %d %d", -1, type, ECAMP_TYPE_Neutral);
		sgMessageManager()->sendMessage(EMT_BUILD_DESTORY, szParam);

	}


}

void BattleControl::playPvpCampWinAni(eCampType type)
{
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end(); ++it)
	{
		if (it->second->getCamp() == type)
		{
			it->second->getBuildView()->playAni(BUILD_ANI_WIN);
		}
	}
}

void BattleControl::clearJungleBuild(int destroyID)
{/*
	//移除阵营建筑引用
	CampList::iterator tIter =mCampList.begin();
	for(; tIter != mCampList.end(); tIter++)
	{
	MapBulidList& tBuildListMap = const_cast<MapBulidList& >(tIter->second->getBuildList());
	MapBulidList::iterator tBuildIter =tBuildListMap.find(destroyID);
	if(tBuildIter != tBuildListMap.end())
	{
	tBuildListMap.erase(tBuildIter);
	mBattleView.clearBuildView(destroyID);
	//置空build对buildView的引用
	mBattleView.clearBuildRefView(destroyID);
	}
	//给当前正在行军的士兵一个源建筑的拷贝点用于队形受力
	Vect_Formation& tFormationVec = tIter->second->getFormationList();
	Vect_Formation::iterator tFormationIter = tFormationVec.begin();
	for(; tFormationIter != tFormationVec.end(); tFormationIter++)
	{
	Formation* tFormationData = tFormationIter->second;
	if(tFormationData->getSrcBuild()->mBuildID == destroyID)
	{
	BaseBuild* tBuild = tFormationData->getSrcBuild()->copyWithTypeWithoutView(tFormationData->getSrcBuild()->getBuildType());
	tBuild->setIsActive(false);
	tFormationData->setSrcBuild(tBuild);
	}
	}
	}

	//释放build
	MapBulidList::iterator tBuildIter2 = mBuildList.find(destroyID);
	if(tBuildIter2 != mBuildList.end())
	{
	delete tBuildIter2->second;
	tBuildIter2->second = NULL;
	mBuildList.erase(tBuildIter2);
	}
	*/
}

void BattleControl::createJungleBuild(int destroyID, int buildID, int locationID, int buffID)
{/*
	//if(destroyID != 0)
	{
	clearJungleBuild(buildID);
	}
	//clearJungleBuild(buildID);
	//创建新的build
	sgMapHandler()->updateBuildInfo(ECAMP_TYPE_Jungle, buildID, locationID);
	CampList::iterator tCampIter = mCampList.find(ECAMP_TYPE_Jungle);
	if (tCampIter != mCampList.end())
	{
	MapBulidList::const_iterator tBuildIter = tCampIter->second->getBuildList().find(buildID);
	if (tBuildIter != tCampIter->second->getBuildList().end())
	{
	tBuildIter->second->setIsActive(true);
	mBuildList.insert(std::make_pair(tBuildIter->first, tBuildIter->second));
	}
	}
	mBattleView.refreshJungle(buildID, locationID, buffID);*/
}

void BattleControl::returnFromJungleBuild(int srcID, int destID)
{
	mBattleView->returnFromJungleBuild(srcID, destID);
}

void BattleControl::closeJungleBuild(int srcID, int srcCamp, int destCamp, int buildID, int locationID)
{
	MapBulidList::iterator iter = mBuildList.find(buildID);
	if (iter != mBuildList.end())
	{
		if (iter->second->getBuildType() == EBULID_TYPE_CREEPS)
		{
			Monster* pBuild = dynamic_cast<Monster*>(iter->second);
			pBuild->createCheckFormationTimer();
		}
		mBattleView->closeJungleBuild(srcID, srcCamp, destCamp, buildID, locationID);
	}
}

bool BattleControl::getIfOtherTwoInBattle(eCampType eOwnCamp)
{
	for (int i = 0; i != ECAMP_TYPE_Invalid; ++i)
	{
		eCampType etype = (eCampType)i;
		if (etype == ECAMP_TYPE_Neutral || etype == eOwnCamp)
			continue;
		// 获取其他有效阵营的攻击列表
		Camp* pCamp = getTargetCamp(etype);
		if (!pCamp)
			continue;

		std::set<int> attlist;
		pCamp->getCurAttCampList(attlist);
		// 判断是否是攻击第三方阵营
		std::set<int>::iterator it = attlist.begin();
		for (; it != attlist.end(); ++it)
		{
			eCampType checktype = (eCampType)(*it);
			if (etype == ECAMP_TYPE_Neutral || etype == eOwnCamp)
				continue;
			return true;
		}
	}
	return false;
}
int BattleControl::getInstituteNum(eCampType type)
{
	CampList::iterator it = mCampList.find(type);

	if (it != mCampList.end())
	{
		return it->second->getInstituteNum();
	}

	return 0;
}

 

void BattleControl::doBombHitSolider(Solider* pSolider, Point pos)
{
	if (nullptr == pSolider)
		return;

	mBattleView->playBombExplosionAni(pos);

	if (!pSolider->checkStatus(Solider::Status::kArrived))
	{
		pSolider->doBombHit();
		pSolider->doDead();
		sgSoundManager()->playMusicEffect(SOUND_BUILD_SOLDIER_DIE);
	}

	pSolider->release();
}

void BattleControl::doBuildSoliderDepart(GameMessage* message)
{
	MessageRowDepart* pMsg = dynamic_cast<MessageRowDepart*>(message);
	if (pMsg)
	{

	}
}


void BattleControl::doFormationOver(GameMessage* message)
{
	MessageFormationOver* pMsg = dynamic_cast<MessageFormationOver*>(message);
	if (pMsg)
	{
		if (pMsg->getDefendCamp() == ECAMP_TYPE_Creeps)
		{
			CreepsBuild* pCreepBuild = dynamic_cast<CreepsBuild*>(this->getTargetBuild(pMsg->getDstBuild()));
			if (pCreepBuild)
			{
				pCreepBuild->eraseOnWayFormation(pMsg->getFormationId());
				if (pCreepBuild->checkClose())
					pCreepBuild->doClose();
			}
		}


		if (sgTimeCalculator()->hasKey(StringUtils::format("att_ice_%d",pMsg->getFormationId())))
		{
			if (m_vFormationIdlist.size() > 0)
			{
				for (auto it = m_vFormationIdlist.begin(); it != m_vFormationIdlist.end(); ++it)
				{
					int id = *it;
					if (id == pMsg->getFormationId())
					{
						m_vFormationIdlist.erase(it);
						break;
					}
						
				}
			}
		}
	}
}


void BattleControl::doCreepsReturnSolider(int creepsBuildId, eCampType originCampType, int originBildUid, int fid, int totalCount)
{
	if (originCampType == ECAMP_TYPE_Creeps)
	{
		CCLOG("[error] BattleControl::doCreepsReturnSolider : Camp[%d] can not return solider", originCampType);
		return;
	}


	BaseBuild* pOriginBuild = this->getTargetBuild(originBildUid);
	if (nullptr == pOriginBuild)
	{
		CCLOG("[error] BattleControl::doCreepsReturnSolider : Build[%d] is null", originBildUid);
		return;
	}

	BaseBuild* pCreepsBuild = this->getTargetBuild(creepsBuildId);
	if (nullptr == pCreepsBuild)
	{
		CCLOG("[error] BattleControl::doCreepsReturnSolider : Build[%d] is null", creepsBuildId);
		return;
	}

	if (EBULID_TYPE_CREEPS != pCreepsBuild->getBuildType())
	{
		CCLOG("[error] BattleControl::doCreepsReturnSolider : Build[%d] is not a creeps", creepsBuildId);
		return;
	}


	// 	const float delayInterval = 0.3f;
	// 	float delayTimer = 0.0f;
	// 	int returnCount = 0;
	// 	while (returnCount < totalCount)
	// 	{
	// 		int curCount = 0;
	// 		if (totalCount > returnCount + FormationV2::RowMaxCount)
	// 			curCount = FormationV2::RowMaxCount;
	// 		else
	// 			curCount = totalCount - returnCount;
	// 
	// 		auto callback = std::bind(&BattleControl::doReturnSoliderArrived, this, creepsBuildId, originCampType, originBildUid, fid, curCount);
	// 		mBattleView.playReturnSoliderParticle(delayTimer, pCreepsBuild->getPosition(), pOriginBuild->getPosition(), callback);
	// 
	// 		delayTimer += delayInterval;
	// 		returnCount += curCount;
	// 	}

	auto callback = std::bind(&BattleControl::doReturnSoliderArrived, this, creepsBuildId, originCampType, originBildUid, fid, totalCount);
	mBattleView->playReturnSoliderParticle(pCreepsBuild->getPosition(), pOriginBuild->getPosition(), callback);
}


void BattleControl::doReturnSoliderArrived(int creepsBuildId, eCampType originCampType, int originBuildUid, int fid, int count)
{
	BaseBuild* pOriginBuild = sgBattleControl()->getTargetBuild(originBuildUid);
	if (nullptr == pOriginBuild)
		return;

	if (originCampType == pOriginBuild->getCamp())
	{
		std::string msg = StringUtils::format("%d %d %d %d %d", count, originBuildUid, creepsBuildId, 0, fid);
		sgMessageManager()->sendMessage(EMT_TRANSFORM, msg);

	}
	else
	{
		std::string msg = StringUtils::format("%d %d %d %d %d %d %d", originCampType, pOriginBuild->getCamp(), count, originBuildUid, creepsBuildId, 0, fid);
		sgMessageManager()->sendMessage(EMT_FIGHT, msg);
	}
}


FormationV2* BattleControl::getFormationById(int id)
{
	CampList::iterator it = mCampList.begin();
	for (; it != mCampList.end(); it++)
	{
		FormationV2* pFormation = it->second->getFormationById(id);
		if (pFormation != nullptr)
		{
			return pFormation;
		}
	}

	return nullptr;
}


void BattleControl::getApproachingBuildFormations(int buildUid, std::vector<FormationV2*>& outList, const std::function<bool(const FormationV2*)>& filter /* = nullptr */)
{
	for (auto itrCamp = mCampList.begin(); itrCamp != mCampList.end(); ++itrCamp)
	{
		Camp* pCamp = itrCamp->second;
		if (nullptr == pCamp)
			continue;

		const Vect_Formation & vecFormations = pCamp->getFormationList();
		for (auto itrFormation = vecFormations.begin(); itrFormation != vecFormations.end(); ++itrFormation)
		{
			FormationV2* pFormation = itrFormation->second;
			if (nullptr == pFormation)
				continue;

			if (pFormation->getDstBuildUid() != buildUid)
				continue;

			if (filter && !filter(pFormation))
				continue;

			outList.push_back(pFormation);
		}
	}
}

void BattleControl::pushOilPoint(const OilPoint& point)
{
	OilPointMap::iterator it = mOilPointList.find(point.mId);
	if (it == mOilPointList.end())
	{
		mOilPointList.insert(std::make_pair(point.mId, point));
	}
}

const OilPointMap& BattleControl::getOilPointList()
{
	return mOilPointList;
}

void BattleControl::updateOil()
{
	OilPointMap::iterator it = mOilPointList.begin();
	for (; it != mOilPointList.end();)
	{
		if (sgTimeCalculator()->getTimeLeft(StringUtils::format("oil_%d",it->second.mId)) == 0)
		{
			sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("oil_%d", it->second.mId));
			mBattleView->removeOilEffect(it->second.mId);
			it = mOilPointList.erase(it);
			continue;
		}
		it++;
	}
}void BattleControl::doBombAoeHitSolider(Solider* pSolider, Point pos,eCampType campType)
{
	
	mBattleView->playBombAttAni(pos, "fla_paptaiSK");

	std::vector<Solider*> soliderList;
	for (CampList::iterator it = mCampList.begin(); it != mCampList.end(); it++)
	{
		Camp* pCamp = it->second;
		if (nullptr == pCamp || it->first == campType)
			continue;
		pCamp->getAliveSoliderListInEllipse(pos, 33.0f, 50.0f, soliderList);
	}

	std::random_shuffle(soliderList.begin(), soliderList.end());

	//const int min = (int)pInfo->duration / 100;
	//const int max = (int)pInfo->duration % 100;
	const int min = 4;
	const int max = 6;
	int rand = cocos2d::random(min, max) * 0.1f * soliderList.size();

	for (int i = 0; i < soliderList.size(); i++)
	{
		if (i <= rand)
		{
			Solider* pSolider = soliderList.at(i);
			if (pSolider)
			{
				pSolider->doDead();
			}
		}
	}

}

void BattleControl::doBombIceHitSolider(int formationID, Point pos)
{
	FormationV2 *formation = getFormationById(formationID);
	if (nullptr == formation)
		return;
	mBattleView->playBombAttAni(pos, "fla_paotaiSK1");

	formation->setSpeedScale(0.5);

	if (!sgTimeCalculator()->hasKey(StringUtils::format("att_ice_%d", formationID)))
	{
		m_vFormationIdlist.push_back(formationID);
		sgTimeCalculator()->createTimeCalcultor(StringUtils::format("att_ice_%d", formationID), 2);
	}
	else
	{
		sgTimeCalculator()->resetTimeCalcultor(StringUtils::format("att_ice_%d", formationID));
	}

}

void BattleControl::stopBornAni()
{
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end();it++)
	{
		it->second->getBuildView()->stopBornAnimation();
	}
}



void BattleControl::addBattleTipsForGlyph(eCampType campType, const std::string& battleTips)
{
	if (this->mOwnCamp != campType)
		return;
	//mBattleView.addBattleTips(battleTips);

	//Color4B colrText(25, 200, 25, 255);
	//Color4B colrOutline(25, 180, 25, 255);
	Color4B colrText(255, 255, 255, 255);
	Color4B colrOutline(198, 0, 79, 255);
	mBattleView->addBattleTips(battleTips, 36, &colrText, &colrOutline, "");
}

int BattleControl::getCampOccupyNum(eCampType camp)
{
	Camp* pCamp = getTargetCamp(camp);
	if (pCamp)
	{
		int occupy = pCamp->getOccupyBuild();
		return occupy;
	}

	return 0;
}

void BattleControl::starAddCreepsPoint()
{
	MapBulidList::iterator it = mBuildList.begin();
	for (; it != mBuildList.end();it++)
	{
		if (it->second->getBuildType() == EBULID_TYPE_CREEPS)
		{
			CreepsBuild* pCareeps = dynamic_cast<CreepsBuild*>(it->second);
			if (pCareeps)
			{
				pCareeps->addCreepsPoint();
			}
		}
	}
}
