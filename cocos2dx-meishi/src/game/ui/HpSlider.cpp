#include "HpSlider.h"
#include "ui/UIHelper.h"
#include "game/battle/BattleControl.h"
#include "game/battle/Camp.h"

#include "cocostudio/ActionTimeline/CSLoader.h"

#define  PI 3.1415926
#define  LABLE_TAG 1125

HpSlider* HpSlider::create()
{
	HpSlider* pret = new HpSlider();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

bool HpSlider::init()
{
	//CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("uiAtlas.plist");

	mRoot = CSLoader::getInstance()->createNode("HpSlider.csb");
	if (!mRoot)
	{
		CCLOG("[error] ArrowWidget::initUI load csb error!");
		return false;
	}

	mRoot->setAnchorPoint(Vec2(0.5, 0.5));
	mHoles = mRoot->getChildByName("mHoles");



	//沒垓夾浬

	minSize = mRoot->getContentSize();
	this->addChild(mRoot);
	offSetX = 0;
	return true;
}

void HpSlider::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{

}


void HpSlider::reset(ui::ImageView* pSended, float percent, float& offsetX)
{
	if (pSended)
	{
		pSended->setContentSize(Size(minSize.width*percent, minSize.height));
		//pSended->setPositionX(offsetX + pSended->getContentSize().width / 2);
		pSended->setPositionX(offsetX);
		offsetX += pSended->getContentSize().width;
		ui::Helper::doLayout(pSended);
	}
}

void HpSlider::updateBar(ui::ImageView* bar, float posX, float width)
{
	if (bar)
	{
		auto sprite9 = dynamic_cast<ui::Scale9Sprite*>(bar->getVirtualRenderer());
		//auto insetsRect = bar->getCapInsets();
		//bar->setCapInsets(insetsRect);
		//auto size = bar->getContentSize();
		//
		//size.width = width;

		//bar->setContentSize(size);

		auto r = sprite9->getPreferredSize();
		r.width = width;
		sprite9->setPreferredSize(r);
		if (width == 0)
			bar->setVisible(false);

		bar->setPositionX(posX + width / 2.0f);
		//bar->setPositionX(posX/* + width*/);
	}

}


void HpSlider::refreshSliderPercent()
{
	const CampList&  camplist = sgBattleControl()->getCampList();

	float totalCount = 0;
	int campNum = 0;
	CampList::const_iterator campItr = camplist.begin();
	for (; campItr != camplist.end(); campItr++)
	{
		if (campItr->first == ECAMP_TYPE_Neutral)
			continue;

		if (campItr->first == ECAMP_TYPE_Creeps)
			continue;
		campNum++;
		totalCount += campItr->second->getAllUnit();
	}

	float numPercent5 = totalCount * ( 125.0f / 750.0f);
	totalCount += numPercent5 * campNum;

	float curPosX = 0;
	int index = 0;
	for (SliderType::iterator barItr = mCampSlider.begin(); barItr != mCampSlider.end(); ++barItr)
	{

		int campType = barItr->first;
		campType = campType > 4 ? campType - 4 : campType;

		auto barImage = barItr->second;

		campItr = camplist.find(barItr->first);
		if (camplist.end() != campItr && barItr->first != ECAMP_TYPE_Neutral && barItr->first != ECAMP_TYPE_Creeps)
		{
			barImage->setVisible(true);
			float count = campItr->second->getAllUnit() + numPercent5;
			float percent = count / (float)totalCount;

			//if (percent == 0)
			//	percent = 0.1;

			//curPosX -= index * 2.5f;
			auto barWidth = minSize.width * percent;
			updateBar(barImage, curPosX, barWidth);

			ui::Text* pLable = dynamic_cast<ui::Text*>(barImage->getChildByName(StringUtils::format("mNumNode%d", campType))->getChildByName("mLabel"));
			if (pLable)
			{
				Camp* pCamp = sgBattleControl()->getTargetCamp(barItr->first);
				bool max = pCamp->checkIsMax();

				if (!max)
					pLable->setText(StringUtils::format("x%d", (int)(count - numPercent5)));
				else
					pLable->setText(StringUtils::format("x%d(max)", (int)(count - numPercent5)));

				Node* pHead = barImage->getChildByName(StringUtils::format("mNumNode%d", campType))->getChildByName("general_baozi1_2");

				int  offSet = pLable->getContentSize().width / 2 - pHead->getContentSize().width / 2;

				if (pHead)
				{
					pHead->setPositionX(pHead->getTag() - offSet);
					pLable->setPositionX(pLable->getTag() - offSet);
				}

			}

			curPosX += barWidth;
			index++;
		}
	}

	//float len = 3;
	//float p = (float)percent / 100.0;
	//reset(mPlayer1, p, len);
	//len -= 3;
	//reset(mPlayer2, 1 - p, len);
}

void HpSlider::setPeopleWithCamp(int num, eCampType type)
{
	if (type == ECAMP_TYPE_Creeps || type == ECAMP_TYPE_Neutral)
		return;

	int campType = type;
	campType = campType > 4 ? campType - 4 : campType;

	ui::ImageView* pPlayer = nullptr;
	ui::Text* pLable = NULL;

	Camp* pCamp = sgBattleControl()->getTargetCamp(type);
	if (pCamp == nullptr) return;



	pPlayer = dynamic_cast<ui::ImageView*>(mHoles->getChildByName(StringUtils::format("mPlayer%d", campType)));
	if (pPlayer)
		pLable = dynamic_cast<ui::Text*>(pPlayer->getChildByName(StringUtils::format("mNumNode%d", campType))->getChildByName("mLabel"));
	if (pLable)
	{
		bool max = pCamp->checkIsMax();
		if (!max)
			pLable->setText(StringUtils::format("x%d", num));
		else
			pLable->setText(StringUtils::format("x%d(max)", num));
	}
}

void HpSlider::initSlider()
{
	mLabelList.clear();
	mCampSlider.clear();

	for (int i = 1; i <= 4; i++)
	{
		ui::ImageView* playerBar = dynamic_cast<ui::ImageView*>(mHoles->getChildByName(StringUtils::format("mPlayer%d", i)));
		playerBar->setVisible(false);

		//ui::Text* pLable = dynamic_cast<ui::Text*>(playerBar->getChildByName(StringUtils::format("mNumNode%d", campType))->getChildByName("mLabel"));
		//pLable->setVisible(false);

	}


	const CampList&  camplist = sgBattleControl()->getCampList();
	CampList::const_iterator sIt = camplist.begin();

	for (; camplist.end() != sIt; sIt++)
	{
		if (sIt->first == ECAMP_TYPE_Creeps || sIt->first == ECAMP_TYPE_Neutral)
			continue;

		int campType = sIt->first;
		campType = campType > 4 ? campType - 4 : campType;

		ui::ImageView* playerBar = dynamic_cast<ui::ImageView*>(mHoles->getChildByName(StringUtils::format("mPlayer%d", campType)));
		playerBar->loadTexture(getTextureName(sIt->first), ui::Widget::TextureResType::PLIST);
		playerBar->setAnchorPoint(Vec2(0.5, 0.5));
		playerBar->setVisible(true);
		mCampSlider.insert(std::make_pair(sIt->first, playerBar));

		ui::Text* pLable = dynamic_cast<ui::Text*>(playerBar->getChildByName(StringUtils::format("mNumNode%d", campType))->getChildByName("mLabel"));
		pLable->setTag(pLable->getPositionX());
		pLable->setVisible(true);

		Sprite* pHead = dynamic_cast<Sprite*> (playerBar->getChildByName(StringUtils::format("mNumNode%d", campType))->getChildByName("general_baozi1_2"));
		pHead->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(getIconName(sIt->first)));
		pHead->setTag(pHead->getPositionX());
		//auto pLable = LabelAtlas::create("50", "suzi.png", 14, 20, '0');
		//pLable->setAnchorPoint(Vec2(0.5, 0.5));
		//pNode->addChild(pLable, 0, LABLE_TAG);



		mLabelList.insert(std::make_pair(sIt->first, pLable));
	}




	//SliderType::iterator sIt =  mCampSlider.begin();
	//for (; sIt != mCampSlider.end(); sIt++)
	//	sIt->second->setVisible(false);


	//sIt = mCampSlider.begin();
	//for (; sIt != mCampSlider.end(); sIt++)
	//{
	//	if (camplist.find(sIt->first) != camplist.end())
	//	{
	//		sIt->second->setVisible(true);
	//	}
	//}

}

std::string HpSlider::getTextureName(eCampType type)
{
	std::string name = "";;

	switch (type)
	{
	case ECAMP_TYPE_Player:
		name = "ui/battle/Battle_ju_2.png";
		break;
	case ECAMP_TYPE_AI:
		name = "ui/battle/Battle_lan_2.png";
		break;
	case ECAMP_TYPE_AI2:
		name = "ui/battle/Battle_hong_2.png";
		break;
	case ECAMP_TYPE_AI3:
		name = "ui/battle/Battle_lv_2.png";
		break;
	case ECAMP_TYPE_Player1:
		name = "ui/battle/Battle_jdt_red.png";

		break;
	case ECAMP_TYPE_Player2:
		name = "ui/battle/Battle_jdt_blue.png";

		break;
	case ECAMP_TYPE_Player3:
		name = "ui/battle/Battle_jdt_violet.png";

		break;
	case ECAMP_TYPE_Player4:
		name = "ui/battle/Battle_jdt_yellow.png";

		break;
	default:
		break;
	}
	return name;
}

std::string HpSlider::getIconName(eCampType type)
{
	std::string name = "";;

	switch (type)
	{
	case ECAMP_TYPE_Player:
		name = "ui/general/general_baozi1.png";
		break;
	case ECAMP_TYPE_AI:
		name = "ui/general/general_zongzi1.png";
		break;
	case ECAMP_TYPE_AI2:
		name = "ui/general/general_hanb1.png";
		break;
	case ECAMP_TYPE_AI3:
		name = "ui/general/general_cake_1.png";
		break;
	case ECAMP_TYPE_Player1:
		name = "ui/general/general_baozi_red.png";

		break;
	case ECAMP_TYPE_Player2:
		name = "ui/general/general_baozi_blue.png";

		break;
	case ECAMP_TYPE_Player3:
		name = "ui/general/general_baozi_violet.png";

		break;
	case ECAMP_TYPE_Player4:
		name = "ui/general/baozi_yellow.png";

		break;
	default:
		break;
	}

	return name;
}
