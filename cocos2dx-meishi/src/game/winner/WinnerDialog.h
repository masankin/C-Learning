#pragma once

#include "game/ui/system/ActionWindow.h"
#include "game/winner/WinnerProxy.h"
#include "Message.h"


class WinnerDialog :public ActionWindow, public MessageHandler
{
public:

	WinnerDialog();
	~WinnerDialog();

	virtual bool init();

	CREATE_FUNC(WinnerDialog);

	virtual void onShow();

	virtual void onClose();

	void setWinnerBtn();

	void updateUI(bool isGet = false);

	void setCurrPanel(int type);

	void updateHongDian(int type, bool flag);

	Node* getWinPanel(int type) { return mWinPanel[mCurrType]; }

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void callBack(cocos2d::Ref*);
	void selectLabel(ImageView* label, bool selected);

	virtual void onReceiveMassage(GameMessage * message);

#ifdef MGAME_PLATFORM_MPLUS
	void onShare(Ref*);
#endif

private:
	int mCurrType;
	cocos2d::ui::ImageView* mWinnerView;
	ui::ImageView* mNormalBtn[3];
	Node* mWinPanel[3];
	Vec2 mBtnPos[3];
	std::vector<ui::ImageView*> mNormalBtnList;
};
