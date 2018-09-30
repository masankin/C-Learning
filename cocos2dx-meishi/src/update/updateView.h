#ifndef __UPDATE_VIEW__H
#define  __UPDATE_VIEW__H



#include "UpdateMgrImp.h"
#include "cocos2d.h"

#include "ui/UIText.h"
#include "ui/UILoadingBar.h"
#include "ui/CocosGUI.h"
#include "ui/UILayout.h"

USING_NS_CC;
using namespace ui;

 struct stGameInfo
{
	std::string url;
} ;

stGameInfo getGameInfo();

enum eUpdateInfoType
{
	eUpdateCheck = 0,
	eUpdateDownload,
	eUpdateFail,
	eUpdateExtract,
	eUpdateSuccess,
	eUpdateLoadRes,
	eUpadateAPK,
	eUpadateAPKFail,
	eUpadateReload,
	eUpdatePrompt,
};


class ConfirmLayout : public Layer
{
public:
	ConfirmLayout(){}
	~ConfirmLayout(){}

	virtual bool init();
	CREATE_FUNC(ConfirmLayout);

	void	setContent(std::string szContent);

protected:


	void cancelDownloadCallback(cocos2d::Ref* sender);
	void confirmDownloadCallback(cocos2d::Ref* sender);
};


class UpdateView:public ui::Layout, public iUpdateCallback
{
public:

	static UpdateView* create(const std::string & csbFile);

	virtual void onEnter() override;
	virtual void onExit() override;


	virtual void onChangeScene();
	virtual void onFail();
	virtual void onRestart();

	virtual bool ccTouchBegan(Touch *pTouch, Event *pEvent);

protected:

	UpdateView();
	virtual ~UpdateView();

	virtual bool init(const std::string& csbFile);

	virtual bool loadTextInfo();
	void updateViewText(float time);
		
	void checkUpdate(float dt);
	void onClickDialogRestartBtn();
	void onClickDialogEnterBtn();
	void reloadAllCfg();

	void setPercentage(float fPercentage);
	void setNoticeWords(const char *pszNoticeWords);

	void playLoadingAni();
	void stopLoadingAni();


	// 提示用户是否继续更新
	void showUpdateConfirm();
private:

	Node* m_rootView;

	//采用Layout 不是LoadingBar
	ui::ImageView* m_loadingImage;
	ui::Text* m_loadingText; 
	ui::Text* m_statusText;
	Node* m_animationNode;
	Sprite* m_logoSpirte1;
	Sprite* m_logoSpirte2;
	Sprite* m_logoSpirte3;
	std::unordered_map<int, std::string> m_sErrString;
	bool m_bIsAllReady;
	bool m_bIsPlayLoading;
	float m_fPercentage;

	//ConfirmLayout* mConfirmLayout;
};



#endif
