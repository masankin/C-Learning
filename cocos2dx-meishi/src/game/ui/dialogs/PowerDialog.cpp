#include "PowerDialog.h"
#include "game/ui/PageFunction.h"
#include "channel/ChannelApi.h"
#include "net/UserObj.h"
#include "EnvSet.h"
#include "SoundManager.h"
#include "act/PowerGiftControl.h"
#include "data/TableManager.h"

#define POWER_GET_MAX (150)

void PowerDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onClose")
	{
		this->setShow(false);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
		return;
	}
	else if (cName == "onPower")
	{
		if (sgPowerGiftControl()->getActID() == 1 && sgUserObj()->getPersonalInfo()->power() > POWER_GET_MAX)
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS),
			__String::createWithFormat("%s%s", sgTextHelper(TEXT_ACT_TILI).c_str(), sgTextHelper(TEXT_BUY_POWER_MAX).c_str())->getCString());
			return;
		}
		if (sgPowerGiftControl()->getActID() == 2 && sgUserObj()->getPersonalInfo()->energy() > POWER_GET_MAX)
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS),
				__String::createWithFormat("%s%s", sgTextHelper(TEXT_ACT_JINGLI).c_str(), sgTextHelper(TEXT_BUY_POWER_MAX).c_str())->getCString());
			return;
		}
		sgPowerGiftControl()->sendReceivePowerEnergyRqst();
	}
}

bool PowerDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PowerDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);

		mBtnPower = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnPower"));

		mTimeText1 = dynamic_cast<ui::Text*>(mBtnPower->getChildByName("mTimeLabel"));

		mTitleText_1 = dynamic_cast<ui::Text*>(mRoot->getChildByName("mTitleText_1"));

		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);


	return ret;
}

void PowerDialog::onShow()
{
	sgPowerGiftControl()->setPowerDialogPtr(this);

	if(mArmWan)
		mArmWan->removeFromParent();

	if (sgPowerGiftControl()->getActID() == 1)
	{
		mArmWan = cocostudio::Armature::create("fla_mianbao");
		mTitleText_1->setText(sgTextHelper(TEXT_POWER_TIPS));
	}
	else
	{
		mArmWan = cocostudio::Armature::create("fla_tili");
		mTitleText_1->setText(sgTextHelper(TEXT_ENERGY_TIPS));
	}

	mRoot->getChildByName("Node_5")->addChild(mArmWan);


	if (sgPowerGiftControl()->getActStartTime() > 0)
	{
		mTimeText1->setVisible(true);
		mBtnPower->setBright(false);
		mBtnPower->getChildByName("Sprite_1")->setVisible(false);
		mBtnPower->getChildByName("Sprite_2")->setVisible(false);
	}
	else 
	{
		mTimeText1->setVisible(false);
		if (sgPowerGiftControl()->getIsGet() == 1 && sgPowerGiftControl()->getActEndTime() > 0)
		{
			mBtnPower->setBright(true);
			mBtnPower->getChildByName("Sprite_1")->setVisible(true);
			mBtnPower->getChildByName("Sprite_2")->setVisible(false);
		}
		else
		{		
			mBtnPower->setBright(false);
			mBtnPower->getChildByName("Sprite_1")->setVisible(false);
			mBtnPower->getChildByName("Sprite_2")->setVisible(true);
			mArmWan->getAnimation()->play("play");
			if(mIsGet == 0)
				mArmWan->getAnimation()->gotoAndPause(74);
			else
				mIsGet = 0;
			if (sgPowerGiftControl()->getIsGet() != 1 && sgPowerGiftControl()->getActEndTime() <= 0)
				mBtnPower->setVisible(false);
		}
	}
	
} 

#ifdef MGAME_PLATFORM_MPLUS
void PowerDialog::showShareTips()
{
	GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_MPLUSHELP_SHARING), CC_CALLBACK_1(PowerDialog::onShare, this));
}
void PowerDialog::onShare(Ref*)
{
	GenerlDialog::hide();
	sgChannelApi()->callGameInfo(TYPE_SHARE_MSG_TYPE, "2");
}
#endif


void PowerDialog::onClose()
{
	ActionWindow::onClose();
	sgPowerGiftControl()->setPowerDialogPtr(NULL);
}

PowerDialog::PowerDialog() :mBtnPower(0), mBtnEnergy(0)
{
	mArmWan = NULL;
	mIsGet = 0;
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_tili.png", "fla_tili.plist", "fla_tili.xml");
}

PowerDialog::~PowerDialog()
{
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_tili.xml");
}

