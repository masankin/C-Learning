/**
* @file     FocusGuideLayer.h
* @brief    新手引导聚焦说明层
* @details  
* @author   Alvin
* @date     2015-8-13
*/
#ifndef __FOCUSGUIDELAYER_H__
#define __FOCUSGUIDELAYER_H__
#include "BaseGuideLayer.h"  
class FocusGuideLayer: public BaseGuideLayer
{
public:
	FocusGuideLayer();
	~FocusGuideLayer(); 
	void onGuideCallback(bool isFinish); //回调 
	bool onTouchBegan(Touch *touch, Event *unused_event); 
	void initAnimation(int type);
	CREATE_FUNC(FocusGuideLayer); 
private:
	void initUI();
	void onDoGuide(float delta);
	void doNextState(E_GuideState state); 
	GuideData* getNextGuideData();

	void createFocusLayer(GuideData* pData);
	void resetClipNode(GuideData* pData);

	void updateFogLayer(float dt); 
	bool drawClipNode(DrawNode* pNode, float dt, int drawType, Point pos);

	void drawCircle(DrawNode* drawNode, float x, float y, float r, bool atOnce = false);
	void drawRect(DrawNode* drawNode, float x, float y, float width, float height, bool atOnce = false);

	float getWaitTime(GuideData* pData);
	Size getClipRange(GuideData* pData);

	

	void doStartAction(float delta);
	void doDoingAction(float delta);
	void doEndAction(float delta);
	void doFinalAction(float delta);
	void doWaitAction(float delta);

	bool checkTouchSwallow(Point touchPos);
	
protected: 	
	 
	//剪裁节点
	ClippingNode* m_ClippingNode;		
	Node* m_StencilNode;
	std::vector<DrawNode* > m_DrawNodeVec;

	float m_VisitRange;
	float m_VisitRangeEx;
	float m_WidthVelocity;
	float m_HeightVelocity;
	bool m_StartFog; 
	Size m_FogRange; 

	bool m_HasSendMessage;

};
 
#endif
