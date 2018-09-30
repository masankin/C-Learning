#include "SyncProxy.h"
#include "net/NetManager.h"
#include "SyncControl.h"

#include "EnvSet.h"
#include "net/UserObj.h"
#include "MessageManager.h"
#include "ui/PageFunction.h"
#include "notice/NoticeControl.h"
#include "utils/Comm.h"
SyncProxy::SyncProxy(void)
{ 
	sgNetManager()->RegCmdListener(fgame::CS_CMD_OPEN_BIG_STAGE,this); 

	sgNetManager()->RegCmdListener(fgame::CS_CMD_CHANGE_NAME,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_GM_COMMAND,this);

	sgNetManager()->RegCmdListener(fgame::CS_CMD_SYC_PLAYERINFO, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_CHANGE_PLAYER_ICON, this);
	
	// 处理充值下发
	sgNetManager()->RegCmdListener(fgame::CS_CMD_RECHARGE_CALLBACK, this);

	sgNetManager()->RegCmdListener(fgame::CS_CMD_SET_TITLE, this);

	sgNetManager()->RegCmdListener(fgame::CS_CMD_SET_SIGNATURE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_SET_EXTRAINFO_VISIBLE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_GET_EXTRAINFO, this);
}


SyncProxy::~SyncProxy(void)
{
}

void SyncProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]SyncProxy::DoRsp error!");

	}	
	else if (stRsp.getS2CMsg()->has_openbigstage_resp())
	{
		const fgame::CSOpenBigStageResp pOpenStageResp = stRsp.getS2CMsg()->openbigstage_resp();
		CCLOG("[trace]SyncProxy: has_openbigstage_resp");
	} 
	else if(stRsp.getS2CMsg()->has_changename_resp())
	{
		const ::fgame::CSChangeNameResp& regRsp = stRsp.getS2CMsg()->changename_resp();
		int _ret = regRsp.ret();
		sgMessageManager()->sendMessage(EMT_CHANGE_NAME,__String::createWithFormat("%d",_ret)->getCString());	
	}else if (stRsp.getS2CMsg()->has_gmcmd_req())
	{
		const fgame::CSGMCmdReq& pGmReq = stRsp.getS2CMsg()->gmcmd_req();
		sgSyncControl()->doGmCmdRsp(pGmReq);
	}
	else if(stRsp.getS2CMsg()->has_sycplayerinfo_resp())
	{
		const ::fgame::CSSycPlayerInfoResp& regRsp = stRsp.getS2CMsg()->sycplayerinfo_resp();
		sgSyncControl()->doSyncPlayerInfoRsp(regRsp); 
	}
	else if (stRsp.getS2CMsg()->has_changeplayericon_resp())
	{
		const ::fgame::CSChangePlayerIconResp& regRsp = stRsp.getS2CMsg()->changeplayericon_resp();
		if (regRsp.ret() == 0)
		{
			//sgUserObj()->setPlayerIconID(regRsp.iconid());
			sgUserObj()->getPersonalInfo()->set_iconid(regRsp.iconid());
			sgMessageManager()->sendMessage(EMT_CHANGE_PLAYER_ICON, regRsp.iconid());
		}
	}
	else if (stRsp.getS2CMsg()->has_settitle_resp())
	{
		//Server 未加入ret值
		const ::fgame::CSSetTitleResp& regRsp = stRsp.getS2CMsg()->settitle_resp();
		sgUserObj()->getPersonalInfo()->set_playertitle(regRsp.title());//sgUserObj()->setPlayerTitle(regRsp.title());
		sgMessageManager()->sendMessage(EMT_CHANGE_PLAYER_BADGE, __String::createWithFormat("%d", regRsp.title())->getCString());
	}
	else if (stRsp.getS2CMsg()->has_rechargecallback_resp()) 
	{
		// 网络充值下发
		const fgame::CSRechargeCallbackResp& payCallback = stRsp.getS2CMsg()->rechargecallback_resp();
		CCLOG("[trace]  product id = %d,   orderId = %s", payCallback.id(), payCallback.orderid().c_str());
		sgMessageManager()->sendMessage(EMT_SHOP_PAYCOMPLENT, StringUtils::format("%d,%s", payCallback.id(), payCallback.orderid().c_str()));
	}
	else if(stRsp.getS2CMsg()->has_syncplayerdata_resp())
	{		
	}
	else if(stRsp.getS2CMsg()->has_getplayerdata_resp())
	{
	}
	else if (stRsp.getS2CMsg()->has_setsignature_resp())
	{	
		const fgame::CSSetSignatureResp& regRsp = stRsp.getS2CMsg()->setsignature_resp();
		sgMessageManager()->sendMessage(EMT_CHANGE_SIGN, "");
	}
	else if (stRsp.getS2CMsg()->has_setextrainfovisible_resp())
	{
		const fgame::CSSetExtraInfoVisibleResp& regRsp = stRsp.getS2CMsg()->setextrainfovisible_resp();
		sgMessageManager()->sendMessage(EMT_CHANGE_EXTRA_VISIBLE, StringUtils::format("%d", regRsp.visible()));
	}
	else if (stRsp.getS2CMsg()->has_getextrainfo_resp())
	{	
		const fgame::CSGetExtraInfoResp& regRsp = stRsp.getS2CMsg()->getextrainfo_resp();
		sgMessageManager()->sendMessage(EMT_GET_EXTRA, StringUtils::format("%d %d %d %d %d %d", regRsp.totalpvpcount(),
			regRsp.totalpvecount(),regRsp.totalpvpuseitem(),regRsp.totalpveuseitem(),regRsp.maxpvppoint(),regRsp.maxpvpwincount()));
	}
}

bool SyncProxy::fault(const fgame::CSErrorMsg &err)
{
	CCLOG("[trace]PropProxy::fault exist!");
	return true;
}
 


void SyncProxy::sendOpenStageRqst(int stageId)
{
	unsigned int uCmd = fgame::CS_CMD_OPEN_BIG_STAGE;
	NetPack msgPack;
	////
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSOpenBigStageReq* pRqst = msgPack.getC2SMsg()->mutable_openbigstage_req();

	pRqst->set_pointid(stageId);

	sgNetManager()->SendPkg(msgPack, false, true);
} 

void SyncProxy::sendChangeName(std::string name, int areaId)
{
	unsigned int uCmd = fgame::CS_CMD_CHANGE_NAME;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSChangeNameReq* pSycInfoRqst = msgPack.getC2SMsg()->mutable_changename_req();

	pSycInfoRqst->set_name(name);
	pSycInfoRqst->set_area(areaId);

	sgNetManager()->SendPkg(msgPack);
}

void SyncProxy::sendSyncPlayerInfoRqst()
{
	unsigned int uCmd = fgame::CS_CMD_SYC_PLAYERINFO;
	NetPack msgPack;
	//
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSSycPlayerInfoReq* pSycInfoRqst = msgPack.getC2SMsg()->mutable_sycplayerinfo_req();

	sgNetManager()->SendPkg(msgPack);
}

void SyncProxy::doSyncPlayerInfoRsp(const fgame::CSSycPlayerInfoResp& resp)
{
	sgUserObj()->getPersonalInfo()->set_energybuycount(resp.energybuycount() + 1);//sgUserObj()->setEnergyBuyCount(resp.energybuycount() + 1);
	sgUserObj()->getPersonalInfo()->set_playertitle(resp.playertitle());//sgUserObj()->setPlayerTitle(resp.playertitle());
	sgUserObj()->getPersonalInfo()->set_name(resp.name());//sgUserObj()->setPlayerName(resp.name());
	sgUserObj()->getPersonalInfo()->set_changenamecount(resp.changenamecount());//sgUserObj()->setChangeNameCount(resp.changenamecount()); 
	sgUserObj()->getPersonalInfo()->set_diamond(resp.diamond());//sgUserObj()->setDiamond(resp.diamond()); 
	sgUserObj()->getPersonalInfo()->set_powerslot(resp.energyslot());//sgUserObj()->setPvpPowerLimit(resp.energyslot());
	sgUserObj()->getPersonalInfo()->set_energyslot(resp.powerslot());//sgUserObj()->setPvePowerLimit(resp.powerslot());
	sgUserObj()->getPersonalInfo()->set_monthcarddays(resp.monthcarddays());//sgUserObj()->setMonthCard(resp.monthcarddays());
	sgUserObj()->getPersonalInfo()->set_shopguide(resp.shopguide());//sgUserObj()->setShopGuideNum(resp.shopguide()); 
	sgUserObj()->getPersonalInfo()->set_iconid(resp.iconid());//sgUserObj()->setPlayerIconID(resp.iconid());
	sgUserObj()->getPersonalInfo()->set_roomticket(resp.roomticket());//sgUserObj()->setRoomTicketCount(resp.roomticket());
	sgUserObj()->getPersonalInfo()->set_explevel(resp.explevel());//sgUserObj()->setPlayerLevel(resp.explevel());
	sgUserObj()->getPersonalInfo()->set_exp(resp.exp());//sgUserObj()->setPlayerCurEX(resp.exp());
	sgUserObj()->getPersonalInfo()->set_viplevel(resp.viplevel());//sgUserObj()->setVipLevel(resp.viplevel());
	sgUserObj()->getPersonalInfo()->set_score(resp.score());
	sgUserObj()->changePvpPower(resp.energy());
	sgUserObj()->changePvePower(resp.power());
	sgUserObj()->initProp(resp);
	sgUserObj()->initBadges(resp);
	sgUserObj()->initVipTime(resp.remaintime());
	sgChattingControl()->chatDataReset(resp.chatfreecount(), resp.timeleft());
	sgUserObj()->getPersonalInfo()->set_area(resp.area());
	sgMessageManager()->sendMessage(EMT_UPDATE_PLAYER_BADGES, "");
	
#ifdef MGAME_IN_APP_SDK
	//by carl
	if (sgChannelApi()->getChannelId() == ChannelApi::eChannelID::ChannelID_Oppo)
	{
		//sgChannelApi()->uploadPlayerInfo(__String::createWithFormat("%d", sgUserObj()->getUin())->getCString(), sgUserObj()->getGameServerName(), sgUserObj()->getPlayerName(), __String::createWithFormat("%d", sgUserObj()->getPlayerLevel())->getCString());
		sgChannelApi()->uploadPlayerInfo(__String::createWithFormat("%d", sgUserObj()->getUin())->getCString(), sgUserObj()->getGameServerName(), sgUserObj()->getPersonalInfo()->name(), __String::createWithFormat("%d", sgUserObj()->getPersonalInfo()->explevel())->getCString());
	}
#endif
#ifdef MGAME_PLATFORM_MPLUS
	if (resp.has_invitecode())
	{
		sgMplusControl()->setInviteCode(resp.invitecode());
	}
	if (resp.has_caninvite())
	{
		sgMplusControl()->setCanInvite(resp.caninvite());
	}
#endif
	CCLOG("[trace]SyncProxy::doSyncPlayerInfoRsp openID is %s and uin is %d", sgUserObj()->getOpenID().c_str() , sgUserObj()->getUin());
}



void SyncProxy::sendChangePlayerIcon(int iconId, std::string iconUrl)
{
	unsigned int uCmd = fgame::CS_CMD_CHANGE_PLAYER_ICON;
	NetPack msgPack;
	
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSChangePlayerIconReq* pRqst = msgPack.getC2SMsg()->mutable_changeplayericon_req();

	pRqst->set_iconid(String::createWithFormat("%d",iconId)->getCString());
#ifdef MGAME_PLATFORM_MPLUS
	pRqst->set_iconurl(iconUrl);
#endif
	
#ifdef MGAME_TENCENT_PLATFORM
	if (iconId == 1 && sgUserObj()->getHeadIconUrl().size() != 0)
	{
		pRqst->set_iconid(sgUserObj()->getHeadIconUrl());
	}
#endif // MGAME_TENCENT_PLATFORM

	sgNetManager()->SendPkg(msgPack);
}

void SyncProxy::sendChangePlayerTitle(int iTitle)
{
	unsigned int uCmd = fgame::CS_CMD_SET_TITLE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSSetTitleReq* pRqst = msgPack.getC2SMsg()->mutable_settitle_req();

	pRqst->set_title(iTitle);

	sgNetManager()->SendPkg(msgPack);
}

void SyncProxy::sendChangeSignRqst(std::string data)
{
	unsigned int uCmd = fgame::CS_CMD_SET_SIGNATURE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSSetSignatureReq* pRqst = msgPack.getC2SMsg()->mutable_setsignature_req();
	pRqst->set_signature(data);
	sgNetManager()->SendPkg(msgPack);
}

void SyncProxy::sendChangeInforVisibleRqst(bool isVisible)
{
	unsigned int uCmd = fgame::CS_CMD_SET_EXTRAINFO_VISIBLE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSSetExtraInfoVisibleReq* pRqst = msgPack.getC2SMsg()->mutable_setextrainfovisible_req();
	pRqst->set_visible(isVisible);
	sgNetManager()->SendPkg(msgPack);
}

void SyncProxy::sendExtraInfoRqst(int uin)
{
	unsigned int uCmd = fgame::CS_CMD_GET_EXTRAINFO;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSGetExtraInfoReq* pRqst = msgPack.getC2SMsg()->mutable_getextrainfo_req();
	pRqst->set_uin(uin);
	sgNetManager()->SendPkg(msgPack);
}
