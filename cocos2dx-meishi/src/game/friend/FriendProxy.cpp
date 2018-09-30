#include "FriendProxy.h"
#include "net/NetManager.h"
#include "FriendControl.h"
#include "net/UserObj.h"
#include "game/data/TableManager.h"
#include "game/utils/Comm.h"

FriendProxy::FriendProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_FRIEND_QUERY, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_FRIEND_REQUEST_ADD, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_FRIEND_CONFIRM_ADD, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_FRIEND_GET_INFO, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_FRIEND_REQUEST_REMOVE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_FRIEND_ADD_BLACK_LIST, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_FRIEND_REMOVE_BLACK_LIST, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PERSONAL_ROOM_APPLY_OR_REFUSE_INVITE, this);

#ifdef MGAME_PLATFORM_MPLUS
	sgNetManager()->RegCmdListener(fgame::CS_CMD_SYC_FRIEND_LIST, this);
#endif
	
}

FriendProxy::~FriendProxy()
{

}

void FriendProxy::DoRsp(NetPack& stRsp)
{
	CCLOG("[edward]=============== receive pack");
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[edward]===============error");
	}
	else if (stRsp.getS2CMsg()->has_friendgetinfo_resp())
	{
		CCLOG("[edward]=============== get friend info");
		const ::fgame::CSFriendGetInfoResp& pRespMsg = stRsp.getS2CMsg()->friendgetinfo_resp();
		
		sgFriendControl()->updateFriendList(pRespMsg);
		sgFriendControl()->updateApplicationList(pRespMsg);
		sgFriendControl()->updatePkList(pRespMsg);
	}
	else if (stRsp.getS2CMsg()->has_friendrequestadd_resp())
	{
		//红点提示
		CCLOG("[edward]=============== get friend info");
		const ::fgame::CSFriendRequestAddResp& pRespMsg = stRsp.getS2CMsg()->friendrequestadd_resp();
		if (pRespMsg.has_otheruin())
		{
			sgFriendControl()->updateApplicationList(pRespMsg);
		}
	}
	else if (stRsp.getS2CMsg()->has_friendrequestremove_resp())
	{
		CCLOG("[edward]=============== get remove info");
		sgFriendControl()->updateFriendList(stRsp.getS2CMsg()->friendrequestremove_resp());
	}
	else if (stRsp.getS2CMsg()->has_friendconfirmadd_resp())
	{
		CCLOG("[edward]=============== get add info");
		sgFriendControl()->updateFriendList(stRsp.getS2CMsg()->friendconfirmadd_resp());
	}
	else if (stRsp.getS2CMsg()->has_friendquery_resp())
	{
		CCLOG("[edward]=============== get query info");
		if (sgFriendControl()->getSeachType() == 0)
			sgFriendControl()->updateSearchList(stRsp.getS2CMsg()->friendquery_resp());
		else
			sgFriendControl()->updateFriendList(stRsp.getS2CMsg()->friendquery_resp());
	}
	else if (stRsp.getS2CMsg()->has_personalroomapplyorrefuseinvite_resp())
	{
		CCLOG("[edward]=============== get invite result");
		//int openLevel = sgUserObj()->getMaxOpenCheckPoint();
		int openLevel = sgUserObj()->getMaxOpenGateID();
		//if (openLevel >= PVP_ROOM_LEVEL && sgUserObj() ->getPlayerLevel() >= sgTableManager()->getPlayerInfo()->getInviteLevel())
		if (openLevel >= sgEnvSet("PVP_ROOM_LEVEL") && sgUserObj()->getPersonalInfo()->explevel() >= sgTableManager()->getPlayerInfo()->getInviteLevel())
		{
			sgFriendControl()->updatePkList(stRsp.getS2CMsg()->personalroomapplyorrefuseinvite_resp());
		}		
	}
#ifdef MGAME_PLATFORM_MPLUS
	else if (stRsp.getS2CMsg()->has_sycfriendlist_resp())
	{
		sgFriendControl()->updateFriendList(stRsp.getS2CMsg()->sycfriendlist_resp());
	}
#endif
	
}

void FriendProxy::sendAllListRequest()
{
	unsigned int uCmd = fgame::CS_CMD_FRIEND_GET_INFO;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSFriendGetInfoReq* pReqMsg = msgPack.getC2SMsg()->mutable_friendgetinfo_req();
	pReqMsg->set_uin(0);//暂时不处理
	pReqMsg->set_friends(true);
	pReqMsg->set_requests(true);
	pReqMsg->set_pk(true);

	sgNetManager()->SendPkg(msgPack);

}

void FriendProxy::sendFriendListRequest()
{
	unsigned int uCmd = fgame::CS_CMD_FRIEND_GET_INFO;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSFriendGetInfoReq* pReqMsg = msgPack.getC2SMsg()->mutable_friendgetinfo_req();
	pReqMsg->set_uin(0);//暂时不处理
	pReqMsg->set_friends(true);

	sgNetManager()->SendPkg(msgPack);
}

void FriendProxy::sendAddFriendRequest(int uin)
{
	unsigned int uCmd = fgame::CS_CMD_FRIEND_REQUEST_ADD;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSFriendRequestAddReq* pReqMsg = msgPack.getC2SMsg()->mutable_friendrequestadd_req();
	pReqMsg->set_uin(uin);

	sgNetManager()->SendPkg(msgPack);
}

void FriendProxy::sendSplitUpRequest(int uin)
{
	unsigned int uCmd = fgame::CS_CMD_FRIEND_REQUEST_REMOVE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSFriendRequestRemoveReq* pReqMsg = msgPack.getC2SMsg()->mutable_friendrequestremove_req();
	pReqMsg->set_uin(uin);

	sgNetManager()->SendPkg(msgPack);
}

void FriendProxy::sendAddConfirmRequest(bool isAgree, int uin)
{
	unsigned int uCmd = fgame::CS_CMD_FRIEND_CONFIRM_ADD;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSFriendConfirmAddReq* pReqMsg = msgPack.getC2SMsg()->mutable_friendconfirmadd_req();
	pReqMsg->set_agree(isAgree);
	pReqMsg->add_uins(uin);

	sgNetManager()->SendPkg(msgPack);
}

void FriendProxy::sendAddConfirmRequest(bool isAgree, std::vector<int> uins)
{
	unsigned int uCmd = fgame::CS_CMD_FRIEND_CONFIRM_ADD;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSFriendConfirmAddReq* pReqMsg = msgPack.getC2SMsg()->mutable_friendconfirmadd_req();
	pReqMsg->set_agree(isAgree);

	int size = uins.size();
	for (int i = 0; i < size; i++)
	{
		pReqMsg->add_uins(uins.at(i));
	}
	sgNetManager()->SendPkg(msgPack);
}

void FriendProxy::sendSearchRequest(int uin, std::string name ,int type /*=0*/)
{
	unsigned int uCmd = fgame::CS_CMD_FRIEND_QUERY;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSFriendQueryReq* pReqMsg = msgPack.getC2SMsg()->mutable_friendquery_req();
	pReqMsg->set_uin(uin);
	pReqMsg->set_nickname(name);
	sgFriendControl()->setSeachType(type);
	sgNetManager()->SendPkg(msgPack);
}

void FriendProxy::sendApplicationListRequest()
{
	unsigned int uCmd = fgame::CS_CMD_FRIEND_GET_INFO;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSFriendGetInfoReq* pReqMsg = msgPack.getC2SMsg()->mutable_friendgetinfo_req();
	pReqMsg->set_uin(0);//暂时不处理
	pReqMsg->set_requests(true);

	sgNetManager()->SendPkg(msgPack);
}

void FriendProxy::sendPKListRequest()
{
	unsigned int uCmd = fgame::CS_CMD_FRIEND_GET_INFO;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSFriendGetInfoReq* pReqMsg = msgPack.getC2SMsg()->mutable_friendgetinfo_req();
	pReqMsg->set_uin(0);//暂时不处理
	pReqMsg->set_pk(true);

	sgNetManager()->SendPkg(msgPack);
}

void FriendProxy::sendPkConfirmRequest(bool isAgree, int roomId, int uin)
{
	unsigned int uCmd = fgame::CS_CMD_PERSONAL_ROOM_APPLY_OR_REFUSE_INVITE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);


	fgame::CSPersonalRoomApplyOrRefuseInviteReq* pReqMsg = msgPack.getC2SMsg()->mutable_personalroomapplyorrefuseinvite_req();
	pReqMsg->set_isapply(isAgree);
	pReqMsg->set_roomid(roomId);
	pReqMsg->set_uin(uin);

	sgNetManager()->SendPkg(msgPack);
}

#ifdef MGAME_PLATFORM_MPLUS
void FriendProxy::sendSycFriendListRequest(std::vector<std::string> _str)
{
	unsigned int uCmd = fgame::CS_CMD_SYC_FRIEND_LIST;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSSycFriendListReq* pReqMsg = msgPack.getC2SMsg()->mutable_sycfriendlist_req();

	for (size_t i = 0; i < _str.size(); i++)
	{
		pReqMsg->add_openid(_str.at(i));
	}

	sgNetManager()->SendPkg(msgPack);
}
#endif
