#ifndef HttpProxy_h__
#define HttpProxy_h__

#include "core/Singleton.h"
#include <string>

struct SvrInfo
{
	std::string zoneid;
	std::string name;
	std::string ip;

	std::string status;
};

// 文件io回调
typedef size_t(*HTTP_WRITE_FUNCTION)(char* buffer, size_t size, size_t nmemb, void* lpVoid);
typedef int(*HTTP_PROGRESS_FUNCTION)(void *clientp, double totalToDownload, double nowDownloaded, double totalToUpLoad, double nowUpLoaded);

class HttpProxy
{
public:
	HttpProxy();
	~HttpProxy();
    
	void sendGetDirInfoRqst(const std::string &strURL);

    //source code: http://blog.csdn.net/huyiyang2010/article/details/7664201
    /** 
    * @brief HTTP GET请求 
    * @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com 
    * @param pResp 输出地址，内存或文件
	* @param bProgress 是否定制进度
	* @param cbWrite 写回调
	* @param target 进度定制对象
	* @param target 进度回调
	* @param bSetTimeOut 是否设置超时
    * @return 返回是否Post成功 
    */  
	int HttpGet(const std::string & strUrl, long& code, void* pResp, bool bProgress, HTTP_WRITE_FUNCTION cbWrite = NULL, void* target = NULL, HTTP_PROGRESS_FUNCTION cbProgress = NULL, bool bSetTimeOut = true);



    /** 
    * @brief HTTP POST请求 
    * @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com 
    * @param strPost 输入参数,使用如下格式para1=val1=val2&… 
    * @param strResponse 输出参数,返回的内容 
    * @return 返回是否Post成功 
    */  
    int HttpPost(const std::string & strUrl, const std::string & strPost, std::string & strResponse);
    bool bIsDoCurlOK(const int iRetCode);
private:
    void parseDirInfoRsp(const std::string& strRspJson);
    bool m_bIsCurlGlobalInited;
};

#define P_HTTP_PROXY Singleton<HttpProxy>::Get()

#endif // HttpProxy_h__
