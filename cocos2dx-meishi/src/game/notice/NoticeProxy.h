#pragma once


#include "BaseProxy.h"


class NoticeProxy : public BaseProxy
{
public:

	NoticeProxy();
	~NoticeProxy();

	virtual void DoRsp(NetPack& stRsp);
	virtual bool fault(const fgame::CSErrorMsg &err);
};
