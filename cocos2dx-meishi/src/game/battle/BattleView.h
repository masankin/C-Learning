/*
* jacey
* 战斗视图类
*/

#pragma once

#include "BattleInfo.h"
#include "game/ui/HpSlider.h"
#include "buff/BuffView.h"
#include "ui/system/ActionTip.h"
enum eBVZorder
{
	EBVZ_INVALID = -1,
	EBVZ_BG = 0,
	EBVZ_TILEMAP = 1,
	EBVZ_BOTTOM_EFFECT = 2,			//buff特效
	EBVZ_FORMATION = 3,	// Note!!!!!: Only for formation row, Do not add ohter child with this ZOrder
	EBVZ_FORMATION_EFFECT = 4,
	EBVZ_WIDGET = 5,		// 场景物件
	EBVC_OBSTACLE = 6,
	
	EBVZ_BUTTON,		// 场景按钮
	EBVZ_UI,			// 
	EBVZ_POP,
	EBVZ_TOP,
	EBVZ_ARROW,			//箭头

	EBVZ_BUILD = 100,
	EBVZ_EFFECT_LOW = 110,
	EBVZ_EFFECT_HIGHT = 120,

	EBVZ_TIPS = 2000,
}; 



enum eBVTag

{
	EBVTAG_INVALID = -1,
	EBVTAG_BG = 0,
};

 
class BuildView;
class ArrowWidget; 
class BattleView : public Layer
{
public:

	BattleView();
	~BattleView();

	CREATE_FUNC(BattleView);
	
	// override begin
	virtual bool load();
	

	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchCancelled(Touch *touch, Event *unused_event); 

	// override end
	void onEnter();
	void onExit();

	void destroy();

	void updateUI(float dt);

	void clean();

	void doCampBuildUpgrade( int buildID);

	bool m_CanStartGuide;

	void setSlider(HpSlider* pSlider);

	HpSlider* getSlider(){ return mSlider; }

	void refreshSliderPercent();
	void initSlider();

	void setTimeLeft(float leftTime);
	void setSpirit(float leftTime);
	void setTimeFont(ui::Text* uiFont) { mTimeUI = uiFont; }
	void setSpiritFont(ui::Text* uiFont) { mSpiritUI = uiFont; }
	void setTimeSprite(ui::LoadingBar* timeSprite) { mTimeSprite = timeSprite; }
	void setTimeTotal(float t) { mTimeTotal = t; }

	void setEnbelTouch(bool touch);

	void playPropAni(ePropType eType, const Point& pos = Point::ZERO);
	void playFreezeAni(const Point& pos,float time);
	void playBombShootAni(const Point& begPos, const Point& endPos, const std::function<void()>& callback);
	void playBombExplosionAni(const Point& pos);
	void playBombAttAni(const Point& pos, const std::string aniName);
	void playReturnSoliderParticle(const Point& startPos, const Point& endPos, const std::function<void()>& callback);

	/*void animationCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);*/

	cocostudio::Armature *getFeidieArm(){ return mFeiDieback; }

	virtual void refreshJungle(int buildID, int locationID, int buffID);
	virtual void returnFromJungleBuild(int srcID, int destID);
	virtual void closeJungleBuild(int srcID, int srcCamp, int destCamp, int buildID, int locationID);

	void addBuildBuffEffect(int buildUid, const std::string& effectFile, GLubyte opacity = 255);
	void removeBuildBuffEffect(int buildUid);

	void addOilEffect(int id ,const Vec2& pos );
	void removeOilEffect(int id);
	
	void showMorale(eCampType campType, float tValue);
	 
	void clearBuildView(int buildID); 
	void clearBuildRefView(int buildID);

	void setChatNode(Node* node) { mChatNode = node; }
	bool activeGuide(int guideLocation = E_GUIDELOC_GATE, int param1 = 0, int param2 = 0);
	void doGuideProp(Point pos);

	void initBuffTips(int insID);
	void addBuffTips(eBuffType buffType, int insID);
	std::string getTips(eBuffType buffType);
	void updateBuffTips();
	void clearBuffTips();
	void playCookBottom(Vec2 pos);

	void setPaintingPos(Point posBegan, Point posEnd);
	void resetPainting();

	void addBattleTips(const std::string& tips, int fontSize, const Color4B* pTextColor, const Color4B* pOutlineColor, const std::string& iconfile);
	void showBattleTips(float dt);

	void doSoldierMove(int srcBuildID, int dstBuildID);

	void addExternalEffect(cocostudio::Armature* pArmature);

protected: 
	int mBuffTipsRefNum;
	ActionTip* mBattleTip;

	bool		mIntialized;

	typedef std::map<int, BuildView*>	BuildViewList;
	BuildViewList	mBuildViewList;

	ArrowWidget* mPainting;
	HpSlider* mSlider;
	ui::Text* mTimeUI;
	ui::Text* mSpiritUI;
	bool isTouch;

	BaseBuild* mBeginBuild;
	BaseBuild* mEndBuild;

	ui::LoadingBar* mTimeSprite;
	float mTimeTotal;

	//道具
	cocostudio::Armature* mFreezeBg;
	cocostudio::Armature* mTimeback_Eff;
	cocostudio::Armature* mTimeback;
	cocostudio::Armature* mFeiDieback;

	//指向头
	cocostudio::Armature* mFaceBuildArmA;
	cocostudio::Armature* mFaceBuildArmB;

private:
	Node* createSoldierBubble(int key); 
	std::map<int, Node* > m_SoldierBubbleMap;
	std::map<int, Node* > mBuffArmatureMap;
	std::map<unsigned, cocostudio::Armature*> mOilEffectList;

	Node* mPlayerNode;
	Node* mEnemyNode;
 
	Node* mChatNode;
	bool mBorn;

	std::list<Node*> mBattleTipsList;
	int mOILSoundId;
	int mFREEZESoundId;
public:
 
	
}; 

 
