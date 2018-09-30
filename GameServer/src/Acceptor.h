//---------------------------------------------------------------------------------------------------
/*
文件名： Acceptor.h
时间：2009.4.4
作者：Saint Jack(修改)
版本：1.0
描述：
接受器，接受用户连接。
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
///打开Socket连接Handle
//	-I / O 每个I/O都有一的独立的会话池。
//	- 一旦建立连接，就会把申请空间，在初始化后添加到TempSessionList中。
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


