
#include "net/NetManager.h"
#include "RankingControl.h"
#include "net/UserObj.h"
#include "luaHelp/CppCallLuaHelper.h"

RankingProxy::RankingProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_RANKING_LIST, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_RANKING_INFO, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_RESULT, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_SELF_RANKLIST, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_RANK_WEEK_REWARD_INFO, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_RANK_WEEK_REWARD_REWARD, this);
}


RankingProxy::~RankingProxy()
{
}

void RankingProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]RankingProxy::DoRsp error!");
	}
	else if (stRsp.getS2CMsg()->has_pvprankinggetlist_resp())
	{
		CCLOG("[trace]RankingProxy has_pvpranking_resp");

		sgRankingControl()->doNetGetRankingListRsp(stRsp.getS2CMsg()->pvprankinggetlist_resp());
	}
	else if (stRsp.getS2CMsg()->has_pvprankingplayerinfo_resp())
	{
		CCLOG("[trace]RankingProxy has_pvprankingplayerinfo_resp");

		//const ::fgame::CSPvpRankingPlayerInfoResp& rankRsp = stRsp.getS2CMsg()->pvprankingplayerinfo_resp();

		sgRankingControl()->doNetPlayerRankingDataRsp(stRsp.getS2CMsg()->pvprankingplayerinfo_resp());

		//if (rankRsp.rankinfo().id() != sgUserObj()->getUin())
		//	return;
		 
		//sgUserObj()->setRanking(rankRsp.rankinfo().order());
		//sgUserObj()->setPvpScore(rankRsp.rankinfo().score());
		//sgUserObj()->setWinRate(rankRsp.rankinfo().winrate());
		//if(rankRsp.rankinfo().has_group())
		//{
		//	sgUserObj()->setRankGroup(rankRsp.rankinfo().group());
		//}
	}
	else if (stRsp.getS2CMsg()->has_pvpresult_resp())
	{
		CCLOG("[trace]RankingProxy has_pvpresult_resp");

		sgRankingControl()->doNetPlayerPvpResultRsp(stRsp.getS2CMsg()->pvpresult_resp());

	}
	else if (stRsp.getS2CMsg()->has_pvpselfranklist_resp())
	{
		CCLOG("[trace]RankingProxy has_pvpselfranklist_resp");

		sgRankingControl()->doNetGetSelfRankingResp(stRsp.getS2CMsg()->pvpselfranklist_resp());
	}
	else if (stRsp.getS2CMsg()->has_rankweekrewardinfo_resp())
	{
		auto data = stRsp.getS2CMsg()->rankweekrewardinfo_resp();
		sgRankingControl()->doNetRankWeekAwardInfoResp(data);

		sgCppCallLuaHelper()->CallOnReceiveMassage_netProto(fgame::CS_CMD_RANK_WEEK_REWARD_INFO);
	}
	else if (stRsp.getS2CMsg()->has_rankweekrewardreward_resp())
	{
		auto data = stRsp.getS2CMsg()->rankweekrewardreward_resp();
		sgRankingControl()->doNetRankWeekAwardResp(data);

		sgCppCallLuaHelper()->CallOnReceiveMassage_netProto(fgame::CS_CMD_RANK_WEEK_REWARD_REWARD);
	}
}

void RankingProxy::sendRankingListReq(eRankingGroupType groupID, int topNSize)
{
	unsigned int uCmd = fgame::CS_CMD_PVP_RANKING_LIST;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSPvpRankingGetListReq* pReqMsg = msgPack.getC2SMsg()->mutable_pvprankinggetlist_req();
	pReqMsg->set_groundid(groupID);
	pReqMsg->set_topnsize(topNSize);


	sgNetManager()->SendPkg(msgPack);
}

void RankingProxy::sendPlayerRankInfoReq(unsigned int uin)
{
	unsigned int uCmd = fgame::CS_CMD_PVP_RANKING_INFO;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSPvpRankingPlayerInfoReq* pReqMsg = msgPack.getC2SMsg()->mutable_pvprankingplayerinfo_req();
	pReqMsg->set_uin(uin);

	sgNetManager()->SendPkg(msgPack);
}


void RankingProxy::sendPlayerPvpResultReq()
{
	unsigned int uCmd = fgame::CS_CMD_PVP_RESULT;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSPvpResultReq* pReqMsg = msgPack.getC2SMsg()->mutable_pvpresult_req();

	sgNetManager()->SendPkg(msgPack);
}


void RankingProxy::sendSelfRankingListReq()
{
	unsigned int uCmd = fgame::CS_CMD_PVP_SELF_RANKLIST;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpSelfRankListReq* pReqMsg = msgPack.getC2SMsg()->mutable_pvpselfranklist_req();

	sgNetManager()->SendPkg(msgPack);
}

void RankingProxy::sendRankWeekAwardInfoReq()
{
	unsigned int uCmd = fgame::CS_CMD_RANK_WEEK_REWARD_INFO;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSRankWeekRewardInfoReq* pReqMsg = msgPack.getC2SMsg()->mutable_rankweekrewardinfo_req();

	sgNetManager()->SendPkg(msgPack);
}

void RankingProxy::sendRankWeekAwardReq()
{
	unsigned int uCmd = fgame::CS_CMD_RANK_WEEK_REWARD_REWARD;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSRankWeekRewardRewardReq* pReqMsg = msgPack.getC2SMsg()->mutable_rankweekrewardreward_req();

	sgNetManager()->SendPkg(msgPack);
}
