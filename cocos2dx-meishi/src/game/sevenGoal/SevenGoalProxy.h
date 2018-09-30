#ifndef __SEVENGOAL_PROXY_H__
#define __SEVENGOAL_PROXY_H__

#include "BaseProxy.h"

class SevenGoalProxy : public BaseProxy
{
public:
	SevenGoalProxy();
	~SevenGoalProxy();

	virtual void DoRsp(NetPack& stRsp);
	virtual bool fault(const fgame::CSErrorMsg &err);

	void sendGetSevenGoalListRqst();
	void sendRetriveSevenGoalRqst(int nowDay);

private:
};

#endif
