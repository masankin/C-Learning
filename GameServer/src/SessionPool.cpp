#include <assert.h>
#include "SessionPool.h"
#include "Session.h"
#include "SessionList.h"

//=============================================================================================================================
/**
	@remarks
			������
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
			���� Ǯ ����
	@param	dwSize
			������ ���� ����
*/
//=============================================================================================================================
VOID SessionPool::Create( DWORD dwSize, DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize, DWORD dwTimeOutTick, DWORD dwIndexStart, BOOL bAcceptSocket )
{
	m_dwMaxSize			= dwSize;

	// ���� ����Ʈ ����
	m_pList = new SessionList;

	// ����Ʈ�� �ν��Ͻ����� ä���.
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
			���� Ǯ���� ������ �����͸� ������ �������ش�.
	@retval	Session*
			���� Ǯ�κ��� �Ҵ� ���� ������ ������
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

	//printf( "New ����Ǯ(%x) ���� ����(%d)\n", this, m_pList->size() ); 

	m_pList->Unlock();

	return pSession;
}

//=============================================================================================================================
/**
	@remarks
			����ϴ� ������ ���� Ǯ�� ��ȯ�Ѵ�.
	@param	pSession
			���� Ǯ�� ��ȯ�� ������ ������
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
			���� ���� Ǯ�� ���� �ִ� ������ ������ �����Ѵ�. (��� ������ ������ ��)
	@retval	int
			��� ������ ������ ��
*/
//=============================================================================================================================
int SessionPool::GetLength()
{
	m_pList->Lock();

	int size = (int)( m_pList->size() );

	m_pList->Unlock();

	return size;
}
