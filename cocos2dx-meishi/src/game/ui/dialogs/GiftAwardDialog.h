#pragma once
#include  "ui/system/ActionWindow.h"
#include "MessageManager.h"
#include "data/TableManager.h"
#include "Constant.h"
#include "BoxRewardDialog.h"
class GiftLayer :public Layer
{

};

class GiftAwardDialog :public Window
{
public:
	GiftAwardDialog(void);
	~GiftAwardDialog(void);

	virtual bool init();
	CREATE_FUNC(GiftAwardDialog);

	static  GiftAwardDialog* showDialogByGiftId(int giftId, bool isShow = true);
	static  GiftAwardDialog* showDialogByChests(std::string);
	static  GiftAwardDialog* showDialogByGiftIds(int gift_id1, int gift_id2);

	static void showDialogByAward(std::map<int,int>);
	void setbGuideCallBack(bool bGuide = false, Node* target = NULL, SEL_CallFuncID fun = NULL, bool isForce = false);

	void clearGuideCallback();

	int mGiftID;
	int mGiftID2;

	void playAllItemAni2();
	void extraWork();

	std::function<void()> mCallBack;

protected:
	void onMenuItemAction(std::string, cocos2d::Ref*);
	void pushBackItem(int type, int id, int number);
	void removeAllItems();
	void onShow();

	// old 
	void playAllItemAni();
	void playTouchNext();

	// new
	Node* getItemNode(std::string iconName, int count);
	void playSigleItem(float dt);
	Vec2 getPosByIndex(int idx);
	void itemAniCall(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);

	void playItemHide(float dt);


private: 
	void showGiftTitle();
private:
	ListView* mItemList;
	std::vector<GiftInfo> mAwardVec;
	cocostudio::Armature* mArmItemBg;
	cocostudio::Armature* mArmItem;
	ui::Text*             mTitleLabel;
	ui::Layout* mPanelNext;
	bool bPanelNextTouched;
	bool mbGuideCallBack;

	bool mIsShowAward;
	bool mIsShowDailyMission;

	Node* mTarget;
	SEL_CallFuncID mFun;
	bool mIsForce;
	
	Node* mAniParentNode;

	cocos2d::Vector<cocostudio::Armature*> mItemAniArray;
	//std::vector<cocostudio::Armature*> mItemAniArray;
	int mItemCounter;

};

