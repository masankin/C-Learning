#pragma once

#include "cocos2d.h"
#include "game/Message.h"
#include "cocostudio/CocoStudio.h"
#include "ui/system/GameScene.h"

class LoginScene :public GameScene, public MessageHandler
{
public:
	LoginScene(void);
	~LoginScene(void);

	virtual bool init();
	CREATE_FUNC(LoginScene);

	void onReceiveMassage(GameMessage * message) ;

	virtual void onShow();
	virtual void onClose();
private:
	void updateLogin(float dt);
	void buttonClickCallBack(cocos2d::Ref*);

	void setLoginBtnEnable(bool enable);

	void playLoginAction();
	void selectSrvShow(cocos2d::Ref*);

	void cancelLogin();
	void update(float dt);

	cocos2d::Sprite* mJuhuaSprite;
	cocos2d::ui::Text* mTextTip;
	cocos2d::ui::Text* mVersionTag;
	ui::Layout* mPanelSelectSrv;
	ui::Text* mSrvNameTTF;
	int m_iLoginType;

	cocos2d::ui::Button* mLoginBtnWX;
	cocos2d::ui::Button* mLoginBtnQQ;
	cocos2d::ui::Button* mLoginBtnYK;
	cocos2d::ui::Button* mLoginBtn;

	cocos2d::EventListenerCustom* m_pCancelLoginListener;
};

