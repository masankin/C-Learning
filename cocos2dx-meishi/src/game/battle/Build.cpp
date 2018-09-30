#include "Build.h"
#include "BuildView.h"
#include "TimeCalculator.h"
#include "HelperFunc.h"
//#include "Formation.h"
#include "Formation/FormationV2.h"
#include "data/TableManager.h"
#include "data/Tables.h"
#include "battle/BattleControl.h"
#include "BattleControl.h"
#include "Camp.h"
#include "Message.h"
#include "battle/AI/basic/PolicyManager.h"
#include "GamePrecedure.h"
#include "prop/PropsControl.h"
#include "SoundManager.h" 
#include "buff/BuffManager.h"
#include "guide/GuideManager.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"
#include "MoraleManager.h"
#include "Glyph/GlyphData.h"
#include "Glyph/GlyphBattleManager.h"
#include "game/battle/AI/basic/BaseState.h"
#include "game/battle/BattleRecordsManager.h"

ActionStatus::ActionStatus() :mOriginalPeople(0), mStatus(BUILD_NONE), mActionNum(0), mId(0), mPlayStand(false)
{

}

ActionStatus::~ActionStatus()
{

}

bool ActionStatus::init(eCampType cType, int id)
{
	setStatus(BUILD_NONE);
	mCamp = cType;
	mId = id;
	return true;
}

void ActionStatus::update(int people, eCampType cType)
{
	if (mStatus == BUILD_NONE)
	{
		mOriginalPeople = people;
	}

	if (cType != mCamp)
	{
		mCamp = cType;
		reset();
		mOriginalPeople = people;
	}

	if (sgTimeCalculator()->getTimeLeft(StringUtils::format("ActionStatu_%d_%d", mId, BUILD_BAD_INJURE)) == 0)
	{
		if (mActionNum <= 5)
		{

			setStatus(BUILD_BAD_INJURE);
			addCaculateTime(mStatus);
		}
		else
		{
			reset();
		}
	}
	if (sgTimeCalculator()->getTimeLeft(StringUtils::format("ActionStatu_%d_%d", mId, BUILD_GOOD_INJURE)) == 0)
	{
		if (mActionNum <= 5)
		{
			setStatus(BUILD_GOOD_INJURE);
			addCaculateTime(mStatus);
		}
		else
		{
			reset();
		}
	}

	change(cType, people);

}

void ActionStatus::change(eCampType type, int num)
{
	if (mOriginalPeople == 0)
		return;

	float percent = num / (float)mOriginalPeople;

	if (mStatus == BUILD_NONE && type != mCamp)
	{
		setStatus(BUILD_BEATTACT);
	}
	else if (mStatus == BUILD_BEATTACT)
	{
		if (percent <= 0.5 && percent >= 0.25)
		{

			setStatus(BUILD_GOOD_INJURE);
			addCaculateTime(BUILD_GOOD_INJURE);
		}
		else if (percent < 0.25)
		{
			setStatus(BUILD_BAD_INJURE);
			addCaculateTime(BUILD_BAD_INJURE);
		}
	}
	else if (mStatus == BUILD_GOOD_INJURE)
	{
		if (percent < 0.25)
		{
			setStatus(BUILD_BAD_INJURE);
			addCaculateTime(BUILD_BAD_INJURE);
		}
		else if (percent > 0.5)
		{
			reset();
		}
	}
	else if (mStatus == BUILD_BAD_INJURE)
	{
		if (percent > 0.5)
		{
			reset();
		}
		else if (percent <= 0.5 && percent >= 0.25)
		{
			setStatus(BUILD_GOOD_INJURE);

			addCaculateTime(BUILD_GOOD_INJURE);
		}
	}
}

void ActionStatus::reset()
{
	setStatus(BUILD_NONE);
	mActionNum = 0;
	sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("ActionStatu_%d_%d", mId, BUILD_BAD_INJURE));
	sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("ActionStatu_%d_%d", mId, BUILD_GOOD_INJURE));
}

eActionStatus ActionStatus::getCurStatus()
{
	return mStatus;
}

ActionStatus* ActionStatus::create(eCampType cType, int id)
{
	ActionStatus *pRet = new(std::nothrow) ActionStatus();
	if (pRet && pRet->init(cType, id))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

void ActionStatus::setStatus(eActionStatus statu)
{
	if (mStatus != statu)
		mPlayStand = true;
	mStatus = statu;
}

void ActionStatus::addCaculateTime(eActionStatus statu)
{
	std::string key = StringUtils::format("ActionStatu_%d_%d", mId, statu);
	if (statu == BUILD_GOOD_INJURE)
	{
		if (sgTimeCalculator()->hasKey(StringUtils::format("ActionStatu_%d_%d", mId, BUILD_BAD_INJURE)))
			sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("ActionStatu_%d_%d", mId, BUILD_BAD_INJURE));

		if (sgTimeCalculator()->hasKey(key))
			sgTimeCalculator()->resetTimeCalcultor(key);
		else
			sgTimeCalculator()->createTimeCalcultor(key, 3.0f);

		mActionNum++;
	}
	else if (statu == BUILD_BAD_INJURE)
	{
		if (sgTimeCalculator()->hasKey(StringUtils::format("ActionStatu_%d_%d", mId, BUILD_GOOD_INJURE)))
			sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("ActionStatu_%d_%d", mId, BUILD_GOOD_INJURE));

		if (sgTimeCalculator()->hasKey(key))
			sgTimeCalculator()->resetTimeCalcultor(key);
		else
			sgTimeCalculator()->createTimeCalcultor(key, 3.0f);

		mActionNum++;
	}
}

bool ActionStatus::play()
{
	return mPlayStand;
}

void ActionStatus::resetPlay()
{
	mPlayStand = false;
}



//////////////////////////////Build////////////////////////////////////////////
int BaseBuild::g_BuildID = 0;

BaseBuild::BaseBuild() : mBuildView(NULL), mLevel(1), mCurentAni(BUILD_ANI_STAND),
mHaveDestroy(false), mBeHurtSoundId(0), mElapsedTime(0.0f), mCoypDestroy(0), mEnableBattleTip(true)
, mVisible(true), mEnable(true)
, mStatus(BuildStatus_Normal)
, mServerUpLevel(0)
, mInviInfo(nullptr)
, mIsleveling(false)
{
	g_BuildID++;
	mBuildID = g_BuildID;
	mDirtyTag.set(0xFFFFFFFF);
}

BaseBuild::~BaseBuild()
{
	if (!mCoypDestroy)
	{
		if (mBeHurtSoundId != 0)
		{
			sgSoundManager()->stopMusicEffect(mBeHurtSoundId);
			mBeHurtSoundId = 0;
		}
	}
	if (mActionStatu)
	{
		CC_SAFE_RELEASE(mActionStatu);
	}

	mStateList.clear();
}

void BaseBuild::init()
{
	mOwnPeople = mBuildInfo.unit;
	mLevel = mBuildInfo.id % 10;
	mBaseCamp = (mBuildInfo.camp);
	const BuildAttr* pAttr = sgTableManager()->getBuildAttr(mBuildInfo.id);
	resolveAIState();
	mTarFormationIdList.clear();
	mComeInList.clear();
	mFormationSendingList.clear();
	mActionStatu = ActionStatus::create(mBaseCamp, getKeyID());
	CC_SAFE_RETAIN(mActionStatu);
}

void BaseBuild::initView()
{
	mBuildView = BuildView::createView(mBuildInfo.camp, getKeyID(), (eBulidType)mBuildInfo.type);
}

BaseBuild* BaseBuild::createBuild(const BuildInfo& buildinfo, bool isCopy /* = false */)
{
	BaseBuild* pBuild = NULL;
	eBulidType eType = (eBulidType)(buildinfo.type);

	if (eType == EBULID_TYPE_BARRACK)
	{
		pBuild = new Barracks();
	}
	else if (eType == EBULID_TYPE_TURRET)
	{
		pBuild = new Turret();
	}
	else if (eType == EBULID_TYPE_INSISTUTE)
	{
		pBuild = new Institute();
	}
	else if (eType == EBULID_TYPE_CREEPS)
	{
		//pBuild = new Monster();
		pBuild = new CreepsBuild();
	}
	else if (eType == EBUILD_TYPE_STABLES)
	{
		pBuild = new StablesBuild();
	}

	if (pBuild)
	{
		pBuild->mBuildID = buildinfo.insID;
		pBuild->setBuildInfo(buildinfo);

		const BuildIns* pIns = sgTableManager()->getBuildIns(buildinfo.insID);
		if (pIns)
		{
			int level = pIns->type % 10;
			pBuild->setLevel(level);
		}

		if (isCopy == false)
		{
			pBuild->initView();
			pBuild->init();
		}
	}
	return pBuild;
}

int BaseBuild::getAttrID()
{
	return mBuildInfo.id;
}

int BaseBuild::getKeyID()
{
	//return getAttrID() * 100 + mBuildID;
	return mBuildID;
}

std::string BaseBuild::getKeyString()
{
	char szKey[128] = { 0, };
	sprintf(szKey, "%d", getKeyID());
	return szKey;
}

void BaseBuild::resetCounter()
{//TODO: 退出关卡，重置计数器
	g_BuildID = 0;
}

void BaseBuild::updateUI()
{
	if (getIsDirty() == true && mBuildView)
	{
		mBuildView->updateUI();
		clearDirtyTag();
	}
}

void BaseBuild::updateTime(float dt)
{
	mElapsedTime = mElapsedTime + dt;

	if (mElapsedTime > 1.0f && mBeHurtSoundId != 0)
	{
		sgSoundManager()->stopMusicEffect(mBeHurtSoundId);
		mBeHurtSoundId = 0;
	}
}

void BaseBuild::addMinusOwnPeople(float owned)
{
	mOwnPeople += owned;
	mOwnPeople = MAX(0, mOwnPeople);
	setDirtyTag(EBDIR_PEOPLE);
}

bool BaseBuild::getIsDirty()
{
	return mDirtyTag.test(EBDIR_ALL);
}

bool BaseBuild::getIsDirty(eBuildDirty eTag) const
{
	return mDirtyTag.test(eTag);
}

void BaseBuild::setDirtyTag(eBuildDirty eTag)
{
	mDirtyTag.set(eTag);
}

void BaseBuild::clearDirtyTag()
{
	mDirtyTag.clear();
}

Rect BaseBuild::boundingBox()
{
	//Rect rect = mBuildView->getBoundingBox();
	//float w = 60;
	//float h = 66;
	//if (mBuildView)
	//{
	//	mBuildView->getContentSize().
	//}
	//return Rect(mBuildInfo.x - mBuildInfo.width / 2, mBuildInfo.y - mBuildInfo.height / 2, mBuildInfo.width, mBuildInfo.height);

	float w = 60;
	float h = 66;
	if (mBuildView)
	{
		w = mBuildView->getBuildSize().width;
		h = mBuildView->getBuildSize().height;
	}

	const Point& position = this->getPosition();
	return Rect(position.x - w / 2, position.y - h / 2, w, h);

}

const Point& BaseBuild::getPosition() const
{
	if (nullptr == mBuildInfo.mapObj)
		return Point::ZERO;

	return (mBuildInfo.mapObj)->getPosition();
}

bool BaseBuild::checkCanProduct()
{

	// 阵营总人口限制
	Camp* pCamp = sgBattleControl()->getTargetCamp(mBuildInfo.camp);
	if (pCamp == nullptr || (pCamp->getAllUnit() >= pCamp->getCampMaxProduct() && getBuildType() == EBULID_TYPE_BARRACK))
	{
		return false;
	}

	// 中立建筑人口只增长到初始值
	const BuildIns* pIns = sgTableManager()->getBuildIns(mBuildInfo.insID);
	if (mBuildInfo.camp == ECAMP_TYPE_Neutral && pIns && pIns->initPeople <= mOwnPeople)
	{
		return false;
	}


	int iMax = getMaxProduct();

	if (mOwnPeople >= iMax)
		return false;

	return true;
}

int BaseBuild::getMaxProduct()
{
	int iProMax = 0;

	const BuildAttr* pBuildAttr = sgTableManager()->getBuildAttr(getBuildInfo().id);
	if (pBuildAttr)
	{
		iProMax += pBuildAttr->proLimit;
	}

	auto pGlyph = sgBattleControl()->getGlyphBattleMgr()->getCampGlyph(getCamp(), Glyph::E_GLYPH_PPL_MAX);
	if (pGlyph)
	{
		iProMax += pGlyph->getValueAsAddition();
	}

	return iProMax;
}

int BaseBuild::getMaxHold()
{
	const BuildAttr* pBuildAttr = sgTableManager()->getBuildAttr(getBuildInfo().id);
	if (!pBuildAttr)
	{
		return 0;
	}

	return pBuildAttr->holdLimit;
}

Formation* BaseBuild::createFormation(float percent, int fId, int endId, float time /* = 0 */)
{
	if (mOwnPeople <= 0) return NULL;
	int num = mOwnPeople * percent;

	Formation* pFormation = Formation::create(mBuildInfo.camp, num, fId, this);
	return pFormation;
}


FormationV2* BaseBuild::createFormationV2(int fid, float percent, int begObjUid, int endObjUid)
{
	int count = mOwnPeople * percent;
	if (count <= 0)
		return nullptr;

	FormationV2* pFormation = FormationV2::create(fid, count, begObjUid, endObjUid);
	return pFormation;
}


int BaseBuild::requestPeople(int reqCount)
{
	int actualCount = 0;
	if (reqCount <= 0)
		return actualCount;

	if ((int)mOwnPeople > reqCount)
		actualCount = reqCount;
	else
		actualCount = mOwnPeople;

	addMinusOwnPeople((float)-actualCount);
	return actualCount;
}


int BaseBuild::getFormationRow(int& num)
{
	int temp = 0;
	if (mOwnPeople >= 6)
	{
		temp = MIN(6, num);
	}
	else
	{
		temp = mOwnPeople >= num ? num : mOwnPeople;
		//num = 0;
	}
	addMinusOwnPeople(-1 * temp);
	num -= temp;

	if (mOwnPeople == 0)
	{
		//num = 0;
		this->changeBuildAni(BUILD_ANI_OUTPUT_END);
	}
	else
	{
		this->changeBuildAni(BUILD_ANI_OUTPUTING);
	}

	return temp;
}


bool BaseBuild::checkCanUpgrade()
{
	const BuildAttr* pBuildAttr = sgTableManager()->getBuildAttr(getBuildInfo().id);
	if (!pBuildAttr)
	{
		return false;
	}

	if (pBuildAttr->upNeedTime <= 0 || pBuildAttr->upNeedPro > mOwnPeople)
		return false;

	const BuildIns* pIns = sgTableManager()->getBuildIns(getBuildInfo().insID);
	if (!pIns)
	{
		return false;
	}

	if (pIns->levelLimit <= mLevel)
		return false;

	if (isCurUpgrade() == true)
		//if (mCurentAni == BUILD_ANI_UPGRADE)
		return false;

	if (mInviInfo != nullptr && !mInviInfo->mUpgrade)
		return false;

	return true;
}

void BaseBuild::resolveAIState()
{
	const BuildIns* pIns = sgTableManager()->getBuildIns(getBuildInfo().insID);
	if (!pIns)
		return;

	std::string szStates = pIns->aiType;
	HelperFunc::replaceAllCharacters(szStates, "+", "\t");

	std::stringstream ssStates(szStates);
	do
	{
		int stateID = 0;
		ssStates >> stateID;

		if (stateID == 0)
			break;

		const StateAction* pStateAct = sgTableManager()->getStateActionByID(stateID);
		if (!pStateAct)
		{
			CCLOG("[error]BaseBuild::resolveAIState getStateActionByID eTag:%d error!", stateID);
			return;
		}
		std::vector<std::string> stateVec = HelperFunc::tokenize(pStateAct->state, "+");
		//处理state and的关系

		std::vector<std::string>::iterator it = stateVec.begin();
		for (; it != stateVec.end(); ++it)
		{
			std::string szState = *it;
			std::vector<std::string>tempStr = splitStr(szState, ":");
			
			vector<BaseState*> tempVect;
			for (auto strIt :tempStr)
			{
				HelperFunc::replaceAllCharacters(strIt, "#", "\t");
				std::stringstream ssState(strIt);
				int id;
				ssState >> id;
				StateFactory *stateFactory = sgPolicyMgr()->getStateFactory(eStateTag(id));
				if (stateFactory != NULL)
				{
					BaseState *baseState = stateFactory->create(ssState);
					//时间
					int time = atoi(pStateAct->time.c_str());
					if (time == -1)
						time = 1;

					//随机时间
					vector<std::string> strList = splitStr(pStateAct->time.c_str(), "&");
					if (strList.size() == 2)
						time = random(atoi(strList[0].c_str()), atoi(strList[1].c_str()));

					baseState->setLimitTime(time);
					baseState->setTargetBuild(this);
					baseState->setIsLimitTime(false);
					baseState->setCando(false);
					tempVect.push_back(baseState);
				}
			}

			if (!tempVect.empty())
				mStateList.insert(std::make_pair(stateID, tempVect));	
		}

	} while (1);
}

void BaseBuild::doAIState(float dt)
{
	if (checkAttacking())
		return;

	MapState::iterator it = mStateList.begin();
	for (; it != mStateList.end(); ++it)
	{
		vector<BaseState*> stateVect = it->second;
		if (stateVect.empty())
			continue;


		bool bSucc = sgPolicyMgr()->doBuildAIState(it->second, it->first);
		BaseState *state = stateVect[0];
		float time = state->getTime();
		if (state->getIsLimitTime() && time < state->getLimitTime())
		{
			time += dt;
			state->setTime(time);
		}
		else if(state->getIsLimitTime() && time >= state->getLimitTime())
		{
			state->setCando(true);
		}

		////取第一个状态的时间时间
		//BaseState *state = stateVect[0];
		//int time = state->getTime();
		//if (time >= state->getLimitTime())
		//{
		//	//执行
		//	bool bSucc = sgPolicyMgr()->doBuildAIState(it->second,it->first);
		//	if (bSucc)
		//	{
		//		state->setTime(0);
		//		break;
		//	}
		//}
		//else
		//{
		//	time += dt;
		//	state->setTime(time);
		//}
	}
}


void BaseBuild::changeBuildAni(eBuildAni enAni)
{

}

eCampType BaseBuild::getCamp()
{
	return mBuildInfo.camp;
}

eExpressType BaseBuild::getExpress()
{
	return mBuildInfo.express;
}

void BaseBuild::calculate(BaseBuild* srcBuild, eCampType type, int num, int isLastRow, int formationID)
{
	if (type == mBuildInfo.camp)
	{
		char params[64] = { 0, };
		sprintf(params, "%d %d %d %d %d", num, getKeyID(), srcBuild->getKeyID(), isLastRow, formationID);
		sgMessageManager()->sendMessage(EMT_TRANSFORM, params);
	}
	else
	{
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d %d %d", type, mBuildInfo.camp, num, getKeyID(), srcBuild->getKeyID(), isLastRow, formationID);
		sgMessageManager()->sendMessage(EMT_FIGHT, params);
	}


}


bool BaseBuild::checkCurBeAttack()
{
	return sgBattleControl()->checkBuildBeAttack(getKeyID(), getCamp());
}


bool BaseBuild::checkAttacking()
{
	eCampType ecamp = this->getBuildInfo().camp;
	Camp* pCamp = sgBattleControl()->getTargetCamp(ecamp);

	const Vect_Formation& form = pCamp->getFormationList();
	for (auto it = form.begin(); it != form.end(); ++it)
	{
		auto pFormation = it->second;
		if (nullptr == pFormation)
			continue;

		if (pFormation->getSrcBuildUid() == getKeyID())
			return true;
	}
	return false;
}

bool BaseBuild::doFight(eCampType attackCamp, int attackNum)
{
	Camp* pAtkCamp = sgBattleControl()->getTargetCamp(attackCamp);
	if (!pAtkCamp)
		return false;

	Camp* pDefCamp = sgBattleControl()->getTargetCamp(getCamp());
	if (!pDefCamp)
		return false;

	float attackPercent = 100.0f;
	float defendPercent = 100.0f;

	attackPercent += pAtkCamp->getAsstAtkAddition();

	Glyph* pGlyph = nullptr;
	pGlyph = pAtkCamp->getGlyph(Glyph::E_GLYPH_ATK_POWER);
	if (pGlyph)
	{
		attackPercent += pGlyph->getValueAsAddition();
	}

	pGlyph = pAtkCamp->getGlyph(Glyph::E_GLYPH_ATK_TIME);
	if (pGlyph)
	{
		attackPercent += pGlyph->getValueAsAddition();
	}

	pGlyph = pAtkCamp->getGlyph(Glyph::E_GLYPH_ATK_KILL);
	if (pGlyph)
	{
		auto killCount = pAtkCamp->getKillPeople();
		auto mainValue = pGlyph->getValue();
		auto viceValue = pGlyph->getDataViceAttr();

		if (viceValue > 0)
		{
			auto val1 = killCount * mainValue / viceValue;
			auto val2 = mainValue * 20;

			auto finalVal = (val1 < val2 ? val1 : val2);
			CCLOG("[Evix]BaseBuild::doFight : killcount = %d, val = %f", killCount, finalVal);
			attackPercent += finalVal;
		}
	}

	pGlyph = pDefCamp->getGlyph(Glyph::E_GLYPH_DEF_DEFENSE);
	if (pGlyph)
	{
		defendPercent += pGlyph->getValueAsAddition();
	}

	pGlyph = pDefCamp->getGlyph(Glyph::E_GLYPH_DEF_TIME);
	if (pGlyph)
	{
		defendPercent += pGlyph->getValueAsAddition();
	}

	pGlyph = pDefCamp->getGlyph(Glyph::E_GLYPH_DEF_DEAD);
	if (pGlyph)
	{
		auto deadCount = pDefCamp->getDeadPeople();
		auto mainValue = pGlyph->getValue();
		auto viceValue = pGlyph->getDataViceAttr();

		if (viceValue > 0)
		{
			auto val1 = deadCount * mainValue / viceValue;
			auto val2 = mainValue * 20;

			auto finalVal = (val1 < val2 ? val1 : val2);
			CCLOG("[Evix]BaseBuild::doFight : deadcount = %d, val = %f", deadCount, finalVal);
			defendPercent += finalVal;
		}
	}

	float atkAdd = 0.0f;
	float defAdd = 0.0f;


	// Institue building attack addition

	int iAtkInstNum = pAtkCamp->getInstituteNum();
	const InsistuteAddAttr* pAttackIAA = sgTableManager()->getInsistuteAddAttr(iAtkInstNum);
	if (pAttackIAA)
	{
		atkAdd += pAttackIAA->addAttack;

		Glyph* pGlyphInsAttack = pAtkCamp->getGlyph(Glyph::E_GLYPH_ATK_INSTITUTE);
		if (pGlyphInsAttack)
		{
			atkAdd += pGlyphInsAttack->getValueAsAddition() * iAtkInstNum;
		}
	}


	// Building defend addition	
	const BuildAttr* tAttr = sgTableManager()->getBuildAttr(mBuildInfo.id);
	if (tAttr)
	{
		defAdd += tAttr->addDef;
	}

	// Institue building defend addition
	int iDefInstNum = pDefCamp->getInstituteNum();
	const InsistuteAddAttr* pDefendIAA = sgTableManager()->getInsistuteAddAttr(iDefInstNum);
	if (pDefendIAA)
	{
		defAdd += pDefendIAA->addDefence;

		Glyph* pGlyphInsDefend = pDefCamp->getGlyph(Glyph::E_GLYPH_DEF_INSTITUTE);
		if (pGlyphInsDefend)
		{
			defAdd += pGlyphInsDefend->getValueAsAddition() * iDefInstNum;
		}
	}

	attackPercent = sgBuffManager()->getBuffAttack(pAtkCamp->getCampType(), attackPercent + atkAdd);
	defendPercent = sgBuffManager()->getBuffDefence(pDefCamp->getCampType(), defAdd + defendPercent);

	// Ratio of attack addition and defend addition
	auto ratio = attackPercent / defendPercent;

	bool battleResult = (attackNum - static_cast<int>(ceilf(mOwnPeople / ratio))) > 1;
	if (battleResult)
	{
		auto remainNum = attackNum - static_cast<int>(ceilf(mOwnPeople / ratio));
		if (!sgBattleRecordsManager()->getIsPlayback())
		{
			pAtkCamp->addKillPeople(mOwnPeople);
			pDefCamp->addDeadPeople(mOwnPeople);
		}
		addMinusOwnPeople(-mOwnPeople);
		addMinusOwnPeople(remainNum);
	}
	else
	{
		float fDefencePeople = attackNum * ratio;
		addMinusOwnPeople(-fDefencePeople);
		if (!sgBattleRecordsManager()->getIsPlayback())
		{
			pAtkCamp->addKillPeople(fDefencePeople);
			pDefCamp->addDeadPeople(fDefencePeople);
		}


		sgMoraleManager()->registMoraleMessage(getCamp(), ceilf(abs(attackNum)), E_MORALECOMPUTE_DEFENCE, getKeyID());
	}

	if (mActionStatu)
	{
		if (getCamp() != ECAMP_TYPE_Neutral)
		{
			mActionStatu->change((eCampType)attackCamp, getOwnPeople());
		}
	}

	//如果被打的是中立建筑则尝试触发引导
	if (getCamp() == ECAMP_TYPE_Neutral)
	{
		if (!sgGuideManager()->checkGuideTypeFinished(E_GUIDELOC_E_ATKED_N))
		{ 
			sgGuideManager()->unRegistGuideMessage();
			sgBattleControl()->getBattleView()->activeGuide(E_GUIDELOC_E_ATKED_N);
		} 
	}
	 
	return battleResult;

}

void BaseBuild::doDegrade()
{
	//int min = 1;
	//setLevel(MAX(min, --mLevel));
	if (mLevel - 1 < 1)
		setLevel(1);
	else
		setLevel(mLevel - 1);
	mBuildInfo.id = mBuildInfo.id / 1000 * 1000 + (getBuildType() == EBULID_TYPE_INSISTUTE ? 0 : mLevel);

	updateProductCycle();
}

void BaseBuild::update()
{
	//战斗信息状态转换计时器
	if (sgTimeCalculator()->getTimeLeft(StringUtils::format("battleTip_%s", getKeyString().c_str())) == 0)
	{
		mEnableBattleTip = true;
	}

	float upTime = sgTimeCalculator()->getTimeLeft(StringUtils::format("upgrade_%s", getKeyString().c_str()));
	if (upTime == 0)
	{

		const BuildIns* pIns = sgTableManager()->getBuildIns(getBuildInfo().insID);
		sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("upgrade_%s", getKeyString().c_str()));
		if (pIns == nullptr || pIns->levelLimit <= mLevel || mLevel < 0)
		{
			return;
		}

		std::string sGlyphTip;

		auto pGlyph = sgBattleControl()->getGlyphBattleMgr()->getCampGlyph(getCamp(), Glyph::E_GLYPH_PPL_UP);
		if (pGlyph)
		{
			float fValue = 0.0f;

			if (pGlyph->isAffectedByBuildLevel())
			{
				fValue = pGlyph->getValue() * pGlyph->getDataBuildAffectAttr(getLevel()) / 100.0f;
			}
			else
			{
				fValue = pGlyph->getValue();
			}

			addMinusOwnPeople(fValue);
			sGlyphTip = StringUtils::format(pGlyph->getDataBattleTip().c_str(), (int)fValue);
		}

		//升级完成阵营增加士气
		sgMoraleManager()->registMoraleMessage(getCamp(), getUpgradeBuildMoraleValue(), E_MORALECOMPUTE_NORMAL, getKeyID());

		//升级完成提示信息
		//加入战斗提示消息发送 by edward 2015-09-23
		if (mEnableBattleTip && getCamp() == sgBattleControl()->getOwnCamp())
		{
			char tPositionParam[256] = { 0, };
			if (getBuildType() == EBULID_TYPE_BARRACK)
			{
				sprintf(tPositionParam, "%d %f %f %s", E_COMPLETE_UPGRADE_BARRACK, mBuildView->getPositionX(), mBuildView->getPositionY() + 30,sGlyphTip.c_str());
			}
			else if (getBuildType() == EBULID_TYPE_TURRET || getBuildType() == EBUILD_TYPE_STABLES)
			{
				sprintf(tPositionParam, "%d %f %f %s", E_COMPLETE_UPGRADE_TURRET, mBuildView->getPositionX(), mBuildView->getPositionY() + 80, sGlyphTip.c_str());
			}
			sgMessageManager()->sendMessage(EMT_BATTLE_TIP, tPositionParam);

			mEnableBattleTip = false;
			sgTimeCalculator()->createTimeCalcultor(StringUtils::format("battleTip_%s", getKeyString().c_str()), 2.0f);
		}

		setLevel(mLevel + 1);
		mBuildInfo.id = mBuildInfo.id / 1000 * 1000 + mLevel;
		//升级完成
		updateProductCycle();
		mBuildView->updateUpgradePrg(getCamp(), 0);
		mBuildView->playLeveledArmature();

	}
	else
	{
		if (upTime > 0)
		{
			float upNeedTime = sgTimeCalculator()->getTimeLength(StringUtils::format("upgrade_%s", getKeyString().c_str()));// sgTableManager()->getBuildAttr(getAttrID());


			int percent = upTime / upNeedTime * 100;
			mBuildView->updateUpgradePrg(getCamp(), percent);
		}
	}

	if (!isCurUpgrade() && getBuildType() != EBULID_TYPE_INSISTUTE && getBuildType() != EBULID_TYPE_CREEPS)//pvp 升级命令
	{
		if (mServerUpLevel > getLevel())
		{
			doUpgrade(false);
			mServerUpLevel = 0;
		}
		//else if (mServerUpLevel == getLevel())
		//{

		//	doUpgrade(false);
		//	mServerUpLevel = 0;
		//}
		//else if (mServerUpLevel == -1)
		//{

		//	doUpgrade(false);
		//	mServerUpLevel = 0;
		//}
	}

	checkBuildChange();

	
	product();

	if (mActionStatu && getBuildType() == EBULID_TYPE_BARRACK)
	{
		mActionStatu->update(mOwnPeople, getCamp());

		if (mActionStatu->getCurStatus() == BUILD_GOOD_INJURE)
		{
			mBuildView->playAni(BUILD_ANI_SMALL_CRY);
		}
		else if (mActionStatu->getCurStatus() == BUILD_BAD_INJURE)
		{
			mBuildView->playAni(BUILD_ANI_BIG_CRY);
		}
		else if (mActionStatu->getCurStatus() == BUILD_NONE)
		{
			if (mActionStatu->play())
			{
				mBuildView->playAni(BUILD_ANI_STAND);
				mActionStatu->resetPlay();
			}
		}
	}


	std::string skey = StringUtils::format("build_Invisibility_%d", getKeyID());
	if (sgTimeCalculator()->hasKey(skey))
	{
		float ftime = sgTimeCalculator()->getTimeLeft(skey);
		if (ftime == 0)
		{
			delete mInviInfo;
			mInviInfo = nullptr;
			mBuildView->updateUI();
			sgTimeCalculator()->removeTimeCalcultor(skey);
			mBuildView->removeInvisibilityEffect();
		}

	}
}

eBulidType BaseBuild::getBuildType()
{
	return (eBulidType)mBuildInfo.type;
}

void BaseBuild::doUpgrade(bool local /* = false */)
{
	if (checkCanUpgrade())
	{

		if (sgBattleControl()->getIsPvp() && local)//发送pvp升级
		{
			c2sUpgrade();
		}

		mBuildView->playLevelingArmature();

		const BuildAttr* pAttr = sgTableManager()->getBuildAttr(getAttrID());
		int need = pAttr->upNeedPro;

		float fUpTime = pAttr->upNeedTime;
		auto pGlyph = sgBattleControl()->getGlyphBattleMgr()->getCampGlyph(getCamp(), Glyph::E_GLYPH_SPD_UP);
		if (pGlyph)
		{
			float fAffect = pGlyph->getDataBuildAffectAttr(getLevel());
			fUpTime -= pGlyph->getValueAsAddition() * fAffect / 100.0f;
		}

		addMinusOwnPeople(need * -1);
		//完成新手引导升级
		if (sgBattleControl()->getIsPvp() == false)
		{
			sgGuideManager()->unRegistGuideMessage(false, E_GuideOperator_DouClick);
		}
		mActionStatu->reset();
		sgTimeCalculator()->createTimeCalcultor(StringUtils::format("upgrade_%s", getKeyString().c_str()), fUpTime);

	}
}

bool BaseBuild::isCurUpgrade()
{
	if (sgTimeCalculator()->hasKey(StringUtils::format("upgrade_%s", getKeyString().c_str())))
	{
		return true;
	}

	return false;
}

void BaseBuild::cancelUpgrade()
{
	sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("upgrade_%s", getKeyString().c_str()));
}

BaseBuild* BaseBuild::copyWithType(eBulidType type)
{
	int level = MIN(MAX(getLevel(), 1), mBuildInfo.levelLimit);
	mBuildInfo.type = type;
	mBuildInfo.id = mBuildInfo.type * 1000 + (type == EBULID_TYPE_INSISTUTE ? 0 : level);

	BaseBuild* pBuild = createBuild(mBuildInfo, true);
	if (pBuild)
	{
		pBuild->mBuildInfo.locationID = this->getBuildInfo().locationID;
		pBuild->mBuildID = mBuildID;
		cleanTimeCalcultor();
		pBuild->init();
		pBuild->mBuildView = this->mBuildView;
		pBuild->mLevel = type == EBULID_TYPE_INSISTUTE ? 0 : level;
		pBuild->mBuildInfo.id = mBuildInfo.id;
		pBuild->mCurentAni = this->mCurentAni;
		pBuild->mOwnPeople = this->mOwnPeople;
		pBuild->mStateList = mStateList;
		pBuild->mDirtyTag = mDirtyTag;
		pBuild->mBeHurtSoundId = mBeHurtSoundId;
		pBuild->mIsPlayingBeHurtMusic = mIsPlayingBeHurtMusic;
		pBuild->mElapsedTime = mElapsedTime;

		pBuild->mVisible = mVisible;
		pBuild->mEnable = mEnable;

		mCoypDestroy = true;

		if (pBuild->getBuildView()->mSpriteType)
			pBuild->getBuildView()->mSpriteType->removeAllChildrenWithCleanup(true);

		return pBuild;
	}
	return NULL;
}


BaseBuild* BaseBuild::copyWithTypeWithoutView(eBulidType type)
{
	mBuildInfo.type = type;
	mBuildInfo.id = mBuildInfo.type * 1000 + (type == EBULID_TYPE_INSISTUTE ? 0 : mLevel);
	BaseBuild* pBuild = createBuild(mBuildInfo, true);
	if (pBuild)
	{
		pBuild->mBuildInfo.locationID = this->getBuildInfo().locationID;
		pBuild->mBuildID = mBuildID;
		cleanTimeCalcultor();
		pBuild->init();
		pBuild->initView();
		pBuild->mLevel = 1;
		pBuild->mBuildInfo.id = mBuildInfo.id;
		pBuild->mCurentAni = this->mCurentAni;
		pBuild->mOwnPeople = this->mOwnPeople;
		pBuild->mStateList = mStateList;
		pBuild->mDirtyTag = mDirtyTag;
		pBuild->mBeHurtSoundId = mBeHurtSoundId;
		pBuild->mIsPlayingBeHurtMusic = mIsPlayingBeHurtMusic;
		pBuild->mElapsedTime = mElapsedTime;

		pBuild->mVisible = mVisible;
		pBuild->mEnable = mEnable;
		mCoypDestroy = true;

		return pBuild;
	}
	return NULL;
}

int BaseBuild::getLevel()
{
	return mLevel;
}

int BaseBuild::getOwnPeople() const
{
	return mOwnPeople;
}

bool BaseBuild::checkCanChange(eBulidType type)
{
	return true;
}
void BaseBuild::setContentSize(Size size)
{
	//mBuildInfo.width = size.width;
	//mBuildInfo.height = size.height;
}

void BaseBuild::cleanTimeCalcultor()
{
	sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("upgrade_%s", getKeyString().c_str()));
	sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("change_%s", getKeyString().c_str()));
	sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("attact_%s", getKeyString().c_str()));


}

void BaseBuild::s2cLevel(int level)
{
	if (level != mLevel)
	{
		const BuildIns* pIns = sgTableManager()->getBuildIns(getBuildInfo().insID);
		if (pIns && level > 0 && level <= pIns->levelLimit)
		{
			setLevel(level);
			mBuildInfo.id = mBuildInfo.id / 1000 * 1000 + (getBuildType() == EBULID_TYPE_INSISTUTE ? 0 : mLevel);
		}
	}
}

void BaseBuild::s2cCamp(int camp)
{
	if (camp != mBuildInfo.camp)
	{
		mBuildInfo.camp = (eCampType)camp;
		if (mBuildInfo.camp != ECAMP_TYPE_Creeps)
		{
			mBuildInfo.express = (eExpressType)camp;
		}
		resetFormationSendingList();
		setDirtyTag(EBDIR_CAMEP);
	}
}


void BaseBuild::s2cNum(int num)
{
	if (num != mOwnPeople)
	{
		mOwnPeople = num;
		setDirtyTag(EBDIR_PEOPLE);
	}
}

void BaseBuild::s2cType(int type)
{
	eBulidType eType = (eBulidType)type;
	if (eType != getBuildType())
	{
		mBuildInfo.type = type;
		mBuildInfo.id = mBuildInfo.type * 1000 + (type == EBULID_TYPE_INSISTUTE ? 0 : mLevel);
		setDirtyTag(EBDIR_TYPE);
	}
}

void BaseBuild::s2cFormationList(std::set<int>& fList)
{
	mTarFormationIdList.clear();
	mDiffList.clear();
	mTarFormationIdList.insert(fList.begin(), fList.end());
}

void BaseBuild::recordSetLevel(int level)
{
	const BuildIns* pIns = sgTableManager()->getBuildIns(mBuildInfo.insID);
	if (NULL == pIns)
	{
		if (level > 0 && level <= pIns->levelLimit && level != mLevel)
		{
			setLevel(level);
		}
	}
}

void BaseBuild::recordSetCamp(int targetCamp)
{
	if (targetCamp != mBuildInfo.camp)
	{
		auto pSrcCamp = sgBattleControl()->getTargetCamp(eCampType(mBuildInfo.camp));
		if (pSrcCamp)
		{
			pSrcCamp->removeBuild(getKeyID());
		}

		auto pTargetCamp = sgBattleControl()->getTargetCamp(eCampType(targetCamp));
		if (pTargetCamp)
		{
			pTargetCamp->occupyBuild(this);
		}
		
		char szParam[64] = { 0 };
		sprintf(szParam, "%d %d %d", getKeyID(), targetCamp, mBuildInfo.camp);
		sgMessageManager()->sendMessage(EMT_BUILD_DESTORY, szParam);
	}
}

void BaseBuild::recordSetPeopleCount(int num)
{
	if (num != mOwnPeople)
	{
		mOwnPeople = num;
		setDirtyTag(EBDIR_PEOPLE);
	}
}
float BaseBuild::getUpgradeBuildMoraleValue()
{
	const BuildAttr* pAttr = sgTableManager()->getBuildAttr(getAttrID());
	if (NULL == pAttr)
	{
		return 0;
	}
	return pAttr->upAddMorale;
}

void BaseBuild::playJungleBorn()
{
	mBuildView->playCreepsBornAni();
}

void BaseBuild::playJungleDead()
{
	mBuildView->playCreepsDeadAni();
}

void BaseBuild::playJungleBuff(const char* armatureName)
{

}

float BaseBuild::getOccupyBuildMoraleValue()
{
	const BuildAttr* pAttr = sgTableManager()->getBuildAttr(getAttrID());
	if (NULL == pAttr)
	{
		return 0;
	}
	return pAttr->holdAddMorale;
}

float BaseBuild::getLostBuildMoraleValue()
{
	const BuildAttr* pAttr = sgTableManager()->getBuildAttr(getAttrID());
	if (NULL == pAttr)
	{
		return 0;
	}
	return pAttr->lostAddMorale;
}

void BaseBuild::addBuffView(Layer* layer)
{
	mBuildView->addChild(layer, 999);
}

void BaseBuild::removeBuffView(Layer* layer)
{
	mBuildView->removeChild(layer, true);
}


void BaseBuild::insertTarFormationId(int id)
{
	mTarFormationIdList.insert(id);
}

void BaseBuild::setVisible(bool val)
{
	mVisible = val;

	if (mBuildView)
		mBuildView->setHide(val);

	if (mBuildInfo.mapObj)
		mBuildInfo.mapObj->setVisible(val);
}


void BaseBuild::setEnable(bool val)
{
	mEnable = val;
}


void BaseBuild::addStatus(eBuildStatus status)
{
	mStatus.set(status);
}


void BaseBuild::removeStatus(eBuildStatus status)
{
	mStatus.clear(status);
}


bool BaseBuild::checkStatus(eBuildStatus status)
{
	return mStatus.test(status);
}

void BaseBuild::addDiffList(int id)
{
	std::map<int, int > ::iterator it = mDiffList.find(id);
	if (it != mDiffList.end())
	{
		if (it->second >= 2)
		{
			std::set<int>::iterator sIt = mTarFormationIdList.find(it->first);
			if (sIt == mTarFormationIdList.end())
			{
				mTarFormationIdList.insert(it->first);
				sgPvpControl()->setPvpTestStr(StringUtils::format("insert diffId = %d \n", it->first));
			}
			else
			{
				sgPvpControl()->setPvpTestStr(StringUtils::format("insert diffId = %d \n", *sIt));
				mTarFormationIdList.erase(sIt);
			}

			mDiffList.erase(it);
		}
		else
		{
			it->second = it->second + 1;
		}
	}
	else
	{
		mDiffList.insert(std::make_pair(id, 1));
	}
}

void BaseBuild::insertComeInFormationId(int id)
{
	std::set<int>::iterator it = mComeInList.find(id);
	if (it == mComeInList.end())
	{
		mComeInList.insert(id);
	}
}

void BaseBuild::removeComeInFormationId(int id)
{
	std::set<int>::iterator it = mComeInList.find(id);

	if (it != mComeInList.end())
	{
		mComeInList.erase(it);
	}
	else
	{
		CCLOG("[trace]BaseBuild:: no this id = %d comein", id);
	}
}

bool BaseBuild::isComeIngFormation()
{
	return mComeInList.size() > 0;
}

void BaseBuild::setServerUpLevel(int level)
{
	mServerUpLevel = level;
}

int BaseBuild::getServerUpLevel()
{
	return mServerUpLevel;
}


unsigned int BaseBuild::getOnWayFormation(int dstBuild) const
{
	auto itrFind = mFormationSendingList.find(dstBuild);
	if (mFormationSendingList.end() != itrFind)
		return itrFind->second;

	return 0;
}


void BaseBuild::addSendingFormation(int dstBuildId, unsigned int formationId)
{
	mFormationSendingList.insert(std::make_pair(dstBuildId, formationId));
}


void BaseBuild::removeSendingFormation(int dstBuildId, unsigned int formationId)
{
	mFormationSendingList.erase(dstBuildId);
}


void BaseBuild::resetFormationSendingList()
{
	mFormationSendingList.clear();
}

void BaseBuild::updateBuildTypeByTime(float time, std::string key, eBulidType type, bool isNew)
{
	if (time == 0)
	{
		if (type != (eBulidType)mBuildInfo.type)
		{
			sgTimeCalculator()->removeTimeCalcultor(key);

			char params[64] = { 0, };
			sprintf(params, "%d %d %d", getKeyID(), type, isNew ? 1 : 0);
			sgMessageManager()->sendMessage(EMT_PROP_CHANGEOK, params);
			mBuildView->setTouch(true);
			mBuildView->updateUI();
		}
	}
	else
	{
		if (time > 0)
		{
			ePropType propType;

			if (type == eBulidType::EBULID_TYPE_BARRACK)
				propType = ePropType::PROP_TYPE_EXCHANGE_B;
			else if (type == PROP_TYPE_EXCHANGE_T)
				propType = ePropType::PROP_TYPE_EXCHANGE_T;
			else if (type == PROP_TYPE_EXCHANGE_I)
				propType = ePropType::PROP_TYPE_EXCHANGE_I;

			const PropInfo* pProp = sgTableManager()->getPropInfoById(sgTableManager()->getPropIdByType(propType));
			if (!pProp) return;

			int percent = time / (float)pProp->duration * 100;
			mBuildView->setTouch(false);
			mBuildView->setExchangeProgress(percent);
			mBuildView->setArrowVisible(false);
		}
	}
}

void BaseBuild::checkBuildChange()
{
	for (int i = eBulidType::EBULID_TYPE_BARRACK; i < eBulidType::EBULID_TYPE_CREEPS; i++)
	{
		std::string key = StringUtils::format("change_%s_%d", getKeyString().c_str(), i);
		if (sgTimeCalculator()->hasKey(key))
		{
			float changeTime = sgTimeCalculator()->getTimeLeft(key);
			updateBuildTypeByTime(changeTime, key, (eBulidType)i, false);
		}

		key = StringUtils::format("exchange_%s_%d", getKeyString().c_str(), i);
		if (sgTimeCalculator()->hasKey(key))
		{
			float changeTime = sgTimeCalculator()->getTimeLeft(key);
			updateBuildTypeByTime(changeTime, key, (eBulidType)i, true);
		}
	}
}

void BaseBuild::getIntoBuild(eBulidType buildType, int buildID, float num)
{
	addMinusOwnPeople(num);
}

float BaseBuild::getMoveSpeed()
{
	const BuildAttr* pAttr = sgTableManager()->getBuildAttr(mBuildInfo.id);
	return pAttr->moveSpeed;
}

void BaseBuild::removeInvisibility()
{
	std::string skey = StringUtils::format("build_Invisibility_%d", getKeyID());
	if (sgTimeCalculator()->hasKey(skey))
	{
		if (mInviInfo)
		{
			delete mInviInfo;
			mInviInfo = nullptr;
		}

		sgTimeCalculator()->removeTimeCalcultor(skey);
		mBuildView->removeInvisibilityEffect();
	}
}

void BaseBuild::c2sUpgrade()
{
	int level = getLevel() + 1;
	sgPvpControl()->getPvPProxy()->sendUpgrade(getKeyID(), level);
}

/////////////////////////////////Barracks/////////////////////////////////////////

Barracks::Barracks()
{

}

Barracks::~Barracks()
{

}

void Barracks::init()
{
	BaseBuild::init();
	updateProductCycle();
}

void Barracks::doAIState()
{



}

void Barracks::updateUI()
{
	BaseBuild::updateUI();
	//product();
}

void Barracks::product()
{
	// 	if(getCamp() == ECAMP_TYPE_Player)
	// 	{
	// 		CCLOG("12345");
	// 	}
	bool flag = false;

	// 阵营总人口限制
	Camp* pCamp = sgBattleControl()->getTargetCamp(mBuildInfo.camp);
	if (pCamp && pCamp->getAllUnit() >= pCamp->getCampMaxProduct() && getBuildType() == EBULID_TYPE_BARRACK)
	{
		flag = true;
	}


	// 建筑人口限制
	const BuildAttr* pAttr = sgTableManager()->getBuildAttr(mBuildInfo.id);
	if (pAttr && mOwnPeople >= getMaxProduct())
	{
		flag = true;
	}


	// 中立建筑人口只增长到初始值
	const BuildIns* pIns = sgTableManager()->getBuildIns(mBuildInfo.insID);
	if (mBuildInfo.camp == ECAMP_TYPE_Neutral && pIns && pIns->initPeople <= mOwnPeople)
	{
		flag = true;
	}


	if (!flag)
	{

		if (sgTimeCalculator()->getTimeLeft(StringUtils::format("product_%s", getKeyString().c_str())) == 0)
		{
			float fProSpeed = pAttr->proSpeed;
			auto pGlyph = pCamp->getGlyph(Glyph::E_GLYPH_SPD_PRODUCE);
			if (pGlyph)
			{
				fProSpeed += pAttr->proSpeed * pGlyph->getValueAsPercent();
			}

			addMinusOwnPeople(1);
			float speed = sgBuffManager()->getBuffProduceSpeed(getCamp(), 1.0f / fProSpeed);
			float time = speed > 0 ? (1.0f / speed) : pAttr->proSpeed;

			sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("product_%s", getKeyString().c_str()));
			sgTimeCalculator()->createTimeCalcultor(StringUtils::format("product_%s", getKeyString().c_str()), time);

			sgMessageManager()->sendMessage(EMT_PRODUCT, getKeyString());

			pGlyph = pCamp->getGlyph(Glyph::E_GLYPH_PPL_PRODUCE);
			if (pGlyph)
			{
				float fValue = pGlyph->getValueAsAddition();
				addMinusOwnPeople(fValue);
				pCamp->addProductPeople(fValue);
			}

		}
	}
}

void Barracks::updateProductCycle()
{
	// 兵营订制生产兵的定时器
	const BuildAttr* pAttr = sgTableManager()->getBuildAttr(mBuildInfo.id);
	if (pAttr)
	{
		if (pAttr->proSpeed <= 0.0f)
		{
			return;
		}
		float time;
		Camp* pCamp = sgBattleControl()->getTargetCamp(getCamp());
		if (NULL != pCamp)
		{
			float fProSpeed = pAttr->proSpeed;
			auto pGlyph = pCamp->getGlyph(Glyph::E_GLYPH_SPD_PRODUCE);
			if (pGlyph)
			{
				fProSpeed += pAttr->proSpeed * pGlyph->getValueAsPercent();
			}

			float speed = sgBuffManager()->getBuffProduceSpeed(getCamp(), 1.0f / fProSpeed);
			time = speed > 0 ? (1.0f / speed) : pAttr->proSpeed;
		}
		else
		{
			time = pAttr->proSpeed;
		}

		sgTimeCalculator()->createTimeCalcultor(StringUtils::format("product_%s", getKeyString().c_str()), time);
	}
}

bool Barracks::doFullPeopel()
{
	addMinusOwnPeople(50);
	return true;
}

Turret::Turret() : mAdditionAtkSpeed(0.0f)
{

}

Turret::~Turret()
{

}

void Turret::init()
{
	BaseBuild::init();
	mAtkCount = 0;

	this->resetTimer();
}

float Turret::getBaseAtkSpeed()
{
	float baseSpeed = 0.0f;
	const BuildAttr* pAttr = sgTableManager()->getBuildAttr(mBuildInfo.id);
	if (pAttr && pAttr->attSpeed > 0.0f)
		baseSpeed = pAttr->attSpeed;
	return baseSpeed;
}

void Turret::resetTimer()
{
	float fBaseSpeed = this->getBaseAtkSpeed();
	float fAtkSpeed = fBaseSpeed;
	auto pGlyph = sgBattleControl()->getGlyphBattleMgr()->getCampGlyph(getCamp(), Glyph::E_GLYPH_ATK_TURRET);
	if (pGlyph)
	{
		fAtkSpeed += fBaseSpeed * pGlyph->getValueAsPercent();
	}

	fAtkSpeed += mAdditionAtkSpeed;

	float finalSpeed = sgBuffManager()->getBuffAttackSpeed(getCamp(), fAtkSpeed);
	finalSpeed = finalSpeed > 0 ? finalSpeed : fBaseSpeed;

	float time = 1.0f / finalSpeed;
	sgTimeCalculator()->createTimeCalcultor(StringUtils::format("attact_%s", getKeyString().c_str()), time);
}

bool Turret::doFullPeopel()
{
	addMinusOwnPeople(50);
	return true;
}

void Turret::update()
{
	//float upTime = sgTimeCalculator()->getTimeLeft(StringUtils::format("upgrade_%s", getKeyString().c_str()));
	//if (upTime == 0)
	//{
	/*if (mLevel == 5)
	mLevel += getStep();*/
	//}

	BaseBuild::update();

	setIsleveling(false);
	if (mLevel == 5)
		mLevel += getStep();

	if (mLevel > 5)
	{
		if (!mBuildView->mSpriteType->isVisible())
		{
			mBuildInfo.id = mBuildInfo.id / 1000 * 1000 + mLevel;
			std::string path = mLevel == 6 ? "ui/battle/Battle_button_3_2.png" : "ui/battle/Battle_button_3_3.png";
			Sprite* node = Sprite::createWithSpriteFrameName(path);
			node->setPosition(10, 12);
			node->setAnchorPoint(Point::ZERO);
			mBuildView->mSpriteType->removeAllChildrenWithCleanup(true);
			mBuildView->mSpriteType->addChild(node);
			mBuildView->mSpriteType->setVisible(true);
		}
	}

	if (sgTimeCalculator()->getTimeLeft(StringUtils::format("attact_%s", getKeyString().c_str())) == 0)
	{
		if (this->getBuildType() == EBULID_TYPE_TURRET)
		{
			char params[64] = { 0, };
			sprintf(params, "%d", getKeyID());
			sgMessageManager()->sendMessage(EMT_TOWER_ATTACK, params);

			this->resetTimer();
		}
	}
}

void Turret::UpgradeAI(bool local /*= true*/)
{
	int rand = cocos2d::random(1, 2);

	switch (rand)
	{
	case 1:
		setStep(rand);
		break;
	case 2:
		setStep(rand);
	default:
		break;
	}
	Upgrade(true);
}

void Turret::Upgrade(bool local)
{
	BaseBuild::doUpgrade(local);
}

void Turret::doUpgrade(bool local /*= true*/)
{
	if (sgBattleControl()->getTargetCamp(getCamp())->checkIsAI())
	{
		UpgradeAI();
		return;
	}

	if (local == false)
	{
		if (mServerUpLevel == 6 || mServerUpLevel == 7)
		{
			int step = mServerUpLevel - getLevel() - 1;
			setStep(step);
		}

		Upgrade(false);
	}
	else
	{
		mLevel == 4 ? selectChooseUpgrade() : Upgrade(local);
	}
}

void Turret::selectChooseUpgrade()
{
	mBuildView->showChooseLevel(true);
}

void Turret::doDegrade()
{
	if (mLevel - 1 < 1)
		setLevel(1);
	else if (mLevel > 5)
	{
		setLevel(4);
		mBuildView->mSpriteType->setVisible(false);
	}
	else
		setLevel(mLevel - 1);

	mBuildInfo.id = mBuildInfo.id / 1000 * 1000 + (getBuildType() == EBULID_TYPE_INSISTUTE ? 0 : mLevel);

	if (sgBattleControl()->getTargetCamp(getCamp())->checkIsAI())
	{
		UpgradeAI();
		return;
	}

	updateProductCycle();
}

void Turret::c2sUpgrade()
{
	int iStep = 0;
	if (mStep == 1 || mStep == 2)
	{
		iStep += mStep;
	}

	int level = getLevel() + 1 + iStep;
	sgPvpControl()->getPvPProxy()->sendUpgrade(getKeyID(), level);
}

void Turret::doAdditonAtkSpeed(float speed, bool op)
{
	if (op)
		mAdditionAtkSpeed += speed;
	else
		mAdditionAtkSpeed -= speed;
}


Institute::Institute()
{

}

Institute::~Institute()
{

}

void Institute::init()
{
	BaseBuild::init();

	mLevel = 0;
}

bool Institute::doFullPeopel()
{
	addMinusOwnPeople(50);

	return true;
}

void Institute::doDegrade()
{
	setLevel(0);
	//mBuildInfo.id = mBuildInfo.id / 1000 * 1000 + (getBuildType() == EBULID_TYPE_INSISTUTE ? 0 : mLevel);
}

#define WAIT_CLEAR_TIME 2.0f
Monster::Monster()
{
	m_ShowState = 0;
	m_WaitClearTime = WAIT_CLEAR_TIME;
	mVisible = false;
}

Monster::~Monster()
{

}

void Monster::init()
{
	BaseBuild::init();
	m_BaseOwnPeople = mOwnPeople;
}

void Monster::update()
{
	BaseBuild::update();
	if (mVisible == false)
	{
		checkFormation();
	}
}

void Monster::createCheckFormationTimer()
{
	std::string sKey = CCString::createWithFormat("MonsterCheckPeople_%d", getKeyID())->getCString();
	if (sgTimeCalculator()->hasKey(sKey))
	{
		sgTimeCalculator()->removeTimeCalcultor(sKey);
	}
	sgTimeCalculator()->createTimeCalcultor(sKey, 0.5f);
}


void Monster::checkFormation()
{
	std::string sKey = CCString::createWithFormat("MonsterCheckPeople_%d", getKeyID())->getCString();
	float tTime = sgTimeCalculator()->getTimeLeft(sKey);
	if (sgTimeCalculator()->hasKey(sKey) && tTime <= 0.0f)
	{
		if (mOwnPeople == 0)
		{
			sgTimeCalculator()->removeTimeCalcultor(sKey);
			playJungleDead();
			int id = getKeyID();
			int locationID = mBuildInfo.locationID;
			mBuildView->runAction(Sequence::create(DelayTime::create(1.5f), CallFunc::create(
				[this, id, locationID](){
				char szParam1[64] = { 0, };
				sprintf(szParam1, "%d %d %d %d %d %d %d %d", id, 0, 0, 0, 0, id, locationID, E_JUNGLE_CLEAR);
				sgMessageManager()->sendMessage(EMT_JUNGLE, szParam1);
			}), NULL));
		}
		sgTimeCalculator()->resetTimeCalcultor(sKey);
	}
}

bool Monster::doFullPeopel()
{
	const BuildAttr* pAttr = sgTableManager()->getBuildAttr(mBuildInfo.id);
	if (mOwnPeople < m_BaseOwnPeople)
	{
		int num = m_BaseOwnPeople - mOwnPeople;
		addMinusOwnPeople(num);
		return true;
	}

	return false;
}

////////////////////////////Creeps//////////////////////////////////////////////
CreepsBuild::CreepsBuild() : BaseBuild()
, mCreepPointID(0)
, mCreepsID(0)
, mBuffID(0)
{
}


CreepsBuild::~CreepsBuild()
{
	sgTimeCalculator()->removeTimeCalcultor(mProductTimerKey);
	this->cleanup();
}


void CreepsBuild::init()
{
	BaseBuild::init();

	this->setVisible(false);
	this->setEnable(false);
	mBuildInfo.express = eExpressType::EXP_TYPE_Invalid;

	const BuildIns* pIns = sgTableManager()->getBuildIns(mBuildInfo.insID);
	if (nullptr == pIns)
		return;

	mCreepPointID = pIns->creepsPoint;

	mProductTimerKey = StringUtils::format("CreepBuild%d_Product", this->getKeyID());
}


void CreepsBuild::cleanup()
{
	for (auto it = mFormationOnWayList.begin(); it != mFormationOnWayList.end(); ++it)
	{
		FormationOnWayInfo* pInfo = it->second;
		if (pInfo)
			delete pInfo;
	}
	mFormationOnWayList.clear();

	mBuffID = 0;
	mCreepsID = 0;
}


void CreepsBuild::update()
{
	this->product();
}


void CreepsBuild::product()
{
	if (sgTimeCalculator()->getTimeLeft(mProductTimerKey) == 0)
	{
		this->resetPeople(mBuildInfo.unit);
	}
}


int CreepsBuild::takeBuffID()
{
	int buffID = mBuffID;
	mBuffID = 0;
	return buffID;
}


CreepsBuild::FormationOnWayInfo* CreepsBuild::getFormationOnWayInfo(int formationId) const
{
	auto itFind = mFormationOnWayList.find(formationId);
	if (mFormationOnWayList.end() != itFind)
		return itFind->second;

	return nullptr;
}


void CreepsBuild::recordOnWayFormation(int formationId, eCampType atkCamp, int srcBuildUid)
{
	if (nullptr != getFormationOnWayInfo(formationId))
		return;

	FormationOnWayInfo* pInfo = new FormationOnWayInfo();
	if (nullptr == pInfo)
		return;

	pInfo->mFormationID = formationId;
	pInfo->mAtkCamp = atkCamp;
	pInfo->mSrcBuildId = srcBuildUid;
	pInfo->mRemainCount = 0;

	mFormationOnWayList.insert(std::make_pair(formationId, pInfo));
}


void CreepsBuild::eraseOnWayFormation(int formationId)
{
	FormationOnWayInfo* pInfo = this->getFormationOnWayInfo(formationId);
	if (nullptr == pInfo)
	{
		CCLOG("[error] CreepsBuild::sendOnWayFormation : Can not found Formation: %d", formationId);
		return;
	}

	//if (pInfo->mRemainCount > 0)
	//	sgBattleControl()->doCreepsReturnSolider(this->getKeyID(), pInfo->mAtkCamp, pInfo->mSrcBuildId, formationId, pInfo->mRemainCount);

	delete pInfo;
	mFormationOnWayList.erase(formationId);
}


bool CreepsBuild::doDefend(eCampType atkCamp, eCampType defCamp, int atkCount, int formationId)
{
	Camp* pAtkCamp = sgBattleControl()->getTargetCamp(atkCamp);
	Camp* pDefCamp = sgBattleControl()->getTargetCamp(defCamp);
	if ( nullptr == pAtkCamp || nullptr == pDefCamp)
		return false;

	//Camp* pDefCamp = sgBattleControl()->getTargetCamp(mBuildInfo.camp);
	//if (!pDefCamp)
	//	return false;

	float attackPercent = 100.0f;
	float defendPercent = 100.0f;

	Glyph* pGlyph = nullptr;

	pGlyph = pAtkCamp->getGlyph(Glyph::E_GLYPH_ATK_POWER);
	if (pGlyph)
	{
		attackPercent += pGlyph->getValueAsAddition();
	}

	pGlyph = pAtkCamp->getGlyph(Glyph::E_GLYPH_ATK_TIME);
	if (pGlyph)
	{
		attackPercent += pGlyph->getValueAsAddition();
	}

	pGlyph = pAtkCamp->getGlyph(Glyph::E_GLYPH_ATK_KILL);
	if (pGlyph)
	{
		auto killCount = pAtkCamp->getKillPeople();
		auto mainValue = pGlyph->getValue();
		auto viceValue = pGlyph->getDataViceAttr();

		if (viceValue > 0)
		{
			auto val1 = killCount * mainValue / viceValue;
			auto val2 = mainValue * 20;

			auto finalVal = (val1 > val2 ? val1 : val2);
			CCLOG("[Evix]BaseBuild::doFight : killcount = %d, val = %f", killCount, finalVal);
			attackPercent += finalVal;
		}
	}

	pGlyph = pDefCamp->getGlyph(Glyph::E_GLYPH_DEF_DEFENSE);
	if (pGlyph)
	{
		defendPercent += pGlyph->getValueAsAddition();
	}

	pGlyph = pDefCamp->getGlyph(Glyph::E_GLYPH_DEF_TIME);
	if (pGlyph)
	{
		defendPercent += pGlyph->getValueAsAddition();
	}

	pGlyph = pDefCamp->getGlyph(Glyph::E_GLYPH_DEF_DEAD);
	if (pGlyph)
	{
		auto deadCount = pDefCamp->getDeadPeople();
		auto mainValue = pGlyph->getValue();
		auto viceValue = pGlyph->getDataViceAttr();

		if (viceValue > 0)
		{
			auto val1 = deadCount * mainValue / viceValue;
			auto val2 = mainValue * 20;

			auto finalVal = (val1 > val2 ? val1 : val2);
			CCLOG("[Evix]BaseBuild::doFight : deadcount = %d, val = %f", deadCount, finalVal);
			defendPercent += finalVal;
		}
	}

	float atkAdd = 0.0f;
	float defAdd = 0.0f;


	// Institue building attack addition

	int iAtkInstNum = pAtkCamp->getInstituteNum();
	const InsistuteAddAttr* pAttackIAA = sgTableManager()->getInsistuteAddAttr(iAtkInstNum);
	if (pAttackIAA)
	{
		atkAdd += pAttackIAA->addAttack;

		Glyph* pGlyphInsAttack = pAtkCamp->getGlyph(Glyph::E_GLYPH_ATK_INSTITUTE);
		if (pGlyphInsAttack)
		{
			atkAdd += pGlyphInsAttack->getValueAsAddition() * iAtkInstNum;
		}
	}


	// Building defend addition	
	const BuildAttr* tAttr = sgTableManager()->getBuildAttr(mBuildInfo.id);
	if (tAttr)
	{
		defAdd += tAttr->addDef;
	}

	// Institue building defend addition
	int iDefInstNum = pDefCamp->getInstituteNum();
	const InsistuteAddAttr* pDefendIAA = sgTableManager()->getInsistuteAddAttr(iDefInstNum);
	if (pDefendIAA)
	{
		defAdd += pDefendIAA->addDefence;

		Glyph* pGlyphInsDefend = pDefCamp->getGlyph(Glyph::E_GLYPH_DEF_INSTITUTE);
		if (pGlyphInsDefend)
		{
			defAdd += pGlyphInsDefend->getValueAsAddition() * iDefInstNum;
		}
	}

	attackPercent = sgBuffManager()->getBuffAttack(pAtkCamp->getCampType(), attackPercent + atkAdd);
	defendPercent = sgBuffManager()->getBuffDefence(pDefCamp->getCampType(), defAdd + defendPercent);

	// Ratio of attack addition and defend addition
	auto ratio = attackPercent / defendPercent;

	bool battleResult = (atkCount - static_cast<int>(ceilf(mOwnPeople / ratio))) > 1;
	if (battleResult)
	{
		auto remainNum = atkCount - static_cast<int>(ceilf(mOwnPeople / ratio));
		if (!sgBattleRecordsManager()->getIsPlayback())
		{
			pAtkCamp->addKillPeople(mOwnPeople);
			pDefCamp->addDeadPeople(mOwnPeople);
		}
		addMinusOwnPeople(-mOwnPeople);

		FormationOnWayInfo *pInfo = this->getFormationOnWayInfo(formationId);
		if (nullptr == pInfo)
		{
			CCLOG("[error] CreepsBuild::doDefend : No Onway info, FormationID %d, AtkCamp : %d", formationId, atkCamp);
		}
		else
		{
			pInfo->mRemainCount += remainNum;

			// jc.nullptr crash.
			sgBattleControl()->doCreepsReturnSolider(this->getKeyID(), pInfo->mAtkCamp, pInfo->mSrcBuildId, formationId, pInfo->mRemainCount);

			pInfo->mRemainCount = 0;
		}
	}
	else
	{
		float fDefencePeople = atkCount * ratio;
		if (!sgBattleRecordsManager()->getIsPlayback())
		{
			pAtkCamp->addKillPeople(fDefencePeople);
			pDefCamp->addDeadPeople(fDefencePeople);
		}
		addMinusOwnPeople(-fDefencePeople);
		sgTimeCalculator()->resetTimeCalcultor(mProductTimerKey);
	}

	return battleResult;
}


bool CreepsBuild::doOpen(const Creeps* pCreeps)
{
	switch (pCreeps->getType())
	{
	case 1: // 辣椒
	{
		mBuildInfo.express = eExpressType::EXP_TYPE_LaJiao;
	}
	break;

	case 2: // 大葱
	{
		mBuildInfo.express = eExpressType::EXP_TYPE_DaCong;
	}
	break;

	case 3: // 鸡蛋
	{
		mBuildInfo.express = eExpressType::EXP_TYPE_JiDan;
	}
	break;

	case 4: // 奶酪
	{
		mBuildInfo.express = eExpressType::EXP_TYPE_NaiLao;
	}
	break;

	case 5: // 蒜头
	{
		mBuildInfo.express = eExpressType::EXP_TYPE_SuanTou;
	}
	break;

	case 6: // 虾米
	{
		mBuildInfo.express = eExpressType::EXP_TYPE_XiaMi1;
	}
	break;

	default:
	{
		CCLOG("[fault]CreepsBuild::refresh : Creeps type no match : %d", pCreeps->getType());
		return false;
	}
	break;
	}

	this->cleanup();

	mCreepsID = pCreeps->getID();
	mBuffID = pCreeps->getBuffId();
	sgBattleControl()->getBattleView()->addBuildBuffEffect(this->getKeyID(), pCreeps->getEffectFile());

	this->setVisible(true);
	this->setEnable(true);
	this->resetPeople(mBuildInfo.unit);

	mBuildView->showCreepsTomb(false);
	mBuildView->changeExpress(mBuildInfo.express);
	mBuildView->playCreepsBornAni();
	mBuildView->adjustByLevel(this->getBuildType(), this->getLevel());
	mBuildView->setTag(mBuildInfo.insID);
	mBuildView->setName(StringUtils::format("%d", mBuildInfo.insID));
	sgTimeCalculator()->createTimeCalcultor(mProductTimerKey, 20.0f);

	int gateID = sgGamePrecedure()->getCurMapInfo().checkpt;
	int level = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel();
	sgGuideManager()->registGuideMessage(gateID, level, E_GUIDELOC_JUNGLE, sgBattleControl()->getBattleView(), sgBattleControl()->getBattleView(), false);
	return true;
}


bool CreepsBuild::checkOccupy()
{
	return mBuffID != 0;
}


void CreepsBuild::doOccupy()
{
	this->setEnable(false);
	sgTimeCalculator()->removeTimeCalcultor(mProductTimerKey);

	mBuildView->playCreepsDeadAni();
	sgBattleControl()->getCreepsManager()->occupyCreepPoint(mCreepPointID);
	sgBattleControl()->getBattleView()->removeBuildBuffEffect(this->getKeyID());
}


bool CreepsBuild::checkClose()
{
	return  mBuffID == 0 && mFormationOnWayList.empty();
}


void CreepsBuild::doClose()
{
	this->setVisible(false); 
	sgBattleControl()->getCreepsManager()->resetCreepPoint(mCreepPointID);

	doGuideAction();
	
}

void CreepsBuild::doGuideAction()
{
	if (!sgGuideManager()->checkGuideTypeFinished(E_GUIDELOC_OCU_JUNGLE))
	{
		auto scheduler = Director::getInstance()->getScheduler();
		scheduler->schedule(std::bind(&CreepsBuild::delayActiveCreedGuide, this,
			std::placeholders::_1, (int)E_GUIDELOC_OCU_JUNGLE),
			this, 1.0f, 0, 0.0f, false,
			"CreepsBuild::delayActiveCreedGuide");
	}
}

void CreepsBuild::delayActiveCreedGuide(float dt, int location)
{
	sgBattleControl()->getBattleView()->activeGuide(location);
}

void CreepsBuild::resetPeople(int num)
{
	this->addMinusOwnPeople(-mOwnPeople);
	this->addMinusOwnPeople(num);
}

void CreepsBuild::addCreepsPoint()
{
	sgBattleControl()->getCreepsManager()->addCreepsPoint(mCreepPointID, CC_CALLBACK_1(CreepsBuild::doOpen, this));
}

void InvisibilityInfo::formatParams(std::string parms)
{
	std::vector<int> v_params = HelperFunc::tokenize_2int(parms, "+");

	for (int i = 0; i < v_params.size(); i++)
	{
		switch (i)
		{
		case 0:
			mSend = v_params.at(i) == 1;
			break;
		case 1:
			mObtain = v_params.at(i) == 1;
			break;
		case 2:
			mUseProp = v_params.at(i) == 1;
			break;
		case 3:
			mUpgrade = v_params.at(i) == 1;
			break;
		default:
			break;
		}
	}
}



///////////////////////////////////////////

StablesBuild::StablesBuild()
{

}


void StablesBuild::init()
{
	BaseBuild::init();
	std::string key = StringUtils::format("BuildReduction_%d", mBuildInfo.insID);
	if (sgTimeCalculator()->hasKey(key))
		sgTimeCalculator()->removeTimeCalcultor(key);
	sgTimeCalculator()->createTimeCalcultor(key, 0.5f);
}

void StablesBuild::getIntoBuild(eBulidType buildType, int buildID, float num)
{
	addMinusOwnPeople(num);
}

void StablesBuild::update()
{
	BaseBuild::update();
	updatePeopleLimit();
}

bool StablesBuild::checkPeopleLimit()
{
	if (getOwnPeople() > getHoldLimit())
	{
		return true;
	}
	return false;
}

void StablesBuild::updatePeopleLimit()
{
	std::string key = StringUtils::format("BuildReduction_%d", mBuildInfo.insID);
	if (!sgTimeCalculator()->hasKey(key))
		return;
	float leftTime = sgTimeCalculator()->getTimeLeft(key);
	if (leftTime <= 0)
	{
		if (checkPeopleLimit())
			doReduce();
		sgTimeCalculator()->resetTimeCalcultor(key);
	}
}


void StablesBuild::doReduce()
{
	addMinusOwnPeople(getReduceNum());
}


float StablesBuild::getReduceNum()
{
	int iHoldLimit = getHoldLimit();

	float ret = (getOwnPeople() - iHoldLimit) * 0.2 + 1;
	float retLimit = -1 * (ret > 50 ? 50 : ret);
	if (getOwnPeople() + retLimit < iHoldLimit)
		retLimit = (iHoldLimit - getOwnPeople());
	return retLimit;
}

int StablesBuild::getHoldLimit()
{
	float fHoldLimit = 0.0f;

	const BuildAttr* pAttr = sgTableManager()->getBuildAttr(mBuildInfo.id);
	if (pAttr)
	{
		fHoldLimit += pAttr->holdLimit;
	}

	auto pGlyph = sgBattleControl()->getGlyphBattleMgr()->getCampGlyph(getCamp(), Glyph::E_GLYPH_PPL_HOLDLIMIT);
	if (pGlyph)
	{
		fHoldLimit += pGlyph->getValueAsAddition();
	}

	return fHoldLimit;
}

bool StablesBuild::doFullPeopel()
{
	addMinusOwnPeople(50);
	return true;
}

