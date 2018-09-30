#pragma once
//延保抖目   create by sovio

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "Constant.h"
#include "Message.h"
#define MORALEVIEW_TAG 100
#define GLYPHINFONODE_TAG	234

enum MORALE_POSITION
{
	M_P_1 = 1,
	M_P_2,
	M_P_3,
	M_P_4,
};

enum eMoralePlayState
{
	E_MORALEANI_IDLE = 0,
	E_MORALEANI_BORN,
	E_MORALEANI_ING,
	E_MORALEANI_END
};
class MoraleView :public cocos2d::Node, public MessageHandler
{
public:
	MoraleView(eCampType, MORALE_POSITION);
	~MoraleView(void);
	virtual bool init();
	static MoraleView* create(eCampType, MORALE_POSITION);

	void initWithCampType(Node* moraleNode, Node* glyInfoNode);
	void changeCampType(eCampType);
	
private:
	void initSelf();
	void changeIco();
	void changeName();
	void changeIconBg(MORALE_POSITION, bool bIsSelf = false);
	virtual void onReceiveMassage(GameMessage * message);
	void changeChatMsg();
	void playMoraleParticle(Point srcPos, Point destPos);

	void doAnimation(int state);
	void doPlayBorn();
	void doPlayStand();
	void doPlayEnd();
	void animationCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName); 
	void onHeadTouchEvent(cocos2d::Ref*, cocos2d::ui::TouchEventType touchEvent);
	void updateGlyphInfo();
	eCampType mCampType;
	MORALE_POSITION mPositionType;

	eCampType mCampType1;
	eCampType mCampType2;
	
	cocos2d::Node* mRoot;
	cocostudio::Armature* mMoraleArm;
	cocos2d::ProgressTimer* mProgress;

	ui::Text* mMsgLabel;
	int mAniState;

	ui::ImageView* mIcon;

	cocos2d::Node* mGlyphInfoNode;
	ui::Text* mAttackTxt;
	ui::Text* mDefTxt;
	ui::Text* mSpeedTxt;
	ui::Text* mPPTxt;
	ui::Text* mArTxt;
	ui::Text* mTotalTxt;
	Sprite* mSprSession;
	Vec2 mGlyphInfoVec;
};

