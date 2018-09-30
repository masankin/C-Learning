
#pragma once

#include "cocos2d.h"
#include "game/ui//system/PageBase.h"

#include "Constant.h"


USING_NS_CC;using namespace cocos2d::ui;

class GameScene:public PageBase
{
public:
	static GameScene* CurGameScene;
	static GameScene* PerGameScene;

	static std::map<std::string, GameScene*>  CACHE_GAMESCENE;
public:

	GameScene();
	~GameScene();

	static GameScene* Create(std::string pageName, const ePageType pageType = MSG_FUC_BEGIN, const int luaType = 0);
	static GameScene* createWithName(std::string pageName, ePageType pageType, const int luaType);
	static GameScene* getGameSceneByName(std::string pageName);

	
	CC_SYNTHESIZE(ePageType, mPageType, PageType);

	virtual bool init();

	virtual void resetWindowManager();

	void setScene(Scene* scene);
	Scene* getScene();
	//TODO:loadAble
	virtual bool loadAsync(std::function<void()> loadCompleteCallback);
	virtual void loadSync();
	virtual void loadOver();
	virtual void unload();

	virtual void onShow();
	virtual void onClose();

	virtual void onEnter();
	virtual void onExit(); 
	//override
	virtual void addChild(Node* node)override;
	virtual void addChild(Node* node, int localZOrder)override;
	virtual void addChild(Node* node, int localZOrder, int tag)override;

	virtual Node* getChildByTag(int tag)const override;
	virtual Node* getChildByName(const std::string& name) const override;

	virtual ssize_t getChildrenCount()const override;
	
	virtual void removeChild(Node* node, bool cleanup = true) override;
	virtual void removeChildByTag(int tag, bool cleanup = true) override;
	virtual void removeChildByName(const std::string &name, bool cleanup = true) override;

	virtual void removeAllChildren()override;
	virtual void removeAllChildrenWithCleanup(bool cleanup)override;

	void onGameSceneEnter();
	void onGameSceneEnterTransitionDidFinish();
	void onGameSceneExitTransitionDidStart();
	void onGameSceneExit();

	CC_SYNTHESIZE(std::string, mPageName, PageName);
protected:

	void uiReset();
	virtual void cleanUp();
protected:

	Layer* layer_game;
	Layer* layer_ui;
private:
	Scene* pScene;
	CC_SYNTHESIZE(int, mLuaType, LuaType);
};
