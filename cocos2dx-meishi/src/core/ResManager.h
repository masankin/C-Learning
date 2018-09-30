#pragma once

#include <string>
#include "Singleton.h"

#include "cocos2d.h"

#include "json/stringbuffer.h"
#include "json/document.h"
#include "json/writer.h"
#include "cocostudio/CCArmature.h"

using namespace cocostudio;
USING_NS_CC;

//资源type
enum eResourceType
{
	eResourcePlist,
	eResourceArmature,
	eResourceCSV,
	eResourcePlistAndPng,
};

class RecourcePath;

class ResManager : public Ref
{
public:

	ResManager();
	~ResManager();

	// 资源路径相关
	void init();

	void	setResourcePath();
	void	setUpdateResPath();

	// 资源加载相关
	void	asyncLoadCsvData();
	void	syncLoadData();
	void	syncLoadLua();

	void	doLoadOver();

	float	getProgress() { return mLoadProgress; }

	Armature* getAramatur(std::string name);
	void removeAramaturFromeParent();
	class SyncRes
	{
	public:

		eResourceType eResType;
		std::string name;
	};

	virtual void update(float delta);

	void loadCommonRes();
	void loadCommonResCallback(cocos2d::EventCustom* customData);
	void loadResByPageType(int pageType, bool bFrameLoad = false);
	void purgeResByPageType(int pageType);

	void addResourcesTask(int pageType);

	const std::vector<std::string>& getResNamesByType(int pageType);
	const std::vector<std::string>& getResNamesByType(char * resName);


	void loadAsync();
protected:
	bool initResourcesJson();
	void	initSyncLoadData();

	void	addSyncRes(std::string, eResourceType);
	bool	doLoadRes(SyncRes* ); 

	RecourcePath*		mResPathTable;

	typedef std::map<std::string, SyncRes*> SyncResMap;
	SyncResMap mSyncRes;

	int		mTotalRes;
	float	mLoadProgress;
	
	bool	m_bLoadedConfig;
	rapidjson::Document m_resConfig;
	// weihua.cui 2015.11.16

	bool loadAnimation(std::string frameName, int frameCount, float playTime);
	void initResources();
	void loadResources(std::string& sPath);
	void purgeResources(std::string& sPath);
	std::unordered_map <std::string, std::vector<std::string>> m_mapResource;
	std::vector<int> m_vecPages;

	std::map<std::string, std::vector<Armature*> > armatureMap;
public:
	const std::unordered_map <std::string, std::vector<std::string>>& getResouces();
};


#define sgResManager() Singleton<ResManager>::Get()
