#include "HttpRequest.h"
#include <regex>
#include <sstream>
NAMESPACE_ZL_START

HttpRequest::HttpRequest()
{
}
HttpRequest::HttpRequest(const std::string& header)
{
    contentLength = 0;

    SetHeader(header);

    ParseHeader();
}
HttpRequest::HttpRequest(const std::string& header, const std::string& document)
{
    contentLength = 0;

    SetHeader(header);
    SetDocument(document);

    ParseHeader();
    ParseDocument();
}

HttpRequest::~HttpRequest()
{
}


bool HttpRequest::ParseHeader()
{
	//解析Http消息头的第一行，即请求行，Method Location HttpVer ： GET /index.html HTTP/1.1
	std::vector<std::string> token;
	auto start = header.begin();
	for (auto it = header.begin(); it != header.end(); ++it)
    {
		if (*it == ' ' || *it == '\r' || *it == '\0')
        {
			std::string tmp;
			copy(start, it, std::back_inserter(tmp));
            token.push_back( tmp );

            start = ++it;
        }
        if( *it == '\n' )
            break;
    }

	auto& method = token[0];
    std::transform(method.begin(), method.end(), method.begin(), ::tolower);
	if(strcmp(method.c_str(), "get") == 0)
       SetHttpMethod(HttpMethod::HttpGet);
	else if (strcmp(method.c_str(), "post") == 0)
        SetHttpMethod(HttpMethod::HttpPost);
	else if (strcmp(method.c_str(), "put") == 0)
        SetHttpMethod(HttpMethod::HttpPut);
	else if (strcmp(method.c_str(), "delete") == 0)
        SetHttpMethod(HttpMethod::HttpDelete);

	location.SetRequestURI(token[1]); //location，即是请求的文件路径
    SetHttpVersion(token[2]); //HttpVersion，Client发过来的http协议版本号

    /* 解析剩余的选项行 */
	std::regex expr("([a-zA-Z_-]*)\\s?:\\s?(.*)");
	std::smatch match;
	std::string request = header;
    while(std::regex_search(request, match, expr))
    {
		std::string key = match[1].str();
		std::string value = match[2].str();
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        if(!ParseOption(key, value))
        {
            /* unknown option */
        }
        request = match.suffix().str();
    }

    return true;
}
bool HttpRequest::ParseDocument()
{
    /* parse post form-data */
    if( GetHttpMethod() == HttpMethod::HttpPost )
    {
        printf("%s\n", GetContentType());
        printf("%s\n", document.c_str() );
    }
    else
    {
        return false;
    }
	return true;
}

bool HttpRequest::Parse()
{
    return true;
}
bool HttpRequest::ParseOption(const std::string& key, const std::string& value)
{
	std::string *dest = nullptr;

    if( key == "host" )
        SetHost( value );
    else if( key == "connection" )
        SetConnection( value );
    else if( key == "accept" )
        SetAccept( value );
    else if( key == "user-agent" )
        SetUserAgent( value );
    else if( key == "accept-encoding" )
        SetAcceptEncoding( value );
    else if( key == "accept-language" )
        SetAcceptLanguage( value );
    else if( key == "content-type" )
        SetContentType( value );
    else if( key == "content-length" )
    {
        unsigned long v;
        sscanf( value.c_str(), "%ul", &v);
        SetContentLength( v );
    }
    else
        return false;

    return true;
}

const std::string& HttpRequest::GetHeader()
{
    return header;
}

void HttpRequest::SetHeader(const std::string& _header)
{
    header = _header;
}

const std::string& HttpRequest::GetDocument()
{
    return document;
}

void HttpRequest::SetDocument(const std::string& _document)
{
    document = _document;
}

HttpMethod HttpRequest::GetHttpMethod()
{
    return method;
}

void HttpRequest::SetHttpMethod(HttpMethod _method)
{
    method = _method;
}

const std::string& HttpRequest::GetHttpVersion()
{
    return version;
}

void HttpRequest::SetHttpVersion(const std::string& httpver)
{
    version = httpver;
}

const std::string& HttpRequest::GetHost()
{
    return host;
}

void HttpRequest::SetHost(const std::string& _host)
{
    host = _host;
}

const std::string& HttpRequest::GetConnection()
{
    return connection;
}

void HttpRequest::SetConnection(const std::string& _connection)
{
    connection = _connection;
}

const std::string& HttpRequest::GetAccept()
{
    return accept;
}

void HttpRequest::SetAccept(const std::string& _accept)
{
    accept = _accept;
}

const std::string& HttpRequest::GetUserAgent()
{
    return userAgent;
}

void HttpRequest::SetUserAgent(const std::string& _userAgent)
{
    userAgent = _userAgent;
}

const std::string& HttpRequest::GetAcceptEncoding()
{
    return acceptEncoding;
}

void HttpRequest::SetAcceptEncoding(const std::string& _acceptEncoding)
{
    acceptEncoding = _acceptEncoding;
}

const std::string& HttpRequest::GetAcceptLanguage()
{
    return acceptLanguage;
}

void HttpRequest::SetAcceptLanguage(const std::string& _acceptLanguage)
{
    acceptLanguage = _acceptLanguage;
}

void HttpRequest::SetContentLength(unsigned long _contentLength)
{
    contentLength = _contentLength;
}

unsigned long HttpRequest::GetContentLength()
{
    return document.length();
}

const std::string& HttpRequest::GetContentType()
{
    return contentType;
}
void HttpRequest::SetContentType(const std::string& _contentType)
{
    contentType = _contentType;
}

NAMESPACE_ZL_END