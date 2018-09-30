#include "PassiveSocket.h"

NAMESPACE_ZL_START

PassiveSocket::PassiveSocket(int port)
{
	if (!Socket::Create())
	{
		throw SocketException ("Could not create server socket.");
	}
	if (!Socket::Bind(port))
	{
		throw SocketException("Could not bind to port.");
	}
	if (!Socket::Listen(5))
	{
		throw SocketException("Could not listen to port.");
	}
}

PassiveSocket::~PassiveSocket()
{
}

bool PassiveSocket::Accept(PassiveSocket& new_socket)
{
	printf("ServerSocket::accept ( PassiveSocket& )\n");
	int addr_length = sizeof(sockaddr_);
	new_socket.sockfd_ = ::accept(sockfd_, (sockaddr *)&sockfd_, (socklen_t *)&addr_length);

	if (new_socket.sockfd_ <= 0)
		return false;
	else
		return true;
}

const PassiveSocket& PassiveSocket::operator << (const std::string& data) const
{
	if (!Socket::Send(data))
	{
		throw SocketException("Could not write to socket.");
	}

	return *this;
}

const PassiveSocket& PassiveSocket::operator >> (std::string& data) const
{
	if (!Socket::Recv(data))
	{
		throw SocketException("Could not read from socket.");
		//printf("Could not read from socket.");
	}

	return *this;
}

NAMESPACE_ZL_END