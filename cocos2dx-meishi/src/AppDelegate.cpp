#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "GamePrecedure.h"
#include "basic/Multiresolution.h"
#include "core/HelperFunc.h"
#include "battle/BattleControl.h"
#include "game/ui/PageFunction.h"
#include "data/ItemManager.h"
#include "guide/GuideManager.h"
#include "statistics/Statistics.h"
#include "game/utils/Comm.h"
#include "battle/JungleBattleManager.h"
#include "battle/MoraleManager.h"
#include "channel/ChannelApi.h"
#include "game/sync/SyncControl.h"
#include "buff/BuffManager.h"
#include "update/UpdateMgr.h"
#include "game/net/UserObj.h"
#include <functional>
#include "ResManager.h"
#include "login/VideoManager.h"
#include "login/LoginProxy.h"

#include "lua/manual/lua_mgame_manual.h"
#include "CCLuaEngine.h"
#include "lua_module_register.h"
#include "login/LoginProxy.h"
#include "lua/LuaFuncHelper.h"

USING_NS_CC;



AppDelegate::AppDelegate() {
//--- masankin
//	FileUtils::setDecodeFunc(std::bind(&decodeXXTEA
//		, std::placeholders::_1
//		, std::placeholders::_2
//		, std::placeholders::_3
//		, std::placeholders::_4));

	sgChannelApi()->init();
	
	if (Window::LAYER_UIMANAGER_LAYER == nullptr)
	{
		Window::LAYER_UIMANAGER_LAYER = Widget::create();
		Window::LAYER_UIMANAGER_LAYER->retain();
	}

	Layer* pLayerDlg = Layer::create();
	Director::getInstance()->setNotificationNode(pLayerDlg);
	pLayerDlg->setLocalZOrder(1000);
	pLayerDlg->onEnter();

	
}

AppDelegate::~AppDelegate() 
{
	//GuideManager::cleanGuideManager();
	JungleBattleManager::cleanJungleBattleManager();
	sgMoraleManager()->cleanMoraleManager();
	CCLOG("[trace]AppDelegate::~AppDelegate() save to file");
	sgUserObj()->saveToFile();
	if (Window::LAYER_UIMANAGER_LAYER != nullptr)
	{
		Window::LAYER_UIMANAGER_LAYER->removeFromParent();
		Window::LAYER_UIMANAGER_LAYER->release();
	}	
	
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}


// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages(lua_State* L)
{
	register_all_mgame_manual(L);
	register_all_mgame(L);

	sgLuaFuncHandlerManager()->bindModule(LuaFuncHandlerManager::ModuleType::kCustom, Director::getInstance());

    return 0; //flag for packages manager
}


bool AppDelegate::applicationDidFinishLaunching() 
{
	LuaEngine* engine = LuaEngine::getInstance();
	ScriptEngineManager::getInstance()->setScriptEngine(engine);
	lua_State* L = engine->getLuaStack()->getLuaState();
	lua_module_register(L);
	register_all_packages(L);

	sgResManager()->syncLoadLua();
	engine->executeString("require 'main'");

	return true;

	/*sgResManager()->setResourcePath();
	sgResManager()->setUpdateResPath();
	CCLOG("=========heheheheheh");
	// 特殊类型表格加载
	sgTableManager()->initLoadTables();

	LuaEngine* engine = LuaEngine::getInstance();
	ScriptEngineManager::getInstance()->setScriptEngine(engine);
	lua_State* L = engine->getLuaStack()->getLuaState();
	lua_module_register(L);
	register_all_packages(L);

	sgResManager()->syncLoadLua();
	engine->executeString("require 'main'");

	sgUserObj()->initUserObj();
	sgStatistic()->init();
	
	
	sgSyncControl()->init();



	auto director = Director::getInstance();

	// turn on display FPS
	if (sgEnvSet("MGAME_SHOW_FPS"))
		director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / 30);


	Multiresolution::setDesignResolutionSize(1280, 720);

	sgGamePrecedure()->init();
	//ItemManager::Get()->init(0);

	
	//PageFunction::Get()->gotoFunction(MSG_FUC_LOGO);
	
	
	
	//////TODO: 
	//sgGamePrecedure()->enterGame();

// 	auto scene = HelloWorld::createScene();
// 	if (scene)
// 		Director::getInstance()->runWithScene(scene);*/

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
	//游戏进入后台时，最后存盘刷新

	if (sgGamePrecedure()->getIncheckPoint() && !sgBattleControl()->getIsPvp() && sgGamePrecedure()->getEnterGame())
	{
		
		if(sgGuideManager()->getIsRunning() == false && sgGuideManager()->getGuideLock() == false)
		{
			if (sgGamePrecedure()->getGameIng() == true)
			{
				sgGamePrecedure()->setGameIng(false);
			}
			PageFunction::Get()->gotoFunction(MSG_FUC_WIN_PAUSE);
		}   
	}
	sgVideoManager()->setVideoState(VideoManager::VIDEO_COMPLETED);	//进入后台是，结束视频
	sgVideoManager()->removeVideoLayer();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	sgLoginProxy()->isLogin();
#endif

}
