#include "net/NetManager.h"
#include "net/UserObj.h"
#include "DayTaskProxy.h"
#include "DayTaskControl.h"

DayTaskProxy::DayTaskProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_MISSION_LIST, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_MISSION_UPDATE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_MISSION_COMPLETE, this);
}


DayTaskProxy::~DayTaskProxy()
{

}

void DayTaskProxy::DoRsp(NetPack& stRsp)
{

	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	//CCLOG("[trace] DayTaskProxy ===========    msg Id ============= %d", msgID);
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]DayTaskProxy::DoRsp error!");
	}
	else if (stRsp.getS2CMsg()->has_missionlist_resp())
	{
		// 同步（已完成的）每日任务
		const ::fgame::CSMissionListResp& missionListResp = stRsp.getS2CMsg()->missionlist_resp();
		sgDayTaskControl()->syncMissionFromServer(missionListResp);
	}
	else if (stRsp.getS2CMsg()->has_missioncomplete_resp()){
		 // 领取奖励
		CCLOG("[trace]     mission Complete ............................. ");
		const ::fgame::CSMissionCompleteResp& completeMission = stRsp.getS2CMsg()->missioncomplete_resp();
		sgDayTaskControl()->updateMission(completeMission.mission());
	}
	else if (stRsp.getS2CMsg()->has_missionupdate_resp()) {
		// 更新任务情况
		sgDayTaskControl()->missionUpdateFromServer(stRsp.getS2CMsg()->missionupdate_resp());
	}
}

void DayTaskProxy::sendMissionListReq()
{
	unsigned int uCmd = fgame::CS_CMD_MISSION_LIST;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);
	fgame::CSMissionListReq* pReqMsg = msgPack.getC2SMsg()->mutable_missionlist_req();
	sgNetManager()->SendPkg(msgPack);
}

void DayTaskProxy::sendMissionComplete(int missionId)
{
	unsigned int uCmd = fgame::CS_CMD_MISSION_COMPLETE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSMissionCompleteReq* pReqMsg = msgPack.getC2SMsg()->mutable_missioncomplete_req();
	pReqMsg->set_missionid(missionId);

	sgNetManager()->SendPkg(msgPack);
}
