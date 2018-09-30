#pragma once

#include  "game/ui//system/ActionWindow.h"


class PowerDialog :public ActionWindow
{
public:
	PowerDialog();;
	~PowerDialog();;

	virtual bool init();
	virtual void onShow();
	virtual void onClose();

	CREATE_FUNC(PowerDialog);

	ui::Text*	mTimeText1;

	ui::Text*	mTitleText1;
	ui::Text*	mTitleText_1;

	ui::ImageView* mImage_2;

	CC_SYNTHESIZE(int, mIsGet, IsGet);//0为领取，1为未领取 
	
#ifdef MGAME_PLATFORM_MPLUS
	void showShareTips();
	void onShare(Ref*);
#endif
protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	ui::Button* mBtnPower;
	ui::Button* mBtnEnergy;

	cocostudio::Armature* mArmWan;
};
