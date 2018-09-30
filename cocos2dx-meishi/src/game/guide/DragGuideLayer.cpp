#include "DragGuideLayer.h"
#include "battle/BattleControl.h"
#include "guide/GuideManager.h"
#define DRAG_MOVE_SPEED 475.0;
DragGuideLayer::DragGuideLayer()
{	
	m_Painting = NULL;	
	m_ShowArrow = false;  
}

DragGuideLayer::~DragGuideLayer()
{
	//内存清理由Manager负责 
	m_GuideDataVec.clear();
}

bool DragGuideLayer::init()
{
	bool isRet = false;
	do 
	{
		CC_BREAK_IF(!BaseGuideLayer::init()); 
		//创建拖曳箭头
		m_Painting = ArrowWidget::create();
		addChild(m_Painting, m_LabelBg->getZOrder() - 10);
		m_Painting->setVisible(false);
		isRet = true;
	} while (0);
	return isRet;
}
 
 
void DragGuideLayer::onDoGuide(float delta)
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

void DragGuideLayer::createDragLayer(GuideData* pData)
{
	adjustNpcUI(pData);
	resetAniAction(pData);
}

void DragGuideLayer::doStartAction(float delta)
{
	if (m_WaitTime >= 0.0f)
	{
		m_WaitTime -= delta;
		return;
	}
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (!pData || pData->getGuideMode() != E_GUIDE_DRAG)
		return;
	createDragLayer(pData);
	doNextState(E_GuideDoing);
}

void DragGuideLayer::doDoingAction(float delta)
{
	//绘制调兵箭头
	if (m_ShowArrow)
		m_Painting->stat2end(m_ArrowStartPos, m_GuideAnimation->getPosition());
}

void DragGuideLayer::doEndAction(float delta)
{
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	m_WaitTime = getWaitTime(pData);

	m_CurGuideDataIndex += 1;
	m_GuideAnimation->runAction(FadeOut::create(0.5f));
	//如果是最后一个节点，那么跳转最终状态
	if (m_CurGuideDataIndex == m_GuideDataVec.size())
	{
		doNextState(E_GuideFinal);
		return;
	}
	doNextState(E_GuideStart);
}


void DragGuideLayer::doFinalAction(float delta)
{
	GuideData* pData = m_GuideDataVec.at(m_GuideDataVec.size() - 1);
	m_WaitTime = getWaitTime(pData);  
	doNextState(E_GuideWait);
}

void DragGuideLayer::doWaitAction(float delta)
{
	if (m_WaitTime > 0.0f)
	{
		m_WaitTime -= delta;
		return;
	} 
	m_CurGuideDataIndex = 0;
	doNextState(E_GuideStart);
}


void DragGuideLayer::onGuideCallback(bool isFinish)
{
	BaseGuideLayer::onGuideCallback(isFinish);
	if(isFinish)
	{
		doExtraAction(); 
		removeFromParentAndCleanup(true);
		sgGuideManager()->setGuideLock(false);
	}
		
}

void DragGuideLayer::addGuideData(GuideData* tData)
{
	m_GuideDataVec.push_back(tData);
}
 

bool DragGuideLayer::onTouchBegan(Touch *touch, Event *unused_event)
{ 
	return true;
}

 
void DragGuideLayer::initAnimation(int type)
{ 
	//创建动画对象  
	m_GuideAnimation = Armature::create("fla_yingdao");
	m_GuideAnimation->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(DragGuideLayer::animationCallback));
	addChild(m_GuideAnimation, m_UINode->getZOrder() + 10); 
	m_GuideAnimation->getAnimation()->stop();
	m_GuideAnimation->setOpacity(0); 
}

void DragGuideLayer::animationCallback(Armature * pAr, MovementEventType type, const std::string& aniName)
{
	if (type == COMPLETE && pAr == m_GuideAnimation)
	{  
		GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
		if(pData->getGuideMode() == E_GUIDE_DRAG)
		{
			if(strcmp("down", aniName.c_str()) == 0)
				doAniMoveAction(pData);

			else if (strcmp("up", aniName.c_str()) == 0)
			{
				finishAniAction(pData);
				doNextState(E_GuideEnd);
			}
				
		} 

	}
}


void DragGuideLayer::resetAniAction(GuideData* pData)
{
	m_GuideNode = dynamic_cast<Node*>(m_GuideParentNode->getChildByName(pData->getGuideNodeID()));
	m_DragNode = dynamic_cast<Node*>(m_GuideParentNode->getChildByName(pData->getDragNodeID()));
	if (m_GuideNode && m_DragNode)
	{
		m_TouchListener->setSwallowTouches(false);
		Point aniPos = m_GuideNode->getPosition() + pData->getGuideNodePos();
		m_GuideAnimation->getAnimation()->stop();
		m_GuideAnimation->getAnimation()->play("moveup");
		//从起点处立即显示 
		m_GuideAnimation->setOpacity(0.0f);
		m_GuideAnimation->setPosition(aniPos);
		m_GuideAnimation->runAction(Sequence::create(FadeIn::create(0.5f), CallFunc::create(
			[&](){
			m_GuideAnimation->getAnimation()->play("down");
		}), NULL));
		m_CurrGuideLabel->setString(sgTextHelper(pData->getGuideTextID()));
	}
	else
	{
		CCLOG("[error]DragGuideLayer::createDragLayer m_GuideNode or m_DragNode is NULL");
		sgGuideManager()->unRegistGuideMessage(true);
	}
	
}

void DragGuideLayer::doAniMoveAction(GuideData* pData)
{
	m_GuideAnimation->getAnimation()->stop();
	m_GuideAnimation->getAnimation()->play("movedown");
	Point pos1 = pData->getDragNodePos() + m_DragNode->getPosition();
	Point offPos = pos1 - m_GuideAnimation->getPosition();
	float distance = sqrt(offPos.x * offPos.x + offPos.y * offPos.y);
	float moveTime = distance / DRAG_MOVE_SPEED;
	m_GuideAnimation->runAction(Sequence::create(MoveTo::create(moveTime, pos1), DelayTime::create(0.1f), CallFunc::create(
		[&](){
		m_GuideAnimation->getAnimation()->play("up");
		m_AnimationState = E_GuideActionState2;
	}), NULL));
	setArrowState(true);
}

void DragGuideLayer::finishAniAction(GuideData* pData)
{
	m_LabelBg->setVisible(true);
	m_GuideAnimation->getAnimation()->stop();
	//动画演示完毕
	m_AnimationOver = true;

	//最后一个任务触摸下发
	if (m_CurGuideDataIndex == m_GuideDataVec.size() - 1)
		m_TouchListener->setSwallowTouches(false);
	m_AnimationState = E_GuideActionState4;
	setArrowState(false);

}

void DragGuideLayer::setArrowState(bool state)
{
	m_ShowArrow = state;
	m_Painting->setVisible(state);
	if (!state)
		m_Painting->reset(); 
	else
		m_ArrowStartPos = m_GuideAnimation->getPosition();
}

float DragGuideLayer::getWaitTime(GuideData* pData)
{
	if (pData)
		return pData->getWaitTime();
	return 1.0f;
}

