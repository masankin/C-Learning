#pragma once
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;


class ActLoginItem :public ui::Widget
{
public:
	ActLoginItem(void);
	~ActLoginItem(void);
	enum BtnStatus
	{
		BUTTON_GETED =1,//已领取
		BUTTON_CANGET,//可领取
		BUTTON_NOTCANGET,//不可领取
	};
	CREATE_FUNC(ActLoginItem);
	virtual bool init();

	void updateInfo(int day ,bool isGet, bool isCanGet, int giftId);

	Node* getGuideNode();

	void onTouch();
protected:
	void onClickCallBack(Ref*);
private:
	bool mIsOnTouch;
	ui::Text* mDay;
	std::string mDayStr;

	ui::ImageView* mIcon;
	ui::Text* mNum;
	ui::Text* mName;

	ui::ImageView* mBtnName;
	ui::ImageView* mBtnName1;
	ui::Button* mBtn;
	ui::ImageView* mTip;
	BtnStatus mBtnStatus;

	// 特效节点
	Node* mEffectNode;
};

