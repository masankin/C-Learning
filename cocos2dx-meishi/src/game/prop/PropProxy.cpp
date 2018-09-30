#include "PropProxy.h"

#include "net/NetManager.h"
#include "net/proto/ClientSvrMsg.pb.h"
#include "EnvSet.h"
#include "prop/PropsControl.h"
#include "battle/BattleRecordsManager.h"
#include "battle/BattleControl.h"
#include "GamePrecedure.h"

PropProxy::PropProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_USE_ITEM,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_BUY_ITEM,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_OPEN_CHEST, this);
}

PropProxy::~PropProxy()
{

}

bool PropProxy::fault(const fgame::CSErrorMsg &err)
{
	CCLOG("[trace]PropProxy::fault exist!");
	return true;
}

void PropProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]PropProxy::DoRsp error!");
		
	}
	else if (stRsp.getS2CMsg()->has_buyitem_resp())
	{
		sgPropsControl()->doBuyPropRsp(stRsp.getS2CMsg()->buyitem_resp());
	}
	else if (stRsp.getS2CMsg()->has_useitem_resp())
	{
		sgPropsControl()->doUsePropRsp(stRsp.getS2CMsg()->useitem_resp());
	}
	else if (stRsp.getS2CMsg()->has_openchest_resp()) {
		sgPropsControl()->doOpenChestResp(stRsp.getS2CMsg()->openchest_resp()); 
	}
}

void PropProxy::sendUsePropRqst(int itemid, int count)
{
	if (sgBattleRecordsManager()->getIsPlayback())
	{
		return;
	}
	unsigned int uCmd = fgame::CS_CMD_USE_ITEM;
	NetPack msgPack;
	//
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSUseItemReq* pUseItemRqst = msgPack.getC2SMsg()->mutable_useitem_req();

	pUseItemRqst->set_itemid(itemid);
	pUseItemRqst->set_count(count);

	sgNetManager()->SendPkg(msgPack);
}

void PropProxy::sendBuyPropRqst(int itemid, int price, int count)
{
	unsigned int uCmd = fgame::CS_CMD_BUY_ITEM;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSBuyITtemReq* pBuyItemRqst = msgPack.getC2SMsg()->mutable_buyitem_req();

	pBuyItemRqst->set_itemid(itemid);
	pBuyItemRqst->set_price(price);
	pBuyItemRqst->set_count(count);
	
	int frome = 0;
	if (sgGamePrecedure()->getEnterGame())
	{
		frome = 1;
		if (sgBattleControl()->getIsPvp())
			frome = 2;
	}
	pBuyItemRqst->set_from(frome);
	sgNetManager()->SendPkg(msgPack,true,true,true);
}

void PropProxy::sendOpenChestRqst(int itemid, int count)
{
	unsigned int uCmd = fgame::CS_CMD_OPEN_CHEST;
	NetPack msgPack;
	//
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSOpenChestReq* req = msgPack.getC2SMsg()->mutable_openchest_req();

	req->set_itemid(itemid);
	req->set_count(count);

	sgNetManager()->SendPkg(msgPack);

	CCLOG("[trace]   send OpenBox id ======== %d,  count = %d", itemid, count);
}
