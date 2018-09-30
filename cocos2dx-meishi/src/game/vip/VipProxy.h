
#ifndef __MGAME_VIPPROXY_H__
#define __MGAME_VIPPROXY_H__

#include "BaseProxy.h"




class VipProxy : public BaseProxy
{
public:
	VipProxy();
	~VipProxy();

	virtual void DoRsp(NetPack& stRsp);

	virtual bool fault(const fgame::CSErrorMsg &err);

	
	// vipID=1,2,3		【白银,黄金,钻石】
	// price
	//  vipBuyID =1,2,3	【荣誉,钻石,现金】
	// vipSelectID =1,2 【7天,30天】
	void sendBuyVipRequest(int vipID, int price, int vipBuyId, int vipSelectID);//购买会员

	void sendVipTimeRequest();
};




#endif //__MGAME_VIPPROXY_H__
