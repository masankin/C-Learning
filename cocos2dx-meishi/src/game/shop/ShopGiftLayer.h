#pragma once

#include "ShopLayer.h"
#include "Message.h"
#include "data/TableManager.h"
#include "core/ExRichText.h"

using namespace cocos2d;

enum GIFTTYPE
{
	gift_null		= 0,
	gift_common,
	gift_newPlayer,
};
class ShopGiftItem: public Node
{
public:

	ShopGiftItem();
	~ShopGiftItem();

	CREATE_FUNC(ShopGiftItem);
	virtual bool init();

	void updateInfo(ShopInfo*);
	void updateTime();
	CC_SYNTHESIZE(int, mInfoId, InfoId);
	CC_SYNTHESIZE(int, mPrice, Price);
	CC_SYNTHESIZE(std::string, mIcon, Icon);
	CC_SYNTHESIZE(bool , mIsBuyMoney, IsBuyMoney);
	CC_SYNTHESIZE(GIFTTYPE, mGiftType, GiftType);
private:
	Node* mDiscountNode;
	ui::ImageView* mDiscountDi;
	ui::Text* mName;
	Node* mLeftNode;
	ExRichText* mLeftTimeTxt;
	ExRichText* mLeftCountTxt;
	Node*   mIconNode;
	std::vector<Node*> mAwardNode;
};

class ShopGiftLayer : public ShopLayer, public MessageHandler
{
public:
	ShopGiftLayer(void);
	~ShopGiftLayer(void);

	virtual bool init();
	CREATE_FUNC(ShopGiftLayer);

	virtual void onShow();

	virtual bool isCanMoveLeft();
	virtual bool isCanMoveRight();

	virtual void onBuy(int postion);
private:
	void initItem();
	virtual void updateContent();
	virtual void onReceiveMassage(GameMessage * message);

	void updateShowItem();
	void onCallBackshowShop(Ref* ref);
	void onConfirmBuy(Ref* ref);
	virtual int getShowItemSize(){ return mShowItemVec.size();};

	std::map<int,ShopGiftItem*> mItemMap;//所有的礼包

	std::vector<ShopGiftItem*> mShowItemVec;//需要显示的礼包
	int			mBuyItemId;
	GIFTTYPE	mGiftType;
};

