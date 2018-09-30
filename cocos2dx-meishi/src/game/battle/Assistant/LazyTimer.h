#ifndef _LAZY_TIMER_H
#define _LAZY_TIMER_H

#include "cocos2d.h"

class TimeCalculator;

class LazyTimer : public cocos2d::Ref
{
public:
	typedef std::function<void(const std::string& key)> TimeoutCallback;

public:
	static LazyTimer* create(TimeCalculator* pTimerEngine = nullptr);

	void update(float delta);

	bool hasTimer(const std::string & timeKey);
	void startTimer(const std::string & timeKey, float timeLeft, const TimeoutCallback& callback = nullptr);
	void stopTimer(const std::string & timeKey);
	void resetTimer(const std::string & timeKey);
	float getTimerLeft(const std::string & timeKey);
	void pauseTimer(const std::string & timeKey);
	void resumeTimer(const std::string & timeKey);

protected:
	LazyTimer();
	virtual ~LazyTimer();

	bool init(TimeCalculator* pTimerEngine);

private:

	typedef std::map<std::string, TimeoutCallback> TimerContainer;
	typedef std::map<std::string, std::pair<float, float>> PauseTimerContainer;
	typedef std::vector<std::string> RemoveContianer;

	TimeCalculator* mTimerEngine;

	TimerContainer mContainer;
	PauseTimerContainer mPauseTimers;
	bool mDelayRemoveFlag;
	RemoveContianer mRemoveTimers;
};

#endif // !_LAZY_TIMER_H
