#pragma once

#include "cocos2d.h"
USING_NS_CC;

// 体力恢复记录
class PowerRecord
{
public:
	PowerRecord() :mTimeNode(-1), mIfGetted(false)
	{

	}

	~PowerRecord()
	{
	}

protected:

	// 时刻点，0-0点 / 1-12点		eRecoveryTime
	CC_SYNTHESIZE(int, mTimeNode, TimeNode);
	// 当天是否已领取
	CC_SYNTHESIZE(int, mIfGetted, IfGetted);
};


// 徽章记录
class BadgeItem
{
public:
	BadgeItem() : mType(0), mTime(0){}
	~BadgeItem(){}

	void setType(unsigned int val) { mType = val; }
	unsigned int getType() const { return mType; }

	void setTime(unsigned int val) { mTime = val; }
	unsigned int getTime() const { return mTime; }
	
protected:
	unsigned int mType;
	unsigned int mTime;
};
