#include "ResManager.h"
#include "data/Tables.h"
#include "data/TableManager.h"
#include "login/VideoManager.h"
#include "ui/PageFunction.h"
#include "sync/SyncControl.h"
#include "cocostudio/CCArmatureDataManager.h"
#include "login/LoginProxy.h"
#include "cocos2d.h"
#include "Constant.h"
#include "guide/GuideManager.h"
#include "battle/BattleRecordsManager.h"
#include "GamePrecedure.h"
#include "base/CCScriptSupport.h"
#include "CCLuaEngine.h"

USING_NS_CC;


#define COMMON_RES (9999)

ResManager::ResManager() :m_bLoadedConfig(false)
{
	init();
}

ResManager::~ResManager()
{
	CC_SAFE_DELETE(mResPathTable);
}

void ResManager::init()
{
	//CCTexture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);
	mResPathTable = new RecourcePath();
	mResPathTable->init();

	std::vector<std::string> mOriSearchPath;
	const std::vector<std::string>& paths = cocos2d::FileUtils::getInstance()->getSearchPaths();
	if (!paths.empty())
	{
		mOriSearchPath.assign(paths.begin(), paths.end());
	}
	cocos2d::FileUtils::getInstance()->setSearchPaths(mOriSearchPath);
	cocos2d::FileUtils::getInstance()->purgeCachedEntries();
}

void ResManager::setResourcePath()
{
	RecourcePath::RecourcePathListIterator itr = mResPathTable->getRecourcePathIterator();
	while (itr.hasMoreElements())
	{
		std::string seachPath = itr.getNext()->path;
		cocos2d::FileUtils::getInstance()->addSearchPath(seachPath.c_str());
	}
	cocos2d::FileUtils::getInstance()->purgeCachedEntries();
}

void ResManager::setUpdateResPath()
{
	std::string mAdditionalSearchPath;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	mAdditionalSearchPath = "data/Resources";
#else
	mAdditionalSearchPath = "data/Resources";
#endif

	std::string searchpath = cocos2d::FileUtils::getInstance()->getWritablePath() + "/" + mAdditionalSearchPath;
	CCLOG("[trace]ResManager::setUpdateResPath searchpath:%s.", searchpath.c_str());

	cocos2d::FileUtils::getInstance()->addSearchPath(searchpath.c_str(), true);
	RecourcePath::RecourcePathListIterator itr = mResPathTable->getRecourcePathIterator();
	while (itr.hasMoreElements())
	{
		std::string path = searchpath + "/" + itr.getNext()->path;
		cocos2d::FileUtils::getInstance()->addSearchPath(path.c_str(), true);
	}
	cocos2d::FileUtils::getInstance()->purgeCachedEntries();
}

void ResManager::asyncLoadCsvData()
{
	sgTableManager()->loadAllTables();
}

void ResManager::syncLoadData()
{
	initSyncLoadData();
}

void ResManager::syncLoadLua()
{
	LuaEngine* engine = LuaEngine::getInstance();
	engine->getLuaStack()->loadChunksFromZIP("reslib");
}

void ResManager::addSyncRes(std::string name, eResourceType etype)
{
	SyncRes* pRes = new SyncRes();
	pRes->eResType = etype;
	pRes->name = name;

	mSyncRes.insert(std::make_pair(name, pRes));
}

void ResManager::initSyncLoadData()
{
	vector<string> names = getResNamesByType("Armature");

	for (vector<string>::iterator iter = names.begin(); iter != names.end(); iter++)
	{
		this->addSyncRes(*iter, eResourceArmature);
	}

	mTotalRes = mSyncRes.size();// +1;// 1:锟饺达拷锟斤拷锟竭程硷拷锟斤拷csv锟斤拷伞锟?

	if (m_mapResource.empty())
	{
		initResources();
	}

	loadCommonRes();

	//Director::getInstance()->getScheduler()->scheduleUpdate(this, -1, false);
}

void ResManager::update(float delta)
{
	{
		// 锟饺达拷锟斤拷锟竭程硷拷锟斤拷csv锟斤拷伞锟?
		eCsvLoadState eLoadState = sgTableManager()->getLoadState();
		VideoManager::eVideoState eVideoState = sgVideoManager()->getVideoState();
        
        if (eVideoState == VideoManager::VIDEO_COMPLETED) {
            sgVideoManager()->removeVideoLayer();
        }
        
		if (eLoadState == LS_OVER && eVideoState == VideoManager::VIDEO_COMPLETED)
		{
			doLoadOver();
			Director::getInstance()->getScheduler()->unscheduleUpdate(this);
		}
	}
}

bool ResManager::doLoadRes(SyncRes* pRes)
{
	//CCLOG("[trace]ResManager::doLoadRes loadres:%s!", pRes->name.c_str());
	if (pRes->eResType == eResourcePlist)
	{
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(pRes->name);
	} 
	else if (pRes->eResType == eResourceArmature)
	{
		//SpriteFrameCache::getInstance()->addSpriteFramesWithFile(pRes->name + ".plist");
		cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo(pRes->name + ".xml");
	}
	else if(pRes->eResType == eResourcePlistAndPng)
	{
		loadResources(pRes->name);
	}

	return true;
}

void ResManager::doLoadOver()
{ 
	//登陆超时到主页面

	sgUserObj()->setSaveDataOffLine(true);
	sgSyncControl()->setSyncSaveDataOver(true); 
	sgLoginProxy()->doResLoadOver(); 
	sgGuideManager()->initGuideData(); 
	 
	if (sgEnvSet("CLOSE_GUIDE") == 1)
	{
		sgGuideManager()->setCloseGuide(true);
		sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
		return;
	} 
	sgGuideManager()->setCloseGuide(false);
	if (sgUserObj()->getPersonalInfo()->changenamecount() != 0)
		sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
	else
	{
		const char* pScript = String::createWithFormat("sgBthControl():sendPveNofityRequest(%d,1,99,0)", 99010)->getCString();
		LuaEngine::getInstance()->executeString(pScript);
		sgBattleRecordsManager()->mBattleRecords.clear();
		sgGamePrecedure()->selectLevel(99010); //暂时这么写吧
		PageFunction::Get()->gotoFunction(MSG_FUC_PLAYING);
	}
}



void pushVector(std::vector<std::string>& vec, const char* ss[], int n ){
	for (int i = 0; i < n; i++)
	{
		vec.push_back(ss[i]);
	}
}

void pushVector(std::vector<std::string>& vec, std::vector<std::string> names)
{
	for (std::vector<std::string>::iterator iter = names.begin(); iter != names.end(); iter++)
	{
		vec.push_back(*iter);
	}
}


void ResManager::initResources()
{
	initResourcesJson();
	
	m_vecPages.push_back(MSG_FUC_CHRISTMAS);

}

const std::unordered_map <std::string, std::vector<std::string >>& ResManager::getResouces()
{
	if (m_mapResource.empty())
	{
		initResources();
	}
	return m_mapResource;
}


void ResManager::loadCommonRes()
{
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("loadCommonResCallback",
		CC_CALLBACK_1(ResManager::loadCommonResCallback, this));
	const char* pScript = __String::createWithFormat("sgSceneManager():loadSceneRes(%d)", 0)->getCString();
	LuaEngine::getInstance()->executeString(pScript);
}

void ResManager::loadCommonResCallback(cocos2d::EventCustom* customData)
{
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("loadCommonResCallback");
	Director::getInstance()->getScheduler()->scheduleUpdate(this, -1, false);
	armatureMap.clear();
	initResourcesJson();
	return;
	std::vector<std::string> larmaturVec;
	larmaturVec = m_mapResource["0"];

	for (int i = 0; i < larmaturVec.size(); i++)
	{
		Armature* armature = cocostudio::Armature::create(larmaturVec[i]);
		armature->retain();
		auto it = armatureMap.find(larmaturVec[i]);
		if (it == armatureMap.end())
		{
			std::vector<Armature*> mapArmatureVec;
			mapArmatureVec.push_back(armature);
			armatureMap.insert(std::make_pair(larmaturVec[i], mapArmatureVec));
		}
		else
		{
			it->second.push_back(armature);
		}
	}
}

Armature* ResManager::getAramatur(std::string name)
{
	auto it = armatureMap.find(name);
	if (it != armatureMap.end())
	{
		std::vector<Armature*> mapArmatureVec = it->second;
		for (auto itt = mapArmatureVec.begin(); itt != mapArmatureVec.end(); itt++)
		{
			if ((*itt)->getReferenceCount() == 1/*(*itt)->getParent() == NULL*/)
			{
				//(*itt)->retain();
				return *itt;
			}
		}
	}
	return cocostudio::Armature::create(name);
}

void ResManager::removeAramaturFromeParent()
{
	auto it = armatureMap.begin();
	for (; it != armatureMap.end(); it++)
	{
		std::vector<Armature*> mapArmatureVec = it->second;
		for (auto itt = mapArmatureVec.begin(); itt != mapArmatureVec.end(); itt++)
		{
			if ((*itt)->getParent() != NULL)
			{
				(*itt)->removeFromParent();
			}
		}
	}

}


void ResManager::loadResByPageType(int pageType, bool bFrameLoad)
{
}

void ResManager::purgeResByPageType(int pageType)
{
	if (pageType == 0)
	{
		return;
	}
	const char* pScript = __String::createWithFormat("sgResourceManager():closePool(%d)", pageType)->getCString();
	LuaEngine::getInstance()->executeString(pScript);
}

void ResManager::addResourcesTask(int pageType)
{
	m_vecPages.push_back(pageType);
}

const std::vector<std::string>& ResManager::getResNamesByType(int pageType)
{
	char buff[16];
	memset(buff, 0, 16);
	sprintf(buff, "%d", pageType);
	return m_mapResource[buff];
}

bool ResManager::initResourcesJson()
{
	if (!m_bLoadedConfig)
	{
		ssize_t size = 0;
		std::string load_str;

		unsigned char* titlech = FileUtils::getInstance()->getFileData("ResLoadConfig.json", "r", &size);
		load_str = std::string((const char*)titlech, size);
		m_resConfig.Parse<0>(load_str.c_str());

		if (m_resConfig.HasParseError())
		{
			CCASSERT(0, "res json load error");
			CCLOG("[error]GetParseError%s\n", m_resConfig.GetParseError());
			return false;
		}
		for (auto iter = m_resConfig.MemberonBegin(); iter != m_resConfig.MemberonEnd(); ++iter)
		{
			auto key = (iter->name).GetString();
			//auto& v = readdoc[key]
			for (auto i = 0; i < m_resConfig[key].Size(); ++i)
			{
				rapidjson::Value& valuedoc = m_resConfig[key][i];
				if (valuedoc.IsString())
				{
					m_mapResource[key].push_back(valuedoc.GetString());
				}
			}
		}
		m_bLoadedConfig = true;
	}
	return true;
}




const std::vector<std::string>& ResManager::getResNamesByType(char * resName)
{
	CCASSERT(resName, "resName cannot be null");
	return m_mapResource[resName];
}

void ResManager::loadResources(std::string& sPath)
{
	if (sPath.find(".plist") != sPath.npos)
	{
		std::string sPng = sPath.substr(0, sPath.length() - 6) + ".png";
		Texture2D* tex = CCTextureCache::getInstance()->addImage(sPng);
		CCLOG("loadResources texture end:%s", sPng.c_str());
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(sPath, tex);
		CCLOG("loadResources spriteframe end :%s", sPath.c_str());

	}
	else if (sPath.find(".png") != sPath.npos)
	{
		CCTextureCache::getInstance()->addImage(sPath);
		CCLOG("loadResources png end:%s", sPath.c_str());
		return;
	}
}


void ResManager::purgeResources(std::string& sPath)
{
	if (sPath.find(".plist") != sPath.npos)
	{
		std::string sPng = sPath.substr(0, sPath.length() - 6) + ".png";
		CCTextureCache::getInstance()->removeTextureForKey(sPng);
		CCLOG("unloadResources texture end:%s", sPng.c_str());
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(sPath);
		CCLOG("unloadResources spriteframe end :%s", sPath.c_str());
	}
	else if (sPath.find(".png") != sPath.npos)
	{
		CCTextureCache::getInstance()->removeTextureForKey(sPath);
		CCLOG("unloadResources spriteframe end :%s", sPath.c_str());
		return;
	}
}

static const int g_uLevel = 5;
static const int s_uSpriteFrames = 10;
static const int s_uSoldierNums = 100;
static const char* s_szSoldierPre = "soliderAtlas";

static const char* s_szAniName[] =
{
	"fla_baozi",
	"fla_zongzi",
	"fla_hanbao",
	"fla_dangao",
	"pvp_fla_baozi",
	"pvp_fla_zongzi",
	"pvp_fla_hanbao",
	"pvp_fla_dangao"
};

static const eCampType s_eCamp[] =
{
	ECAMP_TYPE_Player,
	ECAMP_TYPE_AI,
	ECAMP_TYPE_AI2,
	ECAMP_TYPE_AI3,
	ECAMP_TYPE_Player1,
	ECAMP_TYPE_Player2,
	ECAMP_TYPE_Player3,
	ECAMP_TYPE_Player4
};

int getFrameCount(eCampType campType, eSoldierType soldierType)
{
	switch (campType)
	{
	case ECAMP_TYPE_Neutral:
	case ECAMP_TYPE_Player:
	case ECAMP_TYPE_AI:
	case ECAMP_TYPE_AI2:
	case ECAMP_TYPE_AI3:
		if (soldierType == Soldier_Normal)
			return 10;
		else if (soldierType == Soldier_Speed)
			return 2;
		break;
	case ECAMP_TYPE_Player1:
	case ECAMP_TYPE_Player2:
	case ECAMP_TYPE_Player3:
	case ECAMP_TYPE_Player4:
		if (soldierType == Soldier_Normal)
			return 10;
		else if (soldierType == Soldier_Speed)
			return 2;
		break;
	case ECAMP_TYPE_Creeps:
		return 10;
		break;
	case ECAMP_TYPE_Invalid:
	default:
		return 0;
		break;
	}
}

bool ResManager::loadAnimation(std::string frameName, int frameCount, float playTime)
{
	Vector<SpriteFrame*> animFrames(frameCount < s_uSpriteFrames ? frameCount : s_uSpriteFrames);
	for (int i = 0; i < frameCount; i++)
	{
		int nameStart = i + 1;
		if (nameStart % 2 == 0 && frameCount > 2) continue;
		char sprite[60] = "\0";
		snprintf(sprite, 60, "%s_%02d.png", frameName.c_str(), nameStart);
		SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite);
		if (frame)
			animFrames.pushBack(frame);
	}
	//CCLOG("SelectWorldPage frameName  :%s", sName.c_str());
	auto animation = Animation::createWithSpriteFrames(animFrames, playTime);
	AnimationCache::getInstance()->addAnimation(animation, frameName);
	return true;
}

void ResManager::loadAsync()
{
	if (AnimationCache::getInstance()->getAnimation("jindutiao") == NULL)
	{
		int soldierTypeCnt = 2;
		for (int i = 0; i < g_uLevel; i++)
		{
			for (int j = 0; j < sizeof(s_szAniName) / sizeof(int *); j++)
			{
				char level[2] = "\0";
				snprintf(level, 2, "%01d", i);

				for (int k = 0; k < soldierTypeCnt; k++)
				{
					int frameCnt = getFrameCount(s_eCamp[j], (eSoldierType)k);
					if (frameCnt == 0)
						continue;
					std::string frameName = StringUtils::format("%s_%02d_%02d", sgTableManager()->getAnimationNameByCamp(s_eCamp[j]).c_str(), i, k);
					CCLOG("[trace] ResManager::loadAsync frameName = %s", frameName.c_str());
					this->loadAnimation(frameName, frameCnt, 0.07f);
				}
			}
		}

		this->loadAnimation("jindutiao", 22, 0.07f);
	}
}
