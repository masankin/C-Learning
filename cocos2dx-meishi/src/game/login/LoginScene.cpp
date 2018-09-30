#include "LoginScene.h"
#include "channel/ChannelApi.h"
#include "core/MessageManager.h"
#include "login/LoginProxy.h"
#include "cocostudio/CocoStudio.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "ui/dialogs/ConnectingNode.h"
#include "ui/PageFunction.h"
#include "data/TableManager.h"
#include "net/NetManager.h"
#include "net/UserObj.h"
#include "core/SoundManager.h"
#include "update/UpdateMgrImp.h"
#include "SrvListControl.h"
#include "SrvDownNoticeDialog.h"
#include "game/utils/GameDefault.h"
#include "game/utils/Comm.h"
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
#include "platform/android/jni/ChannelJni.h"
#endif

using namespace cocos2d;
using namespace cocostudio;
using namespace cocos2d::ui;

#ifdef MGAME_PLATFORM_MPLUS
#define LOGIN_TIME_OUT 5.0
#else
#define LOGIN_TIME_OUT 15.0
#endif

#define LOGIN_GAME_OUT sgNetManager()->getConnectingEvent()->getMaxWaitingTime()//*3

enum eLoginType
{
	E_LOGIN_AUTO = 0,
	E_LOGIN_WX = 1,
	E_LOGIN_QQ,
	E_LOGIN_YK,
};

LoginScene::LoginScene(void) : m_iLoginType(E_LOGIN_AUTO), m_pCancelLoginListener(NULL)
{
	sgMessageManager()->regisiterMessageHandler(EMT_SDKLOGIN_SUCCESS, this);
}


LoginScene::~LoginScene(void)
{
	sgMessageManager()->removeMessageHandler(this);
	CCLOG("[trace]LoginScene::~LoginScene ok");
}

bool LoginScene::init()
{
	if (!GameScene::init())
	{
		return false;
	}

	Node* pNode = CSLoader::createNode("Update.csb");
	addChild(pNode);

	dynamic_cast<Node*>(pNode->getChildByName("loadingBack"))->setVisible(false);

	mLoginBtnWX = dynamic_cast<Button*>(pNode->getChildByName("Button_login_wx"));
	mLoginBtnWX->setTag(E_LOGIN_WX);
	mLoginBtnWX->addClickEventListener(CC_CALLBACK_1(LoginScene::buttonClickCallBack, this));

	mLoginBtnQQ = dynamic_cast<Button*>(pNode->getChildByName("Button_login_qq"));
	mLoginBtnQQ->setTag(E_LOGIN_QQ);
	mLoginBtnQQ->addClickEventListener(CC_CALLBACK_1(LoginScene::buttonClickCallBack, this));

	//mLoginBtnYK = dynamic_cast<Button*>(pNode->getChildByName("Button_login_yk"));
	//mLoginBtnYK->setTag(E_LOGIN_YK);
	//mLoginBtnYK->addClickEventListener(CC_CALLBACK_1(LoginScene::buttonClickCallBack, this));

	mLoginBtn = dynamic_cast<Button*>(pNode->getChildByName("Button_login"));
	mLoginBtn->addClickEventListener(CC_CALLBACK_1(LoginScene::buttonClickCallBack, this));

	mPanelSelectSrv = dynamic_cast<ui::Layout*>(pNode->getChildByName("PanelSelectSrv"));
	mPanelSelectSrv->addClickEventListener(CC_CALLBACK_1(LoginScene::selectSrvShow, this));
	mSrvNameTTF = dynamic_cast<ui::Text*>(mPanelSelectSrv->getChildByName("TextSrvName"));
	Size pSz = Director::getInstance()->getWinSize();

	Node* pConnecting = CSLoader::getInstance()->createNode("ui/ConnectingNode.csb");
	pConnecting->setPosition(pSz / 2);
	this->addChild(pConnecting);
	mJuhuaSprite = dynamic_cast<Sprite*>(pConnecting->getChildByName("juhua"));
	mJuhuaSprite->setVisible(false);
	dynamic_cast<ui::Text*>(pConnecting->getChildByName("ConnectingText"))->setVisible(false);

	mTextTip = Text::create(sgTextHelper(TEXT_LOGIN_WAIT), "fonts/font.TTF", 28);
	mTextTip->setPosition(ccp(pSz.width / 2, pSz.height*0.3));
	mTextTip->setTextColor(ccColor4B(141, 88, 57, 255));
	mTextTip->enableOutline(ccColor4B::WHITE, 3);
	mTextTip->setVisible(false);
	addChild(mTextTip);

	std::string szVer = UpdateMgrImp::getInstance()->GetCurrentVersion();
	mVersionTag = Text::create(StringUtils::format(sgTextHelper(TEXT_CUR_VER).c_str(), szVer.c_str()), "fonts/font.TTF", 16);
	mVersionTag->setPosition(ccp(10, pSz.height - 10));
	mVersionTag->setAnchorPoint(ccp(0, 1));
	mVersionTag->setColor(ccColor3B::WHITE);
	mVersionTag->enableShadow(Color4B(82, 82, 82, 255), Size(1, -1));
	addChild(mVersionTag);
	
#ifdef MGAME_TENCENT_PLATFORM
	Node* pLogo = pNode->getChildByName("logo_1");
	if (pLogo)
	{
		pLogo->setVisible(true);
	}
#endif

	m_iLoginType = USER_DEFAULT_INT("USER_LoginType");
	return true;
}

void LoginScene::onShow()
{
	GameScene::onShow();
	
	this->setLoginBtnEnable(true);

	if (!sgEnvSet("MGAME_OPEN_CHOSE_SERVER"))
		mPanelSelectSrv->setVisible(false);
	else
		mPanelSelectSrv->setVisible(true);



	mJuhuaSprite->setVisible(false);
	mJuhuaSprite->stopAllActions();

	mTextTip->setVisible(false);

	sgLoginProxy()->setLoginSuccess(false);
	sgNetManager()->clearCachePack();

	sgMessageManager()->regisiterMessageHandler(EMT_GAMESERVERLOGIN_SUCCESS, this);
	sgMessageManager()->regisiterMessageHandler(EMT_BUTTON_LOGINSCENE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_BUTTON_LOGINSCENE_UNVISIBLE, this);

#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	m_pCancelLoginListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_CANCEL_LOGIN, CC_CALLBACK_0(LoginScene::cancelLogin, this));
#endif
	if (sgEnvSet("MGAME_OPEN_CHOSE_SERVER"))
	{
		sgMessageManager()->regisiterMessageHandler(EMT_CHANGE_SERVER, this);
		sgSrvListControl()->requestSrvList();
	}
	else
	{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
		CCLOG("edward LoginScene : onShow channelid = %d, loginType = %d", sgChannelApi()->getChannelId(), m_iLoginType);
		if (sgChannelApi()->getChannelId() == ChannelApi::ChannelID_Tencent_ShouQ)
		{
			switch (m_iLoginType)
			{
			case E_LOGIN_WX:
				this->setLoginBtnEnable(false);
				scheduleUpdate();
				break;
			case E_LOGIN_QQ:
				if (sgChannelApi()->getIsDiffAccount() == true)
				{
					this->setLoginBtnEnable(false);
					scheduleUpdate();
				}
				else
				{
					m_iLoginType = E_LOGIN_AUTO;
					buttonClickCallBack(NULL);
				}
				break;
			case E_LOGIN_AUTO:
				m_iLoginType = E_LOGIN_QQ;
				SAVE_USER_DEFAULT_INT("USER_LoginType", m_iLoginType);
				buttonClickCallBack(NULL);
				break;
			}
			return;
		}
		else if (sgChannelApi()->getChannelId() == ChannelApi::ChannelID_Tencent_WeiXin)
		{
			switch (m_iLoginType)
			{
			case E_LOGIN_WX:
				if (sgChannelApi()->getIsDiffAccount() == true)
				{
					this->setLoginBtnEnable(false);
					scheduleUpdate();
				}
				else
				{
					m_iLoginType = E_LOGIN_AUTO;
					buttonClickCallBack(NULL);
				}
				break;
			case E_LOGIN_QQ:
				this->setLoginBtnEnable(false);
				scheduleUpdate();
				break;
			case E_LOGIN_AUTO:
				m_iLoginType = E_LOGIN_WX;
				SAVE_USER_DEFAULT_INT("USER_LoginType", m_iLoginType);
				buttonClickCallBack(NULL);
				break;
			}
			return;
		}
#endif

	if (m_iLoginType == E_LOGIN_WX || m_iLoginType == E_LOGIN_QQ)
	{
		m_iLoginType = E_LOGIN_AUTO;
		buttonClickCallBack(NULL);
	}
	else
		scheduleUpdate();

	}
}

void LoginScene::onClose()
{
	sgMessageManager()->removeMessageHandler(EMT_GAMESERVERLOGIN_SUCCESS);
	sgMessageManager()->removeMessageHandler(EMT_BUTTON_LOGINSCENE);
	sgMessageManager()->removeMessageHandler(EMT_BUTTON_LOGINSCENE_UNVISIBLE);
	if (sgEnvSet("MGAME_OPEN_CHOSE_SERVER"))
	{
		sgMessageManager()->removeMessageHandler(EMT_CHANGE_SERVER);
		sgSrvListControl()->deleteSrvList();
	}


#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	Director::getInstance()->getEventDispatcher()->removeEventListener(m_pCancelLoginListener);
#endif
	GameScene::onClose();
}

void LoginScene::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_SDKLOGIN_SUCCESS)
	{
		scheduleOnce(schedule_selector(LoginScene::updateLogin), LOGIN_GAME_OUT);

		sgLoginProxy()->analysisLoginDNS();
		sgLoginProxy()->requestLoginKey();

		playLoginAction();
	}
	else if (message->getMessageType() == EMT_GAMESERVERLOGIN_SUCCESS)
	{
		sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
	}
	else if (message->getMessageType() == EMT_BUTTON_LOGINSCENE)
	{
		m_iLoginType = E_LOGIN_AUTO;
		buttonClickCallBack(NULL);
	}
	else if (message->getMessageType() == EMT_BUTTON_LOGINSCENE_UNVISIBLE)
	{
		this->setLoginBtnEnable(false);
	}
	else if (message->getMessageType() == EMT_CHANGE_SERVER)
	{
		MessageChangeServer* pMessage = dynamic_cast<MessageChangeServer*>(message);
		if (!pMessage)
			return;
		int idx = pMessage->getServerNum();

		vector<SrvInfo*> vSrvList = sgSrvListControl()->getSrvList();
		if (vSrvList.empty() || vSrvList.size() - 1 < idx)
			return;

		SrvInfo* pSrvInfo = vSrvList.at(idx);
		if (!pSrvInfo)
			return;

		SAVE_USER_DEFAULT(USER_SERVER_NAME, pSrvInfo->mName);
		SAVE_USER_DEFAULT(USER_SERVER_IP, pSrvInfo->mIp);

		sgUserObj()->setLoginServerDir(pSrvInfo->mIp);
		
		// 设置当前服务器名字
		mSrvNameTTF->setString(pSrvInfo->mName);
		sgLoginProxy()->setServerState(LoginProxy::E_SS_INVALID);
		sgNetManager()->setEncryMsg(false);
	}
}

void LoginScene::updateLogin(float dt)
{
	this->setLoginBtnEnable(true);

	if (!sgEnvSet("MGAME_OPEN_CHOSE_SERVER"))
		mPanelSelectSrv->setVisible(false);
	else
		mPanelSelectSrv->setVisible(true);

	mJuhuaSprite->setVisible(false);
	mJuhuaSprite->stopAllActions();

	mTextTip->setVisible(false);
} 

void LoginScene::buttonClickCallBack(cocos2d::Ref* ref)
{
	this->setLoginBtnEnable(false);

	if (ref)
	{
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*) ref;
		m_iLoginType = btn->getTag();
		SAVE_USER_DEFAULT_INT("USER_LoginType", m_iLoginType);
	}
	if (sgEnvSet("MGAME_OPEN_CHOSE_SERVER"))
		mPanelSelectSrv->setVisible(false);

	//mLoginBtn->setTouchEnabled(false);

	CCLOG("LoginScene : buttonClickCallBack loginType %d", m_iLoginType);
	unscheduleUpdate();
	scheduleOnce(schedule_selector(LoginScene::updateLogin), LOGIN_TIME_OUT);
	sgChannelApi()->callLogin(m_iLoginType);
}

void LoginScene::playLoginAction()
{
	mJuhuaSprite->setVisible(true);
	mJuhuaSprite->stopAllActions();
	mJuhuaSprite->runAction(RepeatForever::create(CCRotateBy::create(0.3f, 180)));

	mTextTip->setVisible(true);
}

void LoginScene::selectSrvShow(cocos2d::Ref* ref)
{
	sgPageFunction()->gotoFunction(MSG_FUC_CHAGE_SERVER);
}

void LoginScene::cancelLogin()
{
	m_iLoginType = E_LOGIN_AUTO;
	sgChannelApi()->setChannelInfo(0, 1);
	SAVE_USER_DEFAULT_INT("USER_LoginType", E_LOGIN_AUTO);
	updateLogin(0.0f);
	unschedule(schedule_selector(LoginScene::updateLogin));
	scheduleUpdate();
}

void LoginScene::update(float dt)
{
	if (sgChannelApi()->getAccountChoice() == ChannelApi::E_CHOICE_NONE)
		return;
	/*{
		if (sgChannelApi()->getChannelId() == 0)
			return;

		if (sgChannelApi()->getChannelId() == ChannelApi::ChannelID_Tencent_WeiXin)
			m_iLoginType = E_LOGIN_WX;
		else if (sgChannelApi()->getChannelId() == ChannelApi::ChannelID_Tencent_ShouQ)
			m_iLoginType == E_LOGIN_QQ;

		SAVE_USER_DEFAULT_INT("USER_LoginType", m_iLoginType);
	}*/
	if (sgChannelApi()->getAccountChoice() == ChannelApi::E_CHOICE_LOCAL)
	{
		if (m_iLoginType == E_LOGIN_WX)
			sgChannelApi()->setChannelInfo(ChannelApi::ChannelID_Tencent_WeiXin, 1);
		else if (m_iLoginType == E_LOGIN_QQ)
			sgChannelApi()->setChannelInfo(ChannelApi::ChannelID_Tencent_ShouQ, 1);

		m_iLoginType = E_LOGIN_AUTO;
	}
	else if (sgChannelApi()->getAccountChoice() == ChannelApi::E_CHOICE_OUTSIDE)
	{
		if (sgChannelApi()->getChannelId() == ChannelApi::ChannelID_Tencent_WeiXin)
			m_iLoginType = E_LOGIN_WX;
		else if (sgChannelApi()->getChannelId() == ChannelApi::ChannelID_Tencent_ShouQ)
			m_iLoginType == E_LOGIN_QQ;

		SAVE_USER_DEFAULT_INT("USER_LoginType", m_iLoginType);
	}

	buttonClickCallBack(NULL);
	unscheduleUpdate();
	sgChannelApi()->setAccountChoice(ChannelApi::E_CHOICE_NONE);
}

void LoginScene::setLoginBtnEnable(bool enable)
{
#ifdef MGAME_TENCENT_PLATFORM
	if (mLoginBtnWX)
	{
		mLoginBtnWX->setVisible(enable);
		mLoginBtnWX->setEnabled(enable);
	}

	if (mLoginBtnQQ)
	{
		mLoginBtnQQ->setVisible(enable);
		mLoginBtnQQ->setEnabled(enable);
	}
#else
	
	if (mLoginBtnYK)
	{
		mLoginBtnYK->setVisible(true);
		mLoginBtnYK->setEnabled(true);
	}
#endif
}
