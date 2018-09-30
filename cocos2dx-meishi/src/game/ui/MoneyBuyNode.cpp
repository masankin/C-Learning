#include "MoneyBuyNode.h"
#include "cocostudio/ActionTimeline/CSLoader.h"

#include "EnvSet.h"
#include "ui/PageFunction.h"
#include "game/net/UserObj.h"
#include "pvp/PvpControl.h"
#include "guide/GuideManager.h"
#include "notice/NoticeControl.h"
#include "ui/dialogs/GenerlDialog.h"
#include "core/SoundManager.h"

MoneyBuyNode::MoneyBuyNode() :
	m_iCurrentNumber(0),
	m_iChangeNumber(0),
	m_iTargetNumber(0),
	m_iNumberSpeed(1),
	m_bIsPlayNumberEffect(false),
	m_bIsPlayAnimation(false)
{
	sgMessageManager()->regisiterMessageHandler(EMT_MONEY_CHANGE, this);
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_ZuanShitx.png", "fla_ZuanShitx.plist", "fla_ZuanShitx.xml");
}

MoneyBuyNode::~MoneyBuyNode()
{
	sgMessageManager()->removeMessageHandler(this);
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_ZuanShitx.xml");
}

MoneyBuyNode* MoneyBuyNode::create()
{
	MoneyBuyNode* pret = new MoneyBuyNode();
	if (pret && pret->init())
	{
		//pret->autorelease();
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void MoneyBuyNode::onReceiveMassage(GameMessage * message)
{
	if(message->getMessageType() == EMT_MONEY_CHANGE)
	{
		MessageMoneyChange* pMes = dynamic_cast<MessageMoneyChange*>(message);
		if (pMes)
		{
			//通过消息來控制是否要滚动数字
			if (pMes->getChangeOperation() == 2)
			{
				if (pMes->getChangeCnt() == 0)
				{
					m_bIsPlayAnimation = true;
					return;
				}

				if (m_bIsPlayNumberEffect)
				{
					mDiamond->setText(__String::createWithFormat("%d", m_iTargetNumber)->getCString());
					unschedule(CC_SCHEDULE_SELECTOR(MoneyBuyNode::numberEffect));

					m_iCurrentNumber = 0;
					m_iChangeNumber = 0;
					m_iTargetNumber = 0;
					m_iNumberSpeed = 1;
					m_bIsPlayNumberEffect = false;
				}

				numberEffect(pMes->getCurrentCnt(), pMes->getChangeCnt());
			}
			else
			{
				if (m_bIsPlayAnimation)
					return;

				int curGold = sgUserObj()->getPersonalInfo()->diamond();// sgUserObj()->getGoldCoin();
				mDiamond->setText(__String::createWithFormat("%d", curGold)->getCString());
			}
		}
	}
}

bool MoneyBuyNode::init()
{
	Node* mRoot = CSLoader::getInstance()->createNode("MoneyBuyNode.csb");
	if (!mRoot)
	{
		CCLOG("[error] ArrowWidget::initUI load csb error!");
		return false;
	}

	mRoot->setAnchorPoint(Vec2(0.5, 0.5));
	
	mDiamond = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_1"));

	ui::Button* mBuy = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_3"));

	mBuy->addClickEventListener(CC_CALLBACK_1(MoneyBuyNode::onMenuClicked, this));

	Size _winSize = Director::getInstance()->getWinSize();

	mRoot->setPosition(ccp(_winSize.width * 0.89,_winSize.height * 0.94));

	this->addChild(mRoot);

//	Director::getInstance()->getNotificationNode()->addChild(this);

	sgMessageManager()->sendMessage(EMT_MONEY_CHANGE,"");

	cocostudio::Armature *mArmature = cocostudio::Armature::create("fla_ZuanShitx");
	mArmature->setScale(0.8);
	mBuy->addChild(mArmature);
	//mArmature->setAnchorPoint(Vec2(0,0));
	//mArmature->setPosition(0,mArmature->boundingBox().size.height);
	mArmature->getAnimation()->play("play");
	return true;
}

void MoneyBuyNode::onMenuClicked(cocos2d::Ref* _ref)
{
	if(sgGuideManager()->getGuideLock()|| sgGuideManager()->getIsRunning())
		return;

	sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);

	//NoticeDialog* pDialog = dynamic_cast<NoticeDialog* >(Window::Create("NoticeDialog.csb"));
	//if(pDialog->isShow())
	//	return;

	ReLoginDialog* pRet = dynamic_cast<ReLoginDialog*>(sgPageFunction()->getWindowByPageName("ReLogin.csb"));
	if (pRet&& pRet->isShow())
		return;

	if (!sgPvpControl()->getIsWaiing())
	{
		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
		sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM,"1");
	}

}

void MoneyBuyNode::numberEffect(int before, int number)
{
	m_iCurrentNumber = before;
	m_iChangeNumber = number;
	m_iTargetNumber = before + number;
	m_iNumberSpeed = ::ceil(number / 90.0f);	//90帧的来算速度
	m_bIsPlayNumberEffect = true;

	schedule(CC_SCHEDULE_SELECTOR(MoneyBuyNode::numberEffect));
}

void MoneyBuyNode::numberEffect(float dt)
{
	if (m_iCurrentNumber < m_iTargetNumber)
	{
		if (m_iTargetNumber - m_iCurrentNumber > 10)
		{
			m_iCurrentNumber += m_iNumberSpeed;
		}
		else
		{
			m_iCurrentNumber += 1;
		}
		mDiamond->setText(__String::createWithFormat("%d", m_iCurrentNumber)->getCString());
	}
	else
	{
		m_iCurrentNumber = 0;
		m_iChangeNumber = 0;
		m_iTargetNumber = 0;
		m_iNumberSpeed = 1;
		m_bIsPlayNumberEffect = false;
		m_bIsPlayAnimation = false;
		unschedule(CC_SCHEDULE_SELECTOR(MoneyBuyNode::numberEffect));

		mDiamond->setText(__String::createWithFormat("%d", sgUserObj()->getPersonalInfo()->diamond())->getCString()); //sgUserObj()->getGoldCoin())->getCString());
	}
}
