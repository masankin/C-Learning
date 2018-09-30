#include "FormationV2Slot.h"
#include "FormationV2Row.h"
#include "../Solider.h"
#include "../SoliderView.h"

USING_NS_CC;

const float FormationV2Slot::Radius = 10.0f;
const float FormationV2Slot::DodgeSpeed = 50.0f;
const float FormationV2Slot::HomingSpeed = 50.0f;
const float FormationV2Slot::DodgeMaxSpeed = 70.0f;

FormationV2Slot::FormationV2Slot() : mOwner(nullptr)
, mLocalZOrder(0)
, mRotationAngle(0.0f)
, mDodgeSpeed(0.0f)
, mEntity(nullptr)
{

}

FormationV2Slot::~FormationV2Slot()
{

}

bool FormationV2Slot::init(FormationV2Row * owner)
{
	if (nullptr == owner)
		return false;

	mOwner = owner;

	return true;
}


void FormationV2Slot::dispose()
{
	this->takeEntity();

	mLocalZOrder = 0;
	mRotationAngle = 0.0f;

	mCollisionList.clear();
	mDodgeSpeed = 0.0f;

	mOwner = nullptr;
}


void FormationV2Slot::setRealPosition(const cocos2d::Point& val)
{
	this->setRealPosition(val.x, val.y);
}


void FormationV2Slot::setRealPosition(float x, float y)
{
	if (mEntity)
		mEntity->setLocalPosition(x, y);

	mRealPosition.set(x, y);
}


void FormationV2Slot::setLocalZOrder(int zOrder)
{
	if (mEntity)
		mEntity->setLocalZOrder(zOrder);

	mLocalZOrder = zOrder;
}


void FormationV2Slot::setRotation(float angle)
{
	if (mEntity)
		mEntity->setLocalRotation(angle);

	mRotationAngle = angle;
}


void FormationV2Slot::setFlippedX(bool flipped)
{
	if (mEntity)
	{
		if (flipped)
			mEntity->faceLeft();
		else
			mEntity->faceRight();
	}
	
}


void FormationV2Slot::putEntity(Solider* entity)
{
	if (nullptr == entity)
		return;

	mEntity = entity;
	mEntity->retain();
	mEntity->setFormationSlot(this);

	mOwner->addChild(mEntity->getView());
}


Solider* FormationV2Slot::takeEntity()
{
	if (nullptr == mEntity)
		return nullptr;

	mOwner->removeChild(mEntity->getView());
	mEntity->autorelease();
	mEntity->setFormationSlot(nullptr);

	auto entity = mEntity;
	mEntity = nullptr;

	return entity;
}


float FormationV2Slot::getDodgeSpeed() const
{
	float dodgeSpeed = 0.0f;
	for (auto itr = mCollisionList.begin(); itr != mCollisionList.end(); ++itr)
	{
		dodgeSpeed += itr->second;
	}

	if (dodgeSpeed > FormationV2Slot::DodgeMaxSpeed)
		return FormationV2Slot::DodgeMaxSpeed;
	else if (dodgeSpeed < -FormationV2Slot::DodgeMaxSpeed)
		return -FormationV2Slot::DodgeMaxSpeed;

	return dodgeSpeed;
}


void FormationV2Slot::addCollision(int uid, float speed)
{
	//mDodgeSpeed += speed;
	
	mCollisionList[uid] = speed;
}


void FormationV2Slot::removeCollision(int uid)
{
	auto find_itr = mCollisionList.find(uid);
	if (find_itr != mCollisionList.end())
	{
		//mDodgeSpeed -= find_itr->second;
		mCollisionList.erase(find_itr);
	}
}

void FormationV2Slot::draw(const Mat4 &transform, uint32_t flags)
{
	// draw 4 small points
	//Vec2 points[] = { Vec2(60, 60), Vec2(70, 70), Vec2(60, 70), Vec2(70, 60) };
	//Vec2 point(0, 0);

	// position is relative to FormationV2Row
	DrawPrimitives::setPointSize(4);
	//DrawPrimitives::setDrawColor4B(0, 255, 255, 255);
	DrawPrimitives::drawPoints(&mLogicalPosition, 1);
	DrawPrimitives::drawCircle(mRealPosition, FormationV2Slot::Radius, 360.0f, 180, false);

}
