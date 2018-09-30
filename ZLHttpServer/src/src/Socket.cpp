#include "Socket.h"

NAMESPACE_ZL_START

bool zl_socket_startup()
{
#ifdef OS_WINDOWS
    WSADATA wsaData;
    int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (ret != 0)
    {
        return false;
    }
#endif
    return true;
}

bool zl_socket_cleanup()
{
#ifdef OS_WINDOWS
    WSACleanup();
#endif
    return true;
}


Socket::Socket() : sockfd_(ZL_INVALID_SOCKET)
{
	::memset(&sockaddr_, 0, sizeof(sockaddr_));
}

Socket::~Socket()
{
	if (IsValid())
	{
		ZL_CLOSE(sockfd_);
	}
}

bool Socket::Create()
{
	sockfd_ = ZL_CREATE_SOCKET(AF_INET, SOCK_STREAM, 0);

	if (!IsValid())
	{
		return false;
	}

	// TIME_WAIT - argh
	int on = 1;
	if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on)) == -1)
		return false;

	return true;
}

bool Socket::Bind(int port)
{
	if (!IsValid())
	{
		return false;
	}

	sockaddr_.sin_family = AF_INET;
	sockaddr_.sin_addr.s_addr = INADDR_ANY;
	sockaddr_.sin_port = htons(port);

	int bind_return = ZL_BIND(sockfd_, (struct sockaddr *) &sockaddr_, sizeof(sockaddr_));
	if (bind_return == -1)
	{
		return false;
	}

	return true;
}

bool Socket::Listen(int listenNum /*= 5*/) const
{
	if (!IsValid())
	{
		return false;
	}

	int listen_return = ZL_LISTEN(sockfd_, listenNum);
	if (listen_return == -1)
	{
		return false;
	}

	return true;
}

bool Socket::Accept(Socket& new_socket) const
{
	printf("Socket::accept ( Socket& new_socket )\n");
	int addr_length = sizeof(sockaddr_);
	new_socket.sockfd_ = ZL_ACCEPT(sockfd_, (sockaddr *)&sockaddr_, (socklen_t *)&addr_length);

	if (new_socket.sockfd_ <= 0)
		return false;
	else
		return true;
}

bool Socket::Send(const std::string& data) const
{
	int status = ZL_SEND(sockfd_, data.c_str(), data.size(), 0);
	if (status == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int Socket::Recv(std::string& data) const
{
	const static int MAX_RECV_SIZE = 1024;
	char buf[MAX_RECV_SIZE + 1];
	memset(buf, 0, MAX_RECV_SIZE + 1);

	int status = ZL_RECV(sockfd_, buf, MAX_RECV_SIZE, 0);
	if (status == -1)
	{
		printf("status == -1, errno == [%d], in Socket::recv\n", errno);
		return 0;
	}
	else if (status == 0)
	{
		return 0;
	}
	else
	{
		data = buf;
		return status;
	}
}

bool Socket::Connect(const std::string& host, int port)
{
	if (!IsValid())
	{
		return false;
	}

	sockaddr_.sin_family = AF_INET;
	sockaddr_.sin_port = htons(port);

	int status = inet_pton(AF_INET, host.c_str(), &sockaddr_.sin_addr);
	if (errno == EAFNOSUPPORT) 
		return false;

	status = ZL_CONNECT(sockfd_, (sockaddr *)&sockaddr_, sizeof(sockaddr_));

	return status == 0 ? true : false;
}

bool Socket::SetBlocking()
{
#if defined(OS_WINDOWS)
	unsigned long ul = 0;

	int ret = ioctlsocket(sockfd_, FIONBIO, (unsigned long *)&ul);//设置成非阻塞模式。  

	if (ret == SOCKET_ERROR)
		return false;

#elif defined(OS_LINUX)
	int flags = fcntl(sockfd_, F_GETFL);
	if (flags < 0)
		return false;

	flags &= (~O_NONBLOCK);
	if(fcntl(sockfd_, F_SETFL, flags)!=0)
		return false;
#endif

	return true;
}

bool Socket::SetNonBlocking()
{
#if defined(OS_WINDOWS)
	unsigned long ul = 1;

	int ret = ioctlsocket(sockfd_, FIONBIO, (unsigned long *)&ul);//设置成非阻塞模式。  

	if (ret == SOCKET_ERROR) 
		return false;

#elif defined(OS_LINUX)
	int flags = fcntl(sockfd_, F_GETFL);
	if (flags < 0)
		return false;
	
	flags |= O_NONBLOCK;
	if(fcntl(sockfd_, F_SETFL, flags)!=0)
		return false;
#endif
	
	return true;
}

NAMESPACE_ZL_END