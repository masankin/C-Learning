#pragma once

#include  "game/ui//system/GameScene.h"
#include "Message.h"
#include "core/TimeCalculator.h"
#include "buff/BuffView.h"
#include "chat/MsgPanel.h"
#include "ui/LittleTips.h"

class MapWorldLayer;
class CheckPoint;
class PropItem;
class GameReady;
class MsgPanel;
class GamePage :public GameScene, public MessageHandler
{
public:
	GamePage();
	~GamePage();

	virtual bool init();
	void onEnter(); 
	void onExit();
	CREATE_FUNC(GamePage);

	virtual bool loadAsync(std::function<void()> loadCompleteCallback);
	virtual void loadOver();
	virtual void unload();

	void	resetGameUi();
	void	resetStarStr();
	//@param right 是否完成
	void	changeStarStr(int tag, bool right = false);

	void	initResModeScoreUI();

	void	updateResModeScore();

	//void  updateTimer(float dt);

	virtual void onReceiveMassage(GameMessage * message);

	virtual void onShow();
protected:
	void showSoldierControl(bool show);
	void showNoBatControl(bool show);
	void showPropItemNode(bool show);
	void showNoItemBoxNode(float dt);
	void initMoraleView();
	
	void onCallBackExitPlayback(Ref*);
	void onCallBackContinue(Ref*);
	void onCallBackshowShop(Ref*);

	void	loadCampScoreInfo(int idx, eCampType ecamp);

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	void changeOutPercentBg(int percent);
	void resetOutPercentBg();

	void valueChanged(Ref*, SliderEventType);

	void setOpacityAction(Node* pSender, bool isClean = true);

	void SliderBtnOpaAction();

	bool loadAnimation(std::string frameName, int frameCount, float playTime);
	ui::Button* mPercentBg25;
	ui::Button* mPercentBg50;
	ui::Button* mPercentBg75;
	ui::Button* mPercentBg100;
	ui::Button* mShowProp;

	ui::Text* mPassText;

	//ui::Text* mLabel100;
	//ui::Text* mLabel75;
	//ui::Text* mLabel50;
	//ui::Text* mLabel25;

	Node* mLevelNode;
	std::vector<Node*> mPlayer;

	ui::CheckBox* mCheckBox;
	Node* mSlider;

	//TimeCalculator 倒计时
	LoadingBar* m_timeSprite;
	LoadingBar* m_spiritSprite;
	Text * m_timeFont;
	Text * m_spiritFont;
	Node* m_spiritBg;

	cocostudio::Armature * mYanHua;

	//Slider* mPercentSlider;
	//ui::Text* mSliderLabel;

	ui::Text* mSingleText;
	Sprite* mSpriteSingle;
	ui::Text* mSingle_Tip;
	ui::Button* mPauseBtn;
	ui::ImageView* mSpirtDec;

	Node* mBattleTipsLayer;

	GameReady* mGameReady;

	MsgPanel* mMsgPanel;

	LittleTips* mLittleTips;
	ui::Layout* mPanelNoBat;

	std::string mOnMenuStr;
	
	Node* mGlyphInfoNode;
private:
	Armature * mClickAnimation;	//手指点击动画
	Node*  mGuidePropBarNode;

private:

	bool mLockPauseBtn;
	bool mbShowNoItemAward;
	bool mbNoItemLess2Min;

	int mCurPercentage;

private:
	void animationCallback(Armature * pAr, MovementEventType type, const std::string& aniName);
	void initGuidePropBar();
	void addGuideProp(int propID);
	void clearGuidePropBar();
	void doGuideExtraFunc(MessageGuideTips* pMsg);
	void playGuidePropAni(int giftID);
	void onGuidePropAniCallback(int giftID);
	void activeGuide(int location = E_GUIDELOC_GAMEPAGE);
};

class GameReady : public Node
{
public:
	GameReady();;
	~GameReady();;


	CREATE_FUNC(GameReady);
	bool init();
	void onShow();

protected:

	void animationCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);

protected:

	Node* mRoot;
	cocostudio::Armature* mReadyAni;
	cocostudio::Armature* mPvpReadyAni;
	void updateCaptureScreen(float dt);
	void captureCallBack(bool b, std::string path);
};
