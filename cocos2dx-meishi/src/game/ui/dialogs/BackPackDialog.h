/**
* @file     BackPackDialog.h
* @brief    背包功能
* @details  
* @author   Alvin
* @date     2015-9-22
*/
#ifndef __BACKPACKDIALOG_H__
#define __BACKPACKDIALOG_H__
#include  "game/ui/system/ActionWindow.h"
#include "GamePrecedure.h"
#include "Message.h"
#include "data/Tables.h"
#include "cocos2d.h" 
#include "Constant.h"
USING_NS_CC;
enum eBackPackItemType
{
	E_BackPack_Pvp = 0,
	E_BackPack_Pve = 1,
	E_BackPack_Gift = 2,
	E_BackPack_Stuff = 3,
};

enum eBackPackDesc
{ 
	E_BackPackTxt1_Stuff = 16864, //“佐料说明”字样
	E_BackPackTxt1_Prop = 16865,  //“道具说明”字样
};


class BackPropInfoNode: public Layer, public MessageHandler
{
public:
	static BackPropInfoNode* create(int type, int propId, int count);
	BackPropInfoNode();
	~BackPropInfoNode();
	bool init(int type, int propId, int count);
	void setPropTouchCallback(Node* sender, SEL_CallFuncPage fun, Node* target);
	Size getNodeSize();
	int getPropID(){return mPropID;} 
	void setPropCount(int value){ mCount = value;};

	void onReceiveMassage(GameMessage * message);

	//  add by jim
	void setItemSelect(bool select);

	Point getNodePosition() { return mRoot->getPosition(); };
	void  setNodePosition(Point pos) { mRoot->setPosition(pos); };
private:
	void initUI();
	void onSelectCallback();
	void updateItem(cocos2d::Ref* pSender, int id);
	
	int mCount;
	int mPropID;
	int mType;
	CC_SYNTHESIZE(int, mPageIndex, PageIndex);
	Node* mSender;
	SEL_CallFuncPage mFun;
	Node* mTarget; 
	Node* mRoot; 
};
 
class BackPackControl
{
public:
	BackPackControl() :mType(E_BackPack_Pve){};
	~BackPackControl(){};
	void setShowType(int type) { mType = type; };
	int getShowType(){ return mType; };
	void setSelectStuffID(int stuffID){ mSelectStuffID = stuffID; }
	int getSelectStuffID(){ return mSelectStuffID; }
private:
	int mType;
	int mSelectStuffID;
}; 
#define sgBackPackControl() Singleton<BackPackControl>::Get()


class BackPackDialog: public ActionWindow, public MessageHandler
{
public:
	BackPackDialog();
	~BackPackDialog();
	CREATE_FUNC(BackPackDialog);
	bool init(); 
	void onEnter();
	void onExit();
	void onReceiveMassage(GameMessage * message);
	void onMenuItemAction(std::string, cocos2d::Ref*); 
	void onShow();
private:
	void initData();
	void initUI();
	void refreshUI(bool isProp = true);
	void refreshProp();
	void refreshStuff();
	BackPropInfoNode* createInfoNode(int refIndex, int pageIndex, int type, int propID, int count);

	void onShowPropDescCallback(int pageIndex, int id, int index);						//点击单元格的说明文字的回调
	void closeOhterSelectFrame(int id);													//关闭其他单元格边框的选中效果 
	void resetPropDesc(int type, int id);												//刷新说明文字UI
	void clearPropDesc();                                                               //清空显示内容

	void showPvpProp();																	//显示PVP道具
	void shopPveProp();																	//显示PVE道具
	void showGift();																	//显示礼品
	void showStuff();																	//显示材料

	void goNextPage();
	void goPrePage();
	void showSwitchBtn(bool isShow);

	void setOpenBoxBtn(bool isShow, int count = 0, int status = 1 );

	void onPageScrollCallback(Ref* ref, ui::PageViewEventType type);
	void resetShowBtn(std::string typeStr);												//设置标签按钮的样式

	const PropList getPveProp() { return mPvePropList; }
	const PropList getPvpProp() { return mPvpPropList; }
	const PropList getGiftProp();
	const GlyphStuffList getGlyphStuff() { return mStuffList; }

	// 计算可打开的箱子数
	int calculateOpenCount(int item_id, int item_count);
	// 对于开体力药水，精力药水的特殊处理
	// 计算需要打开的数量
	int fullPowerNeed(int bottle_id);
	void showOpenTips(int open_id, int open_count, int show_type = 0);
private:
	 
	ui::PageView* mViewNode;
	Node* mPropDescNode;
	Node* mBgNode;
	 
	Button* mPvpBtn;
	Button* mPveBtn;
	Button* mGiftBtn;
	Button* mStuffBtn;

	//说明标签显示UI

	ui::Text* mDescLable1;
	ui::Text* mDescLabel2;
	ui::ImageView* mImg;
	ui::Text* mNameLable;
	ui::Text* mPropTxt1;
	ui::Text* mPropTxt2;

	std::map<std::string, Button* > mShowBtnMap;

	Button* mOpenBoxBtn;

	PropList mPvePropList;
	PropList mPvpPropList;
	PropList mGiftPropList; 
	PropList mUsableGiftList;
	GlyphStuffList mStuffList;

	int mShowPve;

	int mPageCnt;

	int mToolsId;
	int mOpenBoxCount;
	int mSelectIndex; 
	ui::Text* mPageTxt;

	ui::Button* mPreBtn;
	ui::Button* mNextBtn;

	int mMarkOpenCount;
};

 
#endif
