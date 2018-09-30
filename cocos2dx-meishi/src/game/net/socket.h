#pragma once


#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define SOCKET			int
#define closesocket		close
#define INVALID_SOCKET	(-1)
#else
#include <WinSock2.h>
#endif



namespace GameCom
{

	// socket opt
	int geterror(SOCKET sock);

	SOCKET udpsocket(const char *ipaddr, int port, int reuseaddr = 0);
	int udpsendto(SOCKET sock, const char *ipaddr, int port, const char *buf, int len, int flag);
	int udprecvfrom(SOCKET sock, char *ipaddr, unsigned int *port, char *buf, int len, int flag);

	SOCKET tcpsocket(const char *ipaddr, int port, int reuseaddr = 0);
	int tcpconnect(SOCKET sock, const char *ipaddr, int port);
	SOCKET tcpaccept(SOCKET sock, char *ipaddr, int *port);

	int setnonblocking(SOCKET sock, int nonblock = 1);
	int setreuseaddr(SOCKET sock, int opt);
	int setkeepalive(SOCKET sock, int keepalive = 1, int keepidle = 30, int keepinterval = 1, int keepcount = 5);

	int getpeeraddr(SOCKET sock, char* ip, int* port);
	int getlocaladdr(SOCKET sock, char* ip, int* port);


	class Socket
	{
	public:
		Socket();
		virtual ~Socket();
		static void sleep(unsigned long ms);

		virtual bool onCreate(const char *ipaddr = 0, int port = 0);
		virtual bool onConnect(const char * ipaddr, int port) = 0;
		virtual bool onShutdown(int nHow);
		virtual void onClose();
		virtual bool onAttach(SOCKET hSocket);
		virtual SOCKET onDetach();

		virtual int onReceive(void* lpBuf, int nBufLen) = 0;
		virtual int onForceReceive(void* lpBuf, int nBufLen) = 0;
		virtual int onSend(const void* lpBuf, int nBufLen) = 0;

	protected:
		SOCKET mSocket;
	};


	class AsyncSocket : public Socket
	{
	public:
		AsyncSocket();
		virtual ~AsyncSocket();

		virtual bool onCreate(const char *ipaddr = 0, int port = 0);
		virtual bool onConnect(const char * ipaddr, int port, unsigned long timeout_ms);
		virtual int onReceive(void* lpBuf, int nBufLen, unsigned long timeout_ms);
		virtual int onForceReceive(void* lpBuf, int nBufLen, unsigned long timeout_ms);
		virtual int onSend(const void* lpBuf, int nBufLen, unsigned long timeout_ms);

		virtual bool onConnect(const char * ipaddr, int port);
		virtual int onReceive(void* lpBuf, int nBufLen);
		virtual int onForceReceive(void* lpBuf, int nBufLen);
		virtual int onSend(const void* lpBuf, int nBufLen);
	};
}
