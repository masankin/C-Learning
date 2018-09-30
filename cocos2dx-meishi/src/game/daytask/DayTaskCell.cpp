#include "DayTaskCell.h"
#include "utils/Comm.h"
#include "core/SoundManager.h"

DayTaskCell::DayTaskCell()
	:mRoot(nullptr), 
	mMId(0), 
	mPageId(0), 
	mGetBtn(nullptr), 
	mGoBtn(nullptr),
	mTitleLabel(nullptr),
	mMissionDesLabel(nullptr),
	mMiddlePos(Vec2::ZERO),
	mFirstPos(Vec2::ZERO)
{
	mItems[4] = {nullptr,};
	mPepperSprites[5] = {nullptr,};
}

DayTaskCell::~DayTaskCell()
{

}

bool DayTaskCell::init()
{
	bool ret = false;
	do {
		mRoot = CSLoader::getInstance()->createNode("ui/DayTaskCell.csb");
		auto cellBg = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("Image_2"));	
		if (cellBg) {
			auto cellSize = cellBg->getContentSize();
			mRoot->setPosition(Vec2(cellSize.width / 2 + 5, cellSize.height / 2 + 5));
		}
		else {
			mRoot->setPosition(Vec2(480, 75));
		}

		mGetBtn = dynamic_cast <ui::Button*>(mRoot->getChildByName("GetButton"));
		mGoBtn = dynamic_cast <ui::Button*>(mRoot->getChildByName("GoButton"));

		std::string name[] = { "item01", "item02", "item03", "item04" };
		for (int i = 0; i < 4; i++) {
			mItems[i] = dynamic_cast<Node*>(mRoot->getChildByName(name[i]));
			if (mItems[i]) {
				mItems[i]->setVisible(false);
				auto touch_panel = dynamic_cast<ui::Layout*>(mItems[i]->getChildByName("Panel_1"));
				touch_panel->setEnabled(false);
			}
		}
		if (mItems[0] != nullptr) {
			mFirstPos = mItems[0]->getPosition();
		}
		std::string forkname[] = { "fork1", "fork2", "fork3", "fork4", "fork5" };
		for (int i = 0; i < 5; i++) {
			mPepperSprites[i] = dynamic_cast<Sprite*>(mRoot->getChildByName(forkname[i]));
			mPepperSprites[i]->setVisible(false);
		}

		mTitleLabel = dynamic_cast<ui::Text*> (mRoot->getChildByName("LabelTitle"));
		mMissionDesLabel = dynamic_cast<ui::Text*> (mRoot->getChildByName("LabelDesc"));

		mMiddlePos = dynamic_cast<Node*>(mRoot->getChildByName("SinglePos"))->getPosition();

		this->addChild(mRoot);

		ret = true;
	} while (0);

	return ret;
}

void DayTaskCell::onCellClick(Ref* sender)
{
	cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(sender);
	if (!widget)
		return;
	std::string cbName = widget->getCallbackName();

	if (cbName == "onGo")
	{
		CCLOG("[trace]   task cell GO on Click ........................... ");
		// 界面跳转
		sgDayTaskControl()->goMission(mPageId);
	}
	else if (cbName == "onGet")
	{
		CCLOG("[trace]  task cell Get on Click ...........................missionId = %d ", mMId);
		sgDayTaskControl()->getProxy()->sendMissionComplete(mMId);
	}
}

void DayTaskCell::updateCell(int idx)
{
	do {
		mGetBtn->addClickEventListener(CC_CALLBACK_1(DayTaskCell::onCellClick, this));
		mGoBtn->addClickEventListener(CC_CALLBACK_1(DayTaskCell::onCellClick, this));

		for (int i = 0; i < 4; i++) {
			Node* item = mItems[i];
			if (item) {
				item->setVisible(false);
				auto touch_panel = dynamic_cast<ui::Layout*>(item->getChildByName("Panel_1"));
				touch_panel->setEnabled(false);
			}
		}

		if (mItems[0] != nullptr){
			mItems[0]->setPosition(mFirstPos);
		}

		mGetBtn->setVisible(false);
		mGoBtn->setVisible(false);
		mGetBtn->setEnabled(false);
		mGoBtn->setEnabled(false);
		mGetBtn->setBright(true);

		int missionId = sgDayTaskControl()->getMissionIdByIndex(idx + 1);
		DayTaskData *missionData = sgTableManager()->getDayTaskDataById(missionId);
		if (!missionData)
			break;

		showPeppers(missionData->getHardLevel());

		if (mTitleLabel) {
			mTitleLabel->setString(missionData->getTitle());
		}

		if (mMissionDesLabel) {
			std::string des = missionData->getDesc();
			if (missionData->getID() == 1){  // 月卡
				mMissionDesLabel->setString(StringUtils::format(des.c_str(), sgDayTaskControl()->getCardDaysLeft()));
			}
			else {
				mMissionDesLabel->setString(des);
			}
		}

		mMId = missionData->getID();
		mPageId = missionData->getID();

		// 刷新按钮
		DayMission *cacheMission = sgDayTaskControl()->getMissionById(missionData->getID());
		if (cacheMission) {
			refreshBtn(cacheMission);
		}

		auto giftData = sgTableManager()->getGiftAward(missionData->getGiftId());
		if (!giftData)
			break;

		int  pos = 0;
		// 奖励钻石
		if (giftData->gold > 0) {
			std::string iconName = "ui/general/general_diamonds.png";
			showItemInfo(mItems[pos], iconName, giftData->gold);
			++pos;
		}
		// 体力
		if (giftData->manual > 0) {
			std::string iconName = "ui/general/general_Energy.png";
			showItemInfo(mItems[pos], iconName, giftData->manual);
			++pos;
		}

		if (pos > 2) break;
		// 精力
		if (giftData->energy > 0) {
			std::string iconName = "ui/general/general_vigour.png";
			showItemInfo(mItems[pos],iconName, giftData->energy);
			++pos;
		}

		if (pos > 2) break;
		// 开房券
		if (giftData->roomcard > 0) {
			std::string iconName = "ui/Icon/Icon_roomticket_a_small.png";
			showItemInfo(mItems[pos],iconName, giftData->roomcard);
			++pos;
		}

		if (pos > 2) break;
		// 荣誉点
		if (giftData->mHonor > 0) {
			std::string iconName = "ui/general/general_honor_icon_big.png";
			showItemInfo(mItems[pos], iconName, giftData->mHonor);
			++pos;
		}

		if (pos > 2) break;
		// 道具 and 符文
		const std::map<int, int > &itemMap = giftData->itemMap;
		for (auto it = itemMap.begin(); it != itemMap.end(); ++it) {
			int itemId = it->first;
			int count = it->second;

			const PropInfo *pPropInfo = sgTableManager()->getPropInfoById(itemId);
			if (pPropInfo)
			{
				int type = 0;
				if (pPropInfo->isPve == 1)
					type = 1;
				else
					type = 2;

				showItemInfo(mItems[pos], pPropInfo->icon, count, type);
				++pos;
				if (pos > 2) break;
			}
			else
			{
				if (itemId >= A_GLYPHSTUFF){
					const GlyphStuffData *glyphData = sgTableManager()->getGlyphStuffData(itemId);
					if (glyphData) {
						showItemInfo(mItems[pos], glyphData->mStuffIcon, count);
						++pos;
						if (pos > 2) break;
					}
				}
			}
		}

		// 只有一个物品时居中
		if (pos == 1) {
			mItems[0]->setPosition(mMiddlePos);
		}
	} while (0);
}

std::string DayTaskCell::getStringNum(int num)
{
	//if (num < 0 || num > 10) {
	//	return "";
	//}

	//switch (num) {
	//case 0:
	//	break;
	//case 1:
	//	return "一";
	//case 2:
	//	return "二";
	//case 3:
	//	return "三";
	//}
	return "";
}

void DayTaskCell::showItemInfo(Node* itemNode, std::string iconName, int count, int itemType)
{
	if (itemNode) {
		itemNode->setVisible(true);
		auto countLabel = dynamic_cast<ui::Text*>(itemNode->getChildByName("Text_1"));
		auto icon = dynamic_cast<ui::ImageView*>(itemNode->getChildByName("Image_2")); 
		if (itemType > 0)
		{
			std::string frameBg = "ui/general/general_kuang_3.png";
			if (itemType == 1)
				frameBg = "ui/general/general_kuang_4.png";
			dynamic_cast<ui::ImageView*>(itemNode->getChildByName("Image_1"))->loadTexture(frameBg, ui::Widget::TextureResType::PLIST);
		} 
		if (countLabel && icon) {
			countLabel->setText(StringUtils::format("%d", count));
			icon->loadTexture(iconName, ui::TextureResType::PLIST);
		}
	}
}

void DayTaskCell::refreshBtn(const DayMission *mission_data)
{
	if (mission_data->mIsFinished) {
		mGetBtn->setVisible(true);
		auto getit = dynamic_cast <Sprite*>(mGetBtn->getChildByName("Sprite_4"));
		auto nogetit = dynamic_cast <Sprite*>(mGetBtn->getChildByName("Sprite_3"));
		nogetit->setVisible(false);
		getit->setVisible(false);
		if (mission_data->mIsGet) {
			getit->setVisible(true);
			mGetBtn->setEnabled(false);
			mGetBtn->setBright(false);
		}
		else {
			nogetit->setVisible(true);
			mGetBtn->setEnabled(true);
		}
	}
	else {
		mGoBtn->setVisible(true);
		mGoBtn->setEnabled(true);
	}
}

void DayTaskCell::showPeppers(int count)
{
	for (int i = 0; i < 5; ++i){
		if (mPepperSprites[i]) {
			if (i < count) {
				mPepperSprites[i]->setVisible(true);
			}
			else {
				mPepperSprites[i]->setVisible(false);
			}
		}
	}
}
