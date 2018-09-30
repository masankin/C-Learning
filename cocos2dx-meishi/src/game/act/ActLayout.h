#pragma once
#include "cocos2d.h"
#include "net/proto/ClientSvrMsg.pb.h"

enum ACTTYPE
{
	ACT_NULL = -1,
	ACT_LOGIN,//签到
	ACT_FIRST_CHARGE = 1,//首充
	ACT_DAILY_CHARGE = 2,//每日充
	ACT_RECHARGE = 3,//累计充
//	ACT_MAIL = 4,//邮件(默认打开)
	ACT_NEWER_GIFT = 5,//新手礼包
	ACT_CRAZY_WEEK = 6, //疯狂周末
	ACT_CHRISTMAS  = 7, // 圣诞活动
	ACT_NEW_YEAR = 8,//元旦活动
	ACT_VALENTINE = 14,//情人节活动
	ACT_TOP_RANK_STAR = 16,	//巅峰排行，pve评星排行
	ACT_TOP_RANK_WIN = 17,	//巅峰排行，pvp胜利数量排行
	ACT_TOP_RANK_WINSTREAK = 18,	//巅峰排行，pvp连胜数量排行
};

class ActLayout :public cocos2d::Layer
{
public:
	ActLayout(void);
	~ActLayout(void);

	virtual void updateInfo(){}
	
	virtual void delCell(std::vector<int>){}

	virtual bool tableViewMove(bool isRigth){ return true; }
};

