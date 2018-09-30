#ifndef __MGAME_PVPROOM_H__
#define __MGAME_PVPROOM_H__

#include "ui/system//Window.h"
#include "MessageManager.h"


class PvpRoomItem :Node
{
public:
	virtual bool init();
	CREATE_FUNC(PvpRoomItem);

	Node *		mPlayer;
	Node *		mNone;
	Node *		mClose;
	Button *	mIcon;
	Sprite *	mIconBg;
	Text *		mName;
	Text *		mRank;
	Text *		mWin;
	Text *      mChat;
	Button *    mkillPlayer;
	Sprite *	mCrown;
	Sprite *	mTitle;
	Node *		mVipNode;

private:
	Node *		mRoot;

	void killoffCallBack(cocos2d::Ref* sender);
	void detailCallback(cocos2d::Ref* sender);

	
};



class PvpRoom : public Window
{
public:
	PvpRoom();
	~PvpRoom();

	CREATE_FUNC(PvpRoom);

	virtual bool init();

	virtual void onShow();

	virtual void onClose();
	
	virtual void updateUI();

	void showPlayer();//显示玩家

	void showChat(int uin, int id );	//显示消息

	void changeMode();	//改变模式
	
	Text *getTimeNode(){ return mTime; };
private:
	int			m_iTime2Start;			//自动开始时间	
	vector <PvpRoomItem*>m_vPlayer;		//4个玩家节点

	Button * mBtnStart;					//开始按钮			
	Button * mBtn2;						//两人按钮
	Button * mBtn3;						//三人按钮
	Button * mBtn4;						//四人按钮
	Sprite * mSpriteBtnBg;				//切换模式按钮的背景	
	Text   * mTime;						//time 
	Node   * mQuicChat;					//快捷发言
	Button * mChallenge;				//挑战全服
	Text   * mChallengeTime;			//挑战全服冷却时间
	Window * mInvitePage;				//邀请好友窗口

private:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);


	void removeChat(Node * pSender);

	void update(float dt);


};


#endif //__MGAME_PVPROOM_H__
