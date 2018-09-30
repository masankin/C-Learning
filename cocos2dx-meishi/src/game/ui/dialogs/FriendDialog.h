/***************************************************
	File		: FriendDialog.h
	Description	: 好友面板
	Author		: Edward Chan
	Date		: 2015-10-01
	
	Copyright (C) 2015 - All Rights Reserved.
****************************************************/
#ifndef __MGAME_FRIENDDIALOG_H__
#define __MGAME_FRIENDDIALOG_H__

#include "ui/system/ActionWindow.h"
#include "Message.h"
#include "ui/CocosGUI.h"
#include "friend/FriendControl.h"

using namespace ui;

class FriendDialog : public ActionWindow, public MessageHandler, public cocos2d::ui::EditBoxDelegate
{
public:
	FriendDialog();
	~FriendDialog();

	CREATE_FUNC(FriendDialog);

	virtual bool init();

	virtual void onShow();
	virtual void onClose();

	virtual void update(float dt);

	//搜索输入回调
	virtual void editBoxReturn(cocos2d::ui::EditBox* editBox);

private:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	virtual void onReceiveMassage(GameMessage * message);

	//切换选项卡
	virtual void changeTab(int index);
	//改变好友数
	virtual void changeFriendCount(int count);
	//显隐批量处理按钮
	virtual void enableBatchButton(bool visible);

	//增加好友项
	virtual void addFriendItem(int index);
	//更新好友列表
	virtual void updateFriendList();

	//更新搜索列表
	virtual void updateSearchList();

	//更新申请列表
	virtual void updateApplicationList();
	//删除申请项
	virtual void deleteApplicationItem(int index);

	//更新申请列表
	virtual void updatePkList();
	//删除约战项
	virtual void deletePkItem(int uin, int roomId);

	//显示详情
	virtual void showDetail(FriendSystem::PlayerData*, int type = 0);
	//申请批处理
	virtual void onBatchEvent(bool isAgree);

	//校验输入长度
	virtual bool checkCharacter(const std::string str);
	//时间转换成需求的中文
	std::string time2Chinese(int time);
	//好友上线时间转换
	std::string LogintTime2Chinese(int loginTime, int logoutTime);

	//接受约战回调
	virtual void acceptPvpInviteCallback(cocos2d::Ref*);
	//拒绝决战回调
	virtual void rejcectPvpInviteCallback(cocos2d::Ref*);

	//搜索
	virtual void onSearch();

	void onClickBuyEngerCallBack(cocos2d::Ref*);
private:
	ui::EditBox*				m_pSearchBox;
	Button*						m_pTabButton[4];
	Layout*						m_pPanel[4];
	ListView*					m_pListView[4];
	Sprite*						m_pSelectButtonBg;
	Button*						m_pBatchAcceptButton;
	Button*						m_pBatchRejectButton;
	TextField*					m_pSearchTextTield;
	std::string					m_sRealSearchText;
	bool						m_bNewApply;
	bool						m_bNewPk;
	cocostudio::Armature* 		m_pNewApplyTipNode;
	cocostudio::Armature* 		m_pNewPkTipNode;
	int							m_iSelectTabIndex;
	int							m_iSelectItemIndex;

	int							m_iClickFriendTabCount;
	int							m_iClickApplyTabCount;
	int							m_iClickPkTabCount;

	int							m_iTag;

};

#endif //__MGAME_FRIENDDIALOG_H__
