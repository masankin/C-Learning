#ifndef __DIALOGGUIDELAYER_H__
#define __DIALOGGUIDELAYER_H__
#include "BaseGuideLayer.h"

class DialogGuideLayer: public BaseGuideLayer
{
public:
	 DialogGuideLayer();
	 ~DialogGuideLayer();
	 CREATE_FUNC(DialogGuideLayer);
	 void onEnter();
	 void onExit();
	 bool onTouchBegan(Touch *touch, Event *unused_event);
	 void onTouchMoved(Touch *touch, Event *unused_event);
	 void onTouchEnded(Touch *touch, Event *unused_event);
private:
	virtual void initUI();
	void showDesc();
	void initMask();
	void changeNextDesc();
	void onGuideCallback(bool isFinish);
};


class StoryDialogGuideLayer : public DialogGuideLayer
{
public:
	StoryDialogGuideLayer();
	~StoryDialogGuideLayer();
	CREATE_FUNC(StoryDialogGuideLayer);
private:
	void initUI(); 
	void adjustNpcUI(GuideData* tData);	//调整人物，对话框位置

	ui::ImageView* m_DialogBg;
	ui::ImageView* m_NpcImg;
	 
};


class AnimationGuideLayer: public BaseGuideLayer
{
public:
	AnimationGuideLayer();
	~AnimationGuideLayer();
	CREATE_FUNC(AnimationGuideLayer);
	void onEnter();
	void onExit();
	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);
private:
	void initUI();	
	void showDesc();
	void initMask();
	void onGuideCallback(bool isFinish);
};


 
#endif
