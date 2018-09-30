#include "TimeCalculator.h"
#include "base/ccUtils.h"
#include "cocos2d.h"
#include <chrono>

#include "game/ranking/RankingControl.h"
#include "game/topRank/topRankModel_Cpp.h"


#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <sys/time.h>
#endif

#if  CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#import <mach/mach_time.h>
#endif


USING_NS_CC;


void TimeCalculator::initServerTime(double serverTime)
{
	m_serverTime = serverTime;
	m_clockBegin = getClockTime();
}

double TimeCalculator::getServerTime()
{ 
	return m_serverTime + (getClockTime() - m_clockBegin);
}

double TimeCalculator::getClockTime()
{

#if CC_TARGET_PLATFORM ==CC_PLATFORM_ANDROID
    struct timespec now = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &now);
    //CCLOG("CLOCK_REALTIME: %lu, %lu\n", now.tv_sec, now.tv_nsec);
    return now.tv_sec + (double)now.tv_nsec / 1000000000;
#else
    struct timeval now;
    if (gettimeofday(&now, nullptr) != 0)
    {
        CCLOG("error in gettimeofday");
        return 0.0f;
    }
    return now.tv_sec + (double)now.tv_usec / 1000000;
#endif
}

double TimeCalculator::getBeginClockTime()
{
	return m_clockBegin;
}


double TimeCalculator::getSystemTime()
{

	return getServerTime();

	//auto curTimePt = std::chrono::system_clock::now();
	//double curTime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000000>>>(curTimePt.time_since_epoch()).count();

	//std::chrono::time_point<std::chrono::high_resolution_clock> curTimePt2 = std::chrono::high_resolution_clock::now();
	//double curTime2 = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000000>>>(curTimePt2.time_since_epoch()).count();
	////CCLOG("%u", curTime / 1000000.0f);
	////CCLOG("%u", getCurrentTime());
	//return curTime / 1000000.0f;
}

unsigned long long TimeCalculator::getCurrentTime()
{
	timeval now;
	gettimeofday(&now, NULL);

	unsigned long long part1 =(unsigned long long)now.tv_sec * 1000;
	unsigned long long part2 = (unsigned long long)now.tv_usec / 1000;

	return part1 + part2;
}

TimeCalculator::TimeCalculator() :m_serverTime(0), m_clockBegin(0)
{
	init();
}

void TimeCalculator::init()
{
	//mInitSeconds = utils::gettime();
	//mCurrentSeconds = utils::gettime();
	//mPauseSeconds = 0.0f;

	mInitSeconds = getSystemTime();
	mCurrentSeconds = mInitSeconds;
	mPauseSeconds = 0.0f;
}

void TimeCalculator::update()
{
	//mCurrentSeconds = utils::gettime();

	mCurrentSeconds = getSystemTime();
}


void TimeCalculator::createTimeCalcultor( std::string key, float timeleft )
{  
	TIME_INFO info;
	info.timeleft = timeleft;
	info.registerTime = mCurrentSeconds;
	info.lastGetTime = 0;

	std::map<std::string ,TIME_INFO>::iterator it = mTimes.find(key);

	if(mTimes.find(key)!=mTimes.end())
		it->second = info;	
	else
		mTimes.insert(std::make_pair(key,info));

}

void TimeCalculator::removeTimeCalcultor( std::string key )
{
	if( mTimes.find(key) != mTimes.end() )
		mTimes.erase(key);
}

void TimeCalculator::resetTimeCalcultor(std::string key)
{
	std::map<std::string, TIME_INFO>::iterator it = mTimes.find(key);
	if (it != mTimes.end())
	{
		it->second.registerTime = getSystemTime();
	}
}

bool TimeCalculator::getTimeLeft(std::string key, float& timeleft)
{
	
	std::map<std::string ,TIME_INFO>::iterator it = mTimes.find(key);
	if(it==mTimes.end())
	{
		//CCLOG("[error] TimeCalculator::getTimeLeft get TIME_INFO:%s error!", key.c_str());
		return false;
	} 
	timeleft = it->second.timeleft - (mCurrentSeconds - it->second.registerTime); 
	if(it->second.lastGetTime==mCurrentSeconds)
		return false;
	else 
		return true;
}


float TimeCalculator::getTimeLeft( std::string str)
{
	float ret;
	if(getTimeLeft(str,ret))
	{
		if(ret>0)
			return ret;
		else
			return 0;
	}
	else
		return -1;
}

float TimeCalculator::getCountDownByKey(std::string key)
{
	return getTimeLeft(key);
}

bool TimeCalculator::hasKey(std::string key)
{
	std::map<std::string ,TIME_INFO>::iterator it = mTimes.find(key);
	if(it==mTimes.end())
	{
		return false;
	}
	return true;
}

void TimeCalculator::pause()
{
	mPauseSeconds = mCurrentSeconds;
	CCLOG("[trace] TimeCalculator::pause()");
} 

void TimeCalculator::resume()
{ 
	CCLOG("[trace] TimeCalculator::resume1()");
	if (mPauseSeconds <= 0.0f)
		return;

	//mCurrentSeconds = utils::gettime();
	mCurrentSeconds = getSystemTime();
	double last = mCurrentSeconds - mPauseSeconds;

	std::map<std::string, TIME_INFO>::iterator it = mTimes.begin();
	for (; it != mTimes.end(); ++it)
	{
		it->second.registerTime += last; 
	} 
	mPauseSeconds = 0;
	CCLOG("[trace] TimeCalculator::resume2()");
	
}

void TimeCalculator::cleanData()
{
	std::map<std::string, TIME_INFO>::iterator it = mTimes.begin();
	for (; it != mTimes.end();)
	{
		if (it->first == TIME_PVP_PING || it->first == TIME_CALC_PVP || it->first == TIME_CALC || it->first == TIME_PVP_FullSrv_Invite)
		{
			++it;
			continue;
		}
		it = mTimes.erase(it);
	}
	//mTimes.clear();
	mPauseSeconds = 0;
}

void TimeCalculator::addTimeByKey(std::string key, float time)
{
	std::map<std::string, TIME_INFO>::iterator it = mTimes.find(key);
	if (it != mTimes.end())
	{
		it->second.timeleft += time;
	}
}

double TimeCalculator::getCurTime()
{
	return mCurrentSeconds;
}

float TimeCalculator::getTimeLength(std::string key)
{
	std::map<std::string, TIME_INFO>::iterator it = mTimes.find(key);
	if (it != mTimes.end())
	{
		//CCLOG("[error] TimeCalculator::getTimeLeft get TIME_INFO:%s error!", key.c_str());
		return it->second.timeleft;
	}

	return 0;
}

void TimeCalculator::beganMainCountDown()
{
	double serverTime = getServerTime();
	sgRankingControl()->setAwardTime(serverTime - 100);	//客户端比服务器时间晚一点，给服务器计算时间
	Director::getInstance()->getScheduler()->schedule(schedule_selector(TimeCalculator::updateCountDown), sgTimeCalculator(), 1, false);
}

void TimeCalculator::updateCountDown(float dt)
{
	//周排行奖励
	double standardAwardTime = sgRankingControl()->getAwardTime();
	standardAwardTime += dt;
	if (standardAwardTime < 0)
	{
		standardAwardTime = 0;
	}

	sgRankingControl()->setAwardTime(standardAwardTime);

	//巅峰排行
	sgtopRankModel_Cpp()->setTimeStar(sgtopRankModel_Cpp()->getTimeStar() - dt);
	sgtopRankModel_Cpp()->setTimeWin(sgtopRankModel_Cpp()->getTimeWin() - dt);
	sgtopRankModel_Cpp()->setTimeWinStreak(sgtopRankModel_Cpp()->getTimeWinStreak() - dt);
}
