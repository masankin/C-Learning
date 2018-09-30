#pragma once
#include "ui/system/ActionWindow.h"
#include "data/MailData.h"
#include "core/StringLabel.h"

class MailPanel :public ActionWindow
{
public:
	MailPanel(void);
	~MailPanel(void);

	CREATE_FUNC(MailPanel);
	virtual bool init();

	void updateInfo();
	void delCell(std::vector<int>);

	virtual void onShow();
private:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	void onItemClickCallBack(Ref*);



	void updateUI();

	void updateConten(const fgame::DataMailInfo&);

	void onScrollviewEvent(Ref*, ScrollviewEventType);
	void onItemTouch(Ref*, Widget::TouchEventType);

	void scheduleUpdateListView(float dt);

	cocos2d::ui::ListView* mMailListView;

	cocos2d::ui::Button* mOpenBtn;

	std::vector<int> mDelIdVec;

	std::map<int,int> mAwardMap;

	cocos2d::ui::Text* mTitleTxt;
	cocos2d::ui::Text* mContentTxt;
	StringLabel* mStringLabel;
	std::vector<cocos2d::Node*> mAwardNodeVec;

	cocos2d::Node* mContentNode;
	std::vector<ui::Widget*> mFlagImageVec;

	int mNowJumpPercent;
	bool mIsCanAutoFresh;
};

