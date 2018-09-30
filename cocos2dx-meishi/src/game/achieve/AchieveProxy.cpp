#include "AchieveProxy.h"
#include "net/NetManager.h"
#include "AchieveControl.h"

AchieveProxy::AchieveProxy(void)
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_GET_ACHIEVEMENT,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_RECEIVE_ACHIEVEMENT,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_RECEIVE_ACHIEVEMENT_REWARD,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_UPDATE_ACHIEVEMENT, this);
}


AchieveProxy::~AchieveProxy(void)
{
}

void AchieveProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]AchieveProxy::DoRsp error!");

	}
	else if (stRsp.getS2CMsg()->has_getachievement_resp())//查询
	{
		CCLOG("[trace]AchieveProxy has_getachievementresp");
		sgAchieveControl()->doQueryAchievementStatus(stRsp.getS2CMsg()->getachievement_resp());
	}else if (stRsp.getS2CMsg()->has_receiveachievement_resp())//领取
	{
		CCLOG("[trace]AchieveProxy has_receiveachievement_req");
		sgAchieveControl()->doReceiveAchievement(stRsp.getS2CMsg()->receiveachievement_resp());
		//sendQueryAchievementStatus();
	}else if (stRsp.getS2CMsg()->has_receiveachievementreward_resp())
	{
		CCLOG("[trace]AchieveProxy has_receiveachievementreward_resp");
		sgAchieveControl()->doReceiveAward(stRsp.getS2CMsg()->receiveachievementreward_resp());
		//sgAchieveControl()->getAchievePanel()->showAward(stRsp.getS2CMsg()->receiveachievement_resp().achievementid());
		sendQueryAchievementStatus();
	}else if (stRsp.getS2CMsg()->has_updateachievement_resp())
	{
		CCLOG("[trace]AchieveProxy:DoRsp has_updateachievement_resp()");
		std::list<int> lUpdateList;
		const fgame::CSUpdateAchievementResp& lupdateResp = stRsp.getS2CMsg()->updateachievement_resp();
		for (int i = 0; i<lupdateResp.listids_size(); i++)
		{
			lUpdateList.push_back(lupdateResp.listids(i));
		}
		if (lUpdateList.size()>0)
		{
			sgAchieveControl()->TipAchieveComplent(lUpdateList);
		}
	}
}

bool AchieveProxy::fault(const fgame::CSErrorMsg &err)
{
	CCLOG("[trace]AchieveProxy::fault exist!");
	return true;
}


void AchieveProxy::sendQueryAchievementStatus()
{
	unsigned int uCmd = fgame::CS_CMD_GET_ACHIEVEMENT;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSGetAchievementReq* pRqst = msgPack.getC2SMsg()->mutable_getachievementreq();

	sgNetManager()->SendPkg(msgPack);
}

void AchieveProxy::sendGetAchievementAward(int id)
{
	unsigned int uCmd = fgame::CS_CMD_RECEIVE_ACHIEVEMENT;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSReceiveAchievementReq* pRqst = msgPack.getC2SMsg()->mutable_receiveachievement_req();
	pRqst->set_achievementid(id);
	sgNetManager()->SendPkg(msgPack);
}

void AchieveProxy::sendReceiveAchievementRewardReq(int id)
{
	unsigned int uCmd = fgame::CS_CMD_RECEIVE_ACHIEVEMENT_REWARD;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSReceiveAchievementRewardReq* pRqst = msgPack.getC2SMsg()->mutable_receiveachievementreward_req();
	pRqst->set_achievementlevel(id);
	sgNetManager()->SendPkg(msgPack);
}
