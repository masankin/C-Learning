#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "GamePrecedure.h"
#include "Message.h"
class StarLvNode : public Node
{	
public:
	StarLvNode();
	~StarLvNode();
	static StarLvNode* create();

	virtual bool init();

	void	resetStarStr();
protected:
	Node* mRoot;
};

class NumberEffectNode : public Node
{
public:
	static NumberEffectNode* create(int target, float time, ui::Text* widget);

	bool init(int target, float time, ui::Text* widget);

	void update(float dt);

	void end();

	int			mCurNumber;
	int			mTarget;
	int			mSpeed;
	ui::Text*	mText;
	bool		mIsDone;
};

class ResultDialog :public ActionWindow, public MessageHandler
{
public:
	ResultDialog();
	~ResultDialog();

	void onReceiveMassage(GameMessage* message);

	virtual bool init();

	void onEnter(); 

	CREATE_FUNC(ResultDialog);

	virtual void onShow();
	virtual void onClose();

	void initStar();

	void initScore();
	void scoreEffectDone();
	void enableClickEvent(bool enable);
	bool share(int rank);
	bool winShare(int id);
	void update(float dt);

	void autoClose(float dt);

	ui::Button * mBtnBack;
	ui::Button * mBtnNextLevel;
	cocos2d::Sprite * mWorld;

	NumberEffectNode*	mTimePoints;
	NumberEffectNode*	mPropPoints;
	NumberEffectNode*	mTotalPoints;
	
protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	virtual void windowAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);

	void doGuideExtraAction(MessageGuideTips* pMsg);
	void gotoMainUI(); 

	void showEffectUI();

	cocostudio::Armature*  mArmatureGuang;
	Node* mCenter_1;

	StarLvNode* mStarLvNode;

	ui::Text* mEXP;

	int mShareId;
	int mSelfMissionRank;
	bool mCanGetBaseReward;
	bool mCanGetPerfectReward;
	int mRankLimit;

	bool mHasPlayEffect;
#ifdef MGAME_PLATFORM_MPLUS
	void autoShowAd(float dt);
#endif
};
