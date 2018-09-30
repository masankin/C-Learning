/***************************************************
	File		: ActionTip.h
	Description	: 动态提示
	Author		: Edward Chan
	Date		: 2015-09-23
	
	Copyright (C) 2015 - All Rights Reserved.
****************************************************/
#ifndef __MGAME_ACTIONTIP_H__
#define __MGAME_ACTIONTIP_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"
#include <vector>

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;
using namespace cocos2d::ui;

/*
*	目前只是实现了文本+图片的形式(战斗提示信息),后续有需求再扩展
*
*eg.1:(战斗提示信息示例)
*	Action* pAction = Sequence::create(
*		MoveBy::create(.3f, Vec2(0, 50)),
*		DelayTime::create(1.5f),
*		Spawn::create(MoveBy::create(.2f, Vec2(0, 30)), FadeOut::create(.2f), NULL),
*		NULL);
*	ActionTip* tip = ActionTip::create(string, pAction);
*	tip->addText(string-1);
*	...
*	tip->addText(string-n);
*	if want to hide pic, you can add "tip->setIsShowPic(false);"
*	tip->run();
*	xxx->addChild(tip);
*
*eg.2:
*	ActionTip* tip = ActionTip::create(vector<string>, action);
*	tip->run();
*	xxx->addChild(tip);
*
*eg.3:
*	ActionTip* tip = ActionTip::create(vector<string>, distance, duration);
*	tip->run();
*	xxx->addChild(tip);
*
*eg.4:
*	ActionTip* tip = ActionTip::create(string, distance, duration);
*	tip->addText(string-1);
*	...
*	tip->addText(string-n);
*	tip->run();
*	xxx->addChild(tip);
*
*/
class ActionTip : public Node
{
public:
	ActionTip();
	~ActionTip();

	/*
	*	@parm
	*	text		文本
	*	action		动画
	*	callBack	回调函数
	*/
	static ActionTip* create(string text, Action* action);

	/*
	*	@parm
	*	textList	文本列表
	*	action		动画
	*/
	static ActionTip* create(vector<string> textList, Action* action);

	/*
	*	@parm
	*	text		文本
	*	distance	距离
	*	duration	持续时间
	*/
	static ActionTip* create(string text, Vec2 distance, float duration);

	/*
	*	@parm
	*	textList	文本列表
	*	distance	距离
	*	duration	持续时间
	*/
	static ActionTip* create(vector<string> textList, Vec2 distance, float duration);

	virtual bool init();
	virtual bool init(Vec2 distance, float duration);

	//运行
	virtual void run();

	//暂时只开放添加文本的接口
	virtual void addText(string text);

	//不显示图片
	virtual void setIsShowPic(bool visible) { m_bIsShowPic = visible; }

	//设置回调
	virtual void setCallBackFunc(const std::function<void()>& callBack);

	CC_SYNTHESIZE(Action*, mAction, Action);

protected:
	virtual void runChildrenActions(Node*);

	virtual void runActions(Node* target, bool removeParent);

	void addTextWidget(string text);

	std::function<void()> mCallBack;

	Color4B mColor;
	CC_SYNTHESIZE(int, mFontSize, FontSize);
	string mFontName;
	vector<string> mTextList;
	int mMargin;
	float mContentHeight;

	bool mIsRun;

	bool m_bIsShowPic;

private:
	Vec2 mDistance;
	float mDuration;

};

#endif //__MGAME_ACTIONTIPTEXT_H__
