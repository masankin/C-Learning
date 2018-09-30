#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "GamePrecedure.h"
#include "Message.h"

#define  NEED_GOLDCOIN 99

class NameingDialog :public ActionWindow, public MessageHandler
{
public:
	NameingDialog();
	~NameingDialog();

	virtual bool init();
	virtual void onShow();
	CREATE_FUNC(NameingDialog);
	virtual void onReceiveMassage(GameMessage * message);

protected:

	void loadAreaName();

	void TextFieldCallback(Ref* pSender, TextFiledEventType type);

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	void setBtnAreaBg(Button* btn,int areaID);

	ui::TextField* mTextField;
	ui::Text* mText_Tip;
	ui::Text* mText_Tip1;
	ui::Text* mText_Tip2;
	Button* mBtnArea;

	int mSelectArea;
};
