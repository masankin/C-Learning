#pragma once
#include "ActLayout.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;
USING_NS_CC;

class ActLoginLayout : public ActLayout, public TableViewDataSource, public TableViewDelegate
{
public:
	ActLoginLayout(void);
	~ActLoginLayout(void);
	CREATE_FUNC(ActLoginLayout);
	virtual bool init();
	virtual void updateInfo();
	virtual bool tableViewMove(bool isRigth);


	virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(TableView *table);
	virtual void tableCellTouched(TableView* table, TableViewCell* cell);
	virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
	Node* mGuideNode;
	Node* mPanelNode;
private:
	TableView*	mRewardTableView;

	
};

