#include "socket.h"
#include <stdio.h>

#ifdef WIN32
#include <winsock2.h>    
#include <mstcpip.h> 
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#endif

#include "cocos2d.h"
USING_NS_CC;

namespace GameCom
{

	SOCKET udpsocket(const char *ipaddr, int port, int reuseaddr /* = 0 */)
	{
		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock < 0)
		{
			//fprintf(stderr,"socket() error!\n");
			return INVALID_SOCKET;
		}

		if (reuseaddr)
		{
			int opt = 1;
			setreuseaddr(sock, opt);
		}

		struct sockaddr_in addr;
		int len = sizeof(addr);
		memset(&addr, 0, len);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		if (ipaddr)
		{
			addr.sin_addr.s_addr = inet_addr(ipaddr);
		}
		else
		{
			addr.sin_addr.s_addr = htonl(0);
			//addr.sin_addr.s_addr = INADDR_ANY;
		}

		int status = bind(sock, (struct sockaddr*)&addr, len);
		if (status < 0)
		{
			//fprintf(stderr, "bind address err!\n");
			closesocket(sock);
			return INVALID_SOCKET;
		}

		return sock;
	}

	int udpsendto(SOCKET sock, const char *ipaddr, int port, const char *buf, int len, int flag)
	{
		struct sockaddr_in remote_sin;

		memset(&remote_sin, 0, sizeof(remote_sin));
		remote_sin.sin_family = AF_INET;
		remote_sin.sin_port = htons((short)port);
		if (ipaddr)
		{
			remote_sin.sin_addr.s_addr = inet_addr(ipaddr);
		}
		else
		{
			remote_sin.sin_addr.s_addr = htonl(0);
		}

		return sendto(sock, buf, len, flag, (struct sockaddr*)&remote_sin, sizeof(remote_sin));
	}

	int udprecvfrom(SOCKET sock, char *ipaddr, unsigned int *port, char *buf, int len, int flag)
	{
		struct sockaddr_in remote_sin;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		unsigned int addlen = sizeof(remote_sin);
#else
		int addlen = sizeof(remote_sin);
#endif
		memset(&remote_sin, 0, addlen);

		int ret = recvfrom(sock, buf, len, flag, (struct sockaddr*)&remote_sin, &addlen);
		if (port)
		{
			*port = ntohs(remote_sin.sin_port);
		}
		if (ipaddr)
		{
			strcpy(ipaddr, inet_ntoa(remote_sin.sin_addr));
		}

		return ret;
	}

	SOCKET tcpsocket(const char *ipaddr, int port, int reuseaddr /* = 0 */)
	{
		SOCKET sock;
		struct sockaddr_in local_sin;

		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0)
		{
			//fprintf(stderr,"socket() error!\n");
			return INVALID_SOCKET;
		}

		if (reuseaddr)
		{
			int opt = 1;
			setreuseaddr(sock, opt);
		}

		memset(&local_sin, 0, sizeof(local_sin));
		local_sin.sin_family = AF_INET;
		local_sin.sin_port = htons((short)port);
		if (ipaddr)
		{
			local_sin.sin_addr.s_addr = inet_addr(ipaddr);
		}
		else
		{
			local_sin.sin_addr.s_addr = htonl(0);
		}

		int ret = bind(sock, (struct sockaddr *)&local_sin, sizeof(local_sin));
		if (ret < 0)
		{
			//fprintf(stderr, "bind address err! ret = %d \n", ret);
			closesocket(sock);
			return INVALID_SOCKET;
		}

		return sock;
	}

	int tcpconnect(SOCKET sock, const char *ipaddr, int port)
	{
		if (sock == INVALID_SOCKET)
		{
			return -1;
		}

		struct addrinfo *result, hints;
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		char port_str[16];
		sprintf(port_str, "%d", port);
		int ret = getaddrinfo(ipaddr, port_str, &hints, &result);
		if (ret != 0) {

			//fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
			return INVALID_SOCKET;
		}

		ret = connect(sock, result->ai_addr, result->ai_addrlen);
		freeaddrinfo(result);

		if (ret < 0)
		{
#ifdef WIN32
			CCLOG("[error]tcpconnect socket connect failed, errno:%d!", GetLastError());
#endif
		}
		return ret;
	}

	SOCKET tcpaccept(SOCKET sock, char *ipaddr, int *port)
	{
		if (sock == INVALID_SOCKET)
		{
			return INVALID_SOCKET;
		}
		int newsock;
		struct sockaddr_in remote_sin;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		unsigned int len = sizeof(remote_sin);
#else
		int len = sizeof(remote_sin);
#endif

		memset(&remote_sin, 0, len);
		newsock = accept(sock, (struct sockaddr*)&remote_sin, &len);
		if (newsock < 0)
		{
			return INVALID_SOCKET;
		}
		if (port)
		{
			*port = ntohs(remote_sin.sin_port);
		}
		if (ipaddr)
		{
			strcpy(ipaddr, inet_ntoa(remote_sin.sin_addr));
		}

		return newsock;
	}

	int setnonblocking(SOCKET sock, int nonblock)
	{
		if (sock == INVALID_SOCKET)
		{
			return -1;
		}

#ifdef WIN32
		unsigned long opt = nonblock;
		if (SOCKET_ERROR == ioctlsocket(sock, FIONBIO, &opt))
		{
			return -1;
		}
		else
		{
			return 0;
		}
#else
		int opts = fcntl(sock, F_GETFL);
		if (opts < 0)
		{
			return -1;
		}

		if (nonblock)
		{
			opts |= O_NONBLOCK;
		}
		else
		{
			opts &= ~O_NONBLOCK;
		}

		if (fcntl(sock, F_SETFL, opts) < 0)
		{
			return -1;
		}
		else
		{
			return 0;
		}
#endif
	}

	int setreuseaddr(SOCKET sock, int opt)
	{
		if (sock == INVALID_SOCKET)
		{
			return -1;
		}
		return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	}

	int getpeeraddr(SOCKET sock, char* ip, int* port)
	{
		if (sock == INVALID_SOCKET)
		{
			return -1;
		}

		struct sockaddr addr;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		unsigned int addrlen = sizeof(addr);
#else
		int addrlen = sizeof(addr);
#endif
		int ret = getpeername(sock, &addr, &addrlen);
		if (ret < 0)
		{
			return ret;
		}

		unsigned char *pp = (unsigned char*)(addr.sa_data);
		if (port)
		{
			*port = ((unsigned int)pp[0] << 8) | (pp[1]);
		}
		if (ip)
		{
			sprintf(ip, "%d.%d.%d.%d", pp[2], pp[3], pp[4], pp[5]);
		}

		return 0;
	}

	int getlocaladdr(SOCKET sock, char* ip, int* port)
	{
		if (sock == INVALID_SOCKET)
		{
			return -1;
		}

		struct sockaddr addr;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		unsigned int addrlen = sizeof(addr);
#else
		int addrlen = sizeof(addr);
#endif
		int ret = getsockname(sock, &addr, &addrlen);
		if (ret < 0)
		{
			return ret;
		}

		unsigned char *pp = (unsigned char*)(addr.sa_data);
		if (port)
		{
			*port = ((unsigned int)pp[0] << 8) | (pp[1]);
		}
		if (ip)
		{
			sprintf(ip, "%d.%d.%d.%d", pp[2], pp[3], pp[4], pp[5]);
		}

		return 0;
	}

	int geterror(SOCKET sock)
	{
		int so_error = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		unsigned int len = sizeof(so_error);
#else
		int len = sizeof(so_error);
#endif
		getsockopt(sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&so_error), &len);
		return so_error;
	}

	int setkeepalive(SOCKET sock, int keepalive /*= 1*/, int keepidle /*= 30*/, int keepinterval /*= 1*/, int keepcount /*= 5*/)
	{
		if (sock == INVALID_SOCKET)
		{
			return -1;
		}

		if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (const char*)&keepalive, sizeof(keepalive)) != 0)
		{
			return -1;
		}

#ifdef WIN32
		struct tcp_keepalive kavars = {
			keepalive,
			keepidle * 1000,
			keepinterval * 1000
		};
		DWORD dwReturn(0);
		DWORD dwTransBytes(0);
		if (WSAIoctl(sock, SIO_KEEPALIVE_VALS, &kavars, sizeof(kavars), &dwReturn, sizeof(dwReturn), &dwTransBytes, NULL, NULL) == SOCKET_ERROR)
		{
			return -1;
		}
#else
		// 		if(setsockopt(sock, IPPROTO_TCP, TCP_KEEPALIVE, (const char*)&keepidle, sizeof(keepidle)) != 0)
		// 		{
		// 			return -1;
		// 		}
		// 		if(setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, (const char *)&keepinterval, sizeof(keepinterval)) != 0)
		// 		{
		// 			return -1;
		// 		}
		// 		if(setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, (const char *)&keepcount, sizeof(keepcount)) != 0)
		// 		{
		// 			return -1;
		// 		}

#endif
		return 0;
	}



	Socket::Socket()
	{
		mSocket = INVALID_SOCKET;

#ifndef WIN32
		signal(SIGPIPE, SIG_IGN);
#endif//WINDOWS	
	}

	Socket::~Socket()
	{
		onClose();
	}

	bool Socket::onCreate(const char *ipaddr /* = 0 */, int port /* = 0 */)
	{
		if (mSocket != INVALID_SOCKET)
		{
			//fprintf(stderr,"Socket::Create::(mSocket != INVALID_SOCKET)\n");
			return false;
		}

		mSocket = tcpsocket(ipaddr, port, 1);
		return (mSocket != INVALID_SOCKET);
	}

	bool Socket::onAttach(SOCKET hSocket)
	{
		if (hSocket == INVALID_SOCKET)
		{
			//fprintf(stderr,"Socket::Attach::(hSocket == INVALID_SOCKET)\n");
			return false;
		}

		if (mSocket != INVALID_SOCKET)
		{
			//fprintf(stderr,"Socket::Attach::(mSocket != INVALID_SOCKET)\n");
		}

		mSocket = hSocket;
		return true;
	}

	SOCKET Socket::onDetach()
	{
		SOCKET tmp = mSocket;
		mSocket = INVALID_SOCKET;

		return tmp;
	}

	bool Socket::onShutdown(int nHow)
	{
		if (mSocket == INVALID_SOCKET)
		{
			//fprintf(stderr,"Socket::ShutDown::(mSocket == INVALID_SOCKET)\n");
			return false;
		}

		if (shutdown(mSocket, nHow) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void Socket::onClose()
	{
		if (mSocket == INVALID_SOCKET)
		{
			return;
		}

		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}

	void Socket::sleep(unsigned long ms)
	{
		struct timeval tv;
		tv.tv_sec = ms / 1000;
		tv.tv_usec = (ms % 1000) * 1000;
		select(0, 0, 0, 0, &tv);
	}


	//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
	// AsyncSocket

	const int ASYNC_SOCKET_TIMEOUT_MS = 3000;
	AsyncSocket::AsyncSocket()
		: Socket()
	{
	}

	AsyncSocket::~AsyncSocket()
	{
		onClose();
	}

	bool AsyncSocket::onCreate(const char *ipaddr /* = 0 */, int port /* = 0 */)
	{
		bool ret = Socket::onCreate(ipaddr, port);
		if (ret)
		{
			if (setnonblocking(mSocket, 1) < 0)
			{
				ret = false;
				onClose();
			}
		}
		return ret;
	}

	bool AsyncSocket::onConnect(const char * ipaddr, int port, unsigned long timeout_ms)
	{
		if (mSocket == INVALID_SOCKET)
		{
			//fprintf(stderr,"AsyncSocket::connect::(mSocket == INVALID_SOCKET)\n");
			return false;
		}

		bool ret = false;
		tcpconnect(mSocket, ipaddr, port);
		do
		{
			fd_set fdset;
			FD_ZERO(&fdset);
			FD_SET(mSocket, &fdset);

			struct timeval tv;
			tv.tv_sec = timeout_ms / 1000;
			tv.tv_usec = (timeout_ms % 1000) * 1000;

			int retvalue = 0;
			if ((retvalue = select(mSocket + 1, 0, &fdset, 0, &tv)) != 1)
			{
				//fprintf(stderr,"AsyncSocket::connect(%s:%d) timeout, ret(%d).\n", ipaddr, port, retvalue);
				break;
			}

			int errorno = geterror(mSocket);
			if (errorno != 0)
			{
				//fprintf(stderr,"AsyncSocket::connect(%s:%d) error = %d.\n", ipaddr, port, errorno);
				break;
			}
			ret = true;
		} while (0);

		return ret;
	}

	int AsyncSocket::onReceive(void* lpBuf, int nBufLen, unsigned long timeout_ms)
	{
		if (lpBuf == NULL)
		{
			//fprintf(stderr,"AsyncSocket::Receive::(lpBuf == NULL)\n");
			return -1;
		}

		if (mSocket == INVALID_SOCKET)
		{
			//fprintf(stderr,"AsyncSocket::Receive::(mSocket == INVALID_SOCKET)\n");
			return -1;
		}

		struct timeval tv;
		tv.tv_sec = timeout_ms / 1000;
		tv.tv_usec = (timeout_ms % 1000) * 1000;

		fd_set fdread, fderror;
		FD_ZERO(&fdread);
		FD_SET(mSocket, &fdread);
		FD_ZERO(&fderror);
		FD_SET(mSocket, &fderror);

		select(mSocket + 1, &fdread, 0, &fderror, &tv);

		if (FD_ISSET(mSocket, &fderror))
		{
			return -1;
		}

		int len = -2;
		if (FD_ISSET(mSocket, &fdread))
		{
			len = recv(mSocket, static_cast<char*>(lpBuf), nBufLen, 0);
		}
		/*
		len:0标识socket关闭
		len:-1标识recv错误
		len:-2标识没有可读数据
		len>0标识接收数据成功
		*/
		return len;
	}

	int AsyncSocket::onForceReceive(void* lpBuf, int nBufLen, unsigned long timeout_ms)
	{
		setnonblocking(mSocket, 0);
		int ret = onReceive(lpBuf, nBufLen, timeout_ms);
		setnonblocking(mSocket, 1);

		return ret;
	}

	int AsyncSocket::onSend(const void* lpBuf, int nBufLen, unsigned long timeout_ms)
	{
		if (lpBuf == NULL)
		{
			//fprintf(stderr,"AsyncSocket::Send::(lpBuf == NULL)\n");
			return -1;
		}

		if (mSocket == INVALID_SOCKET)
		{
			//fprintf(stderr,"AsyncSocket::Send::(mSocket == INVALID_SOCKET)\n");
			return -1;
		}

		struct timeval tv;
		tv.tv_sec = timeout_ms / 1000;
		tv.tv_usec = (timeout_ms % 1000) * 1000;

		fd_set fdwrite;
		FD_ZERO(&fdwrite);
		FD_SET(mSocket, &fdwrite);

		//handle exception
		fd_set fdexcept;
		FD_ZERO(&fdexcept);
		FD_SET(mSocket, &fdexcept);

		select(mSocket + 1, 0, &fdwrite, &fdexcept, &tv);

		int len = 0;
		if (FD_ISSET(mSocket, &fdwrite))
		{
			len = send(mSocket, (char*)(lpBuf), nBufLen, 0);
			if (len != nBufLen)
			{
				return -1;
			}
		}
		else if (FD_ISSET(mSocket, &fdexcept))
		{
			//socket has error! need reconnect
			return -2;
		}


		return len;
	}

	bool AsyncSocket::onConnect(const char * ipaddr, int port)
	{
		return AsyncSocket::onConnect(ipaddr, port, ASYNC_SOCKET_TIMEOUT_MS);
	}

	int AsyncSocket::onReceive(void* lpBuf, int nBufLen)
	{
		return AsyncSocket::onReceive(lpBuf, nBufLen, ASYNC_SOCKET_TIMEOUT_MS);
	}

	int AsyncSocket::onForceReceive(void* lpBuf, int nBufLen)
	{
		return AsyncSocket::onForceReceive(lpBuf, nBufLen, ASYNC_SOCKET_TIMEOUT_MS);
	}

	int AsyncSocket::onSend(const void* lpBuf, int nBufLen)
	{
		return AsyncSocket::onSend(lpBuf, nBufLen, ASYNC_SOCKET_TIMEOUT_MS);
	}
}
