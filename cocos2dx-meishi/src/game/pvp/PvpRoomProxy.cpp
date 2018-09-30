#include "PvpRoomProxy.h"
#include "net/proto/ClientSvrMsg.pb.h"
#include "net/NetManager.h"
#include "PvpRoomControl.h"
#include "friend/FriendControl.h"




PvpRoomProxy::PvpRoomProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PERSONAL_ROOM_CREATE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PERSONAL_ROOM_ENTER, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PERSONAL_ROOM_LEAVE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PERSONAL_ROOM_KICK, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PERSONAL_ROOM_INVITE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PERSONAL_ROOM_BROADCAST, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PERSONAL_ROOM_CHAT, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PERSONAL_ROOM_MODE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PERSONAL_ROOM_BEGIN_FIGHT, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PERSONAL_ROOM_COUNT_DOWN, this);
	
}

PvpRoomProxy::~PvpRoomProxy()
{

}
bool PvpRoomProxy::fault(const fgame::CSErrorMsg &err)
{
	CCLOG("[trace]PropProxy::fault exist!");
	return true;
}

void PvpRoomProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]PvpRoomProxy::DoRsp error!");

	}

	if (stRsp.getS2CMsg()->has_personalroomcreate_resp())
	{
		//创建房间
		sgPvpRoomControl()->doCreateRoomRsp(stRsp.getS2CMsg()->personalroomcreate_resp());
	}
	else if (stRsp.getS2CMsg()->has_personalroomenter_resp())
	{
		//进入房间
		sgPvpRoomControl()->doEnterRoomRsp(stRsp.getS2CMsg()->personalroomenter_resp());
	}
	else if (stRsp.getS2CMsg()->has_personalroommode_resp())
	{
		//改变房间
		sgPvpRoomControl()->doChangeRoomModeRsp(stRsp.getS2CMsg()->personalroommode_resp());

	}
	else if (stRsp.getS2CMsg()->has_personalroomleave_resp())
	{
		//离开房间
		const fgame::CSPersonalRoomLeaveResp& stMsg = stRsp.getS2CMsg()->personalroomleave_resp();
		sgPvpRoomControl()->doLeaveRoomRsp(stMsg);

		if (stMsg.has_roomid())
		{
			if (sgFriendControl()->getInviterData()->m_iRoom == stMsg.roomid())
			{
				sgFriendControl()->reomovePvpInviteTip();
				sgFriendControl()->deletePkItem(stMsg.roomid());
			}
		}
	}
	else if (stRsp.getS2CMsg()->has_personalroomkick_resp())
	{
		//踢出玩家
		sgPvpRoomControl()->doKilloffPlayerRsp(stRsp.getS2CMsg()->personalroomkick_resp());
	}
	else if (stRsp.getS2CMsg()->has_personalroominvite_resp())
	{
		//邀请玩家
		if (stRsp.getS2CMsg()->mutable_personalroominvite_resp()->has_info())
		{
			CCLOG("[edward]=============== get invite");
			sgFriendControl()->updatePkList(stRsp.getS2CMsg()->personalroominvite_resp());
		}
		else
		{
			sgPvpRoomControl()->doInvitePlayerRsp(stRsp.getS2CMsg()->personalroominvite_resp());
		}
	}
	else if (stRsp.getS2CMsg()->has_personalroomchat_resp())
	{
		//发言
		sgPvpRoomControl()->doChatRsp(stRsp.getS2CMsg()->personalroomchat_resp());
	}
	else if (stRsp.getS2CMsg()->has_personalroombeginfight_resp())
	{
		//开始战斗
		sgPvpRoomControl()->doBeginFightRsp(stRsp.getS2CMsg()->personalroombeginfight_resp());
	}
	else if (stRsp.getS2CMsg()->has_personalroombroadcast_resp())
	{
		sgPvpRoomControl()->doChallengeRsp(stRsp.getS2CMsg()->personalroombroadcast_resp());
	}
	else if (stRsp.getS2CMsg()->has_personalroomcountdown_resp())
	{
		//自动开始
		sgPvpRoomControl()->doCountDownRsp(stRsp.getS2CMsg()->personalroomcountdown_resp());
	}
}

void PvpRoomProxy::sendCreateRoomRequest()
{
	unsigned int uCmd = fgame::CS_CMD_PERSONAL_ROOM_CREATE;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPersonalRoomCreateReq* pRoomCreateReq = msgPack.getC2SMsg()->mutable_personalroomcreate_req();

	sgNetManager()->SendPkg(msgPack);
}

void PvpRoomProxy::sendEnterRoomRequest(int roomID,std::string serverNode)
{
	unsigned int uCmd = fgame::CS_CMD_PERSONAL_ROOM_ENTER;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);
	
	fgame::CSPersonalRoomEnterReq * pRoomEnterReq = msgPack.getC2SMsg()->mutable_personalroomenter_req();

	pRoomEnterReq->set_roomid(roomID);
	pRoomEnterReq->set_servernode(serverNode);

	sgNetManager()->SendPkg(msgPack);
}

void PvpRoomProxy::sendChangeRoomModeRequest(int mode)
{
	unsigned int uCmd = fgame::CS_CMD_PERSONAL_ROOM_MODE;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPersonalRoomModeReq* pRoomRoomModeReq = msgPack.getC2SMsg()->mutable_personalroommode_req();
	
	pRoomRoomModeReq->set_mode(mode);

	sgNetManager()->SendPkg(msgPack);
}

void PvpRoomProxy::sendLeaveRequest(int ID)
{
	unsigned int uCmd = fgame::CS_CMD_PERSONAL_ROOM_LEAVE;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPersonalRoomLeaveReq* pRoomLeaveReq = msgPack.getC2SMsg()->mutable_personalroomleave_req();

	pRoomLeaveReq->set_roomid(ID);

	sgNetManager()->SendPkg(msgPack);
}

void PvpRoomProxy::sendKickOffRequest(int uin)
{
	unsigned int uCmd = fgame::CS_CMD_PERSONAL_ROOM_KICK;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPersonalRoomKickReq* pRoomKickReq = msgPack.getC2SMsg()->mutable_personalroomkick_req();

	pRoomKickReq->set_uin(uin);

	sgNetManager()->SendPkg(msgPack);
}

void PvpRoomProxy::sendInviteFriendRequest(int uin)
{
	unsigned int uCmd = fgame::CS_CMD_PERSONAL_ROOM_INVITE;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPersonalRoomInviteReq* pRoomInviteReq = msgPack.getC2SMsg()->mutable_personalroominvite_req();

	pRoomInviteReq->set_uin(uin);

	sgNetManager()->SendPkg(msgPack);
}

void PvpRoomProxy::sendChallengeRequest()
{
	unsigned int uCmd = fgame::CS_CMD_PERSONAL_ROOM_BROADCAST;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPersonalRoomBroadcastReq* pRoomBroadcastReq = msgPack.getC2SMsg()->mutable_personalroombroadcast_req();

	sgNetManager()->SendPkg(msgPack);
}

void PvpRoomProxy::sendChatRequest(int id)
{
	unsigned int uCmd = fgame::CS_CMD_PERSONAL_ROOM_CHAT;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPersonalRoomChatReq* pRoomChatReq = msgPack.getC2SMsg()->mutable_personalroomchat_req();

	pRoomChatReq->set_id(id);

	sgNetManager()->SendPkg(msgPack);
}

void PvpRoomProxy::sendBeginFightRequest()
{
	unsigned int uCmd = fgame::CS_CMD_PERSONAL_ROOM_BEGIN_FIGHT;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPersonalRoomBeginFightReq* pRoomBeginFightReq = msgPack.getC2SMsg()->mutable_personalroombeginfight_req();

	sgNetManager()->SendPkg(msgPack);
}

