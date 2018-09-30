#include "SyncPage.h"
#include "login/LoginProxy.h"
#include "MessageManager.h"
#include "net/proto/ClientSvrMsg.pb.h"
#include "ui/PageFunction.h"
#include "Constant.h"
#include "net/UserObj.h"
#include "SyncControl.h"
#include "data/TableManager.h"
SyncPage::SyncPage()
{
	
}

SyncPage::~SyncPage()
{

}

void SyncPage::onReceiveMassage(GameMessage * message)
{
	if(message->getMessageType() == EMT_RQST_RESULT)
	{
		RqstResultMessage* pMsg = dynamic_cast<RqstResultMessage* >(message);
		int id = pMsg->getID();
		int result = pMsg->getResult();
		switch (id)
		{
		default:
			break;
		}
		 
	}
}

void SyncPage::doSyncSaveData()
{  
	playLoadingAnimation(true); 	
	showGettingDataDialog(true); 
	showGetDataFiledDialog(false);
	playLoadingAnimation(true);
	sgSyncControl()->createSyncTimer();
	return; 
}


void SyncPage::onCanelSaveData()
{
	playLoadingAnimation(false); 
	sgUserObj()->setSaveDataOffLine(true);
	gotoMainPage();
}

void SyncPage::doLoginOverTime()
{ 
	playLoadingAnimation(false); 
	sgUserObj()->setSaveDataOffLine(true);
	gotoMainPage(); 
	
}

void SyncPage::gotoMainPage()
{
	sgSyncControl()->setSyncSaveDataOver(true);
	sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
}

bool SyncPage::init()
{
	bool ret = false;
	do 
	{
		initUI();
		CC_BREAK_IF(GameScene::init() == false);
		playLoadingAnimation(true);
		ret = true;
	} while (0);
	return ret;
}


void SyncPage::initUI()
{
	mRoot = CSLoader::getInstance()->createNode("ui/SyncLoadingScene.csb");
	mRoot->setAnchorPoint(Vec2(0.5, 0.5));  
	mRoot->setPosition(Director::getInstance()->getWinSize() * 0.5f);
	this->addChild(mRoot);
	mBaseDescNode = dynamic_cast<Node* >(mRoot->getChildByName("BaseDesc"));
	initGettingDataDialog();
	initGetDataFiledDialog();
	initLoadingAnimation();
}

void SyncPage::initGettingDataDialog()
{
	mGetDataNode = dynamic_cast<Node* >(mRoot->getChildByName("GetDataDialog"));
	if(!mGetDataNode)
		return;
	ui::Button* pBtn = dynamic_cast<ui::Button* >(mGetDataNode->getChildByName("CancelBtn"));
	if(pBtn)
		pBtn->addClickEventListener(CC_CALLBACK_0(SyncPage::onCancelGetDataCallback, this));
	mGetDataNode->setVisible(false);
}

void SyncPage::onCancelGetDataCallback()
{
	onCanelSaveData();
}


void SyncPage::showGettingDataDialog(bool show)
{
	 mGetDataNode->setVisible(show);
	 if(show)
	 {
		 mContentLabel->setVisible(true);
		 mContentLabel->setString(sgTextHelper(TEXT_EnterGameDesc));
	 }
}

void SyncPage::initGetDataFiledDialog()
{
	mGetDataFailedNode = dynamic_cast<Node* >(mRoot->getChildByName("GetDataFiledDialog"));
	if(!mGetDataFailedNode)
		return;
	Button* pEnterBtn = dynamic_cast<Button* >(mGetDataFailedNode->getChildByName("EnterGameBtn"));
	if(pEnterBtn)
		pEnterBtn->addClickEventListener(CC_CALLBACK_0(SyncPage::onCancelGetDataCallback, this));

	Button* pReGetBtn = dynamic_cast<Button* >(mGetDataFailedNode->getChildByName("ReGetBtn"));
	if(pReGetBtn)
		pReGetBtn->addClickEventListener(CC_CALLBACK_0(SyncPage::onContinueGetDataCallback, this));
	mGetDataFailedNode->setVisible(false);
}

void SyncPage::onContinueGetDataCallback()
{
	doSyncSaveData();
}

void SyncPage::showGetDataFiledDialog(bool show)
{
	mGetDataFailedNode->setVisible(show);
	if(show)
	{
		mContentLabel->setVisible(true);
		mContentLabel->setString(sgTextHelper(TEXT_CancelGetDesc));
		playLoadingAnimation(false);
	}
}



void SyncPage::onEnter()
{
	GameScene::onEnter();
	sgMessageManager()->regisiterMessageHandler(EMT_RQST_RESULT, this);
	doSyncSaveData();
}

void SyncPage::onExit()
{
	GameScene::onExit();
	sgMessageManager()->removeMessageHandler(EMT_RQST_RESULT, this);
}

 

void SyncPage::initLoadingAnimation()
{ 
	mContentLabel = dynamic_cast<ui::Text*>(mBaseDescNode->getChildByName("DialogTips")); 
	if (mContentLabel) {
		mContentLabel->setVisible(false);
	}
	mJuhuaSprite = dynamic_cast<Sprite*>(mBaseDescNode->getChildByName("LoadingSp"));
	mJuhuaSprite->setVisible(false);
}

void SyncPage::playLoadingAnimation(bool play)
{
	if(play)
	{
		mJuhuaSprite->setVisible(true);
		mJuhuaSprite->runAction(RepeatForever::create(CCRotateBy::create(0.3f, 180)));
	}
	else
	{
		mJuhuaSprite->setVisible(false);
		mJuhuaSprite->stopAllActions();
	}
}

