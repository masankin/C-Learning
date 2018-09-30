#include "PvpDialog.h"
#include "game/ui//PageFunction.h"
#include "game/ui//system/Window.h"
#include "game/data/Tables.h"
#include "game/data/TableManager.h"
#include "net/UserObj.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"
#include "MessageManager.h"
#include "game/winner/WinnerControl.h"
#include "prop/PropsControl.h"

#include "net/UserObj.h"
#include "net/NetManager.h"
#include "EnvSet.h"
#include "guide/GuideManager.h"
#include "SoundManager.h"
#include "sync/SyncControl.h"
#include "game/daytask/DayTaskControl.h"
#include "game/pvp/PvpRoomControl.h"
#include "game/shop/HonorShopControl.h"
#include "EnvSet.h"
#include "ui/dialogs/UpRankSessionDialog.h"

PvpDialog::~PvpDialog()
{
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_pvpjindutiao.xml");
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_pvpPipei.xml");

	sgMessageManager()->removeMessageHandler(this);
}

#define  PVP_NoItem_1 (1)
#define  PVP_NoItem_2 (2)
PvpDialog::PvpDialog() : showTip(true), mCurMode(2), mPower(0), isRanding(false), mChooseScore(PVP_NoItem_2), mItemText(nullptr), mPrevSession(0), mCurSession(0)
{
	sgMessageManager()->regisiterMessageHandler(EMT_ENERGY_CHANGE,this);
	sgMessageManager()->regisiterMessageHandler(EMT_POINT_CHANGE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_RANK_CHANGE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CHANGE_PLAYER_ICON, this);
	sgMessageManager()->regisiterMessageHandler(EMT_STOP_MATCH, this);
	sgMessageManager()->regisiterMessageHandler(EMT_DALIY_MISSION_TIPS, this);
	sgMessageManager()->regisiterMessageHandler(EMT_SHOW_DAILY_MISSION_PROGRESS, this);
	//sgMessageManager()->regisiterMessageHandler(EMT_CLOSE_PVP, this);
	//sgMessageManager()->regisiterMessageHandler(EMT_CHANGE_NAME, this);
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_pvpjindutiao.png", "fla_pvpjindutiao.plist", "fla_pvpjindutiao.xml");
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_pvpPipei.png", "fla_pvpPipei.plist", "fla_pvpPipei.xml");
}

void PvpDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{ 
	if(cName == "onCancelRand")
	{
		if (isRanding == 0) return;
	
		isRanding = 2;
		sgPvpControl()->getPvPProxy()->sendStopMatch(mCurMode);
	}
		
		
	if (cName == "onBack")
	{
		sgSoundManager()->playMusicBg(MUSIC_UI_MAIN);
	}

	Node* pNode = dynamic_cast<Node*>(sender);
	if (cName == "onRand" || cName == "onRoom" || cName == "onItem")
	{
		sgGuideManager()->unRegistGuideMessage();

		if (!sgLoginProxy()->checkLogin(true))
		{
			return;
		}

		if (!sgNetManager()->IsConnect())
		{
			GenerlDialog::show("", sgTextHelper(TEXT_PVP_NO_CONN));
			return;	
		}
		
	}
	if (cName == "onRand")
	{
		//if (sgUserObj()->getPvpPower() < PVP_ENERGY_VALUE)
		if (sgUserObj()->getPersonalInfo()->energy() < PVP_ENERGY_VALUE)
		{
			PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());// sgUserObj()->getEnergyBuyCount());
			if (pPowerBuy)
			{
				GenerlDialog::show(sgTextHelper(TEXT_BUY_ITEM__JINGLI),
					__String::createWithFormat(sgTextHelper(TEXT_BUY_ITEM_JINGLI_SURE).c_str(),pPowerBuy->getEnergyNum())->getCString(),
					CC_CALLBACK_1(PvpDialog::onClickBuyEngerCallBack, this));
			}
			return;
		}
		sgPvpControl()->setPVPCMode(PVP_MODE_RAND);
		sgPageFunction()->gotoFunction(MSG_FUC_PVP_MATCH_LOADING);

		refreshBtn(PVP_MODE_RAND);
		mCurMode = 0;
		isRanding = 1;
		sgPvpControl()->getPvPProxy()->sendMatchRqst(mCurMode);
		this->scheduleOnce(schedule_selector(PvpDialog::updateMacth),20.0f);
	}
	else if (cName == "onRoom")
	{
		
		//if (sgUserObj()->getPvpPower() < PVP_NEED_PSYCH)
		if (sgUserObj()->getPersonalInfo()->energy() < PVP_NEED_PSYCH)
		{
			//先检查体力
			PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());// sgUserObj()->getEnergyBuyCount());
			if (pPowerBuy)
			{
				GenerlDialog::show(sgTextHelper(TEXT_BUY_ITEM__JINGLI),
					__String::createWithFormat(sgTextHelper(TEXT_BUY_ITEM_JINGLI_SURE).c_str(), pPowerBuy->getEnergyNum())->getCString(),
					CC_CALLBACK_1(PvpDialog::onClickBuyEngerCallBack, this));
			}
			return;
		}

		//优先检查开房券，再检查钻石
		int roomTicket = sgUserObj()->getPersonalInfo()->roomticket();//sgUserObj()->getRoomTicketCount();
		if (roomTicket > 0)
		{
			// chnage by jim, unify tips ui
			//GeneralDialogOther::show(sgTextHelper(TEXT_PVP_ROOM_7), GeneralDialogOther::E_GENERAL_TYPE_CONFIRM, [](Ref* sendr){
			//	GeneralDialogOther::hide();
			//	sgPvpRoomControl()->sendCreateRoomRqst();
			//});
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_PVP_ROOM_7), [](Ref* sendr){
				GenerlDialog::hide();
				sgPvpRoomControl()->sendCreateRoomRqst();
			}, false);
			return;
		}

		//if (sgUserObj()->getGoldCoin() < PVP_NEED_GOLDCOIN)
		if (sgUserObj()->getPersonalInfo()->diamond() < PVP_NEED_GOLDCOIN)
		{
			// change by jim, unify tips ui
			//GeneralDialogOther::show(sgTextHelper(TEXT_BUY_ITEM_NO_EN), GeneralDialogOther::E_GENERAL_TYPE_CONFIRM, [](Ref* sendr){
			//	GeneralDialogOther::hide();
			//	PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
			//});
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_BUY_ITEM_NO_EN), [](Ref* sendr){
				GenerlDialog::hide();
				PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
			},false);
			return;
		}
		else
		{	
			// change by jim, unify tips ui
			//GeneralDialogOther::show(sgTextHelper(TEXT_PVP_ROOM_7), GeneralDialogOther::E_GENERAL_TYPE_CONFIRM, [](Ref* sendr){
			//	GeneralDialogOther::hide();
			//	sgPvpRoomControl()->sendCreateRoomRqst();
			//});
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_PVP_ROOM_7), [](Ref* sendr){
				GenerlDialog::hide();
				sgPvpRoomControl()->sendCreateRoomRqst();
			}, false);
		}

	

	}else if (cName == "onBack")
	{
		showTip = true;
		this->setShow(false);
		sgPageFunction()->setCurrentUIType(MSG_FUC_MAIN_LEVEL); 
		sgMessageManager()->sendMessage(EMT_CLOSE_PVP, "");
		sgMessageManager()->sendMessage(EMT_CLOSEPAGE, "");
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");

	}else if (pNode->getParent()->getName() == "ProjectNode_3")
	{
		PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());// sgUserObj()->getEnergyBuyCount());
		if (pPowerBuy)
		{
			GenerlDialog::show(sgTextHelper(TEXT_BUY_ITEM__JINGLI),
				__String::createWithFormat(sgTextHelper(TEXT_BUY_ITEM_JINGLI_SURE).c_str(),pPowerBuy->getEnergyNum())->getCString(),
				CC_CALLBACK_1(PvpDialog::onClickBuyEngerCallBack, this));
		}		
		
	}else if (cName == "onStore")
	{
		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
	}
	else if(cName == "onRank")
	{
		sgPageFunction()->setCurrentUIType(MSG_FUC_MAIN_LEVEL);
		PageFunction::Get()->gotoFunction(MSG_FUC_RANKING);
		this->setShow(false);
		sgMessageManager()->sendMessage(EMT_CLOSE_PVP, "");
	}
	else if (cName == "onItem")
	{
		if (sgEnvSet("MGAME_OPEN_PVP_LIMIT") == 0)
		{
			if (!sgPvpControl()->getIsNoItemOpen())
			{
				GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_PVP_NOITEM_TIME).c_str());
				return;
			}
			if (!sgUserObj()->getIsGoneNewerProtect())
			{
				GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), StringUtils::format(sgTextHelper(TEXT_PVP_LIMIT_ROUND10).c_str(), sgEnvSet("NEWER_PROTECT_LEVEL")));
				return;
			}

			if (sgUserObj()->getPersonalInfo()->energy() < PVP_ENERGY_VALUE)
			{
				PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());// sgUserObj()->getEnergyBuyCount());
				if (pPowerBuy)
				{
					GenerlDialog::show(sgTextHelper(TEXT_BUY_ITEM__JINGLI),
						__String::createWithFormat(sgTextHelper(TEXT_BUY_ITEM_JINGLI_SURE).c_str(), pPowerBuy->getEnergyNum())->getCString(),
						CC_CALLBACK_1(PvpDialog::onClickBuyEngerCallBack, this));
				}
				return;
			}

			if (sgUserObj()->getPersonalInfo()->honorcount() < PVP_SCORE_VALUE)
			{
				this->showTextTip(sgTextHelper(TEXT_PVP_SCORE));
				return;
			}
		}
		
		sgPvpControl()->setPVPCMode(PVP_MODE_NOITEM);
		sgPageFunction()->gotoFunction(MSG_FUC_PVP_MATCH_LOADING);
		refreshBtn(PVP_MODE_NOITEM);
		mCurMode = 0;
		isRanding = 1;
		sgPvpControl()->getPvPProxy()->sendMatchRqst(mCurMode,PVP_MODE_NOITEM,mChooseScore);
		this->scheduleOnce(schedule_selector(PvpDialog::updateMacth),120.0f);
		if (mItemText)
			mItemText->setVisible(false);

	}
	else if(cName == "onItemScore")
	{
		return;
		std::string str = mItemScore->getString();
		mItemScore->setText(mCurScore->getString());
		mCurScore->setString(str);
		if (mChooseScore == PVP_NoItem_2)
		{
			mChooseScore = PVP_NoItem_1;					
			mBtnChooseScore->setPositionX(104.85);
			mItemScore->setPositionX(215.00);		
		}
		else
		{
			mChooseScore = PVP_NoItem_2;
			mBtnChooseScore->setPositionX(237.85);
			mItemScore->setPositionX(114.52);
		}
		//mCurScore->setText(StringUtils::format("%sx",sgTextHelper(13008).c_str(),mChooseScore));
	}
	else if(cName == "onItemInfo")
	{
		PvpInfoDialog* pRet = dynamic_cast<PvpInfoDialog*>(sgPageFunction()->getWindowByPageName("PvpInfoDialog.csb"));
		if (!pRet)
		{
			sgPageFunction()->gotoFunction(MSG_FUC_PVP_HELP, "PvpInfoDialog.csb");
		}
		
	}
	else if (cName == "onCharge")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_HONOREXCHANGE);
		//sgHonorShopControl()->sendGetExchangeInfoReq();
	}
}

void PvpDialog::onClickBuyEngerCallBack(cocos2d::Ref* ref)
{
	sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);

	//if(sgUserObj()->getPvpPower() >= POWER_BUY_MAX)
	if (sgUserObj()->getPersonalInfo()->energy() > POWER_BUY_MAX)
	{
		GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS),
		__String::createWithFormat("%s%s",sgTextHelper(TEXT_ACT_JINGLI).c_str(),sgTextHelper(TEXT_BUY_POWER_MAX).c_str())->getCString());
		return;
	}
	PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());// sgUserObj()->getEnergyBuyCount());
	if (pPowerBuy)
	{	
		//if (sgUserObj()->getGoldCoin() >= pPowerBuy->getEnergyNum())
		if (sgUserObj()->getPersonalInfo()->diamond() >= pPowerBuy->getEnergyNum())
		{ 
			sgPropsControl()->sendBuyPropRqst(PVP_ENERGY_TYPE,pPowerBuy->getEnergyNum(),1);
		}
		else
		{
			PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
		}
		GenerlDialog::hide();
	}
}

bool PvpDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PVPDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		mPower = dynamic_cast<ui::Text*>(mRoot->getChildByName("ProjectNode_3")->getChildByName("Text_1"));

		mPowerTip = dynamic_cast<ui::Text*>(mRoot->getChildByName("ProjectNode_3")->getChildByName("Sprite_3")->getChildByName("Text_2"));
		mPowerTip->getParent()->setVisible(false);

		mBtnRand = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnRand"));
		mBtnRoom = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnRoom"));
		mBtnItem = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnBull"));
		mBtnRand->setPressedActionEnabled(false);
		mBtnRoom->setPressedActionEnabled(false);
		mBtnItem->setPressedActionEnabled(false);
		mBtnItem->setTouchEnabled(true);

		auto nodeRandAni = mBtnRand->getChildByName("NodeAni");
		auto ArmRandAdditive = cocostudio::CCArmature::create("fla_pvppipei1_ADDITIVE");
		ArmRandAdditive->setBlendFunc(BlendFunc::COMADDITIVE);
		ArmRandAdditive->getAnimation()->play("play");
		nodeRandAni->addChild(ArmRandAdditive);
		

		auto nodeRoomAni = mBtnRoom->getChildByName("NodeAni");
		auto ArmRoomAdditive = cocostudio::CCArmature::create("fla_pvppipei_ADDITIVE");
		ArmRoomAdditive->setBlendFunc(BlendFunc::COMADDITIVE);
		ArmRoomAdditive->getAnimation()->play("play");
		nodeRoomAni->addChild(ArmRoomAdditive);

		
		mBtnRand->setLocalZOrder(2);

		mPanel_1 = dynamic_cast<ui::Layout*>(mRoot->getChildByName("Panel_1"));
		mPanel_1->setLocalZOrder(1);
		mSprGroup = dynamic_cast<Sprite*>(mRoot->getChildByName("Sprite_35"));

		for (int i = 1; i <= 6; i++)
		{
			mPlayerTitel[i-1] = dynamic_cast<Sprite*>(mRoot->getChildByName(StringUtils::format("Sprite_%d",i)));
		}

		mText_Tips = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_Tips"));	
		
		for (int i = 1; i <= 4; i++)
		{
			mRankInfo[i-1] = dynamic_cast<ui::Text*>(mRoot->getChildByName(StringUtils::format("Rank_Info_%d",i)));
		}

		sgMessageManager()->sendMessage(EMT_CHANGE_PLAYER_ICON, sgUserObj()->getPersonalInfo()->iconid());//sgUserObj()->getPlayerIconID())->getCString());

		Node* pnode = mBtnItem->getChildByName("Sprite_Null");
		//mBtnChooseScore = dynamic_cast<ui::Button*>(pnode->getChildByName("Button_2"));
		mItemScore = dynamic_cast<ui::Text*>(pnode->getChildByName("Text_4"));
		//mCurScore = dynamic_cast<ui::Text*>(mBtnChooseScore->getChildByName("Text_3"));

		mCard_Num = dynamic_cast<ui::Text*>(mBtnRoom->getChildByName("Card_Num"));


		auto pRandText = dynamic_cast<ui::Text*>(mBtnRand->getChildByName("mText"));
		pRandText->setText(sgTextHelper(TEXT_PVP_BTN_TEXT_1));
		auto pRoomText = dynamic_cast<ui::Text*>(mBtnRoom->getChildByName("mText"));
		pRoomText->setText(sgTextHelper(TEXT_PVP_BTN_TEXT_2));
		mItemText = dynamic_cast<ui::Text*>(mBtnItem->getChildByName("mText"));
		mItemText->setText(StringUtils::format(sgTextHelper(TEXT_PVP_BTN_TEXT_3).c_str(), "\n"));


		ret = true;
		
	} while (0);


	return ret;
}

void PvpDialog::onEnter()
{
	Window::onEnter();

	

	//刷新精力值
	sgMessageManager()->sendMessage(EMT_ENERGY_CHANGE, "");
	sgUserObj()->saveToFile();
}

void PvpDialog::onExit()
{
	Window::onExit();
	
}

void PvpDialog::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_ENERGY_CHANGE)
	{
		if (mPower)
			mPower->setText(StringUtils::format("%d", sgUserObj()->getPersonalInfo()->energy()));// sgUserObj()->getPvpPower()));

	}
	else if (message->getMessageType() == EMT_POINT_CHANGE)
	{
		if (mRankInfo[1])
		{
			int roundNum = sgEnvSet("NEWER_PROTECT_LEVEL") - sgUserObj()->getPersonalInfo()->pvpround();//sgUserObj()->getRoundCount();
			if (!sgUserObj()->getIsGoneNewerProtect())
			{
				mRankInfo[1]->setText(__String::createWithFormat(sgTextHelper(TEXT_SCORE_NEED).c_str(), roundNum)->getCString());
			}
			else
			{
				mRankInfo[1]->setText(__String::createWithFormat("%d", sgUserObj()->getPersonalInfo()->score())->getCString());//sgUserObj()->getPvpPoint())->getCString());
			}
		}

		auto pGradeData = sgTableManager()->getGradeData(sgUserObj()->getPersonalInfo()->score());
		if (pGradeData)
		{
			auto emblemFile = StringUtils::format("ui/Icon2/ranking_Emblem-%02d.png", pGradeData->getID());
			mSprGroup->setSpriteFrame(emblemFile);

			int rankGroup = pGradeData->getGroup();
			if (sgUserObj()->getPersonalInfo()->rank() > 0 && rankGroup > 0)
			{
				int group = TEXT_RANK_GROUP_1 - 1 + rankGroup;
				mRankInfo[0]->setText(__String::createWithFormat(sgTextHelper(group).c_str(), sgUserObj()->getPersonalInfo()->rank())->getCString());
			}
			else
			{
				mRankInfo[0]->setText(sgTextHelper(TEXT_PVP_NO_RANK));
			}
		}
		else
		{
			mRankInfo[0]->setText(sgTextHelper(TEXT_PVP_NO_RANK));
		}
	}
	else if (message->getMessageType() == EMT_STOP_MATCH)
	{
		MessagePvpClose* pMes = dynamic_cast<MessagePvpClose*>(message);
		switch (pMes->mResult)
		{
		case 0:
		{
			if (isRanding == 0) return;
			this->unschedule(schedule_selector(PvpDialog::updateMacth));
			resetUI();
			isRanding = 0;
			sgPvpControl()->setIsWaiing(false);
			sgPvpControl()->clean();
			// 隐藏匹配界面
			PVPMatchLoadingDialog*  matchLoading = dynamic_cast<PVPMatchLoadingDialog*>(sgPageFunction()->getWindowByPageName(
						"PVPMatchLoadingDialog.csb"));
			if (matchLoading){
				matchLoading->setShow(false);
				sgPageFunction()->setDiamondPanel(true);
			}
			break;
		}
		case 1:
		{
			if (this->isShow())
			{
				mBtnItem->getChildByName("PVP_Item_Time_4")->setVisible(false);
				mItemText->setVisible(true);
			}
			break;
		}
		case 2:
		{
			if (this->isShow())
			{
				mBtnItem->getChildByName("PVP_Item_Time_4")->setVisible(true);
				mItemText->setVisible(false);
				if (isRanding == 1 || isRanding == 2)
				{
					this->unschedule(schedule_selector(PvpDialog::updateMacth));
					resetUI();
					isRanding = 0;
					sgPvpControl()->setIsWaiing(false);
					sgPvpControl()->clean();
					GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_PVP_NOITEM_TIME).c_str());
					sgPvpControl()->getPvPProxy()->sendStopMatch(mCurMode);
				}
			}
			break;
		}
		default:
			break;
		}
	}
	else if (EMT_CLOSE_PVP == message->getMessageType())
	{
		this->setShow(false);
		sgPageFunction()->setCurrentUIType(MSG_FUC_MAIN_LEVEL);
	}
	else if (message->getMessageType() == EMT_CHANGE_PLAYER_ICON)
	{
		MessageChangePlayerIcon* pMes = dynamic_cast<MessageChangePlayerIcon*>(message);
#ifdef MGAME_PLATFORM_MPLUS
		FriendSystem::setPlayerIconSpr(dynamic_cast<Sprite*>(mRoot->getChildByName("Player_Icon")), pMes->getIconId(), sgMplusControl()->getThirdPartyIcon());
#else
		FriendSystem::setPlayerIconSpr(dynamic_cast<Sprite*>(mRoot->getChildByName("Player_Icon")),
			pMes->getIconId());
#endif
	}
	else if (EMT_DALIY_MISSION_TIPS == message->getMessageType())
	{
		MessageDailyMissionTips* pMes = dynamic_cast<MessageDailyMissionTips*>(message);
		if (pMes->getTipType() == 2)
			playDailyMissionTip(mRoot, pMes->getIsShow(), Point(pMes->getX(), pMes->getY()));
	}
	else if (EMT_SHOW_DAILY_MISSION_PROGRESS == message->getMessageType())
	{
		if (sgUserObj()->getIsShowDailyMissionProgress())
		{
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eOpenMissionCompleteDialog", NULL);
			sgUserObj()->setIsShowDailyMissionProgress(false);
		}
	}
	else if (EMT_RANK_CHANGE == message->getMessageType())
	{
		auto pGradeData = sgTableManager()->getGradeData(sgUserObj()->getPersonalInfo()->score());
		if (pGradeData)
		{
			auto emblemFile = StringUtils::format("ui/Icon2/ranking_Emblem-%02d.png", pGradeData->getID());
			mSprGroup->setSpriteFrame(emblemFile);

			int rankGroup = pGradeData->getGroup();
			if (sgUserObj()->getPersonalInfo()->rank() > 0 && rankGroup > 0)
			{
				int group = TEXT_RANK_GROUP_1 - 1 + rankGroup;
				mRankInfo[0]->setText(__String::createWithFormat(sgTextHelper(group).c_str(), sgUserObj()->getPersonalInfo()->rank())->getCString());
			}
			else
			{
				mRankInfo[0]->setText(sgTextHelper(TEXT_PVP_NO_RANK));
			}
		}
		else
		{
			mRankInfo[0]->setText(sgTextHelper(TEXT_PVP_NO_RANK));
		}

	}
}

void PvpDialog::powerTip()
{
	if(!mPowerTip) return;
	Node* pParent = mPowerTip->getParent();
	if (!showTip)
	{
		pParent->setVisible(false);
		return;
	}

	showTip = false;
	pParent->setVisible(true);
	pParent->setOpacity(255.0);

	int curPower = sgUserObj()->getPersonalInfo()->energy();// sgUserObj()->getPvpPower();

	//if (curPower < sgUserObj()->getPvpPowerLimit())
	if (curPower < sgUserObj()->getPersonalInfo()->energyslot())
	{
		mPowerTip->setText(sgTextHelper(TEXT_ENERGY_NOT_FULL));
	}
	else
	{
		mPowerTip->setText(sgTextHelper(TEXT_ENERGY_FULL_TIPS));
	}

	pParent->runAction(Sequence::create(
		DelayTime::create(3.0),
		FadeOut::create(2.0),NULL));
}

void PvpDialog::onShow()
{
	Window::onShow();

	powerTip();

	resetUI();

	updateUI();

	mRoot->addChild(sgChattingControl()->getChattingButton(false));

	Size pSize = Director::getInstance()->getWinSize();
	auto pFullSrvInviteBatTips = sgChattingControl()->getFullSrvInviteBatTips();
	pFullSrvInviteBatTips->setPosition(pSize.width - 120, pSize.height - 145);
	mRoot->addChild(pFullSrvInviteBatTips);

	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("gDoneScene");

	int gateID = sgUserObj()->getMaxOpenGateID();
	int level = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel();
	Size winSize = Director::getInstance()->getWinSize();
	sgGuideManager()->registGuideMessage(gateID, level, E_GUIDELOC_PVPUI, mRoot, this, false, -0.5f * winSize);


	//add by crystal -- dailyMission判断
	auto dailyMissionGuid2Point = sgUserObj()->getDailyMissionTipPointByType(1);
	if (dailyMissionGuid2Point.x > 0.0)
		sgMessageManager()->sendMessage(EMT_DALIY_MISSION_TIPS, StringUtils::format("%d %d %f %f", 2, 1, dailyMissionGuid2Point.x, dailyMissionGuid2Point.y));
	else
		sgMessageManager()->sendMessage(EMT_DALIY_MISSION_TIPS, StringUtils::format("%d %d %f %f", 2, 0, 0.0, 0.0));


	//展示升级效果
	do
	{
		//---------------------------新兵升学徒
		int lastPvpCount = sgPvpControl()->getLastPvpCount();
		int nowPvpCount = sgUserObj()->getPersonalInfo()->pvpround();
		if (lastPvpCount != sgEnvSet("NEWER_PROTECT_LEVEL") && nowPvpCount == sgEnvSet("NEWER_PROTECT_LEVEL"))
		{
			if (lastPvpCount != 0)
			{
				sgPvpControl()->setLastPvpCount(nowPvpCount);

				mPrevSession = 0;
				mCurSession = 1;
				this->scheduleOnce(schedule_selector(PvpDialog::showUpSession), 0.1f);
				sgPvpControl()->getEndRsp()->Clear();
			}
			break;
		}
		else if (nowPvpCount == sgEnvSet("NEWER_PROTECT_LEVEL"))
		{
			sgPvpControl()->getEndRsp()->Clear();
			break;
		}
		//---------------------------其他情况
		int idx = sgPvpControl()->getLastOwnerIdx();

		fgame::CSPvpEndResp* packEnd = sgPvpControl()->getEndRsp();

		if (idx < 0 || idx >= packEnd->result_size())
		{
			break;
		}

		fgame::CSPlayerPvpResult playInfo = packEnd->result(idx);

		int uin = playInfo.uin();

		if (uin == sgUserObj()->getUin())
		{
			//排行榜段位升级
			if (playInfo.addpoint() > 0)
			{
				const GradeData* pPrevGradeData = sgTableManager()->getGradeData(playInfo.prevpoint());
				const GradeData* pCurGradeData = sgTableManager()->getGradeData(playInfo.prevpoint() + playInfo.addpoint());
				if (pCurGradeData)
				{
					int iPrevGrade = 0;
					int iCurGrade = pCurGradeData->getID();
					if (pPrevGradeData)
					{
						iPrevGrade = pPrevGradeData->getID();
					}
					mPrevSession = iPrevGrade;
					mCurSession = iCurGrade;
					if (iCurGrade > iPrevGrade)
					{
						this->scheduleOnce(schedule_selector(PvpDialog::showUpSession), 0.1f);
					}
					sgPvpControl()->getEndRsp()->Clear();
				}
			}
		}
		break;
	} while (true);


}

void PvpDialog::showUpSession(float dt)
{
	UpRankSessionDialog::showUpRankSessionDialog(mPrevSession, mCurSession);
}

void PvpDialog::onClose()
{
	Window::onClose();
	sgPvpControl()->setIsWaiing(false);
	sgMessageManager()->sendMessage(EMT_INVITE_BAT_SWITCH, "");
	isRanding = 0;
}

#define PVP_MACTH_LOADING (10)
void PvpDialog::refreshBtn(int mode)
{
	switch (mode)
	{
	case 1:
		{
			//mBtnRand->setBrightStyle(BrightStyle::HIGHLIGHT);
			//mBtnRoom->setBright(false);
			/* 取消按钮
			Node* pNode= mBtnRand->getChildByName("Node_1");
			pNode->setVisible(true);
			if (pNode->getChildByTag(PVP_MACTH_LOADING) == nullptr)
			{
				cocostudio::Armature*  mArmature = cocostudio::Armature::create("fla_pvpjindutiao");
				pNode->addChild(mArmature,1,PVP_MACTH_LOADING);
				mArmature->getAnimation()->play("play");
			}
			*/
			//mPanel_1->setVisible(true);
			//mPanel_1->setLocalZOrder(3);
			//mBtnItem->setLocalZOrder(2);
			//mBtnRand->setLocalZOrder(4);
		}
		break;
	case 3:
		{
			mBtnItem->setBrightStyle(BrightStyle::HIGHLIGHT);
			//mBtnRoom->setBright(false);
			Node* pNode= mBtnItem->getChildByName("Node_1");
			pNode->setVisible(true);
			if (pNode->getChildByTag(PVP_MACTH_LOADING) == nullptr)
			{
				cocostudio::Armature*  mArmature = cocostudio::Armature::create("fla_pvpjindutiao");
				pNode->addChild(mArmature,1,PVP_MACTH_LOADING);
				mArmature->getAnimation()->play("play");
			}
			mPanel_1->setVisible(true);
			mPanel_1->setLocalZOrder(3);
			mBtnItem->setLocalZOrder(4);
			mBtnRand->setLocalZOrder(2);
			mBtnItem->getChildByName("PVP_Item_Time_4")->setVisible(!sgPvpControl()->getIsNoItemOpen());
			mItemText->setVisible(sgPvpControl()->getIsNoItemOpen());
		}
		break;
	default:
		break;
	}
}

void PvpDialog::resetUI()
{
	//还原按钮状态
	//mBtnRand->setBrightStyle(BrightStyle::NORMAL);
	mBtnRand->setBright(true);
	mBtnRand->getChildByName("Node_1")->setVisible(false);
	mBtnRoom->setBright(true);
	mPanel_1->setVisible(false);
	mBtnItem->getChildByName("Node_1")->setVisible(false);
	mBtnItem->setBright(true);
}

void PvpDialog::updateMacth(float dt)
{
	//CCLOG("[trace updateMatch]  updateMatch %f", dt);
	sgMessageManager()->sendMessage(EMT_STOP_MATCH, "0");
}

void PvpDialog::closeMatchIngArm(float dt)
{
	PVPMatchLoadingDialog*  matchLoading = dynamic_cast<PVPMatchLoadingDialog*>(sgPageFunction()->getWindowByPageName(
			"PVPMatchLoadingDialog.csb"));
	if (matchLoading){
		matchLoading->setShow(false);
	}
}

void PvpDialog::updateUI()
{
	/* //屏蔽勋章显示
	std::vector<int> badges = sgUserObj()->getBadges();
	std::vector<int>::iterator it = badges.begin();
	for (; it != badges.end(); ++it)
	{

		auto pData = sgTableManager()->getBadgeData(*it);
		if (pData && pData->getActive() != 0)
		{
			//spriteTitle->setSpriteFrame(StringUtils::format("ui/general/PlayerTitle_%d.png", info->mTitle));
			mPlayerTitel[*it - 1]->setSpriteFrame(GetPlayerBadgeIconNameById(pData->getId(), true));
			mPlayerTitel[*it - 1]->setVisible(true);
			mPlayerTitel[*it - 1]->setScale(0.6f);
		}
		else
		{
			mPlayerTitel[*it - 1]->setVisible(false);
		}
	}*/	
	
	auto pGradeData = sgTableManager()->getGradeData(sgUserObj()->getPersonalInfo()->score());
	if (pGradeData)
	{
		auto emblemFile = StringUtils::format("ui/Icon2/ranking_Emblem-%02d.png", pGradeData->getID());
		mSprGroup->setSpriteFrame(emblemFile);

		int rankGroup = pGradeData->getGroup();
		if (sgUserObj()->getPersonalInfo()->rank() > 0 && rankGroup > 0)
		{
			int group = TEXT_RANK_GROUP_1 - 1 + rankGroup;
			mRankInfo[0]->setText(__String::createWithFormat(sgTextHelper(group).c_str(), sgUserObj()->getPersonalInfo()->rank())->getCString());
		}
		else
		{
			mRankInfo[0]->setText(sgTextHelper(TEXT_PVP_NO_RANK));
		}
	}
	else
	{
		mRankInfo[0]->setText(sgTextHelper(TEXT_PVP_NO_RANK));
	}

	int roundNum = sgEnvSet("NEWER_PROTECT_LEVEL") - sgUserObj()->getPersonalInfo()->pvpround();//sgUserObj()->getRoundCount();
	if (!sgUserObj()->getIsGoneNewerProtect())
	{
		mRankInfo[1]->setText(__String::createWithFormat(sgTextHelper(TEXT_SCORE_NEED).c_str(),roundNum)->getCString());
	}
	else
	{
		mRankInfo[1]->setText(__String::createWithFormat("%d", sgUserObj()->getPersonalInfo()->score())->getCString());//sgUserObj()->getPvpPoint())->getCString());
	}	

	mRankInfo[2]->setText(__String::createWithFormat("%.2f%%", sgUserObj()->getPersonalInfo()->winrate())->getCString());//sgUserObj()->getWinRate())->getCString());
	//mRankInfo[3]->setText(__String::createWithFormat("%d",sgRankingControl()->getPlayerScore())->getCString());
	mRankInfo[3]->setText(__String::createWithFormat("%d", sgUserObj()->getPersonalInfo()->honorcount())->getCString());//sgUserObj()->getHonorCount())->getCString());

	int rand = cocos2d::random((int)TEXT_PVP_TIPS_1, (int)TEXT_PVP_TIPS_5);
	mText_Tips->setText(sgTextHelper(rand));


	//
	int count = sgUserObj()->getPersonalInfo()->roomticket();//sgUserObj()->getRoomTicketCount();
	bool bShowTicket = count > 0;// sgUserObj()->getRoomTicketCount() > 0;
	if (bShowTicket)
	{
		mBtnRoom->getChildByName("Room")->setVisible(!bShowTicket);
		Text * roomTicket = dynamic_cast<Text*>(mBtnRoom->getChildByName("Room_Ticket"));
		roomTicket->setVisible(bShowTicket);
		roomTicket->setText(StringUtils::format(" %d/1", sgUserObj()->getPersonalInfo()->roomticket()));//sgUserObj()->getRoomTicketCount()));
	}
	else
	{
		mBtnRoom->getChildByName("Room")->setVisible(!bShowTicket);
		mBtnRoom->getChildByName("Room_Ticket")->setVisible(bShowTicket);
	}
	//关卡限制
	if (sgEnvSet("MGAME_OPEN_PVP_LIMIT"))
	{
		mBtnRand->getChildByName("Sprite_Rand_Lock")->setVisible(false);
		mBtnRoom->getChildByName("Sprite_Room_Lock")->setVisible(false);
		mBtnRand->setTouchEnabled(true);
		mBtnRoom->setTouchEnabled(true);

		mBtnItem->getChildByName("PVP_Item_Time_4")->setVisible(false);
		mBtnItem->setTouchEnabled(true);
		return;
	}

	//int openLevel = sgUserObj()->getMaxOpenCheckPoint();
	int openLevel = sgUserObj()->getMaxOpenGateID();
	if (openLevel < sgEnvSet("PVP_RAND_LEVEL"))
	{
		mBtnRand->getChildByName("Sprite_Rand_Lock")->setVisible(true);
		mBtnRoom->getChildByName("Sprite_Room_Lock")->setVisible(true);
		mBtnRand->getChildByName("mText")->setVisible(false);
		mBtnRoom->getChildByName("mText")->setVisible(false);
		mBtnRand->setTouchEnabled(false);
		mBtnRoom->setTouchEnabled(false);
	}
	else
	{
		mBtnRand->getChildByName("Sprite_Rand_Lock")->setVisible(false);
		mBtnRand->getChildByName("mText")->setVisible(true);
		mBtnRand->setTouchEnabled(true);
		if (openLevel < sgEnvSet("PVP_ROOM_LEVEL"))
		{
			mBtnRoom->getChildByName("Sprite_Room_Lock")->setVisible(true);
			mBtnRoom->getChildByName("mText")->setVisible(false);
			mBtnRoom->setTouchEnabled(false);
		}
		else
		{
			mBtnRoom->getChildByName("Sprite_Room_Lock")->setVisible(false);
			mBtnRoom->getChildByName("mText")->setVisible(true);
			mBtnRoom->setTouchEnabled(true);
		}		
	}
	
	if (!sgUserObj()->getIsGoneNewerProtect()){
		mBtnItem->getChildByName("PVP_Item_Time_4")->setVisible(false);
		mBtnItem->getChildByName("Sprite_NoItem")->setVisible(true);
		mItemText->setVisible(false);
		mBtnItem->setTouchEnabled(false);
	}
	else
	{
		mBtnItem->getChildByName("PVP_Item_Time_4")->setVisible(!sgPvpControl()->getIsNoItemOpen());
		mItemText->setVisible(sgPvpControl()->getIsNoItemOpen());
		mBtnItem->getChildByName("Sprite_NoItem")->setVisible(false);
	}
}

PVPWatchDialog::PVPWatchDialog()
{
	sgMessageManager()->regisiterMessageHandler(EMT_DAILY_MISSION_PROGRESS_BACK, this);
}

PVPWatchDialog::~PVPWatchDialog()
{
	sgMessageManager()->removeMessageHandler(this);
}

bool PVPWatchDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PVPWatchDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		setAnimation("fla_fail");
		auto quitBtn = dynamic_cast<ui::Button*> (mRoot->getChildByName("Button_1"));
		auto lookBtn = dynamic_cast<ui::Button*> (mRoot->getChildByName("Button_2"));

		changeBone(mArmatureBg, "fail_2", quitBtn, 0);
		changeBone(mArmatureBg, "fail_1", lookBtn, 0);
		
		setPopType(E_POP_TYPE_NONE);

		mIsBackPVPDialog = false;
		ret = true;
	} while (0);


	return ret;
}
	
void PVPWatchDialog::onShow()
{
	sgSoundManager()->playMusicEffect(SOUND_UI_FAIL);
}

void PVPWatchDialog::onMenuItemAction(std::string cbName, cocos2d::Ref* sender)
{
	if (cbName == "onBack")
	{
		//上报每日任务数据
		sgUserObj()->sendDailyMissionData(true);
		mIsBackPVPDialog = true;
	}
	else if (cbName == "onWatch")
	{
		this->setShow(false);
		sgMessageManager()->sendMessage(EMT_MENU_BTN, "");
	}
}

void PVPWatchDialog::backPVPDialog()
{
	this->setShow(false);
	PageFunction::Get()->gotoFunction(MSG_FUC_MAIN_LEVEL);
	PageFunction::Get()->gotoFunction(MSG_FUC_WIN_PVP);
}

void PVPWatchDialog::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_DAILY_MISSION_PROGRESS_BACK)
	{
		if (mIsBackPVPDialog)
		{
			backPVPDialog();
			mIsBackPVPDialog = false;
		}
	}
}

PVPRoomBackDialog::PVPRoomBackDialog() :mArmBg(nullptr)
{
}

PVPRoomBackDialog::~PVPRoomBackDialog()
{
}

bool PVPRoomBackDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PVPRoomBackDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		Node* mCenter = mRoot->getChildByName("mCenter");
		mArmBg = cocostudio::Armature::create("fla_failXS");
		mArmBg->setScale(0.5);
		mArmBg->setAnchorPoint(Vec2(0.5, 0.5));
		mCenter->addChild(mArmBg);


		ret = true;
	} while (0);


	return ret;
}

void PVPRoomBackDialog::onMenuItemAction(std::string cbName, cocos2d::Ref* sender)
{
	if (cbName == "onBack")
	{
		PageFunction::Get()->gotoFunction(MSG_FUC_MAIN_LEVEL);
		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_PVP);
		sgGamePrecedure()->setGameIng(true);
		this->setShow(false);
	}
}

void PVPRoomBackDialog::onShow()
{
	sgGamePrecedure()->setGameIng(false);
	if (mArmBg)
	{
		mArmBg->getAnimation()->play("play");
	}

}

PvpInfoDialog::PvpInfoDialog()
{

}

PvpInfoDialog::~PvpInfoDialog()
{

}

bool PvpInfoDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PvpInfoDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		auto missionDialog = sgPageFunction()->getWindowByPageName("MissionDialog.csb");
		if (missionDialog != NULL && missionDialog->isShow())
			showMissionRule(true);
		else
			showMissionRule(false);

		ret = true;
	} while (0);


	return ret;
}

void PvpInfoDialog::showMissionRule(bool isShow)
{
	auto scrollView = dynamic_cast<ui::ScrollView*>(mRoot->getChildByName("ScrollView_1"));
	auto text1 = dynamic_cast<ui::Text*>(scrollView->getChildByName("Text_1"));
	auto word1 = dynamic_cast<ui::Text*>(scrollView->getChildByName("Word_1"));
	auto text2 = dynamic_cast<ui::Text*>(scrollView->getChildByName("Text_1_0"));
	auto word2 = dynamic_cast<ui::Text*>(scrollView->getChildByName("Word_1_0"));
	auto text3 = dynamic_cast<ui::Text*>(scrollView->getChildByName("Text_1_0_0"));
	auto word3 = dynamic_cast<ui::Text*>(scrollView->getChildByName("Word_1_0_0"));
	auto missionText = dynamic_cast<ui::Text*>(scrollView->getChildByName("missionText"));
	auto missionWord = dynamic_cast<ui::Text*>(scrollView->getChildByName("missionWord"));

	text1->setVisible(!isShow);
	word1->setVisible(!isShow);
	text2->setVisible(!isShow);
	word2->setVisible(!isShow);
	text3->setVisible(!isShow);
	word3->setVisible(!isShow);
	missionText->setVisible(isShow);
	missionWord->setVisible(isShow);
	scrollView->setTouchEnabled(!isShow);
}

void PvpInfoDialog::onMenuItemAction(std::string cbName, cocos2d::Ref* sender)
{
	if (cbName == "onBack")
	{
		this->setShow(false);
	}
}

void PvpInfoDialog::onShow()
{
	ActionWindow::onShow();
}

PVPMatchLoadingDialog::PVPMatchLoadingDialog()
	:mMatchLoadingArmature(nullptr),
	mSubArm1(nullptr),
	mMatchNoItemArm(nullptr)
{
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_PVPpipeiss.xml");
}

PVPMatchLoadingDialog::~PVPMatchLoadingDialog()
{
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_PVPpipeiss.xml");
}

bool PVPMatchLoadingDialog::init()
{
	bool ret = false;
	do {
		mRoot = CSLoader::getInstance()->createNode("ui/PVPMatchLoadingDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		Node* mCenter = mRoot->getChildByName("mCenter");
		mMatchLoadingArmature = cocostudio::Armature::create("fla_pvppipeiss");
		mMatchLoadingArmature->setAnchorPoint(Vec2(0.5, 0.5));
		mMatchLoadingArmature->setPositionY(-20);
		mCenter->addChild(mMatchLoadingArmature);

		mSubArm1 = cocostudio::Armature::create("fla_pvppipeiss_ADDITIVE");
		mSubArm1->setAnchorPoint(Vec2(0.5, 0.5));
		mSubArm1->setPosition(Vec2(0,-70));
		mSubArm1->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);
		mCenter->addChild(mSubArm1);
		
		mMatchNoItemArm = cocostudio::Armature::create("fla_PVPpipeiA");
		mCenter->addChild(mMatchNoItemArm);

		mTextLess10 = dynamic_cast<ui::Text*>(mRoot->getChildByName("TextLess10"));
		ret = true;
	} while (0);

	return ret;
}

void PVPMatchLoadingDialog::onMenuItemAction(std::string cbName, cocos2d::Ref* sender)
{
	if (cbName == "onCancel")
	{
		//this->setShow(false);

		setCancelBtnEnable(false);
		sgPvpControl()->getPvPProxy()->sendStopMatch(0);
		sgPageFunction()->setDiamondPanel(true);
	}
}

void PVPMatchLoadingDialog::onShow()
{
	Window::onShow();
	if (mMatchNoItemArm)
		mMatchNoItemArm->setVisible(false);
	if (mMatchLoadingArmature)
		mMatchLoadingArmature->setVisible(false);
	if (mSubArm1)
		mSubArm1->setVisible(false);
	mTextLess10->setVisible(false);
	if (sgPvpControl()->getPVPCMode() != PVP_MODE_NOITEM)
	{
		//int roundNum = 10 - sgUserObj()->getPersonalInfo()->pvpround();//sgUserObj()->getRoundCount();
		if (!sgUserObj()->getIsGoneNewerProtect())
		{
			mTextLess10->setVisible(true);
			mTextLess10->setText(StringUtils::format(sgTextHelper(TEXT_PVP_PROTECT_ROUND10).c_str(), sgUserObj()->getPersonalInfo()->pvpround() + 1, sgEnvSet("NEWER_PROTECT_LEVEL")));
		}
		if (mMatchLoadingArmature) {
			mMatchLoadingArmature->setVisible(true);
			mMatchLoadingArmature->getAnimation()->play("play");
		}
		if (mSubArm1) {
			mSubArm1->setVisible(true);
			mSubArm1->getAnimation()->play("play");
		}
	}
	else
	{
		if (mMatchNoItemArm)
		{
			mMatchNoItemArm->setVisible(true);
			mMatchNoItemArm->getAnimation()->play("play");
		}
	}
	setCancelBtnEnable(true);
	sgPageFunction()->setDiamondPanel(false);
}

void PVPMatchLoadingDialog::setCancelBtnEnable(bool flag)
{
	auto cancelBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("CancelBtn"));
	if (cancelBtn) {
		cancelBtn->setEnabled(flag);
		cancelBtn->setBright(flag);
	}
}
