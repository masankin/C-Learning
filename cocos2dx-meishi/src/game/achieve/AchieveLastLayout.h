#pragma once

#include "AchieveLayout.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

class AchieveLastLayout: public AchieveLayout
{
public:
	AchieveLastLayout(void);
	~AchieveLastLayout(void);
	
	CREATE_FUNC(AchieveLastLayout);

	virtual bool init();
	virtual void updateInfo();

	virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(TableView *table);
	virtual void tableCellTouched(TableView* table, TableViewCell* cell);
	virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);

	virtual TableViewCell* tableCellAtIndex2(TableView *table, ssize_t idx);
private:
	TableViewCell* tabeCellInfo(TableViewCell* pCell, ssize_t idx);
	TableView*	mRewardTableView;
	ui::Text* mText;
};

