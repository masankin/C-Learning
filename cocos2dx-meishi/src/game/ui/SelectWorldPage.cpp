#include "SelectWorldPage.h"
#include "ui/UIHelper.h"
#include "GamePrecedure.h"
#include "MessageManager.h"
#include "game/ui//PageFunction.h"
#include "game/data/Tables.h"
#include "game/data/TableManager.h"
#include "guide/GuideManager.h"

#include "net/NetManager.h"
#include "net/UserObj.h"

#include "sync/SyncControl.h"
#include "EnvSet.h"
#include "game/notice/NoticeControl.h"
#include "login/LoginProxy.h"
#include "act/ActControl.h" 
#include "achieve/AchieveControl.h"
#include "SoundManager.h"
#include "act/PowerGiftControl.h"
#include "winner/WinnerControl.h"
#include "chat/ChattingControl.h"
#include "chat/InfoPanel.h"
#include "game/daytask/DayTaskControl.h"
#include "game/pvp/PvpRoomControl.h"
#include "game/pvp/PvpControl.h"
#include "friend/FriendControl.h"
#include "ui/dialogs/GiftAwardDialog.h"
#include "CCLuaEngine.h"
#include "core/ResManager.h"
#include "game/act/ActCrazyWeekControl.h"
#include "mail/MailProxy.h"
#include "core/TimeCalculator.h"
#include "game/sevenGoal/SevenGoalControl.h"
//#include "game/chat/FullSrvInviteBatTips.h"
#define BUTTON_TIP_TAG 100
#define BUTTON_INTELLIGENT_TIP 101

void SelectWorldPage::onMenuItemAction(std::string cbName, cocos2d::Ref* pSender)
{ 
	if (!mNameIcon->getChildByName("TextName")->isVisible() || !mCanTouch)
	{
		CCLOG("[trace]###SelectWorldPage::onMenuItemAction block");
		return;
	}
	CCLOG("[trace]###SelectWorldPage::onMenuItemAction enter");

	if (cbName == "onPanel")
		removeCurIntelGuide(1);
	else
		removeCurIntelGuide(0);

	if (cbName == "onBaoZiFight")
	{
		Node* pNode = dynamic_cast<Node*>(pSender);
		if (pNode)
		{
			//完成新手引导打开关卡界面
			//refreshClickedAni(FLA_ZONGZI_INVADE);

			sgGuideManager()->unRegistGuideMessage();
			CCLOG("[GUIDE]### END to select World message");

			int data = atoi(GET_USERDATA(pNode).c_str());
			sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);
			sgPageFunction()->enableOptionButton(false);
		}
	}
	else if (cbName == "onFoodFuzion")
	{

		sgGuideManager()->unRegistGuideMessage();
		CCLOG("[GUIDE]### END to select World message");

		sgPageFunction()->gotoFunction(MSG_FUC_WIN_PVP);
		sgPageFunction()->enableOptionButton(false);
	}
	else if (cbName == "onTaoTieFeast")
	{
		std::string szTitle = sgTextHelper(TEXT_COMMTIPS_TIPS);
		std::string szContent = sgTextHelper(TEXT_COMMTIPS_WORLD2);
		GenerlDialog::show(szTitle, szContent);
	}
	else if (cbName == "onShop")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_WIN_SHOP);
		sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, "2");
		sgPageFunction()->enableOptionButton(false);  
	}
	else if (cbName == "onSet")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_WIN_SET);
		sgPageFunction()->enableOptionButton(false);
		
	}
	else if (cbName == "onAchieve")
	{
		//if (sgLoginProxy()->checkLogin(true))
		{
			sgPageFunction()->gotoFunction(MSG_FUC_ACHIEVE);
			sgPageFunction()->enableOptionButton(false);
		}
	}
	else if (cbName == "onGift")
	{
		sgActControl()->getActProxy()->sendGetActivityData();

		//完成新手引导--福利
		sgGuideManager()->unRegistGuideMessage();
		CCLOG("[GUIDE]### End to Send Fuli message");
		sgPageFunction()->gotoFunction(MSG_FUC_ACT);
		sgPageFunction()->enableOptionButton(false);
	}
	else if (cbName == "onWinner")
	{
		if (sgWinnerControl()->mWinnerActList.size() == 0)
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_WINNER_ACTTIPS));
			return;
		}
		sgPageFunction()->gotoFunction(MSG_FUC_WINNER);
		sgPageFunction()->enableOptionButton(false);
	}
	else if (cbName == "onRanking")
	{
		//if (sgLoginProxy()->checkLogin())
		{
			sgPageFunction()->gotoFunction(MSG_FUC_RANKING);
			sgPageFunction()->enableOptionButton(false);
		}
	}
	else if (cbName == "onGold")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_WIN_SHOP);
		sgPageFunction()->enableOptionButton(false);
	}
	else if (cbName == "onAccount")
	{
/*		Node* panel = mRoot->getChildByName("mInputPanel");
		ui::TextField* mInputPanel = dynamic_cast<ui::TextField*>(panel->getChildByName("TextField_1"));

		// 请求登陆
#ifndef MGAME_IN_APP_SDK
		panel->setVisible(true);

		sgUserObj()->initPlayerData();
		sgSyncControl()->initSyncData();
		sgUserObj()->setInputID(mInputPanel->getString());
		sgUserObj()->saveToFile();
		sgNetManager()->DisConnect();
		sgLoginProxy()->sendGetServerRqst();
#endif*/
	}
	else if (cbName == "onNotice")
	{
		//公告
		sgPageFunction()->gotoFunction(MSG_FUC_ACTINFORM);
	}
	else if (cbName == "onBag")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_BACKPACK);
	}
	else if(cbName == "onPower")
	{
		//if (sgPowerGiftControl()->getIsOpen() == 1)
		//{
			sgPowerGiftControl()->sendGetPowerActivityDataRqst();
			PageFunction::Get()->gotoFunction(MSG_FUC_POWER_GIFT);
		//}
		/*else
		{
			if(sgPowerGiftControl()->getActID() == 1)
			{
				GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_POWER_OFF));
			}
			else
			{
				GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_ENERGY_OFF));
			}
		}*/
	}
	else if (cbName == "onFirstCharge")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_FIRST_CHARGE);
	}
	else if (cbName == "onDaily")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_DAYTASK);
	}
	else if(cbName == "onHead")
	{
		sgUserObj()->setCurInfoUin(sgUserObj()->getUin());
		sgUserObj()->setCurInfoType(eInfoPanelType::E_TYPE_PERSONAL);
		sgPageFunction()->gotoFunction(MSG_FUC_INFO);
	}
	else if (cbName == "onFriend")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_FRIEND);
	}
	else if (cbName == "onPk")
	{
		if (sgFriendControl()->getPkList().size() > 0)
		{
			sgFriendControl()->setPkPlayerData(sgFriendControl()->getPkList().back());
			std::string inviteName = sgFriendControl()->getPkPlayerData()->m_sName;
			std::string content = StringUtils::format(sgTextHelper(TEXT_PVP_ROOM_ACCEPT_CONFIRM).c_str(), inviteName.c_str());
			// change by jim, unify tips ui
			//GeneralDialogOther::show(content, GeneralDialogOther::E_GENERAL_TYPE_PK_CONFIRM, CC_CALLBACK_1(SelectWorldPage::pkIconCallback, this));
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), content, CC_CALLBACK_1(SelectWorldPage::pkIconCallback, this),false, true, E_GENERAL_TYPE_PK_CONFIRM);
		}
		else
		{
			sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_PK_NONE).asString());
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_PVP_ROOM_12));
		}
	}
	else if (cbName == "onTeach")
	{ 
		openUI(MSG_FUC_NEWER_BIBLE);
	}
	else if (cbName == "onCrazyWeek")
	{
		//CCLOG("   show crazy week");
		sgPageFunction()->gotoFunction(MSG_FUC_CRAZYWEEK);
	}
	else if (cbName == "onMail")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_MAIL);
	}
	else if (cbName == "onVip")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_VIP);
	} 
	else if (cbName == "onGlyph")
	{ 
		const GuideData* pData = sgGuideManager()->getGuideDataByGuideID(GuideID_Glyph);
		if (sgUserObj()->getMaxOpenGateID()< (pData == NULL? 1100 : pData->getGateID()))
		{
			std::string szTitle = sgTextHelper(TEXT_COMMTIPS_TIPS);	
			std::string szContent = sgTextHelper(TEXT_COMMTIPS_GLYPGNOOPEN);
			GenerlDialog::show(szTitle, szContent);
			return;
		}
		sgGuideManager()->unRegistGuideMessage();
		sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "GlyphBookUI.csb", 1);
	}
	else if (cbName == "onNewYear")
	{
		//sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "RedPacketDialog.csb", 1);//c++tolua
		//if (mHasNewYearRed && !mNewYearCanReward)
		//{
		//	sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 0", MSG_FUC_REDPACKET));
		//}
		////判断:1.有红点 2.红包不可领取之后关闭红点
	}
	else if (cbName == "onHonor")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "HonorShopHelp.csb", 1);
	}
	else if (cbName == "onValentine")
	{
		//sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "ValentineDialog.csb", 1);
	}
	else if (cbName == "onTask")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "MissionDialog.csb", 1);
	}
	else if (cbName == "onCheck") // 礼包推送跳转
	{
		CCLOG("[trace]  actNotice Dialog onCheck  type == %d", mActType);
		switch (mActType)
		{
		case ACT_FIRST_CHARGE:  // 跳到首冲
			sgPageFunction()->gotoFunction(MSG_FUC_FIRST_CHARGE);
			break;
		case ACT_RECHARGE:	// 跳到累计冲
			sgPageFunction()->gotoFunction(MSG_FUC_ACT);
			sgMessageManager()->sendMessage(EMT_GOTO_ACTLAB, StringUtils::format("%d %d", MSG_FUC_ACT, ACT_RECHARGE));
			break;
		default:
			break;
		}
		mActType = 0;
	}
	else if (cbName == "onClose")  // 关闭礼包推送
	{
		CCLOG("[trace]  actNotice Dialog onclose");
		showActNoticeDialog(false, ACT_NULL);
	}
	else if (cbName == "onBadge")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_BADGE);
	}
	else if (cbName == "onBtnRank")
	{	
		//去巅峰排行
		sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "TopRankDialog.csb", 1);
	}
	else if (cbName == "onInstitute")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "InstituteView.csb", 1);
	}
	else if (cbName == "onAssistants")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "AssistantPanel.csb", 1);
	}
	else if (cbName == "onSevenGoalShowInfo")
	{
		bool bVisible = mImage_bg_SevenGoal->isVisible();
		changeSevenGoalPanelState(!bVisible);

		if (true == !bVisible)
		{
			sgSevenGoalControl()->sendGetSevenGoalListRqst();
		}
	}
	else if (cbName == "onSevenGoalGet")
	{
		if (mbNowComplete)
		{
			sgSevenGoalControl()->sendRetriveSevenGoalRqst(mNowDay);
		}
		else if (mbNextComplete == true)
		{
			Window::showTextTip(sgTextHelper(TEXT_SEVEN_GOAL_NEXT_GET));
		}
	}

#ifdef MGAME_PLATFORM_MPLUS
	else if (cbName == "onFiveStar")
	{
		if (!sgGuideManager()->getGuideLock())
			sgPageFunction()->gotoFunction(MSG_FUC_FIVE_STAR);
	}
#endif
	showActNoticeDialog(false, ACT_NULL);
}

bool SelectWorldPage::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/SelectWorldPage.csb");
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);  
		sgLoginProxy()->sendOther2Rqst();
		//初始化跑马灯
		sgNoticeControl()->showNoticeTip();
		sgPowerGiftControl();
		this->addChild(mRoot);
		auto btnPower = mRoot->getChildByName("BtnPower");
		btnPower->setVisible(true);
		auto btnShop = mRoot->getChildByName("PanelShop");
		auto btnWinner = mRoot->getChildByName("PanelWinner");
		auto btnFirstCharge = mRoot->getChildByName("FirstChargeBtn");

		// crazy week btn
		auto btnCrazyWeek = mRoot->getChildByName("CrazyWeekBtn");
		if (btnCrazyWeek) {
			if (sgActCrazyWeekControl()->getIsCrazyWeekOpen()){
				btnCrazyWeek->setVisible(true);
			}
			else{
				btnCrazyWeek->setVisible(false);
			}
		}

		auto DailyBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("BtnDaily"));
		if (DailyBtn) {
			DailyBtn->setVisible(false);
			DailyBtn->setEnabled(false);
		}

#ifdef MGAME_PLATFORM_MPLUS
		auto btnFiveStar = mRoot->getChildByName("BtnFiveStar");
		if (btnFiveStar)
			btnFiveStar->setVisible(sgMplusControl()->getIsShowFiveStar());

		if (DailyBtn) {
			DailyBtn->setVisible(true);
			DailyBtn->setEnabled(true);
		}
#endif	

		//redpacket btn
		auto btnNewYear = mRoot->getChildByName("NewYearBtn");
		btnNewYear->setVisible(false);

		// cristmas btn 
		auto christmasBtn = mRoot->getChildByName("ChristmasBtn");
		christmasBtn->setVisible(false);

		// valentine btn 
		auto valentineBtn = mRoot->getChildByName("ValentineBtn");
		valentineBtn->setVisible(false);


		//巅峰排行
		if (false == sgActControl()->getIsTopRankActOpen())
		{
			mRoot->getChildByName("BtnTopRank")->setVisible(false);
		}


		auto instituteBtn = mRoot->getChildByName("InstituteBtn");
		if (instituteBtn) {
			instituteBtn->setVisible(false);
		} 

		// 研究院和将军等级限制
		{
			int openLevel = sgUserObj()->getMaxOpenGateID();
			const GuideData* pData = sgGuideManager()->getGuideDataByGuideID(GuideID_Institute);
			if (openLevel >= (pData == NULL ? sgEnvSet("INSTITUTE_OPEN_LEVLE") : pData->getGateID()))
			{
				/*auto xueyuanAni = cocostudio::CCArmature::create("fla_MSxueyuan");
				auto btnSize = instituteBtn->getContentSize();
				auto pos = instituteBtn->getPosition();
				instituteBtn->setPosition(Vec2(pos.x, pos.y + 5));
				xueyuanAni->setPosition(Vec2(btnSize.width / 2, btnSize.height / 2 + 6));
				xueyuanAni->getAnimation()->play("play");
				instituteBtn->addChild(xueyuanAni);*/

				auto btnSize = instituteBtn->getContentSize();
				mFreeTipsArm = cocostudio::CCArmature::create("fla_mfqp");
				mFreeTipsArm->setPosition(Vec2(-15, btnSize.height - 25));
				mFreeTipsArm->getAnimation()->play("play");
				instituteBtn->addChild(mFreeTipsArm);
				mFreeTipsArm->setVisible(false);

				instituteBtn->setVisible(true);
			}
				
		}
		

		mActivityUpBtn[0] = mRoot->getChildByName("BtnNotice");
		mActivityUpBtn[1] = mRoot->getChildByName("PanelGift");
		mActivityUpBtn[2] = mRoot->getChildByName("PanelWinner");
		mActivityUpBtn[3] = mRoot->getChildByName("CrazyWeekBtn");
		mActivityUpBtn[4] = mRoot->getChildByName("BtnTask");
		mActivityUpBtn[5] = mRoot->getChildByName("BtnPower");
		mActivityUpBtn[6] = mRoot->getChildByName("vipButton");
		mActivityUpBtn[7] = mRoot->getChildByName("FirstChargeBtn");
		mActivityUpBtn[8] = mRoot->getChildByName("ChristmasBtn");
		mActivityUpBtn[9] = mRoot->getChildByName("NewYearBtn");
		mActivityUpBtn[10] = mRoot->getChildByName("ValentineBtn");
		mActivityUpBtn[11] = mRoot->getChildByName("BtnTopRank");
		//mActivityUpBtn[11] = mRoot->getChildByName("BtnDailyHeroes");

		mUpBtnPositions[0] = mActivityUpBtn[0]->getPosition();
		mUpBtnPositions[1] = mActivityUpBtn[1]->getPosition();
		mUpBtnPositions[2] = mActivityUpBtn[2]->getPosition();
		mUpBtnPositions[3] = mActivityUpBtn[3]->getPosition();
		mUpBtnPositions[4] = mActivityUpBtn[4]->getPosition();
		mUpBtnPositions[5] = mActivityUpBtn[5]->getPosition();
		mUpBtnPositions[6] = mActivityUpBtn[6]->getPosition();
		mUpBtnPositions[7] = mActivityUpBtn[7]->getPosition();
		mUpBtnPositions[8] = mActivityUpBtn[8]->getPosition();
		mUpBtnPositions[9] = mActivityUpBtn[9]->getPosition();
		mUpBtnPositions[10] = mActivityUpBtn[10]->getPosition();
		mUpBtnPositions[11] = mActivityUpBtn[11]->getPosition();
		//mUpBtnPositions[11] = mActivityUpBtn[11]->getPosition();

		mDownBtn[0] = mRoot->getChildByName("BtnSet");
		mDownBtn[1] = mRoot->getChildByName("BtnTeach");
		mDownBtn[2] = mRoot->getChildByName("BtnFriend");
		mDownBtn[3] = mRoot->getChildByName("BtnAchieve");
		mDownBtn[4] = mRoot->getChildByName("BtnBag");
		mDownBtn[5] = mRoot->getChildByName("BtnGlyph");
		mDownBtn[6] = mRoot->getChildByName("BtnBadge");
		mDownBtn[7] = mRoot->getChildByName("BtnAssistants");

		mDownBtnPositions[0] = mDownBtn[0]->getPosition();
		mDownBtnPositions[1] = mDownBtn[1]->getPosition();
		mDownBtnPositions[2] = mDownBtn[2]->getPosition();
		mDownBtnPositions[3] = mDownBtn[3]->getPosition();
		mDownBtnPositions[4] = mDownBtn[4]->getPosition();
		mDownBtnPositions[5] = mDownBtn[5]->getPosition();
		mDownBtnPositions[6] = mDownBtn[6]->getPosition();
		mDownBtnPositions[7] = mDownBtn[7]->getPosition();

		reOrderActivityBtn();
		reOrderDownBtn();

		initMainAni();
		Button* mMenu = dynamic_cast<Button*>(mRoot->getChildByName("BtnMenu"));
		if (mMenu)
		mMenu->setPositionX(mMenu->getPositionX() + 8);

		mOptionButton = dynamic_cast<Button*>(mRoot->getChildByName("BtnSet"));
		
		Node* panel = mRoot->getChildByName("mInputPanel");
		
#ifdef MGAME_IN_APP_SDK
		panel->setVisible(false);

#else 
		panel->setVisible(true);
		ui::TextField* mInputPanel = dynamic_cast<ui::TextField*>(panel->getChildByName("TextField_1"));
		mInputPanel->addEventListenerTextField(this, textfieldeventselector(SelectWorldPage::TextFieldCallback));
		mInputPanel->setString(sgUserObj()->getOpenID());
#endif



		//Size size = Director::getInstance()->getWinSize();


		changeStarLevelNum();

		//接入钻石获取动画收集器
		Node* pOriginReceiver = sgPageFunction()->getDiamondPanel()->getChildren().at(0);
		Node* pReceiver = CSLoader::getInstance()->createNode("MoneyBuyNode.csb");
		sgPageFunction()->getActionDiamondLayer()->addReceiver(RECEIVER_ACCOUNT, pReceiver, pOriginReceiver->getPosition(), false, pOriginReceiver);

		//接入道具获取动画收集器
		Node* pItemOriginReceiver = mRoot->getChildByName("BtnBag");
		Node* pItemReceiver = Sprite::createWithSpriteFrameName("ui/main/main_bag.png");
		sgPageFunction()->getActionPackageLayer()->addReceiver(RECEIVER_ALONE, pItemReceiver, pItemOriginReceiver->getPosition(), true);
		//接入礼盒获取动画收集器
		Node* pChestReceiver = Sprite::createWithSpriteFrameName("ui/main/valentine.png");
		sgPageFunction()->getActionChestLayer()->addReceiver(RECEIVER_ALONE, pChestReceiver, pItemOriginReceiver->getPosition(), true);

		//加载shop.csv
		///sgTableManager()->loadShopTable();

		sgPvpRoomControl();	

		//约战提示
		m_pFriendButton = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnFriend"));
		m_pPkIconImage = dynamic_cast<ui::ImageView*>(m_pFriendButton->getChildByName("Image_PkIcon"));

		sgMessageManager()->sendMessage(EMT_CHANGE_PLAYER_ICON, sgUserObj()->getPersonalInfo()->iconid());// sgUserObj()->getPlayerIconID()));

		initItemAni();
		// initShakeBackGround();
		mNameIcon = dynamic_cast<Button*>(mRoot->getChildByName("btn_name"));

		auto btn_level = dynamic_cast<Button*>(mRoot->getChildByName("btn_level"));
		mPlayerLevel = dynamic_cast<Text*>(btn_level->getChildByName("text_level"));

		mHasNewYearRed = false;
		mNewYearCanReward = false;
		//mCurIntelType = 0;	

		mActNoticeNode = CSLoader::createNode("ui/ActNoticeDialog.csb");
		auto teachPos = dynamic_cast<ui::Widget*>(mRoot->getChildByName("BtnTeach"))->getPosition();
		mActNoticeNode->setPosition(Vec2(teachPos.x+50, teachPos.y + 150));
		addChild(mActNoticeNode);
		mActNoticeTitle = dynamic_cast<ui::Text*>(mActNoticeNode->getChildByName("title_label"));
		mActNoticeTitle->setString("XX");

		mActIcon = dynamic_cast<ui::ImageView*>(mActNoticeNode->getChildByName("Image_1"));
		//mActIcon->setVisible(false);
		onLocateClickCallback(mActNoticeNode);

		// act notice unvisibale by default
		mActNoticeNode->setVisible(false);

		//mCurIntelType = 0;
		mAddTag = true;



		const GuideData* pData = sgGuideManager()->getGuideDataByGuideID(GuideID_Glyph);
		if (sgUserObj()->getMaxOpenGateID() >= (pData != NULL ? pData->getGateID() : 1110))
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eGlyphRedPointRqst", NULL);

		if (sgUserObj()->getMaxOpenGateID() > sgEnvSet("ASSISTANT_LIMIT_LEVEL"))
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eAsstRedPointRqst", NULL);

		if(sgMailData()->isNewMail())
			sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 1", MSG_FUC_MAIL));


		Node* pAsstBtn = mRoot->getChildByName("BtnAssistants");
		if (pAsstBtn)
		{
			pAsstBtn->setVisible(false);

			int openLevel = sgUserObj()->getMaxOpenGateID();
			const GuideData* pData = sgGuideManager()->getGuideDataByGuideID(GuideID_Institute);
			if (openLevel >= (pData == NULL ? sgEnvSet("ASSISTANT_LIMIT_LEVEL") : pData->getGateID()))
			{
				pAsstBtn->setVisible(true);
			}  
		}
		auto pTextHonor = dynamic_cast<Text*>(mRoot->getChildByName("Text_Honor"));
		pTextHonor->setString(StringUtils::format("%d", sgUserObj()->getPersonalInfo()->honorcount()));

		ret = true;
		mCanTouch = true;

		//------------------------sevenGoal code-------------------------

		mButton_icon = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_icon"));
		mImage_bg_SevenGoal = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("Image_bg"));
		mText_day = dynamic_cast<ui::Text*>(mImage_bg_SevenGoal->getChildByName("Text_day"));
		mText_info = dynamic_cast<ui::Text*>(mImage_bg_SevenGoal->getChildByName("Text_info"));
		mText_boxInfo = dynamic_cast<ui::Text*>(mImage_bg_SevenGoal->getChildByName("Text_boxInfo"));
		mPanel_pos = dynamic_cast<ui::Layout*>(mImage_bg_SevenGoal->getChildByName("Panel_pos"));

		mPanel_pos->setLocalZOrder(10);
		mText_boxInfo->setLocalZOrder(11);

		CCArmatureDataManager::getInstance()->addArmatureFileInfo("fla_XGBaoX.png", "fla_XGBaoX.plist", "fla_XGBaoX.xml");
		//宝箱
		mBoxAni = cocostudio::CCArmature::create("fla_XGBaoX");
		mBoxAni->setScale(0.5);
		mBoxAni->setAnchorPoint(Point(0.5, 0.5));
		mBoxAni->setPosition(mPanel_pos->getPosition());
		mImage_bg_SevenGoal->addChild(mBoxAni);

		mBoxAni->getAnimation()->play("step3", -1, 1);


		CCArmatureDataManager::getInstance()->addArmatureFileInfo("fla_HongDian.png", "fla_HongDian.plist", "fla_HongDian.xml");
		//红点
		mHongdianAni = cocostudio::CCArmature::create("fla_HongDian");
		mHongdianAni->getAnimation()->play("play");
		mButton_icon->addChild(mHongdianAni);
		auto btnSize = mButton_icon->getContentSize();
		mHongdianAni->setPosition(btnSize.width - 15, btnSize.height - 15);
		mHongdianAni->setVisible(false);

		Director::getInstance()->getEventDispatcher()->addCustomEventListener("eSevenGoalUpdate", CC_CALLBACK_1(SelectWorldPage::updateSevenGoalEvent, this, 2));
	} while (0); 
	return ret;
}


void SelectWorldPage::resetUIShow()
{
	//sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", MSG_FUC_NEWER_GIFT, sgActControl()->getIsNewerGiftOpened()));

	ui::Text* textPlayerName = dynamic_cast<ui::Text*>(mNameIcon->getChildByName("TextName"));
	textPlayerName->setString(sgUserObj()->getPlayerName());
#ifdef MGAME_PLATFORM_MPLUS	
	if (textPlayerName) {
		textPlayerName->setFontName(DEFAULT_FONT);
	}
#endif
	m_pPkIconImage->setVisible(false);
	//加入骨骼动画
	m_pPkIconImage->runAction(CCRepeatForever::create(Sequence::create(ScaleTo::create(.2f, .8f), ScaleTo::create(.2f, 1.0f), NULL)));
	//this->schedule(schedule_selector(SelectWorldPage::updatePkIcon), .1f);

	mRoot->addChild(sgChattingControl()->getChattingButton());
	
	setPlayerIconVisible(false);
	if (sgUserObj()->getPersonalInfo()->changenamecount() == 0)
	{
		openUI(MSG_FUC_NAMEING_DIALOG);
	}
	else
	{
		if (!sgGuideManager()->checkGuideFinishByGroup(GuideGroup_SkipRookieCamp))
			openUI(1025, MSG_FUC_LUA_WIN);
	}
}

void SelectWorldPage::onEnter()
{
	CCLOG("[trace]###SelectWorldPage::onEnter");
	GameScene::onEnter(); 


	sgMessageManager()->regisiterMessageHandler(EMT_CLOSEPAGE, this); 
	sgMessageManager()->regisiterMessageHandler(EMT_GUIDE_TIPS, this); 
	sgMessageManager()->regisiterMessageHandler(EMT_RQST_RESULT, this);
	sgMessageManager()->regisiterMessageHandler(EMT_RED_PACKET_CAN_REWARD, this);
	sgMessageManager()->regisiterMessageHandler(EMT_SELECTWORD_SCHEDULE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_DALIY_MISSION_TIPS, this);
	sgMessageManager()->regisiterMessageHandler(EMT_OPEN_DAILY_MISSION, this);
	sgMessageManager()->regisiterMessageHandler(EMT_GUIDE_FIRST, this);
	this->setAccelerometerEnabled(true);

	//mCurIntelType = 0;
	mAddTag = true;
	sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
	//scheduleOnce(schedule_selector(SelectWorldPage::waitShowPanel), Director::getInstance()->getAnimationInterval() + 0.1f); 
	//sgUserObj()->saveToFile();
	
	removeUnusedTextures();

	checkInsituteFreeState();
}

void SelectWorldPage::onExit()
{
	removeCurIntelGuide(0);
	GameScene::onExit();	
	sgMessageManager()->removeMessageHandler(EMT_CLOSEPAGE, this);
	sgMessageManager()->removeMessageHandler(EMT_GUIDE_TIPS, this);
	sgMessageManager()->removeMessageHandler(EMT_RQST_RESULT, this);
	sgMessageManager()->removeMessageHandler(EMT_RED_PACKET_CAN_REWARD, this);
	sgMessageManager()->removeMessageHandler(EMT_SELECTWORD_SCHEDULE, this);
	sgMessageManager()->removeMessageHandler(EMT_DALIY_MISSION_TIPS, this);
	sgMessageManager()->removeMessageHandler(EMT_OPEN_DAILY_MISSION, this);
	sgMessageManager()->removeMessageHandler(EMT_HONOR_CHANGE, this);
	sgMessageManager()->removeMessageHandler(EMT_GUIDE_FIRST, this);
	sgUserObj()->saveToFile();
}

void SelectWorldPage::initMainAni()
{
	//动画

	auto nodeMainAni = mRoot->getChildByName("NodeMainAni");
	if (!nodeMainAni) return;

	auto beijingAni = cocostudio::CCArmature::create("fla_zhujiemian");
	beijingAni->getAnimation()->play("play");
	nodeMainAni->addChild(beijingAni);

	auto NodeAdditiveAni = mRoot->getChildByName("NodeAdditiveAni");
	if (!NodeAdditiveAni) return;

	auto beijingAdditiveAni = cocostudio::CCArmature::create("fla_zhujiemian_ADDITIVE");
	beijingAdditiveAni->setBlendFunc(BlendFunc::ADDITIVE);
	beijingAdditiveAni->getAnimation()->play("play");
	NodeAdditiveAni->addChild(beijingAdditiveAni);


	auto btnGift = mRoot->getChildByName("PanelGift");
	if (btnGift)
	{
		auto Armat = cocostudio::CCArmature::create("fla_ZhuJieMianAN_fuli");
		auto bone = Armat->getBone("ZhuJieMianAN_fuli_zxw");
		bone->getChildArmature()->setBlendFunc(BlendFunc::ADDITIVE);
		Armat->getAnimation()->play("play");
		Armat->setPosition(btnGift->getContentSize() / 2);
		btnGift->addChild(Armat);
	}

	//auto btnNewerGift = mRoot->getChildByName("PanelNewerGift");
	//if (btnNewerGift)
	//{
	//	//auto Armat = cocostudio::CCArmature::create("fla_ZhuJieMianAN_libao");
	//	//Armat->getAnimation()->play("play", -1, 1);
	//	//Armat->setPosition(btnNewerGift->getContentSize() / 2);
	//	//btnNewerGift->addChild(Armat);
	//}
	auto btnWinner = mRoot->getChildByName("PanelWinner");
	if (btnWinner)
	{
		auto Armat = cocostudio::CCArmature::create("fla_ZhuJieMianAN_wangzhe");
		Armat->getAnimation()->play("play", -1, 1);
		Armat->setPosition(btnWinner->getContentSize() / 2);
		btnWinner->addChild(Armat);

	}
	/*auto btnShop = mRoot->getChildByName("PanelShop");
	if (btnShop)
	{
	auto Armat = cocostudio::CCArmature::create("fla_ZhuJieMianAN_shangcheng");
	Armat->getAnimation()->play("play", -1, 1);
	Armat->setPosition(btnShop->getContentSize() / 2);
	btnShop->addChild(Armat);
	}*/
	
}




SelectWorldPage::SelectWorldPage() :mCoin(0), mActNoticeNode(nullptr), mActNoticeTitle(nullptr), mActIcon(nullptr), mActType(0), mFreeTipsArm(nullptr)
, mNowDay(0)
, mIsRetrived(false)
, mNowNum(0)
, mNextNum(0)
, TextcolorNowNormal(Color3B(202, 118, 57))
, TextcolorNowComplete(Color3B(17, 143, 4))
, TextcolorNextNormal(Color3B(113, 72, 15))
, mbNowComplete(false)
, mbNextComplete(false)

{
	CCLOG("[trace]SelectWorldPage::SelectWorldPage() ctor execute");
	sgMessageManager()->regisiterMessageHandler(EMT_REFRESH_STAR, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CHANNEL_LOGIN, this); 
	sgMessageManager()->regisiterMessageHandler(EMT_OPEN_STAGE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_HONGDIAN_TIP,this);
	sgMessageManager()->regisiterMessageHandler(EMT_ENABLE_OPTIONBUTTON, this);
	sgMessageManager()->regisiterMessageHandler(EMT_BTN_CONTROL, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CHANGE_NAME, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CHANGE_PLAYER_ICON, this);
	sgMessageManager()->regisiterMessageHandler(EMT_FRIEND_UPDATE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CLOSE_PVP, this); 
	sgMessageManager()->regisiterMessageHandler(EMT_HONOR_CHANGE, this);

	sgMessageManager()->regisiterMessageHandler(EMT_BUY_VIP, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PLAYER_UPGRADE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_INVITE_BAT_SWITCH, this);
	
	//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_ZhuJieMian.png", "fla_ZhuJieMian.plist", "fla_ZhuJieMian.xml");
	//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_ZhuJieMianAN.png", "fla_ZhuJieMianAN.plist", "fla_ZhuJieMianAN.xml");
	 
	showVIPTips = false;
	for (int i = 0; i < ACTIVITY_BTN_COUNT; i++) {
		mUpBtnPositions[i] = Vec2::ZERO;
		mActivityUpBtn[i] = NULL;
	}
}

void SelectWorldPage::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_REFRESH_STAR)
	{
		changeStarLevelNum();
	}
	else if (message->getMessageType() == EMT_CHANNEL_LOGIN)
	{
		// sdk登陆成功，连接游戏服务器
		MessageChannelLogin* loginMsg = dynamic_cast<MessageChannelLogin*>(message);
		if (loginMsg && loginMsg->mResult == 1)
		{
			sgLoginProxy()->analysisLoginDNS();
			sgLoginProxy()->requestLoginKey();
			//GenerlDialog::show("login", "login .......");
		}
	}
	else if (message->getMessageType() == EMT_MONEY_CHANGE)
	{
		MessageMoneyChange* pMes = dynamic_cast<MessageMoneyChange*>(message);
		if (pMes)
		{
			int curGold = sgUserObj()->getPersonalInfo()->diamond();// sgUserObj()->getGoldCoin();
			char szText[32] = { 0, };
			sprintf(szText, "%d", curGold);
			mCoin->setText(szText);
		}
	}else if (message->getMessageType() == EMT_OPEN_STAGE)
	{
		MessageOpenStage* pOpenStage = dynamic_cast<MessageOpenStage*>(message);
		if (pOpenStage->mStage == 2)
		{
			changeStarLevelNum();
		}
	}
	else if(message->getMessageType() == EMT_CLOSEPAGE)
	{
		MessageClosePage* tMessage = dynamic_cast<MessageClosePage* >(message);
		if(tMessage != NULL)
		{
			//开始新手引导--选择大关 
			doGuideNode();
		}
	}else if (message->getMessageType() == EMT_HONGDIAN_TIP)
	{
		MessageAchieveTip* pTipMes = dynamic_cast<MessageAchieveTip*>(message);
		if (!pTipMes) return;
		ui::Widget* pBtn = nullptr;
		bool flag = pTipMes->getIsShow();
		int type = 0;
		switch (pTipMes->getBtnType())
		{
		case MSG_FUC_ACHIEVE:
			{
				pBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("BtnAchieve"));
				type = EIT_ACHIEVE;
			}
			break;
		case MSG_FUC_ACT:
			{
				pBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("PanelGift"));
				flag = sgActControl()->checkCurrActCanGet();
				type = EIT_GIFT;
			}
			break;
		case MSG_FUC_WINNER:
			{
				pBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("PanelWinner"));
				flag = sgWinnerControl()->getCanGet();
				type = EIT_WINNER;
			}
			break;
		case MSG_FUC_DAYTASK:
			{
				pBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("BtnDaily"));
				type = EIT_DAYTASK;
			}
			break;
		case MSG_FUC_POWER_GIFT:
			{
				pBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("BtnPower"));
				type = EIT_POWER;
			}
			break;
		case MSG_FUC_CRAZYWEEK:
			{
				pBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("CrazyWeekBtn"));
				type = EIT_CRAZYWEEK;
			}
			break;
		case MSG_FUC_MAIL:
			{
				pBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("MailBtn"));
				pBtn->setVisible(flag);
				type = EIT_NONE;
			}
			break;
		case MSG_FUC_ACTINFORM:
			{
				pBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("BtnNotice"));
				type = EIT_NONE;
			}
			break;
		case MSG_FUC_GLYPH:
			{
				pBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnGlyph"));
				type = EIT_GLYPH;
			}
			break;
		case MSG_FUC_CHRISTMAS:
			{
				pBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("ChristmasBtn"));
				type = EIT_NONE;
			}
			break;
		case MSG_FUC_REDPACKET:
			{
				pBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("NewYearBtn"));
				mHasNewYearRed = flag;
				type = EIT_NEWYEAR;
				break;
			}
		case MSG_FUC_FIRST_CHARGE:
		{
			pBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("FirstChargeBtn"));
			showActNoticeDialog(flag, ACT_FIRST_CHARGE);
			type = EIT_NONE;
			break;
		} 
		case MSG_FUC_TOTAL_CHARGE:
			showActNoticeDialog(flag, ACT_RECHARGE);
			type = EIT_NONE;
			break;
		case MSG_FUC_DAILY_MISSION:
		{
			pBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("BtnTask"));
			type = EIT_NONE;
			sgUserObj()->setIsShowDailyMissionHot(flag);
			break;
		}
		case MSG_FUC_INSTITUTE_FREE:
		{
			if (mFreeTipsArm) {
				mFreeTipsArm->setVisible(flag);
			}
			break;
		}
		case MSG_FUC_LUA_WIN + 50: //精灵界面
		{
			pBtn = dynamic_cast<ui::Widget*>(mRoot->getChildByName("BtnAssistants"));
			type = EIT_NONE;
			 break;
		}
		default:
			break;
		}
		if(pBtn){
			playTipAction(pBtn, flag, type);
		}
	}
	else if (message->getMessageType() == EMT_ENABLE_OPTIONBUTTON)
	{
		return;
		MessageEnableOption* pMsg = dynamic_cast<MessageEnableOption* >(message);
		bool bEnable = pMsg->getEnable() == 1 ? true : false;
		mOptionButton->setVisible(bEnable);
	}
	else if (message->getMessageType() == EMT_RED_PACKET_CAN_REWARD)
	{
		MessageRedReward* pMsg = dynamic_cast<MessageRedReward* >(message);
		if (pMsg)
			mNewYearCanReward = pMsg->getIsReward();
	}
	else if(message->getMessageType() == EMT_BTN_CONTROL)
	{
		MessageBtnControl* pMes = dynamic_cast<MessageBtnControl*>(message);
		if (!pMes) return;
		switch (pMes->getBtnType())
		{
		case MSG_FUC_POWER_GIFT:
			{
				mRoot->getChildByName("BtnPower")->setVisible(pMes->getIsShow());
				break;
			}
		case MSG_FUC_FIRST_CHARGE:
			{
				mRoot->getChildByName("FirstChargeBtn")->setVisible(pMes->getIsShow());
				break;
			}
		case MSG_FUC_CRAZYWEEK:
			{
				mRoot->getChildByName("CrazyWeekBtn")->setVisible(pMes->getIsShow());
				break;
			}
		case MSG_FUC_REDPACKET:
			{
				mRoot->getChildByName("NewYearBtn")->setVisible(pMes->getIsShow());//设置位置(根据圣诞活动) 服务器发送消息判断是否存在
				break;
			}
		case MSG_FUC_CHRISTMAS:
			{
				mRoot->getChildByName("ChristmasBtn")->setVisible(pMes->getIsShow());
				break;
			}
		case MSG_FUC_VALENTINE:
			{
				mRoot->getChildByName("ValentineBtn")->setVisible(pMes->getIsShow());
				break;
			}
		case MSG_FUC_DAILY_MISSION:
			{
				mRoot->getChildByName("BtnTask")->setVisible(pMes->getIsShow());
				sgUserObj()->setIsShowDailyMissionBtn(pMes->getIsShow());
				break;
			}
		case MSG_FUC_TOP_RANK:
		{
			mRoot->getChildByName("BtnTopRank")->setVisible(pMes->getIsShow());
			break;
		}
		default:
			break;
		}

		// 重新排位
		reOrderActivityBtn();
	}
	else if (message->getMessageType() == EMT_CHANGE_NAME)
	{
		MessageChangeName* pMes = dynamic_cast<MessageChangeName*>(message);
		if (pMes && pMes->mResult >= 1)
		{
			ui::Text* textPlayerName = dynamic_cast<ui::Text*>(mNameIcon->getChildByName("TextName"));
			textPlayerName->setString(sgUserObj()->getPlayerName());
			if (pMes->mResult == 2)
			{
				playerIconFly();
			}
		}
	}
	else if (message->getMessageType() == EMT_FRIEND_UPDATE)
	{
		MessageFriend* pMes = dynamic_cast<MessageFriend*>(message);
		if (pMes->getListId() == E_FRIEND_PK_NONE)
		{
			m_pPkIconImage->setVisible(false);
		}
		else if (pMes->getListId() == E_FRIEND_PK_HAVE)
		{
			m_pPkIconImage->setVisible(true);
		}
		else if (pMes->getListId() == E_FRIEND_APPLIACTION_NONE)
		{
			playTipAction(m_pFriendButton, false, EIT_FRIEND);
		}
		else if (pMes->getListId() == E_FRIEND_APPLIACTION_HAVE)
		{
			if (m_pPkIconImage->isVisible())
			{
				playTipAction(m_pFriendButton, false, EIT_FRIEND);
			}
			else
			{
				playTipAction(m_pFriendButton, true, EIT_FRIEND);
			}
		}
	}
	else if (message->getMessageType() == EMT_CHANGE_PLAYER_ICON)
	{
		MessageChangePlayerIcon* pMes = dynamic_cast<MessageChangePlayerIcon*>(message);
		ui::Button* pBtnHead = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnHead"));
#ifdef MGAME_PLATFORM_MPLUS
		//FriendSystem::setPlayerIconBtn(pBtnHead, sgUserObj()->getPlayerIconID(), sgMplusControl()->getThirdPartyIcon());
		FriendSystem::setPlayerIconBtn(pBtnHead, sgUserObj()->getPersonalInfo()->iconid(), sgMplusControl()->getThirdPartyIcon());
#else
		//FriendSystem::setPlayerIconBtn(pBtnHead, sgUserObj()->getPlayerIconID(), "");
		FriendSystem::setPlayerIconBtn(pBtnHead, sgUserObj()->getPersonalInfo()->iconid(), "");
#endif
	}
	else if(message->getMessageType() == EMT_CLOSE_PVP)
	{
		mRoot->addChild(sgChattingControl()->getChattingButton());
	}
	else if(message->getMessageType() == EMT_GUIDE_TIPS)
	{
		doGuideAction(message);
	}
	else if(message->getMessageType() == EMT_RQST_RESULT)
	{
		RqstResultMessage* pMsg = dynamic_cast<RqstResultMessage* >(message);
		if(pMsg->getID() == fgame::CS_CMD_SHOP_GUIDE && pMsg->getResult() == 0)
		{

			//GiftAwardDialog* pDialog = GiftAwardDialog::showDialogByGiftId(1);
			//pDialog->setbGuideCallBack(true);
		    playDiamondAni();
		} 
		//把CS_CMD_SHOP_GUIDE改成新的协议名字
		else if (pMsg->getID() == fgame::CS_CMD_GLYPH_GUIDE_ADDSTUFF && pMsg->getResult() == 0)
		{
			GiftAwardDialog* pDialog = GiftAwardDialog::showDialogByGiftId(999);
			pDialog->setbGuideCallBack(true, this, callfuncID_selector(SelectWorldPage::onGiftAniCallback));
		}
	}	
	else if (message->getMessageType() == EMT_BUY_VIP)
	{
		CCLOG("[trace]SelectWorldPage::onReceiveMassage EMT_BUY_VIP");
		MessageBuyVip * pMes = dynamic_cast<MessageBuyVip*>(message);
		resetVipIcon(pMes->getID());
	}
	else if (message->getMessageType() == EMT_PLAYER_UPGRADE)
	{
		CCLOG("[trace]SelectWorldPage::onReceiveMassage EMT_PLAYER_UPGRADE");
		if (CurGameScene == this)
		{
			sgPageFunction()->gotoFunction(MSG_FUC_PLAYER_UPGRADE);
		}
	}
	else if (message->getMessageType() == EMT_SELECTWORD_SCHEDULE)
	{
		removeCurIntelGuide(0);
		mAddTag = true;
		schedule(schedule_selector(SelectWorldPage::showIntelligentGuide), 10.0f);		
	}
	else if (message->getMessageType() == EMT_DALIY_MISSION_TIPS)
	{
		MessageDailyMissionTips* pMes = dynamic_cast<MessageDailyMissionTips*>(message);
		if (pMes->getTipType() == 1)
			playDailyMissionTip(this, pMes->getIsShow(), Point(pMes->getX(), pMes->getY()));
	}
	else if (message->getMessageType() == EMT_OPEN_DAILY_MISSION)
	{
		if (sgUserObj()->getIsOpenDailyMissionDialog() && Window::getShowingWindowSize() == 0)
		{
			if (!sgGuideManager()->getGuideLock())
			{
				sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "MissionDialog.csb", 1);
				sgUserObj()->setIsOpenDailyMissionDialog(false);
				sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", MSG_FUC_DAILY_MISSION, 1));
			}
			else
			{
				sgUserObj()->setIsOpenDailyMissionFirst(false);
			}

		}		
	}
	else if (message->getMessageType() == EMT_INVITE_BAT_SWITCH)
	{
		Size pSize = Director::getInstance()->getWinSize();
		auto pFullSrvInviteBatTips = sgChattingControl()->getFullSrvInviteBatTips();
		pFullSrvInviteBatTips->setPosition(pSize.width - 120, pSize.height - 240);
		mRoot->addChild(pFullSrvInviteBatTips);
	}
	else if (message->getMessageType() == EMT_HONOR_CHANGE)
	{
		MessageHonorChange* msg = dynamic_cast<MessageHonorChange*>(message);
		if (msg)
		{
			auto pTextHonor = dynamic_cast<Text*>(mRoot->getChildByName("Text_Honor"));
			pTextHonor->setString(StringUtils::format("%d", sgUserObj()->getPersonalInfo()->honorcount()));
		}
	}	
	else if (message->getMessageType() == EMT_GUIDE_FIRST)
	{
		MessageGuideFirst* pMsg = dynamic_cast<MessageGuideFirst*>(message);
		if (!pMsg)
			return; 
		doSkipGuideCamp(pMsg->getState());
	}
	
}
SelectWorldPage::~SelectWorldPage()
{
	CCLOG("[trace]SelectWorldPage::SelectWorldPage() unCtor execute");
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
	sgMessageManager()->removeMessageHandler(this);
	mIntelligentMapList.clear();
}

void SelectWorldPage::changeStarLevelNum()
{
	//废弃接口
	/*int starNum = sgUserObj()->getTotalStars();
	int allStar = sgTableManager()->getWolrdAllStar(1); 
	std::string str = StringUtils::format("%d/%d", starNum, allStar);
	*/
}

void SelectWorldPage::TextFieldCallback(Ref* pSender, TextFiledEventType type)
{
	if (type == TextFiledEventType::TEXTFIELD_EVENT_INSERT_TEXT)
	{
		//ui::TextField* mInputPanel = dynamic_cast<ui::TextField*>(pSender);
		//if (mInputPanel)
		//{
		//	 = mInputPanel->getStringValue();

		//	PlayerSaveData* tData = sgPlayerDataMgr()->getPlayerData();
		//	if (tData == NULL)
		//	{
		//		sgPlayerDataMgr()->updatePlayerDate(CCString::createWithFormat("%d", )->getCString(), 100, POWER_INIT_VALUE, 1010);
		//		sgPlayerDataMgr()->saveDataToFile();
		//	}
		//}
	}

}

void SelectWorldPage::onShow()
{
	sgSevenGoalControl()->setSevenGoalNodeState();

	sgPageFunction()->setCurrentUIType(MSG_FUC_MAIN_LEVEL);
	sgPageFunction()->setDiamondPanel(true,this);

	sgDayTaskControl()->checkShowTip();
	sgAchieveControl()->checkComplentTip();
	sgMailProxy()->checkMailTip();
	resetUIShow();
	resetTipShow(); 
	/*FullSrvInviteBatTips* pFullSrvInviteBatTips = FullSrvInviteBatTips::create();
	mRoot->addChild(pFullSrvInviteBatTips);
	Size pSize = Director::getInstance()->getWinSize();
	pFullSrvInviteBatTips->setPosition(pSize.width / 2 - 80, pSize.height / 4 - 20);*/

	Size pSize = Director::getInstance()->getWinSize();
	auto pFullSrvInviteBatTips = sgChattingControl()->getFullSrvInviteBatTips();
	pFullSrvInviteBatTips->setPosition(pSize.width - 120, pSize.height - 240);
	mRoot->addChild(pFullSrvInviteBatTips);
	//开始新手引导--选择大关
	doGuideNode();

	if (!sgGuideManager()->getGuideLock()
		&& !sgGuideManager()->getOpenRookieCamp()
		&& sgUserObj()->getPersonalInfo()->changenamecount() != 0)
		sgNoticeControl()->showActInform();

	if (sgUserObj()->getLvlUpgrade())
	{
		sgPageFunction()->gotoFunction(MSG_FUC_PLAYER_UPGRADE);
	} 

	if (showVIPTips == false && !sgGuideManager()->getGuideLock())
	{
		showVIPTips = true;
		//VIP到期提示
		int vipLevel = sgUserObj()->getPersonalInfo()->viplevel();//sgUserObj()->getVipLevel();
		if (vipLevel > 0 && vipLevel <= 3)
		{
			CVTimeSpan vipTime = sgUserObj()->getVipTime(vipLevel);
			if (vipTime.GetDays() == 0 && vipTime.GetHours() > 0)
			{
				sgPageFunction()->gotoFunction(MSG_FUC_VIP_TIME);
			}
		}
	}

	sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", MSG_FUC_DAILY_MISSION, sgUserObj()->getIsShowDailyMissionBtn()));
	auto dailyMissionGuid1Point = sgUserObj()->getDailyMissionTipPointByType(0);
	if (dailyMissionGuid1Point.x > 0.0)
		sgMessageManager()->sendMessage(EMT_DALIY_MISSION_TIPS, StringUtils::format("%d %d %f %f", 1, 1, dailyMissionGuid1Point.x, dailyMissionGuid1Point.y));
	else
		sgMessageManager()->sendMessage(EMT_DALIY_MISSION_TIPS, StringUtils::format("%d %d %f %f", 1, 0, 0.0, 0.0));

	sgMessageManager()->sendMessage(EMT_OPEN_DAILY_MISSION, "");

	sgTimeCalculator()->beganMainCountDown();


	//---------------------- sevenGoal code -----------------------------
	sgSevenGoalControl()->sendGetSevenGoalListRqst();
	changeSevenGoalPanelState(false);
}

void SelectWorldPage::onClose()
{
	GameScene::onClose();
	sgActControl()->setisCanAutoShow(false);

	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("eSevenGoalUpdate");
}

void SelectWorldPage::playFinish(Node *pSend)
{
	Button* temp_btn = dynamic_cast<Button*> (mRoot->getChildByName("BtnMenu"));
	Button* btn = dynamic_cast<Button*>(pSend);
	if (temp_btn->getScaleX() == -1.0)
	{
		btn->setVisible(false);
	}

	if (!temp_btn->isEnabled())
	{
		temp_btn->setEnabled(true);
	}
	btn->setEnabled(true);
}


void SelectWorldPage::playTipAction(ui::Widget* btn, bool isShow, int intelligentType)
{
	if (isShow)
	{
		if (!btn->getChildByTag(BUTTON_TIP_TAG))
		{
			cocostudio::Armature* pTipArm = cocostudio::Armature::create("fla_HongDian");
			pTipArm->getAnimation()->play("play");
			btn->addChild(pTipArm, 1, BUTTON_TIP_TAG);
			pTipArm->setPosition(btn->getContentSize().width - 15, btn->getContentSize().height - 15);
			//pTipArm->getAnimation()->play("play");
			if ((intelligentType != EIT_NONE) && (mIntelligentMapList.find(intelligentType) == mIntelligentMapList.end()))
			{
				auto intelligent = sgTableManager()->getIntelligent(intelligentType);
				if (intelligent!=nullptr && intelligent->isable == 1)
					mIntelligentMapList.insert(std::make_pair(intelligentType, btn));
			}				
		}

	}
	else
	{
		if (btn->getChildByTag(BUTTON_TIP_TAG))
		{
			btn->getChildByTag(BUTTON_TIP_TAG)->removeFromParent();
			IntelligentMapList::iterator intel = mIntelligentMapList.find(intelligentType);
			if (intel != mIntelligentMapList.end())
			{
				mIntelligentMapList.erase(intel);
			}
		}
	}
}

void SelectWorldPage::resetTipShow()
{
	ui::Widget* pBtn1 = dynamic_cast<ui::Widget*>(mRoot->getChildByName("PanelGift"));
	ui::Widget* pBtn2 = dynamic_cast<ui::Widget*>(mRoot->getChildByName("PanelWinner"));
	ui::Widget* pBtn3 = dynamic_cast<ui::Widget*>(mRoot->getChildByName("BtnNotice"));
	ui::Widget* pBtn4 = dynamic_cast<ui::Widget*>(mRoot->getChildByName("BtnTask"));
	ui::Widget* pBtn5 = dynamic_cast<ui::Widget*>(mRoot->getChildByName("BtnTeach"));
	//if (!sgNetManager()->IsConnect())
	//{
	//	playTipAction(pBtn1,true);
	//	playTipAction(pBtn2,true);
	//	return;
	//}

	if(sgWinnerControl()->getCanGet())
	{
		playTipAction(pBtn2,true, EIT_WINNER);
	}
	else
	{
		playTipAction(pBtn2,false, EIT_WINNER);
	}

	if(sgActControl()->checkCurrActCanGet())
	{
		playTipAction(pBtn1,true, EIT_GIFT);
	}
	else
	{
		playTipAction(pBtn1,false, EIT_GIFT);
	}
	if (!sgUserObj()->checkIsHaveAllNoticeData())
	{
		playTipAction(pBtn3, true, EIT_NONE);
	}
	else
	{
		playTipAction(pBtn3, false, EIT_NONE);
	}

	if (sgUserObj()->getIsShowDailyMissionHot())
		playTipAction(pBtn4, true, EIT_NONE);
	else
		playTipAction(pBtn4, false, EIT_NONE);
	if (sgUserObj()->getGateState(E_NEWCAMP_5) == E_CHECKPOINTSTATE_PASS)
		playTipAction(pBtn5, false, EIT_NONE);
	else
		playTipAction(pBtn5, true, EIT_NONE);
	resetVipIcon(sgUserObj()->getPersonalInfo()->viplevel());//sgUserObj()->getVipLevel());
	resetLevel(sgUserObj()->getPersonalInfo()->explevel());//sgUserObj()->getPlayerLevel());
}


bool SelectWorldPage::loadAsync(std::function<void()> loadCompleteCallback)
{
	GameScene::loadAsync(nullptr);
	sgResManager()->loadAsync();
	
	

	if (loadCompleteCallback)
		loadCompleteCallback();
	return true;
}

void SelectWorldPage::updatePkIcon(float dt)
{
	if (sgFriendControl()->getPkList().size() > 0)
	{
		if (!m_pPkIconImage->isVisible())
			m_pPkIconImage->setVisible(true);
	}
	else
	{
		if (m_pPkIconImage->isVisible())
			m_pPkIconImage->setVisible(false);
	}

	if (sgFriendControl()->getApplicationList().size() > 0)
	{
		if (m_pFriendButton->getChildrenCount() == 0 && !m_pPkIconImage->isVisible())
		{
			playTipAction(m_pFriendButton, true, EIT_FRIEND);
		}
	}
	else
	{
		if (m_pFriendButton->getChildrenCount() > 0)
		{
			playTipAction(m_pFriendButton, false, EIT_FRIEND);
		}
	}
}

void SelectWorldPage::pkIconCallback(cocos2d::Ref* sender)
{
	//GeneralDialogOther::hide();
	GenerlDialog::hide();
	//if (sgUserObj()->getPvpPower() < PVP_NEED_PSYCH)
	if (sgUserObj()->getPersonalInfo()->energy() < PVP_NEED_PSYCH)
	{
		// change by jim, unify tips ui
		//GeneralDialogOther::show(sgTextHelper(TEXT_PVP_TIPS_6), GeneralDialogOther::E_GENERAL_TYPE_CONFIRM, [this](Ref* sendr){
		//	GeneralDialogOther::hide();
		//		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
		//});
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_PVP_TIPS_6), [this](Ref* sendr){
			GenerlDialog::hide();
			PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
		}, false);
	}
	else
	{
		int size = sgFriendControl()->getPkList().size();
		if (size > 0)
		{
			FriendSystem::PlayerData* pData = sgFriendControl()->getPkPlayerData();
			sgFriendControl()->getProxy()->sendPkConfirmRequest(true, pData->m_iRoom, pData->m_uId);
		}
	}
}

void SelectWorldPage::removeUnusedTextures()
{
	CCTextureCache::getInstance()->removeTextureForKey("uiAtlas_Logo.png");
	CCTextureCache::getInstance()->removeTextureForKey("uiAtlas_Login_bg01.png");
}


void SelectWorldPage::initItemAni()
{
	mDiamondAni = cocostudio::Armature::create("fla_zhuanshi");
	Director::getInstance()->getNotificationNode()->addChild(mDiamondAni,INT_MAX);
	Size pSize = Director::getInstance()->getWinSize();
	mDiamondAni->setPosition(pSize.width/2 + 20, pSize.height/2 + 10);
	mDiamondAni->setVisible(false);

	 
}


void SelectWorldPage::doGuideAction(GameMessage* message)
{
	MessageGuideTips* pMsg = dynamic_cast<MessageGuideTips* >(message);
	if (!pMsg)
		return;
	if(pMsg->getFunctionID() == E_GuideFuc_AddMoney)
	{
		//发送请求新手增加钻石协议
		const ItemBuyInfo* pInfo = sgTableManager()->getItemBuyById(1); 
		sgUserObj()->sendAddGuideDiamond(pInfo->moneynum); 
	} 
	else if (pMsg->getFunctionID() == E_GuideFuc_AddGlyphStuff)
	{
		//发送请求新手材料协议
		sgUserObj()->sendAddGuideGlyphStuff();
	}
	else if (pMsg->getFunctionID() == E_GuideFuc_OpenUI)
	{
		//打开相关的UI
		openUI(pMsg->getFunctionParam());
	}
	else if (pMsg->getFunctionID() == E_GuideFuc_OpenLuaUI)
	{
		openUI(pMsg->getFunctionParam(), MSG_FUC_LUA_WIN);
	}
}

void SelectWorldPage::doSkipGuideCamp(int skipState)
{ 
	CCLOG("[trace]SelectWorldPage::doSkipGuideCamp skipState is %d", skipState);
	if (skipState == -1)
		return;
	
	//如果选择跳过训练营引导，则将该组引导强制完成
	sgGuideManager()->setGroupFinishByID(GuideGroup_SkipRookieCamp, false);
	if (skipState == 1)
	{  
		mCanTouch = false;
		sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "GuideNewCamp.csb", 1);
		return;
	}
	else if (skipState == 2)
	{
		std::map<int, bool> idMap;
		idMap.insert(std::make_pair(33, true));
		idMap.insert(std::make_pair(34, true));
		sgUserObj()->syncGuideFinish(idMap); 
	} 
	doGuideNode();
	mCanTouch = true;
}
 
void SelectWorldPage::doGuideNode(int condition)
{  
	if (sgGuideManager()->getOpenRookieCamp())
		return;
	int gateID = sgUserObj()->getMaxOpenGateID();
	int level = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel();
	sgGuideManager()->registGuideMessage(gateID, level, condition, mRoot, this, false);
	CCLOG("[trace]SelectWorldPage::doGuideNode level is %d", gateID);
}

void SelectWorldPage::playDiamondAni()
{
	if(!mDiamondAni)
		return;
	const ItemBuyInfo* pInfo = sgTableManager()->getItemBuyById(1);
	mDiamondAni->setVisible(true);
	mDiamondAni->stopAllActions();
	mDiamondAni->getAnimation()->play("born");
	mDiamondAni->getAnimation()->setMovementEventCallFunc(
		std::bind(&SelectWorldPage::onAniMovementCallback, 
		this, 
		std::placeholders::_1, 
		std::placeholders::_2, 
		std::placeholders::_3, 
		100, sgUserObj()->getPersonalInfo()->diamond()));// sgUserObj()->getGoldCoin()));
}

void SelectWorldPage::openUI(int uiID, int windowType)
{ 
	if (windowType == 0)
	{
		if (uiID == MSG_FUC_NAMEING_DIALOG)
		{
			if (sgUserObj()->getPersonalInfo()->changenamecount() == 0)
			{
				sgPageFunction()->gotoFunction(MSG_FUC_NAMEING_DIALOG, "NameingDialog.csb");
			}
			else
			{
				sgGuideManager()->setGroupFinishByID(GuideGroup_SkipRookieCamp);
				doGuideNode();
			}

		}
		else if (uiID == MSG_FUC_NEWER_BIBLE)
		{
			sgPageFunction()->gotoFunction(MSG_FUC_NEWER_BIBLE);
		}
	}
	else if (windowType == MSG_FUC_LUA_WIN)
	{ 
		if (uiID == 1025)
		{
			mCanTouch = false;
			sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "SkipNewDialog.csb", 1);
			return;
		} 
	}
	mCanTouch = true;
}

 
void SelectWorldPage::onGiftAniCallback(int id)
{
	sgGuideManager()->unRegistGuideMessage();
	doGuideNode();
}

void SelectWorldPage::onAniMovementCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName, int diamond, int curDiamond)
{
	if (pAr == mDiamondAni)
	{
		if (type == MovementEventType::COMPLETE)
		{
			if (aniName == "born")
			{
				mDiamondAni->getAnimation()->play("stand");
				mDiamondAni->getAnimation()->setMovementEventCallFunc(
					std::bind(&SelectWorldPage::onAniMovementCallback,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3, diamond, curDiamond));

				std::string param = StringUtils::format("%d %d %d %d %d %s"
					, 1
					, 1
					, eVituralCurrencyType::Diamond
					, 0
					, curDiamond
					, "ShopDialog::onDiamondMovementStart");

				//sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
			}
			else if (aniName == "end")
			{
				mDiamondAni->removeFromParent();
				mDiamondAni = NULL;
				std::string param = StringUtils::format("%d %d %d %d %d %s"
					, 1
					, 1
					, eVituralCurrencyType::Diamond
					, diamond
					, curDiamond
					, "ShopDialog::onDiamondMovementEnd");
				sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param); 
			}
		}
		else if (type == MovementEventType::LOOP_COMPLETE)
		{
			if (aniName == "stand")
			{
				mDiamondAni->getAnimation()->play("end");
				mDiamondAni->getAnimation()->setMovementEventCallFunc(
					std::bind(&SelectWorldPage::onAniMovementCallback,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					diamond, curDiamond));
			}
		}
	}
	 
}

void SelectWorldPage::resetVipIcon(int id)
{
	if (id <= 0 || id > 3)
	{
		mNameIcon->loadTextures("ui/main/main_playername.png","ui/main/main_playername.png","", ui::Widget::TextureResType::PLIST);
		mNameIcon->setContentSize(Size(139, 38));
		return;
	}
		
	//by edit
	string path =StringUtils::format("ui/general/general_vip_%d.png",id);
	mNameIcon->loadTextures(path,path,"", ui::Widget::TextureResType::PLIST);
	mNameIcon->setContentSize(Size(193, 42));

}

void SelectWorldPage::resetLevel(int level)
{
	mPlayerLevel->setString(StringUtils::format("%d", level));
}

void SelectWorldPage::reOrderActivityBtn()
{
	int checkPos = 0;
	for (int i = 0; i < ACTIVITY_BTN_COUNT; i++) {

		for (int j = checkPos; j < ACTIVITY_BTN_COUNT; j++) {
			if (mActivityUpBtn[j]->isVisible()) {
				mActivityUpBtn[j]->setPosition(mUpBtnPositions[i]);
				checkPos = j + 1;
				break;
			}
		}

		if (checkPos >= ACTIVITY_BTN_COUNT) {
			break;
		}
	}
}

void SelectWorldPage::reOrderDownBtn()
{
	int checkPos = 0;
	for (int i = 0; i < DOWN_BTN_COUNT; i++) {

		for (int j = checkPos; j < DOWN_BTN_COUNT; j++) {
			if (mDownBtn[j]->isVisible()) {
				mDownBtn[j]->setPosition(mDownBtnPositions[i]);
				checkPos = j + 1;
				break;
			}
		}

		if (checkPos >= DOWN_BTN_COUNT) {
			break;
		}
	}
}

void SelectWorldPage::checkInsituteFreeState()
{
	LuaEngine::getInstance()->executeString("sgInstituteControl():showFreeTipsState()");
}

void SelectWorldPage::showActNoticeDialog(bool isShow, int showType)
{
	if (mActNoticeNode){
		mActNoticeNode->setVisible(isShow);
	}

	mActType = showType;
	 
	if (mActNoticeTitle && mActIcon && isShow) {

		mActNoticeNode->setVisible(false);
		switch (mActType)
		{
		case ACT_FIRST_CHARGE: //
			mActNoticeTitle->setString(sgTextHelper(TEXT_FIRST_CHARGE));
			mActIcon->loadTexture("ui/main/main_first_charge.png", ui::Widget::TextureResType::PLIST);
			mActNoticeNode->setVisible(true);
			break;
		case ACT_RECHARGE:
			mActNoticeTitle->setString(sgTextHelper(TEXT_TOTAL_CHARGE));
			mActIcon->loadTexture("ui/main/main_fuli.png", ui::Widget::TextureResType::PLIST);
			mActNoticeNode->setVisible(true);
			break;
		default:
			break;
		}
	}
}

void SelectWorldPage::showIntelligentGuide(float t)
{
	CCLOG("SelectWorldPage::showIntelligentGuide");

	removeCurIntelGuide(1);

	if (!mAddTag)
		return;

	int type = 0;
	int priority = 0;
	ui::Widget* btn = NULL;
	IntelligentMapList::iterator it = mIntelligentMapList.begin();
	for (; it != mIntelligentMapList.end();++it)
	{
		int _type = it->first;
		auto intelligent = sgTableManager()->getIntelligent(_type);
		if (intelligent->priority > priority)
		{
			priority = intelligent->priority;
			type = _type;
			btn = it->second;
		}
	}

	if (btn != NULL)
	{
		addIntelligentGuide(btn);
		onLocateClickCallback(btn);
		//mCurIntelType = type;
	}

}

void SelectWorldPage::addIntelligentGuide(ui::Widget* btn)
{
	if (sgPageFunction()->getCurrentUIType() != MSG_FUC_MAIN_LEVEL)
		return;

	CCLOG("[trace][crystal][IntelligentGuide]...SelectWorldPage::addIntelligentGuide s");
	auto node = Node::create();
	btn->addChild(node, 1, BUTTON_INTELLIGENT_TIP);

	auto tip_img = ImageView::create("ui/main/qipao.png", TextureResType::PLIST);
	if (tip_img == NULL)
		return;
	tip_img->setTouchEnabled(true);
	tip_img->setCallbackType("Click");
	tip_img->setCallbackName(btn->getCallbackName());
	tip_img->setAnchorPoint(Point(0.0,0.0));
	tip_img->setPosition(Point(0.0,0.0));
	node->addChild(tip_img, 1);

	auto tip_armature = cocostudio::Armature::create("fla_qipaots_baozi");
	if (tip_armature == NULL)
		return;
	tip_armature->setScale(0.35);
	tip_armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(SelectWorldPage::intelGuideAniCallback));
	tip_armature->getAnimation()->play("stand",-1,0);

	node->addChild(tip_armature, 2);

	float _y = 15;
	if (btn->getPositionY() > Director::getInstance()->getVisibleSize().height / 2)
	{
		tip_img->setRotation(180);
		tip_armature->setPosition(Point(tip_img->getContentSize().width*(-1)*0.5, tip_img->getContentSize().height*(-1)*0.35));
	}
	else
	{
		tip_armature->setPosition(Point(tip_img->getContentSize().width*0.5, tip_img->getContentSize().height*0.78));
		_y = btn->getContentSize().height - _y;
	}	
	node->setPosition(Point(btn->getContentSize().width/2,_y));

	CCLOG("[trace][crystal][IntelligentGuide]...SelectWorldPage::addIntelligentGuide e");
}

// tag 0:点击开启别的界面事件取消tips 1:10s自动取消tips或点击不开启别的界面
void SelectWorldPage::removeCurIntelGuide(int tag)
{
	bool bSuccRemove = false;
	auto it = mIntelligentMapList.begin();
	for (; it != mIntelligentMapList.end();++it)
	{
		auto btn = it->second;
		Node* pTipNode = btn->getChildByTag(BUTTON_INTELLIGENT_TIP);
		if (pTipNode)
		{
			pTipNode->removeFromParent();
			bSuccRemove = true;
		}
	}

	if (bSuccRemove == true || tag == 0)
	{
		//mCurIntelType = 0;
		mAddTag = false;
		unschedule(schedule_selector(SelectWorldPage::showIntelligentGuide));
	}
}

void SelectWorldPage::intelGuideAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	static int _c = 1;
	if (type == cocostudio::MovementEventType::COMPLETE)
	{
		if ((aniName == "stand" && _c < 3) || (aniName != "stand"))
		{
			pAr->getAnimation()->play("stand",-1,0);
			_c++;
		}
		else
		{
			srand((unsigned)time(NULL));
			int index = rand() % 3;
			std::string playName = "";
			if (index == 0)
				playName = "blinka";
			else if (index == 1)
				playName = "blinkb";
			else
				playName = "blinkc";
			pAr->getAnimation()->play(playName,-1,0);
			_c = 1;
		}
	}
}

void SelectWorldPage::setPlayerIconVisible(bool flag)
{
	if (sgUserObj()->getPersonalInfo()->changenamecount() != 0)
	{
		if (!flag)
			return;
		else
			flag = true;
	}

	mRoot->getChildByName("BtnHead")->setVisible(flag);

	auto btn_name = dynamic_cast<ui::Button*>(mRoot->getChildByName("btn_name"));
	btn_name->setVisible(flag);
	btn_name->getChildByName("TextName")->setVisible(flag);

	auto btn_level = dynamic_cast<ui::Button*>(mRoot->getChildByName("btn_level"));
	btn_level->setVisible(flag);
	btn_level->getChildByName("text_level")->setVisible(flag);
}

void SelectWorldPage::playerIconFly()
{
	auto pBtnHead = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnHead"));
	pBtnHead->setVisible(true);
	ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_tubiaofei.png", "fla_tubiaofei.plist", "fla_tubiaofei.xml");
	auto flyAni = cocostudio::Armature::create("fla_tubiaofei");
	mIconPos = pBtnHead->getPosition();
	flyAni->getAnimation()->play("play");
	pBtnHead->addChild(flyAni, -1);
	flyAni->setPosition(Vec2(pBtnHead->getContentSize().width / 2, pBtnHead->getContentSize().height / 2));

	Size winSize = Director::getInstance()->getWinSize();
	pBtnHead->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	flyAni->setName("flyAni");
	flyAni->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(SelectWorldPage::flyAniMovementCallback));
}

void SelectWorldPage::flyAniMoveCallBack(Node* node)
{
	(dynamic_cast<cocostudio::Armature*>(node->getChildByName("flyAni")))->getAnimation()->play("play3");
}

void SelectWorldPage::flyAniMovementCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	//if (pAr->getName().compare("flyAni") == 0)
	//{
		if (type == MovementEventType::COMPLETE)
		{
			if (aniName == "play")
			{
				pAr->getAnimation()->play("play2");

				mRoot->getChildByName("BtnHead")->runAction(Sequence::create(MoveTo::create(0.5f, mIconPos), CallFuncN::create(CC_CALLBACK_1(SelectWorldPage::flyAniMoveCallBack, this)), NULL));
			}
			else if (aniName == "play3")
			{
				pAr->removeFromParent();
				setPlayerIconVisible(true);
				//执行改名之后的引导
				//doGuideNode(E_GUIDELOC_RENAMEFINISH); 
				openUI(1025, MSG_FUC_LUA_WIN);
			}
		}
	//}
}



//--------------------------- sevenGoal code  -----------------------------

void SelectWorldPage::updateSevenGoalEvent(cocos2d::EventCustom* customData, int skipState)
{

	mNowDay = sgSevenGoalControl()->getNowDay();
	mIsRetrived = sgSevenGoalControl()->getIsRetrived();
	mNowNum = sgSevenGoalControl()->getNowNum();
	mNextNum = sgSevenGoalControl()->getNextNum();

	if (mNowDay == 7 && mIsRetrived == true)
	{
		mButton_icon->setVisible(false);
		mImage_bg_SevenGoal->setVisible(false);
		return;
	}

	mButton_icon->setVisible(true);

	initSevenGoalInfo();
}

string SelectWorldPage::getSevenGoalInfoStr(string condition, int missionType, int para, int nowCount)
{
	string tmpStr = "";
	string tmp = StringUtils::format("%s%s", condition.c_str(), "  %d/%d");

	if (missionType == 5)
	{
		if (nowCount > 0)
		{

			tmpStr = StringUtils::format(tmp.c_str(), para, 1, 1);

		}
		else
		{
			tmpStr = StringUtils::format(tmp.c_str(), para, 0, 1);
		}
	}
	else
	{
		tmpStr = StringUtils::format(tmp.c_str(), para, nowCount, para);
	}


	return tmpStr;
}

void  SelectWorldPage::changeSevenGoalPanelState(bool bOpen)
{
	mHongdianAni->setVisible(false);

	if (true == bOpen)
	{
		mText_boxInfo->setVisible(false);
		mText_info->setVisible(false);
		mText_day->setVisible(false);
		mBoxAni->setVisible(false);
	}
	else
	{
		if (false == mbNowComplete && false == mIsRetrived)
		{
			//显示红点
			mHongdianAni->setVisible(true);
		}
	}

	mImage_bg_SevenGoal->setVisible(bOpen);
}

void SelectWorldPage::initSevenGoalInfo()
{
	Color3B textColor;
	string textBoxInfo = "";

	//设置描述、进度
	if (false == mIsRetrived)
	{
		mbNextComplete = false;

		//今天没领过, 显示今天的任务信息
		SevenDayMission * missionData = sgTableManager()->getSevenDayMissionByDay(mNowDay);

		if (NULL == missionData)
		{
			return;
		}

		//day
		string tmpStr = sgTextHelper(TEXT_SEVEN_GOAL_NOW_MISSION);
		mText_day->setString(tmpStr);

		//描述、进度
		tmpStr = getSevenGoalInfoStr(missionData->getCondition(), missionData->getType(), missionData->getPara(), mNowNum);
		mText_info->setString(tmpStr);

		//完成状态
		if (mNowNum >= missionData->getPara())
		{
			//完成
			textColor = TextcolorNowComplete;
			textBoxInfo = sgTextHelper(TEXT_SEVEN_GOAL_NOW_GET);
			mBoxAni->getAnimation()->play("step1", -1, 1);

			mPanel_pos->setTouchEnabled(true);
			mbNowComplete = true;
			changeSevenGoalPanelState(true);
		}
		else
		{
			//未完成
			textColor = TextcolorNowNormal;
			mBoxAni->getAnimation()->play("step3", -1, 1);	//xfx test

			mbNowComplete = false;
		}
	}
	else if (mNowDay < 7)
	{
		mbNowComplete = false;

		//今天领过了，显示明天的任务信息
		SevenDayMission * missionData = sgTableManager()->getSevenDayMissionByDay(mNowDay + 1);   //= sevenDayMissionTable[self.nowDay + 1]		//xfxmark 读表

		if (NULL == missionData)
		{
			return;
		}

		//day
		string tmpStr = sgTextHelper(TEXT_SEVEN_GOAL_NEXT_MISSION);
		mText_day->setString(tmpStr);

		//描述、进度
		tmpStr = getSevenGoalInfoStr(missionData->getCondition(), missionData->getType(), missionData->getPara(), mNextNum);
		mText_info->setString(tmpStr);

		//完成状态
		if (mNextNum >= missionData->getPara())
		{
			textBoxInfo = sgTextHelper(TEXT_SEVEN_GOAL_NEXT_GET);
			mBoxAni->getAnimation()->play("step1", -1, 1);
			mbNextComplete = true;
		}
		else
		{
			mBoxAni->getAnimation()->play("step3", -1, 1);
			mbNextComplete = false;
		}

		textColor = TextcolorNextNormal;
	}

	if (true == mIsRetrived)
	{
		//显示红点
		 mHongdianAni->setVisible(false);
	}

	mText_day->setColor(textColor);
	mText_info->setColor(textColor);
	mText_boxInfo->setString(textBoxInfo);

	mText_boxInfo->setVisible(true);
	mText_info->setVisible(true);
	mText_day->setVisible(true);
	mBoxAni->setVisible(true);
}














