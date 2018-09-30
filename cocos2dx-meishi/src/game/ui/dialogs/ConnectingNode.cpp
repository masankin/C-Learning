#include "ConnectingNode.h"
#include "game/ui//PageFunction.h"
#include "game/ui//system/Window.h"

#include "net/NetManager.h"

#define CONNECT_SHOW_TIME (1.1f)
bool ConnectingNode::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/ConnectingNode.csb");
		CC_BREAK_IF(!mRoot);
		mRoot->setAnchorPoint(Vec2(0.5, 0.5)); 
		//onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		mContentLabel = dynamic_cast<ui::Text*>(mRoot->getChildByName("ConnectingText"));

		if (mContentLabel) {
			mContentLabel->setVisible(false);
		}
		mJuhuaSprite = dynamic_cast<Sprite*>(mRoot->getChildByName("juhua"));
		mJuhuaSprite->setVisible(false);

		this->scheduleUpdate();
		///schedule(schedule_selector(ConnectingNode::checkConnecting), 0.1);

		ret = true;
	} while (0);


	return ret;
}

ConnectingNode::ConnectingNode() : mContentLabel(0), mCounter(0.0f), mCurrTaskId(-1), mShowJuhua(true)
{
	mJuhuaSprite = nullptr;
	mContentLabel = nullptr;
	mIsCanClean = false;
	//	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_tuichu.png", "fla_tuichu.plist", "fla_tuichu.xml");
}

ConnectingNode::~ConnectingNode()
{
	//	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_tuichu.xml");
}

void ConnectingNode::update(float delta)
{
	mCounter = mCounter + delta;
	// 超时
	if (mCounter > sgNetManager()->getConnectingEvent()->getMaxWaitingTime())
	{
		hide();
		if (sgPageFunction()->getCurrentUIType() != MSG_FUC_LOGIN)
		{
			ReLoginDialog::show();
		}
		return;
	}
	// 检查是否已经回包
	if (sgNetManager()->getConnectingEvent()->getIsAccepted()){
		hide();
		return;
	}

	// 断网
	//if (!sgNetManager()->IsConnect()){
	//	hide();
	//	return;
	//}

	if (mCounter > sgNetManager()->getConnectingEvent()->getDelayTimes() && mJuhuaSprite && mShowJuhua) {
		mShowJuhua = false;
		mJuhuaSprite->setVisible(true);
		mJuhuaSprite->runAction(RepeatForever::create(CCRotateBy::create(0.3f, 180)));
		setUnlucency();
	}
}

void ConnectingNode::hide()
{
	bool isBack = sgNetManager()->getConnectingEvent()->getIsAccepted();
	// 菊花没出现之前已经回包（delayTime = 0.6）,连接界面则回包就消失。菊花只要出现，至少转一圈。
	if ((isBack && mCounter < sgNetManager()->getConnectingEvent()->getDelayTimes()) || (mCounter > CONNECT_SHOW_TIME)) {
		this->setShow(false);
		mShowJuhua = true;
		mJuhuaSprite->setVisible(false);
		mCounter = 0.0f;
		sgNetManager()->getConnectingEvent()->resetEvent();
	}
}

void ConnectingNode::setShow(bool b)
{
	Window::setShow(b);
	if (b) {
		this->scheduleUpdate();
	}
	setlucency();
	mCounter = 0.0f;
}

void ConnectingNode::setlucency()
{
	if (pWindowLayout) {
		pWindowLayout->setCascadeOpacityEnabled(true);
		pWindowLayout->setOpacity(0);
	}
}

void ConnectingNode::setUnlucency()
{
	if (pWindowLayout) {
		pWindowLayout->setCascadeOpacityEnabled(true);
		pWindowLayout->setOpacity(255);
	}
}
