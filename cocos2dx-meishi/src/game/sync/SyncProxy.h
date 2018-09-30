#pragma once

#include "BaseProxy.h"
class SyncProxy: public BaseProxy
{
public:
	SyncProxy(void);
	~SyncProxy(void);
	void DoRsp(NetPack& stRsp);
	bool fault(const fgame::CSErrorMsg &err);

	void sendOpenStageRqst(int stageId); 
	void sendChangeName(std::string name, int areaId);
	void sendChangePlayerIcon(int iconId,std::string iconUrl = "");
	
	void sendChangePlayerTitle(int iTitle);			//设置当前徽章

	void sendSyncPlayerInfoRqst();														//同步玩家个人信息---发送
	void doSyncPlayerInfoRsp(const fgame::CSSycPlayerInfoResp& resp);					//同步玩家个人信息---接收

	void sendChangeSignRqst(std::string data);                                            //个性签名
	void sendChangeInforVisibleRqst(bool isVisible);                                       //详情面板是否可见
	void sendExtraInfoRqst(int uin);                                                      //获取详情面板信息

};

