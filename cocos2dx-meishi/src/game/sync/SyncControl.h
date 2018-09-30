#pragma once
#include "cocos2d.h"
#include "Constant.h"
#include "SyncProxy.h" 
#include "net/proto/SaveData.pb.h"
#include "Message.h"
class SyncControl
{
public:
	SyncControl(void);
	~SyncControl(void); 
	void init();	
	void update();

	void saveData(); 



	//同步玩家信息

	void sendSyncPlayerInfoRqst();													//同步玩家信息---发送		
	void doSyncPlayerInfoRsp(const fgame::CSSycPlayerInfoResp& resp);				//同步玩家信息---接收


	//同步关卡信息						

	/////////////////////////////////////////


	void doGmCmdRsp(const fgame::CSGMCmdReq&);  
	
	SyncProxy* getSyncProxy();

	void createLoginTimer();
	void removeLoginTimer();

	void createSyncTimer();
	void removeSyncTimer();

	bool checkSyncSaveDataOver();
	void setSyncSaveDataOver(bool value);
private:	
	bool mUploadSaveData;
	bool mSendSavaData;
	bool mSyncSaveDataOver;

	

	void loadSyncData();				//从文件加载同步数据
	void refreshSyncUsePropData();		//将容器的使用道具相关数据写入protobuf中
 

	void updateLoginTimer();

	SyncProxy m_SyncProxy;

	 
};

#define sgSyncControl() Singleton<SyncControl>::Get()
