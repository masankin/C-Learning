#include "ActionTip.h"

ActionTip::ActionTip() : 
	mAction(NULL),
	mCallBack(nullptr)
{
}

ActionTip::~ActionTip()
{
}

ActionTip* ActionTip::create(string text, Action* action)
{
	ActionTip *pRet = new (std::nothrow) ActionTip();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		if (text != "")
		{
			pRet->mTextList.push_back(text);
		}
		pRet->setAction(action);
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

ActionTip* ActionTip::create(vector<string> textList, Action* action)
{
	ActionTip *pRet = new (std::nothrow) ActionTip();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		for (int i = 0; i < textList.size(); i++)
		{
			pRet->mTextList.push_back(textList.at(i));
		}
		pRet->setAction(action);
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

ActionTip* ActionTip::create(string text, Vec2 distance, float duration)
{
	ActionTip *pRet = new (std::nothrow) ActionTip();
	if (pRet && pRet->init(distance, duration))
	{
		pRet->autorelease();
		if (text != "")
		{
			pRet->mTextList.push_back(text);
		}	
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

ActionTip* ActionTip::create(vector<string> textList, Vec2 distance, float duration)
{
	ActionTip *pRet = new (std::nothrow) ActionTip();
	if (pRet && pRet->init(distance, duration))
	{
		pRet->autorelease();
		for (int i = 0; i < textList.size(); i++)
		{
			pRet->mTextList.push_back(textList.at(i));
		}
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

bool ActionTip::init()
{
	mFontSize = 18;
	mFontName = "fonts/font.TTF";

	mColor = Color4B(111, 254, 22, 255);
	mMargin = 0;
	mContentHeight = .0f; 
	mIsRun = false;

	m_bIsShowPic = true;

	return true;
}

bool ActionTip::init(Vec2 distance, float duration)
{
	mDistance = distance;
	mDuration = duration;

	init();

	return true;
}

void ActionTip::addText(string text)
{
	if(text == "")
	{
		return;
	}
	mTextList.push_back(text);
}

void ActionTip::run()
{
	if (mTextList.size() == 0)
	{
		removeFromParent();
		return;
	}

	if (!mIsRun)
	{
		for (int i = 0; i < mTextList.size(); i++)
		{
			addTextWidget(mTextList.at(i));
		}
		setPositionY(getPositionY() + mContentHeight);
		runChildrenActions(this);
		mIsRun = true;
	}
}

void ActionTip::setCallBackFunc(const std::function<void()>& callBack)
{
	mCallBack = callBack;
}

void ActionTip::runChildrenActions(Node* target)
{
	int iCount = target->getChildrenCount();
	if (iCount == 0)
		return;

	for (int i = 0; i < iCount; i++)
	{
		if (target->getChildren().at(i)->getChildrenCount() > 1)
		{
			((ActionTip*)(target->getChildren().at(i)))->runChildrenActions(target->getChildren().at(i));
		}
		else
		{
			runActions(target->getChildren().at(i), i == iCount - 1);
		}
	}
}

void ActionTip::runActions(Node* target, bool removeParent)
{
	ActionTip* pParent = (ActionTip*) target->getParent();
	target->runAction(Sequence::create(
		mAction ? (ActionInterval*)(mAction->clone()) : Spawn::create(MoveBy::create(mDuration, mDistance), FadeOut::create(mDuration), NULL),
		RemoveSelf::create(),
		removeParent ? CallFunc::create([pParent](){pParent->removeFromParent(); }) : NULL,
		pParent->mCallBack ? CallFunc::create(pParent->mCallBack) : NULL,
		NULL));
}

void ActionTip::addTextWidget(string text)
{
	Text* pText = Text::create();
	pText->setFontSize(mFontSize);
	pText->setFontName(mFontName);
	pText->setTextColor(mColor);
	//阴影,暂不用参数控制开关
	pText->enableShadow(Color4B::BLACK, Size(1, -1));
	pText->setString(text);
	Sprite* pSprite = NULL;

	if (m_bIsShowPic)
	{
		if (mFontSize != 18)
		{
			pSprite = Sprite::create("ui/battle/Battle_arrow.png");
		}
		else
		{
			pSprite = Sprite::createWithSpriteFrameName("ui/battle/powerUp.png");
			if (!pSprite)
			{
				pSprite = Sprite::create("ui/battle/powerUp.png");
			}
		}
		pSprite->setPosition((pText->getContentSize().width + pSprite->getContentSize().width) * .5f, -2);//-2为Y轴偏移量

		if (getChildrenCount() == 0)
		{
			mMargin -= pSprite->getContentSize().height * .5f;
			mContentHeight += pSprite->getContentSize().height;
		}
		else
		{
			mMargin -= (pSprite->getContentSize().height + 0);			//0为间隔
			mContentHeight += (pSprite->getContentSize().height + 0);	//0为间隔
		}
	}

	ActionTip* pParent = NULL;
	if (mAction)
	{
		pParent = ActionTip::create(text, mAction);
	}
	else
	{
		pParent = ActionTip::create(text, mDistance, mDuration);
	}
	addChild(pParent);
	pParent->addChild(pText);

	if (m_bIsShowPic)
	{
		pParent->addChild(pSprite);
		pParent->setPosition(pSprite->getContentSize().width * -0.5f, mMargin);
	}
}

//Action* pAction = Sequence::create(
//	MoveBy::create(.3f, Vec2(0, 50)),
//	DelayTime::create(1.5f),
//	Spawn::create(MoveBy::create(.2f, Vec2(0, 30)), FadeOut::create(.2f), NULL),
//	NULL);
//ActionTip* pTip = ActionTip::create(sgTextHelper(TEXT_BATTLE_POPULATION), pAction);
//pTip->addText(sgTextHelper(TEXT_BATTLE_SPEED_PRODUCE));
//pTip->addText(sgTextHelper(TEXT_BATTLE_DEFENSE));
//pTip->run();
