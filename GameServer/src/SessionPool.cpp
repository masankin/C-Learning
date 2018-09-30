#include <assert.h>
#include "SessionPool.h"
#include "Session.h"
#include "SessionList.h"

//=============================================================================================================================
/**
	@remarks
			생성자
	@param	nSize
			
*/
//=============================================================================================================================
SessionPool::SessionPool( DWORD dwSize,
						  DWORD dwSendBufferSize,
						  DWORD dwRecvBufferSize, 
						  DWORD dwMaxPacketSize,
						  DWORD dwTimeOutTick,
						  DWORD dwIndexStart,
						  BOOL bAcceptSocket )
{
	m_pList = NULL;
	Create( dwSize, dwSendBufferSize, dwRecvBufferSize, dwMaxPacketSize, dwTimeOutTick, dwIndexStart, bAcceptSocket );
}

//=============================================================================================================================
/**
	@remarks
			
*/
//=============================================================================================================================
SessionPool::~SessionPool()
{
	if( m_pList ) delete m_pList;
}

//=============================================================================================================================
/**
	@remarks
			세션 풀 생성
	@param	dwSize
			생성할 세션 개수
*/
//=============================================================================================================================
VOID SessionPool::Create( DWORD dwSize, DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize, DWORD dwTimeOutTick, DWORD dwIndexStart, BOOL bAcceptSocket )
{
	m_dwMaxSize			= dwSize;

	// 세션 리스트 생성
	m_pList = new SessionList;

	// 리스트에 인스턴스들을 채운다.
	Session *pSession;

	for( DWORD i = 0; i < dwSize; ++i )
	{
		pSession = new Session( dwSendBufferSize, dwRecvBufferSize, dwMaxPacketSize, dwTimeOutTick );

		pSession->SetIndex( dwIndexStart + i );

		if( bAcceptSocket )
		{
			pSession->SetAcceptSocketFlag();
		}


		m_pList->push_back( pSession );
	}
}

//=============================================================================================================================
/**
	@remarks
			세션 풀에서 세션의 포인터를 꺼내서 리턴해준다.
	@retval	Session*
			세션 풀로부터 할당 받은 세션의 포인터
*/
//=============================================================================================================================
Session* SessionPool::Alloc()
{
	m_pList->Lock();

	if( m_pList->empty() )
	{
		m_pList->Unlock();
		
		return NULL;
	}

	Session *pSession = m_pList->front();

	m_pList->pop_front();

	//printf( "New 세션풀(%x) 남은 세션(%d)\n", this, m_pList->size() ); 

	m_pList->Unlock();

	return pSession;
}

//=============================================================================================================================
/**
	@remarks
			사용하던 세션을 세션 풀에 반환한다.
	@param	pSession
			세션 풀에 반환할 세션의 포인터
*/
//=============================================================================================================================
VOID SessionPool::Free( Session* pSession )
{
	m_pList->Lock();

	assert( m_pList->size() < m_dwMaxSize );

	m_pList->push_back( pSession );

	m_pList->Unlock();
}

//=============================================================================================================================
/**
	@remarks
			현재 세션 풀에 남아 있는 세션의 개수를 리턴한다. (사용 가능한 세션의 수)
	@retval	int
			사용 가능한 세션의 수
*/
//=============================================================================================================================
int SessionPool::GetLength()
{
	m_pList->Lock();

	int size = (int)( m_pList->size() );

	m_pList->Unlock();

	return size;
}
