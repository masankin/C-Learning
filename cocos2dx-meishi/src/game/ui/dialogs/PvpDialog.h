#pragma once

#include  "game/ui/system/ActionWindow.h"
#include "GamePrecedure.h"
#include "Message.h"
#include "data/Tables.h"
#include "cocos2d.h"
#include "game/ui/system/ActionWindow.h"

#define  PVP_NEED_GOLDCOIN 8
#define  PVP_NEED_PSYCH 5
USING_NS_CC;
class PvpDialog :public Window, public MessageHandler
{
public:
	PvpDialog();
	~PvpDialog();

	virtual bool init();
	void onEnter();
	void onExit(); 

	CREATE_FUNC(PvpDialog);

	virtual void onReceiveMassage(GameMessage * message);
	void powerTip();
	virtual void onShow();
	virtual void onClose();

	void showUpSession(float dt);

	void refreshBtn(int mode);

	void resetUI();

	void updateMacth(float dt);

	void closeMatchIngArm(float dt);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void onClickBuyEngerCallBack(cocos2d::Ref*);

	void updateUI();

	ui::Text* mCoin;
	ui::Text* mPower;
	ui::Text* mPowerTip;
	ui::Text* mItemText;

	ui::Text* mText_Tips;

	ui::Button* mBtnRand;
	ui::Button* mBtnRoom;
	ui::Button* mBtnItem;

	ui::Layout* mPanel_1;

	Sprite* mSprGroup;

	Sprite* mPlayerTitel[6];

	//ui::ScrollView* mItemListView;
	//std::map<int,cocos2d::Node*> mRowNodeList;

	ui::Text* mRankInfo[4];

	bool showTip;
	int isRanding;// 0 未匹配 1 匹配中 2 玩家取消---服务器未返回
	int mCurMode;
	int mChooseScore;

	ui::Button* mBtnChooseScore;
	ui::Text* mItemScore;
	ui::Text* mCurScore;

	ui::Text* mCard_Num;

	int mPrevSession;
	int mCurSession;
};

class PVPWatchDialog :public ActionWindow, public MessageHandler
{
public:
	PVPWatchDialog();
	virtual ~PVPWatchDialog();

	virtual bool init();
	virtual void onReceiveMassage(GameMessage * message);
	CREATE_FUNC(PVPWatchDialog);

	virtual void onShow();

	//virtual void  onShow();

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void backPVPDialog();
	bool mIsBackPVPDialog;
	//cocostudio::Armature* mArmBg;

};


class PVPRoomBackDialog :public Window
{
public:
	PVPRoomBackDialog();
	virtual ~PVPRoomBackDialog();

	virtual bool init();

	CREATE_FUNC(PVPRoomBackDialog);

	virtual void  onShow();

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	cocostudio::Armature* mArmBg;

};

class PvpInfoDialog:public ActionWindow
{
public:
	PvpInfoDialog();
	virtual ~PvpInfoDialog();

	virtual bool init();

	CREATE_FUNC(PvpInfoDialog);

	virtual void  onShow();

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void showMissionRule(bool isShow);

};

class PVPMatchLoadingDialog : public Window
{
public:
	PVPMatchLoadingDialog();
	virtual ~PVPMatchLoadingDialog();

	CREATE_FUNC(PVPMatchLoadingDialog);
	bool init();
	virtual void	onShow();
	void			setCancelBtnEnable(bool flag);
protected:
	virtual void	onMenuItemAction(std::string, cocos2d::Ref*);

private:
	cocostudio::Armature* mMatchLoadingArmature;
	cocostudio::Armature* mSubArm1;
	cocostudio::Armature* mMatchNoItemArm;
	ui::Text* mTextLess10;
};
