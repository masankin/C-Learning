#include "ActLoginLayout.h"
#include "ActLoginItem.h"
#include "data/TableManager.h"
#include "act/ActControl.h"

#define  LIST_ITEM_TAG 10
#define TABLE_WIDTH 888.5
#define TABLE_HEIGHT 440
#define ITEM_WIDTH 200
#define ITEM_MARG 50
ActLoginLayout::ActLoginLayout(void)
{
	mGuideNode = NULL;
}


ActLoginLayout::~ActLoginLayout(void)
{
}

bool ActLoginLayout::init()
{
	if (!ActLayout::init())
	{
		return false;
	}
	Node* pRoot = CSLoader::getInstance()->createNode("ui/ActLoginNode.csb");
	

	mRewardTableView = TableView::create(this, Size(TABLE_WIDTH,TABLE_HEIGHT));
	mRewardTableView->setDirection(cocos2d::extension::ScrollView::Direction::HORIZONTAL);
	mRewardTableView->setDelegate(this);
	mRewardTableView->setScaleY(1.0f);
	mRewardTableView->setSwallowsTouches(false);
	mRewardTableView->setBounceable(true);
	dynamic_cast<Node*>(pRoot->getChildByName("Panel_1"))->addChild(mRewardTableView);
	Point tPos = pRoot->getChildByName("Panel_1")->getPosition();
	addChild(pRoot);
	mPanelNode = pRoot->getChildByName("Panel_1");

	//dynamic_cast<ImageView*>(pRoot->getChildByName("Image_13"))->setVisible(false);

	
	return true;
}

void ActLoginLayout::updateInfo()
{
	mRewardTableView->reloadData();
	
	if (sgActControl()->getActProxy()->getSevenDaySiginList().nowday()>=4)
	{
		mRewardTableView->setContentOffset(Vec2( mRewardTableView->getContentOffset().x-
			(sgActControl()->getActProxy()->getSevenDaySiginList().nowday() - 3)*ITEM_WIDTH,
			mRewardTableView->getContentOffset().y ));
	}
	
	//mRewardTableView->setContentOffsetInDuration(Vec2(1,1),0.5);
}

bool ActLoginLayout::tableViewMove(bool isRigth)
{
	int lMoveX = 0;
	bool lIsCanMove = false;
	if (isRigth)
	{
		
		if (mRewardTableView->getContentOffset().x >= -2*ITEM_WIDTH)
		{ 
			lMoveX = -1;
			lIsCanMove = true;
		}
			
	}
	else
	{
		
		if (mRewardTableView->getContentOffset().x < 0)
		{
			lMoveX = 1;
			lIsCanMove = true;
		}
			
	}
	int lCellIndex = mRewardTableView->getContentOffset().x / ITEM_WIDTH + lMoveX;
	if (lIsCanMove)
	{
		
		mRewardTableView->setContentOffsetInDuration(Vec2(
			lCellIndex*ITEM_WIDTH,
			mRewardTableView->getContentOffset().y), 0.25);
	}
	else
	{
		float distance = -ITEM_WIDTH;
		if (!isRigth) {
			distance = -distance;
		}
		mRewardTableView->setContentOffsetInDuration2(Vec2(
			lCellIndex*ITEM_WIDTH,
			mRewardTableView->getContentOffset().y), 0.25, distance, 0.25);
	}
	
	CCLOG("[trace] ActLoginLayout::tableViewMove ContentOffeset.x = %f", mRewardTableView->getContentOffset().x);
	return lIsCanMove;
}


Size ActLoginLayout::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	return Size(ITEM_WIDTH, 420);
}

ssize_t ActLoginLayout::numberOfCellsInTableView(TableView *table) 
{
	return 8;
}

void ActLoginLayout::tableCellTouched(TableView* table, TableViewCell* cell)
{
	ActLoginItem* pItem = dynamic_cast<ActLoginItem*>(cell->getChildByTag(LIST_ITEM_TAG));
	if (!pItem)
	{
	return;
	}

	//pItem->onTouch();
}

TableViewCell* ActLoginLayout::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->dequeueCell();
	ActLoginItem* pLoginItem = NULL;
	if (!cell)
	{
		cell = new TableViewCell();
		cell->autorelease();

		pLoginItem = ActLoginItem::create();
		//pLoginItem->setPosition(Vec2(ITEM_WIDTH + ITEM_MARG, 10));
		
		cell->addChild(pLoginItem,0,LIST_ITEM_TAG);
	}else
	{
		pLoginItem = dynamic_cast<ActLoginItem*>(cell->getChildByTag(LIST_ITEM_TAG));
	}
	
	if(idx == 0)
	{ 
		if(NULL == mGuideNode)
		{
			mGuideNode = Node::create();
			addChild(mGuideNode);
			mGuideNode->setTag(9999);
		}
		mGuideNode->setPosition(convertToWorldSpace(Point(0, 0)) + pLoginItem->getGuideNode()->getPosition()); 
	}

	int pDay = idx +1;	
	if (pDay <=7)
	{
		const ActLogin* pActLogin = sgTableManager()->getActLogin(pDay);
		bool pIsGet = false;
		bool pIsCanGet = false;
		int pGiftId = pActLogin->mGiftNor;

		if (sgActControl()->getActProxy()->getSevenDaySiginList().has_nowday())
		{
			pIsGet = sgActControl()->getActProxy()->getSevenDaySiginList().getlist(idx).isget();
			pIsCanGet = sgActControl()->getActProxy()->getSevenDaySiginList().nowday()>=pDay?true:false;

			if (sgActControl()->getActProxy()->getSevenDaySiginList().getlist(idx).isnewgift())
			{
				pGiftId = pActLogin->mGiftSpe;
			}
		}

		pLoginItem->updateInfo(pDay,pIsGet,pIsCanGet,pGiftId);
		pLoginItem->setVisible(true);
	}else
	{
		pLoginItem->setVisible(false);
		
	}
	return cell;
}

