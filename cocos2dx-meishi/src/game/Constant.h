/* game constant value
* jacey
* 20150506
*/
#pragma once

#include "cocostudio/ActionTimeline/CSLoader.h"
#include "cocostudio/WidgetCallBackHandlerProtocol.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"
#include "cocostudio/CCObjectExtensionData.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "EnvSet.h"
#include "Singleton.h"

typedef unsigned long long uint64;

const int g_TimeNodeInterval = 5 * 6;

#define USER_SERVER_NAME "USER_SERVER_NAME"
#define USER_SERVER_IP "USER_SERVER_IP"
#define SRV_LIST_URL "http://10.10.1.134/address.php"

#define REQUEST_NOTICE "Notice.csv"
#define REQUEST_ACTINFORM "ActInform.csv"
#define TILED_SIZE 20
#define TILED_FEET_SIZE 29
#define FORMATION_ROW_SPACE 1
#define NORMAL_SOLDIER_SPEED 50.0f 
#define POWER_RECOVERY_TIMES 5 * 60		// 体力恢复间隔

#define POWER_RECOVERY_TIMES_PVP 60*5
#define  PVPPOWER_INIT_VALUE 50
#define  PVEPOWER_INIT_VALUE 50
#define  PVPPOWER_RESUMEVALUE 1
#define  PVEPOWER_RESUMEVALUE 1

#define PVP_ENERGY_VALUE  5			//pvp消耗精力
#define POWER_BUY_MAX 150            //体力精力可购买时的最大值
#define PVP_ENERGY_TYPE 1001
#define PVE_POWER_TYPE 1002
#define ITEM_BUY_TYPE 10001 //道具起始

#define MGAME_FILE_DECODE_KEY		"HappyMGame"
#define MGAME_FILE_SIGNATURE		"61Games-MGame"
#define TAG_GUIDELAYER				321654
#define TAG_GUIDELAYERCHILD1		1001
#define TAG_GUIDELAYERCHILD2		1002
#define TAG_GUIDELAYERCHILD3		1003
#define TAG_GUIDELAYERCHILD4		1004
const int DIS_SOLDIER_SOLDIER = TILED_FEET_SIZE;
const int DIS_OTHER_BUILD = TILED_FEET_SIZE * 2;
const int DIS_SRCTAR_BUILD = TILED_FEET_SIZE * 5;

#define PVP_SCORE_VALUE  200			//pvp无道具场需要积分
#define AREA_NAME_COUNT (6)		//地区数量

#define DEFAULT_FONT     "Helvetica"

#define RICH_TEXT_ALL		 "<txt face='%s' size='%d' color='%s'>%s</txt>"		    //包含字体，字体大小，字体颜色的富文本
//#define RICH_TEXT_FONT_COLOR "<txt face='%s' color='%s'>%s</txt>"					//包含字体，字体颜色的富文本
//#define RICH_TEXT_FONT_SIZE  "<txt face='%s' size='%d'>%s</txt>"					//包含字体，字体大小的富文本
//#define RICH_TEXT_SIZE_COLOR "<txt size='%d' color='%s'>%s</txt>"					//包含字体大小，字体颜色的富文本
//#define RICH_TEXT_FONT		 "<txt face='%s'>%s</txt>"								//包含字体的富文本
//#define RICH_TEXT_SIZE		 "<txt size='%d'>%s</txt>"								//包含字体大小的富文本
//#define RICH_TEXT_COLOR		 "<txt color='%s'>%s</txt>"							    //包含字体颜色的富文本

namespace RichHelp
{
	struct pTextDefnition
	{
		std::string text;		//内容
		std::string fntName;	//字体名字 eg: fonts/font.ttf
		std::string fntColor;	//字体颜色 eg: #5a4eae
		int fntSize;			//字体大小 eg: 26

		pTextDefnition() :text(""), fntName("fonts/font.TTF"), fntColor("000000"), fntSize(26){}
	};
}

//
typedef void (CCObject::*SEL_CallFuncPage)(int pageIndex, int id, int index);
#define callfuncPage_selector(_SELECTOR) (SEL_CallFuncPage)(&_SELECTOR)

typedef void (CCObject::*SEL_CallFuncID)(int id);
#define callfuncID_selector(_SELECTOR) (SEL_CallFuncID)(&_SELECTOR)

//好友信息、pvp信息、查看详细信息等模块用到，故置此地
namespace FriendSystem
{
	struct PlayerData
	{
		unsigned int m_uId;
		std::string m_sName;
		std::string m_iIcon;
		int m_iGroup;
		int m_iOrder;
		int m_iScore;
		float m_fWinRate;
		int m_iTime;
		int m_iLogoutTime;
		int m_iRoom;
		unsigned int m_uInviterUin;
		int m_iIndex;
		int m_iTitle;
		int m_iLevel;//成就等级
		int m_iVipLevel;
		bool m_bTPFriend;
		int m_iPlayerLevel;//玩家等级
		int m_iExp;
		std::string m_sOpenId;
		std::string m_sThirdPartyIcon;	
		PlayerData() : m_uId(0), m_sName(""), m_iIcon("0"), m_iGroup(0),
			m_iOrder(0), m_iScore(0), m_fWinRate(0), m_iTime(0),
			m_iLogoutTime(0), m_iRoom(0), m_uInviterUin(0),
			m_iIndex(0), m_iTitle(0), m_iLevel(0), m_iVipLevel(0), 
			m_bTPFriend(false), m_sOpenId(""), m_sThirdPartyIcon(""), m_iPlayerLevel(1), m_iExp(0){}
	};
};

enum eChargeMode
{
	CHARGE_TYPE_INVALID = 0,
	CHARGE_TYPE_DAY,//30天内每天可领500钻石,多次购买延长时间
	CHARGE_TYPE_POWER, //前5次购买每次增加10点体力上限
	CHARGE_TYPE_FLAG, //首次购买赠送标记位
	CHARGE_TYPE_P_POWER, //首次购买赠送精力
	CHARGE_TYPE_COIN, //首次购买金币
	CHARGE_TYPE_MAX //最大
};

enum ERRORCODE
{
	OK                          = 0,   //-- 正常

	//--登陆相关
	ACCOUNT_LOGIN_EXPECTION     = 1001, //-- 登陆异常
	ACCOUNT_AUTH_FAIL           = 1002, //-- 第三方认证失败
	ACCOUNT_NO_LOGIN            = 1003, //-- 无登陆流程

	//--Player相关
	POWER_NOT_ENOUGH            = 2001, //-- 体力不足
	TIME_COUNT_ERROR            = 2002, //-- dao
	NAME_EXIST_ERROR			= 2003, //-- 名称已存在
	NAME_NOT_CHANGE				= 2004, //-- 昵称没有变化

	//cdk 相关
	GIFT_TYPE_IS_USED			= 2008,	//-- 礼包类型已经使用过了
	GIFT_CODE_NOE_EXIST			= 2009, //-- 激活码不存在
	GIFT_TYPE_NOE_EXIST			= 2010, //-- 激活码类型不存在
	CODE_IS_USED				= 2011,	//-- 激活码已经被使用了
	PARAM_ERROR					= 2012,	//-- 参数错误
	NAME_ERROR					= 2014,	//-- 名字存在非法字符

	//--PVP相关
	PVP_NOT_MATCH               = 3001, //-- pvp匹配失败
	ENERGY_NOT_ENOUGH           = 3002, //-- 精力不足	
	PVP_NOT_TIME				= 3003, //-- //不在时间段内
	PVP_NOT_NET					= 3005, //-- //网络断开
	NOT_MATCH_STATE				= 3006, //停止匹配错误码，不是在匹配状态
	NOT_PVP_SCORE				= 3007, //无道具场 积分不足


	//--礼品相关
	GIFT_NOT_EXIST 		        = 4001, //-- 礼包码不存在
	ACHIEVEMENT_REWARD_NOT_EXIST = 4003, //-- 成就奖励不存在
	LEVEL_OR_POINT_ERROR         = 4004, //-- 成就点数或者等久等级错误
	RECEIVE_ERROR        		 = 4005, //-- 成就奖励已经领取完了不能重复领取
	ACHIEVEMENTS_NOT_COMPLEMENT = 4006,  //-- 成就没有完成
	//--关卡相关
	STAGE_STAR_COUNT_ERROR =	5001, //-- 关卡星星总数不够

	//-- 道具相关
	ITEM_COUNT_ERROR    	= 6001, //--道具数量错误
	ITEM_DIAMAON_NOT_ENOUGH_ERROR    	= 6002, //--钻石不够

	//活动相关
	ACTIVITY_RECEIVE_ERROR  = 6101, // 活动奖励已领取
	ACTIVITY_NOT_RECEIVE_ROCODE_ERROR  = 6102, //  没有可领取记录、
	ACTIVITY_NOT_START_ERROR  = 6103 ,	//-- 活动还未开始
	ACTIVITY_END_ERROR  = 6104, //-- 活动已经结束了
};

enum eCampType
 {
	ECAMP_TYPE_Neutral = 0,		//中立	饭团
	ECAMP_TYPE_Player,			//控制端  包子
	ECAMP_TYPE_AI,				//AI	粽子
	ECAMP_TYPE_AI2,				//AI	汉堡
	ECAMP_TYPE_AI3,				//AI	蛋糕
	ECAMP_TYPE_Player1,			//PVP	包子
	ECAMP_TYPE_Player2,			//PVP	粽子
	ECAMP_TYPE_Player3,			//PVP	汉堡
	ECAMP_TYPE_Player4,			//PVP	蛋糕 
	ECAMP_TYPE_Creeps,			//打野建筑
	ECAMP_TYPE_Invalid			//无效
};

enum eExpressType
{
	EXP_TYPE_Neutral = 0,		//中立	饭团
	EXP_TYPE_Player,			//控制端  包子
	EXP_TYPE_AI,				//AI	粽子
	EXP_TYPE_AI2,				//AI	汉堡
	EXP_TYPE_AI3,				//AI	蛋糕
	EXP_TYPE_Player1,			//PVP	包子
	EXP_TYPE_Player2,			//PVP	粽子
	EXP_TYPE_Player3,			//PVP	汉堡
	EXP_TYPE_Player4,			//PVP	蛋糕 
	EXP_TYPE_LaJiao,			//打野建筑-辣椒 9 
	EXP_TYPE_DaCong,			//打野建筑-大葱 10
	EXP_TYPE_JiDan,				//打野建筑-鸡蛋 11
	EXP_TYPE_NaiLao,			//打野建筑-奶酪
	EXP_TYPE_SuanTou,			//打野建筑-蒜头
	EXP_TYPE_XiaMi1,			//打野建筑-虾米 
	EXP_TYPE_Invalid			//无效
};

//建筑类型
enum eBulidType
{
	EBULID_TYPE_BARRACK = 1,	//兵营
	EBULID_TYPE_TURRET,			//防御塔
	EBULID_TYPE_INSISTUTE,		//研究所
	EBULID_TYPE_CREEPS,			// 打野建筑
	EBUILD_TYPE_STABLES,		//马厩
};

//道具类型
enum ePropType
{
	PROP_TYPE_EXCHANGE_B = 1,//转换为兵营
	PROP_TYPE_EXCHANGE_I,	// 转换为研究所
	PROP_TYPE_EXCHANGE_T,	// 转换为炮塔
	PROP_TYPE_ADDPEOPEL,//兵力包
	PROP_TYPE_TIME,//破表
	PROP_TYPE_COOKING,//召唤厨师
	PROP_TYPE_FREEZE,//冰镇
	PROP_TYPE_EXCHANGE,//建筑转换卷
	PROP_TYPE_INVISIBILITY_CLOAK,//隐形斗篷
	PROP_TYPE_CHAIN_OIL,//地沟油
	PROP_TYPE_NULL,
};

//建筑动画类型
enum eBuildAni
{
	BUILD_ANI_STAND = 0,			//站立
	BUILD_ANI_BEHURT,				//受击
	BUILD_ANI_OUTPUT_STAR,			//出兵开始_左
	BUILD_ANI_OUTPUTING,			//出兵进行中_左
	BUILD_ANI_OUTPUT_END,			//出兵结束_左
	BUILD_ANI_OUTPUT_STAR_RIGHT,	//出兵开始_右
	BUILD_ANI_OUTPUTING_RIGHT,		//出兵进行中_右
	BUILD_ANI_OUTPUT_END_RIGHT,		//出兵结束_右
	//BUILD_ANI_INSISTUTE_STAND,		//研究所待机
	//BUILD_ANI_TURRET_STAND,			//炮台待机
	BUILD_ANI_TURRET_ATTACK,		//炮台攻击
	BUILD_ANI_UPGRADE,
	BUILD_ANI_RANGE_BORN,
	BUILD_ANI_RANGEING,
	BUILD_ANI_EXCHANGE,
	BUILD_ANI_EXCHANGE1,
	BUILD_ANI_EXCHANGE2,
	BUILD_ANI_EXCHANGE3,
	BUILD_ANI_EXCHANGE_END,
	BUILD_ANI_SMALL_CRY,
	BUILD_ANI_BIG_CRY,
	BUILD_ANI_WIN,					//胜利
	BUILD_ANI_NONE
};

// 通关规则
enum eBalanceTag
{
	BALANCE_TAG_INVALID = 0,
	BALANCE_TAG_DESTORY,	// 歼灭模式
	BALANCE_TAG_OCCUPY,		// 占领模式
	BALANCE_TAG_RESOURCE,	// 资源模式
	BALANCE_TAG_SURVIVAL,	// 生存模式
	BALANCE_TAG_GUARD,		// 守护模式
};

enum eBalanceScore
{
	SCORE_TIME = 1,
	SCORE_PEOPLE,
	SCORE_BOMB,
	SCORE_FREEZE,
	SCORE_EXCHANGE,
	SCORE_INVISIBILITY,
	SCORE_SPEED,
};

// 评星规则
enum eStarTag
{
	STAR_TAG_INVALID = 0,
	STAR_TAG_SUCCESS_IN_N,
	STAR_TAG_ENE_BUILD_L_N,	// 敌方阵营占据的建筑数量
	STAR_TAG_BUILD_NODESTROY,// 敌方阵营从未占领指定的建筑
	STAR_TAG_NEUTRAL_N,		// 剩余N个中立阵营的数量
	STAR_TAG_NO_PEOP_MN,	// 任何一方兵力上限不超过N
	STAR_TAG_TAR_BUILD,		// 胜利时，占领指定的建筑
	STAR_TAG_MORE_ENE_N,	// 己方兵力超过敌方兵力N
	STAR_TAG_PEOPLE_L_N,	// 己方兵力不超N
	STAR_TAG_PEOPLE_M_N,	// 己方兵力超N
	STAR_ENE_PEOPLE_L_N,	// 敌方兵力不超N
	STAR_ENE_PEOPLE_M_N,	// 敌方兵力超N
	STAR_TAG_BUILD_LOST,	// 己方初始的建筑丢失数量
	STAR_TAG_MY_BUILD_M_N,	// 己方拥有的建筑数量
	STAR_TAG_SUCC_LAST_MN,	// 胜利时己方剩余的兵力
	STAR_TAG_MONSTER_TIMES,	// 打野次数
	STAR_TAG_BUILD_UPGRADE,	// 不升级初始建筑
	STAR_TAG_OCampNoDestroy_GetTar,		// 标星建筑不改变阵营时，占领标框的建筑
	STAR_TAG_MORALE_MN,		// 士气超过N
};

// 游戏内文本
enum eTextTagBase
{
	TEXT_BALANCE_MODE = 100,	// 通关模式
	TEXT_BALANCE_STAR = 200,	// 评星
	TEXT_GUARD = 300,			// 新手指引
	TEXT_COMM_TIPS = 400,		// 通用提示文本
	TEXT_ACHIEVE = 600,       //成就
	TEXT_ACT        = 700,      //活动
	TEXT_LOGIN		= 800,     //登录
	TEXT_RENAME		= 820,     //修改名字
	TEXT_BUY_ITEM = 900,     //购买
	TEXT_PVP	= 1000,		//pvp相关文本
	TEXT_BATTLE = 2000,		//战斗相关文本
	TEXT_CHAT = 7000, //聊天
	TEXT_FRIEND = 8000,		//好友相关文本
	TEXT_POWER_GIFT = 9000,		//体力赠送文本
	TEXT_BUFF	= 10000, //BUFF提示
	TEXT_SAVEDATA = 12000, //存档提示
	TEXT_RANKING = 13000, // 排行榜
	TEXT_VIP     = 15000,//VIP
	TEXT_BADGE = 20000, // 徽章
	TEXT_SHOP = 21100,//shop
	TEXT_BTH =	22100,//选关界面
	TEXT_GET_HONOR = 22104, // 获取荣誉提示
};

enum eTextTagStar
{
	TEXT_STAR_SUCCESS = 250,
	TEXT_STAR_FAILED = TEXT_STAR_SUCCESS + 1,
	TEXT_STAR_SuccessN = TEXT_BALANCE_STAR + 1,		// 在N秒内取得胜利
	TEXT_STAR_EnemyBuildsLN,						// 任意敌人占领的建筑数量不高于N
	TEXT_STAR_Build_NoDestroy,					
	TEXT_STAR_EneBuild_LastLN,						// 剩余不超过%d个寿司建筑取得胜利
	TEXT_STAR_OwnPeople_LN,
	TEXT_STAR_GetTar_Builds,					// 占领全部标记的建筑
	TEXT_STAR_People_M_ENE_N,							// 兵力数量大于敌方%d
	TEXT_STAR_People_LN,							// 兵力数量不超过%d
	TEXT_STAR_People_MN,							// 兵力数量超过%d
	TEXT_STAR_EnePeo_LN,							// 敌方兵力不超N
	TEXT_STAR_EnePeo_MN,							// 敌方兵力超N
	TEXT_STAR_BuildLost_LN,							// 己方初始的建筑丢失数量
	TEXT_STAR_OwnBuild_MN,							// 己方拥有的建筑数量
	TEXT_STAR_SuccOwnPeop_MN,						// 胜利时己方剩余的兵力
	TEXT_STAR_KillMonster_MN,						// 打野次数
	TEXT_STAR_NoInitBuild_Upgrade,					// 不升级初始建筑
	TEXT_STAR_NoDestroyGetTar,						// 指定的阵营不丢失指定的建筑同时指定的建筑被玩家占领
	TEXT_STAR_HAS_N_BUILD,							// 同时拥有N个建筑
	TEXT_STAR_MORALE_MN,							// 士气超过N
	TEXT_STAR_NO,									// 没有评星条件，第一条显示无

};

enum eVipText
{
	TEXT_VIP1 = TEXT_VIP +1,
	TEXT_VIP2 ,
	TEXT_VIP3,
	TEXT_VIP4,
	TEXT_VIP5,
	TEXT_VIP6,
	TEXT_VIP7,
	TEXT_VIP8,
	TEXT_VIP9,
	TEXT_VIP10,
	TEXT_VIP11,
	TEXT_VIP12,
	TEXT_VIP13,
	TEXT_VIP14,
	TEXT_VIP15,
	TEXT_VIP_TIME1,
	TEXT_VIP_TIME2 ,
	TEXT_VIP_HONOR,//荣誉
	TEXT_VIP1_TIPS,  //vip1提示
	TEXT_VIP2_TIPS,
	TEXT_VIP3_TIPS,
	TEXT_VIP22,
	TEXT_VIP23,
	TEXT_VIP24,
	TEXT_VIP25,
};

enum eCommTips
{
	TEXT_COMMTIPS_TIPS = TEXT_COMM_TIPS,
	TEXT_COMMTIPS_WORLD,	
	TEXT_COMMTIPS_WORLD2,
	TEXT_COMMTIPS_CHECKPT,			// 上一关卡未开启
	TEXT_COMMTIPS_CP_STARS,			// 上一关卡星星数不足
	TEXT_COMMTIPS_NO_CHARGE,		// 充值功能未开放
	TEXT_COMMTIPS_NO_TOOLS,			// 道具功能未开放
	TEXT_COMMTIPS_NO_POWERS,		// 体力不足
	TEXT_COMMTIPS_STATE2_1,
	TEXT_COMMTIPS_STATE2_2,
	TEXT_COMMTIPS_PROP_TEXT1,			//道具提示
	TEXT_COMMTIPS_PROP_HEXT,
	TEXT_COMMTIPS_PROP_CONTENT,
	TEXT_COMMTIPS_SHOP_TEXT1,
	TEXT_COMMTIPS_PROP_AIM_TEXT,			//道具没有目标
	TEXT_COMMTIPS_END,						//结算异常提示
	TEXT_COMMTIPS_PROP_DATA,					//死亡不能使用道具提示
	TEXT_COMMTIPS_AWARD,				//恭喜您获得
	TEXT_COMMITPS_CHARGMAX,         //短代充值达上线
	TEXT_COMMITPS_ReLoginOk,           //重连OK
	TEXT_COMMTIPS_GLYPGNOOPEN,			//符文需要玩家等级15级
	TEXT_COMMTIPS_GUIDENODEERROR,      //新手节点号有误
	TEXT_SERVER_WILL_STOP,			//服务器即将关闭
	TEXT_COMMTIPS_GLYPH_TIP,			//战斗界面的符文提示框
	TEXT_GUIDE_LEVEL_TIP = 425,			//教学关卡解锁提示
};

enum eAchieveTips
{
	TEXT_ACHIEVE_GETFAILUES = TEXT_ACHIEVE,  //领取成就失败
	TEXT_ACHIEVE_GETPOINT,								//获得%d点成就点数
	TEXT_ACHIEVE_CANGET,
	TEXT_ACHIEVE_GETED,
	TEXT_ACHIEVE_CANNOTGET,
	TEXT_ACHIEVE_POINT_1,
	TEXT_ACHIEVE_POINT_2,
	TEXT_ACHIEVE_STAGE_NOOPEN,
	TEXT_ACHIEVE_LEVEL_NOOPEN,
	TEXT_ACHIEVE_COMPLENT_PLAYER,
};

enum eActText
{
	TEXT_ACT_GOLD = TEXT_ACT,		//金币
	TEXT_ACT_TILI,								//体力
	TEXT_ACT_JINGLI,							//精力
	TEXT_ACT_NOMAIL,                      //当前没有收到邮件!
	TEXT_ACT_OPEN_TIME,                      //活动开启时间
	TEXT_ACT_ROOM_TICKET,					//  roomticket
	TEXT_ACT_OPEN_FOREVER,                      //永久开启
};

enum eLoginTips
{
	TEXT_LOGIN_BEGIN = TEXT_LOGIN,
	TEXT_LOGIN_AUTO,	//自动登录提示
	TEXT_SERVER_FORCE,//服务器强制退出
	TEXT_LOGIN_QUEUE,//登录排队
	TEXT_NETWORK_ERROR,//网络异常
	TEXT_FORCE_UPDATE,//强更
	TEXT_LOGIN_WAIT,
	TEXT_NET_ERROR_TIP,//网络连接异常，请检查网络后重试
	TEXT_CUR_VER,
	TEXT_LOGIN_BY_WX,
	TEXT_LOGIN_BY_QQ,
	TEXT_LOGIN_BY_YK,
	TEXT_SRV_CONN_FAIL,	//服务器连接失败
};

enum ePvpTips
{
	TEXT_PVP_2V2_LOCK = TEXT_PVP,
	TEXT_PVP_NO_CONN,
	TEXT_PVP_NO_RANK,//未上榜
	TEXT_PVP_MACTH,//匹配中
	TEXT_PVP_SCORE,//积分不足
	TEXT_SCORE_NEED,//段位积分
	TEXT_RANK_GROUP_1 = 1011,
	TEXT_RANK_GROUP_2,
	TEXT_RANK_GROUP_3,
	TEXT_RANK_GROUP_4,
	TEXT_RANK_GROUP_5,
	TEXT_PVP_ROOM_1,
	TEXT_PVP_ROOM_2,
	TEXT_PVP_ROOM_3,
	TEXT_PVP_ROOM_4,
	TEXT_PVP_ROOM_5,
	TEXT_PVP_ROOM_ACCEPT_CONFIRM,	//二次确认对战邀请
	TEXT_PVP_ROOM_REJECT_CONFIRM,	//二次确认对战拒绝
	TEXT_PVP_ROOM_6,
	TEXT_PVP_ROOM_7,
	TEXT_PVP_ROOM_8,
	TEXT_PVP_ROOM_9,
	TEXT_PVP_ROOM_10,
	TEXT_PVP_ROOM_11,
	TEXT_PVP_ROOM_12,
	TEXT_PVP_NOITEM_TIME,//无道具时间未到
	TEXT_PVP_BTN_TEXT_1, //匹配按钮文字
	TEXT_PVP_BTN_TEXT_2,//房间按钮文字
	TEXT_PVP_BTN_TEXT_3,//无道具按钮文字
	TEXT_PVP_LIMIT_ROUND10,
	TEXT_PVP_PROTECT_ROUND10,
	TEXT_PVP_ROOM_13 = 252,
	TEXT_PVP_TIPS_1 = 1051,
	TEXT_PVP_TIPS_2,
	TEXT_PVP_TIPS_3,
	TEXT_PVP_TIPS_4,
	TEXT_PVP_TIPS_5,
	TEXT_PVP_TIPS_6 = 3002,
};

enum eRenameTips
{
	TEXT_RENAME_BEGIN = TEXT_RENAME,
	TEXT_RENAME_SPEND,//改名字花费
	TEXT_RENAME_FAILED,//钻石不足
	TEXT_RENAME_EXIST,//重名
	TEXT_NOT_CHANGE,//名字没改变
	TEXT_RENAME_LENGHT,//名字长度提示
	TEXT_RENAME_FREE,//改名字免费
	TEXT_RENAME_ERROR,//名字存在非法字符
	TEXT_NAMEING_TIP1,
	TEXT_NAMEING_TIP2,
};

enum eBTHTips
{
	TEXT_BTH_BEGIN = TEXT_BTH,
	TEXT_BTH_MISSION,//解锁通关提示
	TEXT_BTH_STAR,//星星不足解锁关卡提示
	TEXT_BTH_TITLE_BASE = 22104,	//通关奖励标题
	TEXT_BTH_TITLE_PERFECT = 22105,	//满星奖励标题
	TEXT_BTH_BEYOUD = 22108,		//超越全服%d%%玩家
	TEXT_BTH_BEYOUD_FIFTH,			//分数超越第%d名
	TEXT_BTH_TOP,					//全服最高纪录
	TEXT_BTH_TOP_CONTENT,			//英勇神武的%s在美食冒险第%d章第%d关刷新了最高分数，大家为他欢呼！！！
	TEXT_BTH_TITLE_SKIP = 22112,    //跳关次数不够
	TEXT_BTH_SKIP_SUCCESS = 22113   //跳关成功
};

enum eSpecialTips
{
	TEXT_WINNER_SUCCESS = 841,//领取连胜奖励成功
	TEXT_WINNER_FAIL,//领取连胜失败
	TEXT_WINNER_CONFIRM,
	TEXT_WINNER_ACTTIPS = 846 ,//活动未开启

	TEXT_ACTTIPS_HOPE = 847,  //活动暂未开放，敬请期待！
	TEXT_WINNER_CONTENT_1 = 848,  //连胜30场以下描述1
	TEXT_WINNER_CONTENT_2 = 849,  //连胜30场以上描述
	TEXT_WINNER_COMPLENT = 850,//今日任务完成
	TEXT_WINNER_NOCOMPLENT = 851,//今日任务未完成

	TEXT_POWER_FULL_TIPS = 881,//体力满满
	TEXT_POWER_NOT_FULL = 882,//体力不满
	TEXT_ENERGY_FULL_TIPS = 883,//精力满满
	TEXT_ENERGY_NOT_FULL = 884, //精力不满

	TEXT_CHAT_NULL = 7002,
	TEXT_CHAT_LEN,
	TEXT_CHAT_INVITE,
	TEXT_CHAT_LIMIT,
	TEXT_CHAT_EXPIRED,
	TEXT_CHAT_LEVEL,
	TEXT_ROOM_LEVEL,//进入房间限制
	TEXT_ROOM_LEVEL1,//进入房间限制
	TEXT_CHAT_FREE,
	TEXT_CHAT_COST,
	TEXT_CHAT_FAIL,
};

enum eBuyItemTips
{
	TEXT_BUY_ITEM_TILI = TEXT_BUY_ITEM,
	TEXT_BUY_ITEM_TILI_SURE,
	TEXT_BUY_ITEM__JINGLI,
	TEXT_BUY_ITEM_JINGLI_SURE,
	TEXT_BUY_ITEM_NO_EN,
	TEXT_BUY_ITEM_CLOSE_PVP,
	TEXT_BUY_ITEM_SUCCESS,
	TEXT_BUY_PVP_DEAD,
	TEXT_BUY_POWER_MAX,
	TEXT_BUY_DIAMOND_COUNT,  //%d钻石
	TEXT_BUY_ITEM_TITLE,//温馨提示
	TEXT_BUY_ITEM_USE,//使用说明
	TEXT_BUY_NEWERGIFT_TITLE,	//购买新手礼包标题
	TEXT_BUY_NEWERGIFT_SURE,	//购买新手礼包内容
	TEXT_BUY_ITEM_QIUCK_CLOSE_PVP,	//两分钟内离开pvp
	TEXT_EXIT_PLAYBACK,			//确定退出观看战斗回放吗
};

//战斗提示
enum eBattleTips
{
	TEXT_BATTLE_POPULATION = TEXT_BATTLE,	//兵力上限提升
	TEXT_BATTLE_SPEED_PRODUCE,				//产兵速度提升
	TEXT_BATTLE_DEFENSE,					//防御提升
	TEXT_BATTLE_SPEED_ATTACK,				//攻击速度提升
	TEXT_BATTLE_ATTACK_ALL,					//全体攻击提升
	TEXT_BATTLE_DEFENSE_ALL,				//全体防御提升
	TEXT_BATTLE_SPEED_MOVE,					//移动速度提升
	TEXT_BATTLE_PEOPLE_FILL,				//兵力全满
};
 
enum eBuffTips
{
	TEXT_BUFF_DESC = TEXT_BUFF,
	TEXT_BUFF_LAJIAO,
	TEXT_BUFF_DACONT,
	TEXT_BUFF_JIDAN,
	TEXT_BUFF_NAILAO,
	TEXT_BUFF_SUANTOU,
	TEXT_BUFF_XIAMI, 
	TEXT_BUFF_ADDATTACK,
	TEXT_BUFF_ADDDEFENCE,
	TEXT_BUFF_FILLPEOPLE,
	TEXT_BUFF_ATTACKSPEED,
	TEXT_BUFF_PRODUCTSPEED,
	TEXT_BUFF_MOVESPEED, 
};

enum eGetSaveDataTips
{
	TEXT_GettingSaveData = TEXT_SAVEDATA,
	TEXT_GetSaveDateOver,
	TEXT_AskGetSaveData,
	TEXT_CancelGetDesc,
	TEXT_EnterGameDesc,
};
//好友面板
enum eFriendText
{
	TEXT_FRIEND_MAX = TEXT_FRIEND,			//好友人数已满
	TEXT_FRIEND_IS_FRIEND,					//你们已经是好基友了
	TEXT_FRIEND_FIND_NONE,					//未找到玩家
	TEXT_FRIEND_DELETE,						//删除确认
	TEXT_FRIEND_RECENTLY,					//最近
	TEXT_FRIEND_HOUR,						//%d小时前
	TEXT_FRIEND_DAY,						//%d天前
	TEXT_FRIEND_MONTH,						//%d个月前
	TEXT_FRIEND_LONGTIME,					//很久很久以前
	TEXT_FRIEND_CHANGE_HEAD,				//更换头像
	TEXT_FRIEND_ONLINE,						//在线
};


// 排行榜
enum eRankingText
{
	TEXT_RANKING_START = TEXT_RANKING,
	TEXT_RANKING_GROUP_1,					// 学徒
	TEXT_RANKING_GROUP_2,					// 主厨
	TEXT_RANKING_GROUP_3,					// 御厨
	TEXT_RANKING_GROUP_4,					// 厨神
	TEXT_RANKING_RANKING,					// 排名
	TEXT_RANKING_PLAYER,					// 玩家
	TEXT_RANKING_WINRATE,					// 胜率
	TEXT_RANKING_POINT,						// 积分
	TEXT_RANKING_ROUNDCOUNT,				// 局数
	TEXT_RANKING_RESULT_WIN,				// 最近在和%s的比赛中获胜
	TEXT_RANKING_RESULT_LOST,				// 最近在和%s的比赛中失败
	TEXT_RANKING_GAP,
	TEXT_RANKING_RESULT_GAINPOINT,			// 收获%d积分
	TEXT_RANKING_RESULT_LOSTPOINT,			// 扣除%d积分
	TEXT_RANKING_RESULT_GAINSCORE,			// 获得%d荣誉
	TEXT_RANKING_SCORE,						// 荣誉
};


// 徽章
enum eBadgeText
{
	TEXT_BADGE_START = TEXT_BADGE,
	TEXT_BADGE_OBTAINTIME,					// XX年XX月XX日获得
	TEXT_BADGE_NOTOBTAIN,					// 未获得
	TEXT_BADGE_NOTACTIVE,					// 未开启
	TEXT_BADGE_DRESSSUCCESS,				// 佩戴成功
};

//激活码
enum eActivationText
{
	TEXT_ACTIVATION_NOTEXIT = 2009,				//激活码不存在,
	TEXT_ACTIVATION_TYPE_NOTEXIT,				//激活码类型不存在,
	TEXT_ACTIVATION_USED,						//激活码已经被使用了,
	TEXT_ACTIVATION_PARAM_ERROR,				//参数错误,
	TEXT_ACTIVATION_GIFTTYPE_USED,				//礼包类型已经使用过了,
	TEXT_ACTIVATION_UNINPUT,					// 未输入激活码,
	TEXT_ACTIVATION_INPUT_NOTENOUGH,			//输入激活码长度不够,
	TEXT_ACTIVATION_EMAIL_SUCCESS,				//兑换成功，请打开邮件领取奖励,
};

//页面类型
enum ePageType
{
	MSG_FUC_BEGIN = 0,

	MSG_FUC_SCENCE_BEGIN,
	MSG_FUC_LOGO,
	MSG_FUC_UPDATE,		//更新
	MSG_FUC_LOGIN,
	MSG_FUC_MAIN_LEVEL,		//大关卡选择
	MSG_FUC_MINI_LEVEL,		//小关卡选择
	MSG_FUC_STORE,			//商店
	MSG_FUC_PLAYING,		//战斗中
	MSG_FUC_SCENCE_END,
	//
	MSG_FUC_WIN_FAIL = 110,		//失败弹框
	MSG_FUC_WIN_RESULT = 111,		//结算选择
	MSG_FUC_PVP_RESULT = 112, //pvp结算
	MSG_FUC_WIN_LEVEL = 113,		//关卡选择
	MSG_FUC_WIN_PROPNOSET = 114,		//未选择道具提示

	MSG_FUC_WIN_PAUSE = 115,		//暂停
	MSG_FUC_WIN_GENERL = 116,		//通用
	MSG_FUC_WIN_SHOP = 117,		//商店
	MSG_FUC_WIN_SET = 118,		//设置
	MSG_FUC_WIN_PVP = 119,		//pvp
	MSG_FUC_WIN_PVP_WATCH = 120,		//pvp watch
	MSG_FUC_WIN_PVP_ROOM_BACK =121,		//pvp ROOM_BACK
	MSG_FUC_ACHIEVE =122,    //成就
	MSG_FUC_WINNER = 123,    //王者之路	
	MSG_FUC_WIN_PRO = 124,	//道具详情	
	MSG_FUC_ACT = 125,           //活动
	MSG_FUC_ACT_LOGIN = 126,   //签到提示
	MSG_FUC_RANKING = 127,	// 排行榜
	MSG_FUC_GIFTDIALOG = 128, //礼包弹框
	MSG_FUC_FAIL_PROMPT = 129, // pvp失败提示框

	MSG_FUC_SET_NAME = 130,//设置名字
	MSG_FUC_CONNECTING = 131, // 连网中节点
	MSG_FUC_BACKPACK = 132, //背了个包
	MSG_FUC_DAYTASK = 133, // 每日任务
	MSG_FUC_POWER_GIFT = 134, //体力大放送
	MSG_FUC_NEWER_GIFT = 135, //新手礼包 

	MSG_FUC_CHATMSG = 136,//聊天信息面板
	MSG_FUC_FRIEND = 137,		//好友界面
	MSG_FUC_SET_HEAD = 138,	//更换头像
	MSG_FUC_PVPROOM = 139,	//PVP开房
	MSG_FUC_SAVEDATA = 140,   //拉取存档

	MSG_FUC_NEWER_BIBLE = 141,//新手宝典
	MSG_FUC_PVP_MATCH_LOADING = 142, // PVP匹配中

	MSG_FUC_KICKPLAYER = 143,
	MSG_FUC_CRAZYWEEK = 144, //周末狂欢
	MSG_FUC_HONOREXCHANGE = 145, // 荣誉点兑换
	MSG_FUC_FIVE_STAR = 146,//五星好评
	MSG_FUC_CUSTOMER_SERVER = 147,//客服中心
	MSG_FUC_INVITECODE = 148,//邀请码

	MSG_FUC_MAIL = 149, //邮件

	MSG_FUC_GW_SELECTAREA = 150,	//争霸赛选区
	MSG_FUC_GW_AUDITION = 151,	//争霸赛海选
	MSG_FUC_GW_FINAL = 152,		//争霸赛食神

	MSG_FUC_ACTINFORM = 153, //活动公告

	MSG_FUC_VIP = 154,	//VIP
	MSG_FUC_VIP_TIME = 155,	//VIP提醒
	MSG_FUC_PLAYER_UPGRADE = 156,//玩家升级
	MSG_FUC_GLYPH = 157, //符文
	MSG_FUC_RANKSESSION_UPGRADE = 158, //PVP结算段位升级


	MSG_FUC_FREEDIAMOND = 159,//�^看影片

	MSG_FUC_REDPACKET = 160,//主界面红包按钮
	MSG_FUC_CHRISTMAS = 161,// 圣诞活动
	MSG_FUC_RELOGIN = 162,//重连
	MSG_FUC_CHAGE_SERVER = 164, //服务器列表
	MSG_FUC_FIRST_CHARGE = 165,  // 首冲礼包
	MSG_FUC_BADGE = 166,  // 徽章
	MSG_FUC_TOTAL_CHARGE = 167,  //累计冲

	MSG_FUC_INFO = 168,//个人信息面板
	MSG_FUC_PVP_HELP = 169,

	MSG_FUC_LUCKY_MONEY = 170, // 抢红包
	MSG_FUC_VALENTINE =171,//主界面情人节按钮	
	MSG_FUC_WEIXINSHARE =172,//微信分享
	MSG_FUC_SRVDOWN_NOTICE = 173,
	MSG_FUC_RESULT_BOX = 174, //宝箱打开后显示物品
	MSG_FUC_NAMEING_DIALOG = 175, //取名

	MSG_FUC_DAILY_MISSION = 176,//主界面每日任务
	MSG_FUC_INSTITUTE_FREE = 178, // 研究院提示
	MSG_FUC_END,
	
	//add by weihua luawindow message
	MSG_FUC_LUA_WIN  = 1000,
	MSG_FUC_TOP_RANK = 10000,

};
//新手引导相关文字key
enum eTextTagGuide
{
	TEXT_GUIDE_0_0 = 300, //第一个数字表示第几组任务，第二个数字表示该组的第几任务
	TEXT_GUIDE_0_1,
	TEXT_GUIDE_1_0,
};


enum eMusicType
{
	SOUND_SOLDIER_OUT = -200,		//建筑出兵指令
	SOUND_BUILD_LEVELUP = -199,		//建筑升级
	SOUND_BUILD_CHANGE_CAMP = -198,	//建筑更改阵营
	SOUND_BUILD_ATTACK = -197,		//炮塔攻击
	SOUND_BUILD_SOLDIER_DIE = -196,	//炮塔击中小兵死亡
	SOUND_SOLDIER_ATTACK = -195,	//小兵围攻建筑
	SOUND_BUILD_CHANGE_TYPE = -194,	//建筑更换类型
	SOUND_CLICK_BACK = -193,		//返回按钮
	//SOUND_CLICK_BIG_GATE = -194,	//点击大关
	//SOUND_LICK_SMALL_GATE = -193,	//点击小关
	SOUND_CLICK_BATTLE = -192,		//点击进入(战斗)
	SOUND_CLICK_COMMON = -191,		//按钮按下, 通用
	SOUND_UI_WIN = -190,			//关卡胜利
	SOUND_UI_FAIL = -189,			//关卡失败
	SOUND_SOLDIER_WALK_1 = -188,	//小兵出发1
	SOUND_SOLDIER_WALK_2 = -187,	//小兵出发2
	SOUND_SOLDIER_WALK_3 = -186,	//小兵出发3
	SOUND_READY = -185,				//read go!
	SOUND_ITEM_GAIN = -184,		//获得道具
	SOUND_SOLDIER_STABLES_WALK_1 = -183,	//马厩小兵出发 1
	SOUND_SOLDIER_STABLES_WALK_2 = -182,	//马厩小兵出发 2
	SOUND_SOLDIER_STABLES_WALK_3 = -181,	//马厩小兵出发 3
	SOUND_PROP_FULLPEOP = -180,	//兵力包
	SOUND_PROP_ADDTIME = -179,		//时钟
	SOUND_PROP_INVISIBILITY = -178,	//隐身衣
	SOUND_PROP_POISON = -177,	//毒蘑菇
	SOUND_PROP_COOKING = -176,		//辣椒/萝卜炸弹
	SOUND_PROP_OIL = -175,		//地沟油
	SOUND_PROP_FREEZE = -174,		//冰弹
	MUSIC_UI_MAIN = -100,			//主界面
	MUSIC_UI_BIG_GATE = -99,		//大关卡界面
	MUSIC_UI_SMALL_GATE = -98,		//小关卡界面
	MUSIC_UI_START = -97,			//片头动画
	MUSIC_UI_UPDATE = -96,			//更新界面
	MUSIC_UI_MALL = -95,			//商城界面
	MUSIC_UI_PVPDIALOG = -94,		//pvp主界面
	MUSIC_UI_TEACH_DIALOG = -93,		//教学UI
	MUSIC_UI_PVPROOM = -92,		//开房UI
	MUSIC_GATE_1010 = 1010,			//关卡1010
	MUSIC_GATE_1020 = 1020,
	MUSIC_GATE_1030 = 1030,
	MUSIC_GATE_1040 = 1040,
	MUSIC_GATE_1050 = 1050,
	MUSIC_GATE_1051 = 1051,
	MUSIC_GATE_1060 = 1060,
	MUSIC_GATE_1070 = 1070,
	MUSIC_GATE_1080 = 1080,
	MUSIC_GATE_1090 = 1090,
	MUSIC_GATE_1100 = 1100,
	MUSIC_GATE_1101 = 1101,
	MUSIC_GATE_1110 = 1110,
	MUSIC_GATE_1120 = 1120,
	MUSIC_GATE_1130 = 1130,
	MUSIC_GATE_1140 = 1140,
	MUSIC_GATE_1150 = 1150,
	MUSIC_GATE_1151 = 1151,
	MUSIC_GATE_1152 = 1152,
	MUSIC_GATE_2010 = 2010,			//关卡2010
	MUSIC_GATE_2020 = 2020,
	MUSIC_GATE_2030 = 2030,
	MUSIC_GATE_2040 = 2040,
	MUSIC_GATE_2050 = 2050,
	MUSIC_GATE_2051 = 2051,
	MUSIC_GATE_2060 = 2060,
	MUSIC_GATE_2070 = 2070,
	MUSIC_GATE_2080 = 2080,
	MUSIC_GATE_2090 = 2090,
	MUSIC_GATE_2100 = 2100,
	MUSIC_GATE_2101 = 2101,
	MUSIC_GATE_2102 = 2102,
	MUSIC_GATE_2110 = 2110,
	MUSIC_GATE_2120 = 2120,
	MUSIC_GATE_2130 = 2130,
	MUSIC_GATE_2140 = 2140,
	MUSIC_GATE_2150 = 2150,
	MUSIC_GATE_2151 = 2151,
	MUSIC_GATE_2152 = 2152,
	MUSIC_PVPGATE_10201 = 10201,
	MUSIC_PVPGATE_10202 = 10202,
	MUSIC_PVPGATE_10203 = 10203,
	MUSIC_PVPGATE_10204 = 10204,
	MUSIC_PVPGATE_10301 = 10301,
	MUSIC_PVPGATE_10303 = 10303,
	MUSIC_PVPGATE_10401 = 10401,
	MUSIC_PVPGATE_10402 = 10402,
	MUSIC_PVPGATE_10403 = 10403,
	MUSIC_PVPGATE_10404 = 10404,
	MUSIC_PVPGATE_10406 = 10406,

};

enum ePVPReultText
{
	TEXT_PVPRESULT_BEGIN = 14000,
	TEXT_PVPRESULT_ADDGLORY,		 //PVP结算获得荣誉
	TEXT_PVPRESULT_ADDSCORE,		 //PVP结算获得积分
	TEXT_PVPRESULT_SUBSCORE,		 //PVP结算获得负积分
	TEXT_PVPRESULT_OPNER_EXTRAAWARD, //PVP结算房主胜利获得额外积分
	TEXT_PVPRESULT_OPNER_AWARD,		 //PVP结算房主失败奖励描述
	TEXT_PVPRESULT_NORANK,			 //PVP结算未上榜
	TEXT_PVPRESULT_ADDHONOR = 14009, //pvp结算额外添加荣誉点
	TEXT_PVPRESULT_VIPADD = 14012,	 //VIP加成奖励,
	TEXT_PVPRESULT_STREAKADD = 14013,  //连胜,
};

enum eCrazyWeekText
{
	TEXT_CRAZYWEEK_DONE = 14008,     // 已完成
};

enum eHonorShopText
{
	TEXT_HONORSHOP_LIMIT = 14010,   // 限量
	TEXT_HONORSHOP_FRESH,			// n天刷新
	TEXT_HONORSHOP_NOTENOUGH = 15008, // 荣誉不足
	TEXT_HONORSHOP_DAYS = 22202,   // %d天
	TEXT_HONORSHOP_HOURS = 22203,  // %d小时
	TEXT_HONORSHOP_MINUTE = 22204, //%02d:%02d
};

enum eActNoticeText
{
	TEXT_FIRST_CHARGE = 19004,    // 首冲礼包
	TEXT_TOTAL_CHARGE,
};

enum eMPlusHelpText
{
	TEXT_MPLUSHELP_BEGIN = 18000,
	TEXT_MPLUSHELP_SHARING,
	TEXT_MPLUSHELP_INPUT5INVITCODE,
	TEXT_MPLUSHELP_INVITED,
	TEXT_MPLUSHELP_INPUTINVITCODE_ERROR,
	TEXT_MPLUSHELP_INPUTINVITCODE_SELF,
	TEXT_MPLUSHELP_INPUTINVITCODE_NOTEXSIT,
	TEXT_MPLUSHELP_INPUTINVITCODE_AGAIN,
	TEXT_MPLUSHELP_FIVESTAR_AGAIN,
	TEXT_MPLUSHELP_WATCH_MOVIE,
	TEXT_MPLUSHELP_AWARD_MAIL,
	TEXT_MPLUSHELP_PLEASE_BINDFB,
	TEXT_MPLUSHELP_FREE_DIAMONDS,
	TEXT_MPLUSHELP_FREE_MOIVE,
};

enum eShopText
{
	TEXT_SHOP_LEFTCOUNT = TEXT_SHOP + 1,
	TEXT_SHOP_LEFTTIME_DAY,
	TEXT_SHOP_LEFTTIME_HOUR,
	TEXT_SHOP_BUY_CONFIRM,
};

// 体力恢复时间节点
enum eRecoveryTime
{
	ERT_Zero = 0,
	ERT_Twelve,
	ERT_Max,
};

// pvp匹配类型
enum ePvpMatchType
{
	EPT_Match_Two = 0,		// 2人对战
	EPT_Match_Three,		// 3人对战
	EPT_Match_Four,			// 4人对战
};

// 排行榜类型
enum eRankingGroupType
{
	RANKINGGROUP_DEFAULT = 0,
	RANKINGGROUP_BRONZE = 1,
	RANKINGGROUP_SILVER = 2,
	RANKINGGROUP_GOLD = 3,
	RANKINGGROUP_DIAMOND = 4,
	RANKINGGROUP_KING = 5,
};

//Buff类型
enum eBuffType
{
	E_BUFF_NONE = 0,
	E_Buff_MoveSpeed = 1,	//移动速度buff
	E_Buff_AttackSpeed,		//攻击速度buff
	E_Buff_Defence,			//防御力buff
	E_Buff_FillPeople,		//满兵buff
	E_Buff_ProduceSpeed,	//产兵速度buff
	E_Buff_Attack,
}; 


enum eCheckPointState
{
	E_CHECKPOINTSTATE_CLOSE = 0,
	E_CHECKPOINTSTATE_OPEN = 1,
	E_CHECKPOINTSTATE_PASS = 2,
};

enum eBuyDiamondAwardType
{
	E_AWARD_MONTH = 1,
	E_AWARD_PVEPOWER_LIMIT,
	E_AWARD_FLAG,
	E_AWARD_PVPPOWER_LIMIT,
	E_AWARD_DIAMOND,
};
enum eCheckPointType
{
	E_CHECKPOINT_ALL = 0,
	E_CHECKPOINT_MAIN,
	E_CHECKPOINT_BRANCH,
};

enum eLevelType
{
	E_Level_Main = 0,		//主线
	E_Level_Branch = 1,		//直线
	E_Level_Boss =2,		//BOSS
	E_Level_Trans = 3,		//训练营关卡
};

enum eWorldType
{
	E_WORLD_1 =1,
	E_WORLD_2,
	E_WORLD_3
};

// 
enum ePaySdkType
{
	E_PAY_JIDI = 0,
	E_PAY_DIANXING,
	E_PAY_ONLING,
};

enum ePropPos//道具位置
{
	E_POS_LEFT = 1,
	E_POS_MID, 
	E_POS_RIGHT,
	E_POS_NONE
};
//////////////////////////////////////////////////////////
enum eGuideMessageType
{
	E_GUIDEMESSAGE_START,
	E_GUIDEMESSAGE_OVER,
};

enum eGuideOperatorType
{
	E_GuideOperator_None = -1,
	E_GuideOperator_Click = 0,
	E_GuideOperator_Drag = 1,
	E_GuideOperator_DouClick = 2, 

};
 

enum eGuideLocationType
{
	E_GUIDELOC_BEGIN = 0,
	E_GUIDELOC_GATE = 1,					//关卡
	E_GUIDELOC_GAMEPAGE = 2,				//战斗UI
	E_GUIDELOC_SHOP = 3,					//商店
	E_GUIDELOC_JUNGLE = 4,					//野怪出现通知battleView
	E_GUIDELOC_MAINUI = 5,					//主界面
	E_GUIDELOC_SELECTGATE = 6,				//选关界面
	E_GUIDELOC_BATTLESUCCESS = 7,			//战斗胜利对话框界面
	E_GUIDELOC_SIGNIN = 8,					//签到
	E_GUIDELOC_PVPUI = 9,					//pvp界面
	E_GUIDELOC_LEVELDIALOG = 10,			//选择战斗界面框
	E_GUIDELOC_OCU1 = 11,					//占领建筑通知gamePage引导
	E_GUIDELOC_LEVELUP = 12,				//升级行为
	E_GUIDELOC_GLYPH = 13,					//符文
	E_GUIDELOC_SOLDIERNUM = 14,				//兵力到达一定值触发
	E_GUIDELOC_E_ATK_P1 = 15,				//battleView玩家接收敌人进攻
	E_GUIDELOC_E_ATK_P2 = 16,				//gamepage玩家接收敌人进攻
	E_GUIDELOC_LEVELUP2 = 17,				//可升级时触发
	E_GUIDELOC_E_ATKED_N = 18,				//中立建筑受到攻击 (强调攻击之后)
	E_GUIDELOC_P_ATK_N = 19,				//中立建筑收到玩家攻击行为 Player attack Neutral (强调攻击前)
	E_GUIDELOC_E_ATK_N = 20,				//中立建筑收到敌人攻击行为 Enemy attack Neutral (强调攻击前) 
	E_GUIDELOC_P_ASS_P = 21,				//玩家发兵支援自己建筑 Player assist Player (强调进入前) 
	E_GUIDELOC_P_ATK_E = 22,				//敌人建筑收到玩家攻击行为 Player attack Enemy  (强调攻击前)  
	E_GUIDELOC_OCU2 = 23,					//占领建筑通知battleView引导
	E_GUIDELOC_JUNGLE2 = 24,				//野怪出现通知gamePage
	E_GUIDELOC_OCU_JUNGLE = 25,				//野怪被占领
	E_GUIDELOC_USEPROP = 26,				//使用道具
	E_GUIDELOC_RENAMEFINISH = 27,           //命名完成触发
	E_GUIDELOC_INSTITUTE_ENTER= 28,			//研究院进入界面
	E_GUIDELOC_INSTITUTE_GETCARD = 29,	    //研究院抽卡完成
	E_GUIDELOC_GENERAL_ENTER = 30,          //美食将军进入页面
	E_GUIDELOC_GENERAL_EQUIP = 31,			//美食将军装备精灵
	E_GUIDELOC_END = 999,					//引导结束
};

enum eGuideFunctionType
{
	E_GuideFuc_Tips = 1,				//战斗提示按钮
	E_GuideFuc_Drag = 2,				//战斗拖曳建筑
	E_GuideFuc_AddMoney = 3,			//加钻石请求
	E_GuideFuc_TabShop = 4,				//切换商城标签
	E_GuideFuc_CloseShopDl = 5,			//关闭购买页面detail
	E_GuideFuc_EnterShopMn = 6,			//进入购买页面main
	E_GuideFuc_AddMoneyAni = 7,			//增加钻石的动画
	E_GuideFuc_AddGlyphStuff = 8,		//增加材料的动画 
	E_GuideFuc_UseProp      = 9,		//使用道具 
	E_GuideFuc_BV2GP = 10,				//battleView通知gamepage
	E_GuideFuc_GP2BV = 11,				//gamepage通知battleView
	E_GuideFuc_AddGift = 12,			//礼包动画
	E_GuideFuc_GotoMain = 13,			//进入主页面
	E_GuideFuc_OpenUI = 14,				//打开UI界面
	E_GuideFuc_OpenLuaUI = 15,			//打开lua的UI界面
};

enum eGuideNodeTag
{
	E_GuideTag_ShopBuyBtn = 832,		//商城购买按钮
	E_GuideTag_LevelStargBtn = 81,		//选关界面开始战斗按钮
	E_GuideTag_LevelPanel = 675,		//道具选择区域
	E_GuideTag_LevelPropOneBtn = 481,	//道具选择按钮1
	E_GuideTag_LevelPropOKBtn = 482,	//道具确认按钮2
	E_GuideTag_ShopPanelBuyBtn = 144,   //商城购买按钮
	E_GuideTag_ShopPanelBackBtn = 293,  //商城返回按钮
}; 

//////////////////////////////////////////////////////////


enum eChattingMsgType
{
	MSG_TYPE_NULL,
	MSG_TYPE_WORLD = 1,
	MSG_TYPE_PVP = 2,
	MSG_TYPE_SYSTEM = 3,
};



//战斗信息提示类型
enum eBattleTip
{
	E_COMPLETE_UPGRADE_BARRACK,		//兵营升级完成
	E_COMPLETE_UPGRADE_TURRET,		//炮塔升级完成
	E_OCCUPY_BARRACK,				//占领兵营完成
	E_OCCUPY_TURRET,				//占领炮塔完成
	E_OCCUPY_INSTITUTE,				//占领研究所完成
	E_GETBUFF_ATTACKSPEED,			//攻击速度buff
	E_GETBUFF_PRODUCTSPEED,
	E_GETBUFF_ATTACK,
	E_GETBUFF_DEFENCE,
	E_GETBUFF_FILLPEOPLE,
	E_GETBUFF_MOVESPEED,

};

//好友列表类型
enum eFriendListType
{
	E_FRIEND_MINE_GET = 1,		//我的好友
	E_FRIEND_MINE_ADD,			//我的好友新增
	E_FRIEND_MINE_REMOVE,		//我的好友删除
	E_FRIEND_APPLIACTION,		//申请列表
	E_FRIEND_APPLIACTION_NONE,	//通知主界面好友申请
	E_FRIEND_APPLIACTION_HAVE,	//通知主界面好友申请
	E_FRIEND_BLACK,				//黑名单
	E_FRIEND_PK,				//约战列表
	E_FRIEND_PK_NONE,			//通知主界面约战申请
	E_FRIEND_PK_HAVE,			//通知主界面约战申请
	E_FRIEND_SEARCH,			//搜索列表
	E_FRIEND_ADD_REQUEST_DONE,	//申请发送
	E_FRIEND_FIND_NONE,			//搜索为空
	E_FRIEND_AGREE_PK,			//同意约战
	E_FRIEND_INFO_PANLE,		//打开详细面板
};

enum ePowerGiftType
{
	TEXT_POWER_NULL = TEXT_POWER_GIFT,
	TEXT_POWER_OFF,
	TEXT_ENERGY_OFF,
	TEXT_POWER_TITLE,
	TEXT_ENERGY_TITLE,
	TEXT_POWER_TIPS,
	TEXT_ENERGY_TIPS,
	TEXT_POWER_GET,
};


enum eCurrencyType
{
	CNY = 0,
	HKD = 1,
	TWD = 2,
	USD = 3,
	EUR = 4,
	GBP = 5,
	JPY = 6,
};

enum eVituralCurrencyType
{
	Diamond = 0,
};

enum eItemOperation
{
	Buy = 0,
	Get = 1,
	Use = 2,
};

enum eItemType
{
	Defualt = 0,
};

enum AWARDTYPE
{
	A_GLYPHSTUFF	= 100000,//符文材料
	A_DIAMOND		= 20001, //钻石
	A_PVEPOWER,				 //体力
	A_PVPPOWER,				 //精力
	A_Honor,				 //荣誉
	A_ROOMTICKET,			 //开房券
};

enum ePveBattleRecord	//pve时玩家的操作等		与Message.h中的EMT_XX兼容
{
	PVE_RECORD_MOVE = 1,
	PVE_RECORD_UPGRADE = 5,
	PVE_RECORD_PROP_CHANGE = 18,
	PVE_RECORD_PROP_CHANGEOK = 19,
	PVE_RECORD_PROP_ADDTIME = 20,
	PVE_RECORD_PROP_FULLPEOP = 21,
	PVE_RECORD_PROP_FREEZE = 22,
	PVE_RECORD_PROP_COOKING = 23,
	PVE_RECORD_CREEPS_BORN = 24,
	PVE_RECORD_OUT_PERCENT = 25,
	PVE_RECORD_TURRET_UPDATE = 26,
}; 


enum eSoldierType
{
	Soldier_Normal, //普通小兵
	Soldier_Speed,  //加速小兵
};


enum TitleType {
	INVALID = 0,
	WINNER = 1,
	BRAVE = 2,
	FIGHTCRAZY = 3,
	MAX,
};

enum eCsvLoadState
{
	LS_LOADING,
	LS_COMPLETE,	// 单文件加载完成
	LS_OVER,		// 加载完成
};

//智能玩法指引类型
enum eIntelligentType
{
	EIT_NONE = -1,//有红点但是没有智能指引
	EIT_GIFT = 1,
	//
	EIT_WINNER = 3,
	EIT_POWER = 4,
	EIT_ACHIEVE = 5,
	EIT_FRIEND = 6,
	EIT_GLYPH = 7,
	EIT_DAYTASK = 8,
	EIT_NEWYEAR = 9,
	EIT_CRAZYWEEK = 10,
};

//个人信息面板
enum ePersonalInfo
{
	EPI_RANK = 22000,
	EPI_VIP_START = 22000,
	EPI_VIP_ONE = 22001,
	EPI_VIP_TWO = 22002,
	EPI_VIP_THREE = 22003,
	EPI_CHANG = 22004,
	EPI_CI = 22005,
	EPI_UNVISIBLE = 22006,
	EPI_EXTRA_UNLOCK = 22007,
	EPI_EXTRA_LOCK = 22008,
	EPI_SIGN_ERROR_ONE = 22009,
	EPI_SIGN_ERROR_TWO = 22010,
};

//时间格式文本
enum eTextTimeFormat
{
	TEXT_TIME_FORMAT1 = 23000,
	TEXT_TIME_FORMAT2 = 23001,
	TEXT_TIME_FORMAT3 = 23002,
	TEXT_TIME_FORMAT4 = 23003,
};

//排行榜奖励文本
enum eTextRankAward
{
	TEXT_RANK_PVP_COUNT  = 23100,
	TEXT_RANK_NEED_SCORE = 23101,
	TEXT_RANK_BOX		 = 23102,
	TEXT_NO_RANK		 = 23103,
};

//七日目标系统文本
enum eTextSevenGoal
{
	TEXT_SEVEN_GOAL_NOW_MISSION = 23201,
	TEXT_SEVEN_GOAL_NEXT_MISSION = 23202,
	TEXT_SEVEN_GOAL_NOW_GET = 23203,
	TEXT_SEVEN_GOAL_NEXT_GET = 23204,
};

enum eTextInstitue
{
	TEXT_INSTITUTE_1 = 30040,
	TEXT_INSTITUTE_2 = 30041,
	TEXT_INSTITUTE_DES11 = 30042,
	TEXT_INSTITUTE_DES12 = 30043,
	TEXT_INSTITUTE_DES21 = 30044,
	TEXT_INSTITUTE_DES22 = 30045,
};

//巅峰排行文本
enum eTextTopRank
{
	TEXT_TOP_RANK_RULE1 = 30100,
	TEXT_TOP_RANK_RULE2 = 30101,
	TEXT_TOP_RANK_RULE3 = 30102,
	TEXT_TOP_RANK_ACTIVITY_TIME = 30103,
	TEXT_TOP_RANK_UPDATE_TIME = 30104,
	TEXT_TOP_RANK_GET_STAR = 30105,
	TEXT_TOP_RANK_GET_WIN = 30106,
	TEXT_TOP_RANK_GET_WINSTREAK = 30107,
	TEXT_TOP_RANK_TEXT_STAR = 30108,
	TEXT_TOP_RANK_TEXT_WIN = 30109,
	TEXT_TOP_RANK_TEXT_WINSTREAK = 30110,
	TEXT_TOP_RANK_ASSISTANT_CARD = 30111,
};

enum eNewCampLevel
{
	E_NEWCAMP_0 = 99000,
	E_NEWCAMP_1 = 99010,
	E_NEWCAMP_2 = 99020,
	E_NEWCAMP_3 = 99030,
	E_NEWCAMP_4 = 99040,
	E_NEWCAMP_5 = 99050,
	E_NEWCAMP_6 = 99060,
};

//lua中的GetTimeRemaining函数使用
enum eTimeRemainingMode
{
	E_NORMAL = 1, //正常
	E_WITH_NEXT = 2, //若是超过目标时间点，又在本周内，则计算到下周的目标时间点时间
};

/* 改变属性时，同时更新脏标记
* @state : 该属性对应的脏标记位
*/
#define CC_SYNTHESIZE_DIRTY(varType, varName, funName, state)\
protected: varType varName; \
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var){ varName = var; mDirtyTag.set(state); }

/* 3.0 uireader
*/
#define DEFINE_PAGE_READER(_page_name)\
class _page_name##Reader : public cocostudio::NodeReader \
{ \
public:\
	static cocostudio::NodeReader* getInstance(){ return Singleton<_page_name##Reader>::Get(); }\
	static void purge(){ Singleton<_page_name##Reader>::Free(); }\
	cocos2d::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)\
{\
	_page_name* node = _page_name::Get(); \
if (node)\
{\
	setPropsWithFlatBuffers(node, nodeOptions); \
	dynamic_cast<PageBase*>(node)->initUi(); \
	return node; \
	}\
	return NULL; \
	}\
	};

#define REGIST_PAGE_READER(_page_name)\
	Singleton<_page_name##Reader>::Get()->registReaderObject(#_page_name, (ObjectFactory::Instance)_page_name::getInstance);
