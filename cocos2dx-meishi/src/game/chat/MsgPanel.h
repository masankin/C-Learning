#pragma once

#include "Constant.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ui/UIHelper.h"
#include "cocos2d.h"
#include "chat/ChattingControl.h"

USING_NS_CC;
using namespace ui;

enum eChatType
{
	Room_Chat,
	Fight_Chat,
	Pvp_Chat = 5,
};
class MsgPanel : public Layer
{	
public:
	MsgPanel();
	~MsgPanel();
	virtual bool onTouchBegan(Touch *touch, Event *unused_event){ return true;}
	virtual void onTouchEnded(Touch *touch, Event *unused_event);

	static MsgPanel* create(int chatType = Room_Chat);

	virtual bool init(int chatType);

	void onMenuClicked(cocos2d::Ref*,int id);

protected:
	ui::ImageView* mImageBg;
	int mChatType;
	ui::Text* mMsgText[4];
};
