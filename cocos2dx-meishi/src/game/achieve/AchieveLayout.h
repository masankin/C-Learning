#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;

enum AchieveType
{
	Achieve_Complete = 0,
	Achieve_PVP,
	Achieve_PVE,
	Achieve_OTHER,
};

#define  LIST_ITEM_TAG 10
#define TABLE_WIDTH 900
#define TABLE_HEIGHT 390

class AchieveLayout:public cocos2d::Layer, public TableViewDataSource, public TableViewDelegate
{
public:
	AchieveLayout(void);
	~AchieveLayout(void);

	virtual void updateInfo() = 0;

	virtual TableViewCell* tableCellAtIndex2(TableView *table, ssize_t idx) = 0;
};

