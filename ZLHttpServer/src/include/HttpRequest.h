#ifndef ZL_HTTPREQUEST_H
#define ZL_HTTPREQUEST_H
#include "Define.h"
#include "HttpProtocol.h"
#include "Location.h"

NAMESPACE_ZL_START

class HttpRequest
{
public:
    HttpRequest();
    HttpRequest(const std::string& header);
    HttpRequest(const std::string& header, const std::string& document);
    virtual ~HttpRequest();

    const std::string& GetHeader();
    const std::string& GetDocument();

    HttpMethod GetHttpMethod();
    void SetHttpMethod(HttpMethod method);

	const Location& GetLocation(){ return location; }

    const std::string& GetHttpVersion();
    void SetHttpVersion(const std::string& httpver);

    const std::string& GetHost();
    void SetHost(const std::string& host);

    const std::string& GetConnection();
    void SetConnection(const std::string& connection);

    const std::string& GetAccept();
    void SetAccept(const std::string& accept);

    const std::string& GetUserAgent();
    void SetUserAgent(const std::string& userAgent);

    const std::string& GetAcceptEncoding();
    void SetAcceptEncoding(const std::string& acceptEncoding);

    const std::string& GetAcceptLanguage();
    void SetAcceptLanguage(const std::string& acceptLanguage);

    void SetContentLength(unsigned long _contentLength);
    unsigned long GetContentLength();

    void SetContentType(const std::string& _contentType);
    const std::string& GetContentType();
protected:
    void SetHeader(const std::string& header);
    void SetDocument(const std::string& document);

    bool Parse();
    bool ParseOption(const std::string& key, const std::string& value);
    bool ParseHeader();
    bool ParseDocument();
protected:
    std::string header;
    std::string document;
    std::string contentType;
    int contentLength;
    HttpMethod method;
	Location location;

    std::string version;

    std::string host;
    std::string connection;
    std::string accept;
    std::string userAgent;
    std::string acceptEncoding;
    std::string acceptLanguage;

};

NAMESPACE_ZL_END

#endif /* ZL_HTTPREQUEST_H */