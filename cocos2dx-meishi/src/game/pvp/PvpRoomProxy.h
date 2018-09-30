
#ifndef __MGAME_PVPROOMPROXY_H__
#define __MGAME_PVPROOMPROXY_H__

#include "BaseProxy.h"




class PvpRoomProxy : public BaseProxy
{
public:
	PvpRoomProxy();
	~PvpRoomProxy();

	virtual void DoRsp(NetPack& stRsp);

	virtual bool fault(const fgame::CSErrorMsg &err);

	//开房
	void sendCreateRoomRequest();

	//进入房间
	void sendEnterRoomRequest(int roomID,std::string serverNode);

	//更换房间规模
	void sendChangeRoomModeRequest(int mode);

	//离开房间
	void sendLeaveRequest(int ID);

	//踢出玩家
	void sendKickOffRequest(int uin);

	//邀请好友
	void sendInviteFriendRequest(int uin);

	//挑战全服
	void sendChallengeRequest();
	
	//发言
	void sendChatRequest(int id );

	//开始战斗
	void sendBeginFightRequest();

	//好友申请
	void sendAddFriendRequest(int ID);


};




#endif //__MGAME_PVPROOMPROXY_H__
