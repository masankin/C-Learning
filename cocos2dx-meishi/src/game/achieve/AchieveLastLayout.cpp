#include "AchieveLastLayout.h"
#include "AchieveRewardItem.h"
#include "../data/TableManager.h"
#include "game/net/UserObj.h"
#include "EnvSet.h"


AchieveLastLayout::AchieveLastLayout(void)
{
}


AchieveLastLayout::~AchieveLastLayout(void)
{
}


bool AchieveLastLayout::init()
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

void AchieveLastLayout::updateInfo()
{
	mRewardTableView->reloadData();//
}


Size AchieveLastLayout::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	return Size(TABLE_WIDTH/3,TABLE_HEIGHT);
} 

 ssize_t AchieveLastLayout::numberOfCellsInTableView(TableView *table) 
{
	return sgTableManager()->getAchievementAwardCount(A_Last);
}

void AchieveLastLayout::tableCellTouched(TableView* table, TableViewCell* cell)
{
	 AchieveRewardItem* pItem = dynamic_cast<AchieveRewardItem*>(cell->getChildByTag(LIST_ITEM_TAG));
	 if (!pItem)
	 {
		 return;
	 }
	 
	 pItem->onTouch();
}

TableViewCell* AchieveLastLayout::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->dequeueCell();
	//TableViewCell* cell = table->cellAtIndex(idx);
	cell = tabeCellInfo(cell,idx);
	
	return cell;
}

 TableViewCell* AchieveLastLayout::tableCellAtIndex2(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->cellAtIndex(idx);
	cell = tabeCellInfo(cell,idx);
	return cell;
}

 TableViewCell* AchieveLastLayout::tabeCellInfo(TableViewCell* cell, ssize_t idx)
 {
	 AchieveRewardItem *pRewardItem = NULL;

	 if (!cell){
		 cell = new TableViewCell();
		 cell->autorelease();

		 pRewardItem = AchieveRewardItem::create(Achieve_Complete);
		 pRewardItem->setPosition(TABLE_WIDTH/6,TABLE_HEIGHT/2 - 23);
		 cell->addChild(pRewardItem, 0, LIST_ITEM_TAG);
	 }
	 else{
		 pRewardItem = dynamic_cast<AchieveRewardItem*>(cell->getChildByTag(LIST_ITEM_TAG));
	 }
	 const AchievementAward* pAward = sgTableManager()->getAchievementAwardByID(A_Last,idx);

	 int pLoadNow ,pLoadAll;
	 if (pAward->category ==A_Star )
	 {
		 pLoadNow = sgUserObj()->getTotalStars();
		 pLoadAll = pAward->getStar;
	 }else if (pAward->category == A_Point)
	 {
		 if ((RewardItemStatus)pAward->mstatus == REWARD_BTN_UNREACHABLE)
		 {
			 pLoadNow = 0;
		 }else
			 pLoadNow = 1;

		 pLoadAll = 1;
	 }else if (pAward->category == A_AddUp)
	 {
		 pLoadNow = 0;
		 pLoadAll = pAward->addTotalProgress;
	 }
	 pLoadNow = pLoadAll;

	 pRewardItem->updateInfo(pAward,pLoadNow,pLoadAll,0);
	 return cell;
 }
