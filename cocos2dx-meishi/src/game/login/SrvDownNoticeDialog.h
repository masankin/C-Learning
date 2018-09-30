#pragma once

#include  "game/ui/system/ActionWindow.h"
#include "core/StringLabel.h"

class SrvDownNoticeDialog :public Window
{
public:
	SrvDownNoticeDialog();
	~SrvDownNoticeDialog();

	virtual bool init();

	void setContent(std::string stContent);

	CREATE_FUNC(SrvDownNoticeDialog);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	
	ui::Text* mTextTitle;
	ui::Text* mTextOpenTime;
};
