#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "net/proto/ClientSvrMsg.pb.h"
#include "ActLayout.h"

class ActLoginPanel :public ActionWindow
{
public:
	ActLoginPanel(void);
	~ActLoginPanel(void);

	CREATE_FUNC(ActLoginPanel);
	virtual bool init();
	void onEnter(); 
	void updateUI();
protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

private:
	ActLayout* mActLayout;
};

