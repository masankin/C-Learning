#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "data/TableManager.h"

enum eBoxRewardType
{
	BOX_REWARD_PVP_NOITEM = 0,
	BOX_REWARD_PVE_BASE,//通关
	BOX_REWARD_PVE_PERFECT,//三星
};

class BoxRewardDialog :public ActionWindow
{
public:
	BoxRewardDialog();
	~BoxRewardDialog();
	virtual bool init();
	void updateUI();
	static  BoxRewardDialog* showBoxRewardDialog(eBoxRewardType eType, int giftId, std::function<void()> callBackFunc = nullptr);
	static  BoxRewardDialog* showBoxRewardDialog(eBoxRewardType eType, std::string strBoxReward, std::function<void()> callBackFunc = nullptr);

	CREATE_FUNC(BoxRewardDialog);

	std::function<void()> mCallBack;
	eBoxRewardType mBoxRewardType;

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void displayBox();
	void playOpenBoxAni(float dt);
	void playItemAni();
	void hideItemAni(float dt);
	void pushBackItem(int type, int id, int number);
	void removeAllItems();
	void playSigleItem(float dt);
	void initBoxAni();
private:
	cocostudio::Armature* mArmBox;
	vector<cocostudio::Armature* > mArmItemVec;
	Node* mNodeBoxAni;
	Node* mNodeItemAni;
	ui::Layout* mPanelOpenBox;
	Sprite* mSpriteOpenBox;
	std::vector<GiftInfo> mAwardVec;
	bool mbOpenedBox;
	bool mbEnableClose;
	int mItemCounter;
	std::string msReward;
	int mGiftId;
};
