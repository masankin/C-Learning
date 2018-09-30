#include "GenerlDialog.h"
#include "game/ui//PageFunction.h"
#include "game/ui//system/Window.h"
#include "SoundManager.h"
#include "cocostudio/CCArmatureDataManager.h"
#include "game/login/LoginProxy.h"
#include "game/net/NetManager.h"
#include "CCLuaEngine.h"
#include "game/utils/GameDefault.h"
 
#define FONT_NAME ("fonts/font.TTF")

void GenerlDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onSure")
	{
		if (strcmp(mConfirmCallStr.c_str(),"") != 0)
			LuaEngine::getInstance()->executeString(mConfirmCallStr.c_str());

		this->setShow(false);
	}
	else if (cName == "onCancel")
	{
		if (strcmp(mCancelCallStr.c_str(), "") != 0)
			LuaEngine::getInstance()->executeString(mCancelCallStr.c_str());
	
		this->setShow(false);
	}
	else if (cName == "onClose")
	{
		if (strcmp(mCloseCallStr.c_str(), "") != 0)
			LuaEngine::getInstance()->executeString(mCloseCallStr.c_str());
		
		this->setShow(false);
	}
	sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
}

bool GenerlDialog::init()
{
	bool ret = false;
	do
	{
		cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("ui/fla_xiaomingban.xml");
		cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("ui/fla_damianban.xml");

		mRoot = CSLoader::getInstance()->createNode("ui/GenerlDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);

		mHeadLabel =  dynamic_cast<ui::Text*>(mRoot->getChildByName("mHeadLabel"));

		mContentLabel =  dynamic_cast<ui::Text*>(mRoot->getChildByName("mContentLabel"));

		mContentLabel->ignoreContentAdaptWithSize(false);
		mContentLabel->setTextAreaSize(Size(400, 200));
		mContentLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
		mContentLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
		mContentLabel->setContentSize(CCSizeMake(400, 200));

		mSure = dynamic_cast<ui::Widget*>(mRoot->getChildByName("m_sure"));
		mCancel = dynamic_cast<ui::Widget*>(mRoot->getChildByName("m_cancel"));

		//关闭按钮
		mClose = dynamic_cast<ui::Widget*>(mRoot->getChildByName("m_close"));

		mContentLabel1 =  dynamic_cast<ui::Text*>(mRoot->getChildByName("mContentLabel_0"));

		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);


	return ret;
}

void GenerlDialog::setHeadText(std::string stHead)
{
	GenerlDialog* pRet = dynamic_cast<GenerlDialog*>(sgPageFunction()->getWindowByPageName("GenerlDialog.csb"));
	if (pRet && pRet->mHeadLabel)
	{
		pRet->mHeadLabel->setText(stHead);
	}
}

void GenerlDialog::setContentText(std::string stContent)
{
	GenerlDialog* pRet = dynamic_cast<GenerlDialog*>(sgPageFunction()->getWindowByPageName("GenerlDialog.csb"));
	if (pRet && pRet->mContentLabel)
	{
		pRet->mContentLabel->setText(stContent);
		pRet->mContentLabel1->setVisible(false);
	}
}

void GenerlDialog::setContentShow()
{
	GenerlDialog* pRet = dynamic_cast<GenerlDialog*>(sgPageFunction()->getWindowByPageName("GenerlDialog.csb"));
	if (pRet && pRet->mContentLabel1)
	{
		pRet->mContentLabel1->setVisible(true);
	}
}

void GenerlDialog::show(std::string stHead, std::string stContent, bool bEnableClose)
{
	GenerlDialog* pRet = dynamic_cast<GenerlDialog*>(sgPageFunction()->createWindow(MSG_FUC_WIN_GENERL,"GenerlDialog.csb"));
	if (pRet && pRet->mContentLabel && pRet->mHeadLabel)
	{
		pRet->onLocateClickCallback(pRet->mRoot);
		pRet->mHeadLabel->setText(stHead);
		pRet->mContentLabel->setText(stContent);
		pRet->setShow(true);

		//关闭按钮
		if (pRet->mClose && pRet->mCancel && pRet->mSure)
		{
			pRet->mCancel->setVisible(false);
			if (bEnableClose)
			{
				pRet->mClose->setVisible(true);
				pRet->mSure->setVisible(true);
			}
			else
			{
				pRet->mClose->setVisible(false);
				pRet->mSure->setVisible(true);
			}
		}
		
	}
}

void GenerlDialog::show(std::string stHead, std::string stContent, const std::function<void(Ref*)>& callBack, bool isAll /* = false */, bool bEnableClose /*=true*/, eGeneralDialogType btnType /*= E_GENERAL_TYPE_NORMAL*/)
{
	GenerlDialog* pRet = dynamic_cast<GenerlDialog*>(sgPageFunction()->createWindow(MSG_FUC_WIN_GENERL, "GenerlDialog.csb"));
	if (pRet && pRet->mContentLabel && pRet->mHeadLabel)
	{
		pRet->onLocateClickCallback(pRet->mRoot);
		pRet->mHeadLabel->setText(stHead);
		pRet->mContentLabel->setText(stContent);
		pRet->setShow(true);

		pRet->mSure->addClickEventListener(callBack);

		//关闭按钮
		pRet->mClose->setVisible(bEnableClose);
		if (pRet->mCancel)
			pRet->mCancel->setVisible(false);
		if (bEnableClose)
		{
			if (isAll)
			{
				pRet->mClose->addClickEventListener(callBack);
			}
		}
		
		//确认框
		auto btnTextSprite = dynamic_cast<Sprite*>(pRet->mSure->getChildByName("Sprite_1"));
		if (btnType == E_GENERAL_TYPE_PK_CONFIRM)
		{
			btnTextSprite->setSpriteFrame("ui/general/general_btn_accept.png");
		}
		else {
			btnTextSprite->setSpriteFrame("ui/general/general_ziti sure.png");
		}
	}
}

void GenerlDialog::show(std::string stHead, std::string stContent, const std::function<void(Ref*)>& ConfirmCallBack, const std::function<void(Ref*)>& CloseCallBack, bool isAll, bool bEnableClose)
{
	GenerlDialog* pRet = dynamic_cast<GenerlDialog*>(sgPageFunction()->createWindow(MSG_FUC_WIN_GENERL, "GenerlDialog.csb"));
	if (pRet && pRet->mContentLabel && pRet->mHeadLabel)
	{
		pRet->onLocateClickCallback(pRet->mRoot);
		pRet->mHeadLabel->setText(stHead);
		pRet->mContentLabel->setText(stContent);
		pRet->setShow(true);

		pRet->mSure->addClickEventListener(ConfirmCallBack);
		pRet->mClose->addClickEventListener(CloseCallBack);

		//关闭按钮
		pRet->mClose->setVisible(bEnableClose);
		if (pRet->mCancel)
			pRet->mCancel->setVisible(false);
		if (bEnableClose)
		{
			if (isAll)
			{
				pRet->mClose->addClickEventListener(ConfirmCallBack);
			}
		}

	}
}

void GenerlDialog::showLuaDialog(std::string stHead, std::string stContent, const std::string ConfirmCallBackStr, const std::string CancelCallBackStr, const std::string CloseCallBackStr, bool bEnableCancel, bool bEnableClose)
{
	GenerlDialog* pRet = dynamic_cast<GenerlDialog*>(sgPageFunction()->createWindow(MSG_FUC_WIN_GENERL, "GenerlDialog.csb"));
	if (pRet && pRet->mContentLabel && pRet->mHeadLabel)
	{
		pRet->onLocateClickCallback(pRet->mRoot);
		pRet->mHeadLabel->setText(stHead);
		pRet->mContentLabel->setText(stContent);
		pRet->mContentLabel->setTextHorizontalAlignment(TextHAlignment::LEFT);
		pRet->setShow(true);

		pRet->mConfirmCallStr = ConfirmCallBackStr;	

		if (bEnableCancel)
		{
			pRet->mCancel->setVisible(true);
			pRet->mSure->setPositionX(pRet->mCancel->getPositionX()*(-1));
			pRet->mCancelCallStr = CancelCallBackStr;
		}
		else
		{
			pRet->mCancel->setVisible(false);
		}


		//关闭按钮
		pRet->mClose->setVisible(bEnableClose);
		if (bEnableClose)
			pRet->mCloseCallStr = CloseCallBackStr;	

	}
}

void GenerlDialog::onClickSureCallBack(cocos2d::Ref*)
{
	mContentLabel1->setVisible(false);
	setShow(false);
}

void GenerlDialog::hide()
{
	GenerlDialog* pRet = dynamic_cast<GenerlDialog*>(sgPageFunction()->getWindowByPageName("GenerlDialog.csb"));
	if (pRet)
	{
		pRet->setShow(false);
		pRet->onLocateClickCallback(pRet->mRoot);
		pRet->mContentLabel1->setVisible(false);
	}
}

GenerlDialog::GenerlDialog() :mHeadLabel(0), mContentLabel(0), mConfirmCallStr(""), mCancelCallStr(""), mCloseCallStr("")
{
}

GenerlDialog::~GenerlDialog()
{

}

void GenerlDialog::setCancel(std::function<void(Ref*)> callBack)
{
	//GenerlDialog* pRet = dynamic_cast<GenerlDialog*>(Window::Create("GenerlDialog.csb"));
	//if (pRet )
	//{
	//	pRet->mCancel->addClickEventListener(callBack);
	//}
}


/*=================================================我是华丽的分割线===============================================*/
/********************** 这个不用了, change by jim
void GeneralDialogOther::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onSure")
	{
		this->setShow(false);
	}
	else if (cName == "onCancel")
	{
		this->setShow(false);
	}
	sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
}

bool GeneralDialogOther::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/GenerlDialog2.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);

		m_pContentLabel = dynamic_cast<ui::Text*>(mRoot->getChildByName("mContentLabel"));

		m_pContentLabel->ignoreContentAdaptWithSize(false);
		m_pContentLabel->setTextAreaSize(Size(400, 200));
		m_pContentLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
		m_pContentLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
		m_pContentLabel->setContentSize(CCSizeMake(400, 200));

		m_pSure = dynamic_cast<ui::Widget*>(mRoot->getChildByName("m_sure"));
		m_pCancel = dynamic_cast<ui::Widget*>(mRoot->getChildByName("m_cancel"));
		m_pSureSprite = dynamic_cast<Sprite*>(m_pSure->getChildByName("Sprite_1"));
		m_pCancelSprite = dynamic_cast<Sprite*>(m_pCancel->getChildByName("Sprite_1"));

		m_pContentLabel1 = dynamic_cast<ui::Text*>(mRoot->getChildByName("mContentLabel_0"));

		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);


	return ret;
}

void GeneralDialogOther::setContentText(std::string stContent)
{
	GeneralDialogOther* pRet = dynamic_cast<GeneralDialogOther*>(Window::Create("GenerlDialog2.csb"));
	if (pRet && pRet->m_pContentLabel)
	{
		pRet->m_pContentLabel->setText(stContent);
	}
}

void GeneralDialogOther::setContentShow()
{
	GeneralDialogOther* pRet = dynamic_cast<GeneralDialogOther*>(Window::Create("GenerlDialog2.csb"));
	if (pRet && pRet->m_pContentLabel1)
	{
		pRet->m_pContentLabel1->setVisible(true);
	}
}

void GeneralDialogOther::show(std::string stContent, int iType)
{
	GeneralDialogOther* pRet = dynamic_cast<GeneralDialogOther*>(Window::Create("GenerlDialog2.csb"));
	if (pRet && pRet->m_pContentLabel)
	{
		pRet->m_pContentLabel->setFontName(FONT_NAME);
		pRet->onLocateClickCallback(pRet->mRoot);
		pRet->m_pContentLabel->setText(stContent);
		pRet->setShow(true);
		pRet->m_pSure->setVisible(true);
		pRet->m_pCancel->setVisible(false);
		pRet->m_pSure->setPositionX(0);
		pRet->m_pSureSprite->setSpriteFrame("ui/general/general_ziti sure.png");
	}
}

void GeneralDialogOther::show(std::string stContent, int iType, const std::function<void(Ref*)>& callBack)
{
	GeneralDialogOther* pRet = dynamic_cast<GeneralDialogOther*>(Window::Create("GenerlDialog2.csb"));
	if (pRet && pRet->m_pContentLabel)
	{
		pRet->m_pContentLabel->setFontName(FONT_NAME);
		pRet->onLocateClickCallback(pRet->mRoot);
		pRet->m_pContentLabel->setText(stContent);
		pRet->setShow(true);
		pRet->m_pSure->setVisible(true);
		pRet->m_pCancel->setVisible(true);
		pRet->m_pSure->setPositionX(120); 
		pRet->m_pSure->addClickEventListener(callBack);

		//确认框
		if (iType == E_GENERAL_TYPE_CONFIRM)
		{
			pRet->m_pSureSprite->setSpriteFrame("ui/general/general_ziti sure.png");
		}
		else if (iType == E_GENERAL_TYPE_PK_CONFIRM)
		{
			pRet->m_pSureSprite->setSpriteFrame("ui/general/general_btn_accept.png");
#ifdef MGAME_PLATFORM_MPLUS
			pRet->m_pContentLabel->setFontName("");
#else
			pRet->m_pContentLabel->setFontName(FONT_NAME);
#endif
		}
		else if(iType == E_GENERAL_TYPE_CANCEL)
		{ 
			pRet->m_pSure->setVisible(false); 
			pRet->m_pCancel->addClickEventListener(callBack); 
			pRet->m_pCancel->setPositionX(120); 
		}
	}
}

void GeneralDialogOther::show(std::string stContent, int iType, const std::function<void(Ref*)>& confirmCallBack, const std::function<void(Ref*)>& cancelCallBack)
{
	GeneralDialogOther* pRet = dynamic_cast<GeneralDialogOther*>(Window::Create("GenerlDialog2.csb"));
	if (pRet && pRet->m_pContentLabel)
	{
		pRet->m_pContentLabel->setFontName(FONT_NAME);
		pRet->m_pContentLabel->setText(stContent);
		pRet->setShow(true);
		pRet->m_pSure->setVisible(true);
		pRet->m_pCancel->setVisible(true);
		pRet->m_pSure->setPositionX(120);

		pRet->m_pSure->addClickEventListener(confirmCallBack);
		pRet->m_pCancel->addClickEventListener(cancelCallBack);
		//确认框
		if (iType == E_GENERAL_TYPE_CONFIRM)
		{
			pRet->m_pSureSprite->setSpriteFrame("ui/general/general_ziti sure.png");
		}
		else if (iType == E_GENERAL_TYPE_PK_CONFIRM)
		{
			pRet->m_pSureSprite->setSpriteFrame("ui/general/general_btn_accept.png");
#ifdef MGAME_PLATFORM_MPLUS
			pRet->m_pContentLabel->setFontName("");
#else
			pRet->m_pContentLabel->setFontName(FONT_NAME);
#endif
		}
	}
}

void GeneralDialogOther::onClickSureCallBack(cocos2d::Ref*)
{
	m_pContentLabel1->setVisible(false);
	setShow(false);
	m_pContentLabel->setFontName(FONT_NAME);
}

void GeneralDialogOther::hide()
{
	GeneralDialogOther* pRet = dynamic_cast<GeneralDialogOther*>(Window::Create("GenerlDialog2.csb"));
	if (pRet)
	{
		pRet->setShow(false);
		pRet->onLocateClickCallback(pRet->mRoot);
		pRet->m_pContentLabel1->setVisible(false);
		pRet->m_pContentLabel->setFontName(FONT_NAME);
	}
}

GeneralDialogOther::GeneralDialogOther() : m_pContentLabel(NULL)
{

}

GeneralDialogOther::~GeneralDialogOther()
{

}
************************/


ReLoginDialog::ReLoginDialog()
{

}

ReLoginDialog::~ReLoginDialog()
{
}

 void ReLoginDialog::show()
{
	sgPageFunction()->createWindow(MSG_FUC_RELOGIN, "ReLogin.csb");
}

 void ReLoginDialog::hide()
 {
	 
	 ReLoginDialog* pRet = dynamic_cast<ReLoginDialog*>(sgPageFunction()->getWindowByPageName("ReLogin.csb"));
	 if (pRet&& pRet->isShow())
	 {
		 pRet->showTextTip(sgTextHelper(TEXT_COMMITPS_ReLoginOk));
		 pRet->setShow(false);
	 }
 }

 void ReLoginDialog::showButton(bool isShowRetry)
 {
	 ReLoginDialog* pRet = dynamic_cast<ReLoginDialog*>(sgPageFunction()->getWindowByPageName("ReLogin.csb"));
	 if (pRet&& pRet->isShow() )
	 {
		 if (isShowRetry)
		 {
			 pRet->setShowButton(true);
		 }else
		 {
			 pRet->hideRetry();
		 } 
	 } 
 }

void ReLoginDialog::hideRetry()
{
	mConFailTxt->setVisible(true);
	mExitBtn->setVisible(true);
	mExitBtn->setTouchEnabled(true);
	mExitBtn->setPositionY(-220);
	mLoadingArm->getAnimation()->stop();
}

void ReLoginDialog::setShowButton(bool b)
{
	mExitBtn->setVisible(b);
	mReConBtn->setVisible(b);
	mExitBtn->setTouchEnabled(b);
	mReConBtn->setTouchEnabled(b);
	mExitBtn->setPositionY(-272);
	if (!b)
	{
		mLoadingArm->getAnimation()->play("play");
	}else
		mLoadingArm->getAnimation()->stop();
}

bool ReLoginDialog::init()
{
	if (!ActionWindow::init())
	{
		return false;
	}
	mRoot = CSLoader::getInstance()->createNode("ui/ReLogin.csb");

	mExitBtn = dynamic_cast<Button*>(mRoot->getChildByName("Button_2"));
	mReConBtn = dynamic_cast<Button*>(mRoot->getChildByName("Button_1"));
	mConFailTxt = dynamic_cast<Text*>(mRoot->getChildByName("Text_1"));


	
	mLoadingArm = cocostudio::Armature::create("fla_loading");
	dynamic_cast<Node*>(mRoot->getChildByName("Node_1"))->addChild(mLoadingArm);

	addChild(mRoot);
	onLocateClickCallback(mRoot);
	return true;
}

void ReLoginDialog::onShow()
{
	ActionWindow::onShow();
	mLoadingArm->getAnimation()->play("play");
	mExitBtn->setVisible(false);
	mReConBtn->setVisible(false);
	mExitBtn->setTouchEnabled(false);
	mReConBtn->setTouchEnabled(false);
	mConFailTxt->setVisible(false);
	sgLoginProxy()->setReloginShowing(true);
}

void ReLoginDialog::onClose()
{
	mLoadingArm->getAnimation()->stop();
	sgLoginProxy()->setReloginShowing(false);
	ActionWindow::onClose();
}

void ReLoginDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	setShowButton(false);
	if (cName == "onRecon")
	{
		sgLoginProxy()->onRetryEnterServer();

	}else if (cName == "onExit")
	{
		sgLoginProxy()->setServerState(LoginProxy::E_SS_INVALID);
		sgNetManager()->setEncryMsg(false);
		sgPageFunction()->gotoFunction(MSG_FUC_LOGIN);
	}
}

KickPlayerDialog::KickPlayerDialog()
{

}

KickPlayerDialog::~KickPlayerDialog()
{

}

bool KickPlayerDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::createNode("ui/KickPlayer.csb");
		CC_BREAK_IF(!mRoot);

		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		this->addChild(mRoot);

		setPopType(E_POP_TYPE_SMALL);
		onLocateClickCallback(mRoot);

		ret = true;

	} while (0);


	return ret;
}

void KickPlayerDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "reLogin")
	{
		//点击登录
		sgMessageManager()->sendMessage(EMT_BUTTON_LOGINSCENE, "");
		setShow(false);
	}
	else if (cName == "changePassword")
	{
		return;
	}
}


TipsManager::TipsManager()
{
}

TipsManager::~TipsManager()
{
}

void TipsManager::showTextTip(const std::string pStr, Point pPoint)
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
	Director::getInstance()->getNotificationNode()->addChild(pText, 1000);
	pText->setPosition(pWinSize / 2);

	pText->setString(pStr);
	pText->runAction(Sequence::create(Spawn::create(MoveBy::create(2.0, Vec2(0, 250)), FadeOut::create(2.0), NULL),
		RemoveSelf::create(),
		NULL));
}
