#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "GamePrecedure.h"
#include "Message.h"

class CheckPoint;
class PropItem;
class PropViewList;
class PropInfo;
class RewardDisplay;
class LevelDialog :public ActionWindow, public MessageHandler
{
public:
	struct sRankInfo
	{
		int rank;
		int uin;
		std::string name;
		std::string icon;
		int points;
	};

	LevelDialog();
	~LevelDialog();

	virtual bool init();
	CREATE_FUNC(LevelDialog);
	virtual void onShow();
	virtual void onClose();


	virtual void onReceiveMassage(GameMessage * message);

	void onEnter();
	void onExit();

	void addMissionTarget();

	void initPropPanel();

	void showAddPropPanel(int index);

	void onClick(cocos2d::Ref*, cocos2d::ui::TouchEventType touchEvent);


	void addPropView(const PropInfo* pInfo);
	void addPropView(Node* parent,const PropInfo* pInfo);

	virtual bool onTouchBegan(Touch *touch, Event *unused_event);

	PropViewList* getPropViewList(){ return mPropViewList; }
protected:
	void showPropPanel(bool show);

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void windowAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
//	void selectLevelGrade(eLevelGrade grade);

	void	createScaleMap(const CheckPoint* );

	bool	removeOtherProp(int id);

	bool	checkNextPropPos();

	bool	checkShowAutoSelect();

	const PropInfo*  getNextPropInfo();//»ñµÃ

	void	showAutoSelect();
	void	onGame(Ref* sendr);
	void	showReward(int type);
	void	hideReward();
	void	showRankList(int missionID);
	void	createRankNode(sRankInfo* info);

	void onCallBackBuyPower(Ref*);
	void onCallBackBuyDiamond(Ref*);
	void playAniBornOrEnd(bool bBorn);

	void updateBegin(float dt);
	void createGuideNode();

	void listViewOnTouch(Ref* ref, TouchEventType touchType);

	CC_SYNTHESIZE(int, mLevel, Level);
	

	ui::Text* mPower;
	ui::Text* mPassText;
	ui::Text* mTitleText;
	ui::Button* mAutoSelect;
	Node* mNodeDetail;
	Node* mNodeDaoJu;
	ui::Button* mBtnBegin;
	ui::Button* mBtnBegin0;
	ui::Button* mBtnSkip;
	ui::Button* mCloseBtn;
	ui::Button* mBattleRecord;
	ui::Button* mBaseReward;
	ui::Button* mPeffectReward;
	cocostudio::Armature* mArmAutoSelect;
	cocostudio::Armature* mArmDaoJuGroup;
	cocostudio::Armature* mArmLanTiao;
	ePropPos mPropPos;
	std::map<ePropPos, int> mPropList;

	PropViewList* mPropViewList;
	RewardDisplay*  mRewardDisplay;

	ui::ListView* mRankList;

	cocos2d::EventListenerCustom*  m_pChangePowerListener;

	Point mTouchBeginPos;
private:
	bool mIsPass;
	int mCurLevelId;
};

class PropNoSetDilog:public Window
{
public:
	PropNoSetDilog(){};
	~PropNoSetDilog(){};

	virtual bool init();
	CREATE_FUNC(PropNoSetDilog)
protected:

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

private:

};
