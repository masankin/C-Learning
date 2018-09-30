#pragma once

#include "cocos2d.h"
#include "Constant.h"
#include "cocos/ui/UIListView.h"
#include "ui/system/Window.h"

using namespace cocos2d;

class PropInfo;
class PropItem :public Node
{
public:
	PropItem();
	~PropItem();

	CREATE_FUNC(PropItem);
	bool init();

	//@swallow 默认不吞掉点击事件 关卡内设置吞掉点击，以允许道具拖动效果
	void updateUI(int id);
	void update(Ref*);
	void updatePropNum();
	void onSelectProp();
	void playPropBuyTips();
	void playPropUseTips();
	void stopPropTipsAni();

	void cleanData();
	void ProEvent(bool _b = false);
	void setCdPercentage(float percent);

	ePropType getItemType();;

	int getPropId(){ return m_Id; };

	void setPropId(int id){ m_Id = id; };

	bool open;
	CC_SYNTHESIZE(bool, mIsTipsProp, IsTipsProp);
private:

	Node* mRoot;
	ProgressTimer* mProgress;
	int m_Id;
	Node* mIcon;
	bool mPlayUseTipAni;
	bool mPlayBuyTipAni;

	ui::Text* mCurOwnText;

	ui::Layout* mTouchLayout;
	ui::Button* mCancel;
};


class  PropViewList :public Node
{
public:
	PropViewList();
	~PropViewList();

	CREATE_FUNC(PropViewList);
	bool init();

	void onShow();
	void onHide();

	void updateItem(cocos2d::Ref* pSender, int id);

	Rect getRect();

	void showBornAnimation(const std::function<void()>& callFun = nullptr);
	void showBackAnimation();

	void refreshData();

	void updateView(int id);

	Vec2 getItemPosition(int id);

	ui::Widget* getItemView(int id);

	void	hideOrShowDesc(bool v);
	void	hideOrShowDescDialog(int id,bool v);
	void	updateArrowDesc(float dt);

protected:

	void initCell(Node* pSender, const PropInfo* info);

protected:

	Node* mRoot;
	Node* mDesDialog;
	Node* mDescription;
	ui::ImageView* mPropBg;

	ui::ListView* mListView;

	CC_SYNTHESIZE(bool, mShow, isShow);
	CC_SYNTHESIZE(bool, mShowArrowDesc, isShowArrowDesc);
	CC_SYNTHESIZE(Node*, mGuideNode, GuideNode);

	std::map<int, ui::Widget*> mCellList;

	
};

class PropExchangeView :public Window
{
public:
	PropExchangeView();
	~PropExchangeView();

	virtual bool init();
	virtual void onShow();

	CREATE_FUNC(PropExchangeView);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	void changePropFrameBg(ePropType type, Button* btn);


};


class GuidePropItem : public Node
{
public:
	GuidePropItem();
	~GuidePropItem();
	CREATE_FUNC(GuidePropItem);
	bool init();
	void initUI();
	void onSelectCallback();
	void loadPropIcon(int propID);
	void setUsedFlag();
	bool getUsedFlag(){ return m_Count > 0 ? true : false; };
private: 
	CC_SYNTHESIZE(bool, m_IsLoad, IsLoad);
	CC_SYNTHESIZE(int, m_PropID, PropID);
	CC_SYNTHESIZE(int, m_Type, Type); 
	CC_SYNTHESIZE(int, m_Count, Count);

	Node* m_Root;
	ui::ImageView* m_IconImg;
	bool m_IsSelect;

};
