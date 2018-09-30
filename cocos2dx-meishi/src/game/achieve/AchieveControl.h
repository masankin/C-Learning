#pragma once
#include "cocos2d.h"
#include "Constant.h"
#include "AchieveProxy.h"
#include "AchievePanel.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"

USING_NS_CC_EXT;

class AchieveControl
{
public:
	AchieveControl(void);
	~AchieveControl(void);

	AchieveProxy* getAchieveProxy();
	CC_SYNTHESIZE(AchievePanel*, mAchievePanel, AchievePanel);

	void sendQueryAchievementStatus();
	void sendGetAchievementAward(int id);
	void sendReceiveAchievementRewardReq(int id);

	void doQueryAchievementStatus(const fgame::CSGetAchievementResp& resp);
	void doReceiveAchievement(const fgame::CSReceiveAchievementResp&);
	void doReceiveAward(const fgame::CSReceiveAchievementRewardResp&);
	void scrolView(int percnt);
	void setRemCell(TableViewCell* pCell);
	void setCanTouch(bool );
	bool isCanTouch(){return mCanTouch;}

	void closeAchievePanel();

	void TipAchieveComplent(std::list<int>);

	void setAchieveGeted();
	int getAchieveLv(){return m_getAchievementResp.achievementlevel();}

	void setIsPvpFight(bool b) {mIsPvpFight = b;}

	void updateLocalData(std::list<int>);

	const fgame::CSGetAchievementResp getAchievementData(){return m_getAchievementResp;};
	bool isSendQueryAchievementStatus();

	void checkComplentTip();
private:
	void updateUI();
	void sortAchieve();

	
private:
	AchieveProxy m_AchieveProxy;

	TableViewCell* mGetCell;
	bool mCanTouch;
	int mRecId;

	fgame::CSGetAchievementResp m_getAchievementResp;

	bool mIsPvpFight;
};

#define sgAchieveControl() Singleton<AchieveControl>::Get()
