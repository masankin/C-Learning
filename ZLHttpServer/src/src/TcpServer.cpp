#include "TcpServer.h"
#include <assert.h>
#include <thread>

NAMESPACE_ZL_START

TcpServer::TcpServer()
{
}

TcpServer::TcpServer(int port)
{
    int nWorkers = std::thread::hardware_concurrency() * 2 - 1;
    this->TcpServer::TcpServer(nWorkers, port);
}

TcpServer::TcpServer(int nWorkers, int _port)
{
    socket_ = 0;
    port_ = _port;
    totalIn_ = totalOut_ = 0;
}

TcpServer::~TcpServer()
{
	Cleanup();
	for (auto & worker : threads_group_)
	{
		worker.detach();
	}
	threads_group_.clear();
}

void TcpServer::PrintError(const char *msg)
{
    assert(msg != nullptr);
    printf("%s\n", msg);
}

int  TcpServer::Send(ZL_SOCKET socket, void *data, unsigned int length)
{
	int sent = ZL_SEND(socket, (const char *)data, length, 0);

    totalOut_ += sent;

    return sent;
}

int  TcpServer::Recv(ZL_SOCKET socket, void *data, unsigned int length, bool complete)
{
    assert(data != nullptr);
    assert(length > 0);

    int received = 0;
    if (complete)
    {
        while (received != length)
        {
			received += ZL_RECV(socket, (char *)data + received, length - received, 0);
        }
    }
    else
    {
		received = ZL_RECV(socket, (char *)data, length, 0);
    }

    totalIn_ += received;

    return received;
}

void TcpServer::Close(ZL_SOCKET socket)
{
	ZL_CLOSE(socket);
}

void TcpServer::GetIOStatus(unsigned long *in, unsigned long *out)
{
    if (in != nullptr)  
		*in = totalIn_;
    if (out != nullptr) 
		*out = totalOut_;
}

void TcpServer::Setopt(int level, int name, const char *value, int len)
{
    assert(value != nullptr);
    assert(len > 0);
	ZL_SETSOCKOPT(socket_, level, name, value, len);
}

bool TcpServer::Setup(const char *ip, const unsigned short port)
{
    zl_socket_startup();

	socket_ = ZL_CREATE_SOCKET(PF_INET, SOCK_STREAM, 0);
    if (socket == 0)
    {
        PrintError("socket error");
        return false;
    }

    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    //addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_.sin_port = htons(port_);
	int nIP = 0;
	if (!ip || '\0' == *ip || 0 == strcmp(ip, "0") 
		|| 0 == strcmp(ip, "0.0.0.0") || 0 == strcmp(ip, "*"))
	{
		nIP = htonl(INADDR_ANY);
	}
	else
	{
		nIP = inet_addr(ip);
	}
	addr_.sin_addr.s_addr = nIP;

    int optval = 1;
	ZL_SETSOCKOPT(socket_, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));

	if (ZL_BIND(socket_, (SOCKADDR *)&addr_, sizeof(addr_)) == -1)
    {
        PrintError("bind error");
        return false;
    }

    if (ZL_LISTEN(socket_, BACKLOG_SIZE) == -1)
    {
        PrintError("listen error");
        return false;
    }

    return true;
}

bool TcpServer::OnConnect(ClientData client)
{
	return true;
}

void TcpServer::Run(const char *ip, const unsigned short port)
{
    if (!Setup(ip, port))
    {
        PrintError("setup failed");
        return;
    }

    while (true)
    {
        SOCKADDR_IN clientAddr;    
		ZL_SOCKLEN clientAddrSize = sizeof(clientAddr);

        /* accept */
		ZL_SOCKET client = ZL_ACCEPT(socket_, (SOCKADDR *)&clientAddr, &clientAddrSize);

		if (client == 0)
		{
			PrintError("accept error");
		}
		else
		{
			char tmpbuf[1024];
			printf("accept one client : [%s:%d]\n",
				inet_ntop(AF_INET, &clientAddr.sin_addr, tmpbuf, 1024),
				ntohs(clientAddr.sin_port) );
		}

        ClientData data =	{ client, clientAddr };
		//auto &thd = std::thread([&](){ OnConnect(data); });
		threads_group_.push_back(std::thread([&](){ OnConnect(data); }));
    }
}

void TcpServer::Cleanup()
{
	Close(socket_);
	zl_socket_cleanup();
}

NAMESPACE_ZL_END