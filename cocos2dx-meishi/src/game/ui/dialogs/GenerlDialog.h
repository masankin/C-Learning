#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "GamePrecedure.h"

// 用于更换确认按钮上的字体
enum eGeneralDialogType
{
	E_GENERAL_TYPE_PK_CONFIRM = 0,	//接受文字
	E_GENERAL_TYPE_NORMAL,          //确认文字
};
	
class GenerlDialog :public ActionWindow
{
public:
	GenerlDialog();
	~GenerlDialog();

	
	

	static void  setHeadText(std::string stHead);
	static void setContentText(std::string stHead);
	static void setContentShow();

	//@param stHead 标题 @param stContent 内容 @bEnableClose 是否显示关闭按钮 , btnType 更换按钮上的文字类型
	static void show(std::string stHead, std::string stContent, bool bEnableClose = false);
	static void show(std::string stHead, std::string stContent, const std::function<void(Ref*)>&, bool isAll = false, bool bEnableClose = true, eGeneralDialogType btnType = E_GENERAL_TYPE_NORMAL);
	static void show(std::string stHead, std::string stContent, const std::function<void(Ref*)>& ConfirmCallBack, const std::function<void(Ref*)>& CloseCallBack, bool isAll = false, bool bEnableClose = true);
	//供lua调用
	static void showLuaDialog(std::string stHead, std::string stContent, std::string ConfirmCallBackStr, std::string CancelCallBackStr="", std::string CloseCallBackStr="", bool bEnableCancel = true, bool bEnableClose = false);
	static void setCancel( std::function<void(Ref*)>);
	static void hide();

	virtual bool init();

	CREATE_FUNC(GenerlDialog);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void onClickSureCallBack(cocos2d::Ref*);
	ui::Text* mHeadLabel;
	ui::Text* mContentLabel;
	ui::Widget* mSure;
	ui::Widget* mCancel;
	ui::Widget* mClose;
	ui::Text* mContentLabel1;

	std::string mConfirmCallStr;
	std::string mCancelCallStr;
	std::string mCloseCallStr;
	//ui::Widget* mBox;
};

/***************** delected by jim
class GeneralDialogOther :public ActionWindow
{
public:
	enum eGeneralDialogOtherType
	{
		E_GENERAL_TYPE_TIP = 0,		//只有一个确定按钮的
		E_GENERAL_TYPE_CONFIRM,		//确认框
		E_GENERAL_TYPE_PK_CONFIRM,	//接受挑战确认框
		E_GENERAL_TYPE_NORMAL,      //确认取消框
		E_GENERAL_TYPE_CANCEL,      //取消框
	};

	GeneralDialogOther();
	~GeneralDialogOther();

	static void setContentText(std::string stHead);
	static void setContentShow();

	//@param stContent 内容 @param iType 类型
	static void show(std::string stContent, int iType = 0);
	static void show(std::string stContent, int iType, const std::function<void(Ref*)>&);
	static void show(std::string stContent, int iType, const std::function<void(Ref*)>& confirmFun, const std::function<void(Ref*)>& cancelFun);
	static void hide();

	virtual bool init();

	CREATE_FUNC(GeneralDialogOther);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void onClickSureCallBack(cocos2d::Ref*);

	ui::Text* m_pContentLabel;
	ui::Text* m_pContentLabel1;
	ui::Widget* m_pSure;
	ui::Widget* m_pCancel;
	Sprite*	m_pSureSprite;
	Sprite*	m_pCancelSprite;
};
******************/

class ReLoginDialog :public ActionWindow
{
public:
	ReLoginDialog();
	~ReLoginDialog();

	static void show();
	static void hide();
	static void showButton(bool isShowRetry = true);
	void setShowButton(bool);

	virtual bool init();

	virtual void onShow();
	virtual void onClose();
	CREATE_FUNC(ReLoginDialog);

	void hideRetry();
private:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	ui::Button* mExitBtn;
	ui::Button* mReConBtn;
	ui::Text* mConFailTxt;

	cocostudio::Armature*  mLoadingArm;
};

class KickPlayerDialog :public ActionWindow
{
public:

	CREATE_FUNC(KickPlayerDialog);

protected:
	KickPlayerDialog();
	virtual ~KickPlayerDialog();

	virtual bool init();
	virtual void onMenuItemAction(std::string cName, cocos2d::Ref* sender);
	
private:
};

class TipsManager
{
public:
	TipsManager();
	~TipsManager();
	static void showTextTip(const std::string pStr, Point pPoint = Point::ZERO);
private:

};
