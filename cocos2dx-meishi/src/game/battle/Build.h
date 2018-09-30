/*
* jacey
* 各种建筑类
*/

#pragma once

#include "BattleInfo.h"
#include "BitSet.h"
#include "Constant.h"
#include "Camp.h"
#include "AIState.h"
#include "cocos2d.h"
#include "cocostudio/CCArmature.h"

USING_NS_CC;
using namespace cocostudio;

enum eBuildDirty
{
	EBDIR_LEVEL		= BITSET(0),
	EBDIR_PEOPLE	= BITSET(1),
	EBDIR_Ani		= BITSET(2),
	EBDIR_CAMEP		=BITSET(3),
	EBDIR_TYPE = BITSET(4),

	EBDIR_ALL = 0xFFFFFFFF,
};


enum eBuildStatus
{
	BuidStatus_Invalied = 0,
	BuildStatus_Normal = BITSET(0),
	BuildStatus_Undering_Attack = BITSET(1),
	BuildStatus_Sending_Solider = BITSET(2),
	BuildStatus_Reciving_Solider = BITSET(3),
};


enum eActionStatus
{
	BUILD_NONE,
	BUILD_BEATTACT,
	BUILD_BAD_INJURE,
	BUILD_GOOD_INJURE,
};

struct InvisibilityInfo
{
	InvisibilityInfo() :mUpgrade(true), mSend(true), mObtain(true), mUseProp(true)
	{};

	~InvisibilityInfo(){};

	void formatParams(std::string parms);

	bool mUpgrade;			//是否可以升级
	bool mSend;				//是否可以派兵
	bool mObtain;			//是否可以收兵
	bool mUseProp;			//是否使用道具
};

class BuildView;
class FormationV2;

class ActionStatus : public Ref
{
public:
	ActionStatus();
	~ActionStatus();

	bool init(eCampType cType,int id);
	static ActionStatus* create(eCampType cType,int id);
	
	void update(int people, eCampType cType);

	void change(eCampType type, int num);

	void reset();

	void setStatus(eActionStatus statu);

	eActionStatus getCurStatus();

	void addCaculateTime(eActionStatus statu);

	bool play();
	void resetPlay();
private:

	eActionStatus mStatus;

	eCampType mCamp;

	int mOriginalPeople;

	int mActionNum;

	int mId;

	bool mPlayStand;
};

class BaseBuild
{
public:
	BaseBuild();
	virtual ~BaseBuild();

	virtual void init();
	virtual void initView();
	virtual void updateUI();
	void updateTime(float dt);

	static BaseBuild*	createBuild(const BuildInfo&,bool isCopy = false);
	static void		resetCounter();

	void setVisible(bool val);
	bool isVisible() const { return mVisible; }

	void setEnable(bool val);
	bool isEnable() const { return mEnable; }

	void addStatus(eBuildStatus status);
	void removeStatus(eBuildStatus status);
	bool checkStatus(eBuildStatus status);

	// 属性表id
	int getAttrID();
	int getKeyID();
	std::string getKeyString();

	bool getIsDirty();
	bool getIsDirty(eBuildDirty) const;
	void setDirtyTag(eBuildDirty);
	void clearDirtyTag();

	Rect boundingBox();
	const Point&  getPosition() const;
	void	setContentSize(Size size);


	BuildView*	getBuildView(){return mBuildView;}
	void setBuildView(BuildView* value) {mBuildView = value;}
	 
	//  人口相关
	int		getMaxProduct();
	int		getMaxHold();
	bool	checkCanProduct();
	int		getOwnPeople() const;// {return mOwnPeople; }
	virtual void	addMinusOwnPeople(float owned);

	virtual int		requestPeople(int reqCount);

	int     getFormationRow(int& num);
	// 更新生产周期
	virtual void updateProductCycle() {}


	// 升级相关
	virtual bool	checkCanUpgrade();
	Formation* createFormation(float percent, int fId, int endId,float time = 0);
	FormationV2* createFormationV2(int fid, float percent, int begObjUid, int endObjUid);

	unsigned int getOnWayFormation(int dstBuild) const;
	void addSendingFormation(int dstBuildId, unsigned int formationId);
	void removeSendingFormation(int dstBuildId, unsigned int formationId);
	void resetFormationSendingList();

	//@param local 是否是本地操作的升级 ，网络过来的消息为 false
	virtual void	doUpgrade(bool local = true);
	virtual void	doDegrade();
	virtual void	c2sUpgrade();
	bool	isCurUpgrade();
	void	cancelUpgrade();

	void	cleanTimeCalcultor();

	// 战斗相关
	bool	checkCurBeAttack();
	//接收战斗 @return 是否被占领
	bool	doFight(eCampType attackCamp, int attackNum);

	// 当前是否在出兵中,不包括已经出去了的兵
	bool	checkAttacking();

	// AI相关
	void	doAIState(float dt);
	typedef std::map<int,vector<BaseState*>> MapState;
	MapState mStateList;

	//道具相关
	virtual bool	doFullPeopel(){ return true; };

	virtual bool	checkCanChange(eBulidType type);

	void changeBuildAni(eBuildAni enAni);

	eExpressType getExpress();
	eCampType getCamp();
	eBulidType getBuildType();
	void calculate(BaseBuild* srcBuild, eCampType type, int num,int isLastRow, int formationID);

	virtual void	update();

	BaseBuild*	copyWithType(eBulidType type);
	BaseBuild*  copyWithTypeWithoutView(eBulidType type);

	int getLevel();

	virtual void product(){};

	//服务器同步
	void s2cLevel(int level);
	void s2cCamp(int camp);
	void s2cNum(int num);
	void s2cType(int type);
	void s2cFormationList(std::set<int>& fList);

	//战斗回放同步,为防与pvp模块冲突，再写几个函数
	void recordSetLevel(int level);
	void recordSetCamp(int camp);
	void recordSetPeopleCount(int num);
	void playJungleBorn();
	void playJungleDead();

	void playJungleBuff(const char* armatureName);

	//返回占领建筑的士气
	float getOccupyBuildMoraleValue();
	//返回升级建筑的士气
	float getUpgradeBuildMoraleValue();
	//返回被占领建筑的士气
	float getLostBuildMoraleValue();

	//添加buff效果
	void addBuffView(Layer* layer);
	void removeBuffView(Layer* layer);
	

	void		insertTarFormationId(int id);
	const std::set<int>& getTarFormationIdList(){ return mTarFormationIdList; };

	void		addDiffList(int id);

	void		insertComeInFormationId(int id);
	void		removeComeInFormationId(int id);
	bool		isComeIngFormation();

	ActionStatus* getActionStatu(){ return mActionStatu; };

	void		setServerUpLevel(int level);
	int			getServerUpLevel();

	/** 建筑类型转换时候 更新建筑
	@param isNew 是否是新的道具转换类型
	*/
	void		updateBuildTypeByTime(float time,std::string key,eBulidType type, bool isNew);
	void		checkBuildChange();

	void		removeInvisibility();


	virtual void getIntoBuild(eBulidType buildType, int buildID, float num); 
	float		getMoveSpeed();

protected: 

	void	resolveAIState();


protected:

	BuildView*	mBuildView;
	CC_SYNTHESIZE(eCampType, mBaseCamp, BaseCamp);//初始阵营
	CC_SYNTHESIZE(BuildInfo, mBuildInfo, BuildInfo);
	CC_SYNTHESIZE_DIRTY(unsigned int, mLevel, Level, EBDIR_LEVEL);
	CC_SYNTHESIZE_DIRTY(eBuildAni, mCurentAni, CurentAni, EBDIR_Ani);
	CC_SYNTHESIZE(bool, mHaveDestroy, HaveDestroy);		// 建筑是否更换过阵营
	CC_SYNTHESIZE(bool, mEnableBattleTip, EnableBattleTip);	//是否能显示战斗信息
	CC_SYNTHESIZE(InvisibilityInfo*, mInviInfo, InviInfo);	//隐形斗篷道具影响建筑的状态
	CC_SYNTHESIZE(bool, mIsleveling,Isleveling);//是否升级中
	//CC_SYNTHESIZE(bool, mIsAI, IsAI);
	float		mOwnPeople;

	
	std::set<int>	mTarFormationIdList;//以当前建筑为目且已进入建筑的阵型id
	std::set<int>	mComeInList;//正在进入当中的阵型 id
	std::map<int, int>	mDiffList;//与主机阵型id不同的记录

	std::map<int, unsigned int> mFormationSendingList; // 正在出发的阵型id
	

	// build属性脏标记 对应eBuildDirty
	BitSet32	mDirtyTag;

	BitSet32	mStatus;

	bool mVisible;
	bool mEnable;

	int		mBuildID;
	static int	g_BuildID;
	int		mServerUpLevel;//服务器过来的等级

	unsigned int mBeHurtSoundId;
	bool mIsPlayingBeHurtMusic;
	float mElapsedTime;

	bool mCoypDestroy;// 拷贝后的销毁

	ActionStatus* mActionStatu;
	friend Camp;
	friend BaseBuild;
};


// 兵营
class Barracks : public BaseBuild
{
public:
	Barracks();
	virtual ~Barracks();

	virtual void init();
	virtual void	updateUI();
	virtual void	doAIState();

	virtual void updateProductCycle();

	void product();

	//道具相关
	virtual bool	doFullPeopel();

protected:



};

// 炮塔
class Turret : public BaseBuild
{
public:
	Turret();
	virtual ~Turret();

	virtual void init();

	//道具相关
	virtual bool	doFullPeopel(); 
	virtual void   doUpgrade(bool local = true);
	virtual void   c2sUpgrade();
	virtual void update();
	void Upgrade(bool local);
	void UpgradeAI(bool local = true);
	virtual void	doDegrade();

	float getBaseAtkSpeed();
	void doAdditonAtkSpeed(float speed, bool op);

protected:
	void selectChooseUpgrade();
	void resetTimer();
private:
	CC_SYNTHESIZE(int, mStep, Step);
	CC_SYNTHESIZE(int,mAtkCount,AtkCount);
	float mAdditionAtkSpeed;
};

// 研究所
class Institute : public BaseBuild
{
public:
	Institute();
	virtual ~Institute();

	virtual void init();
	virtual void	doDegrade();

	//道具相关
	virtual bool	doFullPeopel();

protected:



};

// 打野建筑
class Monster: public BaseBuild
{
public:
	Monster();
	virtual ~Monster();

	virtual void init();

	virtual void update();

	virtual bool doFullPeopel();

	void createCheckFormationTimer();
	void checkFormation();

protected:
private:
	CC_SYNTHESIZE(int, m_ShowState, ShowState);
	CC_SYNTHESIZE(float, m_WaitClearTime, WaitClearTime);  
	CC_SYNTHESIZE(float, m_BaseOwnPeople, BaseOwnPeople);
	Armature* m_BuffArmature;

};


// 野怪建筑
class CreepsBuild : public BaseBuild
{
public:
	CreepsBuild();
	virtual ~CreepsBuild();

	virtual void init() override;
	virtual void update() override;
	void cleanup();

	virtual void product() override;

	int takeBuffID();

	int getCurrentCreepsID() const { return mCreepsID; }

	void recordOnWayFormation(int formationId, eCampType atkCamp, int srcBuildUid);
	void eraseOnWayFormation(int formationId);
	void addCreepsPoint();

	bool doOpen(const Creeps* pCreeps);
	bool doDefend(eCampType atkCamp, eCampType defCamp, int atkCount, int formationId);
	void doOccupy();
	void doClose();

	bool checkOccupy(); // true for can occupy, otherwise for can't
	bool checkClose(); // true for can close,

	
private:
	void doGuideAction();
	void delayActiveCreedGuide(float dt, int location);

protected:
	struct FormationOnWayInfo{

		int mFormationID;
		eCampType mAtkCamp;
		int mSrcBuildId;
		int mRemainCount;
	};

	FormationOnWayInfo* getFormationOnWayInfo(int formationId) const;
	void resetPeople(int num);

protected:
	int mCreepPointID;
	int mCreepsID;
	int mBuffID;
	std::string mProductTimerKey;

	std::map<int, FormationOnWayInfo*> mFormationOnWayList;
};

//马厩
class StablesBuild : public BaseBuild
{
public:
	StablesBuild(); 
	void init();
	void getIntoBuild(eBulidType buildType, int buildID, float num);
	virtual bool	doFullPeopel();
	void update();	
private:  
	bool checkPeopleLimit();
	void updatePeopleLimit();			
	void doReduce();					//衰减操作
	float getReduceNum();				//从公式返回衰减比例
	int getHoldLimit();
private:
	int mReduceLevel; 
};

 
