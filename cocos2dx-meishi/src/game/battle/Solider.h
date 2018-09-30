#pragma once

#include "cocos2d.h"
#include "Constant.h"

USING_NS_CC;

class BaseBuild;
class SoliderView;
class FormationV2Slot;

class Solider : public Ref
{
public:
	enum Status
	{
		kNone = 0x00000000,
		kDead = 0x00000001,
		kArrived = 0x00000002,
		kSpotted = 0x00000004,
		kFreeze = 0x00000008,
	};

	enum AttribType
	{
		kNormal = 0,
		kSpeed = 1,
	};

public:
	static Solider* create(unsigned int fid, AttribType attribType, eCampType srcCamp, eCampType dstCamp);

	void setFormationSlot(FormationV2Slot* slot) { mSlot = slot; }

	void setStatus(Status status);
	bool checkStatus(Status status) const;

	void addStatus(Status status);
	void remvoeStatus(Status status);

	inline AttribType getAttribType() const { return mAttribType; }
	inline unsigned int getFormationId() const { return mFormationId; }

	inline eCampType getSrcCamp() const { return mSrcCamp; }
	inline eCampType getDstCamp() const { return mDstCamp; }

	inline int getLevel() const { return mLevel; }

	inline float getSpeedScale() const { return mSpeedScale; }

	// View Attributes
	SoliderView* getView(){ return mView; }
	void refreshView();

	void faceLeft();
	void faceRight();

	Point getPosition() const;
	void setPosition(const Point& position);
	void setPosition(float x, float y);
	
	const Point& getLocalPosition() const;
	void setLocalPosition(const Point& position);
	void setLocalPosition(float x, float y);

	void setLocalZOrder(int zOrder);
	int getLocalZOrder() const;

	void setLocalRotation(float angle);
	float getLocalRotation() const;

	// Action
	void doSpotted();
	void doBombHit();
	void doDead();
	void doGradeLevel(int level);
	void doSpeedScale(float scale);
	void doFreeze();
	void doUnfreeze();

protected:
	Solider();
	virtual ~Solider();

	virtual bool init(unsigned int fid, AttribType attribType, eCampType srcCamp, eCampType dstCamp);
	virtual void dispose();

protected:
	unsigned int mStatus;
	unsigned int mFormationId;

	eCampType mSrcCamp;
	eCampType mDstCamp;

	FormationV2Slot* mSlot;
	SoliderView* mView;

	AttribType mAttribType;
	int mLevel;
	float mSpeedScale;

public:

	void remove();
	void add(Node* parent);

public:

	//其他游戏元素施加力
	void addFs(Vec2 f, float dir, float speed);
	void addBuildFs(Vec2 f, float speed);
	void addBuildInFs(Vec2 f, float speed);
	void addFormationFs(Vec2 pos, float speed);
	Vec2 getRealNextPosition();
	void addF(float speed);

	//建筑范围检测
	void checkSrcAndTarBuild(BaseBuild* src, BaseBuild* tar, float speed);
	void addWillCheckBuild(BaseBuild* build);

public:

	float calculate(float dis, float maxdis, float k, float speed);
	Vec2 rotateHalfPI(Vec2 pos, float dir);

public:

	CC_SYNTHESIZE(Vec2, m_vFs, Fs);  //力，影响士兵相互的排斥与吸引
	CC_SYNTHESIZE(Vec2, m_vBuildF, BuildF); //建筑的力
	CC_SYNTHESIZE(Vec2, m_pTargetPosition, TargetPosition);
	CC_SYNTHESIZE(bool, m_bIsNearBuild, IsNearBuild);
	CC_SYNTHESIZE(bool, m_bIsLive, Live);
	CC_SYNTHESIZE(Node*, mBullet, Bullet);

protected:

	std::map<int, BaseBuild*> m_vWillCheckBuilds;
	Vec2 m_vPosition;
};
