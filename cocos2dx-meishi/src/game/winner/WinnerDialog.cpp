#include "WinnerDialog.h"
#include "game/ui/PageFunction.h"
#include "game/data/Tables.h"
#include "game/data/TableManager.h"
#include "game/ui/dialogs/GenerlDialog.h"
#include "game/ui/MyFont.h"
#include "net/UserObj.h"
#include "game/winner/WinnerControl.h"
#include "SoundManager.h"
#include "WinnerRoad.h"
#include "WinnerHeart.h"
#include "WinnerFight.h"
#include "EnvSet.h"
#include "MessageManager.h"

#define BUTTON_TIP_TAG 100
void WinnerDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onBack")
	{
		this->setShow(false);
		sgPageFunction()->enableOptionButton(true);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
	}
	else if(cName == "onListBtn0")
	{
		if (sgWinnerControl()->checkCanOpen(WINNER_ROAD) == false)
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_WINNER_ACTTIPS));
			return;
		}

		if(mCurrType != WINNER_ROAD)
			setCurrPanel(WINNER_ROAD);
	}
	else if(cName == "onListBtn1")
	{
		if (sgWinnerControl()->checkCanOpen(WINNER_STAR) == false)
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_WINNER_ACTTIPS));
			return;
		}

		if(mCurrType != WINNER_STAR)
			setCurrPanel(WINNER_STAR);
	}
	else if(cName == "onListBtn2")
	{
		if (sgWinnerControl()->checkCanOpen(WINNER_FIGHT) == false)
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_WINNER_ACTTIPS));
			return;
		}

		if(mCurrType != WINNER_FIGHT)
			setCurrPanel(WINNER_FIGHT);
	}
}

bool WinnerDialog::init()
{
	bool ret = false;
	do
	{				
		mRoot = CSLoader::createNode("ui/WinnerDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);

		this->addChild(mRoot);

		auto pRoot = dynamic_cast<Node*>(mRoot->getChildByName("MainFrame"));

		mWinnerView = dynamic_cast<ui::ImageView*>(pRoot->getChildByName("WinnerView"));

		for (int i = 0; i < 3; i++)
		{
			mNormalBtn[i] = dynamic_cast<ui::ImageView*>(pRoot->getChildByName(StringUtils::format("achieve_type_%d",i))); 
			mWinPanel[i] = NULL;
			mBtnPos[i] = mNormalBtn[i]->getPosition();
			mNormalBtnList.push_back(mNormalBtn[i]);
		}

		setPopType(E_POP_TYPE_BIG);
		onLocateClickCallback(mRoot);

		sgWinnerControl()->setBtnRefresh(true);
		ret = true;
	} while (0);

	return ret;
}

void WinnerDialog::onShow()
{
	sgWinnerControl()->setWinnerDialogPtr(this);

	if (sgWinnerControl()->mWinnerActList.size() == 0)
	{
		sgWinnerControl()->getWinnerProxy()->sendWinnerListRqst();
	}
	else
	{
		setWinnerBtn();

		if (sgWinnerControl()->getWinnerActList(WINNER_ROAD + 1))
		{
			sgWinnerControl()->sendWinnerValueRqst();
		}
		updateUI();
	}
	if (sgUserObj()->getWinnerType() != -1 && sgUserObj()->getWinnerType() != mCurrType)
	{		
		setCurrPanel(sgUserObj()->getWinnerType());
		sgUserObj()->setWinnerType(-1);
	}
		
}

void WinnerDialog::onClose()
{
	ActionWindow::onClose();
	sgWinnerControl()->setWinnerDialogPtr(NULL);
}

WinnerDialog::WinnerDialog()
{
	mCurrType = WINNER_ROAD;
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_baoxiangDH.png", "fla_baoxiangDH.plist", "fla_baoxiangDH.xml");
	sgMessageManager()->regisiterMessageHandler(EMT_GOTO_ACTLAB, this);
}

WinnerDialog::~WinnerDialog()
{
	sgMessageManager()->removeMessageHandler(this);
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_baoxiangDH.xml");
}

void WinnerDialog::updateUI(bool isGet)
{
	if (mWinPanel[mCurrType] == NULL) return;

	if (isGet)
	{
		mWinPanel[mCurrType]->update(1.0);
		if (mCurrType == WINNER_ROAD)
		{
			Window::showTextTip(sgTextHelper(TEXT_WINNER_SUCCESS));
#ifndef MGAME_PLATFORM_MPLUS
		}
#else
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_MPLUSHELP_SHARING), CC_CALLBACK_1(WinnerDialog::onShare, this));
	}
		else
		{
			sgMplusControl()->setIsCanShare(true);
		}
#endif
		updateHongDian(mCurrType, false);
		}
	else{
		mWinPanel[mCurrType]->update(0.0);
		WinnerActivityData* winData = sgWinnerControl()->getWinnerActList(mCurrType + 1);
		if (winData == NULL) return;
		updateHongDian(mCurrType, winData->IsCanGet);
	}
}

void WinnerDialog::callBack(cocos2d::Ref*)
{
	sgWinnerControl()->sendGetRqst();
	GenerlDialog::hide();
}


void WinnerDialog::setCurrPanel(int type)
{	
	if(mWinPanel[type] == NULL)
	{
		switch (type)
		{
		case WINNER_ROAD:
			{		
				mWinPanel[type] = WinnerRoad::create();			
				break;
			}
		case WINNER_STAR:
			{
				mWinPanel[type] = WinnerHeart::create();
				break;
			}
		case WINNER_FIGHT:
			{
				mWinPanel[type] = WinnerFight::create();
				break;
			}
		default:
			break;
		}
		mWinnerView->addChild(mWinPanel[type]);
		mWinPanel[type]->setPosition(mWinnerView->getContentSize().width/2,mWinnerView->getContentSize().height/2);
	}
	
	selectLabel(mNormalBtn[mCurrType],false);
	if(mWinPanel[mCurrType] != NULL)
		mWinPanel[mCurrType]->setVisible(false);
	mCurrType = type;
	selectLabel(mNormalBtn[mCurrType],true);
	mWinPanel[type]->setVisible(true);
	mWinPanel[type]->update(0.0);
}

void WinnerDialog::selectLabel(ImageView* label, bool selected)
{
	if (nullptr == label)
		return;

	if (selected)
	{
		label->loadTexture("ui/winner/Achieve_btn_1.png",cocos2d::ui::Widget::TextureResType::PLIST);
		label->setContentSize(Size(174,102));
		auto title = dynamic_cast<Sprite*>(label->getChildByName("Title"));
		if (title)
		{
			title->setSpriteFrame(StringUtils::format("ui/winner/SubTitle_%d_1.png",mCurrType));
		}
	}
	else
	{
		label->loadTexture("ui/winner/Achieve_btn_0.png",cocos2d::ui::Widget::TextureResType::PLIST);
		label->setContentSize(Size(164,100));
		auto title = dynamic_cast<Sprite*>(label->getChildByName("Title"));
		if (title)
		{
			title->setSpriteFrame(StringUtils::format("ui/winner/SubTitle_%d_0.png",mCurrType));
		}
	}
}

void WinnerDialog::setWinnerBtn()
{
	int i = 0;
	for (; i < 3; i++)
	{
		mNormalBtn[i]->setVisible(false);
		mNormalBtn[i]->getChildByName("mWkq")->setVisible(true);
	}

	if (sgWinnerControl()->mWinnerActList.size() == 0) return;

	auto iteor = sgWinnerControl()->mWinnerActList.begin();
	for (;iteor != sgWinnerControl()->mWinnerActList.end(); iteor ++)
	{			
		int id = iteor->second.Act_type - 1;
		mNormalBtn[id]->setVisible(true);
		mNormalBtn[id]->getChildByName("mWkq")->setVisible(false);
		updateHongDian(id,iteor->second.IsCanGet);
	}

	std::sort(std::begin(mNormalBtnList), std::end(mNormalBtnList), [](ui::ImageView* pBtn1, ui::ImageView* pBtn2)
	{
		int priority1 = 0;
		int priority2 = 0;
		auto dataObj = dynamic_cast<cocostudio::ObjectExtensionData*>(pBtn1->getUserObject());
		if (dataObj)
			priority1 = atoi(dataObj->getCustomProperty().c_str());

		dataObj = nullptr;
		dataObj = dynamic_cast<cocostudio::ObjectExtensionData*>(pBtn2->getUserObject());
		if (dataObj)
			priority2 = atoi(dataObj->getCustomProperty().c_str());

		if (pBtn2->isVisible() != pBtn1->isVisible())
		{
			return pBtn1->isVisible();
		}
		else
		{
			return priority1 > priority2;
		}

	});

	i = 0;
	for (auto it = mNormalBtnList.begin(); it != mNormalBtnList.end(); it++,i++)
	{
		(*it)->setVisible(true);
		(*it)->setPosition(mBtnPos[i]);
	}
	

	if (sgWinnerControl()->getBtnRefresh())
	{
		int cType = sgWinnerControl()->mWinnerActList.begin()->second.Act_type - 1;
		setCurrPanel(cType);
		sgWinnerControl()->setBtnRefresh(false);
	}
}

void WinnerDialog::updateHongDian(int type,bool flag)
{
	if (flag)
	{
		if (mNormalBtn[type]->getChildByTag(BUTTON_TIP_TAG) == nullptr)
		{
			cocostudio::Armature* pTipArm = cocostudio::Armature::create("fla_HongDian");
			mNormalBtn[type]->addChild(pTipArm,1,BUTTON_TIP_TAG);
			pTipArm->setPosition(0,mNormalBtn[type]->getContentSize().height);
			pTipArm->getAnimation()->play("play");
		}	
	}
	else
	{
		mNormalBtn[type]->removeChildByTag(BUTTON_TIP_TAG);
	}
}

void WinnerDialog::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_GOTO_ACTLAB)
	{
		MessageGotoActTab* pMsg = dynamic_cast<MessageGotoActTab*>(message);
		if (!pMsg)
			return;
		if (pMsg->getPage() == MSG_FUC_WINNER)
		{
			int actType = pMsg->getLab();
			if (sgWinnerControl()->getWinnerActList(actType) == NULL) return;

			if (actType - 1 == WINNER_ROAD)
			{
				if (mCurrType != WINNER_ROAD)
					setCurrPanel(WINNER_ROAD);
			}
			else if (actType - 1 == WINNER_STAR)
			{
				if (mCurrType != WINNER_STAR)
					setCurrPanel(WINNER_STAR);
			}
			else if (actType - 1 == WINNER_FIGHT)
			{
				if (mCurrType != WINNER_FIGHT)
					setCurrPanel(WINNER_FIGHT);
			}
		}
	}
}

#ifdef MGAME_PLATFORM_MPLUS
void WinnerDialog::onShare(Ref*)
{
	sgChannelApi()->callGameInfo(TYPE_SHARE_MSG_TYPE, "3");
}
#endif
