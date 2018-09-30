#pragma once

#include  "game/ui/system/ActionWindow.h"
#include "cocos2d.h"

USING_NS_CC;

class WinnerGift:public ActionWindow
{
public:
	WinnerGift();
	virtual ~WinnerGift();

	virtual bool init();

	CREATE_FUNC(WinnerGift);

	virtual void  onShow();

	void setActId(int actId);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

};
