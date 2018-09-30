#pragma once

#include  "game/ui/system/ActionWindow.h"
#include "core/StringLabel.h"

class NoticeDialog :public ActionWindow
{
public:
	NoticeDialog();;
	~NoticeDialog();;

	//@param stHead 标题 @param stContent 内容
	static void show(std::string stHead,std::string stContent);


	virtual bool init();

	void setContent(std::string stHead,std::string stContent);

	CREATE_FUNC(NoticeDialog);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	
	ui::Text* mTextTitle;
	ui::ScrollView* mScrollText;

	StringLabel* mStringLabel;

	CC_SYNTHESIZE(int, mShowTimes, ShowTimes);
};
