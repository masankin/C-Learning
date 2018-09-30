/***************************************************
File		: ActionGain.h
Description	: 获取或消耗的动画表现
Author		: Edward Chan
Date		: 2015-09-25

Copyright (C) 2015 - All Rights Reserved.
****************************************************/
#ifndef __MGAME_ACTIONGAIN_H__
#define __MGAME_ACTIONGAIN_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <vector>

USING_NS_CC;
using namespace cocos2d::ui;

enum eReceiverType
{
	RECEIVER_ALONE,		//单独不带数字
	RECEIVER_ACCOUNT,	//带数字的
};

/*
*	eg.1
*	node = sgPageFunction()->getActionPowerLayer() or ->getActionDiamondLayer() or ->getActionPackageLayer()
*
*	Node* pOriginReceiver = sgPageFunction()->getDiamondPanel()->getChildren().at(0);
*	Node* pReceiver = CSLoader::getInstance()->createNode("MoneyBuyNode.csb");
*	node->addReceiver(RECEIVER_ACCOUNT, pReceiver, pOriginReceiver->getPosition(), false, pOriginReceiver);
*
*	then
*
*	node->addItem("ui/general/general_diamonds.png", btn->getPosition(), 100, 0);
*
*	某些界面的返回处理需要调用stopActionGain接口,清空所有正在飞行的动画
*/
class ActionGain : public Node
{
public:
	ActionGain();
	~ActionGain();

	CREATE_FUNC(ActionGain);

	virtual bool init();

	/*
	*	@function		添加收集器
	*	@parm
	*	type			收集器类型
	*	receiver		收集器
	*	position		收集器位置
	*	isVisible		原节点是否隐藏
	*/
	void addReceiver(eReceiverType type, Node* receiver, Vec2 position, bool isVisible, Node* originReceiver = NULL);

	/*
	*	@function		添加收集项目(先进先出原则)
	*	@parm
	*	item			项目节点
	*	position		收集器位置
	*	itemCount		项目数量
	*	originCount		原始数据
	*/
	void addItem(Node* item, Vec2 position, int itemCount, int originCount, bool isRandomRotation = true, bool isRandomPos = true);

	/*
	*	@function		添加收集项目(先进先出原则)
	*	@parm
	*	itemName		项目图片名字
	*	position		收集器位置
	*	itemCount		项目数量
	*	originCount		原始数据
	*/
	void addItem(std::string itemName, Vec2 position, int itemCount, int originCount);

	//删除收集器
	//删除自身操作交由外部处理
	void removeReceiver();

	//停止动画
	void stopActionGain();

	//移动动画接口,暂不开放
	void setMoveAction();

protected:
	void actionCallback();
	
	void clearItemList();

	void particleCallback(Node* node);

	Node*				mItemsNode;
	eReceiverType		mType;
	Node*				mOriginReceiver;
	Node*				mReceiver;
	Vec2				mReceiverPosition;
	Text*				mAccountText;
	int					mAccount;
	bool				mVisible;
	std::queue<int>		mItemCountVec;
};

#endif //__MGAME_ACTIONGAIN_H__
