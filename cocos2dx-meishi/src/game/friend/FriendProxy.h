/***************************************************
	File		: FriendProxy.h
	Description	: 好友网络请求
	Author		: Edward Chan
	Date		: 2015-10-01
	
	Copyright (C) 2015 - All Rights Reserved.
****************************************************/
#ifndef __MGAME_FRIENDPROXY_H__
#define __MGAME_FRIENDPROXY_H__

#include "BaseProxy.h"
#include "game/Constant.h"

class FriendProxy : public BaseProxy
{
public:
	FriendProxy();
	~FriendProxy();

	virtual void DoRsp(NetPack& stRsp);

	//获取全部列表
	void sendAllListRequest();
	//好友列表
	void sendFriendListRequest();
	//添加好友
	void sendAddFriendRequest(int uin);
	//删除好友
	void sendSplitUpRequest(int uin);
	//加好友应答
	void sendAddConfirmRequest(bool isAgree, int uin);
	void sendAddConfirmRequest(bool isAgree, std::vector<int> uins);
	//搜索用户
	void sendSearchRequest(int uin, std::string name,int type =0);
	//好友申请列表
	void sendApplicationListRequest();
	//约战申请列表
	void sendPKListRequest();
	//约战应答
	void sendPkConfirmRequest(bool isAgree, int roomId, int uin);

#ifdef MGAME_PLATFORM_MPLUS
	//同步好友列表
	void sendSycFriendListRequest(std::vector<std::string>);
#endif
	
};

#endif //__MGAME_FRIENDPROXY_H__
