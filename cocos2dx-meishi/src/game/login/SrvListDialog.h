#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "cocos2d.h"
#include "cocos-ext.h"

#include "BaseProxy.h"

#include "SrvListControl.h"

USING_NS_CC_EXT;
USING_NS_CC;

class SrvListDialog : public Window, public TableViewDataSource, public TableViewDelegate
{
public:
	SrvListDialog();                                           
	~SrvListDialog();

	virtual bool init();

	CREATE_FUNC(SrvListDialog);

	virtual void onShow();


	virtual	TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
	virtual	ssize_t numberOfCellsInTableView(TableView *table);
	virtual	Size cellSizeForTable(TableView *table);
	// tableView Delegate
	virtual void tableCellTouched(TableView* table, TableViewCell* cell);

	void initView();

	void updateUI();
protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
private:
	ui::Layout* mPanelView;
	TableView* mTableView;

};



class SrvListCell : public TableViewCell
{
public:
	SrvListCell();
	~SrvListCell();
	bool init();
	void updateCell(SrvInfo* pSrvInfo);
	CREATE_FUNC(SrvListCell);
private:
	Node* mRoot;
	ui::Text* mTextServerName;
	ui::Text* mTextSatus;
};
