/**
* @file     BaseGuideLayer.h
* @brief    新手引导层基类
* @details  
* @author   Alvin
* @date     2015-7-28
*/
#ifndef __GUIDELAYER_H__
#define __GUIDELAYER_H__
#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocostudio/CCArmature.h" 
#include "battle/ArrowWidget.h"
#include "data/TableManager.h"
#include "data/Tables.h"
#include "utils/Comm.h" 
#include "MessageManager.h"
#include "core/ExRichText.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocostudio;
using namespace ui;
enum E_GUIDE_TYPE
{
	E_GUIDE_NONE = 0,
	E_GUIDE_CLICK = 1,		//点击
	E_GUIDE_DRAG = 2,		//拖曳
	E_GUIDE_FOCUS = 3,		//聚焦
	E_GUIDE_DIALOG = 4,		//说明框
	E_GUIDE_NOMAL =  5,		//纯对话无对焦
	E_GUIDE_ANIMATION = 6,  //纯粹动画
	E_GUIDE_STORY = 7,      //剧情对话框
};

enum E_GuideActionState
{
	E_GuideActionState0 = 0,
	E_GuideActionState1,
	E_GuideActionState2 ,
	E_GuideActionState3 ,
	E_GuideActionState4
};

enum E_GuideState
{
	E_GuideStart = 0,
	E_GuideDoing = 1,
	E_GuideEnd = 2,
	E_GuideFinal = 3,
	E_GuideWait = 4,
};
class BaseGuideLayer: public Layer
{
public:
	BaseGuideLayer();
	~BaseGuideLayer();
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();	
	virtual void update(float delta);
	virtual void onDoGuide(float dt){};
	virtual void onReadyGuide();
	virtual void onGuideCallback(bool isFinish);								//回调
	virtual void addGuideData(GuideData* tData);								//添加系列引导动作
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void initAnimation(int type){};
	virtual void initDialogUI(int type, float angle, Point pos);
	virtual void initGuideInfo(int guideID, int isPause, Node* sceneNode, Node* parentNode, Point pos);
	CREATE_FUNC(BaseGuideLayer);
protected:
	virtual void initUI();
	void addListener();
	virtual void animationCallback(Armature * pAr, MovementEventType type, const std::string& aniName){};  
	virtual void adjustNpcUI(GuideData* tData);										//调整人物，对话框位置
	virtual void doNextState(E_GuideState state);
	virtual void doExtraAction();													//执行自定义方法
	virtual bool checkTouchSwallow(Point touchPos){ return true;};					//触摸区域检测
	virtual void onGuideOver();
	virtual void initMask();

protected: 	
	Node* m_TextNode;						//文字节点
	ExRichText* m_CurrGuideLabel;			//背景框文字
	ui::Text* m_GuideTipsLable;				//提示文字
	Sprite* m_LabelBg;						//背景框
	Sprite* m_NpcBg;						//人物
	ArrowWidget* m_Painting;				//箭头
	bool m_AnimationOver;					//动画是否完毕
	Armature* m_GuideAnimation;				//指引动画对象
	EventListenerTouchOneByOne* m_TouchListener;	
	
	
	float m_WaitTime;						//循环等待时间	
	int m_AnimationState;					//动画执行步骤、状态 
	E_GuideState m_GuideState;

	
	Node* m_UINode;	
	Node* m_GuideNode;											//点击点
	Node* m_DragNode;											//拖曳点
	Point m_BaseLabelPos;
	Point m_BaseNpcBgPos;
	Point m_BaseLabelBgPos; 

	int  m_CurGuideDataIndex;  //当前引导节点索引
	std::vector<GuideData* > m_GuideDataVec;

	//引导层的类型
	CC_SYNTHESIZE(int, m_GuideType, GuideType);
	CC_SYNTHESIZE(int, m_GuideID, GuideID);	
	CC_SYNTHESIZE(Node*, m_SceneNode, SceneNode);
	CC_SYNTHESIZE(Node*, m_GuideParentNode, GuideParentNode);	 
	CC_SYNTHESIZE(bool, m_IsEnablePause, IsEnablePause);				//战斗是否暂停
	CC_SYNTHESIZE(bool, m_IsAdjust, IsAdjust);							//是否调整位置
	CC_SYNTHESIZE(int, m_GuideAnimationType, GuideAnimationType);		//引导动画类型
	CC_SYNTHESIZE(Point, m_GuideAnimationPos, GuideAnimationPos);		//箭头位置
	CC_SYNTHESIZE(float, m_GuideAnimationAngle, GuideAnimationAngle);	//箭头角度 
	CC_SYNTHESIZE(bool, m_IsReady, IsReady);
}; 
#endif
