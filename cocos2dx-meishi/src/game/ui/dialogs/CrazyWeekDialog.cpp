#include "CrazyWeekDialog.h"
#include "SoundManager.h"
#include "data/TableManager.h"
#include "game/act/ActCrazyWeekControl.h"
#include "core/MessageManager.h"

CrazyWeekDialog::CrazyWeekDialog()
	:mTimeLabel(nullptr),
	mCrazyPointLabel(nullptr),
	mExtraHonourPointLabel(nullptr),
	mConditionLabel(nullptr),
	mSubConditionLabel(nullptr),
	mOkSprite1(nullptr),
	mOkSprite2(nullptr),
	mDoneSprite(nullptr),
	mSelectColor(Color4B(0, 0, 0, 0)),
	mNormalColor(Color4B(0, 0, 0, 0))
{
	sgMessageManager()->regisiterMessageHandler(EMT_CRAZY_WEEK, this);
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_KHZ.png", "fla_KHZ.plist", "fla_KHZ.xml");
}

CrazyWeekDialog::~CrazyWeekDialog()
{
	sgMessageManager()->removeMessageHandler(this);
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_KHZ.xml");
}

void CrazyWeekDialog::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_CRAZY_WEEK) {
		MessageCrazyWeek* crazyWeekMsg = dynamic_cast<MessageCrazyWeek*>(message);
		if (crazyWeekMsg) {
			int cmd = crazyWeekMsg->getCmd();
			switch (cmd) {
			case 1:
				updateCrzayInfo();
				break;
			case 2:
				break;
			default:
				break;
			}
		}
	}
}

bool CrazyWeekDialog::init()
{
	bool ret = false;
	do {
		mRoot = CSLoader::getInstance()->createNode("ui/CrazyWeekDialog.csb");
		CC_BREAK_IF(!mRoot);
		mRoot->setAnchorPoint(Vec2(0.5,0.5));
		this->addChild(mRoot);
		onLocateClickCallback(mRoot);

		setAnimation("fla_KHZ");

		int currDay = sgActCrazyWeekControl()->getCurrday();
		const CrazyWeekInfo *info = sgTableManager()->getCrazyWeekInfoById(currDay);
		//CCLOG("[jim]  info id == %d", info->id);
		//CCLOG("[jim]  info condition == %s", info->condition.c_str());
		//CCLOG("[jim]  info subCondition == %s", info->subCondition.c_str());

		auto closeBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("CloseBtn"));
		changeBone(mArmatureBg, "KHZ_xzc", closeBtn, 0);
		auto messageNode = dynamic_cast<Node*>(mRoot->getChildByName("MessageNode"));
		changeBone(mArmatureBg, "KHZ_xinxi", messageNode, 0);
		auto item01 = dynamic_cast<Node*>(mRoot->getChildByName("Item01"));
		changeBone(mArmatureBg, "KHZ_daojua", item01, 0);
		auto item02 = dynamic_cast<Node*>(mRoot->getChildByName("Item01_0"));
		changeBone(mArmatureBg, "KHZ_daojub", item02, 0);
		auto item03 = dynamic_cast<Node*>(mRoot->getChildByName("Item01_1"));
		changeBone(mArmatureBg, "KHZ_daojuc", item03, 0);

		mConditionLabel = dynamic_cast<cocos2d::ui::Text*>(messageNode->getChildByName("ConditionLabel"));
		//mConditionLabel->setText(info->condition);
		mSelectColor = mConditionLabel->getTextColor();
		mSubConditionLabel = dynamic_cast<cocos2d::ui::Text*>(messageNode->getChildByName("SubConditionLabel"));
		//mSubConditionLabel->setText(info->subCondition);
		mNormalColor = mSubConditionLabel->getTextColor();
		mConditionLabel->setTextColor(mNormalColor);

		mTimeLabel = dynamic_cast<cocos2d::ui::Text*>(messageNode->getChildByName("TimeLabel"));
		mExtraHonourPointLabel = dynamic_cast<cocos2d::ui::Text*>(messageNode->getChildByName("HonourPointLabel"));
		//mExtraHonourPointLabel->setg
		mCrazyPointLabel = dynamic_cast<cocos2d::ui::Text*>(messageNode->getChildByName("CrazyPointLabel"));

		mOkSprite1 = dynamic_cast<Sprite*>(messageNode->getChildByName("OKSprite1"));
		mOkSprite1->setVisible(false);
		mOkSprite2 = dynamic_cast<Sprite*>(messageNode->getChildByName("OKSprite2"));
		mOkSprite2->setVisible(false);
		mDoneSprite = dynamic_cast<Sprite*>(messageNode->getChildByName("Done"));
		if (mDoneSprite) {
			mDoneSprite->setVisible(false);
		}


		mItemNode[0] = dynamic_cast<Node*>(mRoot->getChildByName("Item01_0"));
		mItemNode[1] = dynamic_cast<Node*>(mRoot->getChildByName("Item01_1"));
		mItemNode[2] = dynamic_cast<Node*>(mRoot->getChildByName("Item01"));

		setPopType(E_POP_TYPE_NONE);
		ret = true;
	} while (0);

	return ret;
}

void CrazyWeekDialog::onShow()
{
	ActionWindow::onShow();
	updateCrzayInfo();
}

void CrazyWeekDialog::onClose()
{
	ActionWindow::onClose();
}

void CrazyWeekDialog::onMenuItemAction(std::string cbName, cocos2d::Ref* sSender)
{
	if (cbName == "onClose") {
		this->setShow(false);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
	}
	else if (cbName == "onReward"){
		CCLOG("[trace CrazyWeekDialog]  onReward on clicked ..");
		sgActCrazyWeekControl()->sendGetRewardReq();
	}
}

void CrazyWeekDialog::updateCrzayInfo()
{
	// test
	//auto conditionDes = dynamic_cast<cocos2d::ui::Text*>(mRoot->getChildByName("ConditionLabel"));
	//conditionDes->setText("  honour point == " + sgActCrazyWeekControl()->getExtraHonourPoint());
	//auto subConditionDes = dynamic_cast<cocos2d::ui::Text*>(mRoot->getChildByName("SubConditionLabel"));
	//subConditionDes->setText("   Crazy point ==== " + sgActCrazyWeekControl()->getCrazyPoint());
	auto messageNode = dynamic_cast<Node*>(mRoot->getChildByName("MessageNode"));
	auto rewardBtn = dynamic_cast<ui::Button*>(messageNode->getChildByName("RewardBtn"));

	mTimeLabel->setText(sgActCrazyWeekControl()->getTimeString());

	// reward btn
	int state = sgActCrazyWeekControl()->getRewardState();
	switch (state) {
	case 0: // 不可领
		rewardBtn->setEnabled(false);
		rewardBtn->setBright(false);
		rewardBtn->getChildByName("GetSprite_0")->setVisible(true);
		rewardBtn->getChildByName("GetSprite")->setVisible(false);
		rewardBtn->getChildByName("GetSprite_0_0")->setVisible(false);
		break;
	case 1: // 可领
		rewardBtn->setEnabled(true);
		rewardBtn->setBright(true);
		rewardBtn->getChildByName("GetSprite_0")->setVisible(false);
		rewardBtn->getChildByName("GetSprite")->setVisible(true);
		rewardBtn->getChildByName("GetSprite_0_0")->setVisible(false);
		break;
	case 2: // 已领
		rewardBtn->setEnabled(false);
		rewardBtn->setBright(false);
		rewardBtn->getChildByName("GetSprite_0")->setVisible(false);
		rewardBtn->getChildByName("GetSprite")->setVisible(false);
		rewardBtn->getChildByName("GetSprite_0_0")->setVisible(true);
		break;
	default:
		break;
	}

	// condition
	int currDay = sgActCrazyWeekControl()->getCurrday();
	const CrazyWeekInfo *info = sgTableManager()->getCrazyWeekInfoById(currDay);

	if (sgActCrazyWeekControl()->getConditionCompleted() >= info->pvpWinCount) {
		mConditionLabel->setTextColor(mSelectColor);
		std::string str0 = StringUtils::format("%d", info->pvpWinCount);
		std::string displayerstr0 = StringUtils::format((info->condition).c_str(), str0.c_str()) + sgTextHelper(TEXT_CRAZYWEEK_DONE);
		mConditionLabel->setText(displayerstr0);
		mOkSprite1->setVisible(true);
	}
	else {
		mConditionLabel->setTextColor(mNormalColor);
		std::string str = StringUtils::format(" %d/%d ", sgActCrazyWeekControl()->getConditionCompleted(), info->pvpWinCount);
		std::string displayStr = StringUtils::format((info->condition).c_str(), str.c_str());
		mConditionLabel->setText(displayStr);
		mOkSprite1->setVisible(false);
	}
	if (sgActCrazyWeekControl()->getSubConditionCompleted() >= info->pvpCount) {
		mSubConditionLabel->setTextColor(mSelectColor);
		std::string str0 = StringUtils::format("%d", info->pvpCount);
		std::string displayStr0 = StringUtils::format((info->subCondition).c_str(), str0.c_str()) + sgTextHelper(TEXT_CRAZYWEEK_DONE);
		mSubConditionLabel->setText(displayStr0);
		mOkSprite2->setVisible(true);
	}
	else {
		mSubConditionLabel->setTextColor(mNormalColor);
		std::string str = StringUtils::format(" %d/%d ", sgActCrazyWeekControl()->getSubConditionCompleted(), info->pvpCount);
		std::string displayStr1 = StringUtils::format((info->subCondition).c_str(), str.c_str());
		mSubConditionLabel->setText(displayStr1);
		mOkSprite2->setVisible(false);
	}

	if (mDoneSprite) {
		mDoneSprite->setVisible(false);
	}

	if ((mOkSprite2->isVisible() || mOkSprite1->isVisible()) && mDoneSprite) {
		mDoneSprite->setVisible(true);
	}

	if (mOkSprite1->isVisible())
	{
		mSubConditionLabel->setTextColor(Color4B::GRAY);
	}
	else if (mOkSprite2->isVisible()) {
		mConditionLabel->setTextColor(Color4B::GRAY);
	}

	int piont = sgActCrazyWeekControl()->getCrazyPoint();
	mCrazyPointLabel->setText(__String::createWithFormat("%d", piont)->getCString());
	int honourPoint = sgActCrazyWeekControl()->getExtraHonourPoint();
	mExtraHonourPointLabel->setText(__String::createWithFormat("%d", honourPoint)->getCString());

	//CCLOG("[jim updateCrzayInfo] mExtraHonourPoint == %d", honourPoint);
	//CCLOG("[jim updateCrzayInfo] Day == %d", currDay);
	//CCLOG("[jim updateCrzayInfo] mCrazyPoint == %d", piont);

	// reward item
	const GiftAward *gift = sgTableManager()->getGiftAward(info->rewardId);
	if (!gift) return;

	//Node* itemNode[3];
	//mItemNode[0] = dynamic_cast<Node*>(mRoot->getChildByName("Item01"));
	mItemNode[0]->setVisible(false);
	///mItemNode[1] = dynamic_cast<Node*>(mRoot->getChildByName("Item01_0"));
	mItemNode[1]->setVisible(false);
	//mItemNode[2] = dynamic_cast<Node*>(mRoot->getChildByName("Item01_1"));
	mItemNode[2]->setVisible(false);

	do {
		if (gift->mGiftData.size() == 0)
		{
			break;
		}
		int pos = 0;
		for (int i = 0; i < gift->mGiftData.size(); i++)
		{
			GiftInfo lGiftInfo = gift->mGiftData.at(i);
			const GoodsInfo* lGoodsInfo = sgTableManager()->getGoodsInfo(lGiftInfo.mType, lGiftInfo.mId);
			if (!lGoodsInfo)
			{
				break;
			}
			showItemInfo(mItemNode[pos], lGoodsInfo->mIcon, lGiftInfo.mNumber);

			if (++pos > 2) {
				break;
			}
		}

		/*
		int  pos = 0;
		// 奖励钻石
		if (gift->gold > 0) {
			std::string iconName = "ui/general/general_diamonds.png";
			showItemInfo(mItemNode[pos], iconName, gift->gold);
			++pos;
		}
		// 体力
		if (gift->manual > 0) {
			std::string iconName = "ui/general/general_Energy.png";
			showItemInfo(mItemNode[pos], iconName, gift->manual);
			++pos;
		}

		if (pos > 2) break;
		// 精力
		if (gift->energy > 0) {
			std::string iconName = "ui/general/general_vigour.png";
			showItemInfo(mItemNode[pos], iconName, gift->energy);
			++pos;
		}

		if (pos > 2) break;
		// 开房券
		if (gift->roomcard > 0) {
			std::string iconName = "ui/Icon/Icon_roomticket_a_small.png";
			showItemInfo(mItemNode[pos], iconName, gift->roomcard);
			++pos;
		}

		if (pos > 2) break;
		// 荣誉点
		if (gift->mHonor > 0) {
			std::string iconName = "ui/general/general_honor_icon_big.png";
			showItemInfo(mItemNode[pos], iconName, gift->mHonor);
			++pos;
		}

		if (pos > 2) break;
		// 道具 and 符文
		const std::map<int, int > &itemMap = gift->itemMap;
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

				showItemInfo(mItemNode[pos], pPropInfo->icon, count, type);
				++pos;
				if (pos > 2) break;
			}
			else
			{
				if (itemId >= A_GLYPHSTUFF){
					const GlyphStuffData *glyphData = sgTableManager()->getGlyphStuffData(itemId);
					if (glyphData) {
						showItemInfo(mItemNode[pos], glyphData->mStuffIcon, count);
						++pos;
						if (pos > 2) break;
					}
				}
			}
		}
		*/
	} while (0);
}

void CrazyWeekDialog::showItemInfo(Node* itemNode, std::string iconName, int count, int itemType)
{
	if (itemNode) {
		itemNode->setVisible(true);
		auto countLabel = dynamic_cast<ui::Text*>(itemNode->getChildByName("Value01"));
		auto icon = dynamic_cast<ui::ImageView*>(itemNode->getChildByName("Icon01"));

		if (countLabel && icon) {
			countLabel->setText(StringUtils::format("%d", count));
			icon->loadTexture(iconName, ui::TextureResType::PLIST);
		}
	}
}
