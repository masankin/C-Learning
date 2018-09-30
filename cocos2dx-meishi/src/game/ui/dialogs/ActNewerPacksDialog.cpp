#include "ActNewerPacksDialog.h"
#include "../../act/ActControl.h"
#include "ui/PageFunction.h"
#include "SoundManager.h"
#include "../../data/TableManager.h"
#define  NEWER_GIFT_TYPE 1

ActNewerPacksDialog::ActNewerPacksDialog(void)
{
		cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_XinShoulibao.xml");
}


ActNewerPacksDialog::~ActNewerPacksDialog(void)
{
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_XinShoulibao.xml");
}

ActNewerPacksDialog *ActNewerPacksDialog::create()
{
	ActNewerPacksDialog *ret = new (std::nothrow) ActNewerPacksDialog();
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

bool ActNewerPacksDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::createNode("ui/ActNewerPacksDialog.csb");

		CC_BREAK_IF(!mRoot);
		mBtnBuy = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnBuy")); 
		mBtnBack = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnBack")); 
		mTextInfo = dynamic_cast<ui::Text*>(mRoot->getChildByName("TextInfo")); 

		//auto nodePanelAni = mRoot->getChildByName("NodePanelAni");

		setAnimation("fla_XinShoulibao");
		addChild(mRoot);
		for (int i = 0; i < 5; i++)
		{
			mNodeItem[i] = CSLoader::createNode("ui/ItemNode.csb");
			mNodeItem[i]->getChildByName("Image_1")->setVisible(true);
			mNodeItem[i]->setVisible(false);
			addChild(mNodeItem[i], 999);
			const char* boneName = __String::createWithFormat("DAOJU%d", i + 1)->getCString();
			
			cocostudio::Armature* pArmature = cocostudio::Armature::create("fla_XinShoulibao_X");
			mNodeItem[i]->addChild(pArmature);  
			pArmature->getAnimation()->play("play");
			changeBone(mArmatureBg, boneName, mNodeItem[i]);
		}
		
		changeBone(mArmatureBg, "GUANBI", mBtnBack);
		changeBone(mArmatureBg, "ANNIU", mBtnBuy);
		changeBone(mArmatureBg, "WENZI", mTextInfo);

		setPopType(E_POP_TYPE_NONE);
		initUI();
		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);

	return ret;
}

void ActNewerPacksDialog::onMenuItemAction(std::string cbName, cocos2d::Ref*sender)
{
	if (cbName == "onBack")
	{
		this->setShow(false);
		sgMessageManager()->sendMessage(EMT_CLOSEPAGE, "");
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
	}
	if (cbName == "onBuy")
	{
		NewPlayGiftData* pData = sgTableManager()->getNewPlayGiftDataByType(NEWER_GIFT_TYPE);
		if (!pData) return;
		int price = pData->price;
		//if (sgUserObj()->getGoldCoin() < price)
		if (sgUserObj()->getPersonalInfo()->diamond() < price)
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_BUY_ITEM_NO_EN),CC_CALLBACK_1(ActNewerPacksDialog::onCallBackBuyDiamond, this));
		}
		else
		{
			//网络发送购买请求
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), StringUtils::format(sgTextHelper(TEXT_BUY_NEWERGIFT_SURE).c_str(), price), CC_CALLBACK_1(ActNewerPacksDialog::onCallBack, this));
			mGiftId = pData->id;
		}
		
	}
}

void ActNewerPacksDialog::onCallBack(cocos2d::Ref* ref)
{
	GenerlDialog::hide();
	sgActControl()->getActProxy()->sendBuyActNewerGift(mGiftId);
	//为了和动画兼容！！！
	Window::Remove(this);
}

void ActNewerPacksDialog::onCallBackBuyDiamond(Ref*)
{
	GenerlDialog::hide();
	PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
	sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, "1");
}

void ActNewerPacksDialog::initUI()
{
	NewPlayGiftData* pNData = sgTableManager()->getNewPlayGiftDataByType(NEWER_GIFT_TYPE);
	if (!pNData) return;
	const GiftAward* pData = sgTableManager()->getGiftAward(pNData->giftId);
	if (!pData) return;
	int iSize = 0;
	int iNum = 0;
	std::string sIconStr ="";
	std::string sName = "";

	if (pData->gold>0)
	{
		iNum = pData->gold;
		sIconStr = "ui/general/general_diamonds.png";
		sName = sgTextHelper(TEXT_ACT_GOLD);

		(dynamic_cast<ui::Text*>(mNodeItem[iSize]->getChildByName("Text_1")))->setText(__String::createWithFormat("%d", iNum)->getCString());
		(dynamic_cast<ImageView*>(mNodeItem[iSize]->getChildByName("Image_2")))->loadTexture(sIconStr, TextureResType::PLIST);
		
		mNodeItem[iSize]->setVisible(true);
		iSize++;
	}
	if (pData->manual>0)
	{
		iNum = pData->manual;
		sIconStr = "ui/Icon/Icon_Energy.png";
		sName = sgTextHelper(TEXT_ACT_TILI);

		(dynamic_cast<ui::Text*>(mNodeItem[iSize]->getChildByName("Text_1")))->setText(__String::createWithFormat("%d", iNum)->getCString());
		(dynamic_cast<ImageView*>(mNodeItem[iSize]->getChildByName("Image_2")))->loadTexture(sIconStr, TextureResType::PLIST);
		mNodeItem[iSize]->setVisible(true);
		iSize++;
	}
	if (pData->energy>0)
	{
		iNum = pData->energy;
		sIconStr = "ui/Icon/Icon_vigour.png";
		sName = sgTextHelper(TEXT_ACT_JINGLI);

		(dynamic_cast<ui::Text*>(mNodeItem[iSize]->getChildByName("Text_1")))->setText(__String::createWithFormat("%d", iNum)->getCString());
		(dynamic_cast<ImageView*>(mNodeItem[iSize]->getChildByName("Image_2")))->loadTexture(sIconStr, TextureResType::PLIST);
		mNodeItem[iSize]->setVisible(true);
		iSize++;
	}
	if (pData->itemMap.size()>0)
	{
		auto pIt = pData->itemMap.begin();
		for (; pIt != pData->itemMap.end(); pIt++)
		{
			int pItemID = (*pIt).first;
			iNum = (*pIt).second;
			const PropInfo* pProp = sgTableManager()->getPropInfoById(pItemID);
			sIconStr = pProp->icon;
			sName = pProp->name;
			if (iSize < 5)
			{
				(dynamic_cast<ui::Text*>(mNodeItem[iSize]->getChildByName("Text_1")))->setText(__String::createWithFormat("%d", iNum)->getCString());
				(dynamic_cast<ImageView*>(mNodeItem[iSize]->getChildByName("Image_2")))->loadTexture(sIconStr, TextureResType::PLIST);
				mNodeItem[iSize]->setVisible(true);
				std::string iconFrame = "ui/general/general_kuang_3.png";
				if (pProp->isPve == 1)
					iconFrame = "ui/general/general_kuang_4.png";
				dynamic_cast<ImageView*>(mNodeItem[iSize]->getChildByName("Image_1"))->loadTexture(iconFrame, TextureResType::PLIST); 
				iSize++;
			}
		}			
	}
	
}


void ActNewerPacksDialog::onShow()
{
	Window::onShow();
}

