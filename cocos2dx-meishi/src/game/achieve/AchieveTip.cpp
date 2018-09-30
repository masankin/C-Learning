#include "AchieveTip.h"
#include "data/TableManager.h"
#include "achieve/AchieveControl.h"
#include "game/ui/PageFunction.h"

using namespace cocostudio;
USING_NS_CC;
using namespace ui;

AchieveTip::AchieveTip()
{

}

AchieveTip::~AchieveTip()
{

}

bool AchieveTip::init()
{
	if (!NodeRGBA::init())
	{
		return false;
	}
	Node* pNode = CSLoader::createNode("ui/AchieveTip.csb");
	mTitleTxt = dynamic_cast<Text*>(pNode->getChildByName("Text_1"));
	mContentTxt = dynamic_cast<Text*>(pNode->getChildByName("Text_1_0"));
	mPointTxt = dynamic_cast<Text*>(pNode->getChildByName("Text_30_0_0"));
	mPointImg = dynamic_cast<ImageView*>(pNode->getChildByName("Image_3"));
	mTastIcon = dynamic_cast<Sprite*>(pNode->getChildByName("Image_2"));
	dynamic_cast<Button*>(pNode->getChildByName("Button_2"))->addClickEventListener(CC_CALLBACK_1(AchieveTip::onGotoClickCallBack,this));
	
	addChild(pNode);

	return true;
}

void AchieveTip::onGotoClickCallBack(Ref*)
{
	if (showType == MSG_FUC_ACHIEVE)
	{		
		if(sgAchieveControl()->getAchievePanel() == NULL)
		{
			sgPageFunction()->gotoFunction(MSG_FUC_ACHIEVE);
		}
	}
}


void AchieveTip::updateUI()
{
	if (mShowList.size()>0)
	{
		const AchievementAward* pAchieve =  sgTableManager()->getAchievementById(mShowList.front());
		if (pAchieve == NULL) return;

		mShowList.pop_front();

		mTitleTxt->setString(pAchieve->taskName);
		mContentTxt->setString(pAchieve->describe);
		mPointTxt->setString(StringUtils::format("%d",pAchieve->Award));
		mTastIcon->setSpriteFrame(
			StringUtils::format("ui/general/Ach_Type%d_%d.png", 
			pAchieve->category, pAchieve->mIcon));

		//mPointImg->setPositionX(mPointTxt->getPositionX() + mPointTxt->getContentSize().width + mPointImg->getContentSize().width/2 + 10);
		//²¥·Å¶¯»­
		stopAllActions();
		Size winSize = Director::getInstance()->getWinSize();
		//setPosition(Vec2(winSize.width-170 , winSize.height+35));
		setPosition(Vec2(winSize.width , winSize.height+100));
		setOpacity(0);
		Spawn* moveIn = Spawn::create(MoveBy::create(0.6, Vec2(0, -200)), FadeIn::create(0.3), nullptr);
		Spawn* moveOut = Spawn::create(MoveBy::create(0.6, Vec2(0, 200)), FadeOut::create(0.3), nullptr);
		if (mShowList.size() == 0)
		{
			runAction(Sequence::create(moveIn,
				DelayTime::create(3.0f),
				moveOut,
				RemoveSelf::create(),
				NULL));
		}else
		{
			runAction(Sequence::create(moveIn,
				DelayTime::create(1.0f),
				moveOut,
				CallFunc::create(this,CC_CALLFUNC_SELECTOR(AchieveTip::updateUI)),
				NULL));
		}
		
	}
	
}

void AchieveTip::updateUI(std::list<int> pShowList)
{
	mShowList = pShowList;

	updateUI();
	
	showType = MSG_FUC_ACHIEVE;

	//sg  pShowList.front();
}

void AchieveTip::updateUI(std::string icon,std::string strTitle,std::string strContent)
{
	showType = MSG_FUC_WINNER;
	Size winSize = Director::getInstance()->getWinSize();
	setPosition(Vec2(winSize.width , winSize.height+100));
	mTastIcon->setSpriteFrame(icon);
	mTitleTxt->setString(strTitle);
	mPointImg->setVisible(false);
	mPointTxt->setVisible(false);
	if(!strContent.empty())
		mContentTxt->setString(strContent);

	setOpacity(0);
	Spawn* moveIn = Spawn::create(MoveBy::create(0.6, Vec2(0, -200)), FadeIn::create(0.3), nullptr);
	Spawn* moveOut = Spawn::create(MoveBy::create(0.6, Vec2(0, 200)), FadeOut::create(0.3), nullptr);

	runAction(Sequence::create(moveIn,
		DelayTime::create(3.0f),
		moveOut,
		RemoveSelf::create(),
		NULL));
}
