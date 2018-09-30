#ifndef __MGAME_PVPINVITEDIALOG_H__
#define __MGAME_PVPINVITEDIALOG_H__

#include "ui/system//ActionWindow.h"
#include "ui/UIListView.h"
#include "Constant.h"

class  PvpInviteItem :public Node
{
public:
	CREATE_FUNC(PvpInviteItem);
	virtual bool init();

	Button *  mBtnIcon;
	Button *  mBtnInvite;
	Text   *  mName;
	Text   *  mWin;
	Text   *  mRank;
	Sprite *  mPlayerTitle;
	Text   *  mLimitLevel;       //限制等级

private:
	Node * mRoot;

	void inviteFirendCallback(Ref * pSender);
	
	
};
class PvpInviteDialog : public ActionWindow
{
public:
	PvpInviteDialog();
	~PvpInviteDialog();

	CREATE_FUNC(PvpInviteDialog);

	virtual bool init();

	virtual void onShow();

	virtual void updateUI();
	void detailCallback(Ref * pSender);


private:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void pushListItem(PvpInviteItem* item,FriendSystem::PlayerData *playerData);
private:
	Text	 * mTime;				//挑战全服剩余时间
	ListView * mListView;			//好友list
	int		   mFriendNum;			//好友数
	vector<FriendSystem::PlayerData*> mPlayerInfo; //可邀请的好友列表
};


#endif //__MGAME_PVPINVITEDIALOG_H__
