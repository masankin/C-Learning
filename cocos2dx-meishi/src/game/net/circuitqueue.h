#pragma once

#include "cocos2d.h"
USING_NS_CC;

#include "concurrency.h"
using namespace GameCom;


template <typename T>
class CircuitQueue
{
protected:
	T*					m_pData;
	int					m_nUsed;	// 当前占用的缓存大小
	int					m_nTotal;	// 总缓存大小
	int					m_nHead;
	int					m_nTail;	
	int					m_nExtra;

	//Mutex				m_mutex;

public:

	CircuitQueue() : m_pData(NULL)
	{
		clear();
	}

	virtual ~CircuitQueue()
	{
		clear();
	}

	inline void		clear()
	{
		m_nUsed = 0;
		m_nHead = 0;
		m_nTail = 0;
		m_nExtra = 1024 * 4;
		if (m_pData) delete[] m_pData;
	}

	inline void		create(int nSize)
	{
		int nNew = nSize + m_nExtra;
		m_pData = new T[nNew];
		memset(m_pData, 0, nNew);
		m_nTotal = nSize;
	}

	inline int		getUsed()
	{
		int iused = 0;
		//m_mutex.lock();
		iused = m_nUsed;
		//m_mutex.unlock();
		return iused;
	}

	inline int		getTailUsed()
	{
		int iret = 0;
		//m_mutex.lock();
		iret = m_nTotal - m_nHead;
		//m_mutex.unlock();
		return iret;
	}

	inline bool		enqueue(T* pSrc, int nSize)
	{
		//m_mutex.lock();
		int nTotalFree = m_nTotal - m_nUsed;
		if (nTotalFree < nSize)
		{
			//m_mutex.unlock();
			//CCLOG("[warn]CircuitQueue::enqueue no freespace!");
			return false;
		}

		if (pSrc)
		{
			if (m_nHead <= m_nTail)
			{
				int nTailFree = m_nTotal - m_nTail;
				// 尾部足够插入
				if (nTailFree >= nSize)
				{
					memcpy(m_pData + m_nTail, pSrc, sizeof(T)* nSize);
				}
				else
				{
					memcpy(m_pData + m_nTail, pSrc, sizeof(T)* nTailFree);
					memcpy(m_pData, pSrc + nTailFree, sizeof(T)* (nSize - nTailFree));
				}
			}
			else
			{
				memcpy(m_pData + m_nTail, pSrc, sizeof(T)* nSize);
			}
		}

		m_nTail += nSize;
		m_nTail %= m_nTotal;
		m_nUsed += nSize;
		//CCLOG("[trace]CircuitQueue::enqueue size:%d head:%d tail:%d used:%d!", nSize, m_nHead, m_nTail, m_nUsed);
		//m_mutex.unlock();
		return true;
	}

	inline bool		peek(T *pTar, int nSize)
	{
		//m_mutex.lock();
		if (pTar != NULL)
		{
			if (m_nUsed < nSize)
				return false;

			if (m_nHead < m_nTail)
			{
				memcpy(pTar, m_pData + m_nHead, sizeof(T)* nSize);
			}
			else
			{
				int nTailUsed = m_nTotal - m_nHead;// getTailUsed();
				if (nTailUsed >= nSize)
				{
					memcpy(pTar, m_pData + m_nHead, sizeof(T)* nSize);
				}
				else
				{
					memcpy(pTar, m_pData + m_nHead, sizeof(T)* nTailUsed);
					memcpy(pTar + nTailUsed, m_pData, sizeof(T)* (nSize - nTailUsed));
				}
			}
		}
		//m_mutex.unlock();
		return true;
	}

	inline bool		dequeue(T* pTar, int nSize)
	{
		//m_mutex.lock();
		if (peek(pTar, nSize) == false)
		{
			//m_mutex.unlock();
			return false;
		}

		m_nHead += nSize;
		m_nHead %= m_nTotal;
		m_nUsed -= nSize;
		//CCLOG("[trace]CircuitQueue::dequeue size:%d head:%d tail:%d used:%d!", nSize, m_nHead, m_nTail, m_nUsed);
		//m_mutex.unlock();
		return true;
	}

	inline T*       getReadPtr()
	{
		//m_mutex.lock();
		T* pRet = m_pData + m_nHead;

		int nTailUsed = m_nTotal - m_nHead;// getTailUsed();
		if (m_nHead > m_nTail && nTailUsed < m_nExtra)
		{
			//memcpy(m_pData + m_nTotal, m_pData, sizeof(T)* (m_nExtra - nTailUsed));
			//copyHeader2Extra(sizeof(T)* (m_nExtra - nTailUsed));

			memcpy(m_pData + m_nTotal, m_pData, sizeof(T)* (m_nUsed - nTailUsed));
			//copyHeader2Extra(sizeof(T)* (m_nExtra - nTailUsed));
		}
		//m_mutex.unlock();
		return pRet;
	}

	inline int		getReadableLen()
	{
		int iRet;
		//m_mutex.lock();
		if (m_nHead == m_nTail)		iRet = m_nUsed > 0 ? m_nUsed : 0;
		else if (m_nHead < m_nTail)	iRet = m_nTail - m_nHead;
		else						iRet = m_nUsed;
		//m_mutex.unlock();
		return iRet;
	}

	inline T*       getWritePtr()
	{
		T *pRet = NULL;
		//m_mutex.lock();
		pRet = m_pData + m_nTail;
		//m_mutex.unlock();
		return pRet;
	}

	inline int		getWritableLen()
	{
		int iRet;
		//m_mutex.lock();
		if (m_nHead == m_nTail)		iRet = m_nUsed > 0 ? 0 : m_nTotal - m_nTail;
		else if (m_nHead < m_nTail)	iRet = m_nTotal - m_nUsed;
		else						iRet = m_nHead - m_nTail;
		//m_mutex.unlock();
		return iRet;
	}

	inline void		copyHeader2Extra(int nSize)
	{
		//m_mutex.lock();
		memcpy(m_pData + m_nTotal, m_pData, nSize);
		////m_mutex.unlock();
	}
};
