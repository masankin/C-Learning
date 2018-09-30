#ifndef  _QBASE_PROXY_
#define  _QBASE_PROXY_

#include "net/NetPack.h"

class BaseProxy
{
public:
	virtual void DoRsp(NetPack& stRsp) = 0;
	
	virtual bool fault(const fgame::CSErrorMsg &err) { return true; };
};


#endif // _QBASE_PROXY_



