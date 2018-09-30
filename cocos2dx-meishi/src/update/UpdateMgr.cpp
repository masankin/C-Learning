#include <sys/stat.h>
#include <ctime>
#ifdef WIN32
//do nothing
#define sleep(ts)	Sleep(ts * 1000);
#else  
#include <utime.h> 
#include <sys/time.h> 
#include <dirent.h>  
#include <unistd.h>
#define sleep(ts)	usleep(ts * 1000 * 1000);



#endif


#include "curl/curl.h"
#include "UpdateMgr.h"
#include "UpdateHelper.h"
#include "data/ItemManager.h"
#include "update/HttpProxy.h"

#include "json/document.h"
#include "json/filestream.h"
#include "game/utils/Comm.h"
#include "editor-support/cocostudio/DictionaryHelper.h"
#include "channel/ChannelApi.h"
#include "GameJniHelper.h"
#include "ResManager.h"
#include "game/login/LoginProxy.h"

#include "ui/dialogs/GenerlDialog.h"
#include "data/TableManager.h"

using namespace cocostudio;

USING_NS_CC;
using namespace std;

const char * VERSION_FILE_NAME = "version.json";
const char * SERVER_STATE_FILE_NAME = "serverState.json";

namespace
{


	size_t writefile_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
	{
		TempFile * file = (TempFile*)userdata;
		if (file != NULL) {
			size_t n = file->Write(ptr, size, nmemb);
			if (n != size * nmemb) {
				CCLOG("Fail to serialize to local disk");
			}
			//CCLOG("[trace]saveData,Write %d byte data", (int)n);
			return n;
		}
		return 0;
	}
	size_t writeAPK_callback(char* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
	{
		FILE* fp = (FILE*)pParam;
		size_t nWrite = fwrite(pBuffer, nSize, nMemByte, fp);
		return nWrite;
	}

    int progress_callback(void *clientp, double totalToDownload, double nowDownloaded, double totalToUpLoad, double nowUpLoaded)
	{
        UpdateMgr * updatemgr = (UpdateMgr *) clientp;
        return updatemgr->OnProgress(totalToDownload, nowDownloaded);
    }

	size_t version_callback(char *ptr, size_t size, size_t nmemb, void *userdata) 
	{
		
		FILE * file = (FILE*)userdata;
		return fwrite(ptr, size, nmemb, file);
	}

	size_t serverState_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
	{

		FILE * file = (FILE*)userdata;
		return fwrite(ptr, size, nmemb, file);
	}

    class UpdateException {
    public:
        UpdateException(const char* filename, int what):
        m_filename(filename), m_what(what){
        }
      
    public:
        std::string m_filename;
        int m_what;
    };
}


UpdateMgr::UpdateMgr() {
    m_status.errorCode = 0;
    m_status.stage = CHECK_VERSION;
    m_status.totalStep = 0;
    m_status.nowStep = 0;
    m_status.nowProgress = 0;
    m_status.needRestartGame = false;
	m_updateThread = NULL;
	b_downloadApk = false;
	m_needAPK.apkName = "";
	m_needAPK.apkURL = "";
	mUpdateConfirm = 0;
}

bool UpdateMgr::DownloadFile(TempFile & file, const char * url, const char * filename) 
{
	if (!file.Open("wb")) {
		CCLOG("[trace]Fail to open local file to write %s", file.GetPath().c_str());
		return false;
	}

	m_status.stage = DOWNLOAD_RESOURCE;
	CCLOG("[trace]UpdateMgr::DownloadFile set stage:%d!", DOWNLOAD_RESOURCE);

	long code = 0;
	std::string strRspData;
	int iRet = P_HTTP_PROXY->HttpGet(url, code, &file, true, writefile_callback, this, progress_callback, false);
	CCLOG("[trace]UpdateMgr::DownloadFile,iRet:[%d], rsp:[%s]", iRet, strRspData.c_str());
	if (!P_HTTP_PROXY->bIsDoCurlOK(iRet))
	{
		CCLOG("[trace]UpdateMgr::DownloadFile fetch data from %s failed!", url);
		return false;
	}
	return true;
}

int UpdateMgr::OnProgress(float total, float now) {
    if(total <= 0.1) {
        return 0;
    }
	m_mutex.lock();
    m_status.nowProgress = int(now * 100 / total);

	m_mutex.unlock();
    //CCLOG("[trace]on progress %d", m_status.nowProgress);
    return 0;
}

bool UpdateMgr::DownloadVersionFile(const char * path, 
                                    const char * url, 
                                    const char * filename) 
{
	std::string tmpfilename(path);
	FILE * file = fopen(tmpfilename.c_str(), "wb");

	long code = 0;
	std::string strRspData;
	int iRet = P_HTTP_PROXY->HttpGet(url, code, file, false, version_callback);
	CCLOG("[trace]UpdateMgr::DownloadFile,iRet:[%d], rsp:[%s]", iRet, strRspData.c_str());
	if (!P_HTTP_PROXY->bIsDoCurlOK(iRet))
	{
		CCLOG("[trace]UpdateMgr::DownloadFile fetch data from %s failed!", url);
		fclose(file);
		return false;
	}

	fclose(file);

	if (iRet == 0 && code == 200) {
		//200 - 确定。客户端请求已成功。
		CCLOG("[trace]UpdateMgr::DownloadVersionFile success!url:%s filename:%s, path:%s!", url, filename, path);
	}
	else if (iRet != 0)
	{
		m_status.errorCode = ERROR_CURL_DO_FAIL;
		remove(tmpfilename.c_str());
		return false;
	}
	return true;
}

bool UpdateMgr::DownloadServerStateFile(const char * url)
{
	std::string tmpfilename = GetTempDownloadPath(SERVER_STATE_FILE_NAME).c_str();
	FILE * file = fopen(tmpfilename.c_str(), "wb");

	long code = 0;
	int iRet = P_HTTP_PROXY->HttpGet(url, code, file, false, version_callback);

	if (!P_HTTP_PROXY->bIsDoCurlOK(iRet))
	{
		fclose(file);
		return false;
	}

	fclose(file);

	if (iRet == 0 && code == 200) {
		//200 - 确定。客户端请求已成功。
	}
	else if (iRet != 0)
	{
		remove(tmpfilename.c_str());
		return false;
	}

	return true;
}

void UpdateMgr::startUpdate() {

	if (sgLoginProxy()->getIsUpdateResource())//非灰度发布或属于白名单
	{
		CCLOG("[trace]UpdateMgr::startUpdate new thread!");
		m_updateThread = new std::thread(&UpdateMgr::doUpdate, this); 
		if (m_updateThread == 0) 
		{
		    //pthread_detach(thread);
			CCLOG("failed to create thread");
			return;
		}
		setUpdateConfirm(0);
	}
	else//灰度发布中并且不属于白名单
	{
		SetStage(UpdateMgr::LOAD);
		//sgResManager()->syncLoadLua();
		//sgResManager()->asyncLoadCsvData();
		//sgResManager()->syncLoadData();
	}
}


bool UpdateMgr::checkNeedPackage()
{

	try {
		UpdateHelper::MakeDirs(this->GetExtractPath().c_str()); //先创建文件夹
		std::string path = this->GetExtractPath();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) 
		//win下m_writableFolder最后带着 反斜杠
		path += "\\Resources\\";
#else  
		path += "/Resources/";
#endif
		if (IsApkNewest()) 
			delResDataDir();

		// url地址为空，直接返回
		if (GetPkgUrl("").empty())
		{
			m_mutex.lock();
			m_status.stage = UPDATE_FAIL;
			m_status.errorCode = ERROR_JSON_NOT_EXIST;
			m_mutex.unlock();
			CCLOG("[trace]UpdateMgr::checkNeedPackage resUrl empty!", UPDATE_FAIL);
			return false;
		}

		UpdateHelper::MakeDirs(path.c_str()); //先创建文件夹

		std::string versionFilePath = GetTempDownloadPath(VERSION_FILE_NAME);

		bool ret = DownloadVersionFile(versionFilePath.c_str(), GetPkgUrl(VERSION_FILE_NAME).c_str(), VERSION_FILE_NAME);
		if (!ret)
		{
			m_mutex.lock();
			m_status.stage = UPDATE_FAIL;
			m_status.errorCode = ERROR_CURL_DO_FAIL;
			m_mutex.unlock();
			CCLOG("[trace]UpdateMgr::checkNeedPackage DownloadVersionFile fail,set stage:%d!", UPDATE_FAIL);
			return false;
		}
#ifdef MGAME_PLATFORM_MPLUS
		setResUrl(m_resUrl.c_str());
#endif
		GetNeedPackageList(versionFilePath.c_str(), GetCurrentVersion().c_str());
	}
	catch (const UpdateException & e) 
		{
			m_mutex.lock();
			m_status.stage = UPDATE_FAIL;
			m_status.errorCode = e.m_what;
			m_status.filename = e.m_filename;
			m_mutex.unlock();
			CCLOG("[trace]UpdateMgr::checkNeedPackage UpdateException,set stage:%d!", UPDATE_FAIL);
			return false;
		}
	return true;
}

void UpdateMgr::doUpdate() 
{
	if (m_updateThread)
		CCLOG("[trace]UpdateMgr::doUpdate, Now Thread ID:%d",m_updateThread->get_id());

	m_mutex.lock();
	m_status.stage = CHECK_VERSION;
	m_mutex.unlock();
	if (!initUpdate())
	{
		m_mutex.lock();
		m_status.stage = UPDATE_FAIL;
		m_mutex.unlock();
		CCLOG("[trace]UpdateMgr::doUpdate initUpdate fail,set stage:%d!", UPDATE_FAIL);
		return;
	}

	if (!checkNeedPackage())
	{
		m_mutex.lock();

		if (m_status.errorCode == ERROR_JSON_NOT_EXIST ||
			m_status.errorCode == ERROR_CURL_DO_FAIL)
			m_status.stage = DONE;
		else if (m_status.stage != RELOAD_APK)
			m_status.stage = UPDATE_FAIL;
		m_mutex.unlock();
		CCLOG("[trace]UpdateMgr::doUpdate checkNeedPackage fail,set stage:%d!", m_status.stage);
		return;
	}

	if (!waitCheckDownload())
	{
		m_mutex.lock();
		m_status.stage = UPDATE_FAIL;
		m_status.errorCode = ERROR_USER_CANCEL;
		m_mutex.unlock();
		CCLOG("[trace]UpdateMgr::doUpdate checkNeedPackage fail,set stage:%d!", m_status.stage);
		return;
	}

	if (m_needAPK.apkURL != "" && m_needAPK.apkName != "")
	{
		downloadAPK(m_needAPK.apkName.c_str(), m_needAPK.apkURL.c_str());
		return;
	}

	if (m_needPkg.size() <= 0) {
		CCLOG("[trace]UpdateMgr::doUpdate,no pkg need to be updated.");
	}
	else {
		for (size_t i = 0; i < m_needPkg.size(); ++i)
		{
			CCLOG("[trace]UpdateMgr::doUpdate,need Updated Num:%d",m_needPkg.size());
			bool bSuccessUpdate = UpdateSinglePkg(m_needPkg[i]);
			if (!bSuccessUpdate)
			{
				m_mutex.lock();
				m_status.stage = DONE;
				m_status.errorCode = ERROR_CURL_DO_FAIL2;
				m_mutex.unlock();
				CCLOG("[trace]UpdateMgr::doUpdate %d UpdateSinglePkg fail,set stage:%d!", i, DONE);
				return;
			}

			//如果需要重启，直接返回 
			if (GetStatus().needRestartGame)
			{
				m_mutex.lock();
				m_status.stage = RESTART;
				m_mutex.unlock();
				CCLOG("[trace]UpdateMgr::doUpdate %d needRestartGame,set stage:%d!", i, DONE);
				return;
			}
		}
	}

	m_mutex.lock();
	m_status.stage = DONE;
	m_mutex.unlock();
	CCLOG("[trace]UpdateMgr::doUpdate end,set stage:%d!", DONE);
}

bool UpdateMgr::GetNeedPackageList(const char * versionFile, const char * version)
{
	Data bytecpy = FileUtils::getInstance()->getDataFromFile(versionFile);
	std::string fileContent = std::string((const char*)bytecpy.getBytes(), bytecpy.getSize());

	rapidjson::Document json;
	rapidjson::StringStream stream((const char*)fileContent.c_str());

	if (fileContent.size() >= 3) {
		// Skip BOM if exists
		const unsigned char* c = (const unsigned char *)fileContent.c_str();
		unsigned bom = c[0] | (c[1] << 8) | (c[2] << 16);

		if (bom == 0xBFBBEF)  // UTF8 BOM
		{
			stream.Take();
			stream.Take();
			stream.Take();
		}
	}
	json.ParseStream<0>(stream);
	if (json.HasParseError()) {
		CCLOG("GetParseError %s\n", json.GetParseError());
		m_mutex.lock();
		m_status.stage = UPDATE_FAIL;
		m_status.errorCode = ERROR_JSON_PARSE_FAIL;
		m_mutex.unlock();
		CCLOG("[trace]UpdateMgr::GetNeedPackageList json.HasParseError,set stage:%d!", UPDATE_FAIL);
		return false;
	}

#if (CC_TARGET_PLATFORM ==CC_PLATFORM_ANDROID)
	b_downloadApk = false;
	const rapidjson::Value& subApkNewVersion = DICTOOL->getSubDictionary_json(json, "apknewest"); //最新APK版本

	if (isUpdate(subApkNewVersion.GetString(), version) && b_downloadApk)
	{
		//获取ApkList
		const rapidjson::Value& subApkList = DICTOOL->getSubDictionary_json(json, "androidapks");

		//获取自己渠道对应的cdnApk包
		std::string channelName = sgChannelApi()->getPlatformName();
		if (!channelName.empty())
		{
			std::string _apkPath = DICTOOL->getStringValue_json(subApkList, channelName.c_str());
			if (!_apkPath.empty())
			{
				vector<std::string > vApkPath = splitStr(_apkPath, "/");
				if (vApkPath.size() == 2 && vApkPath[1] != "")
				{
					std::string url = "apk/";
					url += _apkPath;
					m_needAPK.apkName = vApkPath[1];
					m_needAPK.apkURL = GetPkgUrl(url.c_str());
					CCLOG("[trace]GetNeedPackageList  need DownLoad APK ,apkName:%s----apkURL:%s", m_needAPK.apkName.c_str(), m_needAPK.apkURL.c_str());
					//downloadAPK(vApkPath[1].c_str(), GetPkgUrl(url.c_str()).c_str());//下载APK
					return false; //不需要更新资源。返回false
				}
			}
		}
	}
#endif

	int verLen = DICTOOL->getArrayCount_json(json, "versions");
	std::string szCurPlatform = sgChannelApi()->getPlatformName();

	for (size_t i = 0; i < verLen; ++i) {
		const rapidjson::Value& val = DICTOOL->getDictionaryFromArray_json(json, "versions", i);
		
		if (!val.IsObject()) {
			m_mutex.lock();
			m_status.stage = UPDATE_FAIL;
			m_status.errorCode = ERROR_JSON_PARSE_FAIL;
			m_mutex.unlock();
			CCLOG("[trace]UpdateMgr::GetNeedPackageList json.item:%d,set stage:%d!", i, UPDATE_FAIL);
			return false;
			//return ThrowError(VERSION_FILE_NAME, ERROR_JSON_FORMAT_ERROR);
		}

		std::string thisVersion = DICTOOL->getStringValue_json(val, "ver");
		bool bNeedUpdate = false;

		/*
		* 0:需要更新 
		* 不需要更新 1：当前平台名为空 2:版本较低 3：不在更新平台列表内
		*/
		int errorNo = 0;
		do 
		{
			if (szCurPlatform.empty())
			{
				errorNo = 1;
			}else if (isUpdate(thisVersion, version))
			{
				int all_platform = DICTOOL->getIntValue_json(val, "all_platform");
				if (all_platform == 1)
				{
					bNeedUpdate = true;
					break;
				}
				else{

					int len = DICTOOL->getArrayCount_json(val, "platforms");
					for (int i = 0; i < len; ++i)
					{
						const rapidjson::Value& __platform = DICTOOL->getDictionaryFromArray_json(val, "platforms", i);
						string _name = DICTOOL->getStringValue_json(__platform, "name");
						if (_name == szCurPlatform)
						{
							bNeedUpdate = true;
							break;
						}
					}
					errorNo = 3;
				}
			}
			else{
				errorNo = 2;
			}

		} while (0);
		
		if (bNeedUpdate)
		{
			PushNeedPackage(DICTOOL->getStringValue_json(val, "pkg"), DICTOOL->getStringValue_json(val, "sign"), thisVersion.c_str(), DICTOOL->getIntValue_json(val, "size"));
		}
		CCLOG("[trace]UpdateMgr::GetNeedPackageList [current]-platform:%s ver:%s [thisver]-ver:%s errorNo:%d", szCurPlatform.c_str(), version, thisVersion.c_str(), errorNo);
	}
	return true;
}

UpdateMgr::~UpdateMgr() {
    //pthread_mutex_destroy(&m_mutex);
}

void UpdateMgr::PushNeedPackage(const char * pkg, const char * sign, const char * version, int nSize) {
    CCLOG("[trace]need pkg=%s,sign=%s,version=%s,size:%d!", pkg, sign, version, nSize);
    m_needPkg.push_back(DownloadPackage(pkg, sign, version, nSize));
}


bool UpdateMgr::UpdateSinglePkg(const DownloadPackage & pkgInfo) 
{
    {
		m_mutex.lock();
        m_status.nowStep++;
        m_status.nowProgress = 0;
		m_status.filename = pkgInfo.packageName;
		m_status.stage = DOWNLOAD_RESOURCE;
		m_status.totalStep = (int)m_needPkg.size();
		m_mutex.unlock();
    }

	//CCLOG("[trace]UpdateMgr::UpdateSinglePkg, Now Thread ID:%d", m_updateThread->get_id());
    std::string url = GetPkgUrl(pkgInfo.packageName.c_str());
    TempFile tmpFile(GetTempDownloadPath(pkgInfo.packageName.c_str()));


	if (!DownloadFile(tmpFile, url.c_str(), pkgInfo.packageName.c_str()))
	{
		return false;
	}


	CCLOG("tempFile:%s", tmpFile.GetPath().c_str());
	CCLOG("url:%s", tmpFile.GetPath().c_str());

    if (tmpFile.GetSign() != pkgInfo.sign) 
	{
        CCLOG("[error]pkg %s's sign not equal,tmp file sign=%s,need sign=%s",
            pkgInfo.packageName.c_str(), tmpFile.GetSign().c_str(), pkgInfo.sign.c_str());
        //ThrowError(pkgInfo.packageName.c_str(), ERROR_SIGN_WRONG);

		m_mutex.lock();
		m_status.stage = UPDATE_FAIL;
		m_status.nowProgress = 0;
		m_mutex.unlock();
		CCLOG("[trace]UpdateMgr::UpdateSinglePkg checkSign,set stage:%d!", UPDATE_FAIL);
		return false;
    }


	if (!ExtractFile(tmpFile, pkgInfo.packageName.c_str()))
	{
		return false;
	}


	if (!UpdateVersion(pkgInfo.version))
	{
		return false;
	}
    {
		m_mutex.lock();
        m_status.nowProgress = 100;
		m_mutex.unlock();
		return true;
    }
}

UpdateMgr::Status UpdateMgr::GetStatus() {	 
    return m_status;
}

void UpdateMgr::SetStage(Stage stage)
{
	m_mutex.lock();
	m_status.stage = stage;
	m_mutex.unlock();

	CCLOG("[trace]UpdateMgr::SetStage stage:%d!", (int)stage);
}


bool UpdateMgr::ExtractFile(TempFile & tmpFile, const char * pkgname) 
{
    tmpFile.Close();

    //解压文件
	FZipFile zipfile(tmpFile.GetPath().c_str(), &(m_status.nowProgress));
    m_status.stage = EXTRACT_RESOURCE;
	CCLOG("[trace]UpdateMgr::ExtractFile set stage:%d!", EXTRACT_RESOURCE);

    if (!zipfile.IsOpen()) 
	{
        CCLOG("[error]UpdateMgr::ExtractFile,Fail to open zip file %s", pkgname);
        //ThrowError(pkgname, ERROR_ZIP_OPEN_FAIL);
		return false;
    }

    if (zipfile.ExtractTo(GetExtractPath().c_str()) != 0) {
        CCLOG("[error]UpdateMgr::ExtractFile,Fail to extract zip file %s", pkgname);
        //ThrowError(pkgname, ERROR_ZIP_EXTRACT_FAIL);
		return false;
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if(zipfile.getIsHaveSo()) 
        m_status.needRestartGame = true;
#endif
    
	return true;
}

void UpdateMgr::loadGameRes()
{
	{
		m_mutex.lock();
		m_status.nowProgress = 0;
		m_mutex.unlock();
	}


	sgItemManager()->setProgress(&(m_status.nowProgress));
	sgItemManager()->loadResDataAsync();
}


void UpdateMgr::ThrowError(const char * filename, int what) 
{
    throw UpdateException(filename, what);
}

void UpdateMgr::delResDataDir()
{
    //不需要更新时，删除下载目录
    CCLOG("[trace]UpdateMgr::delResDataDir");
    std::string path = this->GetExtractPath();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) 
    //win下m_writableFolder最后带着 反斜杠
    path += "\\Resources\\";
#else  
    path += "/Resources/"; 
#endif

    UpdateHelper::RemoveDir(path.c_str());
}

bool UpdateMgr::isUpdate(std::string serS, std::string locS)
{
	auto serV = splitStr(serS, ".");
	auto locV = splitStr(locS, ".");

	for (int i = 0; i < serV.size(); i++)
	{
		if (i + 1>locV.size())
			return false;


		int ser = atoi(serV.at(i).c_str());
		int loc = atoi(locV.at(i).c_str());
		if (ser > loc)
		{
			if (i == 0)
			{
				b_downloadApk = true;//大版本需要更新
			}
			return true;
		}
			
		else  if (ser < loc)
			return false;
	}

	return false;
}
 
void UpdateMgr::downloadAPK(const char *apkName, const char * url)
{

	//std::string tmpfilename(GetTempDownloadPath(apkName));
	std::string tmpfilename = GameJniHelper::getInstance()->getSDCardPath();
	tmpfilename += apkName;

	FILE * file = fopen(tmpfilename.c_str(), "w");
	CCLOG("[trace]UpdateMgr::downloadAPK filePath:%s----url:%s-----apkName:%s", tmpfilename.c_str(), url, apkName);
	std::string strRspData;

	{	
		m_mutex.lock();
		m_status.stage = DOWNLOAD_APK;
		m_status.nowProgress = 0;
		m_mutex.unlock();
	}

	long code = 0;
	int iRet = P_HTTP_PROXY->HttpGet(url, code, file, true, writeAPK_callback, this, progress_callback, false);

	if (!P_HTTP_PROXY->bIsDoCurlOK(iRet))
	{
		CCLOG("[trace]UpdateMgr::DownloadFile fetch data from %s failed!  ret:%d", url,iRet);
		m_mutex.lock();
		m_status.stage = DOWNLOAD_APKFAIL;
		m_mutex.unlock();
		fclose(file);
		return;	
	}

	m_mutex.lock();
	m_status.stage = RELOAD_APK;
	m_mutex.unlock();

	CCLOG("[trace]UpdateMgr::DownloadFile succeed");
	fclose(file);

	//delResDataDir();
	
	GameJniHelper::getInstance()->reloadAPK(apkName);
}

bool UpdateMgr::waitCheckDownload()
{
	if (m_needAPK.apkURL != "" && m_needAPK.apkName != "")
		return true;

	// 统计需要下载的更新包大小
	int nTotalSize = 0;
	for (size_t i = 0; i < m_needPkg.size(); ++i)
		nTotalSize += m_needPkg[i].nSize;

	if (nTotalSize < m_LimitPrompt)
		return true;
	else{

		//弹窗提示
		m_mutex.lock();
		m_status.stage = WAIT_CONFIRM;
		m_mutex.unlock();

#if(CC_PLATFORM_ANDROID == CC_TARGET_PLATFORM)
		// 当前wifi在线
		if (GameJniHelper::getInstance()->isWifiConnected())
			return true;
#endif
		int nInput = 0;
		while (1)
		{
			if (mUpdateConfirm == 1)
			{
				m_mutex.lock();
				m_status.stage = UPDATE_FAIL;
				m_status.errorCode = ERROR_USER_CANCEL;
				m_mutex.unlock();
				CCLOG("[trace]UpdateMgr::doUpdate thread wait, user cancel!");
				return false;
			}else if (mUpdateConfirm == 2)
			{
				return true;
			}
			sleep(0.2f);
		}
	}
	return true;
}
