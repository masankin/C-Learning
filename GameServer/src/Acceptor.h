//---------------------------------------------------------------------------------------------------
/*
�ļ����� Acceptor.h
ʱ�䣺2009.4.4
���ߣ�Saint Jack(�޸�)
�汾��1.0
������
�������������û����ӡ�
*/

//---------------------------------------------------------------------------------------------------
#ifndef _IOGROUP_H_
#define _IOGROUP_H_

#include <windows.h>
#include <process.h>
#include <Ws2tcpip.h>

#define MAX_ACCEPT_THREAD	16

class Session;
class IoHandler;
class SessionPool;

typedef struct tagIOHANDLER_DESC IOHANDLER_DESC, *LPIOHANDLER_DESC;

//=============================================================================================================================
///��Socket����Handle
//	-I / O ÿ��I/O����һ�Ķ����ĻỰ�ء�
//	- һ���������ӣ��ͻ������ռ䣬�ڳ�ʼ������ӵ�TempSessionList�С�
//=============================================================================================================================
class Acceptor
{
	friend unsigned __stdcall accept_thread( LPVOID param );

public:
	Acceptor();
	~Acceptor();

	VOID				Init( IoHandler *pIoHandler, DWORD dwNumberOfThreads );
	BOOL				StartListen( char *pIP, WORD wPort );
	VOID				Shutdown();
	inline BOOL			IsListening() { return m_listenSocket != INVALID_SOCKET; }
	VOID				SuspendListenThread();
	VOID				ResumeListenThread();
	inline SOCKET		GetListenSocket() { return m_listenSocket; }

private:
	BOOL				CreateListenSocket( char *pIP, WORD wPort );

	IoHandler			*m_pIoHandler;
	HANDLE				m_hThread[MAX_ACCEPT_THREAD];
	HANDLE				m_hIOCP;
	SOCKET				m_listenSocket;
	SOCKADDR_IN			m_sockaddr;
	DWORD				m_numThreads;
};

#endif


