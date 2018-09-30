
#include "AssistantView.h"

USING_NS_CC;
using namespace cocostudio;

const std::string AssistantView::ActionKeyWalk = "walk";
const std::string AssistantView::ActionKeyStand = "stand";
const std::string AssistantView::ActionKeyCastSkill = "skill";

const std::string AssistantView::FrameKeyCastSkill = "atk";
const float AssistantView::ArcActionDuration = 5.0f;
const float AssistantView::ArcActionDeltaAngle = 0.9f;
const float AssistantView::ArcActionDelayTime = 1.5;


AssistantView* AssistantView::create(const std::string& aniName)
{
	auto pRet = new AssistantView();
	if (pRet && pRet->init(aniName))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}

	return pRet;
}

AssistantView::AssistantView(): mAnimation(nullptr), mAniEventCallback(nullptr)
{

}

AssistantView::~AssistantView()
{
	ArmatureDataManager::getInstance()->removeArmatureFileInfo(mAniFilename + ".xml");
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(mAniFilename + ".plist");
	Director::getInstance()->getTextureCache()->removeTextureForKey(mAniFilename + ".png");

	mAnimation = nullptr;
}

bool AssistantView::init(const std::string& aniFilename)
{
	if (!Node::init())
		return false;

	if (aniFilename.empty())
	{
		CCLOG("[error]AssistantView::init : Animation filename is empty.");
		return false;
	}

	auto texFile = aniFilename + ".png";
	auto pTexture = Director::getInstance()->getTextureCache()->addImage(texFile);
	if (nullptr == pTexture)
	{
		CCLOG("[error]AssistantView::init : Animation load texture fail, name = %s.", texFile.c_str());
		return false;
	}

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(aniFilename + ".plist", pTexture);
	ArmatureDataManager::getInstance()->addArmatureFileInfo(aniFilename + ".xml");
	auto aniNameBegPos = aniFilename.find_last_of('/');
	if (aniNameBegPos == std::string::npos)
		auto aniNameBegPos = aniFilename.find_last_of('\\');

	std::string aniName;

	if (aniNameBegPos == std::string::npos)
	{
		aniName = aniFilename;
	}
	else
	{
		aniName = aniFilename.substr(aniNameBegPos + 1, aniFilename.size() - aniNameBegPos);
	}

	if (aniName.empty())
	{
		CCLOG("[error]AssistantView::init : Animation name is invalid, name = %s", aniFilename.c_str());
		return false;
	}

	if (nullptr == ArmatureDataManager::getInstance()->getArmatureData(aniName))
	{
		CCLOG("[error]AssistantView::init : Animation could not found, aniName = %s", aniName.c_str());
		return false;
	}

	// Animation
	mAnimation = Armature::create(aniName);
	if (nullptr == mAnimation)
	{
		CCLOG("[error]AssistantView::init : Animation create fail, name = %s.", aniFilename.c_str());
		return false;
	}

	mAniFilename = aniFilename;

	mAnimation->getAnimation()->setMovementEventCallFunc(std::bind(&AssistantView::onAniMovementEventCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	mAnimation->getAnimation()->setFrameEventCallFunc(std::bind(&AssistantView::onAniFrameEventCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	mAnimation->setAnchorPoint(Vec2(0.5, 0.5));

	this->addChild(mAnimation, LZO::kAnimation);

	// Effect 
	std::string effectName;

	effectName = aniName + "_ADDITIVE";
	auto pBone = mAnimation->getBone(effectName);
	if (pBone)
	{
		pBone->setBlendFunc(BlendFunc::COMADDITIVE);
	}


	return true;
}

void AssistantView::setAniEventCallback(const AniEventCallback& callback)
{
	mAniEventCallback = callback;
}


void AssistantView::onCastSkill()
{
	if (mAnimation )
		mAnimation->getAnimation()->play(AssistantView::ActionKeyCastSkill);
}

void AssistantView::onIdle()
{
	if (mAnimation)
	{
		//if (cocos2d::random(1, 10000) <= 2000)
		//	mAnimation->getAnimation()->play(AssistantView::ActionKeyStand);
		//else
		//	mAnimation->getAnimation()->play(AssistantView::ActionKeyWalk); 
		mAnimation->getAnimation()->play(AssistantView::ActionKeyWalk);
	}
}

bool AssistantView::isCastingSkill() const
{
	if (mAnimation)
		return mAnimation->getAnimation()->getCurrentMovementID() == AssistantView::ActionKeyCastSkill;

	return false;
}

void AssistantView::sendAniEvent(AniEvent eventType)
{
	if (mAniEventCallback)
		mAniEventCallback(eventType);
}

void AssistantView::onAniMovementEventCallback(cocostudio::Armature *pAr, cocostudio::MovementEventType type, const std::string& id)
{
	if (AssistantView::ActionKeyWalk == id)
	{
		switch (type)
		{
		case cocostudio::MovementEventType::START:		break;
		case cocostudio::MovementEventType::COMPLETE:	this->onIdle(); break;
		case cocostudio::MovementEventType::LOOP_COMPLETE:	break;
		}
	}
	else if (AssistantView::ActionKeyStand == id)
	{
		switch (type)
		{
		case cocostudio::MovementEventType::START:		break;
		case cocostudio::MovementEventType::COMPLETE:	this->onIdle(); break;
		case cocostudio::MovementEventType::LOOP_COMPLETE:	break;
		}
	}
	else if (AssistantView::ActionKeyCastSkill == id)
	{
		switch (type)
		{
		case cocostudio::MovementEventType::START:
		{
			mAnimation->getActionManager()->pauseTarget(mAnimation);
			float scalex = (mAnimation->getScaleX() > 0 ? 1 : -1) *  1.5;
			mAnimation->setScaleY(1.5);
			mAnimation->setScaleX(scalex);
			this->sendAniEvent(AniEvent::kCastSkillStart);
		}
		break;

		case cocostudio::MovementEventType::COMPLETE:
		{
			this->onIdle();
			this->sendAniEvent(AniEvent::kCastSkillEnd);
			float scalex = (mAnimation->getScaleX() > 0 ? 1 : -1) *  0.8;
			mAnimation->setScaleY(0.8);
			mAnimation->setScaleX(scalex);
			mAnimation->getActionManager()->resumeTarget(mAnimation);
		}
		break;

		case cocostudio::MovementEventType::LOOP_COMPLETE:
		{
		}
		break;

		}
	}
}

void AssistantView::onAniFrameEventCallback(cocostudio::Bone *bone, const std::string& eventId, int originFrameIndex, int currentFrameIndex)
{
	if (eventId == AssistantView::FrameKeyCastSkill)
	{
		this->sendAniEvent(kCastSkill);
	}
}

void AssistantView::playMoveAciton(Vec2 pos, float scale)
{
	if (mAnimation)
	{
		scale = 0.8 * scale;
		mAnimation->setScaleX(scale);
		mAnimation->setScaleY(0.8);

		CCArcBy* pAction = CCArcBy::create(AssistantView::ArcActionDuration / 2, Vec2::ZERO, AssistantView::ArcActionDeltaAngle / 2);
		DelayTime* pDelay = DelayTime::create(AssistantView::ArcActionDelayTime);
		CallFunc* pCallStand = CallFunc::create([this](void){
			if (mAnimation)
			{
				mAnimation->getAnimation()->play(AssistantView::ActionKeyStand);
			}
		});
		CallFunc* pCallWalk = CallFunc::create([this](void){
			if (mAnimation)
			{
				mAnimation->getAnimation()->play(AssistantView::ActionKeyWalk);
			}
		});

		CallFunc* pScale = CallFunc::create([this](void){
			if (mAnimation)
			{
				float scale = mAnimation->getScaleX() * -1;
				mAnimation->setScaleX(scale);
			}
		});

		Sequence* pSequence = Sequence::create(pCallStand, pDelay, pCallWalk , pAction,
			pCallStand->clone(), pDelay->clone(), pCallWalk->clone(), pAction->clone(), pScale,
			pCallStand->clone(), pDelay->clone(), pCallWalk->clone(), pAction->reverse(),
			pCallStand->clone(), pDelay->clone(), pCallWalk->clone(), pAction->reverse(), pScale->clone(), nullptr);
		RepeatForever* pForever = RepeatForever::create(pSequence);

		mAnimation->setPosition(pos);
		mAnimation->runAction(pForever);
	}
}

void AssistantView::stopAnimtion()
{
	if (mAnimation)
	{
		mAnimation->getAnimation()->stop();
		mAnimation->stopAllActions();
	}
}






CCArcBy* CCArcBy::create(float duration, const Vec2& ptCenter, float deltaAngle)
{
	CCArcBy* pRet = new CCArcBy();
	pRet->initWithDuration(duration, ptCenter, deltaAngle);
	pRet->autorelease();

	return pRet;
}

bool CCArcBy::initWithDuration(float duration, const Vec2& ptCenter, float deltaAngle)
{
	if (CCActionInterval::initWithDuration(duration))
	{
		m_ptCenter = ptCenter;
		m_fAngleDelta = deltaAngle;
		return true;
	}

	return false;
}

void CCArcBy::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	m_previousPosition = m_startPosition = pTarget->getPosition();
}

CCActionInterval* CCArcBy::reverse()const
{
	
	return CCArcBy::create(_duration, m_ptCenter, -m_fAngleDelta);
}

void CCArcBy::update(float time)
{
	if (_target)
	{
#if CC_ENABLE_STACKABLE_ACTIONS
		Vec2 currentPos = _target->getPosition();
		Vec2 diff = currentPos - m_previousPosition;
		m_startPosition = m_startPosition + diff;
		Vec2 newPos = m_ptCenter + ccpRotateByAngle(m_startPosition - m_ptCenter, Vec2::ZERO, m_fAngleDelta*time);
		_target->setPosition(newPos);
		//_target->setRotation(-CC_RADIANS_TO_DEGREES(m_fAngleDelta*time));
		m_previousPosition = newPos;

#else
		_target->setPosition(m_ptCenter + ccpRotateByAngle(m_startPosition - m_ptCenter, Vec2::ZERO, m_fAngleDelta*time));
#endif  // CC_ENABLE_STACKABLE_ACTIONS
	}
}

CCActionInterval* CCArcBy::clone() const
{
	CCArcBy* pRet = new CCArcBy();
	pRet->initWithDuration(_duration, m_ptCenter, m_fAngleDelta);
	pRet->autorelease();

	return pRet;
}
