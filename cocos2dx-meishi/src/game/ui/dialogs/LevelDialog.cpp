#include "LevelDialog.h"
#include "game/ui//PageFunction.h"
#include "game/data/Tables.h"
#include "game/data/TableManager.h"
#include "guide/GuideManager.h"
#include "EnvSet.h"
#include "game/balance/BalanceControl.h"
#include "game/balance/StarModes.h"
#include "prop/PropItem.h"
#include "prop/PropsControl.h"
#include "MessageManager.h"
#include "battle/BattleControl.h"
#include "stl_helper.h"
#include "game/net/UserObj.h"
#include "SoundManager.h"
#include "battle/BattleRecordsManager.h"
#include "shop/ShopPanel.h"
#include "game/ui/RewardDisplay.h"
#include "lua/LuaFuncHelper.h"
#include "game/utils/GameDefault.h"
#include "utils/HeadIconUtils.h"

void LevelDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
	
	if(mAnimationFinish == false)
		return;
	if(sgGuideManager()->getGuideLock() == true)
	{
		return;
	}
	if (cName == "onGame")
	{	
		sgSoundManager()->playMusicEffect(SOUND_CLICK_BATTLE);
		onGame(nullptr);
	}
	else if (cName == "onBuyCoin")
	{
	}
	else if (cName == "onEasy")
	{
		//selectLevelGrade(LEVEL_EASY);

	}else if (cName == "onClose")
	{
		this->setShow(false);
		playAniBornOrEnd(false);

	}
	else if (cName == "onProp1")
	{
		sgGuideManager()->unRegistGuideMessage();
		showAddPropPanel(1);
	}
	else if (cName == "onProp2")
	{
		showAddPropPanel(2);
	}
	else if (cName == "onProp3")
	{
		showAddPropPanel(3);
	}
	else if (cName == "onAutoSelect")
	{
		const PropInfo* pInfo = getNextPropInfo();
		while (pInfo && checkNextPropPos())
		{
			addPropView(pInfo);
			pInfo = getNextPropInfo();
		}

	}
	else if (cName == "onBaseReward")
	{
		showReward(RewardDisplay::MISSION);

	}
	else if (cName == "onPeffectReward")
	{
		showReward(RewardDisplay::STAR);
	}
	else if (cName == "onDisplay")
	{
		if (mRewardDisplay)
		{
			mRewardDisplay->setVisible(false);
		}
	}
	else if (cName == "onRecord")
	{
		//战斗回放
		bool isExist = sgBattleRecordsManager()->loadBattleRecords(checkpt);

		if (!isExist)
		{
			std::string szTitle = sgTextHelper(TEXT_COMMTIPS_TIPS);
			std::string szContent = sgTableManager()->getTextHelperByID(13012)->text;
			GenerlDialog::show(szTitle, szContent);	
		}
		else
		{
			if (mIsPass)
			{
				sgBattleRecordsManager()->setIsPlayback(true);
				sgBattleRecordsManager()->setLastPlaybackPoint(checkpt);

				sgPageFunction()->setCurrentUIType(MSG_FUC_MINI_LEVEL);
				sgPageFunction()->gotoFunction(MSG_FUC_PLAYING);

				playAniBornOrEnd(false);

				setShow(false);
			}
			else
			{
				char buff[128];
				memset(buff, 0, 128);

				sprintf(buff, sgTextHelper(14007).c_str(), sgTableManager()->getCheckPointByID(checkpt)->failCount);
				showTextTip(buff);
				return;
			}
		}
	}
	else if (cName == "onRankIcon")
	{
		auto endPos = dynamic_cast<ImageView*>(sender)->getTouchEndPosition();
		if (abs(mTouchBeginPos.x - endPos.x) < 20 && abs(mTouchBeginPos.y - endPos.y) < 30)
		{
			int uin = ((Node*)sender)->getTag();
			sgUserObj()->setCurInfoUin(uin);
			sgUserObj()->setCurInfoType(0);
			sgPageFunction()->gotoFunction(MSG_FUC_INFO);
		}
	}
	else if (cName == "onSkip")
	{
		sgUserObj()->onSkip(mCurLevelId);
	}
}

void LevelDialog::onShow()
{
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_USER, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_USE_ALL, this);
	sgMessageManager()->regisiterMessageHandler(EMT_SKIP_SUCCESS, this);

	LuaEngine* scriptEngine = dynamic_cast<LuaEngine*>(ScriptEngineManager::getInstance()->getScriptEngine());
	LuaStack* stack = scriptEngine->getLuaStack();

	bool starStatus = false;
	bool checkPtStatus = false;
	int	 missionId = 0;
	int  chapterId = 0;
	auto callback = [&starStatus, &checkPtStatus, &missionId, &chapterId](lua_State* tolua_S, int argc){
		if (argc >= 4)
		{
			if (lua_isnumber(tolua_S, -4))
				missionId = lua_tointeger(tolua_S, -4);

			if (lua_isnumber(tolua_S, -3))
				chapterId = lua_tointeger(tolua_S, -3);

			if (lua_isboolean(tolua_S, -2))
				checkPtStatus = lua_toboolean(tolua_S, -2);

			if (lua_isboolean(tolua_S, -1))
				starStatus = lua_toboolean(tolua_S, -1);
		}
	};

	auto luaFuncHandler = sgLuaFuncHandlerManager()->getHandler(LuaFuncHandlerManager::ModuleType::kCustom, (int)LuaFuncHandlerManager::CustomHandlerType::kGetMissionData);
	if (luaFuncHandler > 0)
	{
		auto tolua_S = stack->getLuaState();

		LuaStatePrint(tolua_S);
		auto rlt = stack->executeFunction(luaFuncHandler, 0, 4, callback);
		LuaStatePrint(tolua_S);
		stack->clean();

		//TODO:需要有礼包状态的动画 现在设置为不可见
		const CheckPoint* pInfo = sgTableManager()->getCheckPointByID(missionId);
		if (pInfo->baseBonus == 0)
		{
			mBaseReward->setVisible(false);
		}
		else
		{
			mBaseReward->setVisible(true);
			mBaseReward->setEnabled(!checkPtStatus);
			mBaseReward->setBright(!checkPtStatus);
		}
		if (pInfo->perfectBonus == 0)
		{
			mPeffectReward->setVisible(false);
		}
		else
		{
			mPeffectReward->setVisible(true);
			mPeffectReward->setEnabled(!starStatus);
			mPeffectReward->setBright(!starStatus);
		}
		sgGamePrecedure()->selectLevel(missionId);
		mCurLevelId = missionId;
	}

	m_pChangePowerListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener("eBthChangePower", CC_CALLBACK_1(LevelDialog::onGame, this));
	
	mPropViewList->setisShowArrowDesc(true);
	playAniBornOrEnd(true);
	mBtnBegin->setEnabled(true);
	mBtnSkip->setEnabled(true);
	mBtnBegin0->setEnabled(true);
	mCloseBtn->setEnabled(true);
	mAutoSelect->setVisible(false);
	const GameInfo info = sgGamePrecedure()->getCurMapInfo();
	const CheckPoint* pCP = sgTableManager()->getCheckPointByID(info.checkpt);
	if (!pCP)
		return;
	
	
	char szText[32] = { 0, };
	int consumeCount = sgGuideManager()->checkNeedPower(info.checkpt) == true ? pCP->consumeEnergy : 0;
	sprintf(szText, "%d", consumeCount);
	mPower->setString(szText);

	int id = pCP->checkType + 100;
	const TextHelper* pText = sgTableManager()->getTextHelperByID(id);
	if (pText){
		mPassText->setText(pText->text);
	}
	//关卡名
	mTitleText->setString(pCP->passName);

	//缩略图-----------------
	createScaleMap(pCP);

	//// 道具
	//PropItem* pNode1 = m_PropNodeVec[0];
	//pNode1->updateUI(ePropType(pCP->tool1));

	//PropItem* pNode2 = m_PropNodeVec[1];
	//pNode2->updateUI(ePropType(pCP->tool2));

	//PropItem* pNode3 = m_PropNodeVec[2];
	//pNode3->updateUI(ePropType(pCP->tool3));


	addMissionTarget();
	initPropPanel();

	bool showProp = sgGuideManager()->checkOpenPropControl(sgGamePrecedure()->getCurMapInfo().checkpt);
	showPropPanel(showProp);


	EventListenerTouchOneByOne* _touchListener = EventListenerTouchOneByOne::create();
	_touchListener->onTouchBegan = CC_CALLBACK_2(LevelDialog::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);

	int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
	unsigned int uin = sgUserObj()->getUin();
	char buff[32];
	memset(buff, 0, 32);
	sprintf(buff, "%d_%d%s", uin, checkpt, "failCount");
	int failCount = USER_DEFAULT_INT(buff);

	char buff_tryCount[32];
	memset(buff_tryCount, 0, 32);
	sprintf(buff_tryCount, "%d_%d%s", uin, checkpt, "tryCount");
	int tryCount = USER_DEFAULT_INT(buff_tryCount);
	//eCheckPointState levelState = (eCheckPointState)sgUserObj()->getCheckPointState(checkpt);
	eCheckPointState levelState = (eCheckPointState)sgUserObj()->getGateState(checkpt);
	mIsPass = (levelState == E_CHECKPOINTSTATE_PASS) || (failCount >= pCP->failCount) || (tryCount >= pCP->tryCount);

	/*
	if (mIsPass)	//通关，或失败x次
	{
		mBattleRecord->setTouchEnabled(true);
		mBattleRecord->setBright(true);
	}
	else
	{
		mBattleRecord->setBright(false);
	}
	*/


	//当前关卡是否通关，并且失败次数满足条件 add by crystal 2016/3/7
	const CheckPoint* p_Info = sgTableManager()->getCheckPointByID(mCurLevelId);
	if ((sgUserObj()->getGateState(mCurLevelId) == eCheckPointState::E_CHECKPOINTSTATE_OPEN) && (p_Info->isSkip == 1) && !(failCount < p_Info->menuSkipCount))
	{
		mBtnSkip->setVisible(true);
		mBtnBegin0->setVisible(true);
		mBtnBegin->setVisible(false);

		auto skipValue = dynamic_cast<ui::Text*>(mBtnSkip->getChildByName("Text_1"));
		int hasSkipCount = sgUserObj()->getHasSkipCount();
		int skipSize = p_Info->skipValueList.size();
		skipValue->setString(p_Info->skipValueList.at((hasSkipCount < skipSize) ? hasSkipCount : (skipSize-1)));

		dynamic_cast<ui::Text*>(mBtnBegin0->getChildByName("Text_3"))->setString(mPower->getString());
	}
	else
	{
		mBtnSkip->setVisible(false);
		mBtnBegin0->setVisible(false);
		mBtnBegin->setVisible(true);
	}

	showRankList(missionId);
}

void LevelDialog::playAniBornOrEnd(bool bBorn)
{
	if (bBorn)
	{
		if (mArmDaoJuGroup)
			mArmDaoJuGroup->getAnimation()->play("born");
	}
	else
	{
		if (mPropViewList->isVisible() && mArmLanTiao)
			mArmLanTiao->getAnimation()->play("end");
		if (mArmDaoJuGroup)
			mArmDaoJuGroup->getAnimation()->play("end");
		if (mAutoSelect->isVisible() && mArmAutoSelect)
		{
			mArmAutoSelect->getAnimation()->play("end");
			mArmAutoSelect->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID) {
				if (movementType == cocostudio::MovementEventType::COMPLETE)
				{
					mAutoSelect->setVisible(false);
				}
			});
		}
	}
}

void LevelDialog::createGuideNode()
{ 
	
	const GameInfo info = sgGamePrecedure()->getCurMapInfo();
	const CheckPoint* pCP = sgTableManager()->getCheckPointByID(info.checkpt);
	Node* tParentNode = dynamic_cast<Node* >(getParent()); 

	if (!mArmatureBg)
		return;
	Bone* pStartBone = mArmatureBg->getBone("guge_kaishi"); 
	if (!pStartBone)
		return;
	
	
	Point startPos = mRoot->convertToWorldSpace(mBtnBegin->getPosition()) + ccp(pStartBone->getWorldInfo()->x, pStartBone->getWorldInfo()->y);
	Node* pNode1 = getChildByTag(E_GuideTag_LevelStargBtn);
	if(!pNode1)
	{
		pNode1 = Node::create();
		addChild(pNode1); 
		pNode1->setTag(E_GuideTag_LevelStargBtn);	 
		pNode1->setName("E_GuideTag_LevelStargBtn");
	} 
	pNode1->setPosition(startPos - mBtnBegin->getContentSize() * 0.5f);

	if (!mArmDaoJuGroup)
		return;
	Bone* pPropBone = mArmDaoJuGroup->getBone("guge_daoju");
	if (!pPropBone)
		return;

	Point pPropPos1 = pPropBone->convertToWorldSpace(mNodeDaoJu->getChildByTag(668)->getPosition());
	Node* pNode2 = getChildByTag(E_GuideTag_LevelPropOneBtn);
	if(!pNode2)
	{
		pNode2 = Node::create();
		addChild(pNode2);
		pNode2->setTag(E_GuideTag_LevelPropOneBtn);
		pNode2->setName("E_GuideTag_LevelPropOneBtn");
	} 
	pNode2->setPosition(pPropPos1); 

	
	Node* pNode3 = getChildByTag(E_GuideTag_LevelPropOKBtn);
	if(!pNode3)
	{
		pNode3 = Node::create();
		addChild(pNode3);
		pNode3->setTag(E_GuideTag_LevelPropOKBtn);
		pNode3->setName("E_GuideTag_LevelPropOKBtn");
	} 					
	if (!getPropViewList() || !getPropViewList()->getGuideNode())
		return;

	Point propViewPos = getPropViewList()->convertToWorldSpace(getPropViewList()->getGuideNode()->getPosition()); 
	Size viewSize = getPropViewList()->getContentSize();
	propViewPos = getPropViewList()->convertToNodeSpace(propViewPos) + getPropViewList()->getContentSize() * 0.5f;
	pNode3->setPosition(ccp(69,324));  

	Node* pNode4 = getChildByTag(E_GuideTag_LevelPanel); 
	if(!pNode4)
	{
		pNode4 = Node::create();
		addChild(pNode4);
		pNode4->setTag(E_GuideTag_LevelPanel);
		pNode4->setName("E_GuideTag_LevelPanel");
	} 

	if (!mNodeDaoJu)
		return;
	Node* pNodePanel = mNodeDaoJu->getChildByTag(E_GuideTag_LevelPanel); 	
	pNode4->setPosition(pNodePanel->convertToWorldSpace(mNodeDaoJu->getPosition()));
	
	int gateID = sgGamePrecedure()->getCurMapInfo().checkpt;
	int level = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel();
	Size winSize = Director::getInstance()->getWinSize();
	sgGuideManager()->registGuideMessage(gateID, level, E_GUIDELOC_LEVELDIALOG, this, this, false, -0.5f * winSize);
	bool showProp = sgGuideManager()->checkOpenPropControl(sgGamePrecedure()->getCurMapInfo().checkpt);
	showPropPanel(showProp);
	return;
	 
}

void LevelDialog::onClose()
{
	Director::getInstance()->getEventDispatcher()->removeEventListener(m_pChangePowerListener);

	_eventDispatcher->removeEventListenersForTarget(this);
	this->unschedule("LevelDialog::updateBegin");
	
	if (sgGamePrecedure()->getEnterGame() == false)
	{
		ActionWindow::onClose();
	}
	//SAFE_RELEASE_VECTOR(m_PropNodeVec);

	if (mRewardDisplay)
		mRewardDisplay->setVisible(false);
}

bool LevelDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::createNode("ui/LevelDialog.csb");
		CC_BREAK_IF(!mRoot);

		setAnimation("fla_mianban");

		mRoot->setAnchorPoint(Vec2(0.5, 0.5));


		this->addChild(mRoot);

		for (int i = ePropPos::E_POS_LEFT; i <= ePropPos::E_POS_RIGHT; i++)
		{
			mPropList.insert(std::make_pair((ePropPos)i, 0));
		}


		mPropViewList = PropViewList::create();
		if (mPropViewList)
		{
			auto mPropViewNode = mRoot->getChildByName("mPropViewNode");
			mPropViewNode->addChild(mPropViewList);
			auto nodeMid = mRoot->getChildByName("NodeMid");
			mArmLanTiao = cocostudio::Armature::create("fla_lantiao");
			auto lanTiaoBone = mArmLanTiao->getBone("guge_lantiao");
			lanTiaoBone->addDisplay(mPropViewNode,0);
			nodeMid->addChild(mArmLanTiao);
			mPropViewList->setVisible(false);
			mPropViewList->setisShow(false);
		}

		sgPropsControl()->clean();
		sgPropsControl()->initSaveProp();

		mCloseBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("mCloseBtn"));
		mBtnBegin = dynamic_cast<ui::Button*>(mRoot->getChildByName("mBeginGame"));
		mBtnBegin0 = dynamic_cast<ui::Button*>(mRoot->getChildByName("mBeginGame_0"));
		mBtnSkip = dynamic_cast<ui::Button*>(mRoot->getChildByName("skipBtn"));
		mAutoSelect = dynamic_cast<ui::Button*>(mRoot->getChildByName("mAutoSelect"));
		mNodeDetail = mRoot->getChildByName("NodeDetail");
		mNodeDaoJu = mRoot->getChildByName("NodeDaoJu");
		mPeffectReward = dynamic_cast<ui::Button*>(mNodeDetail->getChildByName("PeffectReward"));
		mBaseReward = dynamic_cast<ui::Button*>(mNodeDetail->getChildByName("BaseReward"));
		
		mArmDaoJuGroup = cocostudio::Armature::create("fla_daoju");
		auto daoJuBone = mArmDaoJuGroup->getBone("guge_daoju");
		daoJuBone->addDisplay(mNodeDaoJu, 0);
		auto nodeUp = mRoot->getChildByName("NodeUp");
		nodeUp->addChild(mArmDaoJuGroup);

		auto nodeAutoSelect = mRoot->getChildByName("NodeAutoSelect");
		mArmAutoSelect = cocostudio::Armature::create("fla_biaoqian");
		auto bQianBone = mArmAutoSelect->getBone("guge_GuanKiaKSb_sdsa");
		bQianBone->addDisplay(mAutoSelect, 0);
		nodeAutoSelect->addChild(mArmAutoSelect);

		mPower = dynamic_cast<ui::Text*>(mBtnBegin->getChildByName("Text_3"));
		mPower->setText("0/0");

		mPassText = dynamic_cast<ui::Text*>(mNodeDetail->getChildByName("uiAtlas_Level_bg")->getChildByName("levelTarget"));
		mTitleText = dynamic_cast<ui::Text*>(mNodeDetail->getChildByName("uiAtlas_Level_bg")->getChildByName("levelName"));
		changeBone(mArmatureBg, "guge_XX", mCloseBtn);
		changeBone(mArmatureBg, "guge_shuju", mNodeDetail);

		changeBone(mArmatureBg, "guge_kaishi3", mBtnBegin0);
		changeBone(mArmatureBg, "guge_kaishi2", mBtnSkip);
		changeBone(mArmatureBg, "guge_kaishi", mBtnBegin);


		setPopType(E_POP_TYPE_NONE);
		onLocateClickCallback(mRoot);

		mBattleRecord = dynamic_cast<ui::Button *>(mRoot->getChildByName("NodeDetail")->getChildByName("mLevelMapBg")->getChildByName("mBattleRecord"));


		Node* pRankNode = mRoot->getChildByName("Node_Rank");
		mRankList = dynamic_cast<ui::ListView*>(pRankNode->getChildByName("ListView_1"));
		changeBone(mArmatureBg, "guge_paihangbang", pRankNode);
		mRankList->addTouchEventListener(this, toucheventselector(LevelDialog::listViewOnTouch));

		ret = true;
	} while (0);


	return ret;
}

void LevelDialog::listViewOnTouch(Ref* ref, TouchEventType touchType)
{
	auto listView = dynamic_cast<ListView*>(ref);
	if (touchType ==TouchEventType::TOUCH_EVENT_BEGAN)
		this->mTouchBeginPos = listView->getTouchBeganPosition();
}

void LevelDialog::addMissionTarget()
{

	for (int i = 1; i <= 3; i++)
	{
		Node* mission = mNodeDetail->getChildByName(StringUtils::format("MissionTarget%d", i));
		Node* iconNode = mission->getChildByName("iconNode");
		ui::Text* info = dynamic_cast<ui::Text*>(mission->getChildByName("info"));
		info->ignoreContentAdaptWithSize(false);
		info->setTextAreaSize(Size(168, 48));
		iconNode->removeAllChildren();
		info->setText("");
	}

	int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
	sgBalanceControl()->initCheckPtStarList(checkpt);
	auto list = sgBalanceControl()->getStarList();

	BalanceControl::StarList::iterator it = list.begin();
	int cout = 1;
	if (list.empty())
	{
		Node* mission = mNodeDetail->getChildByName(StringUtils::format("MissionTarget%d", cout));
		ui::Text* info = dynamic_cast<ui::Text*>(mission->getChildByName("info"));
		info->setTextVerticalAlignment(TextVAlignment::CENTER);
		info->setTextHorizontalAlignment(TextHAlignment::CENTER);
		info->setPositionX(541);
		info->setText(sgTextHelper(TEXT_STAR_NO));
	}
	else{

		for (; it != list.end() && cout <= 3; it++, cout++)
		{
			Node* mission = mNodeDetail->getChildByName(StringUtils::format("MissionTarget%d", cout));
			Node* iconNode = mission->getChildByName("iconNode");
			Sprite* mIcon = Sprite::createWithSpriteFrameName(it->second->getIconString());
			if (mIcon)
				iconNode->addChild(mIcon);

			ui::Text* info = dynamic_cast<ui::Text*>(mission->getChildByName("info"));
			std::string szStarText = it->second->getStarText();

			info->setTextVerticalAlignment(TextVAlignment::TOP);
			info->setTextHorizontalAlignment(TextHAlignment::LEFT);
			info->setPositionX(551);
			info->setText(szStarText);
		}
	}
}

LevelDialog::LevelDialog() :mPassText(0), mAutoSelect(nullptr), mArmAutoSelect(nullptr),
mArmDaoJuGroup(nullptr), mArmLanTiao(nullptr), mIsPass(false), mRewardDisplay(nullptr), mBaseReward(nullptr), mPeffectReward(nullptr), mCurLevelId(0)
{
	mIsCanClean = false;
	mIsNoRelease = true;
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_GuanKiaKS.png", "fla_GuanKiaKS.plist", "fla_GuanKiaKS.xml");
}

LevelDialog::~LevelDialog()
{
	sgMessageManager()->removeMessageHandler(this);
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_GuanKiaKS.xml");
	mArmAutoSelect = nullptr;
	//CC_SAFE_DELETE(mArmAutoSelect);
	mArmDaoJuGroup = nullptr;
	//CC_SAFE_DELETE(mArmDaoJuGroup);
	mArmLanTiao = nullptr;
	//CC_SAFE_DELETE(mArmLanTiao);
}

void LevelDialog::onEnter()
{
	Window::onEnter();	
	
}

void LevelDialog::onExit()
{
	Window::onExit();
}

 


void LevelDialog::showPropPanel(bool show)
{
	Node* pNode = dynamic_cast<Node* >(mNodeDetail->getChildByName("uiAtlas_Level_bg"));
	if(NULL != pNode)
	{
		Node* pNode1 = dynamic_cast<Node* >(pNode->getChildByName("Text_2"));
		if(NULL != pNode1)
		{
			pNode1->setVisible(show);
		} 
	}
	Node* pNode1 = dynamic_cast<Node* >(mNodeDaoJu->getChildByName("Panel_2"));
	if(NULL != pNode1)
	{
		pNode1->setVisible(show);
	}
	Node* pNode2 = dynamic_cast<Node* >(mNodeDaoJu->getChildByName("Panel_2_0"));
	if(NULL != pNode2)
	{
		pNode2->setVisible(show);
	}
	Node* pNode3 = dynamic_cast<Node* >(mNodeDaoJu->getChildByName("Panel_2_0_0"));
	if(NULL != pNode3)
	{
		pNode3->setVisible(show);
	}
	dynamic_cast<Node* >(mNodeDaoJu->getChildByName("prop_node1"))->setVisible(show);
	dynamic_cast<Node* >(mNodeDaoJu->getChildByName("prop_node2"))->setVisible(show);
	dynamic_cast<Node* >(mNodeDaoJu->getChildByName("prop_node3"))->setVisible(show);
	dynamic_cast<Node*>(mNodeDaoJu->getChildByName("prop_add1"))->setVisible(show);
	dynamic_cast<Node*>(mNodeDaoJu->getChildByName("prop_add2"))->setVisible(show);
	dynamic_cast<Node*>(mNodeDaoJu->getChildByName("prop_add3"))->setVisible(show);
}

void LevelDialog::createScaleMap(const CheckPoint* pCP)
{
	auto bg = dynamic_cast<ui::Layout*>(mNodeDetail->getChildByName("mLevelMapBg")->getChildByName("Panel_map"));
	bg->removeAllChildren();
	auto bgSize = bg->getContentSize();


	//数据
	auto mTileMap = TMXTiledMap::create(pCP->tileMap + ".tmx");
	auto mObjectGroup = mTileMap->getObjectGroup("objects");
	ValueVector& arrGroup = mObjectGroup->getObjects();
	int arrSize = arrGroup.size();

	auto realWidth = mTileMap->getMapSize().width * mTileMap->getTileSize().width;
	auto realHeight = mTileMap->getMapSize().height * mTileMap->getTileSize().height;

	// 缩放比
	float scaleX = realWidth / bgSize.width;//5.54113;
	float scaleY = realHeight / bgSize.height;//5.53846;	

	for (int k = 0; k < arrSize; ++k)
	{
		const ValueMap& objInfo = arrGroup.at(k).asValueMap();

		if(objInfo.find("BuildId") == objInfo.end())
			continue;

		int x = objInfo.at("x").asInt();
		int y = objInfo.at("y").asInt();
		int width = objInfo.at("width").asInt();
		int height = objInfo.at("height").asInt();

		int id = objInfo.at("BuildId").asInt();
		const BuildIns* tInfo = sgTableManager()->getBuildIns(id);
		if (nullptr == tInfo)
			continue;

		int campType = tInfo->initCamp;
		int buildType = tInfo->type / 1000;
		if (buildType != eBulidType::EBULID_TYPE_BARRACK
			&& buildType != eBulidType::EBULID_TYPE_TURRET
			&& buildType != eBulidType::EBULID_TYPE_INSISTUTE
			&& buildType != eBulidType::EBUILD_TYPE_STABLES)
			continue;

		Sprite* miniBuild = CCSprite::create(StringUtils::format("miniBuild%d%d.png", campType, buildType));
		if (miniBuild)
		{
			miniBuild->setPosition(Vec2((x + width / 2.0) / scaleX, (y + height / 2.0) / scaleY));
			bg->addChild(miniBuild);
		}
	}
}

void LevelDialog::initPropPanel()
{

	if (!sgGuideManager()->checkGuideFinishByGroup(GuideGroup_PveProp))
	{
		for (int i = 1; i <= 3; i++)
		{
			PropItem* pItem = sgPropsControl()->getPropItem((ePropPos)i);
			if (pItem->getPropId() != 0)
			{
				pItem->cleanData();
				mPropList[(ePropPos)i] = 0;
			}
		}
	}

	mPropViewList->setVisible(false);
	mPropViewList->setisShow(false);

	mPropList[ePropPos::E_POS_LEFT] = 0;
	mPropList[ePropPos::E_POS_RIGHT] = 0;
	mPropList[ePropPos::E_POS_MID] = 0;

	bool flag = sgPropsControl()->hasPropInLevel();
	for (int i = 1; i <= 3; i++)
	{

		std::string prop_str = StringUtils::format("prop_node%d", i);
		Node* prop_node = mNodeDaoJu->getChildByName(prop_str);

		if (prop_node)
		{
			prop_node->removeAllChildren();
		}

		mPropPos = (ePropPos)i;
		PropItem* pItem = sgPropsControl()->getPropItem(mPropPos);

		if (!flag)
			continue;

		if (sgPropsControl()->checkPropInLevel(mPropPos))
		{
			
			if (pItem->getPropId() != 0)
			{
				if (sgUserObj()->getProp(pItem->getPropId()) > 0 && pItem->getIsTipsProp() == false)
				{
					const PropInfo* pInfo = sgTableManager()->getPropInfoById(pItem->getPropId());
					if (pInfo->isPve == 1)
					{
						addPropView(pInfo);
					}
					else
					{
						pItem->cleanData();
					}
				}
				else
				{
					pItem->cleanData();
				}

			}
		}
		else
		{
			if (pItem->getPropId() != 0)
			{
				pItem->cleanData();
			}
		}
		

	}

	for (int i = 1; i <= 3; i++)
	{
		std::string btn_str = StringUtils::format("prop_add%d", i);
		Sprite* prop_add = dynamic_cast<Sprite*> (mNodeDaoJu->getChildByName(btn_str));

		if (prop_add)
		{
			prop_add->setVisible(sgPropsControl()->checkPropInLevel((ePropPos)i));
		}
	}

	//for (int i = ePropPos::E_POS_LEFT; i <= ePropPos::E_POS_RIGHT; i++)
	//{
	//	mPropList[(ePropPos)i] = 0;
	//}

	mPropViewList->refreshData();
	showAutoSelect();
}

void LevelDialog::showAddPropPanel(int index)
{

	if (!sgPropsControl()->checkPropInLevel((ePropPos)index))
	{
		return;
	}
	
	auto callFun = [this, index](){

		if (sgGamePrecedure()->getEnterGame())
			return;

		if (mPropList[(ePropPos)index] != 0)
		{
			removeOtherProp(mPropList[(ePropPos)index]);
			mPropList[(ePropPos)index] = 0;
		}

		checkNextPropPos();
		showAutoSelect();
	};

	if (!mPropViewList->getisShow())
	{
		mPropViewList->onShow(/*callFun*/);
		if (mArmLanTiao)
		{
			mArmLanTiao->getAnimation()->play("born");
			mArmLanTiao->getAnimation()->setMovementEventCallFunc([callFun ](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID) {
				if (movementType == cocostudio::MovementEventType::COMPLETE  && movementID == "born")
				{
					callFun();
				}
			});
		}
	}
	else
	{
		callFun();
	}

}

void LevelDialog::onClick(cocos2d::Ref* sender, cocos2d::ui::TouchEventType touchEvent)
{
	static bool bLoogTouch = false;

	Node* pNode = dynamic_cast<Node*>(sender);
	int id = pNode->getTag();

	if (touchEvent == TouchEventType::TOUCH_EVENT_BEGAN)
	{
		bLoogTouch = false;
		scheduleOnce([this,id](float)
		{
			mPropViewList->hideOrShowDesc(false);
			mPropViewList->hideOrShowDescDialog(id, true);
			mPropViewList->setisShowArrowDesc(false);
			bLoogTouch = true;

		}, 0.5f, "LevelDialog::onClick");
	}


	if (touchEvent == TouchEventType::TOUCH_EVENT_ENDED)
	{
		sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);

		this->unschedule("LevelDialog::onClick");

		if (bLoogTouch)
		{
			bLoogTouch = false;
			return;
		}
		
		const PropInfo* pInfo = sgTableManager()->getPropInfoById(id);

		if (removeOtherProp(pNode->getTag()))
		{
			checkNextPropPos();
			showAutoSelect();

			return;
		}

		if (sgUserObj()->getProp(pInfo->id) <= 0)
		{

			ShopDetailed::show(/*pId,*/ pInfo->id/*, CC_CALLBACK_1(PropViewList::updateItem, mPropViewList, pInfo->id)*/);
			return;
		}

		addPropView(pInfo);
		bLoogTouch = false;
	}

	if (touchEvent == TouchEventType::TOUCH_EVENT_ENDED || touchEvent == TouchEventType::TOUCH_EVENT_CANCELED)
	{
		this->unschedule("LevelDialog::onClick");
		bLoogTouch = false;
	}
	
}

bool LevelDialog::removeOtherProp(int id)
{
	for (int i = ePropPos::E_POS_LEFT; i <= ePropPos::E_POS_RIGHT; i++)
	{

		if (mPropList[(ePropPos)i] == id)
		{
			std::string node_str = StringUtils::format("prop_node%d", i);
			Node* prop_node = mNodeDaoJu->getChildByName(node_str);
			Node* pChild = prop_node->getChildByName("propView");
			mPropList[(ePropPos)i] = 0;

			if (pChild)
			{
				Vec2 pos = prop_node->convertToNodeSpaceAR(mPropViewList->getItemPosition(id));// - mRoot->convertToWorldSpace(parent->getPosition());
				auto pMove = MoveTo::create(0.3, pos);
				auto pFun = CallFunc::create([pChild](){pChild->removeFromParent(); });
				pChild->runAction(Sequence::create(pMove,pFun,nullptr));

				Node* mClose = pChild->getChildByName("mClose");
				mClose->setVisible(false);

				sgPropsControl()->removePropItem(id);
				mPropViewList->updateView(id);
				return true;
			}



		}
	}



	return false;

}

void LevelDialog::addPropView(Node* parent,const PropInfo* pInfo)
{
	parent->removeAllChildren();

	Widget* cell = dynamic_cast<Widget*>(CSLoader::createNode("PropView.csb")->getChildByName("mTouch"));
	cell->removeFromParent();
	cell->removeChildByName("mAdd");
	cell->setAnchorPoint(Vec2(0.5, 0.5));
	cell->setSwallowTouches(false);

	ui::Button* propBg = dynamic_cast<ui::Button* >(cell->getChildByName("mBg"));
	std::string normalStr = "ui/general/pvpSelect1.png";
	std::string pressStr = "ui/general/pvpSelect1.png";
	std::string disableStr = "ui/general/pvpSelect2.png";
	if (pInfo->isPve == 1)
	{
		normalStr = "ui/general/pveSelect1.png";
		pressStr = "ui/general/pveSelect1.png";
		disableStr = "ui/general/pveSelect1.png";
	}
	propBg->loadTextureNormal(normalStr, ui::TextureResType::PLIST);
	propBg->loadTexturePressed(pressStr, ui::TextureResType::PLIST);
	propBg->loadTextureDisabled(disableStr, ui::TextureResType::PLIST);

	ui::Button* mIcon = dynamic_cast<ui::Button*>(cell->getChildByName("mIcon"));
	Size size = SpriteFrameCache::getInstance()->getSpriteFrameByName(pInfo->icon)->getOriginalSize();
	mIcon->setContentSize(size);

	mIcon->loadTextureNormal(pInfo->icon, TextureResType::PLIST);
	std::string iconString = pInfo->icon.substr(0, pInfo->icon.find_last_of("."));
	mIcon->loadTextureDisabled(StringUtils::format("%s_2.png", iconString.c_str()), TextureResType::PLIST);
	mIcon->setScale(0.4);


	int propNum = sgUserObj()->getProp(pInfo->id);
	Text* mPropNum = dynamic_cast<Text*>(cell->getChildByName("mPropNum"));
	mPropNum->setText(StringUtils::format("x%d", propNum));


	Vec2 pos = parent->convertToNodeSpaceAR(mPropViewList->getItemPosition(pInfo->id));// - mRoot->convertToWorldSpace(parent->getPosition());
	parent->addChild(cell,mPropViewList->getLocalZOrder(),"propView");

	Node* mClose = cell->getChildByName("mClose");

	if (mPropViewList->getisShow())
	{


		cell->setPosition(pos);
		auto pMove = MoveTo::create(0.3, Vec2(0, 0));
		auto callFun = CallFunc::create([mClose](){mClose->setVisible(true); });
		cell->runAction(Sequence::createWithTwoActions(pMove,callFun));
	}
	else
	{
		mClose->setVisible(true);
	}
	
	mPropViewList->updateView(pInfo->id);
	
}

void LevelDialog::addPropView(const PropInfo* pInfo)
{
	if (!pInfo)
		return;

	std::string node_str = StringUtils::format("prop_node%d", mPropPos);
	Node* prop_node = mNodeDaoJu->getChildByName(node_str);
	if (prop_node)
	{
		sgPropsControl()->addPropItem(pInfo->id);
		addPropView(prop_node, pInfo);
		mPropList[mPropPos] = pInfo->id;



		std::string btn_str = StringUtils::format("prop_btn%d", mPropPos);
		ui::Button* prop_btn = dynamic_cast<ui::Button*> (mNodeDaoJu->getChildByName(btn_str));
		if (prop_btn)
		{
			prop_btn->setVisible(false);
		}
		sgGuideManager()->unRegistGuideMessage();

	}

	showAutoSelect();

	if (!checkNextPropPos() && mPropViewList->getisShow())
	{
		if (mArmLanTiao)
		{
			mArmLanTiao->getAnimation()->play("end");
			mArmLanTiao->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID) {
				if (movementType == cocostudio::MovementEventType::COMPLETE && movementID == "end")
				{
					mPropViewList->setVisible(false);
				}
			});
		}
		mPropViewList->onHide();
	}

}

bool LevelDialog::onTouchBegan(Touch *touch, Event *unused_event)
{
	Vec2 pos = touch->getLocation();
	if (mPropViewList)
	{
		if (!mPropViewList->getRect().containsPoint(pos))
		{
			if (mPropViewList->getisShow() && mArmLanTiao)
			{
				sgSoundManager()->playMusicEffect(SOUND_CLICK_BACK);
				mArmLanTiao->getAnimation()->play("end");
				mArmLanTiao->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID) {
					if (movementType == cocostudio::MovementEventType::COMPLETE&& movementID == "end")
					{
						mPropViewList->setVisible(false);
					}
				});
				mPropViewList->onHide();
				showAutoSelect();
			}
		}
	}
	return true;
}

bool LevelDialog::checkNextPropPos()
{
	for (int i = ePropPos::E_POS_LEFT; i <= ePropPos::E_POS_RIGHT; i++)
	{
		if (sgPropsControl()->checkPropInLevel((ePropPos)i)  && mPropList[(ePropPos)i] == 0)
		{
			mPropPos = (ePropPos)i;
			return true;
		}
	}

	return false;
}

void LevelDialog::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_PROP_USER)
	{
		if (isShow())
		{
			int id = dynamic_cast<MessagePropUserData*>(message)->getPropID();
			mPropViewList->updateView(id);
			showAutoSelect();
		}

	}else if (message->getMessageType() == EMT_SHOP_AFTER_BUYDIAMOND)
	{
		int pDiamond = sgUserObj()->getPersonalInfo()->diamond();// sgUserObj()->getGoldCoin();
		if (pDiamond>=50)
		{
			if(sgChannelApi()->isOnlineSdk() == true)
			{
				sgPropsControl()->sendBuyPropRqst(PVE_POWER_TYPE, 50, 1);
				return;
			}

			sgMessageManager()->removeMessageHandler(EMT_SHOP_AFTER_BUYDIAMOND);

			playAniBornOrEnd(false);
		}
	}
	else if (message->getMessageType() == EMT_PROP_USE_ALL)
	{
		auto pMes = dynamic_cast<MessagePropUseAll*>(message);
		int id = pMes->getPropId();
		std::map<ePropPos, int>::iterator it =  mPropList.begin();
		for (; it != mPropList.end();it++)
		{
			if (id == it->second)
			{
				it->second = 0;
			}
		}
	}
	else if (message->getMessageType() == EMT_SKIP_SUCCESS)
	{
		this->setShow(false);
	}
}

bool LevelDialog::checkShowAutoSelect()
{
	bool bShow = false;

	int num = sgPropsControl()->getSelectPropNum();

	int toolNum = 0;
	for (int i = 1; i <= 3;i ++)
	{
		int tool = 0;
		switch (i)
		{
		case 1:
			tool = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt)->tool1;
			break;
		case 2:
			tool = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt)->tool2;
			break;
		case 3:
			tool = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt)->tool3;
			break;
		default:
			break;
		}
		
		if (tool != 0)
		{
			toolNum++;
		}
	}
	
	if (num < toolNum)
	{
		const PropList& propList = sgTableManager()->getPropInfoList();
		PropList::const_iterator tableIter = propList.begin();

		for ( ;tableIter  != propList.end(); tableIter++)
		{
			if (tableIter->second->display == 1 && tableIter->second->isPve == 1)
			{
				PropItem* pItem = sgPropsControl()->getPropItem(tableIter->first);
				if (pItem == nullptr && sgUserObj()->getProp(tableIter->first) > 0)
				{
					bShow = true;
				}
			}

		}
	}

	return bShow;

}

void LevelDialog::onGame(Ref* sendr)
{
	sgBattleRecordsManager()->mBattleRecords.clear();
	sgBattleRecordsManager()->setIsPlayback(false);
	//完成新手引导--进入战斗
	sgGuideManager()->unRegistGuideMessage();
	CCLOG("[GUIDE]### End to Send Battle message");

	int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
	int curPower = sgUserObj()->getPersonalInfo()->power();//sgUserObj()->getPvePower();

	const CheckPoint* pCP = sgTableManager()->getCheckPointByID(checkpt);
	bool needConsumePower = sgGuideManager()->checkNeedPower(sgGamePrecedure()->getCurMapInfo().checkpt);
	if (!pCP || pCP->consumeEnergy > curPower && needConsumePower)
	{
		//std::string szTitle = sgTextHelper(TEXT_COMMTIPS_TIPS);
		//std::string szContent =   StringUtils::format("%s,%s?",sgTextHelper(TEXT_COMMTIPS_NO_POWERS).c_str(),sgTextHelper(TEXT_BUY_ITEM_TILI_SURE).c_str());
		//GenerlDialog::show(szTitle, szContent,   CC_CALLBACK_1(LevelDialog::onCallBackBuyPower,this));
		sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "BthPowerDialog.csb", 1);

		return;
	}
	//bool isPropControl = sgGuideManager()->checkOpenPropControl(checkpt); 
	//if (isPropControl || sgEnvSet("CLOSE_GUIDE") == 1)
	//{
	//	if (checkShowAutoSelect())
	//	{
	//		sgPageFunction()->gotoFunction(MSG_FUC_WIN_PROPNOSET);
	//		this->setShow(false);
	//		playAniBornOrEnd(false);

	//		return;
	//	}
	//	CCLOG("[trace]  levelDialog: onGame");
	//}

	if (needConsumePower)
	{
		//sgUserObj()->sendEnterPoint(checkpt);
		//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBthStartMission");
		const char* pScript = String::createWithFormat("sgBthControl():sendStartRequest(%d)", 1)->getCString();
		LuaEngine::getInstance()->executeString(pScript);
	}
	else
	{
		sgBattleRecordsManager()->mBattleRecords.clear(); 
		sgPageFunction()->setCurrentUIType(MSG_FUC_MINI_LEVEL);
		PageFunction::Get()->gotoFunction(MSG_FUC_PLAYING);
		setShow(false);
		//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBthStartMissionGuide");
		const char* pScript = String::createWithFormat("sgBthControl():sendStartRequest(%d)", 0)->getCString();
		LuaEngine::getInstance()->executeString(pScript);
		return;
	}

	mBtnBegin->setEnabled(false);
	mCloseBtn->setEnabled(false);
	scheduleOnce(CC_CALLBACK_1(LevelDialog::updateBegin,this), 8.0f,"LevelDialog::updateBegin");
	playAniBornOrEnd(false);
	
}

void LevelDialog::updateBegin(float dt)
{
	mBtnBegin->setEnabled(true);
	mCloseBtn->setEnabled(true);
}

void LevelDialog::onCallBackBuyPower(Ref*)
{
	int pDiamond = sgUserObj()->getPersonalInfo()->diamond();// sgUserObj()->getGoldCoin();
	if (pDiamond>=50)
	{
		if(sgChannelApi()->isOnlineSdk() == true)
		{
			sgPropsControl()->sendBuyPropRqst(PVE_POWER_TYPE, 50, 1);
		}
		else
		{
			playAniBornOrEnd(false);
		}
		GenerlDialog::hide();
	}else
	{
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_BUY_ITEM_NO_EN),CC_CALLBACK_1(LevelDialog::onCallBackBuyDiamond, this));
	}
	
}

void LevelDialog::onCallBackBuyDiamond(Ref*)
{
	sgMessageManager()->regisiterMessageHandler(EMT_SHOP_AFTER_BUYDIAMOND, this);
	ShopPanel::showShop();
}



void LevelDialog::showAutoSelect()
{
	if (mAutoSelect )
	{
		bool showProp = sgGuideManager()->checkOpenPropControl(sgGamePrecedure()->getCurMapInfo().checkpt);
		if (checkShowAutoSelect() && showProp)
		{
			if (!mAutoSelect->isVisible())
			{
				mAutoSelect->setVisible(true);
				mArmAutoSelect->getAnimation()->play("born");
				mArmAutoSelect->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID) {
					if (movementType == cocostudio::MovementEventType::COMPLETE)
					{
						mArmAutoSelect->getAnimation()->play("stand");
					}
				});
			}
		}
		else
		{
			if (mAutoSelect->isVisible())
			{
				mArmAutoSelect->getAnimation()->play("end");
				mArmAutoSelect->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID) {
					if (movementType == cocostudio::MovementEventType::COMPLETE)
					{
						mAutoSelect->setVisible(false);
					}
				});
			}	

		}

	}
}

const PropInfo* LevelDialog::getNextPropInfo()
{
	const PropList& propList = sgTableManager()->getPropInfoList();
	PropList::const_iterator tableIter = propList.begin();

	const PropInfo* pSelect = nullptr;
	for (; tableIter != propList.end(); tableIter++)
	{
		if (tableIter->second->isPve == 0 || tableIter->second->display == 0 || tableIter->second->isPve == 2)
			continue;

		PropItem* pItem = sgPropsControl()->getPropItem(tableIter->first);
		if (pItem == nullptr && sgUserObj()->getProp(tableIter->first) > 0)
		{
			if (pSelect)
			{

				if (tableIter->second->priority < pSelect->priority)
				{
					pSelect = tableIter->second;
				}
			}
			else
			{
				pSelect = tableIter->second;
			}
		}
	}

	return pSelect;
}

bool PropNoSetDilog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PropNoSetDilog.csb");
		CC_BREAK_IF(!mRoot);

		ui::Text* pHead = dynamic_cast<ui::Text*>(mRoot->getChildByName("mHeadLabel"));
		pHead->setText(sgTextHelper(TEXT_COMMTIPS_PROP_HEXT));

		ui::Text* pContent = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_2"));
		pContent->setText(sgTextHelper(TEXT_COMMTIPS_PROP_CONTENT));


		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		ret = true;
	} while (0);

	return ret;

}

void PropNoSetDilog::onMenuItemAction(std::string cbName , cocos2d::Ref* pSender)
{
	if (cbName == "onEnter")
	{
		sgSoundManager()->playMusicEffect(SOUND_CLICK_BATTLE);

		int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
		int curPower = sgUserObj()->getPersonalInfo()->power(); //sgUserObj()->getPvePower();
		bool needConsumePower = sgGuideManager()->checkNeedPower(sgGamePrecedure()->getCurMapInfo().checkpt);
		const CheckPoint* pCP = sgTableManager()->getCheckPointByID(checkpt);
		if (!pCP || pCP->consumeEnergy > curPower && needConsumePower)
		{
			std::string szTitle = sgTextHelper(TEXT_COMMTIPS_TIPS);
			std::string szContent = sgTextHelper(TEXT_COMMTIPS_NO_POWERS);

			GenerlDialog::show(szTitle, szContent, [pCP](Ref* sendr){
				GenerlDialog::hide();
			}, true);
			this->setShow(false);
			return;
		}

		//完成新手引导--进入战斗
		sgGuideManager()->unRegistGuideMessage();
		CCLOG("[GUIDE]### End to Send Battle message");

		if (needConsumePower)
		{
			const char* pScript = String::createWithFormat("sgBthControl():sendStartRequest(%d)", 1)->getCString();
			LuaEngine::getInstance()->executeString(pScript);
		} 
		else
		{
			sgBattleRecordsManager()->mBattleRecords.clear(); 
			sgPageFunction()->setCurrentUIType(MSG_FUC_MINI_LEVEL);
			PageFunction::Get()->gotoFunction(MSG_FUC_PLAYING);
		} 

	}
	else if (cbName == "onSetProp")
	{
		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_LEVEL);
		this->setShow(false);
	}
}

void LevelDialog::windowAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	ActionWindow::windowAniCallback(pAr, type, aniName);
	if (pAr == mArmatureBg)
	{
		if (aniName == "born")
		{
			if (type == COMPLETE)
			{
				mAnimationFinish = true;
				createGuideNode();
			}
		}
	}
}

void LevelDialog::showReward(int type)
{
	const CheckPoint* pInfo = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().mapId);
	if (pInfo == nullptr)
		return;

	int id = 0;
	if (type == RewardDisplay::STAR)
		id = pInfo->perfectBonus;
	else
		id = pInfo->baseBonus;

	if (mRewardDisplay == nullptr)
	{
		mRewardDisplay = RewardDisplay::create(id, (RewardDisplay::eRewardType)type);
		if (mRewardDisplay)
		{
			mRewardDisplay->setPosition(Vec2(200, 80));
			mRoot->addChild(mRewardDisplay);
		}
	}
	else
	{
		mRewardDisplay->updateUI(id, (RewardDisplay::eRewardType)type);
		mRewardDisplay->setVisible(true);
	}

	mRewardDisplay->stopAllActions();
	mRewardDisplay->runAction(CCSequence::create(CCDelayTime::create(10.0f), CCCallFunc::create(CC_CALLBACK_0(LevelDialog::hideReward, this)), NULL));
}

void LevelDialog::hideReward()
{
	if (mRewardDisplay)
	{
		mRewardDisplay->setVisible(false);
	}
}

void LevelDialog::createRankNode(sRankInfo* info)
{
	Node* pNode = CSLoader::createNode("ui/BthRankNode.csb");
	std::string iconBg = "ui/LevelDetail/rankIconBg.png";
	std::string iconFront = "ui/LevelDetail/rankIconF.png";
	ImageView* btn = (ImageView*) pNode->getChildByName("Image_1");
	if (info->rank <= 3)
	{
		iconBg = String::createWithFormat("ui/LevelDetail/rankIconBg_%d.png", info->rank)->getCString();
		iconFront = String::createWithFormat("ui/LevelDetail/rankIconF_%d.png", info->rank)->getCString();
	}

	btn->loadTexture(iconBg, Widget::TextureResType::PLIST);
	((ImageView*)btn->getChildByName("Image_front"))->loadTexture(iconFront, Widget::TextureResType::PLIST);
	btn->setSwallowTouches(false);
	btn->setTag(info->uin);
	sgHeadIconUtils()->changeIcon(((ImageView*)btn->getChildByName("Image_icon")), info->icon, HeadIconUtils::E_SHAPE_SQUARE);
	//((ImageView*)btn->getChildByName("Image_icon"))->loadTexture(FriendSystem::GetPlayerIconNameFromId(info->icon), Widget::TextureResType::PLIST);
	((Text*)btn->getChildByName("Text_score"))->setString(String::createWithFormat("%d", info->points)->getCString());
	((Label*)((Text*)btn->getChildByName("Text_score"))->getVirtualRenderer())->setAdditionalKerning(-3.0f);
	((Sprite*)pNode->getChildByName("Sprite_rank"))->setSpriteFrame(String::createWithFormat("ui/LevelDetail/rank_%d.png", info->rank)->getCString());
	((Text*)pNode->getChildByName("Text_name"))->setString(info->name);

	Widget* pWidget = Widget::create();
	pWidget->setContentSize(Size(0, 130));
	pWidget->addChild(pNode);
	pNode->setPositionY(65);

	mRankList->pushBackCustomItem(pWidget);
}

void LevelDialog::showRankList(int missionID)
{
	mRankList->removeAllItems();

	auto luaFuncHandler = sgLuaFuncHandlerManager()->getHandler(LuaFuncHandlerManager::ModuleType::kCustom, (int)LuaFuncHandlerManager::CustomHandlerType::kGetBthRankList);
	LuaEngine* scriptEngine = dynamic_cast<LuaEngine*>(ScriptEngineManager::getInstance()->getScriptEngine());
	LuaStack* stack = scriptEngine->getLuaStack();

	auto createRankNode = [this](lua_State* tolua_S, int argc){
		LuaStatePrint(tolua_S);
		if (lua_istable(tolua_S, -1))
		{
			lua_pushnil(tolua_S);
			LuaStatePrint(tolua_S);
			while (lua_next(tolua_S, -2))
			{
				if (lua_istable(tolua_S, -1))
				{
					sRankInfo* pRankInfo = new sRankInfo();

					lua_pushnil(tolua_S);
					while (lua_next(tolua_S, -2))
					{
						LuaStatePrint(tolua_S);
						std::string key;
						if (lua_isstring(tolua_S, -2))
							key = lua_tostring(tolua_S, -2);

						if (key == "rank")
						{
							if (lua_isnumber(tolua_S, -1))
								pRankInfo->rank = lua_tointeger(tolua_S, -1);
						}
						else if (key == "uin")
						{
							if (lua_isnumber(tolua_S, -1))
								pRankInfo->uin = lua_tointeger(tolua_S, -1);
						}
						else if (key == "name")
						{
							if (lua_isstring(tolua_S, -1))
								pRankInfo->name = lua_tostring(tolua_S, -1);
						}
						else if (key == "icon")
						{
							if (lua_isstring(tolua_S, -1))
								pRankInfo->icon = lua_tostring(tolua_S, -1);
						}
						else if (key == "points")
						{
							if (lua_isnumber(tolua_S, -1))
								pRankInfo->points = lua_tointeger(tolua_S, -1);
						}
						lua_pop(tolua_S, 1);
					}
					lua_pop(tolua_S, 1);

					this->createRankNode(pRankInfo);
				}
			}
		}
	};

	if (luaFuncHandler > 0)
	{
		auto tolua_S = stack->getLuaState();

		stack->pushInt(missionID);

		LuaStatePrint(tolua_S);
		auto rlt = stack->executeFunction(luaFuncHandler, 1, 1, createRankNode);
		LuaStatePrint(tolua_S);
		stack->clean();

		onLocateClickCallback(mRoot);
	}
}
