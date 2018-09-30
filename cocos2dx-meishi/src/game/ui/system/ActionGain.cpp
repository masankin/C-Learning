#include "ActionGain.h"
#include "cocostudio/CocoStudio.h"

#define FlyTrackType 0

ActionGain::ActionGain() :
	mType(RECEIVER_ALONE),
	mOriginReceiver(NULL),
	mReceiver(NULL),
	mAccountText(NULL),
	mVisible(true),
	mAccount(0)
{
	clearItemList();
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_TB.png", "fla_TB.plist", "fla_TB.xml");
}

ActionGain::~ActionGain()
{
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_TB.xml");
	clearItemList();
}

bool ActionGain::init()
{
	if (!Node::init())
		return false;

	mItemsNode = Node::create();
	addChild(mItemsNode, 10);

	return true;
}

void ActionGain::addReceiver(eReceiverType type, Node* receiver, Vec2 position, bool isVisible, Node* originReceiver)
{
	if (mReceiver)
		return;

	mType = type;
	mReceiver = receiver;
	mReceiverPosition = position;
	mOriginReceiver = originReceiver;
	mVisible = isVisible;
	mReceiver->setPosition(position);
	mReceiver->setVisible(false);
	addChild(mReceiver);
}

void ActionGain::addItem(Node* item, Vec2 position, int itemCount, int originCount, bool isRandomRotation, bool isRandomPos)
{
	if (mOriginReceiver && !mVisible)
		mOriginReceiver->setVisible(false);

	if (!mReceiver->isVisible())
		mReceiver->setVisible(true);

	if (mType == RECEIVER_ACCOUNT && mItemCountVec.empty())
	{
		mAccount = originCount;
		mAccountText = dynamic_cast<Text*> (mReceiver->getChildByName("Text_1"));
		mAccountText->setString(cocos2d::Value(mAccount).asString());
	}

	mItemCountVec.push(itemCount);
	mItemsNode->addChild(item);
	item->setPosition(position);

#if FlyTrackType
	item->setOpacity(0);
	
	Vec2 distance = mReceiverPosition - position;
	Vec2 controlPoint = position;
	if (distance.x > 0)
	{
		controlPoint += distance.y > 0 ? Vec2(100, -200) : Vec2(100, 200);
	}
	else
	{
		controlPoint += distance.y > 0 ? Vec2(-100, -200) : Vec2(-100, 200);
	}
	ccBezierConfig config = { mReceiverPosition, controlPoint, mReceiverPosition };

	item->stopAllActions();
	item->runAction(Sequence::create(
		FadeIn::create(.3f),
		EaseIn::create(BezierTo::create(.7f, config), 2.0f),
		DelayTime::create(.1f),
		CallFunc::create(this, CC_CALLFUNC_SELECTOR(ActionGain::actionCallback)),
		RemoveSelf::create(true),
		NULL));

#else
	int dir = /*cocos2d::random(-1, 1) > 0 ? 1 :*/ -1;
	int randomRotate = 0;
	if (isRandomRotation)
		randomRotate = cocos2d::random(0, 360);

	Vec2 fallEndPoint = position + Vec2(-100, -200);
	Vec2 fallControlPoint = position + Vec2(-80, 100);
	if (isRandomPos)
	{
		int randomEndX = dir * cocos2d::random(0, 160);
		fallEndPoint = position + Vec2(randomEndX, -200);
		fallControlPoint = position + Vec2(randomEndX / 2, 100);
	}
	ccBezierConfig config = { fallEndPoint, fallControlPoint, fallEndPoint };

	item->stopAllActions();
	item->runAction(Sequence::create(
		Spawn::create(BezierTo::create(.5f, config), RotateBy::create(.5f, randomRotate), NULL),
		MoveTo::create(0.4f, Vec2(fallEndPoint.x-1,fallEndPoint.y + 15)),
		MoveTo::create(0.4f, fallEndPoint),
		MoveTo::create(0.4f, Vec2(fallEndPoint.x+1,fallEndPoint.y + 10)),
		MoveTo::create(0.4f, fallEndPoint),
		DelayTime::create(.3f),CallFuncN::create(this, callfuncN_selector(ActionGain::particleCallback)),
		Spawn::create(EaseIn::create(MoveTo::create(.5f, mReceiverPosition), 2.0f), ScaleBy::create(.5f, .8f), NULL),
		DelayTime::create(.1f),
		CallFunc::create(this, CC_CALLFUNC_SELECTOR(ActionGain::actionCallback)),
		RemoveSelf::create(true),
		NULL));

#endif
}

void ActionGain::addItem(std::string itemName, Vec2 position, int itemCount, int originCount)
{
	Sprite* pItem = Sprite::createWithSpriteFrameName(itemName);
	addItem(pItem, position, itemCount, originCount);
}

void ActionGain::removeReceiver()
{
	if (!mItemCountVec.empty())
		return;

	if (mOriginReceiver && !mVisible)
	{
		mOriginReceiver->setVisible(true);
	}
	mReceiver->removeChildByTag(11);
	mReceiver->setVisible(false);
	clearItemList();
}

void ActionGain::stopActionGain()
{
	if (mOriginReceiver && !mVisible)
	{
		mOriginReceiver->setVisible(true);
	}

	mItemsNode->removeAllChildren();
	clearItemList();
}

void ActionGain::setMoveAction()
{
	//TODO
}

void ActionGain::actionCallback()
{
	if (mType == RECEIVER_ACCOUNT)
	{
		/*int count = mItemCountVec.back();
		mItemCountVec.pop();
		if (mItemCountVec.empty())
		{
		mAccount += count;
		mAccountText->setString(cocos2d::Value(mAccount).asString());
		}*/
		mAccount += mItemCountVec.back();
		mAccountText->setString(cocos2d::Value(mAccount).asString());
		mItemCountVec.pop();
		removeReceiver();
	}
	else
	{
		mItemCountVec.pop();
		mReceiver->stopAllActions();
		mReceiver->runAction(Sequence::create(
			ScaleTo::create(.0f, 1.0f),
			ScaleTo::create(.2f, 1.3f),
			ScaleTo::create(.2f, 1.0f),
			CallFunc::create(this, CC_CALLFUNC_SELECTOR(ActionGain::removeReceiver)),
			NULL));

		auto pAni = cocostudio::CCArmature::create("fla_TB");
		pAni->getAnimation()->play("play");
		pAni->setPosition(mReceiver->getContentSize().width/2,mReceiver->getContentSize().height/2);
		mReceiver->addChild(pAni,1,11);
	}
}

void ActionGain::clearItemList()
{
	while (!mItemCountVec.empty())
	{
		mItemCountVec.pop();
	}
}

void ActionGain::particleCallback(Node* node)
{
	ParticleSystemQuad* pParticle = ParticleSystemQuad::create("solider_return.plist");
	if (nullptr == pParticle)
		return;

	pParticle->setPosition(node->getPosition());
	mItemsNode->addChild(pParticle,-1);
	pParticle->runAction(Sequence::create(Spawn::create(EaseIn::create(MoveTo::create(.5f, mReceiverPosition), 2.0f), ScaleBy::create(.5f, .8f), NULL),
		DelayTime::create(.3f),
		RemoveSelf::create(true),
		NULL));

}
