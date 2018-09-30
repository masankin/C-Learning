#pragma once
#include "ActLayout.h"
#include "cocos-ext.h"
#include "cocos2d.h"
#include  "game/ui/system/Window.h"
#include "game/ui/system/ActionWindow.h"
USING_NS_CC_EXT;
USING_NS_CC;

class ActFirstCharge:public ActionWindow
{
public:
	ActFirstCharge(void);
	~ActFirstCharge(void);

	static ActFirstCharge *create();

	virtual bool init();
	//virtual void updateInfo();

	void initView();
	void showItemInfo(Node* itemNode, std::string iconName, int count, int itemType = 0);

	void onShow();

	void updateBtn();

	//void _onMenuAction(cocos2d::Ref*);
	void onMenuItemAction(std::string cbName, cocos2d::Ref*sender);

protected:

	Node* mItemNode[8];

	ui::Button* mGetBtn;
	ui::Button* mChargeBtn;

	Sprite* mBtnSprite_0;
	Sprite* mBtnSprite_1;

	cocostudio::Armature* mArmature;
};

