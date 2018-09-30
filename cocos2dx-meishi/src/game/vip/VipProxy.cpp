#include "VipProxy.h"
#include "net/proto/ClientSvrMsg.pb.h"
#include "net/NetManager.h"
#include "VipControl.h"





VipProxy::VipProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_BUY_VIP, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_VIP_TIME, this);
}

VipProxy::~VipProxy()
{

}
bool VipProxy::fault(const fgame::CSErrorMsg &err)
{
	CCLOG("[trace]PropProxy::fault exist!");
	return true;
}

void VipProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]PvpRoomProxy::DoRsp error!");

	}

	if (stRsp.getS2CMsg()->has_buyvip_resp())
	{
		
		sgVipControl()->doBuyVipResp(stRsp.getS2CMsg()->buyvip_resp());
	}
	
}



void VipProxy::sendBuyVipRequest(int vipID, int price, int vipBuyId, int vipSelectID)
{
	CCLOG("[trace]VipProxy::sendBuyVipRequest vipID:%d==price:%d==vipBuyID:%d==vipSelectID:%d", vipID, price, vipBuyId, vipSelectID);
	unsigned int uCmd = fgame::CS_CMD_BUY_VIP;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSBuyVipReq* pBuyVipReq = msgPack.getC2SMsg()->mutable_buyvip_req();
	pBuyVipReq->set_viplevel(vipID);
	pBuyVipReq->set_price(price);
	pBuyVipReq->set_payment(vipBuyId);
	pBuyVipReq->set_timetype(vipSelectID);
	sgNetManager()->SendPkg(msgPack);
}

void VipProxy::sendVipTimeRequest()
{
	CCLOG("[trace]VipProxy::sendVipTimeRequest");
	unsigned int uCmd = fgame::CS_CMD_VIP_TIME;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSVipTimeReq* pVipTimeReq = msgPack.getC2SMsg()->mutable_viptime_req();

	sgNetManager()->SendPkg(msgPack);
}



