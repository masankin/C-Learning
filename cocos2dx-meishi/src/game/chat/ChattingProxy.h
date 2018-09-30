#pragma once


#include "BaseProxy.h"


class ChattingProxy : public BaseProxy
{
public:

	ChattingProxy();
	~ChattingProxy();

	virtual void DoRsp(NetPack& stRsp);
	virtual bool fault(const fgame::CSErrorMsg &err);

	void sendChattingRpst(int msgType,std::string sContent,int isBuy = 0,int msgId = 0);

	void sendQueryPlayerInfoRpsr(long long uin);

};
