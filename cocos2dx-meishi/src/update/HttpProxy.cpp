#include "HttpProxy.h"
#include "curl/curl.h"
#include "cocos2d.h"

#include "json/document.h"
#include "json/filestream.h"

USING_NS_CC;


static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)  
{  
    std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);  
    if( NULL == str || NULL == buffer )  
    {  
        return -1;  
    }  

    char* pData = (char*)buffer;  
    str->append(pData, size * nmemb);  
    return nmemb;  
} 

HttpProxy::HttpProxy()
{
    m_bIsCurlGlobalInited = false;
}

HttpProxy::~HttpProxy()
{
}

void HttpProxy::parseDirInfoRsp(const std::string& strRspJson) 
{
    rapidjson::Document jsonDoc;
    jsonDoc.Parse<0>(strRspJson.c_str());
    if(jsonDoc.HasParseError()) 
	{
        CCLOG("HttpProxy::parseSvrListRsp,json parse error [%s]", jsonDoc.GetParseError());
        return;
    }

    rapidjson::Value &jsonElmts = jsonDoc["zones"];
    if(!(jsonElmts.IsArray() && jsonElmts.Size() > 0)) {
        CCLOG("HttpProxy::parseSvrListRsp, invalid json format");
        return;
    }
    std::vector<SvrInfo> svrInfos;

    for(unsigned int idx = 0; idx < jsonElmts.Size(); idx++) {
        rapidjson::Value &jsonElmt = jsonElmts[idx];
        SvrInfo stSvrInfo;

        if(jsonElmt.HasMember("zoneid") && jsonElmt["zoneid"].IsString()) {
            stSvrInfo.zoneid = std::atoi(jsonElmt["zoneid"].GetString());
        }

        if(jsonElmt.HasMember("name") && jsonElmt["name"].IsString()) {
            stSvrInfo.name = jsonElmt["name"].GetString();
        }

        if(jsonElmt.HasMember("ip") && jsonElmt["ip"].IsString()) {
            stSvrInfo.ip = jsonElmt["ip"].GetString();
        }

        if(jsonElmt.HasMember("status") && jsonElmt["status"].IsString()) {
            stSvrInfo.status = jsonElmt["status"].GetString();
        }


        svrInfos.push_back(stSvrInfo);
    }
}

void HttpProxy::sendGetDirInfoRqst(const std::string &strURL) 
{
	//CCLOG("[trace]HttpProxy::sendGetSvrListRqst,get svr list url %s", strURL.c_str());
	//
	//std::string strRsp;
	//int iRet = HttpGet(strURL, (void*)&strRsp, false);
	//if (iRet != CURLM_OK)
	//{
	//    CCLOG("[trace]HttpProxy::sendGetSvrListRqst, fail to do curl,url %s, ret %d", strURL.c_str(), iRet);
	//    return;
	//}
	//parseDirInfoRsp(strRsp);
}

bool HttpProxy::bIsDoCurlOK(const int iRetCode) {
    return (iRetCode == CURLM_OK);
}

int HttpProxy::HttpGet(const std::string & strUrl, long& code, void* pResp, bool bProgress, HTTP_WRITE_FUNCTION cbWrite /*= NULL*/, void* target /*= NULL*/, HTTP_PROGRESS_FUNCTION cbProgress/*= NULL*/, bool bSetTimeOut /*=TRUE */)
{
    CURLcode ret;
    if(!m_bIsCurlGlobalInited) {
        curl_global_init(CURL_GLOBAL_ALL);   //global init just once,neilwu comment
        m_bIsCurlGlobalInited = true;
    }
    
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cbWrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, pResp);

	if (bProgress)
	{
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, !bProgress);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, cbProgress);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, target);
	}
	
	/**
	* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
	* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	*/
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	//curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
	if (bSetTimeOut)
	{
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10); //10s超时
	}

	ret = curl_easy_perform(curl);
	if (ret != CURLM_OK)
	{
		CCLOG("[warn]HttpProxy::HttpGet,Fail to fetch data from %s,curl ret %d", strUrl.c_str(), ret);
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
	curl_easy_cleanup(curl);
	return ret;
}

int HttpProxy::HttpPost(const std::string & strUrl, const std::string & strPost, std::string & strResponse)
{
    CURLcode res;
    if(!m_bIsCurlGlobalInited) {
        curl_global_init(CURL_GLOBAL_ALL);   //global init just once,neilwu comment
        m_bIsCurlGlobalInited = true;
    }
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
        return CURLE_FAILED_INIT;
    }

    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    //curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);//10s超时
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res;
}
