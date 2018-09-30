#pragma once
#include  "game/ui/system/ActionWindow.h"
#include "game/Message.h"

class CrazyWeekDialog :public ActionWindow, MessageHandler
{
public:
	CrazyWeekDialog();
	virtual ~CrazyWeekDialog();

	virtual bool	init();
	virtual void	onShow();
	virtual void	onClose();
	void			updateCrzayInfo();
	CREATE_FUNC(CrazyWeekDialog);

	virtual void onReceiveMassage(GameMessage * message);
protected:
	virtual void onMenuItemAction(std::string cbName, cocos2d::Ref* sSender);
	void		 showItemInfo(Node* itemNode, std::string iconName, int count, int itemType = 0);
private:
	ui::Text* mTimeLabel;
	ui::Text* mCrazyPointLabel;
	ui::Text* mExtraHonourPointLabel;
	ui::Text* mConditionLabel;
	ui::Text* mSubConditionLabel;
	Sprite*	  mOkSprite1;
	Sprite*   mOkSprite2;
	Sprite*   mDoneSprite;
	Color4B mSelectColor;
	Color4B mNormalColor;
	Node* mItemNode[3];
};
