#pragma once

#include  "game/ui//system/Window.h"
#include "GamePrecedure.h"

class PropItem;
class ProDialog :public Window
{
public:
	ProDialog(){};
	~ProDialog(){};

	virtual bool init();
	CREATE_FUNC(ProDialog);
	virtual void onShow();


protected:
	void backCallBack();
	void initPage();
private:
	ui::Text * mTextName;
	ui::Text * mTextExplain1;
	ui::Text * mTextExplain2;
	ui::Text * mTextNum;
	cocos2d::Node *mIcon;
};
