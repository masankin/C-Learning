#pragma once

#include "Singleton.h"

using namespace std;

// CVTimeSpan
class CVTimeSpan
{
public:
	CVTimeSpan() {}
	CVTimeSpan(time_t time) { m_timeSpan = time; }
	CVTimeSpan(long lDays, int nHours, int nMins, int nSecs) { m_timeSpan = nSecs + 60* (nMins + 60* (nHours + 24* lDays)); }

	CVTimeSpan(const CVTimeSpan& timeSpanSrc) { m_timeSpan = timeSpanSrc.m_timeSpan; }
	const CVTimeSpan& operator=(const CVTimeSpan& timeSpanSrc) { m_timeSpan = timeSpanSrc.m_timeSpan; return *this; }

	int GetDays() const { return static_cast<int>(m_timeSpan / (24 * 3600)); }
    int GetHours() const { return static_cast<int>(GetTotalHours() - GetDays() * 24); }
    int GetMinutes() const { return static_cast<int>(GetTotalMinutes() - GetTotalHours() * 60); }
    int GetSeconds() const { return static_cast<int>(GetTotalSeconds() - GetTotalMinutes() * 60); }

    int GetTotalHours() const { return static_cast<int>(m_timeSpan / 3600); }
	int GetTotalMinutes() const { return static_cast<int>(m_timeSpan / 60); }
	int GetTotalSeconds() const { return static_cast<int>(m_timeSpan); }

	CVTimeSpan operator-(CVTimeSpan timeSpan) const { return CVTimeSpan(m_timeSpan - timeSpan.m_timeSpan); }
	CVTimeSpan operator+(CVTimeSpan timeSpan) const { return CVTimeSpan(m_timeSpan + timeSpan.m_timeSpan); }
	const CVTimeSpan& operator+=(CVTimeSpan timeSpan) { m_timeSpan += timeSpan.m_timeSpan; return *this; }
	const CVTimeSpan& operator-=(CVTimeSpan timeSpan) { m_timeSpan -= timeSpan.m_timeSpan; return *this; }
	bool operator==(CVTimeSpan timeSpan) const { return m_timeSpan == timeSpan.m_timeSpan; }
	bool operator!=(CVTimeSpan timeSpan) const { return m_timeSpan != timeSpan.m_timeSpan; }
	bool operator<(CVTimeSpan timeSpan) const  { return m_timeSpan < timeSpan.m_timeSpan; }
	bool operator>(CVTimeSpan timeSpan) const  { return m_timeSpan > timeSpan.m_timeSpan; }
	bool operator<=(CVTimeSpan timeSpan) const { return m_timeSpan <= timeSpan.m_timeSpan; }
	bool operator>=(CVTimeSpan timeSpan) const { return m_timeSpan >= timeSpan.m_timeSpan; }

    /**
    * 时间格式化.
    * 仅支持以下格式参数
    *    -  %D - 天数
    *    -  %H - 小时 (0-23)
    *    -  %M - 分 (0-59)
    *    -  %S - 秒 (0-59)
    *    -  %% - 百分符号
    */
	/*string Format(const char *pszFormat) const
	{
		char szBuffer[32];
		char ch = 0;
        string s;
	
		while ((ch = *pszFormat++) != '\0')
		{
			if (ch == '%')
			{
				switch (ch = *pszFormat++)
				{
				case '%':
					s += ch;
					break;
				case 'D':
                    ct_snprintf(CV_STRSIZE(szBuffer), "%04d", GetDays());
					s += szBuffer;
					break;
				case 'H':
                    ct_snprintf(CV_STRSIZE(szBuffer), "%02d", GetHours());
                    s += szBuffer;
                    break;
				case 'M':
                    ct_snprintf(CV_STRSIZE(szBuffer), "%02d", GetMinutes());
                    s += szBuffer;
                    break;
				case 'S':
                    ct_snprintf(CV_STRSIZE(szBuffer), "%02d", GetSeconds());
                    s += szBuffer;
                    break;
				default:
					return "";
				}
			}
			else
			{
                s += ch;
			}
		}

		return s;
	}*/

private:
	time_t m_timeSpan;
	friend class CVTime;
};	


// CVTime
class CVTime
{
public:
	CVTime() {}
	CVTime(time_t time)  { m_time = time; }
	CVTime(struct tm t) 
	{
		m_time = mktime(&t);
	}
	
	CVTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST = -1)
	{
		struct tm atm;
		atm.tm_sec = nSec;
		atm.tm_min = nMin;
		atm.tm_hour = nHour;
		atm.tm_mday = nDay;
		atm.tm_mon = nMonth - 1;        // tm_mon is 0 based
		atm.tm_year = nYear - 1900;     // tm_year is 1900 based
		atm.tm_isdst = nDST;
		m_time = mktime(&atm);
	}

    CVTime(const string& strDateTime)
    {
        struct tm stTm;
        if (strDateTime.length() == 19)	// YYYY-mm-dd HH:MM:SS
        {
            stTm.tm_year = strtol(strDateTime.substr(0, 4).c_str(), NULL, 10) - 1900;
            stTm.tm_mon = strtol(strDateTime.substr(5, 2).c_str(), NULL, 10) - 1;
            stTm.tm_mday = strtol(strDateTime.substr(8, 2).c_str(), NULL, 10);
            stTm.tm_hour = strtol(strDateTime.substr(11, 2).c_str(), NULL, 10);
            stTm.tm_min = strtol(strDateTime.substr(14, 2).c_str(), NULL, 10);
            stTm.tm_sec = strtol(strDateTime.substr(17, 2).c_str(), NULL, 10);
            m_time = mktime(&stTm);
            if (m_time == -1)
            {
                m_time = 0;
            }
        }
        else if (strDateTime.length() == 14) // YYYYmmddHHMMSS
        {
            stTm.tm_year = strtol(strDateTime.substr(0, 4).c_str(), NULL, 10) - 1900;
            stTm.tm_mon = strtol(strDateTime.substr(4, 2).c_str(), NULL, 10) - 1;
            stTm.tm_mday = strtol(strDateTime.substr(6, 2).c_str(), NULL, 10);
            stTm.tm_hour = strtol(strDateTime.substr(8, 2).c_str(), NULL, 10);
            stTm.tm_min = strtol(strDateTime.substr(10, 2).c_str(), NULL, 10);
            stTm.tm_sec = strtol(strDateTime.substr(12, 2).c_str(), NULL, 10);
            m_time = mktime(&stTm);
            if (m_time == -1)
            {
                m_time = 0;
            }
        }
        else if (strDateTime.length() == 10)	// YYYY-mm-dd
        {
            stTm.tm_year = strtol(strDateTime.substr(0, 4).c_str(), NULL, 10) - 1900;
            stTm.tm_mon = strtol(strDateTime.substr(5, 2).c_str(), NULL, 10) - 1;
            stTm.tm_mday = strtol(strDateTime.substr(8, 2).c_str(), NULL, 10);
            stTm.tm_hour = 0;
            stTm.tm_min = 0;
            stTm.tm_sec = 0;
            m_time = mktime(&stTm);
            if (m_time == -1)
            {
                m_time = 0;
            }
        }
        else if (strDateTime.length() == 8)     // YYYYmmdd
        {
            stTm.tm_year = strtol(strDateTime.substr(0, 4).c_str(), NULL, 10) - 1900;
            stTm.tm_mon = strtol(strDateTime.substr(4, 2).c_str(), NULL, 10) - 1;
            stTm.tm_mday = strtol(strDateTime.substr(6, 2).c_str(), NULL, 10);
            stTm.tm_hour = 0;
            stTm.tm_min = 0;
            stTm.tm_sec = 0;
            m_time = mktime(&stTm);
            if (m_time == -1)
            {
                m_time = 0;
            }
        }
        else
        {
            m_time = 0;
        }
    }

	CVTime(const CVTime& timeSrc) { m_time = timeSrc.m_time; }

	const CVTime& operator=(const CVTime& timeSrc) { m_time = timeSrc.m_time; return *this; }
	const CVTime& operator=(time_t t) { m_time = t; return *this; }

	struct tm* GetGmtTm(struct tm* ptm = NULL) const
	{
		if (ptm != NULL)
		{
			*ptm = *gmtime(&m_time);
			return ptm;
		}
		else
		{
			return gmtime(&m_time);
		}
	}
	
	struct tm* GetLocalTm(struct tm* ptm = NULL) const
	{
		if (ptm != NULL)
		{
			struct tm* ptmTemp = localtime(&m_time);
			if (ptmTemp == NULL)
            {
				return NULL;    // indicates the m_time was not initialized!
            }

			*ptm = *ptmTemp;
			return ptm;
		}
		else
		{
			return localtime(&m_time);
		}
	}

	time_t GetTime() const { return m_time; }
	int GetYear() const { return (GetLocalTm(NULL)->tm_year) + 1900; }
	int GetMonth() const { return GetLocalTm(NULL)->tm_mon + 1; }
	int GetDay() const { return GetLocalTm(NULL)->tm_mday; }
	int GetHour() const { return GetLocalTm(NULL)->tm_hour; }
	int GetMinute() const { return GetLocalTm(NULL)->tm_min; }
	int GetSecond() const { return GetLocalTm(NULL)->tm_sec; }

    /**
    * 返回星期几.
    * @return 0=星期天 1=星期一 6=星期六
    */
	int GetDayOfWeek() const { return GetLocalTm(NULL)->tm_wday; }

	CVTimeSpan operator-(CVTime time) const { return CVTimeSpan(m_time - time.m_time); }
	CVTime operator-(CVTimeSpan timeSpan) const { return CVTime(m_time - timeSpan.m_timeSpan); }
	CVTime operator+(CVTimeSpan timeSpan) const { return CVTime(m_time + timeSpan.m_timeSpan); }
	const CVTime& operator+=(CVTimeSpan timeSpan) { m_time += timeSpan.m_timeSpan; return *this; }
	const CVTime& operator-=(CVTimeSpan timeSpan) { m_time -= timeSpan.m_timeSpan; return *this; }
	bool operator==(CVTime time) const { return m_time == time.m_time; }
	bool operator!=(CVTime time) const { return m_time != time.m_time; }
	bool operator<(CVTime time) const  { return m_time < time.m_time; }
	bool operator>(CVTime time) const  { return m_time > time.m_time; }
	bool operator<=(CVTime time) const { return m_time <= time.m_time; }
	bool operator>=(CVTime time) const { return m_time >= time.m_time; }

	string Format(const char* pszFormat) const
	{
		//TODO: jacey
		/* time_t time = m_time;
		struct tm* ptmTemp = localtime(&time);

		
        char szBuffer[256];
        if (ptmTemp == NULL || !strftime(CV_STRSIZE(szBuffer), pszFormat, ptmTemp))
		{
			return "";
		}

		return string(szBuffer);*/	
		return "";
	}

	string FormatGmt(const char* pszFormat) const
	{
		//TODO: jacey
        /*time_t time = m_time;
        struct tm* ptmTemp = gmtime(&time);

        char szBuffer[256];
        if (ptmTemp == NULL || !strftime(CV_STRSIZE(szBuffer), pszFormat, ptmTemp))
        {
            return "";
        }

        return string(szBuffer);	*/
		return "";
	}

    char* Format(char* pszBuffer, int iMaxLen, const char* pszFormat) const
    {
        time_t time = m_time;
        struct tm* ptmTemp = localtime(&time);
        if (ptmTemp == NULL || !strftime(pszBuffer, iMaxLen, pszFormat, ptmTemp))
        {
            pszBuffer[0] = '\0';
        }
        return pszBuffer;
    }

    char* FormatGmt(char* pszBuffer, int iMaxLen, const char* pszFormat) const
    {
        time_t time = m_time;
        struct tm* ptmTemp = gmtime(&time);
        if (ptmTemp == NULL || !strftime(pszBuffer, iMaxLen, pszFormat, ptmTemp))
        {
            pszBuffer[0] = '\0';
        }
        return pszBuffer;
    }

private:
	time_t m_time;
};

// CVTimeUtils
class CVTimeUtils
{
public:
	static struct timeval& GetNow(struct timeval &tv)
	{
#ifdef WIN32
		SYSTEMTIME sy;
		GetSystemTime(&sy);
		tv.tv_usec = sy.wMilliseconds * 1000;
		tv.tv_sec = static_cast<long>(time(0));
#else
		gettimeofday(&tv, NULL);
#endif

        return tv;
	}

    /**
    * 计算两个时间的差
    * @return 返回差值, 单位微妙
    */
	static int Diff(const struct timeval& tv1, const struct timeval& tv2)
	{
		return (tv1.tv_sec - tv2.tv_sec) * 1000000 + 
			   (tv1.tv_usec - tv2.tv_usec);
	}

    /**
    * 比较两个时间是否是同一天
    * @return true/false
    */
    static bool IsSameDay(time_t tTime1, time_t tTime2)
    {
    	CVTime tmTime1(tTime1);
		CVTime tmTime2(tTime2);
        return (tmTime1.GetYear() == tmTime2.GetYear()
			&& tmTime1.GetMonth() == tmTime2.GetMonth()
			&& tmTime1.GetDay() == tmTime2.GetDay());
    }

    static bool IsSameWeek(time_t tTime1, time_t tTime2)
    {
        time_t tOld = tTime1>tTime2?tTime2:tTime1;
        time_t tNew = tTime1>=tTime2?tTime1:tTime2;
        tOld = GetDayZero(tOld);
        tNew = GetDayZero(tNew);
        CVTime tmOld(tOld);
        CVTime tmNew(tNew);
        int iDays = (tNew - tOld) / (24 * 3600);
        int iDowNew = tmNew.GetDayOfWeek();
        if (iDowNew == 0)
        {
            iDowNew = 7;
        }

        if (iDays >= 7 || iDays >= iDowNew)
        {
            return false;
        }
        
        return true;
    }

    //获取第二天北京时间零点
    static int GetNextDayZero(time_t tTime)
    {
        struct tm stTemp;
#ifdef WIN32
        localtime(&tTime);
#else
        localtime_r(&tTime, &stTemp);
#endif
        stTemp.tm_hour = 0;
        stTemp.tm_min = 0;
        stTemp.tm_sec = 0;
        return mktime(&stTemp) + 86400;
    }

    static int GetDayZero(time_t tTime)
    {
        struct tm stTemp;
#ifdef WIN32
        localtime(&tTime);
#else
        localtime_r(&tTime, &stTemp);
#endif
        stTemp.tm_hour = 0;
        stTemp.tm_min = 0;
        stTemp.tm_sec = 0;
        return mktime(&stTemp);
    }
};

#define sgTimeUtils()	Singleton<CVTimeUtils>::Get()
