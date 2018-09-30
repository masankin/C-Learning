/***************************************************
	File		: FriendControl.h
	Description	: 好友系统控制类单例
	Author		: Edward Chan
	Date		: 2015-10-01
	
	Copyright (C) 2015 - All Rights Reserved.
****************************************************/
#ifndef __MGAME_FRIENDCONTROL_H__
#define __MGAME_FRIENDCONTROL_H__

#include "Constant.h"
#include "Singleton.h"
#include "net/NetPack.h"
#include "game/Message.h"
#include "FriendProxy.h"
#include <vector>

using namespace std;

class FriendControl
{
public:
	FriendControl();
	~FriendControl();

	FriendProxy* getProxy() { return m_pProxy; };

	vector<FriendSystem::PlayerData*> getFriendList() { return m_vFriendList; };
	vector<FriendSystem::PlayerData*> getSearchList() { return m_vSearchList; };
	vector<FriendSystem::PlayerData*> getApplicationList() { return m_vApplicationList; };
	vector<FriendSystem::PlayerData*> getPkList() { return m_vPkList; };
	vector<FriendSystem::PlayerData*> getBlackList() { return m_vBlackList; };

	//更新好友列表
	void updateFriendList(const fgame::CSFriendGetInfoResp& stMsg);
	//加好友
	void updateFriendList(const fgame::CSFriendConfirmAddResp& stMsg);
	//被删除
	void updateFriendList(const fgame::CSFriendRequestRemoveResp& stMsg);
	//删除好友
	void deleteFriendItem(int uin);

	//更新搜索列表
	void updateSearchList(const fgame::CSFriendQueryResp& stMsg);
	void clearSearchList();
	
	void updateFriendList(const fgame::CSFriendQueryResp& stMsg);

	//更新申请列表
	void updateApplicationList(const fgame::CSFriendGetInfoResp& stMsg);
	//被申请
	void updateApplicationList(const fgame::CSFriendRequestAddResp& stMsg);
	//删除申请项
	void deleteApplicationItem(int index);

	void clearApplicationList();
	
	//更新约战列表
	void updatePkList(const fgame::CSFriendGetInfoResp& stMsg);
	//被约战
	void updatePkList(const fgame::CSPersonalRoomInviteResp& stMsg);
	//收到约战消息
	void updatePkList(const fgame::CSPersonalRoomApplyOrRefuseInviteResp& stMsg);
	//删除约战项
	void deletePkItem(int uin, int roomId);
	void deletePkItem(int roomId);

	//二次封装接口
	void sendAddFriendRequest(int index);
	void sendAddConfirmRequest(bool isAgree, int index);
	void sendPkConfirmRequest(bool isAgree, int index);

	//判断是否已是好友
	bool isFriend(int uin);
	//判断是否有好友
	bool hasFriend();

	//显示约战提示
	void showPvpInviteTip(const ::fgame::FriendPkRequestInfo& stMsg);
	void reomovePvpInviteTip();

	//获取邀请的信息
	FriendSystem::PlayerData* getInviterData() { return m_pInviterData; };
	//存储应战者信息
	FriendSystem::PlayerData* getPkPlayerData() { return m_pPkPlayerData; };
	void setPkPlayerData(FriendSystem::PlayerData*);

#ifdef MGAME_PLATFORM_MPLUS
	//同步平台好友列表
	void updateFriendList(const fgame::CSSycFriendListResp& stMsg);
	void addFriendInfo(const std::string extStr);
#endif
	
private:
	void onPvpInvite(cocos2d::Ref*);
	void acceptPvpInviteCallback(cocos2d::Ref*);

	FriendProxy*				m_pProxy;				//网络代理
	Node*						m_pPvpInviteTip;		//约战提示按钮
	FriendSystem::PlayerData*	m_pInviterData;			//挑战者信息
	FriendSystem::PlayerData*	m_pPkPlayerData;		//应战者信息

	vector<FriendSystem::PlayerData*>		m_vFriendList;			//好友列表
	vector<FriendSystem::PlayerData*>		m_vSearchList;			//搜索添加列表
	vector<FriendSystem::PlayerData*>		m_vApplicationList;		//好友申请列表
	vector<FriendSystem::PlayerData*>		m_vPkList;				//约战列表
	vector<FriendSystem::PlayerData*>		m_vBlackList;			//黑名单列表
	CC_SYNTHESIZE(int, mSeachType,SeachType);
};

#define sgFriendControl() Singleton<FriendControl>::Get()

#endif //__MGAME_FRIENDCONTROL_H__
