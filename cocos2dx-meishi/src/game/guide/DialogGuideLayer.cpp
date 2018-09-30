#include "DialogGuideLayer.h"
#include "GuideManager.h"
DialogGuideLayer::DialogGuideLayer()
{
	m_CurGuideDataIndex = 0;
}

DialogGuideLayer::~DialogGuideLayer()
{

}


void DialogGuideLayer::initUI()
{ 
	BaseGuideLayer::initUI(); 
	m_UINode->setVisible(true);
}

void DialogGuideLayer::onEnter()
{
	BaseGuideLayer::onEnter(); 
	showDesc();
}

void DialogGuideLayer::onExit()
{
	BaseGuideLayer::onExit();
}

bool DialogGuideLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	sgGuideManager()->setGuideLock(false);
	return true;
}

void DialogGuideLayer::onTouchMoved(Touch *touch, Event *unused_event)
{

}

void DialogGuideLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
	changeNextDesc();
}

void DialogGuideLayer::changeNextDesc()
{ 
	m_CurGuideDataIndex += 1;
	if(m_CurGuideDataIndex >= m_GuideDataVec.size())
	{
		onGuideOver();
		return;
	}
	showDesc(); 
}

void DialogGuideLayer::onGuideCallback(bool isFinish)
{
	BaseGuideLayer::onGuideCallback(isFinish);
	if(isFinish)
	{
		doExtraAction();
		removeFromParentAndCleanup(true);
	}
}

void DialogGuideLayer::showDesc()
{ 
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	 if(!pData)
		 return;
	adjustNpcUI(pData);
	m_CurrGuideLabel->setVisible(true);
	m_CurrGuideLabel->setString(sgTextHelper(pData->getGuideTextID()));
	initMask();
}

void DialogGuideLayer::initMask()
{
	//黑幕效果
	Size size = Director::getInstance()->getWinSize();
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (pData && pData->getEnableFocus() == 1)
	{
		CCLayerColor* pColor = CCLayerColor::create(ccc4(0, 0, 0, 180));
		pColor->setContentSize(CCSizeMake(size.width, size.height));
		this->addChild(pColor); 
	} 
	else
		m_GuideTipsLable->setVisible(false);
}
////////////////////////////////////////////
StoryDialogGuideLayer::StoryDialogGuideLayer()
{
	m_CurrGuideLabel = NULL;
}


StoryDialogGuideLayer::~StoryDialogGuideLayer()
{

}


void StoryDialogGuideLayer::initUI()
{
	m_UINode = CSLoader::getInstance()->createNode("ui/GuideStroyDialog.csb");
	m_UINode->setAnchorPoint(ccp(0, 0));
	m_UINode->setPosition(ccp(150, 100));
	addChild(m_UINode, 100);
	m_UINode->setVisible(false);
	setAnchorPoint(ccp(0.5, 0.5));

	m_DialogBg = dynamic_cast<ui::ImageView* >(m_UINode->getChildByName("DialogBg"));
	if (!m_DialogBg)
		return;
	ui::Text* pText = dynamic_cast<ui::Text* >(m_UINode->getChildByName("DialogText"));
	if (!pText)
		return;
	pText->setVisible(false);
	m_CurrGuideLabel = ExRichText::create();
	m_CurrGuideLabel->setAnchorPoint(ccp(0, 1));
	m_CurrGuideLabel->ignoreContentAdaptWithSize(false);
	m_CurrGuideLabel->setSize(pText->getContentSize());
	m_CurrGuideLabel->setLimitWidth(pText->getContentSize().width);
	m_CurrGuideLabel->setText("");
	m_CurrGuideLabel->setPosition(pText->getPosition());
	m_UINode->addChild(m_CurrGuideLabel);
	
	if (!m_CurrGuideLabel)
		return; 
	m_NpcImg = dynamic_cast<ui::ImageView* >(m_UINode->getChildByName("NpcImg"));
	
	Node* pArrowNode = dynamic_cast<Node*>(m_UINode->getChildByName("NextArrowNode"));
	Armature* pArrowAni = Armature::create("fla_tishijiantou"); 
	if (!pArrowAni)
		return;
	pArrowNode->addChild(pArrowAni);
	pArrowAni->getAnimation()->play("tishijiantou_jiantou");
}
 
void StoryDialogGuideLayer::adjustNpcUI(GuideData* pData)
{
	m_UINode->setVisible(true);
}

////////////////////////////////////////////

AnimationGuideLayer::AnimationGuideLayer()
{
	m_CurGuideDataIndex = 0;
}

AnimationGuideLayer::~AnimationGuideLayer()
{

}


void AnimationGuideLayer::initUI()
{
	BaseGuideLayer::initUI();
	m_UINode->setVisible(true); 
}

void AnimationGuideLayer::showDesc()
{
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	adjustNpcUI(pData);
	m_CurrGuideLabel->setString(sgTextHelper(pData->getGuideTextID()));
	m_GuideTipsLable->setVisible(true); 
	initMask();
}

void AnimationGuideLayer::initMask()
{
	//黑幕效果
	Size size = Director::getInstance()->getWinSize();
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (pData && pData->getEnableFocus() == 1)
	{
		CCLayerColor* pColor = CCLayerColor::create(ccc4(0, 0, 0, 180));
		pColor->setContentSize(CCSizeMake(size.width, size.height));
		this->addChild(pColor); 
	}
	else
		m_GuideTipsLable->setVisible(false);

}


void AnimationGuideLayer::onEnter()
{
	BaseGuideLayer::onEnter();
	doExtraAction();
	showDesc();
}

void AnimationGuideLayer::onExit()
{
	BaseGuideLayer::onExit();
}

bool AnimationGuideLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	sgGuideManager()->setGuideLock(false);
	return true;
}

void AnimationGuideLayer::onTouchMoved(Touch *touch, Event *unused_event)
{

}

void AnimationGuideLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	//if (sgUserObj()->getShopGuideNum() > 0 || sgUserObj()->getGlyphStuffGuideNum() > 0)
	if (sgUserObj()->getPersonalInfo()->shopguide() > 0 || sgUserObj()->getPersonalInfo()->glyphguide() > 0)
	{
		onGuideOver();
	}  
}

void AnimationGuideLayer::onGuideCallback(bool isFinish)
{
	BaseGuideLayer::onGuideCallback(isFinish);
	if(isFinish)
	{
		removeFromParentAndCleanup(true);
	}
}







