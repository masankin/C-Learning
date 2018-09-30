#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"

enum SHOPTYPE
{
	S_NULL,
	S_SHOP=1,
	S_PROP_PVE,
	S_PROP_PVP,
	S_GIFT,
};

class ShopLayer : public cocos2d::Layer
{
public:
	ShopLayer(void);
	virtual ~ShopLayer(void);

	struct BuyButtonInfo
	{
		cocos2d::ui::Text* mPrice;
		cocos2d::Sprite* mIcon;
	};

	void setBuyButton(std::vector<cocos2d::ui::Button*>);

	virtual bool init();

	virtual bool isCanMoveLeft(){return false;};
	virtual bool isCanMoveRight(){return false;};

	virtual void moveLeft();
	virtual void moveRight();

	virtual void onShow();
	virtual void onClose();

	virtual void onBuy(int postion){};
	std::vector<cocos2d::Node*> getContentVec(){return mContentVec;}
	std::vector<cocos2d::Node*> getDishVec(){return mDishVec;}
protected:
	virtual void updateContent();

	virtual int getShowItemSize() = 0;

	std::vector<BuyButtonInfo> mBuyBtnInfoVec;

	std::vector<cocos2d::Node*> mContentVec;
	std::vector<cocos2d::Node*> mDishVec;
	int mShowLeft;
	bool mIsShow;
};

