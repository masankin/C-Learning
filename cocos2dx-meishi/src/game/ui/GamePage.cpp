#include "GamePage.h"
#include "game/ui//PageFunction.h"
#include "GamePrecedure.h"
#include "game/battle/BattleControl.h"
#include "core/HelperFunc.h"
#include "MessageManager.h"
#include "game/balance/BalanceControl.h"
#include "game/balance/StarModes.h"

#include "game/ui/HpSlider.h"
#include "game/prop/PropsControl.h"
#include "EnvSet.h"
#include "prop/PropItem.h"
#include "game/battle/Map/MapHandler.h"
#include "battle/Camp.h"
#include "balance/BalanceModes.h"
#include "login/LoginProxy.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"

#include "battle/MoraleView.h"

#include "guide/GuideManager.h"
#include "winner/WinnerControl.h"
#include "SoundManager.h"

#include "game/net/UserObj.h"
#include "system/ActionTip.h"

#include "battle/BattleRecordsManager.h"
#include "game/achieve/AchieveControl.h"
#include "dialogs/BoxRewardDialog.h"

#include "ui/dialogs/GiftAwardDialog.h"
#include "battle/Assistant/AssistantView.h"
#include "battle/Assistant/Assistant.h"
#include "core/ResManager.h"

#include "CCLuaEngine.h"
#define MAPLAYER_TAG 100025
#define STATU_BTN_TAG 10026 //状态栏是否观战

#define COLOR_BTN cocos2d::Color3B(238,58,58)

GamePage::GamePage() :mPercentBg100(0)
, mPercentBg50(0), mPercentBg75(0), mPercentBg25(0)
, mYanHua(0), mCheckBox(0), mPauseBtn(0), mGameReady(nullptr)
,mMsgPanel(0)
, m_spiritFont(nullptr)
, mSpirtDec(nullptr)
, m_spiritSprite(nullptr)
, m_spiritBg(nullptr)
, mLockPauseBtn(false)
, mPanelNoBat(0)
, mbShowNoItemAward(false)
{
	sgMessageManager()->regisiterMessageHandler(EMT_CHECKPOINT_END, this);
	sgMessageManager()->regisiterMessageHandler(EMT_STAR_STATU, this);
	sgMessageManager()->regisiterMessageHandler(EMT_GUIDE_TIPS, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CHECKPOINT_START, this);
	sgMessageManager()->regisiterMessageHandler(EMT_RESMODE_SCORE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_SIGNAL_CHANGE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_MENU_BTN, this);
	sgMessageManager()->regisiterMessageHandler(EMT_BATTLE_TIP, this);
	sgMessageManager()->regisiterMessageHandler(EMT_BUILD_DESTORY, this);
	sgMessageManager()->regisiterMessageHandler(EMT_GAMESTART, this);
	sgMessageManager()->regisiterMessageHandler(EMT_LITTLETIPS, this);
	sgMessageManager()->regisiterMessageHandler(EMT_SIMPLIFY_UI, this);
	sgMessageManager()->regisiterMessageHandler(EMT_BUTTON_OUT_PERCENT, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PVPRESULT_OPENBOX, this);
	sgMessageManager()->regisiterMessageHandler(EMT_DAILY_MISSION_PROGRESS_BACK, this);
}


GamePage::~GamePage()
{
	sgMessageManager()->removeMessageHandler(this);
	sgPropsControl()->setInfomTextBottom(NULL);
	sgPropsControl()->setInfomTextHead(NULL);
	sgPropsControl()->setCancelBtn(NULL);
}

void GamePage::onMenuItemAction(std::string cbName, cocos2d::Ref* psender)
{
	int percent = 0;

	if(sgGuideManager()->getGuideLock() == true)
	{ 
		resetOutPercentBg();
		return;
	}
	if (mMsgPanel)
		mMsgPanel->setVisible(false);

	if (cbName == "onMenu")
	{
		PageFunction::Get()->gotoFunction(MSG_FUC_MAIN_LEVEL);
	}
	else if (cbName == "onPercent100")
	{
		sgBattleControl()->setOutputPercent(100);
		changeOutPercentBg(100);
		percent = 100;
	}
	else if (cbName == "onPercent75")
	{
		sgBattleControl()->setOutputPercent(75);
		changeOutPercentBg(75);
		percent = 75;
	}
	else if (cbName == "onPercent50")
	{
		sgGuideManager()->unRegistGuideMessage();
		sgBattleControl()->setOutputPercent(50);
		changeOutPercentBg(50);
		percent = 50;
	}
	else if (cbName == "onPercent25")
	{
		sgBattleControl()->setOutputPercent(25);
		changeOutPercentBg(25);
		percent = 25;
	}
	else if (cbName == "onPause")
	{
		if (sgGuideManager()->getIsRunning() || mLockPauseBtn == true)
		{
			return;
		}
		if (!sgBattleControl()->getIsPvp())
		{
			if (sgBattleRecordsManager()->getIsPlayback())
			{
				GenerlDialog::show("", sgTextHelper(TEXT_EXIT_PLAYBACK), CC_CALLBACK_1(GamePage::onCallBackExitPlayback, this), CC_CALLBACK_1(GamePage::onCallBackContinue, this));
			}
			else
			{
				PageFunction::Get()->gotoFunction(MSG_FUC_WIN_PAUSE);
			}
			sgGamePrecedure()->setGameIng(false);
			if (mLittleTips != NULL)
				mLittleTips->pauseSchedulerAndActions();
			
		}
		else
		{
			if (sgPvpControl()->getPvpStatus() == PVP_STATUS_C_END || sgPvpControl()->getPvpStatus() == PVP_STATUS_S_END)
				return;

			sgUserObj()->saveToFile();
			if (sgPvpControl()->getIsDead() && sgPvpControl()->getPVPMode() != PVP_MODE_NOITEM)
			{
				//上报每日任务相关数据
				sgUserObj()->sendDailyMissionData(true);
				mOnMenuStr = "backWinPvp";
			}
			else
			{
				float allTime = 900.0f;
				float fGameTime = sgBalanceControl()->getTimeLeft();

				const CheckPoint* pCheck = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);
				if (pCheck)
				{
					allTime = pCheck->timeLimit;
				}
				mbNoItemLess2Min = false;
				if (fGameTime > (allTime - 120) /*&& sgPvpControl()->getPVPMode() != PVP_MODE_NOITEM*/)
				{
					if (sgPvpControl()->getPVPMode() == PVP_MODE_NOITEM)
						mbNoItemLess2Min = true;
					GenerlDialog::show("", sgTextHelper(TEXT_BUY_ITEM_QIUCK_CLOSE_PVP), CC_CALLBACK_1(GamePage::onCallBackshowShop, this));
					
				}
				else
				{
					GenerlDialog::show("", sgTextHelper(TEXT_BUY_ITEM_CLOSE_PVP), CC_CALLBACK_1(GamePage::onCallBackshowShop, this));
				}
			}


		}
		
		setOpacityAction(dynamic_cast<Node*>(psender));

	}
	else if (cbName == "onShowText")
	{
		if (mLevelNode)
		{
			bool bOpen = !mLevelNode->isVisible();
			mLevelNode->setVisible(bOpen);
			setOpacityAction(mLevelNode);
			sgUserObj()->changeGamePassTip(bOpen);
//#ifdef MGAME_PLATFORM_MPLUS
//			sgUserObj()->changeGamePassTip(bOpen);
//#endif // DEBUG

		}

		setOpacityAction(dynamic_cast<Node*>(psender));
		//sgGamePrecedure()->onFailGame();
	}
	else if (cbName == "onAddProp")
	{
		sgPropsControl()->setProp(ePropType::PROP_TYPE_FREEZE);
	}
	else if (cbName == "onPvpText")
	{
		sgPvpControl()->setLockText(!sgPvpControl()->getLockText());
	}
	else if (cbName == "onPropCancel")
	{
		sgPropsControl()->propEvetCancel();
	}
	else if(cbName == "onChat")
	{
		if (!mMsgPanel)
		{
			mMsgPanel = MsgPanel::create(Fight_Chat);
			this->addChild(mMsgPanel,100);
			sgBattleControl()->getBattleView()->setChatNode(mMsgPanel);
		}
		mMsgPanel->setVisible(true);
	}
	else if (cbName == "onSpirit")
	{
		if (mSpirtDec)
		{
			mSpirtDec->stopAllActions();
			mSpirtDec->setOpacity(0);
			mSpirtDec->runAction(Sequence::create(FadeIn::create(0.3f)
				,DelayTime::create(1.0f)
				, FadeOut::create(2.0f), nullptr));
		}
	}
	
	//记录百分比
	if (!sgBattleRecordsManager()->getIsPlayback() && percent != 0)
	{
		char buff[32];
		memset(buff, 0, 32);
		sprintf(buff, "%d", percent);

		sgMessageManager()->sendMessage(EMT_RECORD_OUT_PERCENT, buff);
	}
}


void GamePage::resetGameUi()
{

	HpSlider * pHp = HpSlider::create();	

	mSlider->removeAllChildren();
	mSlider->addChild(pHp);


	//Slider* slider = dynamic_cast<Slider*> (mRoot->getChildByName("mSlider"));
	sgBattleControl()->getBattleView()->setSlider(pHp);

	if (mLevelNode)
		mLevelNode->setVisible(false);

	//resetStarStr(pCheck);
	Node* contaner = mRoot->getChildByName("mGameLayer");
	contaner->removeAllChildren();
	contaner->addChild(sgBattleControl()->getBattleView());

	sgBattleControl()->getBattleView()->setTimeFont(m_timeFont);
	sgBattleControl()->getBattleView()->setSpiritFont(m_spiritFont);
	sgBattleControl()->getBattleView()->setTimeSprite(m_timeSprite);
	m_timeSprite->setPercent(100);


	if (mYanHua)
	{
		mYanHua->getAnimation()->stop();
		mYanHua->setVisible(false);
	}

	mSingle_Tip->setVisible(false);
	mSingleText->setVisible(sgBattleControl()->getIsPvp());
	mSpriteSingle->setVisible(sgBattleControl()->getIsPvp());
	if (sgBattleControl()->getIsPvp())
	{
		sgMessageManager()->sendMessage(EMT_SIGNAL_CHANGE, "1");
	}

	bool flag = true;	
	if (sgBattleControl()->getIsPvp())
	{
		mPauseBtn->loadTexturePressed("ui/Pause/Pause_baiqi_1.png", TextureResType::PLIST);
		mPauseBtn->loadTextureNormal("ui/Pause/Pause_baiqi.png", TextureResType::PLIST);
		mRoot->getChildByName("Button_Chat")->setVisible(true);
		if (sgPvpControl()->getPVPMode() == PVP_MODE_NOITEM)
		{
			flag = false;
		}
	}
	else
	{
		if (sgBattleRecordsManager()->getIsPlayback())
		{
			mPauseBtn->loadTexturePressed("ui/Pause/Pause_fail.png", TextureResType::PLIST);
			mPauseBtn->loadTextureNormal("ui/Pause/Pause_fail.png", TextureResType::PLIST);
			mPercentBg100->setTouchEnabled(false);
			mPercentBg75->setTouchEnabled(false);
			mPercentBg50->setTouchEnabled(false);
			mPercentBg25->setTouchEnabled(false);
		}
		else
		{
			mPauseBtn->loadTextureNormal("ui/battle/Battle_uiAtlas_Battle_button_click.png", TextureResType::PLIST);
			mPauseBtn->loadTexturePressed("ui/battle/Battle_uiAtlas_Battle_button_click.png", TextureResType::PLIST);
			mPercentBg100->setTouchEnabled(true);
			mPercentBg75->setTouchEnabled(true);
			mPercentBg50->setTouchEnabled(true);
			mPercentBg25->setTouchEnabled(true);
		}
		mRoot->getChildByName("Button_Chat")->setVisible(false);
		//if (sgGamePrecedure()->getCurMapInfo().checkpt > sgEnvSet("PVE_CHAT_LEVEL") && sgGamePrecedure()->getCurMapInfo().checkpt < E_NEWCAMP_1)
		//{
			//mRoot->addChild(sgChattingControl()->getChattingButton(true));
		//}
	}

	showPropItemNode(flag);

	changeOutPercentBg(sgBattleControl()->getOutputPercent());

	mBattleTipsLayer->removeAllChildren();

}

void GamePage::changeOutPercentBg(int percent)
{
	mCurPercentage = percent;

	mPercentBg25->setBrightStyle(mPercentBg25->getTag() == percent ? BrightStyle::NORMAL : BrightStyle::HIGHLIGHT);
	mPercentBg50->setBrightStyle(mPercentBg50->getTag() == percent ? BrightStyle::NORMAL : BrightStyle::HIGHLIGHT);
	mPercentBg75->setBrightStyle(mPercentBg75->getTag() == percent ? BrightStyle::NORMAL : BrightStyle::HIGHLIGHT);
	mPercentBg100->setBrightStyle(mPercentBg100->getTag() == percent ? BrightStyle::NORMAL : BrightStyle::HIGHLIGHT); 

	SliderBtnOpaAction();
}


void GamePage::resetOutPercentBg()
{
	mPercentBg25->setBrightStyle(mPercentBg25->getTag() == mCurPercentage ? BrightStyle::NORMAL : BrightStyle::HIGHLIGHT);
	mPercentBg50->setBrightStyle(mPercentBg50->getTag() == mCurPercentage ? BrightStyle::NORMAL : BrightStyle::HIGHLIGHT);
	mPercentBg75->setBrightStyle(mPercentBg75->getTag() == mCurPercentage ? BrightStyle::NORMAL : BrightStyle::HIGHLIGHT);
	mPercentBg100->setBrightStyle(mPercentBg100->getTag() == mCurPercentage ? BrightStyle::NORMAL : BrightStyle::HIGHLIGHT);
}

bool GamePage::loadAsync(std::function<void()> loadCompleteCallback)
{
	GameScene::loadAsync(nullptr);

	int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
	int curPower = sgUserObj()->getPersonalInfo()->power(); //sgUserObj()->getPvePower();
	sgResManager()->loadAsync();

	{
		if (!sgGamePrecedure()->preEnterGame())
			//失败
			return false;

		if (loadCompleteCallback)
		{
			loadCompleteCallback();
		}
		//sgResourceManager()->loadResources([loadCompleteCallback](const std::string& sFile, float percent)
		//	{
		//		if (percent>=1.0f)
		//		{
		//			loadCompleteCallback();
		//		}
		//	});
	}
	sgBattleControl()->init();
	//重新设置ui;
	this->resetGameUi();
	sgGamePrecedure()->enterGame();
	this->resetStarStr();


	sgSoundManager()->playMusicBg(checkpt);

	sgPropsControl()->init();

	setOpacityAction(mCheckBox);
	setOpacityAction(mPauseBtn);
	mLockPauseBtn = false;

	return true;
}

void GamePage::loadOver()
{
	const CheckPoint* pCheck = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);
	if (pCheck)
	{
		int id = pCheck->checkType + 100;
		const TextHelper* pText = sgTableManager()->getTextHelperByID(id);
		if (pText && mPassText)
		{
			mPassText->setText(pText->text);
			mPassText->setVisible(true);
			mPassText->stopAllActions();
			mPassText->setOpacity(225);
			FadeOut* pAction = FadeOut::create(10);
			mPassText->runAction(pAction);

		}

		if (pCheck->timeLimit <= 0)
		{
			m_timeSprite->setVisible(false);
			m_timeFont->setVisible(false);
			mPanelNoBat->getChildByName("Image_1")->setVisible(false);
		}
		else
		{
			m_timeSprite->setVisible(true);
			m_timeFont->setVisible(true);
			mPanelNoBat->getChildByName("Image_1")->setVisible(true);
			sgBattleControl()->getBattleView()->setTimeTotal(pCheck->timeLimit);

		} 

		const GuideData* pData = sgGuideManager()->getGuideDataByGuideID(GuideID_Glyph);
		if (pData)
		{
			int iGLevel = pData->getLevel();
			int iPlevel = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel();
			m_spiritBg->setVisible(iPlevel >= iGLevel);
			m_spiritFont->setVisible(iPlevel >= iGLevel);
			m_spiritSprite->setVisible(iPlevel >= iGLevel);
		}


	}

	sgBattleControl()->setOutputPercent(100);
	changeOutPercentBg(100);       

	if (pCheck->levelType == E_Level_Trans)
		mPauseBtn->setVisible(false);
	initGuidePropBar();
	mGameReady->onShow();
}

void GamePage::unload()
{
	sgGamePrecedure()->preEndGame();

}

void GamePage::onEnter()
{
	GameScene::onEnter();

	int id = sgGamePrecedure()->getCurMapInfo().mapId;	
	 
	

	auto tag1 = sgBattleControl()->getIsPvp();
	int tag2 = (id % 1000)/10;
	int tag3 = id / 1000;

	mLittleTips = NULL;

	if (!sgGuideManager()->checkHaveGateGuide(id))
	{
		if (!tag1 && ((tag2 < TIPS_UP_CONTROL_ACOUT) || (tag2 == TIPS_UP_CONTROL_ACOUT)) && tag3 == 1)//提示出现条件
		{
			auto visibleSize = Director::getInstance()->getVisibleSize();
			mLittleTips = LittleTips::create(LittleTips::EDT::EDT_LEFT);
			mLittleTips->setPosition(Point(0.0, 0.0));
			addChild(mLittleTips);
		}
		else if (!tag1)
		{
			if (sgGamePrecedure()->getCurMapInfo().checkpt > sgEnvSet("PVE_CHAT_LEVEL") && sgGamePrecedure()->getCurMapInfo().checkpt < E_NEWCAMP_1)
			{
				mRoot->addChild(sgChattingControl()->getChattingButton(true));
			}
		}
	}
	 

	int gateID = sgGamePrecedure()->getCurMapInfo().checkpt; 
	bool isShowSoldierControlFlag = (sgBattleControl()->getIsPvp() ? true : sgGuideManager()->checkOpenSoldierControl(gateID)); 
	
	showSoldierControl(isShowSoldierControlFlag);
}

void GamePage::onExit()
{
	GameScene::onExit();
	if (mLittleTips != NULL)
	{
		mLittleTips->removeFromParent();
		mLittleTips = NULL;
	}
	sgUserObj()->saveToFile();
}

void GamePage::onShow()
{
	sgPageFunction()->setCurrentUIType(MSG_FUC_PLAYING);
	//sgPageFunction()->setDiamondPanel(false,this);
	sgAchieveControl()->setIsPvpFight(true);
	
	sgSoundManager()->beganBattleEffect();

	mbShowNoItemAward = false;
	showNoBatControl(true);

	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("gDoneScene");
}

bool GamePage::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/GamePage.csb");
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		for (int i = 1; i <= 4; i++)
		{
			char pName[40];
			sprintf(pName, "mPlayer%d", i);
			Node* playerNode = mRoot->getChildByName(pName);
			playerNode->stopAllActions();
			playerNode->setVisible(false);
			playerNode->setScaleY(0.8);
			playerNode->setScaleX(mRoot->getChildByName(pName)->getScaleX()*0.8);
			mPlayer.push_back(playerNode);
		}
		mGlyphInfoNode = CSLoader::getInstance()->createNode("ui/PvpGlyphInfo.csb");
		addChild(mGlyphInfoNode,500);
		mGlyphInfoNode->setVisible(false);

		mPanelNoBat = dynamic_cast<ui::Layout*>(mRoot->getChildByName("PanelNoBat"));
		Node* panl = mPanelNoBat->getChildByName("uiAtlas_Battle_jindutiaobg_08_2_1");
		//panl->setGlobalZOrder(1000);

		mCheckBox = dynamic_cast<ui::CheckBox*>(mPanelNoBat->getChildByName("mCheckBox"));

		mPassText = dynamic_cast<ui::Text*>(mPanelNoBat->getChildByName("mPassText"));

		mLevelNode = mPanelNoBat->getChildByName("mLevelNode");

		mSingleText = dynamic_cast<ui::Text*>(mPanelNoBat->getChildByName("mSingleText"));
		mSpriteSingle = dynamic_cast<Sprite*>(mPanelNoBat->getChildByName("SpriteSingle"));
		mSingle_Tip = dynamic_cast<ui::Text*>(mPanelNoBat->getChildByName("mSingle_Tip"));


		mPercentBg25 = dynamic_cast<ui::Button*>(mPanelNoBat->getChildByName("mSliderBtn_1"));
		mPercentBg25->setTag(25);
		mPercentBg50 = dynamic_cast<ui::Button*>(mPanelNoBat->getChildByName("mSliderBtn_2"));
		mPercentBg50->setTag(50);
		mPercentBg75 = dynamic_cast<ui::Button*>(mPanelNoBat->getChildByName("mSliderBtn_3"));
		mPercentBg75->setTag(75);
		mPercentBg100 = dynamic_cast<ui::Button*>(mPanelNoBat->getChildByName("mSliderBtn_4"));
		mPercentBg100->setTag(100);

		mPercentBg25->setPressedActionEnabled(false);
		mPercentBg50->setPressedActionEnabled(false);
		mPercentBg75->setPressedActionEnabled(false);
		mPercentBg100->setPressedActionEnabled(false);

		//mLabel100 = dynamic_cast<ui::Text*>(mPanelNoBat->getChildByName("mLabel100"));
		//mLabel75 = dynamic_cast<ui::Text*>(mPanelNoBat->getChildByName("mLabel75"));
		//mLabel50 = dynamic_cast<ui::Text*>(mPanelNoBat->getChildByName("mLabel50"));
		//mLabel25 = dynamic_cast<ui::Text*>(mPanelNoBat->getChildByName("mLabel25"));


		m_timeSprite = dynamic_cast<LoadingBar*>(mPanelNoBat->getChildByName("LoadingBar_1"));
		m_timeFont = dynamic_cast<Text*>(mPanelNoBat->getChildByName("mLevelLabel3_0"));

		m_spiritSprite = dynamic_cast<LoadingBar*>(mPanelNoBat->getChildByName("mSpirit_icon"));
		m_spiritFont = dynamic_cast<Text*>(mPanelNoBat->getChildByName("mSpirit_label"));
		m_spiritBg = mPanelNoBat->getChildByName("mSpirit_bg");

		mSlider = mPanelNoBat->getChildByName("mSlider");
		mPauseBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("mPauseBtn"));
		mSpirtDec = dynamic_cast<ui::ImageView*>(mPanelNoBat->getChildByName("mSpirt_Dec"));
		mSpirtDec->setOpacity(0);
		dynamic_cast<ui::Text*>(mSpirtDec->getChildByName("Text_1"))->setText(sgTextHelper(TEXT_COMMTIPS_GLYPH_TIP));
		
		
		//mPercentSlider = dynamic_cast<ui::Slider*>(mRoot->getChildByName("Slider_1"));
		//mPercentSlider->setPercent(20);
		//mPercentSlider->addEventListenerSlider(this,sliderpercentchangedselector(GamePage::valueChanged));
		//mSliderLabel = dynamic_cast<ui::Text*>(mRoot->getChildByName("mSliderLabel"));


		//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_UIeffect2.xml");
		mYanHua = sgResManager()->getAramatur("fla_yanhua");//cocostudio::Armature::create("fla_yanhua");
		if (mYanHua)
		{
			/*mYanHua->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID){
				if (movementType == cocostudio::COMPLETE)
				{
				armature->setBlendFunc(BlendFunc::ADDITIVE);
				armature->stopAllActions();
				sgPageFunction()->gotoFunction(MSG_FUC_WIN_RESULT);
				}

				});*/
			//mYanHua->getAnimation()->playByIndex(0, -1, 1);
			mYanHua->setBlendFunc(BlendFunc::ADDITIVE);
			mYanHua->getAnimation()->setFrameEventCallFunc([this](cocostudio::Bone *bone, const std::string& frameEventName, int originFrameIndex, int currentFrameIndex){
				if (frameEventName == "atk")
				{
					if (sgBattleControl()->getIsPvp())
					{
						sgPageFunction()->gotoFunction(MSG_FUC_PVP_RESULT);
						if (sgPvpControl()->getPVPMode() == PVP_MODE_NOITEM)
						{
							showNoBatControl(false);
							this->scheduleOnce(schedule_selector(GamePage::showNoItemBoxNode), 1.5f);
						}
					}
					else
					{
						if (sgBattleRecordsManager()->getIsPlayback())
						{
							sgPageFunction()->gotoFunction(MSG_FUC_WIN_RESULT);
						}
						else
						{
							//新手训练营pve结算不显示
							int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
							const CheckPoint* pCP = sgTableManager()->getCheckPointByID(checkpt);
							if (pCP)
							{
								int levelType = pCP->levelType;
								if (levelType == E_Level_Trans)
								{
									Layout* layout = Layout::create();
									layout->setSize(Director::getInstance()->getWinSize());
									this->addChild(layout,10000);
									//胜利者建筑播放动画,pve
									//eCampType eWinCamp = sgBattleControl()->getOwnCamp();
									//sgBattleControl()->playPvpCampWinAni(eWinCamp);
									layout->setOpacity(0);
									layout->setBackGroundColorType(LAYOUT_COLOR_SOLID);
									layout->setBackGroundColor(Color3B::BLACK);
									layout->setBackGroundColorOpacity(180);
									layout->runAction(Sequence::create(
									FadeIn::create(1.2f),
									CallFunc::create(
									[this]()
									{
										sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
										if (E_CHECKPOINTSTATE_PASS == sgUserObj()->getGateState(99010) && sgUserObj()->getPersonalInfo()->changenamecount() != 0)
										{
											sgGuideManager()->setOpenRookieCamp(true);
											sgPageFunction()->gotoFunction(MSG_FUC_NEWER_BIBLE);
										}
										else
											sgGuideManager()->setOpenRookieCamp(false); 
										sgUserObj()->sendUpdateCheckPoint();
									}),
									RemoveSelf::create(true),

									NULL));
								}
								else
									sgUserObj()->sendUpdateCheckPoint();
							}
							
						}
					}
					sgBattleControl()->setCurPause(true);
				}

			});

			mYanHua->setPosition(Director::getInstance()->getVisibleSize() / 2.0f);

			layer_ui->addChild(mYanHua, 10);

		}

		for (int i = 1; i <= 3; i++)
		{
			PropItem* pPropItem = sgPropsControl()->getPropItem((ePropPos)i);
			Node* pNode = mPanelNoBat->getChildByName(__String::createWithFormat("prop_item%d", i)->getCString());
			if (pPropItem)
			{
				pPropItem->setScale(0.8f);
				pNode->addChild(pPropItem);
			}
		}

		ui::Text* pPropInformText1 = dynamic_cast<ui::Text*>(mPanelNoBat->getChildByName("mPropInformText1"));
		sgPropsControl()->setInfomTextHead(pPropInformText1);

		ui::Text* pPropInformText2 = dynamic_cast<ui::Text*>(mPanelNoBat->getChildByName("mPropInformText2"));
		sgPropsControl()->setInfomTextBottom(pPropInformText2);

		ui::Button* pCancel = dynamic_cast<ui::Button*>(mPanelNoBat->getChildByName("mCancelBtn"));
		pCancel->setVisible(false);
		sgPropsControl()->setCancelBtn(pCancel);

		mGameReady = GameReady::create();
		mRoot->addChild(mGameReady);

		ui::Text* pText = dynamic_cast<ui::Text*>(mPanelNoBat->getChildByName("m_pvp_test"));

		if (EnvSet::leon())
		{
			Size size = Director::getInstance()->getWinSize();
			if (pText)
			{
				pText->setVisible(sgBattleControl()->getIsPvp());
				sgPvpControl()->setPvpTest(pText);
			}
		}else{
			if (pText)
			{
				pText->setVisible(false);
			}
		}

		mBattleTipsLayer = Node::create();
		addChild(mBattleTipsLayer, 20);

		mClickAnimation = sgResManager()->getAramatur("fla_yingdao");//Armature::create("fla_yingdao");
		mClickAnimation->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(GamePage::animationCallback));
		mClickAnimation->setScaleX(-1);
		mClickAnimation->setVisible(false);
		mRoot->addChild(mClickAnimation);
		mOnMenuStr = "";
		mCurPercentage = 100;

		ret = true;
	} while (0);


	return ret;
}

void GamePage::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_CHECKPOINT_END)
	{
		mLockPauseBtn = true;

		MessageCheckPointEnd* pMes = dynamic_cast<MessageCheckPointEnd*>(message);

		if (pMes->mReason != "return")
		{
			sgSoundManager()->stopAllMusicEffects();
			sgSoundManager()->stopBattleEffect();
		}

		if (pMes->mSuccess == 0)
		{
			if (sgBattleControl()->getIsPvp())
			{
				sgPageFunction()->gotoFunction(MSG_FUC_PVP_RESULT);
				if (sgPvpControl()->getPVPMode() == PVP_MODE_NOITEM)
				{
					showNoBatControl(false);
					this->scheduleOnce(schedule_selector(GamePage::showNoItemBoxNode), 1.5f);
				}
			}
			else
			{

				if (pMes->mReason == "return")
				{
					//上报每日任务数据 pve点击暂停返回选关界面
					sgUserObj()->sendDailyMissionData(false);
					mOnMenuStr = "return";
				}
				else{

				}
			}
		}
		else
		{
			//sgPageFunction()->gotoFunction(MSG_FUC_WIN_RESULT);
			if (mYanHua)
			{
				mYanHua->setVisible(true);
				mYanHua->getAnimation()->playWithIndex(0, -1, 0);
			}

			sgSoundManager()->setBackgroundMusicVolume(0.1f);
			sgSoundManager()->playMusicEffect(SOUND_UI_WIN);
			
		}
	}
	else if(message->getMessageType() == EMT_BUILD_DESTORY)

	{   
		if (!sgGuideManager()->checkGuideTypeFinished(E_GUIDELOC_OCU1))
			activeGuide(E_GUIDELOC_OCU1);
	}
	else if (message->getMessageType() == EMT_STAR_STATU)
	{
		MessageStarStatu* pMes = dynamic_cast<MessageStarStatu*>(message);
		if (pMes->mStatu != -1)
			changeStarStr(pMes->mTag);
	}
	else if (message->getMessageType() == EMT_GUIDE_TIPS)
	{
		MessageGuideTips* pMes = dynamic_cast<MessageGuideTips* >(message);
		
		doGuideExtraFunc(pMes);
	}
	else if (message->getMessageType() == EMT_CHECKPOINT_START)
	{
		initResModeScoreUI();
	}
	else if (message->getMessageType() == EMT_RESMODE_SCORE)
	{
		updateResModeScore();
	}
	else if (message->getMessageType() == EMT_SIGNAL_CHANGE)
	{
		mSingleText->setText(__String::createWithFormat("%d", sgLoginProxy()->getCurrSignalMs())->getCString());
		MessageSignalChange* pMes = dynamic_cast<MessageSignalChange*>(message);
		if (pMes->mResult == 1)
		{
			const char* spr = __String::createWithFormat("ui/battle/PVP_Signal_%d.png", sgLoginProxy()->getCurrSignal())->getCString();
			mSpriteSingle->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(spr));

			if (sgLoginProxy()->getCurrSignal() < 2)
			{
				mSingle_Tip->setVisible(true);
				mSingle_Tip->setOpacity(255.0);
				mSingle_Tip->stopAllActions();
				mSingle_Tip->runAction(Sequence::create(
					DelayTime::create(3.0),
					FadeOut::create(2.0), NULL));

			}
			if (sgLoginProxy()->getCurrSignal() == 0)
			{
				mSpriteSingle->getChildByName("SpriteSingle_1")->setVisible(true);
			}
			else
			{
				mSpriteSingle->getChildByName("SpriteSingle_1")->setVisible(false);
			}
		}
	}
	else if (message->getMessageType() == EMT_MENU_BTN)
	{
		if (mPauseBtn)
		{
			mPauseBtn->loadTexturePressed("ui/Pause/Pause_fail.png", TextureResType::PLIST);
			mPauseBtn->loadTextureNormal("ui/Pause/Pause_fail.png", TextureResType::PLIST);
		}

	}
	else if (message->getMessageType() == EMT_BATTLE_TIP)
	{
		MessageBattleTips* pMsg = dynamic_cast<MessageBattleTips*>(message);

		//加入战斗提示 by edward 2015-09-23
		Action* pAction = Sequence::create(
			MoveBy::create(.3f, Vec2(0, 50)),
			DelayTime::create(1.0f),
			Spawn::create(MoveBy::create(.2f, Vec2(0, 30)), FadeOut::create(.2f), NULL),
			NULL);
		ActionTip* pBattleTip = ActionTip::create("", pAction);
		switch (pMsg->getBattleTipMsg())
		{
			case E_COMPLETE_UPGRADE_BARRACK:
			{
				pBattleTip->addText(sgTextHelper(TEXT_BATTLE_POPULATION));
				pBattleTip->addText(sgTextHelper(TEXT_BATTLE_SPEED_PRODUCE));
				pBattleTip->addText(sgTextHelper(TEXT_BATTLE_DEFENSE));
 
				if (!sgGuideManager()->checkGuideTypeFinished(E_GUIDELOC_LEVELUP))
					activeGuide(E_GUIDELOC_LEVELUP);
			}
			break;
			case E_COMPLETE_UPGRADE_TURRET:
			{
				pBattleTip->addText(sgTextHelper(TEXT_BATTLE_SPEED_ATTACK));
				pBattleTip->addText(sgTextHelper(TEXT_BATTLE_DEFENSE)); 
 
				if (!sgGuideManager()->checkGuideTypeFinished(E_GUIDELOC_LEVELUP))
					activeGuide(E_GUIDELOC_LEVELUP);
			}
			break;
			case E_OCCUPY_BARRACK:
			{
				pBattleTip->addText(sgTextHelper(TEXT_BATTLE_POPULATION));
			}
			break;
			case E_OCCUPY_TURRET:
			{
				pBattleTip->addText(sgTextHelper(TEXT_BATTLE_DEFENSE));
			}
			break;
			case E_OCCUPY_INSTITUTE:
			{
				pBattleTip->addText(sgTextHelper(TEXT_BATTLE_ATTACK_ALL));
				pBattleTip->addText(sgTextHelper(TEXT_BATTLE_DEFENSE_ALL));
			}
			break;
		}

		const std::string& sGlyphMsg = pMsg->getGlyphMsg();
		if (!sGlyphMsg.empty() && sGlyphMsg != "")
		{
			pBattleTip->addText(sGlyphMsg);
		}

		pBattleTip->setPosition(pMsg->getPositionX(), pMsg->getPositionY());
		pBattleTip->run();
		mBattleTipsLayer->addChild(pBattleTip);
	}
	else if(message->getMessageType() == EMT_GAMESTART)
	{
		MessageGameStart* pMsg = dynamic_cast<MessageGameStart*>(message);
		activeGuide(pMsg->getLocationID());
	}
	else if (message->getMessageType() == EMT_LITTLETIPS)
	{
		LittleTipsMessage* pMsg = dynamic_cast<LittleTipsMessage*>(message);
		bool isPause = pMsg->getIsPause();
		if (mLittleTips != NULL)
		{
			if (isPause)
				mLittleTips->pauseSchedulerAndActions();
			else
				mLittleTips->resumeSchedulerAndActions();
		}
	}
	else if (message->getMessageType() == EMT_SIMPLIFY_UI)
	{
		showSoldierControl(false);
		showPropItemNode(false);
	}
	else if (message->getMessageType() == EMT_BUTTON_OUT_PERCENT)
	{
		//更新状态
		MessageOutPercentBtn* pMes = dynamic_cast<MessageOutPercentBtn*>(message);
		int percent = pMes->getPercent();

		switch (percent)
		{
		case 100:
			mClickAnimation->setPosition(mPercentBg100->getPosition() + Vec2(-10, -20));
			break;
		case 75:
			mClickAnimation->setPosition(mPercentBg75->getPosition() + Vec2(-10, -20));
			break;
		case 50:
			mClickAnimation->setPosition(mPercentBg50->getPosition() + Vec2(-10, -20));
			break;
		case 25:
			mClickAnimation->setPosition(mPercentBg25->getPosition() + Vec2(-10, -20));
			break;
		default:
			break;
		}
		mClickAnimation->setVisible(true);
		mClickAnimation->getAnimation()->play("Double");
		sgBattleControl()->setOutputPercent(percent);
		changeOutPercentBg(percent);
	}
	else if (message->getMessageType() == EMT_PVPRESULT_OPENBOX)
	{
		//showNoItemBoxNode();
		mbShowNoItemAward = true;
		fgame::CSPvpGetAwardResp* packPvpGetAwardResp = sgPvpControl()->getPvpAwardResp();
		int iType = packPvpGetAwardResp->awardtype();
		if (iType == 2)
		{
			showNoItemBoxNode(0);
		}
	}
	else if (message->getMessageType() == EMT_DAILY_MISSION_PROGRESS_BACK)
	{
		if (sgBattleControl()->getIsPvp())
		{
			if (strcmp(mOnMenuStr.c_str(),"backWinPvp") == 0)
			{
				PageFunction::Get()->gotoFunction(MSG_FUC_MAIN_LEVEL);
				PageFunction::Get()->gotoFunction(MSG_FUC_WIN_PVP);
			}
		}
		else
		{
			if (strcmp(mOnMenuStr.c_str(),"return") == 0)
			{
				sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);
			}
		}
		mOnMenuStr = "";
	}

}

void GamePage::resetStarStr()
{
	int cout = 0;

	auto list = sgBalanceControl()->getStarList();

	if (list.size() <= 0)
	{
		mLevelNode->setVisible(false);
		mCheckBox->setVisible(false);
		return;
	}
//#ifdef MGAME_PLATFORM_MPLUS
//	mLevelNode->setVisible(sgUserObj()->isOpenGamePassTip());
//#else
//	mLevelNode->setVisible(false);
//#endif

	mLevelNode->setVisible(sgUserObj()->isOpenGamePassTip());

	mCheckBox->setVisible(true);
	mCheckBox->setSelected(true);

	if (sgBattleControl()->getIsPvp())
		mCheckBox->setVisible(false);


	BalanceControl::StarList::iterator it = list.begin();
	for (int i = 1; i <= 3; i++)
	{
		ui:Text* pLabel = dynamic_cast<ui::Text*>(mLevelNode->getChildByName(StringUtils::format("mLevelLabel%d", i)));
		ui::Button* mStatu = dynamic_cast<ui::Button*>(mLevelNode->getChildByName(StringUtils::format("mStatu%d", i)));
		pLabel->setVisible(false);
		mStatu->setVisible(false);
	}

	for (; it != list.end(); it++)
	{
		cout++;
		BaseStar* pStar = it->second;
		auto * pLabel = dynamic_cast<ui::Text*>(mLevelNode->getChildByName(StringUtils::format("mLevelLabel%d", cout)));
		auto * mStatu = dynamic_cast<ui::Button*>(mLevelNode->getChildByName(StringUtils::format("mStatu%d", cout)));


		if (pStar && pLabel)
		{
			pLabel->setText(pStar->getStarText());
			pLabel->setVisible(true);
			Size size = pLabel->getContentSize();
			pLabel->setTag(pStar->getStarTag());
			pLabel->setUserData(pStar);

			if (mStatu)
			{
				mStatu->setPosition(Vec2(mStatu->getContentSize().width  / 2, mLevelNode->getContentSize().height - cout * size.height / 2));
				mStatu->setTag(pStar->getStarTag() + STATU_BTN_TAG);
			}

			pLabel->setPosition(Vec2(size.width / 2 + 36, mLevelNode->getContentSize().height - cout * size.height / 2));
			changeStarStr(pStar->getStarTag(), false);
		}

	}
	cout++;
	for (int i = cout; i <= 3; i++)
	{
		auto* pLabel = dynamic_cast<ui::Text*>(mLevelNode->getChildByName(StringUtils::format("mLevelLabel%d", i)));
		if (pLabel)
		{
			pLabel->setVisible(false);
		}
	}

}


void GamePage::changeStarStr(int tag, bool right /* = false */)
{
	ui:Text* pLabel = dynamic_cast<ui::Text*>(mLevelNode->getChildByTag(tag));
	auto * mStatu = dynamic_cast<ui::Button*>(mLevelNode->getChildByTag(tag + STATU_BTN_TAG)); 
	BaseStar* pStar = sgBalanceControl()->getTargetStar(tag);
	if (pLabel && pStar)
	{
		int statu = pStar->getEndTag();
		if (statu == 0)
		{
			pLabel->setTextColor(Color4B::RED);
			mStatu->setVisible(true);
			mStatu->setBright(false);
		}
		else if (statu == 1)
		{
			pLabel->setTextColor(Color4B::GREEN);
			mStatu->setVisible(true);
			mStatu->setBright(true);
		}
		else
		{
			pLabel->setTextColor(Color4B::WHITE);
			mStatu->setVisible(false);
		}

		pLabel->setText(pStar->getStarText());
	}
}

void GamePage::initResModeScoreUI()
{
	for (int i = 1; i < 4; ++i)
	{
		mPlayer[i]->setVisible(false);
	} 
	const CampList& mCampList = sgBattleControl()->getCampList();
	int pPosition = 1;//sgBattleControl()->getOwnCamp();
	for (auto it = mCampList.begin(); it != mCampList.end(); it++)
	{ 

		if (it->first == ECAMP_TYPE_Neutral || it->first == ECAMP_TYPE_Creeps || it->first == ECAMP_TYPE_Invalid)
		{
			continue;
		}
		Node* pPlayerNode = NULL;
		switch ((MORALE_POSITION)pPosition)
		{
		case M_P_1:
			pPlayerNode = mPlayer[0];
			break;
		case M_P_2:
			pPlayerNode = mPlayer[1];
			break;
		case M_P_3:
			pPlayerNode = mPlayer[2];
			break;
		case M_P_4:
			pPlayerNode = mPlayer[3];
			break;
		default:
			break;
		}
		if (pPlayerNode)
		{
			initMoraleView();
			sgBattleControl()->initCampBuffView(pPlayerNode, it->first);
			pPlayerNode->setVisible(true);
			if (!pPlayerNode->getChildByTag(MORALEVIEW_TAG))
			{
				MoraleView* pMoral = MoraleView::create(it->first, (MORALE_POSITION)pPosition);
				pMoral->initWithCampType(pPlayerNode, mGlyphInfoNode);
			}
			else
			{
				dynamic_cast<MoraleView*>(pPlayerNode->getChildByTag(MORALEVIEW_TAG))->changeCampType(it->first);
			}

			Assistant* pAssistant = it->second->getAssistant();
			if (pAssistant)
			{
				AssistantView* pAsstView = pAssistant->getView();
				if (pAsstView)
				{
					float scale = 1;
					Node* asstParent = pPlayerNode->getChildByName("mAsstNode");
					cocostudio::ObjectExtensionData* pData = dynamic_cast<cocostudio::ObjectExtensionData*>(asstParent->getUserObject());
					if (pData)
					{
						scale = atoi(pData->getCustomProperty().c_str());
					}
					pAsstView->setAnchorPoint(Vec2(0.5, 0.5));
					asstParent->addChild(pAsstView);
					asstParent->setLocalZOrder(3);
					pAsstView->playMoveAciton(asstParent->getPosition(), scale);
					asstParent->setPosition(Vec2::ZERO);

				}
			}

		}
		pPosition++;
	} 
	
}

void GamePage::showSoldierControl(bool show)
{
	dynamic_cast<Node* >(mPanelNoBat->getChildByName("SpriteSingle_0"))->setVisible(show);
	dynamic_cast<Node* >(mPanelNoBat->getChildByName("mSliderBtn_4"))->setVisible(show);
	dynamic_cast<Node* >(mPanelNoBat->getChildByName("mSliderBtn_3"))->setVisible(show);
	dynamic_cast<Node* >(mPanelNoBat->getChildByName("mSliderBtn_2"))->setVisible(show);
	dynamic_cast<Node* >(mPanelNoBat->getChildByName("mSliderBtn_1"))->setVisible(show);
	//dynamic_cast<Node* >(mPanelNoBat->getChildByName("mLabel100"))->setVisible(show);
	//dynamic_cast<Node* >(mPanelNoBat->getChildByName("mLabel75"))->setVisible(show);
	//dynamic_cast<Node* >(mPanelNoBat->getChildByName("mLabel50"))->setVisible(show);
	//dynamic_cast<Node* >(mPanelNoBat->getChildByName("mLabel25"))->setVisible(show);
}

void GamePage::showNoBatControl(bool show)
{
	mPanelNoBat->setVisible(show);
}


void GamePage::initMoraleView()
{
	
}



void GamePage::loadCampScoreInfo(int idx, eCampType ecamp)
{
	return;
	Node* pPlayerNode = mPlayer[idx];
	pPlayerNode->setTag(ecamp);

	ImageView* pPhoto = dynamic_cast<ImageView*>(pPlayerNode->getChildByName("mPhoto"));
	Text* pScoreLabel = dynamic_cast<Text*>(pPlayerNode->getChildByName("mScoreLabel"));

	std::string szPhoto;
	switch (ecamp % 5)
	{
	case ECAMP_TYPE_Player:
		szPhoto = "uiAtlas_Battle_baozi.png";
		break;
	case ECAMP_TYPE_AI:
		szPhoto = "uiAtlas_Battle_zhongzi.png";
		break;
	case ECAMP_TYPE_AI2:
		break;
	case ECAMP_TYPE_AI3:
		break;
	case ECAMP_TYPE_Neutral:
	case ECAMP_TYPE_Invalid:
		break;
	default:
		break;
	}

	pPhoto->loadTexture(szPhoto, ui::TextureResType::PLIST);

	int campScore = sgBattleControl()->getTargetCamp(ecamp)->getResScore();
	pScoreLabel->setString(CCString::createWithFormat("%d", campScore)->getCString());//setText(CCString::createWithFormat("%d", campScore)->getCString());
}

void GamePage::updateResModeScore()
{
	return;
	for (int i = 0; i != 4; ++i)
	{
		Node* pPlayerNode = mPlayer[i];
		if (!pPlayerNode->isVisible())
			break;

		eCampType ecamp = (eCampType)pPlayerNode->getTag();

		Text* pScoreLabel = dynamic_cast<Text*>(pPlayerNode->getChildByName("mScoreLabel"));
		int campScore = 0;

		Camp* pCamp = sgBattleControl()->getTargetCamp(ecamp);
		if (pCamp)
		{
			campScore = sgBattleControl()->getTargetCamp(ecamp)->getResScore();
		}
		pScoreLabel->setString(CCString::createWithFormat("%d", campScore)->getCString());//setText(CCString::createWithFormat("%d", campScore)->getCString());
	}
}


void GamePage::valueChanged(Ref* _slider, SliderEventType _type)
{
	Slider* slider = dynamic_cast<Slider*>(_slider);

	if (slider->getPercent() < 20)
	{
		slider->setPercent(20);
	}

	if (slider->getPercent() > 80)
	{
		slider->setPercent(80);
	}

	int p = (100 - slider->getPercent()) * 100 / 80;

	if (p < 25)
	{
		p = 25;
		slider->setPercent(80);
	}
	sgBattleControl()->setOutputPercent(p);

	//mSliderLabel->setText(CCString::createWithFormat("%d%%", p)->getCString());

	CCLOG("current value is %d", p);
}

void GamePage::setOpacityAction(Node* pSender, bool isClean)
{
	if (pSender)
	{
		pSender->stopAllActions();
		if (isClean)
		{
			pSender->setOpacity(255);
		}

		auto pAction = FadeTo::create(2.0f, pSender->getOpacity() * .5f);
		pSender->runAction(pAction);
	}
}

void GamePage::SliderBtnOpaAction()
{
	//auto pPercentBg = dynamic_cast<Sprite*>(mPanelNoBat->getChildByName("SpriteSingle_0"));
	//setOpacityAction(pPercentBg);

	//setOpacityAction(mPercentBg25);
	//setOpacityAction(mPercentBg50);
	//setOpacityAction(mPercentBg75);
	//setOpacityAction(mPercentBg100);

	//setOpacityAction(mLabel100, false);
	//setOpacityAction(mLabel75, false);
	//setOpacityAction(mLabel50, false);
	//setOpacityAction(mLabel25, false);
}

void GamePage::onCallBackExitPlayback(Ref*)
{
	sgBattleRecordsManager()->setIsBeganPlayback(false);
	sgBattleRecordsManager()->setIsPlayback(false);

	// 战斗场景点击返回，判定失败
	sgBalanceControl()->doCheckPtEnd(false, true);

	int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
	if (checkpt / 1000 == 1)
		PageFunction::Get()->gotoFunction(MSG_FUC_MINI_LEVEL);
	else if (checkpt / 1000 == 2)
		PageFunction::Get()->gotoFunction(MSG_FUC_MINI_LEVEL);

	GenerlDialog::hide();
}

void GamePage::onCallBackContinue(Ref*)
{
	//此界面关闭 游戏action继续
	sgGamePrecedure()->setGameIng(true);
	sgMessageManager()->sendMessage(EMT_LITTLETIPS, "0");
	GenerlDialog::hide();
}
void GamePage::onCallBackshowShop(Ref*)
{
	//上报每日任务相关数据
	sgUserObj()->sendDailyMissionData(true);

	if (sgPvpControl()->getPVPMode() == PVP_MODE_RAND)
		sgWinnerControl()->setWinnerCount(0);
	if (sgPvpControl()->getPVPMode() == PVP_MODE_NOITEM && !mbNoItemLess2Min)
	{
		//此处只为无道具pvpAward 请求
		sgPvpControl()->getPvPProxy()->sendLeave();
	}
	else
	{
		mOnMenuStr = "backWinPvp";
	}

	GenerlDialog::hide();
}

void GamePage::showPropItemNode(bool show)
{
	if (sgBattleRecordsManager()->getIsPlayback())
	{
		show = false;
	}

	for (int i = 1; i <= 3; i++)
	{
		Node* pNode = mPanelNoBat->getChildByName(__String::createWithFormat("prop_item%d", i)->getCString());
		if (pNode)
		{
			pNode->setVisible(show);
		}
	}
}

void GamePage::animationCallback(Armature * pAr, MovementEventType type, const std::string& aniName)
{
	if (type == COMPLETE && pAr == mClickAnimation)
	{
		mClickAnimation->setVisible(false);
	}
}

void GamePage::initGuidePropBar()
{
	clearGuidePropBar();
	const CheckPoint* pCheck = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);
	const GuideData* pData = sgGuideManager()->getGuideDataByGuideID(GuideID_PveProp);
	if (!pData)
		return;
	if (pCheck->levelType != E_Level_Trans && pData->getGateID() != pCheck->id)
		return;
	for (int i = 1; i <= 3; i++)
	{
		Node* pNode = mPanelNoBat->getChildByName(__String::createWithFormat("prop_item%d", i)->getCString());
		sgPropsControl()->initGuideProp(pNode, i); 
	}
}

void GamePage::addGuideProp(int propID)
{
	sgPropsControl()->addGuideProp(propID);
}

void GamePage::clearGuidePropBar()
{
	const CheckPoint* pCheck = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt); 
	const GuideData* pData = sgGuideManager()->getGuideDataByGuideID(GuideID_PveProp);
	if (!pData)
		return;
	if (pCheck->levelType != E_Level_Trans && pData->getGateID() != pCheck->id)
		return; 
	sgPropsControl()->clearGuideProp();
}

void GamePage::doGuideExtraFunc(MessageGuideTips* pMsg)
{
	if (pMsg->getFunctionID() == E_GuideFuc_Tips)
	{
		if (mLevelNode)
			mLevelNode->setVisible(!mLevelNode->isVisible());
	}
	else if (pMsg->getFunctionID() == E_GuideFuc_BV2GP)
	{
		activeGuide(pMsg->getFunctionParam());
	}
	else if (pMsg->getFunctionID() == E_GuideFuc_AddGift)
	{
		playGuidePropAni(pMsg->getFunctionParam());
	}
}

void GamePage::playGuidePropAni(int giftID)
{
	GiftAwardDialog* pDialog = GiftAwardDialog::showDialogByGiftId(giftID);
	if (pDialog)
		pDialog->setbGuideCallBack(true, this, callfuncID_selector(GamePage::onGuidePropAniCallback), true);
}

void GamePage::onGuidePropAniCallback(int giftId)
{ 
	sgGuideManager()->unRegistGuideMessage();
	const GiftAward* giftData = sgTableManager()->getGiftAward(giftId);
	if (!giftData)
		return;
	int propID = giftData->getPropIdByIndex(1);
	addGuideProp(propID);
	activeGuide();
}

void GamePage::activeGuide(int location)
{
	int gateID = sgGamePrecedure()->getCurMapInfo().checkpt;
	int level = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel();
	//开始新手引导--战斗界面UI	 
	sgGuideManager()->registGuideMessage(gateID, level, location, mPanelNoBat, this, false, ccp(0, 0));
}

void GamePage::showNoItemBoxNode(float dt)
{
	if (!mbShowNoItemAward) return;
	mbShowNoItemAward = false;
	fgame::CSPvpGetAwardResp* packPvpGetAwardResp = sgPvpControl()->getPvpAwardResp();
	int iType = packPvpGetAwardResp->awardtype();
	std::string sub_str = "";
	std::string total_str = "";
	for (int i = 0; i < packPvpGetAwardResp->updatedata_size(); i++)
	{
		const fgame::PropertyUpdateData& pUpdate = packPvpGetAwardResp->updatedata(i);

		sub_str = StringUtils::format("%d+%d+%d#", pUpdate.uptype(), pUpdate.value2(), pUpdate.value());
		total_str = StringUtils::format("%s%s", total_str.c_str(), sub_str.c_str());
	}

	if (iType != 0 && packPvpGetAwardResp->updatedata_size() > 0)
	{
		
		if (iType == 2)
		{
			auto func = [](){
				sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
				sgPageFunction()->gotoFunction(MSG_FUC_WIN_PVP);
			};
			BoxRewardDialog* pDialog = BoxRewardDialog::showBoxRewardDialog(BOX_REWARD_PVP_NOITEM, total_str, func);
		}
		else
		{
			BoxRewardDialog* pDialog = BoxRewardDialog::showBoxRewardDialog(BOX_REWARD_PVP_NOITEM, total_str);
		}
	}
	else
	{
		sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
		sgPageFunction()->gotoFunction(MSG_FUC_WIN_PVP);
	}
}

bool GameReady::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("GenerlPage.csb");

		CC_BREAK_IF(!Node::init());
		CC_BREAK_IF(!mRoot);
		
		//mContainer
		mReadyAni = sgResManager()->getAramatur("fla_PVEdaoshu_fixed");//Armature::create("fla_PVEdaoshu_fixed");
		mReadyAni->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(GameReady::animationCallback));
		mRoot->getChildByName("mContainer")->addChild(mReadyAni);

		mPvpReadyAni = sgResManager()->getAramatur("fla_daoshu_fixed");//Armature::create("fla_daoshu_fixed");
		mPvpReadyAni->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(GameReady::animationCallback));
		mRoot->getChildByName("mContainer")->addChild(mPvpReadyAni);
		

		this->addChild(mRoot);
		ret = true;
	} while (0);

	return ret;
}

GameReady::GameReady() : mReadyAni(nullptr), mPvpReadyAni(nullptr)
{

}

GameReady::~GameReady()
{

}

void GameReady::onShow()
{
	if (mReadyAni)
	{
		mReadyAni->setVisible(!sgBattleControl()->getIsPvp());
		if (!sgBattleControl()->getIsPvp())
		{
			mReadyAni->getAnimation()->play("play");
		}
	}

	if (mPvpReadyAni)
	{
		mPvpReadyAni->setVisible(sgBattleControl()->getIsPvp());
		if (sgBattleControl()->getIsPvp())
		{
			mPvpReadyAni->getAnimation()->play("play");
		}
	}

	const CheckPoint* pCheck = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);
	if (pCheck && pCheck->checkType == 2 && !sgUserObj()->getOccupyModeFlag())
	{
		sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "LevelModeTip.csb", 1);
	}

	this->setVisible(true);
}

void GameReady::animationCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (type == COMPLETE)
	{
	//	scheduleOnce(schedule_selector(GameReady::updateCaptureScreen),0.1);
		
		sgBattleControl()->setBorning();
		sgPropsControl()->initPropCd();
		sgBalanceControl()->starGameTime();
		sgBattleControl()->starAddCreepsPoint();
		sgBattleControl()->getAsstBattleCtrl()->run();
		sgBattleControl()->getBattleView()->setEnbelTouch(true);
		sgMessageManager()->sendMessage(EMT_GAMESTART, StringUtils::format("%d", E_GUIDELOC_GAMEPAGE));
		this->setVisible(false);

		sgBattleRecordsManager()->resetCommondInterval();

		if (sgBattleRecordsManager()->getIsPlayback())
		{
			sgBattleRecordsManager()->setIsBeganPlayback(true);
		}

		if (!sgBattleControl()->getIsPvp() && Window::getWindow("LevelModeTip.csb") == NULL)
		{
			LuaEngine::getInstance()->executeString("sgUiUtils():saveSceneImage(1)");
		}

	}
}

void GameReady::updateCaptureScreen(float dt)
{
	cocos2d::utils::captureScreen(CC_CALLBACK_2(GameReady::captureCallBack,this), "screenshot.png");
	unschedule(schedule_selector(GameReady::updateCaptureScreen));
}

void GameReady::captureCallBack(bool b, std::string path)
{
	log("success %s, %s", b ? "true" : "false", path.c_str());
}


