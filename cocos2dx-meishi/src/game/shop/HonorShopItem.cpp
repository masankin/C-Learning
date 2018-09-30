#include "HonorShopItem.h"
#include "game/data/TableManager.h"
#include "game/shop/HonorShopControl.h"
#include "game/ui/system/ActionTip.h"
#include "core/TimeCalculator.h"
#include "game/ui/dialogs/GenerlDialog.h"

#define TIPS_TAG 201512
#define HONOR_ITEM_KAY "HONOR_ITEM_KAY_%d"

HonorShopItem::HonorShopItem()
:mRoot(nullptr),
mExchangeId(1),
mItemNode(nullptr),
mItemIcon(nullptr),
mItemNumLabel(nullptr),
mNameLabel(nullptr),
mHonorLabel(nullptr),
mLimitLabel(nullptr),
mTimeLabel(nullptr),
mLimitBg(nullptr),
mAllCountlabel(nullptr),
mExchangeBtn(nullptr),
mAbleBtnSprite(nullptr),
mUnableBtnSprite(nullptr),
mItemType(UserObj::CHAT),
mItemId(0),
mItemData(nullptr),
mHonorColor(Color3B::GRAY),
mItemCount(0)
{

}

HonorShopItem::~HonorShopItem()
{

}

bool HonorShopItem::init()
{
	bool ret = false;
	do {
		if (!TableViewCell::init()) break;

		mRoot = CSLoader::getInstance()->createNode("ui/HonorShopItem.csb");
		if (!mRoot) break;
		mRoot->setAnchorPoint(Vec2(0.0, 0.0));
		mRoot->setPosition(Vec2(145, 184));
		addChild(mRoot);

		auto exchangeBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("ExchangeBtn"));
		exchangeBtn->addClickEventListener(CC_CALLBACK_1(HonorShopItem::onExchangeCallback, this));

		mItemNode = dynamic_cast<cocos2d::Node*>(mRoot->getChildByName("ItemNode"));
		mItemNode->getChildByName("Image_1")->setVisible(false);
		mItemIcon = dynamic_cast<ui::ImageView*>(mItemNode->getChildByName("Image_2"));
		mItemNumLabel = dynamic_cast<ui::Text*>(mItemNode->getChildByName("Text_1"));
		mNameLabel = dynamic_cast<ui::Text*>(mRoot->getChildByName("NameLabel"));
		mHonorLabel = dynamic_cast<ui::Text*>(mRoot->getChildByName("HonorLabel"));
		mHonorColor = mHonorLabel->getColor();
		mLimitLabel = dynamic_cast<ui::Text*>(mRoot->getChildByName("LimitLabel"));
		mTimeLabel = dynamic_cast<ui::Text*>(mRoot->getChildByName("TimeLabel"));
		mAllCountlabel = dynamic_cast<ui::Text*>(mRoot->getChildByName("AllCountLabel"));
		mLimitBg = dynamic_cast<cocos2d::Sprite*>(mRoot->getChildByName("LimitBg"));
		mExchangeBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("ExchangeBtn"));
		mAbleBtnSprite = dynamic_cast<cocos2d::Sprite*>(exchangeBtn->getChildByName("NormalSprite_a"));
		mUnableBtnSprite = dynamic_cast<cocos2d::Sprite*>(exchangeBtn->getChildByName("NormalSprite_b"));

		auto touchPanel = dynamic_cast<ui::Widget*>(mItemNode->getChildByName("Panel_1"));
		touchPanel->setTouchEnabled(false);

		mItemIcon->setContentSize(Size(180,180));
		mItemIcon->setTouchEnabled(true);
		mItemIcon->setEnabled(true);
		mItemIcon->addTouchEventListener(this, toucheventselector(HonorShopItem::onItemTouch));

		ret = true;
	} while (0);

	return ret;
}

void HonorShopItem::onItemTouch(Ref* ref, Widget::TouchEventType type)
{
	switch (type) {
	case Widget::TouchEventType::BEGAN:
		showItemDes();
		break;
	case Widget::TouchEventType::MOVED:
		break;
	case Widget::TouchEventType::ENDED:
		hideItemDes();
		break;
	case Widget::TouchEventType::CANCELED:
		hideItemDes();
		break;
	default:
		break;
	}
}

void HonorShopItem::onExchangeCallback(cocos2d::Ref* caller)
{
	//ui::Button* exchange = dynamic_cast<ui::Button*>(caller);
	//CCLOG(" onExchangeCallback........................%d ", mExchangeId);
	// 判断荣誉点是否足够 
	if (sgHonorShopControl()->checkHonorCountEnough(mExchangeId)) {
		// 体力精力上限判断
		if (checkPowerLimit()) {
			sgHonorShopControl()->sendExchangeItemReq(mExchangeId);
		}
	}
	else {
		// honorcount is not enough to exchange.
		CCLOG("[trace]  jim honorcount is not enough to exchange!");
		showTips();
	}
}

void HonorShopItem::showItemDes()
{
	std::string title_str = "";
	std::string des_str = "";
	std::string icon_str = "";

	if (mItemType == UserObj::STUFF) {
		// read stuff info.
		auto itemInfo = sgTableManager()->getGlyphStuffData(mItemId);
		if (itemInfo) {
			icon_str = itemInfo->mStuffIcon;
			title_str = itemInfo->mStuffName;
			des_str = itemInfo->mStuffDesc;
		}
	}
	else if (mItemType == UserObj::PT_ITEM){
		// read tools info.
		const PropInfo* propInfo = sgTableManager()->getPropInfoById(mItemId);
		if (propInfo) {
			icon_str = propInfo->icon;
			des_str = propInfo->explain1;
			title_str = propInfo->name;
		}
	}

	if (title_str.length() == 0 || des_str.length() == 0 || icon_str.length() == 0) {
		return;
	}

	auto director = CCDirector::getInstance();
	auto desNode = mRoot = CSLoader::getInstance()->createNode("ui/PropDesc.csb");
	auto win_size = director->getWinSize();
	desNode->setPosition(Vec2(win_size.width / 2, win_size.height / 20 * 13));
	desNode->setTag(TIPS_TAG);
	director->getRunningScene()->addChild(desNode);

	Sprite* itemIcon = dynamic_cast<Sprite*>(desNode->getChildByName("Icon_blb_small_2"));
	if (itemIcon) {
		itemIcon->setSpriteFrame(icon_str);

		if (mItemType == UserObj::PT_ITEM) {
			itemIcon->setScale(0.5);
		}
		else{
			itemIcon->setScale(0.7);
		}
	}

	ui::Text* title = dynamic_cast<ui::Text*>(desNode->getChildByName("Text_1"));
	if (title) {
		title->setString(title_str);
	}

	ui::Text* content = dynamic_cast<ui::Text*>(desNode->getChildByName("Text_2"));
	if (content) {
		content->setString(des_str);
	}
}

void HonorShopItem::hideItemDes()
{
	CCDirector::getInstance()->getRunningScene()->removeChildByTag(TIPS_TAG);
}

void HonorShopItem::showTips()
{
	auto t = this->getChildByTag(TIPS_TAG);
	if (t) {
		t->stopAllActions();
		t->removeFromParentAndCleanup(true);
	}
	Action* pAction = Sequence::create(
		MoveBy::create(.3f, Vec2(0, 50)),
		DelayTime::create(0.5f),
		Spawn::create(MoveBy::create(.2f, Vec2(0, 30)), FadeOut::create(.2f), NULL),
		NULL);
	ActionTip* tips = ActionTip::create(sgTextHelper(TEXT_HONORSHOP_NOTENOUGH), pAction);
	tips->setIsShowPic(false);
	tips->setPosition(Vec2(145, 184));
	tips->setFontSize(32);
	tips->setTag(TIPS_TAG);
	tips->run();
	this->addChild(tips);
}

void HonorShopItem::updateItem(int exchangeId)
{
	const ExchangeItem *itemInfo = sgTableManager()->getExchangeItem(exchangeId);

	if (!itemInfo) { 
		this->setVisible(false);
		return; 
	}
	this->setVisible(true);
	mExchangeId = exchangeId;

	const GiftAward* pGiftAward = sgTableManager()->getGiftAward(itemInfo->mGiftId);
	if (!pGiftAward) return;

	int rewardNum  = 0;
	std::string iconName = "ui/general/general_Energy.png";
	std::string itemName = "";
	if (pGiftAward->mGiftData.size() == 0)
	{
		return;
	}
	for (int i = 0; i < pGiftAward->mGiftData.size(); i++)
	{
		GiftInfo lGiftInfo = pGiftAward->mGiftData.at(i);
		const GoodsInfo* lGoodsInfo = sgTableManager()->getGoodsInfo(lGiftInfo.mType, lGiftInfo.mId);
		if (!lGoodsInfo) break;

		rewardNum = lGiftInfo.mNumber;
		//iconName = lGoodsInfo->mIcon;
		iconName = lGoodsInfo->mIconBig;
		itemName = lGoodsInfo->mName;

		mItemId = lGiftInfo.mId;
		mItemType = lGiftInfo.mType;
		mItemCount = lGiftInfo.mNumber;
	}

	// set item icon
	mItemIcon->loadTexture(iconName, TextureResType::PLIST);
	// set item count
	mItemNumLabel->setText(StringUtils::format("X%d", rewardNum));
	// set item name
	mNameLabel->setText(itemName);

	mHonorLabel->setText(StringUtils::format("%d", itemInfo->mNeedScore));
	mLimitLabel->setText(StringUtils::format(sgTextHelper(TEXT_HONORSHOP_LIMIT).c_str(), 0, itemInfo->mDailyCount));
	mTimeLabel->setString("");

	// set btn status
	mAbleBtnSprite->setVisible(false);
	mUnableBtnSprite->setVisible(false);
	mExchangeBtn->setEnabled(false);
	mExchangeBtn->setBright(false);

	bool checkHonorEnough = sgHonorShopControl()->checkHonorCountEnough(mExchangeId);

	if (checkHonorEnough) {
		mHonorLabel->enableOutline(Color4B(11, 95, 133, 255), 2);
	}
	else{
		mHonorLabel->enableOutline(Color4B(71, 71, 71, 255), 2);
	}

	// remove tips in case tips action does't finished.
	auto t = this->getChildByTag(TIPS_TAG);
	if (t) {
		t->stopAllActions();
		t->removeFromParentAndCleanup(true);
	}

	mItemData = sgHonorShopControl()->getShopDataById(exchangeId);
	// CCLOG("   [jim]   exchangeid == %d",  exchangeId);
	// item exchangeAble;
	if (mItemData) {
		// 全服数量
		// mAllCountlabel->setText(StringUtils::format("All:%d", data->getAllLeftCount()));
		// 限买数量
		mLimitLabel->setString(StringUtils::format(sgTextHelper(TEXT_HONORSHOP_LIMIT).c_str(), mItemData->getExchangeLeftCount(), itemInfo->mDailyCount));

		if (mItemData->getExchangeLeftCount() > 0 && mItemData->getRefreshTime() > 0 && checkHonorEnough) {
			mExchangeBtn->setEnabled(true);
			mExchangeBtn->setBright(true);
		}
		if (mItemData->getRefreshTime() > 0) {
			updateTime(1);
			this->schedule(schedule_selector(HonorShopItem::updateTime), 1.0f);
		}
	}
}

void HonorShopItem::updateTime(float dt)
{
	std::string time_key = StringUtils::format(HONOR_ITEM_KAY, mExchangeId);
	float time_left = sgTimeCalculator()->getTimeLeft(time_key);
	if (mItemData && time_left > 0) {
		mTimeLabel->setString(getTimeStrByTimeNum((int)time_left));
	}
	else{
		//this->unschedule(schedule_selector(HonorShopItem::updateTime));
		sgHonorShopControl()->removeItemById(mExchangeId);
	}
}

std::string	HonorShopItem::getTimeStrByTimeNum(int secs)
{
	int days = secs / (24 * 3600);
	int hours = secs % (24 * 3600) / 3600;
	int minute = secs % (24 * 3600) % 3600 / 60;
	int sec = secs % (24 * 3600) % 3600 % 60;

	//CCLOG("[trace]   days == %d, hours == %d, minuts == %d, sec == %d", days, hours, minute, sec);
	std::string time_str = "";
	if (days > 0) {
		time_str = StringUtils::format(sgTextHelper(TEXT_HONORSHOP_DAYS).c_str(), days);
	}
	else if (hours > 0) {
		time_str = StringUtils::format(sgTextHelper(TEXT_HONORSHOP_HOURS).c_str(), hours);
	}
	else if (minute > 0 || sec > 0) {
		time_str = StringUtils::format(sgTextHelper(TEXT_HONORSHOP_MINUTE).c_str(), minute, sec);
	}
	else{
		time_str = "";
	}

	return time_str;
}

bool HonorShopItem::checkPowerLimit()
{
	bool ret = true;
	do {
		switch (mItemType) {
		case UserObj::PT_PVEPOWER:
		{
			int pveNum = (int)sgUserObj()->getPersonalInfo()->power();
			if (mItemCount + pveNum > POWER_BUY_MAX + PVEPOWER_INIT_VALUE){
				GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS),
					__String::createWithFormat("%s%s", sgTextHelper(TEXT_ACT_TILI).c_str(), sgTextHelper(TEXT_BUY_POWER_MAX).c_str())->getCString());
				ret = false;
			}
			break;
		}
		case UserObj::PT_PVPPOWER:
		{
			int pvpNum = (int)sgUserObj()->getPersonalInfo()->energy();
			if (mItemCount + pvpNum > POWER_BUY_MAX + PVPPOWER_INIT_VALUE){
				GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS),
					__String::createWithFormat("%s%s", sgTextHelper(TEXT_ACT_JINGLI).c_str(), sgTextHelper(TEXT_BUY_POWER_MAX).c_str())->getCString());
				ret = false;
			}
			break;
		}
		default:
			;
		}
	} while (0);

	return ret;
}
