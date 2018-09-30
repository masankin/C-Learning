#pragma once

#include "cocos2d.h"
#include "Constant.h"


class FormationV2Row;
class MapObject;
class Solider;


class FormationV2 : public cocos2d::Ref
{
public:
	static const int RowMaxCount = 6;

	enum class AttribType
	{
		Normal = 0,
		Speed = 1,
	};

protected:
	static const int StepInterval = 1;
	static const int StepInvalid = -1;

	static const float DistanceSegment;

	static const float RowInterval;

	static const std::string PauseTimerKeyFormat;

	struct PathStep {
		PathStep();
		~PathStep();

		cocos2d::Point mBegPoint;
		cocos2d::Point mEndPoint;

		cocos2d::Vec2 mBegFaceDir;
		cocos2d::Vec2 mEndFaceDir;

		float mDistance;
	};

	struct FormVarInfo{

		FormVarInfo();
		~FormVarInfo();

		void clear();

		float mRowRadiusAVarForExit;
		float mRowRadiusBVarForExit;

		float mRowRadiusAVarForWalk;
		float mRowRadiusBVarForWalk;

		float mRowRadiusAVarForEnter;
		float mRowRadiusBVarForEnter;
	};

	struct FrozenData{

		FrozenData();
		~FrozenData();

		std::string mKey;
		std::set<FormationV2Row*> mForzenRowList;
	};

public:
	static unsigned int NewId();

	static FormationV2* create(unsigned int id, int count, int begObjID, int endObjId);

	void update(float dt);
	inline unsigned int getId() const { return mId; }

	void pause();
	void resume();

	void pauseByTimer(float time);
	void resumeByTimer();

	AttribType getAttribType() const { return mAttribType; }
	void setAttribType(AttribType type) { mAttribType = type; }

	// Product
	bool checkProducer(float dt);
	void updateProducer(float dt, int count);
	void closeProducer() { mCloseProducer = true; }

	bool checkProducerOver() const;
	void doProductOver();

	void addExpectedCount(int count) { if (count > 0) mExpectedCount += count; }

	// Path Attributes
	bool resetPath(int begObjID, int endObjId);
	bool resetPath(const std::list<cocos2d::Point>& path);

	inline int getFirstStep() const { return 0; }
	inline int getLastStep() const { return static_cast<int>(mPathList.size()) - 2; }
	const PathStep* getStep(int step);

	void setBaseSpeed(float val) { mBaseSpeed = val; }
	float getBaseSpeed() const { return mBaseSpeed; }

	void setSpeed(float val);
	float getSpeed() const { return mSpeed; }

	void setSpeedScale(float val);
	float getSpeedScale() const { return mSpeedScale; }

	// Render Attributes
	void enterView(cocos2d::Node* view, int zOrder);
	void exitView();

	// Logic Attributes
	int getExpectedCount() const { return mExpectedCount; }
	int getProductedCount() const { return mProducedCount; }

	void setSrcMapObjUid(int val) { mSrcMapObjID = val; }
	int getSrcMapObjUid() const{ return mSrcMapObjID; }

	void setDstMapObjUid(int val) { mDstMapObjID = val; }
	int getDstMapObjUid() const{ return mDstMapObjID; }

	void setScrBuildUid(int val) { mSrcBuildUid = val; }
	int getSrcBuildUid() const{ return mSrcBuildUid; }

	void setDstBuildUid(int val) { mDstBuildUid = val; }
	int getDstBuildUid() const{ return mDstBuildUid; }

	void setSrcCamp(eCampType val) { mSrcCamp = val; }
	eCampType getSrcCamp() const { return mSrcCamp; }

	void setDstCamp(eCampType val) { mDstCamp = val; }
	eCampType getDstCamp() const { return mDstCamp; }

	eCampType getCurDstCamp() const;

	const Point& getPosition() const;
	int getAliveEntityCount() const;

	Solider* getEntityInEllipseRange(const cocos2d::Point& worldPos, float radiusA, float radiusB, const std::function<bool(Solider*)>& filter = nullptr);
	Solider* getEntityInRectRange(const cocos2d::Point& worldPos, float width, float height, const std::function<bool(Solider*)>& filter = nullptr);

	void getEntityListInEllipseRange(const cocos2d::Point& worldPos, float radiusA, float radiusB, std::vector<Solider*>& outList, const std::function<bool(Solider*)>& filter = nullptr);
	void getEntityListInRectRange(const cocos2d::Point& worldPos, float width, float height, std::vector<Solider*>& outList, const std::function<bool(Solider*)>& filter = nullptr);

	bool checkOver() const;
	void doOver();

	void doFreeze(float time);
	void doGradeSoliders(int level);
	
protected:
	static int CalculateStep(int curStep, int count);

	FormationV2();
	virtual ~FormationV2();

	bool init(unsigned int id, int count, int begObjID, int endObjId);
	void dispose();

	void updateProducer(float dt);
	void updateCollision(float dt);
	void updateRows(float dt);
	void updateLogic();
	void updateForced();

	void calculateFormVarInfo();
	bool checkRowCollision(FormationV2Row* row, MapObject* obj);
	bool checkRowCollision(FormationV2Row* row, const cocos2d::Point& center, float radiusA, float radiusB);

	void doRowDepart(FormationV2Row* row);
	void doRowArrived(FormationV2Row* row);
	void doRowEmpty(FormationV2Row* row);

	void addRowToView(FormationV2Row* row);
	void removeRowFromView(FormationV2Row* row);

protected:
	unsigned int mId;
	bool mUpdate;
	std::string mPauseTimerKey;
	AttribType mAttribType;

	// Path
	cocos2d::Point mBegPoint;
	cocos2d::Point mEndPoint;
	std::vector<PathStep*> mPathList;
	float mBaseSpeed;
	float mSpeed;
	float mDistance;

	float mSpeedScale;

	// Formation
	float mDistanceForExit;
	float mDistanceForEnter;

	FormVarInfo mFormVarInfo;
	std::list<FormationV2Row*> mRowList;

	// Producer
	int mExpectedCount;
	int mProducedCount;
	float mProducerTimer;
	bool mCloseProducer;

	// Render
	cocos2d::Node* mRootView;
	int mLocalZOrder;

	// Logic
	int mSrcMapObjID;
	int mDstMapObjID;

	int mSrcBuildUid;
	int mDstBuildUid;
	eCampType mSrcCamp;
	eCampType mDstCamp;

	bool mIsPlayWalkSound;

	std::list<FrozenData*> mFrozenDataList;
};
