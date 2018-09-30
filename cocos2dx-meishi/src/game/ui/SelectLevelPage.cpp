#include "SelectLevelPage.h"
#include "GamePrecedure.h"

#include "game/ui//PageFunction.h"
#include "MessageManager.h"
#include "game/balance/BalanceControl.h"
#include "ui/dialogs/GiftAwardDialog.h"
#include "prop/PropsControl.h"
#include "game/net//UserObj.h"
#include "EnvSet.h"
#include "guide/GuideManager.h"
#include "SoundManager.h"
#include "battle/BattleControl.h"
//#include "LevelPageLayer.h"
#include "sync/SyncControl.h"
#include "stl_helper.h"
#include "battle/BattleRecordsManager.h"

#define MAX_MAP_NUM 3

//#define MGAME_POWER_OPEN

SelectLevelPage::SelectLevelPage() :mScrollViewLevel(0), mNodeMapLevel(0), mCoin(0), showTip(true), 
mArmatureBaoZi(NULL), mArmatureCurLevel(NULL), mArmatureUnLock(NULL), mLastPoint(1010), mUpdateState(UPDATE_INIT), mbInitMove(false), mbJumpOpen(false)
{
	//sgMessageManager()->regisiterMessageHandler(EMT_MONEY_CHANGE, this);
	//sgMessageManager()->regisiterMessageHandler(EMP_POWER_CHANGE, this);
	//sgMessageManager()->regisiterMessageHandler(EMT_REFRESH_STAR, this);
	//sgMessageManager()->regisiterMessageHandler(EMT_ACHIEVE_GOTOLEVEL,this);
	//sgMessageManager()->regisiterMessageHandler(EMT_OPEN_LEVEL_DIALOG, this);
	//sgMessageManager()->regisiterMessageHandler(EMT_BTN_CONTROL, this);
	//sgMessageManager()->regisiterMessageHandler(EMT_PLAYER_UPGRADE, this);
}

SelectLevelPage::~SelectLevelPage()
{
	sgMessageManager()->removeMessageHandler(this);
}

void SelectLevelPage::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if(sgGuideManager()->getGuideLock() == true)
	{
		return;
	}
	Node* pNode = dynamic_cast<Node*>(sender);

	if (cName == "onLevel")
	{

		//完成新手引导--选择关卡
		sgGuideManager()->unRegistGuideMessage();
		CCLOG("[GUIDE]### End to select Gate message"); 

		Size size = Director::getInstance()->getWinSize();
		int level = atoi(GET_USERDATA(pNode).c_str());
		// 当前开启的等级
		eCheckPointState levelState = getLevelState(level);
		bool curOpen = (levelState == E_CHECKPOINTSTATE_PASS || levelState == E_CHECKPOINTSTATE_OPEN || level == 1010);
#ifdef MGAME_OPEN_ALL_CP
		curOpen = true;
#endif


		if (curOpen)
		{
			if (!sgGamePrecedure()->selectLevel(level))
				return;
			//if (level != mLastPoint)//包子跳过去

			if (mArmatureBaoZi == NULL)
			{
				//解锁动画
				mArmatureBaoZi = cocostudio::Armature::create("fla_XuanGuanQ_baozi");
				mNodeMapLevel->addChild(mArmatureBaoZi, 20);
			}

			Vec2 world = pNode->getParent()->convertToWorldSpace(pNode->getPosition());

			Vec2 pPoint = mNodeMapLevel->convertToNodeSpace(world);

			Vec2 pPoint2 = Vec2(pPoint.x/*+mArmatureBaoZi->getAnchorPoint().x*mArmatureBaoZi->getContentSize().width*/,
				pPoint.y/*+mArmatureBaoZi->getAnchorPoint().y*mArmatureBaoZi->getContentSize().height+mArmatureBaoZi->getContentSize().height*0.4*/);
			mLastPoint = level;
			if (mLastPoint %10 !=0)//大关卡位置矫正
			{
				if (mLastPoint % 10 == 1) 
					pPoint2.y += 45;
				else
				{
					pPoint2.x -= 20;
				}
			}
			else
			{
				const CheckPoint* pAttr = sgTableManager()->getCheckPointByID(level);
				if (!pAttr) return;
				bool isBoss = pAttr->levelType == E_Level_Boss;
				if (isBoss)
					pPoint2.y += 20;
				else
					pPoint2.y += 5;
			}
			baozhiJump(pPoint2,true);
			if (mArmatureCurLevel)
			{
				if (level != mCurMapLevel)
				{
					mArmatureCurLevel->setVisible(true);
				} 
				else
				{
					mArmatureCurLevel->setVisible(false);
				}
			}
		}
		else
		{
			if (level % 10 != 0)
			{
				int openLevel = sgUserObj()->getMaxOpenCheckPoint();
				int allStat = sgUserObj()->getTotalStars();
				const CheckPoint* pCheck = sgTableManager()->getCheckPointByID(level);

				int neetStar = allStat - pCheck->preNeedStar;

				if (neetStar > 0 && openLevel > level)
				{
					std::string szTitle = sgTextHelper(TEXT_COMMTIPS_TIPS);

					std::string szContent = StringUtils::format(sgTextHelper(TEXT_COMMTIPS_CP_STARS).c_str(), neetStar);
					GenerlDialog::show(szTitle, szContent);
					return;
				}
			}

			std::string szTitle = sgTextHelper(TEXT_COMMTIPS_TIPS);
			std::string szContent = sgTextHelper(TEXT_COMMTIPS_CHECKPT);
			GenerlDialog::show(szTitle, szContent); 
		}
	}else if (cName == "onBack")
	{
		this->unscheduleAllSelectors();
		//完成新手引导--返回主页面
		sgGuideManager()->unRegistGuideMessage();
		CCLOG("[GUIDE]### End to return MainUI message"); 

		if (sgUserObj()->getMaxOpenCheckPoint() == 2010)
		{
			sgMessageManager()->sendMessage(EMT_OPEN_STAGE, "2");
		}

		showTip = true;
		sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
		sgPageFunction()->enableOptionButton(true);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
	}
	else if (cName == "onGold")
	{
		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
	}
	else if (pNode->getParent()->getName() == "mScroceBuy_0")
	{
		GenerlDialog::show(sgTextHelper(TEXT_BUY_ITEM_TITLE), sgTextHelper(TEXT_BUY_ITEM_TILI_SURE), CC_CALLBACK_1(SelectLevelPage::onAddPowerCallBack, this), false, true);
	}
}

void SelectLevelPage::onAddPowerCallBack(cocos2d::Ref* ref)
{
	sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);

	//if (sgUserObj()->getGoldCoin() >= 50)
	if (sgUserObj()->getPersonalInfo()->diamond() >= 50)
	{
		//if (sgUserObj()->getPvePower() <= POWER_BUY_MAX)
		if (sgUserObj()->getPersonalInfo()->power() <= POWER_BUY_MAX)
		{
			if(sgChannelApi()->isOnlineSdk() == true)
			{ 
				sgPropsControl()->sendBuyPropRqst(PVE_POWER_TYPE, 50, 1);
			}
			else
			{
				//获取体力动画
				Size winSize = Director::getInstance()->getWinSize();
				sgPageFunction()->getActionPowerLayer()->addItem("ui/general/general_Energy.png", Vec2(winSize / 2), 50, sgUserObj()->getPersonalInfo()->power());//sgUserObj()->getPvePower());

				std::string param = StringUtils::format("%d %d %d %d %d %s"
					, 1
					, 1
					, eVituralCurrencyType::Diamond
					, -50
					, sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
					, "SelectLevelPage::onAddPowerCallBack");
				sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
			}
			
		}else
			GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS),
			__String::createWithFormat("%s%s",sgTextHelper(TEXT_ACT_TILI).c_str(),sgTextHelper(TEXT_BUY_POWER_MAX).c_str())->getCString());
		
	}else
	{
		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
		GenerlDialog::hide();
	}
	
}

bool SelectLevelPage::init()
{
	sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "BthMainPanel.csb", 1,true);
	return true;
	/*bool ret = false;
	do
	{
		cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_XuanGuanQ.xml");
		mLastPoint = 1010;
		mRoot = CSLoader::getInstance()->createNode("ui/SelectLevelPage.csb");
		CC_BREAK_IF(!mRoot);

		onLocateClickCallback(mRoot);

		this->addChild(mRoot);

		auto scrollViewLevel = mRoot->getChildByName("ScrollViewLevel");
		CC_BREAK_IF(!scrollViewLevel);
		mScrollViewLevel = dynamic_cast<ui::ScrollView*>(scrollViewLevel);
		CC_BREAK_IF(!mScrollViewLevel);
		mNodeMapLevel = mScrollViewLevel->getChildByName("NodeMapLevel");
		CC_BREAK_IF(!mNodeMapLevel);
		auto panelSize = mNodeMapLevel->getChildByName("PanelSize");
		CC_BREAK_IF(!panelSize);
		for (int i = 0; i < 5; i++)
		{
			std::string nodeName = __String::createWithFormat("NodeBg%d", i)->getCString();
			mNodeBg[i] = mRoot->getChildByName(nodeName);
			if (i > 2)
			{
				std::string armName = __String::createWithFormat("fla_XuanGuanQ_bg%d", i)->getCString();
				auto armatureBg = cocostudio::CCArmature::create(armName);
				armatureBg->getAnimation()->play("play");
				mNodeBg[i]->addChild(armatureBg);
			}
		}

		float prevPosX = 0;
		auto armatureBgNear = cocostudio::CCArmature::create("fla_XuanGuanQ_bg0");
		float fOneNearBgArmWidth = armatureBgNear->getContentSize().width;
		int  nearArmNum = ceil(mNodeMapLevel->getContentSize().width * 5 / (fOneNearBgArmWidth * 3));
		if (mNodeMapLevel->getContentSize().width * 5 - (fOneNearBgArmWidth * 3)*nearArmNum > 0)
			nearArmNum = nearArmNum + 1;
		for (int i = 0; i < nearArmNum; i++)
		{
			auto armatureBg0 = cocostudio::CCArmature::create("fla_XuanGuanQ_bg0");
			armatureBg0->getAnimation()->play("play");
			mNodeBg[0]->addChild(armatureBg0);
			armatureBg0->setPositionX(prevPosX);
			//mMapArmNearBg.insert(make_pair(i, NodeArm));
			prevPosX += fOneNearBgArmWidth;
		}


		prevPosX = 0;
		auto armatureBgMid1 = cocostudio::CCArmature::create("fla_XuanGuanQ_bg1");
		float fOneMidBg1ArmWidth = armatureBgMid1->getContentSize().width;
		int  mid1ArmNum = ceil(mNodeMapLevel->getContentSize().width / (fOneMidBg1ArmWidth * 3));

		if (mNodeMapLevel->getContentSize().width - (fOneMidBg1ArmWidth * 3)*mid1ArmNum > 0)
			mid1ArmNum = mid1ArmNum + 1;
		for (int i = 0; i < mid1ArmNum; i++)
		{
			auto armatureBg1 = cocostudio::CCArmature::create("fla_XuanGuanQ_bg1");
			armatureBg1->getAnimation()->play("play");
			mNodeBg[1]->addChild(armatureBg1);
			armatureBg1->setPositionX(prevPosX);
			//mMapArmNearBg.insert(make_pair(i, NodeArm));
			prevPosX += fOneMidBg1ArmWidth;
		}


		prevPosX = 0;
		auto armatureBgMid2 = cocostudio::CCArmature::create("fla_XuanGuanQ_bg2");
		float fOneMidBg2ArmWidth = armatureBgMid2->getContentSize().width;
		int  mid2ArmNum = ceil(mNodeMapLevel->getContentSize().width / (fOneMidBg2ArmWidth * 3));


		//10像素处理动画接缝问题
		if (mNodeMapLevel->getContentSize().width * 2 - (fOneMidBg2ArmWidth * 15)*mid2ArmNum - 10*(mid2ArmNum -1)> 0)
			mid2ArmNum = mid2ArmNum + 1;
		for (int i = 0; i < mid2ArmNum; i++)
		{
			auto armatureBg2 = cocostudio::CCArmature::create("fla_XuanGuanQ_bg2");
			armatureBg2->getAnimation()->play("play");
			mNodeBg[2]->addChild(armatureBg2);
			armatureBg2->setPositionX(prevPosX);
			//mMapArmNearBg.insert(make_pair(i, NodeArm));
			prevPosX += (fOneMidBg2ArmWidth-10);
		}

		mCheckPointList = sgTableManager()->getCheckPoint();
		mCheckPointCursor = mCheckPointList.begin();

		initScrollViewInfo();
		mPower = dynamic_cast<ui::Text*>(mRoot->getChildByName("mScroceBuy_0")->getChildByName("Text_1"));
		int curPower = sgUserObj()->getPersonalInfo()->power(); //sgUserObj()->getPvePower();
		char szText[32] = { 0, };
		sprintf(szText, "%d", curPower);
		mPower->setText(szText);

		//mPower->setText("0");

		//mCoin = dynamic_cast<ui::Text*>(mRoot->getChildByName("mScroceBuy")->getChildByName("Text_1"));
		//mCoin->setText("0");

		mPowerTip = dynamic_cast<ui::Text*>(mRoot->getChildByName("mScroceBuy_0")->getChildByName("Sprite_3")->getChildByName("Text_2"));

		cocostudio::Armature* ar = cocostudio::CCArmature::create("fla_XuanGuanQ__ADDITIVE");
		ar->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);
		Size winSize = CCDirector::getInstance()->getWinSize();
		ar->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
		ar->getAnimation()->play("play");
		mRoot->addChild(ar, 0);

		mRoot->getChildByName("BtnBack")->setLocalZOrder(1); 

		//获取体力动画层
		Node* pOriginReceiver = mRoot->getChildByName("mScroceBuy_0");
		Node* pReceiver = CSLoader::getInstance()->createNode("MoneyBuyNode.csb");
		dynamic_cast<ui::Button*>(pReceiver->getChildByName("Button_3"))->loadTextureNormal("ui/general/general_Energy.png", TextureResType::PLIST);
		sgPageFunction()->getActionPowerLayer()->addReceiver(RECEIVER_ACCOUNT, pReceiver, pOriginReceiver->getPosition(), false, pOriginReceiver);
		mUnLockLevel = sgUserObj()->getMaxOpenCheckPoint();

		initMapLevelInfo();

		ret = true;
	} while (0);

	return ret;*/
}

void SelectLevelPage::initScrollViewInfo()
{
	mInnerSize = mNodeMapLevel->getContentSize();
	mScrollViewLevel->setBounceEnabled(false);
	mScrollViewLevel->setInnerContainerSize(Size(mInnerSize.width, mInnerSize.height));
	mScrollViewLevel->addEventListenerScrollView(this, scrollvieweventselector(SelectLevelPage::onScrollViewCallBack));
}

SelectLevelPage* SelectLevelPage::create()
{
	SelectLevelPage* pPage = new SelectLevelPage();
	if (pPage && pPage->init())
	{
		pPage->autorelease();
		return pPage;
	}
	CC_SAFE_DELETE(pPage);
	return NULL;
}

void SelectLevelPage::onEnter()
{
	GameScene::onEnter();
	return;
	if (mUpdateState == UPDATE_INIT)
	{
		mUpdateState = UPDATE_ONENTER;
	}
	else if (mUpdateState != UPDATE_COMPLETE)
	{
		this->scheduleUpdate();
	}
	resetGuideNode();
	doGuideNode();	
}

void SelectLevelPage::onExit()
{
	GameScene::onExit();
	return;
	sgUserObj()->saveToFile();
}

 

bool SelectLevelPage::loadAsync(std::function<void()> loadCompleteCallback)
{
	GameScene::loadAsync(nullptr);
	if (loadCompleteCallback)
		loadCompleteCallback();
	return true;
}

void SelectLevelPage::initMapLevelInfo()
{
	int level = sgUserObj()->getMaxOpenCheckPoint();
	if (level <= 0 || level == 1010){
		level = 1010;
		initLevelInfo(level);
		setLevelBg(level);
	}
		
	auto it = mCheckPointList.find(level);
	auto tmp = it;

	int num = MAX_MAP_NUM + 1;
	for (; tmp != mCheckPointList.begin(); tmp--)
	{
		initLevelInfo(tmp->first);
		setLevelBg(tmp->first);
		num--;
		if (num == 0 || tmp == mCheckPointList.begin())break;
	}
	tmp = ++it;
	num = MAX_MAP_NUM;
	for (; tmp != mCheckPointList.end(); ++tmp)
	{
		initLevelInfo(tmp->first);
		setLevelBg(tmp->first);
		num--;
		if (num == 0)break;
	}
	this->scheduleUpdate();
}

void SelectLevelPage::update(float dt)
{
	int curLevel = mCheckPointCursor->first;
	std::map<int, Node* >::iterator iter = mMapLevelBtns.find(curLevel);
	if (iter == mMapLevelBtns.end())
	{
		initLevelInfo(curLevel);
		setLevelBg(curLevel);
	}

	++mCheckPointCursor;

	if (mCheckPointCursor->first / 1000 > 2 || mCheckPointCursor == mCheckPointList.end())
	{
		int levelId = checkloadAllLevel();
		if (levelId > 0)
		{
			mCheckPointCursor = mCheckPointList.find(levelId);
		}
		else if (levelId == 0)
		{
			this->unscheduleUpdate();
			mUpdateState = UPDATE_COMPLETE;
			return;
		}

	}
}

int SelectLevelPage::checkloadAllLevel()
{
	auto iter = mCheckPointList.begin();
	for (; iter->first / 1000 > 2 && iter != mCheckPointList.end(); iter++)
	{
		auto it = mMapLevelBtns.find(iter->first);
		if (it == mMapLevelBtns.end())
			return iter->first;
	}
	return 0;
}

void SelectLevelPage::RefreshBg(int level)
{
	setLevelBg(level);
}

void SelectLevelPage::setLevelBg(int levelId)
{
	Node* parent = getLevelBtn(levelId);
	if (nullptr == parent)
		return;

	const CheckPoint* pAttr = sgTableManager()->getCheckPointByID(levelId);
	if (!pAttr)return;
	
	ui::Button* mBg = dynamic_cast<Button*>(parent->getChildByName("mBg"));
	ui::Button* mBook = dynamic_cast<Button*>(parent->getChildByName("mBook"));
	Node* mLevelNode = parent->getChildByName("mLevelNode"); 
	Node* mCrown = parent->getChildByName("mCrown");
	Node* mStory = parent->getChildByName("mStory");

	Node* mLock = parent->getChildByName("Sprite_1");

	ui::Text* pName = dynamic_cast<ui::Text*>(parent->getChildByName("mLevelText"));
	if (pName)
	{
		pName->setText(pAttr->levelName);
	}

	
	int level = (pAttr->id % 1000) / 10;

	eCheckPointState levelState = getLevelState(pAttr->id); // 是否开启关卡
	bool isOpen = (levelState == E_CHECKPOINTSTATE_OPEN || levelState == E_CHECKPOINTSTATE_PASS || pAttr->id == 1010);
	if (mLock)
	{
		mLock->setVisible(!isOpen);
	}
	// 书标记  是否有新手引导
	if (mBook)
	{
		mBook->setVisible(pAttr->levelLogo == 1);
		mBook->setBright(isOpen);
	}

	// 是否有剧情
	if (mStory)
		mStory->setVisible(pAttr->levelLogo == 2);

	if (levelState == E_CHECKPOINTSTATE_PASS)
	{

		mBg->setBrightStyle(BrightStyle::HIGHLIGHT);

		// 关卡获得到的星级
		setLevelStar(parent, pAttr->id);

		//是否有通关奖励 

		auto btnGift = parent->getChildByName("mGift");
		ui::Button* mGift = dynamic_cast<ui::Button*>(btnGift);
		if (!mGift) return;
		int  type = 0;
		bool bShowGift = false;
		if (pAttr->baseBonus > 0)
		{
			if (!sgUserObj()->getReceiveAward(pAttr->id, 1))
			{
				type = 1;
				bShowGift = true;
			}
		}

		//有通关礼包就不判断满星礼包
		if (!bShowGift && pAttr->perfectBonus  && sgUserObj()->getCheckPointStar(pAttr->id) == sgTableManager()->getCheckPointStars(pAttr->id))
		{
			if (!sgUserObj()->getReceiveAward(pAttr->id, 2))
			{
				type = 2;
				bShowGift = true;
			}
		}

// 		if (bShowGift)
// 		{
// 			//显示礼包
// 			//加上礼包特效
// 			if (type == 2)
// 				mGift->loadTextureNormal("ui/Map/Map_Gift_1.png", TextureResType::PLIST);
// 
// 			if (mGift->getChildrenCount() == 0)
// 			{
// 				auto pArmature = Armature::create("fla_libaotexiao_ADDITIVE");
// 				pArmature->setBlendFunc(BlendFunc::COMADDITIVE);
// 				pArmature->getAnimation()->play("play");
// 				pArmature->setPosition(mGift->getContentSize() / 2);
// 				mGift->addChild(pArmature);
// 			}
// 
// 			mGift->addClickEventListener(CC_CALLBACK_1(SelectLevelPage::getGift, this, type));
// 			mGift->setTag(pAttr->id);
// 			mGift->setTouchEnabled(true);
// 			mGift->setVisible(true);
// 		}
// 		else
// 			mGift->setVisible(false);

		//该关卡通关线刷新
		std::string btnName = StringUtils::format("Button%d", pAttr->id);
		auto btnNode = mNodeMapLevel->getChildByName(btnName);
		if (btnNode)
		{
			auto btn = dynamic_cast<ui::Button*>(btnNode);
			btn->setBrightStyle(BrightStyle::HIGHLIGHT);
		}

	}
	else if (levelState == E_CHECKPOINTSTATE_OPEN)
	{
		if (pAttr->levelType == E_Level_Main)
		{
			//mBg->setBright(true);
			mBg->setHighlighted(false);
			//mBg->setHighlighted(true);
		}

		if (pAttr->id == parent->getTag() && pAttr->id % 10 == 0)
		{

			if (mArmatureUnLock == NULL)
			{
				//解锁动画
				mArmatureUnLock = cocostudio::Armature::create("fla_guanqiasuo");
				mNodeMapLevel->addChild(mArmatureUnLock, 18);
			}
			Vec2 world = parent->convertToWorldSpace(mBg->getPosition());

			Vec2 pPoint = mNodeMapLevel->convertToNodeSpace(world);
			Vec2 pPoint2 = Vec2(pPoint.x + mArmatureUnLock->getAnchorPoint().x*mArmatureUnLock->getContentSize().width - 20,
				pPoint.y + mArmatureUnLock->getAnchorPoint().y*mArmatureUnLock->getContentSize().height - 40);
			mArmatureUnLock->setPosition(pPoint2);
			mArmatureUnLock->getAnimation()->play("play");
			mUnLockLevel = pAttr->id;
		}	
	}
	else if (levelState == E_CHECKPOINTSTATE_CLOSE)
	{
		auto btnGift = parent->getChildByName("mGift");
		ui::Button* mGift = dynamic_cast<ui::Button*>(btnGift);
		if (mGift)
		{
			mGift->setVisible(false);
		}
	}

}


void SelectLevelPage::onReceiveMassage(GameMessage * message)
{
	switch (message->getMessageType())
	{
	case EMP_POWER_CHANGE:
	{
		MessagePowerChanged* pMes = dynamic_cast<MessagePowerChanged*>(message);
		if (pMes)
		{
			int curPower = sgUserObj()->getPersonalInfo()->power();// sgUserObj()->getPvePower();
			char szText[32] = { 0, };
			sprintf(szText, "%d", curPower);
			mPower->setText(szText);
		}
	}
		break;
	case EMT_REFRESH_STAR:
	{
		MessageRefreshStar* pMes = dynamic_cast<MessageRefreshStar*>(message);
		if (pMes)
		{
			if (pMes->getFlashAll() == 0)
			{
				// 设置该关卡获得的星级显示
				int stars = sgBalanceControl()->getStarNum();
				int level = sgUserObj()->getNowPassCheckPoint();// sgGamePrecedure()->getCurMapInfo().checkpt;
				Node* parent = getLevelBtn(level);
				if (nullptr == parent)
					return;
				//setStarNum(leveBg->getChildByTag(level), stars, true, level);//刷新
				// 开启后续关卡
				RefreshBg(level);
				CCLOG("[trace] EMT_REFRESH_STAR RefreshBg:%d",level);
				int levelId = sgUserObj()->getMaxOpenCheckPoint();
				int allStar = sgUserObj()->getTotalStars();
				std::vector<int> openList = sgTableManager()->getNextCheckPoints(level, allStar);
				for (unsigned int i = 0; i < openList.size(); i++)
				{
					int checkPoint = openList.at(i);
					int state = sgUserObj()->getCheckPointState(checkPoint);
					if (state == E_CHECKPOINTSTATE_OPEN)
					{
						RefreshBg(checkPoint);
					}
				}

			}
			else if (pMes->getFlashAll() == 1)
			{
				initLevelAllBg();
			}
			
		}

	}
		break;
	case EMT_MONEY_CHANGE:
		{
			MessageMoneyChange* pMes = dynamic_cast<MessageMoneyChange*>(message);
			if (pMes)
			{
				int curGold = sgUserObj()->getPersonalInfo()->diamond();//sgUserObj()->getGoldCoin();
				char szText[32] = { 0, };
				sprintf(szText, "%d", curGold);
				mCoin->setText(szText);
			}
		}
		break;
	case EMT_ACHIEVE_GOTOLEVEL:
		{
			MessageAchieveGoto* pMes = dynamic_cast<MessageAchieveGoto*>(message);
			if (pMes)
			{
				Node* parent = getLevelBtn(pMes->getGotoLevel());
				if (nullptr == parent)
					return;

				onMenuItemAction("onLevel", parent->getParent());
			}
		}
		
		break;
	case EMT_OPEN_LEVEL_DIALOG:

		break;
	case EMT_BTN_CONTROL:
	{
		MessageBtnControl* pMes = dynamic_cast<MessageBtnControl*>(message);
		if (!pMes) return;
		setLevelBg(pMes->getBtnType());

	}
		break;
	case  EMT_PLAYER_UPGRADE:
	{

		CCLOG("[trace]SelectWorldPage::onReceiveMassage EMT_PLAYER_UPGRADE");
		if (CurGameScene == this)
		{
			sgPageFunction()->gotoFunction(MSG_FUC_PLAYER_UPGRADE);
		}
	}
		break;
	default:
		break;
	}
}

void SelectLevelPage::onShow()
{
	GameScene::onShow();
	sgPageFunction()->setCurrentUIType(MSG_FUC_MINI_LEVEL);
	sgPageFunction()->setDiamondPanel(true, this);


	return;

	/*powerTip();
	mRoot->addChild(sgChattingControl()->getChattingButton());

	sgMessageManager()->sendMessage(EMP_POWER_CHANGE, "");
	sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, "");
	if (!mbInitMove)
		initPanelMove();

	if (sgUserObj()->getLvlUpgrade())
	{
		sgPageFunction()->gotoFunction(MSG_FUC_PLAYER_UPGRADE);
	}*/
}

void SelectLevelPage::resetGuideNode()
{
	int tag_begin = 10000;
	int tagArr[3] = {1010,1020,1120};

	Node* pPanel = mScrollViewLevel->getInnerContainer();
	for (int i = 0; i < sizeof(tagArr) / sizeof(int *); i++)
	{
		std::string nodeName = __String::createWithFormat("mMap%d", tagArr[i])->getCString();
		Node* pointNode = mNodeMapLevel->getChildByName(nodeName);                      //getLevelBtn(tagArr[i]);
		if (nullptr == pointNode)
			continue;
		Point pos = pointNode->getPosition();
		if(!mRoot->getChildByTag(tagArr[i] + tag_begin))
		{
			Node* pNode = Node::create();
			mRoot->addChild(pNode, 1, tagArr[i] + tag_begin);
			pNode->setName(StringUtils::format("%d", tagArr[i] + tag_begin));
			pNode->setPosition(pos + pPanel->getPosition());
		}
		else
		{
			Node* pNode = mRoot->getChildByTag(tagArr[i] + tag_begin);
			pNode->setPosition(pos + pPanel->getPosition());
		}
	}
}

void SelectLevelPage::doGuideNode()
{
	sgGuideManager()->unRegistGuideMessage(true);
	Layout* pInner = mScrollViewLevel->getInnerContainer();
	//开始新手引导--选择关卡  	 
	int gateID = sgUserObj()->getMaxOpenGateID();
	int level = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel();
	sgGuideManager()->registGuideMessage(gateID, level, E_GUIDELOC_SELECTGATE, mRoot, this, false);
	CCLOG("[GUIDE]### Start to select Gate message");  
}

void SelectLevelPage::setStarNum(Node* parent, int starNum, bool isOpen,int level)
{
	if (!parent) return;
	
	eCheckPointState state = (eCheckPointState)sgUserObj()->getCheckPointState(level);
	if (state != E_CHECKPOINTSTATE_PASS ) return;

	int maxNum = sgTableManager()->getCheckPointStars(level);
	for (int i = 1; i <= starNum; i++)
	{
		ui::Button* mStar = dynamic_cast<Button*>(parent->getChildByName(StringUtils::format("mStar_%d_%d", maxNum, i)));
		if (mStar)
			mStar->setBright(isOpen);
	}
	parent->removeChildByTag(20);
}

int SelectLevelPage::getNextCheckPt(int pt)
{
	int allStar = sgUserObj()->getTotalStars();
	std::vector<int> openList = sgTableManager()->getNextCheckPoints(pt, allStar);
	for (unsigned int i = 0; i < openList.size(); i++)
	{
		int checkPoint = openList.at(i);
		int state = sgUserObj()->getCheckPointState(checkPoint);
		if (state == E_CHECKPOINTSTATE_OPEN && checkPoint%10 == 0)
		{
			return checkPoint;
		}
	}
	return -1;

}

void SelectLevelPage::initLevelInfo(int level)
{
	std::string nodeName = __String::createWithFormat("mMap%d", level)->getCString();
	Node* parent = mNodeMapLevel->getChildByName(nodeName);
	if (!parent) return;
	auto panel = dynamic_cast<ui::Layout*>(parent);
	if (!panel) return;
	panel->setSwallowTouches(false);

	const CheckPoint* pAttr = sgTableManager()->getCheckPointByID(level);
	if (!pAttr) return;
	Node* pLevelBtn = nullptr;

	if ((level % 10) == 0)
	{
		bool isBoss = pAttr->levelType == E_Level_Boss;

		std::string stPathBg = "LevelButton.csb";
		if (isBoss)
			stPathBg = "LevelBossButton.csb";
		pLevelBtn = CSLoader::getInstance()->createNode(stPathBg);
		if (isBoss && level / 1000 == 2)
		{
			ImageView* pImage = dynamic_cast<ImageView*>(pLevelBtn->getChildByName("mOpenSpr"));
			pImage->loadTexture("ui/Map/Map_sushi.png", TextureResType::PLIST);
		}
	}
	else
	{
		std::string stPathBg = "LevelBossButton2.csb";
		if (level % 10 == 1)
			stPathBg = "LevelBossButton1.csb";

		pLevelBtn = CSLoader::getInstance()->createNode(stPathBg);
		if (level / 1000 == 2)
		{
			ImageView* pImage = dynamic_cast<ImageView*>(pLevelBtn->getChildByName("mOpenSpr"));
			pImage->loadTexture("ui/Map/Map_sushi.png", TextureResType::PLIST);
		}		
	}
	if (pLevelBtn)
	{
		pLevelBtn->setPosition(parent->getContentSize().width / 2, parent->getContentSize().height / 2);
		pLevelBtn->setTag(level);
		parent->addChild(pLevelBtn);
		mMapLevelBtns.insert(std::make_pair(level, pLevelBtn));
	}
}
void SelectLevelPage::initLevelAllBg()
{
	auto it = mMapLevelBtns.begin();
	for (; it != mMapLevelBtns.end(); ++it)
	{
		setLevelBg(it->first);
	}
}

eCheckPointState SelectLevelPage::getLevelState(int level)
{
	eCheckPointState state = (eCheckPointState)sgUserObj()->getCheckPointState(level);
	return state;
}

void SelectLevelPage::setLevelStar(Node* parent, int level)
{
	if (level > sgUserObj()->getMaxOpenCheckPoint()) return;
	int starNum = sgUserObj()->getCheckPointStar(level);
	int maxStarNum = sgTableManager()->getCheckPointStars(level);
	for (int i = 1; i <= 3; i++)
	{
		for (int j = 1; j <= i; j++)
		{
			ImageView* mStar = dynamic_cast<ImageView* >(parent->getChildByName(StringUtils::format("mStar_%d_%d", i, j)));
			mStar->setVisible(false);
		}
	}
		
	//if (starNum == 0)return;

	std::string starImgStr0 = "ui/Map/star_m_0.png";
	std::string starImgStr1 = "ui/Map/star_m_0.png";
	for (int i = 1; i <= maxStarNum; i++)
	{
		ImageView* mStar = dynamic_cast<ImageView* >(parent->getChildByName(StringUtils::format("mStar_%d_%d", maxStarNum, i)));
		mStar->setVisible(true);
		if (maxStarNum == 1)
		{
			starImgStr0 = "ui/Map/star_m_0.png";
			starImgStr1 = "ui/Map/star_m_1.png";
		}
		else if (maxStarNum == 2)
		{
			if (i == 1)
			{
				starImgStr0 = "ui/Map/star_l_0.png";
				starImgStr1 = "ui/Map/star_l_1.png";
			}
			else if (i == 2)
			{
				starImgStr0 = "ui/Map/star_r_0.png";
				starImgStr1 = "ui/Map/star_r_1.png";
			} 
			
		}
		else if (maxStarNum == 3)
		{
			if (i == 1)
			{
				starImgStr0 = "ui/Map/star_l_0.png";
				starImgStr1 = "ui/Map/star_l_1.png";
			}
			else if (i == 2)
			{
				starImgStr0 = "ui/Map/star_m_0.png";
				starImgStr1 = "ui/Map/star_m_1.png";
			}
			else if (i == 3)
			{
				starImgStr0 = "ui/Map/star_r_0.png";
				starImgStr1 = "ui/Map/star_r_1.png";
			}
		}

		if (i > starNum)
			mStar->loadTexture(starImgStr0, TextureResType::PLIST);
		else
		{
			mStar->loadTexture(starImgStr1, TextureResType::PLIST);
			if (i == maxStarNum)
				dynamic_cast<ImageView*>(parent->getChildByName("mMaxStarBg"))->setVisible(true); 
		} 
	}
	
}

void SelectLevelPage::getGift(Ref *pSender,int type)
{
	auto button = dynamic_cast<Button*>(pSender);
	//
	auto pArrt = sgTableManager()->getCheckPointByID(button->getTag());

	sgUserObj()->setReceiveAward(button->getTag(),type);
	button->setVisible(false);

	//RefreshBg(button->getTag());
	// 弹窗提示
	//GiftAwardDialog::showDialogByGiftId(pArrt->baseBonus);
}


void SelectLevelPage::powerTip()	
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

	int curPower = sgUserObj()->getPersonalInfo()->power();// sgUserObj()->getPvePower();
	if (curPower < sgUserObj()->getPersonalInfo()->powerslot())// sgUserObj()->getPvePowerLimit())
	{
		mPowerTip->setText(sgTextHelper(TEXT_POWER_NOT_FULL));
	}
	else
	{
		mPowerTip->setText(sgTextHelper(TEXT_POWER_FULL_TIPS));
	}

	pParent->stopAllActions();
	pParent->runAction(Sequence::create(
		DelayTime::create(3.0),
		FadeOut::create(2.0),NULL));
}

void SelectLevelPage::onScrollViewCallBack(Ref*, ScrollviewEventType type)
{
	if (type == SCROLLVIEW_EVENT_SCROLLING)
	{
		Layout* pInner = mScrollViewLevel->getInnerContainer();
		float offsetX = pInner->getPositionX();
		CCLOG("Size.width = %f, Size.height = %f", pInner->getSize().width, pInner->getSize().height);
		if (offsetX >= 0 || offsetX <= -(pInner->getSize().width - mScrollViewLevel->getContentSize().width))
			return;
		touchMovePanel(offsetX);
	}
}


void SelectLevelPage::initPanelMove()
{
	if (!mArmatureBaoZi)
	{
		//解锁动画
		mArmatureBaoZi = cocostudio::Armature::create("fla_XuanGuanQ_baozi");
		mNodeMapLevel->addChild(mArmatureBaoZi, 20);
		mArmatureBaoZi->setPositionX(-50);
	}

	if (!mArmatureCurLevel)
	{
		//解锁动画
		mArmatureCurLevel = cocostudio::Armature::create("fla_dangqian");
		mArmatureCurLevel->getAnimation()->play("play");
		mNodeMapLevel->addChild(mArmatureCurLevel, 19);
		mArmatureCurLevel->setPositionX(-50);
	}
	
	int level = sgUserObj()->getMaxOpenCheckPoint();
	
	Node* mMapPoint = nullptr;
	bool bHaveNextLevel = false;
	if (level > 0)
	{  
		int levelId = sgUserObj()->getNextCheckPt();
		if (levelId > 0)
		{
			bHaveNextLevel = true;
			auto it = mMapLevelBtns.find(levelId);
			if (it != mMapLevelBtns.end())
			{
				mMapPoint = it->second->getParent();
			}
			
		}
		else
		{
			auto it = mMapLevelBtns.find(level);
			if (it != mMapLevelBtns.end())
			{
				mMapPoint = it->second->getParent();
			}
		}

	}
	else if (level == 0)
	{
		auto it = mMapLevelBtns.find(mCheckPointList.begin()->first);
		if (it != mMapLevelBtns.end())
		{
			mMapPoint = it->second->getParent();
		}
	}

	if (!mMapPoint) return;
	
	//从回放模式返回时
	int lastPlaybackPoint = sgBattleRecordsManager()->getLastPlaybackPoint();
	if (0 != lastPlaybackPoint)
	{
		int levelId = lastPlaybackPoint;//getNextCheckPt(lastPlaybackPoint);
		Node * myChild = nullptr;
		auto iter = mMapLevelBtns.find(levelId);
		if (iter != mMapLevelBtns.end())
		{
			myChild = iter->second->getParent();
		}

		if (myChild)
		{
			Vec2 point = myChild->getPosition();
			Layout* pInner = mScrollViewLevel->getInnerContainer();
			const Size& frameSize = CCDirector::getInstance()->getOpenGLView()->getFrameSize();

			if ((point.x - frameSize.width / 2 >= 0) && (pInner->getContentSize().width - point.x >= frameSize.width))
				touchMovePanel(-point.x + frameSize.width / 2);
			else if (point.x - frameSize.width / 2 < 0)
			{
				touchMovePanel(0);

			}
			else if (pInner->getContentSize().width - point.x < frameSize.width)
			{
				touchMovePanel(-pInner->getContentSize().width + frameSize.width);
			}

			if (levelId % 10 != 0)//大关卡位置矫正
			{
				if (levelId % 10 == 1)
					point.y += 45;
				else
				{
					point.x -= 20;
				}
			}
			else
			{
				const CheckPoint* pAttr = sgTableManager()->getCheckPointByID(levelId);
				if (!pAttr) return;
				bool isBoss = pAttr->levelType == E_Level_Boss;
				if (isBoss)
					point.y += 20;
				else
					point.y += 5;
			}
			baozhiJump(point);
		}
		if (sgUserObj()->getPlayerUpgradeState() == 0)
			PageFunction::Get()->gotoFunction(MSG_FUC_WIN_LEVEL);
		sgBattleRecordsManager()->setLastPlaybackPoint(0);
		return;
	}
	int tmpLevel = level;
	if (level == 0)
		tmpLevel = 1010;
	auto tmp = mCheckPointList.find(tmpLevel);
	for (; tmp != mCheckPointList.end(); tmp++)
	{
		int nextLevel = tmp->first;
		Node* child = nullptr;
		auto itCh = mMapLevelBtns.find(nextLevel);
		if (itCh == mMapLevelBtns.end()) continue;
		child = itCh->second->getParent();

		eCheckPointState levelState = getLevelState(nextLevel);
		if (nextLevel >= level && level > 0)
		{
			bool bNextInfoShow = false;
			if (bHaveNextLevel && (nextLevel % 10 == 0 && levelState == E_CHECKPOINTSTATE_OPEN))
			{
				bNextInfoShow = true;
			}
			else if (!bHaveNextLevel && nextLevel == level)
			{
				bNextInfoShow = true;
			}
			if (!bNextInfoShow) continue;
			mCurMapLevel = nextLevel;
			Vec2 point = child->getPosition();
			Layout* pInner = mScrollViewLevel->getInnerContainer();
			const Size& frameSize = CCDirector::getInstance()->getOpenGLView()->getFrameSize();
			if ((point.x - frameSize.width / 2 >= 0) && (pInner->getContentSize().width - point.x >= frameSize.width))
				touchMovePanel(-point.x + frameSize.width / 2);
			else if (point.x - frameSize.width / 2 < 0)
			{
				touchMovePanel(0);

			}
			else if (pInner->getContentSize().width - point.x < frameSize.width)
			{
				touchMovePanel(-pInner->getContentSize().width + frameSize.width);
			}
			if (nextLevel % 10 != 0)//大关卡位置矫正
			{
				if (nextLevel % 10 == 1)
					point.y += 45;
				else
				{
					point.x -= 20;
				}
			}
			else
			{
				const CheckPoint* pAttr = sgTableManager()->getCheckPointByID(nextLevel);
				if (!pAttr) return;
				bool isBoss = pAttr->levelType == E_Level_Boss;
				if (isBoss)
					point.y += 20;
				else
					point.y += 5;
			}
			if (bHaveNextLevel)
			{
				mArmatureCurLevel->setPosition(Vec2(point.x,
					point.y + child->getContentSize().height / 2 + mArmatureCurLevel->getContentSize().height / 2));
				baozhiJump(point);
				mArmatureCurLevel->setVisible(false);
				break;
			}

		}

		else if ((tmpLevel = 1010 && nextLevel == 1010 && nextLevel == mLastPoint))
		{
			mCurMapLevel = nextLevel;
			Vec2 point = child->getPosition();
			mArmatureCurLevel->setPosition(Vec2(point.x,
				point.y + child->getContentSize().height / 2 + mArmatureCurLevel->getContentSize().height / 2 + 20));
			touchMovePanel(0);
			if (nextLevel % 10 != 0)//大关卡位置矫正
			{
				if (nextLevel % 10 == 1)
					point.y += 45;
				else
				{
					point.x -= 20;
				}
			}
			else
			{
				const CheckPoint* pAttr = sgTableManager()->getCheckPointByID(nextLevel);
				if (!pAttr) return;
				bool isBoss = pAttr->levelType == E_Level_Boss;
				if (isBoss)
					point.y += 20;
				else
					point.y += 5;
			}
			
			baozhiJump(point);
			mArmatureCurLevel->setVisible(false);
			break;
		}
	}
}


Node* SelectLevelPage::getLevelBtn(int level)
{
	std::map<int, Node* >::iterator it = mMapLevelBtns.find(level);
	if (it != mMapLevelBtns.end())
	{
		return it->second;
	}
	return nullptr;
}

void SelectLevelPage::touchMovePanel(float offsetX)
{
	Layout* pInner = mScrollViewLevel->getInnerContainer();
	pInner->setPositionX(offsetX);
	mNodeBg[3]->setPositionX(offsetX / 30);
	mNodeBg[2]->setPositionX(offsetX * 2 / 15);
	mNodeBg[1]->setPositionX(offsetX / 3);
	mNodeBg[0]->setPositionX(offsetX * 5 / 3);
}

void SelectLevelPage::baozhiJump(Vec2 movePos,bool isGoto)
{
	if (!mArmatureBaoZi)
	{
		CCLOG("[trace]SelectLevelPage::baozhiJump, XuanGuanBaoAni is Failed");
		return;
	}
	mArmatureBaoZi->stopAllActions();
	mArmatureBaoZi->getAnimation()->stop();
	
	mArmatureBaoZi->runAction(Sequence::create(DelayTime::create(0.18), MoveTo::create(0.4, movePos),NULL));

	mArmatureBaoZi->getAnimation()->play("jump_stand");
	mArmatureBaoZi->getAnimation()->setMovementEventCallFunc([isGoto](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID) {
		if (movementType ==  cocostudio::MovementEventType::COMPLETE)
		{
			armature->getAnimation()->play("jump_ing");
			armature->getAnimation()->setMovementEventCallFunc([isGoto](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID)
			{
				if (movementType == cocostudio::MovementEventType::COMPLETE)
				{
					armature->getAnimation()->play("jump_end");
					armature->getAnimation()->setMovementEventCallFunc([isGoto](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID)
					{
						if(movementType == cocostudio::MovementEventType::COMPLETE)
						{
							armature->getAnimation()->play("stand");
							if (isGoto && sgUserObj()->getPlayerUpgradeState() == 0)
							{
								PageFunction::Get()->gotoFunction(MSG_FUC_WIN_LEVEL);
							}
						}
					});
				}
			});
		}
	});
}
