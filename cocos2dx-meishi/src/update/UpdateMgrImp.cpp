#include "UpdateMgrImp.h"

#include "CCFileUtils.h"
#include "platform/CCFileUtils.h"
#include "cocostudio/DictionaryHelper.h"
#include "game/utils/Comm.h"
#include "HttpProxy.h"


using namespace std;
using namespace cocos2d;
using namespace cocostudio;

static bool s_cdnup = true;
static std::string s_versionUrl;
static const string LOCAL_VERSION_FILE = "local_version.json";

UpdateMgrImp* UpdateMgrImp::s_updatMgrImp = nullptr;

UpdateMgrImp::UpdateMgrImp()
{
	m_isShowUpdateScene = false;
	m_writableFolder = FileUtils::getInstance()->getWritablePath();
	//m_bCanUpdateSearchPath = false;
}

//UpdateMgrImp::~UpdateMgrImp()
//{
//
//}

UpdateMgrImp* UpdateMgrImp::getInstance()
{
	if (nullptr == s_updatMgrImp)
	{
		s_updatMgrImp = new UpdateMgrImp;
	}
	return s_updatMgrImp;
}
 
std::string UpdateMgrImp::GetExtractPath() 
{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) 
//	//win下m_writableFolder最后带着 反斜杠
//	return m_writableFolder + "data";
//#else  
//	CCLOG("[trace]UpdateMgrImp::GetExtractPath m_writableFolder:%s.", m_writableFolder.c_str());
//	return m_writableFolder + "/data";
//#endif
	//return "";
	CCLOG("[trace]UpdateMgrImp::GetExtractPath m_writableFolder:%s.", m_writableFolder.c_str());
	return m_writableFolder + "data";

}

bool UpdateMgrImp::UpdateVersion(std::string version)
{
	//更新完成，写入本地版本号
	std::string strLocalVer = m_writableFolder + LOCAL_VERSION_FILE;

	//int iVer = 0;
	FILE * fs = fopen(strLocalVer.c_str(), "wb");
	if (NULL == fs) 
	{
		CCLOG("[error]UpdateMgrImp::UpdateVersion,fail to open file %s for write.", strLocalVer.c_str());
		return false;
	}

	char strBuf[100] = { 0 };
	snprintf(strBuf, 100, "{\"ver\":\"%s\"}", version.c_str());
	fputs(strBuf, fs);
	fclose(fs);

	CCLOG("[trace]UpdateMgrImp::UpdateVersion,update local version to %s", version.c_str());

	/*//检查，如果这个版本高比apk中资源版本，则需要更新搜索路径
	std::string fullPath = FileUtils::getInstance()->fullPathForFilename("res_version.json");
	const std::string sApkResVer = getJsonVer(fullPath.c_str());
	CCLOG("[trace]UpdateMgrImp::UpdateVersion,update local version to %s,res version to %s", version.c_str(), sApkResVer.c_str());
	
	
	if (isUpdate(sApkResVer,version))
	{
		m_bCanUpdateSearchPath = true;

		//如果apk中资源版本比这个版本高
		//删除下载目录
		delResDataDir();
	}else
		m_bCanUpdateSearchPath = false;*/

	return true;
}

std::string UpdateMgrImp::getJsonVer(const char* strFilePath) 
{
	Data data = FileUtils::getInstance()->getDataFromFile(strFilePath);
	unsigned char* pBytes = data.getBytes();
	size_t size = data.getSize();

	std::string load_str = std::string((const char*)pBytes, size);
	rapidjson::Document json;
	json.Parse<0>(load_str.c_str());

	std::string ver = "0";
	if (json.HasParseError()) {
		CCLOG("[error]json file %s,parse error %s", strFilePath, json.GetParseError());
	}
	else {
		ver = DICTOOL->getStringValue_json(json, "ver"); //默认值为0
	}
	return ver;
}


void UpdateMgrImp::updateSearchPath()
{
// 	std::string path = GetExtractPath();
// 	if (m_bCanUpdateSearchPath) {
// 		CCLOG("[trace]UpdateMgrImp::updateSearchPath,add search path:%s", path.c_str());
// 		//sgResourceManager()->initUpdateResourcePath(path);
// 	}
// 	else {
// 		CCLOG("[trace]UpdateMgrImp::updateSearchPath,del search path:%s", path.c_str());
// 
// 		std::vector<std::string> searchPaths = FileUtils::getInstance()->getSearchPaths();
// 		std::vector<std::string>::iterator it = find(searchPaths.begin(), searchPaths.end(), path);
// 		if (it != searchPaths.end()) 
// 			searchPaths.erase(it);
// 	}
}

bool UpdateMgrImp::IsApkNewest()
{
	//1. 判断LOCAL_VERSION_FILE文件是否存在
	std::string strLocalVer = m_writableFolder + LOCAL_VERSION_FILE;

	//如果存在，则同assets里的比较
	FILE * fs = fopen(strLocalVer.c_str(), "r");
	if (!fs) {
		return false;
	}
	fclose(fs);

	std::string localVer = getJsonVer(strLocalVer.c_str());

	std::string fullPath = FileUtils::getInstance()->fullPathForFilename("res_version.json");

	std::string apkResVer = getJsonVer(fullPath.c_str());
	if (isUpdate(apkResVer,localVer))
	{
		CCLOG("[trace]UpdateMgrImp::IsApkNewest, apk ver [%s] is newest,local ver [%s]", apkResVer.c_str(), localVer.c_str());
		return true;
	}
	return false;
}

void UpdateMgrImp::delResDataDir() 
{
	CCLOG("[trace]UpdateMgrImp::delResDataDir");
	UpdateMgr::delResDataDir();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) 

#else  
	std::string strLocalVer = m_writableFolder + LOCAL_VERSION_FILE;
	remove(strLocalVer.c_str());
#endif
}


std::string UpdateMgrImp::GetCurrentVersion() 
{
	//返回当前所使用版本的版本号

	//1. 判断LOCAL_VERSION_FILE文件是否存在
	std::string strLocalVer = m_writableFolder + LOCAL_VERSION_FILE;

	//如果存在，则返回此中信息
	std::string ver = "0";
	FILE * fs = fopen(strLocalVer.c_str(), "r");
	if (fs) {
		fclose(fs);
		ver = getJsonVer(strLocalVer.c_str());
	}
	else {
		//如果不存在，返回apk里的版本信息
		std::string fullPath = FileUtils::getInstance()->fullPathForFilename("res_version.json");
		ver = getJsonVer(fullPath.c_str());
	}
	CCLOG("[trace]current version = %s", ver.c_str());
	return ver;
}


void UpdateMgrImp::setResUrl(const char* pszUrl) 
{
	if (pszUrl) 
	{
		CCLOG("[trace]UpdateMgrImp::setResUrl,set res url [%s]", pszUrl);
	
		m_strResUrl = pszUrl;
	}
}

std::string UpdateMgrImp::GetPkgUrl(const char * pkgName) {
	return (m_strResUrl)+std::string(pkgName);
}

std::string UpdateMgrImp::GetTempDownloadPath(const char * pkgName) {
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)  
//	return m_writableFolder + pkgName;
//#else  
//	return m_writableFolder + "/" + pkgName;
//#endif
	return m_writableFolder + pkgName;
}

void UpdateMgrImp::startUpdate()
{
	CCLOG("UpdateResScreen,WritablePath=%s", FileUtils::getInstance()->getWritablePath().c_str());
//	updateSearchPath(); //提前更新一下搜索路径
	UpdateMgr::startUpdate();
}

bool UpdateMgrImp::initUpdate()
{
	std::string strFilePath = "update.json";
	Data data = FileUtils::getInstance()->getDataFromFile(strFilePath);
	unsigned char* pBytes = data.getBytes();
	size_t size = data.getSize();

	std::string load_str = std::string((const char*)pBytes, size);
	rapidjson::Document json;
	json.Parse<0>(load_str.c_str());

	int ver = 0;
	if (json.HasParseError()) 
	{
		CCLOG("[error]json file %s,parse error %s", strFilePath.c_str(), json.GetParseError());
		m_status.errorCode = ERROR_JSON_PARSE_FAIL;
		return false;
	}
	else 
	{
		const rapidjson::Value& subVal = DICTOOL->getSubDictionary_json(json, "message");
		const rapidjson::Value& urlSubVal = DICTOOL->getSubDictionary_json(json, "config");

		s_versionUrl = DICTOOL->getStringValue_json(urlSubVal, "url");
		s_cdnup = DICTOOL->getBooleanValue_json(urlSubVal, "cdn");

		int nLimit = DICTOOL->getIntValue_json(urlSubVal, "limitprompt");
		setPromptLimit(nLimit);
#ifdef MGAME_PLATFORM_MPLUS
		m_resUrl = DICTOOL->getStringValue_json(urlSubVal, "resUrl");
#endif		
		CCLOG("[trace]load json file %s", strFilePath.c_str());
	}
	return queryDirConfig();
}

//请求dir，获取更新地址
bool UpdateMgrImp::queryDirConfig()
{
	std::string strURL = s_versionUrl;

	if (s_cdnup)
	{
		strURL += "/cdnurl";

		std::string strRspData;

		long code = 0;
		int iRet = P_HTTP_PROXY->HttpGet(strURL, code, &strRspData, false);
		CCLOG("[trace]UpdateResScreen::onQueryDirConfigRsp,rsp code [%d], rsp [%s]", iRet, strRspData.c_str());

		if (!P_HTTP_PROXY->bIsDoCurlOK(iRet)) 
		{
			//连接失败
			CCLOG("[error]UpdateResScreen::onQueryDirConfigRsp,rsp code %d", iRet);
			m_status.errorCode = iRet;
			return false;
		}

		if (strRspData.size() <= 0) 
		{
			CCLOG("[trace]UpdateResScreen::onQueryDirConfigRsp, dir config is empty,no need update");
			return true;
		}
		else 
		{
			CCLOG("[trace]UpdateResScreen::onQueryDirConfigRsp, dir config is [%s]", strRspData.c_str());
			setResUrl(strRspData.c_str());
			return true;
		}
	}

	CCLOG("[trace]UpdateResScreen::queryDirConfig,query dir config, %s", strURL.c_str());
	setResUrl(s_versionUrl.c_str());
	return true;
}



