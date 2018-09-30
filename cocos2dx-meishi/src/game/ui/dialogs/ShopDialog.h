#pragma once

#include  "game/ui/system/ActionWindow.h"
#include "GamePrecedure.h"
#include "game/data/Tables.h"
#include "Message.h"

class ShopDetailed :public Window, public MessageHandler
{
public:
	ShopDetailed();;
	~ShopDetailed();;

	void onReceiveMassage(GameMessage * message);


	virtual bool init();
	
	virtual void onShow();
	virtual void onClose();

	void onEnter();
	void onExit();

	static  void show(int propID);

	CREATE_FUNC(ShopDetailed);
	
	void onSelectBright(int index);

	ui::Button* m_btn1;
	ui::Button* m_btn2;
	ui::Button* m_btn3;

	ui::Text* m_text1;
	ui::Text* m_text2;
	ui::Text* m_text3;
	ui::Text* mDescribeTxt;
	ui::Text* mItemNameTxt;
	ui::Text* m_money;
	ui::Text* mOpTxt;
	Node* m_Icon;
	ui::Text* mNumberTxt;

	Node *mVipNode;
	Sprite* mVipIcon;
	Text *mVipPoint;
	Text *mVipPrice;
	static int m_ID;
	static int m_Index;

	static void setPropId(int id);
	static void setIndex(int id);

	void refresh(const ItemBuyInfo* pInfo); 
	void doFinishGuide();

protected:
	void initGuideNode();
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void onCallBackshowShop(Ref*);
};
