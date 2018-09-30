#pragma once

#include  "game/ui//system/GameScene.h"
#include "Message.h"
#include "game/data/TableManager.h"

class CheckPoint;
class SelectLevelPage :public GameScene, public MessageHandler
{
public:
	SelectLevelPage();
	~SelectLevelPage();

	virtual bool init();
	virtual void update(float delta);

	void onEnter();
	void onExit(); 

	
	virtual bool loadAsync(std::function<void()> loadCompleteCallback) override;
	static SelectLevelPage* create();

	virtual void onReceiveMassage(GameMessage * message);
	virtual void onShow();

	void RefreshBg(int level);
	void setLevelBg(int levelId);
	void initLevelInfo(int iLevel);

	void initLevelAllBg();
	void initMapLevelInfo();

	Node* getLevelBtn(int level);

	//设置星级显示
	void setStarNum(Node* parent,int num,bool isOpen,int level);

	//设置当前关卡星星数
	void setLevelStar(Node* parent, int level);

	int	getNextCheckPt(int pt);

	eCheckPointState getLevelState(int level);

	void powerTip();
	void powerFadeOut();
protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void onAddPowerCallBack(cocos2d::Ref*);
	void getGift(Ref *pSender,int type);
	void baozhiJump(Vec2 movePos, bool isGoto = false);

	void initScrollViewInfo();
	void onScrollViewCallBack(Ref*, ScrollviewEventType);
	void initPanelMove();
	void touchMovePanel(float offsetX);

	void resetGuideNode();
	void doGuideNode();

	int checkloadAllLevel();

	ui::ScrollView* mScrollViewLevel;
	Node* mNodeMapLevel;

	Size mInnerSize;

	Node* mNodeBg[5];
	
	ui::Text* mCoin;
	ui::Text* mPower;
	std::vector<int> mOpenList;

	ui::Text* mPowerTip;
	bool showTip;
	cocostudio::Armature*  mArmatureBaoZi;
	cocostudio::Armature*  mArmatureCurLevel;
	cocostudio::Armature*  mArmatureUnLock;
	int mUnLockLevel;
	int mLastPoint;
	std::map<int, Node* > mMapLevelBtns;
	
	int mCurMapLevel;
	CheckPointTable::CheckPointList::iterator mCheckPointCursor;

	CheckPointTable::CheckPointList mCheckPointList;
private:
	enum UPDATESTATE
	{
		UPDATE_INIT,
		UPDATE_ONENTER,
		UPDATE_COMPLETE,
	};

	UPDATESTATE mUpdateState;
	bool mbInitMove; 
	bool mbJumpOpen;
};
