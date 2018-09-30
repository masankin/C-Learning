#include "ShopPropLayer.h"
#include "guide/GuideManager.h"
#include "ui/dialogs/ShopDialog.h"
#include "MessageManager.h"
#include "prop/PropsControl.h"
#include "core/HelperFunc.h"

using namespace ui;
using namespace cocostudio;

ShopPropItem::ShopPropItem()
{
	
}

ShopPropItem::~ShopPropItem()
{

}

bool ShopPropItem::init()
{
	if (!Node::init())
	{
		return false;
	}
	Node* pNode= CSLoader::createNode("ui/ShopItem2.csb");
	mPropName = dynamic_cast<Text*>(pNode->getChildByName("mPropNum"));
	mDescribTxt = dynamic_cast<Text*>(pNode->getChildByName("Text_3"));
	mIconNode = dynamic_cast<Node*>(pNode->getChildByName("mLevelBg"));
	mNumberTxt = dynamic_cast<Text*>(pNode->getChildByName("Text_1"));
	mFlagtImg = dynamic_cast<ui::ImageView*> (pNode->getChildByName("flagImg"));
	mFlagtImg->setVisible(true);
	addChild(pNode);

	return true;
}

void ShopPropItem::updateInfo(ItemBuyInfo* info)
{
	mDescribTxt->setString(info->text);
	Sprite* pSp =Sprite::createWithSpriteFrameName(info->icon);
	mIconNode->addChild(pSp);
	const PropInfo* pProp =  sgTableManager()->getPropInfoById(info->PropId);
	mPropName->setString(pProp->name);
	mNumberTxt->setString(StringUtils::format("%d", info->num));

	setInfoId(info->id);
	setPrice(info->moneynum);
	setIcon("ui/Shop/shop_iconDiamond.png");
	const PropInfo *  lPropInfo = sgTableManager()->getPropInfoById( info->PropId);
	if (lPropInfo->isPve)
	{
		mFlagtImg->loadTexture("ui/Shop/Shop_Purchase_PVE.png",ui::Widget::TextureResType::PLIST);
	}else
	{
		mFlagtImg->loadTexture("ui/Shop/Shop_Purchase_PVP.png",ui::Widget::TextureResType::PLIST);
	}
}


ShopPropLayer::ShopPropLayer(void)
{
	sgMessageManager()->regisiterMessageHandler(EMT_SHOP_AFTER_BUYDIAMOND,this);
}


ShopPropLayer::~ShopPropLayer(void)
{
	sgMessageManager()->removeMessageHandler(this);
	for (auto it = mItemVec.begin(); it != mItemVec.end(); it++)
	{
		CC_SAFE_RELEASE(*it);
	}
}

ShopPropLayer* ShopPropLayer::createWithType(SHOPTYPE type)
{ 
	ShopPropLayer *pRet = new(std::nothrow) ShopPropLayer(); 
	if (pRet && pRet->init(type))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool ShopPropLayer::init(SHOPTYPE type)
{
	if (!ShopLayer::init())
	{
		return false;
	}
	mType = type;
	initItem();
	return true;
}

void ShopPropLayer::initItem()
{
	ItemBuyList mItemList = sgTableManager()->getItemBuyList();

	for (auto it = mItemList.begin(); it!= mItemList.end(); it++)
	{
		auto second = it->second;
		const PropInfo *  lPropInfo = sgTableManager()->getPropInfoById(second->PropId);
		if ((lPropInfo->isPve && mType == S_PROP_PVE) || (!lPropInfo->isPve && mType == S_PROP_PVP))
		{
			ShopPropItem* pItem = ShopPropItem::create();
			pItem->updateInfo(second);
			mItemVec.push_back(pItem);
			CC_SAFE_RETAIN(pItem);
		}
	}
}

void ShopPropLayer::updateContent()
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
		mContentVec[i]->addChild(mItemVec[mShowLeft + i]);

		((Button*)mBuyBtnInfoVec[i].mPrice->getParent())->setVisible(true);
		mBuyBtnInfoVec[i].mPrice->setVisible(true);
		mBuyBtnInfoVec[i].mIcon->setVisible(true);
		mBuyBtnInfoVec[i].mPrice->setString(StringUtils::format("%d",mItemVec[mShowLeft+i]->getPrice()));
		mBuyBtnInfoVec[i].mIcon->setSpriteFrame(mItemVec[mShowLeft+i]->getIcon());
		mBuyBtnInfoVec[i].mIcon->setScale(1.0);
	}

}

bool ShopPropLayer::isCanMoveLeft()
{
	return (mShowLeft - 1)>=0?true:false;
}

bool ShopPropLayer::isCanMoveRight()
{
	return (mShowLeft +2 + 1)< mItemVec.size()?true:false;
}

 void ShopPropLayer::onBuy(int postion)
{
	int itemID = mItemVec[mShowLeft+postion]->getInfoId();
	auto itmeList = sgTableManager()->getItemBuyList();
	auto it = itmeList.find(itemID);
	if (it != itmeList.end())
	{
		sgGuideManager()->unRegistGuideMessage();


		iAfterBuyID = it->second->PropId;
		sgMessageManager()->removeMessageHandler(EMT_SHOP_AFTER_BUYDIAMOND);
		sgMessageManager()->regisiterMessageHandler(EMT_SHOP_AFTER_BUYDIAMOND,this);
		ShopDetailed::show(it->second->PropId); 
	} 
}

 void ShopPropLayer::onReceiveMassage(GameMessage * message)
 {
	 if (message->getMessageType() == EMT_SHOP_AFTER_BUYDIAMOND)
	 {
		 const PropInfo* pPorp = sgTableManager()->getPropInfoById( iAfterBuyID
			 );
		 int coin = sgUserObj()->getPersonalInfo()->diamond();//sgUserObj()->getGoldCoin();

		 int pItemIdx = 0;
		 ItemBuyList pItemList = sgTableManager()->getItemBuyList();
		 for (auto it = pItemList.begin(); it!=pItemList.end(); it++)
		 {
			 if ((*it).second->PropId == iAfterBuyID
				 )
			 {
				 pItemIdx = (*it).first;
				 break;
			 }
		 }

		 const ItemBuyInfo* pInfo = sgTableManager()->getItemBuyById(pItemIdx);
		 if (pPorp && pInfo != NULL && coin >= pInfo->moneynum)
		 {
			 if(sgChannelApi()->isOnlineSdk() == true)
			 {
				 sgPropsControl()->sendBuyPropRqst(pInfo->PropId, pInfo->moneynum, pInfo->num);
				 return;
			 }

			 std::string param = StringUtils::format("%d %d %d %d %d %s"
				 , 1
				 , 1
				 , eVituralCurrencyType::Diamond
				 , -pInfo->moneynum
				 , sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
				 , "ShopDialog::onReceiveMassage");
			 sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
			 sgMessageManager()->sendMessage(EMT_PROP_USER,__String::createWithFormat("%d",pInfo->PropId)->getCString());
			 //showTextTip(sgTextHelper(TEXT_BUY_ITEM_SUCCESS)); 
			 sgMessageManager()->removeMessageHandler(EMT_SHOP_AFTER_BUYDIAMOND);

		 }else
		 {
			 sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM,"1");
		 }

	 }
 }
