#ifndef ZL_HTTPSERVER_H
#define ZL_HTTPSERVER_H

#include <atomic>
#include "OsDefine.h"
#include "Define.h"
#include "TcpServer.h"
#include "HttpProtocol.h"
#include "HttpResponse.h"
#include "HttpRequest.h"

NAMESPACE_ZL_START


class HttpServer : public TcpServer
{
public:
	HttpServer();
	HttpServer(int port);
	HttpServer(const std::string& serverName, int port);
	virtual ~HttpServer();
public:
	void SetServerName(const std::string& name) { server_name_ = name; }
	const std::string& GetServerName() { return server_name_; }
	unsigned int GetRequestCount()
	{
		return requestCounter.load(std::memory_order_acquire);
	}
protected:
	virtual bool OnConnect(ClientData client);
	virtual int  SendString(ZL_SOCKET socket, const std::string& str);
	virtual bool SendResponse(ZL_SOCKET socket, HttpResponse& response);
protected:
	std::string server_name_;
	std::atomic<unsigned int> requestCounter;
private:
	HttpServer(const HttpServer&);
	HttpServer& operator = (const HttpServer&);
};

NAMESPACE_ZL_END

#endif  /* ZL_HTTPSERVER_H */