/***************************************************
	File		: SetHeadDialog.h
	Description	: ¸ü»»Í·Ïñ
	Author		: Edward Chan
	Date		: 2015-10-07
	
	Copyright (C) 2015 - All Rights Reserved.
****************************************************/
#ifndef __MGAME_SETHEADDIALOG_H__
#define __MGAME_SETHEADDIALOG_H__

#include "ui/system/ActionWindow.h"
#include "MessageManager.h"

class SetHeadDialog : public ActionWindow, public MessageHandler
{
public:
	SetHeadDialog();
	~SetHeadDialog();

	CREATE_FUNC(SetHeadDialog);

	virtual bool init();

	virtual void onShow();

private:
	void onEnter();

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	virtual void onReceiveMassage(GameMessage * message);

	void setIconTouchEnable(bool isTouch);

	std::vector<Widget*> mButtonList;

	int tipsIndex[3];
};

#endif //__MGAME_SETHEADDIALOG_H__
