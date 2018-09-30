#include "BuffView.h"
#include "core/TimeCalculator.h"
#define  BUFFICON_KEY "buffIcon_%d_%d"
BuffView::BuffView()
{
	mBuffProgress = NULL;
	mBuffBg = NULL;
}

BuffView::~BuffView()
{
	 
}

BuffView* BuffView::create()
{
	BuffView* pt = new BuffView();
	if(NULL != pt)
	{
		pt->autorelease();
		return pt;
	}
	delete pt;
	return NULL;
}
 

bool BuffView::initBuffView(eBuffType buffType)
{
	std::string name;
	switch (buffType)
	{
	case E_Buff_MoveSpeed:
		name = "SpeedBuff.png"; 
		break;
	case E_Buff_AttackSpeed:
	case E_Buff_Attack:
		name = "AttackBuff.png"; 
		break;
	case E_Buff_Defence:
		name = "DefenceBuff.png"; 
		break;
	case E_Buff_FillPeople:
		return false;
	case E_Buff_ProduceSpeed:
		name = "ProduceBuff.png"; 
		break; 
	default:
		return false;
		break;
	} 
	initBg(name);
	initProgress(name);
	return true;
}


void BuffView::initBg(std::string fileName)
{
	if(NULL != mBuffBg)
	{
		mBuffBg->removeFromParentAndCleanup(true);
		mBuffBg = NULL;
	}
	mBuffBg = Sprite::create(fileName);
	if(NULL != mBuffBg)
	{
		mBuffBg->setVisible(true);
		mBuffBg->setColor(ccc3(mBuffBg->getColor().r * 0.65f, 
			mBuffBg->getColor().g * 0.65f, 
			mBuffBg->getColor().b * 0.65f));
		addChild(mBuffBg);
	} 
}

void BuffView::initProgress(std::string fileName)
{
	if(NULL != mBuffProgress)
	{
		mBuffProgress->removeFromParentAndCleanup(true);
		mBuffProgress = NULL;
	}
	mBuffProgress = ProgressTimer::create(Sprite::create(fileName));
	if(NULL != mBuffProgress)
	{
		mBuffProgress->setLocalZOrder(999);
		addChild(mBuffProgress, 99999);
		mBuffProgress->setVisible(true);
		mBuffProgress->setType(ProgressTimer::Type::RADIAL);
		mBuffProgress->setBarChangeRate(Vec2(0, 1));
		mBuffProgress->setMidpoint(ccp(0.5, 0.5));
		mBuffProgress->setPercentage(100.0f);
		mBuffProgress->runAction(CCFadeIn::create(1.2f));
	}  
}

void BuffView::updateBuffView(float value)
{
	if(mBuffProgress)
	{
		mBuffProgress->setPercentage(value);
	}
}

void BuffView::hideBuffView()
{
	if(mBuffProgress && mBuffBg)
	{
		mBuffProgress->removeFromParentAndCleanup(true);
		mBuffProgress = NULL;
		mBuffBg->removeFromParentAndCleanup(true);
		mBuffBg = NULL;
	}
}
////////////////////////////////////


BuffIconNode::BuffIconNode()
{
	mLock = false;
	mCampType = ECAMP_TYPE_Neutral;
	mBuffType = E_BUFF_NONE;
	mBuffView = BuffView::create(); 
}

BuffIconNode::~BuffIconNode()
{
	mBuffView->removeFromParentAndCleanup(true);
}

void BuffIconNode::setPosition(Point pos)
{
	mBuffView->setPosition(pos);
}

Point BuffIconNode::getPoistion()
{
	return mBuffView->getPosition();
}
 

void BuffIconNode::update()
{
	updateBuffTimer();
}

void BuffIconNode::start(eCampType camp, eBuffType buffType, float time)
{
	mCampType = camp;
	mBuffType = buffType;
	createBuffTimer(time);  
	mBuffView->initBuffView(buffType);
	mLock = true;
	mTotalTime = time;
}

void BuffIconNode::clearNode()
{
	mLock = false;
	
}
 

void BuffIconNode::createBuffTimer(float time)
{
	removeBuffTimer();
	std::string key = StringUtils::format(BUFFICON_KEY,mCampType, mBuffType);
	if(sgTimeCalculator()->hasKey(key))
	{
		sgTimeCalculator()->removeTimeCalcultor(key);
	}
	sgTimeCalculator()->createTimeCalcultor(key,time);
}

void BuffIconNode::updateBuffTimer()
{
	std::string key = StringUtils::format(BUFFICON_KEY,mCampType, mBuffType);
	if (sgTimeCalculator()->hasKey(key) && mBuffView)
	{
		float time = sgTimeCalculator()->getTimeLeft(key);
		float value = time/mTotalTime;
		mBuffView->updateBuffView(value * 100);
		if(time <= 0.0f)
		{
			mBuffView->hideBuffView();
			removeBuffTimer();
		}
	}
}

void BuffIconNode::removeBuffTimer()
{
	std::string key = StringUtils::format(BUFFICON_KEY,mCampType, mBuffType);
	if(sgTimeCalculator()->hasKey(key))
	{
		sgTimeCalculator()->removeTimeCalcultor(key);
	}
	mLock = false;
}
