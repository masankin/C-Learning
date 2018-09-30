#pragma once

#pragma comment( lib, "ws2_32.lib" )

#include <windows.h>
#include <map>

class NetworkObject;
class IoHandler;

typedef std::map<DWORD, IoHandler*>		IOHANDLER_MAP;
typedef std::pair<DWORD, IoHandler*>	IOHANDLER_MAP_PAIR;
typedef IOHANDLER_MAP::iterator			IOHANDLER_MAP_ITER;

typedef NetworkObject* (*fnCallBackCreateAcceptedObject)();
typedef VOID (*fnCallBackDestroyAcceptedObject)( NetworkObject *pNetworkObject );
typedef VOID (*fnCallBackDestroyConnectedObject)( NetworkObject *pNetworkObject );

//-------------------------------------------------------------------------------------------------
/// I/O 勤甸矾 檬扁拳俊 鞘夸茄 沥焊甫 淬绰 备炼眉
//-------------------------------------------------------------------------------------------------
typedef struct tagIOHANDLER_DESC
{
	DWORD								dwIoHandlerKey;					///< I/O 键值
	DWORD								dwMaxAcceptSession;				///< 最大请求数
	DWORD								dwMaxConnectSession;			///< 最大连接数
	DWORD								dwSendBufferSize;				///< 发送缓冲区大小
	DWORD								dwRecvBufferSize;				///< 接收缓冲区大小
	DWORD								dwTimeOut;						///< 延时大小
	DWORD								dwMaxPacketSize;				///< 最大数据包大小
	DWORD								dwNumberOfIoThreads;			///< I/O 线程数量
	DWORD								dwNumberOfAcceptThreads;		///< Accept 线程数量
	DWORD								dwNumberOfConnectThreads;		///< Connect 线程数量
	fnCallBackCreateAcceptedObject		fnCreateAcceptedObject;			///< CreateAccepted		创建回调函数
	fnCallBackDestroyAcceptedObject		fnDestroyAcceptedObject;		///< DestroyAccepted	创建回调函数
	fnCallBackDestroyConnectedObject	fnDestroyConnectedObject;		///< DestroyConnected	创建回调函数
} IOHANDLER_DESC, *LPIOHANDLER_DESC;

//-------------------------------------------------------------------------------------------------
/// IOCP Server 类
/**
	@remarks
			- IOCP Server 拥有的I/O 处理器的个数.
			  (2 或更多 I / O 处理器，需要使用LPIOHANDLER_DESC数组进行初始化)
			  - I / O 处理器,使用相同的端口号.
			- Update of the session when you call the receive buffer by checking all of the completed packet is processed.
	@参看
			IoHandler
			tagIOHANDLER_DESC
*/
//-------------------------------------------------------------------------------------------------
class IOCPServer
{
	friend unsigned __stdcall send_thread( LPVOID param );

public:
	IOCPServer();
	virtual ~IOCPServer();

	BOOL				Init( LPIOHANDLER_DESC lpDesc, DWORD dwNumberofIoHandlers );
	BOOL				StartListen( DWORD dwIoHandlerKey, char *pIP, WORD wPort );
	VOID				Update();
	VOID				Shutdown();
	DWORD				Connect( DWORD dwIoHandlerKey, NetworkObject *pNetworkObject, char *pszIP, WORD wPort );
	BOOL				IsListening( DWORD dwIoHandlerKey );
	DWORD				GetNumberOfConnections( DWORD dwIoHandlerKey );

private:
	BOOL				InitWinsock();
	VOID				CreateIoHandler( LPIOHANDLER_DESC lpDesc );

	HANDLE				m_hSendThread;
	BOOL				m_bShutdown;
	IOHANDLER_MAP		m_mapIoHandlers;
};
