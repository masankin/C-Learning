#include "ActFirstCharge.h"
#include "data/TableManager.h"
#include "act/ActControl.h"
#include "ui/PageFunction.h"
#include "SoundManager.h"

ActFirstCharge::ActFirstCharge(void)
{
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_scjl.png", "fla_scjl.plist", "fla_scjl.xml");
}


ActFirstCharge::~ActFirstCharge(void)
{
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_scjl.xml");
}

ActFirstCharge *ActFirstCharge::create()
{
	ActFirstCharge *ret = new (std::nothrow) ActFirstCharge();
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

bool ActFirstCharge::init()
{
	if (!Layer::init())
	{
		return false;
	}
	mRoot = CSLoader::getInstance()->createNode("ui/ActFirstCharge.csb");
	
	mRoot->setAnchorPoint(Vec2(0.5, 0.5));
	auto winSize = Director::getInstance()->getWinSize();
	mRoot->setPosition(Vec2(winSize.width/2+40, winSize.height/2));
	addChild(mRoot);

	mGetBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("GetBtn"));
	mChargeBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("ChargeBtn"));
	mGetBtn->setVisible(false);
	//mGetBtn->addClickEventListener(CC_CALLBACK_1(ActFirstCharge::_onMenuAction, this));
	//mChargeBtn->addClickEventListener(CC_CALLBACK_1(ActFirstCharge::_onMenuAction, this));

	for (int i = 0; i < 7; i++)
	{
		mItemNode[i] = mRoot->getChildByName(__String::createWithFormat("Node_%d", i)->getCString());
		mItemNode[i]->setVisible(false);
	}

	mBtnSprite_0 = dynamic_cast<Sprite*>(mChargeBtn->getChildByName("BtnSprite_0"));
	mBtnSprite_1 = dynamic_cast<Sprite*>(mChargeBtn->getChildByName("BtnSprite_1"));

	cocostudio::Armature* mArmatureGuang = cocostudio::Armature::create("fla_scjl_ADDITIVE");
	mRoot->addChild(mArmatureGuang);
	mArmatureGuang->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);
	mArmatureGuang->getAnimation()->play("play");


	cocostudio::Armature* pArmature = cocostudio::Armature::create("fla_scjl");
	mRoot->addChild(pArmature);
	pArmature->getAnimation()->play("play");


	mArmature = pArmature->getBone("guge_AnNiu")->getChildArmature();
	cocostudio::Bone* pBone = mArmature->getBone("scjl_scjl");
	if (pBone)
	{
		pBone->addDisplay(mChargeBtn, 0);
	}

	setPopType(E_POP_TYPE_BIG);
	onLocateClickCallback(mRoot);

	//onShow();

	return true;
}

//void ActFirstCharge::_onMenuAction(Ref* sender)
//{
//	cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(sender);
//	if (!widget)
//		return;
//
//	sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);
//
//	std::string cbName = widget->getCallbackName();
//
//	if (cbName == "onGet")
//	{
//		sgActControl()->getActProxy()->sendReceiveActivityReward(ACT_FIRST_CHARGE);
//	}
//	else if (cbName == "onCharge")
//	{
//		//sgActControl()->getActPanel()->setShow(false);
//		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
//		sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, "1");
//	}
//}

void ActFirstCharge::onMenuItemAction(std::string cbName, cocos2d::Ref*sender)
{
	if (cbName == "onClose")
	{
		setShow(false);
	}
	else if (cbName == "onGet")
	{
		sgActControl()->getActProxy()->sendReceiveActivityReward(ACT_FIRST_CHARGE);
	}
	else if (cbName == "onCharge")
	{
		//sgActControl()->getActPanel()->setShow(false);
		setShow(false);
		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
		sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, "1");
	}

	sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);
}

void ActFirstCharge::onShow()
{
	ActionWindow::onShow();

	initView();
	updateBtn();
}

void ActFirstCharge::initView()
{
	int id = sgActControl()->getFirstId();
	RechargeData* pData = sgTableManager()->getRechargeData(id);
	if (!pData) return;

	const GiftAward* pGiftAward = sgTableManager()->getGiftAward(pData->getReward());
	if (!pGiftAward) return;

	int iSize = 0;
	int iNum = 0;
	std::string sIconStr = "";
	std::string sName = "";
	if (pGiftAward->mGiftData.size() == 0)
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
		showItemInfo(mItemNode[iSize], lGoodsInfo->mIconSmall, lGiftInfo.mNumber);
		++iSize;
	}
	
}

void ActFirstCharge::showItemInfo(Node* itemNode, std::string iconName, int count, int itemType)
{
	if (itemNode) {
		itemNode->setVisible(true);
		auto countLabel = dynamic_cast<ui::Text*>(itemNode->getChildByName("Text_1"));
		auto icon = dynamic_cast<ui::ImageView*>(itemNode->getChildByName("Image_2"));
		if (itemType > 0)
		{
			std::string frameBg = "ui/general/general_kuang_3.png";
			if (itemType == 1)
				frameBg = "ui/general/general_kuang_4.png";
			dynamic_cast<ui::ImageView*>(itemNode->getChildByName("Image_1"))->loadTexture(frameBg, ui::Widget::TextureResType::PLIST);
		}
		if (countLabel && icon) {
			countLabel->setText(StringUtils::format("%d", count));
			icon->loadTexture(iconName, ui::TextureResType::PLIST);
		}
	}
}

void ActFirstCharge::updateBtn()
{
	int id = sgActControl()->getFirstId();
	RechargeData* pData = sgTableManager()->getRechargeData(id);
	if (!pData) return;

	int flag = pData->getIsGet();

	switch (flag)
	{
	case 0:
	{
		mGetBtn->setVisible(false);
		mChargeBtn->setVisible(true);
		mBtnSprite_1->setVisible(true);
		mBtnSprite_0->setVisible(false);
		mChargeBtn->setBright(false);
		mChargeBtn->setEnabled(false);
		mArmature->getAnimation()->pause();
		break;
	}
	case 1:
	{
		mGetBtn->setVisible(true);
		mChargeBtn->setVisible(false);
		break;
	}
	case 2:
	{
		mGetBtn->setVisible(false);
		mChargeBtn->setVisible(true);
		mBtnSprite_1->setVisible(false);
		mBtnSprite_0->setVisible(true);
		mChargeBtn->setBright(true);
		mChargeBtn->setEnabled(true);
		break;
	}
	default:
		break;
	}
}

//void ActFirstCharge::updateInfo()
//{
//	onShow();
//}

