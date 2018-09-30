#pragma once
#include "TableReader.h"
#include "IteratorWrapper.h"
#include "EnvSet.h"
#include "net/UserObj.h"
#include "channel/ChannelApi.h"


class RecourcePathItem
{
public:
	unsigned int	itemID;
	std::string		path;

private:
	void readline(std::stringstream& _stream);

	friend class RecourcePath;
};

class RecourcePath
	: public TableReader
	, public Singleton<RecourcePath>
{
public:

	RecourcePath(){}
	~RecourcePath(){ purge(); }

	bool init(){ init("GameRecourcePath.csv"); return true; };
	void init(const std::string& filename){ parse(filename, 1); }
	void purge();

	typedef std::map<unsigned int, RecourcePathItem* > RecourcePathList;
	typedef ConstMapIterator<RecourcePathList> RecourcePathListIterator;

	RecourcePathListIterator getRecourcePathIterator(void)
	{
		return RecourcePathListIterator(mRecourcePathList.begin(), mRecourcePathList.end());
	}

	// game resources path settings.
	//void initSearchPath();
	//void setOriginalSearchPath();
	//void setAdditionalSearchPath();

private:
	virtual void readline(std::stringstream& _stream)
	{
		RecourcePathItem* data = new RecourcePathItem;
		data->readline(_stream);
		mRecourcePathList.insert(RecourcePathList::value_type(data->itemID, data));
	}

	RecourcePathList mRecourcePathList;
	// file search paths
	//std::vector<std::string> mOriSearchPath;
	//std::string mAdditionalSearchPath;
};

// 建筑属性表 BuildAttr.csv
class BuildAttr
{
public:
	BuildAttr(){}
	~BuildAttr(){};

	bool init(){ return true; };
	static BuildAttr* create();

	void readline(std::stringstream& _stream);

	int id;
	unsigned int type;		// <eBulidType>
	unsigned int level;
	float proSpeed;			// 兵营产生士兵的速度
	unsigned int proLimit;	// 兵营最大人口数
	unsigned int holdLimit;	// 兵营容纳人口上限
	unsigned int addDef;	// 附加防御
	unsigned int attRadius;	// 攻击半径
	float attSpeed;			// 攻击速度
	int   attCount;			// 攻击次数
	float upAddMorale;		// 升级增加士气
	float holdAddMorale;		// 占领增加士气
	float lostAddMorale;		// 被占领增加士气
	unsigned int upNeedPro;	// 升级需要士兵个数
	float upNeedTime;		// 升级需要时间
	float moveSpeed;		//移动速度
	float exchangeRate;		//兑换比例
};


class BuildAttrTable : public TableReader
{
public:

	BuildAttrTable() {  }
	~BuildAttrTable() { purge(); }

	bool init(){ init("BuildAttr.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mBuildAttrList.clear(); };

	const BuildAttr* getBuildAttrByID(int);


	typedef std::map<unsigned int, BuildAttr*> BuildAttrList;


private:

	virtual void readline(std::stringstream& _stream)
	{
		BuildAttr* data = BuildAttr::create();
		data->readline(_stream);
		mBuildAttrList.insert(std::make_pair(data->id, data));
	}

	BuildAttrList mBuildAttrList;
};

// 关卡建筑实例表 (BuildIns.csv)
class BuildIns
{
public:
	bool init(){ return true; };
	static BuildIns* create();

	void readline(std::stringstream& _stream);

	int id;
	int gateID;
	int type;
	unsigned int levelLimit;
	unsigned int initPeople;
	unsigned int initCamp;
	unsigned int initExpress;
	std::string aiType;
	unsigned int isModesTag;	// 是否是占领模式和资源模式下的标记建筑
	unsigned int isGuardTag;	// 是否是守护模式下的守护建筑
	unsigned int scoreSpeed;	// 资源模式 下获取积分的速度 
	int creepsPoint;
};

typedef std::map<unsigned int, BuildIns*> BuildInsMap;
typedef std::vector<BuildIns* > BuildInsVec;
class BuildInsTable : public TableReader
{
public:

	BuildInsTable() { }
	~BuildInsTable() { purge(); }

	bool init(){ init("BuildIns.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mBuildInsMap.clear(); };

	const BuildIns* getBuildInsByID(int);  

	const BuildInsVec getBuildIndsByGateID(int gateID);
private:

	virtual void readline(std::stringstream& _stream)
	{
		BuildIns* data = BuildIns::create();
		data->readline(_stream);
		mBuildInsMap.insert(std::make_pair(data->id, data));
	}

	BuildInsMap mBuildInsMap;
	
};

 

class CheckPoint
{
public:
	bool init(){ return true; };
	static CheckPoint* create();

	void readline(std::stringstream& _stream);

	int id;
	std::string levelName;		// 关卡名 
	int page;				//关卡所在屏数
	unsigned int checkType;	// 胜负判定类型
	std::string star1;		// 关卡评星
	std::string star2;
	std::string star3;
	unsigned int timeLimit;
	unsigned int tool1;			// 关卡可使用的道具
	unsigned int tool2;
	unsigned int tool3;
	unsigned int viewUpgrade;	// 是否显示升级提示
	unsigned int viewEnemyInfo;	// 是否显示敌方兵力
	unsigned int viewHelpInfo;	// 是否显示救援信息
	std::string bgMap;
	std::string obstacleMap;
	std::string tileMap;
	unsigned int preCheckPt;	// 前置关卡
	unsigned int preNeedStar;	// 开启关卡需要的星星数量
	unsigned int consumeEnergy;	// 消耗体力
	unsigned int dailyTimes;	// 每次次数
	unsigned int levelType;		//关卡类型
	unsigned int branches;		// 支线数量
	unsigned int levelLogo;		//关卡装饰标示
	unsigned int baseBonus;	//通关奖励
	unsigned int perfectBonus;	//满星奖励获得最高评星的奖励
	std::string passDes;		//关卡描述
	std::string passName;		//关卡名称
	int failCount;			//失败几次可以观看视频
	int tryCount;			//失败几次可以观看视频
	int spirit;				// 魂值（火候值）
	int firstEX;			//首次通关经验
	int maxStarEX;			//满星通关经验
	int repeatEX;			//重复通关经验
	int owner;			//所属章节
	int nextStage;			//后置关卡
	int shareID;			//分享ID
	int tipsProp;			//是否提示使用道具
	int isSkip;           //可否跳关
	int menuSkipCount;    //界面跳关次数
	int failSkipCount;    //失败跳关次数
	std::string skipValue;        //跳关价格
	int hasClearanceTip;  //是否有通关提示
	std::string clearanceTipImg; //通关提示图片路径
	std::string clearanceTip; //通关提示Id

	std::vector<std::string > skipValueList;
};

class CheckPointTable : public TableReader
{
public:

	CheckPointTable() { }
	~CheckPointTable() { purge(); }
	bool init()  { init("CheckPoint.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mCheckPointList.clear(); };

	const CheckPoint* getCheckPointByID(int);
	const CheckPoint* getCheckPointByMapName(std::string szMap);
	
	typedef std::map<unsigned int, CheckPoint*> CheckPointList;
	typedef ConstMapIterator<CheckPointList> CheckPointListIterator;

	typedef std::map<std::string, CheckPoint*> MapCPointList;
	typedef ConstMapIterator<MapCPointList> MapCPointListIterator;

	CheckPointListIterator getCheckPointIterator(void)
	{
		return CheckPointListIterator(mCheckPointList.begin(), mCheckPointList.end());
	}

	MapCPointListIterator getMapCPointIterator(void)
	{
		return MapCPointListIterator(mMapCPList.begin(), mMapCPList.end());
	}
	const CheckPointList getCheckPoint(){return mCheckPointList;};
private:

	virtual void readline(std::stringstream& _stream)
	{
		CheckPoint* data = CheckPoint::create();
		data->readline(_stream);
		mCheckPointList.insert(std::make_pair(data->id, data));
		mMapCPList.insert(std::make_pair(data->tileMap, data));
	}

	CheckPointList mCheckPointList; //<idx, cp>
	MapCPointList mMapCPList;	//<mapid, cp>
};

// 拥有研究所个数，对属性的加成
class InsistuteAddAttr
{
public:
	bool init(){ return true; };
	static InsistuteAddAttr* create();

	void readline(std::stringstream& _stream);

	int id;
	int addDefence;
	int addAttack;
};


class InsistuteAddAttrTable : public TableReader
{
public:

	InsistuteAddAttrTable() { }
	~InsistuteAddAttrTable() { purge(); }

	bool init()  { init("InsistuteAddAttr.csv"); return true; };

	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mInsistuteAddAttrList.clear(); };

	const InsistuteAddAttr* getInsistuteAddAttrByID(int);


	typedef std::map<unsigned int, InsistuteAddAttr*> InsistuteAddAttrList;


private:

	virtual void readline(std::stringstream& _stream)
	{
		InsistuteAddAttr* data = InsistuteAddAttr::create();
		data->readline(_stream);
		mInsistuteAddAttrList.insert(std::make_pair(data->id, data));
	}

	InsistuteAddAttrList mInsistuteAddAttrList;
};


// ai状态行为表(AI.csv)
class StateAction
{
public:
	bool init(){ return true; };
	static StateAction* create();

	void readline(std::stringstream& _stream);

	int id;
	std::string state;
	//std::string action;

	std::vector<std::string > actionList;

	std::string time;
};


class StateActionTable : public TableReader
{
public:

	StateActionTable() {  }
	~StateActionTable() { purge(); }
	bool init()  { init("AI.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mStateActionList.clear(); };

	const StateAction* getStateActionByID(int);


	typedef std::map<unsigned int, StateAction*> StateActionList;


private:

	virtual void readline(std::stringstream& _stream)
	{
		StateAction* data = StateAction::create();
		data->readline(_stream);
		mStateActionList.insert(std::make_pair(data->id, data));
	}

	StateActionList mStateActionList;
};

// 游戏内文本(TextHelper.csv)
class TextHelper
{
public:
	bool init(){  return true; };
	static TextHelper* create();

	void readline(std::stringstream& _stream);

	int id;
	std::string text;
	
	std::string getText(){ return text; };
};

class TextHelperTable : public TableReader
{
public:

	TextHelperTable() {}
	~TextHelperTable() { purge(); }
	bool init() { init("TextHelper.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mTextHelperList.clear(); };

	const TextHelper* getTextHelperByID(int);
	typedef std::map<unsigned int, TextHelper*> TextHelperList;


private:

	virtual void readline(std::stringstream& _stream)
	{
		TextHelper* data = TextHelper::create();
		data->readline(_stream);
		mTextHelperList.insert(std::make_pair(data->id, data));
	}

	TextHelperList mTextHelperList;
};



////////////////////////////////////////////////////////////////////////////////////////////////
class TextMusicTable : public TableReader
{
public:

	TextMusicTable() {  }
	~TextMusicTable() { purge(); }
	bool init()  { init("music.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mTextHelperList.clear(); };

	const TextHelper* getTextHelperByID(int);


	typedef std::map<unsigned int, TextHelper*> TextHelperList;


private:

	virtual void readline(std::stringstream& _stream)
	{
		TextHelper* data = TextHelper::create();
		data->readline(_stream);
		mTextHelperList.insert(std::make_pair(data->id, data));
	}

	TextHelperList mTextHelperList;
};
////////////////////////////////////////////////////////////////////////////////////////////////

class MusicInfo
{
public:
	bool init(){ return true; };
	static MusicInfo* create();

	void readline(std::stringstream& _stream);

	int id;

	std::string description;	
	int wait;
	int interval;
	int durationTime;
	float volume;
	std::string fileName;
};

typedef std::map<unsigned int, MusicInfo*> MusicList;

class MusicTable : public TableReader
{
public:

	MusicTable() { /*init("Music.csv");*/ }
	~MusicTable() { purge(); }
	bool init()  { init("Music.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mMusicList.clear(); };

	const MusicInfo* getMusicInfoByID(int);

private:

	virtual void readline(std::stringstream& _stream)
	{
		MusicInfo* data = MusicInfo::create();
		data->readline(_stream);
		mMusicList.insert(std::make_pair(data->id, data));
		//CCLOG("[trace]MusicTable::readline id:%d!", data->id);
	}

	MusicList mMusicList;
};

// 游戏内文本(TextHelper.csv)
class PropInfo
{
public:
	bool init(){ return true; };
	static PropInfo* create();

	void readline(std::stringstream& _stream);

	int id;				//Id
	ePropType type;		//道具类型
	std::string range;	//作用范围
	std::string headText;
	float duration;		//作用时间
	float cdtime;		//冷却时间
	int   priority;		//优先级
	int   beginCd;		//初始是否cd
	int   useRange;		//范围类型
	int   isPve;		//是否是pve道具
	int   display;		//是否显示在道具栏
	std::string icon;	//道具图标
	std::string name;	//道具名字
	std::string explain1;	//道具描述
	std::string explain2;	//使用说明
	std::string params;	// 特殊参数列表

	std::string getIcon(){ return icon; };
	std::string getName(){ return name; };
};

typedef std::map<unsigned int, PropInfo*> PropList;

class PropTable : public TableReader
{
public:

	PropTable() {  }
	~PropTable() { purge(); }
	bool init()  { init("PropAttr.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mPropList.clear(); };

	const PropInfo* getPropInfoByID(int);

	const PropList& getPropList(){ return mPropList; };

	const PropList& getDisplayPropList(){ return mDisplayPropList; };
private:

	virtual void readline(std::stringstream& _stream)
	{
		PropInfo* data = PropInfo::create();
		data->readline(_stream);
		mPropList.insert(std::make_pair(data->id, data));
		if (data->display)
		{
			mDisplayPropList.insert(std::make_pair(data->id, data)); 
		} 
	}
	PropList mDisplayPropList;
	PropList mPropList;
	
};



// 游戏内文本(TextHelper.csv)
class ShopInfo
{
public:
	enum ShopType
	{
		Shop_Diamond = 1,
		Shop_Gift,
	};
	bool init(){ return true; };
	static ShopInfo* create();

	void readline(std::stringstream& _stream);

	int id;
	int type;
	std::string  goldStr;
	int   gold;//充值金额
	int manual;
	int gift;
	int giftCout;
	int  energy;//标识
	float cdtime;
	std::string icon;
	std::string img;
	std::string other;
	std::string text;
	
	std::string name;
	std::string startTime;
	std::string endTime;
	int            leftCount;
	int            giftId;
	int            discount;
	int            priceGold;
	std::string giftIcon;
	
	int           leftDay;
	bool        isShow;
	std::vector<int> mSdkIsOpen;

	bool isFirst;
	int giftType;
};

typedef std::map<unsigned int, ShopInfo*> ShopList;

class ShopTable : public TableReader
{
public:

	ShopTable() {  }
	~ShopTable() { purge(); }
	bool init()  { init("shop.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mShopList.clear(); mGiftList.clear();}

	const ShopInfo* getShopInfoByID(int);
	void                   updateShopInfo(std::map<int,ShopInfo>);
	const ShopList& getShopList();
	const ShopList& getGiftList(){return mGiftList;}
private:

	virtual void readline(std::stringstream& _stream)
	{
		ShopInfo* data = ShopInfo::create();
		data->readline(_stream);

		if (data->type == ShopInfo::ShopType::Shop_Diamond)
		{
			if (data->mSdkIsOpen.size() == 3 && data->mSdkIsOpen[sgChannelApi()->getPaySdkType()] == 1)
			{
				mShopList.insert(std::make_pair(data->id, data));
			}
		}
		else if (data->type == ShopInfo::ShopType::Shop_Gift && data->giftType == 2)
		{
			mGiftList.insert(std::make_pair(data->id, data));
		}
	}

	ShopList mShopList;
	std::map<unsigned int, ShopInfo*> mGiftList;
};


// itembuy
class ItemBuyInfo
{
public:
	bool init(){ return true; };
	static ItemBuyInfo* create();

	void readline(std::stringstream& _stream);

	int id;
	int PropId;
	int num;
	int moneynum;
	std::string icon;
	std::string energy;
	std::string text;
};

typedef std::map<unsigned int, ItemBuyInfo*> ItemBuyList;

class ItemBuyTable : public TableReader
{
public:

	ItemBuyTable() {}
	~ItemBuyTable() { purge(); }
	bool init()  { init("itembuy.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mItemBuyList.clear(); };

	const ItemBuyInfo* getItemBuyInfoByID(int);

	const ItemBuyList& getItemBuyList();

private:

	virtual void readline(std::stringstream& _stream)
	{
		ItemBuyInfo* data = ItemBuyInfo::create();
		data->readline(_stream);
		mItemBuyList.insert(std::make_pair(data->id, data));
	}

	ItemBuyList mItemBuyList;
};

///////////////////////新手引导表///////////////////////////////
enum eGuideFocusType
{
	E_FOCUS_CIRCLE =1,
	E_FOCUS_RECT = 2
};
class GuideData
{
public:
	GuideData();
	GuideData(const GuideData* copy);
	~GuideData();
	bool init();
	static GuideData* create();
	void readline(std::stringstream& _stream);
public:
	//是否组完成
	CC_SYNTHESIZE(bool, m_GroupFinish, GroupFinish);

	//ID
	CC_SYNTHESIZE(int, m_ID, ID); 
	//关卡引导标记
	CC_SYNTHESIZE(int, m_GateGuide, GataGuide);
	//新手引导ID
	CC_SYNTHESIZE(int, m_NodeID, NodeID);
	//指引ID
	CC_SYNTHESIZE(int, m_GuideID, GuideID); 
	//该组任务对应的ID
	CC_SYNTHESIZE(float, m_GroupID, GroupID); 
	//引导类型
	CC_SYNTHESIZE(int, m_Type, Type);
	//步骤
	CC_SYNTHESIZE(int, m_Step, Step);
	//可跳过
	CC_SYNTHESIZE(int, m_Skip, Skip);
	//操作方式：单击，双击，拖动
	CC_SYNTHESIZE(int, m_GuideMode, GuideMode); 
	//描述文本ID
	CC_SYNTHESIZE(int, m_GuideTextID, GuideTextID); 
	//聚焦形状
	CC_SYNTHESIZE(int, m_FocusType, FocusType); 
	//对应相关的节点
	CC_SYNTHESIZE(std::string, m_GuideNodeID, GuideNodeID); 
	CC_SYNTHESIZE(Point, m_GuideNodePos, GuideNodePos); 
	//拖曳需要的节点
	CC_SYNTHESIZE(std::string, m_DragNodeID, DragNodeID);
	CC_SYNTHESIZE(Point, m_DragNodePos, DragNodePos);
	//是否强制执行
	CC_SYNTHESIZE(int, m_IsForce, IsForce);
	//定时器触发
	CC_SYNTHESIZE(float, m_Timer, Timer);
	//下一个指引的ID
	CC_SYNTHESIZE(int, m_NextGuideID, NextGuideID);
	//上一个指引的ID
	CC_SYNTHESIZE(int, m_PreGuideID, PreGuideID);
	//触发条件---关卡ID
	CC_SYNTHESIZE(int, m_GateID, GateID);
	//触发条件---等级
	CC_SYNTHESIZE(int, m_Level, Level); 
	//触发条件---兵力数
	CC_SYNTHESIZE(int, m_SoldierNum, SoldierNum);	
	//触发条件---目标建筑
	CC_SYNTHESIZE(int, m_TargetID, TargetID);
	//是否完成
	CC_SYNTHESIZE(bool, m_IsFinish, IsFinish);

	CC_SYNTHESIZE(bool, m_IsShowHand, IsShowHand);

	//当前步骤完成后下一个的等待时间
	CC_SYNTHESIZE(float, m_WaitTime, WaitTime);
	//完成后是否隐藏
	CC_SYNTHESIZE(bool, m_IsShow, IsShow); 
	//妹纸图片
	CC_SYNTHESIZE(std::string, m_imgFile, ImgFile); 

	//附加参数
	CC_SYNTHESIZE(std::string, m_GuideFrameSize, GuideFrameSize);
	CC_SYNTHESIZE(int, m_ExtraFunc, ExtraFunc);					//引导完毕后执行额外的方法
	CC_SYNTHESIZE(int, m_ExtraParam, ExtraParam);				//方法参数
	CC_SYNTHESIZE(int, m_NpcTurn, NpcTurn);						//NPC翻转
	CC_SYNTHESIZE(std::string, m_NpcPos, NpcPos);				//NPC位置
	CC_SYNTHESIZE(int, m_IsPause, IsPause);						//是否触发暂停
	CC_SYNTHESIZE(int, m_IsBattleGuide, IsBattleGuide);			//标记是否战斗引导(无效)
	CC_SYNTHESIZE(int, m_IsFingerAni, IsFingerAni);				//标记手指还是指针
	CC_SYNTHESIZE(std::string, m_PointPos, PointPos);			//指针位置
	CC_SYNTHESIZE(float, m_PointAngle, PointAngle);				//指针角度
	CC_SYNTHESIZE(int, m_TouchAtOnce, TouchAtOnce);				//触摸是否立即生效
	CC_SYNTHESIZE(int, m_EnableFocus, EnableFocus);				//是否有聚焦过程
	CC_SYNTHESIZE(int, m_OperatorType, OperatorType);			//操作类型 ： 0 - 点击 1 - 拖动 2 - 双击 
	CC_SYNTHESIZE(int, m_CanRollback, CanRollback);			    //可以回滚
	CC_SYNTHESIZE(int, m_RookieGateID, RookieGateID);			//受到训练营关卡引导
	CC_SYNTHESIZE(int, m_SaveAtOnce, SaveAtOnce);				//立即存储  
	
};

typedef std::vector<const GuideData*> GuideDataVec;
class GuideDataTable : public TableReader
{
public:

	GuideDataTable() { m_GuideDataVec.clear(); }
	~GuideDataTable() { purge(); }
	bool init()  { init("Guide.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { m_GuideDataVec.clear(); };
	//由于存在同一个id而不同操作的guideID，所以返回了第一个符合条件的ID
	const GuideData* getGuideDataByID(int);  
	
	GuideDataVec getGuideDataVec();

private:

	virtual void readline(std::stringstream& _stream)
	{
		GuideData* data = GuideData::create();
		data->readline(_stream);
		m_GuideDataVec.push_back(data);
	}

	GuideDataVec m_GuideDataVec;
};

enum AchieveModule
{
	A_M_PVP = 3,
	A_M_PVE = 1,
	A_M_OTHER = 2
};

enum AchieveCategory
{
	A_NULL = 0,
	A_Point = 1,
	A_Star,
	A_AddUp,
	A_Last,
};

class AchievementAward
{
public:
	struct AchieveInfo
	{
		int mstatus;
		int mReceivePlayerCount;
		int mCompleteCount;
	};

	bool init(){ return true; };
	static AchievementAward* create();

	void readline(std::stringstream& _stream);

	int id;                               //id
	int module;					//模块
	unsigned int category;     //成就类别

	int point_Type;			//关卡类别
	int pointId;              //通关关卡Id
	int pvePropCount;		//pve道具使用次数

	int pointType;         //关卡章节
	int getStar;			 //获得评星数

	int addUpType;       //累积类型
	int fightCount;       //赛季
	int danGrad;          //段位
	int pvpModule;     //对战模式
	int pvpCount;       //对战次数
	int addTotalProgress;   //累计总次数

	int Award;             //奖励成就点数
	std::string describe;    //成就描述
	std::string taskName;   //任务名字
	
	int mstatus;      //状态
	int mGetPlayer; //领取人数
	int mProgress;  //进度

	int mGetPlayerBegain; //领取人数默认值

	int mIcon;
};

class AchievementAwardTable : public TableReader
{
public:

	AchievementAwardTable() {  }
	~AchievementAwardTable() { purge(); }
	bool init()  { init("AchievementAward.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() {
		//未释放
		m_AchievePointMap.clear();
		m_AchieveStarMap.clear();
		m_AchieveAddUpMap.clear();
		m_AchieveLastMap.clear();
	};
	AchievementAward* getAchievementByKey(AchieveCategory categroy,int key);
	AchievementAward* getAchievementById(int id);
	int getAchievementCount(AchieveCategory);
	bool isAchieveMentAwardComplent(AchieveModule, bool isLast = false);
	void sortAchievementAward(std::map<int,AchievementAward::AchieveInfo>& pStatusMap);
	
private:
	AchieveCategory getAchieveCategory(int id, int progress = 0, int getPlayeres = 0);
	void sortAchieveByCategory(AchieveCategory category, std::vector<int> nogetVec,std::vector<int> getedVec);
	virtual void readline(std::stringstream& _stream)
	{
		AchievementAward* data = AchievementAward::create();
		data->readline(_stream);
		switch (data->module)
		{
		case A_M_PVE:
			m_AchievePointMap.push_back(data);
			break;
		case A_M_OTHER:
			m_AchieveStarMap.push_back(data);
			break;
		case A_M_PVP:
			m_AchieveAddUpMap.push_back(data);
			break;
		default:
			break;
		}
	}

	std::vector<AchievementAward*> m_AchievePointMap;//关卡
	std::vector<AchievementAward*> m_AchieveStarMap;//其他
	std::vector<AchievementAward*> m_AchieveAddUpMap;//对战
	std::vector<AchievementAward*> m_AchieveLastMap;

	std::vector<AchievementAward*> m_AchieveComplentMap;
	std::vector<AchievementAward*> m_AchieveNoComplentMap;
};

struct GiftInfo
{
	UserObj::PROPERTYTYPE mType;//类型
	int	mId;//二级id
	int	mNumber;//数量
};

class GiftAward 
{
public:
	GiftAward();
	virtual ~GiftAward();
	bool init(){ return true; };
	static GiftAward* create();

	void readline(std::stringstream& _stream);
	std::vector<GiftInfo> mGiftData;

	int id;			//礼包ID
	int gold;		//金币
	int manual;		//体力
	int energy;		//精力
	int roomcard;   //开房券
	int mHonor;		//荣誉
	int mBageId;	//勋章Id
	std::string mTitle; //礼包名
	std::string mOrign; //礼包来源

	std::map<int, int >itemMap; //解析后的道具
	std::vector<int> itemIdList;
	std::vector<int> itemCountList;

	int getGold(){ return gold; };
	int getPvePower(){ return manual; };
	int getPvpPower(){ return energy; };
	int getRoomCard(){ return roomcard; };
	int getHonor(){ return mHonor; };
	int getPropIdByIndex(int index) const{ return (index > itemIdList.size()) ? 0 : itemIdList.at(index - 1); };
	int getPropCountByIndex(int index){ return itemCountList.at(index-1);};

	int getGiftInfoSize(){ return mGiftData.size(); };
	int getGiftInfoTypeByIndex(int index){ return mGiftData.at(index).mType; };
	int getGiftInfoIdByIndex(int index) { return mGiftData.at(index).mId; };
	int getGiftInfoNumberByIndex(int index) { return mGiftData.at(index).mNumber; };
};

class GiftTable : public TableReader
{
public:

	GiftTable() {  }
	~GiftTable() { purge(); }
	bool init()  { init("Gift.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() {
		mGiftAwadrVec.clear();
	};
	GiftAward* getGiftAwardById(int id);
	typedef std::vector<GiftAward*> GiftAwardVec;
private:
	virtual void readline(std::stringstream& _stream)
	{
		GiftAward* data = GiftAward::create();
		data->readline(_stream);
		mGiftAwadrVec.push_back(data);
	}
	GiftAwardVec mGiftAwadrVec;
};
class AchievePointAward
{
public:
	bool init(){ return true; };
	static AchievePointAward* create();

	void readline(std::stringstream& _stream);

	int id;                               //id
	int achievePoint;     //成就点
	int giftId;     //礼包Id
	std::string icon;	//Icon
};

//////////////////////////////////////////////////////
class AchievePointAwardTable : public TableReader
{
public:

	AchievePointAwardTable() {  }
	~AchievePointAwardTable() { purge(); }
	bool init()  { init("AchievePointAward.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() {
		mAwardPointMap.clear();
	};
	AchievePointAward* getAchievePointAward(int id);
private:

	virtual void readline(std::stringstream& _stream)
	{
		AchievePointAward* data = AchievePointAward::create();
		data->readline(_stream);
		mAwardPointMap.insert(std::make_pair(data->id,data));
	}

	std::map<int,AchievePointAward*> mAwardPointMap;
};

///////////////////////打野配置表/////////////////////////////// 
class JungleData
{
public:
	JungleData();
	~JungleData();
	bool init();
	static JungleData* create();
	void readline(std::stringstream& _stream);
public:
	CC_SYNTHESIZE(int, m_ID, ID);
	CC_SYNTHESIZE(int, m_GateID, GateID);
	CC_SYNTHESIZE(int, m_BuildID, BuildID);
	CC_SYNTHESIZE(int, m_LocationID, LocationID);
	CC_SYNTHESIZE(int, m_RefreshWeight, RefreshWeight);
	CC_SYNTHESIZE(int, m_BuffID, BuffID);
	CC_SYNTHESIZE(int, m_RefreshCount, RefreshCount); 
	CC_SYNTHESIZE(int, m_TotalCount, TotalCount);
	CC_SYNTHESIZE(float, m_RefreshTime, RefreshTime); 
	CC_SYNTHESIZE(std::string, m_AnimationName, AnimationName);
	CC_SYNTHESIZE(int, m_RandRangeStar, RandRangeStar);
	CC_SYNTHESIZE(int, m_RandRangeEnd, RandRangeEnd);
};

typedef std::vector<JungleData*> JungleDataVec;
class JungleDataTable : public TableReader
{
public:

	JungleDataTable() {  }
	~JungleDataTable() { purge(); }
	bool init()  { init("JungleData.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { m_JungleDataVec.clear(); }; 

	const JungleData* getJungleDataByID(int buildID);   
	JungleDataVec getJungleDataVec();
	JungleDataVec getJungleDataVecByID(int gateID);
private:

	virtual void readline(std::stringstream& _stream)
	{
		JungleData* data = JungleData::create();
		data->readline(_stream);
		m_JungleDataVec.push_back(data);
	}

	JungleDataVec m_JungleDataVec;
}; 


//////////////////////////士气防御计算数据表///////////////////////////////////
class MoraleDefenceData
{
public:
	MoraleDefenceData();
	~MoraleDefenceData(); 
	static MoraleDefenceData* create();
	void readline(std::stringstream& _stream);
private:
	CC_SYNTHESIZE(int, m_ID, ID);
	CC_SYNTHESIZE(float, m_MoraleStart, MoraleStart);
	CC_SYNTHESIZE(float, m_MoraleEnd, MoraleEnd);
	CC_SYNTHESIZE(float, m_AddDefenceMoral, AddDefenceMoral);
}; 

typedef std::vector<MoraleDefenceData* > MoraleDefenceDataVec;
class MoraleDefenceDataTable: public TableReader
{
public:

	MoraleDefenceDataTable() {  }
	~MoraleDefenceDataTable() { purge(); }
	bool init()  { init("MoraleDefenceData.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { m_MoraleDefenceDataVec.clear(); }; 
	   
	MoraleDefenceData* getMoraleDefenceDataByRange(float moraleValue);
	MoraleDefenceDataVec getMoraleDefenceDataVec();
	
private:

	virtual void readline(std::stringstream& _stream)
	{
		MoraleDefenceData* data = MoraleDefenceData::create();
		data->readline(_stream);
		m_MoraleDefenceDataVec.push_back(data);
	}
	MoraleDefenceDataVec m_MoraleDefenceDataVec; 
};


////////////////////////士气加成buff表///////////////////////////////////
class MoraleBuffData
{
public:
	MoraleBuffData();
	~MoraleBuffData(); 
	static MoraleBuffData* create();
	void readline(std::stringstream& _stream);
private:
	CC_SYNTHESIZE(int, m_ID, ID);
	CC_SYNTHESIZE(int, m_MoraleStart, MoraleStart);
	CC_SYNTHESIZE(int, m_MoraleEnd, MoraleEnd);
	CC_SYNTHESIZE(int, m_BuffID, BuffID);
	CC_SYNTHESIZE(int, m_Level, Level);
}; 

typedef std::vector<MoraleBuffData* > MoraleBuffDataVec;
class MoraleBuffDataTable: public TableReader
{
public:

	MoraleBuffDataTable() { }
	~MoraleBuffDataTable() { purge(); }
	bool init()  { init("MoraleBuffData.csv");  return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { m_MoraleBuffDataVec.clear(); }; 

	MoraleBuffDataVec getMoraleBuffDataVecByRange(float moraleValue);
	MoraleBuffDataVec getMoraleBuffDataVec();
	int getMoraleValueLevel(float moraleValue);
private:

	virtual void readline(std::stringstream& _stream)
	{
		MoraleBuffData* data = MoraleBuffData::create();
		data->readline(_stream);
		m_MoraleBuffDataVec.push_back(data);
	}
	MoraleBuffDataVec m_MoraleBuffDataVec; 
};
 


//////////////////////Buff表/////////////////////////////////// 
enum eMutexType
{ 
	E_NONE_MUTEX = 0, //完全非互斥
	E_TYPE_MUTEX, //同类型互斥
	E_ID_MUTEX, //同buffID互斥
	E_ALL_MUTEX //全互斥(清除列表所有buff) 
};
class BuffData
{
public:
	BuffData();
	BuffData(const BuffData* copy);
	~BuffData(); 
	static BuffData* create();
	void readline(std::stringstream& _stream);
public:
	int m_ID;
	int m_BuffType;
	float m_AddValue;
	float m_AddRate;
	float m_CDTime;
	int m_targetType;
	int m_targetNum;
	int m_mutex; 
}; 

typedef std::vector<BuffData* > BuffDataVec;
typedef std::map<int, BuffData* > BuffDataMap;
class BuffDataTable: public TableReader
{
public:

	BuffDataTable() { }
	~BuffDataTable() { purge(); }
	bool init()  { init("BuffData.csv");  return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { m_BuffDataMap.clear(); }; 

	BuffData* getBuffDataByID(int id); 
private:

	virtual void readline(std::stringstream& _stream)
	{
		BuffData* data = BuffData::create();
		data->readline(_stream);
		m_BuffDataMap.insert(std::make_pair(data->m_ID, data));
	} 
	BuffDataMap m_BuffDataMap;
};
  

////////////////////////连胜奖励winner表///////////////////////////////////
class WinnerData
{
public:
	WinnerData();
	~WinnerData(); 
	static WinnerData* create();
	void readline(std::stringstream& _stream);
private:
	CC_SYNTHESIZE(int, m_ID, ID);
	CC_SYNTHESIZE(int, m_GoldNum, GoldNum);
}; 

typedef std::vector<WinnerData* > WinnerDataVec;
class WinnerDataTable: public TableReader
{
public:

	WinnerDataTable() { }
	~WinnerDataTable() { purge(); }
	bool init()  { init("Winner.csv");  return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { m_WinnerDataVec.clear(); }; 

	WinnerData* getWinnerData(int id);
	int getWinnerDataNum();
private:

	virtual void readline(std::stringstream& _stream)
	{
		WinnerData* data = WinnerData::create();
		data->readline(_stream);
		m_WinnerDataVec.push_back(data);
	}
	WinnerDataVec m_WinnerDataVec; 
};

class ActLogin 
{
public:
	bool init(){ return true; };
	static ActLogin* create();

	void readline(std::stringstream& _stream);

	int mId;
	int mDay;
	int mGiftSpe;
	int mGiftNor;

};

class ActLoginTable : public TableReader
{
public:

	ActLoginTable() { }
	~ActLoginTable() { purge(); }
	bool init()  { init("ActLoginAward.csv");  return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() {
		mActLoginMap.clear();
	};
	ActLogin* getActLogin(int id);
private:

	virtual void readline(std::stringstream& _stream)
	{
		ActLogin* data = ActLogin::create();
		data->readline(_stream);
		mActLoginMap.insert(std::make_pair(data->mDay,data));
	}

	std::map<int,ActLogin*> mActLoginMap;
};

////////////////////////Notice表///////////////////////////////////
class NoticeData
{
public:
	NoticeData();
	~NoticeData(); 
	static NoticeData* create();
	void readline(std::stringstream& _stream);
private:
	int m_User;
	int m_Type;
	int m_Urgent;
	int m_TimeGap;
	int m_MsgType;
	int m_Value;
	int m_Text;
	CC_SYNTHESIZE(int, m_ID, ID);
	CC_SYNTHESIZE(int, m_NoticeCount, NoticeCount);
	CC_SYNTHESIZE(std::string, m_Title, Title);
	CC_SYNTHESIZE(std::string, m_Content, NoticeContent);
}; 

typedef std::vector<NoticeData* > NoticeDataVec;
class NoticeDataTable: public TableReader
{
public:

	NoticeDataTable() { /*init(); */}
	~NoticeDataTable() { purge(); }
	bool init()  { init("Notice.csv");  return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { m_NoticeDataVec.clear(); }; 
	NoticeData* getNoticeData(int id);
	NoticeDataVec* getNoticeDataVec();
private:

	virtual void readline(std::stringstream& _stream)
	{
		NoticeData* data = NoticeData::create();
		data->readline(_stream);
		m_NoticeDataVec.push_back(data);
	}
	NoticeDataVec m_NoticeDataVec; 
};


	////////////////////////RechargeActivity表///////////////////////////////////
class RechargeData
{
public:
	RechargeData();
	~RechargeData(); 
	static RechargeData* create();
	void readline(std::stringstream& _stream);
private:

	CC_SYNTHESIZE(int, mID, ID);
	CC_SYNTHESIZE(int, mStage, StageId);
	CC_SYNTHESIZE(std::string, mStartTime, StartTime);
	CC_SYNTHESIZE(std::string, mEndTime, EndTime);
	CC_SYNTHESIZE(int, mRechargeType, RechargeType);
	CC_SYNTHESIZE(int, mRechargeNum, RechargeNum);
	CC_SYNTHESIZE(int, mReward, Reward);
	CC_SYNTHESIZE(int, mIsGet, IsGet);
}; 

typedef std::map<unsigned int, RechargeData*> RechargeList;
class RechargeActivityTable: public TableReader
{
public:

	RechargeActivityTable() {  }
	~RechargeActivityTable() { purge(); }

	bool init()  { init("RechargeActivity.csv");  return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mRechargeList.clear(); }; 

	RechargeList* getRechargeList();

	RechargeData* getRechargeData(int id);

private:

	virtual void readline(std::stringstream& _stream)
	{
		RechargeData* data = RechargeData::create();
		data->readline(_stream);
		mRechargeList.insert(std::make_pair(data->getID(),data));
	}
	RechargeList mRechargeList; 
};


////////////////////////powerBuy.csv表///////////////////////////////////
class PowerBuy
{
public:
	PowerBuy();
	~PowerBuy(); 
	static PowerBuy* create();
	void readline(std::stringstream& _stream);
private:

	CC_SYNTHESIZE_READONLY(int, mID, ID);
	CC_SYNTHESIZE_READONLY(int, mPowerNum, PowerNum);
	CC_SYNTHESIZE_READONLY(int, mEnergyNum, EnergyNum);
}; 

typedef std::map<unsigned int, PowerBuy*> PowerBuyList;
class PowerBuyTable: public TableReader
{
public:

	PowerBuyTable() {  }
	~PowerBuyTable() { purge(); }
	
	bool init()  { init("powerBuy.csv");  return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mPowerBuyList.clear(); }; 

	PowerBuyList* getPowerBuyList();

	PowerBuy* getPowerBuy(int id);

private:

	virtual void readline(std::stringstream& _stream)
	{
		PowerBuy* data = PowerBuy::create();
		data->readline(_stream);
		mPowerBuyList.insert(std::make_pair(data->getID(),data));
	}
	PowerBuyList mPowerBuyList; 
};

////////////////// 每日任务表 ///////////////
class DayTaskData
{
public:
	DayTaskData();
	~DayTaskData();
	static DayTaskData* create();
	void readline(std::stringstream& _stream);
private:

	CC_SYNTHESIZE_READONLY(int, mID, ID);
	CC_SYNTHESIZE_READONLY(int, mType, Type);
	CC_SYNTHESIZE_READONLY(int, mHardLevel, HardLevel)
	CC_SYNTHESIZE_READONLY(int, mRequire, Rrequire);
	CC_SYNTHESIZE_READONLY(int, mValue, Value);
	//CC_SYNTHESIZE_READONLY(std::string, mRewards, Rewards);
	//CC_SYNTHESIZE_READONLY(int, mDiamond, Diamond);
	//CC_SYNTHESIZE_READONLY(int, mPvePower, PvePower);
	//CC_SYNTHESIZE_READONLY(int, mPvpPower, PvpPower);
	//CC_SYNTHESIZE_READONLY(int, mRoomTicket, RoomTicket);
	CC_SYNTHESIZE_READONLY(int, mGiftId, GiftId);
	CC_SYNTHESIZE_READONLY(std::string, mTitle, Title);
	CC_SYNTHESIZE_READONLY(std::string, mDesc, Desc);

};

typedef std::map<unsigned int, DayTaskData*> DayTaskDataList;

class DayTaskTable : public TableReader
{
public:
	DayTaskTable() { }
	virtual ~DayTaskTable() { purge(); }
	virtual bool init() { parse("mission.csv", 2); return true; }
	void purge() { mDayTaskDataList.clear(); }

	DayTaskDataList* getDayTaskList() { return &mDayTaskDataList; }

	DayTaskData* getDayTaskById(int id);

private:
	virtual void readline(std::stringstream& _stream)
	{
		DayTaskData* data = DayTaskData::create();
		data->readline(_stream);
		mDayTaskDataList.insert(std::make_pair(data->getID(), data));
	}

	DayTaskDataList mDayTaskDataList;
};

////////////////////////野怪点表///////////////////////////////////
class CreepsPoint
{
public:
	static CreepsPoint* create();

	CreepsPoint();
	~CreepsPoint();
	void readline(std::stringstream& _stream);

	int getID() const { return mID; }
	int getGroup() const { return mGroup; }
	float getProb() const { return mProb; }
	float getInterval() const { return mInterval; }
	const std::vector<int>& getCreepsList() const { return mCreepsList; }
	
private:
	int mID;
	int mGroup;
	float mProb;
	float mInterval;
	std::vector<int> mCreepsList;
};


class CreepsPointTable : public TableReader
{
public:

	CreepsPointTable() { }
	~CreepsPointTable() { purge(); }

	virtual bool init() { parse("CreepsPoint.csv", 2); return true; }
	void purge();

	CreepsPoint* getCreepsPoint(int id);

private:

	virtual void readline(std::stringstream& _stream)
	{
		CreepsPoint* pData = CreepsPoint::create();
		if (pData)
		{
			pData->readline(_stream);
			mTable.insert(std::make_pair(pData->getID(), pData));
		}
	}

	std::unordered_map<int, CreepsPoint*> mTable;
};


////////////////////////野怪表///////////////////////////////////
class Creeps
{
public:
	static Creeps* create();

	Creeps();
	~Creeps();
	void readline(std::stringstream& _stream);

	int getID() const { return mID; }
	int getType() const { return mType; }
	float getProb() const { return mProb; }
	int getBuffId() const { return mBuffId; }
	const std::string& getEffectFile() const { return mEffectAni; }

private:
	int mID;
	int mType;
	float mProb;
	int mBuffId;
	std::string mEffectAni;
};

class CreepsTable : public TableReader
{
public:

	CreepsTable() { }
	~CreepsTable() { purge(); }

	virtual bool init() { parse("Creeps.csv", 2); return true; }
	void purge();

	Creeps* getCreeps(int id);

private:
	virtual void readline(std::stringstream& _stream)
	{
		Creeps* pData = Creeps::create();
		if (pData)
		{
			pData->readline(_stream);
			mTable.insert(std::make_pair(pData->getID(), pData));
		}
	}

	std::unordered_map<int, Creeps*> mTable;
};

////////////////////////NewPlayGift.csv表///////////////////////////////////


class NewPlayGiftData
{
public:
	NewPlayGiftData();
	~NewPlayGiftData();

	static NewPlayGiftData* create();

	void readline(std::stringstream& _stream);

	int id;			//ID
	int type;
	int moneyType;
	int price;
	int giftId;		//礼包id

	std::map<int, int >itemMap; //解析后的道具
};

typedef std::map<unsigned int, NewPlayGiftData*> NewPlayGiftList;
class NewPlayGiftTable : public TableReader
{
public:

	NewPlayGiftTable() { }
	~NewPlayGiftTable() { purge(); }

	virtual bool init() { parse("NewPlayGift.csv", 2); return true; }
	void purge() { mNewPlayGiftList.clear(); };

	NewPlayGiftList* getNewPlayGiftList();

	NewPlayGiftData* getNewPlayGiftDataByType(int type);
private:

	virtual void readline(std::stringstream& _stream)
	{
		NewPlayGiftData* data = NewPlayGiftData::create();
		data->readline(_stream);
		mNewPlayGiftList.insert(std::make_pair(data->id, data));
	}
	NewPlayGiftList mNewPlayGiftList;
};

//////////////////pvp聊天//////////////////////////////
class PvpChat
{
public:
	PvpChat();
	void readline(std::stringstream& _stream);

	int mID;
	int mType;
	std::string mTitle;
	std::string mText;
	int mTime;
}; 
typedef std::map<unsigned int, PvpChat*> PvpChatList;
typedef std::map<unsigned int,std::vector<PvpChat*>> PvpChatMap;
class PvpChatTable: public TableReader
{
public:

	PvpChatTable() {  }
	~PvpChatTable() { purge(); }

	bool init()  { init("pvpChat.csv");  return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mPvpChatList.clear(); mPvpChatMap.clear();}; 

	PvpChatMap* getPvpChatMap();

	PvpChat* getPvpChat(int id);

	std::vector<PvpChat*>* getChatType(int type);

private:

	virtual void readline(std::stringstream& _stream)
	{
		PvpChat* data = new PvpChat();
		data->readline(_stream);
		mPvpChatList.insert(std::make_pair(data->mID,data));

		PvpChatMap::iterator it = mPvpChatMap.find(data->mType);
		if (it != mPvpChatMap.end())
		{
			it->second.push_back(data);
		}
		else
		{
			std::vector<PvpChat*> _vertor;
			_vertor.push_back(data);
			mPvpChatMap.insert(std::make_pair(data->mType,_vertor));
		}
	}
	PvpChatList mPvpChatList; 
	PvpChatMap mPvpChatMap;
};

class PlatformItem
{
public:
	unsigned int	id;
	std::string		name;

private:
	void readline(std::stringstream& _stream)
	{
		_stream >> id >> name;
	}

	friend class PlatformTable;
};

class PlatformTable
	: public TableReader
	, public Singleton<PlatformTable>
{
public:

	PlatformTable(){}
	~PlatformTable(){ purge(); }

	bool init(){ init("Platforms.csv"); return true; };
	void init(const std::string& filename){ parse(filename, 1); }
	void purge() { mPlatformItemList.clear(); }

	typedef std::map<unsigned int, PlatformItem* > PlatformItemList;
	typedef ConstMapIterator<PlatformItemList> PlatformItemListIterator;

	std::string getPlatformName(int id)
	{
		PlatformItemList::iterator it = mPlatformItemList.find(id);
		if (it != mPlatformItemList.end())
		{
			return it->second->name;
		}
		return "";
	}

private:
	virtual void readline(std::stringstream& _stream)
	{
		PlatformItem* data = new PlatformItem();
		data->readline(_stream);
		mPlatformItemList.insert(PlatformItemList::value_type(data->id, data));
	}

	PlatformItemList mPlatformItemList;
};


// 游戏内文本(TextHelper.csv)
class ErrorCode
{
public:
	bool init(){  return true; };
	static ErrorCode* create();

	void readline(std::stringstream& _stream);

	int id;
	std::string text;
};

class ErrorCodeTable : public TableReader
{
public:

	ErrorCodeTable() {}
	~ErrorCodeTable() { purge(); }
	bool init() { init("ErrorCode.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mErrorCodeList.clear(); };

	const ErrorCode* getErrorCodeByID(int);
	typedef std::map<unsigned int, ErrorCode*> ErrorCodeList;


private:

	virtual void readline(std::stringstream& _stream)
	{
		ErrorCode* data = ErrorCode::create();
		data->readline(_stream);
		mErrorCodeList.insert(std::make_pair(data->id, data));
	}

	ErrorCodeList mErrorCodeList;
};

class PayLimit
{
public:
	bool init(){ return true; };
	static PayLimit* create();

	void readline(std::stringstream& _stream);
	
	int id;
	int type;
	int l_day;
	int l_month;
};


class PayLimitTable : public TableReader
{
public:

	PayLimitTable() {  }
	~PayLimitTable() { purge(); }
	bool init()  { init("PayLimit.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mLimitMap.clear(); };

	const PayLimit* getPayLimit();
private:

	virtual void readline(std::stringstream& _stream)
	{
		PayLimit* data = PayLimit::create();
		data->readline(_stream);
		mLimitMap.insert(std::make_pair(data->type,data));
	}

	std::map<int ,PayLimit*> mLimitMap;
};

//新手宝典
class Bible
{
public:
	bool init(){ return true; };
	static Bible* create();

	void readline(std::stringstream& _stream);

	int id;
	std::string title;
	int mark;
	std::string icon;
	int loop;
	int level;
};

typedef std::map<int, Bible*> BibleList;
class BibleTable : public TableReader
{
public:

	BibleTable() {  }
	~BibleTable() { purge(); }
	bool init()  { init("Bible.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 1); }
	void purge() { mBibleMap.clear(); };

	Bible* getBible(int id);
	BibleList* getBibleList() { return &mBibleMap; }
private:

	virtual void readline(std::stringstream& _stream)
	{
		Bible* data = Bible::create();
		data->readline(_stream);
		mBibleMap.insert(std::make_pair(data->id, data));
	}
	
	BibleList mBibleMap;
};

class CrazyWeekInfo
{
public:
	bool init(){ return true; };
	static CrazyWeekInfo* create();

	void readline(std::stringstream& _stream);

	int id;
	int pvpWinCount;  // 连胜数
	int pvpCount;	// 参与数
	int rewardId;
	std::string condition;
	std::string subCondition;
};


class CrazyWeekTable : public TableReader
{
public:

	CrazyWeekTable() {  }
	~CrazyWeekTable() { purge(); }
	bool init()  { init("ActivityCrazyGift.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 2); }
	void purge() { mCrazyWeekMap.clear(); };

	const CrazyWeekInfo* getCrazyWeekInfoById(int id);
private:

	virtual void readline(std::stringstream& _stream)
	{
		CrazyWeekInfo* data = CrazyWeekInfo::create();
		data->readline(_stream);
		mCrazyWeekMap.insert(std::make_pair(data->id, data));
	}

	std::map<int, CrazyWeekInfo*> mCrazyWeekMap;
};


////////////////////////荣誉兑换表///////////////////////////////////
class ExchangeItem
{
public:
	static ExchangeItem* create();

	ExchangeItem();
	~ExchangeItem();
	void readline(std::stringstream& _stream);

	int mID;   // 走礼包方式
	//std::string mName;
	//int mType;		// 1 体力 2 精力 3 钻石 4 物品 
	//int mItemId;	// 1.体力，2 精力，3 钻石， 4，开房券，   物品ID > 10000 ??
	int mGiftId;
	int mDailyCount;
	int mNeedScore;
	int mFreshTime;
	//std::string mIconFile;
};

class ExchangeItemTable: public TableReader
{
public:

	ExchangeItemTable() { }
	~ExchangeItemTable() { purge(); }

	virtual bool init() { parse("ExchangeItem.csv", 2); return true; }
	void purge();

	ExchangeItem* getExchangeItem(int id);
	int			  getExchangeItemCount();

private:
	virtual void readline(std::stringstream& _stream)
	{
		ExchangeItem* pData = ExchangeItem::create();
		if (pData)
		{
			pData->readline(_stream);
			mTable.insert(std::make_pair(pData->mID, pData));
		}
	}

	std::unordered_map<int, ExchangeItem*> mTable;
};


////////////////////////段位表///////////////////////////////////
class GradeData
{
public:
	static GradeData* create();

	GradeData();
	~GradeData();
	void readline(std::stringstream& _stream);

	int getID() const { return mID; }
	const std::string& getName() const { return mName; }
	eRankingGroupType getGroup() const { return (eRankingGroupType)mGroup; }
	int getMinPoint() const { return mMinPoint; }
	int getMaxPoint() const { return mMaxPoint; }
	int getRandWinScore() const { return mRandWinScore; }
	int getRandLoseScore() const { return mRandLoseScore; }

private:
	int mID;
	std::string mName;
	int mGroup;
	int mMinPoint;
	int mMaxPoint;
	int mRandWinScore;
	int mRandLoseScore;
};


class GradeDataTable : public TableReader
{
public:

	GradeDataTable() { }
	~GradeDataTable() { purge(); }

	virtual bool init() { parse("GradeData.csv", 2); return true; }
	void purge();

	GradeData* getGradeData(int point);
	GradeData* getGradeByGrade(int id);

private:
	virtual void readline(std::stringstream& _stream)
	{
		GradeData* pData = GradeData::create();
		if (pData)
		{
			pData->readline(_stream);
			mTable.push_back(pData);
		}
	}

	std::vector<GradeData*> mTable;
};

struct sVipType
{
	int mPrice;												//价格
	int mPayType;											//支付方式
	int mTime;												//持续时间
	int mShopID;											//计费ID
	sVipType() :mPrice(0), mPayType(0), mTime(0){}
};


class VipData
{
public:

	VipData(){};
	~VipData(){};
	void readline(std::stringstream& _stream);

	static VipData* create();
	typedef std::map<int, sVipType>  VipTypeMap;

	std::vector<std::string> getDescribeID(){ return mDescribeID; }
	sVipType getVipType(int id);// 1挡，2挡		 eg: price: 30钻石#70人民币  1是30，2是70

	sVipType m_sVipType;
private:
	CC_SYNTHESIZE(int ,mID,ID);									//会员ID
	CC_SYNTHESIZE(std::string, mName, Name);					//会员名字
	CC_SYNTHESIZE(std::string, mHIcon, HIcon);					//头像Icon
	CC_SYNTHESIZE(int, mPowerSlot,PowerSlot);					//体力上限
	CC_SYNTHESIZE(int, mEnergySlot,EnergySlot);					//精力上限
	CC_SYNTHESIZE(int, mAddHonor, AddHonor);					//荣誉加成
	CC_SYNTHESIZE(int, mShopDiscount,ShopDiscount);				//商店折扣
	std::vector<std::string> mDescribeID;							//描述
	VipTypeMap mVipTypeMap;				
};

class VipDataTable : public TableReader
{
public:

	VipDataTable() { }
	~VipDataTable() { purge(); }

	virtual bool init() { parse("vip.csv", 2); return true; }
	void purge();
	VipData* getVipData(int id);
private:
	virtual void readline(std::stringstream& _stream)
	{
		VipData* pData = VipData::create();
		if (pData)
		{
			pData->readline(_stream);
			mTable.push_back(pData);
		}
	}
	std::vector<VipData*> mTable;
};


////////////////////////ActInform表///////////////////////////////////
class ActInformData
{
public:
	ActInformData();
	~ActInformData();
	static ActInformData* create();
	void readline(std::stringstream& _stream);
	friend class ActInformTable;
public:
	int mID;
	int mType;
	int mActType;
	int mShowType1;
	int mShowType2;
	int mTControlType;
	CC_SYNTHESIZE(std::string, mActName, ActName);
	CC_SYNTHESIZE(std::string, mOpenTime, OpenTime);
	CC_SYNTHESIZE(std::string, mCloseTime, CloseTime);
	CC_SYNTHESIZE(std::string, mContent, Content);
	CC_SYNTHESIZE(std::string, mPageId, PageId);
	

};

typedef std::vector<ActInformData* > ActInformList;
class ActInformTable : public TableReader
{
public:

	ActInformTable() { /*init(); */ }
	~ActInformTable() { purge(); }
	bool init()  { init("ActInform.csv");  return true; }
	void init(const std::string& filename) { parse(filename, 2); }
	void purge(){ mActInformList.clear(); }

	ActInformList getActInformList() { return mActInformList; }
	ActInformData* getActInformData(int id);
private:

	virtual void readline(std::stringstream& _stream)
	{
		ActInformData* data = ActInformData::create();
		data->readline(_stream);
		mActInformList.push_back(data);
	}
	ActInformList mActInformList;
};

class LevelData
{
public:
	LevelData(){};
	~LevelData(){};
	static LevelData* create();
	void readline(std::stringstream& _stream);
	friend class ActInformTable;
public:
	
	CC_SYNTHESIZE(int, mLevel, Level);
	CC_SYNTHESIZE(int, mLevelEX, LevelEX);
};

class LevelTable : public TableReader
{
public:

	LevelTable() { /*init(); */ }
	~LevelTable() { purge(); }
	bool init()  { init("levelConfig.csv");  return true; }
	void init(const std::string& filename) { parse(filename, 2); }
	void purge(){};

	typedef std::map<int, int> LevelDataMap;
	int getLevelData(int level);
private:

	virtual void readline(std::stringstream& _stream)
	{
		LevelData* data = LevelData::create();
		data->readline(_stream);
		mLevelDataMap.insert(std::make_pair(data->getLevel(), data->getLevelEX()));
	}
	LevelDataMap mLevelDataMap;
};






//////////////////////////////
class GlyphStuffData
{
public:
	GlyphStuffData(){};
	~GlyphStuffData(){};
	static GlyphStuffData* create();
	void readline(std::stringstream& _stream); 
public:

	int mStuffID;
	std::string mStuffName;
	std::string mStuffIcon;
	std::string mStuffClass;
	std::string mStuffDesc;


	std::string getIcon(){ return mStuffIcon; };
};

typedef std::map<int, GlyphStuffData*> GlyphStuffList;
class GlyphStuffTable : public TableReader
{
public:

	GlyphStuffTable() { /*init(); */ }
	~GlyphStuffTable() { purge(); }
	bool init()  { init("Glyphstuff.csv");  return true; }
	void init(const std::string& filename) { parse(filename, 2); }
	void purge(){}; 
	GlyphStuffData* getGlyphStuffData(int id);
	const GlyphStuffList& getGlyphStuffList() { return mGlyphStuffList; }
private: 
	virtual void readline(std::stringstream& _stream)
	{
		GlyphStuffData* data = GlyphStuffData::create();
		data->readline(_stream);
		mGlyphStuffList.insert(std::make_pair(data->mStuffID, data));
	}
	GlyphStuffList mGlyphStuffList;
};
 
class PlayerInfo
{
public:
	PlayerInfo(){};
	~PlayerInfo(){};
	static PlayerInfo* create();
	void readline(std::stringstream& _stream);
public:
	CC_SYNTHESIZE(int, mInviteLevel, InviteLevel);
};

class PlayerInfoTable:public TableReader
{
public:

	PlayerInfoTable() { /*init(); */ }
	~PlayerInfoTable() { purge(); }
	bool init()  { init("playerInfo.csv");  return true; }
	void init(const std::string& filename) { parse(filename, 2); }
	void purge(){};

	PlayerInfo *getPlayerInfo(){ return mPlayerInfo; }
private:
	PlayerInfo *mPlayerInfo;
	virtual void readline(std::stringstream& _stream)
	{
		mPlayerInfo = PlayerInfo::create();
		mPlayerInfo->readline(_stream);
	}
};

//智能指引
class Intelligent
{
public:
	bool init(){ return true; };
	static Intelligent* create();

	void readline(std::stringstream& _stream);

	int id;
	int priority;//优先级
	int isable;//是否可用
};

typedef std::map<int, Intelligent*> IntelligentList;
class IntelligentTable : public TableReader
{
public:

	IntelligentTable() {  }
	~IntelligentTable() { purge(); }
	bool init()  { init("IntelligentGuide.csv"); return true; };
	void init(const std::string& filename) { parse(filename, 1); }
	void purge() { mIntelligentMap.clear(); };

	const Intelligent* getIntelligent(int id);
private:

	virtual void readline(std::stringstream& _stream)
	{
		Intelligent* data = Intelligent::create();
		data->readline(_stream);
		mIntelligentMap.insert(std::make_pair(data->id, data));
	}

	IntelligentList mIntelligentMap;
};


class BadgeData
{
public:
	BadgeData();
	~BadgeData();
	static BadgeData* create();
	void readline(std::stringstream& _stream);

public:
	CC_SYNTHESIZE(int, mId, Id);
	CC_SYNTHESIZE(int, mActive, Active);
	CC_SYNTHESIZE(std::string, mName, Name);
	CC_SYNTHESIZE(std::string, mAlias, Alias);
	CC_SYNTHESIZE(std::string, mIntroduce, Introduce);
	CC_SYNTHESIZE(std::string, mJumpPoint, JumpPoint);
	CC_SYNTHESIZE(std::string, mIcon, Icon);
	CC_SYNTHESIZE(std::string, mPath, Path);
};


class BadgeDataTable :public TableReader
{
public:

	BadgeDataTable() { /*init(); */ }
	~BadgeDataTable() { purge(); }
	bool init()  { init("Badge.csv");  return true; }
	void init(const std::string& filename) { parse(filename, 2); }
	void purge();

	BadgeData* getBadge(int id) const;
	const std::vector<BadgeData*>& getCache() const { return mCache; }

private:
	virtual void readline(std::stringstream& _stream)
	{
		BadgeData* pData = BadgeData::create();
		if (pData)
		{
			pData->readline(_stream);
			mCache.push_back(pData);
		}
	}

	std::vector<BadgeData*> mCache;
};

class GoodsInfo
{
public:
	GoodsInfo(){};
	~GoodsInfo(){};
	static GoodsInfo* create();
	void readline(std::stringstream& _stream);

	int mType;
	std::string mName;
	std::string mIcon;
	//原基础上加，只能这样
	std::string mIconBig;
	std::string mIconSmall;
	std::string getIcon() { return mIcon; };
};

class GoodsInfoTable :public TableReader
{
public:

	GoodsInfoTable() { /*init(); */ }
	~GoodsInfoTable() { purge(); }
	bool init()  { init("GoodsInfo.csv");  return true; }
	void init(const std::string& filename) { parse(filename, 2); }
	void purge(){};

	GoodsInfo *getGoodsInfo(int type);
private:
	std::map<int, GoodsInfo*> mGoodsInfoMap;
	virtual void readline(std::stringstream& _stream)
	{
		GoodsInfo* lGoodsInfo = GoodsInfo::create();
		lGoodsInfo->readline(_stream);
		mGoodsInfoMap.insert(std::make_pair(lGoodsInfo->mType, lGoodsInfo));
	}

};

class NameArea
{
public:
	NameArea(){};
	~NameArea(){};
	static NameArea* create();
	void readline(std::stringstream& _stream);
	int mID;
	int mType;
	std::string mName;
};

class NameAreaTable :public TableReader
{
public:

	NameAreaTable() { /*init(); */ }
	~NameAreaTable() { purge(); }
	bool init()  { init("NameArea.csv");  return true; }
	void init(const std::string& filename) { parse(filename, 2); }
	void purge(){};

	NameArea* getNameArea(int id);
private:
	std::map<int, NameArea*> mNameAreaMap;
	virtual void readline(std::stringstream& _stream)
	{
		NameArea* pNameArea = NameArea::create();
		pNameArea->readline(_stream);
		mNameAreaMap.insert(std::make_pair(pNameArea->mID, pNameArea));
	}

};

class ScoreInfo
{
public:
	ScoreInfo(){};
	~ScoreInfo(){};
	static ScoreInfo* create();
	void readline(std::stringstream& _stream);
	int id;
	int useScore;
	int fightScore;
	int percentScore;
};

class ScoreInfoTable :public TableReader
{
public:

	ScoreInfoTable() { /*init(); */ }
	~ScoreInfoTable() { purge(); }
	bool init()  { init("ScoreInfo.csv");  return true; }
	void init(const std::string& filename) { parse(filename, 2); }
	void purge(){};

	ScoreInfo* getScoreInfo(int id);
private:
	std::map<int, ScoreInfo*> mScoreInfoMap;
	virtual void readline(std::stringstream& _stream)
	{
		ScoreInfo* pInfo = ScoreInfo::create();
		pInfo->readline(_stream);
		mScoreInfoMap.insert(std::make_pair(pInfo->id, pInfo));
	}

};

class SevenDayMission
{
public:
	static SevenDayMission* create();

	SevenDayMission();
	~SevenDayMission();
	void readline(std::stringstream& _stream);

	CC_SYNTHESIZE(int ,mID, ID);
	CC_SYNTHESIZE(int, mType, Type);
	CC_SYNTHESIZE(string, mCondition, Condition);
	CC_SYNTHESIZE(int, mPara, Para);
	CC_SYNTHESIZE(int, mReward, Reward);
};

class SevenDayMissionTable :public TableReader
{
public:

	SevenDayMissionTable() { }
	~SevenDayMissionTable() { purge(); }
	bool init()  { init("sevenDayMission.csv");  return true; }
	void init(const std::string& filename) { parse(filename, 2); }
	void purge(){};

	SevenDayMission * getDataByDay(int day);
private:
	std::map<int, SevenDayMission*> mScoreInfoMap;
	virtual void readline(std::stringstream& _stream);

};
