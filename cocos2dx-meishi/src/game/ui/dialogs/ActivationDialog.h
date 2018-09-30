#pragma once

#include  "game/ui/system/ActionWindow.h"
#include "Message.h"

class ActivationDialog : public ActionWindow, public MessageHandler
{
public:
	ActivationDialog();;
	~ActivationDialog();;

	virtual bool init();
	virtual void onShow();

	virtual void onReceiveMassage(GameMessage * message);

	CREATE_FUNC(ActivationDialog);

protected:
	void TextFieldCallback(Ref* pSender, TextFiledEventType type);
	void showTextTips(int tableKey);

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	ui::TextField* mTextFieldCode;
	ui::Text* mTextDes;
};
