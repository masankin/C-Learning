#pragma once
#include "AchieveLayout.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "data/TableManager.h"

enum RewardItemStatus
{
	REWARD_BTN_UNREACHABLE = 0,	// current can't get
	REWARD_BTN_GETTED,		// current getted
	REWARD_BTN_CANGET,		// current can get
	REWARD_BTN_MAX,
};
class AchieveRewardItem :public cocos2d::Node
{
public:
	AchieveRewardItem(AchieveType);
	~AchieveRewardItem(void);
	static AchieveRewardItem* create(AchieveType);
	virtual bool init();
	void updateInfo(const AchievementAward* pAward,int progressNow, int progressAll, int gotoLevel);
	void updateInfo(const AchievementAward* pAward);
	void onTouch();
private:
	void onTouchEvent(cocos2d::Ref* ,cocos2d::ui::TouchEventType);


	AchieveType m_AchieveType;
	bool m_isTouch;
	cocos2d::ui::Text* m_describeLab;
	cocos2d::ui::Text* m_tasknameLab;
	cocos2d::ui::Button* m_getBtn;
	cocos2d::ui::LoadingBar* m_progressLoadBar;
	cocos2d::ui::Text* m_progressText;
	cocos2d::ui::Text* m_statusText;
	cocos2d::ui::Text* m_complentplayer;
	cocos2d::ui::Text* mPointText;
	cocos2d::ui::Button* mPointBtn;
	Sprite* mProgressIamge;

	int mId;

	RewardItemStatus mRewardStatus;

	int mGoToLevel;   //去完成关卡
};

