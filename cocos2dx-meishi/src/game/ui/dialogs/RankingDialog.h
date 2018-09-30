#pragma once

#include "game/ui/system/ActionWindow.h"
#include "ranking/RankingControl.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
class RankingDialog : public ActionWindow
{
public:
	CREATE_FUNC(RankingDialog);

	void updateRankingList(eRankingGroupType groupID, const RankingControl::RankingList& rankingList);
	void setRankAwardInfo(bool status);

private:
	void updateCountDown(float dt);
	ui::Text* mTextGetTime;

	Node* mPanel_noRank;
	Node* mPanel_maxGrade;
	Node* mPanel_normal;
	Node* mBtnAward;
	Node* mImage_timeBg;
	int   mTimeRemainingModel;
	int   mTargetWeek;
	string   mTargetTime;
protected:
	
	static void SelectLabel(ImageView* label, bool selected);
	static std::string GetGroupName(eRankingGroupType groupType);

	RankingDialog();
	virtual ~RankingDialog();

	virtual bool init();
	virtual void onShow() override;
	virtual void onClose() override;

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	virtual void aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
	//virtual void onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex);

	void onListViewScrollEvent(cocos2d::Ref* sender, ui::ScrollView::EventType eventType);
	void onSelectList(cocos2d::Ref* pSender, eRankingGroupType groupID);

	cocos2d::ui::Widget* fetchRowNode();
	void recycleRowNode(cocos2d::ui::Widget* node);
	void initRowNode(cocos2d::ui::Widget* node, const RankingControl::PlayerData* info, int index);
	void cleanRankingList();

	void updatePlayerInfo();

	void updatePvpResultFrame();
	void cleanResultList();

protected:
	static const int DefaultTopNSize = 50;
	static const int RowNodeTag = 100;

	eRankingGroupType mCurrentGroup;

	cocos2d::ui::ListView* mRankListView;
	cocos2d::ui::ImageView* mSelectedGroupBtn;

	cocos2d::Node* mPlayerInfoNode;
	cocos2d::ui::ImageView* mPortraitFrame;

	/*cocos2d::ui::ImageView* mPlayerFrame;
	cocos2d::ui::ImageView* mPvpResultFrame;

	cocos2d::ui::ImageView* mPlayerRowFrame;
	cocos2d::ui::ImageView* mPlayerRow;*/

	std::list<cocos2d::ui::Widget*> mRowNodeRecycleList;

	cocostudio::CCArmature * mBoxAni;
};


/************************************************************************/
/*                                                                      */
/************************************************************************/
class RankingInfoDialog :public ActionWindow
{
public:
	RankingInfoDialog();
	virtual ~RankingInfoDialog();

	virtual bool init();

	CREATE_FUNC(RankingInfoDialog);

	virtual void  onShow();

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

};
