#pragma once

#include  "game/ui//system/ActionWindow.h"

class About :public ActionWindow
{
public:
	About();;
	~About();;

	virtual bool init();
	virtual void onShow();

	CREATE_FUNC(About);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	Text * mVersion;
};
