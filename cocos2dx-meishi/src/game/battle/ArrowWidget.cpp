#include "ArrowWidget.h"
#include "ui/UIHelper.h"

#include "cocostudio/ActionTimeline/CSLoader.h"
#include "core/ResManager.h"
#define  PI 3.1415926

ArrowWidget* ArrowWidget::create()
{
	ArrowWidget* pret = new ArrowWidget();
	if (pret && pret->initUI())
	{
		pret->autorelease();
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

bool ArrowWidget::initUI()
{

	//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_Battle_arrow.png", "fla_Battle_arrow.plist", "fla_Battle_arrow.xml");
	mBattleArrowTou = sgResManager()->getAramatur("fla_Battle_arrow_tou");//cocostudio::Armature::create("fla_Battle_arrow_tou");

	addChild(mBattleArrowTou);
	reset();

	
	return true;
}

void ArrowWidget::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{

}

void ArrowWidget::stat2end(Vec2 star, Vec2 end, bool isFaceBuild)
{
	Vec2 pos = end - star;
	float distant = sqrt(pow(pos.x,2) + pow(pos.y,2));

	if (distant < 80)return;
	mBattleArrowTou->setVisible(true);

	float pScale = (distant )*1.0/(mBattleArrowTou->getContentSize().width*1.0);
	pScale = isFaceBuild?(pScale-0.05*pScale):pScale;
	pScale = pScale<0?0:pScale;
	this->setScaleX(pScale);

	this->setPosition(star);
	float pRotation = atan2(pos.x, pos.y) * 180 / PI - 90;
	this->setRotation(pRotation);
	
	Vec2 posHead = end - star;

	/*float pLength1 = (pScale-1)*170;
	float pMx =(posHead.x/distant)*pLength1;
	float pMy =(posHead.y/distant)*pLength1;*/
	/*mBattleArrowTou->setPosition(pMx,
		pMy);*/
	if (!mBattleArrowTou->getAnimation()->isPlaying())
	{
		mBattleArrowTou->getAnimation()->playWithIndex(0);
	}
	
	///////////////////////////////////
	
	//{
	//	float pJiajiao = pRotation <0?-pRotation:pRotation;

	//	Vec2 pVec = Vec2(0,0);
	//	float pLength = (pScale-1)*mArrowEnd->getContentSize().width;
	//	pVec.x= cos(pJiajiao*PI/180);
	//	pVec.y= sin(pJiajiao*PI/180);
	//	log("Rotation:%f MOVE: x:%f,y:%f",pJiajiao,pVec.x,pVec.y);
	//	pVec.x*=pLength;
	//	pVec.y*=pLength;
	//	this->setPosition(pVec);
	//}
	//
	//mArrowHead->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.3,1.4,1.0),ScaleTo::create(0.3,1.0,1.0),NULL)));
	//mArrowEnd->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.3,pScale+0.3,1.0),ScaleTo::create(0.3,pScale,1.0),NULL)));
}

void ArrowWidget::reset()
{
	/*mRoot->setVisible(false);
	mArrowEnd->stopAllActions();
	mArrowHead->stopAllActions();*/
	mBattleArrowTou->setPosition(Vec2::ZERO);
	mBattleArrowTou->getAnimation()->stop();
	mBattleArrowTou->setVisible(false);
}

