#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "ActLayout.h"
#include "ActItem.h"
#include "net/proto/ClientSvrMsg.pb.h"
#include "Message.h"

class ActPanel :public ActionWindow, public MessageHandler
{
public:
	ActPanel();
	~ActPanel();
	CREATE_FUNC(ActPanel);
	void onEnter();
	void onExit(); 
	virtual bool init();
	virtual void setShow(bool b);
	void updateUI(int type);
	
	void setActPanelBtn();
protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

private:
	ActLayout* createActLayout(ACTTYPE);
	void onBtnListViewListener(Ref*,ListViewEventType);
	void onBtnListViewScrolListener(Ref*, ScrollviewEventType);
	void setTypeIdx(int);
	void updateContent();
	ActLayout* getActLayout(ACTTYPE);
	void onReceiveMassage(GameMessage * message);

private:
	Node* mActContent;
	std::map<int,ActItem*> mActItemMap;
	std::map<int ,ActLayout*> mActLayerMap;
	int mSelectBtn;
	Node* mGuideNode;
	Node* mGuideNode2;
	bool mHasReceive;

	ui::Button* mSiginFlagLeft;
	ui::Button* mSiginFlagRight;

};

