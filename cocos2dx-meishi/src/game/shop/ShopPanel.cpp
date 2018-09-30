#include "ShopPanel.h"
#include "ShopStoreLayer.h"
#include "ShopPropLayer.h"
#include "ShopGiftLayer.h"
#include "MessageManager.h"
#include "guide/GuideManager.h"
#include "GamePrecedure.h"
#include "ui/PageFunction.h"
#include "core/SoundManager.h"

using namespace cocos2d;
using namespace cocostudio;
ShopPanel::ShopPanel(void):mSelectType(S_NULL),mShowShopLayer(NULL),mIsChanging(false)
{
	sgMessageManager()->regisiterMessageHandler(EMT_SHOP_SHOW_BUYITEM,this);
	sgMessageManager()->regisiterMessageHandler(EMT_GUIDE_TIPS, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CLOSESHOP, this);
	sgMessageManager()->regisiterMessageHandler(EMT_GOTO_ACTLAB, this);
}


ShopPanel::~ShopPanel(void)
{
	sgMessageManager()->removeMessageHandler(this);
	for (auto it = mShopLayerMap.begin(); it != mShopLayerMap.end(); it++)
	{
		CC_SAFE_RELEASE(it->second);
	}
	 
}

void ShopPanel::showShop()
{
	ShopPanel* pRet = dynamic_cast<ShopPanel*>(sgPageFunction()->createWindow(MSG_FUC_WIN_SHOP, "ShopNode.csb"));
	if (pRet)
	{
		sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, "1");
	}
}

bool ShopPanel::init()
{
	if (!ActionWindow::init())
	{
		return false;
	}
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_shangcheng.xml");
	mRoot = CSLoader::createNode("ui/ShopNode.csb");

	setAnimation("fla_shangcheng");

	Button* pClose = dynamic_cast<Button*>(mRoot->getChildByName("BtnBack"));
	changeBone(mArmatureBg,"shangcheng_guanbi", pClose);

	for (int i = 1; i<=3; i++)
	{
		ImageView* pType = dynamic_cast<ImageView*>(mRoot->getChildByName( __String::createWithFormat("shop_type_%d",i)->getCString()));
		
		switch (i)
		{
		case 1:
			changeBone(mArmatureBg,"shangcheng_anniudiban",pType);
			break;
		case 2:
			changeBone(mArmatureBg,"shangcheng_anniudiban1",pType);
			break;
		case 3:
			
#ifdef MGAME_TENCENT_PLATFORM
			pType->removeFromParent();
			pType = NULL;
			mArmatureBg->getBone("shangcheng_anniudiban0")->retain();
			mArmatureBg->getBone("shangcheng_anniudiban0")->removeFromParent();
#else
			changeBone(mArmatureBg,"shangcheng_anniudiban0",pType);
#endif // MGAME_TENCENT_PLATFORM

			
			break;
		default:
			break;
		}
		if (pType)
		{
			pType->getParent()->setZOrder(-100);
			mShopButtonMap.insert(std::make_pair(i, pType));
		}
	}
	Button* pLeftBtn = dynamic_cast<Button*>(mRoot->getChildByName("BtnLeft"));
	changeBone(mArmatureBg,"fla_shangcheng_tubiaoyia",pLeftBtn);

	Button* pRightBtn = dynamic_cast<Button*>(mRoot->getChildByName("BtnRight"));
	changeBone(mArmatureBg,"fla_shangcheng_tubiaoyi",pRightBtn);

	mPropChangeBtn = dynamic_cast<Node*>(mRoot->getChildByName("PropNode"));
	changeBone(mArmatureBg, "shangcheng_QH", mPropChangeBtn);
	mPropIsPve = true;
	mOnPropTag = false;

	
	mPropPve.mBtn = dynamic_cast<Button*>(mPropChangeBtn->getChildByName("pveBtn"));
	mPropPve.mNorImg = dynamic_cast<ImageView*>(mPropPve.mBtn->getChildByName("Image_1"));
	mPropPve.mSelectImg = dynamic_cast<ImageView*>(mPropPve.mBtn->getChildByName("Image_2"));


	mPropPvp.mBtn = dynamic_cast<Button*>(mPropChangeBtn->getChildByName("pvpBtn"));
	mPropPvp.mNorImg = dynamic_cast<ImageView*>(mPropPvp.mBtn->getChildByName("Image_1"));
	mPropPvp.mSelectImg = dynamic_cast<ImageView*>(mPropPvp.mBtn->getChildByName("Image_2"));

	for (int i = 1; i<=3; i++)
	{
		Button* pButton = dynamic_cast<Button*>(mRoot->getChildByName(__String::createWithFormat("mBuyBtn_%d",i)->getCString()));
		std::string pflaName ="";
		switch (i)
		{
		case 1:
			pflaName = "shangcheng_anjian";  
			break;
		case 2:
			pflaName = "shangcheng_anjian0";
			break;
		case 3:
			pflaName = "shangcheng_anjian1";
			break;
		default:
			break;
		}
		changeBone(mArmatureBg,pflaName,pButton);
		mBuyBtnVec.push_back(pButton);
	}
	

	addChild(mRoot);

	setPopType(E_POP_TYPE_NONE);
	onLocateClickCallback(mRoot);

	mContentNode = Node::create();
	addChild(mContentNode);
	
	return true;
}

void ShopPanel::updateConten()
{

}

void ShopPanel::changeContent(SHOPTYPE shopType)
{
	if (mSelectType != shopType && shopType >= S_SHOP)
	{
		setTypeImageSelect(mSelectType,false);
		setTypeImageSelect(shopType,true);

	}
	
}

void ShopPanel::setTypeImageSelect(SHOPTYPE stype, bool b)
{
	if (b)
	{
		mSelectType = stype;

		if (stype == S_PROP_PVP)
			stype = S_PROP_PVE;
		if (stype == S_GIFT)
			stype = S_PROP_PVP;
		
		auto pBtn = mShopButtonMap.find(stype);
		if (pBtn != mShopButtonMap.end())
		{
			pBtn->second->setPositionX(pBtn->second->getPositionX() - 10);
			pBtn->second->loadTexture("ui/Shop/shop_bottom.png", cocos2d::ui::Widget::TextureResType::PLIST);
			pBtn->second->getChildByName("Title_nor")->setVisible(false);
			pBtn->second->getChildByName("Title_sel")->setVisible(true);
		}
		//mShopButtonMap[stype]->setPositionX(mShopButtonMap[stype]->getPositionX() - 10);
		//mShopButtonMap[stype]->loadTexture("ui/Shop/shop_bottom.png", cocos2d::ui::Widget::TextureResType::PLIST);
		//mShopButtonMap[stype]->getChildByName("Title_nor")->setVisible(false);
		//mShopButtonMap[stype]->getChildByName("Title_sel")->setVisible(true);
		
		
		auto it = mShopLayerMap.find(mSelectType);
		if (it==mShopLayerMap.end())
		{
			mShowShopLayer = createShopLayer(mSelectType);
		}else
			mShowShopLayer =(*it).second;

		
		for (int contentI = 0; contentI <mShowShopLayer->getContentVec().size(); contentI++)
		{
			
		}
		changeBone(mArmatureBg,"gugeshangcheng_wupin1",mShowShopLayer->getContentVec()[0]);
		changeBone(mArmatureBg,"gugeshangcheng_wupin0",mShowShopLayer->getContentVec()[1]);
		changeBone(mArmatureBg,"gugeshangcheng_wupin2",mShowShopLayer->getContentVec()[2]);


		mContentNode->removeAllChildren();
		mContentNode->addChild(mShowShopLayer);
		mShowShopLayer->onShow();

	}else if(stype >=S_SHOP)
	{
		auto it = mShopLayerMap.find(stype);
		if (it != mShopLayerMap.end())
		{
			(*it).second->onClose();
		}
		//mShopButtonMap[stype]->setContentSize(Size(92,113));
		if (stype == S_PROP_PVP)
			stype = S_PROP_PVE;
		if (stype == S_GIFT)
			stype = S_PROP_PVP;

		auto pBtn = mShopButtonMap.find(stype);
		if (pBtn != mShopButtonMap.end())
		{
			pBtn->second->setPositionX(pBtn->second->getPositionX() + 10);
			pBtn->second->loadTexture("ui/Shop/shop_bottom_2.png", cocos2d::ui::Widget::TextureResType::PLIST);
			pBtn->second->getChildByName("Title_nor")->setVisible(true);
			pBtn->second->getChildByName("Title_sel")->setVisible(false);
		}		
	}
}

bool dishIsRight = false;
bool isCanChange = true;
void ShopPanel::refreshContent(bool isRight)
{
	if (mIsChanging)
	{
		return;
	}
	mIsChanging = true;
	isCanChange = true;
	if (isRight)
	{
		if (mShopLayerMap[mSelectType]->isCanMoveRight() == false)
		{
			isCanChange = false;
		}
	}else
	{
		if (mShopLayerMap[mSelectType]->isCanMoveLeft() == false)
		{
			isCanChange = false;
		}
	}
	
	dishIsRight = isRight;
	
	mArmatureBg->getAnimation()->play("change");
	mArmatureBg->getAnimation()->setMovementEventCallFunc(std::bind(
		&ShopPanel::onFreshMovement, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, isRight));
	
	
}


void ShopPanel::onFreshMovement(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName, bool isRight )
{
	if (type == COMPLETE && "change" == aniName)
	{
		pAr->getAnimation()->play("stand");
		mIsChanging = false;
	}
}

void ShopPanel::onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex)
{
	if (evt == "tk")
	{
		if (!isCanChange)
		{
			return;
		}
		if (dishIsRight)
		{
			mShopLayerMap[mSelectType]->moveRight();
		}else
		{
			mShopLayerMap[mSelectType]->moveLeft();
		}
	}
}

void ShopPanel::onMenuItemAction(std::string name, cocos2d::Ref* ref)
{
	if (name == "onBack")
	{
		setShow(false);
		sgGuideManager()->unRegistGuideMessage();
		char tParam[64] = {0,};
		sprintf(tParam,"");
		sgMessageManager()->sendMessage(EMT_CLOSEPAGE, tParam);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");

	}else if(name == "onShopItem")
	{
		mPropChangeBtn->setVisible(false);
		mOnPropTag = false;
		changeContent(S_SHOP);		
	}else if (name == "onPropItem")
	{
		mOnPropTag = false;
		changeContent(S_PROP_PVE);
		onChangePropByType(S_PROP_PVE);
		mPropChangeBtn->setVisible(true);
	}else if (name == "onGiftItem")
	{
		mOnPropTag = false;
		mPropChangeBtn->setVisible(false);
		changeContent(S_GIFT);		
	}else if (name == "onLeft")
	{
		refreshContent(false);
		
	}else if (name == "onRight")
	{
		refreshContent(true);
		
	}else if (name == "onBuy")
	{
		for (int i = 0; i<mBuyBtnVec.size(); i++)
		{
			if (mBuyBtnVec[i] == ref)
			{
				mShowShopLayer->onBuy(i);
			}
		}
	}else if (name == "onPropPVE")
	{
		mOnPropTag = true;
		changeContent(S_PROP_PVE);
		onChangePropByType(S_PROP_PVE);

	}else if (name == "onPropPVP")
	{
		mOnPropTag = true;
		changeContent(S_PROP_PVP);
		onChangePropByType(S_PROP_PVP);
	}
	
}

void ShopPanel::onShow()
{
	sgSoundManager()->playMusicBg(MUSIC_UI_MALL);

	ActionWindow::onShow();
	changeContent(S_PROP_PVE);
	mPropIsPve = true;
	mOnPropTag = false;
	mPropChangeBtn->setVisible(true);
	onChangePropByType(S_PROP_PVE);
	//mShowShopLayer->changeMode(P_PVE);
	sgGuideManager()->unRegistGuideMessage(); 
}

void ShopPanel::onClose()
{
	ActionWindow::onClose();
	sgSoundManager()->playLastMusicBg();
}

void ShopPanel::onChangePropByType(SHOPTYPE type)
{ 
	if (type == S_PROP_PVE)
	{
		mPropPve.mBtn->setHighlighted(true);
		mPropPve.mNorImg->setVisible(false);
		mPropPve.mSelectImg->setVisible(true);

		mPropPvp.mBtn->setHighlighted(false);
		mPropPvp.mNorImg->setVisible(true);
		mPropPvp.mSelectImg->setVisible(false);

	}
	else
	{
		mPropPve.mBtn->setHighlighted(false);
		mPropPve.mNorImg->setVisible(true);
		mPropPve.mSelectImg->setVisible(false);

		mPropPvp.mBtn->setHighlighted(true);
		mPropPvp.mNorImg->setVisible(false);
		mPropPvp.mSelectImg->setVisible(true);
	}
}

ShopLayer* ShopPanel::createShopLayer(SHOPTYPE type)
{
	ShopLayer* pRet = NULL;
	switch (type)
	{
	case S_NULL:
		break;
	case S_SHOP:
		pRet = ShopStoreLayer::create();
		break;
	case S_PROP_PVE:
	case S_PROP_PVP:
		pRet = ShopPropLayer::createWithType(type);
		break;
	case S_GIFT:
		pRet = ShopGiftLayer::create();
		break;
	default:
		break;
	}
	mShopLayerMap.insert(make_pair(type,pRet));
	pRet->setBuyButton(mBuyBtnVec);
	CC_SAFE_RETAIN(pRet);
	

	return pRet;
}

void ShopPanel::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_SHOP_SHOW_BUYITEM)
	{
		MessageShopBuyItem* pShopType = dynamic_cast<MessageShopBuyItem*>(message);
		if (pShopType->getItemType() == 1)
		{
			if (isShow())
			{
				changeContent(S_SHOP);
				mOnPropTag = false;
				mPropChangeBtn->setVisible(false);
			}		
		}else if (pShopType->getItemType() == 2)
		{
			if (isShow())
			{
				changeContent(S_PROP_PVE);
				mPropIsPve = true;
				mOnPropTag = false;
				mPropChangeBtn->setVisible(true);
			}
		}else if (pShopType->getItemType() == 3)
		{
			if (isShow())
			{
				changeContent(S_GIFT);
				mOnPropTag = false;
				mPropChangeBtn->setVisible(false);
			}
		}

	}else if(message->getMessageType() == EMT_GUIDE_TIPS)
	{
		MessageGuideTips* pMsg = dynamic_cast<MessageGuideTips* >(message);
		if(!pMsg)
			return;
		if(pMsg->getFunctionID() == E_GuideFuc_TabShop)
		{
			changeContent(S_PROP_PVE);
			mPropIsPve = true;
			mOnPropTag = false;
			mPropChangeBtn->setVisible(true);
			scheduleOnce(schedule_selector(ShopPanel::doGuideNode), Director::getInstance()->getDeltaTime() + 0.3f);
		}
		else if(pMsg->getFunctionID() == E_GuideFuc_EnterShopMn)
		{
			int gateID = sgUserObj()->getMaxOpenGateID();
			int level = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel();
			Size winSize = Director::getInstance()->getWinSize();
			sgGuideManager()->registGuideMessage(gateID, level, E_GUIDELOC_SHOP, mRoot, this, false, -0.5 * winSize);
		}
	}
	else if (message->getMessageType() == EMT_CLOSESHOP)
	{
		setShow(false);
	}
	
}

void ShopPanel::doGuideNode(float dt)
{
	int gateID = sgUserObj()->getMaxOpenGateID();
	int level = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel(); 
	Size winSize = Director::getInstance()->getWinSize();
	sgGuideManager()->registGuideMessage(gateID, level, E_GUIDELOC_SHOP, mRoot, this, false, -0.5 * winSize);

	Node* pBuyNode = Node::create();
	pBuyNode->setTag(E_GuideTag_ShopPanelBuyBtn);
	pBuyNode->setName("E_GuideTag_ShopPanelBuyBtn");
	pBuyNode->setPosition(ccp(352,101));
	mRoot->addChild(pBuyNode);

	Node* pBackNode = Node::create();
	pBackNode->setTag(E_GuideTag_ShopPanelBackBtn);
	pBackNode->setName("E_GuideTag_ShopPanelBackBtn");
	pBackNode->setPosition(ccp(1156,606));
	mRoot->addChild(pBackNode);
	
}
