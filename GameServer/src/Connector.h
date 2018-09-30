#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include <windows.h>

#define MAX_CONNECT_THREAD	16

class IoHandler;
class Session;
class SessionList;

//=============================================================================================================================
//连接到另外的服务器，该服务器会进行响应
/**
	@注意：
			Connect( char *, WORD ) 当你调用该函数时，线程已经创建好了。
	@参数
			Operation to generate a thread for the session will attempt to connect
			为Session创建线程,以建立连接。

	@par
			弊 技记捞 楷搬俊 Success窍搁 辑滚狼 PreConnectedList俊 眠啊登绊 角菩窍搁 ConnectFailList俊
			眠啊等促.
	@par
			Server update of the session in the City PreConnectedList OnConnect (TRUE) is called,
		 
			Server 诀单捞飘矫 PreConnectedList俊 乐绰 技记甸俊 措秦辑绰 OnConnect(TRUE)啊 龋免登绊,
			ConnectFailList俊 乐绰 技记俊 措秦辑绰 OnConnect(FALSE)啊 龋免等促.
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