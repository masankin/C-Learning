
#include "game/ui//PageFunction.h"
#include "MessageManager.h"
#include "game/data/TableManager.h"

//add by wayen
#include "update/UpdateMgrImp.h"
#include "update/UpdateScene.h"

//add by airel
#include "login/LogoScene.h"
#include "MoneyBuyNode.h"
#include "SoundManager.h"
#include "net/NetManager.h"

#include "luaHelp/CppCallLuaHelper.h"
#include "CCLuaEngine.h"
#include "core/ResManager.h"
#include "game/utils/GameDefault.h"

#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
#include "platform/android/jni/ChannelJni.h"
#endif
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "login/VideoLayer.h"
#endif

PageFunction::PageFunction(void)
	:mDiamondPanel(0),
	mNetworkTips(nullptr),
	mActionPowerLayer(nullptr),
	mActionDiamondLayer(nullptr),
	mActionPackageLayer(nullptr),
	mActionChestLayer(nullptr),
	mIsLoadSceneRes(false),
	mIsRePlay(false),
	mCurrentLoadingPage(MSG_FUC_BEGIN)
{
	sgMessageManager()->regisiterMessageHandler(EMT_PAGE_GO, this);
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_CANCEL_LOGIN, CC_CALLBACK_0(PageFunction::logout, this));
#endif
}

PageFunction::~PageFunction()
{
	sgMessageManager()->removeMessageHandler(EMT_PAGE_GO, this);
}

void PageFunction::onReceiveMassage(GameMessage * message)
{
	MessagePage* pMes = dynamic_cast<MessagePage*>(message);
	if (!pMes) return;



	//重复界面id 不执行跳转
	CCLOG("[trace]PageFunction::onReceiveMassage,massageType=[%d] luaType:[%d] name[%s]", pMes->getPageType(),pMes->getLuaType(), pMes->getPageName().c_str());
	if(mCurrentUIType == pMes->getPageType()) return;
	//资源加载
	if (pMes->getIsCacheRes() == false)
	{
		if (isSence(pMes->getPageType()))
		{
			int lpage = (int)pMes->getPageType();

			auto levelDialog = sgPageFunction()->getWindowByPageName("LevelDialog.csb");
			if (levelDialog != NULL && levelDialog->isShow() && lpage == MSG_FUC_LOGIN)
				levelDialog->setShow(false);
			
			const char* pScript = __String::createWithFormat("sgSceneManager():enterScene(%d)", lpage)->getCString();
			LuaEngine::getInstance()->executeString(pScript);
			return;
		}
		else
		{
			int lpage = (int)pMes->getPageType();
			if (pMes->getLuaType())
			{
				lpage = sgCppCallLuaHelper()->getLuaWindowId(pMes->getPageName());
			}
			const char* pScript = __String::createWithFormat("sgSceneManager():loadPanelRes(%d)", lpage)->getCString();
			LuaEngine::getInstance()->executeString(pScript);
		}
	}
	showWaitPage();
	if (pMes->getLuaType())
	{
		Window* pPage = Window::Create(pMes->getPageName(), pMes->getPageType(), pMes->getLuaType());
		if (pPage)
			pPage->setShow(true);
	}
	else
	{
		if (pMes->getPageType() == MSG_FUC_LOGIN)
		{
			SAVE_USER_DEFAULT_BOOL("USER_inLoginScene", true);
		}
		else
		{
			SAVE_USER_DEFAULT_BOOL("USER_inLoginScene", false);
		}

		if (pMes->getPageType() == MSG_FUC_LOGO)
		{
			Scene* pScene = LogoScene::create();
			Director::getInstance()->runWithScene(pScene);
			mCurrentUIType = MSG_FUC_LOGO;
		}
		else if (pMes->getPageType() == MSG_FUC_UPDATE)
		{
			GameScene* pLoginScene = GameScene::getGameSceneByName("Update.csb");
			if (pLoginScene)
			{
				pLoginScene->unload();
				pLoginScene->onClose();
				sgResManager()->purgeResByPageType(pLoginScene->getPageType());
			}
			Scene* pScene = UpdateScene::create();
			Director::getInstance()->replaceScene(pScene);
			mCurrentUIType = pMes->getPageType();
		}
		else if (pMes->getPageType() == MSG_FUC_LOGIN)
		{
			GameScene* pPage = GameScene::Create("Update.csb", pMes->getPageType());
			showPage(pPage->getScene());
			mCurrentUIType = pMes->getPageType();
		}
		else if (pMes->getPageType() == MSG_FUC_MAIN_LEVEL)
		{
			if (!GameScene::getGameSceneByName("SelectWorldPage.csb"))
			{
				GameScene* pPage = GameScene::Create("SelectWorldPage.csb", pMes->getPageType());
				showPage(pPage->getScene());
				mCurrentUIType = pMes->getPageType();

				sgSoundManager()->playMusicBg(MUSIC_UI_MAIN);
			}
			
		}
		else if (pMes->getPageType() == MSG_FUC_MINI_LEVEL)
		{
			if (!GameScene::getGameSceneByName("SelectLevelPage.csb"))
			{
				GameScene* pPage = GameScene::Create("SelectLevelPage.csb", pMes->getPageType());
				showPage(pPage->getScene());
				mCurrentUIType = pMes->getPageType();

				sgSoundManager()->playMusicBg(MUSIC_UI_SMALL_GATE);
			}

		}
		else if (pMes->getPageType() == MSG_FUC_PLAYING)
		{
			GameScene* pPage = GameScene::Create("GamePage.csb", pMes->getPageType());
			showPage(pPage->getScene());

			if (!GameScene::getGameSceneByName("GamePage.csb"))
			{
				pPage->removeFromParent();
				mIsRePlay = true;
				GameScene* pPage = GameScene::Create("GamePage.csb", pMes->getPageType());
				showPage(pPage->getScene());
			}
			mCurrentUIType = pMes->getPageType();
		}

		else if (pMes->getPageType() == MSG_FUC_WIN_FAIL)
		{
			Window* pPage = Window::Create("FailDialog.csb", pMes->getPageType());
			pPage->setShow(true);
			sgSoundManager()->setBackgroundMusicVolume(0.1f);
			sgSoundManager()->playMusicEffect(SOUND_UI_FAIL);
		}
		else if (pMes->getPageType() == MSG_FUC_WIN_RESULT)
		{
			Window* pPage = Window::Create("ResultDialog.csb", pMes->getPageType());
			pPage->setShow(true);

		}
		else if (pMes->getPageType() == MSG_FUC_WIN_LEVEL)
		{
			Window* pPage = Window::Create("LevelDialog.csb", pMes->getPageType());
			pPage->setShow(true);

		}
		else if (pMes->getPageType() == MSG_FUC_WIN_PAUSE)
		{
			Window* pPage = Window::Create("PauseDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_WIN_GENERL)
		{
			Window* pPage = Window::Create("GenerlDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_WIN_SHOP)
		{
			Window* pPage = Window::Create("ShopNode.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_WIN_SET)
		{
			Window* pPage = Window::Create("SetDialog.csb",  pMes->getPageType());
			pPage->setShow(true);
			enableOptionButton(false);
		}
		else if (pMes->getPageType() == MSG_FUC_ACHIEVE)
		{
			Window* pPage = Window::Create("AchieveNode.csb", pMes->getPageType());
			pPage->setShow(true);
			enableOptionButton(false);
		}
		else if (pMes->getPageType() == MSG_FUC_WIN_PVP)
		{
			Window* pPage = Window::Create("PVPDialog.csb", pMes->getPageType());
			pPage->setShow(true);
			enableOptionButton(false);
			mCurrentUIType = pMes->getPageType();

			sgSoundManager()->playMusicBg(MUSIC_UI_PVPDIALOG);
		}
		else if (pMes->getPageType() == MSG_FUC_ACT)
		{
			Window* pPage = Window::Create("ActNode.csb", pMes->getPageType());
			pPage->setShow(true);
			enableOptionButton(false);
		}
		else if (pMes->getPageType() == MSG_FUC_ACT_LOGIN)
		{
			Window* pPage = Window::Create("ActLoginPanel.csb", pMes->getPageType());
			pPage->setShow(true);
			enableOptionButton(false);
		}

		else if (pMes->getPageType() == MSG_FUC_WINNER)
		{
			Window* pPage = Window::Create("WinnerDialog.csb", pMes->getPageType());
			pPage->setShow(true);
			enableOptionButton(false);
		}

		else if (pMes->getPageType() == MSG_FUC_WIN_PRO)
		{
			Window* pPage = Window::Create("ProDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_RANKING)
		{
			Window* pPage = Window::Create("RankingDialog.csb", pMes->getPageType());
			pPage->setShow(true);
			enableOptionButton(false);
		}
		else if (pMes->getPageType() == MSG_FUC_GIFTDIALOG)
		{
			Window* pPage = Window::Create("GiftAwardNode.csb", pMes->getPageType());
			//pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_PVP_RESULT)
		{
			Window* pPage = Window::Create("PVPResult.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_SET_NAME)
		{
			Window* pPage = Window::Create("SetNameDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_FAIL_PROMPT)
		{
			Window* pPage = Window::Create("FailPrompt.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_WIN_PVP_WATCH)
		{
			Window* pPage = Window::Create("PVPWatchDialog.csb", pMes->getPageType());
			pPage->setShow(true);
			enableOptionButton(false);
		}
		else if (pMes->getPageType() == MSG_FUC_WIN_PVP_ROOM_BACK)
		{
			Window* pPage = Window::Create("PVPRoomBackDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_BACKPACK)
		{
			Window* pPage = Window::Create("backpackDialog.csb", pMes->getPageType());
			pPage->setShow(true);
			enableOptionButton(false);
		}
		else if (pMes->getPageType() == MSG_FUC_CONNECTING)
		{
			Window* pPage = Window::Create("ConnectingNode.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_POWER_GIFT)
		{
			Window* pPage = Window::Create("PowerDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_FIRST_CHARGE)
		{
			Window* pPage = Window::Create("ActFirstCharge.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_DAYTASK)
		{
			Window* pPage = Window::Create("DayTaskNode.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_CHATMSG)
		{
			Window* pPage = Window::Create("ChattingDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_FRIEND)
		{
			Window* pPage = Window::Create("FriendPanel.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_WIN_PROPNOSET)
		{
			Window* pPage = Window::Create("PropNoSetDilog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_SET_HEAD)
		{
			Window* pPage = Window::Create("SetHeadDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_PVPROOM)
		{
			Window* pPage = Window::Create("PVPRoom.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_SAVEDATA)
		{
			GameScene* pPage = GameScene::Create("SyncPage.csb", pMes->getPageType());
			showPage(pPage->getScene());
		}
		else if (pMes->getPageType() == MSG_FUC_NEWER_BIBLE)
		{
			Window* pPage = Window::Create("NewsBibleDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_PVP_MATCH_LOADING)
		{
			Window* pPage = Window::Create("PVPMatchLoadingDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_KICKPLAYER)
		{
			Window* pPage = Window::Create("KickPlayer.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_CRAZYWEEK)
		{
			Window* pPage = Window::Create("CrazyWeekDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_MAIL)
		{
			Window* pPage = Window::Create("MailNode.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_HONOREXCHANGE)
		{
			Window* pPage = Window::Create("HonorShopDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_ACTINFORM)
		{
			if (Window::getShowingWindowSize() == 0)
			{
				Window* pPage = Window::Create("ActInform.csb", pMes->getPageType());
				pPage->setShow(true);
			}
		}
		else if (pMes->getPageType() == MSG_FUC_VIP)
		{
			Window* pPage = Window::Create("VIPDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_PLAYER_UPGRADE)
		{
			Window* pPage = Window::Create("upgradeNode.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_VIP_TIME)
		{
			Window* pPage = Window::Create("VipTimeEnd.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_RANKSESSION_UPGRADE)
		{
			Window* pPage = Window::Create("UpRankSessionNode.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_PVP_HELP)
		{
			Window* pPage = Window::Create("PvpInfoDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_CHAGE_SERVER)
		{
			Window* pPage = Window::Create("SrvListDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_BADGE)
		{
			Window* pPage = Window::Create("BadgeDialog.csb", pMes->getPageType());
			pPage->setShow(true);

		}
		else if (pMes->getPageType() == MSG_FUC_INFO)
		{
			//todo
			sgChattingControl()->getChattingProxy()->sendQueryPlayerInfoRpsr(sgUserObj()->getCurInfoUin());
			Window* pPage = Window::Create("InfoPanel.csb", pMes->getPageType());
			pPage->setShow(false);
		}
		else if (pMes->getPageType() ==  MSG_FUC_SRVDOWN_NOTICE)
		{
			Window* pPage = Window::Create("SrvDownNoticeDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() ==  MSG_FUC_RESULT_BOX)
		{
			Window* pPage = Window::Create("BoxRewardDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() ==  MSG_FUC_NAMEING_DIALOG)
		{
			Window* pPage = Window::Create("NameingDialog.csb", pMes->getPageType());
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_RELOGIN)
		{
			Window* pPage = Window::Create("ReLogin.csb", pMes->getPageType());
			pPage->setShow(true);
		}
#ifdef MGAME_PLATFORM_MPLUS
		else if (pMes->getPageType() == MSG_FUC_FIVE_STAR)
		{
			Window* pPage = Window::Create("FiveStar.csb");
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_INVITECODE)
		{
			Window* pPage = Window::Create("InviteCodeDialog.csb");
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_CUSTOMER_SERVER)
		{
			Window* pPage = Window::Create("CustomerService.csb");
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_FREEDIAMOND)
		{
			Window* pPage = Window::Create("FreeDiamond.csb");
			pPage->setShow(true);
		}
		else if (pMes->getPageType() == MSG_FUC_DAILY_HEROES)
		{
			Window* pPage = Window::Create("DailyHeroesNode.csb", pMes->getPageType());
			pPage->setShow(true);
		}
#endif
	}
	if (pMes->getLuaType())
	{
		int luaWinId = sgCppCallLuaHelper()->getLuaWindowId(pMes->getPageName());
		assert(luaWinId);

		sgMessageManager()->sendMessage(EMT_OPEN_UI, StringUtils::format("%d", luaWinId));
	}
	else{
		sgMessageManager()->sendMessage(EMT_OPEN_UI, StringUtils::format("%d", pMes->getPageType()));
	}
	mIsRePlay = false;
	mCurrentLoadingPage = MSG_FUC_BEGIN;
}

void PageFunction::gotoFunction(const ePageType massageType, const std::string mesName, const int luaType, bool isCacheRes /*= false*/)
{
	if (this->checkMassage(massageType))
	{
		char params[64] = { 0, };
		sprintf(params, "%d %s %d %d", massageType, mesName.c_str(), luaType, isCacheRes);
		if (isSence(massageType))
		{
			if (!isCacheRes && mCurrentLoadingPage == massageType)
			{
				return;
			}
			mCurrentLoadingPage = massageType;

			if (mCurrentUIType != massageType && !isCacheRes)
				mIsLoadSceneRes = true;
		}
		else
		{
			if (mIsLoadSceneRes)
			{
				mWaitShowPageVec.push_back(params);
				return;
			}
		}
		sgMessageManager()->sendMessage(EMT_PAGE_GO, params);
	}
}
bool PageFunction::checkMassage(const ePageType massageType)
{
	return (massageType > MSG_FUC_BEGIN && massageType < MSG_FUC_END) || (massageType >= MSG_FUC_LUA_WIN && massageType < MSG_FUC_LUA_WIN +100);
}


void PageFunction::showPage(Scene* pScene)
{
	auto director = Director::getInstance();
	auto pCurScene = director->getRunningScene();
	if (pScene && pCurScene != NULL )
	{
		GameScene* pPage = static_cast<GameScene*>(pCurScene->getUserData());
		if (pPage && !mIsRePlay)
		{
			ePageType lType = pPage->getPageType();
			pPage->unload();
			pPage->onClose();
			GameScene::PerGameScene = pPage;
			if (pScene == pCurScene)
			{
				return;
			}
			sgResManager()->purgeResByPageType(lType);
		}

		GameScene* pCurPage = static_cast<GameScene*>(pScene->getUserData());
		if (pCurPage)
			GameScene::CurGameScene = pCurPage;

		//director->replaceScene(pScene);

		//modify by wayen 
		pCurPage->loadAsync([pCurPage, pScene]()
		{	
			auto director = Director::getInstance();
			pCurPage->resetWindowManager();
			director->replaceScene(pScene);
			pCurPage->onShow();
		});
	}
	else
	{
		GameScene* pPage = static_cast<GameScene*>(pScene->getUserData());
		if (pPage)
			GameScene::CurGameScene = pPage;
		
		//director->runWithScene(pScene);

		pPage->loadAsync([pPage, pScene]()
		{
			auto director = Director::getInstance();
			pPage->resetWindowManager();
			director->runWithScene(pScene);
			pPage->onShow();
		});
	}
	Window::setAllWindowHide();
}

bool PageFunction::isSence(const ePageType massageType)
{
	if (massageType == MSG_FUC_LOGIN ||
		massageType == MSG_FUC_MAIN_LEVEL ||
		massageType == MSG_FUC_MINI_LEVEL ||
		massageType == MSG_FUC_PLAYING)
	{
		return true;
	}
	return false;
}

Node* PageFunction::getDiamondPanel()
{
	if (!mDiamondPanel)
	{
		mDiamondPanel = MoneyBuyNode::create();
		mDiamondPanel->setVisible(false);
	}

	return mDiamondPanel;
}


void PageFunction::setDiamondPanel(bool _visible, GameScene* showScene)
{
	getDiamondPanel()->setVisible(_visible);

	if (showScene)
	{
		getDiamondPanel()->removeFromParentAndCleanup(false);
		showScene->getScene()->addChild(getDiamondPanel(),99);
	}
}

void PageFunction::enableOptionButton(bool visible)
{
	return;
	char cEnableParam[64] = { 0, };
	sprintf(cEnableParam, "%d", visible ? 1 : 0);
	sgMessageManager()->sendMessage(EMT_ENABLE_OPTIONBUTTON, cEnableParam);

	Size sWinSize = Director::getInstance()->getWinSize();
	if (!mDiamondPanel)
		return;

	if (visible)
	{
		mDiamondPanel->setPosition(-120, 0);
	}
	else
	{
		mDiamondPanel->setPosition(0, 0);
	}
}

ActionGain* PageFunction::getActionPowerLayer()
{
	if (!mActionPowerLayer)
	{
		mActionPowerLayer = ActionGain::create();
		Director::getInstance()->getNotificationNode()->addChild(mActionPowerLayer);
	}
	return mActionPowerLayer;
}

ActionGain* PageFunction::getActionDiamondLayer()
{
	if (!mActionDiamondLayer)
	{
		mActionDiamondLayer = ActionGain::create();
		Director::getInstance()->getNotificationNode()->addChild(mActionDiamondLayer);
	}
	return mActionDiamondLayer;
}

ActionGain* PageFunction::getActionPackageLayer()
{
	if (!mActionPackageLayer)
	{
		mActionPackageLayer = ActionGain::create();
		Director::getInstance()->getNotificationNode()->addChild(mActionPackageLayer);
	}
	return mActionPackageLayer;
}

ActionGain* PageFunction::getActionChestLayer()
{
	if (!mActionChestLayer)
	{
		mActionChestLayer = ActionGain::create();
		Director::getInstance()->getNotificationNode()->addChild(mActionChestLayer);
	}
	return mActionChestLayer;
}

Window* PageFunction::getWindowByPageName(std::string pageName)
{
	return Window::getWindow(pageName);
}

Window* PageFunction::createWindow(const ePageType massageType, const std::string mesName, const int luaType, bool isCacheRes /*= false*/)
{
	if (getWindowByPageName(mesName))
	{
		return getWindowByPageName(mesName);
	}

	if (this->checkMassage(massageType))
	{
		char params[64] = { 0, };
		sprintf(params, "%d %s %d %d", massageType, mesName.c_str(), luaType, isCacheRes);
		const GameMessage* gameMessage = sgMessageManager()->createMessage(EMT_PAGE_GO, params);
		onReceiveMassage((GameMessage*)(gameMessage));

		return getWindowByPageName(mesName);
	}
	return NULL;
}

void PageFunction::showWaitPage()
{
	mIsLoadSceneRes = false;

	for (auto it = mWaitShowPageVec.begin(); it != mWaitShowPageVec.end(); it++)
	{
		sgMessageManager()->sendMessage(EMT_PAGE_GO, *it);
	}
	mWaitShowPageVec.clear();
}

void PageFunction::logout()
{
	if (mCurrentUIType == MSG_FUC_LOGIN || mCurrentUIType == MSG_FUC_LOGO)
		return;

	sgLoginProxy()->sendLogOut();
	gotoFunction(MSG_FUC_LOGIN);

	//每日任务相关数据重置
	sgUserObj()->resetDailyMissionData();
}
