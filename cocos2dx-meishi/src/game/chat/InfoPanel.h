#pragma once

#include "Constant.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ui/UIHelper.h"
#include "cocos2d.h"
#include "chat/ChattingControl.h"
#include "ui/system/ActionWindow.h"
#include "MessageManager.h"
#include "ui/UILoadingBar.h"
USING_NS_CC;
using namespace ui;

enum eInfoPanelType
{
	E_TYPE_NORMAL,
	E_TYPE_PERSONAL,
	E_TYPE_FRIEND_MINE,
};

class InfoPanel : public ActionWindow, public MessageHandler
{
public:
	InfoPanel();
	~InfoPanel();

	static InfoPanel* create();
	bool initUI();
	void init(const fgame::CSQueryPlayerInfoResp &resp);

	void updatePanelIcon(const fgame::CSQueryPlayerInfoResp &resp);
	void updatePanelInfo(const fgame::CSQueryPlayerInfoResp &resp);
	void updatePanelExtra(MessageGetExtra* pMes);
	void updateOtherInfo(const fgame::CSQueryPlayerInfoResp &resp);

	void signIsVisible(bool isVisible);
	void setLockVisible(bool isVisible);
	void showLevel(int level, int curEx);

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	virtual void onReceiveMassage(GameMessage * message);
	virtual void windowAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
	virtual void onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex);

	//virtual void onClose();
	//virtual void onShow();
	void deleteFriendCallback(cocos2d::Ref*);
	void TextFieldCallback(Ref* pSender, TextFiledEventType type);

protected:

	Node* mPanelIcon;
	Node* mPanelInfo;
	Node* mPanelExtra;
	Node* mLockNode;
	long long mUin;

	Button* mSignBtn;
	Button* mSignOkBtn;
	Button* mSignFailBtn;
	Sprite* mSignBg;
	TextField* mSignText;

	Widget* mExtraBtn;
	Button* mLockBtn;
	Button* mUnLockBtn;

	std::string mPreSignStr;
	eInfoPanelType mType;
	int mExtraIsVisible;

	Sprite* mHeadIcon;
};
