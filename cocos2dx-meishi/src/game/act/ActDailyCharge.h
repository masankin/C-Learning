#pragma once
#include "ActLayout.h"
#include "cocos-ext.h"
#include "cocos2d.h"
#include  "game/ui/system/Window.h"
USING_NS_CC_EXT;
USING_NS_CC;

class ActDailyCharge:public ActLayout
{
public:
	ActDailyCharge(void);
	~ActDailyCharge(void);

	static ActDailyCharge *create();

	virtual bool init();
	virtual void updateInfo();

	void onShow();

	void _onMenuAction(cocos2d::Ref*);

protected:

	ui::Text* mTextHad;
	ui::Text* mTextNeed;
	ui::LoadingBar* mLoadingBar;

	Node* mItemNode[7];

	ui::Button* mGetBtn;
	ui::Button* mChargeBtn;

	Sprite* mSprTitle;

	Sprite* mBtnSprite_0;
	Sprite* mBtnSprite_1;
};

