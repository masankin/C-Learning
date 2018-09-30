#pragma once
#include "cocos2d.h"
#include "Constant.h"
#include "WinnerDialog.h"
#include "WinnerProxy.h"

enum WINNERTYPE
{
	WINNER_ROAD = 0,//王者之路
	WINNER_STAR,//王者之心
	WINNER_FIGHT,//斗争之王
};

class WinnerSubActivity{
public:
	WinnerSubActivity()
	{
		Act_Id = 0;
		Sub_Id = 0;
		GiftId = 0;
		Request = 0;
		Value = 0;
		Completed = false;
		Submid = false;
	}
	 int Act_Id; //活动ID
	 int Sub_Id; //子ID
	 int GiftId; 
	 int Request; //需求值
	 int Value;   //当前值
	 bool   Completed; //是否完成
	 bool   Submid;   //是否提交
};

class WinnerActivityData{
public:
	WinnerActivityData()
	{
		Act_type = 0;
		Cur_SubId = 0;
		DaysRemain = 0;
		IsCanGet = false;
	}
	 int Act_type;  //活动类型 
	 int Cur_SubId; //当前活动子ID
	 int DaysRemain; //活动剩余天数
	 bool IsCanGet; //true 可领取
	 std::map<int,WinnerSubActivity> SubList;
};

class WinnerControl
{
public:
	WinnerControl(void);
	~WinnerControl(void);

	CC_SYNTHESIZE(WinnerDialog*, mWinnerDialog, WinnerDialogPtr);

	CC_SYNTHESIZE(bool, m_IsGet, IsGet);

	CC_SYNTHESIZE_READONLY(int, m_WinnerCount, WinnerCount);

	CC_SYNTHESIZE(int, m_GetCount, GetWinnerCount);

	CC_SYNTHESIZE(bool,mBtnRefresh,BtnRefresh);

	void sendGetRqst();

	void sendWinnerValueRqst();

	void setWinnerCount(int val);

	bool getCanGet();

	void sendTaskComplete(int actId);

	void sendWinnerTip(int cout,int gif);

	bool checkCanOpen(int type);
	
	WinnerProxy* getWinnerProxy() { return &mWinnerProxy ;}

	bool getWinnerActOpen(int actID);

	WinnerActivityData* getWinnerActList(int actId);

	WinnerSubActivity* getWinnerSubID(int actId,int subId);

	std::map<int, WinnerActivityData>  mWinnerActList;

	//是否开启长胜将军
	bool getWinnerRoadOpen() { return mWinnerRoadOpen; }
	void setWinnerRoadOpen(unsigned long long _time);
private:
	WinnerProxy mWinnerProxy;
	bool mWinnerRoadOpen;
};

#define sgWinnerControl() Singleton<WinnerControl>::Get()
