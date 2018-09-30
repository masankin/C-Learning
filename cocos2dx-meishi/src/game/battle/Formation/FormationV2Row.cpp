
#include <math.h>

#include "FormationV2Row.h"
#include "FormationV2.h"
#include "FormationV2Slot.h"

#include "../Map/MapObject.h"
#include "../Solider.h"

USING_NS_CC;

const float FormationV2Row::RadiusAMax = 70.0f;
const float FormationV2Row::RadiusBMax = 25.0f;

//const float FormationV2Row::Radius = 80.0f;
const float FormationV2Row::AngleVar = float(M_PI) * 60.0f / 180.0f;

float FormationV2Row::CorrectAngle(float angle)
{
	if (angle < -M_PI)
	{
		int k = static_cast<int>(angle / (2 * M_PI));
		angle = angle - k * 2 * M_PI;

		if (angle >= -2 * M_PI)
			angle += 2 * M_PI;
	}
	else if (angle > M_PI)
	{
		int k = static_cast<int>(angle / (2 * M_PI));
		angle = angle - k * 2 * M_PI;

		if (angle <= 2 * M_PI)
			angle -= 2 * M_PI;
	}

	return angle;
}

float FormationV2Row::CalculateAngleDiff(float& angleFrom, float& angleTo)
{
	//if (angleFrom < -M_PI || angleFrom > M_PI)
	//	return 0.0f;

	if (fabs(angleFrom) - M_PI > FLT_EPSILON)
		return 0.0f;

	//if (angleTo < -M_PI || angleTo > M_PI)
	//	return 0.0f;

	if (fabs(angleTo) - M_PI > FLT_EPSILON)
		return 0.0f;

	auto checkQuadrant = [](float angle)->int{

		// 0 for none
		// 1 for Alpha Quadrant
		// 2 for Beta Quadrant
		// 3 for Gamma Quadrant
		// 4 for Delta Quadrant


		/*if (angle >= 0.0f && angle < M_PI_2)
			return 1;
		else if (angle >= M_PI_2 && angle < M_PI)
			return 2;
		else if (angle >= -M_PI_2 && angle < 0.0f)
			return 4;
		else if (angle >= -M_PI && angle < -M_PI_2)
			return 3;*/

		if (angle >= FLT_EPSILON && angle - M_PI_2< FLT_EPSILON)
			return 1;
		else if (angle - M_PI_2 >= FLT_EPSILON && angle - M_PI < FLT_EPSILON)
			return 2;
		else if (angle + M_PI_2 >= FLT_EPSILON && angle < FLT_EPSILON)
			return 4;
		else if (angle + M_PI >= FLT_EPSILON && angle + M_PI_2 < FLT_EPSILON)
			return 3;

		return 0;
	};

	int srcQuadrant = checkQuadrant(angleFrom);
	int dstQuadrant = checkQuadrant(angleTo);

	float diffAngle = 0.0f;
	switch (srcQuadrant)
	{
	case 1:
	{
		switch (dstQuadrant)
		{
		case 1:
		{
			diffAngle = angleTo - angleFrom;
		}
		break;

		case 2:
		{
			diffAngle = angleTo - angleFrom;
		}
		break;

		case 3:
		{
			auto diff1 = (angleTo + M_PI * 2) - angleFrom;
			auto diff2 = angleTo - angleFrom;

			//diffAngle = fabs(diff1) < fabs(diff2) ? diff1 : diff2;

			if (fabs(diff1) < fabs(diff2))
			{
				diffAngle = diff1;

				angleTo = angleTo + M_PI * 2;
			}
			else
			{
				diffAngle = diff2;
			}
		}
		break;

		case 4:
		{
			diffAngle = angleTo - angleFrom;
		}
		break;
		}
	}
	break;

	case 2:
	{
		switch (dstQuadrant)
		{
		case 1:
		{
			diffAngle = angleTo - angleFrom;
		}
		break;

		case 2:
		{
			diffAngle = angleTo - angleFrom;
		}
		break;

		case 3:
		{
			diffAngle = (angleTo + M_PI * 2) - angleFrom;

			angleTo = angleTo + M_PI * 2;
		}
		break;

		case 4:
		{
			auto diff1 = (angleTo + M_PI * 2) - angleFrom;
			auto diff2 = angleTo - angleFrom;

			//diffAngle = fabs(diff1) < fabs(diff2) ? diff1 : diff2;

			if (fabs(diff1) < fabs(diff2))
			{
				diffAngle = diff1;

				angleTo = angleTo + M_PI * 2;
			}
			else
			{
				diffAngle = diff2;
			}
		}
		break;
		}
	}
	break;

	case 3:
	{
		switch (dstQuadrant)
		{
		case 1:
		{
			auto diff1 = angleTo - (angleFrom + M_PI * 2);
			auto diff2 = angleTo - angleFrom;

			//diffAngle = fabs(diff1) < fabs(diff2) ? diff1 : diff2;

			if (fabs(diff1) < fabs(diff2))
			{
				diffAngle = diff1;

				angleFrom = angleFrom + M_PI * 2;
			}
			else
			{
				diffAngle = diff2;
			}
		}
		break;

		case 2:
		{
			diffAngle = angleTo - (angleFrom + M_PI * 2);

			angleFrom = angleFrom + M_PI * 2;
		}
		break;

		case 3:
		{
			diffAngle = angleTo - angleFrom;
		}
		break;

		case 4:
		{
			diffAngle = angleTo - angleFrom;
		}
		break;
		}
	}
	break;

	case 4:
	{
		switch (dstQuadrant)
		{
		case 1:
		{
			diffAngle = angleTo - angleFrom;
		}
		break;

		case 2:
		{
			auto diff1 = angleTo - (angleFrom + M_PI * 2);
			auto diff2 = angleTo - angleFrom;

			//diffAngle = fabs(diff1) < fabs(diff2) ? diff1 : diff2;

			if (fabs(diff1) < fabs(diff2))
			{
				diffAngle = diff1;

				angleFrom = angleFrom + M_PI * 2;
			}
			else
			{
				diffAngle = diff2;
			}
		}
		break;

		case 3:
		{
			diffAngle = angleTo - angleFrom;
		}
		break;

		case 4:
		{
			diffAngle = angleTo - angleFrom;
		}
		break;
		}
	}
	break;

	}

	return diffAngle;
}


FormationV2Row* FormationV2Row::create(FormationV2 * owner, int count)
{
	FormationV2Row * pRet = pRet = new FormationV2Row();
	if (nullptr != pRet && pRet->init(owner, count))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		return nullptr;
	}
}


FormationV2Row::FormationV2Row() : mOwner(nullptr)
, mPauseCount(0)
, mPathStep(0)
, mStepDistance(0.0f)
, mStepBegAngle(0.0f)
, mStepEndAngle(0.0f)
, mStepAngleDiff(0.0f)

, mFaceDirectionAngle(0.0f)
, mRadiusA(0.0f)
, mRadiusB(0.0f)

, mSpeedScale(1.0f)

, mAliveEntityCount(0)
{

}


FormationV2Row::~FormationV2Row()
{

}


bool FormationV2Row::init(FormationV2 * owner, int count)
{
	if (!Node::init())
		return false;

	if (nullptr == owner)
		return false;

	mOwner = owner;

	for (int i = 0; i < count; ++i)
	{
		auto slot = createSlot();
		if (nullptr == slot)
			continue;

		mSlotList.push_back(slot);

		slot->retain();

		Solider::AttribType soliderAttribType = Solider::AttribType::kNormal;
		switch (mOwner->getAttribType())
		{
		case FormationV2::AttribType::Normal: soliderAttribType = Solider::kNormal; break;
		case FormationV2::AttribType::Speed: soliderAttribType = Solider::kSpeed; break;
		default:
			break;
		}

		slot->putEntity(Solider::create(mOwner->getId(), soliderAttribType, mOwner->getSrcCamp(), mOwner->getDstCamp()));
	}

	return true;
}


void FormationV2Row::cleanup()
{
	mOwner = nullptr;
	mPauseCount = 0;
	for (auto & slot : mSlotList)
	{
		if (slot)
		{
			slot->takeEntity();
			slot->release();
		}
	}
	mSlotList.clear();

	mStepBegPoint = Point::ZERO;
	mStepEndPoint = Point::ZERO;

	mPathStep = 0;
	mStepBegAngle = 0.0f;
	mStepEndAngle = 0.0f;
	mStepAngleDiff = 0.0f;

	mPrevPos = Point::ZERO;

	mFaceDirection = Vec2::ZERO;
	mFaceDirectionAngle = 0.0f;
	mRadiusA = 0.0f;
	mRadiusB = 0.0f;

	mSpeedDirection = Vec2::ZERO;

	mSpeedScale = 1.0f;

	mCollisionList.clear();

	mAliveEntityCount = 0;

	Node::cleanup();
}


void FormationV2Row::update(float dt, float speed, float radiusAVar, float radiusBVar, MarchState state)
{
	if (mPauseCount > 0)
		return;

	dt *= mSpeedScale;

	Node::update(dt);

	auto curPos = this->getPosition();
	auto curDirToEnd = mStepEndPoint - curPos;
	float curToEndDis = curDirToEnd.getLengthSq();
	if (curToEndDis < 4.0f)
	{
		this->setPosition(mStepEndPoint);
		return;
	}

	auto prevDirToEnd = mStepEndPoint - mPrevPos;
	if (prevDirToEnd.dot(curDirToEnd) < 0.0f)
	{
		this->setPosition(mStepEndPoint);
		return;
	}


	this->updateSlot(dt, radiusAVar, radiusBVar, state);

	/*auto disAngle = curDirToEnd.getAngle();
	auto curAngle = mFaceDirection.getAngle();
	if (fabs(curAngle - disAngle) > 1e-4)
	{
		if (fabs(disAngle - curAngle) > M_PI_2 / 1800.0f)
		{
			auto angleDiff = FormationV2Row::CalculateAngleDiff(curAngle, disAngle);
			auto angleCoe = angleDiff / fabs(angleDiff);

			curAngle += angleCoe * FormationV2Row::AngleVar * dt;

			this->setFaceDirection(curAngle);
		}
		else
		{
			this->setFaceDirection(disAngle);
		}
	}*/

	if (fabs(mFaceDirectionAngle - mStepEndAngle) > 1e-4)
	{
		float timeForStep = mStepDistance / speed;
		float angleVar = mStepAngleDiff / timeForStep;

		//auto newAngle = (mFaceDirection + angleVarDir * dt).getAngle();
		float angleDelta = angleVar * dt;
		this->setFaceDirection(mFaceDirectionAngle + angleDelta);
	}

	mPrevPos.set(curPos);
	auto nextPos = curPos + mSpeedDirection * speed * dt;
	this->setPosition(nextPos);
}


void FormationV2Row::doPause()
{
	++mPauseCount;
	if (mPauseCount <= 0)
		return;

	this->pause();

	for (auto child : _children)
	{
		child->pause();
	}
}


// void FormationV2Row::pause()
// {
// 	++mPauseCount;
// 	if (mPauseCount <= 0)
// 		return;
// 
// 	Node::pause();
// 
// 	for (auto child : _children)
// 	{
// 		child->pause();
// 	}
// }


void FormationV2Row::doResume()
{
	--mPauseCount;
	if (mPauseCount < 0)
		mPauseCount = 0;

	if (mPauseCount > 0)
		return;

	for (auto child : _children)
	{
		child->resume();
	}

	this->resume();
}


// void FormationV2Row::resume()
// {
// 	--mPauseCount;
// 	if (mPauseCount < 0)
// 		mPauseCount = 0;
// 
// 	if (mPauseCount > 0)
// 		return;
// 
// 	for (auto child : _children)
// 	{
// 		child->resume();
// 	}
// 
// 	Node::resume();
// }


void FormationV2Row::setPathStep(int step, const cocos2d::Point& begPt, const cocos2d::Point& endPt, const cocos2d::Point& begFaceDir, const cocos2d::Point& endFaceDir, float distance)
{
	mPathStep = step;

	mStepBegPoint = begPt;
	mStepEndPoint = endPt;

	mSpeedDirection = std::move((mStepEndPoint - mStepBegPoint).getNormalized());

	mStepDistance = distance;

	mStepBegAngle = begFaceDir.getAngle();
	mStepEndAngle = endFaceDir.getAngle();

	mStepAngleDiff = FormationV2Row::CalculateAngleDiff(mStepBegAngle, mStepEndAngle);

	this->setFaceDirection(mStepBegAngle);

	mPrevPos.set(begPt);
	this->setPosition(begPt);
}


void FormationV2Row::setFaceDirection(const cocos2d::Vec2& dirVec)
{
	this->setFaceDirection(dirVec.getAngle());
}


void FormationV2Row::setFaceDirection(float angle)
{
	mFaceDirectionAngle = angle;
	mFaceDirection.x = cos(angle);
	mFaceDirection.y = sin(angle);

	float angleForRot = CC_RADIANS_TO_DEGREES(angle);
	this->setRotation(90.0f - angleForRot);

	int zOrder = 1;
	if (mFaceDirection.x > 0.0f)
	{
		for (auto itr = mSlotList.begin(); itr != mSlotList.end(); ++itr)
		{
			auto & slot = *itr;
			if (slot)
			{
				slot->setRotation(angleForRot - 90.0f);
				slot->setFlippedX(false);
				slot->setLocalZOrder(zOrder);
				
				++zOrder;
			}
		}
	}
	else if (mFaceDirection.x < 0.0f)
	{
		for (auto itr = mSlotList.rbegin(); itr != mSlotList.rend(); ++itr)
		{
			auto & slot = *itr;
			if (slot)
			{
				slot->setRotation(angleForRot - 90.0f);
				slot->setFlippedX(true);
				slot->setLocalZOrder(zOrder);

				++zOrder;
			}
		}
	}
}


bool FormationV2Row::isPathStepOver() const
{
	return this->getPosition() == mStepEndPoint;
}


FormationV2Slot* FormationV2Row::createSlot()
{
	auto slot = new FormationV2Slot();
	if (slot && slot->init(this))
	{
		slot->autorelease();
		return slot;
	}
	else
	{
		delete slot;
		return nullptr;
	}
}


void FormationV2Row::updateSlot(float dt, float radiusAVar, float radiusBVar, MarchState state)
{
	mRadiusA += radiusAVar * dt;
	if (mRadiusA > FormationV2Row::RadiusAMax)
		mRadiusA = FormationV2Row::RadiusAMax;
	else if (mRadiusA < -FormationV2Row::RadiusAMax)
		mRadiusA = -FormationV2Row::RadiusAMax;

	mRadiusB += radiusBVar * dt;
	if (mRadiusB > FormationV2Row::RadiusBMax)
		mRadiusB = FormationV2Row::RadiusBMax;
	else if (mRadiusB < -FormationV2Row::RadiusBMax)
		mRadiusB = -FormationV2Row::RadiusBMax;

	auto radiusASq = mRadiusA * mRadiusA;
	auto radiusBSq = mRadiusB * mRadiusB;

	float xInterval = mRadiusA * 2 / (mSlotList.size() + 1);
	float posXItr = -mRadiusA + xInterval;
	
	int sign = mRadiusB / fabs(mRadiusB);
	float kRatio = 0.0f;
	if (mRadiusA != 0.0f)
		kRatio = mRadiusB / mRadiusA;

	for (std::size_t index = 0; index < mSlotList.size(); ++index)
	{
		auto& slot = mSlotList[index];

		if (nullptr == slot)
			continue;

		// Position
		auto posX = posXItr;
		auto posY = 0.0f;

		//if (radiusASq != 0.0f)
		//	posY = sqrt(radiusBSq - radiusBSq * posX * posX / radiusASq) * sign;

		if (posX < 0.0)
			posY = kRatio * posX + mRadiusB;
		else
			posY = -kRatio * posX + mRadiusB;

		slot->setLogicalPosition(posX, posY);

		if (state == MarchState::eWalking)
		{
			if (slot->getDodgeSpeed() != 0.0f)
			{
				const Point& realPos = slot->getRealPosition();
				posX = realPos.x;
				posX += slot->getDodgeSpeed() * dt;
				slot->setRealPosition(posX, posY);
			}
			else
			{
				const Point& realPos = slot->getRealPosition();
				const Point& logicalPos = slot->getLogicalPosition();
				if (realPos != logicalPos)
				{
					posX = realPos.x;
					if (realPos.x < logicalPos.x)
					{
						posX += FormationV2Slot::HomingSpeed * dt;
						if (posX > logicalPos.x)
							posX = logicalPos.x;
					}
					else if (realPos.x > logicalPos.x)
					{
						posX -= FormationV2Slot::HomingSpeed * dt;
						if (posX < logicalPos.x)
							posX = logicalPos.x;
					}
				}
				slot->setRealPosition(posX, posY);
			}
		}
		else
		{
			slot->setRealPosition(posX, posY);
		}

		posXItr += xInterval;
	}

}


void FormationV2Row::calculateAliveEntity()
{
	mAliveEntityCount = 0;
	for (auto & slot : mSlotList)
	{
		if (nullptr == slot)
			continue;

		Solider* pEntity = slot->getEntity();
		if (pEntity)
		{
			if (!pEntity->checkStatus(Solider::Status::kDead))
				++mAliveEntityCount;
		}
	}
}


Solider* FormationV2Row::getEntityInEllipseRange(const Point& worldPos, float radiusA, float radiusB, const std::function<bool(Solider*)>& filter /* = nullptr */)
{
	const Point curPos = this->getPosition();
	float  radius = radiusA > radiusB ? radiusA : radiusB;
	if (curPos.getDistanceSq(worldPos) > powf(radius + mRadiusA, 2))
		return nullptr;

	float radiusASq = radiusA * radiusA;
	float radiusBSq = radiusB * radiusB;

	Mat4 matToWorldTransform = this->getNodeToWorldTransform();
	Vec3 inTransformPos;
	Vec3 outTransformPos;
	for (std::size_t index = 0; index < mSlotList.size(); ++index)
	{
		auto& slot = mSlotList[index];
		if (nullptr == slot)
			continue;

		Solider* entity = slot->getEntity();
		if (nullptr == entity)
			continue;

		if (filter && !filter(entity))
			continue;

		inTransformPos.set(slot->getRealPosition().x, slot->getRealPosition().y, 0);
		outTransformPos.setZero();
		matToWorldTransform.transformPoint(inTransformPos, &outTransformPos);
		Point slotWorldPos(outTransformPos.x, outTransformPos.y);

		Vec2 temp = worldPos - slotWorldPos;
		if (temp.x * temp.x / radiusASq + temp.y * temp.y / radiusBSq <= 1.0f)
			return entity;
	}

	return nullptr;
}


Solider* FormationV2Row::getEntityInRectRange(const Point& worldPos, float width, float height, const std::function<bool(Solider*)>& filter /* = nullptr */)
{
	const Point curPos = this->getPosition();
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	float radius = halfWidth > halfHeight ? halfWidth : halfHeight;
	if (curPos.getDistanceSq(worldPos) > powf(radius + mRadiusA, 2))
		return nullptr;

	Rect rangeRect(worldPos.x - halfHeight, worldPos.y - halfHeight, width, height);

	Mat4 matToWorldTransform = this->getNodeToWorldTransform();
	Vec3 inTransformPos;
	Vec3 outTransformPos;
	for (std::size_t index = 0; index < mSlotList.size(); ++index)
	{
		auto& slot = mSlotList[index];
		if (nullptr == slot)
			continue;

		Solider* entity = slot->getEntity();
		if (nullptr == entity)
			continue;

		if (filter && !filter(entity))
			continue;

		inTransformPos.set(slot->getRealPosition().x, slot->getRealPosition().y, 0);
		outTransformPos.setZero();
		matToWorldTransform.transformPoint(inTransformPos, &outTransformPos);
		Point slotWorldPos(outTransformPos.x, outTransformPos.y);

		if (rangeRect.containsPoint(slotWorldPos))
			return entity;
	}

	return nullptr;
}


void FormationV2Row::getEntityListInEllipseRange(const Point& worldPos, float radiusA, float radiusB, std::vector<Solider*>& outList, const std::function<bool(Solider*)>& filter /* = nullptr */)
{
	const Point curPos = this->getPosition();
	float  radius = radiusA > radiusB ? radiusA : radiusB;
	if (curPos.getDistanceSq(worldPos) > powf(radius + mRadiusA, 2))
		return;

	float radiusASq = radiusA * radiusA;
	float radiusBSq = radiusB * radiusB;

	Mat4 matToWorldTransform = this->getNodeToWorldTransform();
	Vec3 inTransformPos;
	Vec3 outTransformPos;
	for (std::size_t index = 0; index < mSlotList.size(); ++index)
	{
		auto& slot = mSlotList[index];
		if (nullptr == slot)
			continue;

		Solider* entity = slot->getEntity();
		if (nullptr == entity)
			continue;

		if (filter && !filter(entity))
			continue;

		inTransformPos.set(slot->getRealPosition().x, slot->getRealPosition().y, 0);
		outTransformPos.setZero();
		matToWorldTransform.transformPoint(inTransformPos, &outTransformPos);
		Point slotWorldPos(outTransformPos.x, outTransformPos.y);

		Vec2 temp = worldPos - slotWorldPos;
		if (temp.x * temp.x / radiusASq + temp.y * temp.y / radiusBSq <= 1.0f)
			outList.push_back(entity);
	}
}


void FormationV2Row::getEntityListInRectRange(const cocos2d::Point& worldPos, float width, float height, std::vector<Solider*>& outList, const std::function<bool(Solider*)>& filter /* = nullptr */)
{
	const Point curPos = this->getPosition();
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	float radius = halfWidth > halfHeight ? halfWidth : halfHeight;
	if (curPos.getDistanceSq(worldPos) > powf(radius + mRadiusA, 2))
		return;

	Rect rangeRect(worldPos.x - halfHeight, worldPos.y - halfHeight, width, height);

	Mat4 matToWorldTransform = this->getNodeToWorldTransform();
	Vec3 inTransformPos;
	Vec3 outTransformPos;
	for (std::size_t index = 0; index < mSlotList.size(); ++index)
	{
		auto& slot = mSlotList[index];
		if (nullptr == slot)
			continue;

		Solider* entity = slot->getEntity();
		if (nullptr == entity)
			continue;

		if (filter && !filter(entity))
			continue;

		inTransformPos.set(slot->getRealPosition().x, slot->getRealPosition().y, 0);
		outTransformPos.setZero();
		matToWorldTransform.transformPoint(inTransformPos, &outTransformPos);
		Point slotWorldPos(outTransformPos.x, outTransformPos.y);

		if (rangeRect.containsPoint(slotWorldPos))
			outList.push_back(entity);
	}
}


void FormationV2Row::doCollision(MapObject* obj, bool collision)
{
	if (mPauseCount > 0)
		return;

	if (!obj)
		return;

	auto find_itr = mCollisionList.find(obj->getUid());

	if (collision)
	{
		if (find_itr == mCollisionList.end())
			mCollisionList.insert(obj->getUid());

		auto objLocalPos = this->convertToNodeSpace(obj->getPosition());

		Mat4 matToWorldTransform = this->getNodeToWorldTransform();
		Vec3 inTransformPos;
		Vec3 outTransformPos;
		for (std::size_t index = 0; index < mSlotList.size(); ++index)
		{
			auto& slot = mSlotList[index];

			if (nullptr == slot)
				continue;

			inTransformPos.set(slot->getRealPosition().x, slot->getRealPosition().y, 0);
			outTransformPos.setZero();
			matToWorldTransform.transformPoint(inTransformPos, &outTransformPos);
			Point slotWorldPos(outTransformPos.x, outTransformPos.y);

			if (obj->containsPoint(slotWorldPos))
			{
				if (objLocalPos.x < slot->getRealPosition().x)
				{
					slot->addCollision(obj->getUid(), FormationV2Slot::DodgeSpeed);
				}
				else
				{
					slot->addCollision(obj->getUid(), -FormationV2Slot::DodgeSpeed);
				}
			}
			else
			{
				slot->removeCollision(obj->getUid());
			}
		}
	}
	else
	{
		if (find_itr == mCollisionList.end())
			return;

		for (std::size_t index = 0; index < mSlotList.size(); ++index)
		{
			auto& slot = mSlotList[index];

			if (nullptr == slot)
				continue;

			slot->removeCollision(obj->getUid());
		}

		mCollisionList.erase(find_itr);
	}

}


void FormationV2Row::doArrived()
{
	for (std::size_t index = 0; index < mSlotList.size(); ++index)
	{
		auto& slot = mSlotList[index];

		if (nullptr == slot)
			continue;

		Solider* pSolider = slot->getEntity();
		if (pSolider)
			pSolider->addStatus(Solider::Status::kArrived);
	}
}


void FormationV2Row::doFreeze()
{
	for (std::size_t index = 0; index < mSlotList.size(); ++index)
	{
		auto& slot = mSlotList[index];

		if (nullptr == slot)
			continue;

		Solider* pSolider = slot->getEntity();
		if (pSolider)
			pSolider->doFreeze();
	}
}


void FormationV2Row::doUnfreeze()
{
	for (std::size_t index = 0; index < mSlotList.size(); ++index)
	{
		auto& slot = mSlotList[index];

		if (nullptr == slot)
			continue;

		Solider* pSolider = slot->getEntity();
		if (pSolider)
			pSolider->doUnfreeze();
	}
}


void FormationV2Row::doGradeSolisers(int level)
{
	for (std::size_t index = 0; index < mSlotList.size(); ++index)
	{
		auto& slot = mSlotList[index];

		if (nullptr == slot)
			continue;

		Solider* pSolider = slot->getEntity();
		if (pSolider)
			pSolider->doGradeLevel(level);
	}
}


void FormationV2Row::changeSolidersSpeedScale()
{
	float scale = mSpeedScale * mOwner->getSpeedScale(); 
	for (std::size_t index = 0; index < mSlotList.size(); ++index)
	{
		auto& slot = mSlotList[index];

		if (nullptr == slot)
			continue;

		Solider* pSolider = slot->getEntity();
		if (pSolider)
			pSolider->doSpeedScale(scale);
	}
}


void FormationV2Row::doChangeSpeedScale(float scale)
{
	if (scale == mSpeedScale)
		return;

	mSpeedScale = scale;

	this->changeSolidersSpeedScale();
}


#if (MGAME_FORMATION_DEBUG > 0)

void FormationV2Row::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(FormationV2Row::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}

void FormationV2Row::onDraw(const Mat4 &transform, uint32_t flags)
{
	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);


	// draw 4 small points
	//Vec2 points[] = { Vec2(60, 60), Vec2(70, 70), Vec2(60, 70), Vec2(70, 60) };
	Vec2 point(0, 0);
	DrawPrimitives::setPointSize(4);
	if(!mCollisionList.empty())
		DrawPrimitives::setDrawColor4B(255, 0, 255, 255);
	else
		DrawPrimitives::setDrawColor4B(255, 255, 0, 255);
		
	DrawPrimitives::drawPoints(&point, 1);
	//DrawPrimitives::drawCircle(point, FormationV2Row::Radius, 360.0f, 180, false);

	DrawPrimitives::drawRect(Point(-mRadiusA, mRadiusB), Point(mRadiusA, -mRadiusB));

	for (std::size_t index = 0; index < mSlotList.size(); ++index)
	{
		auto& slot = mSlotList[index];
		if (slot)
		{

			DrawPrimitives::setDrawColor4B(0, 255, 255, 255);

			/*for (auto& e : _collisionBuildingList)
			{
				auto building = e.first;
				if (nullptr == building)
					continue;

				auto collisionDistance = building->getPhysicalRaidus() + FormationV2Slot::PhysicalRadius;
				auto buildingLocalPos = this->convertToNodeSpace(building->getPosition());

				if (slot->getLogicalPosition().getDistanceSq(buildingLocalPos) <= collisionDistance * collisionDistance)
					DrawPrimitives::setDrawColor4B(255, 0, 0, 255);
			}*/

			slot->draw(transform, flags);
		}
	}

	//end draw
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

#endif
