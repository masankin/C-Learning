#include "ChattingProxy.h"
#include "net/UserObj.h"
#include "MessageManager.h"
#include "net/NetManager.h"
#include "ChattingControl.h"
#include "core/TimeCalculator.h"

ChattingProxy::ChattingProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_CHAT, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_QUERY_PLAYERINFO,this);
}

ChattingProxy::~ChattingProxy()
{

}

bool ChattingProxy::fault(const fgame::CSErrorMsg &err)
{
	if (err.errcode() == 10001)
	{
		CCLOG("[trace]ChattingProxy::fault !");
	}
	return true;
}

void ChattingProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]ChattingProxy::DoRsp error!");		
	}
	else if(stRsp.getS2CMsg()->has_pvpchat_resp())
	{
		const ::fgame::CSPvpChatResp& pRsp = stRsp.getS2CMsg()->pvpchat_resp();
		sgChattingControl()->addMessage(pRsp);
	}
	else if(stRsp.getS2CMsg()->has_queryplayerinfo_resp())
	{
		const ::fgame::CSQueryPlayerInfoResp& pRsp = stRsp.getS2CMsg()->queryplayerinfo_resp();
		sgChattingControl()->showPlayerInfo(pRsp);
	}
}

void ChattingProxy::sendChattingRpst(int msgType, std::string sContent, int isBuy, int msgId)
{
	unsigned int uCmd = fgame::CS_CMD_PVP_CHAT;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	::fgame::CSPvpChatReq* pRqst = msgPack.getC2SMsg()->mutable_pvpchat_req();	 

	pRqst->set_channel(msgType);
	pRqst->set_content(sContent);
	pRqst->set_id(msgId);
	pRqst->set_isbuy(isBuy);

	sgNetManager()->SendPkg(msgPack);
}

void ChattingProxy::sendQueryPlayerInfoRpsr(long long uin)
{
	unsigned int uCmd = fgame::CS_CMD_QUERY_PLAYERINFO;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	::fgame::CSQueryPlayerInfoReq* pRqst = msgPack.getC2SMsg()->mutable_queryplayerinfo_req();	 

	pRqst->set_uin(uin);
	sgNetManager()->SendPkg(msgPack);
}
