#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include <windows.h>

#define MAX_CONNECT_THREAD	16

class IoHandler;
class Session;
class SessionList;

//=============================================================================================================================
//���ӵ�����ķ��������÷������������Ӧ
/**
	@ע�⣺
			Connect( char *, WORD ) ������øú���ʱ���߳��Ѿ��������ˡ�
	@����
			Operation to generate a thread for the session will attempt to connect
			ΪSession�����߳�,�Խ������ӡ�

	@par
			�� ������ ���ῡ Success�ϸ� ������ PreConnectedList�� �߰��ǰ� �����ϸ� ConnectFailList��
			�߰��ȴ�.
	@par
			Server update of the session in the City PreConnectedList OnConnect (TRUE) is called,
		 
			Server ������Ʈ�� PreConnectedList�� �ִ� ���ǵ鿡 ���ؼ��� OnConnect(TRUE)�� ȣ��ǰ�,
			ConnectFailList�� �ִ� ���ǿ� ���ؼ��� OnConnect(FALSE)�� ȣ��ȴ�.
	@see
			Session
*/
//=============================================================================================================================
class Connector
{
	friend unsigned __stdcall connect_thread( LPVOID param );

public:
	Connector(void);
	~Connector(void);

	VOID			Init( IoHandler *pIoHandler, DWORD dwNumberOfThreads );
	VOID			Connect( Session *pSession );
	VOID			Shutdown();

private:
	IoHandler		*m_pIoHandler;
	SessionList		*m_pConnectingList;
	HANDLE			m_hThread[MAX_CONNECT_THREAD];
	HANDLE			m_hEvent[2];
	BOOL			m_bShutdown;
	DWORD			m_numThreads;
};

#endif