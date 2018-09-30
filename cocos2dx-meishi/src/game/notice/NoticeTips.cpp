#include "NoticeTips.h"
#include "game/ui/PageFunction.h"
#include "game/ui/system/Window.h"
#include "NoticeControl.h"

#define TIP_TEXT_LENGTH (1000.0)
#define TIP_MOVE_TIME (11.0)

NoticeTips::NoticeTips() :mSpriteBg(0), mContent(0),
	mIsMoveing(false),
	mTipsLabel(0)
{

}

NoticeTips::~NoticeTips()
{

}


bool NoticeTips::init()
{
	bool ret = false;
	do
	{
		mWinSize = Director::getInstance()->getWinSize();
		Node* mRoot = CSLoader::getInstance()->createNode("ui/NoticeTips.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);
		mSpriteBg =  dynamic_cast<Sprite*>(mRoot->getChildByName("Sprite_4"));
		mContent = dynamic_cast<ui::Text*>(mSpriteBg->getChildByName("ScrollView_1")->getChildByName("Text_Tips"));;
		mContent->setVisible(false);
#ifdef MGAME_PLATFORM_MPLUS
		if (mContent) {
			mContent->setFontName(DEFAULT_FONT);
		}
#endif
		mTipsLabel = StringLabel::create();
		mTipsLabel->setSingleLine(true);
		mTipsLabel->setContentSize(CCSize(mSpriteBg->getContentSize().width, 0));
		mTipsLabel->setTextSize(mContent->getFontSize());
		mTipsLabel->setFontName(mContent->getFontName());
		mTipsLabel->setAnchorPoint(mContent->getAnchorPoint());
		mContent->getParent()->addChild(mTipsLabel);
		mTipsLabel->setPosition(mContent->getPosition());
		mTipsLabel->setTextColor(mContent->getColor());
		ret = true;
	} while (0);


	return ret;
}


void NoticeTips::startNotice()
{
	Array* noticeLamp = sgNoticeControl()->getNoticeLampArray();
	if (!noticeLamp) return;

	if (!noticeLamp->count())
	{
		mIsMoveing = false;
		this->setVisible(false);
		return;
	}

	if (!this->isVisible())
	{
		this->setVisible(true);
	}
	if (!mIsMoveing)
	{
		mIsMoveing = true;
		NoticeInfo* noticeInfo = (NoticeInfo*)noticeLamp->objectAtIndex(0);

		PowerString ps;
		ps.init(noticeInfo->getNoticeContent().c_str());
		mTipsLabel->setPowerString(&ps);
		setTipsMove();
	}
}

void NoticeTips::setTipsMove()
{
	mTipsLabel->setVisible(true);
	mTipsLabel->stopAllActions();
	mTipsLabel->setPositionX(TIP_TEXT_LENGTH);	
	int width = mTipsLabel->getContentSize().width;
	int dt = width/TIP_TEXT_LENGTH * TIP_MOVE_TIME;
	mTipsLabel->runAction(Sequence::create(
		MoveBy::create(TIP_MOVE_TIME + dt, Vec2(-TIP_TEXT_LENGTH - width, 0)),
		DelayTime::create(1.0),
		CallFunc::create(CC_CALLBACK_0(NoticeTips::callback, this)),nullptr)
		);//(&NoticeTips::callback);
}

void NoticeTips::callback()
{
	mTipsLabel->setVisible(false);
	mTipsLabel->setPositionX(TIP_TEXT_LENGTH);
	if (sgNoticeControl()->getNoticeLampArray()->count()){
		sgNoticeControl()->getNoticeLampArray()->removeObjectAtIndex(0);
	}

	mIsMoveing = false;
	startNotice();	
}
