#pragma once


#include  "game/ui/system/ActionWindow.h"

class FailPromptDialog :public ActionWindow
{
public:
	FailPromptDialog();;
	~FailPromptDialog();;

	virtual bool init();

	CREATE_FUNC(FailPromptDialog);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

};
