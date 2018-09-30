#include "AchieveAddUpLayout.h"
#include "AchieveRewardItem.h"
#include "../data/TableManager.h"
#include "EnvSet.h"


AchieveAddUpLayout::AchieveAddUpLayout(void)
{
}


AchieveAddUpLayout::~AchieveAddUpLayout(void)
{
}


bool AchieveAddUpLayout::init()
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

void AchieveAddUpLayout::updateInfo()
{
	mRewardTableView->reloadData();//
}


Size AchieveAddUpLayout::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	return Size(TABLE_WIDTH/3,TABLE_HEIGHT);
}

 ssize_t AchieveAddUpLayout::numberOfCellsInTableView(TableView *table) 
{
	return sgTableManager()->getAchievementAwardCount(A_AddUp);
}

void AchieveAddUpLayout::tableCellTouched(TableView* table, TableViewCell* cell)
{
	 AchieveRewardItem* pItem = dynamic_cast<AchieveRewardItem*>(cell->getChildByTag(LIST_ITEM_TAG));
	 if (!pItem)
	 {
		 return;
	 }
	 
	 pItem->onTouch();
}

TableViewCell* AchieveAddUpLayout::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->dequeueCell();
	//TableViewCell* cell = table->cellAtIndex(idx);
	cell = tabeCellInfo(cell,idx);

	return cell;
}

TableViewCell* AchieveAddUpLayout::tableCellAtIndex2(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->cellAtIndex(idx);
	cell = tabeCellInfo(cell,idx);
	return cell;
}

TableViewCell* AchieveAddUpLayout::tabeCellInfo(TableViewCell* cell, ssize_t idx)
{
	AchieveRewardItem *pRewardItem = NULL;

	if (!cell){
		cell = new TableViewCell();
		cell->autorelease();

		pRewardItem = AchieveRewardItem::create(Achieve_PVP);
		pRewardItem->setPosition(TABLE_WIDTH/6,TABLE_HEIGHT/2 - 23);
		cell->addChild(pRewardItem, 0, LIST_ITEM_TAG);
	}
	else{
		pRewardItem = dynamic_cast<AchieveRewardItem*>(cell->getChildByTag(LIST_ITEM_TAG));
	}
	const AchievementAward* pAward = sgTableManager()->getAchievementAwardByID(A_AddUp,idx);
	pRewardItem->updateInfo(pAward);
	return cell;
}
