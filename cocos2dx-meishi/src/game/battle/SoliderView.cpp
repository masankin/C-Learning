#include "SoliderView.h"
#include "Solider.h"

#include "game/data/TableManager.h"
#include "GamePrecedure.h"
#include "battle/BattleControl.h"


SoliderView* SoliderView::create(Solider* owner)
{
	SoliderView* pRet = new SoliderView();
	if (pRet && pRet->init(owner))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return pRet;
	}

}

SoliderView::SoliderView() : mOwner(nullptr)
, mSprite(nullptr)
{

}


SoliderView::~SoliderView()
{
}


bool SoliderView::init(Solider* owner)
{
	if (!Node::init() || nullptr == owner)
		return false;

	mOwner = owner;
	mSprite = Sprite::create();
	if (mSprite)
	{
		mSprite->setContentSize(Size(45, 45));
		this->addChild(mSprite);
	}

	return true;
}


void SoliderView::cleanup()
{
	if (mSprite)
	{
		this->removeChild(mSprite);
		mSprite = nullptr;
	}
	
	mOwner = nullptr;

	Node::cleanup();
}


void SoliderView::pause()
{
	Node::pause();

	mSprite->pause();
}


void SoliderView::resume()
{
	mSprite->resume();

	Node::resume();
}


void SoliderView::setWorldPosition(const cocos2d::Point& worldPos)
{
	this->setWorldPosition(worldPos.x, worldPos.y);
}


void SoliderView::setWorldPosition(float worldX, float worldY)
{
	Node* parent = this->getParent();
	if (nullptr == parent)
		return;

	this->setPosition(parent->convertToNodeSpace(Point(worldX, worldY)));
}


Point SoliderView::getWorldPosition()
{
	const Node* parent = this->getParent();
	if (nullptr == parent)
		return Point::ZERO;

	return parent->convertToWorldSpace(this->getPosition());
}

void SoliderView::refresh()
{
	if (nullptr == mOwner || nullptr == mSprite)
		return;

	auto campType = mOwner->getSrcCamp();
	int level = mOwner->getLevel(); 

	std::string filename = StringUtils::format("%s_%02d_%02d", sgTableManager()->getAnimationNameByCamp(campType).c_str(), level, mOwner->getAttribType());

	Animation* pAni = AnimationCache::getInstance()->getAnimation(filename.c_str());
	if (pAni && !pAni->getFrames().empty())
	{
		SpriteFrame* pFrame = pAni->getFrames().at(0)->getSpriteFrame();
		if (pFrame)
		{
			mSprite->setTexture(pFrame->getTexture());
			auto pAction = Speed::create(RepeatForever::create(Animate::create(pAni)), mOwner->getSpeedScale());
			if (pAction)
			{
				pAction->setTag(SoliderView::ActionTagMain);
				mSprite->stopActionByTag(SoliderView::ActionTagMain);
				mSprite->runAction(pAction);
			}
		}
	}

}


void SoliderView::face(bool flippedX)
{
	if (mSprite)
		mSprite->setFlippedX(flippedX);
}


void SoliderView::show(bool val)
{
	if (mSprite)
		mSprite->setVisible(val);
}


void SoliderView::setGray(bool val)
{
	if (val)
		mSprite->setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_GRAYSCALE));
	else
		mSprite->setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));
}

// void SoliderView::doBombHit()
// {
// 	auto armature = cocostudio::Armature::create("fla_eqp041bz");
// 	if (nullptr == armature)
// 		return;
// 
// 	armature->getAnimation()->play("born");
// 
// 	auto movementCallback = [this](cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName){
// 		if (type == cocostudio::COMPLETE)
// 		{
// 			// Important!!!!! release armature
// 			pAr->removeFromParent();
// 
// 			this->doDead();
// 		}
// 	};
// 
// 	armature->getAnimation()->setMovementEventCallFunc(movementCallback);
// 
// 	/*auto pos = this->getPosition();
// 	pos.y -= 10.0f;
// 	armature->setPosition(pos);
// 
// 	armature->setLocalZOrder(this->getLocalZOrder());
// 
// 	if (this->getParent())
// 		this->getParent()->addChild(armature);*/
// 
// 	auto pos = armature->getPosition();
// 	pos.y -= 10.0f;
// 	armature->setPosition(pos);
// 
// 	this->addChild(armature);
// }


void SoliderView::playDeadAni()
{
	auto armature = cocostudio::Armature::create("fla_ghost");
	if (nullptr == armature)
		return;

	armature->getAnimation()->play("play");

	auto movementCallback = [this](cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName){
		if (type == cocostudio::COMPLETE)
		{
			// Important!!!!! release armature
			pAr->removeFromParent();

			// remove view
			//this->removeFromParent();
		}
	};
	armature->getAnimation()->setMovementEventCallFunc(movementCallback);

	/*auto pos = this->getPosition();
	pos.y -= 10.0f;
	armature->setPosition(pos);

	armature->setLocalZOrder(this->getLocalZOrder());

	if (this->getParent())
	this->getParent()->addChild(armature);*/

	auto pos = this->getPosition();
	pos.y -= 10.0f;
	armature->setPosition(pos);

	this->addChild(armature);
}


std::string SoliderView::getFrameName(int type, eCampType camp, int level)
{ 
	std::string key;
	switch (type)
	{
	case 0:
	default:
		key = "%s%d"; 
		break;
	case 1:
		key = "%sSpeed%d"; 
		break; 
	} 
	return StringUtils::format(key.c_str(), sgTableManager()->getAnimationNameByCamp(camp).c_str(), level);
}

void SoliderView::playUpgradeAni()
{
	if (mSprite)
	{
		Sprite* pRing = Sprite::create("build/buildUpgrade.png");
		pRing->setBlendFunc(BlendFunc::ADDITIVE);

		//Size contentSize = mSprite->getContentSize();
		Vec2 pos = mSprite->getPosition() + Vec2(0, 100);// + Vec2(0, contentSize.height / 2);
		pRing->setPosition(pos);
		pRing->setScaleY(5);
		pRing->setScaleX(0.6);
		pRing->setOpacity(0);
		this->addChild(pRing,mSprite->getLocalZOrder());

		
		pRing->runAction(Sequence::create(
			Spawn::create(ScaleTo::create(0.3, 1.3), MoveBy::create(0.3, Vec2(0, -100)),FadeIn::create(0.3), nullptr),
			DelayTime::create(1.0f),
			FadeOut::create(1.5f),
			RemoveSelf::create(),
			nullptr));
	}
}
