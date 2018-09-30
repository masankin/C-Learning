
#ifndef _ASST_ACTIVE_SKILL_H
#define _ASST_ACTIVE_SKILL_H

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"

class Assistant;
class AsstActiveSkillData;
class Camp;
class BaseBuild;
/************************************************************************/
/*                        AsstActiveSkill                              */
/************************************************************************/
class AsstActiveSkill : public cocos2d::Ref
{
public:
	enum class Type : int
	{
		kNone = 0,
		kTroop = 1,
		kTurret = 2,
		kMoveSpeed = 3,
		kTroopView = 4,
		kAtk = 5,
		kDef = 6,
	};

	enum class CastType : int
	{
		kNone = 0,
		kAuto = 1,
		kManul = 2,
	};

	enum class Target : int
	{
		kNone = 0,
		kSelf = 1,
		kEnermy = 2,
		kALL = 3,
	};

	enum class Operator : int
	{
		kNone = 0,
		kAdd = 1,
		kSub = 2,
	};

	enum class Range : int
	{
		kNone = 0,
		kALL = 1,
		kRandom = 2,
	};

	enum class NumRule : int
	{
		kNone = 0,
		kAll = 1,
		kRandom = 2,
	};

	enum class NumType : int
	{
		kNone = 0,
		kReal = 1,
		kPercent = 2,
	};

	//Action Key
	static const std::string EffAKBeg;
	static const std::string EffAKIng;
	static const std::string EffAKEnd;

	//Frame Key
	static const std::string EffFKCast;

public:
	AsstActiveSkill();
	virtual ~AsstActiveSkill();

	virtual bool init(AsstActiveSkillData* pBase);

	unsigned int getId() const;
	const AsstActiveSkillData* getBaseData() const;

	const std::string& getCoolTimerKey() const { return mCoolTimerKey; }
	const std::string& getDelayTimerKey() const { return mDelayTimerKey; }
	const std::string& getDelayIntervalTimerKey() const { return mDelayIntervalTimerKey; }
	const std::string& getContinualTimerKey() const { return mContinualTimerKey; }
	const std::string& getIntervalTimerKey() const { return mIntervalTimerKey; }
	
	void setCaster(Assistant* pCaster);
	Assistant* getCaster() const;

	void setRandoms(std::vector<int>& randomList);
	virtual int getRandomCount() = 0;

	virtual bool check() const;

	virtual void castBeg() = 0;
	virtual void castIng() = 0;
	virtual void castEnd() = 0;

protected:
	static int RandomRange(int min, int max, int rand);

	int getFinalNum(int rand = 0) const;
	int getFinalRange(int min, int max, int rand) const;
	void getFinalTarget(int rand, std::vector<Camp*>& outList) const;
	
	std::string getEffectName() const;
	cocostudio::Armature* fetchEffect();
	void recycleEffect(cocostudio::Armature* pEffect);
	virtual void onEffMovementEvent(cocostudio::Armature *pArmature, cocostudio::MovementEventType movementType, const std::string& movementID);

protected:

	Assistant* mCaster;
	AsstActiveSkillData* mBaseData;
	std::vector<int> mRandoms;

	std::list<cocostudio::Armature*> mEffectCache;

private:
	std::string mCoolTimerKey;
	std::string mDelayTimerKey;
	std::string mDelayIntervalTimerKey;
	std::string mContinualTimerKey;
	std::string mIntervalTimerKey;
};

/************************************************************************/
/*                     AsstActiveSkillData                              */
/************************************************************************/
class AsstActiveSkillData : public cocos2d::Ref
{
public:
	static AsstActiveSkillData* create();

	unsigned int				mId;
	std::string					mName;
	std::string					mIconFile;
	std::string					mEffectfile;
	AsstActiveSkill::Type		mType;
	AsstActiveSkill::CastType	mCastType;
	float						mCoolTime;
	float						mDelayTime;
	float						mContinualTime;
	float						mIntervalTime;
	AsstActiveSkill::Target		mTarget;
	AsstActiveSkill::Range		mRange;
	int							mRgMin;
	int							mRgMax;
	AsstActiveSkill::Operator	mOperator;
	AsstActiveSkill::NumRule	mNumRule;
	AsstActiveSkill::NumType	mNumType;
	int							mInitNum;
	int							mNumRange;

protected:
	AsstActiveSkillData();
	virtual ~AsstActiveSkillData();
};

/************************************************************************/
/*                               AASTroop                               */
/************************************************************************/
class AASTroop :public AsstActiveSkill
{
public:
	AASTroop();
	~AASTroop();

public:
	virtual int getRandomCount();
	virtual void castBeg();
	virtual void castIng();
	virtual void castEnd();

protected:
	void getBuilds(Camp* pCamp, int rand, std::vector<BaseBuild*>& outList);
	virtual void onEffMovementEvent(cocostudio::Armature *pArmature, cocostudio::MovementEventType movementType, const std::string& movementID) override;

private:
	static const int RandCount;

	struct BuildInfo
	{
		float finalNum;
		cocostudio::Armature* pEffect;
	};

	std::map<int, BuildInfo> mBuildInfoList;
};

/************************************************************************/
/*                             AASTurret                                */
/************************************************************************/
class AASTurret :public AsstActiveSkill
{
public:
	AASTurret();
	~AASTurret();

public:
	virtual bool check() const;
	virtual int getRandomCount();
	virtual void castBeg();
	virtual void castIng();
	virtual void castEnd();

protected:
	void getBuilds(Camp* pCamp, int rand, std::vector<BaseBuild*>& outList);

private:
	static const int RandCount;

	struct BuildInfo
	{
		float finalNum;
		cocostudio::Armature* pEffect;
	};

	std::map<int, BuildInfo> mBuildInfoList;
};

/************************************************************************/
/*                         AASTroopView                                 */
/************************************************************************/
class AASTroopView :public AsstActiveSkill
{
public:
	AASTroopView();
	~AASTroopView();

public:
	virtual bool check() const;
	virtual int getRandomCount();
	virtual void castBeg();
	virtual void castIng();
	virtual void castEnd();

protected:
	void doShowBubble(bool bShow);
	void getBuilds(const cocos2d::Point& center, float radius, int randCount, const std::vector<BaseBuild*>& candidateList, std::vector<BaseBuild*>& outList);
	virtual void onEffMovementEvent(cocostudio::Armature *pArmature, cocostudio::MovementEventType movementType, const std::string& movementID) override;

protected:
	static const int RandCount;

	static const int ViewRangeWidth;
	static const int ViewRangeHeight;

	std::set<int> mBuildList;
	cocostudio::Armature * mEffect;
};


/************************************************************************/
/*                             AASAtk                                   */
/************************************************************************/
class AASAtk :public AsstActiveSkill
{
public:
	AASAtk();
	~AASAtk();

public:
	virtual int getRandomCount();
	virtual void castBeg();
	virtual void castIng();
	virtual void castEnd();

protected:
	void doChange();

protected:
	static const int RandCount;
	std::map<int, float> mCampInfoList;
};

#endif //_BATTLE_ASST_H
