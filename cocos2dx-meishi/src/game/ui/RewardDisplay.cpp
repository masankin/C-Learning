#include "RewardDisplay.h"
#include "ui/UIHelper.h"
#include "data/TableManager.h"
#include "data/Tables.h"
#include "net/UserObj.h"

#include "cocostudio/ActionTimeline/CSLoader.h"


RewardDisplay::RewardDisplay() :mBaseTitle(nullptr), mPerfectTitle(nullptr)
{
	mNumList.clear();
	mNumList.clear();
}

RewardDisplay::~RewardDisplay()
{

}

RewardDisplay* RewardDisplay::create(int id, eRewardType type)
{
	RewardDisplay* pret = new RewardDisplay();
	if (pret && pret->init(id,type))
	{
		pret->autorelease();
		return pret;
	}
	CC_SAFE_RELEASE(pret);
	pret = nullptr;
	return NULL;
}

bool RewardDisplay::init(int id, eRewardType type)
{
	Node* mRoot = CSLoader::getInstance()->createNode("RewardDisplay.csb");
	mGiftId = id;
	mRewardType = type;

	if (!mRoot || mGiftId == 0 || mRewardType == NONE)
	{
		return false;
	}

	mRoot->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(mRoot);

	ui::Button* pBg = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_1"));
	mBaseTitle = mRoot->getChildByName("Node_BaseTitle");
	mPerfectTitle = mRoot->getChildByName("Node_PerfectTitle");
	for (int i = 1; i <= 3; i++)
	{
		std::string strName = StringUtils::format("RewardNode%d",i);
		Node* parent = mRoot->getChildByName(strName);
		Sprite* icon = dynamic_cast<Sprite*>(parent->getChildByName("Icon"));
		ui::Text* num = dynamic_cast<ui::Text*>(parent->getChildByName("NumLabel"));
		mNumList.push_back(num);
		mIconList.push_back(icon);
	}

	pBg->setSwallowTouches(false);
	pBg->addClickEventListener(CC_CALLBACK_1(RewardDisplay::onClose, this));
	updateUI(id, type);

	return true;
}

void RewardDisplay::updateUI(int id, eRewardType type)
{
	if (id == 0 || type == eRewardType::NONE)
		return;

	mGiftId = id;
	mRewardType = type;
	const GiftAward* pGiftInfo = sgTableManager()->getGiftAward(id);
	if (pGiftInfo == nullptr)
		return;

	for (int i = 0; i < mIconList.size(); i++)
	{
		Sprite* icon = mIconList.at(i);
		ui::Text* label = mNumList.at(i);
		icon->setVisible(false);
		label->setVisible(false);
	}

	int i = 0;
	for (std::vector<GiftInfo>::const_iterator it = (pGiftInfo->mGiftData).begin(); it != (pGiftInfo->mGiftData).end(); ++it) {
		const GoodsInfo* lGoodsInfo = sgTableManager()->getGoodsInfo(it->mType, it->mId);
		if (!lGoodsInfo) break;

		Sprite* icon = mIconList.at(i);
		ui::Text* label = mNumList.at(i);

		std::string iconName = lGoodsInfo->mIconSmall;
		int number = it->mNumber;
		icon->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(iconName));
		label->setText(StringUtils::format("%d", number));
		icon->setVisible(true);
		label->setVisible(true);

		i++;
	}

	if (type == eRewardType::MISSION)
	{
		mBaseTitle->setVisible(true);
		mPerfectTitle->setVisible(false);
	}
	else
	{
		mBaseTitle->setVisible(false);
		mPerfectTitle->setVisible(true);
	}
}

void RewardDisplay::onClose(Ref*)
{
	this->setVisible(false);
}
