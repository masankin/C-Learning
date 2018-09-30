#ifndef __UpdateMgrImp_H_
#define __UpdateMgrImp_H_

#include "UpdateMgr.h"


class UpdateMgrImp : public UpdateMgr 
{
public:

	static UpdateMgrImp* getInstance();

	virtual void setResUrl(const char* pszUrl);

	//-1表示更新失败，0 表示不需要更新 1表示需要更新
	void updateSearchPath();
	void startUpdate();

	void setIsUpdateSceneShow(bool isShow)
	{
		m_isShowUpdateScene = isShow;
	}

	bool getIsUpdateSceneShow()
	{
		return m_isShowUpdateScene;
	}
public:
	virtual std::string GetExtractPath();
	virtual std::string GetCurrentVersion();
protected:

	UpdateMgrImp();
	//~UpdateMgrImp();

	bool initUpdate();
	bool queryDirConfig();

    virtual std::string GetPkgUrl(const char * pkgName);
    virtual std::string GetTempDownloadPath(const char * pkgName);
    virtual bool UpdateVersion(std::string version);
    virtual bool IsApkNewest();
    virtual void delResDataDir();


	std::string getJsonVer(const char* strFilePath);

	//比较版本号是否更新

private:
	bool m_isShowUpdateScene;

	static UpdateMgrImp* s_updatMgrImp;

    std::string m_strResUrl;
	std::string m_writableFolder;

	//是否可以更新搜索路径(默认true,如果apk版本更高，则为false)
	//bool m_bCanUpdateSearchPath; 
};

#endif //__UpdateMgrImp_H_
