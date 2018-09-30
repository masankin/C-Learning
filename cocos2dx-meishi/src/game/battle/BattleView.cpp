#include "BattleView.h"
#include "Map/MapHandler.h"
#include "GamePrecedure.h"
#include "BattleControl.h"
#include "game/prop/PropsControl.h"
#include "balance/BalanceControl.h"
#include "TimeCalculator.h"
#include "BuildView.h"
#include "Build.h"
#include "Camp.h"
#include "ArrowWidget.h"
#include "Message.h"
//#include "Formation.h"
#include "Formation/FormationV2.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"
#include "EnvSet.h" 

#include "guide/GuideManager.h"
#include "core/SoundManager.h"
#include "battle/BattleRecordsManager.h"
#include "core/ResManager.h"
#include "luaHelp/CppCallLuaHelper.h"
#include "CCLuaEngine.h"
#define BUILD_START_TAG 1000
#define LOADPROP_START 1120
BattleView::BattleView() : mIntialized(false), mSlider(0), isTouch(0), mBeginBuild(0), mEndBuild(0), mTimeUI(0)
	,mFaceBuildArmA(NULL)
	,mFaceBuildArmB(NULL)
	,mChatNode(NULL)
	, mBorn(false)
	, mSpiritUI(nullptr)
	, mOILSoundId(0)
	, mFREEZESoundId(0)
{
	EventListenerTouchOneByOne* _touchListener = EventListenerTouchOneByOne::create();
	_touchListener->onTouchBegan = CC_CALLBACK_2(BattleView::onTouchBegan, this);
	_touchListener->onTouchEnded = CC_CALLBACK_2(BattleView::onTouchEnded, this);
	_touchListener->onTouchMoved = CC_CALLBACK_2(BattleView::onTouchMoved, this);
	_touchListener->onTouchCancelled = CC_CALLBACK_2(BattleView::onTouchCancelled, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
}

BattleView::~BattleView()
{
}

bool BattleView::load()
{  

	mBuildViewList.clear();

	mPainting = ArrowWidget::create();

	Point originPos = Director::getInstance()->getVisibleOrigin();
	Point centerPos;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	centerPos.x = originPos.x + visibleSize.width / 2;
	centerPos.y = originPos.y + visibleSize.height /  2;

	// tilemap
	auto tileMap = sgMapHandler()->getTitleMap();
	auto layer_baidu = tileMap->getLayer("mask_baidu");
	auto layer_mask = tileMap->getLayer("mask");

	if (layer_baidu)
	{
		int channelId = sgChannelApi()->getChannelId();

		if (sgChannelApi()->isOnlineSdk() == true)	//联网版
		{
			if (channelId == ChannelApi::ChannelID_Lenovo || channelId == ChannelApi::ChannelID_Lenovo_LeShangDian || channelId == ChannelApi::ChannelID_Lenovo_YouXiZhongXin)
			{
				layer_baidu->setVisible(true);
			}
			else
			{
				layer_baidu->setVisible(false);
			}
		}
		else										//单机版
		{
			if (channelId == ChannelApi::ChannelID_BaiDu || channelId == ChannelApi::ChannelID_BaiDu_91 || channelId == ChannelApi::ChannelID_BaiDu_ZhuShou || channelId == ChannelApi::ChannelID_BaiDu_DuoKu || channelId == ChannelApi::ChannelID_BaiDu_TieBa)
			{
				layer_baidu->setVisible(true);
			}
			else
			{
				layer_baidu->setVisible(false);
			}
		}
	}

	if (layer_mask)
	{
		layer_mask->setVisible(true);
	}
	else
	{
		//layer_mask->setVisible(false);

		// Obstacle
		std::string obstacleFilename = sgGamePrecedure()->getCurMapInfo().mapObstacleName;
		std::string otcPlistFile = obstacleFilename + ".plist";
		std::string otcArmFile = obstacleFilename + ".xml";
		if (FileUtils::getInstance()->isFileExist(otcPlistFile)
			&& FileUtils::getInstance()->isFileExist(otcArmFile))
		{
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile(otcPlistFile);
			ArmatureDataManager::getInstance()->addArmatureFileInfo(otcArmFile);
			std::string strArmatureName = StringUtils::format("fla_otc_%d", sgGamePrecedure()->getCurMapInfo().mapId);
			if (nullptr != ArmatureDataManager::getInstance()->getAnimationData(strArmatureName))
			{
				Armature* pObstacle = Armature::create(strArmatureName);
				if (pObstacle)
				{
					pObstacle->getAnimation()->play("play");
					pObstacle->setPosition(centerPos);
					this->addChild(pObstacle, EBVC_OBSTACLE);
				}
			}
		}
	}

	this->addChild(tileMap, EBVZ_TILEMAP, EBVZ_TILEMAP);

	

	//bttale bg
	std::string mapName = sgGamePrecedure()->getCurMapInfo().mapBgName;
	const char* pScript = __String::createWithFormat("sgSceneManager():directLoadSceneImgRes(%d,\"%s\")", 
		MSG_FUC_PLAYING,mapName.c_str())->getCString();
	LuaEngine::getInstance()->executeString(pScript);

	Sprite* pBg = Sprite::create(mapName);
	pBg->setAnchorPoint(Vec2(0, 0));
	this->addChild(pBg, EBVZ_BG, EBVZ_BG);

	if (sgGamePrecedure()->getCurMapInfo().checkpt <= 1100)
	{
		auto bgEffect = sgResManager()->getAramatur("fla_beijingtx_ADDITIVE");//cocostudio::Armature::create("fla_beijingtx_ADDITIVE");
		bgEffect->setBlendFunc(BlendFunc::COMADDITIVE);
		bgEffect->setPosition(Vec2(640, 360));
		this->addChild(bgEffect, 1000);
		bgEffect->getAnimation()->play("play");

		auto bgEffect2 = sgResManager()->getAramatur("fla_beijingtx");//cocostudio::Armature::create("fla_beijingtx");
		bgEffect2->setPosition(Vec2(640, 360));
		this->addChild(bgEffect2, bgEffect->getZOrder() - 1);
		bgEffect2->getAnimation()->play("play");

	}

	this->getParent()->addChild(mPainting, EBVZ_ARROW, EBVZ_BG+15);

	// map cell
	const CampList& camps = sgBattleControl()->getCampList();
	CampList::const_iterator it = camps.begin();
	int tIndex = 1;
	for (; it!=camps.end(); ++it)
	{
		Camp* pCamp = it->second;

		const MapBulidList& buildlist = pCamp->getBuildList();

		MapBulidList::const_iterator bt = buildlist.begin();
		for (; bt!=buildlist.end(); ++bt)
		{
			BaseBuild* pBuild = bt->second;
			
			BuildView* pView = pBuild->getBuildView();
			
			BuildInfo buildinfo = pBuild->getBuildInfo(); 
			int key = pBuild->getKeyID();
			//打野类型不在这里创建
			if (mBuildViewList.find(key) == mBuildViewList.end() 
				/*&& pBuild->getBuildInfo().locationID != 0*/)
			{
				Node* pNode = createSoldierBubble(pBuild->getKeyID());
				if (pNode)
				{
					pView->setBubbleNode(pNode, pBuild);
					if (!pBuild->isVisible())
						pNode->setVisible(false);
				}

				pView->setPosition(pBuild->boundingBox().origin); 
				pView->setAnchorPoint(Vec2(0.5, 0.5));
				this->addChild(pView, EBVZ_WIDGET, buildinfo.insID);
				pView->setName(StringUtils::format("%d", buildinfo.insID));
				pView->setLocalZOrder(10);
				CCLOG("[trace]BattleView::load build:%d pos:(%f,%f)", key, pView->getPositionX(), pView->getPositionY());
				mBuildViewList.insert(std::make_pair(key, pView)); 
			}
			tIndex ++;
		}
	} 
	m_CanStartGuide = true; 
	mIntialized = true; 

	const Size& size = Director::getInstance()->getWinSize();
	//道具
	if (sgBattleControl()->getIsPvp() || sgGamePrecedure()->getCurMapInfo().checkpt >= LOADPROP_START)
	{
		
		mFreezeBg = Armature::create("fla_bing_quanping_ADDITIVE");
		mFreezeBg->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);
		mFreezeBg->setPosition(Vec2(size.width / 2, size.height / 2));
		mFreezeBg->setVisible(false);

		this->addChild(mFreezeBg,10001);


		mTimeback_Eff = cocostudio::Armature::create("fla_timeback_effect_ADDITIVE");
		mTimeback_Eff->setBlendFunc(BlendFunc::ADDITIVE);
		mTimeback_Eff->setPosition(Vec2(size.width / 2, size.height / 2));
		mTimeback_Eff->setVisible(false);
		this->addChild(mTimeback_Eff, EBVZ_EFFECT_HIGHT);

		mTimeback = cocostudio::Armature::create("fla_timeback");
		mTimeback->setPosition(Vec2(size.width / 2, size.height / 2));
		mTimeback->setVisible(false);
		mTimeback->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *pAr, cocostudio::MovementEventType type, const std::string& id){
		if (MovementEventType::COMPLETE == type)
		{
			mTimeback->setVisible(false);
			mTimeback_Eff->setVisible(false);
		}
		});
		this->addChild(mTimeback, EBVZ_EFFECT_HIGHT);

		mFeiDieback = cocostudio::Armature::create(!sgBattleControl()->getIsPvp() ? "fla_LaJiaoZD2" : "fla_huidie");
		mFeiDieback->setPosition(Vec2(size.width / 2, size.height / 2));
		mFeiDieback->setVisible(false);
		mFeiDieback->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *pAr, cocostudio::MovementEventType type, const std::string& id){
		if (MovementEventType::COMPLETE == type)
		{
			mFeiDieback->setVisible(false);
			mFeiDieback->setVisible(false);
		}
		});

		mFeiDieback->getAnimation()->setFrameEventCallFunc([this](cocostudio::Bone *bone, const std::string& frameEventName, int originFrameIndex, int currentFrameIndex){
		if (frameEventName == "atk")
		{
			sgBattleControl()->doCallCook();
		}

		});
		this->addChild(mFeiDieback);
		
	}
	

	//清理未完成的新手引导
	sgGuideManager()->unRegistGuideMessage(true);
	const CheckPoint* pPoint = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);
	if (pPoint->levelType != E_Level_Trans)
		sgGuideManager()->rollbackGuide();
	
	if (!sgBattleControl()->getIsPvp())
		sgGuideManager()->resetGuideDragFlag(sgGamePrecedure()->getCurMapInfo().checkpt);

	Size tWinSize = Director::getInstance()->getWinSize();
	//显示士气
	mPlayerNode = Node::create(); 
	mPlayerNode->setPosition(ccp(200,tWinSize.height * 0.85f));
	addChild(mPlayerNode, 99999);

	LabelTTF* tCamp1 = LabelTTF::create(); 
	tCamp1->setFontName("arial");
	tCamp1->setFontSize(16);
	tCamp1->setColor(ccc3(255,0,0));
	mPlayerNode->addChild(tCamp1,1,1000);


	mEnemyNode = Node::create(); 
	mEnemyNode->setPosition(ccp(200,tWinSize.height * 0.81f));
	addChild(mEnemyNode, 99999);
	 
	LabelTTF* tCamp2 = LabelTTF::create(); 
	tCamp2->setFontName("arial");
	tCamp2->setFontSize(16);
	tCamp2->setColor(ccc3(255,0,0));
	mEnemyNode->addChild(tCamp2,1,1000);

	mFaceBuildArmA = sgResManager()->getAramatur("fla_youxidingwei");//Armature::create("fla_youxidingwei");
	this->getParent()->addChild(mFaceBuildArmA,EBVZ_ARROW);
	mFaceBuildArmA->setVisible(false);
	mFaceBuildArmB = sgResManager()->getAramatur("fla_youxidingweiB");//Armature::create("fla_youxidingweiB");
	this->getParent()->addChild(mFaceBuildArmB,EBVZ_ARROW);
	mFaceBuildArmB->setVisible(false);
	// buff icon 
	// slider
	initSlider();

#if (MGAME_MAP_OBJ_DEBUG > 0)
	auto & objList = sgMapHandler()->getMapObjectList();
	for (auto itr = objList.begin(); itr != objList.end(); ++itr)
	{
		MapObject * obj = itr->second;
		if (obj && obj->getDebugNode())
			this->addChild(obj->getDebugNode(), 100);
	}
#endif
	 mBuffTipsRefNum = 0;
	 mBattleTip = NULL;
	return true;
}




void BattleView::destroy()
{
	mBeginBuild = nullptr;
	mEndBuild = nullptr;
}

void BattleView::doCampBuildUpgrade(int buildID)
{
	BuildViewList::iterator it = mBuildViewList.find(buildID);
	if (it == mBuildViewList.end())
	{
		CCLOG("[error] BattleView::doCampBuildUpgrade find build:%d error!", buildID);
		return;
	}
	it->second->doBuildUpgrade();
}

void BattleView::updateUI(float dt)
{
	setTimeLeft(sgBalanceControl()->getTimeLeft());
	setSpirit(sgBattleControl()->getGlyphBattleMgr()->getSpirit());

	BuildViewList::iterator it = mBuildViewList.begin();
	for (; it!=mBuildViewList.end(); ++it)
	{
		int key = it->first;
		BaseBuild* pBuild = sgBattleControl()->getTargetBuild(key);
		if (pBuild && pBuild->isVisible())
		{
			pBuild->updateUI();
		} 
	} 
	if(sgBattleControl()->isBorning() == true && m_CanStartGuide == true)
	{
		m_CanStartGuide = false;
		if(sgBattleControl()->getIsPvp() == false)
		{
			activeGuide(); 
		} 
	}
	updateBuffTips();
	showBattleTips(dt);
}

void BattleView::setTimeLeft(float leftTime)
{
	if (mTimeUI)
	{
		leftTime = leftTime > 0 ? leftTime : 0;
		char temp[32] = "\0";
		sprintf(temp, "%02d:%02d", static_cast<int>(leftTime / 60), static_cast<int>(leftTime) % 60);
		std::string sTimerString = temp;
		mTimeUI->setString(sTimerString.c_str());
	}

	//if (mTimeSprite)
	//{
	//	mTimeSprite->setPercent(leftTime*100/mTimeTotal);
	//}
}

void BattleView::setSpirit(float leftTime)
{
	if (leftTime < 0.0f)
		return;

	if (mSpiritUI)
	{
		char temp[32] = "\0";
		sprintf(temp, "%d%%", static_cast<int>(leftTime));
		std::string sTimerString = temp;
		mSpiritUI->setString(sTimerString.c_str());
	}
}

bool BattleView::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	if (true == sgBattleRecordsManager()->getIsPlayback())
	{
		return false;
	}

	if (sgPropsControl()->isPropIng())
	{
		sgPropsControl()->doProp(touch->getLocation(), sgBattleControl()->getOwnCamp());
		return isTouch;
	}

	if (isTouch)
	{
		BaseBuild* build = sgBattleControl()->getTargetBuildTouch(touch->getLocation());
		
		if (sgEnvSet("FREE_CONTROL") == 1)
		{
			if (build && (build->getInviInfo() == nullptr || build->getInviInfo()->mSend))
			{
				mBeginBuild = build;
			}
		}
		else
		{ 
			if (build && build->getCamp() == sgBattleControl()->getOwnCamp() && 
				(build->getInviInfo() == nullptr || build->getInviInfo()->mSend))
			{
				mBeginBuild = build;
			}
		}
		
			
	}

	const MapBulidList & buildList = sgBattleControl()->getBuildList();
	for (auto it = buildList.begin(); it != buildList.end();it ++)
	{
		
		if (it->second->getBuildType() == EBULID_TYPE_TURRET)
		{
			Turret *build = dynamic_cast<Turret*> (it->second);
			if (build->getBuildView()->getChooseLevelShow())
			{
				build->getBuildView()->showChooseLevel(false);
			}
		}
	}
	
	return isTouch;
}

void BattleView::onTouchMoved(Touch *touch, Event *unused_event)
{

	if (mBeginBuild)
	{
		Vec2 end = touch->getLocation();

		mEndBuild = sgBattleControl()->getTargetBuildTouch(end);

		mFaceBuildArmA->setVisible(false);
		mFaceBuildArmB->setVisible(false);

		if (mEndBuild && mEndBuild->isEnable() == true &&
			(mEndBuild->getInviInfo() == nullptr || mEndBuild->getInviInfo()->mObtain))
		{
			if (mEndBuild != mBeginBuild)
			{
				Armature* pArmature = NULL;
				if (sgBattleControl()->getOwnCamp() == mEndBuild->getCamp())
				{
					pArmature = mFaceBuildArmA;
				}else
					pArmature = mFaceBuildArmB;

				pArmature->setPosition(mEndBuild->getPosition().x,mEndBuild->getPosition().y+mEndBuild->getBuildView()->getBuildSize().height*0.5);
				pArmature->setVisible(true);
				pArmature->getAnimation()->playByIndex(0);
			}
			//CCLOG("BattleView::onTouchMoved endBuild locationID is %d",mEndBuild->getBuildInfo().locationID);
			mPainting->stat2end(mBeginBuild->getPosition(), mEndBuild->getPosition(),true);
		} 
		else
		{
			mEndBuild = nullptr;
			mPainting->stat2end(mBeginBuild->getPosition(), end);
		}

	}
}

void BattleView::onTouchEnded(Touch *touch, Event *unused_event)
{	
	if(mChatNode)
	{
		mChatNode->setVisible(false);
	}

	doGuideProp(touch->getLocation());

	if (mBeginBuild && mEndBuild && mBeginBuild != mEndBuild)
	{
		eCampType camp = mBeginBuild->getBuildInfo().camp; 
		
		if (sgEnvSet("FREE_CONTROL") == 1)
			CCLOG("Free control");
		else
		{
			if (camp != sgBattleControl()->getOwnCamp())
			{
				CCLOG("[trace]camp != sgBattleControl()->getOwnCamp() camp = %d", camp);
				mPainting->reset();
				return;
			}

		}

		CCLOG("[trace]camp == sgBattleControl()->getOwnCamp() camp = %d",camp);

		doSoldierMove(mBeginBuild->getKeyID(), mEndBuild->getKeyID());
	}

	mBeginBuild = nullptr;
	mEndBuild = nullptr;

	mPainting->reset();
	if (mFaceBuildArmA->getAnimation()->isPlaying())
	{
		mFaceBuildArmA->getAnimation()->stop();
		sgSoundManager()->playMusicEffect(SOUND_SOLDIER_OUT);
	}
	if (mFaceBuildArmB->getAnimation()->isPlaying())
	{
		mFaceBuildArmB->getAnimation()->stop();
		sgSoundManager()->playMusicEffect(SOUND_SOLDIER_OUT);
	}
	mFaceBuildArmA->setVisible(false);
	mFaceBuildArmB->setVisible(false);

	if (!mBorn && sgBattleControl()->isBorning() && !sgBattleControl()->getIsPvp())
	{
		sgBattleControl()->stopBornAni();
		mBorn = true;
	}
}

void BattleView::onTouchCancelled(Touch *touch, Event *unused_event)
{
	mBeginBuild = nullptr;
	mEndBuild = nullptr;
	
	mPainting->reset();
	if (mFaceBuildArmA->getAnimation()->isPlaying())
	{
		mFaceBuildArmA->getAnimation()->stop();
	}
	if (mFaceBuildArmB->getAnimation()->isPlaying())
	{
		mFaceBuildArmB->getAnimation()->stop();
	}
	mFaceBuildArmA->setVisible(false);
	mFaceBuildArmB->setVisible(false);
} 
 

void BattleView::clean()
{
	mBuildViewList.clear();
	mOilEffectList.clear();
	this->removeAllChildren();

	mPainting = 0;
	mSlider = 0;
	mIntialized = false;
	isTouch = false; 
	mBorn = false;
	
	m_SoldierBubbleMap.clear();
	
	mBuffArmatureMap.clear();

	for (auto & pActionTips : mBattleTipsList)
	{
		if (pActionTips)
			pActionTips->release();
	}
	mBattleTipsList.clear(); 
	sgGuideManager()->setStopDragFlag(false);
}



void BattleView::onEnter()
{ 
	Layer::onEnter();
	

	BuildViewList::iterator it = mBuildViewList.begin();
	for (; it != mBuildViewList.end();++it)
	{
		it->second->onEnter();
	}
}

void BattleView::onExit()
{ 
	BuildViewList::iterator it = mBuildViewList.begin();
	for (; it != mBuildViewList.end(); ++it)
	{
		it->second->onExit();
	}
	Layer::onExit();
}

void BattleView::setSlider(HpSlider* pSlider)
{
	if (pSlider)
	{
		mSlider = pSlider;
	}
}

void BattleView::refreshSliderPercent()
{
	if (mSlider)
	{
		mSlider->refreshSliderPercent();
	}
}

void BattleView::setEnbelTouch(bool touch)
{
	isTouch = touch;
	BuildViewList::iterator it = mBuildViewList.begin();
	for (; it != mBuildViewList.end(); ++it)
	{
		it->second->setTouch(touch);
		if (it->second->getCampType() != ECAMP_TYPE_Creeps)
			it->second->updateUI();

	}
}


Node* BattleView::createSoldierBubble(int key)
{
	Node* tNode = Node::create();
	addChild(tNode, EBVZ_EFFECT_LOW);

	m_SoldierBubbleMap.insert(std::make_pair(key, tNode));
	return tNode;
}

void BattleView::playPropAni(ePropType eType, const Point& pos /* = Point::ZERO */)
{
	if (eType == PROP_TYPE_FREEZE)
	{
		if (mFreezeBg)
		{
			mFreezeBg->setVisible(true);
			mFreezeBg->getAnimation()->play("play");
		}
	}

	else if (eType == PROP_TYPE_TIME)
	{
		if (mTimeback_Eff && mTimeback)
		{
			mTimeback->setVisible(true);
			mTimeback_Eff->setVisible(true);

			mTimeback_Eff->getAnimation()->play("play");
			mTimeback->getAnimation()->play("play");

		}
	}

	else if (eType == PROP_TYPE_COOKING)
	{

		mFeiDieback->setVisible(true);
		mFeiDieback->setPosition(pos);
		//mFeiDieback->setBlendFunc(BlendFunc::ADDITIVE);
		mFeiDieback->getAnimation()->play("play");
	
	}
}

void BattleView::playFreezeAni(const Point& pos,float time)
{
	if (mFREEZESoundId != 0)
	{
		sgSoundManager()->stopMusicEffect(mFREEZESoundId);
		mFREEZESoundId = 0;
	}
	mFREEZESoundId = sgSoundManager()->playMusicEffect(SOUND_PROP_FREEZE, true);

	cocostudio::Armature* pAni2 = nullptr;
	cocostudio::Armature* armature = nullptr;

	auto fun = [this](cocostudio::Armature *pAr, cocostudio::MovementEventType type, const std::string& id){
		if (MovementEventType::COMPLETE == type)
		{
			if (id == "born")
				pAr->getAnimation()->play("stand", 1);
			if (id == "end")
			{
				pAr->removeFromParent();
				if (mFREEZESoundId != 0)
				{
					sgSoundManager()->stopMusicEffect(mFREEZESoundId);
					mFREEZESoundId = 0;
				}
			}
		}
	};

	if (!sgBattleControl()->getIsPvp())
	{
		pAni2 = Armature::create("fla_bingMGb");
		pAni2->setBlendFunc(BlendFunc::COMADDITIVE);
		pAni2->getAnimation()->play("born");
		pAni2->setPosition(pos);
		this->addChild(pAni2, EBVZ_EFFECT_LOW);
		pAni2->getAnimation()->setMovementEventCallFunc(fun);
	}

	std::string sAniName = !sgBattleControl()->getIsPvp() ? "fla_bingMG" : "fla_bingdong";
	armature = Armature::create(sAniName);
	armature->getAnimation()->setMovementEventCallFunc(fun);
	armature->setPosition(pos);

	std::string sAniBegin = !sgBattleControl()->getIsPvp() ? "born" : "start";
	armature->getAnimation()->play(sAniBegin);

	CallFunc* callFun = CallFunc::create([armature,pAni2](){
		if (armature)
			armature->getAnimation()->play("end", 0);
		if (pAni2)
			pAni2->getAnimation()->play("end", 0);
	
	});
	armature->runAction(Sequence::createWithTwoActions(DelayTime::create(time), callFun));
	//TODO: 需要改层级 坐标越低层级
	this->addChild(armature, EBVZ_EFFECT_LOW);
}


void BattleView::playBombShootAni(const Point& begPos, const Point& endPos, const std::function<void()>& callback)
{
	// 在战斗场景添加子弹
	Sprite* pBullet = Sprite::create("bulletNor.png");
	pBullet->setPosition(begPos);

	Vector<cocos2d::FiniteTimeAction*> bulletActionList;
	bulletActionList.pushBack(EaseSineIn::create(JumpTo::create(0.3f, endPos, 100, 1)));
	bulletActionList.pushBack(CallFunc::create(callback));
	bulletActionList.pushBack(RemoveSelf::create());

	pBullet->runAction(Sequence::create(bulletActionList));

	this->addChild(pBullet, EBVZ_FORMATION);
}


void BattleView::playBombExplosionAni(const Point& pos)
{
	auto armature = cocostudio::Armature::create("fla_eqp041bz");
	if (nullptr == armature)
		return;

	armature->getAnimation()->setMovementEventCallFunc([](cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName){
		if (type == cocostudio::COMPLETE)
		{
			// Important!!!!! release armature
			pAr->removeFromParent();
		}
	});

	armature->setPosition(pos.x, pos.y - 10.0f);
	armature->getAnimation()->play("born");
	this->addChild(armature, EBVZ_FORMATION);
}


/*
void BattleView::animationCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (MovementEventType::COMPLETE == type && pAr->getName() == "fla_bing_bingdong_ADDITIVE" && aniName == "start")
	{
		pAr->getAnimation()->play("end");
	}
	if (MovementEventType::COMPLETE == type && pAr->getName() == "fla_bing_bingdong_ADDITIVE" && aniName == "end")
	{
		pAr->removeFromParentAndCleanup(true);
	}
	if (MovementEventType::COMPLETE == type && pAr == mTimeback)
	{
		mTimeback->setVisible(false);
		mTimeback_Eff->setVisible(false);
	}
	if (MovementEventType::COMPLETE == type && pAr == mFeiDieback)
	{
		mFeiDieback->setVisible(false);
		mFeiDieback->setVisible(false);
	}
}*/

void BattleView::initSlider()
{
	if (mSlider)
	{
		mSlider->initSlider();
	}
}


void BattleView::refreshJungle(int buildID, int locationID, int buffID)
{   
	 
	 
}

void BattleView::returnFromJungleBuild(int srcID, int destID)
{
 
}

void BattleView::closeJungleBuild(int srcID, int srcCamp, int destCamp, int buildID, int locationID)
{  
}

void BattleView::showMorale(eCampType campType, float tValue)
{ 
	if(NULL == mPlayerNode || NULL == mEnemyNode) {return;}
	if(campType == ECAMP_TYPE_Player)
	{
		LabelTTF* tLabel = dynamic_cast<LabelTTF* >(mPlayerNode->getChildByTag(1000));
		if(NULL != tLabel)
		{ 
			tLabel->setString(CCString::createWithFormat("Player's morale value is : %.2f", tValue)->getCString());
		}
	}
	else if(campType == ECAMP_TYPE_AI)
	{ 
		LabelTTF* tLabel = dynamic_cast<LabelTTF* >(mEnemyNode->getChildByTag(1000));
		if(NULL != tLabel)
		{ 
			tLabel->setString(CCString::createWithFormat("Enemy's morale value is : %.2f", tValue)->getCString());
		}
	}
} 


 


 

void BattleView::clearBuildView(int buildID)
{
	BuildViewList::iterator tIter = mBuildViewList.find(buildID);
	if( tIter != mBuildViewList.end())
	{
		tIter->second->removeFromParentAndCleanup(true);
		mBuildViewList.erase(tIter);
	} 
}

void BattleView::clearBuildRefView(int buildID)
{ 
	const MapBulidList& buildlist = sgBattleControl()->getBuildList();
	MapBulidList::const_iterator it = buildlist.find(buildID);
	if (it != buildlist.end())
	{
		it->second->setBuildView(NULL);
	} 
}

bool BattleView::activeGuide(int guideLocation, int param1, int param2)
{
	sgGuideManager()->setExtraCondition(param1);
	sgGuideManager()->setExtraCondition2(param2);

	int gateID = sgGamePrecedure()->getCurMapInfo().checkpt;   
	int level = sgUserObj()->getPersonalInfo()->explevel();//sgUserObj()->getPlayerLevel();
	return sgGuideManager()->registGuideMessage(gateID, level, guideLocation, this, this, false);
}

void BattleView::doGuideProp(Point pos)
{   
	const GuideData* pCurData = sgGuideManager()->getCurGuideData();
	if (!pCurData)
		return;
	if (!sgGuideManager()->checkIsReady() || pCurData->getType() != E_GUIDELOC_USEPROP)
		return;

	const CheckPoint* pCheck = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);
	const GuideData* pData = sgGuideManager()->getGuideDataByGuideID(GuideID_PveProp);
	if (!pData)
		return;
	if (pCheck->levelType != E_Level_Trans && pData->getGateID() != pCheck->id)
		return;
	sgPropsControl()->doUseGuideProp(pos);
}


std::string BattleView::getTips(eBuffType buffType)
{  
	char positionParam[64] = { 0, };
	switch (buffType)
	{
	case E_Buff_AttackSpeed:
		return sgTextHelper(TEXT_BUFF_ATTACKSPEED); 
	case E_Buff_ProduceSpeed: 
		return sgTextHelper(TEXT_BUFF_PRODUCTSPEED); 
	case E_Buff_Attack:
		return sgTextHelper(TEXT_BUFF_ADDATTACK); 
	case E_Buff_MoveSpeed:
		return sgTextHelper(TEXT_BUFF_MOVESPEED); 
	case E_Buff_Defence:		
		return sgTextHelper(TEXT_BUFF_ADDDEFENCE); 
	case E_Buff_FillPeople:
		return sgTextHelper(TEXT_BUFF_FILLPEOPLE); 
	default:
		return ""; 
	}   
}

void BattleView::initBuffTips(int insID)
{

	Size winSize = Director::getInstance()->getWinSize();   
	mBattleTip = ActionTip::create("", NULL);
	mBattleTip->setFontSize(36);
	mBattleTip->setPosition(winSize.width/2, winSize.height/2);  
	this->addChild(mBattleTip,99999);
	mBuffTipsRefNum = 0;

	std::string text = sgTextHelper(TEXT_BUFF);
	CreepsBuild* pBuild = dynamic_cast<CreepsBuild*>(sgBattleControl()->getTargetBuildByIns(insID));
	const Creeps* pInfo =  sgTableManager()->getCreeps(pBuild->getCurrentCreepsID());
	if(NULL == pInfo)
		return;
	int type = pInfo->getType();
	switch (type)
	{
	case 1: // 辣椒
		{ 
			mBattleTip->addText(text+ sgTextHelper(TEXT_BUFF_LAJIAO));
		}
		break;

	case 2: // 大葱
		{ 
			mBattleTip->addText(text+ sgTextHelper(TEXT_BUFF_DACONT));
		}
		break;

	case 3: // 鸡蛋
		{ 
			mBattleTip->addText(text+ sgTextHelper(TEXT_BUFF_JIDAN));
		}
		break;

	case 4: // 奶酪
		{ 
			mBattleTip->addText(text+ sgTextHelper(TEXT_BUFF_NAILAO));
		}
		break;

	case 5: // 蒜头
		{ 
			mBattleTip->addText(text+ sgTextHelper(TEXT_BUFF_SUANTOU));
		}
		break;

	case 6: // 虾米
		{
			mBattleTip->addText(text+ sgTextHelper(TEXT_BUFF_XIAMI));
		}
		break;

	default:

		break;
	} 
}

void BattleView::addBuffTips(eBuffType buffType, int insID)
{   
	if(0 == mBuffTipsRefNum)
	{ 
		sgTimeCalculator()->removeTimeCalcultor("BuffTipsTimer");
		sgTimeCalculator()->createTimeCalcultor("BuffTipsTimer",0.5f);
		initBuffTips(insID);
	}
	mBuffTipsRefNum+=1;
	mBattleTip->addText(getTips(buffType));  
}

void BattleView::updateBuffTips()
{ 
	if(sgTimeCalculator()->hasKey("BuffTipsTimer") == false || mBuffTipsRefNum <= 0)
	{
		return;
	}
	float time = sgTimeCalculator()->getTimeLeft("BuffTipsTimer");
	if(time <= 0.0f && NULL != mBattleTip)
	{ 
		sgTimeCalculator()->createTimeCalcultor("BuffTipsTimer",0.5f);
		//加入战斗提示 by edward 2015-09-23
		Action* pAction = Sequence::create(
			MoveBy::create(.3f, Vec2(0, 50)),
			DelayTime::create(1.0f),
			Spawn::create(MoveBy::create(.2f, Vec2(0, 30)), FadeOut::create(.2f), NULL),
			NULL);
		mBattleTip->setAction(pAction);
		mBattleTip->run();
		mBuffTipsRefNum = 0;
	}	
}

void BattleView::clearBuffTips()
{

}


void BattleView::playReturnSoliderParticle(const Point& startPos, const Point& endPos, const std::function<void()>& callback)
{

// 	auto createParticle = [this, &startPos, &endPos, callback](){
// 
// 		Vector<FiniteTimeAction*> vecActionList;
// 
// 		ParticleSystemQuad* pParticle = ParticleSystemQuad::create("solider_return.plist");
// 		if (nullptr == pParticle)
// 			return;
// 
// 		pParticle->setPosition(startPos);
// 		pParticle->setAutoRemoveOnFinish(true);
// 
// 		Point vecDir = (endPos - startPos) / 2.0f;
// 		Point ptControl;
// 		int retRand = cocos2d::random(0, 1);
// 		if (retRand)
// 		{
// 			ptControl = vecDir + vecDir.getPerp() / 2.0f + startPos;
// 		}
// 		else
// 		{
// 			ptControl = vecDir + vecDir.getRPerp() / 2.0f + startPos;
// 		}
// 
// 		ccBezierConfig config;
// 		config.endPosition = endPos;
// 		config.controlPoint_1 = ptControl;
// 		config.controlPoint_2 = ptControl;
// 
// 		vecActionList.pushBack(BezierTo::create(0.6f, config));
// 		vecActionList.pushBack(CallFunc::create(callback));
// 
// 		pParticle->runAction(Sequence::create(vecActionList));
// 
// 		this->addChild(pParticle, 6);
// 	};
// 
// 	
// 	this->runAction(Sequence::create(DelayTime::create(delay), CallFunc::create(createParticle), nullptr));


	Vector<FiniteTimeAction*> vecActionList;

	ParticleSystemQuad* pParticle = ParticleSystemQuad::create("solider_return.plist");
	if (nullptr == pParticle)
		return;

	pParticle->setPosition(startPos);
	pParticle->setAutoRemoveOnFinish(true);

	Point vecDir = (endPos - startPos) / 2.0f;
	Point ptControl;
	int retRand = cocos2d::random(0, 2);
	if (retRand == 0)
	{
		ptControl = vecDir + vecDir.getPerp() / 2.0f + startPos;
	}
	else if ( retRand == 1)
	{
		ptControl = vecDir + startPos;
	}
	else
	{
		ptControl = vecDir + vecDir.getRPerp() / 2.0f + startPos;
	}

	ccBezierConfig config;
	config.endPosition = endPos;
	config.controlPoint_1 = ptControl;
	config.controlPoint_2 = ptControl;

	vecActionList.pushBack(BezierTo::create(0.5f, config));
	vecActionList.pushBack(CallFunc::create(callback));

	pParticle->runAction(Sequence::create(vecActionList));

	this->addChild(pParticle, EBVZ_EFFECT_LOW);

}


void BattleView::addBuildBuffEffect(int buildUid, const std::string& effectFile, GLubyte opacity /* = 255 */)
{
	BaseBuild* pBaseBuild = sgBattleControl()->getTargetBuild(buildUid);
	if (nullptr == pBaseBuild)
	{
		CCLOG("[error]BattleView::addBuildBuffEffect : build is nullptr");
		return;
	}


	Armature* pArmature = Armature::create(effectFile);
	if (nullptr == pBaseBuild)
	{
		CCLOG("[error]BattleView::addBuildBuffEffect : armature is nullptr");
		return;
	}
	
	pArmature->setPosition(pBaseBuild->getPosition());
	pArmature->setScaleY(0.75f * 0.65f);
	pArmature->setScaleX(0.65f);
	pArmature->getAnimation()->play("play", -1, 1);
	pArmature->setOpacity(opacity);
	mBuffArmatureMap.insert(std::make_pair(buildUid, pArmature));

	this->addChild(pArmature, EBVZ_BOTTOM_EFFECT);
}


void BattleView::removeBuildBuffEffect(int buildUid)
{
	std::map<int, Node* >::iterator it = mBuffArmatureMap.find(buildUid);
	if (it == mBuffArmatureMap.end())
	{
		CCLOG("[warn]BattleView::removeBuildBuffEffect : this build hasn't buff effect : %d", buildUid);
		return;
	}

	Node* pArmature = it->second;
	if (nullptr != pArmature)
	{
		this->removeChild(pArmature);
	}
	else
	{
		CCLOG("[warn]BattleView::removeBuildBuffEffect : armature is nullptr");
	}

	mBuffArmatureMap.erase(buildUid);

}

void BattleView::playCookBottom(Vec2 pos)
{
	std::string sAniName = !sgBattleControl()->getIsPvp() ? "fla_LaJiaoZD_ADDITIVE" : "fla_huidie_ADDITIVE";
	auto effect = Armature::create(sAniName);
	if (effect)
	{
		effect->getAnimation()->setMovementEventCallFunc([this, sAniName](cocostudio::Armature *pAr, cocostudio::MovementEventType type, const std::string& id){

			if (pAr->getName() == sAniName && id == "play" && type == COMPLETE)
			{
				pAr->removeFromParent();
			}
		});
		BlendFunc blend;
		blend.src = GL_ONE;
		blend.dst = GL_ONE;

		effect->setBlendFunc(blend);
		effect->setPosition(pos);
		effect->getAnimation()->play("play");
		this->addChild(effect, EBVZ_EFFECT_HIGHT);
	};



}

void BattleView::setPaintingPos(Point posBegan, Point posEnd)
{
	mPainting->stat2end(posBegan, posEnd, true);         
}

void BattleView::resetPainting()
{
	mPainting->reset();
}

void BattleView::addOilEffect(int id ,const Vec2& pos)
{
	if (mOILSoundId != 0)
	{
		sgSoundManager()->stopMusicEffect(mOILSoundId);
		mOILSoundId = 0;
	}

	if (true == sgBattleControl()->getIsPvp())
	{
		mOILSoundId = sgSoundManager()->playMusicEffect(SOUND_PROP_OIL, true);
	}
	else
	{
		mOILSoundId = sgSoundManager()->playMusicEffect(SOUND_PROP_POISON, true);
	}

	std::string  sAniName1 = sgBattleControl()->getIsPvp() ? "fla_digouyou1" : "fla_dumogu";
	std::string  sAniName2 = sgBattleControl()->getIsPvp() ? "fla_digouyou" : "fla_dumoguTX";

	Armature* pOilEffect = Armature::create(sAniName1);
	pOilEffect->setPosition(pos);

	if (!sgBattleControl()->getIsPvp())
	{
		//pOilEffect->setBlendFunc(BlendFunc::COMADDITIVE);
		pOilEffect->setOpacity(225 * 0.9);
	}

	this->addChild(pOilEffect, EBVZ_TILEMAP);
	pOilEffect->getAnimation()->play("born");

	Armature* pOilEffectTX = Armature::create(sAniName2);
	pOilEffectTX->setPosition(pos);
	this->addChild(pOilEffectTX, EBVZ_EFFECT_HIGHT);
	pOilEffectTX->getAnimation()->play("born");
	
	pOilEffectTX->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *pAr, cocostudio::MovementEventType type, const std::string& id){
		if (MovementEventType::COMPLETE == type && id == "born")
		{
			pAr->getAnimation()->play("stand");
		}

		if (MovementEventType::COMPLETE == type && id == "end")
		{
			pAr->removeFromParent();
		}
	});


	pOilEffect->getAnimation()->setMovementEventCallFunc([this, pOilEffectTX](cocostudio::Armature *pAr, cocostudio::MovementEventType type, const std::string& id){
		if (MovementEventType::COMPLETE == type && id == "born")
		{
			pAr->getAnimation()->play("stand");
		}

		if (MovementEventType::COMPLETE == type && id == "end")
		{
			pOilEffectTX->getAnimation()->play("end");
			pAr->removeFromParent();
			
		}
	});

	mOilEffectList.insert(std::make_pair(id, pOilEffect));
	//pOilEffect->set

}

void BattleView::removeOilEffect(int id)
{
	std::map<unsigned, cocostudio::Armature*>::iterator  it = mOilEffectList.find(id);

	if (it != mOilEffectList.end())
	{
		if (it->second != nullptr)
		{
			it->second->getAnimation()->play("end");
		}
		mOilEffectList.erase(it);
	}
	if (mOILSoundId != 0)
	{
		sgSoundManager()->stopMusicEffect(mOILSoundId);
		mOILSoundId = 0;
	}
}

void BattleView::playBombAttAni(const Point& pos, const std::string attAniName)
{
	
	auto armature = cocostudio::Armature::create(attAniName);
	if (nullptr == armature)
		return;

	armature->getAnimation()->setMovementEventCallFunc([](cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName){
		if (type == cocostudio::COMPLETE)
		{
			// Important!!!!! release armature
			pAr->removeFromParent();
		}
	});
	armature->setBlendFunc(BlendFunc::ADDITIVE);
	armature->setPosition(pos.x, pos.y - 10.0f);
	armature->getAnimation()->play("play");
	this->addChild(armature, EBVZ_FORMATION);
}


void BattleView::addBattleTips(const std::string& tips, int fontSize, const Color4B* pTextColor, const Color4B* pOutlineColor, const std::string& iconfile)
{
	ui::RichText* pRichText = RichText::create();
	if (nullptr == pRichText)
		return;

	Size size;

	Label* pLabel = Label::createWithTTF(tips, "fonts/font.TTF", fontSize);
	if (pLabel)
	{
		if (nullptr != pTextColor)
			pLabel->setTextColor(*pTextColor);

		if (nullptr != pOutlineColor)
			pLabel->enableOutline(*pOutlineColor, 2);

		auto pElem = ui::RichElementCustomNode::create(1
			, Color3B(0xff, 0xff, 0xff)
			, 255
			, pLabel);

		pRichText->pushBackElement(pElem);

		size.width += pLabel->getContentSize().width;
		size.height = size.height < pLabel->getContentSize().height ? pLabel->getContentSize().height : size.height;
	}


	if (!iconfile.empty())
	{
		Sprite* pIcon = Sprite::create(iconfile);
		if (pIcon)
		{
			auto pElem = ui::RichElementCustomNode::create(1
				, Color3B(0xff, 0xff, 0xff)
				, 255
				, pIcon);

			pRichText->pushBackElement(pElem);

			size.width += pIcon->getContentSize().width;
			size.height = size.height < pIcon->getContentSize().height ? pIcon->getContentSize().height : size.height;
		}
	}

	pRichText->ignoreContentAdaptWithSize(false);
	pRichText->setContentSize(size);
	pRichText->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
	pRichText->setCascadeOpacityEnabled(true);

	pRichText->retain();
	mBattleTipsList.push_back(pRichText);

	if (!sgTimeCalculator()->hasKey("BattleView_BattleTips"))
	{
		sgTimeCalculator()->createTimeCalcultor("BattleView_BattleTips", 0.9f);
	}
}


void BattleView::showBattleTips(float dt)
{
	if (sgTimeCalculator()->getTimeLeft("BattleView_BattleTips") == 0)
	{
		if (mBattleTipsList.size() > 0)
		{
			Size winSize = Director::getInstance()->getWinSize();
			Point startPos(winSize.width / 2.0f, winSize.height / 2.0f + 80.0f);

			std::vector<Node*> removeList;
			const unsigned int showCount = 99;

			unsigned int curCount = 0;
			auto curItr = mBattleTipsList.begin();
			auto endItr = mBattleTipsList.end();
			while (curItr != endItr && curCount < showCount)
			{
				auto pNode = *curItr;
				if (pNode)
				{
					pNode->setPosition(startPos);

					Action* pAction = Sequence::create(
						DelayTime::create(0.05f * curCount),
						MoveBy::create(.1f, Vec2(0, 50)),
						MoveBy::create(0.7f, Vec2(0, 30)),
						Spawn::create(FadeOut::create(.08f), MoveBy::create(.1f, Vec2(0, 50)), nullptr),
						RemoveSelf::create(),
						nullptr);

					startPos.y -= pNode->getContentSize().height;
					curCount += 1;

					pNode->runAction(pAction);
					this->addChild(pNode, EBVZ_TIPS);
					pNode->release();
				}

				curItr = mBattleTipsList.erase(curItr);
			}
		}

		sgTimeCalculator()->resetTimeCalcultor("BattleView_BattleTips");
	}
}

void BattleView::doSoldierMove(int srcBuildID, int dstBuildID)
{
	bool soldierMove = false;
	 
	if (sgGuideManager()->getStopDragFlag() && !sgBattleControl()->getIsPvp())
	{
		return;
	}
	if (sgBattleControl()->getIsPvp())
		soldierMove = true;  

	//判断当前是否存在拖曳的引导,如果有则判断当前操作是否符合引导的约束
	if (sgGuideManager()->checkOpeartor(E_GuideOperator_Drag))
	{ 
		const GuideData* pData = sgGuideManager()->getCurGuideData();
		if (pData && srcBuildID == atoi(pData->getGuideNodeID().c_str()) && dstBuildID == atoi(pData->getDragNodeID().c_str()))
		{
			sgGuideManager()->unRegistGuideMessage(false, E_GuideOperator_Drag);
			soldierMove = true;
		}
	}
	else
		soldierMove = true; 
	
	if (soldierMove)
	{
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d", srcBuildID, dstBuildID, FormationV2::NewId(), 0, sgBattleControl()->getOutputPercent());
		sgMessageManager()->sendMessage(EMT_MOVE, params);
		CCLOG("[trace]BattleView::onTouchEnded move event params:%s.", params);
	}
	else
	{
		CCLOG("[error]BattleView:onTouched move failed because guide is running which id is %d and getRunning is %s",
			sgGuideManager()->getCurGuideID(),
			sgGuideManager()->getIsRunning() == true ? "true" : "false");
	}
}

void BattleView::addExternalEffect(cocostudio::Armature* pArmature)
{
	if (pArmature)
	{
		this->addChild(pArmature, EBVZ_EFFECT_LOW);
	}
}
