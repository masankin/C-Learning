#include "UpdateScene.h"
#include "updateView.h"
#include "EnvSet.h"
#include "core/SoundManager.h"
#include "channel/ChannelApi.h"
#include "guide/GuideManager.h"
#include "core/ResManager.h"
UpdateScene::UpdateScene()
{
	m_pUpdateView = NULL;
}

UpdateScene::~UpdateScene()
{
}

bool UpdateScene::init()
{
	bool bRet = false;

	do {
		CC_BREAK_IF(!Scene::init());
		bRet = true;

		scheduleUpdate();
	} while (0);

	return bRet;
}

void UpdateScene::onEnter() 
{
	Scene::onEnter();
	Director::getInstance()->setAnimationInterval(1.0 / 60);

	m_pUpdateView = UpdateView::create("Update.csb");
	addChild(m_pUpdateView);

	UpdateMgrImp::getInstance()->startUpdate();

	sgGuideManager()->rollbackGuide();
	sgResManager()->loadResByPageType(MSG_FUC_UPDATE);
}

void UpdateScene::onExit() 
{
	Scene::onExit();
	Director::getInstance()->setAnimationInterval(1.0 / 30);
	removeChild(m_pUpdateView);
	sgResManager()->purgeResByPageType(MSG_FUC_UPDATE);
}

void UpdateScene::update(float dt)
{
	if (sgSoundManager()->getCanPlayMusic())
	{
		sgSoundManager()->playMusicBg(MUSIC_UI_UPDATE);
		unscheduleUpdate();
	}
}
