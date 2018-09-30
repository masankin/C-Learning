#pragma once

#include  "game/ui/system/Window.h"
#include "core/luaHelp/CppCallLuaHelper.h"

enum ActionWindowPopType
{
	E_POP_TYPE_SMALL,
	E_POP_TYPE_BIG,
	E_POP_TYPE_NONE,
};


class ActionWindow :public Window
{
public:
	ActionWindow() :mArmatureBg(0), mCenter(0), mSender(0), m_eType(E_POP_TYPE_SMALL), mIsPlayEndOnClose(true), mIsPlayBorn(true){};
	~ActionWindow(){};

	CREATE_FUNC(ActionWindow);

	virtual void setShow(bool b) override;

	void setPopType(ActionWindowPopType type);
	bool setAnimation(std::string name);
	void changeBone(cocostudio::Armature* pAr,std::string stBone, Node* display,int index = 0); 
	void changeBoneByDefaultArmature(std::string stBone, Node* display, int index = 0); 

	void playAnimationByName(std::string name);
	void setStandName(std::string name);

	cocostudio::Armature* getArmatureBg();
	Point getBonePosition(std::string boneName, Point offPos = ccp(0, 0), cocostudio::Armature* ani = NULL); //获取骨骼在当前帧的位置

	virtual bool _initWindow(ePriority priority, Widget* showFrom);
	
	void setIsPlayEndOnClose(bool isPlay){ mIsPlayEndOnClose = isPlay; };

	void setIsPlayBorn(bool isPlay){ mIsPlayBorn = isPlay; };

	
protected:
	

	virtual void onMenuItemAction(std::string, cocos2d::Ref*, Widget::TouchEventType);
	virtual void onMenuItemAction(std::string, cocos2d::Ref*){};

	cocostudio::Armature* mArmatureBg;
	Node* mCenter;

	//主动画回调
	virtual void windowAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
	//主动画事件
	void onWindowFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex);

	//主动画回调 子类 继承
	virtual void aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
	//主动画事件回调 子类 继承
	virtual void onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex);

	ui::Widget* mSender;
	ActionWindowPopType m_eType;

	virtual void onActionClose(){};
	virtual void onActionShow(){};

	bool mAnimationFinish;

	std::string mStandName;

	bool mIsPlayEndOnClose;

	bool mIsPlayBorn;
};
