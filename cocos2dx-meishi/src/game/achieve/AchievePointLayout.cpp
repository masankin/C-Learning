#include "AchievePointLayout.h"
#include "AchieveRewardItem.h"
#include "../data/TableManager.h"
#include "EnvSet.h"
#include "AchieveControl.h"
#include "game/net/UserObj.h"

AchievePointLayout::AchievePointLayout(void)
{
}


AchievePointLayout::~AchievePointLayout(void)
{
}


bool AchievePointLayout::init()
{
	if (!AchieveLayout::init())
	{
		return false;
	}

	mRewardTableView = TableView::create(this, Size(TABLE_WIDTH,TABLE_HEIGHT+10));
	mRewardTableView->setDirection(extension::ScrollView::Direction::HORIZONTAL);
	mRewardTableView->setDelegate(this);
	mRewardTableView->setSwallowsTouches(false);
	addChild(mRewardTableView);
	
	return true;
}

void AchievePointLayout::updateInfo()
{
	mRewardTableView->reloadData();//
}


Size AchievePointLayout::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	return Size(TABLE_WIDTH/3,TABLE_HEIGHT);
}

 ssize_t AchievePointLayout::numberOfCellsInTableView(TableView *table) 
{
	return sgTableManager()->getAchievementAwardCount(A_Point);
}

void AchievePointLayout::tableCellTouched(TableView* table, TableViewCell* cell)
{
	 AchieveRewardItem* pItem = dynamic_cast<AchieveRewardItem*>(cell->getChildByTag(LIST_ITEM_TAG));
	 if (!pItem)
	 {
		 return;
	 }
	 
	 pItem->onTouch();
}

TableViewCell* AchievePointLayout::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->dequeueCell();
	//TableViewCell* cell = table->cellAtIndex(idx);
	cell = tabeCellInfo(cell,idx);

	return cell;
}

void AchievePointLayout::scrollViewDidScroll(extension::ScrollView* view)
{
	//sgAchieveControl()->scrolView(std::fabs(view->getContainer()->getPositionY()/view->getContainer()->getContentSize().height)*100);
}

TableViewCell* AchievePointLayout::tableCellAtIndex2(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->cellAtIndex(idx);
	cell = tabeCellInfo(cell,idx);
	return cell;
}

TableViewCell* AchievePointLayout::tabeCellInfo(TableViewCell* cell, ssize_t idx)
{
	AchieveRewardItem *pRewardItem = NULL;

	if (!cell){
		cell = new TableViewCell();
		cell->autorelease();
		cell->setAnchorPoint(ccp(0.5,0.5));
		pRewardItem = AchieveRewardItem::create(Achieve_PVE);
		pRewardItem->setPosition(TABLE_WIDTH/6,TABLE_HEIGHT/2 - 23);
		cell->addChild(pRewardItem, 0, LIST_ITEM_TAG);
	}
	else{
		pRewardItem = dynamic_cast<AchieveRewardItem*>(cell->getChildByTag(LIST_ITEM_TAG));
	}
	const AchievementAward* pAward = sgTableManager()->getAchievementAwardByID(A_Point,idx);

	pRewardItem->updateInfo(pAward);
	return cell;
}
