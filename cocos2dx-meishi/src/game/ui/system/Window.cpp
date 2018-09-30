#include "Window.h"
#include "game/ui//PageFunction.h"
#include "achieve/AchievePanel.h"
#include "game/winner/WinnerControl.h"
#include "act/ActPanel.h"
#include "game/notice/NoticeDialog.h"
#include "game/ui/dialogs/GiftAwardDialog.h"
#include "game/ui/dialogs/About.h"
#include "game/ui/dialogs/ConnectingNode.h"
#include "game/ui/dialogs/BackPackDialog.h"
#include "game/ui/dialogs/DayTaskDialog.h"
#include "game/act/ActFirstCharge.h"
#include "game/ui/dialogs/ActivationDialog.h"
#include "game/chat/ChattingDialog.h"
#include "game/chat/ExpressionPanel.h"
#include "game/ui/dialogs/PvpRoom.h"
#include "game/ui/dialogs/FriendDialog.h"
#include "game/ui/dialogs/PvpInviteDialog.h"
#include "game/ui/dialogs/SetHeadDialog.h"
#include "game/chat/InfoPanel.h"
#include "game/ui/dialogs/NewsBibleDialog.h"
#include "prop/PropItem.h"
#include "data/TableManager.h"
#include "winner/WinnerGift.h"
#include "game/ui/dialogs/CrazyWeekDialog.h"
#include "shop/ShopPanel.h"
#include "game/mail/MailProxy.h"
#include "game/ui/dialogs/HonorShopDialog.h"
#include "game/notice/ActInform.h"
#include "game/ui/dialogs/VipDialog.h"
#include "game/ui/dialogs/VipTimeDialog.h"
#include "game/ui/dialogs/PlayerUpgradeDialog.h"
#include "game/ui/dialogs/UpRankSessionDialog.h"
#include "ResManager.h"
#include "core/luaHelp/CppCallLuaHelper.h"
#include "game/login/SrvListDialog.h"
#include "game/ui/dialogs/BadgeDialog.h"
#include "game/login/SrvDownNoticeDialog.h"
#include "game/ui/dialogs/BoxRewardDialog.h"
#include "CCLuaEngine.h"
#include "game/battle/BattleControl.h"

#ifdef MGAME_PLATFORM_MPLUS
#include "mplus/CustomerService.h"
#include "mplus/FiveStar.h"
#include "mplus/InviteCodeDialog.h"
#include "mplus/DailyHeroes.h"
#endif

Widget* Window::LAYER_UIMANAGER_LAYER = nullptr;

map<std::string, Window*> Window::CACHE_WINDOWS = *(new std::map<std::string, Window*>());


bool Window::initOnceFlag = false;

Window::Window()
	:mIsCanClean(true)
	,mIsNoRelease(false)
{

}

Window::Window(ePriority priority)
{
}

Window::Window(Widget* belongTo)
{
}

Window::Window(ePriority priority, Widget* showFrom)
{
}

ePriority Window::getPriority()
{
	return mPriority;
}
//窗口暂时不分类
void Window::initOnce()
{
	//if (!initOnceFlag)
	//{
	//	LAYER_UIMANAGER_LAYER->addChild(LAYER_WINDOW);
	//	initOnceFlag = true;
	//}
}

Widget* Window::getShowFromLayer()
{
	return pShowFrom;
}

void Window::setWindowLayoutEnable(bool enable)
{
	this->pWindowLayout->setVisible(enable);
}

bool Window::isWindowLayoutEnable()
{
	return this->pWindowLayout->isVisible();
}

bool Window::_initWindow(ePriority priority, Widget* showFrom)
{
	//first init
	//Window::initOnce();
	//

	//Size size = Director::getInstance()->getWinSize();

	this->bIsShow = false;

	mPriority = priority;
	pShowFrom = showFrom != NULL ? showFrom : LAYER_UIMANAGER_LAYER;
	pWindowLayout = CSLoader::getInstance()->createNode("GenerlPage.csb");
	pWindowLayout->retain();
	addChildCenter(this);
	//this->setPosition(Vec2(-1 * this->getContentSize().width / 2, -1 * this->getContentSize().height / 2));
	pWindowLayout->addChild(this);
	pWindowLayout->setZOrder((int)this->mPriority);
	
	return true;
}

void Window::setShow(bool b)
{
	if (bIsShow != b)
	{
		if (b)
		{
			Window::Add(this);
			this->bIsShow = true;
			this->onShow();
		}
		else
		{
			Window::Remove(this);
			this->bIsShow = false;
			this->onClose();
			this->afterClose();
			cleanUp();
		}
	}
	//else if (bIsShow&&b)
	//{
	//	Window::Add(this);
	//}
}

bool Window::isShow()
{
	return bIsShow;
}

void Window::addChildCenter(Node* node)
{
	Vec2 pos;
	Size size = (Director::getInstance()->getWinSize() - node->getContentSize()) / 2;
	pos.x = size.width;
	pos.y = size.height;
	node->setPosition(pos);
}


void Window::Add(Window* ui)
{
	ui->getShowFromLayer()->addChild(ui->pWindowLayout);
}

void Window::Remove(Window* ui)
{
	ui->pWindowLayout->removeFromParent();
	/*map<std::string, Window*>::iterator it = CACHE_WINDOWS.find(ui->getPageName());
	if (it != CACHE_WINDOWS.end())
	{
	CACHE_WINDOWS.erase(it);
	}*/
}

Window* Window::Create(std::string pageName, ePageType pageType,const int luaType )
{
	map<std::string, Window*>::iterator it = CACHE_WINDOWS.find(pageName);
	if (it != CACHE_WINDOWS.end())
	{
		if (luaType == 1)
		{
			int luaWinId = sgCppCallLuaHelper()->getLuaWindowId(pageName);
			sgResManager()->loadResByPageType(luaWinId);
		}
		else
		{
			sgResManager()->loadResByPageType(pageType);
		}
		return it->second;
	}
	else
	{
		Window* gameWindow = createWithName(pageName,   pageType, luaType);
		if (gameWindow && gameWindow->_initWindow(PRIORITY_WINDOW, NULL))
		{

			CACHE_WINDOWS.insert(std::make_pair(pageName, gameWindow));	
			return gameWindow;
		}
	}

	return nullptr;
}

Window* Window::createWithName(std::string pageName, ePageType pageType, const int luaType)
{
	Window* pPage = nullptr;
	//目前代码中init用到了资源
	if (luaType == 1)
	{
		int luaWinId = sgCppCallLuaHelper()->getLuaWindowId(pageName);
		sgResManager()->loadResByPageType(luaWinId);
	}
	else
	{
		sgResManager()->loadResByPageType(pageType);
	}
	if (luaType)
	{
		pPage = sgCppCallLuaHelper()->CallOnReceiveMassage(pageName);
	}
	else
	{
		if (pageName == "ResultDialog.csb")
			pPage = ResultDialog::create();
		else if (pageName == "FailDialog.csb")
			pPage = FailDialog::create();
		else if (pageName == "LevelDialog.csb")
			pPage = LevelDialog::create();
		else if (pageName == "PauseDialog.csb")
			pPage = PauseDialog::create();
		else if (pageName == "GenerlDialog.csb")
			pPage = GenerlDialog::create();
		else if (pageName == "ShopNode.csb")
			pPage = ShopPanel::create();
		else if (pageName == "SetDialog.csb")
			pPage = SetDialog::create();
		else if (pageName == "PVPDialog.csb")
			pPage = PvpDialog::create();
		else if (pageName == "AchieveNode.csb")
			pPage = AchievePanel::create();
		else if (pageName == "ActNode.csb")
			pPage = ActPanel::create();
		else if (pageName == "RankingDialog.csb")
			pPage = RankingDialog::create();
		else if (pageName == "WinnerDialog.csb")
			pPage = WinnerDialog::create();
		else if (pageName == "ProDialog.csb")
			pPage = ProDialog::create();
		else if (pageName == "NoticeDialog.csb")
			pPage = NoticeDialog::create();
		else if (pageName == "GiftAwardNode.csb")
			pPage = GiftAwardDialog::create();
		//else if (pageName == "SetNameDialog.csb")
		//	pPage = SetNameDialog::create();
		else if (pageName == "PVPResult.csb")
			pPage = PVPResult::create();
		else if (pageName == "ShopDetailed.csb")
			pPage = ShopDetailed::create();
		else if (pageName == "PVPWatchDialog.csb")
			pPage = PVPWatchDialog::create();
		else if (pageName == "PVPRoomBackDialog.csb")
			pPage = PVPRoomBackDialog::create();
		else if (pageName == "About.csb")
			pPage = About::create();
		else if (pageName == "ConnectingNode.csb")
			pPage = ConnectingNode::create();
		else if (pageName == "backpackDialog.csb")
			pPage = BackPackDialog::create();
		else if (pageName == "DayTaskNode.csb")
			pPage = DayTaskDialog::create();
		else if (pageName == "PowerDialog.csb")
			pPage = PowerDialog::create();
		//else if (pageName == "ActNewerPacksDialog.csb")
		//	pPage = ActNewerPacksDialog::create();
		else if (pageName == "ActivationDialog.csb")
			pPage = ActivationDialog::create();
		else if (pageName == "ChattingDialog.csb")
			pPage = ChattingDialog::create();
		else if (pageName == "ExpressionPanel.csb")
			pPage = ExpressionPanel::create();
		else if (pageName == "FriendPanel.csb")
			pPage = FriendDialog::create();
		else if (pageName == "PropNoSetDilog.csb")
			pPage = PropNoSetDilog::create();
		else if (pageName == "PVPRoom.csb")
			pPage = PvpRoom::create();
		else if (pageName == "PVPInvite.csb")
			pPage = PvpInviteDialog::create();
		else if (pageName == "SetHeadDialog.csb")
			pPage = SetHeadDialog::create();
		else if (pageName == "InfoPanel.csb")
			pPage = InfoPanel::create();
		//else if (pageName == "GenerlDialog2.csb")
		//	pPage = GeneralDialogOther::create();
		else if (pageName == "PropExchangeView")
			pPage = PropExchangeView::create();
		else if (pageName == "NewsBibleDialog.csb")
			pPage = NewsBibleDialog::create();
		else if (pageName == "PvpInfoDialog.csb")
			pPage = PvpInfoDialog::create();
		else if (pageName == "ReLogin.csb")
			pPage = ReLoginDialog::create();
		else if (pageName == "PVPMatchLoadingDialog.csb")
			pPage = PVPMatchLoadingDialog::create();
		else if (pageName == "WinnerGift.csb")
			pPage = WinnerGift::create();
		else if (pageName == "KickPlayer.csb")
			pPage = KickPlayerDialog::create();
		else if (pageName == "CrazyWeekDialog.csb")
			pPage = CrazyWeekDialog::create();
		else if (pageName == "RankingInfoNode.csb")
			pPage = RankingInfoDialog::create();
		else if (pageName == "MailNode.csb")
			pPage = MailPanel::create();
		else if (pageName == "UpRankSessionNode.csb")
			pPage = UpRankSessionDialog::create();
		else if (pageName == "ActFirstCharge.csb")
			pPage = ActFirstCharge::create();
		/*else if (pageName == "ResultDialog.csb")
			pPage = ResultDialog::create();
		else if (pageName == "FailDialog.csb")
			pPage = FailDialog::create();
		else if (pageName == "LevelDialog.csb")
			pPage = LevelDialog::create();
		else if (pageName == "PauseDialog.csb")
			pPage = PauseDialog::create();
		else if (pageName == "GenerlDialog.csb")
			pPage = GenerlDialog::create();
		else if (pageName == "ShopNode.csb")
			pPage = ShopPanel::create();
		else if (pageName == "PVPDialog.csb")
			pPage = PvpDialog::create();
		else if (pageName == "RankingDialog.csb")
			pPage = RankingDialog::create();
		else if (pageName == "WinnerDialog.csb")
			pPage = WinnerDialog::create();
		else if (pageName == "ProDialog.csb")
			pPage = ProDialog::create();
		else if (pageName == "NoticeDialog.csb")
			pPage = NoticeDialog::create();
		else if (pageName == "GiftAwardNode.csb")
			pPage = GiftAwardDialog::create();
		else if (pageName == "SetNameDialog.csb")
			pPage = SetNameDialog::create();
		else if (pageName == "PVPResult.csb")
			pPage = PVPResult::create();
		else if (pageName == "ShopDetailed.csb")
			pPage = ShopDetailed::create();
		else if (pageName == "PVPWatchDialog.csb")
			pPage = PVPWatchDialog::create();
		else if (pageName == "PVPRoomBackDialog.csb")
			pPage = PVPRoomBackDialog::create();
		else if (pageName == "About.csb")
			pPage = About::create();
		else if (pageName == "ConnectingNode.csb")
			pPage = ConnectingNode::create();
		else if (pageName == "backpackDialog.csb")
			pPage = BackPackDialog::create();
		else if (pageName == "DayTaskNode.csb")
			pPage = DayTaskDialog::create();
		else if (pageName == "PowerDialog.csb")
			pPage = PowerDialog::create();
		else if (pageName == "ActFirstCharge.csb")
			pPage = ActFirstCharge::create();
		else if (pageName == "ActivationDialog.csb")
			pPage = ActivationDialog::create();
		else if (pageName == "ChattingDialog.csb")
			pPage = ChattingDialog::create();
		else if (pageName == "ExpressionPanel.csb")
			pPage = ExpressionPanel::create();
		else if (pageName == "FriendPanel.csb")
			pPage = FriendDialog::create();
		else if (pageName == "PropNoSetDilog.csb")
			pPage = PropNoSetDilog::create();
		else if (pageName == "PVPRoom.csb")
			pPage = PvpRoom::create();
		else if (pageName == "PVPInvite.csb")
			pPage = PvpInviteDialog::create();
		else if (pageName == "SetHeadDialog.csb")
			pPage = SetHeadDialog::create();
		else if (pageName == "InfoPanel.csb")
			pPage = InfoPanel::create();
		else if (pageName == "PropExchangeView")
			pPage = PropExchangeView::create();
		else if (pageName == "NewsBibleDialog.csb")
			pPage = NewsBibleDialog::create();
		else if (pageName == "PvpInfoDialog.csb")
			pPage = PvpInfoDialog::create();
		else if (pageName == "ReLogin.csb")
			pPage = ReLoginDialog::create();
		else if (pageName == "PVPMatchLoadingDialog.csb")
			pPage = PVPMatchLoadingDialog::create();
		else if (pageName == "WinnerGift.csb")
			pPage = WinnerGift::create();
		else if (pageName == "KickPlayer.csb")
			pPage = KickPlayerDialog::create();
		else if (pageName == "CrazyWeekDialog.csb")
			pPage = CrazyWeekDialog::create();
		else if (pageName == "RankingInfoDialog.csb")
			pPage = RankingInfoDialog::create();
		else if (pageName == "MailNode.csb")
			pPage = MailPanel::create();
			*/
		else if (pageName == "HonorShopDialog.csb")
			pPage = HonorShopDialog::create();
		else if (pageName == "ActInform.csb")
			pPage = ActInform::create();
		else if (pageName == "VIPDialog.csb")
			pPage = VipDialog::create();
		else if (pageName == "VipTimeEnd.csb")
			pPage = VipTimeDialog::create();
		else if (pageName == "upgradeNode.csb")
			pPage = PlayerUpgradeDialog::create();
		else if (pageName == "SrvListDialog.csb")
			pPage = SrvListDialog::create();
		else if (pageName == "BadgeDialog.csb")
			pPage = BadgeDialog::create();
		else if (pageName == "SrvDownNoticeDialog.csb")
			pPage = SrvDownNoticeDialog::create();
		else if (pageName == "ChattingTips.csb")
			pPage = ChattingTips::create();
		else if (pageName == "BoxRewardDialog.csb")
			pPage = BoxRewardDialog::create();
		else if (pageName == "NameingDialog.csb")
			pPage = NameingDialog::create();
#ifdef MGAME_PLATFORM_MPLUS
		else if (pageName == "SetDialog.csb")
			pPage = SetDialog::create();
		else if (pageName == "CustomerService.csb")
			pPage = CustomerService::create();
		else if (pageName == "InviteCodeDialog.csb")
			pPage = InviteCodeDialog::create();
		else if (pageName == "FiveStar.csb")
			pPage = FiveStar::create();
		else if (pageName == "FreeDiamond.csb")
			pPage = FreeDiamond::create();
		else if (pageName == "DailyHeroesNode.csb")
			pPage = DailyHeroes::create();
#endif
		else
		{
			pPage = sgCppCallLuaHelper()->CallOnReceiveMassage(pageName);
		}
	}

	if (pPage != nullptr)
	{
		pPage->setLuaType(luaType);
		pPage->setPageType(pageType);
		pPage->setPageName(pageName);
	}
	return pPage;
}

void Window::setAllWindowHide()
{
	for (auto it = CACHE_WINDOWS.begin(); it != CACHE_WINDOWS.end();)
	{
		auto window = it->second;
		if (window && !window->mIsNoRelease)
		{
			if (window->isShow())
			{
				window->onClose();
			}

			it = CACHE_WINDOWS.erase(it);
			Window::Remove(window);

			window->removeFromParent();
			window = nullptr;
		}
		else
			it++;
	}
}

void Window::showTextTip(const std::string pStr, Point pPoint, Size pSize)
{
	Text* pText = Text::create();
	pText->setFontSize(36);
	pText->setFontName("font.TTF");
	pText->setTextColor(Color4B(111, 254, 22, 255));
	Size pWinSize = Director::getInstance()->getWinSize();
	pText->setPosition(pPoint//mRoot->getPosition()
		);
	//阴影,暂不用参数控制开关
	pText->enableShadow(Color4B::BLACK, Size(1, -1));
	Director::getInstance()->getNotificationNode()->addChild(pText,1000);
	pText->setPosition(pWinSize/2);

	pText->setString(pStr);
	if (pSize.width != 0.0)
	{
		pText->ignoreContentAdaptWithSize(false);
		pText->setContentSize(pSize);
	}
		
	pText->runAction(Sequence::create(Spawn::create(MoveBy::create(2.0,Vec2(0,250)),FadeOut::create(2.0),NULL),
		RemoveSelf::create(),
		NULL));

}

void Window::setPageType(ePageType pageType)
{
	mPageType = pageType;
}

void Window::onShow()
{
	if (mLuaType  == 1)
	{
		int luaWinId = sgCppCallLuaHelper()->getLuaWindowId(mPageName);
		sgResManager()->loadResByPageType(luaWinId);
	}
	else
	{
		sgResManager()->loadResByPageType(mPageType);
	}
	if (getLuaType())
	{
		sgCppCallLuaHelper()->onShow(this, true);
	}
}

void Window::onClose()
{
	if (mLuaType == 1)
	{
		int luaWinId = sgCppCallLuaHelper()->getLuaWindowId(mPageName);
		sgResManager()->purgeResByPageType(luaWinId);
	}
	else
	{
		sgResManager()->purgeResByPageType(mPageType);
	}
	if (getLuaType())
	{
		sgCppCallLuaHelper()->onShow(this, false);
	}

	PageBase::cleanup();
	
	auto msgHandle = dynamic_cast<MessageHandler*>(this);
	if (msgHandle)
		sgMessageManager()->removeMessageHandler(msgHandle);

	if (mPageName == "LevelModeTip.csb" && !sgBattleControl()->getIsPvp())
		LuaEngine::getInstance()->executeString("sgUiUtils():saveSceneImage(1)");
}

void Window::afterClose()
{
	if (this->getShowingWindowSize() == 0)
		sgMessageManager()->sendMessage(EMT_OPEN_DAILY_MISSION, "");
}

void Window::setBgOpacity(int opacity)
{
	Layout* m = (Layout*)pWindowLayout->getChildByName("Panel_5");
	m->setBackGroundColor(Color3B::BLACK);
	m->setBackGroundColorOpacity(opacity);
}

void Window::cleanUp()
{
	if (!mIsCanClean)
	{
		return;
	}
	map<std::string, Window*>::iterator it = CACHE_WINDOWS.find(getPageName());
	if (it != CACHE_WINDOWS.end())
	{
		CACHE_WINDOWS.erase(it);
	}
	removeFromParent();
}

Window* Window::getWindow(std::string pageName)
{
	Window* pWindow = NULL;
	auto it = CACHE_WINDOWS.find(pageName);
	if (it != CACHE_WINDOWS.end())
	{
		pWindow = (*it).second;
	}

	return pWindow;
}

Layout* Window::getBgPanel()
{
	Layout* m = (Layout*)pWindowLayout->getChildByName("Panel_5");
	return m;
}

int Window::getWindowSize()
{
	return CACHE_WINDOWS.size();
}

int Window::getShowingWindowSize()
{
	int pRet = 0;
	for (auto it = CACHE_WINDOWS.begin(); it != CACHE_WINDOWS.end(); it++)
	{
		if (it->second->isShow() && it->first.compare("SevenGoalNode.csb") != 0)
		{
			pRet += 1;
		}
	}
	return pRet;
}
