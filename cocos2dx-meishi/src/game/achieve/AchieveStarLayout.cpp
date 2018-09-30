#include "AchieveStarLayout.h"
#include "AchieveRewardItem.h"
#include "../data/TableManager.h" 
#include "EnvSet.h"
#include "game/net/UserObj.h"

AchieveStarLayout::AchieveStarLayout(void)
{
}


AchieveStarLayout::~AchieveStarLayout(void)
{
}


bool AchieveStarLayout::init()
{
	if (!AchieveLayout::init())
	{
		return false;
	}

	mRewardTableView = TableView::create(this, Size(TABLE_WIDTH,TABLE_HEIGHT));
	mRewardTableView->setDirection(extension::ScrollView::Direction::HORIZONTAL);
	mRewardTableView->setDelegate(this);
	mRewardTableView->setSwallowsTouches(false);
	addChild(mRewardTableView);
	
	return true;
}

void AchieveStarLayout::updateInfo()
{
	mRewardTableView->reloadData();//
}


Size AchieveStarLayout::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	return Size(TABLE_WIDTH/3,TABLE_HEIGHT);
}

 ssize_t AchieveStarLayout::numberOfCellsInTableView(TableView *table) 
{
	return sgTableManager()->getAchievementAwardCount(A_Star);
}

void AchieveStarLayout::tableCellTouched(TableView* table, TableViewCell* cell)
{
	 AchieveRewardItem* pItem = dynamic_cast<AchieveRewardItem*>(cell->getChildByTag(LIST_ITEM_TAG));
	 if (!pItem)
	 {
		 return;
	 }
	 
	 pItem->onTouch();
}

TableViewCell* AchieveStarLayout::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->dequeueCell();
	//TableViewCell* cell = table->cellAtIndex(idx);
	cell = tabeCellInfo(cell,idx);

	return cell;
}

 TableViewCell* AchieveStarLayout::tableCellAtIndex2(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->cellAtIndex(idx);
	cell = tabeCellInfo(cell,idx);
	return cell;
}

TableViewCell* AchieveStarLayout::tabeCellInfo(TableViewCell* cell, ssize_t idx)
{
	AchieveRewardItem *pRewardItem = NULL;

	if (!cell){
		cell = new TableViewCell();
		cell->autorelease();

		pRewardItem = AchieveRewardItem::create(Achieve_OTHER);
		pRewardItem->setPosition(TABLE_WIDTH/6,TABLE_HEIGHT/2 - 23);
		cell->addChild(pRewardItem, 0, LIST_ITEM_TAG);
	}
	else{
		pRewardItem = dynamic_cast<AchieveRewardItem*>(cell->getChildByTag(LIST_ITEM_TAG));
	}
	const AchievementAward* pAward = sgTableManager()->getAchievementAwardByID(A_Star,idx);
	if (pAward)
	{
		pRewardItem->updateInfo(pAward);
	}	
	return cell;
}
