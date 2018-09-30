#pragma once

#include <iostream>
#include <vector>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocostudio/CocoStudio.h"
#include "net/NetPack.h"
#include "ChattingProxy.h"
#include "ChattingDialog.h"
#include "FullSrvInviteBatTips.h"

USING_NS_CC;
using namespace ui;
using namespace std;
using namespace extension;

#define TextInput_Count (60)
#define Msg_Chat_Time (180)
#define Chat_Text "chat_time"
#define Msg_Invite_Time (20.0)
#define Msg_Free_Max (3)
#define Chat_PVP_FullSrv_Invite_TimeOut (5.0)

enum eContentType
{
	ContentType_Normal,
	ContentType_Emoji,
	ContentType_Voice,
	ContentType_Challenge,
};

class ChatInfo: public Node{
public:
	ChatInfo();
	~ChatInfo();
	virtual void update(float delta);
	void setSchedule();
	std::string iconID;
	std::string mContent;
	std::string mName;
	int mUin;
	int mMsgType;
	int mTitle;
	int mContentType;
	std::string mServerNode;
	int mVipLevel;
};

class ChattingControl{

public:
	ChattingControl();
	~ChattingControl();

	ChattingProxy* getChattingProxy() { return &mChattingProxy;};
	std::string analyzeExpression(std::string text);
    
	void addMessage(const fgame::CSPvpChatResp& stMsg);
    
    std::vector<ChatInfo*>* getchatPvpHty();
    std::vector<ChatInfo*>* getChatWorldHty();

    void clearHistory();

	void setExpression(int id, int isBuy = 0);

	void sendChallengeMsg(const fgame::CSPersonalRoomBroadcastResp &resp);

	CC_SYNTHESIZE(ChattingDialog*, mChattingDialog, ChattingDialogPtr);

	CC_SYNTHESIZE(ChatInfo*, mPVPInviteChatInfoPtr, PVPInviteChatInfoPtr);

	ChattingButton* getChattingButton(bool flag = false);
	FullSrvInviteBatTips* getFullSrvInviteBatTips(bool flag = false);

	void showPlayerInfo(const fgame::CSQueryPlayerInfoResp &resp);
	void chatDataReset(int count,int left);
	void calculateFreeCount();
private:
	ChattingProxy mChattingProxy;
	std::vector<ChatInfo*> chatPvpHty;
	std::vector<ChatInfo*> chatWorldHty;
	ChattingButton* mChatButton;
	FullSrvInviteBatTips* mFullSrvInviteBatTips;
};

#define sgChattingControl() Singleton<ChattingControl>::Get()
