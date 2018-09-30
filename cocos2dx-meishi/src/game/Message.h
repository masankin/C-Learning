/*
* jacey
* 消息
*/
#pragma once

#include "cocos2d.h"
USING_NS_CC;

#include "Constant.h"

enum eMessageType
{
	EMT_INVALID = -1,
	EMT_ATTACK,			// 建筑A   攻击  建筑B

	EMT_MOVE,			//移动
	EMT_TRANSFORM,		//同阵营嵌入
	EMT_FIGHT,			//士兵战斗（一排）
	EMT_PRODUCT,		//生产
	EMT_UPGRADE,		//升级
	EMT_TOWER_ATTACK,	//炮塔攻击
	EMT_BUILD_DESTORY,	// 建筑被占领
	EMT_SUCC_CHECKSTAR,	// 玩家胜利时评星结算 
	EMT_REFRESH_STAR,	// 评星结算完成，刷新UI
	EMT_PAGE_GO,			//界面
	EMT_STAR_STATU,			//评星完成
	EMT_CHECKPOINT_START,	// 关卡开始
	EMT_CHECKPOINT_END,		// 关卡结束
	EMP_POWER_CHANGE,		// 体力值改变
	EMT_ENERGY_CHANGE,   //精力值改变
	EMT_GUIDE_STEP_OVER,	// 指引结束
	EMT_OPEN_UI,			// 打开UI 
	//道具相关
	EMT_PROP_CHANGE,	//建筑类型转换
	EMT_PROP_CHANGEOK,	//建筑类型转换成功
	EMT_PROP_ADDTIME,	//破表转换
	EMT_PROP_FULLPEOP,	//兵力增加
	EMT_PROP_FREEZE,	//冰冻
	EMT_PROP_COOKING,	//召唤厨师
	EMT_PROP_INVISIBILITY,	//隐形斗篷
	EMT_PROP_Oil,	//地沟油
	EMT_PROP_USER,
	EMT_CHANNEL_LOGIN = 1000,		// 登录完成
	EMT_CHANNEL_LOGOUT,				// 登出完成
	EMT_GUIDE,			//新手引导
	EMT_GUIDE_TIPS,		//触发点击提示
	EMT_GUIDE_PAUSE,	//游戏暂停  
	EMT_JUNGLE,			//打野消息相关
	EMT_MORALE,			//士气消息相关
	EMT_BUFF,			//buff消息相关
	EMT_REDUCEPEOPLE,	//
	EMT_MONEY_CHANGE,//金币改变
	EMT_RESMODE_SCORE, // 积分模式，玩家积分变化
	EMT_CLOSEPAGE, //关闭界面
	EMT_STOP_MATCH, // 停止匹配
	EMT_CLOSE_PVP, // 关闭pvp
	EMT_OPEN_STAGE, //PVE开启新章节
	EMT_ENABLE_OPTIONBUTTON, //开关设置按钮

	EMT_CHANGE_NAME,//改名字
	EMT_CHANGE_PLAYER_ICON,//改头像 add in 2015-10-08

	EMT_SIGNAL_CHANGE,//信號更換
	EMT_BUFFSHOW, //Buff显示相关
	EMT_MORALESHOW, //士气显示相关

	EMT_MENU_BTN, //gamepage 投降按钮更换

	EMT_ROW_DEPART, //士兵离开（一排）
	EMT_FORMATION_OVER, // 阵型结束

	EMT_ACHIEVE_GOTOLEVEL,  //成就前往关卡
	EMT_HONGDIAN_TIP = 1025,//有未领取的提示领取

	EMT_SHOP_PAYCOMPLENT,  //支付完成后回调
	EMT_SHOP_SHOW_BUYITEM,//显示购买钻石项
	EMT_SHOP_AFTER_BUYDIAMOND, //购买钻石后购买相应的物品
	//战斗提示信息
	EMT_BATTLE_TIP,					//战斗信息
	EMT_MISSION_STATE,              //每日任务

	EMT_BTN_CONTROL,			//活动按钮开启/结束
	EMT_ACTIVATION_TIP,		//激活码领取失败原因提示
	EMT_CHAT_MSG,		//聊天通知
	EMT_FRIEND_UPDATE,			//好友系统列表更新
	EMT_BUFFER_TIP,		//战斗buff提示
	EMT_GAMESTART,		//游戏正式开始

	EMT_ITEM_OPERATION, // 物品操作
	EMT_PROP_USE_ALL, // 道具用完
	EMT_PVP_INFO,		// pvp信息

	EMT_LITTLETIPS,//小提示
	EMT_GETSAVEDATAOVER, //拉取存档完毕
	EMT_RQST_RESULT, //请求结果 
	EMT_SDKLOGIN_SUCCESS,//SDK登陆成功
	EMT_GAMESERVERLOGIN_SUCCESS,//游戏服登陆成功
	EMT_SIMPLIFY_UI,    //pvp 观战模式下 ui简化
	EMT_UPDATE_PLAYER_BADGES,   //勋章获得列表更新
	EMT_CHANGE_PLAYER_BADGE,   //当前勋章更新
	EMT_CONTROL_OUT_PERCENT,	//战斗记录操作,出兵百分比按钮状态更新
	EMT_CONTROL_PROP_USE,	//战斗记录操作,使用道具，点击目标
	EMT_CONTROL_CLICK_PROP,	//战斗记录操作,点击道具
	EMT_CONTROL_CLICK_PROP_SECEND,//战斗记录操作,点击道具二次选择，建筑转换等
	EMT_CONTROL_CANCEL_PROP,	//战斗记录操作,取消使用道具

	EMT_BUTTON_OUT_PERCENT,	//出兵百分比按钮状态更新

	EMT_BUTTON_LOGINSCENE,//登录界面开始游戏按钮
	EMT_BUTTON_LOGINSCENE_UNVISIBLE,

	EMT_CRAZY_WEEK,// 狂欢周活动
	EMT_OPEN_LEVEL_DIALOG,	//打开关卡信息界面
	EMT_UPDATE_EXCHANGE,	//荣誉兑换
	EMT_UPDATE_SHOPLIST,//充值礼包
	EMT_CLOSESHOP,//关闭商城
	EMT_GW,	//食神争霸赛(GlobalWars)
	EMT_GOTO_ACTLAB,	//跳转活动标签
	EMT_BUY_VIP,		//vip
	EMT_PLAYER_UPGRADE,//玩家升级

	EMT_GAME_LOGIN_FLOW, // 登录流程
	EMT_RED_PACKET_CAN_REWARD = 1065,//红包可领
	EMT_SELECTWORD_SCHEDULE = 1066,//主界面定时器开启
	EMT_CHANGE_SERVER,  //更新登陆服务器

	EMT_CHANGE_SIGN,//更改签名
	EMT_CHANGE_EXTRA_VISIBLE,//更改额外信息的可见性
	EMT_GET_EXTRA,//获取额外信息
	EMT_POINT_CHANGE,   //玩家pvp积分改变

	EMT_RECORD_CREEPS_BORN,	//记录野怪出生
	EMT_RECORD_OUT_PERCENT,	//记录出兵比例
	EMT_RECORD_CLICK_PROP,	//记录点击道具
	EMT_RECORD_CLICK_PROP_SECEND,	//记录二次点击道具
	EMT_RECORD_CANCEL_PROP,	//记录取消选择道具

	EMT_PVPRESULT_OPENBOX,       //pvp结算宝箱打开

	EMT_OPEN_DAILY_MISSION = 1078,//打开每日任务界面
	EMT_DALIY_MISSION_TIPS = 1079,//每日任务提示
	EMT_DAILY_MISSION_PROGRESS_BACK = 1080, //每日任务上报进度数据的回包
	EMT_SHOW_DAILY_MISSION_PROGRESS = 1081, //显示每日任务进度界面
	EMT_SHARE_SUCCESS, //分享成功
	EMT_SKIP_SUCCESS = 1083,//跳关成功
	EMT_INVITE_BAT_SWITCH = 1084, //邀战提示提示界面切换

	EMT_RECORD_TURRET_UPDATE, //记录炮台转aoe或ice
	EMT_RECORD_EXECUTE_TURRET_UPDATE, //回放，执行设置炮台转换的操作

	EMT_HONOR_CHANGE, // 荣誉值改变
	
	EMT_GUIDE_FIRST = 1088, //第一次进入游戏相关步骤
	
	EMT_RANK_CHANGE,   //玩家排名改变
};


class GameMessage
{
public:
	virtual int getMessageType() const { return -1; }

	virtual void	formatMessage(std::stringstream& ) = 0;
};

class MessageHandler
{
public:
	virtual void onReceiveMassage(GameMessage * message) = 0;
};

class GameMessageFactory
{
public:
	virtual GameMessage* create(std::string params) = 0;
};

#define DEFINE_FACTORY(message)	\
class message##Factory : public GameMessageFactory	\
{	\
public:	\
	virtual GameMessage* create(std::string params)	\
{\
	message* pret = new message(); \
if (pret)\
{\
std::stringstream ssParams(params);\
pret->formatMessage(ssParams); \
return pret; \
}\
	return NULL; \
}\
};


class MessageMove : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		mIsNet = false;
		mTime = 0;
		sstream >> mStarId >> mEndId >> mFormationId >> mNumType >> mNumValue >> mTime;
		mIsNet = (mTime != 0); //flag = 1 为true

	}

	virtual int getMessageType() const { return EMT_MOVE; }

protected:

	CC_SYNTHESIZE(int, mEndId, ToBuild);	// build key
	CC_SYNTHESIZE(int, mStarId, FromBuild);
	CC_SYNTHESIZE(int, mFormationId, FormationId);
	CC_SYNTHESIZE(float, mNumValue, NumValue);
	CC_SYNTHESIZE(int, mNumType, NumType);	// 发兵类型 0-percent 1-num
	CC_SYNTHESIZE(bool, mIsNet, IsNet);	// 是否为网络发兵 
	CC_SYNTHESIZE(float, mTime, Time);	// 网络延迟时间
};

DEFINE_FACTORY(MessageMove);


class MessageAttack : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mFromBuild
			>> mToBuild;
	}

	virtual int getMessageType() const { return EMT_ATTACK; }

protected:

	CC_SYNTHESIZE(int, mFromBuild, FromBuild);	// build key
	CC_SYNTHESIZE(int, mToBuild, ToBuild);
};

DEFINE_FACTORY(MessageAttack);
class MessageFight : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		int attact;
		int defense;
		sstream >> attact >>defense >> mAttactNum >> mBuild >> mBuild2 >> mIsLastRow >> mFormationID;
		mAttactCamp = (eCampType)attact;
		mDefenseCamp = (eCampType)defense;

	}

	virtual int getMessageType() const { return EMT_FIGHT; }

protected:

	CC_SYNTHESIZE(eCampType, mAttactCamp, AttactCamp);
	CC_SYNTHESIZE(eCampType, mDefenseCamp, DefenseCamp);
	CC_SYNTHESIZE(int, mAttactNum, AttactNum);	
	CC_SYNTHESIZE(int, mBuild, Build);
	CC_SYNTHESIZE(int, mBuild2, Build2);
	CC_SYNTHESIZE(int, mIsLastRow, IsLastRow);
	CC_SYNTHESIZE(int, mFormationID, FormationID);
};

DEFINE_FACTORY(MessageFight);


class MessageProduct : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mBuildKey;
	}

	int getMessageType() const { return EMT_PRODUCT; }

	CC_SYNTHESIZE(int, mBuildKey, BuildKey);
};
DEFINE_FACTORY(MessageProduct);


class MessageTransform : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mInput >> mBuild >> mBuild2 >> mIsLastRow >> mFormationID; 
	}

	virtual int getMessageType() const { return EMT_TRANSFORM; }

protected:

	CC_SYNTHESIZE(int, mInput, TransformNum);
	CC_SYNTHESIZE(int, mBuild, Build);
	CC_SYNTHESIZE(int, mBuild2, Build2);
	CC_SYNTHESIZE(int, mIsLastRow, IsLastRow);
	CC_SYNTHESIZE(int, mFormationID, FormationID);
};

DEFINE_FACTORY(MessageTransform);

class MessageUpgrade : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		isNet = false;
		int flag = 0;
		sstream >> mBuild >> flag;
		isNet = (flag == 1);
	}

	virtual int getMessageType() const { return EMT_UPGRADE; }

protected:
	CC_SYNTHESIZE(int, mBuild, Build);
	CC_SYNTHESIZE(bool, isNet, IsNet);

};

DEFINE_FACTORY(MessageUpgrade);

class MessageBuildChange : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		mCamp = 10;
		int iNew = 0;
		sstream >> mBuild >> mCamp >> iNew >> mPropId;

		isNew = iNew == 1;
	}

	virtual int getMessageType() const { return EMT_PROP_CHANGE; }

protected:

	CC_SYNTHESIZE(int, mBuild, Build);
	CC_SYNTHESIZE(int, mCamp, Camp);
	CC_SYNTHESIZE(bool,isNew , New);//是否是 三合一 转换卷.
	CC_SYNTHESIZE(int, mPropId, PropId);
};

DEFINE_FACTORY(MessageBuildChange);

class MessageBuildChangeOk : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		int type;
		int iNew = 0;
		sstream >> mBuild >> type >> iNew;
		mChangeType = (eBulidType)type;
		isNew = iNew == 1;
	}

	virtual int getMessageType() const { return EMT_PROP_CHANGEOK; }

protected:

	CC_SYNTHESIZE(int, mBuild, Build);
	CC_SYNTHESIZE(eBulidType, mChangeType, ChangeType);
	CC_SYNTHESIZE(bool, isNew, isNew);//是否是 三合一 转换卷.
};

DEFINE_FACTORY(MessageBuildChangeOk);


class MessageFullPeopel : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		mCamp = 10;
		sstream >> mBuild >> mCamp;
	}

	virtual int getMessageType() const { return EMT_PROP_FULLPEOP; }

protected:

	CC_SYNTHESIZE(int, mBuild, BuildId);
	CC_SYNTHESIZE(int, mCamp, Camp);
};

DEFINE_FACTORY(MessageFullPeopel);

class MessageAddTime : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> time;
	}

	virtual int getMessageType() const { return EMT_PROP_ADDTIME; }

public:
	
	float time;
};

DEFINE_FACTORY(MessageAddTime);

class MessageFreeze : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> camp;
	}

	virtual int getMessageType() const { return EMT_PROP_FREEZE; }

public:

	float time;
	int camp;
};

DEFINE_FACTORY(MessageFreeze);

class MessageCooking : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> x >> y >> camp ;

	}

	virtual int getMessageType() const { return EMT_PROP_COOKING; }

public:

	float x;
	float y;
	int camp;
};

DEFINE_FACTORY(MessageCooking);

class MessageOil : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> x >> y >> camp;

	}

	virtual int getMessageType() const { return EMT_PROP_Oil; }

public:

	float x;
	float y;
	int camp;
};

DEFINE_FACTORY(MessageOil);

class MessageInvisibility : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> buildId >> camp;
	}

	virtual int getMessageType() const { return EMT_PROP_INVISIBILITY; }

public:

	int buildId;
	int camp;
};

DEFINE_FACTORY(MessageInvisibility);

class MessageBuildAttact : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mBuild;
	}

	virtual int getMessageType() const { return EMT_TOWER_ATTACK; }

protected:

	CC_SYNTHESIZE(int, mBuild, Build);
};

DEFINE_FACTORY(MessageBuildAttact);

class MessageBuildDestroy : public GameMessage
{
public:

	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mKeyID >> mInitCamp >> mToCamp;
	}

	virtual int getMessageType() const { return EMT_BUILD_DESTORY; }

	CC_SYNTHESIZE(int, mKeyID, KeyID);
	CC_SYNTHESIZE(int, mInitCamp, InitCamp);
	CC_SYNTHESIZE(int, mToCamp, ToCamp);
};

DEFINE_FACTORY(MessageBuildDestroy);

// 评星结算
class MessageCheckStar : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mPlayerSuccess;
	}

	virtual int getMessageType() const { return EMT_SUCC_CHECKSTAR; }

	int mPlayerSuccess;
};
DEFINE_FACTORY(MessageCheckStar);

// 刷新评星UI
class MessageRefreshStar : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mFlashAll;
	}

	virtual int getMessageType() const { return EMT_REFRESH_STAR; }
	CC_SYNTHESIZE(int, mFlashAll, FlashAll);
};
DEFINE_FACTORY(MessageRefreshStar);

class MessagePage : public GameMessage
{
public:

	MessagePage() : mLuaType(0), mName(""), mIsCacheRes(false){}
	virtual void formatMessage(std::stringstream& sstream)
	{
		int page;
		sstream >> page >> mName >> mLuaType >> mIsCacheRes;
		mPage = (ePageType)page;
	}

	virtual int getMessageType() const { return EMT_PAGE_GO; }

protected:

	CC_SYNTHESIZE(ePageType, mPage, PageType);
	CC_SYNTHESIZE(int, mLuaType, LuaType);
	CC_SYNTHESIZE(std::string, mName, PageName);
	CC_SYNTHESIZE(bool, mIsCacheRes, IsCacheRes);
};

DEFINE_FACTORY(MessagePage);

// 关卡开始
class MessageCheckPointStart : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mCpIndex;
	}

	virtual int getMessageType() const { return EMT_CHECKPOINT_START; }

	int mCpIndex;
};

DEFINE_FACTORY(MessageCheckPointStart);


// 关卡结束
class MessageCheckPointEnd : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mCpIndex >> mSuccess >> mReason;
	}

	virtual int getMessageType() const { return EMT_CHECKPOINT_END; }

	int mCpIndex;
	int mSuccess;
	std::string mReason;
};

DEFINE_FACTORY(MessageCheckPointEnd);


// 关卡结束
class MessageStarStatu : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mStatu >> mTag;
	}

	virtual int getMessageType() const { return EMT_STAR_STATU; }

	int mStatu;
	int mTag;
};

DEFINE_FACTORY(MessageStarStatu);

// Channel登录
class MessageChannelLogin : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mResult >> mToken >> channelId >> XGToken;
	}

	virtual int getMessageType() const { return EMT_CHANNEL_LOGIN; }

	int mResult;
	std::string mUid;
	std::string mToken;
	int  channelId;
	std::string XGToken;
	//std::string version;
};

DEFINE_FACTORY(MessageChannelLogin);

// Channel登出
class MessageChannelLogout : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mResult >> mParam1;
	}

	virtual int getMessageType() const { return EMT_CHANNEL_LOGOUT; }

	int mResult;
	std::string mParam1;
};

DEFINE_FACTORY(MessageChannelLogout);


class MessageGuideStepOver : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mStep;
	}

	virtual int getMessageType() const { return EMT_GUIDE_STEP_OVER; }

	int mStep;

};

DEFINE_FACTORY(MessageGuideStepOver);


// 体力值改变
class MessagePowerChanged : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{		
	}

	virtual int getMessageType() const { return EMP_POWER_CHANGE; }
};

DEFINE_FACTORY(MessagePowerChanged);


class MessageOpenUI : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mPageType;
	}

	virtual int getMessageType() const { return EMT_OPEN_UI; }

	int mPageType;

};

DEFINE_FACTORY(MessageOpenUI);


//新手引导消息
class MessageGuideData: public GameMessage
{
public: 
	MessageGuideData(): m_GateID(0), m_GuideLocation(0), m_State(0){};
	~MessageGuideData(){};
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> m_GateID
			>> m_GuideLocation
			>> m_State;
	}

	virtual int getMessageType() const { return EMT_GUIDE; }
private:  
	CC_SYNTHESIZE(int, m_GateID, GateID);
	CC_SYNTHESIZE(int, m_GuideLocation, GuideLocation);
	CC_SYNTHESIZE(int, m_State, State); 
};
DEFINE_FACTORY(MessageGuideData);

class MessageGuideTips: public GameMessage
{
public:
	MessageGuideTips() : m_GuideID(0), m_FunctionID(0), m_FunctionParam(0){};
	~MessageGuideTips(){};
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> m_GuideID
			>> m_FunctionID
			>> m_FunctionParam;
	}

	virtual int getMessageType() const { return EMT_GUIDE_TIPS; }
private:
	CC_SYNTHESIZE(int, m_GuideID, GuideID); 
	CC_SYNTHESIZE(int, m_FunctionID, FunctionID);
	CC_SYNTHESIZE(int, m_FunctionParam, FunctionParam);
};
DEFINE_FACTORY(MessageGuideTips);
 


class MessagePropUserData : public GameMessage
{
public:
	MessagePropUserData() {};
	~MessagePropUserData(){};
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mPropID;
	}

	virtual int getMessageType() const { return EMT_PROP_USER; }
private:
	CC_SYNTHESIZE(int, mPropID, PropID);
};
DEFINE_FACTORY(MessagePropUserData);

class MessageGuidePause: public GameMessage
{
public:
	MessageGuidePause(){};
	~MessageGuidePause(){};
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream 
			>> m_GuideID
			>> m_StopGame;
	}

	virtual int getMessageType() const { return EMT_GUIDE_PAUSE; }

private:
	CC_SYNTHESIZE(int, m_GuideID, GuideID);
	CC_SYNTHESIZE(bool, m_StopGame, StopGame);

}; 
DEFINE_FACTORY(MessageGuidePause);


enum eJungleOccupy
{
	E_JUNGLE_CREATE = 0,	//创建打野建筑
	E_JUNGLE_ATTACK,		//进攻打野建筑
	E_JUNGLE_SUCCESS,		//占领成功
	E_JUNGLE_FAILED,		//占领失败
	E_JUNGLE_RETURN,		//占领者回兵
	E_JUNGLE_MOVE,			//上层通知下层士兵移动
	E_JUNGLE_CLOSE,			//打野建筑失效
	E_JUNGLE_REFRESH,		//打野建筑刷新
	E_JUNGLE_REMOVEATTACK,  //移除攻击者
	E_JUNGLE_CLEAR,	//移除引用
};
class MessageJungle: public GameMessage
{
public:
	MessageJungle():m_SrcID(0), m_DestID(0), m_SrcCamp(0), m_DestCamp(0), m_State(0), m_FormationID(0), m_JungleBuildID(0), m_JungleLocationID(0){};
	~MessageJungle(){};
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream 
			>> m_SrcID
			>> m_DestID
			>> m_SrcCamp
			>> m_DestCamp
			>> m_FormationID
			>> m_JungleBuildID
			>> m_JungleLocationID
			>> m_State;
	}


	virtual int getMessageType() const { return EMT_JUNGLE; }

private:
	CC_SYNTHESIZE(int, m_SrcID, SrcID);
	CC_SYNTHESIZE(int, m_DestID, DestID);
	CC_SYNTHESIZE(int, m_SrcCamp, SrcCamp);
	CC_SYNTHESIZE(int, m_DestCamp, DestCamp);
	CC_SYNTHESIZE(int, m_FormationID, FormationID);
	CC_SYNTHESIZE(int, m_JungleBuildID, JungleBuildID);
	CC_SYNTHESIZE(int, m_JungleLocationID, JungleLocationID);
	CC_SYNTHESIZE(int, m_State, State);
	

}; 
DEFINE_FACTORY(MessageJungle);

///////////////士气相关消息
enum eMoraleComputeType
{
	E_MORALECOMPUTE_NORMAL = 0, //普通计算方式 
	E_MORALECOMPUTE_GLYPH, //符文计算方式 
	E_MORALECOMPUTE_DEFENCE //防御表计算方式,
}; 
class MessageMorale: public GameMessage
{
public:
	MessageMorale(): m_CampType(0), m_MoraleValue(0), m_ComputeType(0){};
	~MessageMorale(){};
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream
			>> m_CampType
			>> m_MoraleValue
			>> m_ComputeType; 
	} 
	virtual int getMessageType() const { return EMT_MORALE; }
private:
	CC_SYNTHESIZE(int, m_CampType, CampType);
	CC_SYNTHESIZE(float, m_MoraleValue, MoraleValue); 
	CC_SYNTHESIZE(int, m_ComputeType, ComputeType);  
};
DEFINE_FACTORY(MessageMorale);

////////////士气显示相关
class MessageMoraleShow: public GameMessage
{
public:
	MessageMoraleShow(): m_CampType(0), m_MoraleValue(0), m_SrcID(0){};
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream
			>> m_CampType
			>> m_MoraleValue
			>> m_SrcID;
	} 
	virtual int getMessageType() const { return EMT_MORALESHOW; }
private:
	CC_SYNTHESIZE(int, m_CampType, CampType);
	CC_SYNTHESIZE(float, m_MoraleValue, MoraleValue);
	CC_SYNTHESIZE(int, m_SrcID, SrcID);
}; 
DEFINE_FACTORY(MessageMoraleShow);


////////////buff相关消息
class MessageBuff: public GameMessage
{
public:
	MessageBuff(): m_ID(0),m_CampType(0), m_BuffSrc(0){};
	~MessageBuff(){};
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream
			>> m_ID
			>> m_CampType
			>> m_BuffSrc;
	} 
	virtual int getMessageType() const { return EMT_BUFF; }
private: 
	CC_SYNTHESIZE(int, m_ID, ID); //buffID  
	CC_SYNTHESIZE(int, m_CampType, CampType);  //目标类型 
	CC_SYNTHESIZE(int, m_BuffSrc, BuffSrc); //buff来源 0--士气触发  1---野怪触发
};
DEFINE_FACTORY(MessageBuff);

//////////buff显示
class MessageBuffShow: public GameMessage
{
public:
	MessageBuffShow(): m_ID(0),m_CampType(0), m_CDTime(0), m_BuffSrc(0){};
	~MessageBuffShow(){};
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream
			>> m_CampType
			>> m_ID
			>> m_CDTime
			>> m_BuffSrc;
	} 
	virtual int getMessageType() const { return EMT_BUFFSHOW; }
private: 
	CC_SYNTHESIZE(int, m_ID, ID); //buffType 
	CC_SYNTHESIZE(int, m_CampType, CampType);  //阵营
	CC_SYNTHESIZE(float, m_CDTime, CDTime); //CD时间
	CC_SYNTHESIZE(int, m_BuffSrc, BuffSrc);
};
DEFINE_FACTORY(MessageBuffShow);
 

/////////////建筑减兵显示
class MessageReducePeople: public GameMessage
{
public:
	MessageReducePeople(): m_BuildID(0), m_Num(0), m_Camp(0){};
	~MessageReducePeople();
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream
			>> m_BuildID
			>> m_Camp
			>> m_Num; 
	} 
	virtual int getMessageType() const { return EMT_REDUCEPEOPLE; }
	int m_BuildID;
	int m_Num;
	int m_Camp;
private:

}; 
DEFINE_FACTORY(MessageReducePeople);

 
// 钻石改变
class MessageMoneyChange : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mRecordData
			>> mChangeOperation
			>> mVCType
			>> mChangeCnt
			>> mCurrentCnt
			>> mReason;
	}

	MessageMoneyChange() : mRecordData(0), mChangeOperation(0), mVCType(0), mChangeCnt(0), mCurrentCnt(0)
	{

	}

	virtual int getMessageType() const { return EMT_MONEY_CHANGE; }

	CC_SYNTHESIZE(int, mRecordData, RecordData); // OSS record, 0 for not, 1 for yes
	CC_SYNTHESIZE(int, mChangeOperation, ChangeOperation); // 1 for add or remove, 2 for set directly
	CC_SYNTHESIZE(int, mVCType, VCType); // 0 for diamond
	CC_SYNTHESIZE(int, mChangeCnt, ChangeCnt);
	CC_SYNTHESIZE(int, mCurrentCnt, CurrentCnt);
	CC_SYNTHESIZE(std::string, mReason, Reason);
};

DEFINE_FACTORY(MessageMoneyChange);

// 积分模式，玩家积分变化
class MsgResModeScore : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream){}

	virtual int getMessageType() const { return EMT_RESMODE_SCORE; }
};

DEFINE_FACTORY(MsgResModeScore);

//精力改变
class MessageEnergyChange : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{		
	}

	virtual int getMessageType() const { return EMT_ENERGY_CHANGE; }
};
DEFINE_FACTORY(MessageEnergyChange);

// 荣誉点改变
class MessageHonorChange : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream &sstream)
	{
	
	}
	virtual int getMessageType() const { return EMT_HONOR_CHANGE; }
};
DEFINE_FACTORY(MessageHonorChange);

//积分改变
class MessagePvpPointChange : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}

	virtual int getMessageType() const { return EMT_POINT_CHANGE; }
};
DEFINE_FACTORY(MessagePvpPointChange);

//关闭UI界面
class MessageClosePage: public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		 
	}

	virtual int getMessageType() const { return EMT_CLOSEPAGE;}
	 
};
DEFINE_FACTORY(MessageClosePage);


//取消匹配/无道具场开关
class MessagePvpClose : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mResult;
	}

	virtual int getMessageType() const { return EMT_STOP_MATCH; }
	int mResult;//0 正常结束 1 无道具场活动开启 2 无道具场活动关闭
};

DEFINE_FACTORY(MessagePvpClose);

//关闭pvp界面
class MessagePvpDilogClose : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}

	virtual int getMessageType() const { return EMT_CLOSE_PVP; }
};

DEFINE_FACTORY(MessagePvpDilogClose);

// 改变名字
class MessageChangeName : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{		
		sstream >> mResult;
	}

	virtual int getMessageType() const { return EMT_CHANGE_NAME; }

	int mResult;
};

DEFINE_FACTORY(MessageChangeName);

// 信號改变
class MessageSignalChange : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{		
		sstream >> mResult;
	}

	virtual int getMessageType() const { return EMT_SIGNAL_CHANGE; }

	int mResult;
};

DEFINE_FACTORY(MessageSignalChange);

//PVE开启新章节
class MessageOpenStage : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mStage;
	}

	virtual int getMessageType() const { return EMT_OPEN_STAGE; }
	int mStage;
};

DEFINE_FACTORY(MessageOpenStage);


//PVP
class MessageMenuBtn : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}

	virtual int getMessageType() const { return EMT_MENU_BTN; }
};

DEFINE_FACTORY(MessageMenuBtn);


class MessageRowDepart: public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mFormationId
			>> mAttackCamp
			>> mDefendCamp
			>> mScrBuild
			>> mDstBuild
			>> mCount;
	}

	virtual int getMessageType() const { return EMT_ROW_DEPART; }

	CC_SYNTHESIZE(int, mFormationId, FormationId);
	CC_SYNTHESIZE(int, mAttackCamp, AttackCamp);
	CC_SYNTHESIZE(int, mDefendCamp, DefendCamp);
	CC_SYNTHESIZE(int, mScrBuild, ScrBuild);
	CC_SYNTHESIZE(int, mDstBuild, DstBuild);
	CC_SYNTHESIZE(int, mCount, Count);
};

DEFINE_FACTORY(MessageRowDepart);

class MessageAchieveGoto: public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mGotoLevel;
	}

	virtual int getMessageType() const { return EMT_ACHIEVE_GOTOLEVEL; }

	CC_SYNTHESIZE(int, mGotoLevel, GotoLevel);
};

DEFINE_FACTORY(MessageAchieveGoto);

class MessageAchieveTip: public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mBtnType >> mIsShow;
	}

	virtual int getMessageType() const { return EMT_HONGDIAN_TIP; }
	CC_SYNTHESIZE(int, mBtnType, BtnType);//按钮类型 --->弹窗id
	CC_SYNTHESIZE(bool, mIsShow, IsShow);
};

DEFINE_FACTORY(MessageAchieveTip);

class MessageShopPayComplent: public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mProductId >> mOrderId;
	}

	virtual int getMessageType() const { return EMT_SHOP_PAYCOMPLENT; }

	CC_SYNTHESIZE(int, mProductId, ProductId);
	CC_SYNTHESIZE(std::string, mOrderId, OrderId);
};

DEFINE_FACTORY(MessageShopPayComplent);

class MessageBattleTips : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mBattleTipMsg >> mPositionX >> mPositionY >> mGlyphMsg;
	}

	virtual int getMessageType() const { return EMT_BATTLE_TIP; }

	CC_SYNTHESIZE(int, mBattleTipMsg, BattleTipMsg);
	CC_SYNTHESIZE(float, mPositionX, PositionX);
	CC_SYNTHESIZE(float, mPositionY, PositionY);
	CC_SYNTHESIZE(std::string, mGlyphMsg, GlyphMsg);
};
DEFINE_FACTORY(MessageBattleTips);

class MessageEnableOption : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mEnable;
	}

	virtual int getMessageType() const { return EMT_ENABLE_OPTIONBUTTON; }

	CC_SYNTHESIZE(int, mEnable, Enable);
};
DEFINE_FACTORY(MessageEnableOption);

class MessageBtnControl: public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mBtnType >> mIsShow;
	}

	virtual int getMessageType() const { return EMT_BTN_CONTROL; }
	CC_SYNTHESIZE(int, mBtnType, BtnType);//按钮类型 --->弹窗id
	CC_SYNTHESIZE(bool, mIsShow, IsShow);
};
DEFINE_FACTORY(MessageBtnControl);


class MessageFormationOver : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mFormationId
			>> mAttackCamp
			>> mDefendCamp
			>> mScrBuild
			>> mDstBuild
			>> mIsArrived;
	}

	virtual int getMessageType() const { return EMT_FORMATION_OVER; }

	CC_SYNTHESIZE(int, mFormationId, FormationId);
	CC_SYNTHESIZE(int, mAttackCamp, AttackCamp);
	CC_SYNTHESIZE(int, mDefendCamp, DefendCamp);
	CC_SYNTHESIZE(int, mScrBuild, ScrBuild);
	CC_SYNTHESIZE(int, mDstBuild, DstBuild);
	CC_SYNTHESIZE(int, mIsArrived, IsArrived);

};
DEFINE_FACTORY(MessageFormationOver);

class MessageDayMission : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mMissionId;
	}

	virtual int getMessageType() const { return EMT_MISSION_STATE; }

	CC_SYNTHESIZE(int, mMissionId, MissionId);
};
DEFINE_FACTORY(MessageDayMission);

class MessageActivationTip : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mStatusId;
	}

	virtual int getMessageType() const { return EMT_ACTIVATION_TIP; }

	CC_SYNTHESIZE(int, mStatusId, StatusId);
};

DEFINE_FACTORY(MessageActivationTip);

class MessageChatMsg : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mChatType;
	}

	virtual int getMessageType() const { return EMT_CHAT_MSG; }
	CC_SYNTHESIZE(int, mChatType, ChatType);
};

DEFINE_FACTORY(MessageChatMsg);

class MessageFriend : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mListId;
	}

	virtual int getMessageType() const { return EMT_FRIEND_UPDATE; }

	CC_SYNTHESIZE(int, mListId, ListId);
};
DEFINE_FACTORY(MessageFriend);

class MessageShopBuyItem : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mItemType;
	}

	virtual int getMessageType() const { return EMT_SHOP_SHOW_BUYITEM; }

	CC_SYNTHESIZE(int, mItemType, ItemType);
};

DEFINE_FACTORY(MessageShopBuyItem);

class MessageAfterBuyDiamond : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}

	virtual int getMessageType() const { return EMT_SHOP_AFTER_BUYDIAMOND; }
};

DEFINE_FACTORY(MessageAfterBuyDiamond);

class MessageChangePlayerIcon : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mIconId;
	}

	virtual int getMessageType() const { return EMT_CHANGE_PLAYER_ICON; }

	CC_SYNTHESIZE(std::string, mIconId, IconId);
};
DEFINE_FACTORY(MessageChangePlayerIcon);

class MessageBufferTips : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mBufferTipMsg >> mPositionX >> mPositionY;
	}

	virtual int getMessageType() const { return EMT_BUFFER_TIP; }

	CC_SYNTHESIZE(int, mBufferTipMsg, BufferTipMsg);
	CC_SYNTHESIZE(float, mPositionX, PositionX);
	CC_SYNTHESIZE(float, mPositionY, PositionY);
};
DEFINE_FACTORY(MessageBufferTips);


class MessageGameStart: public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mLocationID;
	}
	virtual int getMessageType() const { return EMT_GAMESTART; }
private:
	CC_SYNTHESIZE(int, mLocationID, LocationID);
};
DEFINE_FACTORY(MessageGameStart);


class MessageItemOperation : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mOperationType
			>> mItemId
			>> mItemType
			>> mItemCnt
			>> mVCCnt
			>> mVCType
			>> mParam;
	}
	virtual int getMessageType() const { return EMT_ITEM_OPERATION; }


	CC_SYNTHESIZE(int, mOperationType, OperationType);	// 0 for buy, 1 for get, 2 for use
	CC_SYNTHESIZE(int, mItemId, ItemId);
	CC_SYNTHESIZE(int, mItemType, ItemType);
	CC_SYNTHESIZE(int, mItemCnt, ItemCnt);
	CC_SYNTHESIZE(int, mVCType, VCType); // 0 for diamond
	CC_SYNTHESIZE(int, mVCCnt, VCCnt);
	CC_SYNTHESIZE(std::string, mParam, Param); // ConsumePoint for buy type, other for reason
};

DEFINE_FACTORY(MessageItemOperation);


class MessagePVPInfo : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mPVPType
			>> mPVPMode
			>> mOperation;
	}
	virtual int getMessageType() const { return EMT_PVP_INFO; }


	CC_SYNTHESIZE(int, mPVPType, PVPType); // 
	CC_SYNTHESIZE(int, mPVPMode, PVPMode); // 1 for match mode, 2 for room mode
	CC_SYNTHESIZE(int, mOperation, Operation); // 1 for start, 2 for end

};

DEFINE_FACTORY(MessagePVPInfo);

class MessagePropUseAll : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mPropId;
	}
	virtual int getMessageType() const { return EMT_PROP_USE_ALL; }


	CC_SYNTHESIZE(int, mPropId, PropId);

};

DEFINE_FACTORY(MessagePropUseAll);


class LittleTipsMessage : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mIsPause;
	}
	virtual int getMessageType() const { return EMT_LITTLETIPS; }
	CC_SYNTHESIZE(bool, mIsPause, IsPause);

};
DEFINE_FACTORY(LittleTipsMessage);


class GetSaveDataMessage : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mResult;
	}
	virtual int getMessageType() const { return EMT_GETSAVEDATAOVER; }
	CC_SYNTHESIZE(int, mResult, Result);

};
DEFINE_FACTORY(GetSaveDataMessage);

class RqstResultMessage : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mID >> mResult;
	}
	virtual int getMessageType() const { return EMT_RQST_RESULT; }
	CC_SYNTHESIZE(int, mID, ID); //请求ID
	CC_SYNTHESIZE(int, mResult, Result); //正确返回0，其他为错误码
};
DEFINE_FACTORY(RqstResultMessage);

class SDKLOGINMessage : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}
	virtual int getMessageType() const { return EMT_SDKLOGIN_SUCCESS; }
};
DEFINE_FACTORY(SDKLOGINMessage);

class GAMESERVERLOGINMessage : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}
	virtual int getMessageType() const { return EMT_GAMESERVERLOGIN_SUCCESS; }
};
DEFINE_FACTORY(GAMESERVERLOGINMessage);

class SimplifyUIMessage : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}
	virtual int getMessageType() const { return EMT_SIMPLIFY_UI; }

};
DEFINE_FACTORY(SimplifyUIMessage);

class MessageUpdatePlayerBadges : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}

	virtual int getMessageType() const { return EMT_UPDATE_PLAYER_BADGES; }

};
DEFINE_FACTORY(MessageUpdatePlayerBadges);

class MessageChangePlayerBadge : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mBadgeId;
	}

	virtual int getMessageType() const { return EMT_CHANGE_PLAYER_BADGE; }
	CC_SYNTHESIZE(int, mBadgeId, BadgeId); 

};
DEFINE_FACTORY(MessageChangePlayerBadge);

class MessageOutPercentBtn : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mPercent;
	}

	CC_SYNTHESIZE(int, mPercent, Percent); //请求ID
	virtual int getMessageType() const { return EMT_BUTTON_OUT_PERCENT; }
};

DEFINE_FACTORY(MessageOutPercentBtn);
//PVP
class MessageOutPercentControl : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mPercent;
	}

	CC_SYNTHESIZE(int, mPercent, Percent); //请求ID
	virtual int getMessageType() const { return EMT_CONTROL_OUT_PERCENT; }
};

DEFINE_FACTORY(MessageOutPercentControl);

class MessagePropUseControl : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mX >> mY;
	}

	CC_SYNTHESIZE(float, mX, X);	//坐标x
	CC_SYNTHESIZE(float, mY, Y);	//坐标y

	virtual int getMessageType() const { return EMT_CONTROL_PROP_USE; }
};

DEFINE_FACTORY(MessagePropUseControl);

class MessageClickPropControl : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mPropId;
	}

	CC_SYNTHESIZE(int, mPropId, PropId);

	virtual int getMessageType() const { return EMT_CONTROL_CLICK_PROP; }
};

DEFINE_FACTORY(MessageClickPropControl);

class MessageClickPropSecendControl : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mPropId;
	}

	CC_SYNTHESIZE(int, mPropId, PropId);

	virtual int getMessageType() const { return EMT_CONTROL_CLICK_PROP_SECEND; }
};

DEFINE_FACTORY(MessageClickPropSecendControl);

class MessageCanelPropControl : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mClickType;
	}

	CC_SYNTHESIZE(int, mClickType, ClickType);

	virtual int getMessageType() const { return EMT_CONTROL_CANCEL_PROP; }
};

DEFINE_FACTORY(MessageCanelPropControl);

class MessageClickStartGameBtn : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}

	virtual int getMessageType() const { return EMT_BUTTON_LOGINSCENE; }

};
DEFINE_FACTORY(MessageClickStartGameBtn);

class MessageClickStartGameBtnUnvisible : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}

	virtual int getMessageType() const { return EMT_BUTTON_LOGINSCENE_UNVISIBLE; }

};
DEFINE_FACTORY(MessageClickStartGameBtnUnvisible);

class MessageCrazyWeek : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mCmd;
	}

	virtual int getMessageType() const { return EMT_CRAZY_WEEK; }

	CC_SYNTHESIZE(int, mCmd, Cmd); // 
};
DEFINE_FACTORY(MessageCrazyWeek);

class MessageOpenLevelDialog : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}

	virtual int getMessageType() const { return EMT_OPEN_LEVEL_DIALOG; }

};
DEFINE_FACTORY(MessageOpenLevelDialog);

class MessageShopList : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}

	virtual int getMessageType() const { return EMT_UPDATE_SHOPLIST; }

};
DEFINE_FACTORY(MessageShopList);

class MessageGW : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> m_iType;
	}

	virtual int getMessageType() const { return EMT_GW; }

	CC_SYNTHESIZE(int, m_iType, Type);

};
DEFINE_FACTORY(MessageGW);

class MessageUpdateExchange : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream &sstream)
	{
		sstream >> mItemId;
	}

	virtual int getMessageType() const { return EMT_UPDATE_EXCHANGE;  }

	CC_SYNTHESIZE(int, mItemId, ItemId);
};
DEFINE_FACTORY(MessageUpdateExchange);

class MessageCloseShop : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}

	virtual int getMessageType() const { return EMT_CLOSESHOP; }

};
DEFINE_FACTORY(MessageCloseShop);


class MessageGotoActTab : public GameMessage
{

public:
	virtual void formatMessage(std::stringstream &sstream)
	{
		sstream >> mPage >> mLab;
	}

	virtual int getMessageType() const { return EMT_GOTO_ACTLAB; }

	CC_SYNTHESIZE(int, mPage, Page);
	CC_SYNTHESIZE(int, mLab, Lab);

};

DEFINE_FACTORY(MessageGotoActTab);


class MessageBuyVip : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mID;
	}

	virtual int getMessageType() const { return EMT_BUY_VIP; }
	CC_SYNTHESIZE(int, mID, ID);
};
DEFINE_FACTORY(MessageBuyVip);

class MessagePlayerUpgrade : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}

	virtual int getMessageType() const { return EMT_PLAYER_UPGRADE; }
	
};
DEFINE_FACTORY(MessagePlayerUpgrade);


class MessageGameLoginFlow : public GameMessage
{
public:
	enum FlowType
	{
		ChannelLogin = 1,
		EnterCenterSvr,
		EnterLoginSvr,
		EnterGameSvr,
	};

public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mFlowType >> mResult >> mParam1;
	}

	virtual int getMessageType() const { return EMT_GAME_LOGIN_FLOW; }

	int mFlowType;
	int mResult;
	std::string mParam1;
};

DEFINE_FACTORY(MessageGameLoginFlow);

class MessageRedReward : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mIsReward;
	}

	virtual int getMessageType() const { return EMT_RED_PACKET_CAN_REWARD; }
	CC_SYNTHESIZE(bool, mIsReward, IsReward);
};
DEFINE_FACTORY(MessageRedReward);

class MessageChangeServer : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mServerNum;
	}

	virtual int getMessageType() const { return EMT_CHANGE_SERVER; }
	CC_SYNTHESIZE(int, mServerNum, ServerNum);

};
DEFINE_FACTORY(MessageChangeServer);


class MessageSchedule : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}
	virtual int getMessageType() const { return EMT_SELECTWORD_SCHEDULE; }

};
DEFINE_FACTORY(MessageSchedule);

class MessageChangeSign : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
	}
	virtual int getMessageType() const { return EMT_CHANGE_SIGN; }

};
DEFINE_FACTORY(MessageChangeSign);

class MessageChangeExtraVisible : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mIsVisible;
	}
	virtual int getMessageType() const { return EMT_CHANGE_EXTRA_VISIBLE; }
	CC_SYNTHESIZE(int, mIsVisible, IsVisible);

};
DEFINE_FACTORY(MessageChangeExtraVisible);

class MessageGetExtra : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mTotalPvpCount >> mTotalPveCount >> mTotalPvpUseItem >> mTotalPveUseItem >> mMaxPvpPoint >> mMaxPvpWinCount;
	}
	virtual int getMessageType() const { return EMT_GET_EXTRA; }

	CC_SYNTHESIZE(int, mTotalPvpCount, TotalPvpCount);
	CC_SYNTHESIZE(int, mTotalPveCount, TotalPveCount);
	CC_SYNTHESIZE(int, mTotalPvpUseItem, TotalPvpUseItem);
	CC_SYNTHESIZE(int, mTotalPveUseItem, TotalPveUseItem);
	CC_SYNTHESIZE(int, mMaxPvpPoint, MaxPvpPoint);
	CC_SYNTHESIZE(int, mMaxPvpWinCount, MaxPvpWinCount);
};
DEFINE_FACTORY(MessageGetExtra);

class MessageRecordCreepsBorn : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mCreepsId >> mHittedCreepsID >> mHitIndex;
	}
	virtual int getMessageType() const { return EMT_RECORD_CREEPS_BORN; }

	CC_SYNTHESIZE(int, mCreepsId, CreepsId);
	CC_SYNTHESIZE(int, mHittedCreepsID, HittedCreepsID);
	CC_SYNTHESIZE(unsigned int, mHitIndex, HitIndex);
};
DEFINE_FACTORY(MessageRecordCreepsBorn);

class MessageRecordOutPercent : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mOutPercent;
	}
	virtual int getMessageType() const { return EMT_RECORD_OUT_PERCENT; }

	CC_SYNTHESIZE(int, mOutPercent, OutPercent);

};
DEFINE_FACTORY(MessageRecordOutPercent);

class MessageRecordClickProp : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mPropId;
	}
	virtual int getMessageType() const { return EMT_RECORD_CLICK_PROP; }

	CC_SYNTHESIZE(int, mPropId, PropId);

};
DEFINE_FACTORY(MessageRecordClickProp);

class MessageRecordClickPropSecend : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mPropId;
	}
	virtual int getMessageType() const { return EMT_RECORD_CLICK_PROP_SECEND; }

	CC_SYNTHESIZE(int, mPropId, PropId);

};
DEFINE_FACTORY(MessageRecordClickPropSecend);

class MessageRecordCancelProp : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mCancelType;
	}
	virtual int getMessageType() const { return EMT_RECORD_CANCEL_PROP; }

	CC_SYNTHESIZE(int, mCancelType, CancelType);

};
DEFINE_FACTORY(MessageRecordCancelProp);

class MessagePvpResultOpenBox : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		
	}
	virtual int getMessageType() const { return EMT_PVPRESULT_OPENBOX; }


};
DEFINE_FACTORY(MessagePvpResultOpenBox);

class MessageOpenDailyMission : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		
	}
	virtual int getMessageType() const { return EMT_OPEN_DAILY_MISSION; }

};
DEFINE_FACTORY(MessageOpenDailyMission);

class MessageDailyMissionTips : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mTipType >> mIsShow >> mX >> mY;
	}
	virtual int getMessageType() const { return EMT_DALIY_MISSION_TIPS; }
	CC_SYNTHESIZE(int, mTipType, TipType);
	CC_SYNTHESIZE(int, mIsShow, IsShow);
	CC_SYNTHESIZE(float, mX, X);	
	CC_SYNTHESIZE(float, mY, Y);
};
DEFINE_FACTORY(MessageDailyMissionTips);

class MessageDailyMissionProgressBack : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{

	}
	virtual int getMessageType() const { return EMT_DAILY_MISSION_PROGRESS_BACK; }

};
DEFINE_FACTORY(MessageDailyMissionProgressBack);

class MessageShowDailyMissionProgress : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{

	}
	virtual int getMessageType() const { return EMT_SHOW_DAILY_MISSION_PROGRESS; }

};
DEFINE_FACTORY(MessageShowDailyMissionProgress);

class MessageShareSuccess: public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{

	}
	virtual int getMessageType() const { return EMT_SHARE_SUCCESS; }

};
DEFINE_FACTORY(MessageShareSuccess);


class MessageSkipSuccess : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{

	}
	virtual int getMessageType() const { return EMT_SKIP_SUCCESS; }

};
DEFINE_FACTORY(MessageSkipSuccess);

class MessageInviteBatSwitch : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{

	}
	virtual int getMessageType() const { return EMT_INVITE_BAT_SWITCH; }
};
DEFINE_FACTORY(MessageInviteBatSwitch);

class MessageRecordTurretUpdate : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mBuildId >> mStep;
	}

	CC_SYNTHESIZE(int, mBuildId, BuildId);
	CC_SYNTHESIZE(int, mStep, Step);

	virtual int getMessageType() const { return EMT_RECORD_TURRET_UPDATE; }
};
DEFINE_FACTORY(MessageRecordTurretUpdate);

class MessageRecordExecuteTurretUpdate : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> mBuildId >> mStep;
	}

	CC_SYNTHESIZE(int, mBuildId, BuildId);
	CC_SYNTHESIZE(int, mStep, Step);

	virtual int getMessageType() const { return EMT_RECORD_EXECUTE_TURRET_UPDATE; }
};
DEFINE_FACTORY(MessageRecordExecuteTurretUpdate);


//关闭UI界面
class MessageGuideFirst : public GameMessage
{
public:
	MessageGuideFirst() : m_State(-1){};
	virtual void formatMessage(std::stringstream& sstream)
	{
		sstream >> m_State;
	}

	virtual int getMessageType() const { return EMT_GUIDE_FIRST; }
private:
	CC_SYNTHESIZE(int, m_State, State);
};
DEFINE_FACTORY(MessageGuideFirst);


// 排名改变
class MessageRankChange : public GameMessage
{
public:
	virtual void formatMessage(std::stringstream &sstream)
	{

	}
	virtual int getMessageType() const { return EMT_RANK_CHANGE; }
};
DEFINE_FACTORY(MessageRankChange);
