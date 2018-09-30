#ifndef ZL_HTTPRESPONSE_H
#define ZL_HTTPRESPONSE_H
#include "Define.h"
#include "HttpProtocol.h"
#include "Location.h"

NAMESPACE_ZL_START

class HttpResponse
{
public:
	HttpResponse();
	virtual ~HttpResponse();

	const std::string GetHeader();

	HttpResponseCode GetStatusCode();
	void SetStatusCode(HttpResponseCode status);

	const std::string& GetHttpVersion();
	void SetHttpVersion(const std::string& httpver);

	const std::string& GetServerName();
	void SetServerName(const std::string& server);

	const std::string& GetConnectionType();
	void SetConnectionType(const std::string& connectionType);

	const std::string& GetContentType();
	void SetContentType(const std::string& contentType);

	const std::string& GetDocument();
	void SetDocument(const std::string& document);

	void SetLocation(const Location& location) { location_ = location; }
	const Location& GetLocation(){ return location_; }

	unsigned long GetContentLength();


	bool Compile();
protected:
	void ReadBoby();

protected:
	std::string header;
	Location location_;

	HttpResponseCode status;

	std::string version;
	std::string server;
	std::string connectionType;

	std::string contentType;

	std::string document;

	unsigned long contentLength;
};

NAMESPACE_ZL_END

# endif /* ZL_HTTPRESPONSE_H */