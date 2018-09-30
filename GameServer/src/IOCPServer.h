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
/// I/O �ڵ鷯 �ʱ�ȭ�� �ʿ��� ������ ��� ����ü
//-------------------------------------------------------------------------------------------------
typedef struct tagIOHANDLER_DESC
{
	DWORD								dwIoHandlerKey;					///< I/O ��ֵ
	DWORD								dwMaxAcceptSession;				///< ���������
	DWORD								dwMaxConnectSession;			///< ���������
	DWORD								dwSendBufferSize;				///< ���ͻ�������С
	DWORD								dwRecvBufferSize;				///< ���ջ�������С
	DWORD								dwTimeOut;						///< ��ʱ��С
	DWORD								dwMaxPacketSize;				///< ������ݰ���С
	DWORD								dwNumberOfIoThreads;			///< I/O �߳�����
	DWORD								dwNumberOfAcceptThreads;		///< Accept �߳�����
	DWORD								dwNumberOfConnectThreads;		///< Connect �߳�����
	fnCallBackCreateAcceptedObject		fnCreateAcceptedObject;			///< CreateAccepted		�����ص�����
	fnCallBackDestroyAcceptedObject		fnDestroyAcceptedObject;		///< DestroyAccepted	�����ص�����
	fnCallBackDestroyConnectedObject	fnDestroyConnectedObject;		///< DestroyConnected	�����ص�����
} IOHANDLER_DESC, *LPIOHANDLER_DESC;

//-------------------------------------------------------------------------------------------------
/// IOCP Server ��
/**
	@remarks
			- IOCP Server ӵ�е�I/O �������ĸ���.
			  (2 ����� I / O ����������Ҫʹ��LPIOHANDLER_DESC������г�ʼ��)
			  - I / O ������,ʹ����ͬ�Ķ˿ں�.
			- Update of the session when you call the receive buffer by checking all of the completed packet is processed.
	@�ο�
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
