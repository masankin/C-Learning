#pragma once
#include "ShopLayer.h"
#include "data/TableManager.h"
#include "Message.h"

using namespace cocos2d;


class ShopStoreItem: public Node
{
public:
	ShopStoreItem();
	~ShopStoreItem();

	CREATE_FUNC(ShopStoreItem);
	virtual bool init();

	void updateInfo(ShopInfo*);
	void changeFlag(int type, bool isShow);
	CC_SYNTHESIZE(int, mInfoId, InfoId);

	CC_SYNTHESIZE(int, mPrice, Price);
	CC_SYNTHESIZE(std::string, mIcon, Icon);
private:
	ui::ImageView* mMonthcartImg;
	ui::Text* mDiamondTxt;
	ui::Text* mDescribTxt;
	Node*   mIconNode;
};

class ShopStoreLayer :public ShopLayer, public MessageHandler
{
public:
	ShopStoreLayer(void);
	~ShopStoreLayer(void);

	CREATE_FUNC(ShopStoreLayer);
	virtual bool init();

	virtual void onShow();

	virtual bool isCanMoveLeft();
	virtual bool isCanMoveRight();

	virtual void onBuy(int postion);

private:
	void initItem();
	virtual void updateContent();
	virtual int getShowItemSize(){ return mItemVec.size();};
	virtual void onReceiveMassage(GameMessage * message);
	void onDiamondMovement(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName, int diamond, int curDiamond);

	std::vector<ShopStoreItem*> mItemVec;

	cocostudio::Armature* mArmatureDiamond;
};

