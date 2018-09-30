#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "Message.h"
#include "BaseProxy.h"

USING_NS_CC_EXT;
USING_NS_CC;

class PVPResult :public ActionWindow, MessageHandler
{
public:
	PVPResult();                                           
	~PVPResult();

	virtual bool init();

	void onReceiveMassage(GameMessage * message);

	CREATE_FUNC(PVPResult);

	virtual void onShow();
	virtual void onClose();

	void updateUI();
	void clearActionData();
public:
	static Vec2 mShowTipsPos;
protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
#ifdef MGAME_PLATFORM_MPLUS
	void autoShowAd(float dt);
#endif
private:
	ui::Layout* mPanelView;
	Node* mNodeResultAni;
	Sprite* mSpriteWinBg;
	Sprite* mSpriteFailBg;
	ui::Text* mTextStreak;
	ui::Text* mTextWinRate;
	ui::Text* mTextTips;
	bool mbResultWin;
	ui::Text* mTextLess10;
	Button * mShareButton;
public:
	enum PVPRESULT
	{
		PVPRESULT_WIN,
		PVPRESULT_FAIL,
		PVPRESULT_DRAW,
	};
	enum PVPACTIONTAG
	{
		PVPACTIONTAG_TEXT_BEGIN = 10000,
		PVPACTIONTAG_ARROW_BEGIN = 100000,
	};
};



class PVPResultCell : public Node
{
public:
	PVPResultCell();
	~PVPResultCell();
	bool init();
	void initUI();
	void updateCell(int idx);
	void showTips();
	CREATE_FUNC(PVPResultCell);
private:
	Node* mRoot;
	Sprite* mSpriteSelfWinBg;
	Sprite* mSpriteSelfFailBg;
	Sprite* mSpriteDownFlag;
	Sprite* mSpriteUpFlag;
	Sprite* mSpriteZongJie;
	ui::Text* mTextStreak;
	Node* mNodeGlory;
	float mNodeGloryPosY;
	Node* mNodeScore;
	float mNodeScorePosY;

	ui::Text* mTextGloryName;
	ui::Text* mTextGlory;

	ui::Text* mTextScoreName;
	ui::Text* mTextScore;

	ui::Text* mTextRank;
	ui::Text* mTextName;
	Sprite* mSpriteFangZhu;
	Node* mNodeHead;
	Node* mNodeRankSection;
	ui::Text* mTextActLabel;
	Node *mVipNode;
	Node *mVipIcon;
	Text * mVipHonor;
	Sprite* mSpriteSection;
	int mPrevSession;
	int mCurSession;
	std::map<std::string, Point> mvTextTips;
};
