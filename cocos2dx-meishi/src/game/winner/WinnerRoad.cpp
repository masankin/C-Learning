#include "WinnerRoad.h"
#include "game/ui/PageFunction.h"
#include "game/data/Tables.h"
#include "game/data/TableManager.h"
#include "game/ui/dialogs/GenerlDialog.h"
#include "net/UserObj.h"
#include "game/winner/WinnerControl.h"
#include "SoundManager.h"
#include "utils/Comm.h"

#include "EnvSet.h"

void WinnerRoad::onMenuItemAction(cocos2d::Ref* sender)
{
	Widget* pWidgt = dynamic_cast<Widget*>(sender);
	std::string cName = pWidgt->getCallbackName();

	 if (cName == "onAgain" || cName == "onGet")
	{
		if (!sgLoginProxy()->checkLogin())
		{
			return;
		}
	}
	if (cName == "onAgain")
	{
		sgWinnerControl()->getWinnerDialogPtr()->setShow(false);
		sgPageFunction()->gotoFunction(MSG_FUC_WIN_PVP);
	}
	else if (cName == "onGet")
	{
		GenerlDialog::show("", StringUtils::format(sgTextHelper(TEXT_WINNER_CONFIRM).c_str(), mMoreGold).c_str(),CC_CALLBACK_1(WinnerRoad::callBack,this));
	}
}

bool WinnerRoad::init()
{
	bool ret = false;
	do
	{				
		mRoot = CSLoader::createNode("ui/WinnerRoad.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);

		this->addChild(mRoot);

		mDayNum_1 = dynamic_cast<Text*>(mRoot->getChildByName("DayNum_1"));
		mDayNum_1->setString("5");

		mWinNum_1 = dynamic_cast<Text*>(mRoot->getChildByName("WinNum_1"));
		mWinNum_1->setString("");

		mTextGetted = dynamic_cast<Text*>(mRoot->getChildByName("GettedSprite")->getChildByName("TextGetted"));

		mGoldNum_1 = dynamic_cast<Text*>(mRoot->getChildByName("GoldNum_1"));
		mGoldNum_1->setString("");

		mGoldNum_2 = dynamic_cast<Text*>(mRoot->getChildByName("GoldNum_2"));
		mGoldNum_2->setString("");


		mGettedSprite = dynamic_cast<Sprite*>(mRoot->getChildByName("GettedSprite"));
		mGettedSprite->setVisible(true);

		Button* mBtn = dynamic_cast<Button*>(mRoot->getChildByName("AgainBtn"));
		mBtn->addClickEventListener(CC_CALLBACK_1(WinnerRoad::onMenuItemAction, this));

		mGetBtn = dynamic_cast<Button*>(mRoot->getChildByName("GetBtn"));
		mGetBtn->setVisible(false);
		mGetBtn->addClickEventListener(CC_CALLBACK_1(WinnerRoad::onMenuItemAction, this));

		mBtnSprite_0 = dynamic_cast<Sprite*>(mRoot->getChildByName("GettedSprite")->getChildByName("Sprite_0"));

		mSprite_2 = dynamic_cast<Sprite*>(mRoot->getChildByName("Sprite_2"));
		mSprite_2_0 = dynamic_cast<Sprite*>(mRoot->getChildByName("Sprite_2_0"));

		mSpriteWhq = dynamic_cast<Sprite*>(mRoot->getChildByName("mSpriteWhq"));

		ret = true;
	} while (0);

	return ret;
}

WinnerRoad::WinnerRoad()
{
	mWinNum_1 = NULL;
	mBtnSprite_0 = NULL;
	mGoldNum_1 = NULL;
	mGoldNum_2 = NULL;
	mGettedSprite = NULL;
	mGetBtn = NULL;
	mSpriteWhq = nullptr;
}

WinnerRoad::~WinnerRoad()
{
}

void WinnerRoad::update(float delta)
{

	mRoot->getChildByName("Act_Time")->setVisible(!sgWinnerControl()->getWinnerRoadOpen());
	if (delta > 2.0) return;

	WinnerActivityData* winData = sgWinnerControl()->getWinnerActList(WINNER_ROAD+1);
	if (winData == NULL) return;
	mDayNum_1->setString(StringUtils::format("%d",winData->DaysRemain));

	int totalNum = sgTableManager()->getWinnerDataNum();

	std::string sString =  StringUtils::format("%d",sgWinnerControl()->getWinnerCount());
	mWinNum_1->setString(sString.c_str());
	
	int nextCount = sgWinnerControl()->getWinnerCount()+1;

	//当前连胜场数--->实际领取的对应场数奖励
	int canCount = sgWinnerControl()->getWinnerCount() > totalNum ? totalNum : sgWinnerControl()->getWinnerCount();
	
	nextCount = nextCount > totalNum ? totalNum : nextCount;
	

	int nextGold = 0;
	int currGold = 0;

	const WinnerData* pWinner = sgTableManager()->getWinnerData(canCount);
	if (pWinner)
	{
		currGold = pWinner->getGoldNum();
	}

	pWinner = sgTableManager()->getWinnerData(nextCount);

	if (pWinner)
	{
		nextGold = pWinner->getGoldNum();
	}

	sString =  StringUtils::format("%d",currGold);
	mGoldNum_1->setString(sString.c_str());

	sString = StringUtils::format("%d",nextGold);
	mGoldNum_2->setString(sString.c_str());

	mMoreGold = nextGold - currGold;

	if (sgWinnerControl()->getIsGet())
	{
		mGetBtn->setVisible(false);
		mGettedSprite->setVisible(true);
		mBtnSprite_0->setSpriteFrame("ui/winner/Btn_lq_2.png");
		mTextGetted->setVisible(true);
	}
	else if (sgWinnerControl()->getWinnerCount() < 1)
	{
		mGetBtn->setVisible(false);
		mGettedSprite->setVisible(true);
		mBtnSprite_0->setSpriteFrame("ui/winner/Btn_lq_1.png");
		mTextGetted->setVisible(false);
	}
	else
	{
		mGetBtn->setVisible(true);
		mGettedSprite->setVisible(false);
	}

	//if (sgUserObj()->getPlayerTitle() == TitleType::WINNER)
	if (sgUserObj()->getPersonalInfo()->playertitle() == TitleType::WINNER)
	{
		mSprite_2->setSpriteFrame("ui/winner/Winner.png");
		//mSprite_2_0->setSpriteFrame("ui/general/PlayerTitle_1.png");
		mSprite_2_0->setSpriteFrame(GetPlayerBadgeIconNameById(TitleType::WINNER, true));
		mSprite_2_0->setScale(0.6f);
		mSpriteWhq->setVisible(true);
	}
	else
	{
		mSprite_2->setSpriteFrame("ui/winner/Winnerhui.png");
		//mSprite_2_0->setSpriteFrame("ui/general/PlayerTitle_1_0.png");
		mSprite_2_0->setSpriteFrame(GetPlayerBadgeIconNameById(TitleType::WINNER, false));
		mSprite_2_0->setScale(0.6f);
		mSpriteWhq->setVisible(false);
	}
}

void WinnerRoad::callBack(cocos2d::Ref*)
{
	sgWinnerControl()->sendGetRqst();
	GenerlDialog::hide();
}
