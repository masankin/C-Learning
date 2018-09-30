#ifndef __GUIDEPROXY_H__
#define __GUIDEPROXY_H__
#include "BaseProxy.h"
class GuideProxy: public BaseProxy
{
public:
	GuideProxy();
	~GuideProxy();
	void DoRsp(NetPack& stRsp);

	bool fault(const fgame::CSErrorMsg &err);

	void sendGuide2Oss(int id);

private:

};

 
#endif
