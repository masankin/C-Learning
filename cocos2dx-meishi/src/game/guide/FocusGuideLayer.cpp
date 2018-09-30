#include "FocusGuideLayer.h"
#include "GamePrecedure.h" 
#include "battle/BattleControl.h"
#include "guide/GuideManager.h"
#define FOCUS_RADIUS 1366
#define FOCUS_SUBCOUNT 2500
#define FOCUS_MAXSUBRADIUS 65
#define FOCUS_PER_WAITIME 0.5f
FocusGuideLayer::FocusGuideLayer()
{        
	m_VisitRange = FOCUS_RADIUS;
	m_VisitRangeEx = FOCUS_RADIUS;
	m_WidthVelocity = FOCUS_SUBCOUNT;
	m_HeightVelocity = FOCUS_SUBCOUNT;;
	m_StartFog = false;  
	m_StencilNode = NULL;
	m_HasSendMessage = false;
	m_DrawNodeVec.clear();  
	m_FogRange = Size(FOCUS_MAXSUBRADIUS, 0);
	m_GuideAnimationType = 1;
}

FocusGuideLayer::~FocusGuideLayer()
{ 
	for(int i = 0; i < m_DrawNodeVec.size(); i++)
	{
		if(NULL != m_DrawNodeVec.at(i))
		{
			m_DrawNodeVec.at(i) = NULL;
		}		 
	}
	m_DrawNodeVec.clear();
}

 

void FocusGuideLayer::initUI()
{
	BaseGuideLayer::initUI();
	//创建聚焦对象
	CCSize size = Director::getInstance()->getWinSize();
	m_ClippingNode = ClippingNode::create();
	addChild(m_ClippingNode, 10);
	m_ClippingNode->setInverted(true);
	m_ClippingNode->setVisible(false);
	m_ClippingNode->setContentSize(CCSizeMake(size.width, size.height));

	CCLayerColor* pColor = CCLayerColor::create(ccc4(0, 0, 0, 180));
	pColor->setContentSize(CCSizeMake(size.width, size.height));
	m_ClippingNode->addChild(pColor);

	m_StencilNode = CCNode::create();
	m_StencilNode->setAnchorPoint(ccp(0.5, 0.5));
	m_ClippingNode->setStencil(m_StencilNode);
}

void FocusGuideLayer::onGuideCallback(bool isFinish)
{
	BaseGuideLayer::onGuideCallback(isFinish);
	if (isFinish == true)
	{
		//如果有定义自动调用的方法则发送消息
		doExtraAction();
		removeFromParentAndCleanup(true);
	}
		
}

bool FocusGuideLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	sgGuideManager()->setGuideLock(false);
	return checkTouchSwallow(touch->getLocation());
}

void FocusGuideLayer::initAnimation(int type)
{
	m_GuideAnimationType = type;
	//创建动画对象 
	m_GuideAnimation = Armature::create("fla_yingdao");
	m_GuideAnimation->getAnimation()->setMovementEventCallFunc(
		this, 
		movementEvent_selector(FocusGuideLayer::animationCallback));
	addChild(m_GuideAnimation, m_UINode->getZOrder() + 10);
	m_GuideAnimation->getAnimation()->stop();
	m_GuideAnimation->setOpacity(0);
}


void FocusGuideLayer::onDoGuide(float delta)
{ 
	if (m_GuideState == E_GuideStart)
		doStartAction(delta);
	else if (m_GuideState == E_GuideDoing)
		doDoingAction(delta);
	else if (m_GuideState == E_GuideEnd)
		doEndAction(delta);
	else if (m_GuideState == E_GuideFinal)
		doFinalAction(delta);
	else if (m_GuideState == E_GuideWait)
		doWaitAction(delta);
}

void FocusGuideLayer::doNextState(E_GuideState state)
{
	BaseGuideLayer::doNextState(state);  
}  

void FocusGuideLayer::createFocusLayer(GuideData* pData)
{
	m_GuideNode = static_cast<Node*>(m_GuideParentNode->getChildByName(pData->getGuideNodeID()));
	if (!m_GuideNode)
	{
		sgGuideManager()->unRegistGuideMessage(true);
		return;
	} 
	adjustNpcUI(pData);
	resetClipNode(pData); 
}

void FocusGuideLayer::resetClipNode(GuideData* pData)
{ 
	m_GuideTipsLable->setVisible(true); 
	m_NpcBg->setVisible(true);
	m_LabelBg->setVisible(true);
	m_UINode->setVisible(true);
	m_CurrGuideLabel->setVisible(true);
	m_CurrGuideLabel->setString(sgTextHelper(pData->getGuideTextID()));
	
	//添加新的聚焦绘制节点
	CCSize size = Director::getInstance()->getWinSize();
	DrawNode* pDrawNode = DrawNode::create();
	pDrawNode->setPosition(ccp(0, 0));
	pDrawNode->setContentSize(Size(size.width, size.height)); 
	m_StencilNode->addChild(pDrawNode);
	m_DrawNodeVec.push_back(pDrawNode); 
	m_FogRange = getClipRange(pData);
	m_VisitRange = FOCUS_RADIUS;
	m_VisitRangeEx = FOCUS_RADIUS;
	m_WidthVelocity = FOCUS_SUBCOUNT;
	float time = (FOCUS_RADIUS - m_FogRange.width) / (float)m_WidthVelocity;
	m_HeightVelocity = (FOCUS_RADIUS - m_FogRange.height) / time;
}

void FocusGuideLayer::updateFogLayer(float dt)
{ 
	m_ClippingNode->setVisible(true);
	DrawNode* pDrawNode = m_DrawNodeVec.at(m_CurGuideDataIndex);	 
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (!pData || !m_GuideNode || !pDrawNode || m_DrawNodeVec.size() <= 0)
	{
		sgGuideManager()->unRegistGuideMessage(true);
		return;
	} 
	Point pos = pData->getGuideNodePos() + m_GuideNode->getPosition();
	if (drawClipNode(pDrawNode, dt, pData->getFocusType(), pos))
		doNextState(E_GuideEnd);
}

bool FocusGuideLayer::drawClipNode(DrawNode* pDrawNode, float dt, int drawType, Point pos)
{
	pDrawNode->clear();
	bool updateOver = false;
	if (drawType == E_FOCUS_CIRCLE)
	{
		if (m_VisitRange <= m_FogRange.width)
		{
			m_VisitRange = m_FogRange.width;
			updateOver = true;
		} 
		drawCircle(pDrawNode, pos.x, pos.y, m_VisitRange); 
	}
	else if (drawType == E_FOCUS_RECT)
	{ 

		bool ret1 = false, ret2 = false;
		if (m_VisitRange <= m_FogRange.width)
		{
			m_VisitRange = m_FogRange.width;
			ret1 = true;
		}
		if (m_VisitRangeEx <= m_FogRange.height)
		{
			m_VisitRangeEx = m_FogRange.height;
			ret2 = true;
		}
		drawRect(pDrawNode, pos.x, pos.y, m_VisitRange, m_VisitRangeEx);
		if (ret1 && ret2)
			updateOver = true;
	} 
	if (!updateOver)
	{ 
		m_VisitRange -= m_WidthVelocity * dt;
		m_VisitRangeEx -= m_HeightVelocity * dt;
	}
	else
		m_AnimationOver = true;
	return updateOver;
}
 

void FocusGuideLayer::drawCircle(DrawNode* pDrawNode, float x, float y, float r, bool atOnce)
{ 
	const int nCount = 80;
	const float coef = 2.0f * (float)M_PI / nCount;
	CCPoint circle[nCount];
	for (unsigned int i = 0; i < nCount; i++) {
		float rads = i*coef;
		circle[i].x = x + r * cosf(rads) * 0.8;
		circle[i].y = y + r * sinf(rads) * 0.8;
	}
	pDrawNode->drawPolygon(circle, nCount, ccc4f(1, 1, 1, 1), 0, ccc4f(1, 1, 1, 1)); 
}

void FocusGuideLayer::drawRect(DrawNode* pDrawNode, float x, float y, float width, float height, bool atOnce)
{
	Point rectArr[4];
	if (atOnce)
	{
		rectArr[0] = ccp(x - m_FogRange.width / 2, y + m_FogRange.height / 2);
		rectArr[1] = ccp(x + m_FogRange.width / 2, y + m_FogRange.height / 2);
		rectArr[2] = ccp(x + m_FogRange.width / 2, y - m_FogRange.height / 2);
		rectArr[3] = ccp(x - m_FogRange.width / 2, y - m_FogRange.height / 2);
	}
	else
	{
		rectArr[0] = ccp(x - width / 2, y + height / 2);
		rectArr[1] = ccp(x + width / 2, y + height / 2);
		rectArr[2] = ccp(x + width / 2, y - height / 2);
		rectArr[3] = ccp(x - width / 2, y - height / 2);
	}
	pDrawNode->drawPolygon(rectArr, 4, ccc4f(1, 1, 1, 1), 0, ccc4f(1, 1, 1, 1)); 
}

float FocusGuideLayer::getWaitTime(GuideData* pData)
{
	return pData->getWaitTime();
}

Size FocusGuideLayer::getClipRange(GuideData* pData)
{
	if (pData->getFocusType() == E_FOCUS_CIRCLE)
	{
		int width = atoi(pData->getGuideFrameSize().c_str());
		return Size(width, width);
	}
	else if (pData->getFocusType() == E_FOCUS_RECT)
	{
		std::string tStr = pData->getGuideFrameSize();
		std::vector<std::string > fogStrVec = splitStr(tStr, "#");
		if (fogStrVec.size() == 2)
		{
			int width = atoi(fogStrVec.at(0).c_str());
			int height = atoi(fogStrVec.at(1).c_str());
			return Size(width, height);
		}
	}
	return Size(100, 100);
}

void FocusGuideLayer::doStartAction(float delta)
{
	if (m_WaitTime > 0.0f)
	{
		m_WaitTime -= delta;
		return;
	}
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (!pData || pData->getGuideMode() != E_GUIDE_FOCUS)
		return;
	 
	m_StartFog = true; 

	createFocusLayer(pData);	
	doNextState(E_GuideDoing);
}

void FocusGuideLayer::doDoingAction(float delta)
{
	updateFogLayer(delta);
}

void FocusGuideLayer::doEndAction(float delta)
{
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	m_WaitTime = getWaitTime(pData);

	m_CurGuideDataIndex += 1;
	//如果是最后一个节点，那么跳转最终状态
	if (m_CurGuideDataIndex == m_GuideDataVec.size())
	{
		doNextState(E_GuideFinal);
		return;
	}  
	doNextState(E_GuideStart); 
}


void FocusGuideLayer::doFinalAction(float delta)
{ 
	GuideData* pData = m_GuideDataVec.at(m_GuideDataVec.size() - 1);
	m_WaitTime = getWaitTime(pData);
	doNextState(E_GuideWait);
}

void FocusGuideLayer::doWaitAction(float delta)
{
	if (m_WaitTime > 0.0f)
	{
		m_WaitTime -= delta;
		return;
	}
	m_StencilNode->removeAllChildrenWithCleanup(true);
	m_DrawNodeVec.clear(); 
	m_CurGuideDataIndex = 0;
	doNextState(E_GuideStart);
}

bool FocusGuideLayer::checkTouchSwallow(Point touchPos)
{
	m_HasSendMessage = true;	
	if (m_CurGuideDataIndex == m_GuideDataVec.size())
		m_CurGuideDataIndex = m_GuideDataVec.size()-1;
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (!pData)
		return false;

	if (m_IsEnablePause == true)
		sgBattleControl()->doGamePause(false); 
	onGuideOver(); 
	return true;
}

