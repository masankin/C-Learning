#pragma once
#include "cocos-ext.h"
#include "cocos2d.h"
#include  "game/ui//system/ActionWindow.h"
USING_NS_CC_EXT;
USING_NS_CC;

class ActNewerPacksDialog :public ActionWindow
{
public:
	ActNewerPacksDialog(void);
	~ActNewerPacksDialog(void);

	static ActNewerPacksDialog* create();

	virtual bool init();
	virtual void onShow();
	void initUI();

	//void onShow();
	void onCallBack(cocos2d::Ref* ref);
protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	void onCallBackBuyDiamond(Ref*);

	Node* mNodeItem[5];
	ui::Button* mBtnBack;
	ui::Button* mBtnBuy;
	ui::Text* mTextInfo;
	int mGiftId;
};

