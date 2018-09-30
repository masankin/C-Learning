//---------------------------------------------------------------------------------------------------
/*
文件名：CircuitQueue.h
描述：线程安全 的循环队列模板

*/
//---------------------------------------------------------------------------------------------------
#ifndef _CIRCUITQUEUE_H_
#define _CIRCUITQUEUE_H_

#include <windows.h>
#include <stdio.h>
#include <assert.h>

//=============================================================================================================================
/// 循环队列模板
//=============================================================================================================================
template<typename T>
class CircuitQueue
{
public:
	CircuitQueue() : m_pData( NULL ), m_nLength( 0 ), m_nSize( 0 ), m_nHead( 0 ), m_nTail( 0 )
	{
		InitializeCriticalSection( &m_cs );		
	}
	
	virtual ~CircuitQueue()
	{
		if( m_pData ) delete [] m_pData;
		DeleteCriticalSection( &m_cs );
	}
	
//=============================================================================================================================
/**
	@remarks
			创建队列
	@param	nSize
			Queue Size
	@param	nExtraSize
	Forward from the back of the packet into the buffer when the packet back to gain extra memory buffer size (actual extra memory = sizeof (T) * nExtraSize)
	*/
//=============================================================================================================================
	void Create( int nSize, int nExtraSize = 0 )
	{
		EnterCriticalSection( &m_cs );
		if( m_pData ) delete [] m_pData;

		m_pData			= new T[nSize + nExtraSize];
		m_nSize			= nSize;
		m_nExtraSize	= nExtraSize;
		LeaveCriticalSection( &m_cs );
	}

//=============================================================================================================================
/**
	@remarks
			清空队列
*/
//=============================================================================================================================
	inline void Clear()
	{
		EnterCriticalSection( &m_cs );

		m_nLength       = 0;
		m_nHead         = 0;
		m_nTail         = 0;

		LeaveCriticalSection( &m_cs );
	}
	
	/// 返回队列剩下的空间大小
	inline int      GetSpace()
	{
		int iRet;

		EnterCriticalSection( &m_cs );
		iRet = m_nSize - m_nLength;		
		LeaveCriticalSection( &m_cs );

		return iRet;
	}

	///返回已经使用的队列的长度。
	inline int      GetLength()
	{
		int iRet;

		EnterCriticalSection( &m_cs );
		iRet = m_nLength;
		LeaveCriticalSection( &m_cs );

		return iRet;
	}

	/// Data to the back of the queue full, the case forward, it will return back to the end of the number of data
	inline int      GetBackDataCount()
	{
		int iRet;

		EnterCriticalSection( &m_cs );
		iRet = m_nSize - m_nHead;
		LeaveCriticalSection( &m_cs );

		return iRet;
	}

	/// Read data pointers (Head) will return.
	inline T*       GetReadPtr()
	{
		T *pRet;

		EnterCriticalSection( &m_cs );
		pRet = m_pData + m_nHead;

		// If you want to read the data back if the extra buffer, the buffer provides the appropriate copy.
		int nSplitFirstDataCount;
		if( m_nHead > m_nTail && ( nSplitFirstDataCount = m_nSize - m_nHead ) < m_nExtraSize )
		{
			memcpy( m_pData + m_nSize, m_pData, sizeof(T) * ( m_nExtraSize - nSplitFirstDataCount ) );
		}

		LeaveCriticalSection( &m_cs );

		return pRet;
	}

	/// Pointer to write data (Tail) will return.
	inline T*       GetWritePtr()
	{
		T *pRet;

		EnterCriticalSection( &m_cs );
		pRet = m_pData + m_nTail;
		LeaveCriticalSection( &m_cs );

		return pRet;
	}
	
	/// You can read without being truncated to the length is returned.
	inline int GetReadableLen()
	{
		int iRet;

		EnterCriticalSection( &m_cs );
		if( m_nHead == m_nTail )		iRet = GetLength() > 0 ? m_nSize - m_nHead: 0;
		else if( m_nHead < m_nTail )	iRet = m_nTail - m_nHead;
		else							 iRet = m_nSize - m_nHead;
		LeaveCriticalSection( &m_cs );

		return iRet;
	}
	
	/// Write to the possible length of time without being truncated is returned.
	inline int GetWritableLen()
	{
		int iRet;

		EnterCriticalSection( &m_cs );
		if( m_nHead == m_nTail )		iRet = GetLength() > 0 ? 0 : m_nSize - m_nTail;
		else if( m_nHead < m_nTail )	iRet = m_nSize - m_nTail;
		else							iRet = m_nHead - m_nTail;
		LeaveCriticalSection( &m_cs );

		return iRet;
	}

//=============================================================================================================================
/**
	@remarks
			把数据放入都队列中.
	@param	pSrc
			要插入的数据的指针
	@param	nSize
			插入数据的长度
	@retval	BOOL
			在队尾插入数据成功返回TRUE
			如果不能插入数据就返回FALSE
*/
//=============================================================================================================================
	inline bool Enqueue( T *pSrc, int nSize )
	{
		EnterCriticalSection( &m_cs );

		if( GetSpace() < nSize )
		{
			LeaveCriticalSection( &m_cs );
			return false;
		}

		/*
		// If copies of the data is divided in two?
		BOOL bCopyToExtraBuffer = ( ( m_nHead <= m_nTail ) && ( m_nSize - m_nTail < nSize ) );
		*/

		// If NULL, pSrc the tail and the length of the data without queuing to renew.
		if( pSrc )
		{
			if( m_nHead <= m_nTail )
			{
				// 1. 如果HEAD在tail之前
				int nBackSpaceCount = m_nSize - m_nTail;

				if( nBackSpaceCount >= nSize )  
				{
					// 如果队列剩余的空间>=当前插入数据的大小.
					memcpy( m_pData + m_nTail, pSrc, sizeof(T) * nSize );
				}
				else
				{
					// 如果插入数据>队列尾剩余控件，就分两次拷贝。
					memcpy( m_pData + m_nTail, pSrc, sizeof(T) * nBackSpaceCount );
					memcpy( m_pData, pSrc + nBackSpaceCount, sizeof(T) * ( nSize - nBackSpaceCount ) );
				}
			}
			else
			{
				// 2. 如果HEAD在TAIL之后
				memcpy( m_pData + m_nTail, pSrc, sizeof(T) * nSize );
			}
		}

		/*
		if( bCopyToExtraBuffer )
		{
			// 第率狼 肋赴 单捞磐甫 咯盒狼 滚欺俊档 汗荤秦霖促.
			memcpy( m_pData + m_nSize, pSrc + m_nSize - m_nTail, sizeof(T) * ( nSize - ( m_nSize - m_nTail ) ) );
		}
		*/

		m_nTail		+= nSize;
		m_nTail		%= m_nSize;
		m_nLength	+= nSize;

		LeaveCriticalSection( &m_cs );

		return true;
	}

//=============================================================================================================================
/**
	@remarks
			从队列中移除数据
	@param	pTar
			Remove the data copied from the queue of the buffer pointer
	@param	nSize
			要移除的数据的大小
	@retval	BOOL
			如果成功移除数据返回TRUE，否则返回FALSE
		
*/
//=============================================================================================================================
	inline bool Dequeue( T *pTar, int nSize )
	{    
		EnterCriticalSection( &m_cs );

		if( !Peek( pTar, nSize ) )
		{
			LeaveCriticalSection( &m_cs );
			return false;
		}

		m_nHead		+= nSize;
		m_nHead		%= m_nSize;
		m_nLength	-= nSize;

		LeaveCriticalSection( &m_cs );

		return true;
	}

//=============================================================================================================================
/**
	@remarks
			从队列中把数据拷贝出来.
	@param	pTar
			Pointer of the buffer to copy data to a queue
	@param	nSize
			Copy the data queue length
	@retval	BOOL
			如果拷贝成功就返回TRUE，否则返回FALSE.
*/
//=============================================================================================================================
	inline bool Peek( T *pTar, int nSize )
	{
		EnterCriticalSection( &m_cs );

		if( m_nLength < nSize )
		{
			LeaveCriticalSection( &m_cs );
			return false;
		}

		// Receive information only if non-NULL pointer will be copied to memory.
		if( pTar != NULL )
		{
			if( m_nHead < m_nTail )
			{
				// 1. 如果HEAD在tail之前
				memcpy( pTar, m_pData + m_nHead, sizeof(T) * nSize );
			}
			else
			{
				// 2. head在 tail之后
				if( GetBackDataCount() >= nSize )
				{
					// 如果队列剩余的空间>=当前插入数据的大小.
					memcpy( pTar, m_pData + m_nHead, sizeof(T) * nSize );                           
				}
				else
				{
					// 如果插入数据>队列尾剩余控件，就分两次拷贝。
					memcpy( pTar, m_pData + m_nHead, sizeof(T) * GetBackDataCount() );
					memcpy( pTar + GetBackDataCount(), m_pData, sizeof(T) * ( nSize - GetBackDataCount() ) );
				}
			}
		}

		LeaveCriticalSection( &m_cs );//

		return true;
	}

//=============================================================================================================================
/**
	@remarks
			When a packet is a packet buffer divided the front from the back of the copy is to make the extra space.
	@param	nSize
			Truncated length of the data resulted in the back of the front
*/
//=============================================================================================================================
	inline void CopyHeadDataToExtraBuffer( int nSize )
	{
		assert( nSize <= m_nExtraSize );

		EnterCriticalSection( &m_cs );

		// Man in front of the back of the extra buffer, the data buffer, write buffer plus
		memcpy( m_pData + m_nSize, m_pData, nSize );

		LeaveCriticalSection( &m_cs );
	}

protected:
	CRITICAL_SECTION	m_cs;
	T					*m_pData;			/// 队列BUFFER指针
	int					m_nLength;			/// 队列BUFFER长度
	int					m_nSize;			/// The total size of the buffer queue
	int					m_nHead;			/// 当前数据的开始位置
	int					m_nTail;			/// 当前数据的结束位置
	int					m_nExtraSize;		/// Get extra in the back of the queue buffer size 
};

#endif










