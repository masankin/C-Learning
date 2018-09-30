#pragma once
#include  "game/ui//system/ActionWindow.h"
#include "AchieveLayout.h"
#include "net/proto/ClientSvrMsg.pb.h"

USING_NS_CC;

class AchievePanel :public ActionWindow
{
public:
	AchievePanel(void);
	~AchievePanel(void);

	CREATE_FUNC(AchievePanel);
	virtual bool init();

	void updateUI(const fgame::CSGetAchievementResp&);

	virtual void setShow(bool b);

	void showAward(int achieveId, bool pgeted = true);
	void scrolViewSlider(int percnt);
	void showAwardByLv(int achieveLv);

	void removeTableViewCell(TableViewCell* pCell);

	virtual void showTextTip(const std::string);

	void onSelectLayout(int type);

	void jewelCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	virtual void aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
private:
	void updateContent(fgame::CSGetAchievementResp&);
	AchieveLayout* createAchieveLayout(AchieveType);
	AchieveLayout* getAchieveLayout(AchieveType);

	void SelectLabel(ImageView* label, bool selected);
	void scheduleLoad(float dt);

	void updateRedTip();
private:
	AchieveType m_achieveType;
	std::vector<ImageView*> m_AchieveBtnVec;
	std::map<int,AchieveLayout*> mAchieveLayoutMap;

	Node* m_contentNode;
	ui::Text* mAchievePointCount;
	ui::Text* mAchieveLevel;
	ui::LoadingBar* mPointCountLoad;
	ui::Layout* mPointAwardBtn;
	ui::Slider* mScrolSlider;
	ui::Text* mAchievePointPrent;
	cocos2d::Sprite* mPointIcoSp;
	TableView* mloadTableView;
	cocostudio::Armature* pBtnJewel;
	cocostudio::Armature* pGetPointArm;
	int mRmCellIdx;
	float mMoveY;
};

