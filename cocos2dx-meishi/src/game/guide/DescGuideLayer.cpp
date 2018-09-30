#include "DescGuideLayer.h"
#include "GamePrecedure.h"
#include "core/TimeCalculator.h"
#include "battle/BattleControl.h"
#include "guide/GuideManager.h" 
#define DESCGUIDE_KEY "DescGuideLayerWait" 
DescGuideLayer::DescGuideLayer()
{
	m_CurGuideDataIndex = 0;
	mCanControl = false;
}

DescGuideLayer::~DescGuideLayer()
{

}
 

void DescGuideLayer::initUI()
{ 
	 mUIDialogNode = CSLoader::getInstance()->createNode("GuideDescDialog.csb");
	 addChild(mUIDialogNode,10); 

	 //此处关闭按钮替换成按钮动画
	 ui::Button* closeButton = dynamic_cast<ui::Button* >(mUIDialogNode->getChildByName("Button_3"));
	 closeButton->setVisible(false);

	 ui::Layout* pTouchPanel = dynamic_cast<ui::Layout* >(mUIDialogNode->getChildByName("clickPanel"));
	 pTouchPanel->addClickEventListener(CC_CALLBACK_0(DescGuideLayer::onCloseDialogCallback, this));
	 pTouchPanel->setSwallowTouches(true);   

	 Armature* pBtnAni = Armature::create("fla_xinshou");
	 mUIDialogNode->addChild(pBtnAni, pTouchPanel->getZOrder());
	 pBtnAni->setPosition(closeButton->getPosition());
	 pBtnAni->setScale(0.8f);
	 pBtnAni->getAnimation()->play("play",-1, 1);

	 //文本描述区域 
	 mDescTips = dynamic_cast<ui::Text* >(mUIDialogNode->getChildByName("Text_1"));
	 ui::Text* pText = dynamic_cast<ui::Text*>(mUIDialogNode->getChildByName("GuideLabel"));  
	 if (!pText)
		 return;
	 pText->setVisible(false);

	 mDescGuideText = ExRichText::create();
	 mDescGuideText->setAnchorPoint(ccp(0, 1));
	 mDescGuideText->ignoreContentAdaptWithSize(false);
	 mDescGuideText->setSize(pText->getContentSize());
	 mDescGuideText->setLimitWidth(pText->getContentSize().width);
	 mDescGuideText->setText("");
	 mDescGuideText->setPosition(pText->getPosition());
	 mUIDialogNode->addChild(mDescGuideText);

	 Size size = Director::getInstance()->getWinSize();
	 CCLayerColor* pColor = CCLayerColor::create(ccc4(0,0,0,180));
	 pColor->setContentSize(CCSizeMake(size.width, size.height));
	 this->addChild(pColor);

	 Node* pTipsNode = mUIDialogNode->getChildByName("Text_1");
	 if(pTipsNode)
		 pTipsNode->setVisible(false);
}

void DescGuideLayer::onGuideCallback(bool isFinish)
{
	BaseGuideLayer::onGuideCallback(isFinish);
	if(isFinish)
	{
		doExtraAction(); 
		removeFromParentAndCleanup(true);
	}
}


void DescGuideLayer::showDesc()
{
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if(!pData)
		return;
	mDescGuideText->setString(sgTextHelper(pData->getGuideTextID())); 
	mCanControl = true;  
}

void DescGuideLayer::onEnter()
{
	BaseGuideLayer::onEnter();
	loadGuideType((eDescGuideType)m_GuideType);    
	showDesc();
}



void DescGuideLayer::onExit()
{
	BaseGuideLayer::onExit(); 
}



void DescGuideLayer::onCloseDialogCallback()
{
	m_CurGuideDataIndex += 1;
	if(m_CurGuideDataIndex >= m_GuideDataVec.size())
		onGuideOver();
	else
		showDesc();
} 

void DescGuideLayer::loadGuideType(eDescGuideType type)
{
	if(type == 0)
	{
		mUIDialogNode->setVisible(false);
		return;
	}
	dynamic_cast<Node* >(mUIDialogNode->getChildByTag(E_DESCTYPE_MORALE))->setVisible(false);
	dynamic_cast<Node* >(mUIDialogNode->getChildByTag(E_DESCTYPE_TURRET))->setVisible(false);
	dynamic_cast<Node* >(mUIDialogNode->getChildByTag(E_DESCTYPE_LEVELUP))->setVisible(false);
	dynamic_cast<Node* >(mUIDialogNode->getChildByTag(E_DESCTYPE_INSTITUTE))->setVisible(false);
	dynamic_cast<Node* >(mUIDialogNode->getChildByTag(type))->setVisible(true);
	 
}
 

bool DescGuideLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	sgGuideManager()->setGuideLock(false);
	return true;
}

void DescGuideLayer::onTouchMoved(Touch *touch, Event *unused_event)
{

}

void DescGuideLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
	 
}
