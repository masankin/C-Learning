#pragma once
#include  "game/ui/system/ActionWindow.h"
class UpRankSessionDialog :public Window
{
public:
	UpRankSessionDialog(void);
	~UpRankSessionDialog(void);

	virtual bool init();
	CREATE_FUNC(UpRankSessionDialog);

	static  UpRankSessionDialog* showUpRankSessionDialog(int iPrevSession, int iCurSession);
	void updateUI();

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void playUpgradeAni();
	void setFirstTipsShow(bool bShow);
	CC_SYNTHESIZE(int, mPrevRankSession, PrevRankSession);
	CC_SYNTHESIZE(int, mCurRankSession, CurRankSession);

private:
	cocostudio::Armature* mArmItem;
	cocostudio::Armature* mArmAdditive;
	Node* mNodeDes;
	ui::Text* mTextTips;
};

