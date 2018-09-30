#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "GamePrecedure.h"


class SetDialog :public ActionWindow
{
public:
	SetDialog();;
	~SetDialog();;

	virtual bool init();
	virtual void onShow();

	void	onVoice();
	void	onMusic();

	CREATE_FUNC(SetDialog);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	ui::Button* mVoiceNo;
	ui::Button* mMusicNo;
	Sprite* mVoice_5;
	Sprite* mMusic_6;

	ui::Text*	mName;
	ui::Text*	mId;
#ifdef MGAME_PLATFORM_MPLUS
	void resetSDKBtnShow();
	ui::Button* mBtnExit;
	ui::Button* mBtnBind;
#endif

};
