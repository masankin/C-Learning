/**
* @file     DragGuideLayer.h
* @brief    新手引导拖曳层
* @details  
* @author   Alvin
* @date     2015-8-13
*/
#ifndef __DRAGGUIDELAYER_H__
#define __DRAGGUIDELAYER_H__
#include "BaseGuideLayer.h"
class DragGuideLayer: public BaseGuideLayer
{
public:
	DragGuideLayer();
	~DragGuideLayer();
	bool init();  
	void onGuideCallback(bool isFinish); //回调
	void addGuideData(GuideData* tData); //添加系列引导动作 
	bool onTouchBegan(Touch *touch, Event *unused_event); 
	void initAnimation(int type);
	CREATE_FUNC(DragGuideLayer);

private: 
	void onDoGuide(float delta);
	void createDragLayer(GuideData* data);
	void doStartAction(float delta);
	void doDoingAction(float delta);
	void doEndAction(float delta);
	void doFinalAction(float delta);
	void doWaitAction(float delta);

	void animationCallback(Armature * pAr, MovementEventType type, const std::string& aniName);  
	void resetAniAction(GuideData* pData);
	void doAniMoveAction(GuideData* pData);
	void finishAniAction(GuideData* pData);
	void setArrowState(bool state);  

	float getWaitTime(GuideData* pData);
 
	//箭头起始点
	Point m_ArrowStartPos;
	//是否显示箭头
	bool m_ShowArrow; 
};

 
#endif
