#include "HonorShopDialog.h"
#include "core/SoundManager.h"
#include "game/shop/HonorShopItem.h"
#include "core/MessageManager.h"
#include "game/net/UserObj.h"
#include "game/data/TableManager.h"
#include "game/ui/PageFunction.h"
#include "game/ui/dialogs/GenerlDialog.h"

#define PAGE_DATA_COUNT 3
#define CMD_RIGHT       2
#define CMD_LEFT        1

#define LIST_ITEM_TAG 10
#define TABLE_WIDTH 870
#define TABLE_HEIGHT 420
#define ITEM_WIDTH 290
#define ITEM_MARG 50

HonorShopDialog::HonorShopDialog()
:mItemIndex(1),
mHonorCountLabel(nullptr)
{
	sgMessageManager()->regisiterMessageHandler(EMT_UPDATE_EXCHANGE, this);

	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_RongYSC.png", "fla_RongYSC.plist", "fla_RongYSC.xml");
}

HonorShopDialog::~HonorShopDialog()
{
	sgMessageManager()->removeMessageHandler(EMT_UPDATE_EXCHANGE);
}

void HonorShopDialog::onReceiveMassage(GameMessage *message)
{
	MessageUpdateExchange* msg = dynamic_cast<MessageUpdateExchange*>(message);
	if (msg && msg->getMessageType() == EMT_UPDATE_EXCHANGE) {
		
		// 错误码处理
		CCLOG("[jim]  MessageUpdateExchange  update itemId = %d", msg->getItemId());
		int cmdId = msg->getItemId();
		if (cmdId != 0) {
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), "sale out!", CC_CALLBACK_1(HonorShopDialog::sellOutCallback, this), false);
		}

		refreshShop();
	}
}

void HonorShopDialog::onMenuItemAction(std::string cbName, cocos2d::Ref* sender)
{
	if (cbName == "onHonorShopClose") {
		sgSoundManager()->playMusicEffect(SOUND_CLICK_BACK);
		mHonorTableView->runAction(Spawn::createWithTwoActions(ScaleTo::create(0.2f, 0.0), FadeOut::create(0.15f)));
		this->setShow(false);
	}
	else if (cbName == "onLeft"){
		tableViewMove(false);
	}
	else if (cbName == "onRight") {
		tableViewMove(true);
	}
	else if (cbName == "onHelp")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "HonorShopHelp.csb", 1);
	}
	else if (cbName == "onHonorPlus")
	{
		CCLOG("[trace]   onPlus.......... ");
		sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "HonorShopHelp.csb", 1);
		//GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_GET_HONOR), CC_CALLBACK_1(HonorShopDialog::goGetHonorCallback, this), false, true);
	}
}

void HonorShopDialog::sellOutCallback(cocos2d::Ref* sender)
{
	//CCLOG("[trace]  sellOut ................... ");
	sgHonorShopControl()->try2SyncExchangeInfo();
}

void HonorShopDialog::goGetHonorCallback(cocos2d::Ref* sender)
{
	CCLOG("[trace]  goGetHonor .................");
	GenerlDialog::hide();
	this->setShow(false);
	sgPageFunction()->gotoFunction(MSG_FUC_WIN_PVP);
}

bool HonorShopDialog::init()
{
	bool ret = false;

	do {
		mRoot = CSLoader::getInstance()->createNode("HonorShopDialog.csb");
		CC_BREAK_IF(!mRoot);
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		setAnimation("fla_RongYSC");
		
		auto closeBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("CloseBtn"));
		closeBtn->setPosition(Vec2::ZERO);
		auto honorNode = dynamic_cast<cocos2d::Node*>(mRoot->getChildByName("HonorNode"));
		honorNode->setPosition(Vec2::ZERO);

		auto rightBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("RightBtn"));
		rightBtn->setPosition(Vec2::ZERO);

		auto leftBtnNode = dynamic_cast<cocos2d::Node*>(mRoot->getChildByName("LeftBtnNode"));
		//auto leftBtn = dynamic_cast<ui::Button*>(leftBtnNode->getChildByName("LeftBtn"));
		leftBtnNode->setPosition(Vec2::ZERO);

		mHonorCountLabel = dynamic_cast<ui::Text*>(honorNode->getChildByName("HonorLabel"));
		mHonorCountLabel->setString(StringUtils::format("%d", sgUserObj()->getPersonalInfo()->honorcount()));//sgUserObj()->getHonorCount()));

		mHonorTableView = TableView::create(this, Size(TABLE_WIDTH, TABLE_HEIGHT));
		mHonorTableView->setDirection(cocos2d::extension::ScrollView::Direction::HORIZONTAL);
		mHonorTableView->setDelegate(this);
		mHonorTableView->setSwallowsTouches(false);
		mHonorTableView->setBounceable(true);
		mHonorTableView->setAnchorPoint(ccp(0.5, 0.5));
		mHonorTableView->setPosition(ccp(-TABLE_WIDTH / 2 + 5, -TABLE_HEIGHT/2 + 80));
		mRoot->getChildByName("mCenter")->addChild(mHonorTableView);

		changeBone(mArmatureBg, "RongYSC_B2", mHonorTableView, 0);
		mHonorTableView->setScale(0.0f);
		mHonorTableView->runAction(ScaleTo::create(0.25f, 1.0));
		changeBone(mArmatureBg, "RongYSC_guangbia", closeBtn, 0);
		changeBone(mArmatureBg, "RongYSC_RYB", honorNode, 0);
		changeBone(mArmatureBg, "RongYSC_anniubi", rightBtn, 0);
		changeBone(mArmatureBg, "RongYSC_anniua", leftBtnNode, 0);
		
		setPopType(E_POP_TYPE_NONE);
		ret = true;
	} while (0);

	return ret;
}

void HonorShopDialog::onShow()
{
	ActionWindow::onShow();
	sgHonorShopControl()->try2SyncExchangeInfo();
	//refreshShop();
}

void HonorShopDialog::updateItem(int itemId)
{
	
}

void HonorShopDialog::refreshShop()
{
	mHonorTableView->reloadData();
	// refresh honorcount
	if (mHonorCountLabel){
		mHonorCountLabel->setString(StringUtils::format("%d", sgUserObj()->getPersonalInfo()->honorcount()));//sgUserObj()->getHonorCount()));
	}
}

bool HonorShopDialog::tableViewMove(bool isRigth)
{
	int lMoveX = 0;
	bool lIsCanMove = false;
	float curPosX = mHonorTableView->getContentOffset().x;
	
	int max_page_count = getMaxPageCount();
	int cur_page = getCurPage(curPosX);

	//CCLOG("[trace]   tableViewMove =========== cur_page = %d", cur_page);

	if (isRigth)
	{
		if (cur_page < max_page_count)
		{
			++cur_page;
			lIsCanMove = true;
		}
	}
	else
	{
		if (cur_page > 1)
		{
			--cur_page;
			lIsCanMove = true;
		}
	}

	if (lIsCanMove)
	{
		mHonorTableView->setContentOffsetInDuration(Vec2(-(cur_page - 1)*TABLE_WIDTH,mHonorTableView->getContentOffset().y), 0.25);
	}
	else
	{
		float distance = -ITEM_WIDTH / 2;
		if (!isRigth) {
			distance = - distance;
		}

		mHonorTableView->setContentOffsetInDuration2(Vec2(-(cur_page - 1)*TABLE_WIDTH, mHonorTableView->getContentOffset().y), 0.25, distance, 0.25);
	}

	//CCLOG("[trace] ActLoginLayout::tableViewMove ContentOffeset.x = %f", curPosX);
	return lIsCanMove;
}

int	HonorShopDialog::getMaxPageCount()
{
	int item_count = sgHonorShopControl()->getShopDataCount();
	int max_page = item_count / PAGE_DATA_COUNT;

	int last_page_count = item_count % PAGE_DATA_COUNT;
	if (last_page_count > 0) {
		++max_page;
	}

	return max_page;
}

int	HonorShopDialog::getCurPage(float posX)
{
	int cur_page = fabs(posX) / TABLE_WIDTH + 1;

	return cur_page;
}


Size HonorShopDialog::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	return Size(ITEM_WIDTH, TABLE_HEIGHT);
}

ssize_t HonorShopDialog::numberOfCellsInTableView(TableView *table)
{
	return sgHonorShopControl()->getShopDataCount();
}

TableViewCell* HonorShopDialog::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->dequeueCell();
	if (!cell)
	{
		cell = HonorShopItem::create();
	}

	HonorShopItem* itemCell = dynamic_cast<HonorShopItem*>(cell);
	if (itemCell) {
		itemCell->updateItem(sgHonorShopControl()->getItemIdByIndex(idx));
		itemCell->setAnchorPoint(ccp(0.5, 0.5));
	}

	return itemCell;
}

