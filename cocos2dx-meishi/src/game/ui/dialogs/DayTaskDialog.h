#pragma once

#include "game/ui/system/ActionWindow.h"
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;
USING_NS_CC;


class DayTaskDialog : public ActionWindow, public TableViewDataSource, public TableViewDelegate
{
public:

	DayTaskDialog();
	~DayTaskDialog();

	virtual bool	init();
	void			onShow();
	// tableViewDataSource
	virtual			TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
	virtual			ssize_t numberOfCellsInTableView(TableView *table);
	virtual			Size cellSizeForTable(TableView *table);
	// tableView Delegate
	virtual void	tableCellTouched(TableView* table, TableViewCell* cell);
	virtual void	onMenuItemAction(std::string, cocos2d::Ref*);
	void			updateCell();
	void			updateMissions(bool showAction = false);
	CREATE_FUNC(DayTaskDialog);

	void			update(float dt);

	//?¡Â?¡¥?-??¦Ì¡Â ¡Á¨®¨¤¨¤ ?¨¬3D
	virtual void aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
	//?¡Â?¡¥?-¨º??t??¦Ì¡Â ¡Á¨®¨¤¨¤ ?¨¬3D
	virtual void onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex);

private:
	TableView*		mTaskTableView;
	ImageView*      mTopShawdow;
	ImageView*      mDownShawdow;

	cocostudio::CCArmature* mDialogArmature;

	int				mListOriginOffSetX;
};
