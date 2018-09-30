
#pragma once
#include "cocos2d.h"
#include "core/Singleton.h"
#include "Message.h"
#include "Constant.h"
#include "game/ui//SelectWorldPage.h"
#include "game/ui//ShopPage.h"
#include "game/ui//SelectLevelPage.h"
#include "game/ui//dialogs/LevelDialog.h"
#include "game/ui//GamePage.h"
#include "game/ui//dialogs/FailDialog.h"
#include "game/ui//dialogs/ResultDialog.h"
#include "game/ui//dialogs/PauseDialog.h"
#include "game/ui//dialogs/GenerlDialog.h"
#include "game/ui/dialogs/ShopDialog.h"
#include "game/ui/dialogs/SetDialog.h"
#include "game/ui/dialogs/PvpDialog.h"
#include "game/ui/dialogs/RankingDialog.h"
#include "game/ui//system/GameScene.h"
#include "game/ui//system/Window.h"	
#include "game/achieve/AchievePanel.h"
#include "game/winner/WinnerDialog.h"
#include "game/ui/dialogs/ProDialog.h"
#include "game/ui/dialogs/SetNameDialog.h"
#include "game/ui/dialogs/PVPResult.h"
#include "game/ui/dialogs/PowerDialog.h"
#include "game/ui//system/ActionGain.h"



class PageFunction :public Singleton<PageFunction>, public MessageHandler
{
public:

	static Scene* mScen;

	PageFunction();
	~PageFunction();

	virtual void onReceiveMassage(GameMessage * message);

	virtual void gotoFunction(const ePageType massageType, const std::string messageName = "", const int mesType = 0, 
							bool isCacheRes = false);

	void showPage(Scene* pScene);
	 
	void setDiamondPanel(bool _visible, GameScene* nowScene = NULL);
	Node* getDiamondPanel();
	ActionGain* getActionPowerLayer();
	ActionGain* getActionDiamondLayer();
	ActionGain* getActionPackageLayer();
	ActionGain* getActionChestLayer();
	void enableOptionButton(bool visible);

	//返回一个已存在的Window.
	Window* getWindowByPageName(std::string pageName);
	//返回一个已存在的Window,不存在再去创建
	Window* createWindow(const ePageType massageType, const std::string messageName = "", const int luaType = 0,
		bool isCacheRes = false);

	void logout();

protected:
	bool checkMassage(const ePageType massageType);
	bool isSence(const ePageType massageType);
	void showWaitPage();

	Node* mDiamondPanel;

	Node* mNetworkTips;

	//获取和消耗体力动画层
	ActionGain* mActionPowerLayer;
	ActionGain* mActionDiamondLayer;
	ActionGain* mActionPackageLayer;
	ActionGain* mActionChestLayer;

	//存在的意义只为一个设置按钮的垃圾交互造成, 需整体优化ui的框架才会有意义
	CC_SYNTHESIZE(int, mCurrentUIType, CurrentUIType);

	bool mIsLoadSceneRes;//是否在加载场景资源中
	std::vector<std::string> mWaitShowPageVec;
	bool mIsRePlay;
	ePageType mCurrentLoadingPage;//当前正在加载的Scene
};

#define sgPageFunction() PageFunction::Get()
