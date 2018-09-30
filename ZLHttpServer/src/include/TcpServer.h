// ***********************************************************************
// Filename         : TcpServer.h
// Author           : LIZHENG
// Created          : 2014-07-04
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-07-04
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_TCPSERVER_H
#define ZL_TCPSERVER_H
#include "Define.h"
#include "Socket.h"
#include <thread>

#define BACKLOG_SIZE   5
#define INBUF_SIZE     64

NAMESPACE_ZL_START

struct ClientData
{
    ZL_SOCKET      socket_;
    ZL_SOCKADDR_IN addr_;
};

class TcpServer
{
public:
    TcpServer();
    TcpServer(int port);
    TcpServer(int nWorkers, int port);
    virtual ~TcpServer();        
    
public:
	virtual void Run(const char *ip, const unsigned short port);
	virtual void Setopt(int level, int name, const char *value, int len);
	virtual bool OnConnect(ClientData client);
	virtual bool Setup(const char *ip, const unsigned short port);
    virtual void Cleanup();
	virtual int  Send(ZL_SOCKET socket, void *data, unsigned int length);
	virtual int  Recv(ZL_SOCKET socket, void *data, unsigned int length, bool complete = true);
	virtual void Close(ZL_SOCKET socket);

	void GetIOStatus(unsigned long *in, unsigned long *out);
    virtual void PrintError(const char *msg);

protected:
    int              port_;
    ZL_SOCKET        socket_;
    ZL_SOCKADDR_IN   addr_;

	unsigned long    totalIn_;
	unsigned long    totalOut_;

	std::vector<std::thread>  threads_group_;
};

NAMESPACE_ZL_END

#endif /* ZL_TCPSERVER_H */