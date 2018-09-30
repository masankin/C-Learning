#pragma once

#include <map>

enum eStateTag
{
	EST_INVALID = 0,
	EST_Build_CanUpgrade,	// 建筑可升级时
	EST_Build_ProductRate,	// 建筑兵力达到生产上限的百分比
	EST_Camp_MorePeople,	// 己方总兵力达到N
	EST_Build_HoldRate,		// 建筑兵力达到容纳上限的百分比
	EST_Builds_BeAttN,		// 距离N格己方建筑受到攻击时
	EST_N_NeutralBuild,		// 距离N格存在有中立阵营时
	EST_N_MorePeople,		// 己方兵力超过敌方兵力N, 多方混战则超过最多的为N
	EST_N_Builds,			// 己方占领的建筑数量大于N
	EST_Battle_Other,		// 当非友方出兵攻打第三方时
	EST_D_Monster,			// 附近生成打野点
	EST_D_Builds,			// 距离若干格存在的敌方建筑大于N
	EST_D_MorePeople,		// 距离若干格存在的敌方建筑兵力大于N
	EST_D_LessPeople,		// 距离若干格存在的敌方建筑兵力小于N
	EST_NO_BeAttack,		// 所有己方建筑没有受到任何攻击
	EST_Build_PeopleNum,	// 当前建筑内的兵力达到指定值
	EST_PeopleCountInN,		// N格范围内存在兵力少于M的建筑
	EST_HaveXXbuffer,		// 拥有xxbuff
	EST_MoraleGtN,			// 己方士气大于M
	EST_OwnerNotDamage,		// 自身建筑没有收到伤害
	EST_OwnerCampPeople,	// 总兵力低于M
	EST_RunTransformCommand,//指定建筑发出救援指令
	EST_CountDownSurplus,	//剩余时间少于M秒
	EST_BuildSoldierNum,	//指定建筑兵力
	EST_EnemySubDstSoldierNum,	//敌方向本建筑派兵数与驻兵数的差距
	EST_EnemySubFriendSoldierNum, //敌方向友方建筑派兵数与驻兵数的差距
	EST_EnemySubOtherSoldierNum, //敌方向第三方派兵数与驻兵数的差距
	EST_EnemyBuildAfterSendNum, //敌方派兵之后的数量
	EST_EnemyBuildAfterReceiveNum, //敌方受到之后的数量
	EST_SelfBuildLevel, //自身建筑等级
	EST_BuildTypeNum,		//已经占领的数量
	EST_TimeCompareLeft=31,		//当前时间比较剩余时间
	EST_SoldierNumCompareALL=32,	//总兵力与总生产上限的百分比与M比较
	EST_SildierNumOrBuildNum=33,	//敌方总兵力和总建筑的数量
	EST_PlayerPropCount,			//任一玩家在战斗中所携带的道具数量
};	

enum eActionTag
{
	EAT_INVALID = 0,
	EAT_DO_NOTHING = 1,			// 按兵不动
	EAT_ALL_ATTACK = 2,			// 全军出动
	EAT_BUILD_UPGRADE = 3,		// 升级
	EAT_SHIFT1 = 4,				// 屯兵1
	EAT_SHIFT2 = 5,				// 屯兵2
	EAT_ATTACK1 = 6,			// 进攻1
	EAT_ATTACK2 = 7,			// 进攻2
	EAT_HELP1 = 8,				// 救援1
	EAT_HELP2 = 9,				//
	EAT_MONSTER_ATTACK1 = 10,			// 打野
	EAT_MONSTER_ATTACK2 = 11,
	EAT_ATTACK_RANGE = 12,			// 派出M个兵攻击N格攻击力最少者
	EAT_ATTACK_NEAREST = 13,		// 派出M个兵攻击最近的敌方建筑 
	EAT_ATTACK_RATE_RANGE = 14,			// 派出M%个兵攻击N格攻击力最少者
	EAT_ATTACK_RATE_NEAREST = 15,		// 派出M%个兵攻击最近的敌方建筑 
	EAT_PROP_SILDIER = 16,				//根据ID使用兵力包
	EAT_PROP_SILDIER_TYPE = 17,			//根据建筑类型最少兵建筑使用兵力包
	EAT_PROP_BOOM = 18,					//炸弹 目标点为敌方小兵最多的位置
	EAT_PROP_ICE = 19,					//冰冻	目标点为敌方小兵最多的位置
	EAT_PROP_OIL =20,					//地沟油	目标点为敌方小兵最多的位置

};

class BaseState;
class StateFactory;
class BaseAction;
class ActionFactory;

typedef std::map<eStateTag, BaseState*> StateList;
typedef std::map<eStateTag, StateFactory*> StateFactoryList;

typedef std::map<eActionTag, BaseAction*> ActionList;
typedef std::map<eActionTag, ActionFactory*> ActionFactoryList;
