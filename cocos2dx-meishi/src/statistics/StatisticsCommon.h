#pragma once

enum eLostPointStep
{
	None = 0,
	FirstLogin = 1,
	FirstSelectCheckPoint = 2,
	FirstGuideOver = 3,

	// 关卡更改：用关卡ID*2,表示开始关卡; 用关卡ID*2+1,表示关卡结束
/*
	eCompleteCheckPoint1 = 4,

	eEnterCheckPoint2 = 5,
	eCompleteCheckPoint2 = 6,

	eCompleteCheckPoint3 = 7,
	eCompleteCheckPoint4 = 8,
	eCompleteCheckPoint5 = 9,
	eCompleteCheckPoint6 = 10,
	eCompleteCheckPoint7 = 11,
	eCompleteCheckPoint8 = 12,
	eCompleteCheckPoint9 = 13,
	eCompleteCheckPoint10 = 14,
	*/
};


#define STATISTICS_CURRENCY_NAME_CNY		"CNY"
#define STATISTICS_CURRENCY_NAME_HKD		"HKD"
#define STATISTICS_CURRENCY_NAME_TWD		"TWD"
#define STATISTICS_CURRENCY_NAME_USD		"USD"
#define STATISTICS_CURRENCY_NAME_EUR		"EUR"
#define STATISTICS_CURRENCY_NAME_GBP		"GBP"
#define STATISTICS_CURRENCY_NAME_JPY		"JPY"


#define STATISTICS_VITURALCURRENCY_NAME_DIAMOND					"Diamond"


#define STATISTICS_ITEM_TYPE_DEFAULT							"Default"

#define STATISTICS_ITEM_CONSUMEPOINT_DEFAULT					"Default"




#define STATISTICS_LEVEL_ID									"CheckPoint_%d"

#define STATISTICS_EVENT_LEVEL_ID							"Event_ChekcPoint_%d"
#define STATISTICS_EVENT_LEVEL_KEY_UID						"Uid"
#define STATISTICS_EVENT_LEVEL_KEY_STARCOUNT				"StarCount"
#define STATISTICS_EVENT_LEVEL_KEY_RESULT					"Result"
#define STATISTICS_EVENT_LEVEL_KEY_PERFECT					"Perfect"


#define STATISTICS_EVENT_BOSS_ID							"Event_Boss_%d"
#define STATISTICS_EVENT_BOSS_KEY_UNLOCK					"Unlock"
#define STATISTICS_EVENT_BOSS_KEY_UID						"Uid"
#define STATISTICS_EVENT_BOSS_KEY_RESULT					"Result"


#define STATISTICS_EVENT_CHAPTER_ID							"Event_Chapter_%d"
#define STATISTICS_EVENT_CHAPTER_KEY_UNLOCK					"Unlock"
#define STATISTICS_EVENT_CHAPTER_KEY_UID					"Uid"


#define STATISTICS_EVENT_LOST_POINT_ID						"Event_Lost_Point"
#define STATISTICS_EVENT_LOST_POINT_KEY_STEP				"Step"


#define STATISTICS_EVENT_PVP_MATCHMODE_ID						"Event_PVP_Match_%d"
#define STATISTICS_EVENT_PVP_ROOMMODE_ID						"Event_PVP_Room_%d"
#define STATISTICS_EVENT_PVP_NOITEMMODE_ID						"Event_PVP_NoItem_%d"


#define STATISTICS_EVENT_OPEN_UI_ONCE_ID					"Event_OpenUI_%s"
#define STATISTICS_EVENT_OPEN_UI_ALL_ID						"Event_OpenUI_ALL"


#define STATISTICS_EVENT_LOGIN_FLOW_ID						"Event_LoginFlow_%s"
#define STATISTICS_EVENT_LOGIN_FLOW_KEY_RESULT				"Result"





