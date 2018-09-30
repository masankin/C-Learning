#pragma once
#include "core/StringLabel.h"
#include "ui/system/ActionWindow.h"
#include "extensions/cocos-ext.h"
#include "Message.h"

class StringLabel;
class ChattingButton : public Node,public MessageHandler
{	
public:
	ChattingButton();
	~ChattingButton();
	static ChattingButton* create();

	virtual bool init();
	virtual void onReceiveMassage(GameMessage * message);
	void	resetUI(bool flag);
protected:
	ImageView* mImageBg;
	StringLabel *m_pLabel;
	void chatCallback(cocos2d::Ref*);
	void changeChatMsg();
	bool mShow;

};

class ChatInfo;
class ChattingCell : public Node
{	
public:
	static ChattingCell* create(ChatInfo* info);
	virtual bool init(ChatInfo* info);
	virtual void update(float delta);

	// add vip icon, add by jim
	virtual void addVipSymbol(Node* nameNode,int vipLevel, bool isMe = false);
protected:
	void inviteCallback(cocos2d::Ref*,int uin);
	void popCharaterInfo(Ref* pSender,int uin);
	void acceptInviteCallback(cocos2d::Ref*);
	void onClickBuyEngerCallBack(cocos2d::Ref*);
	ui::Text *mText;
	int mInviteRoomId;	
	std::string mServerNode;
	ImageView* mImage;
};


class ChattingDialog : public Window
{ 
public:
    ChattingDialog();
	~ChattingDialog();
	virtual bool init();
	CREATE_FUNC(ChattingDialog);
	virtual void onShow();
	virtual void onClose();
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	
    void addMessage();

    void loadChatHistory();
	
	void TextFieldCallback(Ref* pSender, TextFiledEventType type);
    
	void updateChating(ui::ScrollView*);

	void moveCallBack();

	void moveShow(bool show);

	void setChannel(bool all);

	void updateTime(float dt);

	void callBack(cocos2d::Ref*);

	void sendComplete(); 

	void startUpdateTime();
protected:
	ui::TextField* mTextField;
	ui::ScrollView* mScrollView_1;
	ui::ScrollView* mScrollView_2;
	ui::Text* mTextCount;

	ui::Button* mBtnAll;
	ui::Button* mBtnChallage;
	ui::Text* mTextLevelLimit;
	int mInviteRoomId;

	ui::Text* mTextFree;
	ui::Text* mTextGap;
	Sprite* mSpriteGold;
};


class ChattingTips :public ActionWindow
{
public:
	ChattingTips();
	~ChattingTips();

	static void hide();
	static void showChattingTips(const std::function<void(Ref*)>&);
	virtual bool init();

	CREATE_FUNC(ChattingTips);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	ui::Widget* mSure;
	ui::Widget* mCancel;
};
