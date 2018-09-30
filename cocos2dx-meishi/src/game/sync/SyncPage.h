#ifndef __SYNCPAGE_H__
#define __SYNCPAGE_H__  
#include "Message.h"
#include "ui/system/GameScene.h"

USING_NS_CC;
class SyncPage: public GameScene, public MessageHandler
{
public: 
	SyncPage();
	~SyncPage();
	CREATE_FUNC(SyncPage);
	void onReceiveMassage(GameMessage * message);
	bool init();
	void onEnter();
	void onExit();
	//void update(float dt);
private:
	void onCanelSaveData();
	void gotoMainPage();
	void doLoginOverTime(); 
	void doSyncSaveData();


	void initUI();
	void initGettingDataDialog();
	void onCancelGetDataCallback();
	void showGettingDataDialog(bool show);

	void initGetDataFiledDialog(); 
	void onContinueGetDataCallback();
	void showGetDataFiledDialog(bool show);


	void initLoadingAnimation();
	void playLoadingAnimation(bool play);



	CC_SYNTHESIZE(Node*, mRoot, UIRoot);
	Node* mGetDataNode;
	Node* mGetDataFailedNode;
	Node* mBaseDescNode;

	ui::Text* mContentLabel;
	cocos2d::Sprite* mJuhuaSprite;
};

 
#endif
