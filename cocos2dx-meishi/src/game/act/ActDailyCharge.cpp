#include "ActDailyCharge.h"
#include "data/TableManager.h"
#include "act/ActControl.h"
#include "ui/PageFunction.h"
#include "SoundManager.h"

ActDailyCharge::ActDailyCharge(void)
{

}


ActDailyCharge::~ActDailyCharge(void)
{
}

ActDailyCharge *ActDailyCharge::create()
{
	ActDailyCharge *ret = new (std::nothrow) ActDailyCharge();
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

bool ActDailyCharge::init()
{
	if (!Layer::init())
	{
		return false;
	}
	Node* pRoot = CSLoader::getInstance()->createNode("ui/ActDailyCharge.csb");
	
	addChild(pRoot);

	mSprTitle = dynamic_cast<Sprite*>(pRoot->getChildByName("Sprite_2"));
	mTextHad = dynamic_cast<ui::Text*>(pRoot->getChildByName("GoldNum_1"));
	mTextNeed = dynamic_cast<ui::Text*>(pRoot->getChildByName("GoldNum_2"));
	mLoadingBar = dynamic_cast<ui::LoadingBar*>(pRoot->getChildByName("LoadingBar_1"));
	mGetBtn = dynamic_cast<ui::Button*>(pRoot->getChildByName("GetBtn"));
	mChargeBtn = dynamic_cast<ui::Button*>(pRoot->getChildByName("ChargeBtn"));
	mGetBtn->addClickEventListener(CC_CALLBACK_1(ActDailyCharge::_onMenuAction, this));
	mChargeBtn->addClickEventListener(CC_CALLBACK_1(ActDailyCharge::_onMenuAction, this));

	for (int i = 0; i < 7; i++)
	{
		mItemNode[i] = dynamic_cast<Node*>(pRoot->getChildByName(__String::createWithFormat("Node_%d",i)->getCString()));
		mItemNode[i]->setVisible(false);
	}

	mBtnSprite_0 = dynamic_cast<Sprite*>(mChargeBtn->getChildByName("BtnSprite_0"));
	mBtnSprite_1 = dynamic_cast<Sprite*>(mChargeBtn->getChildByName("BtnSprite_1"));

	onShow();

	return true;
}

void ActDailyCharge::_onMenuAction(Ref* sender)
{
	cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(sender);
	if (!widget)
		return;

	std::string cbName = widget->getCallbackName();

	if (cbName == "onGet")
	{
		sgActControl()->getActProxy()->sendReceiveActivityReward(ACT_DAILY_CHARGE);
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

void ActDailyCharge::onShow()
{
	int num = sgActControl()->getDailyCount();
	int id = sgActControl()->getDailyId();
	mSprTitle->setSpriteFrame("ui/Recharge/recharge_Title_1.png");

	RechargeData* pData = sgTableManager()->getRechargeData(id);
	if (!pData) return;
	
	mLoadingBar->setPercent(num*100.0/pData->getRechargeNum());

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
	
	const GiftAward* pGiftAward = sgTableManager()->getGiftAward(pData->getReward());
	if (!pGiftAward) return;

	int iSize = 0;
	int iNum = 0;
	std::string sIconStr ="";
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
		iNum = lGiftInfo.mNumber;
		sIconStr = lGoodsInfo->mIconSmall;
		sName = lGoodsInfo->mName;

		(dynamic_cast<ui::Text*>(mItemNode[iSize]->getChildByName("Text_1")))->setText(__String::createWithFormat("%d", iNum)->getCString());
		(dynamic_cast<ImageView*>(mItemNode[iSize]->getChildByName("Image_2")))->loadTexture(sIconStr, TextureResType::PLIST);
		mItemNode[iSize]->setVisible(true);
		iSize++;
	}
}

void ActDailyCharge::updateInfo()
{
	onShow();
}
