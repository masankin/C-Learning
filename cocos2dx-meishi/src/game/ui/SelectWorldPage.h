#pragma once

#include  "game/ui//system/GameScene.h"
#include "Message.h"
#include "login/LoginProxy.h"

class LoginProxy;

#define ACTIVITY_BTN_COUNT 12
#define DOWN_BTN_COUNT 8
class SelectWorldPage :public GameScene,public MessageHandler
{
public:
	SelectWorldPage();
	~SelectWorldPage();

	virtual bool init();
	virtual bool loadAsync(std::function<void()> loadCompleteCallback) override;
	bool loadAnimation(std::string frameName, int frameCount, float playTime);
	int getFrameCount(eCampType campType, eSoldierType soldierType);


	void onEnter();
	void onExit();

	CREATE_FUNC(SelectWorldPage);

	virtual void onReceiveMassage(GameMessage * message);

	void changeStarLevelNum();

	void TextFieldCallback(Ref*, TextFiledEventType);
	virtual void onShow();
	virtual void onClose();

	void doSkipGuideCamp(int skipState);
protected:

	//移除update 和 logo 无效的纹理
	void removeUnusedTextures();

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void playFinish(Node *pSend);

	void playTipAction(ui::Widget* btn, bool isShow, int intelligentType);
	void showIntelligentGuide(float t);//显示智能指引
	void addIntelligentGuide(ui::Widget* btn);
	void intelGuideAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
	void removeCurIntelGuide(int tag);//1 10s后自动消失  0 点击消失 
	void initMainAni();

	void updatePkIcon(float dt);
	void pkIconCallback(cocos2d::Ref*);

	ui::Text* mCoin;
	std::vector< cocostudio::Armature*> mWorldArmatureVec;

	ui::Button* mOptionButton;
	Node* mStage2;

	ui::ImageView* m_pPkIconImage;
	ui::Button*	m_pFriendButton;
	ui::Button* mNameIcon;
	Text *mPlayerLevel;
	cocostudio::Armature* mDiamondAni;
	cocostudio::Armature* mItemAni;
public:
	void resetUIShow();
	void resetTipShow();

private:
	void showActNoticeDialog(bool isShow, int showType);  //显示礼包可领取提示
private: 
	void doGuideNode(int condition = E_GUIDELOC_MAINUI);
	void initItemAni();
	void doGuideAction(GameMessage* message);
	void playDiamondAni();
	void openUI(int uiID, int winType = 0); 
	void onGiftAniCallback(int id);
	void onAniMovementCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName, int diamond, int curDiamond);
	void resetVipIcon(int id);
	void resetLevel(int level);

	// 对活动图标进行重新排序
	void reOrderActivityBtn();
	void reOrderDownBtn();

	void setPlayerIconVisible(bool flag);
	void playerIconFly();
	void flyAniMoveCallBack(Node* node);
	void flyAniMovementCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
	 

	Vec2 mUpBtnPositions[ACTIVITY_BTN_COUNT];
	Node* mActivityUpBtn[ACTIVITY_BTN_COUNT];

	Vec2 mDownBtnPositions[DOWN_BTN_COUNT];
	Node* mDownBtn[DOWN_BTN_COUNT];

	bool showVIPTips;
	bool mHasNewYearRed;//红包上是否有红点
	bool mNewYearCanReward;//红包是否可领取

	// 礼包推送消息
	Node			*mActNoticeNode;
	ui::Text		*mActNoticeTitle;
	ui::ImageView	*mActIcon;
	int				mActType;

	//智能指引
	typedef std::map<int, ui::Widget*>	IntelligentMapList;
	IntelligentMapList	mIntelligentMapList;
	//int mCurIntelType;
	bool mAddTag;

	Vec2 mIconPos;

	cocostudio::Armature* mFreeTipsArm;
	void checkInsituteFreeState();

	//----------------------------sevenGoal code---------------------------------
private:
	//数值
	int mNowDay = 0;
	bool mIsRetrived;
	int mNowNum = 0;
	int mNextNum = 0;
	Color3B TextcolorNowNormal;
	Color3B TextcolorNowComplete;
	Color3B TextcolorNextNormal;
	bool mbNowComplete;
	bool mbNextComplete;

	//控件
	ui::Button*		mButton_icon;
	ui::ImageView* 	mImage_bg_SevenGoal;
	ui::Text* 		mText_day;
	ui::Text* 		mText_info;
	ui::Text* 		mText_boxInfo;
	ui::Layout* 	mPanel_pos;

	cocostudio::CCArmature * mBoxAni;
	cocostudio::CCArmature * mHongdianAni;

	void updateSevenGoalEvent(cocos2d::EventCustom* customData, int skipState);
	void initSevenGoalInfo();
	std::string getSevenGoalInfoStr(std::string condition, int missionType, int para, int nowCount);
	void changeSevenGoalPanelState(bool bOpen);

	bool mCanTouch;
};
