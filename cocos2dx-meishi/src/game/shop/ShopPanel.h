#pragma once

#include "game/ui/system/ActionWindow.h"
#include "ShopLayer.h"
#include "Message.h"

class ShopPanel :public ActionWindow, public MessageHandler
{
public:
	ShopPanel(void);
	~ShopPanel(void);

	CREATE_FUNC(ShopPanel);
	virtual bool init();

	static void showShop();

	struct PropStruct
	{
		Button* mBtn;
		ImageView* mNorImg;
		ImageView* mSelectImg;
	};

private:
	virtual void onReceiveMassage(GameMessage * message);

	void updateConten();
	void changeContent(SHOPTYPE);
	void setTypeImageSelect(SHOPTYPE, bool);

	void refreshContent(bool isRight);
	void onFreshMovement(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName, bool isRight );

	virtual void onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex);


	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	virtual void onShow();
	virtual void onClose();
	void onChangePropByType(SHOPTYPE type);

	ShopLayer* createShopLayer(SHOPTYPE);

	void doGuideNode(float dt);


	std::map<int, ImageView*> mShopButtonMap;

	SHOPTYPE mSelectType;

	std::vector< cocostudio::Armature*> mRefreshDishArmVec;

	std::map<int, ShopLayer*> mShopLayerMap;

	std::vector<ui::Button*> mBuyBtnVec;
	cocos2d::Node* mContentNode;

	ShopLayer* mShowShopLayer;
	
	cocos2d::ClippingNode* clipNode1;
	cocos2d::ClippingNode* clipNode2;

	bool mIsChanging;
	Node* mPropChangeBtn;
	PropStruct mPropPve;
	PropStruct mPropPvp;
	/*ImageView* mPveImg;
	ImageView* mPvpImg;
	ImageView* mPropChangeImgBg;
	ImageView* mPropChangeImg;*/

	CC_SYNTHESIZE(bool, mPropIsPve, PropIsPve);
	CC_SYNTHESIZE(bool, mOnPropTag, OnPropTag);
};



