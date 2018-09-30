#pragma once

#include "AchieveLayout.h"
USING_NS_CC;
class AchievePointLayout: public AchieveLayout
{
public:
	AchievePointLayout(void);
	~AchievePointLayout(void);
	
	CREATE_FUNC(AchievePointLayout);

	virtual bool init();
	virtual void updateInfo();

	virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(TableView *table);
	virtual void tableCellTouched(TableView* table, TableViewCell* cell);
	virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
	virtual void scrollViewDidScroll(extension::ScrollView* view);

	virtual TableViewCell* tableCellAtIndex2(TableView *table, ssize_t idx);

private:
	TableViewCell* tabeCellInfo(TableViewCell* pCell, ssize_t idx);

	TableView*	mRewardTableView;
};

