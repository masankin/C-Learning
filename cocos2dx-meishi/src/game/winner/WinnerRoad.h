#pragma once

#include "game/winner/WinnerProxy.h"
#include "game/ui/system/ActionWindow.h"

class WinnerRoad :public Node
{
public:

	WinnerRoad();
	~WinnerRoad();

	virtual bool init();

	CREATE_FUNC(WinnerRoad);

	virtual void update(float delta);

protected:

	void onMenuItemAction(cocos2d::Ref*);
	void callBack(cocos2d::Ref*);
	Node* mRoot;
private:
	int mMoreGold;
	ui::Text* mWinNum_1;
	ui::Text* mTextGetted;
	ui::Text* mGoldNum_1;
	ui::Text* mGoldNum_2;
	Sprite* mGettedSprite;
	Button* mGetBtn;
	Sprite* mBtnSprite_0;
	Sprite* mSprite_2_0;
	Sprite* mSprite_2;
	Sprite* mSpriteWhq;
	ui::Text* mDayNum_1;
};
