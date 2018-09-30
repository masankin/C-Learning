#include "ShopDialog.h"
#include "shop/ShopPanel.h"

#include "game/ui/PageFunction.h"
#include "game/ui/system/Window.h"
#include "game/data/Tables.h"
#include "game/data/TableManager.h"
#include "game/ui/dialogs/GenerlDialog.h"
#include "MessageManager.h"
#include "EnvSet.h"
#include "core/HelperFunc.h"
#include "charge/ChargeManager.h"
#include "prop/PropsControl.h"
#include "battle/BattleControl.h"
#include "game/net/UserObj.h"
#include "SoundManager.h"
#include "sync/SyncControl.h"
#include "guide/GuideManager.h"
using namespace cocostudio;

bool ShopDetailed::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/ShopDetailed.csb");
		CC_BREAK_IF(!mRoot);
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));


		m_btn1 = dynamic_cast<ui::Button*>(mRoot->getChildByName("mbtn1"));
		m_btn2 = dynamic_cast<ui::Button*>(mRoot->getChildByName("mbtn2"));
		m_btn3 = dynamic_cast<ui::Button*>(mRoot->getChildByName("mbtn3"));
		mItemNameTxt = dynamic_cast<ui::Text*>(mRoot->getChildByName("mHead"));
		mDescribeTxt = dynamic_cast<ui::Text*>(mRoot->getChildByName("describe_txt"));
		mOpTxt = dynamic_cast<ui::Text*>(mRoot->getChildByName("describe_txt_0_0"));

		m_text1 = dynamic_cast<ui::Text*>(m_btn1->getChildByName("mSelect"));
		m_text2 = dynamic_cast<ui::Text*>(m_btn2->getChildByName("mSelect"));
		m_text3 = dynamic_cast<ui::Text*>(m_btn3->getChildByName("mSelect"));
		

		m_money = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_3"));
		m_Icon = mRoot->getChildByName("icon_Node");
		mNumberTxt = dynamic_cast<ui::Text*>(m_Icon->getChildByName("Text_1"));
		mNumberTxt->setString("10");

		mVipNode = dynamic_cast<Node*>(mRoot->getChildByName("vipNode"));
		mVipIcon = dynamic_cast<Sprite*>(mVipNode->getChildByName("vipIcon"));
		mVipPoint = dynamic_cast<Text*>(mVipNode->getChildByName("text_Point"));
		mVipPrice = dynamic_cast<Text*>(mVipNode->getChildByName("text_price"));

		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		ret = true;

	} while (0);


	return ret;
}

void ShopDetailed::onShow()
{
	int gateID = sgUserObj()->getNowPassCheckPoint() > sgUserObj()->getNextCheckPt() ? sgUserObj()->getNowPassCheckPoint() : sgUserObj()->getNextCheckPt();
	int level = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel();
	sgGuideManager()->registGuideMessage(gateID, level, E_GUIDELOC_SHOP, this, getParent(), false);

	sgMessageManager()->regisiterMessageHandler(EMT_GUIDE_TIPS, this);
	const ItemBuyInfo* pInfo = sgTableManager()->getItemBuyById(m_Index);
	if (pInfo)
	{
		onSelectBright(pInfo->num);
		refresh(pInfo);

	}
	else if(m_Index == 0)
	{
		onSelectBright(1);
	}

	if (sgGamePrecedure()->getGameIng() && !sgBattleControl()->getIsPvp())
		sgGamePrecedure()->setGameIng(false);

}

void ShopDetailed::onMenuItemAction(std::string cName, cocos2d::Ref*)
{
	if(sgGuideManager()->getGuideLock() == true)
		return;
	if (cName == "onClose")
	{
		this->setShow(false);

		return;
	}
	else if (cName == "onBuy")
	{
		const PropInfo* pPorp = sgTableManager()->getPropInfoById(m_ID);
		int coin = sgUserObj()->getPersonalInfo()->diamond();// sgUserObj()->getGoldCoin();
		const ItemBuyInfo* pInfo = sgTableManager()->getItemBuyById(m_Index);

		int money;
		int level = sgUserObj()->getPersonalInfo()->viplevel();//sgUserObj()->getVipLevel();
		if (level > 0 && level <= 3)
		{
			VipData *vipData = sgTableManager()->getVipData(level);
			int discount = vipData->getShopDiscount();
			money = pInfo->moneynum *vipData->getShopDiscount()*0.1;//혤憐
		}
		else
				money = pInfo->moneynum;

		if (pPorp && pInfo != NULL  && coin >= money)
		{
			if(sgChannelApi()->isOnlineSdk() == true)
			{
				sgPropsControl()->sendBuyPropRqst(pInfo->PropId, pInfo->moneynum, pInfo->num);
				return;
			}
			else
			{
				//삿혤돛야
				auto pItemIcon = Sprite::createWithSpriteFrameName(pPorp->icon.c_str());
				pItemIcon->setScale(.5f);
				sgPageFunction()->getActionPackageLayer()->addItem(pItemIcon, Vec2(m_Icon->convertToWorldSpace(m_Icon->getPosition()) - m_Icon->getPosition()), 0, 0);

				std::string param = StringUtils::format("%d %d %d %d %d %s"
					, 1
					, 1
					, eVituralCurrencyType::Diamond
					, -pInfo->moneynum
					, sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
					, "ShopDetailed::onMenuItemAction");
				sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
				sgMessageManager()->sendMessage(EMT_PROP_USER,__String::createWithFormat("%d",pInfo->PropId)->getCString());
				//sgUserObj()->getSyncPropData()->addDPropata(pInfo->PropId,1);
				showTextTip(sgTextHelper(TEXT_BUY_ITEM_SUCCESS)); 
				if(sgGamePrecedure()->getEnterGame())
				{
					setShow(false);
				}
			} 

			std::string param = StringUtils::format("%d %d %d %d %d %d %s"
				, eItemOperation::Buy
				, pPorp->id
				, 0
				, pInfo->num
				, eVituralCurrencyType::Diamond
				, pInfo->moneynum
				, "ShopDetailed::onMenuItemAction");

			sgMessageManager()->sendMessage(EMT_ITEM_OPERATION, param);

		}else
		{
			if (sgBattleControl()->isBorning() )
			{
				if (!sgBattleControl()->getIsPvp())
				{
					//sgMessageManager()->regisiterMessageHandler(EMT_SHOP_AFTER_BUYDIAMOND,this);
					//setShow(false);
					
					GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_BUY_ITEM_NO_EN),CC_CALLBACK_1(ShopDetailed::onCallBackshowShop, this));
					//sgPageFunction()->gotoFunction(MSG_FUC_WIN_SHOP);
				}
				else
				{
					setShow(false);
					showTextTip(sgTextHelper(TEXT_COMMTIPS_SHOP_TEXT1));
				}
			}
			else
			{
				setShow(false);
				GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_BUY_ITEM_NO_EN),CC_CALLBACK_1(ShopDetailed::onCallBackshowShop, this));
			}
				
		}

	}
	else if (cName == "onSelect1")
	{
		onSelectBright(1);
	}
	else if (cName == "onSelect5")
	{
		onSelectBright(5);
	}
	else if (cName == "onSelect10")
	{
		onSelectBright(10);
	}

	sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);
}

void ShopDetailed::onCallBackshowShop(Ref* ref)
{
	GenerlDialog::hide();
	ShopPanel::showShop();
}

void ShopDetailed::onSelectBright(int index)
{
	return;
	const ItemBuyList list = sgTableManager()->getItemBuyList();
	const PropInfo* mProp = sgTableManager()->getPropInfoById(m_ID);

	ItemBuyList::const_iterator it = list.begin();
	for (; list.end() != it; it++)
	{
		const PropInfo* pPorp = sgTableManager()->getPropInfoById(it->second->PropId);
		if (pPorp != NULL && mProp->type == pPorp->type && it->second->num == index && (m_ID/10000) == (it->second->PropId/10000))
		{
			m_ID = pPorp->id;
			m_Index = it->first;
			refresh(it->second);
			break;
		}
	}


	m_btn1->setBright(index == 1);
	m_btn2->setBright(index == 5);
	m_btn3->setBright(index == 10);

	//m_btn1->setEnabled(index == 1);
	//m_btn2->setEnabled(index == 5);
	//m_btn3->setEnabled(index == 10);

	m_text1->setBright(index == 1);
	m_text2->setBright(index == 1);
	m_text3->setBright(index == 1);

	

}

ShopDetailed::ShopDetailed() /*:mCallBack(0)*/
{
	
}

ShopDetailed::~ShopDetailed()
{
	sgMessageManager()->removeMessageHandler(this);
}

void ShopDetailed::onReceiveMassage(GameMessage * message)
{
	if(message->getMessageType() == EMT_GUIDE_TIPS)
	{
		MessageGuideTips* pMsg = dynamic_cast<MessageGuideTips* >(message);
		if(!pMsg)
			return;
		 if(pMsg->getFunctionID() == E_GuideFuc_CloseShopDl)
		 {
			 this->setShow(false); 
			 sgSoundManager()->playMusicEffect(SOUND_CLICK_BACK);

			 int guideID = sgGuideManager()->getCurGuideID();
			 int funID = E_GuideFuc_EnterShopMn;
			 char params[32] = { 0, };
			 sprintf(params, "%d %d %d", 
				 guideID, funID,  0);
			 sgMessageManager()->sendMessage(EMT_GUIDE_TIPS, params);  
		 } 
	}
 
}

void ShopDetailed::setPropId(int id)
{
	m_ID = id;
}

void ShopDetailed::setIndex(int id)
{
	m_Index = 0;
}

void ShopDetailed::show(int propID)
{
	bool hasProp = false;

	ItemBuyList pItemList = sgTableManager()->getItemBuyList();
	for (auto it = pItemList.begin(); it != pItemList.end(); it++)
	{
		if ((*it).second->PropId == propID)
		{
			m_Index = (*it).first;
			hasProp = true;
			break;
		}
	}

	if (!hasProp)
	{
		return;
	}
	m_ID = propID;
	ShopDetailed*  pWindow = dynamic_cast<ShopDetailed*>(Window::Create("ShopDetailed.csb"));
	if (pWindow)
	{
		pWindow->setShow(true);
	}
}

void ShopDetailed::refresh(const ItemBuyInfo* pInfo)
{
	const PropInfo* pProp = sgTableManager()->getPropInfoById(pInfo->PropId);
	ImageView* pIconImg = dynamic_cast<ImageView*>(m_Icon->getChildByName("Image_2"));
	pIconImg->loadTexture(pInfo->icon,TextureResType::PLIST) ;
	pIconImg->setContentSize(Size(120,120));
	mDescribeTxt->setString(pProp->explain1);
	mItemNameTxt->setString(pProp->name);
	mOpTxt->setString(StringUtils::format("%s", pProp->explain2.c_str()));

	m_money->setText(StringUtils::format("%d", pInfo->moneynum));
	mNumberTxt->setText(StringUtils::format("%d", pInfo->num));
	int level = sgUserObj()->getPersonalInfo()->viplevel();//sgUserObj()->getVipLevel();
	if (level>0 && level <=3)
	{
		VipData *vipData = sgTableManager()->getVipData(level);
		int discount = vipData->getShopDiscount();
		if (discount == 0)
			mVipNode->setVisible(false);
		else
		{
			mVipNode->setVisible(true);
			mVipIcon->setSpriteFrame(StringUtils::format("ui/general/vip%d_small.png", level));
			mVipPoint->setString(StringUtils::format("%d", discount));
			float price = pInfo->moneynum *vipData->getShopDiscount()*0.1;
			mVipPrice->setString(StringUtils::format("%d", (int)price));//혤憐
		}
	}
	else
	{
		mVipNode->setVisible(false);
	}
	std::string frameBg = "ui/general/general_kuang_3.png";
	if (pProp->isPve == 1)
		frameBg = "ui/general/general_kuang_4.png";
	if (m_Icon->getChildByName("Image_1"))
		dynamic_cast<ui::ImageView*>(m_Icon->getChildByName("Image_1"))->loadTexture(frameBg, TextureResType::PLIST);
}



void ShopDetailed::initGuideNode()
{
	if(getChildByTag(E_GuideTag_ShopBuyBtn))
		return;
	Node* pNode = Node::create();
	addChild(pNode);
	pNode->setTag(E_GuideTag_ShopBuyBtn);
	pNode->setName("E_GuideTag_ShopBuyBtn");
	Point srcPos = mRoot->getChildByName("Button_6")->getPosition();
	Point destPos = mRoot->convertToWorldSpace(srcPos);
	pNode->setPosition(destPos);
}


void ShopDetailed::doFinishGuide()
{
	sgGuideManager()->unRegistGuideMessage();
}

void ShopDetailed::onClose()
{
	if (!sgGamePrecedure()->getGameIng() && sgGamePrecedure()->getEnterGame())
		sgGamePrecedure()->setGameIng(true);
}

void ShopDetailed::onEnter()
{
	Window::onEnter();
	initGuideNode();
}

void ShopDetailed::onExit()
{
	Window::onExit();
}

int ShopDetailed::m_Index = 1;

int ShopDetailed::m_ID = 0;
