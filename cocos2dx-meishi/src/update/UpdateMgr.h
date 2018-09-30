#ifndef __UpdateMgr_H_
#define __UpdateMgr_H_

//#include <thread>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <condition_variable>
#include <queue>
#include <string>
#include <unordered_map>
#include <functional>

class TempFile;
struct DownloadPackage {
    DownloadPackage(){
    }
	DownloadPackage(const char * _pkg, const char * _sign, const char * _version, int nSize) :
		packageName(_pkg), sign(_sign), version(_version), nSize(nSize){
    }
    std::string packageName;
    std::string sign;
	std::string version;
	int		nSize;
};

struct DownloadApk
{
	DownloadApk(){
	}
	DownloadApk(const char * _apkName, const char * _apkURL) :
		apkName(_apkName), apkURL(_apkURL){
	}
	std::string apkName;
	std::string apkURL;
};

class iUpdateCallback
{
public:
	//virtual void setPercentage(float percent) = 0;
	//virtual void setNoticeWords(const char* szText) = 0;
	virtual void onChangeScene() = 0;
	virtual void onFail() = 0;
	virtual void onRestart() = 0;
};


class UpdateMgr {
public:
    enum ExceptionDef
    {
        //出错定义
        ERROR_CURL_INIT_FAIL    = 1001, //curl初始化失败
        ERROR_CURL_DO_FAIL     , //curl_easy_perform失败
        ERROR_CURL_DO_FAIL2    , //

        ERROR_JSON_PARSE_FAIL  , //JSON parse fail
        ERROR_JSON_FORMAT_ERROR, //JSON 格式不对
        ERROR_JSON_EXCEPTION   , //JsonException
		ERROR_JSON_NOT_EXIST   , //json 文件不存在

        ERROR_SIGN_WRONG       , //单个包md5校验失败

        ERROR_ZIP_OPEN_FAIL    , //ZipFile open失败
        ERROR_ZIP_EXTRACT_FAIL , //解压失败
        ERROR_VER_FILE_FAIL    , //version文件打开失败

		ERROR_USER_CANCEL,		 //用户取消更新
    };


    enum Stage {
        CHECK_VERSION = 0,
		WAIT_CONFIRM,		//等待用户确认
        DOWNLOAD_RESOURCE,
        EXTRACT_RESOURCE, //解压文件
		RESTART,
        DONE,
		UPDATE_FAIL, //更新失败
		LOAD,
		DOWNLOAD_APK,
		DOWNLOAD_APKFAIL,
		RELOAD_APK,
    };


    struct Status {
        Stage stage;
        int totalStep;
        int nowStep;
        int nowProgress;
        std::string filename;
        int errorCode;
        bool needRestartGame; //是否需要重启游戏
    };
    
    UpdateMgr();
    virtual ~UpdateMgr();
	void setUpdateCallback(iUpdateCallback* callback) { m_callback = callback; };

	virtual void startUpdate();
    Status GetStatus();
	void SetStage(Stage stage);
    int OnProgress(float total, float now);
	void ThrowError(const char * filename, int what);

	bool checkNeedPackage();
	bool isUpdate(std::string serS, std::string locS);
	std::thread * getUpdateThread(){ return m_updateThread; }

	virtual std::string GetExtractPath() = 0;
	virtual void setResUrl(const char* pszUrl) = 0;

	bool DownloadServerStateFile(const char * url);

	int	getUpdateConfirm(){ return mUpdateConfirm; }
	void	 setUpdateConfirm(int nConfirm){ mUpdateConfirm = nConfirm; }

	void	setPromptLimit(int nLimit){ m_LimitPrompt = nLimit; }

private:

	// 提示用户是否继续更新 0:未输入 1:取消 2:确认
	bool waitCheckDownload();
	void PushNeedPackage(const char * pkg, const char * sign, const char * version, int nSize);
    bool GetNeedPackageList(const char * versionFile, const char * version);
    bool UpdateSinglePkg(const DownloadPackage & pkgInfo);
	bool DownloadFile(TempFile & file, const char * url, const char * filename);
    bool DownloadVersionFile(const char * path, const char * url, const char * filename);
    bool ExtractFile(TempFile & tmpFile, const char * pkgname);
	void loadGameRes();
	void downloadAPK(const char *apkName ,const char * url);
protected:
	virtual bool initUpdate(){ return true; };

    virtual std::string GetPkgUrl(const char * pkgName) = 0;
    virtual std::string GetTempDownloadPath(const char * pkgName) = 0;
	virtual std::string GetCurrentVersion() = 0;
    virtual bool UpdateVersion(std::string version) = 0;
    virtual bool IsApkNewest() = 0; //apk assets版本是否比本地版本高
    virtual void delResDataDir();

    void doUpdate();



protected:
	iUpdateCallback* m_callback;

	Status m_status;
	std::vector<DownloadPackage> m_needPkg;
	DownloadApk m_needAPK;

	std::string m_resUrl;

	// 用户确认更新信息 1:取消 2:确定
	int	mUpdateConfirm;

private:
    std::mutex m_mutex;
	std::thread* m_updateThread;
	bool b_downloadApk;
	int		m_LimitPrompt;	// 更新包大于该值，提示wifi下载
};

#endif //__UpdateMgr_H_

