#pragma once

#include "cocos2d.h"

class FormationV2Row;
class Solider;

class FormationV2Slot : public cocos2d::Ref
{
	friend FormationV2Row;

public:
	void setLogicalPosition(const cocos2d::Point& val) { mLogicalPosition = val; }
	void setLogicalPosition(float x, float y) { mLogicalPosition.set(x, y); }
	const cocos2d::Point& getLogicalPosition() const { return mLogicalPosition; }

	void setRealPosition(const cocos2d::Point& val);
	void setRealPosition(float x, float y);
	const cocos2d::Point& getRealPosition() const { return mRealPosition; }

	void setLocalZOrder(int zOrder);
	int getLocalZOrder(int) const { return mLocalZOrder; }

	void setRotation(float angle);
	float getRotation() const { return mRotationAngle; }

	void setFlippedX(bool flipped);

	float getPhysicalRadius() const { return FormationV2Slot::Radius; }

	Solider* getEntity() const { return mEntity; }
	void putEntity(Solider* entity);
	Solider* takeEntity();

	float getDodgeSpeed() const;
	void addCollision(int uid, float speed);
	void removeCollision(int uid);

protected:
	FormationV2Slot();
	virtual ~FormationV2Slot();

	bool init(FormationV2Row * owner);
	void dispose();
	void draw(const cocos2d::Mat4& transform, uint32_t flags);

protected:
	static const float Radius;
	static const float DodgeSpeed;
	static const float HomingSpeed;
	static const float DodgeMaxSpeed;

	FormationV2Row* mOwner;

	cocos2d::Point mLogicalPosition;
	cocos2d::Point mRealPosition;
	int mLocalZOrder;
	float mRotationAngle;

	float mDodgeSpeed;
	std::map<int, float> mCollisionList;

	Solider* mEntity;
};

