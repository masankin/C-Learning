
#include "game/ui//system/GameScene.h"
#include "game/ui//system/Window.h"
#include "game/ui//PageFunction.h"
#include "game/sync/SyncPage.h"
#include "login/LoginScene.h"

#include "ResManager.h"

std::map<std::string, GameScene*> GameScene::CACHE_GAMESCENE = *(new std::map<std::string, GameScene*>());

GameScene* GameScene::CurGameScene = nullptr;
GameScene* GameScene::PerGameScene = nullptr;


GameScene::GameScene() :pScene(0), mLuaType(0), mPageName("")
{
	layer_game = Layer::create();
	layer_ui = Layer::create();
	PageBase::addChild(layer_game);
	PageBase::addChild(layer_ui);
	this->uiReset();
}

GameScene::~GameScene()
{

}

GameScene* GameScene::Create(std::string pageName, ePageType mPageType, const int luaType)
{
	GameScene* gameScene = nullptr;
	std::map<std::string, GameScene*>::iterator it = CACHE_GAMESCENE.find(pageName);
	if (it != CACHE_GAMESCENE.end())
	{
		//sgResManager()->loadCommonRes();
		sgResManager()->loadResByPageType(mPageType);
		gameScene = it->second;
	}
	else
	{
		GameScene* gameWindow = createWithName(pageName, mPageType, luaType);
		if (gameWindow)
		{
			CACHE_GAMESCENE.insert(std::make_pair(pageName, gameWindow));
			gameWindow->setPageName(pageName);
			//gameWindow->retain();
			gameScene = gameWindow;
			Scene *scene = Scene::create();
			gameScene->setScene(scene);
		}
	}

	return gameScene;
}

bool GameScene::init()
{
	if (!PageBase::init())
	{
		return false;
	}

	return true;
}

void GameScene::resetWindowManager()
{
	Window::LAYER_UIMANAGER_LAYER->removeFromParentAndCleanup(false);
	layer_ui->addChild(Window::LAYER_UIMANAGER_LAYER);
	bool canAddToThis = this->_ID != layer_ui->_ID;
	if (canAddToThis)
	{
		this->uiReset();
	}
}

void GameScene::setScene(Scene* scene)
{
	if (pScene)
		pScene->release();

	pScene = scene;
	pScene->retain();
	scene->addChild(this);
	pScene->setUserData(this);

	pScene->setOnEnterCallback(std::bind(&GameScene::onGameSceneEnter, this));
	pScene->setonEnterTransitionDidFinishCallback(std::bind(&GameScene::onGameSceneEnterTransitionDidFinish, this));
	pScene->setonExitTransitionDidStartCallback(std::bind(&GameScene::onGameSceneExitTransitionDidStart, this));
	pScene->setOnExitCallback(std::bind(&GameScene::onGameSceneExit, this));
}

Scene* GameScene::getScene()
{
	return pScene;
}

bool GameScene::loadAsync(std::function<void()> loadCompleteCallback)
{
	sgResManager()->loadResByPageType(mPageType);
	if (loadCompleteCallback)
	{
		loadCompleteCallback();
	}
	return true;
}

void GameScene::loadSync()
{
	sgResManager()->loadResByPageType(mPageType);
}

void GameScene::loadOver()
{

}

void GameScene::unload()
{
}

void GameScene::onShow()
{

}

void GameScene::onClose()
{
	cleanUp();
}

void GameScene::onEnter()
{
	PageBase::onEnter();
}

void GameScene::onExit()
{
	PageBase::onExit();
}

void GameScene::addChild(Node* node)
{
	this->layer_game->addChild(node);

	this->uiReset();
}

void GameScene::addChild(Node* node, int localZOrder)
{
	this->layer_game->addChild(node, localZOrder);

	this->uiReset();
}

void GameScene::addChild(Node* node, int localZOrder, int tag)
{
	this->layer_game->addChild(node, localZOrder, tag);

	this->uiReset();
}

Node* GameScene::getChildByTag(int tag)const
{
	return this->layer_game->getChildByTag(tag);
}

Node* GameScene::getChildByName(const std::string& name)const
{
	return this->layer_game->getChildByName(name);
}

ssize_t GameScene::getChildrenCount()const
{
	return this->layer_game->getChildrenCount();
}

void GameScene::removeChild(Node* node, bool cleanup /* = true */)
{
	this->layer_game->removeChild(node, cleanup);
}

void GameScene::removeChildByTag(int tag, bool cleanup /* = true */)
{
	this->layer_game->removeChildByTag(tag, cleanup);
}

void GameScene::removeChildByName(const std::string &name, bool cleanup /* = true */)
{
	this->layer_game->removeChildByName(name, cleanup);
}

void GameScene::removeAllChildren()
{
	this->layer_game->removeAllChildren();
}

void GameScene::removeAllChildrenWithCleanup(bool cleanup)
{
	this->layer_game->removeAllChildrenWithCleanup(cleanup);
}

void GameScene::uiReset()
{
	if (layer_ui && layer_game)
	{
		layer_game->setZOrder(0);
		layer_ui->setZOrder(1);
	}

}

GameScene* GameScene::createWithName(std::string pageName, ePageType pageType, const int luaType)
{
	GameScene* pPage = nullptr;

	if (pageName == "SelectLevelPage.csb")
	{
		pPage = SelectLevelPage::create();
	}
	else if (pageName == "SelectWorldPage.csb")
	{
		pPage = SelectWorldPage::create();
	}
	else if (pageName == "GamePage.csb")
	{
		pPage = GamePage::create();
	}
	else if(pageName == "SyncPage.csb")
	{
		pPage = SyncPage::create();
	}
	else if (pageName == "Update.csb")
	{
		pPage = LoginScene::create();
	}
	if (pPage != nullptr){
		pPage->setLuaType(luaType);
		pPage->setPageType(pageType);
	}
	return pPage;
}

void GameScene::onGameSceneEnter()
{
// 	if (this->loadAsync())
// 	{
// 		this->resetWindowManager();
// 	}
	//if (mPageType != MSG_FUC_LOGIN)
	//{
	//	sgResManager()->loadCommonRes();
	//}

}

void GameScene::onGameSceneEnterTransitionDidFinish()
{
	this->loadOver();
}

void GameScene::onGameSceneExitTransitionDidStart()
{

}

void GameScene::onGameSceneExit()
{
	//sgResManager()->purgeResByPageType(mPageType);
}

void GameScene::cleanUp()
{
	std::map<std::string, GameScene*>::iterator it = CACHE_GAMESCENE.find(getPageName());
	if (it != CACHE_GAMESCENE.end())
	{
		CACHE_GAMESCENE.erase(it);
	}
	/*if (getPageName() == "SelectWorldPage.csb")
	{
		it = CACHE_GAMESCENE.find("Update.csb");
		if (it != CACHE_GAMESCENE.end())
		{
			CACHE_GAMESCENE.erase(it);
		}
	}*/
	CC_SAFE_RELEASE_NULL(pScene);
}

GameScene* GameScene::getGameSceneByName(std::string pageName)
{
	std::map<std::string, GameScene*>::iterator it = CACHE_GAMESCENE.find(pageName);
	if (it != CACHE_GAMESCENE.end())
	{
		return it->second;
	}
	return NULL;
}
