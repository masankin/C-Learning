#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/UILayout.h"
#include "ui/UIWidget.h"
#include "game/daytask/DayTaskControl.h"

USING_NS_CC_EXT;
USING_NS_CC;

class DayTaskCell: public TableViewCell
{
public:
	DayTaskCell();
	~DayTaskCell();
	bool		init();	
	void		onCellClick(Ref* sender);
	void		updateCell(int id);
	CREATE_FUNC(DayTaskCell);
private:
	std::string getStringNum(int num);   
	void		showItemInfo(Node* itemNode, std::string iconName, int count, int itemType = 0);
	void		refreshBtn(const DayMission *mission_data);
	void		showPeppers(int count);

private:
	Node*		mRoot;
	int			mMId;
	int			mPageId;

	ui::Button* mGetBtn;
	ui::Button* mGoBtn;

	ui::Text*   mTitleLabel;
	ui::Text*   mMissionDesLabel;

	Node*       mItems[4];
	Sprite*     mPepperSprites[5];

	Vec2        mMiddlePos;
	Vec2        mFirstPos;
};
