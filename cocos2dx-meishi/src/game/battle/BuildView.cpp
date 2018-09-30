#include "BuildView.h"
#include "Singleton.h"
#include "battle/BattleControl.h"
#include "battle/Build.h"
#include "Message.h"
#include "BattleControl.h"
#include "GamePrecedure.h"
#include "TimeCalculator.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"
#include "game/guide/GuideManager.h"
#include <math.h>
#include "SoundManager.h"

#include "cocostudio/ActionTimeline/CSLoader.h"
#include "BattleRecordsManager.h"
#include "core/ResManager.h"
#define Fla_Arrow_Tag 66
BuildView::BuildView() :isClick(0),
mChangeBg(0),
mLevelingBg(0),
mLevelingBg2(0),
mLevelUpBg(0),
mBeHurtBg(0),
mPeopleNode(0),
mProgress(0),
isBorn(0),
canTouch(0),
mBubbleNode(0),
mRangeBg(0),
mChooseLevelNode(0),
mSpriteType(0),
m_UpProgress(nullptr),
m_UpgaradeBg(nullptr),
mInvisArm(nullptr),
mPeopleNumLabel(nullptr),
mBottomEff(nullptr),
mTomb(nullptr),
mInvisArmPvp(nullptr),
mBornAnim(nullptr),
mShowEnemyInfo(false)
{
	mBgArmature = NULL;
}

BuildView::~BuildView()
{
	CCLOG("");
}

bool BuildView::init(eCampType etype, int buildID, eBulidType buildType)
{
	Node* mRoot = CSLoader::createNode("Build.csb");

	mBuildBgNode = mRoot->getChildByName("mBuildBgNode");
	mPeopleNode = mRoot->getChildByName("mPeopleNode");
	mArrow = mRoot->getChildByName("mArrow");
	dynamic_cast<ui::Text*>(mPeopleNode->getChildByName("mPeopleNumLabel"))->setVisible(false);

	//加入动态升级箭头

	Armature *pArrowArmature = sgResManager()->getAramatur("fla_shenjiJT"); //Armature::create("fla_shenjiJT");
	pArrowArmature->setName("fla_shenjiJT");
	pArrowArmature->getAnimation()->play("play");
	mArrow->addChild(pArrowArmature, 1, Fla_Arrow_Tag);

	mTouch = dynamic_cast<ui::Widget*>(mRoot->getChildByName("mTouch"));
	mTouch->addClickEventListener(CC_CALLBACK_1(BuildView::onClick, this));
	mTouch->setSwallowTouches(false); 

	this->addChild(mRoot);
	this->setLocalZOrder(10);

	mSpriteType = dynamic_cast<Sprite*>(mRoot->getChildByName("mSpriteType"));
	mSpriteType->setVisible(false);
	//mSpriteType->setScale(0.6);

	mBuildID = buildID;
	 
	mBuildType = buildType;  
	m_TagSp = Sprite::create("queen.png");
	if(NULL != m_TagSp)
	{ 
		mBuildBgNode->addChild(m_TagSp, 1000);
		m_TagSp->setVisible(false);
	}
	m_IsWarning = false;
	m_StopWarning = false;
	m_WarningSp = sgResManager()->getAramatur("fla_sos");//Armature::create("fla_sos");
	if (NULL != m_WarningSp)
	{
		m_WarningSp->setAnchorPoint(Vec2(0.5, 0.5));
		m_WarningSp->getBone("sos_az242")->getChildArmature()->setBlendFunc(BlendFunc::COMADDITIVE);
		m_WarningSp->setVisible(false);

		sgBattleControl()->getBattleView()->addChild(m_WarningSp, 1005);
		m_WarningSp->setTag(655123);
	}


	mExchangeBg = sgResManager()->getAramatur("fla_gaijian");//Armature::create("fla_gaijian");
	mExchangeBg->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(BuildView::animationCallback));
	mExchangeBg->setVisible(false);
	mBuildBgNode->addChild(mExchangeBg, 10);

	m_PrBg = Sprite::createWithSpriteFrameName("jindutiao.png");
	mBuildBgNode->addChild(m_PrBg, 1000);
	m_PrBg->setVisible(false);

	m_Pr = Sprite::createWithSpriteFrameName("jindutiao0001.png");
	auto pAction = Animate::create(AnimationCache::getInstance()->getAnimation("jindutiao"));
	m_Pr->runAction(RepeatForever::create(pAction));
	mBuildBgNode->addChild(m_Pr, 1000);
	m_Pr->setVisible(false);


	Sprite* m_PrBg2 = Sprite::createWithSpriteFrameName("jindutiao.png");
	m_ProgressBg = ProgressTimer::create(m_PrBg2);
	m_ProgressBg->setType(kCCProgressTimerTypeBar);
	m_ProgressBg->setBarChangeRate(ccp(1, 0));
	m_ProgressBg->setMidpoint(ccp(1, 0));
	m_ProgressBg->setPercentage(100);
	m_ProgressBg->setVisible(false);
	mBuildBgNode->addChild(m_ProgressBg, 1000);
	isJungleBorn = false;

	//炮塔升级选择
	mChooseLevelNode = mRoot->getChildByName("mChooseLevelNode");
	showChooseLevel(false);

	Button* _aoeBtn = dynamic_cast<Button*>(mChooseLevelNode->getChildByName("mAoeButton"));
	Button* _iceBtn = dynamic_cast<Button*>(mChooseLevelNode->getChildByName("mIceButton"));
	_aoeBtn->addClickEventListener(CC_CALLBACK_1(BuildView::onChooseCallBack, this));
	_iceBtn->addClickEventListener(CC_CALLBACK_1(BuildView::onChooseCallBack, this));

	if (sgBattleRecordsManager()->getIsPlayback() == true)
	{
		_aoeBtn->setTouchEnabled(false);
		_iceBtn->setTouchEnabled(false);
	}

	return true;
}


void BuildView::onEnter()
{
	Node::onEnter();
	if (mBgArmature)
	{
		mBgArmature->onEnter();
	}


	
}

void BuildView::onExit()
{
	if (mBgArmature)
		mBgArmature->onExit();
	Node::onExit();
}


BuildView* BuildView::createView(eCampType etype, int buildID, eBulidType buildType)
{
	BuildView* pRet = new BuildView();
	if (pRet && pRet->init(etype, buildID, buildType))
	{ 
		pRet->autorelease();
		return pRet;
	}
	return NULL;
}

bool BuildView::changeExpress(eExpressType type)
{
	if (nullptr != mBgArmature)
	{
		mBgArmature->removeFromParent();
		mBgArmature = nullptr;
	}

	if (EXP_TYPE_Invalid == type)
		return false;

	mBgArmature = sgResManager()->getAramatur(sgTableManager()->getAnimationNameByExpress(type));
	if (nullptr == mBgArmature)
	{
		CCLOG("[error]BuildView::changeExpress, Create mBgArmature failed!, express Index = %d", (int)type);
		return false;
	}

	mBgArmature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(BuildView::animationCallback));

	mBuildBgNode->addChild(mBgArmature);

	return true;
}

void BuildView::updateUI()
{
	BaseBuild* pBuild = sgBattleControl()->getTargetBuild(mBuildID);
	if (!pBuild)
	{
		CCLOG("[error] BuildView::updateUI get target build:%d failed.", mBuildID);
		return;
	}

	mBuildType = pBuild->getBuildType();

	if(pBuild->checkCurBeAttack() == false)
	{
		callSOS(false);
	} 


	// level变化 更新图片 
	if (pBuild->getIsDirty(EBDIR_LEVEL))
	{
		// 打野怪没有等级变化
		if (mBgArmature)
		{
			//mBgArmature->setScale(getScaleByLevel(pBuild->getLevel()));
			this->adjustByLevel(pBuild->getBuildType(), pBuild->getLevel());

			changeFlagByLevel(pBuild->getLevel());

			changeTowerLogo(pBuild);

			//音效
			if (pBuild->getCamp() == sgBattleControl()->getOwnCamp() )
			{
				sgSoundManager()->playMusicEffect(SOUND_BUILD_LEVELUP);
			}

			
			

			//adjustNode(pBuild->getBuildType(), mBgArmature->getContentSize(), m_ProgressBg->getScale());

			//绘制攻击范围点
			//drawAttackRange();
			changeTurretBone(pBuild);

			updateSosPostion();
		}

		//炮塔换箭头
		if (pBuild->getBuildType() == EBULID_TYPE_TURRET)
		{
			string aniName = pBuild->getLevel() == 4 ? "fla_shenjiJTb" : "fla_shenjiJT";
			changeArrow(aniName);
		}
	}

	// ani
	if (pBuild->getIsDirty(EBDIR_Ani))
	{
		playAni(pBuild->getCurentAni());
	}


	//显示箭头 pvp不显示升级箭头
	bool bUpArrow = sgGamePrecedure()->getCurMapInfo().bUpArrow;
	setArrowVisible(bUpArrow && pBuild->checkCanUpgrade() && pBuild->getCamp() == sgBattleControl()->getOwnCamp() && pBuild->getInviInfo() == nullptr && canTouch);

	
	if (pBuild->getCamp() == sgBattleControl()->getOwnCamp())
	{
		if (!pBuild->checkCanUpgrade())
		{
			if (getChooseLevelShow())
				showChooseLevel(false);
		}
		else
		{
			if (!sgGuideManager()->checkGuideTypeFinished(E_GUIDELOC_LEVELUP2))
			{
				sgBattleControl()->getBattleView()->activeGuide(E_GUIDELOC_LEVELUP2);
			}
		}
	}

	

	// 显示士兵数
	updatePeopleNum(pBuild);
	

	//阵营改变
	if (pBuild->getIsDirty(EBDIR_CAMEP))
	{
		if (mChangeBg == nullptr)
		{
			mChangeBg = sgResManager()->getAramatur("fla_zhuanhuanTX");//cocostudio::Armature::create("fla_zhuanhuanTX");
			mChangeB2 = sgResManager()->getAramatur("fla_zhuanhuanTX_ADDITIVE");//cocostudio::Armature::create("fla_zhuanhuanTX_ADDITIVE");
			mChangeB2->setBlendFunc(BlendFunc::COMADDITIVE);

			auto battleView = sgBattleControl()->getBattleView();
			Vec2 world = mBuildBgNode->getParent()->convertToWorldSpace(mBuildBgNode->getPosition());
			Vec2 pos = battleView->convertToNodeSpace(world);
			mChangeBg->setPosition(pos);
			mChangeB2->setPosition(pos);

			battleView->addChild(mChangeBg, EBVZ_EFFECT_LOW);
			battleView->addChild(mChangeB2, EBVZ_EFFECT_LOW);
			mChangeBg->setVisible(false);
			mChangeB2->setVisible(false);
		}
		else
		{
			mChangeBg->setVisible(true);
			mChangeBg->getAnimation()->play("play");

			if (pBuild->getBuildType() == EBULID_TYPE_INSISTUTE)
			{
				mChangeB2->setVisible(true);
				mChangeB2->getAnimation()->play("play");
			}

			if (pBuild->mBeHurtSoundId != 0)
			{
				sgSoundManager()->stopMusicEffect(pBuild->mBeHurtSoundId);
				pBuild->mBeHurtSoundId = 0;
			}

			//音效
			sgSoundManager()->playMusicEffect(SOUND_BUILD_CHANGE_CAMP);
		}

		stopUpgradeAnimation();
		changeBottomEffect(pBuild);

		if (this->changeExpress(pBuild->getExpress()))
		{
			if (pBuild->getCamp() == ECAMP_TYPE_Neutral)
			{
				//睡眠动画
				//Bone * bone = NULL;
				//bone = mBgArmature->getBone("Fantuan_ZD");

				//auto ar = cocostudio::Armature::create("fla_Fantuan_ZD");
				//ar->getAnimation()->play("play", -1, 1);

				//int index = bone->getDisplayManager()->getCurrentDisplayIndex();
				//bone->addDisplay(ar, index);
				//bone->changeDisplayWithIndex(index, true);
			}


			//this->setContentSize(mBgArmature->getContentSize());
			changeFlagByLevel(pBuild->getLevel());

			this->adjustByLevel(pBuild->getBuildType(), pBuild->getLevel());

			//mBgArmature->setScale(getScaleByLevel(pBuild->getLevel()));

			if (getCampType() == ECAMP_TYPE_Neutral)
				grayArmature(mBgArmature);

			//adjustNode(pBuild->getBuildType(), mBgArmature->getContentSize(), m_ProgressBg->getScale());
		}
		


		if (!isBorn && pBuild->getCamp() == sgBattleControl()->getOwnCamp())//出生动画
		{
			mBornAnim = sgResManager()->getAramatur("fla_dingweiguang"); //Armature::create("fla_dingweiguang");
			mBornAnim->setLocalZOrder(mBuildBgNode->getZOrder() - 1);
			mBornAnim->getAnimation()->play("play");
			this->addChild(mBornAnim);

			mBornAnim->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *pAr, cocostudio::MovementEventType type, const std::string& id){
				if (MovementEventType::COMPLETE == type)
				{
					mBornAnim->removeFromParent();
					mBornAnim = nullptr;
				}
			});

		}

		if(getCampType() == ECAMP_TYPE_Creeps && isJungleBorn == false)
		{
			isJungleBorn = true;
			playCreepsBornAni();
		}

		isBorn = true;

		playAni(BUILD_ANI_STAND);
		if (mBuildType == EBULID_TYPE_TURRET)
		{
			//绘制攻击范围点
			drawAttackRange();
		}

		changeTowerLogo(pBuild);

		updateBubble(pBuild->getCamp(), pBuild->getBuildType()); 

		showExchangeProgress(false);

		updateUpgradePrg(pBuild->getCamp(), 0);

		changeTurretBone(pBuild);

		updateSosPostion();

		pBuild->removeInvisibility();
	}

	if (pBuild->getIsDirty(EBDIR_TYPE))
	{
		if (mBuildType == EBULID_TYPE_BARRACK)
		{
			playAni(BUILD_ANI_STAND);
			changeFlagByLevel(pBuild->getLevel());
			changeArrow("fla_shenjiJT");
			if (mRangeBg)
			{
				mRangeBg->removeFromParent();
				mRangeBg = 0;
			}
		}
		else if (mBuildType == EBULID_TYPE_TURRET)
		{
			playAni(BUILD_ANI_STAND);
			//绘制攻击范围点
			drawAttackRange();
			changeTowerLogo(pBuild);

		}
		else if (mBuildType == EBULID_TYPE_INSISTUTE)
		{
			playAni(BUILD_ANI_STAND);
			if (mRangeBg)
			{
				mRangeBg->removeFromParent();
				mRangeBg = 0;
			}
		}
		changeBottomEffect(pBuild);

		stopUpgradeAnimation();

		updateSosPostion();
	}

	updatePeoplePercent(pBuild);

	checkOccupyTag();
}

void BuildView::doBuildUpgrade()
{
	const BaseBuild* pBuildInfo = sgBattleControl()->getTargetBuild(mBuildID);
	if (!pBuildInfo)
		return;

	int toLevel = pBuildInfo->getLevel();

	//TODO: play upgrade effect

	char params[64] = { 0, };
	sprintf(params, "%d", mBuildID);
	sgMessageManager()->sendMessage(EMT_UPGRADE, params);
}

void BuildView::doBuildChange(eBulidType enType)
{
	//改变类型
	char params[64] = { 0, };
	sprintf(params, "%d %d", mBuildID,enType);
	sgMessageManager()->sendMessage(EMT_PROP_CHANGE, params);
}

void BuildView::animationCallback(Armature * pAr, MovementEventType type, const std::string& aniName)
{ 
	if (type == COMPLETE && pAr == mBgArmature)
	{ 
		if (aniName == "rc_skill_start")
		{
			mBgArmature->getAnimation()->play("rc_skill_ing", -1, 0);
		}
		else if (aniName == "rc_skill_ing")
		{
			mBgArmature->getAnimation()->play("rc_skill_end", -1, 0);
		}
		else if (aniName == "lc_skill_start")
		{
			mBgArmature->getAnimation()->play("lc_skill_ing", -1, 0);
		}
		else if (aniName == "lc_skill_ing")
		{
			mBgArmature->getAnimation()->play("lc_skill_end", -1, 0);
		}
		else if (aniName == "paotai_shoot")
		{
			mBgArmature->getAnimation()->play("paotai");
		}
		
		else if (pAr->getName() == "fla_jiabingli_ADDITIVE")
		{
			pAr->setVisible(false);
		}
		else if(aniName == "dead")
		{
			//this->setVisible(false);
			this->showCreepsTomb(true);
		}
		else if(aniName == "born")
		{
			playAni(BUILD_ANI_STAND);	
		}
		else
		{
			playAni(BUILD_ANI_STAND);
		}
	}

	if (type == COMPLETE && pAr == mRangeBg)
	{
		playAni(BUILD_ANI_RANGEING);
	}

	//	兵营	 4/5的概率播放眨眼动画
	if (type == LOOP_COMPLETE && pAr == mBgArmature && aniName == "stand")
	{	
		int result = rand() % 5;	

		if (result <= 4)
		{
			if(mBgArmature->getAnimation()->getAnimationData()->getMovement("blink"))
			{
				mBgArmature->getAnimation()->play("blink", -1, 0);
			} 
		}
	}

	if (type == COMPLETE && pAr == mBottomEff && aniName == "start")
	{
		pAr->getAnimation()->play("stand");
	}

	if (type == COMPLETE && pAr == mBottomEff && aniName == "end")
	{
		pAr->removeFromParent();
		mBottomEff = nullptr;
	}

	if (pAr->getName() == "fla_buffguangb_ADDITIVE" && aniName == "play" && type == COMPLETE)
	{
		pAr->removeFromParent();
	}

	if ((pAr->getName() == "fla_yinshen_ADDITIVE" || pAr->getName() == "fla_yinshenb") && type == COMPLETE)
	{
		if (aniName == "born")
		{
			pAr->getAnimation()->play("stand");
		}
		if (aniName == "end")
		{
			pAr->removeFromParentAndCleanup(true);
		}
	}

	if (type == COMPLETE && pAr == mExchangeBg && aniName == "end")
	{
		showExchangeProgress(false);
	}

	if ((pAr->getName() == "fla_BingLiBao" || pAr->getName() == "fla_jiabingli_ADDITIVE"  || pAr->getName() == "fla_BingLiBao2") 
		&& aniName == "play" && type == COMPLETE)
	{
		pAr->removeFromParent();
	}

}

void BuildView::onClick(Ref*)
{
	if (true == sgBattleRecordsManager()->getIsPlayback())
	{
		return;
	}
	if (isClick)
	{
		isClick = false;
		if (getCampType() == sgBattleControl()->getOwnCamp() && canTouch)
		{
			doBuildUpgrade(); 
		}
		
	}
	else
	{
		scheduleOnce(schedule_selector(BuildView::onCaleTime),0.25f);
		isClick = true;
	}

}

void BuildView::onCaleTime(float dt)
{
	if (isClick) {

		isClick= false;
		//CCLog("singleclick");
	}

} 

void BuildView::playAni(eBuildAni eAni)
{
	if (NULL == mBgArmature)
		return;
	
	BaseBuild* pBuild = sgBattleControl()->getTargetBuild(mBuildID);

	switch (eAni)
	{
	case BUILD_ANI_STAND:

		if (pBuild->getBuildType() == EBULID_TYPE_TURRET)
		{
			if (mBgArmature->getAnimation()->getAnimationData()->getMovement("paotai"))
			{
				mBgArmature->getAnimation()->play("paotai", -1, 1);
			}
		}
		else if (pBuild->getBuildType() == EBULID_TYPE_INSISTUTE)
		{
			if (mBgArmature->getAnimation()->getAnimationData()->getMovement("yanjiusuo"))
			{
				mBgArmature->getAnimation()->play("yanjiusuo", -1, 1);
			}
		}
		else if(pBuild->getBuildType() == EBUILD_TYPE_STABLES)
		{
			if (mBgArmature->getAnimation()->getAnimationData()->getMovement("jiasubing"))
			{ 
				mBgArmature->getAnimation()->play("jiasubing", -1, 1);
			}
		}
		else
		{
			if (mBgArmature->getAnimation()->getCurrentMovementID() != "stand")
			{
				if (mBgArmature->getAnimation()->getAnimationData()->getMovement("stand"))
					mBgArmature->getAnimation()->play("stand", -1, 1);
			}
		}

		break;
	case BUILD_ANI_BEHURT:


		if (mBgArmature->getAnimation()->getAnimationData()->getMovement(StringUtils::format("behurt_%d", pBuild->getBuildType()))
			&& pBuild->getBuildType() != EBULID_TYPE_TURRET)
		{
			mBgArmature->getAnimation()->play(StringUtils::format("behurt_%d", pBuild->getBuildType()), -1, 0);
		}

		if (mBeHurtBg == 0)
		{
			mBeHurtBg = cocostudio::Armature::create("fla_beidatexiao");

			Vec2 world = mBuildBgNode->getParent()->convertToWorldSpace(mBuildBgNode->getPosition());
			Vec2 pos = sgBattleControl()->getBattleView()->convertToNodeSpace(world);// mBuildBgNode->addChild(mBottomEff);
			mBeHurtBg->setPosition(pos);
			sgBattleControl()->getBattleView()->addChild(mBeHurtBg, EBVZ_EFFECT_LOW);

			//mBeHurtBg->setBlendFunc(BlendFunc::ADDITIVE);
			mBeHurtBg->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(BuildView::BeHurtBgCallback));
			mBeHurtBg->setVisible(false);
		}
		else
		{
			mBeHurtBg->setVisible(true);
			mBeHurtBg->getAnimation()->play("play", -1, 0);
		}

		break;
	case BUILD_ANI_OUTPUT_STAR:
		if (mBgArmature->getAnimation()->getAnimationData()->getMovement("rc_skill_start"))
		{
			mBgArmature->getAnimation()->play("rc_skill_start", -1, 0);
		}
		break;
	case BUILD_ANI_OUTPUTING:
		if (mBgArmature->getAnimation()->getAnimationData()->getMovement("rc_skill_ing"))
		{
			mBgArmature->getAnimation()->play("rc_skill_ing", -1, 0);
		}
		break;
	case BUILD_ANI_OUTPUT_END:
		if (mBgArmature->getAnimation()->getAnimationData()->getMovement("rc_skill_end"))
		{
			mBgArmature->getAnimation()->play("rc_skill_end", -1, 0);
		}
		break;
	case BUILD_ANI_OUTPUT_STAR_RIGHT:
		if (mBgArmature->getAnimation()->getAnimationData()->getMovement("lc_skill_start"))
		{
			mBgArmature->getAnimation()->play("lc_skill_start", -1, 0);
		}
		break;
	case BUILD_ANI_OUTPUTING_RIGHT:
		if (mBgArmature->getAnimation()->getAnimationData()->getMovement("lc_skill_ing"))
		{
			mBgArmature->getAnimation()->play("lc_skill_ing", -1, 0);
		}
		break;
	case BUILD_ANI_OUTPUT_END_RIGHT:
		if (mBgArmature->getAnimation()->getAnimationData()->getMovement("lc_skill_end"))
		{
			mBgArmature->getAnimation()->play("lc_skill_end", -1, 0);
		}
		break;
		//case BUILD_ANI_INSISTUTE_STAND:
		//	if(mBgArmature->getAnimation()->getAnimationData()->getMovement("yanjiusuo"))
		//	{
		//		mBgArmature->getAnimation()->play("yanjiusuo", -1, 1);
		//	}		
		//	break;
		//case BUILD_ANI_TURRET_STAND:
		//	if(mBgArmature->getAnimation()->getAnimationData()->getMovement("paotai"))
		//	{
		//		mBgArmature->getAnimation()->play("paotai", -1, 1);
		//	} 
		//	break;
	case BUILD_ANI_TURRET_ATTACK:
		if (mBgArmature->getAnimation()->getAnimationData()->getMovement("paotai_shoot"))
		{
			mBgArmature->getAnimation()->play("paotai_shoot", -1, 0);
		}
		break;
	case BUILD_ANI_RANGE_BORN:
		if (mRangeBg->getAnimation()->getAnimationData()->getMovement("start"))
		{
			mRangeBg->getAnimation()->play("start");
		}
		break;
	case BUILD_ANI_RANGEING:
		if (mRangeBg->getAnimation()->getAnimationData()->getMovement("stand"))
		{
			mRangeBg->getAnimation()->play("stand");
		}
		break;
	case BUILD_ANI_EXCHANGE:
		if (mExchangeBg->getAnimation()->getAnimationData()->getMovement("step1")
			&& mExchangeBg->getAnimation()->getCurrentMovementID() != "step1")
		{
			mExchangeBg->getAnimation()->play("step1");
		}
		showExchangeProgress(true);
		break;
	case BUILD_ANI_EXCHANGE1:
		if (mExchangeBg->getAnimation()->getAnimationData()->getMovement("step2") 
			&& mExchangeBg->getAnimation()->getCurrentMovementID() != "step2")
		{
			mExchangeBg->getAnimation()->play("step2");
		}
		break;
	case BUILD_ANI_EXCHANGE2:
		if (mExchangeBg->getAnimation()->getAnimationData()->getMovement("step3")
			&& mExchangeBg->getAnimation()->getCurrentMovementID() != "step3")
		{
			mExchangeBg->getAnimation()->play("step3");
		}
		break;
	case BUILD_ANI_EXCHANGE3:
		if (mExchangeBg->getAnimation()->getAnimationData()->getMovement("step4")
			&& mExchangeBg->getAnimation()->getCurrentMovementID() != "step4")
		{
			mExchangeBg->getAnimation()->play("step4");
		}
		break;
	case BUILD_ANI_EXCHANGE_END:
		if (mExchangeBg->getAnimation()->getAnimationData()->getMovement("end"))
		{
			mExchangeBg->getAnimation()->play("end");
		}
		break;
	
	case BUILD_ANI_SMALL_CRY:
		if (mBgArmature->getAnimation()->getAnimationData()->getMovement("fail_kua"))
		{
			std::string name = mBgArmature->getAnimation()->getCurrentMovementID();
			if (name != "fail_kua")
			{
				mBgArmature->getAnimation()->play("fail_kua", 0);
			}
		}
		break;
	case BUILD_ANI_BIG_CRY:
		if (mBgArmature->getAnimation()->getAnimationData()->getMovement("fail_kub"))
		{
			std::string name = mBgArmature->getAnimation()->getCurrentMovementID();
			if (name != "fail_kub")
			{
				mBgArmature->getAnimation()->play("fail_kub", 0);
			}
		}
		break;
	case BUILD_ANI_WIN:
		if (mBgArmature->getAnimation()->getAnimationData()->getMovement("victory"))
		{
			std::string name = mBgArmature->getAnimation()->getCurrentMovementID();
			if (name != "victory")
			{
				mBgArmature->getAnimation()->play("victory", 0);
			}
		}
		break;
		
	default:
		break;
	}
}

void BuildView::BeHurtBgCallback(Armature * pAr, MovementEventType type, const std::string& aniName)
{
	if (type == COMPLETE && pAr == mBeHurtBg)
	{
		mBeHurtBg->setVisible(false);
	}
}

//更换旗子动画
void BuildView::changeFlagByLevel(int level)
{
	BaseBuild* pBuild = sgBattleControl()->getTargetBuild(mBuildID);

	eBulidType type = pBuild->getBuildType();

	if (!mBgArmature || pBuild->getBuildType() != EBULID_TYPE_BARRACK)
	{
		return;
	}

	Bone * bone = NULL;
	bone = mBgArmature->getBone("guge_qizi");

	if (!bone)
	{
		bone = mBgArmature->getBone("qizi");
	}

	if (!bone)
	{
		bone = mBgArmature->getBone("zongzi_guge_qizi");
	}

	if (bone)
	{

		std::string flag_List[5];
		flag_List[0] = sgTableManager()->getFlagNameByCamp(pBuild->getCamp()) + "_111";
		flag_List[1] = sgTableManager()->getFlagNameByCamp(pBuild->getCamp()) + "_222";
		flag_List[2] = sgTableManager()->getFlagNameByCamp(pBuild->getCamp()) + "_333";
		flag_List[3] = sgTableManager()->getFlagNameByCamp(pBuild->getCamp()) + "_444";
		flag_List[4] = sgTableManager()->getFlagNameByCamp(pBuild->getCamp()) + "_555";

		/*int camp = pBuild->getCamp();

		if (pBuild->getCamp() == ECAMP_TYPE_Player || pBuild->getCamp() == ECAMP_TYPE_AI || pBuild->getCamp() == ECAMP_TYPE_Neutral)
		{
		flag_List[0] = "fla_QiZhiShengJi_111";
		flag_List[1] = "fla_QiZhiShengJi_222";
		flag_List[2] = "fla_QiZhiShengJi_333";
		flag_List[3] = "fla_QiZhiShengJi_444";
		flag_List[4] = "fla_QiZhiShengJi_555";
		}
		else
		{
		return;
		}*/

		//旗子只有5个，等级超过5时，旗子就不变了
		if (level > 5)
		{
			level = 5;
		}
		Armature* ar = sgResManager()->getAramatur(flag_List[level - 1].c_str());
		bone->addDisplay(ar, 0);
		ar->getAnimation()->play("play", -1, 1);

		//int index = bone->getDisplayManager()->getCurrentDisplayIndex();
		
		//bone->changeDisplayWithIndex(index, true);

		if (getCampType() == ECAMP_TYPE_Neutral)
			grayArmature(ar);
	}
}

void BuildView::changeTowerLogo(BaseBuild* pBuild)
{
	if (pBuild->getBuildType() == EBULID_TYPE_TURRET)
	{

		int level = pBuild->getLevel();
		std::string logo_List[7];

		logo_List[0] = "fla_paotaidengji_1.png";
		logo_List[1] = "fla_paotaidengji_2.png";
		logo_List[2] = "fla_paotaidengji_3.png";
		logo_List[3] = "fla_paotaidengji_4.png";
		logo_List[4] = "fla_paotaidengji_5.png";
		//by edit
		logo_List[5] = "fla_paotaidengji_5.png";
		logo_List[6] = "fla_paotaidengji_5.png";
		auto ar = Sprite::createWithSpriteFrameName(logo_List[level - 1]);

		Bone* pBone = mBgArmature->getBone("guge_paotaidengji");
		if(NULL != pBone)
		{
			//int index = pBone->getDisplayManager()->getCurrentDisplayIndex();
			pBone->addDisplay(ar, 0);
			//pBone->changeDisplayWithIndex(index, true);
		} 
	}
}

void BuildView::updatePeoplePercent(BaseBuild* pBuild)
{
	eCampType type = sgBattleControl()->getOwnCamp();
	int id = pBuild->getKeyID();
	
	if (sgBattleControl()->getIsPvp() || sgGamePrecedure()->getCurMapInfo().bEnemyInfo == false)
	{
		if (pBuild->getCamp() != sgBattleControl()->getOwnCamp() && pBuild->getCamp() != ECAMP_TYPE_Neutral)
		{
			if (!mShowEnemyInfo)
			{
				if (mBubbleNode)
					mBubbleNode->setVisible(false);
				return;
			}
		}
	}

	const BuildAttr* pAttr = sgTableManager()->getBuildAttr(pBuild->getAttrID());
	if (pAttr)
	{
		float percent = pBuild->getOwnPeople() / (float)pAttr->proLimit;
		percent = percent == 1 ? 0.999 : percent;
		if (NULL != mProgress && mBubbleNode != NULL && mPeopleNumLabel != NULL)
		{ 
			mBubbleNode->setVisible(true);
			if (pBuild->getCamp() != ECAMP_TYPE_Neutral)
			{
				mProgress->setPercentage(percent * 100);
			}
			else
			{
				mProgress->setPercentage(0);
			}
		}
	}
}

void BuildView::updatePeopleNum(BaseBuild* pBuild)
{
	if (pBuild->getIsDirty(EBDIR_PEOPLE) || pBuild->getIsDirty(EBDIR_TYPE))
	{ 
		char szSoliders[32] = { 0, };
		sprintf(szSoliders, "%d", pBuild->getOwnPeople()); 
		if (NULL == mPeopleNumLabel)
			return; 
		mPeopleNumLabel->setString(szSoliders);
		
		Text* pNode = dynamic_cast<Text* >(mPeopleNumLabel->getChildByTag(123));
		if(NULL == pNode)
			return;

		pNode->setVisible(false);
		if(pBuild->getBuildType() == EBUILD_TYPE_STABLES)
		{
			if(pBuild->getOwnPeople() > pBuild->getMaxHold())
				mPeopleNumLabel->setColor(ccc3(255,0,0)); 
			else 
				mPeopleNumLabel->setColor(ccc3(0,0,0));  
			pNode->setVisible(true);
			char szSoliderLim[32] = { 0, };
			sprintf(szSoliderLim, "/%d", pBuild->getMaxHold());
			pNode->setString(szSoliderLim); 
			float width = mPeopleNumLabel->getContentSize().width;
			pNode->setPosition(ccp(24, -7));
		} 
	}
}

cocos2d::Size BuildView::getBuildSize()
{
	if (mBgArmature)
	{
		return mBgArmature->getContentSize();
	}
	return Size(0, 0);
}

void BuildView::adjustNode(eBulidType type, const Size& size, float scale)
{

	//调整人数位置
	if (mPeopleNode && mBuildBgNode)
	{
		if (type == EBULID_TYPE_TURRET)
		{
			
		}
		static Vec2 sPos = mPeopleNode->getPosition();

		auto curPos = sPos + ((type == EBULID_TYPE_TURRET) ? Vec2(0, 50) : Vec2::ZERO);
		
		float len = curPos.length();
		float lenght = curPos.length() * scale;

		float y = curPos.y / len * lenght;
		float x = curPos.x / len * lenght;
		
		mPeopleNode->setPosition(Vec2(x,y));//Vec2(m_pos.x + m_size.width * 2 / 5, m_pos.y + m_size.height));
		if (NULL != mBubbleNode)
		{
			mBubbleNode->setPosition(mPeopleNode->getPosition() + getPosition());
		} 
	}

	//调整箭头位置
	if (mArrow)
	{
		Size m_size = size * scale / 4;
		Vec2 m_pos = mBuildBgNode->getPosition();
		mArrow->setPosition(Vec2(m_pos.x + m_size.width, m_pos.y + m_size.height));
	}
}


void BuildView::adjustByLevel(eBulidType type, int level)
{
	float scale = 1.0f;
	Size size;
	if (mBgArmature)
	{
		mBgArmature->setScale(getScaleByLevel(type,level));
		scale = mBgArmature->getScale();
		size = mBgArmature->getContentSize();
	}

	//调整人数位置
	if (mPeopleNode )
	{
		static Vec2 sPos = mPeopleNode->getPosition();

		auto curPos = sPos + ((type == EBULID_TYPE_TURRET) ? Vec2(0, 50) : Vec2::ZERO);

		float len = curPos.length();
		float lenght = curPos.length() * scale;

		float y = curPos.y / len * lenght;
		float x = curPos.x / len * lenght;

		mPeopleNode->setPosition(Vec2(x, y));//Vec2(m_pos.x + m_size.width * 2 / 5, m_pos.y + m_size.height));
		if (NULL != mBubbleNode)
		{
			mBubbleNode->setPosition(mPeopleNode->getPosition() + getPosition());
		}
	}

	//调整箭头位置
	if (mArrow && mBuildBgNode)
	{
		Size m_size = size * scale / 4;
		Vec2 m_pos = mBuildBgNode->getPosition();
		mArrow->setPosition(Vec2(m_pos.x + m_size.width, m_pos.y + m_size.height));
	}
}

eCampType BuildView::getCampType()
{
	return sgBattleControl()->getTargetBuild(mBuildID)->getCamp();
}

void BuildView::playLevelingArmature()
{
	if (mLevelingBg == 0)
	{
		mLevelingBg = cocostudio::Armature::create("fla_shengjizhong_ADDITIVE");
		mLevelingBg->setBlendFunc(BlendFunc::ADDITIVE);
		if (mBgArmature)
		{
			mLevelingBg->setLocalZOrder(mBgArmature->getLocalZOrder() + 1);
		}
		mBuildBgNode->addChild(mLevelingBg);

		mLevelingBg->setVisible(false);
	}
	
	if (mLevelingBg)
	{
		mLevelingBg->setVisible(true);
		mLevelingBg->getAnimation()->play("play",-1,1);
	}

	if (mLevelingBg2 == 0)
	{
		mLevelingBg2 = cocostudio::Armature::create("fla_shengjizhong2_ADDITIVE");
		mLevelingBg2->setBlendFunc(BlendFunc::ADDITIVE);
		if (mBgArmature)
		{
			mLevelingBg2->setLocalZOrder(mBgArmature->getLocalZOrder() - 1);
		}
		mBuildBgNode->addChild(mLevelingBg2);

		mLevelingBg2->setVisible(false);
	}

	if (mLevelingBg2)
	{
		mLevelingBg2->setVisible(true);
		mLevelingBg2->getAnimation()->play("play", -1, 1);
	}

}


void BuildView::playLeveledArmature()
{
	//播放升级动画
	if (mLevelUpBg == nullptr)
	{
		mLevelUpBg = cocostudio::Armature::create("fla_shengjizhong_done_ADDITIVE");
		mLevelUpBg->setBlendFunc(BlendFunc::ADDITIVE);
		if (mBgArmature)
		{
			mLevelUpBg->setLocalZOrder(mBgArmature->getLocalZOrder() - 1);
		}

		mBuildBgNode->addChild(mLevelUpBg);
		mLevelUpBg->setVisible(false);
	}

	if (mLevelUpBg)
	{
		mLevelUpBg->setVisible(true);
		mLevelUpBg->getAnimation()->play("play");
	}

	if (mLevelingBg)
	{
		mLevelingBg->setVisible(false);
		mLevelingBg->getAnimation()->stop();
	}
	if (mLevelingBg2)
	{
		mLevelingBg2->setVisible(false);
		mLevelingBg2->getAnimation()->stop();
	}
}

void BuildView::drawAttackRange()
{
	//int pointCount = 60;
	//float pi = 3.1415926f;

	//if (mBuildType == EBULID_TYPE_TURRET)
	//{
	//	Vec2 pos = this->getPosition();
	//	auto parent = this->getParent();

	//	unsigned radius = sgTableManager()->getBuildAttr( sgBattleControl()->getTargetBuild(mBuildID)->getAttrID() )->attRadius;
	//	
	//	Size winSize = CCDirector::getInstance()->getWinSize();
	//	float halfWidth = radius;
	//	float halfHeight = (halfWidth / winSize.width) * winSize.height;

	//	for (float i = -halfWidth; i <= halfWidth; i += 5)
	//	{
	//		float xOffset = i;
	//		float yOffset = sqrt((1 - (pow(i, 2) / pow(halfWidth, 2))) * pow(halfHeight, 2));

	//		float x = pos.x + xOffset;
	//		float y = pos.y + yOffset;
	//		float y2 = pos.y + (-1) * yOffset;

	//		auto spr = CCSprite::create("baoziPoint.png");
	//		auto spr2 = CCSprite::create("baoziPoint.png");

	//		spr->setPosition(Vec2(x, y));
	//		parent->addChild(spr);

	//		spr2->setPosition(Vec2(x, y2));
	//		parent->addChild(spr2);
	//		
	//	}
	//}

	BaseBuild* pBuild = sgBattleControl()->getTargetBuild(mBuildID);
	if (!pBuild || pBuild->getBuildType() != EBULID_TYPE_TURRET) return;

	std::string name = pBuild->getCamp() == sgBattleControl()->getOwnCamp() ? "fla_range_lv" : "fla_range_hong";

	if (mRangeBg)
		mRangeBg->removeFromParent();

	mRangeBg = Armature::create(name);
	//mRangeBg->setBlendFunc(BlendFunc::ADDITIVE);
	mRangeBg->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(BuildView::animationCallback));

	Vec2 world = mBuildBgNode->getParent()->convertToWorldSpace(mBuildBgNode->getPosition());
	Vec2 pos = sgBattleControl()->getBattleView()->convertToNodeSpace(world);// mBuildBgNode->addChild(mBottomEff);
	mRangeBg->setPosition(pos);

	sgBattleControl()->getBattleView()->addChild(mRangeBg, EBVZ_BOTTOM_EFFECT);
	this->playAni(BUILD_ANI_RANGE_BORN);

}

float BuildView::getScaleByLevel(eBulidType type, int level)
{
	if (type == EBULID_TYPE_INSISTUTE)
		return 0.8f;

	float levelScale_b[5] = { 0.6f, 0.7f, 0.8f, 0.8f, 0.8f };

	float scale = 1;
	if (type == EBULID_TYPE_BARRACK || type == EBULID_TYPE_CREEPS)
	{
		scale = levelScale_b[(level - 1)];
	}
	else if (type == EBULID_TYPE_TURRET || type == EBUILD_TYPE_STABLES)
	{
		scale = 0.6f;
	}
	return scale;
}

void BuildView::callSOS(bool enable)
{
	if (sgBattleControl()->getIsPvp() || sgGamePrecedure()->getCurMapInfo().bHelpInfo == false) return;

	Bone* tDWBone = NULL;
	if(NULL != mBgArmature && NULL != mBubbleNode)
	{
		tDWBone = mBgArmature->getBone("DW");	
	}
	
	if(enable == true)
	{
		if(m_StopWarning == false)
		{
			m_StopWarning = true;
			if(getCampType() == sgBattleControl()->getOwnCamp())
			{
				if(NULL != tDWBone)
				{
					m_WarningSp->setPosition(mBaseSosPos + ccp((tDWBone->getWorldInfo()->x + 70) * mBgArmature->getScale(), (tDWBone->getWorldInfo()->y + 15) * mBgArmature->getScale()));
				}

				m_WarningSp->runAction(Sequence::create(
					DelayTime::create(0.45f)
					, FadeIn::create(0.3f)
					, CallFunc::create(
					[this]()
				{
					if (nullptr != m_WarningSp && nullptr != mBubbleNode)
					{
						mBubbleNode->setVisible(false);
						m_WarningSp->setVisible(true);
						m_WarningSp->getAnimation()->play("play");
					}

				})
					, DelayTime::create(3.0f)
					, CallFunc::create(
					[this]()
				{
					if (nullptr != m_WarningSp && nullptr != mBubbleNode)
					{
						m_StopWarning = false;
						mBubbleNode->setVisible(true);
						m_WarningSp->setVisible(false);
						m_WarningSp->getAnimation()->stop();
					}

				})
					, FadeOut::create(0.6f)
					, nullptr));
			}
		}
		
	}
	else
	{
		m_WarningSp->setOpacity(0.0f);
		// stopAllActions();
		if (NULL != mBubbleNode)
		{
			mBubbleNode->setVisible(true);
		}
		
		m_StopWarning = false;
		
	} 
}

void BuildView::checkOccupyTag()
{
	BaseBuild* pBuild = sgBattleControl()->getTargetBuild(mBuildID);
	const BuildIns* tInfo = sgTableManager()->getBuildIns(pBuild->getBuildInfo().insID);
	if(tInfo->isModesTag != 0 || tInfo->isGuardTag != 0)
	{ 
		Bone* tDWBone = mBgArmature->getBone("DW");	
		m_TagSp->setVisible(true);
		if(NULL != tDWBone)
		{  
			m_TagSp->setPosition(ccp(tDWBone->getWorldInfo()->x * mBgArmature->getScale(), (tDWBone->getWorldInfo()->y + 20) * mBgArmature->getScale())); 
		}
		else
		{
			m_TagSp->setPosition(ccp(0,65)); 
			m_TagSp->setVisible(false);
		}	 
	}
	else
	{
		m_TagSp->setVisible(false);
	}
}


void BuildView::setBubbleNode(Node* tNode, BaseBuild* pBuild)
{

	if (tNode != NULL)
	{
		mBubbleNode = tNode;

		Point tPos = mPeopleNode->getPosition() + getPosition();
		mBubbleNode->setPosition(tPos);

		

		mProgress = ProgressTimer::create(NULL);
		if (mProgress)
			mBubbleNode->addChild(mProgress, 20);  
		
		if(pBuild->getBuildType() == EBUILD_TYPE_STABLES)
		{
			Sprite* pBg = Sprite::create("build/Battle_MajiuZengzhang_bg.png"); 
			pBg->setScale(1.3f);
			mBubbleNode->addChild(pBg, 10, 456); 
			mPeopleNumLabel = Text::create("", "fonts/font.TTF", 16);
			mPeopleNumLabel->setColor(ccc3(255,0,0));
			mPeopleNumLabel->setAnchorPoint(ccp(0.0f, 0.0f));
			mPeopleNumLabel->setPosition(ccp(-15, 0));
			mBaseBubblePos = pBg->getPosition();
		}    
		else
		{
			Sprite* pBg = Sprite::create("build/Battle_zengzhang_bg.png");
			mBubbleNode->addChild(pBg, 10, 456); 
			mPeopleNumLabel = Text::create("", "fonts/font.TTF", 16);
			mPeopleNumLabel->setColor(ccc3(0,0,0));
			mPeopleNumLabel->setPosition(Vec2(2, 2));
			mBaseBubblePos = pBg->getPosition();
		} 
		mBubbleNode->addChild(mPeopleNumLabel, 30);
		Text* pText = Text::create("", "fonts/font.TTF", 16);
		mPeopleNumLabel->addChild(pText);  
		pText->setVisible(false);
		pText->setTag(123);
		pText->setColor(ccc3(0,0,0));
	}
}

void BuildView::resetBubbleNode(BaseBuild* pBuild)
{
	Sprite* pSp = dynamic_cast<Sprite* >(mBubbleNode->getChildByTag(456));
	if(NULL == pSp)
		return;
	pSp->removeFromParentAndCleanup(true);  

	if(pBuild->getBuildType() == EBUILD_TYPE_STABLES)
	{
		pSp = Sprite::create("build/Battle_MajiuZengzhang_bg.png");
		mBubbleNode->addChild(pSp, 10,456); 
		pSp->setScale(1.3f);
		mPeopleNumLabel->setAnchorPoint(ccp(0.0f, 0.0f));
		mPeopleNumLabel->setPosition(ccp(-15, 0));
	}
	else
	{
		pSp = Sprite::create("build/Battle_zengzhang_bg.png");
		mBubbleNode->addChild(pSp, 10,456); 
		pSp->setScale(1.0f); 
		mPeopleNumLabel->setColor(ccc3(0,0,0));
		mPeopleNumLabel->setAnchorPoint(ccp(0.5f, 0.5f));
		mPeopleNumLabel->setPosition(Vec2(2, 2)); 
	}
	mBaseBubblePos = pSp->getPosition();
	updateBubble(pBuild->getCamp(), pBuild->getBuildType());
	updatePeopleNum(pBuild);
}


void BuildView::removeBubble()
{
	mBubbleNode = NULL;
	mProgress = NULL;
	mPeopleNumLabel = NULL;
}

void BuildView::updateBubble(eCampType camp, eBulidType buildType)
{ 
	//气泡
	Sprite* sprite = NULL;
	std::string pProgIcon = "ui/battle/Battle_zengzhang_bg04.png";
	switch (camp)
	{
	case ECAMP_TYPE_Neutral:
		break;
	case ECAMP_TYPE_Player:
	case ECAMP_TYPE_Player1: 
		if(buildType == EBUILD_TYPE_STABLES)
			pProgIcon = "ui/battle/Battle_majiuZengzhang_bg04.png";
		else 
			pProgIcon = "ui/battle/Battle_zengzhang_bg04.png";
		break;
	case ECAMP_TYPE_AI:
	case ECAMP_TYPE_Player2:
		if(buildType == EBUILD_TYPE_STABLES)
			pProgIcon = "ui/battle/Battle_majiuZengzhang_bg01.png";
		else 
			pProgIcon = "ui/battle/Battle_zengzhang_bg01.png";
		break;
	case ECAMP_TYPE_AI2:
	case ECAMP_TYPE_Player3:
		if(buildType == EBUILD_TYPE_STABLES)
			pProgIcon = "ui/battle/Battle_majiuZengzhang_bg03.png";
		else 
			pProgIcon = "ui/battle/Battle_zengzhang_bg03.png";
		break;
	case ECAMP_TYPE_AI3:
	case ECAMP_TYPE_Player4:
		if(buildType == EBUILD_TYPE_STABLES)
			pProgIcon = "ui/battle/Battle_majiuZengzhang_bg04.png";
		else 
			pProgIcon = "ui/battle/Battle_zengzhang_bg04.png";
		break;
	case ECAMP_TYPE_Creeps:
		break;
	default:
		break;
	}

	if (camp != ECAMP_TYPE_Creeps && camp != ECAMP_TYPE_Neutral)
		sprite = Sprite::createWithSpriteFrameName(pProgIcon);

	if (sprite)
	{
		sprite->runAction(Blink::create(1000, 1000));

		if (mProgress != NULL)
		{ 
			mProgress->removeFromParentAndCleanup(true);
			mProgress = ProgressTimer::create(NULL);
			if (mProgress)
				mBubbleNode->addChild(mProgress, 20);  
			mProgress->setSprite(sprite);

			mProgress->setLocalZOrder(12);
			mProgress->setPosition(Vec2(2, 2));

			//设置进度条的模式
			//kCCProgressTimerTypeBar表示条形模式  
			mProgress->setType(kCCProgressTimerTypeBar);

			//设置进度条变化的方向  
			//ccp(0,1)表示沿着y轴变化  
			mProgress->setBarChangeRate(ccp(0, 1));

			//设置进度条的起始位置  
			//ccp(0,0)表示下面  
			mProgress->setMidpoint(ccp(0, 0));
		} 
	}

	Sprite* pBg = dynamic_cast<Sprite* >(mBubbleNode->getChildByTag(456)); 
	if(!pBg)
		return;
	
	if(buildType == EBUILD_TYPE_STABLES)
	{ 
		mProgress->setScale(1.3f);
		pBg->setPosition(mBaseBubblePos + ccp(2, 0));
		pBg->setTexture( Sprite::create("build/Battle_MajiuZengzhang_bg.png")->getTexture()); 
	}		
	else
		pBg->setTexture(Sprite::create("build/Battle_zengzhang_bg.png")->getTexture());
}

void BuildView::grayArmature(Armature* pArmature)
{
	return;
	if (!pArmature)
		return;

	auto& map = pArmature->getBoneDic();
	for (auto& entry : map)
	{
		auto bone = entry.second;

		auto p = ShaderCache::getInstance()->getGLProgram("Shader_Armature_gray");
		if (bone)
		{
			auto list = bone->getDisplayManager()->getDecorativeDisplayList();
			for (auto dd : list)
			{
				if (dd->getDisplay())
					dd->getDisplay()->setGLProgram(p);
			}
		}
	}
}

void BuildView::addBuildProgress()
{

}

void BuildView::setExchangeProgress(int progress)
{
	if (m_PrBg && m_Pr && m_ProgressBg)
	{
		m_ProgressBg->setPercentage( progress);
	}
	if (mExchangeBg)
	{
		if (progress >= 20 && progress < 40)
			playAni(BUILD_ANI_EXCHANGE3);
		else if (progress >= 40 && progress < 60)
			playAni(BUILD_ANI_EXCHANGE2);
		else if (progress >= 60 && progress < 80)
			playAni(BUILD_ANI_EXCHANGE1);
	}
}

void BuildView::showExchangeProgress(bool isShow)
{
	if (m_PrBg && m_Pr && m_ProgressBg)
	{
		Bone* tDWBone = NULL;
		if (NULL != mBgArmature)
		{
			tDWBone = mBgArmature->getBone("DW");
		}

		if (NULL != tDWBone && NULL != mBgArmature)
		{
			Vec2 pos ;
			if (mBuildType == EBULID_TYPE_TURRET)
			{
				pos = Vec2((tDWBone->getWorldInfo()->x) * mBgArmature->getScale(), (tDWBone->getWorldInfo()->y + 30) * mBgArmature->getScale());
			}
			else
				pos = Vec2((tDWBone->getWorldInfo()->x) * mBgArmature->getScale(), (tDWBone->getWorldInfo()->y) * mBgArmature->getScale());

			m_PrBg->setPosition(pos);
			m_ProgressBg->setPosition(pos);
			m_Pr->setPosition(pos);

			m_PrBg->setVisible(isShow);
			m_Pr->setVisible(isShow);
			m_ProgressBg->setVisible(isShow);
			mExchangeBg->setVisible(isShow);
			if (!isShow)
			{
				std::string key = StringUtils::format("exchange_%d", mBuildID);
				if (sgTimeCalculator()->hasKey(key))
				{
					sgTimeCalculator()->removeTimeCalcultor(key);
				}

				key = StringUtils::format("change_%d", mBuildID);
				if (sgTimeCalculator()->hasKey(key))
				{
					sgTimeCalculator()->removeTimeCalcultor(key);
				}

			}
		}
	}
}


void BuildView::setHide(bool val)
{
	if (val)
	{
		this->setVisible(true);
		if (mBubbleNode)
		{
			mBubbleNode->setVisible(true);
		}
	}
	else
	{
		this->setVisible(false);
		if (mBubbleNode)
		{
			mBubbleNode->setVisible(false);
		}
	}
}

#define PROP_AIM_TAG 1000

void BuildView::addPropAimEffect()
{
	BaseBuild* pBuild = sgBattleControl()->getTargetBuild(mBuildID);
	if (pBuild)
	{
		Armature* amiEffect = Armature::create("fla_DaoJuzhizhen");
		amiEffect->getAnimation()->play("play");

		amiEffect->setPosition(pBuild->getPosition().x,pBuild->getPosition().y+pBuild->getBuildView()->getBuildSize().height*0.5);
		this->getParent()->addChild(amiEffect,this->getZOrder());
		amiEffect->setTag(PROP_AIM_TAG + mBuildID);
	}

}
 
void BuildView::RemovePropAimEffect()
{
	this->getParent()->removeChildByTag(PROP_AIM_TAG+mBuildID);
}

void BuildView::changeBottomEffect(BaseBuild* pBuild)
{
	if (pBuild->getCamp() == sgBattleControl()->getOwnCamp())
	{
		if (mBottomEff)
		{
			mBottomEff->getAnimation()->stop();
			mBottomEff->removeFromParent();
			mBottomEff = nullptr;
		}

		mBottomEff = sgResManager()->getAramatur("fla_battle_jifangguanghuan");//Armature::create("fla_battle_jifangguanghuan");
		Vec2 world = mBuildBgNode->getParent()->convertToWorldSpace(mBuildBgNode->getPosition());
		Vec2 pos = sgBattleControl()->getBattleView()->convertToNodeSpace(world);// mBuildBgNode->addChild(mBottomEff);
		mBottomEff->setPosition(pos);
		sgBattleControl()->getBattleView()->addChild(mBottomEff, EBVZ_BOTTOM_EFFECT);

		mBottomEff->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(BuildView::animationCallback));
		mBottomEff->getAnimation()->play("start");
	}
	else
	{
		if (mBottomEff)
		{
			mBottomEff->getAnimation()->play("end");
		}
	}
}

void BuildView::updateUpgradePrg(eCampType camp, float percent)
{
	if (sgBattleControl()->getIsPvp() && camp != sgBattleControl()->getOwnCamp())
	{
		return;
	}

	if (percent > 0)
	{
		if (m_UpProgress != nullptr)
		{
			m_UpProgress->setPercentage(100.0f - percent);
		}
		else
		{

			Bone* tDWBone = mBgArmature->getBone("DW");
			float scale = mBgArmature->getScale();
			Vec2 world = mBgArmature->convertToWorldSpace(Vec2(tDWBone->getWorldInfo()->x * scale, tDWBone->getWorldInfo()->y * scale));
			Vec2 pos = sgBattleControl()->getBattleView()->convertToNodeSpace(world);

			m_UpgaradeBg = Sprite::create("build_update1.png");
			m_UpgaradeBg->setBlendFunc(BlendFunc::ADDITIVE);
			m_UpgaradeBg->setAnchorPoint(Vec2(0.5, 0));
			m_UpgaradeBg->setPosition(pos);
			sgBattleControl()->getBattleView()->addChild(m_UpgaradeBg, EBVZ_EFFECT_LOW);

			m_UpProgress = ProgressTimer::create(NULL);
			m_UpProgress->setType(kCCProgressTimerTypeRadial);
			m_UpProgress->setPercentage(0);
			m_UpProgress->setSprite(Sprite::create("build_update2.png"));
			m_UpProgress->setPosition(pos);
			m_UpProgress->setAnchorPoint(Vec2(0.5, 0));
			sgBattleControl()->getBattleView()->addChild(m_UpProgress, EBVZ_EFFECT_LOW);
			
		}
	}
	else
	{
		if (m_UpProgress != nullptr)
		{
			m_UpgaradeBg->removeFromParent();
			m_UpProgress->removeFromParent();

			Bone* tDWBone = mBgArmature->getBone("DW");
			float scale = mBgArmature->getScale();
			Vec2 world = mBgArmature->convertToWorldSpace(Vec2(tDWBone->getWorldInfo()->x * scale, tDWBone->getWorldInfo()->y * scale));
			Vec2 pos = mBgArmature->convertToNodeSpace(world);

			Armature* pUpgradeOk = Armature::create("fla_shengjizhong_jindudone");
			pUpgradeOk->setAnchorPoint(Vec2(0.5, 0));
			pUpgradeOk->setPosition(pos);
			pUpgradeOk->getAnimation()->setMovementEventCallFunc([this](Armature *armature, MovementEventType movementType, const std::string& movementID)
			{
				if (MovementEventType::COMPLETE)
				{
					armature->removeFromParent();
				}
			});
			
			pUpgradeOk->getAnimation()->play("play");
			mBuildBgNode->addChild(pUpgradeOk);

			m_UpProgress = nullptr;
			m_UpgaradeBg = nullptr;
		}
	}
}


void BuildView::playCreepsBornAni()
{
	if (mBgArmature->getAnimation()->getAnimationData()->getMovement("born"))
	{
		mBgArmature->getAnimation()->play("born");
	}
}


void BuildView::playCreepsDeadAni()
{
	if (mBgArmature->getAnimation()->getAnimationData()->getMovement("dead"))
	{
		mBgArmature->getAnimation()->play("dead");
	}
}


void BuildView::showCreepsTomb(bool val)
{
	int zOrder = 0;
	if (mBgArmature)
	{
		mBgArmature->setVisible(!val);
		zOrder = mBgArmature->getLocalZOrder();
	}

	if (mBuildBgNode)
		mBubbleNode->setVisible(!val);


	if (val)
	{
		if (nullptr == mTomb)
		{
			mTomb = Sprite::create("mubei.png");
			if (mTomb)
			{
				mTomb->setAnchorPoint(Point(0.5f, 0.2f));
				this->addChild(mTomb, zOrder);
			}
		}
	}
	else
	{
		if (nullptr != mTomb)
		{
			this->removeChild(mTomb);
			mTomb = nullptr;
		}
	}
}

void BuildView::changeTurretBone(BaseBuild* pBuild)
{
	if (mBgArmature && pBuild->getBuildType() == EBULID_TYPE_TURRET)
	{
		int level = pBuild->getLevel();
		int flag = 1;
		std::string newName; 

		if (!sgBattleControl()->getIsPvp())
		{
			flag = pBuild->getCamp();
			flag = MIN(4, flag);
			flag = MAX(1, flag);
		}
		if (level == 6)
		{
			newName = StringUtils::format("build/camp_fire_%d.png", flag);
			std::string strFire = StringUtils::format("build/paotai_fire_%d.png", flag);
			showOrHideBone(mBgArmature, "paotai", true, strFire);
		}
		else if (level == 7)
		{
			newName = StringUtils::format("build/camp_ice_%d.png", flag);
			std::string strIce = StringUtils::format("build/paotai_ice_%d.png", flag);
			showOrHideBone(mBgArmature, "paotai", true, strIce);
		}
		else
		{
			newName = StringUtils::format("build/camp_%d.png", pBuild->getCamp());
		}
		//std::string newName = StringUtils::format("build/camp_%d.png", pBuild->getCamp());

		showOrHideBone(mBgArmature, "guge_paotailv3", level == 3, newName);
		showOrHideBone(mBgArmature, "guge_paotailv4_1", level == 4, newName);
		showOrHideBone(mBgArmature, "guge_paotailv4_2", level == 4, newName);
		showOrHideBone(mBgArmature, "guge_paotailv5_5", level >= 5, newName);
		showOrHideBone(mBgArmature, "guge_paotailv5_6", level >= 5, newName);
		showOrHideBone(mBgArmature, "guge_paotailv5_3", level >= 5, newName);
		showOrHideBone(mBgArmature, "guge_paotailv5_4", level >= 5, newName);

		if (level == 5)
		{
			if (!sgBattleControl()->getIsPvp() || pBuild->getCamp() == ECAMP_TYPE_Neutral)
			{
				newName = StringUtils::format("build/camp_bottom_%d.png", pBuild->getCamp());
			}
			else
				newName = "camp_bottom_1.png";

			//showOrHideBone(mBgArmature, "guge_paotailv5_1", level == 5, newName);
			//showOrHideBone(mBgArmature, "guge_paotailv5_2", level == 5, newName);

			//Bone * pBone = mBgArmature->getBone("guge_paotailv5_2");
			//if (pBone)
			//{
			//	Sprite* pNew = Sprite::create(newName);
			//	pNew->setScaleX(-1);
			//	pBone->addDisplay(pNew, 0);
			//}

		}

	}
}

void BuildView::showOrHideBone(Armature* pArm,std::string name,bool v /* = true */,std::string newName /* = "" */)
{
	Bone * pBone = pArm->getBone(name);
	if (pBone)
	{
		if (v)
		{
			//int index = pBone->getDisplayManager()->getCurrentDisplayIndex();
			Sprite* pNew = Sprite::create(newName );
			pBone->addDisplay(pNew, 0);
		}

		Node* pNode = pBone->getDisplayRenderNode();
		if (pNode)
		{
			pNode->setVisible(v);
		}

	}
}

void BuildView::stopUpgradeAnimation()
{
	if (m_UpProgress != nullptr)
	{
		m_UpgaradeBg->removeFromParent();
		m_UpProgress->removeFromParent();
		m_UpgaradeBg = nullptr;
		m_UpProgress = nullptr;
	}

	if (mLevelingBg)
	{
		mLevelingBg->setVisible(false);
		mLevelingBg->getAnimation()->stop();
	}
	if (mLevelingBg2)
	{
		mLevelingBg2->setVisible(false);
		mLevelingBg2->getAnimation()->stop();
	}
}

void BuildView::playInvisibilityEffect()
{
	if (mInvisArm != nullptr)
	{
		mInvisArm->removeFromParentAndCleanup(true);
		mInvisArm = nullptr;
	}

	if (mInvisArmPvp != nullptr)
	{
		mInvisArmPvp->removeFromParentAndCleanup(true);
		mInvisArmPvp = nullptr;
	}

	if (mBgArmature != nullptr)
	{
		mInvisArm = Armature::create("fla_yinshen_ADDITIVE");
		mInvisArm->setBlendFunc(BlendFunc::ADDITIVE);
		mInvisArm->getAnimation()->play("born");
		mInvisArm->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(BuildView::animationCallback));
		mBuildBgNode->addChild(mInvisArm);

		if (sgBattleControl()->getIsPvp())
		{
			mInvisArmPvp = Armature::create("fla_yinshenb");
			mInvisArmPvp->getAnimation()->play("born");
			mInvisArmPvp->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(BuildView::animationCallback));
			mBuildBgNode->addChild(mInvisArmPvp);

		}
		else
		{
			mBgArmature->setOpacity(255 * 0.5f);
		}



	}

	setArrowVisible(false);
}

void BuildView::removeInvisibilityEffect()
{
	if (mBgArmature != nullptr)
	{
		mBgArmature->setOpacity(255);
		if (mInvisArm)
		{
			mInvisArm->getAnimation()->play("end");
			mInvisArm = nullptr;
		}
		if (mInvisArmPvp)
		{
			mInvisArmPvp->getAnimation()->play("end");
			mInvisArmPvp = nullptr;
		}
	}
}

void BuildView::updateSosPostion()
{
	if (mBgArmature && !sgBattleControl()->getIsPvp())
	{
		Bone* tDWBone = mBgArmature->getBone("DW");
		if (tDWBone)
		{
			float scale = mBgArmature->getScale();
			Vec2 world = mBgArmature->convertToWorldSpace(Vec2((tDWBone->getWorldInfo()->x - 80) * scale, (tDWBone->getWorldInfo()->y - 20) * scale));
			Vec2 pos = sgBattleControl()->getBattleView()->convertToNodeSpace(world);

			mBaseSosPos = pos;
			m_WarningSp->setPosition(pos);
			m_WarningSp->setZOrder(mBubbleNode->getZOrder() + 100);
		}
	}

}


void BuildView::onChooseCallBack(Ref * pRef)
{
	Button *btn = dynamic_cast<Button*>(pRef);
	CCLOG("[trace] BuildView::onChooseCallBack ,button name :%s", btn->getName().c_str());

	Turret* pBuild = dynamic_cast<Turret*>(sgBattleControl()->getTargetBuild(mBuildID));

	int buildStep = 1;

	if (btn->getName() == "mAoeButton")
	{	
		pBuild->setStep(1);
		buildStep = 1;
	}
	if (btn->getName() == "mIceButton")
	{
		pBuild->setStep(2);
		buildStep = 2;
	}

	//战斗回放_记录炮塔转换类型
	char params[128];
	memset(params, 0, 128);
	sprintf(params, "%d %d", mBuildID, buildStep);
	sgMessageManager()->sendMessage(EMT_RECORD_TURRET_UPDATE, params);

	showChooseLevel(false);
	pBuild->setIsleveling(true);
	pBuild->Upgrade(true);
}

void BuildView::showChooseLevel(bool isShow)
{
	BaseBuild* pBuild = sgBattleControl()->getTargetBuild(mBuildID);
	if (pBuild && pBuild->checkCanUpgrade() && !pBuild->getIsleveling() && pBuild->getCamp() == sgBattleControl()->getOwnCamp())
	{
		if (mChooseLevelNode)
			mChooseLevelNode->setVisible(isShow);
		if (mBubbleNode)
			mBubbleNode->setVisible(!isShow);
	}
	else
	{
		if (mChooseLevelNode)
			mChooseLevelNode->setVisible(false);
	}
}

bool BuildView::getChooseLevelShow()
{
	if (mChooseLevelNode)
		return	mChooseLevelNode->isVisible();
	
	return false;
}

void BuildView::addAddPeopel()
{
	auto battleView = sgBattleControl()->getBattleView();
	Vec2 world = mBuildBgNode->getParent()->convertToWorldSpace(mBuildBgNode->getPosition());
	Vec2 pos = battleView->convertToNodeSpace(world);

	if (!sgBattleControl()->getIsPvp())
	{
		Armature* pAddPeopolBg = Armature::create("fla_jiabingli_ADDITIVE");
		pAddPeopolBg->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(BuildView::animationCallback));
		pAddPeopolBg->setBlendFunc(BlendFunc::ADDITIVE);
		pAddPeopolBg->getAnimation()->play("play");
		pAddPeopolBg->setPosition(pos);

		battleView->addChild(pAddPeopolBg, EBVZ_EFFECT_HIGHT);
	}
	else
	{
		Armature* pAddPeopolBg = Armature::create("fla_BingLiBao");
		pAddPeopolBg->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(BuildView::animationCallback));
		pAddPeopolBg->getAnimation()->play("play");
		pAddPeopolBg->setPosition(pos);

		battleView->addChild(pAddPeopolBg, EBVZ_EFFECT_HIGHT);

		Armature* pPeopolBg = Armature::create("fla_BingLiBao2");
		pPeopolBg->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(BuildView::animationCallback));
		pPeopolBg->getAnimation()->play("play");
		battleView->addChild(pPeopolBg, EBVZ_EFFECT_HIGHT);

	}
}

void BuildView::stopBornAnimation()
{
	if (mBornAnim)
	{
		mBornAnim->getAnimation()->stop();
		mBornAnim->removeFromParent();
		mBornAnim = nullptr;
	}
}

void BuildView::setArrowVisible(bool v)
{
	if (mArrow)
	{
		mArrow->setVisible(v);
	}
}

void BuildView::changeArrow(const std::string& aniName)
{
	if (mArrow != nullptr)
	{
		Node* flaArrow = mArrow->getChildByTag(Fla_Arrow_Tag);
		if (flaArrow && flaArrow->getName() == aniName)
		{
			return;//如果是一样就不再创建了！！！影响性能。
		}
		Armature *pArrowArmature = sgResManager()->getAramatur(aniName);
		pArrowArmature->setName(aniName);
		if (pArrowArmature != nullptr)
		{
			pArrowArmature->getAnimation()->play("play");
			mArrow->removeAllChildren();
			mArrow->addChild(pArrowArmature, 1, Fla_Arrow_Tag);
		}

	}
}

void BuildView::setShowEnemyInfo(bool v)
{
	mShowEnemyInfo = v;
}


void BuildView::addExternalEffect(Armature* pArmature)
{
	if (pArmature)
	{
		this->addChild(pArmature, 20);
	}
}
