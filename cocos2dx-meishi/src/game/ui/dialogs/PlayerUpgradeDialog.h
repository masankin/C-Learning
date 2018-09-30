#ifndef __MGAME_UPGRDEDIALOG_H__
#define __MGAME_UPGRDEDIALOG_H__

#include  "game/ui/system/ActionWindow.h"
#include "MessageManager.h"
class PlayerUpgradeDialog : public ActionWindow
{
public:
	PlayerUpgradeDialog();
	~PlayerUpgradeDialog();

	CREATE_FUNC(PlayerUpgradeDialog);

	virtual bool init();

	virtual void onShow();
	virtual void onClose();
	//virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void closeCallBack();
private:
	TextBMFont *mNodeLevel;
};


#endif //__MGAME_UPGRDEDIALOG_H__
