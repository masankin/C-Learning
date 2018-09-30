/**
* @file     ClickGuideLayer.h
* @brief    新手引导点击触发层
* @details  
* @author   Alvin
* @date     2015-8-13
*/
#ifndef __CLICKGUIDELAYER_H__
#define __CLICKGUIDELAYER_H__
#include "BaseGuideLayer.h"
class ClickGuideLayer: public BaseGuideLayer
{
public:
	ClickGuideLayer();
	~ClickGuideLayer();
	void onDoGuide(float dt); 
	void onGuideCallback(bool isFinish); //回调 
	bool onTouchBegan(Touch *touch, Event *unused_event); 
	void initAnimation(int type);
	CREATE_FUNC(ClickGuideLayer);
protected:
	void animationCallback(Armature * pAr, MovementEventType type, const std::string& aniName); 
private:
	void initUI(); 
	void updateFogLayer(float dt); 
	bool drawClipNode(float dt, int drawType, Point pos);
	void playAnimation(GuideData* pData);
	void drawCircle(float x, float y, float r, bool atOnce = false);
	void drawRect(float x, float y, float width, float height, bool atOnce = false);
	void startGuide();
	bool checkTouchSwallow(Point touchPos);		//检测触摸点是否在规定范围内
	void createClickLayer(GuideData* tData);	//创建引导层
	void resetAnimation(GuideData* pData);		//重设设置动画
	void resetClipRange(GuideData* pData);		//重设裁剪范围
	void resetClipNode(GuideData* pData);		//重设裁剪节点
	void resetRectBorder(Point pos);			//重新设置矩形边框
	void resetCircleBorder(Point pos);			//重新设置圆形边框
	void resetNpcUI(GuideData* pData);			//调整人物以及对话框 

	void doStartAction(float delta);
	void doDoingAction(float delta);
	void doEndAction(float delta);
	void doFinalAction(float delta);  

protected: 	
	 
	
	//剪裁节点
	ClippingNode* m_ClippingNode;	
	DrawNode* m_Stencil; 
	 
	float m_VisitRange;
	float m_VisitRangeEx;
	bool m_StartFog;  
	Size m_FogRange;

	extension::Scale9Sprite * m_GuideRectBorderSp;
	Size m_BaseRectSize;
	extension::Scale9Sprite* m_GuideCircleBorderSp;
	Size m_BaseCircleSize;


	bool mPlayingAni;
};  
#endif
