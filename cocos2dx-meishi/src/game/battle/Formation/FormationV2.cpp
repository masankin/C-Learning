
#include "FormationV2.h"
#include "FormationV2Row.h"

#include "../Map/MapHandler.h"
#include "core/MessageManager.h"
#include "core/TimeCalculator.h"
#include "battle/BattleControl.h"
#include "battle/Build.h"

USING_NS_CC;

/////////////////////////////////FormationV2Row::PathStep/////////////////////////////////////////
FormationV2::PathStep::PathStep() : mDistance(0.0f)
{

}


FormationV2::PathStep::~PathStep()
{

}


////////////////////////////////FormationV2::FormVarInfo//////////////////////////////////////////

FormationV2::FormVarInfo::FormVarInfo() : mRowRadiusAVarForExit(0.0f)
, mRowRadiusBVarForExit(0.0f)

, mRowRadiusAVarForWalk(0.0f)
, mRowRadiusBVarForWalk(0.0f)

, mRowRadiusAVarForEnter(0.0f)
, mRowRadiusBVarForEnter(0.0f)
{

}


FormationV2::FormVarInfo::~FormVarInfo()
{

}


void FormationV2::FormVarInfo::clear()
{
	mRowRadiusAVarForExit = 0.0f;
	mRowRadiusBVarForExit = 0.0f;

	mRowRadiusAVarForWalk = 0.0f;
	mRowRadiusBVarForWalk = 0.0f;

	mRowRadiusAVarForEnter = 0.0f;
	mRowRadiusBVarForEnter = 0.0f;
}


////////////////////////////////FormationV2::FormationV2FrozenInfo//////////////////////////////////////////
FormationV2::FrozenData::FrozenData()
{
	static int id = 0;
	++id;

	mKey = StringUtils::format("FormationV2_FrozenData_%d", id);
}


FormationV2::FrozenData::~FrozenData()
{

}


/////////////////////////////////FormationV2/////////////////////////////////////////


const float FormationV2::DistanceSegment = 10.0f;
const float FormationV2::RowInterval = 25.0f;

const std::string FormationV2::PauseTimerKeyFormat("FormationV2_%d_Pause");


unsigned int FormationV2::NewId()
{
	//static unsigned int sUidGenerator = 0;

	//return ++sUidGenerator;

	return cocos2d::random(1, INT_MAX);
}


FormationV2* FormationV2::create(unsigned int id, int count, int begObjID, int endObjId)
{
	FormationV2* pRet = new FormationV2();
	if (pRet && pRet->init(id, count, begObjID, endObjId))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return pRet;
	}
}


int FormationV2::CalculateStep(int curStep, int count)
{
	return curStep + FormationV2::StepInterval * count;
}


FormationV2::FormationV2() : mId(0)
, mUpdate(true)
, mAttribType(AttribType::Normal)

, mBaseSpeed(1.0f)
, mSpeed(0.0f)
, mDistance(0.0f)

, mSpeedScale(1.0f)

, mDistanceForExit(0.0f)
, mDistanceForEnter(0.0f)

, mExpectedCount(0)
, mProducedCount(0)
, mProducerTimer(0.0f)
, mCloseProducer(false)

, mRootView(nullptr)
, mLocalZOrder(0)

, mSrcMapObjID(0)
, mDstMapObjID(0)

, mSrcBuildUid(0)
, mDstBuildUid(0)
, mSrcCamp(eCampType::ECAMP_TYPE_Invalid)
, mDstCamp(eCampType::ECAMP_TYPE_Invalid)
, mIsPlayWalkSound(false)
{

}


FormationV2::~FormationV2()
{
	this->dispose();
}


bool FormationV2::init(unsigned int id, int count, int begObjID, int endObjId)
{
	if (0 == id || 0 == count)
		return false;

	mId = id;
	mExpectedCount = count;
	mPauseTimerKey = StringUtils::format(PauseTimerKeyFormat.c_str(), mId);

	mSpeedScale = 1.0f;

	if (!this->resetPath(begObjID, endObjId))
		return false;

	return true;
}


void FormationV2::dispose()
{
	mId = 0;
	mUpdate = true;
	mPauseTimerKey = "";
	mAttribType = AttribType::Normal;

	mBegPoint = Point::ZERO;
	mEndPoint = Point::ZERO;
	for (auto & step : mPathList)
	{
		if (step)
			delete step;
	}
	mPathList.clear();
	mBaseSpeed = 0.0f;
	mSpeed = 0.0f;
	mDistance = 0.0f;

	mSpeedScale = 1.0f;
	
	mDistanceForExit = 0.0f;
	mDistanceForEnter = 0.0f;
	for (auto & row : mRowList)
	{
		if (row)
		{
			row->removeFromParent();
			row->release();
		}
	}
	mRowList.clear();
	mFormVarInfo.clear();

	mExpectedCount = 0;
	mProducedCount = 0;
	mProducerTimer = 0.0f;
	mCloseProducer = false;
	
	mRootView = nullptr;
	mLocalZOrder = 0;

	mSrcMapObjID = 0;
	mDstMapObjID = 0;

	mSrcBuildUid = 0;
	mDstBuildUid = 0;
	mSrcCamp = eCampType::ECAMP_TYPE_Invalid;
	mDstCamp = eCampType::ECAMP_TYPE_Invalid;

	for (auto& pFrozenData : mFrozenDataList)
	{
		if (pFrozenData)
		{
			sgTimeCalculator()->removeTimeCalcultor(pFrozenData->mKey);
			pFrozenData->mForzenRowList.clear();
			
			delete pFrozenData;
		}
	}
	mFrozenDataList.clear();
}


void FormationV2::update(float dt)
{
	dt *= mSpeedScale;
	
	if (sgTimeCalculator()->getTimeLeft(mPauseTimerKey) == 0.0f)
		this->resumeByTimer();

	this->updateForced();

	if (!mUpdate)
		return;

	if (nullptr == mRootView)
		return;

	this->updateLogic();

	this->updateCollision(dt);

	this->updateRows(dt);
}


bool FormationV2::checkRowCollision(FormationV2Row* row, MapObject* obj)
{
	if (nullptr == row || nullptr == obj)
		return false;

	if (!obj->isVisible())
		return false;

	auto& objWorldPos = obj->getPosition();
	auto& rowWorldPos = row->getPosition();

	float objRadiusA = obj->getRadiusA();
	float objRadiusB = obj->getRadiusB();

	float rowRadiusA = row->getRadiusA();
	float rowRadiusB = row->getRadiusB();

	float objRadius = objRadiusA > objRadiusB ? objRadiusA : objRadiusB;
	float rowRadius = rowRadiusA;

	if (rowWorldPos.getDistanceSq(objWorldPos) > powf(objRadius + rowRadius, 2))
		return false;

	Point connerPt[4];

	static auto lessCmp = [](float val1, float val2)->bool {
		return val1 < val2;
	};

	static auto greterCmp = [](float val1, float val2)->bool {
		return val1 > val2;
	};

	static auto getExtremeX = [](const Point pts[], int count, const std::function<bool(float, float)>& compare)->float{

		if (count <= 0)
			return 0.0f;

		float extremeX = pts[0].x;
		for (int i = 1; i < count; ++i)
			if (compare(pts[i].x, extremeX))
				extremeX = pts[i].x;

		return extremeX;

	};

	static auto getExtremeY = [](const Point pts[], int count, const std::function<bool(float, float)>& compare)->float{

		if (count <= 0)
			return 0.0f;

		float extremeY = pts[0].y;
		for (int i = 1; i < count; ++i)
			if (compare(pts[i].y, extremeY))
				extremeY = pts[i].y;

		return extremeY;
	};

	// Convert object position to row local position , then check it

	connerPt[0] = std::move(row->convertToNodeSpace(obj->getLTPoint()));
	connerPt[1] = std::move(row->convertToNodeSpace(obj->getRTPoint()));
	connerPt[2] = std::move(row->convertToNodeSpace(obj->getLBPoint()));
	connerPt[3] = std::move(row->convertToNodeSpace(obj->getRBPoint()));

	//float objMinX = getExtremeX(std::less<float>());
	//float objMaxX = getExtremeX(std::greater<float>());

	float objMinX = getExtremeX(connerPt, 4, lessCmp);
	float objMaxX = getExtremeX(connerPt, 4, greterCmp);

	if (objMaxX < -rowRadiusA || objMinX > rowRadiusA)
		return false;


	//float objMinY = getExtremeY(std::less<float>());
	//float objMaxY = getExtremeY(std::greater<float>());

	float objMinY = getExtremeY(connerPt, 4, lessCmp);
	float objMaxY = getExtremeY(connerPt, 4, greterCmp);

	if (objMaxY < -rowRadiusB || objMinY > rowRadiusB)
		return false;


	// Convert row position to object local position , then check it

	Point mRowCordY = row->getFaceDirection();
	Point mRowCordX = mRowCordY.getRPerp();

	Point rowWorldLT = std::move(rowWorldPos - mRowCordX * rowRadiusA + mRowCordY * rowRadiusB);
	Point rowWorldRT = std::move(rowWorldPos + mRowCordX * rowRadiusA + mRowCordY * rowRadiusB);
	Point rowWorldLB = std::move(rowWorldPos - mRowCordX * rowRadiusA - mRowCordY * rowRadiusB);
	Point rowWorldRB = std::move(rowWorldPos + mRowCordX * rowRadiusA - mRowCordY * rowRadiusB);

	connerPt[0] = std::move(obj->convertWorldToLocal(rowWorldLT));
	connerPt[1] = std::move(obj->convertWorldToLocal(rowWorldRT));
	connerPt[2] = std::move(obj->convertWorldToLocal(rowWorldLB));
	connerPt[3] = std::move(obj->convertWorldToLocal(rowWorldRB));


	float rowMinX = getExtremeX(connerPt, 4, lessCmp);
	float rowMaxX = getExtremeX(connerPt, 4, greterCmp);

	if (rowMaxX < -objRadiusA || rowMinX > objRadiusA)
		return false;


	float rowMinY = getExtremeY(connerPt, 4, lessCmp);
	float rowMaxY = getExtremeY(connerPt, 4, greterCmp);

	if (rowMaxY < -objRadiusB || rowMinY > objRadiusB)
		return false;

	return true;
}


bool FormationV2::checkRowCollision(FormationV2Row* row, const cocos2d::Point& center, float radiusA, float radiusB)
{
	if (nullptr == row)
		return false;

	auto& objWorldPos = center;
	auto& rowWorldPos = row->getPosition();

	float objRadiusA = radiusA;
	float objRadiusB = radiusB;

	float rowRadiusA = row->getRadiusA();
	float rowRadiusB = row->getRadiusB();

	float objRadius = objRadiusA > objRadiusB ? objRadiusA : objRadiusB;
	float rowRadius = rowRadiusA;

	if (rowWorldPos.getDistanceSq(objWorldPos) > powf(objRadius + rowRadius, 2))
		return false;

	Point connerPt[4];

	static auto lessCmp = [](float val1, float val2)->bool {
		return val1 < val2;
	};

	static auto greterCmp = [](float val1, float val2)->bool {
		return val1 > val2;
	};

	static auto getExtremeX = [](const Point pts[], int count, const std::function<bool(float, float)>& compare)->float{

		if (count <= 0)
			return 0.0f;

		float extremeX = pts[0].x;
		for (int i = 1; i < count; ++i)
			if (compare(pts[i].x, extremeX))
				extremeX = pts[i].x;

		return extremeX;

	};

	static auto getExtremeY = [](const Point pts[], int count, const std::function<bool(float, float)>& compare)->float{

		if (count <= 0)
			return 0.0f;

		float extremeY = pts[0].y;
		for (int i = 1; i < count; ++i)
			if (compare(pts[i].y, extremeY))
				extremeY = pts[i].y;

		return extremeY;
	};

	// Convert object position to row local position , then check it

	connerPt[0] = std::move(row->convertToNodeSpace(Point(objWorldPos.x - radiusA, objWorldPos.y + radiusB)));
	connerPt[1] = std::move(row->convertToNodeSpace(Point(objWorldPos.x + radiusA, objWorldPos.y + radiusB)));
	connerPt[2] = std::move(row->convertToNodeSpace(Point(objWorldPos.x - radiusA, objWorldPos.y - radiusB)));
	connerPt[3] = std::move(row->convertToNodeSpace(Point(objWorldPos.x + radiusA, objWorldPos.y - radiusB)));

	//float objMinX = getExtremeX(std::less<float>());
	//float objMaxX = getExtremeX(std::greater<float>());

	float objMinX = getExtremeX(connerPt, 4, lessCmp);
	float objMaxX = getExtremeX(connerPt, 4, greterCmp);

	if (objMaxX < -rowRadiusA || objMinX > rowRadiusA)
		return false;


	//float objMinY = getExtremeY(std::less<float>());
	//float objMaxY = getExtremeY(std::greater<float>());

	float objMinY = getExtremeY(connerPt, 4, lessCmp);
	float objMaxY = getExtremeY(connerPt, 4, greterCmp);

	if (objMaxY < -rowRadiusB || objMinY > rowRadiusB)
		return false;


	// Convert row position to object local position , then check it

	Point mRowCordY = row->getFaceDirection();
	Point mRowCordX = mRowCordY.getRPerp();

	Point rowWorldLT = std::move(rowWorldPos - mRowCordX * rowRadiusA + mRowCordY * rowRadiusB);
	Point rowWorldRT = std::move(rowWorldPos + mRowCordX * rowRadiusA + mRowCordY * rowRadiusB);
	Point rowWorldLB = std::move(rowWorldPos - mRowCordX * rowRadiusA - mRowCordY * rowRadiusB);
	Point rowWorldRB = std::move(rowWorldPos + mRowCordX * rowRadiusA - mRowCordY * rowRadiusB);

	connerPt[0] = std::move(rowWorldLT - objWorldPos);
	connerPt[1] = std::move(rowWorldRT - objWorldPos);
	connerPt[2] = std::move(rowWorldLB - objWorldPos);
	connerPt[3] = std::move(rowWorldRB - objWorldPos);


	float rowMinX = getExtremeX(connerPt, 4, lessCmp);
	float rowMaxX = getExtremeX(connerPt, 4, greterCmp);

	if (rowMaxX < -objRadiusA || rowMinX > objRadiusA)
		return false;


	float rowMinY = getExtremeY(connerPt, 4, lessCmp);
	float rowMaxY = getExtremeY(connerPt, 4, greterCmp);

	if (rowMaxY < -objRadiusB || rowMinY > objRadiusB)
		return false;

	return true;
}


void FormationV2::updateCollision(float dt)
{
	if (mRowList.empty())
		return;

	const std::map<int, MapObject*>& mapObjList = sgMapHandler()->getMapObjectList();
	for (auto objItr = mapObjList.begin(); objItr != mapObjList.end(); ++objItr)
	{
		MapObject* mapObj = objItr->second;
		if (nullptr == mapObj)
			continue;

		if (mapObj->getUid() == mSrcMapObjID || mapObj->getUid() == mDstMapObjID)
			continue;

		if (mapObj->getType() == MapObject::eWayPoint)
			continue;

		//if (!mapObj->isVisible())
		//	continue;

		
		for (auto rowItr = mRowList.begin(); rowItr != mRowList.end(); ++rowItr)
		{
			FormationV2Row* row = (*rowItr);
			if (nullptr == row)
				continue;

			if (this->checkRowCollision(row, mapObj))
			{
				row->doCollision(mapObj, true);
			}
			else
			{
				row->doCollision(mapObj, false);
			}
		}
	}
}


void FormationV2::updateRows(float dt)
{
	auto itr = mRowList.begin();
	while (itr != mRowList.end())
	{
		auto row = *itr;
		if (nullptr == row)
		{
			itr = mRowList.erase(itr);
			continue;
		}

		if (row->isPathStepOver())
		{
			auto newStepIndex = FormationV2::CalculateStep(row->getPathStep(), 1);
			auto pNewStep = this->getStep(newStepIndex);

			if (nullptr == pNewStep)
			{
				this->doRowArrived(row);
				itr = mRowList.erase(itr);

				row->release();
				continue;
			}

			row->setPathStep(newStepIndex, pNewStep->mBegPoint, pNewStep->mEndPoint, pNewStep->mBegFaceDir, pNewStep->mEndFaceDir, pNewStep->mDistance);
		}

		auto rowPos = row->getPosition();

		float radiusAVar = 0.0f;
		float radiusBVar = 0.0f;
		FormationV2Row::MarchState state;
		if (rowPos.getDistanceSq(mBegPoint) <= mDistanceForExit * mDistanceForExit)
		{
			radiusAVar = mFormVarInfo.mRowRadiusAVarForExit;
			radiusBVar = mFormVarInfo.mRowRadiusBVarForExit;
			state = FormationV2Row::MarchState::eExiting;
		}
		else if (rowPos.getDistanceSq(mEndPoint) <= mDistanceForEnter * mDistanceForEnter)
		{
			radiusAVar = mFormVarInfo.mRowRadiusAVarForEnter;
			radiusBVar = mFormVarInfo.mRowRadiusBVarForEnter;
			state = FormationV2Row::MarchState::eEntering;
		}
		else
		{
			radiusAVar = mFormVarInfo.mRowRadiusAVarForWalk;
			radiusBVar = mFormVarInfo.mRowRadiusBVarForWalk;
			state = FormationV2Row::MarchState::eWalking;

			//加入行走音效
			/*if (!mIsPlayWalkSound && rowPos.getDistanceSq((mEndPoint + mBegPoint) / 2) <= mDistanceForEnter * mDistanceForEnter)
			{
				mIsPlayWalkSound = true;
				int soundIndex = cocos2d::random(0, 4);
				if (soundIndex >= 3)
					return;
				sgSoundManager()->playMusicEffect(eMusicType(SOUND_SOLDIER_WALK_1 + soundIndex));
			}*/
		}

		row->update(dt, mSpeed, radiusAVar, radiusBVar, state);


		// zOrder 需优化
		/*auto visibleSize = Director::getInstance()->getVisibleSize();

		int orderX = static_cast<int>(rowPos.x / 20.0f);
		int orderY = static_cast<int>(rowPos.y / 20.0f);

		int orderWidth = static_cast<int>(visibleSize.width / 20.0f);
		int orderHeight = static_cast<int>(visibleSize.height / 20.0f);

		//auto zOrder = orderY * orderWidth + orderX;
		//row->setLocalZOrder(zOrder);
		auto arrivalOrder = orderY * orderWidth + orderX;
		row->setOrderOfArrival(arrivalOrder);*/


		// Evix QTag : should remove row ?
		/*if (row->getAliveEntityCount() <= 0)
		{
			this->doRowEmpty(row);
			itr = mRowList.erase(itr);
			continue;
		}*/

		++itr;
	}
}


void FormationV2::updateLogic()
{
	auto itrFrozenData = mFrozenDataList.begin();
	while (itrFrozenData != mFrozenDataList.end())
	{
		auto pData = *itrFrozenData;
		if (nullptr != pData)
		{
			if (sgTimeCalculator()->getTimeLeft(pData->mKey) == 0)
			{
				for (auto & pRow : pData->mForzenRowList)
				{
					pRow->doResume();
					pRow->doUnfreeze();
				}

				sgTimeCalculator()->removeTimeCalcultor(pData->mKey);

				delete pData;

				itrFrozenData = mFrozenDataList.erase(itrFrozenData);
				continue;
			}
		}
		else
		{
			itrFrozenData = mFrozenDataList.erase(itrFrozenData);
			continue;
		}

		++itrFrozenData;
	}


	std::map<FormationV2Row*, float> rowSpeedScaleCache;

	auto & oilPointList = sgBattleControl()->getOilPointList();
	for (auto itr = oilPointList.begin(); itr != oilPointList.end(); ++itr)
	{
		auto & oilPt = itr->second;

		for (auto & pRow : mRowList)
		{
			if (this->checkRowCollision(pRow, oilPt.mPos, oilPt.mRadiusA, oilPt.mRadiusB))
			{
				rowSpeedScaleCache[pRow] = oilPt.mSpeed;
			}
		}
	}

	for (auto & pRow : mRowList)
	{
		auto itrSpeedScale = rowSpeedScaleCache.find(pRow);
		if (itrSpeedScale != rowSpeedScaleCache.end())
			pRow->doChangeSpeedScale(itrSpeedScale->second);
		else
			pRow->doChangeSpeedScale(1.0f);
	}
}


void FormationV2::updateForced()
{
	for (auto & row : mRowList)
	{
		if (nullptr != row)
			row->calculateAliveEntity();
	}
}


void FormationV2::pause()
{
	mUpdate = false;
	for (auto & row : mRowList)
	{
		if (nullptr != row)
			row->doPause();
	}
}


void FormationV2::resume()
{
	mUpdate = true;
	for (auto & row : mRowList)
	{
		if (nullptr != row)
			row->doResume();
	}
}


void FormationV2::pauseByTimer(float time)
{
	this->pause();

	sgTimeCalculator()->createTimeCalcultor(mPauseTimerKey, time);
}


void FormationV2::resumeByTimer()
{
	this->resume();

	sgTimeCalculator()->removeTimeCalcultor(mPauseTimerKey);
}


//////////////////////////////////Producer////////////////////////////////////////
bool FormationV2::checkProducer(float dt)
{
	if (!mUpdate)
		return false;

	if (mExpectedCount <= mProducedCount)
		return false;

	if (mCloseProducer)
		return false;

	auto productivity = FormationV2::RowInterval / mSpeed;

	mProducerTimer += dt;
	if (mProducerTimer < productivity)
		return false;

	mProducerTimer -= productivity;

	return true;
}


void FormationV2::updateProducer(float dt, int count)
{
	if (!mUpdate)
		return;

	//auto producingCount = mExpectedCount - mProducedCount;
	//producingCount = producingCount > FormationV2::RowMaxCount ? FormationV2::RowMaxCount : producingCount;

	auto row = FormationV2Row::create(this, count);
	if (row)
	{
		CCLOG("[Leon]FormationV2::updateProducer : [id:%d] Src-%d, Dst-%d", this->mId, this->getSrcCamp(), this->getDstCamp());

		auto stepIndex = this->getFirstStep();
		auto pStep = this->getStep(stepIndex);

		if (pStep)
		{
			row->setPathStep(stepIndex, pStep->mBegPoint, pStep->mEndPoint, pStep->mBegFaceDir, pStep->mEndFaceDir, pStep->mDistance);
			//row->setDirection(pStep->mBegAngle);

			row->retain();

			mRowList.push_back(row);

			this->addRowToView(row);
			this->doRowDepart(row);
		}
	}

	//mProducedCount += producingCount;
	mProducedCount += count;
}



//////////////////////////////////Path////////////////////////////////////////

void FormationV2::calculateFormVarInfo()
{
	if (fabs(mSpeed) == 0.0f)
		return;

	float timeTotalConsume = mDistance / mSpeed;

	auto maxExitDistance = 3.0f * FormationV2::DistanceSegment;
	auto minWalkDistance = 3.0f * FormationV2::DistanceSegment;
	auto maxEnterDistance = 3.0f * FormationV2::DistanceSegment;
	auto criticalDistance = maxExitDistance + minWalkDistance + maxEnterDistance;

	float radiusBVarForExit = mSpeed * FormationV2Row::RadiusBMax / maxExitDistance;
	float radiusBVarForEnter = mSpeed * FormationV2Row::RadiusBMax / maxEnterDistance;

	float radiusBForExit = FormationV2Row::RadiusBMax;
	float radiusBForEnter = FormationV2Row::RadiusBMax;


	float timeForExit = 0.0f;
	float timeForEnter = 0.0f;
	float timeForWalk = 0.0f;
	if (mDistance <= criticalDistance)
	{
		// Exit
		auto ratioForExit = maxExitDistance / criticalDistance;

		mDistanceForExit = mDistance * ratioForExit;
		timeForExit = timeTotalConsume * ratioForExit;

		radiusBForExit = radiusBVarForExit * timeForExit;


		// Enter
		auto ratioEnter = maxEnterDistance / criticalDistance;

		mDistanceForEnter = mDistance * ratioEnter;
		timeForEnter = timeTotalConsume * ratioEnter;

		radiusBForEnter = radiusBVarForEnter * timeForEnter;


		// Walk
		float ratioWalk = minWalkDistance / criticalDistance;

		timeForWalk = timeTotalConsume * ratioWalk;
	}
	else
	{
		// Exit
		mDistanceForExit = maxExitDistance;

		timeForExit = maxExitDistance / mSpeed;

		//Enter
		mDistanceForEnter = maxEnterDistance;

		timeForEnter = maxEnterDistance / mSpeed;

		//Walk
		timeForWalk = timeTotalConsume - timeForExit - timeForEnter;
	}

	if (timeForExit > 0.0f)
	{
		mFormVarInfo.mRowRadiusAVarForExit = FormationV2Row::RadiusAMax / timeForExit;
		//mFormVarInfo.rowRadiusBVarForExit = FormationV2Row::RadiusBMax / timeForExit;
		mFormVarInfo.mRowRadiusBVarForExit = radiusBForExit / timeForExit;
	}

	if (timeForEnter > 0.0f)
	{
		mFormVarInfo.mRowRadiusAVarForEnter = -FormationV2Row::RadiusAMax / timeForEnter;
		//mFormVarInfo.rowRadiusBVarForEnter = FormationV2Row::RadiusBMax / timeForEnter;
		mFormVarInfo.mRowRadiusBVarForEnter = radiusBForEnter / timeForEnter;
	}

	if (timeForWalk > 0.0f)
	{
		mFormVarInfo.mRowRadiusAVarForWalk = 0.0f;
		//mFormVarInfo.rowRadiusBVarForWalk = -2.0f * FormationV2Row::RadiusBMax / timeForWalk;
		mFormVarInfo.mRowRadiusBVarForWalk = -(radiusBForExit + radiusBForEnter) / timeForWalk;
	}
}


bool FormationV2::resetPath(int begObjID, int endObjId)
{
	mSrcMapObjID = begObjID;
	mDstMapObjID = endObjId;

	auto path(std::move(sgMapHandler()->searchPath(begObjID, endObjId)));

	return this->resetPath(path);
}


bool FormationV2::resetPath(const std::list<cocos2d::Point>& path)
{
	if (path.size() < 2)
		return false;

	for (auto & step : mPathList)
	{
		if (step)
			delete step;
	}
	mPathList.clear();
	mDistance = 0.0f;

	mPathList.reserve(path.size());
	//mPathList.insert(mPathList.begin(), path.begin(), path.end());

	mBegPoint = path.front();
	mEndPoint = path.back();

	/*const Point* visitPt = &mBegPoint;
	for (std::size_t index = 1; index < mPathList.size(); ++index)
	{
	auto curPt = mPathList[index];
	mDistance += visitPt->getDistance(curPt);
	visitPt = &mPathList[index];
	}*/

	auto prevItr = path.begin();
	auto curItr = path.begin();
	while (++curItr != path.end())
	{
		PathStep* pStep = new PathStep();
		if (nullptr == pStep)
		{
			CCLOG("[error] FormationV2::resetPath: allocate memory Failed.");
			return false;
		}


		pStep->mBegPoint = *prevItr;
		pStep->mEndPoint = *curItr;
		pStep->mDistance = pStep->mBegPoint.getDistance(pStep->mEndPoint);
		mPathList.push_back(pStep);

		mDistance += pStep->mDistance;

		prevItr = curItr;
	}


	Vec2 lastEndFaceDir = Vec2::ZERO;
	auto stepItr = mPathList.begin();
	while (stepItr != mPathList.end())
	{
		PathStep* pCurStep = *stepItr;
		if (nullptr == pCurStep)
		{
			CCLOG("[error] FormationV2::resetPath: Path step is null.");
			return false;
		}

		Vec2 curDir = std::move((pCurStep->mEndPoint - pCurStep->mBegPoint).getNormalized());
		//float curAngle = curDir.getAngle();

		if (mPathList.begin() == stepItr)
			pCurStep->mBegFaceDir = curDir;
		else
			pCurStep->mBegFaceDir = lastEndFaceDir;
		
		++stepItr;

		if (stepItr != mPathList.end())
		{
			PathStep* pNextStep = *stepItr;
			if (nullptr == pNextStep)
			{
				CCLOG("[error] FormationV2::resetPath: Path is break, not contiunous.");
				return false;
			}

			Vec2 nextDir = std::move((pNextStep->mEndPoint - pNextStep->mBegPoint).getNormalized());
			//float nextAngle = nextDir.getAngle();

			pCurStep->mEndFaceDir = std::move((curDir + nextDir).getNormalized());
		}
		else
		{
			pCurStep->mEndFaceDir = curDir;
		}

		lastEndFaceDir = pCurStep->mEndFaceDir;
	}

	this->calculateFormVarInfo();

	return true;
}


const FormationV2::PathStep* FormationV2::getStep(int step)
{
	if (step < 0 || step >= (int)mPathList.size())
		return nullptr;

	return mPathList[step];
}


void FormationV2::setSpeed(float val)
{
	if (mSpeed == val)
		return;

	mSpeed = val;
	this->calculateFormVarInfo();
}


void FormationV2::setSpeedScale(float val)
{
	if (mSpeedScale == val)
		return; 

	mSpeedScale = val;
	for (auto &row : mRowList)
	{
		if (row)
			row->changeSolidersSpeedScale();
	}
}


////////////////////////////////View//////////////////////////////////////////
void FormationV2::addRowToView(FormationV2Row* row)
{
	if (nullptr == row || nullptr == mRootView)
		return;

	mRootView->addChild(row, mLocalZOrder);
}


void FormationV2::removeRowFromView(FormationV2Row* row)
{
	if (nullptr == row || nullptr == mRootView)
		return;

	mRootView->removeChild(row);

}


void FormationV2::enterView(cocos2d::Node* view, int zOrder)
{
	if (nullptr == view)
		return;

	if (mRootView)
		this->exitView();

	mRootView = view;
	mLocalZOrder = zOrder;

	for (auto &row : mRowList)
	{
		this->addRowToView(row);
	}
}


void FormationV2::exitView()
{
	if (nullptr == mRootView)
		return;

	for (auto &row : mRowList)
	{
		this->removeRowFromView(row);
	}

	mRootView = nullptr;
	mLocalZOrder = 0;
}


//////////////////////////////////Logic////////////////////////////////////////

Solider* FormationV2::getEntityInEllipseRange(const Point& worldPos, float radiusA, float radiusB, const std::function<bool(Solider*)>& filter /* = nullptr */)
{
	Solider * pRet = nullptr;
	for (auto & row : mRowList)
	{
		if (nullptr == row)
			continue;

		pRet = row->getEntityInEllipseRange(worldPos, radiusA, radiusB, filter);
		if (pRet)
			return pRet;
	}

	return pRet;
}


Solider* FormationV2::getEntityInRectRange(const cocos2d::Point& worldPos, float width, float height, const std::function<bool(Solider*)>& filter /* = nullptr */)
{
	Solider * pRet = nullptr;
	for (auto & row : mRowList)
	{
		if (nullptr == row)
			continue;

		pRet = row->getEntityInRectRange(worldPos, width, height, filter);
		if (pRet)
			return pRet;
	}

	return pRet;
}


void FormationV2::getEntityListInEllipseRange(const cocos2d::Point& worldPos, float radiusA, float radiusB, std::vector<Solider*>& outList, const std::function<bool(Solider*)>& filter /* = nullptr */)
{
	for (auto & row : mRowList)
	{
		if (nullptr == row)
			continue;

		row->getEntityListInEllipseRange(worldPos, radiusA, radiusB, outList, filter);
	}
}


void FormationV2::getEntityListInRectRange(const cocos2d::Point& worldPos, float width, float height, std::vector<Solider*>& outList, const std::function<bool(Solider*)>& filter /* = nullptr */)
{
	for (auto & row : mRowList)
	{
		if (nullptr == row)
			continue;

		row->getEntityListInEllipseRange(worldPos, width, height, outList, filter);
	}
}


eCampType FormationV2::getCurDstCamp() const
{
	BaseBuild* pDstBuild = sgBattleControl()->getTargetBuild(mDstBuildUid);
	if (pDstBuild)
		return pDstBuild->getCamp();

	return ECAMP_TYPE_Invalid;
}


const Point& FormationV2::getPosition() const
{
	int midIndex = mRowList.size() / 2;
	int index = 0;
	for (auto & row : mRowList)
	{
		if (index == midIndex && row)
			return row->getPosition();

		++index;
	}

	return Point::ZERO;
}


int FormationV2::getAliveEntityCount() const
{
	int count = 0;
	for (auto & row : mRowList)
		count += row->getAliveEntityCount();

	return count;
}


bool FormationV2::checkProducerOver() const
{
	return (mExpectedCount <= mProducedCount) || mCloseProducer;
}


void FormationV2::doProductOver()
{
	BaseBuild* pSrcBuild = sgBattleControl()->getTargetBuild(mSrcBuildUid);
	if (pSrcBuild)
	{
		pSrcBuild->removeStatus(BuildStatus_Sending_Solider);
		
		pSrcBuild->removeSendingFormation(mDstBuildUid, mId);
	}
}



bool FormationV2::checkOver() const
{
	if (!mCloseProducer && (mProducedCount < mExpectedCount))
		return false;

	if (mRowList.empty())
		return true;

	bool hasAliveEntity = false;
	for (auto & row : mRowList)
	{
		if (row->getAliveEntityCount() <= 0)
			continue;

		hasAliveEntity = true;
		break;
	}

	return !hasAliveEntity;
}


void FormationV2::doOver()
{
	BaseBuild* pDestBuild = sgBattleControl()->getTargetBuild(mDstBuildUid);
	if (pDestBuild)
	{
		pDestBuild->removeStatus(BuildStatus_Reciving_Solider);
		pDestBuild->removeStatus(BuildStatus_Undering_Attack);

		pDestBuild->removeComeInFormationId(mId);
		pDestBuild->insertTarFormationId(getId());

		CCLOG("[trace] pvp  doOver fightId = %d", getId());
	}

	std::string msg = StringUtils::format("%d %d %d %d %d %d", mId, mSrcCamp, mDstCamp, mSrcBuildUid, mDstBuildUid, mRowList.empty() ? 0 : 1);
	sgMessageManager()->sendMessage(EMT_FORMATION_OVER, msg);
}


void FormationV2::doRowDepart(FormationV2Row* row)
{
	BaseBuild* pSrcBuild = sgBattleControl()->getTargetBuild(mSrcBuildUid);
	if (pSrcBuild)
		pSrcBuild->addStatus(BuildStatus_Sending_Solider);

	std::string msg = StringUtils::format("%d %d %d %d %d %d", mId, mSrcCamp, mDstCamp, mSrcBuildUid, mDstBuildUid, row->getAliveEntityCount());
	sgMessageManager()->sendMessage(EMT_ROW_DEPART, msg);
}


void FormationV2::doRowArrived(FormationV2Row* row)
{
	if (nullptr == row)
		return;

	BaseBuild* pSrcBuild = sgBattleControl()->getTargetBuild(mSrcBuildUid);
	BaseBuild* pDestBuild = sgBattleControl()->getTargetBuild(mDstBuildUid);
	if (pDestBuild)		
	{
		if (row->getAliveEntityCount() > 0)
		{
			if (mSrcCamp == pDestBuild->getCamp())
			{
				pDestBuild->addStatus(BuildStatus_Reciving_Solider);

				std::string msg = StringUtils::format("%d %d %d %d %d", row->getAliveEntityCount(), mDstBuildUid, mSrcBuildUid, 0, mId);
				sgMessageManager()->sendMessage(EMT_TRANSFORM, msg);

			}
			else
			{
				pDestBuild->addStatus(BuildStatus_Undering_Attack);
				std::string msg = StringUtils::format("%d %d %d %d %d %d %d", mSrcCamp, pDestBuild->getCamp(), row->getAliveEntityCount(), mDstBuildUid, mSrcBuildUid, 0, mId);
				sgMessageManager()->sendMessage(EMT_FIGHT, msg);
			}

		}
		pDestBuild->insertComeInFormationId(mId);
	}


	for (auto& pFrozenData : mFrozenDataList)
	{
		if (pFrozenData)
			pFrozenData->mForzenRowList.erase(row);
	}

	row->doArrived();
	this->removeRowFromView(row);
}


void FormationV2::doRowEmpty(FormationV2Row* row)
{
	if (nullptr == row)
		return;

	this->removeRowFromView(row);
}


void FormationV2::doFreeze(float time)
{
	FrozenData * pData = new FrozenData();
	if (nullptr == pData)
	{
		CCLOG("[error]FormationV2::doFreeze : no memory.");
		return;
	}

	for (auto &pRow : mRowList)
	{
		pData->mForzenRowList.insert(pRow);
		pRow->doPause();
		pRow->doFreeze();
	}

	sgTimeCalculator()->createTimeCalcultor(pData->mKey, time);
	mFrozenDataList.push_back(pData);
}


void FormationV2::doGradeSoliders(int level)
{
	for (auto &pRow : mRowList)
	{
		if (pRow)
			pRow->doGradeSolisers(level);
	}
}
