#include "LazyTimer.h"
#include "TimeCalculator.h"

LazyTimer* LazyTimer::create(TimeCalculator* pTimerEngine /* = nullptr */)
{
	auto pRet = new LazyTimer();
	if (pRet && pRet->init(pTimerEngine))
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}

	return pRet;
}

LazyTimer::LazyTimer() : mTimerEngine(nullptr)
, mDelayRemoveFlag(false)
{

}

LazyTimer::~LazyTimer()
{
	for (auto & timerPair : mContainer)
	{
		mTimerEngine->removeTimeCalcultor(timerPair.first);
	}
	mContainer.clear();

	mPauseTimers.clear();
}

bool LazyTimer::init(TimeCalculator* pTimerEngine)
{
	if (nullptr == pTimerEngine)
		mTimerEngine = sgTimeCalculator();
	else
		mTimerEngine = pTimerEngine;

	return true;
}

void LazyTimer::update(float delta)
{
	mDelayRemoveFlag = true;

	for (auto & timerPair : mContainer)
	{
		if (0.0f == mTimerEngine->getTimeLeft(timerPair.first))
		{
			CCLOG("[trace]LazyTimer::update : Timer[%s] is timer out", timerPair.first.c_str());
			auto & callback = timerPair.second;
			if (callback)
				callback(timerPair.first);
		}
	}

	mDelayRemoveFlag = false;

	for (auto & timeKey : mRemoveTimers)
	{
		this->stopTimer(timeKey);
	}
	mRemoveTimers.clear();
}

bool LazyTimer::hasTimer(const std::string & timeKey)
{
	return mContainer.end() != mContainer.find(timeKey);
}

void LazyTimer::startTimer(const std::string & timeKey, float interval, const TimeoutCallback& callback /* = nullptr */)
{
	if (timeKey.empty() || interval <= 0.0f)
		return;

	auto findItr = mContainer.find(timeKey);
	if (findItr == mContainer.end())
	{
		CCLOG("[trace]LazyTimer::startTimer : Start timer[%s] = %f", timeKey.c_str(), interval);
		mContainer.insert(std::make_pair(timeKey, callback));
	}
	else
	{
		CCLOG("[trace]LazyTimer::startTimer : Reset timer[%s] = %f", timeKey.c_str(), interval);
		findItr->second = callback;
	}

	if (mDelayRemoveFlag)
	{
		auto removeItr = std::find(mRemoveTimers.begin(), mRemoveTimers.end(), timeKey);
		if (removeItr != mRemoveTimers.end())
			mRemoveTimers.erase(removeItr);
	}

	mTimerEngine->createTimeCalcultor(timeKey, interval);
}

void LazyTimer::stopTimer(const std::string & timeKey)
{
	if (mDelayRemoveFlag)
	{
		CCLOG("[trace]LazyTimer::stopTimer : Delay remove timer[%s]", timeKey.c_str());

		mRemoveTimers.push_back(timeKey);
	}
	else
	{
		CCLOG("[trace]LazyTimer::stopTimer : Stop timer[%s]", timeKey.c_str());

		mContainer.erase(timeKey);
		mPauseTimers.erase(timeKey);

		mTimerEngine->removeTimeCalcultor(timeKey);
	}
}

void LazyTimer::resetTimer(const std::string & timeKey)
{
	if (this->hasTimer(timeKey))
	{
		CCLOG("[trace]LazyTimer::resetTimer : Reset timer[%s]", timeKey.c_str());

		auto findItr = mPauseTimers.find(timeKey);
		if (findItr == mPauseTimers.end())
		{
			mTimerEngine->resetTimeCalcultor(timeKey);
		}
		else
		{
			float interval = findItr->second.second;
			mTimerEngine->createTimeCalcultor(timeKey, interval);
		}
	}
}

float LazyTimer::getTimerLeft(const std::string & timeKey)
{
	if (this->hasTimer(timeKey))
	{
		auto findItr = mPauseTimers.find(timeKey);
		if (findItr == mPauseTimers.end())
			return mTimerEngine->getTimeLeft(timeKey);
		else
			return findItr->second.first;
	}

	return 0.0f;
}

void LazyTimer::pauseTimer(const std::string & timeKey)
{
	if (!this->hasTimer(timeKey))
		return;

	auto findItr = mPauseTimers.find(timeKey);
	if (findItr == mPauseTimers.end())
	{
		float timeLeft = mTimerEngine->getTimeLeft(timeKey);
		float timeInterval = mTimerEngine->getTimeLength(timeKey);
		std::pair<float, float> timerInfo(timeLeft, timeInterval);
		if (timeLeft > 0.0f)
		{
			mPauseTimers.insert(std::make_pair(timeKey, timerInfo));
			mTimerEngine->removeTimeCalcultor(timeKey);

			CCLOG("[trace]LazyTimer::pauseTimer : Pause timer[%s], time left %f", timeKey.c_str(), timeLeft);
		}
	}
}

void LazyTimer::resumeTimer(const std::string & timeKey)
{
	if (!this->hasTimer(timeKey))
		return;

	auto findItr = mPauseTimers.find(timeKey);
	if (findItr != mPauseTimers.end())
	{
		float timeLeft = findItr->second.first;
		mTimerEngine->createTimeCalcultor(timeKey, timeLeft);

		CCLOG("[trace]LazyTimer::resumeTimer : Resume timer[%s], time left %f", timeKey.c_str(), timeLeft);
	}
}
