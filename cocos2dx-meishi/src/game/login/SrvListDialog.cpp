#include "SrvListDialog.h"
#include "game/ui//PageFunction.h"
#include "net/UserObj.h"
#include "SoundManager.h"


#define SERVER_INFO_TAG_BEGIN 10000
#define TBVIEW_HEIGH 300
#define TBVIEW_WIDTH 520
#define CELL_HEIGH 95

SrvListDialog::~SrvListDialog()
{
}


SrvListDialog::SrvListDialog()
{
	mIsCanClean = false;
}

void SrvListDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	sgSoundManager()->setEffectToLastState();

	if (cName == "onClose")
	{
		this->setShow(false);
		return;
	}
	
}

bool SrvListDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/SrvListDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);
		mPanelView = dynamic_cast<ui::Layout *>(mRoot->getChildByName("PanelView"));
		initView();

		//setPopType(E_POP_TYPE_NONE);

		ret = true;	

	} while (0);


	return ret;
}

void SrvListDialog::initView()
{
	mTableView = TableView::create(this, Size(TBVIEW_WIDTH, TBVIEW_HEIGH));
	mTableView->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	//mTableView->setSwallowsTouches(true);
	//mTableView->setTouchEnabled(false);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	//mTableView->setPosition(mPanelView->getContentSize()/2);
	mPanelView->addChild(mTableView);
	
}


void SrvListDialog::onShow()
{

	Window::onShow();

	updateUI();
}

void SrvListDialog::updateUI()
{
	if (mTableView)
		mTableView->reloadData();
}


// tableViewDataresource
TableViewCell* SrvListDialog::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->cellAtIndex(idx);
	if (!cell) {
		cell = SrvListCell::create();
	}
	SrvInfo* pSrvInfo = sgSrvListControl()->getSrvList().at(idx);
	if (!pSrvInfo) return nullptr;
	(dynamic_cast<SrvListCell*>(cell))->updateCell(pSrvInfo);
	cell->setTag(idx+SERVER_INFO_TAG_BEGIN);
	return cell;
}

ssize_t SrvListDialog::numberOfCellsInTableView(TableView *table)
{
	return sgSrvListControl()->getSrvList().size();
}

Size SrvListDialog::cellSizeForTable(TableView *table)
{
	return Size(TBVIEW_WIDTH, CELL_HEIGH);
}

// tableView Delegate
void SrvListDialog::tableCellTouched(TableView* table, TableViewCell* cell)
{
	int idx = cell->getTag() - SERVER_INFO_TAG_BEGIN;
	if (idx >= 0)
		sgMessageManager()->sendMessage(EMT_CHANGE_SERVER, StringUtils::format("%d", idx));
	this->setShow(false);
}



SrvListCell::SrvListCell() :mRoot(nullptr)
{

}

SrvListCell::~SrvListCell()
{

}

bool SrvListCell::init()
{
	bool ret = false;
	do {
		mRoot = CSLoader::getInstance()->createNode("ui/SrvListCell.csb");
		mTextServerName = dynamic_cast<ui::Text* >(mRoot->getChildByName("TextServerName"));
		mTextSatus = dynamic_cast<ui::Text* >(mRoot->getChildByName("TextSatus"));
		this->addChild(mRoot);
		ret = true;
	} while (0);

	return ret;
}

void SrvListCell::updateCell(SrvInfo* pSrvInfo)
{

	CCLOG("%s", pSrvInfo->mName.c_str());
	mTextServerName->setString(pSrvInfo->mName.c_str());
	//mTextSatus->setText("");
}
