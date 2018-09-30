#pragma once

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "game/shop/HonorShopControl.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;

class HonorShopItem : public TableViewCell
{
public:
	HonorShopItem();
	virtual ~HonorShopItem(); 

	bool		init();
	void		onExchangeCallback(cocos2d::Ref* caller);
	void		updateItem(int exchangeId);
	void		updateTime(float dt);
	CREATE_FUNC(HonorShopItem);
private:
	void		showTips();
	std::string	getTimeStrByTimeNum(int secs);
	void		onItemTouch(Ref* ref, ui::Widget::TouchEventType type);
	void		showItemDes();
	void		hideItemDes();
	bool        checkPowerLimit();
private:
	cocos2d::Node *mRoot;
	int			   mExchangeId;
	cocos2d::Node*			mItemNode;
	ui::ImageView*			mItemIcon;
	ui::Text*				mItemNumLabel;
	ui::Text*				mNameLabel;
	ui::Text*				mHonorLabel;
	ui::Text*				mLimitLabel;
	ui::Text*				mTimeLabel;
	ui::Text*				mAllCountlabel;
	cocos2d::Sprite*		mLimitBg;
	ui::Button*				mExchangeBtn;
	cocos2d::Sprite*		mAbleBtnSprite;
	cocos2d::Sprite*		mUnableBtnSprite;

	UserObj::PROPERTYTYPE   mItemType;
	int						mItemId;
	int						mItemCount;

	HonorShopItemData*		mItemData;
	Color3B                 mHonorColor;
};
