#include "PVPResult.h"
#include "game/ui//PageFunction.h"
#include "pvp/PvpControl.h"
#include "net/UserObj.h"
#include "SoundManager.h"
#include "data/TableManager.h"
#include "balance/BalanceControl.h"
#include "game/ui/LabelRotation.h"
#include "CCLuaEngine.h"
#include "game/utils/Comm.h"
#define CELL_HEIGH 82

PVPResult::~PVPResult()
{
	//cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_pvpshengli.xml");
	/*		cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_pingju.xml");*/
	sgMessageManager()->removeMessageHandler(EMT_SHARE_SUCCESS, this);
}


PVPResult::PVPResult() : mbResultWin(false)
{
	//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_shibai.xml");
	//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_pvpshengli.xml");
	//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_pingju.png", "fla_pingju.plist", "fla_pingju.xml");
	sgMessageManager()->regisiterMessageHandler(EMT_SHARE_SUCCESS, this);
}

void PVPResult::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	sgSoundManager()->beganBattleEffect();

	if (cName == "onBack")
	{
		this->setShow(false);
		PageFunction::Get()->gotoFunction(MSG_FUC_MAIN_LEVEL);
		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_PVP);

		return;
	}

	else if (cName == "onShare")
	{
#ifdef MGAME_PLATFORM_MPLUS
		if (mbResultWin)
		{
			std::string param = sgChannelApi()->getShareMsg("message1", "", false);
			int levelId = sgGamePrecedure()->getCurMapInfo().checkpt;
			int useTime = sgTableManager()->getCheckPointByID(levelId)->timeLimit - sgBalanceControl()->getTimeLeft();
			std::string winInfo = StringUtils::format(param.c_str(), useTime, sgPvpControl()->getKillPeople(sgUserObj()->getUin()));
			sgChannelApi()->callGameInfo(TYPE_SHARE_MSG, "message1", winInfo);
		}
		else
			sgChannelApi()->callGameInfo(TYPE_SHARE_MSG_TYPE, "3");
#else
		fgame::CSPvpEndResp* packEnd = sgPvpControl()->getEndRsp();
		int id = 0;
	
		if (packEnd->win() == sgUserObj()->getUin())
			id = PVPRESULT_WIN;
		else if (packEnd->win() == FIGHT_END_NUM)	
			id = PVPRESULT_DRAW;
		else
			id = PVPRESULT_FAIL;
#ifdef MGAME_IN_APP_SDK
		const char* pScript = __String::createWithFormat("sgShareControl():openView(%d,%d)", 3, id)->getCString();
		LuaEngine::getInstance()->executeString(pScript);
#endif // MGAME_IN_APP_SDK
		

#endif
	}
}

bool PVPResult::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PVPResult.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);
		mPanelView = dynamic_cast<ui::Layout *>(mRoot->getChildByName("PanelView"));
		mNodeResultAni = mRoot->getChildByName("NodeResultAni");
		mSpriteWinBg = dynamic_cast<Sprite*>(mRoot->getChildByName("SpriteWinBg"));
		mSpriteFailBg = dynamic_cast<Sprite*>(mRoot->getChildByName("SpriteFailBg"));

		auto spriteTitle = dynamic_cast<Sprite*>(mRoot->getChildByName("SpriteTitle"));
		mTextStreak = dynamic_cast<ui::Text*>(spriteTitle->getChildByName("TextStreak"));
		mTextWinRate = dynamic_cast<ui::Text*>(spriteTitle->getChildByName("TextWinRate"));
		//initView();
		mTextLess10 = dynamic_cast<ui::Text*>(mRoot->getChildByName("TextLess10"));
		mShareButton = dynamic_cast<ui::Button*>(mRoot->getChildByName("ButtonShare")); 
		setPopType(E_POP_TYPE_NONE);

		ret = true;	

	} while (0);


	return ret;
}

#define WIN_TAG (10)


void PVPResult::onShow()
{
#ifdef MGAME_PLATFORM_MPLUS
	this->scheduleOnce(schedule_selector(PVPResult::autoShowAd), 1.0f);
#endif

	Window::onShow();
	clearActionData();
	updateUI();


	if (sgUserObj()->getIsShowDailyMissionProgress())
	{
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eOpenMissionCompleteDialog", NULL);
		sgUserObj()->setIsShowDailyMissionProgress(false);
	}
	onLocateClickCallback(mRoot);
}

void PVPResult::clearActionData()
{
	Vector<Node*> children = mRoot->getChildren();
	for (auto child : children)
	{
		if (child->getTag() >= PVPACTIONTAG_TEXT_BEGIN)
			child->removeFromParentAndCleanup(true);
	}

}
void PVPResult::updateUI()
{
	auto nodeResultAni = mRoot->getChildByName("NodeResultAni");
	nodeResultAni->removeAllChildren();
	mTextLess10->setVisible(false);
	int roundNum = sgEnvSet("NEWER_PROTECT_LEVEL") - sgUserObj()->getPersonalInfo()->pvpround();//sgUserObj()->getRoundCount();
	if (roundNum > 1)
	{
		mTextLess10->setVisible(true);
		mTextLess10->setText(StringUtils::format(sgTextHelper(TEXT_PVP_PROTECT_ROUND10).c_str(), sgUserObj()->getPersonalInfo()->pvpround(), sgEnvSet("NEWER_PROTECT_LEVEL")));
	}
	fgame::CSPvpEndResp* packEnd = sgPvpControl()->getEndRsp();

	int iResult = PVPRESULT_FAIL;
	if (packEnd->win() == sgUserObj()->getUin())
	{
		iResult = PVPRESULT_WIN;
	}
	else if (packEnd->win() == FIGHT_END_NUM)
	{
		iResult = PVPRESULT_DRAW;
	}
	else
	{
		iResult = PVPRESULT_FAIL;
	}

	mbResultWin = (iResult == PVPRESULT_WIN);
	bool bSelfWin = (packEnd->win() == sgUserObj()->getUin() || packEnd->win() == FIGHT_END_NUM);
	if (iResult == PVPRESULT_WIN)
	{
		mSpriteWinBg->setVisible(true);
		mSpriteFailBg->setVisible(false);
	}
	else
	{
		sgSoundManager()->playMusicEffect(SOUND_UI_FAIL);
		mSpriteWinBg->setVisible(false);
		mSpriteFailBg->setVisible(true);
	}
	
	if (sgPvpControl()->getPVPMode() == PVP_MODE_ROOM )
	{
		mTextWinRate->setVisible(true);
		mTextStreak->setVisible(false);
	}
	else if (sgPvpControl()->getPVPMode() == PVP_MODE_RAND || sgPvpControl()->getPVPMode() == PVP_MODE_NOITEM)
	{
		mTextWinRate->setVisible(false);
		mTextStreak->setVisible(true);
	}
	if (packEnd->shareid()>0)
	{
#ifdef MGAME_IN_APP_SDK
		const char* pScript = __String::createWithFormat("sgShareControl():openView(%d,%d)", 1, packEnd->shareid())->getCString();
		LuaEngine::getInstance()->executeString(pScript);
#endif // MGAME_IN_APP_SDK
	}


	cocostudio::Armature* armPVPResult = nullptr;
	cocostudio::Armature* armPVPResultAdd = nullptr;
	if (iResult == PVPRESULT_WIN)
	{
		armPVPResult = cocostudio::Armature::create("fla_pvpshengli");
		armPVPResultAdd = cocostudio::Armature::create("fla_pvpshengli_ADDITIVE");

	}
	else if (iResult == PVPRESULT_FAIL)
	{
		armPVPResult = cocostudio::Armature::create("fla_shibai1");
		armPVPResultAdd = cocostudio::Armature::create("fla_shibai1_ADDITIVE");
	}
	else if (iResult == PVPRESULT_DRAW)
	{
		armPVPResult = cocostudio::Armature::create("fla_shibai2");
		armPVPResultAdd = cocostudio::Armature::create("fla_shibai1_ADDITIVE");
	}
	if (armPVPResult && armPVPResultAdd)
	{
		armPVPResult->getAnimation()->play("born");
		nodeResultAni->addChild(armPVPResult);
		armPVPResultAdd->setBlendFunc(BlendFunc::COMADDITIVE);
		armPVPResultAdd->getAnimation()->play("born");
		nodeResultAni->addChild(armPVPResultAdd);

		armPVPResult->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID) {
			if (movementType == cocostudio::MovementEventType::COMPLETE)
			{
				armature->getAnimation()->play("stand");
			}
		});

		armPVPResultAdd->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID) {
			if (movementType == cocostudio::MovementEventType::COMPLETE)
			{
				armature->getAnimation()->play("stand");

			}
		});

	}

	////////////////////// 显示获得袜子, add by jim
	/// 圣诞活动获得袜子
	// CCLOG("   PVPResult  ============= updata size -- %d", packEnd->updatedata_size());
	for (int i = 0; i < packEnd->updatedata_size(); i++) {
		const fgame::PropertyUpdateData &data = packEnd->updatedata(i);
		auto itemId = data.value2();//add by crystal
		if (itemId < 80001)
			continue;
		if (data.has_value()) {
			for (int j = 0; j < data.value(); j++) {
				const PropInfo* pPorp = sgTableManager()->getPropInfoById(80001);
				if (pPorp)
				{
					auto pItemIcon = Sprite::createWithSpriteFrameName(pPorp->icon.c_str());
					pItemIcon->setScale(.5f);
					Size winSize = Director::getInstance()->getWinSize();
					sgPageFunction()->getActionPackageLayer()->addItem(pItemIcon, Vec2(winSize.width / 2 - 197, winSize.height / 2 + 19), 0, 0);
				}
			}
		}
	}
	//////////////////////

		////////////////////// 显示获得玫瑰, add by crystal
	/// 情人节活动获得玫瑰 有可能同一种有多个  
	auto packerLayer = sgPageFunction()->getActionChestLayer();
	for (int i = 0; i < packEnd->updatedata_size(); i++) {
		const fgame::PropertyUpdateData &data = packEnd->updatedata(i);
		auto itemId = data.value2();
		if ((itemId < 40001) || (itemId > 40003))
			continue;

		if (data.has_value()) {
			const PropInfo* pPorp = sgTableManager()->getPropInfoById(itemId);
			Size winSize = Director::getInstance()->getWinSize();
			auto roseNode = CSLoader::createNode("ui/RoseNode.csb");

			auto img = dynamic_cast<ImageView*>(roseNode->getChildByName("Img"));
			img->loadTexture(pPorp->icon, Widget::TextureResType::PLIST);
			dynamic_cast<Text*>(roseNode->getChildByName("Text_count"))->setString(Value(int(data.value())).asString());

			packerLayer->addItem(roseNode, Vec2(winSize.width / 2 - 197, winSize.height / 2 + 19), 0, 0,true,false);
		}
	} 
	//////////////////////


	mPanelView->removeAllChildren();
	int height = mPanelView->getContentSize().height;
	for (int idx = 0; idx < packEnd->result_size(); idx++)
	{
		PVPResultCell* cell = PVPResultCell::create();
		mPanelView->addChild(cell);
		cell->setPosition(Vec2(5, height - (idx + 1) *CELL_HEIGH));
		cell->updateCell(idx);
	}


}


void PVPResult::onClose()
{
	Window::onClose();
}

#ifdef MGAME_PLATFORM_MPLUS
void PVPResult::autoShowAd(float dt)
{
	sgMplusControl()->getPvpCanShowAd();
}



#endif

void PVPResult::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_SHARE_SUCCESS)
	{
			if (mShareButton)
			{
				mShareButton->setBright(false);
				mShareButton->setEnabled(false);
				mShareButton->setTouchEnabled(false);
			}
	}
}

PVPResultCell::PVPResultCell() :mRoot(nullptr), mTextActLabel(nullptr), mPrevSession(0), mCurSession(0)
{

}

PVPResultCell::~PVPResultCell()
{

}

bool PVPResultCell::init()
{
	bool ret = false;
	do {
		mRoot = CSLoader::getInstance()->createNode("ui/PVPResultCell.csb");
		this->addChild(mRoot);
		initUI();
		ret = true;
	} while (0);

	return ret;
}

void PVPResultCell::initUI()
{
	mSpriteSelfWinBg = dynamic_cast<Sprite *>(mRoot->getChildByName("SpriteSelfWinBg"));
	mSpriteSelfFailBg = dynamic_cast<Sprite *>(mRoot->getChildByName("SpriteSelfFailBg"));
	mSpriteDownFlag = dynamic_cast<Sprite *>(mRoot->getChildByName("SpriteDownFlag"));
	mSpriteUpFlag = dynamic_cast<Sprite *>(mRoot->getChildByName("SpriteUpFlag"));
	mSpriteZongJie = dynamic_cast<Sprite *>(mRoot->getChildByName("SpriteZongJie"));

	mTextStreak = dynamic_cast<ui::Text *>(mRoot->getChildByName("TextStreak"));

	mNodeGlory = mRoot->getChildByName("NodeGlory");
	mNodeGloryPosY = mNodeGlory->getPositionY();


	mTextGloryName = dynamic_cast<ui::Text *>(mNodeGlory->getChildByName("TextGloryName"));
	mTextGlory = dynamic_cast<ui::Text *>(mNodeGlory->getChildByName("TextGlory"));

	mNodeScore = mRoot->getChildByName("NodeScore");
	mNodeScorePosY = mNodeScore->getPositionY();

	mTextScoreName = dynamic_cast<ui::Text *>(mNodeScore->getChildByName("TextScoreName"));
	mTextScore = dynamic_cast<ui::Text *>(mNodeScore->getChildByName("TextScore"));
	

	mTextRank = dynamic_cast<ui::Text *>(mRoot->getChildByName("TextRank"));

	mTextName = dynamic_cast<ui::Text *>(mRoot->getChildByName("TextName"));
#ifdef MGAME_PLATFORM_MPLUS
	if (mTextName) {
		mTextName->setFontName(DEFAULT_FONT);
	}
#endif
	mSpriteFangZhu = dynamic_cast<Sprite *>(mRoot->getChildByName("SpriteFangZhu"));

	mNodeHead = mRoot->getChildByName("NodeHead");

	mNodeRankSection = mRoot->getChildByName("NodeRankSection");

	mTextActLabel = dynamic_cast<ui::Text *>(mRoot->getChildByName("TextActDes"));

	mVipNode = dynamic_cast<Node *>(mRoot->getChildByName("node_vip"));

	mVipIcon = dynamic_cast<Node *>(mVipNode->getChildByName("icon"));

	mVipHonor = dynamic_cast<Text *>(mVipNode->getChildByName("text_honor"));

	mSpriteSection = dynamic_cast<Sprite *>(mRoot->getChildByName("SpriteSection"));
	mSpriteSection->setVisible(false);

}

void PVPResultCell::updateCell(int idx)
{

	fgame::CSPvpEndResp* packEnd = sgPvpControl()->getEndRsp();

	if (idx < 0 || idx >= packEnd->result_size()) return;

	fgame::CSPlayerPvpResult playInfo = packEnd->result(idx);
	
	int uin = playInfo.uin();
	
	int iResult = PVPResult::PVPRESULT_FAIL;
	if (packEnd->win() == uin)
	{
		iResult = PVPResult::PVPRESULT_WIN;
	}
	else if (packEnd->win() == FIGHT_END_NUM)
	{
		iResult = PVPResult::PVPRESULT_DRAW;
	}
	else
	{
		iResult = PVPResult::PVPRESULT_FAIL;
	}

	if (uin == sgUserObj()->getUin())
	{
		const char* pScript = __String::createWithFormat("sgShareControl():setWinCount(%d)",playInfo.wincount())->getCString();
		LuaEngine::getInstance()->executeString(pScript);
	}

	mSpriteSelfWinBg->setVisible(false);
	mSpriteSelfFailBg->setVisible(false);

	mSpriteFangZhu->setVisible(false);

	mTextStreak->setColor(Color3B::WHITE);
	mTextGlory->setColor(Color3B::WHITE);
	mTextScore->setColor(Color3B::WHITE);
	mTextRank->setColor(Color3B::WHITE);

	mSpriteDownFlag->setVisible(false);
	mSpriteUpFlag->setVisible(false);
	mSpriteZongJie->setVisible(false);

	std::string stHonourPoint = "";
	bool bExtraFlag = false;   // 是否有疯狂周的额外的荣誉奖励
	if (playInfo.has_extrascore() && playInfo.extrascore() > 0) {
		//stHonourPoint = __String::createWithFormat(sgTextHelper(TEXT_PVPRESULT_ADDHONOR).c_str(), playInfo.extrascore())->getCString();
		stHonourPoint = sgTextHelper(TEXT_PVPRESULT_ADDHONOR);
		//CCLOG("[jim]  stHonourPoint ======== %s", stHonourPoint.c_str());
		bExtraFlag = true;
	}

	mTextName->setText(playInfo.name());

#ifdef MGAME_PLATFORM_MPLUS
	//vip 荣誉加成
	int vipLevel = playInfo.viplevel();
	if ( vipLevel > 0 && vipLevel <= 3)
	{
		mVipNode->setVisible(true);
		if (playInfo.addscore() > 0)
		{
			mVipIcon->removeAllChildren();
			Sprite * left = Sprite::createWithSpriteFrameName(StringUtils::format("ui/general/general_vip%d.png", vipLevel));
			Sprite * right = Sprite::createWithSpriteFrameName(StringUtils::format("ui/general/general_vip%d.png", vipLevel));
			left->setPosition(-(mTextName->getContentSize().width ) / 2 - 20, 0);
			right->setFlippedX(true);
			right->setPosition(mTextName->getContentSize().width/2 + 20, 0);
			mVipIcon->addChild(left);
			mVipIcon->addChild(right);
		}
		else
			mVipHonor->setVisible(false);
	}
	else
		mVipNode->setVisible(false);
#else
	mVipNode->setVisible(false);
#endif

	//段位图标显示
	const GradeData* pCurGradeData = sgTableManager()->getGradeData(playInfo.prevpoint() + playInfo.addpoint());
	if (pCurGradeData)
	{
		int iCurGrade = pCurGradeData->getID();
		mSpriteSection->setSpriteFrame(StringUtils::format("ui/Icon2/ranking_Emblem-%02d.png", iCurGrade));
		mSpriteSection->setScale(0.5);
		mSpriteSection->setVisible(true);
	}

	int PvpBatMode = sgPvpControl()->getPVPMode();

	if (uin == sgUserObj()->getUin())
	{	
		sgPvpControl()->setLastOwnerIdx(idx);

		if (iResult == PVPResult::PVPRESULT_WIN)
		{
			mSpriteSelfWinBg->setVisible(true);

			mTextStreak->setColor(Color3B::GREEN);
			mTextGlory->setColor(Color3B::GREEN);
			mTextScore->setColor(Color3B::GREEN);
			mTextRank->setColor(Color3B::GREEN);
		}
		else
		{
			mSpriteSelfFailBg->setVisible(true);

			mTextStreak->setColor(Color3B::RED);
			mTextGlory->setColor(Color3B::RED);
			mTextScore->setColor(Color3B::RED);
			mTextRank->setColor(Color3B::RED);
		}
		if (PvpBatMode == PVP_MODE_RAND || PvpBatMode == PVP_MODE_NOITEM)
		{
			int rank = sgUserObj()->getPersonalInfo()->rank();
			//if (playInfo.newrank() != 0 && playInfo.newrank() < sgUserObj()->getRanking())
			if (playInfo.newrank() != 0 && playInfo.newrank() < rank)
			{
				mSpriteUpFlag->setVisible(true);
			}
			//else if (playInfo.newrank() > sgUserObj()->getRanking() && sgUserObj()->getRanking() != 0)
			else if (playInfo.newrank() > rank && rank != 0)
			{
				mSpriteDownFlag->setVisible(true);
			}
		}
	}
	else
	{
		if (playInfo.newrank() != 0)
		{
			if (PvpBatMode == PVP_MODE_RAND || PvpBatMode == PVP_MODE_NOITEM)
			{
				if (iResult == PVPResult::PVPRESULT_WIN)
					mSpriteUpFlag->setVisible(true);
				else
					mSpriteDownFlag->setVisible(true);
			}
		}
	}

	std::string text = "";
	if (PvpBatMode == PVP_MODE_ROOM)
	{
		mNodeScore->setVisible(false);

		mTextStreak->setText(__String::createWithFormat("%d%%", playInfo.winrate())->getCString());

		mNodeGlory->setPositionY((mNodeGloryPosY + mNodeScorePosY) / 2);
		mTextGloryName->setText(sgTextHelper(TEXT_PVPRESULT_ADDGLORY));
		text = __String::createWithFormat("%d", playInfo.addscore())->getCString();
		mTextGlory->setText(text);

		if (uin == sgPvpControl()->getMatchRsp()->opener())
			mSpriteFangZhu->setVisible(true);

	}
	else if (PvpBatMode == PVP_MODE_RAND || PvpBatMode == PVP_MODE_NOITEM)
	{
		mNodeScore->setVisible(true);
		if (playInfo.addpoint() == 0)
		{
			mNodeScore->setVisible(false);
			if (PvpBatMode == PVP_MODE_RAND)
			{
				mNodeGlory->setPositionY((mNodeScorePosY + mNodeGloryPosY) / 2);
			}
		}
		else if (playInfo.addpoint() < 0)
		{
			mTextScoreName->setText(sgTextHelper(TEXT_PVPRESULT_SUBSCORE));
			text = __String::createWithFormat("%d", abs(playInfo.addpoint()))->getCString();
			mTextScore->setText(text);

		}
		else if (playInfo.addpoint() > 0)
		{
			mTextScoreName->setText(sgTextHelper(TEXT_PVPRESULT_ADDSCORE));
			text = __String::createWithFormat("%d", playInfo.addpoint())->getCString();
			mTextScore->setText(text);

		}

		mTextGloryName->setText(sgTextHelper(TEXT_PVPRESULT_ADDGLORY));
		text = __String::createWithFormat("%d", playInfo.addscore())->getCString();
		mTextGlory->setText(text);

		if (PvpBatMode == PVP_MODE_NOITEM)
		{
			mNodeGlory->setVisible(false);
		}

		if (iResult == PVPResult::PVPRESULT_WIN)
			mTextStreak->setText(__String::createWithFormat("%d", playInfo.wincount() + 1)->getCString());
		else
		{
			mSpriteZongJie->setVisible(true);
			mTextStreak->setText(__String::createWithFormat("%d", playInfo.wincount())->getCString());
		}

	}
	if (playInfo.newrank() > 0)
	{
		mTextRank->setText(__String::createWithFormat("%d", playInfo.newrank())->getCString());
	}
	else
	{
		mTextRank->setText(sgTextHelper(TEXT_PVPRESULT_NORANK));
	}

	
	if (uin == sgUserObj()->getUin() && iResult == PVPResult::PVPRESULT_WIN)
	{
		if (playInfo.addpoint() > 0)
		{
			LabelRotation* label = LabelRotation::create(mTextScore);
			CCPoint pos = mTextScore->getPosition();
			label->setPosition(ccp(pos.x + 5, pos.y));
			mTextScore->getParent()->addChild(label);
			mTextScore->setVisible(false);
		}
		
		if (playInfo.addscore() > 0)
		{
			LabelRotation* label = LabelRotation::create(mTextGlory);
			CCPoint pos = mTextGlory->getPosition();
			label->setPosition(ccp(pos.x + 5, pos.y));
			mTextGlory->getParent()->addChild(label);
			mTextGlory->setVisible(false);
		}

		mvTextTips.clear();
		text = "";
		if (uin == sgPvpControl()->getMatchRsp()->opener())
		{
			text = sgTextHelper(TEXT_PVPRESULT_OPNER_AWARD);
			mvTextTips.insert(std::make_pair(text, mNodeScore->getPosition()));
		}
		if (playInfo.extrascore() > 0)
		{
			text = sgTextHelper(TEXT_PVPRESULT_ADDHONOR);
			mvTextTips.insert(std::make_pair(text, mNodeScore->getPosition()));

		}
#ifdef MGAME_PLATFORM_MPLUS
		if (playInfo.addscore() > 0 && vipLevel > 0 && vipLevel <= 3)
		{
			text = sgTextHelper(TEXT_PVPRESULT_VIPADD);
			mvTextTips.insert(std::make_pair(text, mNodeScore->getPosition()));
		}
#endif
		if (PvpBatMode == PVP_MODE_RAND || PvpBatMode == PVP_MODE_NOITEM)
		{
			text = sgTextHelper(TEXT_PVPRESULT_STREAKADD);
			mvTextTips.insert(std::make_pair(text, mTextStreak->getPosition()));
		}
		if (mvTextTips.size() > 0)
			showTips();
	}

	mNodeHead->removeAllChildren();
	cocostudio::Armature* armBaoZi = nullptr;
	if (packEnd->win() == uin)
	{
		armBaoZi = cocostudio::Armature::create("fla_pvpshengli_baozi");
	}
	else
	{
		armBaoZi = cocostudio::Armature::create("fla_pvpshengli_baozi2");
	}
	if (armBaoZi)
	{
		armBaoZi->setScale(0.7);
		int type = (int)sgPvpControl()->getCampTypeByUid(uin) - ECAMP_TYPE_AI3;
		std::string str = "";
		for (int i = 1; i <= 4; i++)
		{
			str = StringUtils::format("guge_pvpshengli_toujin%d", (int)i);
			auto node = armBaoZi->getBone(str);
			if (!node) continue;

			if (type != i)
			{
				node->changeDisplayWithIndex(-1, true);
			}
		}
		armBaoZi->getAnimation()->play("play");
		mNodeHead->addChild(armBaoZi);
	}
}

void PVPResultCell::showTips()
{
	int dt = 0;
	int num = 0;
	std::map<std::string, Point>::iterator iter = mvTextTips.begin();
	for (; iter != mvTextTips.end(); iter++)
	{
		Text* pText = Text::create();
		pText->setFontSize(30);
		pText->setFontName("font.TTF");
		pText->setTextColor(Color4B(111, 254, 22, 255));
		Vec2 world = this->convertToWorldSpace(iter->second);

		Vec2 pPoint = this->getParent()->getParent()->convertToNodeSpace(world);
		pText->setPosition(pPoint);

		//阴影,暂不用参数控制开关
		//pText->enableShadow(Color4B::BLACK, Size(1, -1));
		Sprite* pSpriteArrow = Sprite::create("ui/battle/Battle_arrow.png");
		pSpriteArrow->setPosition(pPoint);
		pText->setString(iter->first);
		pText->setPositionX(pText->getPositionX() - pSpriteArrow->getContentSize().width / 2);
		pSpriteArrow->setPositionX(pText->getPositionX() + (pText->getContentSize().width + pSpriteArrow->getContentSize().width) / 2 + 5);
		this->getParent()->getParent()->addChild(pText);
		pText->setTag(PVPResult::PVPACTIONTAG_TEXT_BEGIN + num);

		this->getParent()->getParent()->addChild(pSpriteArrow);
		pSpriteArrow->setTag(PVPResult::PVPACTIONTAG_ARROW_BEGIN + num);
		pText->runAction(Sequence::create(
			FadeOut::create(0),
			DelayTime::create(dt),
			Spawn::create(MoveBy::create(0.7, Vec2(0, 30)), FadeIn::create(0.7), NULL),
			MoveBy::create(1.0, Vec2(0, 60)),
			Spawn::create(MoveBy::create(0.7, Vec2(0, 30)), FadeOut::create(0.7), NULL),
			RemoveSelf::create(),
			NULL));
		pSpriteArrow->runAction(Sequence::create(
			FadeOut::create(0),
			DelayTime::create(dt),
			Spawn::create(MoveBy::create(0.7, Vec2(0, 30)), FadeIn::create(0.7), NULL),
			MoveBy::create(1.0, Vec2(0, 60)),
			Spawn::create(MoveBy::create(0.7, Vec2(0, 30)), FadeOut::create(0.7), NULL),
			RemoveSelf::create(),
			NULL));
		dt = dt + 1;
		num = num + 1;
	}
}
