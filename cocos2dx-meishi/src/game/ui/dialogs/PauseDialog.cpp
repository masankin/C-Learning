#include "PauseDialog.h"
#include "game/ui//PageFunction.h"
#include "game/ui//system/Window.h"
#include "game/data/Tables.h"
#include "game/data/TableManager.h"
#include "battle/BattleControl.h"
#include "pvp/PvpControl.h"
#include "balance/BalanceControl.h"
#include "SoundManager.h"
#include "prop/PropsControl.h"
#include "battle/BattleRecordsManager.h"
#include "shop/ShopPanel.h"
#include "guide/GuideManager.h"
#include "lua/LuaFuncHelper.h"
#include "game/utils/GameDefault.h"

void PauseDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onStopMusic_Bg")
	{
		//背景音乐
		bool isPlayBg = sgSoundManager()->getIsPlayMusicBg();

		if (isPlayBg == true)
		{
			sgSoundManager()->setIsPlayMusicBg(false);
			sgSoundManager()->stopMusicBg();
			dynamic_cast<Button*>(sender)->setBrightStyle(BrightStyle::HIGHLIGHT);
			//mRoot->getChildByName("mMusic")->setVisible(true);
		}
		else
		{
			sgSoundManager()->setIsPlayMusicBg(true);
			//找到当前场景的音乐开始播
			int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
			sgSoundManager()->playMusicBg(checkpt);
			dynamic_cast<Button*>(sender)->setBrightStyle(BrightStyle::NORMAL);
			//mRoot->getChildByName("mMusic")->setVisible(false);
		}
	}
	else if (cName == "onStopMusic_Effect")
	{
		//音效
		bool isplayEffect = sgSoundManager()->getIsPlayMusicEffect();

		if (isplayEffect == true)
		{
			sgSoundManager()->setIsPlayMusicEffect(false);
			sgSoundManager()->stopAllMusicEffects();
			dynamic_cast<Button*>(sender)->setBrightStyle(BrightStyle::HIGHLIGHT);
			//mRoot->getChildByName("mVoice")->setVisible(true);
		}
		else
		{
			sgSoundManager()->setIsPlayMusicEffect(true);
			dynamic_cast<Button*>(sender)->setBrightStyle(BrightStyle::NORMAL);
			//mRoot->getChildByName("mVoice")->setVisible(false);
		}
	}
	else if (cName == "onResume")
	{
		//sgGamePrecedure()->setGameIng(true);
		this->setShow(false);
	}
	else if (cName == "onMenu")
	{
		// 战斗场景点击返回，判定失败
		sgBalanceControl()->doCheckPtEnd(false, true);

		//by crystal 2016/3/4
		//int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
		//if (checkpt / 1000 == 1)
		//	PageFunction::Get()->gotoFunction(MSG_FUC_MINI_LEVEL);
		//else if (checkpt / 1000 == 2)
		//	PageFunction::Get()->gotoFunction(MSG_FUC_MINI_LEVEL);
		//PageFunction::Get()->gotoFunction(MSG_FUC_MINI_LEVEL_2);

		this->setShow(false);
		//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBthQuitMission");
		int pTime = sgBalanceControl()->getBattleTime();
		const char* pScript = String::createWithFormat("sgBthControl():sendQuitRequest(%d)", pTime / 1000)->getCString();
		LuaEngine::getInstance()->executeString(pScript);

		char buff[32];
		unsigned int uin = sgUserObj()->getUin();
		int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
		memset(buff, 0, 32);
		sprintf(buff, "%d_%d%s", uin, checkpt, "failCount");
		int deadCount = USER_DEFAULT_INT(buff);
		SAVE_USER_DEFAULT_INT(buff, deadCount + 1);
	}
	else if (cName == "onRegame")
	{	
		//上报每日任务数据
		sgUserObj()->sendDailyMissionData(false);

		restart();
	}
	else if (cName == "onGame")
	{
	}
}

bool PauseDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PauseDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		ret = true;
	} while (0);

	return ret;
}

void PauseDialog::onShow()
{
	bool bMusic = sgSoundManager()->getIsPlayMusicBg();
	bool bVoice = sgSoundManager()->getIsPlayMusicEffect();

	auto musicButton = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_1"));
	auto voiceButton = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_2"));

	//更新按钮状态
	if (!bMusic)
	{
		musicButton->setBrightStyle(BrightStyle::HIGHLIGHT);
	}
	else
	{
		musicButton->setBrightStyle(BrightStyle::NORMAL);
	}

	if (!bVoice)
	{
		voiceButton->setBrightStyle(BrightStyle::HIGHLIGHT);
	}
	else
	{
		voiceButton->setBrightStyle(BrightStyle::NORMAL);
	}

	mButton = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_5"));

	mButton->setTouchEnabled(true);
	mButton->setBrightStyle(BrightStyle::NORMAL);

	m_pChangePowerListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener("eBthChangePower", CC_CALLBACK_0(PauseDialog::restart, this));

}

void PauseDialog::onClose()
{
	Window::onClose();
	//此界面关闭 游戏action继续
	sgGamePrecedure()->setGameIng(true);

	if (sgGamePrecedure()->getEnterGame())
		sgMessageManager()->sendMessage(EMT_LITTLETIPS, "0");

	Director::getInstance()->getEventDispatcher()->removeEventListener(m_pChangePowerListener);
}

void PauseDialog::onCallBackBuyPower(Ref*)
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
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_BUY_ITEM_NO_EN),CC_CALLBACK_1(PauseDialog::onCallBackBuyDiamond, this));
	}

}

void PauseDialog::onCallBackBuyDiamond(Ref*)
{
	ShopPanel::showShop();
}

void PauseDialog::restart()
{
	if (!sgBattleControl()->getIsPvp())
	{
		int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
		int curPower = sgUserObj()->getPersonalInfo()->power(); //sgUserObj()->getPvePower();
		bool needConsumePower = sgGuideManager()->checkNeedPower(sgGamePrecedure()->getCurMapInfo().checkpt);
		const CheckPoint* pCP = sgTableManager()->getCheckPointByID(checkpt);
		if (!pCP || pCP->consumeEnergy > curPower && needConsumePower)
		{
			//std::string szTitle = sgTextHelper(TEXT_COMMTIPS_TIPS);
			//std::string szContent =   StringUtils::format("%s,%s?",sgTextHelper(TEXT_COMMTIPS_NO_POWERS).c_str(),sgTextHelper(TEXT_BUY_ITEM_TILI_SURE).c_str());
			//GenerlDialog::show(szTitle, szContent,   CC_CALLBACK_1(PauseDialog::onCallBackBuyPower,this));
			sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "BthPowerDialog.csb", 1);

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
			sgPageFunction()->gotoFunction(MSG_FUC_PLAYING, "GamePage.csb", 0, true);
			//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBthRestartMissionGuide");
			int pTime = sgBalanceControl()->getBattleTime();
			const char* pScript = String::createWithFormat("sgBthControl():sendRestartRequest(%d, %d)", 0, pTime / 1000)->getCString();
			LuaEngine::getInstance()->executeString(pScript);
		}

		char buff[32];
		unsigned int uin = sgUserObj()->getUin();
		memset(buff, 0, 32);
		sprintf(buff, "%d_%d%s", uin, checkpt, "failCount");
		int deadCount = USER_DEFAULT_INT(buff);
		SAVE_USER_DEFAULT_INT(buff, deadCount + 1);
	}
	else
	{
		//std::string strParam = StringUtils::format("%d %s", 1, "win32");
		//sgMessageManager()->sendMessage(EMT_CHANNEL_LOGIN, strParam);
		this->setShow(false);
	}
}
