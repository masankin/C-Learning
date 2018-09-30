#include "LogoScene.h"
#include "cocos2d.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "game/ui/PageFunction.h"
#include "channel/ChannelApi.h"
#include "net/UserObj.h"
#include "EnvSet.h"	
#include "guide/GuideManager.h"
#include "notice/NoticeControl.h"
#include "GameJniHelper.h"
#include "core/ResManager.h"

#ifdef MGAME_PLATFORM_MPLUS
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "login/VideoLayer.h"
#endif
#endif

USING_NS_CC;using namespace cocos2d::ui;

LogoScene::LogoScene()
{

}

LogoScene::~LogoScene()
{

}

//增加反编译难度用的函数
int dvefdsfa()
{
	int a = 1;
	a += 1;
	return 1;
}

//增加反编译难度用的函数
int vbhbvgr()
{
	int b = 1;
	b += 554;
	return 2;
}
bool LogoScene::init() {

	bool bRet = false;

	do {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

		if(sgEnvSet("MD5_CHECK_COLSE") == 0)
		{
			CCLOG("[trace]LogoScene::init check md5 tag 1");
			std::string str = GameJniHelper::getInstance()->getCode();

			if (dvefdsfa() == vbhbvgr() || str != "973928471b03e32603ca901fad1922e4" && vbhbvgr() == vbhbvgr())
			{
				CCLOG("[trace]LogoScene::init check md5 tag 2");
				Director::getInstance()->end();
			}
			CCLOG("[trace]LogoScene::init check md5 tag 3");
		}

#endif
		CC_BREAK_IF(!Scene::init());

		mRoot = CSLoader::getInstance()->createNode("ui/LogoPage.csb");

		CC_BREAK_IF(!mRoot);

		this->addChild(mRoot);

		playLogo();

		bRet = true;

	} while(0);
    return bRet;
}

void LogoScene::playLogo()
{
	Node* logoBg = mRoot->getChildByName("Logo");

	float scale = logoBg->getScale();

	float startScale = scale * 0.8f;

	logoBg->setScale(startScale);

	ActionInterval* action = Sequence::create(
		ScaleTo::create(0.2,scale),
		DelayTime::create(1.3),
		CallFuncN::create(this, callfuncN_selector(LogoScene::playFinish)),NULL);
	logoBg->runAction(action);

}
void LogoScene::onEnter()
{
	Scene::onEnter();
	sgResManager()->loadResByPageType(MSG_FUC_LOGO);
}

void LogoScene::onExit()
{
	Scene::onExit();
	sgResManager()->purgeResByPageType(MSG_FUC_LOGO);
}

void LogoScene::playFinish(Node* node)
{

	sgTableManager()->loadGuideTable();

//	sgChannelApi()->callLogin();update资源之后才去登陆
	 
    //pre load resources //兼容vs2012 不传入nullptr
    //sgResourceManager()->loadResources([](const std::string&, float){});
    //公告
	sgNoticeControl()->requestNotice(REQUEST_ACTINFORM);

	PageFunction::Get()->gotoFunction(MSG_FUC_LOGIN);
}
