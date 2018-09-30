
#include "net/NetManager.h"
#include "net/proto/ClientSvrMsg.pb.h"
#include "SevenGoalProxy.h"
#include "SevenGoalControl.h"
#include "core/luaHelp/CppCallLuaHelper.h"

SevenGoalProxy::SevenGoalProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_GET_SEVENDAY_GOAL_LIST, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_RETRIVE_SEVENDAY_GOAL, this);
}

SevenGoalProxy::~SevenGoalProxy()
{

}

bool SevenGoalProxy::fault(const fgame::CSErrorMsg &err)
{
	CCLOG("[trace]SevenGoalProxy::fault exist!");
	return true;
}

void SevenGoalProxy::DoRsp(NetPack& stRsp)
{
	if (stRsp.getS2CMsg()->has_getsevengoallist_resp())
	{
		sgSevenGoalControl()->doGetSevenGoalListRsp(stRsp.getS2CMsg()->getsevengoallist_resp());
		//sgCppCallLuaHelper()->CallOnReceiveMassage_netProto(fgame::CS_CMD_GET_SEVENDAY_GOAL_LIST);
	}
	else if (stRsp.getS2CMsg()->has_retrivesevengoal_resp())
	{
		sgSevenGoalControl()->doRetriveSevenGoalResp(stRsp.getS2CMsg()->retrivesevengoal_resp());
		sgCppCallLuaHelper()->CallOnReceiveMassage_netProto(fgame::CS_CMD_RETRIVE_SEVENDAY_GOAL);
	}	
}

void SevenGoalProxy::sendGetSevenGoalListRqst()
{
	unsigned int uCmd = fgame::CS_CMD_GET_SEVENDAY_GOAL_LIST;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSGetSevenGoalListReq * pGetSevenGoalListRqst = msgPack.getC2SMsg()->mutable_getsevengoallist_req();

	sgNetManager()->SendPkg(msgPack);
}

void SevenGoalProxy::sendRetriveSevenGoalRqst(int nowDay)
{
	unsigned int uCmd = fgame::CS_CMD_RETRIVE_SEVENDAY_GOAL;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSRetriveSevenGoalReq * pGetSevenGoalListRqst = msgPack.getC2SMsg()->mutable_retrivesevengoal_req();
	pGetSevenGoalListRqst->set_nowday(nowDay);

	sgNetManager()->SendPkg(msgPack);
}
