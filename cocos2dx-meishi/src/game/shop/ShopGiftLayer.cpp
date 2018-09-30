#include "ShopGiftLayer.h"
#include "ShopProxy.h"
#include "MessageManager.h"
#include "ui/dialogs/GiftAwardDialog.h"
#include "ui/dialogs/GenerlDialog.h"
#include "game/login/LoginProxy.h"
#include "game/act/ActControl.h"
#include "core/CVTime.h"
#include "net/UserObj.h"
#include "utils/Comm.h"
using namespace ui;

ShopGiftItem::ShopGiftItem()
{

}

ShopGiftItem::~ShopGiftItem()
{

}

bool ShopGiftItem::init()
{
	if (!Node::init())
	{
		return false;
	}
	Node* pNode= CSLoader::createNode("ui/ShopItem3.csb");
	mDiscountNode = dynamic_cast<Node*>(pNode->getChildByName("discountNode"));
	mDiscountDi = dynamic_cast<ui::ImageView*>(pNode->getChildByName("Image_2"));
	mName = dynamic_cast<ui::Text*>(pNode->getChildByName("mPropNum"));
	mLeftNode = dynamic_cast<Node*>(pNode->getChildByName("leftNode"));
	ui::Text* ltimeTxt = dynamic_cast<ui::Text*>(mLeftNode->getChildByName("leftTimeTxt"));
	ui::Text* lcountTxt = dynamic_cast<ui::Text*>(mLeftNode->getChildByName("leftCountTxt"));
	mLeftTimeTxt = ExRichText::create();
	mLeftTimeTxt->setPosition(ltimeTxt->getPosition());
	mLeftTimeTxt->setPosition(Vec2(mLeftTimeTxt->getPositionX()+7.5,mLeftTimeTxt->getPositionY() + 17));
	mLeftTimeTxt->ignoreContentAdaptWithSize(false);
	mLeftTimeTxt->setSize(Size(150, 20));

	mLeftCountTxt = ExRichText::create();
	mLeftCountTxt->setPosition(lcountTxt->getPosition());
	mLeftCountTxt->setPositionY(mLeftTimeTxt->getPositionY() + 22);
	mLeftCountTxt->ignoreContentAdaptWithSize(false);
	mLeftCountTxt->setSize(Size(150, 20));

	mLeftNode->addChild(mLeftTimeTxt);
	mLeftNode->addChild(mLeftCountTxt);
	ltimeTxt->removeFromParent();
	lcountTxt->removeFromParent();

	mIconNode = dynamic_cast<Node*>(pNode->getChildByName("mLevelBg"));
	Node* lAwardNode = dynamic_cast<Node*>(pNode->getChildByName("mAward_Node"));
	for (int i = 1; i <= 6; i++)
	{
		Node* pNode1 = dynamic_cast<Node*>(lAwardNode->getChildByName(
			__String::createWithFormat("award_node_%d", i)->getCString()
			));
		pNode1->setVisible(false);
		mAwardNode.push_back(pNode1);
	}


	addChild(pNode);
	return true;
}

void ShopGiftItem::updateInfo(ShopInfo* info)
{
	mName->setString(info->name);
	Sprite* bg = Sprite::createWithSpriteFrameName(info->giftIcon);
	bg->setScaleX(1.2);
	mIconNode->addChild(bg);
	
	mGiftType = (GIFTTYPE)info->giftType;
	if (info->discount == 0)
	{
		mDiscountNode->setVisible(false);
		mDiscountDi->setVisible(false);
		mLeftNode->setVisible(false);
	}else
	{
		Sprite* discountSp = Sprite::createWithSpriteFrameName(
			__String::createWithFormat("ui/Shop/shop_0%d.png", info->discount)->getCString());
		mDiscountNode->addChild(discountSp);
	}
	const GiftAward *pGiftAward = sgTableManager()->getGiftAward(info->giftId);
	auto awardNode_it = mAwardNode.begin();
	//////////////////////////////////////////////////////////////////////////
	if (!pGiftAward || pGiftAward->mGiftData.size() == 0)
	{
		return;
	}
	for (int i = 0; i < pGiftAward->mGiftData.size(); i++)
	{
		GiftInfo lGiftInfo = pGiftAward->mGiftData.at(i);
		const GoodsInfo* lGoodsInfo = sgTableManager()->getGoodsInfo(lGiftInfo.mType, lGiftInfo.mId);
		if (!lGoodsInfo)
		{
			break;
		}
		(*awardNode_it)->setVisible(true);
		dynamic_cast<ImageView*>((*awardNode_it)->getChildByName("award_img"))->
			loadTexture(lGoodsInfo->mIcon, TextureResType::PLIST);
		dynamic_cast<Text*>((*awardNode_it)->getChildByName("award_count"))->setString(
			__String::createWithFormat("x%d", lGiftInfo.mNumber)->getCString());
		if (++awardNode_it == mAwardNode.end())
		{
			break;
		}
	}
	
	setInfoId(info->id);
	if (info->gold == 0)
	{
		setIsBuyMoney(false);
		setPrice(info->priceGold);
		setIcon("ui/Shop/shop_iconDiamond.png");
	}else
	{
		setIsBuyMoney(true);
		setPrice(info->gold);
		setIcon("ui/Shop/Shop_rmb_mark.png");
	}
}

void ShopGiftItem::updateTime()
{
	if (mGiftType == gift_newPlayer)
	{
		return;
	}
	ShopList mShopList = sgTableManager()->getGiftList();
	auto infoIt = mShopList.find(mInfoId);
	if (infoIt != mShopList.end())
	{
		ShopInfo* info = infoIt->second;
		std::string lleftCountStr = StringUtils::format(
			getRichFormatText(
			sgTextHelper(TEXT_SHOP_LEFTCOUNT)).c_str(), info->leftCount
			);
		mLeftCountTxt->setText(lleftCountStr);

		//CVTime lendCvTime(info->endTime);//
		int leftTime = atoi(info->endTime.c_str()) - sgUserObj()->getNowTime();//
		if (leftTime > 0)
		{
			int days = leftTime / ( 60 * 60 * 24);
			if (days >365)
			{
				return;
			}
			int hours = (leftTime - days*( 60 * 60 * 24)) / ( 60 * 60);
			if (days>0)
			{
				mLeftTimeTxt->setText(
					StringUtils::format(
					getRichFormatText(
					sgTextHelper(TEXT_SHOP_LEFTTIME_DAY)).c_str(), days, hours));
			}
			else
			{
				int minutes = (leftTime - days*( 60 * 60 * 24) - hours*( 60 * 60)) / ( 60);
				mLeftTimeTxt->setText(
					StringUtils::format(
					getRichFormatText(
					sgTextHelper(TEXT_SHOP_LEFTTIME_HOUR)).c_str(), hours, minutes));
			}
			
		}
	}
}

ShopGiftLayer::ShopGiftLayer(void)
{
	mShowItemVec.clear();
	sgMessageManager()->regisiterMessageHandler(EMT_SHOP_PAYCOMPLENT,this);
	sgMessageManager()->regisiterMessageHandler(EMT_UPDATE_SHOPLIST,this);

}


ShopGiftLayer::~ShopGiftLayer(void)
{
	sgMessageManager()->removeMessageHandler(this);
	for (auto it = mItemMap.begin(); it != mItemMap.end(); it++)
	{
		CC_SAFE_RELEASE(it->second);
	}
}

bool ShopGiftLayer::init()
{
	if (!ShopLayer::init())
	{
		return false;
	}

	//initItem();
	return true;
}

void ShopGiftLayer::onShow()
{
	mIsShow = true;
	for (int i = 0; i < mBuyBtnInfoVec.size(); i++)
	{
		((Button*)mBuyBtnInfoVec[i].mPrice->getParent())->setVisible(false);
	}
	
	sgShopProxy()->sendShopList();

	//ShopLayer::onShow();
}

void ShopGiftLayer::initItem()
{
	ShopList mShopList = sgTableManager()->getGiftList();
	for (auto it = mShopList.begin(); it!= mShopList.end(); it++)
	{
		ShopInfo* lshopInfo = it->second;
		if (mItemMap.find(lshopInfo->id) == mItemMap.end())
		{
			ShopGiftItem* pItem = ShopGiftItem::create();
			pItem->updateInfo(lshopInfo);
			mItemMap.insert(std::make_pair(lshopInfo->id,pItem));
			CC_SAFE_RETAIN(pItem);
		}
	}
}

 void ShopGiftLayer::updateContent()
{
	for (int i = 0; i<3; i++)
	{
		mContentVec[i]->removeAllChildren(); 
	}
	for (int i = 0; i < 3; i++)
	{
		if (mShowLeft + i >= mShowItemVec.size())
		{
			continue;
		}

		mContentVec[i]->addChild(mShowItemVec[mShowLeft + i]);

		((Button*)mBuyBtnInfoVec[i].mPrice->getParent())->setVisible(true);
		mBuyBtnInfoVec[i].mPrice->setVisible(true);
		mBuyBtnInfoVec[i].mIcon->setVisible(true);
		mBuyBtnInfoVec[i].mPrice->setString(StringUtils::format("%d", mShowItemVec[mShowLeft + i]->getPrice()));
		mBuyBtnInfoVec[i].mIcon->setSpriteFrame(mShowItemVec[mShowLeft + i]->getIcon());
		mShowItemVec[mShowLeft + i]->updateTime();
	}
}

 bool ShopGiftLayer::isCanMoveLeft()
 {
	 return (mShowLeft - 1)>=0?true:false;
 }

 bool ShopGiftLayer::isCanMoveRight()
 {
	 return (mShowLeft +2 + 1)< mShowItemVec.size()?true:false;
 }

 void ShopGiftLayer::onBuy(int postion)
 {
	 int itemID = mShowItemVec[mShowLeft+postion]->getInfoId();
	 if (mShowItemVec[mShowLeft+postion]->getIsBuyMoney())
	 {
		 auto shopList = sgTableManager()->getGiftList();
		 auto it = shopList.find(itemID);
		 if (it != shopList.end())
		 {
			 ShopInfo* info = it->second;

#ifdef	MGAME_IN_APP_SDK

			 std::string productName = StringUtils::format("%d", info->manual);
			 float productPrice = info->gold;
			 std::string productDescribe = info->text;


			 std::string name = sgUserObj()->getPlayerName();
			 int	pid = sgUserObj()->getUin();
			 int gold = sgUserObj()->getPersonalInfo()->diamond();//sgUserObj()->getGoldCoin();
			 productDescribe = StringUtils::format("%d#%d#%s", gold, pid, name.c_str());

			 // 小米平台支付先判断是否成功登陆，没登陆成功而付费成功，会导致冲值不到账问题(暂时处理)
			 if (!sgLoginProxy()->checkLogin(true)) {
				 return;
			 }

			 sgChannelApi()->channelPay(info->id, productName, productPrice, productDescribe);
#else
			 sgMessageManager()->sendMessage(EMT_SHOP_PAYCOMPLENT,__String::createWithFormat("%d,%s",info->id,"")->getCString());

#endif
		 }
	 }else
	 {
		 //if (sgUserObj()->getGoldCoin() >= mShowItemVec[mShowLeft + postion]->getPrice())
		 if (sgUserObj()->getPersonalInfo()->diamond() >= mShowItemVec[mShowLeft + postion]->getPrice())
		 {
			 auto shopList = sgTableManager()->getGiftList();
			 auto it = shopList.find(itemID);
			 if (it != shopList.end())
			 {
				 mBuyItemId = itemID;
				 mGiftType = mShowItemVec[mShowLeft + postion]->getGiftType();

				 ShopInfo* info = it->second;
				 GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), 
					 StringUtils::format(sgTextHelper(TEXT_SHOP_BUY_CONFIRM).c_str(), 
					 mShowItemVec[mShowLeft + postion]->getPrice(),
					 info->name.c_str()),
					 CC_CALLBACK_1(ShopGiftLayer::onConfirmBuy, this));
			 }
			 else
			 {
				 CCLOG("[trace] ShopGiftLayer::onBuy gTableManager()->getShopList nil by id = %d", itemID);
			 }
			 
		 }
		 else
		 {
			 GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_BUY_ITEM_NO_EN), CC_CALLBACK_1(ShopGiftLayer::onCallBackshowShop, this));
		 }
	 }
 }

 void ShopGiftLayer::onCallBackshowShop(Ref* ref)
 {
	 GenerlDialog::hide();
	 sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, "1");
 }
 
  void ShopGiftLayer::onConfirmBuy(Ref* ref)
 {
	 //GenerlDialog::hide();
	 if (mGiftType == gift_newPlayer)
	 {
		 sgActControl()->getActProxy()->sendBuyActNewerGift(mBuyItemId);
	 }
	 else
	 {
		 sgShopProxy()->sendShopBuy(mBuyItemId);
	 }
 }

 void ShopGiftLayer::onReceiveMassage(GameMessage * message)
 {
	 if (message->getMessageType() == EMT_SHOP_PAYCOMPLENT)
	 {
		 MessageShopPayComplent* pMessage = dynamic_cast<MessageShopPayComplent*>(message);
		 int productId = pMessage->getProductId();
		 std::string orderId = pMessage->getOrderId();

		 int totalDiamond = 0;

		 auto shopList = sgTableManager()->getGiftList();
		 auto it = shopList.find(productId);
		 if (it != shopList.end())
		 {
			ShopInfo* info = it->second;
			GiftAwardDialog::showDialogByGiftId(info->giftId);
		 }
	 }else if (message->getMessageType() == EMT_UPDATE_SHOPLIST)
	 {
		 updateShowItem();
	 }
 }

 void ShopGiftLayer::updateShowItem()
 {
	 initItem();
	 mShowItemVec.clear();
	 ShopList mShopList = sgTableManager()->getGiftList();

	 for (auto it = mShopList.begin(); it!= mShopList.end(); it++)
	 {
		 ShopInfo* lshopInfo = it->second;
		 if (lshopInfo->isShow || (lshopInfo->giftType == gift_newPlayer && sgActControl()->getIsNewerGiftOpened()))
		 {
			 auto itemit =  mItemMap.find(lshopInfo->id);
			 if (itemit != mItemMap.end())
			 {
				 mShowItemVec.push_back((itemit)->second);
			 }
		 }
	 }
	 if (mIsShow)
		 ShopLayer::onShow();
 }
