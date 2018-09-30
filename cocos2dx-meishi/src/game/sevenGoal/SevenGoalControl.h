
#ifndef __SEVENGOAL_CONTROL_H__ 
#define __SEVENGOAL_CONTROL_H__

#include "SevenGoalProxy.h"
#include "net/proto/ClientSvrMsg.pb.h"

class SevenGoalControl
{
public:
	SevenGoalControl();
	~SevenGoalControl();

	void sendGetSevenGoalListRqst();
	void sendRetriveSevenGoalRqst(int nowDay);

	void doGetSevenGoalListRsp(const fgame::CSGetSevenGoalListResp& stMsg);
	void doRetriveSevenGoalResp(const fgame::CSRetriveSevenGoalResp& stMsg);

	void setSevenGoalNodeState();

	void setNowDay(int b){ mNowDay = b; };
	int getNowDay(){ return mNowDay; };

	void setIsRetrived(bool b){ mIsRetrived = b; };
	bool getIsRetrived(){ return mIsRetrived; };

	void setNowNum(int b){ mNowNum = b; };
	int getNowNum(){ return mNowNum; };

	void setNextNum(int b){ mNextNum = b; };
	int getNextNum(){ return mNextNum; };

private:

	SevenGoalProxy * mProxy;

	int  mNowDay;
	int  mIsRetrived;
	int  mNowNum;
	int  mNextNum;
};

#define sgSevenGoalControl() Singleton<SevenGoalControl>::Get()

#endif
