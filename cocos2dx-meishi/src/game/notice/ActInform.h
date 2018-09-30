#pragma once

#include  "game/ui/system/ActionWindow.h"
#include "core/StringLabel.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "data/TableManager.h"

USING_NS_CC_EXT;
USING_NS_CC;


class ActInform :public ActionWindow, public TableViewDataSource, public TableViewDelegate
{
private:
	enum ACTINFORMTYPE
	{
		ACTINFORM_ACT,
		ACTINFORM_INFORM,

	};
public:
	ActInform();
	~ActInform();

	virtual bool init();

	virtual void onShow();
	virtual void onClose();

	void onEnter();
	void onExit();

	virtual	TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
	virtual	ssize_t numberOfCellsInTableView(TableView *table);
	virtual	Size cellSizeForTable(TableView *table);
	// tableView Delegate
	virtual void tableCellTouched(TableView* table, TableViewCell* cell);

	virtual void onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex);
	virtual void windowAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);

	void initView();
	void setContent(std::string stHead,std::string stContent);
	void updateContentUI(int iSubId);
	void updateBtnUI();

	void switchTilte(int iActId);
	void updateBtnTips();
	CREATE_FUNC(ActInform);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	bool checkIsActOpen(int iPageA,int iPageB = -1);

	ui::ScrollView* mScrollText;
	StringLabel* mStringLabel;

	CC_SYNTHESIZE(int, mShowTimes, ShowTimes);
private:
	Node* mContent;
	int mActInformType;
	int mPrevActInformType;
	int mPrevActSelectedId;
	int mPrevInformSelectedId;
	std::map<int, Node* > mActItemMap;
	TableView* mActItemTableView;
	TableView* mNoticeItemTableView;
	ui::Layout* mPanelViewActItem;
	ui::Layout* mPanelViewNoticeItem;
	ui::Layout* mPanelAct;
	ui::Layout* mPanelInform;
	float mfPanelActY;
	float mfPanelInformY;
	ui::Button* mBtnClose;
	ImageView* mImageAct;
	ImageView* mImageInform;
	ui::Button* mBtnGoto;
	Node* mNodeItem;
	Node* mNodeCom;
	Node* mNodeAct;
	ui::Text* mTextTitle;
	ui::Text* mTextActTime;
	ui::Text* mTextActDetail;
	Sprite* mSpriteGrade;
	cocostudio::Armature* mArnDengGuang;
	ActInformList mInformList;
	ActInformList mActList;
	bool mEnableClickMusic;
	bool mbFirstTouchAct;
};


class ActInformCell : public TableViewCell
{
public:
	ActInformCell();
	~ActInformCell();
	bool init();
	void initUI();
	void updateCell(ActInformData* pData);
	void onTouched();
	CREATE_FUNC(ActInformCell);
private:
	Node* mRoot;
	Sprite* mSpriteItem;

	ui::Text* mTextName;

	ui::Text* mTextName1;
	Sprite* mSpriteRedDot;
	bool mIsClicked;
};
