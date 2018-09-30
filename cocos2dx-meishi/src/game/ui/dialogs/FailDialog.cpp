#include "FailDialog.h"
#include "game/ui//PageFunction.h"
#include "game/ui//system/Window.h"
#include "game/data/Tables.h"
#include "game/data/TableManager.h"
#include "battle/BattleControl.h"
#include "SoundManager.h"
#include "ResultDialog.h"
#include "prop/PropsControl.h"
#include "battle/BattleRecordsManager.h"
#include "shop/ShopPanel.h"
#include "guide/GuideManager.h"
#include "game/utils/GameDefault.h"
#include "game/balance/BalanceControl.h"
#include "lua/LuaFuncHelper.h"

FailDialog::~FailDialog()
{
	//cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_shibai.xml");
}


FailDialog::FailDialog()
{
	//modify by weihua
	//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_shibai.xml");
}

void FailDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	sgSoundManager()->beganBattleEffect();

	if (cName == "onResultBack")
	{
		/*if (sgGamePrecedure()->getCurMapInfo().checkpt / 1000 == 2)
			sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL_2);
		else*/
		sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);

		//this->setShow(false);

		return;
	}
	else if (cName == "onResult")
	{
		int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
		int curPower = sgUserObj()->getPersonalInfo()->power();//sgUserObj()->getPvePower();
		bool needConsumePower = sgGuideManager()->checkNeedPower(sgGamePrecedure()->getCurMapInfo().checkpt);
		const CheckPoint* pCP = sgTableManager()->getCheckPointByID(checkpt);
		if (!pCP || pCP->consumeEnergy > curPower && needConsumePower)
		{
			std::string szTitle = sgTextHelper(TEXT_COMMTIPS_TIPS);
			std::string szContent =   StringUtils::format("%s,%s?",sgTextHelper(TEXT_COMMTIPS_NO_POWERS).c_str(),sgTextHelper(TEXT_BUY_ITEM_TILI_SURE).c_str());

			GenerlDialog::show(szTitle, szContent,   CC_CALLBACK_1(FailDialog::onCallBackBuyPower,this));

			return;
		}
		if (needConsumePower)
		{
			//sgUserObj()->sendEnterPoint(checkpt);
			//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBthRestartMission");
			int pTime = sgBalanceControl()->getBattleTime();
			const char* pScript = String::createWithFormat("sgBthControl():sendRestartRequest(%d, %d)", 1, pTime / 1000)->getCString();
			LuaEngine::getInstance()->executeString(pScript);
		}
		else
		{
			sgBattleRecordsManager()->mBattleRecords.clear(); 
			sgPageFunction()->setCurrentUIType(MSG_FUC_MINI_LEVEL);
			PageFunction::Get()->gotoFunction(MSG_FUC_PLAYING);
			//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBthRestartMissionGuide");
			int pTime = sgBalanceControl()->getBattleTime();
			const char* pScript = String::createWithFormat("sgBthControl():sendRestartRequest(%d, %d)", 0, pTime / 1000)->getCString();
			LuaEngine::getInstance()->executeString(pScript);
		}
	}
	else if (cName == "onTeach")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_NEWER_BIBLE);
	}
	else if (cName == "onSkip")
	{
		sgUserObj()->onSkip(mCurLevelId);
	}
}

bool FailDialog::init()
{
	bool ret = false;
	do
	{

		mRoot = CSLoader::getInstance()->createNode("ui/FailDialog.csb");
		CC_BREAK_IF(!mRoot);
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));

		//mRoot->getChildByName("LOSE")->runAction(RepeatForever::create(RotateBy::create(4.5f,360)));

		this->addChild(mRoot);

		setAnimation("fla_shibai");

		mStarLvNode = StarLvNode::create();
		mCenter->addChild(mStarLvNode,10);

		mArmatureGuang = cocostudio::Armature::create("fla_shibai_ADDITIVE");
		mCenter->addChild(mArmatureGuang,11);
		ccBlendFunc tmp_oBlendFunc;
		tmp_oBlendFunc.src = 1;
		tmp_oBlendFunc.dst = 1;
		mArmatureGuang->setBlendFunc(tmp_oBlendFunc);
		mArmatureGuang->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(FailDialog::windowAniCallback));

		setPopType(E_POP_TYPE_NONE);
		
		auto bible = (Layout*)mRoot->getChildByName("bible");
		mBibleAni = cocostudio::CCArmature::create("fla_jiaoxue");		
		mBibleAni->setPosition(bible->getPosition());
		mRoot->addChild(mBibleAni, bible->getZOrder());
		mBibleAni->getAnimation()->play("play");

		ret = true;

	} while (0);

	return ret;
}

void FailDialog::onShow()
{
	mStarLvNode->resetStarStr();

	int tag = sgUserObj()->getFirstFail();
	auto text = (Text*)mRoot->getChildByName("text");
	auto sprite = (Sprite*)mRoot->getChildByName("dhk_6");	
	if (tag == 1)
	{		
		const TextHelper* pTextHelper = sgTableManager()->getTextHelperByID(350);
		auto str = pTextHelper->text;
		text->setText(str);
		text->setVisible(true);

		sprite->setVisible(true);
		sgUserObj()->setFirstFail(0);

	}
	else
	{
		text->setVisible(false);
		sprite->setVisible(false);
	}
	//mRoot->getChildByName("LOSE")->stopAllActions();
	//mRoot->getChildByName("LOSE")->runAction(RepeatForever::create(RotateBy::create(4.5f,360)));
	int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
	mCurLevelId = checkpt;

	if (sgBattleRecordsManager()->getIsPlayback() == false)
	{
		//��¼�ιؿ�ʧ�ܴ���
		char buff[32];
		unsigned int uin = sgUserObj()->getUin();
		memset(buff, 0, 32);
		sprintf(buff, "%d_%d%s", uin, checkpt, "failCount");
		int deadCount = USER_DEFAULT_INT(buff);
		SAVE_USER_DEFAULT_INT(buff, deadCount + 1);

		auto skipBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("skipBtn"));
		auto nextBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("mNextLevel"));
		const CheckPoint* p_Info = sgTableManager()->getCheckPointByID(checkpt); 
		if ((sgUserObj()->getGateState(checkpt) == eCheckPointState::E_CHECKPOINTSTATE_OPEN) && (p_Info->isSkip == 1) && !(deadCount + 1 < p_Info->failSkipCount))
		{
			skipBtn->setVisible(true);
			auto skipValue = dynamic_cast<ui::Text*>(skipBtn->getChildByName("Text_1"));
			int hasSkipCount = sgUserObj()->getHasSkipCount();
			int skipSize = p_Info->skipValueList.size();
			skipValue->setString(p_Info->skipValueList.at((hasSkipCount < skipSize) ? hasSkipCount : (skipSize - 1)));

			nextBtn->setPositionX(Director::getInstance()->getVisibleSize().width - skipBtn->getPositionX());
		}
		else
		{
			skipBtn->setVisible(false);
		}
	}

	if (sgUserObj()->getIsShowDailyMissionProgress())
	{
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eOpenMissionCompleteDialog", NULL);
		sgUserObj()->setIsShowDailyMissionProgress(false);
	}
	onLocateClickCallback(mRoot);
}

void FailDialog::aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{

}

void FailDialog::onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex)
{

}

void FailDialog::onCallBackBuyPower(Ref*)
{
	int pDiamond = sgUserObj()->getPersonalInfo()->diamond();// sgUserObj()->getGoldCoin();
	if (pDiamond>=50)
	{
		if(sgChannelApi()->isOnlineSdk() == true)
		{
			sgPropsControl()->sendBuyPropRqst(PVE_POWER_TYPE, 50, 1);
		}
		else
		{
		}

	}else
	{
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_BUY_ITEM_NO_EN),CC_CALLBACK_1(FailDialog::onCallBackBuyDiamond, this));
	}

}

void FailDialog::onCallBackBuyDiamond(Ref*)
{
	ShopPanel::showShop();
}

void FailDialog::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_SKIP_SUCCESS)
		this->setShow(false);
}
