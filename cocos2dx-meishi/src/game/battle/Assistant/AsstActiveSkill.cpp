#include "AsstActiveSkill.h"
#include "Assistant.h"
#include "game/battle/BattleControl.h"
#include "game/battle/Build.h"
#include "game/battle/BuildView.h"
#include "core/TimeCalculator.h"

/************************************************************************/
/*                     AsstActiveSkillData                              */
/************************************************************************/
AsstActiveSkillData* AsstActiveSkillData::create()
{
	auto pRet = new AsstActiveSkillData();
	if (pRet)
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

AsstActiveSkillData::AsstActiveSkillData() : mId(0)
, mName("Unknown")
, mType(AsstActiveSkill::Type::kNone)
, mCastType(AsstActiveSkill::CastType::kNone)
, mCoolTime(0.0f)
, mDelayTime(0.0f)
, mContinualTime(0.0f)
, mTarget(AsstActiveSkill::Target::kNone)
, mRange(AsstActiveSkill::Range::kNone)
, mRgMin(0)
, mRgMax(0)
, mOperator(AsstActiveSkill::Operator::kNone)
, mNumRule(AsstActiveSkill::NumRule::kNone)
, mNumType(AsstActiveSkill::NumType::kNone)
, mInitNum(0)
, mNumRange(0)
{

}

AsstActiveSkillData::~AsstActiveSkillData()
{

}


/************************************************************************/
/*                        AsstActiveSkill                               */
/************************************************************************/
const std::string AsstActiveSkill::EffAKBeg = "start";
const std::string AsstActiveSkill::EffAKIng = "stand";
const std::string AsstActiveSkill::EffAKEnd = "end";

const std::string AsstActiveSkill::EffFKCast = "atk";

int AsstActiveSkill::RandomRange(int min, int max, int rand)
{
	rand -= 1;

	if (rand < 0)
		return min;

	if (rand >= ASST_RANDOM_LIMIT)
		return max;

	if (min > max)
	{
		int temp = min;
		min = max;
		max = temp;
	}

	int result = min;

	int randIntervalCount = max - min + 1;
	if (randIntervalCount > 0 && randIntervalCount <= ASST_RANDOM_LIMIT)
	{
		float randInterval = (float)ASST_RANDOM_LIMIT / randIntervalCount;
		result += (int)(rand / randInterval);
	}

	return result;
}

AsstActiveSkill::AsstActiveSkill() : mCaster(nullptr)
, mBaseData(nullptr)
{

}

AsstActiveSkill::~AsstActiveSkill()
{
	for (auto pEffect : mEffectCache)
	{
		if (pEffect)
		{
			pEffect->release();
			pEffect = nullptr;
		}
	}
	mEffectCache.clear();

	if (!mBaseData->mEffectfile.empty())
	{
		ArmatureDataManager::getInstance()->removeArmatureFileInfo(mBaseData->mEffectfile + ".xml");
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(mBaseData->mEffectfile + ".plist");
		Director::getInstance()->getTextureCache()->removeTextureForKey(mBaseData->mEffectfile + ".png");
	}

	if (mBaseData)
	{
		mBaseData->release();
		mBaseData = nullptr;
	}

	mCaster = nullptr;
}

bool AsstActiveSkill::init(AsstActiveSkillData* pBase)
{
	if (nullptr == pBase)
	{
		CCLOG("[warning]AsstActiveSkill::init : Base data is null");
		return false;
	}

	if (!pBase->mEffectfile.empty())
	{
		auto texFile = pBase->mEffectfile + ".png";
		auto pTexture = Director::getInstance()->getTextureCache()->addImage(texFile);
		if (nullptr == pTexture)
		{
			CCLOG("[error]AsstActiveSkill::init : Effect load texture fail, name = %s.", texFile.c_str());
			return false;
		}

		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(pBase->mEffectfile + ".plist", pTexture);
		ArmatureDataManager::getInstance()->addArmatureFileInfo(pBase->mEffectfile + ".xml");
	}

	pBase->retain();
	mBaseData = pBase;

	//mRandoms.reserve(this->getRandomCount());

	return true;
}

unsigned int AsstActiveSkill::getId() const
{
	if (mBaseData)
		return mBaseData->mId;
	return 0;
}

const AsstActiveSkillData* AsstActiveSkill::getBaseData() const
{
	return mBaseData;
}

void AsstActiveSkill::setCaster(Assistant* pCaster)
{
	mCaster = pCaster;
	if (mCaster)
	{
		std::string strIdentify = StringUtils::format("%d_%d", pCaster->getCampType(), getId());
		mCoolTimerKey = "AAS_CoolTimer_" + strIdentify;
		mDelayTimerKey = "AAS_DelayTimer_" + strIdentify;
		mDelayIntervalTimerKey = "AAS_DelayIntervalTimer_" + strIdentify;
		mContinualTimerKey = "AAS_ContinualTimer_" + strIdentify;
		mIntervalTimerKey = "AAS_IntervalTimer_" + strIdentify;
	}
	else
	{
		mCoolTimerKey = "";
		mDelayTimerKey = "";
		mDelayIntervalTimerKey = "";
		mContinualTimerKey = "";
		mIntervalTimerKey = "";
	}
}

Assistant* AsstActiveSkill::getCaster() const
{
	return mCaster;
}

bool AsstActiveSkill::check() const
{
	return true;
}

void AsstActiveSkill::setRandoms(std::vector<int>& randomList)
{
	if (randomList.size() < this->getRandomCount())
	{
		CCLOG("[warning]AsstActiveSkill::setRandomList : New list's size less then request size");
		return;
	}

	mRandoms.clear();
	mRandoms.swap(randomList);
}

int AsstActiveSkill::getFinalNum(int rand /* = 0 */) const
{
	int finalNum = 0;
	if (mBaseData)
	{
		switch (mBaseData->mNumRule)
		{
		case NumRule::kAll:
		{
			finalNum = mBaseData->mInitNum + mBaseData->mNumRange;
		}
		break;

		case NumRule::kRandom:
		{
			finalNum = mBaseData->mInitNum + AsstActiveSkill::RandomRange(0, mBaseData->mNumRange, rand);
		}
		break;

		}
	}

	return finalNum;
}

int AsstActiveSkill::getFinalRange(int min, int max, int rand) const
{
	int finalRange = 0;
	if (mBaseData)
	{
		switch (mBaseData->mRange)
		{
		case Range::kALL:
		{
			finalRange = max;
		}
		break;

		case Range::kRandom:
		{
			finalRange = AsstActiveSkill::RandomRange(min, max, rand);
		}
		break;

		}
	}
	return finalRange;
}

void AsstActiveSkill::getFinalTarget(int rand, std::vector<Camp*>& outList) const
{
	if (mCaster == nullptr)
		return;	

	Camp* pCamp = nullptr;
	BattleControl *pBattleControl = sgBattleControl();

	switch (mBaseData->mTarget)
	{
	case AsstActiveSkill::Target::kSelf:
	{
		pCamp = pBattleControl->getTargetCamp(mCaster->getCampType());
		if (pCamp)
			outList.push_back(pCamp);
	}
	break;

	case AsstActiveSkill::Target::kEnermy:
	{
		std::vector<Camp*> randomList;
		auto& campList = pBattleControl->getCampList();
		for (auto cIt = campList.begin(); cIt != campList.cend(); cIt++)
		{
			auto pCamp = cIt->second;
			if (cIt->first == ECAMP_TYPE_Neutral
				|| cIt->first == ECAMP_TYPE_Creeps
				|| mCaster->getCampType() == cIt->first
				|| nullptr == pCamp)
				continue;

			if (pCamp->getBuildList().empty())
				continue;

			randomList.push_back(pCamp);
		}

		if (randomList.size() > 0)
		{
			int randCampIndex = AsstActiveSkill::RandomRange(0, int(randomList.size() - 1), rand);
			outList.push_back(randomList[randCampIndex]);
		}
	}
	break;

	case AsstActiveSkill::Target::kALL:
	{
		auto & campList = pBattleControl->getCampList();
		for (auto itr = campList.begin(); itr != campList.cend(); itr++)
		{
			auto pCamp = itr->second;
			if (itr->first == ECAMP_TYPE_Neutral
				|| itr->first == ECAMP_TYPE_Creeps
				|| nullptr == pCamp)
				continue;

			outList.push_back(pCamp);
		}
	}
	break;

	default:
		break;
	}
}

std::string AsstActiveSkill::getEffectName() const
{
	std::string effAniName;
	if (nullptr != mBaseData && !mBaseData->mEffectfile.empty())
	{
		const std::string &effectFile = mBaseData->mEffectfile;

		auto effNameBegPos = effectFile.find_last_of('/');
		if (effNameBegPos == std::string::npos)
			auto effNameBegPos = effectFile.find_last_of('\\');
		if (effNameBegPos == std::string::npos)
		{
			effAniName = effectFile;
		}
		else
		{
			effAniName = effectFile.substr(effNameBegPos + 1, effectFile.size() - effNameBegPos);
		}
	}

	return effAniName;
}

cocostudio::Armature* AsstActiveSkill::fetchEffect()
{
	Armature* pEffect = nullptr;
	if (mEffectCache.empty())
	{
		auto effName = this->getEffectName();
		pEffect = Armature::create(effName);
		if (pEffect)
		{
			pEffect->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(AsstActiveSkill::onEffMovementEvent, this));
		}
	}
	else
	{
		pEffect = mEffectCache.front();
		pEffect->autorelease();

		mEffectCache.pop_front();
	}

	return pEffect;
}

void AsstActiveSkill::recycleEffect(cocostudio::Armature* pEffect)
{
	if (nullptr == pEffect)
		return;

	pEffect->retain();
	pEffect->removeFromParent();
	mEffectCache.push_back(pEffect);
}

void AsstActiveSkill::onEffMovementEvent(cocostudio::Armature *pArmature, cocostudio::MovementEventType movementType, const std::string& movementID)
{
	if (nullptr == pArmature)
		return;

	if (movementID == AsstActiveSkill::EffAKBeg)
	{
		switch (movementType)
		{
		case cocostudio::START:
			break;
		case cocostudio::COMPLETE:
			pArmature->getAnimation()->play(AsstActiveSkill::EffAKIng);
			break;
		case cocostudio::LOOP_COMPLETE:
			break;
		default:
			break;
		}
	}
	else if (movementID == AsstActiveSkill::EffAKIng)
	{
		switch (movementType)
		{
		case cocostudio::START:
			break;
		case cocostudio::COMPLETE:
			pArmature->getAnimation()->play(AsstActiveSkill::EffAKIng);
			break;
		case cocostudio::LOOP_COMPLETE:
			break;
		default:
			break;
		}
	}
	else if (movementID == AsstActiveSkill::EffAKEnd)
	{
		switch (movementType)
		{
		case cocostudio::START:
			break;
		case cocostudio::COMPLETE:
			this->recycleEffect(pArmature);
			break;
		case cocostudio::LOOP_COMPLETE:
			break;
		default:
			break;
		}
	}
}

/************************************************************************/
/*                               AASTroop                               */
/************************************************************************/

const int AASTroop::RandCount = 3;

AASTroop::AASTroop()
{

}

AASTroop::~AASTroop()
{

}

int AASTroop::getRandomCount()
{
	return AASTroop::RandCount;
}

void AASTroop::getBuilds(Camp* pCamp, int rand, std::vector<BaseBuild*>& outList)
{
	auto& campBuildList = pCamp->getBuildList();
	switch (mBaseData->mRange)
	{
	case AsstActiveSkill::Range::kALL:
	{
		for (auto itr = campBuildList.cbegin(); itr != campBuildList.cend(); itr++)
		{
			if (itr->second)
				outList.push_back(itr->second);
		}
	}
	break;

	case AsstActiveSkill::Range::kRandom:
	{
		std::vector<BaseBuild*> buildList;
		for (auto itr = campBuildList.cbegin(); itr != campBuildList.cend(); itr++)
		{
			if (itr->second)
				buildList.push_back(itr->second);
		}

		auto operType = mBaseData->mOperator;

		switch (mBaseData->mOperator)
		{
		case AsstActiveSkill::Operator::kAdd:
		{
			std::sort(std::begin(buildList), std::end(buildList), [operType](BaseBuild* pInfo1, BaseBuild* pInfo2){
				return pInfo1->getOwnPeople() < pInfo2->getOwnPeople();
			});
		}
		break;

		case AsstActiveSkill::Operator::kSub:
		{
			std::sort(std::begin(buildList), std::end(buildList), [operType](BaseBuild* pInfo1, BaseBuild* pInfo2){
				return pInfo1->getOwnPeople() > pInfo2->getOwnPeople();
			});
		}
		break;

		default:
			break;
		}

		int count = 0;
		int finalRange = getFinalRange(mBaseData->mRgMin, mBaseData->mRgMax, rand);
		for (auto build : buildList)
		{
			if (count >= finalRange)
				break;

			outList.push_back(build);
			++count;
		}
	}
	break;

	default:
		break;
	}
}

void AASTroop::onEffMovementEvent(cocostudio::Armature *pArmature, cocostudio::MovementEventType movementType, const std::string& movementID)
{
	if (nullptr == pArmature)
		return;

	if (movementID == AsstActiveSkill::EffAKBeg)
	{
		switch (movementType)
		{
		case cocostudio::START:
			break;
		case cocostudio::COMPLETE:
			this->recycleEffect(pArmature);
			break;
		case cocostudio::LOOP_COMPLETE:
			break;
		default:
			break;
		}
	}
}

void AASTroop::castBeg()
{
	if (mRandoms.size() < RandCount)
		return;

	int randTarget = mRandoms.at(0);
	int randNum = mRandoms.at(1);
	int randRange = mRandoms.at(2);

	std::vector<Camp*> campList;
	this->getFinalTarget(randTarget, campList);
	for (auto pCamp : campList)
	{
		std::vector<BaseBuild*> buildList;
		this->getBuilds(pCamp, randRange, buildList);
		int finalNum = getFinalNum(randNum);
		int finalTroopCount = 0;
		for (auto pBaseBuild : buildList)
		{
			switch (mBaseData->mNumType)
			{
			case AsstActiveSkill::NumType::kReal:
			{
				switch (mBaseData->mOperator)
				{
				case AsstActiveSkill::Operator::kAdd:	finalTroopCount = finalNum;		break;
				case AsstActiveSkill::Operator::kSub:	finalTroopCount = -finalNum;	break;
				}
			}
			break;

			case AsstActiveSkill::NumType::kPercent:
			{
				switch (mBaseData->mOperator)
				{
				case AsstActiveSkill::Operator::kAdd:	finalTroopCount = pBaseBuild->getOwnPeople() * (finalNum / 100.0f);		break;
				case AsstActiveSkill::Operator::kSub:	finalTroopCount = -pBaseBuild->getOwnPeople() * (finalNum / 100.0f);		break;
				}
			}
			break;

			default:
				break;
			}

			auto pEffect = this->fetchEffect();
			if (pEffect)
			{
				auto pAni = pEffect->getAnimation();

				pAni->setFrameEventCallFunc([finalTroopCount, pBaseBuild](cocostudio::Bone *pBone, const std::string& eventID, int originFrameIndex, int currentFrameIndex){
					if (eventID == AsstActiveSkill::EffFKCast)
						pBaseBuild->addMinusOwnPeople(finalTroopCount);
				});

				pAni->play(AsstActiveSkill::EffAKBeg);

				pBaseBuild->getBuildView()->addExternalEffect(pEffect);
			}
			else
			{
				pBaseBuild->addMinusOwnPeople(finalTroopCount);
			}

			//BuildInfo info;
			//info.finalNum = finalTroopCount;
			//info.pEffect = pEffect;

			//mBuildInfoList.emplace(std::make_pair(pBaseBuild->getKeyID(), std::move(info)));
		}
	}
}

void AASTroop::castIng()
{
// 	if (mRandoms.size() < RandCount)
// 		return;
// 
// 	int randTarget = mRandoms.at(0);
// 	int randNum = mRandoms.at(1);
// 	int randRange = mRandoms.at(2);
// 
// 	std::vector<Camp*> campList;
// 	this->getFinalTarget(randTarget, campList);
// 	for (auto pCamp : campList)
// 	{
// 		std::vector<BaseBuild*> buildList;
// 		this->getBuildList(pCamp, randRange, buildList);
// 		int finalNum = getFinalNum(randNum);
// 		int finalTroopCount = 0;
// 		for (auto build : buildList)
// 		{
// 			switch (mBaseData->mNumType)
// 			{
// 			case AsstActiveSkill::NumType::kReal:
// 			{
// 				switch (mBaseData->mOperator)
// 				{
// 				case AsstActiveSkill::Operator::kAdd:	finalTroopCount = finalNum;		break;
// 				case AsstActiveSkill::Operator::kSub:	finalTroopCount = -finalNum;	break;
// 				}
// 			}
// 			break;
// 
// 			case AsstActiveSkill::NumType::kPercent:
// 			{
// 				switch (mBaseData->mOperator)
// 				{
// 				case AsstActiveSkill::Operator::kAdd:	finalTroopCount = build->getOwnPeople() * (finalNum / 100.0f);		break;
// 				case AsstActiveSkill::Operator::kSub:	finalTroopCount = -build->getOwnPeople() * (finalNum / 100.0f);		break;
// 				}
// 			}
// 			break;
// 
// 			}
// 
// 			build->addMinusOwnPeople(finalTroopCount);
// 		}
// 	}
}

void AASTroop::castEnd()
{
// 	for (auto & infoPair : mBuildInfoList)
// 	{
// 		auto pEffect = infoPair.second.pEffect;
// 		if (pEffect)
// 			pEffect->getAnimation()->play(AsstActiveSkill::EffAKEnd);
// 	}
//  	mBuildInfoList.clear();
}

/************************************************************************/
/*                              AASTurret                              */
/************************************************************************/

const int AASTurret::RandCount = 3;

AASTurret::AASTurret()
{

}

AASTurret::~AASTurret()
{

}

bool AASTurret::check() const
{
	// ÁÙÊ±·½°¸
	std::vector<Camp*> campList;
	this->getFinalTarget(1, campList);
	for (auto pCamp : campList)
	{
		auto& campBuildList = pCamp->getBuildList();
		for (auto itr = campBuildList.cbegin(); itr != campBuildList.cend(); itr++)
		{
			auto pBuild = itr->second;
			if (pBuild && pBuild->getBuildType() == eBulidType::EBULID_TYPE_TURRET)
				return true;
		}
	}
	return false;
}

int AASTurret::getRandomCount()
{
	return AASTurret::RandCount;
}

void AASTurret::getBuilds(Camp* pCamp, int rand, std::vector<BaseBuild*>& outList)
{
	auto& campBuildList = pCamp->getBuildList();
	switch (mBaseData->mRange)
	{
	case AsstActiveSkill::Range::kALL:
	{
		for (auto itr = campBuildList.cbegin(); itr != campBuildList.cend(); itr++)
		{
			auto pBuild = itr->second;
			if (pBuild && pBuild->getBuildType() == eBulidType::EBULID_TYPE_TURRET)
				outList.push_back(itr->second);
		}
	}
	break;

	case AsstActiveSkill::Range::kRandom:
	{
		std::vector<BaseBuild*> buildList;
		for (auto itr = campBuildList.cbegin(); itr != campBuildList.cend(); itr++)
		{
			auto pBuild = itr->second;
			if (pBuild && pBuild->getBuildType() == eBulidType::EBULID_TYPE_TURRET)
				buildList.push_back(itr->second);
		}

		int count = 0;
		int finalRange = getFinalRange(mBaseData->mRgMin, mBaseData->mRgMax, rand);
		for (auto build : buildList)
		{
			if (count >= finalRange)
				break;

			outList.push_back(build);
			++count;
		}
	}
	break;

	default:
		break;
	}
}

void AASTurret::castBeg()
{
	if (mRandoms.size() < RandCount)
		return;

	int randTarget = mRandoms.at(0);
	int randAtkSpeed = mRandoms.at(1);
	int randBuildCount = mRandoms.at(2);

	auto effectFile = this->getEffectName();

	bool isPercent = false;
	float finalAtkSpeedNum = 0.0f;

	int finalNum = this->getFinalNum(randAtkSpeed);
	switch (mBaseData->mNumType)
	{
	case AsstActiveSkill::NumType::kReal:
	{
		isPercent = false;
		switch (mBaseData->mOperator)
		{
		case AsstActiveSkill::Operator::kAdd:	finalAtkSpeedNum = finalNum;	break;
		case AsstActiveSkill::Operator::kSub:	finalAtkSpeedNum = -finalNum;	break;
		default:	break;
		}
	}
	break;

	case AsstActiveSkill::NumType::kPercent:
	{
		isPercent = true;
		switch (mBaseData->mOperator)
		{
		case AsstActiveSkill::Operator::kAdd:	finalAtkSpeedNum = finalNum / 100.0f;	break;
		case AsstActiveSkill::Operator::kSub:	finalAtkSpeedNum = -finalNum / 100.0f;	break;
		default:	break;
		}
	}
	break;

	default:
		break;
	}

	std::vector<Camp*> campList;
	this->getFinalTarget(randTarget, campList);
	for (auto pCamp : campList)
	{
		std::vector<BaseBuild*> buildList;
		this->getBuilds(pCamp, randBuildCount, buildList);
		for (auto pBaseBuild : buildList)
		{
			auto pBuild = dynamic_cast<Turret*>(pBaseBuild);
			if (nullptr == pBuild)
				continue;

			float finalSpeed = 0.0f;
			if (isPercent)
				finalSpeed = pBuild->getBaseAtkSpeed() * finalAtkSpeedNum;
			else
				finalSpeed = finalAtkSpeedNum;

			auto pEffect = this->fetchEffect();
			if (pEffect)
			{
				auto pAni = pEffect->getAnimation();
				
				pAni->setFrameEventCallFunc([finalSpeed, pBuild](cocostudio::Bone *pBone, const std::string& eventID, int originFrameIndex, int currentFrameIndex){
					if (eventID == AsstActiveSkill::EffFKCast)
					{
						pBuild->doAdditonAtkSpeed(finalSpeed, true);
					}
				});

				pAni->play(AsstActiveSkill::EffAKBeg);

				pBuild->getBuildView()->addExternalEffect(pEffect);
			}
			else
			{
				pBuild->doAdditonAtkSpeed(finalSpeed, true);
			}

			BuildInfo info;
			info.finalNum = finalSpeed;
			info.pEffect = pEffect;
			CCLOG("[warning]Evix castBeg: %d, %f", pBaseBuild->getKeyID(), finalSpeed);
			mBuildInfoList.emplace(std::make_pair(pBaseBuild->getKeyID(), std::move(info)));
		}
	}
}

void AASTurret::castIng()
{
// 	auto findItr = mTurretInfoList.find(pBuild->getKeyID());
// 	if (mTurretInfoList.end() == findItr)
// 	{
// 		mTurretInfoList.insert(std::make_pair(pBuild->getKeyID(), finalSpeed));
// 	}
// 	else
// 	{
// 		findItr->second += finalSpeed;
// 	}
}

void AASTurret::castEnd()
{
	BattleControl *pBattleControl = sgBattleControl();
	for (auto & infoPair : mBuildInfoList)
	{
		auto pEffect = infoPair.second.pEffect;
		if (pEffect)
			pEffect->getAnimation()->play(AsstActiveSkill::EffAKEnd);

		auto pBuild = dynamic_cast<Turret*>(pBattleControl->getTargetBuild(infoPair.first));
		if (nullptr == pBuild)
			continue;

		CCLOG("[warning]Evix castEnd: %d, %f", pBuild->getKeyID(), infoPair.second.finalNum);
		pBuild->doAdditonAtkSpeed(infoPair.second.finalNum, false);
	}
	mBuildInfoList.clear();
}

/************************************************************************/
/*                            AASTroopView                              */
/************************************************************************/

const int AASTroopView::RandCount = 5;

const int AASTroopView::ViewRangeWidth = 920;
const int AASTroopView::ViewRangeHeight = 320;

AASTroopView::AASTroopView() : mEffect(nullptr)
{

}

AASTroopView::~AASTroopView()
{

}

bool AASTroopView::check() const
{
	return sgBattleControl()->getIsPvp();
}

int AASTroopView::getRandomCount()
{
	return AASTroopView::RandCount;
}

void AASTroopView::getBuilds(const cocos2d::Point& center, float radius, int randCount,const std::vector<BaseBuild*>& candidateList, std::vector<BaseBuild*>& outList)
{
	float radiusSQ = radius * radius;
	switch (mBaseData->mRange)
	{
	case AsstActiveSkill::Range::kALL:
	{
		for (auto itr = candidateList.cbegin(); itr != candidateList.cend(); itr++)
		{
			auto pBuild = *itr;
			if (pBuild && center.getDistanceSq(pBuild->getPosition()) <= radiusSQ)
				outList.push_back(pBuild);
		}
	}
	break;

	case AsstActiveSkill::Range::kRandom:
	{
		std::vector<BaseBuild*> buildList;
		for (auto itr = candidateList.cbegin(); itr != candidateList.cend(); itr++)
		{
			auto pBuild = *itr;
			if (pBuild && center.getDistanceSq(pBuild->getPosition()) <= radiusSQ)
				buildList.push_back(pBuild);
		}

		int count = 0;
		int finalRange = getFinalRange(0, buildList.size(), randCount);
		for (auto build : buildList)
		{
			if (count >= finalRange)
				break;

			outList.push_back(build);
			++count;
		}
	}
	break;

	default:
		break;
	}
}

void AASTroopView::onEffMovementEvent(cocostudio::Armature *pArmature, cocostudio::MovementEventType movementType, const std::string& movementID)
{
	if (nullptr == pArmature)
		return;

	if (movementID == AsstActiveSkill::EffAKBeg)
	{
		switch (movementType)
		{
		case cocostudio::START:
			break;
		case cocostudio::COMPLETE:
			this->recycleEffect(pArmature);
			break;
		case cocostudio::LOOP_COMPLETE:
			break;
		default:
			break;
		}
	}
}

void AASTroopView::doShowBubble(bool bShow)
{
	if (mCaster && mCaster->getCampType() != sgBattleControl()->getOwnCamp())
		return;

	BattleControl *pBattleControl = sgBattleControl();
	for (auto & buildId : mBuildList)
	{
		auto pBuild = (pBattleControl->getTargetBuild(buildId));
		if (nullptr == pBuild)
			continue;

		pBuild->getBuildView()->setShowEnemyInfo(bShow);
	}
}

void AASTroopView::castBeg()
{
	if (mRandoms.size() < RandCount)
		return;

	int randTarget = mRandoms.at(0);
	//int randPosX = mRandoms.at(1);
	//int randPosY = mRandoms.at(2);
	int randPos = mRandoms.at(1);
	int randRadius = mRandoms.at(3);
	int randCount = mRandoms.at(4);

	Size winSize = Director::getInstance()->getWinSize() / 2;
	Vec2 center(winSize.width, winSize.height);

	//int width = AASTroopView::ViewRangeWidth / 2;
	//int height = AASTroopView::ViewRangeHeight / 2;
	//float x = AsstActiveSkill::RandomRange(center.x - width, center.x + width, randPosX);
	//float y = AsstActiveSkill::RandomRange(center.y - height, center.y + height, randPosY);
	//cocos2d::Point finalCenter(x, y);

	float finalRadius = getFinalNum(randRadius);

	cocos2d::Point finalCenter;
	std::vector<Camp*> campList;
	this->getFinalTarget(randTarget, campList);
	std::vector<BaseBuild*> candidateList;
	for (auto pCamp : campList)
	{
		if (nullptr == pCamp || pCamp->getCampType() == mCaster->getCampType() )
			continue;
		
		auto& campBuildList = pCamp->getBuildList();
		if (campBuildList.empty())
			return;

		for (auto buildPair : campBuildList)
		{
			auto pBuild = buildPair.second;
			if (nullptr == pBuild)
				return;

			candidateList.push_back(pBuild);
		}
	}

	int index = AsstActiveSkill::RandomRange(0, candidateList.size() - 1, randPos);
	for (auto pBuild : candidateList)
	{
		if (index-- > 0)
			continue;

		if (nullptr != pBuild)
		{
			center = pBuild->getPosition();
			break;
		}
	}

	finalCenter = center;

	std::vector<BaseBuild*> buildList;
	this->getBuilds(center, randRadius, randCount, candidateList, buildList);
	for (auto pBuild : buildList)
	{
		if (nullptr == pBuild)
			continue;
		mBuildList.insert(pBuild->getKeyID());
	}

	// Show Effect
	auto effectFile = this->getEffectName();
	auto pEffect = Armature::create(effectFile);
	if (pEffect)
	{
		
		float scaleX = finalRadius * 2.0f / 1060.0f;
		float scaleY = finalRadius * 2.0f / 900.0f;
		float scale = (scaleX + scaleY) / 2.0f;
		pEffect->setScale(scale);

		auto pAni = pEffect->getAnimation();

		pAni->play(AsstActiveSkill::EffAKBeg);

		pAni->setFrameEventCallFunc([this](cocostudio::Bone *pBone, const std::string& eventID, int originFrameIndex, int currentFrameIndex){
			if (eventID == AsstActiveSkill::EffFKCast)
				this->doShowBubble(true);
		});

		pEffect->setBlendFunc(BlendFunc::COMADDITIVE);
		pEffect->setPosition(finalCenter);
		sgBattleControl()->getBattleView()->addExternalEffect(pEffect);

		//mEffect = pEffect;
	}
	else
	{
		this->doShowBubble(true);
	}
}

void AASTroopView::castIng()
{

}

void AASTroopView::castEnd()
{
	//if (mEffect)
	//	mEffect->getAnimation()->play(AsstActiveSkill::EffAKEnd);
	this->doShowBubble(false);

	mBuildList.clear();
}


/************************************************************************/
/*                                 AASAtk                               */
/************************************************************************/

const int AASAtk::RandCount = 2;

AASAtk::AASAtk()
{

}

AASAtk::~AASAtk()
{

}

int AASAtk::getRandomCount()
{
	return AASAtk::RandCount;
}

void AASAtk::doChange()
{
	if (mRandoms.size() < AASAtk::RandCount)
		return;

	int randTarget = mRandoms.at(0);
	int randNum = mRandoms.at(1);

	bool isPercent = false;
	float finalAtkNum = 0.0f;

	int finalNum = getFinalNum(randNum);
	switch (mBaseData->mNumType)
	{
	case AsstActiveSkill::NumType::kReal:
	{
		isPercent = false;
		switch (mBaseData->mOperator)
		{
		case AsstActiveSkill::Operator::kAdd:		break;
		case AsstActiveSkill::Operator::kSub:		break;
		}
	}
	break;

	case AsstActiveSkill::NumType::kPercent:
	{
		isPercent = true;
		switch (mBaseData->mOperator)
		{
		case AsstActiveSkill::Operator::kAdd:	finalAtkNum = finalNum;		break;
		case AsstActiveSkill::Operator::kSub:	finalAtkNum = -finalNum;		break;
		}
	}
	break;

	}

	std::vector<Camp*> campList;
	this->getFinalTarget(randTarget, campList);
	for (auto pCamp : campList)
	{
		if (isPercent)
		{
			pCamp->setAsstAtkAddition(finalAtkNum);
		}
		else
		{

		}

		auto findItr = mCampInfoList.find(pCamp->getCampType());
		if (mCampInfoList.end() == findItr)
		{
			mCampInfoList.insert(std::make_pair(pCamp->getCampType(), finalAtkNum));
		}
		else
		{
			findItr->second += finalAtkNum;
		}
	}
}

void AASAtk::castBeg()
{
	this->doChange();
}

void AASAtk::castIng()
{
	this->doChange();
}

void AASAtk::castEnd()
{
	BattleControl *pBattleControl = sgBattleControl();
	for (auto & infoPair : mCampInfoList)
	{
		auto pCamp = dynamic_cast<Camp*>(pBattleControl->getTargetCamp(eCampType(infoPair.first)));
		if (nullptr == pCamp)
			continue;

		pCamp->setAsstAtkAddition(0.0f);
	}
	mCampInfoList.clear();

	/*if (mRandoms.size() >= mRandCount)
	{
		int randAtkAddition = mRandoms.at(1);
		int randNum = mRandoms.at(0);

		const std::vector<Camp*>& campList = getFinalTarget(randNum);
		for (auto pCamp : campList)
		{
			pCamp->setAsstAtkAddition(0);
		}
	}*/
}

