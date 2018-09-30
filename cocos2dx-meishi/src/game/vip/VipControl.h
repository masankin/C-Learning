#pragma once

#include "net/NetPack.h"
#include "VipProxy.h"
#include "game/data/TableManager.h"
#include "game/ui/dialogs/VipDialog.h"
class VipControl
{
public:
	VipControl();
	~VipControl();

	
	void sendBuyVipRequest(int vipID ,int buyID);

	void sendVipTimeRequest();

	void doBuyVipResp(const fgame::CSBuyVipResp& resp);

	//void setView(VipDialog * view){ mVipDialog = view; };
private:
	void buyVip(Ref* sendr, sVipType vipType, int buyID, int selectID);

	VipProxy mVipProxy;
//	VipDialog * mVipDialog;
	CC_SYNTHESIZE(int, mSelectVip, SelectVip);			//当前选择的Vip 1，2，3
	CC_SYNTHESIZE(int, mSelectBuyID, SelectBuyID);		//选择7天 30天购买  7天是1，30天是2

};

#define  sgVipControl()  Singleton<VipControl>::Get()

