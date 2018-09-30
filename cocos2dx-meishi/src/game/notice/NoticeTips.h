#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "core/StringLabel.h"

class NoticeTips :public Layer
{
public:
	NoticeTips();;
	~NoticeTips();;

	virtual bool init();

	CREATE_FUNC(NoticeTips);

	void startNotice();

	void setTipsMove();

	StringLabel* mTipsLabel;
protected:
	
	Sprite* mSpriteBg;
	ui::Text* mContent;

	void callback();

private:
	bool mIsMoveing;
	Size mWinSize;
};
