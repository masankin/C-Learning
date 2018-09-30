#ifndef __MGAME_VIPTIMEDIALOG_H__
#define __MGAME_VIPTIMEDIALOG_H__

#include  "game/ui/system/Window.h"
#include "MessageManager.h"

class VipTimeDialog : public Window, MessageHandler
{
public:
	VipTimeDialog();
	~VipTimeDialog();
	void onReceiveMassage(GameMessage * message);

	CREATE_FUNC(VipTimeDialog);

	virtual bool init();

	virtual void onShow();

private:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void refreshContent(int id);

	Node *mNode1;
	Node *mNode2;
	Node *mNode3;

	Text *mVip1Price;
	Text *mVip2Price1;
	Text *mVip2Price2;
	Text *mVip3Price1;
	Text *mVip3Price2;

	Text *mVipName;

};


#endif //__MGAME_VIPTIMEDIALOG_H__
