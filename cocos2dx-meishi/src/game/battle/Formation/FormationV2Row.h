#pragma once

#include "cocos2d.h"
#include "EnvSet.h"

class FormationV2;
class FormationV2Slot;
class MapObject;
class Solider;

#define MGAME_FORMATION_DEBUG 0		// 阵营调试

class FormationV2Row : public cocos2d::Node
{
public:
	static const float RadiusAMax;
	static const float RadiusBMax;

	enum MarchState
	{
		eExiting,
		eWalking,
		eEntering,
	};

public:
	static FormationV2Row* create(FormationV2 * owner, int count);

	void doPause();
	void doResume();

	//virtual void pause() override;
	//virtual void resume() override;

	void update(float dt, float speed, float radiusAVar, float radiusBVar, MarchState state);
	
	void setPathStep(int step, const cocos2d::Point& begPt, const cocos2d::Point& endPt, const cocos2d::Point& begFaceDir, const cocos2d::Point& endFaceDir, float distance);
	void setFaceDirection(const cocos2d::Vec2& dirVec);
	void setFaceDirection(float angle);
	const cocos2d::Vec2 & getFaceDirection() const { return mFaceDirection; }

	inline float getRadiusA() const { return mRadiusA; }
	inline float getRadiusB() const { return mRadiusB; }

	int getPathStep() const { return mPathStep; }
	bool isPathStepOver() const;

	void calculateAliveEntity();
	int getAliveEntityCount() const { return mAliveEntityCount; }

	Solider* getEntityInEllipseRange(const cocos2d::Point& worldPos, float radiusA, float radiusB, const std::function<bool(Solider*)>& filter = nullptr);
	Solider* getEntityInRectRange(const cocos2d::Point& worldPos, float width, float height, const std::function<bool(Solider*)>& filter = nullptr);

	void getEntityListInEllipseRange(const cocos2d::Point& worldPos, float radiusA, float radiusB, std::vector<Solider*>& outList, const std::function<bool(Solider*)>& filter = nullptr);
	void getEntityListInRectRange(const cocos2d::Point& worldPos, float width, float height, std::vector<Solider*>& outList, const std::function<bool(Solider*)>& filter = nullptr);

	void doArrived();
	void doCollision(MapObject* obj, bool collision);
	void doFreeze();
	void doUnfreeze();
	void doGradeSolisers(int level);

	void changeSolidersSpeedScale();
	void doChangeSpeedScale(float scale);

protected:
	static float CorrectAngle(float angle);
	static float CalculateAngleDiff(float& angleFrom, float& angleTo);

	FormationV2Row();
	virtual ~FormationV2Row();

	bool init(FormationV2 * owner, int count);
	virtual void cleanup() override;

	FormationV2Slot * createSlot();
	void updateSlot(float dt, float radiusAVar, float radiusBVar, MarchState state);



protected:
	//static const float Radius;
	static const float AngleVar;

	FormationV2* mOwner;
	int mPauseCount;
	std::vector<FormationV2Slot*> mSlotList;

	cocos2d::Point mPrevPos;

	int mPathStep;
	cocos2d::Point mStepBegPoint;
	cocos2d::Point mStepEndPoint;
	float mStepDistance;
	float mStepBegAngle;
	float mStepEndAngle;
	float mStepAngleDiff;

	cocos2d::Vec2 mFaceDirection;
	float mFaceDirectionAngle;
	float mRadiusA;
	float mRadiusB;

	cocos2d::Vec2 mSpeedDirection;

	float mSpeedScale;

	int mAliveEntityCount;

	std::set<int> mCollisionList;

#if (MGAME_FORMATION_DEBUG > 0)
public:
	void setCollisionFlag(bool val) { mCollisionFlag = val; }

protected:
	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
	void onDraw(const cocos2d::Mat4& transform, uint32_t flags);

	cocos2d::CustomCommand _customCommand;

protected:
	bool mCollisionFlag;
#endif
};
