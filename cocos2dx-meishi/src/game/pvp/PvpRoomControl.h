#pragma once
#include "Constant.h"
#include "Singleton.h"
#include "net/NetPack.h"
#include "game/ui/dialogs/PvpRoom.h"
#include "PvpRoomProxy.h"
#include "game/ui/dialogs/PvpInviteDialog.h"
#define DEFAULT_TIME_TO_START 10
#define DEFAULT_TIME_TO_DEFAULTSTART 60
#define  CREATE_PRICE 8
#define  ChallengeTime 20
enum eRoomMode
{
	ROOMMODE_TWO = 2,		//2人模式
	ROOMMODE_THREE = 3,		//3人模式
	ROOMMODE_FOUR = 4,		//4人模式
};

class PvpRoomControl
{
public:
	PvpRoomControl();
	~PvpRoomControl();
	
	//创建房间请求
	void sendCreateRoomRqst();
	//创建房间回包
	void doCreateRoomRsp(const fgame::CSPersonalRoomCreateResp& resp);

	//进入房间回包
	void doEnterRoomRsp(const fgame::CSPersonalRoomEnterResp& resp);
	
	//改变房间模式请求
	void semdChangeRoomModeRqst(eRoomMode);
	//改变房间模式回包
	void doChangeRoomModeRsp(const fgame::CSPersonalRoomModeResp &resp);

	//离开房间请求
	void sendLeaveRoomRqst();
	//离开房间回包
	void doLeaveRoomRsp(const fgame::CSPersonalRoomLeaveResp &resp);

	//踢出玩家请求
	void sendKilloffPlayer(int uin);
	//踢出玩家回包
	void doKilloffPlayerRsp(const fgame::CSPersonalRoomKickResp & resp);

	//邀请玩家请求
	void sendInvitePlayer(int uin);
	//邀请玩家回包
	void doInvitePlayerRsp(const fgame::CSPersonalRoomInviteResp &resp);

	//挑战全服请求
	void sendChallengeRequest();
	//挑战全服回包
	void doChallengeRsp(const fgame::CSPersonalRoomBroadcastResp&resp);

	//发言请求	
	void sendChatRequest(int id );
	//发言回包
	void doChatRsp(const fgame::CSPersonalRoomChatResp& resp);
	
	//开始战斗请求
	void sendBeginFightRequest();
	//开始战斗回包
	void doBeginFightRsp(const fgame::CSPersonalRoomBeginFightResp &resp);
	
	//聊天接受挑战
	void sendEnterRoomRequest(int roomid,std::string serverNode);

	//倒计时自动进入
	void doCountDownRsp(const fgame::CSPersonalRoomCountDownResp &resp);

	vector <FriendSystem::PlayerData>  mPlayerList;

	void showTip(std::string str);//提示信息

	void deletePlayer(int uin);	
private:
	PvpRoomProxy g_PvpRoomProxy;
	CC_SYNTHESIZE(PvpRoom*,mPvpRoom,PvpRoom);				
	CC_SYNTHESIZE(PvpInviteDialog*, mPvpInvieDialog, PvpInviteDilog);
	CC_SYNTHESIZE(eRoomMode, mRoomMode, RoomMode);			//当前房间模式
	CC_SYNTHESIZE(int,mRoomID,RoomId);						//房间ID
	CC_SYNTHESIZE(bool,mIsHourse,IsHourse);					//是否是房主
	CC_SYNTHESIZE(int, mPlayerNum,PlayerNum);				//房间人数			
};

#define  sgPvpRoomControl()  Singleton<PvpRoomControl>::Get()

