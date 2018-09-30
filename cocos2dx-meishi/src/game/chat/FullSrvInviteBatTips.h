#pragma once
#include "ChattingDialog.h"
enum eInviteBatState
{
	InviteBatType_Null,
	InviteBatType_Born,
	InviteBatType_Stand,
	InviteBatType_End,
};

class FullSrvInviteBatTips : public Node
{	
public:
	FullSrvInviteBatTips(void);
	~FullSrvInviteBatTips(void);
	static FullSrvInviteBatTips* create();
	virtual bool init();
	void updateTime(float delta);
	void updateUI(ChatInfo* info);
	CC_SYNTHESIZE(eInviteBatState, mInviteBatState, InviteBatState);
protected:
	void resetUI(ChatInfo* info);
	void inviteCallback(cocos2d::Ref*,int uin);
	void acceptInviteCallback(cocos2d::Ref*);
	void onClickBuyEngerCallBack(cocos2d::Ref*);
	void aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
	void hideAni();
	ui::Text *mText;
	int mInviteRoomId;	
	std::string mServerNode;
	ui::Layout* mPanelInvite;
	Node* mRoot;
	cocostudio::Armature* mInviteAni;

};
