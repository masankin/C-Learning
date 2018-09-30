#include "ShopStoreLayer.h"
#include "ui/dialogs/GenerlDialog.h"
#include "MessageManager.h"
#include "ShopProxy.h"
#include "core/SoundManager.h"
#include "game/login/LoginProxy.h"
using namespace ui;
using namespace cocostudio;
ShopStoreItem::ShopStoreItem()
{

}

ShopStoreItem::~ShopStoreItem()
{

}

bool ShopStoreItem::init()
{
	if (!Node::init())
	{
		return false;
	}
	Node* pNode= CSLoader::createNode("ui/ShopItem1.csb");
	mDiamondTxt = dynamic_cast<Text*>(pNode->getChildByName("mPropNum"));
	mDescribTxt = dynamic_cast<Text*>(pNode->getChildByName("Text_3"));
	mIconNode = dynamic_cast<Node*>(pNode->getChildByName("mLevelBg"));
	mMonthcartImg = dynamic_cast<ui::ImageView*> (pNode->getChildByName("mLogo"));
	mMonthcartImg->setVisible(true);
	addChild(pNode);

	return true;
}

void ShopStoreItem::updateInfo(ShopInfo* shopinfo)
{
	mDiamondTxt->setString(StringUtils::format(sgTextHelper(TEXT_BUY_DIAMOND_COUNT).c_str(), shopinfo->manual));
	mDescribTxt->setText(shopinfo->text);
	Sprite* bg = Sprite::createWithSpriteFrameName(shopinfo->img);
	mIconNode->addChild(bg);

	changeFlag(shopinfo->energy,true);

	setInfoId(shopinfo->id);
	setPrice(shopinfo->gold);
	setIcon("ui/Shop/Shop_rmb_mark.png");
}

void ShopStoreItem::changeFlag(int type, bool isShow)
{
	if (type == 1)
	{
		mMonthcartImg->loadTexture("ui/Shop/Shop_Purchase_yueka.png",ui::Widget::TextureResType::PLIST);
	}else if (type == 5&&isShow)
	{
		mMonthcartImg->loadTexture("ui/Shop/Shop_Purchase_shouchong.png",ui::Widget::TextureResType::PLIST);
	}else
	{
		mMonthcartImg->setVisible(false);
		mDescribTxt->setVisible(false);
	}
}

ShopStoreLayer::ShopStoreLayer(void):mArmatureDiamond(NULL)
{
	sgMessageManager()->regisiterMessageHandler(EMT_SHOP_PAYCOMPLENT,this	);
	sgMessageManager()->regisiterMessageHandler(EMT_UPDATE_SHOPLIST,this	);

}


ShopStoreLayer::~ShopStoreLayer(void)
{
	sgMessageManager()->removeMessageHandler(this);
	for (auto it = mItemVec.begin(); it != mItemVec.end(); it++)
	{
		CC_SAFE_RELEASE(*it);
	}
}

bool ShopStoreLayer::init()
{
	if (!ShopLayer::init())
	{
		return false;
	}

	initItem();

	return true;
}

void ShopStoreLayer::initItem()
{
	ShopList mShopList = sgTableManager()->getShopList();

	for (auto it = mShopList.begin(); it!= mShopList.end(); it++)
	{
		ShopStoreItem* pItem = ShopStoreItem::create();
		pItem->updateInfo(it->second);
		mItemVec.push_back(pItem);
		CC_SAFE_RETAIN(pItem);
	}
	sgShopProxy()->sendShopList();
}

void ShopStoreLayer::onShow()
{
	ShopLayer::onShow();
	
}

void ShopStoreLayer::updateContent()
{
	
	for (int i = 0; i<3; i++)
	{
		mContentVec[i]->removeAllChildren();
	}
	for (int i = 0; i<3; i++)
	{
		if (mShowLeft + i >= mItemVec.size())
		{
			((Button*)mBuyBtnInfoVec[i].mPrice->getParent())->setVisible(false);
			continue;
		}
		mContentVec[i]->addChild(mItemVec[mShowLeft+i]);

		((Button*)mBuyBtnInfoVec[i].mPrice->getParent())->setVisible(true);
		mBuyBtnInfoVec[i].mPrice->setVisible(true);
		mBuyBtnInfoVec[i].mIcon->setVisible(true);
		mBuyBtnInfoVec[i].mPrice->setString(StringUtils::format("%d",mItemVec[mShowLeft+i]->getPrice()));
		mBuyBtnInfoVec[i].mIcon->setSpriteFrame(mItemVec[mShowLeft+i]->getIcon());
#ifndef MGAME_PLATFORM_MPLUS
		mBuyBtnInfoVec[i].mIcon->setScale(1.0);
#else
		mBuyBtnInfoVec[i].mIcon->setScale(0.7);
#endif
	}
	ShopLayer::updateContent();
}

 bool ShopStoreLayer::isCanMoveLeft()
{
	return (mShowLeft - 1)>=0?true:false;
}

 bool ShopStoreLayer::isCanMoveRight()
{
	return (mShowLeft +2 + 1)< mItemVec.size()?true:false;
}

  void ShopStoreLayer::onBuy(int postion)
 {
	 if (!mArmatureDiamond)
	 {
		 mArmatureDiamond = cocostudio::Armature::create("fla_zhuanshi");
		 Director::getInstance()->getNotificationNode()->addChild(mArmatureDiamond,INT_MAX);
		 Size pSize = Director::getInstance()->getWinSize();
		 mArmatureDiamond->setPosition(pSize.width/2 + 20, pSize.height/2 + 10);
		 mArmatureDiamond->setVisible(false);
	 }

	 int itemID = mItemVec[mShowLeft+postion]->getInfoId();
	 auto shopList = sgTableManager()->getShopList();
	 auto it = shopList.find(itemID);
	 if (it != shopList.end())
	 {
		 ShopInfo* info = it->second;
		 if (!sgChannelApi()->isOnlineSdk() &&sgUserObj()->isCostMax(info->gold))
		 {
			 GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_COMMITPS_CHARGMAX));
			 return;
		 }
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
 }

  void ShopStoreLayer::onReceiveMassage(GameMessage * message)
  {
	  if (message->getMessageType() == EMT_SHOP_PAYCOMPLENT)
	  {
		  sgSoundManager()->playMusicEffect(SOUND_ITEM_GAIN);

		  sgUserObj()->setIsShopBuy(true);
		  MessageShopPayComplent* pMessage = dynamic_cast<MessageShopPayComplent*>(message);
		  int productId = pMessage->getProductId();
		  std::string orderId = pMessage->getOrderId();

		  int totalDiamond = 0;

		  auto shopList = sgTableManager()->getShopList();
		  auto it = shopList.find(productId);
		  if (it != shopList.end())
		  {
			  ShopInfo* info = it->second;
			  int price =info->manual;
			  totalDiamond = price;

			  if (sgChannelApi()->isOnlineSdk() == false)
			  {
				  sgUserObj()->addCostMoney(info->gold);
			  }
			  else
			  {
#if CC_PLATFORM_WIN32 ==  CC_TARGET_PLATFORM
				  sgUserObj()->addTestMoney(info->manual);
#endif 
			  }

			  if (!mArmatureDiamond)
			  {
				  std::string param = StringUtils::format("%d %d %d %d %d %s"
					  , 1
					  , 1
					  , eVituralCurrencyType::Diamond
					  , totalDiamond
					  , sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
					  , "ShopDialog::onDiamondMovement");
				  sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
				  sgMessageManager()->sendMessage(EMT_SHOP_AFTER_BUYDIAMOND,"");
				  return;
			  }
			  mArmatureDiamond->setVisible(true);
			  mArmatureDiamond->stopAllActions();
			  mArmatureDiamond->getAnimation()->play("born");
			  mArmatureDiamond->getAnimation()->setMovementEventCallFunc(std::bind(
				  &ShopStoreLayer::onDiamondMovement, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, totalDiamond, sgUserObj()->getPersonalInfo()->diamond() - totalDiamond));//sgUserObj()->getGoldCoin() - totalDiamond));

			  sgShopProxy()->sendShopList();
		  }
	  }else if (message->getMessageType() == EMT_UPDATE_SHOPLIST)
	  {
		  for (int i = 0; i<mItemVec.size(); i++)
		  {
			  const ShopInfo* lshopInfo = sgTableManager()->getShopInfoById(mItemVec[i]->getInfoId());
			  mItemVec[i]->changeFlag(lshopInfo->energy,lshopInfo->isFirst);
		  }
		 
	  }
  }

  void ShopStoreLayer::onDiamondMovement(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName, int diamond, int curDiamond)
  {
	  if (type == MovementEventType::COMPLETE)
	  {
		  if (aniName == "born")
		  {
			  mArmatureDiamond->getAnimation()->play("stand");
			  mArmatureDiamond->getAnimation()->setMovementEventCallFunc(std::bind(&ShopStoreLayer::onDiamondMovement, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, diamond, curDiamond));

			  std::string param = StringUtils::format("%d %d %d %d %d %s"
				  , 0
				  , 2
				  , eVituralCurrencyType::Diamond
				  , 0
				  , curDiamond
				  , "ShopDialog::onDiamondMovementStart");

			  sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
		  }
		  else if (aniName == "end")
		  {
			  mArmatureDiamond->removeFromParent();
			  mArmatureDiamond = NULL;
			 
			  sgMessageManager()->sendMessage(EMT_SHOP_AFTER_BUYDIAMOND,"");

			  std::string param = StringUtils::format("%d %d %d %d %d %s"
				  , 0
				  , 2
				  , eVituralCurrencyType::Diamond
				  , diamond
				  , curDiamond
				  , "ShopDialog::onDiamondMovementEnd");

			  sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
			  sgUserObj()->setIsShopBuy(false);
		  }
	  }else if (type == MovementEventType::LOOP_COMPLETE)
	  {
		  if(aniName == "stand")
		  {
			  mArmatureDiamond->getAnimation()->play("end");
			  mArmatureDiamond->getAnimation()->setMovementEventCallFunc(std::bind(&ShopStoreLayer::onDiamondMovement, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, diamond, curDiamond));
		  }
	  }
  }
