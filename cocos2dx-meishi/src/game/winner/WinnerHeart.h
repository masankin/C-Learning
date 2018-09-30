#pragma once

#include "game/winner/WinnerProxy.h"
#include "game/ui/system/ActionWindow.h"

class WinnerHeart :public Node
{
public:

	WinnerHeart();
	~WinnerHeart();

	virtual bool init();

	CREATE_FUNC(WinnerHeart);

	virtual void update(float delta);

protected:
	void jewelCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
	void onMenuItemAction(cocos2d::Ref*);
	Node* mRoot;
private:
	ui::Text* mDayNum_1;
	ui::Text* mWinGoal;
	Button* mButton_4;
	ImageView* mImageBtn[7];
	LoadingBar* mLoadingBar_1;
	Layout* mPanel_1;

};
