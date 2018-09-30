/*
* jacey
* 建筑视图
*/
#pragma once

#include "BattleInfo.h"


#include "ui/UIImageView.h"
#include "ui/UIText.h"
#include "ui/UILayout.h"

#include "cocostudio/CCArmature.h"

#include "cocos2d.h"
USING_NS_CC;
using namespace cocostudio;

class BuildView : public Node
{
public:

	BuildView();
	~BuildView();

	virtual bool init(eCampType etype, int buildID, eBulidType buildType); 
	
	virtual void onEnter();
	virtual void onExit();

	static BuildView*	createView(eCampType etype, int buildID, eBulidType buildType);
	//virtual const Vec2& getPosition() const;

	void updateUI();

	void doBuildUpgrade();
	void doBuildChange(eBulidType enType);

	void playAni(eBuildAni eAni);

	cocos2d::Size getBuildSize();

	bool changeExpress(eExpressType type);
	void adjustByLevel(eBulidType type, int level);

	//调整 建筑 兵力，升级 节点位置
	void adjustNode(eBulidType type, const Size& size, float scale);

	eCampType getCampType();

	void playLevelingArmature();
	//升级动画完成
	void playLeveledArmature();

	//绘制攻击范围，对炮塔有效
	void drawAttackRange();

	float getScaleByLevel(eBulidType type , int level);

	void callSOS(bool enable);
	void updateSosPostion();

	void checkOccupyTag();

	void setBubbleNode(Node* tNode, BaseBuild* pBuild);
	void removeBubble();
	void resetBubbleNode(BaseBuild* pBuild);
	void updateBubble(eCampType camp, eBulidType buildType);

	void addBuildProgress();

	void setExchangeProgress(int progress);

	void showExchangeProgress(bool isShow);

	void playCreepsBornAni();
	void playCreepsDeadAni();
	void showCreepsTomb(bool val);

	void setHide(bool val);


	void addPropAimEffect();
	void RemovePropAimEffect();

	void changeBottomEffect(BaseBuild* pBuild);

	void updateUpgradePrg(eCampType camp, float percent);
	void playInvisibilityEffect();
	void removeInvisibilityEffect();
	void showChooseLevel(bool isShow);
	bool getChooseLevelShow();
	void addAddPeopel();
	void stopBornAnimation();
	void setArrowVisible(bool v);
	void changeArrow(const std::string& aniName);
	void setShowEnemyInfo(bool v);

	void addExternalEffect(Armature* pAramature);

protected:

	void	grayArmature(Armature*);

	void onClick(Ref*);
	void onCaleTime(float dt);
	void onChooseCallBack(Ref *);
	void BeHurtBgCallback(Armature *, MovementEventType, const std::string&);
	void animationCallback(Armature *, MovementEventType, const std::string&);
	void changeFlagByLevel(int level);
	void changeTowerLogo(BaseBuild* pBuild);
	void changeTurretBone(BaseBuild* pBuild);
	void showOrHideBone(Armature* pArm,std::string name,bool v = true,std::string newName = "");
	void updatePeoplePercent(BaseBuild* pBuild);
	void updatePeopleNum(BaseBuild* pBuild);

	void stopUpgradeAnimation();

	CC_SYNTHESIZE(int, mBuildID, BuildID);
	CC_SYNTHESIZE(bool, canTouch, Touch);


	ui::Text*	mPeopleNumLabel;
	ProgressTimer* mProgress;
	Node*	mBuildBgNode;
	Node*	mPeopleNode;
	Node*	mArrow;
	Node*   mChooseLevelNode;

	Armature* mBgArmature;
	Armature* mChangeBg;
	Armature* mChangeB2;
	Armature* mLevelingBg;
	Armature* mLevelingBg2;
	Armature* mLevelUpBg;
	Armature* mBeHurtBg;
	Armature* mRangeBg;

	Armature* mExchangeBg;
	Armature* mBottomEff;
	Armature* mInvisArm;
	Armature* mInvisArmPvp;
	Armature* mBornAnim;
	

	ui::Widget* mTouch;
	eBulidType	mBuildType;
	Armature* m_WarningSp;
	Sprite* m_TagSp;
	Sprite *mSpriteType;
	Node* mBubbleNode;

	//建筑转换进度
	Sprite* m_PrBg;
	Sprite* m_Pr;
	ProgressTimer* m_ProgressBg;

	//建筑升级进度
	Sprite* m_UpgaradeBg;
	ProgressTimer* m_UpProgress;

	Sprite* mTomb;


	bool m_IsWarning;
	bool m_StopWarning;

	bool isClick;
	bool isBorn;
	bool isJungleBorn;
	bool mShowEnemyInfo;

	Point mBaseBubblePos;
	Point mBaseSosPos;
};

 
