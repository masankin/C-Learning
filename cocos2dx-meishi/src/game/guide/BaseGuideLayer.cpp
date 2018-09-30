 
#include "BaseGuideLayer.h" 
#include "core/TimeCalculator.h"
#include "battle/BattleControl.h"
#include "Constant.h"
#include "guide/GuideManager.h"

BaseGuideLayer::BaseGuideLayer()
{ 
	m_CurrGuideLabel = NULL;
	m_LabelBg = NULL;
	m_Painting = NULL; 
	m_AnimationOver = false;
	m_GuideAnimation = NULL;  
	m_WaitTime = 0.0f;
	m_AnimationState = E_GuideActionState0;
	m_GuideDataVec.clear();
	m_CurGuideDataIndex = 0; 
	m_GuideNode = NULL;
	m_GuideID = 0;
	m_DragNode = NULL;
	m_GuideParentNode = NULL;
	m_SceneNode = NULL;
	m_IsEnablePause = false;
	m_IsAdjust = false;
	m_GuideAnimationType = 0;
	m_GuideAnimationAngle = 0;
	m_GuideState = E_GuideStart;
	m_IsReady = false;
}

BaseGuideLayer::~BaseGuideLayer()
{
	//内存清理由Manager负责
	for(int i = 0; i < m_GuideDataVec.size(); i++)
	{ 
		if(m_GuideDataVec.at(i) != NULL)
			m_GuideDataVec.at(i) = NULL;
	}
	m_GuideDataVec.clear();
}

bool BaseGuideLayer::init()
{
	bool isRet = false;
	do 
	{
		CC_BREAK_IF(Layer::init() == false); 		
		initUI();		
		addListener();
		setTag(TAG_GUIDELAYER);		 
		isRet = true;
	} while (0);
	return isRet;
}

void BaseGuideLayer::initUI()
{
	m_UINode = CSLoader::getInstance()->createNode("ui/GuideTalkNode.csb");
	m_UINode->setAnchorPoint(ccp(0,0));
	m_UINode->setPosition(ccp(150,100));
	addChild(m_UINode, 100);
	m_UINode->setVisible(false); 
	setAnchorPoint(ccp(0.5,0.5));

	//添加对话框背景		
	m_LabelBg = dynamic_cast<Sprite* >(m_UINode->getChildByName("uiAtlas_Battle_tip_1"));
	m_LabelBg->setVisible(false); 
	m_BaseLabelBgPos = m_LabelBg->getPosition();

	m_NpcBg = dynamic_cast<Sprite* >(m_UINode->getChildByName("uiAtlas_Battle_npc_2"));
	m_NpcBg->setVisible(false);  
	m_BaseNpcBgPos = m_NpcBg->getPosition();

	//添加对话框文本
	ui::Text* pText = dynamic_cast<ui::Text* >(m_UINode->getChildByName("GuideLabel"));
	pText->setText("");
	pText->setVisible(false);
	m_BaseLabelPos = pText->getPosition();

	m_CurrGuideLabel = ExRichText::create();
	m_CurrGuideLabel->setAnchorPoint(ccp(0,1));
	m_CurrGuideLabel->ignoreContentAdaptWithSize(false);
	m_CurrGuideLabel->setSize(pText->getContentSize());
	m_CurrGuideLabel->setLimitWidth(pText->getContentSize().width); 
	m_CurrGuideLabel->setText("");
	m_CurrGuideLabel->setPosition(m_BaseLabelPos);
	m_UINode->addChild(m_CurrGuideLabel);

	m_GuideTipsLable = dynamic_cast<ui::Text* >(m_UINode->getChildByName("GuideTips"));
	m_GuideTipsLable->setVisible(false); 
	Point pos1 = ccp(0, m_GuideTipsLable->getPosition().y);
	Size  size1 = Director::getInstance()->getWinSize();
	m_GuideTipsLable->setPosition(ccp(490,0));

#ifdef MGAME_PLATFORM_MPLUS
#define TEXT_GUIDE_CLICK  378
	m_GuideTipsLable->setText(sgTextHelper(TEXT_GUIDE_CLICK));
#endif
}

void BaseGuideLayer::addListener()
{
	//添加触摸监听
	m_TouchListener = EventListenerTouchOneByOne::create();
	m_TouchListener->onTouchBegan = CC_CALLBACK_2(BaseGuideLayer::onTouchBegan, this);
	m_TouchListener->onTouchEnded = CC_CALLBACK_2(BaseGuideLayer::onTouchEnded, this);
	m_TouchListener->onTouchMoved = CC_CALLBACK_2(BaseGuideLayer::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_TouchListener, this);
	m_TouchListener->setSwallowTouches(true);
}


void BaseGuideLayer::onEnter()
{ 
	Layer::onEnter(); 
}

void BaseGuideLayer::onExit()
{ 
	Layer::onExit();	
}

void BaseGuideLayer::update(float delta)
{  
	onDoGuide(delta);
}


void BaseGuideLayer::onReadyGuide()
{ 
	if(m_IsEnablePause == true)
		sgBattleControl()->doGamePause(true);
}

void BaseGuideLayer::onGuideCallback(bool isFinish)
{ 
	sgGuideManager()->setGuideLock(false);
	if(!isFinish)		
	{
		m_AnimationState = E_GuideActionState0;
		return;
	}
	if(m_CurGuideDataIndex >= m_GuideDataVec.size())
		m_CurGuideDataIndex = m_GuideDataVec.size() - 1;

	if(m_IsEnablePause == true)
		sgBattleControl()->doGamePause(false);
	
}

void BaseGuideLayer::addGuideData(GuideData* tData)
{
	m_GuideDataVec.push_back(tData);
} 

bool BaseGuideLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	return true;
}

void BaseGuideLayer::initDialogUI(int type, float angle, Point pos)
{
	initAnimation(type);
	setGuideAnimationAngle(angle);
	setGuideAnimationPos(pos);
}

void BaseGuideLayer::initGuideInfo(int guideID, int isPause, Node* sceneNode, Node* parentNode, Point pos)
{
	setIsEnablePause(isPause == 0 ? false : true);
	setGuideID(guideID);
	setGuideParentNode(parentNode);
	setSceneNode(sceneNode); 
	setPosition(pos);
	onReadyGuide();  
}
 

void BaseGuideLayer::adjustNpcUI(GuideData* pData)
{  
	m_NpcBg->setVisible(true);
	m_LabelBg->setVisible(true);
	m_UINode->setVisible(true);
	m_CurrGuideLabel->setVisible(true);

	int uiState = pData->getNpcTurn();
	if(uiState == 1)
	{
		if(m_IsAdjust == false)
		{
			m_IsAdjust = true;
			m_NpcBg->setScaleX(-1);
			m_LabelBg->setScaleX(-1); 
			m_BaseNpcBgPos = m_BaseNpcBgPos + ccp(496,0);
			m_NpcBg->setPosition(m_BaseNpcBgPos);  
		} 
	}
	else if(uiState == 2)
	{
		m_NpcBg->setVisible(false);
		m_LabelBg->setVisible(false); 
	}
	std::string posStr = pData->getNpcPos(); 
	Point pos = ccp(0, 0);
	std::vector<std::string > strVec =  splitStr(posStr, "#");
	if(strVec.size() == 2)
	{
		pos.x = atoi(strVec.at(0).c_str());
		pos.y = atoi(strVec.at(1).c_str());
	}
	m_NpcBg->setPosition(m_BaseNpcBgPos + pos);
	m_LabelBg->setPosition(m_BaseLabelBgPos + pos); 
	m_CurrGuideLabel->setPosition(m_BaseLabelPos + pos);
	
}

void BaseGuideLayer::doNextState(E_GuideState state)
{
	m_GuideState = state;
}

void BaseGuideLayer::doExtraAction()
{
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (pData && pData->getExtraFunc() > 0)
	{
		int guideID = pData->getGuideID();
		int funID = pData->getExtraFunc();

		char params[32] = { 0, };
		sprintf(params, "%d %d %d", 
			guideID, 
			funID, pData->getExtraParam());
		sgMessageManager()->sendMessage(EMT_GUIDE_TIPS, params);  
	}
	
}

void BaseGuideLayer::onGuideOver()
{
// 	MessageGuideData* pMessage = new MessageGuideData();
// 	pMessage->setState(E_GUIDEMESSAGE_OVER);
// 	sgMessageManager()->sendMessage(pMessage);   
	sgGuideManager()->unRegistGuideMessage();
}


void BaseGuideLayer::initMask()
{

}

 
