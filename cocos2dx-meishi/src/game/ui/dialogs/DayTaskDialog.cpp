#include "DayTaskDialog.h"
#include "game/daytask/DayTaskControl.h"
#include "game/daytask/DayTaskCell.h"
#include "core/SoundManager.h"
#include "core/MessageManager.h"

#define MISSION_LIST_HEIGH 460
#define MISSION_LIST_WIDTH 960
#define MISSION_CELL_HEIGH 150

DayTaskDialog::DayTaskDialog()
	:mTopShawdow(nullptr),
	mDownShawdow(nullptr),
	mTaskTableView(nullptr),
	mDialogArmature(nullptr),
	mListOriginOffSetX(-590)
{
	//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_Mrbz.png", "fla_Mrbz.plist", "fla_Mrbz.xml");
}

DayTaskDialog::~DayTaskDialog()
{
	sgDayTaskControl()->setDialog(nullptr);
	//cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_Mrbz.xml");
}


void DayTaskDialog::onMenuItemAction(std::string cName, cocos2d::Ref*)
{
	if (cName == "onBack")
	{
		//mTaskTableView->setVisible(false);
		if (mDownShawdow) {
			mDownShawdow->setVisible(false);
		}
		this->setShow(false);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
	}
}

bool DayTaskDialog::init()
{
	bool flag = false;
	do {
		mRoot = CSLoader::getInstance()->createNode("DayTaskNode.csb");
		CC_BREAK_IF(!mRoot);
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);
		sgDayTaskControl()->setDialog(this);

		setAnimation("fla_Mrbz");

		mTopShawdow = dynamic_cast<ImageView*>(mRoot->getChildByName("TopShaw"));
		auto biSprite = dynamic_cast<Sprite*>(mRoot->getChildByName("BiSprite"));
		auto closeBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("BackButton"));
		mDownShawdow = dynamic_cast<ImageView*>(mRoot->getChildByName("DownShaw"));
		changeBone(mArmatureBg, "Mrbz_guanbi", closeBtn,0);
		changeBone(mArmatureBg, "Mrbz_bi", biSprite,  0);
		changeBone(mArmatureBg, "Mrbz_zedang", mDownShawdow, 0);
		changeBone(mArmatureBg, "Mrbz_zedang1", mTopShawdow, 0);

		// 调整位置 
		mTopShawdow->setRotation(360);
		mTopShawdow->setPosition(Vec2(mTopShawdow->getPositionX(), mTopShawdow->getPositionY() + 32));
		// end 

		mTaskTableView = TableView::create(this, Size(MISSION_LIST_WIDTH, MISSION_LIST_HEIGH));
		mTaskTableView->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
		mTaskTableView->setAnchorPoint(Vec2(0.5f, 0.5f));
		mTaskTableView->setDelegate(this);
		mTaskTableView->setSwallowsTouches(true);
		mTaskTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
		mTaskTableView->setPosition(Vec2(-(MISSION_LIST_WIDTH / 2), -(MISSION_LIST_HEIGH / 2)));
		mCenter->addChild(mTaskTableView);

		changeBone(mArmatureBg, "Mrbz_MR", mTaskTableView, 0);
		//mTaskTableView->setVisible(false);

		mListOriginOffSetX = -((sgDayTaskControl()->getAllMissionCount() - 3) * MISSION_CELL_HEIGH - 10);
		CCLOG("[trace mission]  mListOriginOffsetX: %d", mListOriginOffSetX);

		setPopType(E_POP_TYPE_NONE);

		flag = true;
	} while (0);

	return flag;
}

void DayTaskDialog::onShow()
{
	Window::onShow();
	//CCLOG("      DayTaskDialog .............. on show ............... ");
	sgDayTaskControl()->trySyncMission();
	updateMissions();

	scheduleUpdate();
	//mTaskTableView->setVisible(false);
	if (mDownShawdow) {
		mDownShawdow->setVisible(false);
	}
}

void DayTaskDialog::updateCell()
{

}

void DayTaskDialog::updateMissions(bool showAction)
{
	if (mTaskTableView) {
		mTaskTableView->reloadData();
		//if (showAction) {
		//	mTaskTableView->setContentOffset(Vec2(0, mListOriginOffSetX-MISSION_CELL_HEIGH));
		//	mTaskTableView->setContentOffsetInDuration(Vec2(0, mListOriginOffSetX), 0.2f);
		//}
	}
}

void DayTaskDialog::update(float dt)
{
	//CCLOG("  DayTaskDialog -------------------- update");
	if (mTopShawdow && mTaskTableView){
		Vec2 offset = mTaskTableView->getContentOffset();
		if (offset.y > mListOriginOffSetX) {
			mTopShawdow->setVisible(true);
		}else{	
			mTopShawdow->setVisible(false);		
		}
	}
}

// tableViewDataresource
TableViewCell* DayTaskDialog::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->dequeueCell();
	if (!cell) {
		cell = DayTaskCell::create();
	}
	dynamic_cast<DayTaskCell*>(cell)->updateCell(idx);

	return cell;
}

ssize_t DayTaskDialog::numberOfCellsInTableView(TableView *table)
{
	return sgDayTaskControl()->getAllMissionCount();
}

Size DayTaskDialog::cellSizeForTable(TableView *table)
{
	return Size(MISSION_LIST_WIDTH-10, MISSION_CELL_HEIGH);
}

// tableView Delegate
void DayTaskDialog::tableCellTouched(TableView* table, TableViewCell* cell)
{

}

void DayTaskDialog::aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{

}

void DayTaskDialog::onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex)
{
	//CCLOG("[trace mission update frame]   bone->getName()== %s , evt = %s, originFrameIndex: %d,  currentFrameIndex: %d",  bone->getName().c_str(), evt.c_str(), originFrameIndex, currentFrameIndex);
	if (evt == "atk" && originFrameIndex == 8) {
		//CCLOG("[trace mission action]  run show list ");
		//mTaskTableView->setContentOffset(Vec2(0, mListOriginOffSetX - 3 * MISSION_CELL_HEIGH));
		//mTaskTableView->setContentOffsetInDuration2(Vec2(0, mListOriginOffSetX), 0.2f, MISSION_CELL_HEIGH/2, 0.2f);
		//mTaskTableView->setVisible(true);
		if (mDownShawdow) {
			mDownShawdow->setVisible(true);
		}
	}
}
