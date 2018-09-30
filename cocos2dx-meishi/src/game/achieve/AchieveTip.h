#pragma once
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "Constant.h"

class AchieveTip:public cocos2d::NodeRGBA
{
public:
	AchieveTip();
	~AchieveTip();

	virtual bool init();

	CREATE_FUNC(AchieveTip);

	void updateUI(std::list<int>);

	void updateUI(std::string icon,std::string strTitle,std::string strContent);

	void setShowType(ePageType type) { showType = type; }

private:
	void updateUI();

	void onGotoClickCallBack(Ref*);

	cocos2d::ui::Text* mTitleTxt;
	cocos2d::ui::Text* mContentTxt;
	cocos2d::ui::Text* mPointTxt;
	
	Sprite* mTastIcon;
	cocos2d::ui::ImageView* mPointImg;

	std::list<int> mShowList;

	ePageType showType;
};

