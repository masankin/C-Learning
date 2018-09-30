#include "ResultDialog.h"
#include "game/ui//PageFunction.h"
#include "game/data/Tables.h"
#include "game/data/TableManager.h"
#include "game/balance/BalanceControl.h"
#include "battle/BattleControl.h"
#include "EnvSet.h"
#include "game/net/UserObj.h"
#include "guide/GuideManager.h"
#include "SoundManager.h"
#include "balance/StarModes.h"
#include "net/NetManager.h"
#include "battle/BattleRecordsManager.h"
#include "lua/LuaFuncHelper.h"
#include "ui/dialogs/GiftAwardDialog.h"
#include "ui/dialogs/BoxRewardDialog.h"
#include "game/notice/NoticeControl.h"

StarLvNode::StarLvNode()
{
}

StarLvNode::~StarLvNode()
{
}


StarLvNode* StarLvNode::create()
{
	StarLvNode* pret = new StarLvNode();
	if (pret)
	{ 
		if(pret->init())
		{
			
			return pret;
		} 
		else
		{
			CCLOG("[trace]StarLvNode init failed");
		}
	}
	else
	{
		CCLOG("[trace]StarLvNode new failed");
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

bool StarLvNode::init()
{
	mRoot = CSLoader::getInstance()->createNode("ui/StarLvNode.csb");
	if (!mRoot)
	{
		CCASSERT(mRoot == NULL, "StarLvNode---init ----createNode:mRoot failed");
		CCLOG("[error] ArrowWidget::initUI load csb error!");
		return false;
	}
	this->addChild(mRoot);

	mRoot->setPositionX(-Director::getInstance()->getWinSize().width/2);
	return true;
}

void StarLvNode::resetStarStr()
{
	auto list = sgBalanceControl()->getStarList();
	if (list.size() == 0)
	{
		this->setVisible(false);
		return;
	}
	this->setVisible(true);
	ui:Text* pLabel[3];
	Sprite* mStatu[3];
	for (int i = 1; i <= 3; i++)
	{
		pLabel[i-1] = dynamic_cast<ui::Text*>(mRoot->getChildByName(StringUtils::format("mLevelLabel%d", i)));
		mStatu[i-1] = dynamic_cast<Sprite*>(mRoot->getChildByName(StringUtils::format("Sprite_%d", i)));
		pLabel[i-1]->setVisible(false);
		mStatu[i-1]->setVisible(false);
	}

	int i = 0;

	BalanceControl::StarList::iterator it = list.begin();
	for (; it != list.end(); it++)
	{	
		BaseStar* pStar = it->second;
		pLabel[i]->setText(pStar->getStarText());
		pLabel[i]->setVisible(true);

		if (pStar->getEndTag() == 1)
		{
			mStatu[i]->setSpriteFrame("ui/Result/Pve_get.png");
		}
		else
		{
			mStatu[i]->setSpriteFrame("ui/Result/Pve_Notget.png");
		}
		
		mStatu[i]->setVisible(true);
		i++;
	}

}

NumberEffectNode* NumberEffectNode::create(int target, float time, ui::Text* widget)
{
	NumberEffectNode* pRet = new NumberEffectNode();
	if (pRet && pRet->init(target, time, widget))
		return pRet;

	CC_SAFE_DELETE(pRet);
	return NULL;
}

bool NumberEffectNode::init(int target, float time, ui::Text* widget)
{
	mCurNumber = 0;
	mTarget = target;
	mSpeed = ::ceil(target / (time * 60.0f));
	mText = widget;
	if (mText)
	{
		mText->addChild(this);
		mText->setString("0");
	}
	mIsDone = false;

	scheduleUpdate();

	return true;
}

void NumberEffectNode::update(float dt)
{
	if (mCurNumber < mTarget)
	{
		if (mTarget - mCurNumber > 10)
		{
			mCurNumber += mSpeed;
		}
		else
		{
			mCurNumber += 1;
		}
		if (mText)
			mText->setString(__String::createWithFormat("%d", mCurNumber)->getCString());
	}
	else
	{
		end();
	}
}

void NumberEffectNode::end()
{
	mIsDone = true;
	unscheduleUpdate();
	if (mText)
		mText->setString(__String::createWithFormat("%d", mTarget)->getCString());
}

ResultDialog::ResultDialog() : mTimePoints(NULL), mPropPoints(NULL), mTotalPoints(NULL)
{
	//weihua.cui
	//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_pvpshengli.xml");
	
}

ResultDialog::~ResultDialog()
{
	// comment weihua.cui
	//cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_pvpshengli.xml");
}

void ResultDialog::onReceiveMassage(GameMessage* message)
{ 
	if (message->getMessageType() == EMT_GUIDE_TIPS)
	{
		MessageGuideTips* pMsg = dynamic_cast<MessageGuideTips*>(message);
		if (!pMsg)
			return;
		doGuideExtraAction(pMsg);
	}
}

void ResultDialog::doGuideExtraAction(MessageGuideTips* pMsg)
{
	if (pMsg->getFunctionID() == E_GuideFuc_GotoMain)
		gotoMainUI();
}

void ResultDialog::gotoMainUI()
{
	sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
	sgPageFunction()->enableOptionButton(true);
	sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
}

void ResultDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (false == sgBattleRecordsManager()->getIsPlayback() && (cName == "onResultBack" || cName == "onResultNext"))
	{
		if (sgEnvSet("MGAME_OPEN_BATTLE_RECORD")){
			sgBattleRecordsManager()->saveBattleRecords(sgGamePrecedure()->getCurMapInfo().checkpt);
		}		
	}

	sgSoundManager()->beganBattleEffect();

	if(sgGuideManager()->getGuideLock() == true)
	{
		return;
	}

	if (cName == "onResultNext")
	{	
		/*
		if (sgGamePrecedure()->nextLevel())
		{
			sgPageFunction()->gotoFunction(MSG_FUC_PLAYING);
		}
		else{
			 if (sgUserObj()->getMaxOpenCheckPoint() == 2010)
			{
				sgMessageManager()->sendMessage(EMT_OPEN_STAGE, "2");
			}
				sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
			
		}
		*/
		/*sgGuideManager()->unRegistGuideMessage();
		if (sgGamePrecedure()->nextLevel())
		{
			if (sgUserObj()->getLvlUpgrade())
				sgUserObj()->setPlayerUpgradeState(MSG_FUC_WIN_LEVEL);
		
			sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);
			int level = sgGamePrecedure()->getCurMapInfo().checkpt;
			sgMessageManager()->sendMessage(EMT_ACHIEVE_GOTOLEVEL,
				__String::createWithFormat("%d", level)->getCString());
		}
		else{
			sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);
		}*/
		sgGuideManager()->unRegistGuideMessage();
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBthGotoMission");

		if (sgPageFunction()->getCurrentUIType() == MSG_FUC_MINI_LEVEL)
		{
			this->setShow(false);
			sgPageFunction()->setDiamondPanel(true);
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBtnRefreshPanel");
		}
		else
		{
			sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);
		}
	}
	else if (cName == "onResultBack")
	{
		//完成新手引导--战斗胜利返回
		sgGuideManager()->unRegistGuideMessage();
		CCLOG("[GUIDE]### End to battle success message");  
		if (sgPageFunction()->getCurrentUIType() == MSG_FUC_MINI_LEVEL)
		{
			this->setShow(false);
			sgPageFunction()->setDiamondPanel(true);
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBtnRefreshPanel");
		}
		else
		{
			sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);
		}
	}
	else if (cName == "onSkipAnimation")
	{
		scoreEffectDone();
	}
}

bool ResultDialog::init()
{
	//

	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/ResultDialogWin.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);

		setAnimation("fla_shengli");
		auto bgBone = mArmatureBg->getBone("guge_bg");
		Sprite* bgSp = Sprite::create("ui/Result_bg-big.png");
		bgBone->addDisplay(bgSp, 0);
		
		mStarLvNode = StarLvNode::create();
		mCenter->addChild(mStarLvNode,10);

		//mRoot->getChildByName("Win")->runAction(RepeatForever::create(RotateBy::create(4.5f,360)));

		
		mCenter_1 = mRoot->getChildByName("mCenter_1");

		mArmatureGuang = cocostudio::Armature::create("fla_shengli_ADDITIVE");
		mCenter->addChild(mArmatureGuang,11);
		ccBlendFunc tmp_oBlendFunc;
		tmp_oBlendFunc.src = 1;
		tmp_oBlendFunc.dst = 1;
		mArmatureGuang->setBlendFunc(tmp_oBlendFunc);
		mArmatureGuang->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(ResultDialog::windowAniCallback));

		setPopType(E_POP_TYPE_NONE);

		mBtnBack = dynamic_cast<ui::Button *>(mRoot->getChildByName("Button_1"));
		mBtnNextLevel = dynamic_cast<ui::Button *>(mRoot->getChildByName("mNextLevel"));
		//mWorld = dynamic_cast<cocos2d::Sprite *>(mBtnNextLevel->getChildByName("uiAtlas_Result_zongfeng_5"));

		Node *tempNode = dynamic_cast<Node *>(mRoot->getChildByName("node_exp"));

		mEXP = dynamic_cast<ui::Text *>(tempNode->getChildByName("text_exp"));

		mHasPlayEffect = false;
		ret = true;
	} while (0);


	return ret;
}

void ResultDialog::onEnter()
{
	ActionWindow::onEnter();
	
}
 
void ResultDialog::onShow()
{
	sgGuideManager()->unRegistGuideMessage(true);
	sgMessageManager()->regisiterMessageHandler(EMT_GUIDE_TIPS, this);

	mStarLvNode->resetStarStr();
	mRoot->getChildByName("Node_score")->setVisible(false);
	mRoot->getChildByName("Button_skipAnimation")->setVisible(false);

	mArmatureBg->getAnimation()->play("born");
	mArmatureGuang->getAnimation()->play("born");

	if (!sgBattleRecordsManager()->getIsBeganPlayBack())
	{
		mEXP->getParent()->setVisible(true);
		mEXP->setText(StringUtils::format("%d", sgUserObj()->getPersonalInfo()->addexp()));//sgUserObj()->getPlayerAddEX()));
	}
	else
	{
		mEXP->getParent()->setVisible(false);
	}
	
	//mRoot->getChildByName("Win")->stopAllActions();
	//mRoot->getChildByName("Win")->runAction(RepeatForever::create(RotateBy::create(4.5f,360)));

	if (Director::getInstance()->getScheduler()->isScheduled(schedule_selector(LoginProxy::updateHearBeat),sgLoginProxy())==false)
	{
		sgLoginProxy()->setConnTimes(CONNET_COUNT);
		Director::getInstance()->getScheduler()->schedule(schedule_selector(LoginProxy::updateHearBeat),
			sgLoginProxy(),HEARBEAT_TICK,CC_REPEAT_FOREVER,0.1,false);
	}else if (sgNetManager()->IsConnect() == false)
	{
		ReLoginDialog::show();
	}

	if (sgBattleRecordsManager()->getIsPlayback())
	{
		enableClickEvent(false);
		//mBtnBack->setVisible(false);
		//mBtnNextLevel->setVisible(false);
		//mWorld->setVisible(false);
		this->scheduleOnce(schedule_selector(ResultDialog::autoClose), 2.0f);
	}
	else
	{
		initStar();
		enableClickEvent(false);
		//mBtnBack->setVisible(true);
		//mBtnNextLevel->setVisible(true);
		//mWorld->setVisible(true);

#ifdef MGAME_PLATFORM_MPLUS
		int level = sgGamePrecedure()->getCurMapInfo().checkpt;
		if (level >= sgEnvSet("PVP_RAND_LEVEL"))
		{
			this->scheduleOnce(schedule_selector(ResultDialog::autoShowAd), 1.0f);
		}
#endif	
	}
	sgGamePrecedure()->setEnterGame(false);

	if (sgUserObj()->getIsShowDailyMissionProgress())
	{
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eOpenMissionCompleteDialog", NULL);
		sgUserObj()->setIsShowDailyMissionProgress(false);
	}
	onLocateClickCallback(mRoot);

	scheduleUpdate();

	if (sgUserObj()->getIsSkipSuccess())
	{
		showTextTip(sgTextHelper(TEXT_BTH_SKIP_SUCCESS).c_str());
		sgUserObj()->setIsSkipSuccess(false);
	}
}

void ResultDialog::initStar()
{
	int level = sgGamePrecedure()->getCurMapInfo().checkpt;
	int starNum  =  sgTableManager()->getCheckPointStars(level);
	int num  = sgBalanceControl()->getStarNum();

	if (starNum)
	{
		std::string name = StringUtils::format("fla_shengji_%dxing%d",starNum,num);

		cocostudio::Armature* arm = cocostudio::Armature::create(name);
		arm->getAnimation()->play("born");
		mCenter_1->addChild(arm);
		arm->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(ResultDialog::windowAniCallback));
	}	
	else
	{	
		cocostudio::Armature* arm = cocostudio::Armature::create("fla_shengli_zi");
		arm->getAnimation()->play("play");
		mCenter_1->addChild(arm);
		arm->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(ResultDialog::windowAniCallback));
	}
}

void ResultDialog::autoClose(float dt)
{
	sgBattleRecordsManager()->setIsPlayback(false);
	sgBattleRecordsManager()->setIsBeganPlayback(false);
	sgSoundManager()->beganBattleEffect();

	sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);
	//this->setShow(false);
}
void ResultDialog::onClose()
{
	ActionWindow::onClose();
	mCenter_1->removeAllChildrenWithCleanup(true);
	sgMessageManager()->removeMessageHandler(EMT_GUIDE_TIPS, this);
}

#ifdef MGAME_PLATFORM_MPLUS
void ResultDialog::autoShowAd(float dt)
{
	sgMplusControl()->getPveCanShowAd();
}
#endif

void ResultDialog::initScore()
{
	Node* pointsNode = mRoot->getChildByName("Node_score");
	Node* totalScoreNode = pointsNode->getChildByName("Node_totalScore");
	pointsNode->getChildByName("Node_level")->setVisible(false);
	pointsNode->getChildByName("Button_share")->setVisible(false);
	ui::Text* timeText = (ui::Text*) pointsNode->getChildByName("Text_timeScore");
	ui::Text* propText = (ui::Text*) pointsNode->getChildByName("Text_propScore");
	ui::Text* totalText = (ui::Text*) totalScoreNode->getChildByName("Text_totalScore");
	pointsNode->setVisible(true);
	mRoot->getChildByName("Button_skipAnimation")->setVisible(true);

	int timeScore = sgBalanceControl()->getTimeScore();
	int propScore = sgBalanceControl()->getPropScore();
	mTimePoints = NumberEffectNode::create(timeScore, 1.0f, timeText);
	mPropPoints = NumberEffectNode::create(propScore, 1.1f, propText);
	mTotalPoints = NumberEffectNode::create(timeScore + propScore, 1.3f, totalText);
	if (mTotalPoints->mTarget == 0)
		pointsNode->getChildByName("Node_level")->setVisible(false);

	cocostudio::Armature* scoreArmature = cocostudio::Armature::create("fla_fenshupai");
	mCenter->addChild(scoreArmature, 11);
	auto scoreBone = scoreArmature->getBone("guge_fenshu");
	scoreBone->addDisplay(totalScoreNode, 0);
	scoreArmature->getAnimation()->play("born");
	scoreArmature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(ResultDialog::windowAniCallback));
}

void ResultDialog::enableClickEvent(bool enable)
{
	mBtnBack->setVisible(enable);
	mBtnNextLevel->setVisible(enable);
}

void ResultDialog::scoreEffectDone()
{  
	if (mHasPlayEffect)
		return;
	mHasPlayEffect = true;

	unscheduleUpdate();
	mTimePoints->end();
	mPropPoints->end();
	mTotalPoints->end();

	Node* pointsNode = mRoot->getChildByName("Node_score");
	Node* levelNode = pointsNode->getChildByName("Node_level");

	mRoot->getChildByName("Button_skipAnimation")->setVisible(false);
	
	

	bool canGetBaseReward = false;
	bool canGetPerfectReward = false;
	int selfMissionRank;
	int shareId = 0;
	int rankLimit = 0;
	auto callback = [&canGetBaseReward, &canGetPerfectReward, &selfMissionRank, &shareId, &rankLimit](lua_State* tolua_S, int argc){
		if (argc >= 5)
		{
			if (lua_isboolean(tolua_S, -5))
				canGetBaseReward = lua_toboolean(tolua_S, -5);

			if (lua_isboolean(tolua_S, -4))
				canGetPerfectReward = lua_toboolean(tolua_S, -4);

			if (lua_isnumber(tolua_S, -3))
				selfMissionRank = lua_tonumber(tolua_S, -3);

			if (lua_isnumber(tolua_S, -2))
				shareId = lua_tonumber(tolua_S, -2);

			if (lua_isnumber(tolua_S, -1))
				rankLimit = lua_tonumber(tolua_S, -1);
		}
	};

	LuaEngine* scriptEngine = dynamic_cast<LuaEngine*>(ScriptEngineManager::getInstance()->getScriptEngine());
	LuaStack* stack = scriptEngine->getLuaStack();
	auto luaFuncHandler = sgLuaFuncHandlerManager()->getHandler(LuaFuncHandlerManager::ModuleType::kCustom, (int)LuaFuncHandlerManager::CustomHandlerType::kGetPveResultValue);
	if (luaFuncHandler > 0)
	{
		auto tolua_S = stack->getLuaState();

		LuaStatePrint(tolua_S);
		auto rlt = stack->executeFunction(luaFuncHandler, 0, 5, callback);
		LuaStatePrint(tolua_S);
		stack->clean();
	}

	std::string levelSp = "";
	std::string levelText = "";
	ui::Text* levelTextNode = ((ui::Text*) levelNode->getChildByName("Text_8"));
	if (selfMissionRank == 0)
	{
		mBtnNextLevel->setPositionX(640);
		levelSp = "ui/Result/scoreLevelBg_3.png";

		int percent = 0;
		if (mTotalPoints->mTarget < 2000)
		{
			percent = random(1, 9);
		}
		else if (mTotalPoints->mTarget < 12000)
		{
			percent = random(10, 39);
		}
		else if (mTotalPoints->mTarget < 20000)
		{
			percent = random(40, 69);
		}
		else if (mTotalPoints->mTarget < 25000)
		{
			percent = random(70, 79);
		}
		else if (mTotalPoints->mTarget < 28000)
		{
			percent = random(80, 89);
		}
		else
		{
			percent = random(90, 95);
		}
		levelText = __String::createWithFormat(sgTextHelper(TEXT_BTH_BEYOUD).c_str(), percent)->getCString();
		levelTextNode->setPositionY(2);
	}
	else if (selfMissionRank == 1)
	{
		NoticeInfo* pInfo = new NoticeInfo();
		pInfo->autorelease();

		pInfo->setIsUrgent(1);
		const GameInfo info = sgGamePrecedure()->getCurMapInfo();
		const CheckPoint* pCP = sgTableManager()->getCheckPointByID(info.checkpt);
		pInfo->setNoticeContent(__String::createWithFormat(sgTextHelper(TEXT_BTH_TOP_CONTENT).c_str(), sgUserObj()->getPlayerName().c_str(), pCP->owner, pCP->levelName.c_str())->getCString());
		sgNoticeControl()->addNotice(1, pInfo);

		//pointsNode->getChildByName("Button_share")->setVisible(true);
		mBtnNextLevel->setPositionX(640);
		levelSp = "ui/Result/scoreLevelBg_1.png";
		levelText = sgTextHelper(TEXT_BTH_TOP);
		levelTextNode->setPositionY(-18);
	}
	else
	{
		mBtnNextLevel->setPositionX(640);
		levelSp = "ui/Result/scoreLevelBg_2.png";
		levelText = __String::createWithFormat(sgTextHelper(TEXT_BTH_BEYOUD_FIFTH).c_str(), selfMissionRank)->getCString();
		levelTextNode->setPositionY(-8);
	}

	((Sprite*)levelNode->getChildByName("Sprite_level"))->setSpriteFrame(levelSp);
	levelTextNode->setString(levelText);
	levelNode->setPosition(Point(0, 0));

	mShareId = shareId;
	mCanGetBaseReward = canGetBaseReward;
	mCanGetPerfectReward = canGetPerfectReward;
	mSelfMissionRank = selfMissionRank;
	mRankLimit = rankLimit;

	if (mTotalPoints->mTarget != 0)
	{
		cocostudio::Armature* levelArmature = cocostudio::Armature::create("fla_pvpshengli_jieyin");
		levelArmature->setPosition(Point(500, 180));
		mCenter->addChild(levelArmature, 11);
		auto levelBone = levelArmature->getBone("guge_huizhang");
		levelBone->addDisplay(levelNode, 0);
		levelArmature->getAnimation()->play("born");
		levelArmature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(ResultDialog::windowAniCallback));
	}
	else
	{
		showEffectUI();
	}
}

bool ResultDialog::share(int rank)
{
	if (rank > 0 && rank <= mRankLimit)
	{
#ifdef MGAME_PLATFORM_MPLUS
		std::string param = sgChannelApi()->getShareMsg("message1", "", false);
		int levelId = sgGamePrecedure()->getCurMapInfo().checkpt;
		int useTime = sgTableManager()->getCheckPointByID(levelId)->timeLimit - sgBalanceControl()->getTimeLeft();
		std::string winInfo = StringUtils::format(param.c_str(), useTime, sgPvpControl()->getKillPeople(sgUserObj()->getUin()));
		sgChannelApi()->callGameInfo(TYPE_SHARE_MSG, "message1", winInfo);
#else
#ifdef MGAME_IN_APP_SDK
		const char* pScript = __String::createWithFormat("sgShareControl():openView(%d,%d,%d)", 2, 6001, rank)->getCString();
		LuaEngine::getInstance()->executeString(pScript);
#endif // MGAME_IN_APP_SDK
#endif
		return true;
	}
	return false;
}

bool ResultDialog::winShare(int id)
{
	if (id > 0)
	{
#ifdef MGAME_PLATFORM_MPLUS
		std::string param = sgChannelApi()->getShareMsg("message1", "", false);
		int levelId = sgGamePrecedure()->getCurMapInfo().checkpt;
		int useTime = sgTableManager()->getCheckPointByID(levelId)->timeLimit - sgBalanceControl()->getTimeLeft();
		std::string winInfo = StringUtils::format(param.c_str(), useTime, sgPvpControl()->getKillPeople(sgUserObj()->getUin()));
		sgChannelApi()->callGameInfo(TYPE_SHARE_MSG, "message1", winInfo);
#else
#ifdef MGAME_IN_APP_SDK
		Node* pointsNode = mRoot->getChildByName("Node_score");
		Node* totalScoreNode = pointsNode->getChildByName("Node_totalScore");
		totalScoreNode->setPosition(ccp(640,470));
		const char* pScript = __String::createWithFormat("sgShareControl():openView(%d,%d)", 3, id)->getCString();
		LuaEngine::getInstance()->executeString(pScript);
		totalScoreNode->setPosition(ccp(0, 0));
#endif // MGAME_IN_APP_SDK
#endif
		return true;
	}
	return false;
}

void ResultDialog::update(float dt)
{
	
	if (mTotalPoints && mTotalPoints->mIsDone == true)
	{
		scoreEffectDone();
	}
}

void ResultDialog::showEffectUI()
{
	enableClickEvent(true);

	//开始新手引导--战斗胜利界面返回	 
	int gateID = sgGamePrecedure()->getCurMapInfo().checkpt;
	int level = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel();
	Size winSize = Director::getInstance()->getWinSize(); 
	sgGuideManager()->registGuideMessage(gateID, level, E_GUIDELOC_BATTLESUCCESS, mRoot, this, false, -0.5 * winSize);
	CCLOG("[GUIDE]### Start to battle success message");

	//TODO:播放通关奖励和满星奖励
	const CheckPoint* pInfo = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);
	if (mCanGetBaseReward && mCanGetPerfectReward)
	{
		int giftId = pInfo->perfectBonus;
		int rank = mSelfMissionRank;
		int shareID = mShareId;
		auto func = [this, giftId, rank, shareID](){
			auto cb = [this, rank, shareID](){
				if (!this->winShare(shareID))
					this->share(rank);
			};
			BoxRewardDialog::showBoxRewardDialog(BOX_REWARD_PVE_PERFECT, giftId, cb);
		};
		BoxRewardDialog::showBoxRewardDialog(BOX_REWARD_PVE_BASE, pInfo->baseBonus, func);
	}
	else if (mCanGetBaseReward)
	{
		int rank = mSelfMissionRank;
		int shareID = mShareId;
		auto func = [this, rank, shareID](){
			if (!this->winShare(shareID))
				this->share(rank);
		};
		BoxRewardDialog::showBoxRewardDialog(BOX_REWARD_PVE_BASE, pInfo->baseBonus, func);
		//GiftAwardDialog::showDialogByGiftId(pInfo->baseBonus);
	}
	else if (mCanGetPerfectReward)
	{
		int rank = mSelfMissionRank;
		int shareID = mShareId;
		auto func = [this, rank, shareID](){
			if (!this->winShare(shareID))
				this->share(rank);
		};
		BoxRewardDialog::showBoxRewardDialog(BOX_REWARD_PVE_PERFECT, pInfo->perfectBonus, func);
		//GiftAwardDialog::showDialogByGiftId(pInfo->perfectBonus);
	}
	else
	{
		if (!winShare(mShareId))
			share(mSelfMissionRank);
	}

}

void ResultDialog::windowAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (type == cocostudio::COMPLETE)
	{
		std::string name = pAr->getName();
		if (aniName == "born" && name.compare("fla_pvpshengli_jieyin") == 0)
		{
			showEffectUI();
		}
		else 
		if (aniName == "stand")
		{
			int flag = name.find("fla_shengji_");
			if (flag > -1)
				initScore();
		}
	}
	ActionWindow::windowAniCallback(pAr, type, aniName);
}
