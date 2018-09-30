#include "ClickGuideLayer.h" 
#include "battle/BattleControl.h"
#include "guide/GuideManager.h"
#define FOCUS_RADIUS 1366
#define FOCUS_SUBCOUNT 68
#define FOCUS_MAXSUBRADIUS 65
enum eClickType{
	E_Click_Arrow = 0,
	E_SingleClick_Right = 1,
	E_SingleClick_Left = 2,
	E_DoubleClick_Right = 3,
	E_DoubleClick_Left = 4,
};

ClickGuideLayer::ClickGuideLayer()
{         
	m_VisitRange = FOCUS_RADIUS; 
	m_VisitRangeEx = FOCUS_RADIUS;
	m_StartFog = false;  
	m_FogRange = Size(FOCUS_MAXSUBRADIUS, FOCUS_MAXSUBRADIUS);
	m_GuideAnimationType = E_SingleClick_Right;
	m_GuideRectBorderSp = NULL; 
	m_GuideCircleBorderSp = NULL;
	mPlayingAni = false;
	m_IsReady = false;
}

ClickGuideLayer::~ClickGuideLayer()
{
	 
}


void ClickGuideLayer::initUI()
{
	BaseGuideLayer::initUI(); 
	//创建聚焦对象
	CCSize tSize = Director::getInstance()->getWinSize();
	m_ClippingNode = ClippingNode::create();
	m_ClippingNode->setInverted(true);
	addChild(m_ClippingNode,10);
	m_ClippingNode->setVisible(false);  
	m_ClippingNode->setContentSize(CCSizeMake(tSize.width, tSize.height));

	CCLayerColor* pColor = LayerColor::create(ccc4(0,0,0,180));
	pColor->setContentSize(CCSizeMake(tSize.width, tSize.height));
	m_ClippingNode->addChild(pColor);

	m_Stencil = DrawNode::create();
	m_Stencil->setContentSize(CCSizeMake(tSize.width, tSize.height));
	m_Stencil->setPosition(ccp(0, 0));
	m_Stencil->clear();
	m_ClippingNode->setStencil(m_Stencil);

	m_GuideTipsLable->setVisible(false);
} 
 
void ClickGuideLayer::onDoGuide(float delta)
{
	if (m_GuideState == E_GuideStart)
		doStartAction(delta);
	else if (m_GuideState == E_GuideDoing)
		doDoingAction(delta);
	else if (m_GuideState == E_GuideEnd)
		doEndAction(delta);
	else if (m_GuideState == E_GuideFinal)
		doFinalAction(delta);
}
 

void ClickGuideLayer::onGuideCallback(bool isFinish)
{
	BaseGuideLayer::onGuideCallback(isFinish);
	if (!isFinish)
		return;
	doExtraAction();
	removeFromParentAndCleanup(true);
} 

bool ClickGuideLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	sgGuideManager()->setGuideLock(false);
	CCLOG("ClickGuideLayer::onTouchBegan");
	return checkTouchSwallow(touch->getLocation());
} 


 
void ClickGuideLayer::initAnimation(int type)
{
	m_GuideAnimationType = type;
	//创建动画对象 
	if (m_GuideAnimationType == E_SingleClick_Right || m_GuideAnimationType == E_DoubleClick_Right)
	{
		m_GuideAnimation = Armature::create("fla_yingdao");
		m_GuideAnimation->getAnimation()->setMovementEventCallFunc(this, 
			movementEvent_selector(ClickGuideLayer::animationCallback));
	}
	else if (m_GuideAnimationType == E_SingleClick_Left || m_GuideAnimationType == E_DoubleClick_Left)
	{
		m_GuideAnimation = Armature::create("fla_yingdao");
		m_GuideAnimation->setScaleX(-1);
		m_GuideAnimation->getAnimation()->setMovementEventCallFunc(this,
			movementEvent_selector(ClickGuideLayer::animationCallback));
	}
	else
	{
		m_GuideAnimation = Armature::create("fla_tishi");
		m_GuideAnimation->setVisible(false);
	} 
	addChild(m_GuideAnimation, m_UINode->getZOrder() + 10); 
	m_GuideAnimation->getAnimation()->stop();
	m_GuideAnimation->setOpacity(0); 
}





void ClickGuideLayer::animationCallback(Armature * pAr, MovementEventType type, const std::string& aniName)
{
	if (type != COMPLETE || pAr != m_GuideAnimation)
		return;

	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (!pData || pData->getGuideMode() != E_GUIDE_CLICK)
		return;

	if (strcmp("Double", aniName.c_str()) == 0)
	{ 
		m_GuideAnimation->getAnimation()->play("up");
	}
	else if (strcmp("Single", aniName.c_str()) == 0)
	{
		m_GuideAnimation->getAnimation()->play("up");
	}
	else if (strcmp("up", aniName.c_str()) == 0)
	{
		m_AnimationOver = true;  
		m_StartFog = false; 
		m_GuideAnimation->runAction(FadeOut::create(0.5f));
		doNextState(E_GuideStart);
	}
	else if (strcmp("moveup", aniName.c_str()) == 0)
	{
		m_GuideAnimation->runAction(Sequence::create(DelayTime::create(0.3f), CallFunc::create(
			[&](){
			if (m_GuideAnimationType == E_DoubleClick_Left || m_GuideAnimationType == E_DoubleClick_Right)
				m_GuideAnimation->getAnimation()->play("Double");
			else if(m_GuideAnimationType == E_SingleClick_Left || m_GuideAnimationType == E_SingleClick_Right)
				m_GuideAnimation->getAnimation()->play("Single");

		}), NULL));
	}
} 
 

void ClickGuideLayer::updateFogLayer(float dt)
{   
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (!pData || !m_GuideNode)
		return; 
	Point pos = pData->getGuideNodePos() + m_GuideNode->getPosition();
	int type = pData->getFocusType();
	if (drawClipNode(dt, type, pos))
	{
		m_IsReady = true;
		doNextState(E_GuideEnd);
	}  
}

bool ClickGuideLayer::drawClipNode(float dt, int drawType, Point pos)
{
	m_Stencil->clear();
	bool updateOver = false;
	
	if (drawType == E_FOCUS_CIRCLE)
	{
		if (m_VisitRange <= m_FogRange.width)
		{
			m_VisitRange = m_FogRange.width;
			updateOver = true;
		}
		drawCircle(pos.x, pos.y, m_VisitRange);
	}
	else
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
		drawRect(pos.x, pos.y, m_VisitRange, m_VisitRangeEx);
		if (ret1 && ret2)
			updateOver = true;
	}  
	if (!updateOver)
	{
		m_VisitRange -= FOCUS_SUBCOUNT * dt * 60;
		m_VisitRangeEx -= FOCUS_SUBCOUNT * dt * 60;
	}
	else
		m_AnimationOver = true;
	return updateOver;
}


void ClickGuideLayer::playAnimation(GuideData* pData)
{
	if (mPlayingAni)
		return;
	mPlayingAni = true;
	m_GuideAnimationType = pData->getIsFingerAni();
	m_GuideAnimation->setVisible(false);
	if (pData->getIsShowHand() == 1)
	{
		m_GuideAnimation->setVisible(true); 
		if (m_GuideAnimationType == E_DoubleClick_Left || m_GuideAnimationType == E_DoubleClick_Right)
			m_GuideAnimation->getAnimation()->play("Double");
		else if (m_GuideAnimationType == E_SingleClick_Left || m_GuideAnimationType == E_SingleClick_Right)
			m_GuideAnimation->getAnimation()->play("Single");
		else
			m_GuideAnimation->getAnimation()->play("play", -1, 1);
			
	} 
}
 
void ClickGuideLayer::drawCircle(float x, float y, float r, bool atOnce)
{  
	const int nCount = 80;
	const float coef = 2.0f * (float)M_PI/nCount;
	CCPoint circle[nCount];
	for(unsigned int i = 0;i <nCount; i++) {
		float rads = i*coef;
		circle[i].x = x + r * cosf(rads) * 0.8;
		circle[i].y = y + r * sinf(rads) * 0.8;
	} 
	m_Stencil->drawPolygon(circle, nCount, ccc4f(1,1,1,1), 0, ccc4f(1,1,1,1)); 

	m_GuideCircleBorderSp->setVisible(true); 
	float widthRate = (r * 0.8f)/(m_BaseCircleSize.width/2.0f);
	float heightRate = (r * 0.8f)/(m_BaseCircleSize.height/2.0f);
	Size tBorderSize = m_BaseCircleSize; 
	m_GuideCircleBorderSp->setScale(widthRate, heightRate); 
	m_GuideCircleBorderSp->setPosition(x,y);
}

void ClickGuideLayer::drawRect(float x, float y, float width, float height, bool atOnce)
{  
	Point rectArr[4]; 
	if(atOnce)
	{
		rectArr[0] = ccp(x - m_FogRange.width / 2, y + m_FogRange.height/ 2);
		rectArr[1] = ccp(x + m_FogRange.width / 2, y + m_FogRange.height/ 2);
		rectArr[2] = ccp(x + m_FogRange.width / 2, y - m_FogRange.height/ 2);
		rectArr[3] = ccp(x - m_FogRange.width / 2, y - m_FogRange.height/ 2);
	}
	else
	{
		rectArr[0] = ccp(x - width/2, y + height/2);
		rectArr[1] = ccp(x + width/2, y + height/2);
		rectArr[2] = ccp(x + width/2, y - height/2);
		rectArr[3] = ccp(x - width/2, y - height/2);
	}  
	m_Stencil->drawPolygon(rectArr, 4, ccc4f(1,1,1,1), 0, ccc4f(1,1,1,1));  
	m_GuideRectBorderSp->setVisible(true); 
	float widthRate = (rectArr[1].x - rectArr[0].x)/(m_BaseRectSize.width);
	float heightRate = (rectArr[0].y - rectArr[3].y)/(m_BaseRectSize.height);
	Size borderSize = m_BaseRectSize;
	m_GuideRectBorderSp->setPreferredSize(CCSizeMake(borderSize.width * widthRate, borderSize.height * heightRate));   
	m_GuideRectBorderSp->setPosition(x,y);
}

 
bool ClickGuideLayer::checkTouchSwallow(Point touchPos)
{ 
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (pData && E_GUIDE_CLICK == pData->getGuideMode() && m_GuideNode)
	{ 
		//是否强制完成
		if (pData->getIsForce() == 1)
		{
			bool canTouch = false;
			if (pData->getTouchAtOnce() == 0 || m_AnimationOver == true)
				canTouch = true;
		 
			if (!canTouch)
				return true; 
			

			//定义触摸区域  
			Point pos = pData->getGuideNodePos() + m_GuideNode->getPosition();
			CCRect rect = CCRectMake(
				pos.x - m_FogRange.width / 2,
				pos.y - m_FogRange.height / 2,
				m_FogRange.width,
				m_FogRange.height);

			//检测触摸下发是否到达可触摸位置
			if (rect.containsPoint(touchPos) == true)
			{
				m_TouchListener->setSwallowTouches(false); 
				CCLOG("[trace]ClickGuideLayer::onTouchBegan , pData->getIsForce() == true, tRect.containsPoint(tTouchPos) == true");
				return false;
			}
		}
		else if (pData->getIsForce() == 2)
		{
			sgGuideManager()->unRegistGuideMessage();
			return false;
		} 
		else
		{
			CCLOG("[trace]ClickGuideLayer::onTouchBegan , pData->getIsForce() == false, tRect.containsPoint(tTouchPos) == true");
			m_TouchListener->setSwallowTouches(false);
		}
		m_TouchListener->setSwallowTouches(true);
		return true;
	}
	return true;
}


void ClickGuideLayer::createClickLayer(GuideData* pData)
{
	m_IsReady = false;
	m_ClippingNode->setVisible(true);
	m_VisitRange = FOCUS_RADIUS;
	m_VisitRangeEx = FOCUS_RADIUS; 
	m_StartFog = true; 
	m_GuideNode = dynamic_cast<Node*>(m_GuideParentNode->getChildByName(pData->getGuideNodeID()));
	if (!m_GuideNode)
	{
		//如果找不到节点就强行完成
		sgGuideManager()->unRegistGuideMessage(true);
		return;
	}
	adjustNpcUI(pData);
	resetAnimation(pData);
	resetClipNode(pData);
	resetNpcUI(pData);
	
}

void ClickGuideLayer::resetAnimation(GuideData* pData)
{
	Point pos = pData->getGuideNodePos() + m_GuideNode->getPosition();
	m_GuideAnimation->setAnchorPoint(ccp(0.0, 1.0f));
	m_GuideAnimation->runAction(FadeIn::create(0.5f));

	//是否显示手指
	m_GuideAnimation->setVisible(pData->getIsShowHand() == 1 ? true : false);

	//标记引导的是手指动画还是指针动画
	m_GuideAnimationType = pData->getIsFingerAni();
	if (m_GuideAnimationType == E_SingleClick_Right || m_GuideAnimationType == E_DoubleClick_Right)
		m_GuideAnimation->setPosition(ccpAdd(pos, ccp(30, 50)));
	else if (m_GuideAnimationType == E_SingleClick_Left || m_GuideAnimationType == E_DoubleClick_Left)
		m_GuideAnimation->setPosition(m_GuideAnimationPos + pos);
	else
	{
		m_GuideAnimation->setPosition(m_GuideAnimationPos + pos);
		m_GuideAnimation->setRotation(m_GuideAnimationAngle);
		m_AnimationOver = true; 
	} 
}

void ClickGuideLayer::resetClipNode(GuideData* pData)
{
	Point pos = pData->getGuideNodePos() + m_GuideNode->getPosition();
	
	resetClipRange(pData);
	resetRectBorder(pos);
	resetCircleBorder(pos); 

	if (pData->getEnableFocus() == 0)
	{
		if (pData->getFocusType() == E_FOCUS_CIRCLE)
			drawCircle(pos.x, pos.y, m_FogRange.width, true);
		else
			drawRect(pos.x, pos.y, m_FogRange.width, m_FogRange.height, true);
	}
}

void ClickGuideLayer::resetClipRange(GuideData* pData)
{ 
	//聚焦类型 1 = 圆形， 2 = 矩形
	if (pData->getFocusType() == E_FOCUS_CIRCLE)
	{
		int width =  atoi(pData->getGuideFrameSize().c_str());
		int height = width;
		m_FogRange = Size(width, height);
	}
	else if (pData->getFocusType() == E_FOCUS_RECT)
	{
		std::string tStr = pData->getGuideFrameSize();
		std::vector<std::string > rangeStrVec = splitStr(tStr, "#");
		if (rangeStrVec.size() == 2)
		{
			int width = atoi(rangeStrVec.at(0).c_str());
			int height = atoi(rangeStrVec.at(1).c_str());
			m_FogRange = Size(width, height);
		}
	} 
}

void ClickGuideLayer::resetRectBorder(Point pos)
{
	//裁剪区域的外部框 -- 矩形
	if (m_GuideRectBorderSp)
		m_GuideRectBorderSp->removeFromParentAndCleanup(true);
	m_GuideRectBorderSp = extension::Scale9Sprite::create("fanquan.png");
	m_GuideRectBorderSp->setVisible(false);
	m_GuideRectBorderSp->setPosition(pos);
	m_BaseRectSize = m_GuideRectBorderSp->getContentSize();
	addChild(m_GuideRectBorderSp, m_GuideAnimation->getZOrder() - 20);
}

void ClickGuideLayer::resetCircleBorder(Point pos)
{
	//裁剪区域的外部框 -- 圆形
	if (m_GuideCircleBorderSp)
		m_GuideCircleBorderSp->removeFromParentAndCleanup(true);
	m_GuideCircleBorderSp = extension::Scale9Sprite::create("yuanquan.png");
	m_GuideCircleBorderSp->setPosition(pos);
	m_GuideCircleBorderSp->setVisible(false);
	m_BaseCircleSize = m_GuideCircleBorderSp->getContentSize();
	addChild(m_GuideCircleBorderSp, m_GuideAnimation->getZOrder() - 20);
}

void ClickGuideLayer::resetNpcUI(GuideData* pData)
{
	m_GuideTipsLable->setVisible(false);
	m_CurrGuideLabel->setString(sgTextHelper(pData->getGuideTextID()));
	m_NpcBg->setZOrder(m_GuideAnimation->getZOrder() - 10);
	m_LabelBg->setZOrder(m_GuideAnimation->getZOrder() - 10);
	m_CurrGuideLabel->setZOrder(m_GuideAnimation->getZOrder() - 5);
}

 

void ClickGuideLayer::doStartAction(float delta)
{
	if (m_WaitTime > 0.0f)
	{
		m_WaitTime -= delta;
		return;
	}
	doNextState(E_GuideDoing);
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (!pData || pData->getGuideMode() != E_GUIDE_CLICK)
		return;
	createClickLayer(pData); 
}

void ClickGuideLayer::doDoingAction(float delta)
{
	if (m_StartFog)
		updateFogLayer(delta);
}

void ClickGuideLayer::doEndAction(float delta)
{
	GuideData* pData = m_GuideDataVec.at(m_CurGuideDataIndex);
	if (!pData)
		return; 
	m_WaitTime = pData->getWaitTime();
	mPlayingAni = false;
	playAnimation(pData); 
	doNextState(E_GuideFinal);
}

void ClickGuideLayer::doFinalAction(float delta)
{
	
}

 
