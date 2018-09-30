#pragma once

#include  "game/ui//system/Window.h"
#include "GamePrecedure.h"

class PauseDialog :public Window
{
public:
	PauseDialog(){};
	~PauseDialog(){};

	virtual bool init();
	virtual void onShow();
	virtual void onClose();

	CREATE_FUNC(PauseDialog);

	void restart();
protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void onCallBackBuyPower(Ref*);
	void onCallBackBuyDiamond(Ref*);

	ui::Button* mButton;

	cocos2d::EventListenerCustom*  m_pChangePowerListener;
};
