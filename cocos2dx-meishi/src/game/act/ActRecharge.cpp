#include "ActRecharge.h"
#include "data/TableManager.h"
#include "act/ActControl.h"
#include "ui/PageFunction.h"
#include "SoundManager.h"
#include "utils/Comm.h"
ActRecharge::ActRecharge(void)
{
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_LeiJiCZ.png", "fla_LeiJiCZ.plist", "fla_LeiJiCZ.xml");
}


ActRecharge::~ActRecharge(void)
{
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_LeiJiCZ.xml");
}

ActRecharge *ActRecharge::create()
{
	ActRecharge *ret = new (std::nothrow) ActRecharge();
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

bool ActRecharge::init()
{
	if (!Layer::init())
	{
		return false;
	}
	Node* pRoot = CSLoader::getInstance()->createNode("ui/ActRecharge.csb");
	
	addChild(pRoot);

	//mSprTitle = dynamic_cast<Sprite*>(pRoot->getChildByName("Sprite_2"));
	mTextHad = dynamic_cast<ui::Text*>(pRoot->getChildByName("GoldNum_2"));
	mTextNeed = dynamic_cast<ui::Text*>(pRoot->getChildByName("GoldNum_1"));
	mLoadingBar_1 = dynamic_cast<ui::LoadingBar*>(pRoot->getChildByName("LoadingBar_1"));
	mLoadingBar_2 = dynamic_cast<ui::LoadingBar*>(pRoot->getChildByName("LoadingBar_1"));
	mGetBtn = dynamic_cast<ui::Button*>(pRoot->getChildByName("GetBtn"));
	mChargeBtn = dynamic_cast<ui::Button*>(pRoot->getChildByName("ChargeBtn"));
	mGetBtn->addClickEventListener(CC_CALLBACK_1(ActRecharge::_onMenuAction, this));
	mChargeBtn->addClickEventListener(CC_CALLBACK_1(ActRecharge::_onMenuAction, this));
	mScrollView = dynamic_cast<ui::ScrollView*>(pRoot->getChildByName("ScrollView_1"));
	mOpenTime = dynamic_cast<ui::Text*>(pRoot->getChildByName("TextOpenTime"));
	cocostudio::Armature *mArmature = cocostudio::Armature::create("fla_LeiJiCZ");
	pRoot->getChildByName("Sprite_118")->addChild(mArmature);
	mArmature->getAnimation()->play("play");
	mArmature->setPosition(550,250);

	mBtnSprite_0 = dynamic_cast<Sprite*>(mChargeBtn->getChildByName("BtnSprite_0"));
	mBtnSprite_1 = dynamic_cast<Sprite*>(mChargeBtn->getChildByName("BtnSprite_1"));

	onShow();

	return true;
}

void ActRecharge::_onMenuAction(Ref* sender)
{
	cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(sender);
	if (!widget)
		return;

	std::string cbName = widget->getCallbackName();

	if (cbName == "onGet")
	{
		sgActControl()->getActProxy()->sendReceiveActivityReward(ACT_RECHARGE);
	}
	else if (cbName == "onCharge")
	{
		if (sgActControl()->getActPanel())
		{
			sgActControl()->getActPanel()->setShow(false);
		}
		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
		sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, "1");
	}
}

void ActRecharge::onShow()
{
	int id = sgActControl()->getRechargeId();
	int num = sgActControl()->getRechargeCount();
	//mSprTitle->setSpriteFrame("ui/Recharge/recharge_Title_2.png");

	RechargeData* pData = sgTableManager()->getRechargeData(id);
	if (!pData) return;
	std::string openTime = pData->getStartTime();
	std::string closeTime = pData->getEndTime();
	std::vector<std::string > tTempStrVec = splitStr(openTime, "#");
	std::string str[3] = {""};
	if (tTempStrVec.size() == 2)
	{
		openTime = tTempStrVec.at(1).substr(0, tTempStrVec.at(1).length() - 6);
		str[0] = openTime.substr(0, 4) + ".";
		str[1] = openTime.substr(4, 2) + ".";
		str[2] = openTime.substr(6, 2);
		openTime = str[0] + str[1] + str[2];
	}

	tTempStrVec = splitStr(closeTime, "#");
	if (tTempStrVec.size() == 2)
	{
		closeTime = tTempStrVec.at(1).substr(0, tTempStrVec.at(1).length() - 6);
		str[0] = closeTime.substr(0, 4) + ".";
		str[1] = closeTime.substr(4, 2) + ".";
		str[2] = closeTime.substr(6, 2);
		closeTime = str[0] + str[1] + str[2];
	}

	mOpenTime->setText(__String::createWithFormat(sgTextHelper(TEXT_ACT_OPEN_TIME).c_str(), openTime.c_str(), closeTime.c_str())->getCString());
	mLoadingBar_1->setPercent(num*100.0/pData->getRechargeNum());

	mTextHad->setText(__String::createWithFormat("%d",num)->getCString());
	num = pData->getRechargeNum() - num;

	mTextNeed->setText(__String::createWithFormat("%d", num > 0 ? num : 0)->getCString());

	switch (pData->getIsGet())
	{
	case 0:
		{
			mGetBtn->setVisible(false);
			mChargeBtn->setVisible(true);
			mBtnSprite_1->setVisible(true);
			mBtnSprite_0->setVisible(false);
			mChargeBtn->setBright(false);
			mChargeBtn->setEnabled(false);
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

	mScrollView->removeAllChildren();

	
	const GiftAward* pGiftAward = sgTableManager()->getGiftAward(pData->getReward());
	if (!pGiftAward) return;

	int iSize = 0;
	int iNum = 0;
	std::string sIconStr ="";
	//std::string sName = "";
	if (pGiftAward->mGiftData.size() == 0)
	{
		return;
	}
	for (int i = 0; i < pGiftAward->mGiftData.size(); i++)
	{
		GiftInfo lGiftInfo = pGiftAward->mGiftData.at(i);
		const GoodsInfo* lGoodsInfo = sgTableManager()->getGoodsInfo(lGiftInfo.mType, lGiftInfo.mId);

		Node* prow = createRowNode(lGiftInfo.mType, lGiftInfo.mNumber, lGoodsInfo->mIconSmall);
		mScrollView->addChild(prow);
		prow->setPosition(44 + iSize * 88, 50);

		++iSize;
	}
	
}

Node* ActRecharge::createRowNode(int id,int num,std::string strIcon)
{
	Node* node = CSLoader::createNode("ui/ItemNode.csb");
	node->setAnchorPoint(Vec2(0.5,0.5));

	(dynamic_cast<ui::Text*>(node->getChildByName("Text_1")))->setText(__String::createWithFormat("%d",num)->getCString());
	(dynamic_cast<ImageView*>(node->getChildByName("Image_2")))->loadTexture(strIcon,TextureResType::PLIST);
	ui::Layout* pPanel = dynamic_cast<ui::Layout*>(node->getChildByName("Panel_1"));
	pPanel->addClickEventListener(CC_CALLBACK_1(ActRecharge::_onMenuAction, this));
	pPanel->setTag(id);
	node->setScale(0.5);
	return node;
}

void ActRecharge::updateInfo()
{
	onShow();
}

void ActRecharge::updateInfo(fgame::CSSevenDaySignListResp& resp)
{
}
