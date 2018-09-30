#pragma once
#include "ActLayout.h"
#include "cocos-ext.h"
#include "cocos2d.h"
#include  "game/ui/system/Window.h"
USING_NS_CC_EXT;
USING_NS_CC;

class ActRecharge:public ActLayout
{
public:
	ActRecharge(void);
	~ActRecharge(void);

	static ActRecharge *create();

	virtual void updateInfo(fgame::CSSevenDaySignListResp&);
	virtual bool init();
	virtual void updateInfo();

	void onShow();

	void _onMenuAction(cocos2d::Ref*);

	Node* createRowNode(int id,int num,std::string strIcon);

protected:
	int mRechargeType;

	ui::Text* mTextHad;
	ui::Text* mTextNeed;
	ui::LoadingBar* mLoadingBar_1;
	ui::LoadingBar* mLoadingBar_2;

	ui::Button* mGetBtn;
	ui::Button* mChargeBtn;

	Sprite* mSprTitle;
	ui::Text* mOpenTime;
	ui::ScrollView* mScrollView;

	Sprite* mBtnSprite_0;
	Sprite* mBtnSprite_1;
};

