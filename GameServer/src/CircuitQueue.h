//---------------------------------------------------------------------------------------------------
/*
�ļ�����CircuitQueue.h
�������̰߳�ȫ ��ѭ������ģ��

*/
//---------------------------------------------------------------------------------------------------
#ifndef _CIRCUITQUEUE_H_
#define _CIRCUITQUEUE_H_

#include <windows.h>
#include <stdio.h>
#include <assert.h>

//=============================================================================================================================
/// ѭ������ģ��
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
			��������
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
			��ն���
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
	
	/// ���ض���ʣ�µĿռ��С
	inline int      GetSpace()
	{
		int iRet;

		EnterCriticalSection( &m_cs );
		iRet = m_nSize - m_nLength;		
		LeaveCriticalSection( &m_cs );

		return iRet;
	}

	///�����Ѿ�ʹ�õĶ��еĳ��ȡ�
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
			�����ݷ��붼������.
	@param	pSrc
			Ҫ��������ݵ�ָ��
	@param	nSize
			�������ݵĳ���
	@retval	BOOL
			�ڶ�β�������ݳɹ�����TRUE
			������ܲ������ݾͷ���FALSE
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
				// 1. ���HEAD��tail֮ǰ
				int nBackSpaceCount = m_nSize - m_nTail;

				if( nBackSpaceCount >= nSize )  
				{
					// �������ʣ��Ŀռ�>=��ǰ�������ݵĴ�С.
					memcpy( m_pData + m_nTail, pSrc, sizeof(T) * nSize );
				}
				else
				{
					// �����������>����βʣ��ؼ����ͷ����ο�����
					memcpy( m_pData + m_nTail, pSrc, sizeof(T) * nBackSpaceCount );
					memcpy( m_pData, pSrc + nBackSpaceCount, sizeof(T) * ( nSize - nBackSpaceCount ) );
				}
			}
			else
			{
				// 2. ���HEAD��TAIL֮��
				memcpy( m_pData + m_nTail, pSrc, sizeof(T) * nSize );
			}
		}

		/*
		if( bCopyToExtraBuffer )
		{
			// ������ �߸� �����͸� ������ ���ۿ��� �������ش�.
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
			�Ӷ������Ƴ�����
	@param	pTar
			Remove the data copied from the queue of the buffer pointer
	@param	nSize
			Ҫ�Ƴ������ݵĴ�С
	@retval	BOOL
			����ɹ��Ƴ����ݷ���TRUE�����򷵻�FALSE
		
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
			�Ӷ����а����ݿ�������.
	@param	pTar
			Pointer of the buffer to copy data to a queue
	@param	nSize
			Copy the data queue length
	@retval	BOOL
			��������ɹ��ͷ���TRUE�����򷵻�FALSE.
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
				// 1. ���HEAD��tail֮ǰ
				memcpy( pTar, m_pData + m_nHead, sizeof(T) * nSize );
			}
			else
			{
				// 2. head�� tail֮��
				if( GetBackDataCount() >= nSize )
				{
					// �������ʣ��Ŀռ�>=��ǰ�������ݵĴ�С.
					memcpy( pTar, m_pData + m_nHead, sizeof(T) * nSize );                           
				}
				else
				{
					// �����������>����βʣ��ؼ����ͷ����ο�����
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
	T					*m_pData;			/// ����BUFFERָ��
	int					m_nLength;			/// ����BUFFER����
	int					m_nSize;			/// The total size of the buffer queue
	int					m_nHead;			/// ��ǰ���ݵĿ�ʼλ��
	int					m_nTail;			/// ��ǰ���ݵĽ���λ��
	int					m_nExtraSize;		/// Get extra in the back of the queue buffer size 
};

#endif










