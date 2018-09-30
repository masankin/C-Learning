#ifndef _SESSIONPOOL_H_
#define _SESSIONPOOL_H_

#include <windows.h>
#include <vector>

using namespace std;

class Session;
class Session;
class SessionList;

//=============================================================================================================================
/// 세션들을 최대 개수만큼 미리 할당해서 관리하는 세션 풀 잚깎
//		세션풀이 가지고 있던 메모리의 소멸은 세션풀에서 담당한다.
//=============================================================================================================================
class SessionPool
{
public:
	SessionPool( DWORD dwSize, DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize, DWORD dwTimeOutTick, DWORD dwIndexStart, BOOL bAcceptSocket );
	~SessionPool();

	Session*		Alloc();
	void			Free( Session* pSession );
	int				GetLength();

private:
	VOID			Create( DWORD dwSize, DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwTimeOutTick, DWORD dwMaxPacketSize, DWORD dwIndexStart, BOOL bAcceptSocket );

	DWORD			m_dwMaxSize;
	SessionList		*m_pList;
};

#endif
