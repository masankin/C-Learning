#ifndef __MGAME_VIPDIALOG_H__
#define __MGAME_VIPDIALOG_H__

#include  "game/ui/system/ActionWindow.h"
#include "MessageManager.h"
#include "vip/VipControl.h"
class VipDialog : public ActionWindow,  MessageHandler
{
public:
	VipDialog();
	~VipDialog();

	CREATE_FUNC(VipDialog);

	void onReceiveMassage(GameMessage * message);
	virtual bool init();

	virtual void onShow();
	void UpdateUI(int tag);

private:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	void refreshBtn(int tag);
	void refreshContent(int id);
	void refreshBuyNode(int id);
	void buyVip(Ref* sendr, sVipType vipType, int buyID, int selectID);
	void refreshVIPTime(int id);


	Text * mName;
	ui::ScrollView * mContent;
	Text * mTime;

	Node * mBuy1Node;
	Node * mBuy2Node;
	Sprite * mSprite_Icon;
	Text   *mText1_price;
	Sprite *mSprite_bg;
	Sprite *mSprite_title;
	Sprite *mSprite2_gouhao1;
	Sprite *mSprite2_gouhao2;
	Sprite *mSprite2_price;
	Text   *mText2_price;
	
	Vector<Button*>mSelectBtnVec;

	bool mPlayTag;
};


#endif //__MGAME_VIPDIALOG_H__
