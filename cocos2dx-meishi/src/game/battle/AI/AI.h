/*
* jacey
* AI入口 隶属于每个阵营单位

@brief 一个“AI判断”包括，多个条件，对应一个行为

1. class AI. 阵营AI的主入口，拥有决策类Policy成员。
2. class PolicyManager. 目前代理AI状态，行为的管理。
3. Policy类遍历当前AI所属阵营的所有建筑，顺序执行建筑拥有的状态行为（单次执行AI判断成功，则退出，不进行后续AI判断）。

*/
#pragma once

#include "basic/Policy.h"
#include "Constant.h"
#include "Message.h"

class Camp;
class AI : public MessageHandler
{
public:
	AI();
	~AI();

	bool init(eCampType etype);
	void update(float deta);
	static AI* create(eCampType etype);

	virtual void onReceiveMassage(GameMessage * message);

	eCampType	getCampType(){ return mCampType; }


protected:

	// AI 决策
	Policy	mPolicy;

	bool	mPauseAI;


	/* 游戏逻辑相关属性
	*/
	eCampType	mCampType;

};
