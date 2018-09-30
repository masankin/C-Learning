#pragma once
#include "ShopLayer.h"
#include "data/TableManager.h"
#include "Message.h"

using namespace cocos2d;
class ShopPropItem: public Node
{
public:
	ShopPropItem();
	~ShopPropItem();

	CREATE_FUNC(ShopPropItem);
	virtual bool init();

	void updateInfo(ItemBuyInfo*);
	CC_SYNTHESIZE(int, mInfoId, InfoId);

	CC_SYNTHESIZE(int, mPrice, Price);
	CC_SYNTHESIZE(std::string, mIcon, Icon);
private:
	ui::ImageView* mFlagtImg;
	ui::Text* mPropName;
	ui::Text* mDescribTxt;
	ui::Text* mNumberTxt;
	Node*   mIconNode;
};


class ShopPropLayer : public ShopLayer, public MessageHandler
{
public:
	ShopPropLayer(void);
	~ShopPropLayer(void);

	static ShopPropLayer* createWithType(SHOPTYPE type);
	virtual bool init(SHOPTYPE type);

	virtual bool isCanMoveLeft();
	virtual bool isCanMoveRight();

	virtual void onBuy(int postion);

private:
	virtual void onReceiveMassage(GameMessage * message);

	void initItem();
	virtual void updateContent();

	virtual int getShowItemSize(){ return mItemVec.size();};


	std::vector<ShopPropItem*> mItemVec;

	int iAfterBuyID;
	SHOPTYPE mType;
};

