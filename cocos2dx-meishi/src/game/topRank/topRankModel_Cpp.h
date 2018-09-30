
#ifndef __TOPRANK_MODEL_H__
#define __TOPRANK_MODEL_H__

#include "cocos2d.h"
USING_NS_CC;

class topRankModel_Cpp
{
public:
	topRankModel_Cpp();
	~topRankModel_Cpp();

	float mTimeStar;
	float mTimeWin;
	float mTimeWinStreak;
	bool mForceUpdateTime;	//强行更新时间

	void setTimeStar(float var);
	float getTimeStar(){ return mTimeStar; };

	void setTimeWin(float var);
	float getTimeWin(){ return mTimeWin; };

	void setTimeWinStreak(float var);
	float getTimeWinStreak(){ return mTimeWinStreak; };

	void setForceUpdateTime(bool b){ mForceUpdateTime = b; };
	bool getForceUpdateTime(){ return mForceUpdateTime; };
private:

};

#define sgtopRankModel_Cpp() Singleton<topRankModel_Cpp>::Get()
#endif // !1
