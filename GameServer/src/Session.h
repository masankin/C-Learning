#ifndef _SESSION_H_
#define _SESSION_H_

#include <windows.h>

#define SEND_POSTED			1
#define RECV_POSTED			2
#define ACCEPT_POSTED		3
#define DISCONNECT_POSTED	4

class SendBuffer;
class RecvBuffer;
class SessionPool;
class NetworkObject;
class Session;

/// OVERLAPPED 扩展结构
struct OVERLAPPEDEX : public OVERLAPPED
{
	DWORD		dwIoSize;
	DWORD		dwFlags;
	DWORD		dwOperationType;
	Session*	pSession;
};

//=============================================================================================================================
/// 会话类，该类带有一个基本的SOCKET处理函数
/**
	@remarks
			要做扩展请继承该类。
	@par
			- 继承这个Session类的派生类可以重写以下函数，进行功能更改.
			- OnAccept:		当客户端的连接被接受调用这个函数。
			- OnDisConnect:	当客户端断开连接的时候，调用这个函数。
			- OnRecv:		客户端收到包的时候，这个函数被调用.
			- OnConnect:	连接服务器的时候调用，返回的结果是成功或者失败。
			- OnUpdate:		引擎会每个更新时间内都会调用这个函数。
	@note
			- 注意添加 ws2_32.lib
			- 包含 winsock2.h

*/
//=============================================================================================================================
class Session
{
	friend unsigned __stdcall io_thread( LPVOID param );
	friend class SessionPool;

public:
	/// 如果超时 dwTimeOut 是0，不必发送消息，也不用挂起该连接。.
	Session( DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize, DWORD dwTimeOut );
	virtual ~Session();

	VOID					Init();
	VOID					Reuse( HANDLE hIOCP );
	bool					Send( BYTE *pMsg, DWORD dwSize );
	bool					SendEx( DWORD dwNumberOfMessages, BYTE **ppMsg, WORD *pwSize );
	BOOL					PreSend();
	BOOL					PreRecv();
	BOOL					PreAccept( SOCKET listenSocket );
	SOCKET					CreateSocket();
	BOOL					ProcessRecvdPacket( DWORD dwMaxPacketSize );
	VOID					BindNetworkObject( NetworkObject *pNetworkObject );
	VOID					UnbindNetworkObject();
	VOID					OnAccept();
	VOID					OnConnect( BOOL bSuccess );
	VOID					OnLogString( char *pszLog, ... );

	inline VOID				SetSocket( SOCKET socket ) { m_socket = socket; }
	inline VOID				SetSockAddr( SOCKADDR_IN& sockaddr ) { m_sockaddr = sockaddr; }
	inline VOID				CloseSocket() { closesocket( m_socket ); m_socket = INVALID_SOCKET; }
	inline NetworkObject*	GetNetworkObject() { return m_pNetworkObject; }
	inline SendBuffer*		GetSendBuffer() { return m_pSendBuffer; }				///< 焊郴扁 滚欺 器牢磐甫 府畔茄促.
	inline RecvBuffer*		GetRecvBuffer() { return m_pRecvBuffer; }				///< 罐扁 滚欺 器牢磐甫 府畔茄促.
	inline SOCKET			GetSocket()		{ return m_socket; }					///< 家南 勤甸阑 府畔茄促.
	inline SOCKADDR*		GetSockAddr()	{ return (SOCKADDR*)&m_sockaddr; }		///< 家南 林家狼 器牢磐甫 府畔茄促.
	inline char*			GetIP() { return inet_ntoa( m_sockaddr.sin_addr ); }	///< 技记俊 拱赴 家南狼 IP甫 胶飘傅栏肺 馆券

	inline DWORD			GetIdleTick() const	{ return m_dwTimeOut? m_dwLastSyncTick + m_dwTimeOut : 0; }
	inline BOOL				IsOnIdle()
		{ return m_dwTimeOut ? m_dwLastSyncTick + m_dwTimeOut < GetTickCount() : FALSE; }
	inline DWORD			GetIndex() { return m_dwIndex; }
	inline BOOL				IsAcceptSocket() { return m_bAcceptSocket; }
	inline VOID				SetAcceptSocketFlag() { m_bAcceptSocket = TRUE; }
	VOID					Remove() { InterlockedExchange( (long*)&m_bRemove, TRUE ); }
	inline VOID				ResetKillFlag() { InterlockedExchange( (long *)&m_bRemove, FALSE ); }
	inline BOOL				ShouldBeRemoved() { return m_bRemove; }
	VOID					Disconnect( BOOL bGracefulDisconnect );
	inline BOOL				HasDisconnectOrdered() { return m_bDisconnectOrdered; }

private:
	VOID					SetIndex( DWORD index ) { m_dwIndex = index; }
	inline VOID				ResetTimeOut() { m_dwLastSyncTick = GetTickCount(); }

	NetworkObject			*m_pNetworkObject;
	SendBuffer				*m_pSendBuffer;
	RecvBuffer				*m_pRecvBuffer;

	SOCKET					m_socket;
	SOCKADDR_IN				m_sockaddr;

	OVERLAPPEDEX			m_sendIoData;
	OVERLAPPEDEX			m_recvIoData;
	OVERLAPPEDEX			m_acceptIoData;
	OVERLAPPEDEX			m_disconnectIoData;
	DWORD					m_dwLastSyncTick;
	BOOL					m_bRemove;
	DWORD					m_dwTimeOut;
	DWORD					m_dwIndex;
	BOOL					m_bAcceptSocket;
	BOOL					m_bDisconnectOrdered;

	// 临时测试
	int						m_iCategory;
	int						m_iProtocol;
	int						m_iSize;
};

#endif
