#pragma once

#include <map>

#include "BaseProxy.h"
#include "net/NetPack.h"
#include "net/Session.h"
#include "Singleton.h"

#include "cocos2d.h"
USING_NS_CC;

#define SOCKET_RECV_INTERVAL 7

class ConnectingEvent: public Ref
{
public:
	ConnectingEvent()
	{
		resetEvent();
	}

	void resetEvent(){
		mCurrTaskId = -1;
		mMaxWaitingTime = SOCKET_RECV_INTERVAL;
		mDelayTimes = 0.6f;
		mIsAccepted = false;
	}

	~ConnectingEvent(){}
 
	CC_SYNTHESIZE(int, mCurrTaskId, CurrTaskId);
	CC_SYNTHESIZE(int, mMaxWaitingTime, MaxWaitingTime);
	CC_SYNTHESIZE(float, mDelayTimes, DelayTimes);
	CC_SYNTHESIZE(int, mIsAccepted, IsAccepted);
};

//网络连接管理
class NetManager : public Ref
{
public:
	NetManager();
	~NetManager();  
    enum {
        BUF_SIZE = 65535,
    };

	int InitNetConnection(const std::string &strIP, unsigned short uPort);
     
	void SendPkg(NetPack& stRqst, bool bIsCheckRecv = false, bool bIsBlock = false, bool isOnlyOnce = true);
	void SendCachePack();//断线重连后重发未发送请求
	void clearCachePack();

	void RecvPkg(void*, int len);


	void DisConnect();
	void ReConnect();


	bool IsConnect();
	 

    //注册消息处理函数,如果已经注册过并且isUpdateFunc是true,则更新该函数指针
	int  RegCmdListener(const int iCmd, BaseProxy *pstProxy, bool isUpdateFunc = false);
    void delCmdListener(const int iCmd);


	void GenRqstMsgHead(MsgHead& stHead, const int iCmd, const int uUin, bool bIsRestSeq = false, bool bIsUpdateSeq = true);
	void GenRqstMsgHead(MsgHead& stHead, const int iCmd);

	ConnectingEvent *getConnectingEvent() { return connectingData; }
private:


    void DoOneCmd(NetPack& stRsp);
	bool checkCmd2Server(int cmdid);

private:
	std::map<int, BaseProxy*> m_stCmdProxyMap; // <CmdID, CmdTarget>
	std::map<int, std::list<NetPack>> m_stMsgCacheMap; // <CmdID, MsgTarget>
	  
	char m_acCodeBuff[BUF_SIZE]; 

	ConnectingEvent *connectingData;

	CC_SYNTHESIZE(bool, mEncryMsg, EncryMsg);
};

#define sgNetManager()	Singleton<NetManager>::Get()
