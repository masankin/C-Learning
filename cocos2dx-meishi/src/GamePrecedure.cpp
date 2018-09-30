#include "GamePrecedure.h"
#include "battle/BattleControl.h"
#include "battle/Map/MapHandler.h"
#include "prop/PropsControl.h"
#include "MessageManager.h"
#include "data/ItemManager.h"
#include "sync/SyncControl.h"
#include "balance/BalanceControl.h"
#include "TimeCalculator.h"
#include "game/ui//system/GameScene.h"
#include "ui/dialogs/GenerlDialog.h"
#include "EnvSet.h"
#include "net/NetManager.h"
#include "channel/ChannelApi.h"
#include "game/guide/GuideManager.h"
#include "net/UserObj.h"
#include "battle/JungleBattleManager.h"
#include "battle/MoraleManager.h"
#include "buff/BuffManager.h"
#include "SoundManager.h"
#include "battle/BattleRecordsManager.h"

GamePrecedure::GamePrecedure() :isGameIng(false), mInCheckPoint(false), mIsClean(true)
{
	mIsEnter = false;
	init();
}

GamePrecedure::~GamePrecedure()
{

}

bool GamePrecedure::init()
{

	Director::getInstance()->getScheduler()->scheduleUpdate(this, -1, false);

	//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui/uiAtlas2.plist");


	// 选关卡设置
	mCurrentInfo.checkpt = 1010;
	mCurrentInfo.mapName = "map/map_0_3.tmx";
	mCurrentInfo.mapId = 1;
	mCurrentInfo.mapBgName = "ui/uiAtlas_Battle_bg_11.jpg";
	mCurrentInfo.mapObstacleName = "";
	mCurrentInfo.bUpArrow = true;
	mCurrentInfo.bEnemyInfo = true;
	mCurrentInfo.bHelpInfo = true;
	mInCheckPoint = false;
	return true;
}

void GamePrecedure::update(float delta)
{
#define NET_RECV_INTERVAL 1

	static int iRecvCnt = 0;
	if (iRecvCnt++ >= NET_RECV_INTERVAL)
	{
		iRecvCnt = 0;
		sgNetManager()->RecvPkg(NULL, 0);
	}

	sgSyncControl()->update();
	sgTimeCalculator()->update();

	if (isGameIng)
		sgBattleControl()->update(delta);

	sgBattleRecordsManager()->update(delta);

	sgMessageManager()->update(); 

	sgUserObj()->update(delta);

	sgGuideManager()->update(delta); 

	sgSoundManager()->update(delta);
}

void GamePrecedure::enterGame()
{
	// 加载地图
	sgMapHandler()->createMap(mCurrentInfo.mapName);

	sgBattleControl()->loadMapData(mCurrentInfo.mapId);

	//if (sgBattleControl()->getIsPvp())
	//{
	//	sgTimeCalculator()->createTimeCalcultor("pvp_hearbeat", 5);
	//}
	

	//BattleView* pLayer = sgBattleControl()->getBattleView();
	//auto scence = Scene::create();
	//scence->addChild(pLayer);

	//auto director = Director::getInstance();
	//if (director->getRunningScene() != NULL)
	//{
	//	director->replaceScene(scence);
	//}
	//else
	//{
	//	director->runWithScene(scence);
	//} 

	//// 开启模式 
	if (!sgBattleControl()->getIsPvp())
	{
		setIncheckPoint(true);
	}

	sgBalanceControl()->startCheckPoint(mCurrentInfo.checkpt);
	isGameIng = true;
}

void GamePrecedure::destroy()
{
	(void)0;
}

bool GamePrecedure::preEnterGame()
{
	bool pRet = false;
	do
	{
		mIsClean = false;
		setEnterGame(true);
		//sgResourceManager()->loadResources([](const std::string&, const float) {});
		pRet = true;
	} while (0); 

	return pRet;
}



void GamePrecedure::preEndGame()
{
#ifdef MGAME_PLATFORM_MPLUS
	if (mIsClean == false)
	{
#endif
		isGameIng = false;
		sgBattleControl()->cleanGame();
		sgTimeCalculator()->cleanData();
		sgMapHandler()->cleanMapData();
		sgBalanceControl()->clean();
		sgPropsControl()->cleanCdTime();
		sgUserObj()->setIsHost(false);

		if (!sgBattleControl()->getIsPvp())
			setIncheckPoint(false);

		setEnterGame(false);

#ifdef MGAME_PLATFORM_MPLUS
		mIsClean = true;
	}
#endif
} 

bool GamePrecedure::selectLevel(int levelId)
{
	const CheckPoint* pCheckPt = sgTableManager()->getCheckPointByID(levelId);
	if (!pCheckPt) return false;

	mCurrentInfo.checkpt = levelId;
	mCurrentInfo.mapName = pCheckPt->tileMap + ".tmx";
	mCurrentInfo.mapId = levelId;
	mCurrentInfo.mapBgName = pCheckPt->bgMap + ".jpg";// "ui/uiAtlas_Battle_bg_11.jpg";
	mCurrentInfo.mapObstacleName = pCheckPt->obstacleMap;
	mCurrentInfo.bUpArrow = pCheckPt->viewUpgrade != 0;
	mCurrentInfo.bEnemyInfo = pCheckPt->viewEnemyInfo != 0;
	mCurrentInfo.bHelpInfo = pCheckPt->viewHelpInfo != 0;
	mCurrentInfo.bTipsProp = pCheckPt->tipsProp != 0;
	

	if (!FileUtils::getInstance()->isFileExist(mCurrentInfo.mapName))
	{
		std::string szTitle = "sorry";
		std::string szContent = StringUtils::format("sorry no this file %s", mCurrentInfo.mapName.c_str());
		GenerlDialog::show(szTitle, szContent);
		return false;
	}

	sgBattleControl()->setIsPvp(false);
	return true;
}

bool GamePrecedure::selectPvpMap(int mapID)
{
	const CheckPoint* pCheckPt = sgTableManager()->getCheckPointByID(mapID);
	if (!pCheckPt) return false;

	mCurrentInfo.checkpt = pCheckPt->id;
	mCurrentInfo.mapName = pCheckPt->tileMap + ".tmx";
	mCurrentInfo.mapId = pCheckPt->id;
	mCurrentInfo.mapBgName = pCheckPt->bgMap + ".jpg";
	mCurrentInfo.mapObstacleName = pCheckPt->obstacleMap;
	mCurrentInfo.bUpArrow = pCheckPt->viewUpgrade != 0;
	mCurrentInfo.bEnemyInfo = pCheckPt->viewEnemyInfo != 0;
	mCurrentInfo.bHelpInfo = pCheckPt->viewHelpInfo != 0;

	sgBattleControl()->setIsPvp(true);

	return true;
}

bool GamePrecedure::nextLevel()
{
	return false;
	/*int pBeforeLv = mCurrentInfo.checkpt;
	int cpNext = -1;
	do 
	{
		int stars = sgUserObj()->getCheckPointStar(mCurrentInfo.checkpt);

		// 只找本章节主线关卡
		std::vector<int> cpVec = sgTableManager()->getNextCheckPoints(mCurrentInfo.checkpt, stars);
		std::vector<int>::iterator it = cpVec.begin();
		for (; it != cpVec.end(); ++it)
		{
			if (*it % 10 == 0)
			{
				cpNext = *it;
				break;
			}
		}
	} while (0);

	if (cpNext == -1 || cpNext / 1000 > 2)
		return false;

	return selectLevel(cpNext);*/
}














void  GamePrecedure::addDieEffect(SoliderView * soliderView)
{
	auto ar = cocostudio::Armature::create("fla_ghost");
	ar->getAnimation()->play("play");
	ar->getAnimation()->setMovementEventCallFunc(ar, movementEvent_selector(GamePrecedure::animationCallback));
	ar->setPositionY(ar->getPositionY() - 10);
	soliderView->getParent()->addChild(ar, 100);
	ar->setPosition(soliderView->getPosition());
}

void GamePrecedure::animationCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (type == cocostudio::COMPLETE)
	{
		pAr->removeFromParent();
	}
}

void GamePrecedure::setEnterGame(bool isEnter)
{
	mIsEnter = isEnter;
}

bool GamePrecedure::getEnterGame()
{
	return mIsEnter;
}

void GamePrecedure::setGameIng(bool bIng)
{
	static Vector<Node*> gActions;
	static std::set<void*> gSchedulers;

	isGameIng = bIng;
	if (bIng)
	{
		//Director::getInstance()->getScheduler()->resumeTargets(gSchedulers);
		//sgItemManager()->init(0);
		Director::getInstance()->getActionManager()->resumeTargets(gActions);
		//sgBattleControl()->getBattleView()->onEnter();
		sgTimeCalculator()->resume();
	}
	else
	{
		//gSchedulers = Director::getInstance()->getScheduler()->pauseAllTargets();
		gActions = Director::getInstance()->getActionManager()->pauseAllRunningActions();
		//sgBattleControl()->getBattleView()->onExit();
		sgTimeCalculator()->pause();
		sgSoundManager()->stopAllMusicEffects();
	}
}

void GamePrecedure::enableGameRunning(bool open)
{
	static Vector<Node*> gActions2;  
	isGameIng = open;
	if (open)
	{ 
		Director::getInstance()->getActionManager()->resumeTargets(gActions2); 
		sgTimeCalculator()->resume();
	}
	else
	{ 
		gActions2 = Director::getInstance()->getActionManager()->pauseAllRunningActions();
		Director::getInstance()->getActionManager()->resumeTarget(Director::getInstance()->getRunningScene()->getChildByTag(TAG_GUIDELAYER)); 
		sgTimeCalculator()->pause();
	} 
}
