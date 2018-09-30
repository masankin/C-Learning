#pragma once


#include "BaseProxy.h"

#define  CONNET_COUNT (6)
#define HEARBEAT_TICK (7.0)

class LoginProxy : public Ref, public BaseProxy
{
public:

	enum eServerState
	{
		E_SS_INVALID = -1,
		E_SS_CENTER,	// 中心服
		E_SS_LOGIN,		// 登陆服
		E_SS_GAME,		// 逻辑服
	};

	LoginProxy();
	~LoginProxy();

	struct serverState
	{
		serverState(int state, std::string content)
		{
			m_state = state;
			m_content = content;
		}

		int m_state;
		std::string m_content;
	};

	serverState getServerStateInfo();

	virtual void DoRsp(NetPack& stRsp);
	virtual bool fault(const fgame::CSErrorMsg &err);

	void sendRegisterRqst();

	void sendEnterServerRqst();
	void sendDirectEnterRqst();
	void sendGetServerRqst(bool block = false);

	void	analysisLoginDNS();
	void	requestLoginKey();

	//add by weihua.cui
	void sendPKeyRest();
	void sendClientKey();


	void loginSuccess();

	bool checkLogin(bool show = false);

	void sendHeartBeat();

	void updateHearBeat(float dt);

	void InitNetConnection(std::string addrs);

	void currSignalNum();

	void onCallBack(cocos2d::Ref* ref);

	void onRetryEnterServer();
	void updateRetryEnterServer(float dt);

	void sendOtherRqst();

	void sendOther2Rqst();
	void isLogin();

	void doResLoadOver();

	void sendLogOut();
	CC_SYNTHESIZE(bool, mIsAccepted, IsAccepted);
	CC_SYNTHESIZE(std::string, mGameServer, GameServer);
	CC_SYNTHESIZE(std::string, mGameServerName, GameServerName);

	CC_SYNTHESIZE_READONLY(int, mCurrSignal, CurrSignal);
	CC_SYNTHESIZE(int, mCurrSignalMs, CurrSignalMs);
	CC_SYNTHESIZE(int, mConnTimes, ConnTimes);
	CC_SYNTHESIZE(unsigned long long, mSendStart, SendStart);

	CC_SYNTHESIZE(unsigned long long, mServerTime, ServerTime);//服务器时间

	CC_SYNTHESIZE(int,mRetryLoginServer, RetryLoginServer);
	CC_SYNTHESIZE(eServerState, mServerState, ServerState);

	CC_SYNTHESIZE(bool, mLoginSuccess, LoginSuccess);
	CC_SYNTHESIZE(bool, mReloginShowing, ReloginShowing);
	CC_SYNTHESIZE(bool, mIsUpdateResource, IsUpdateResource); 
	CC_SYNTHESIZE(int, mRetry, Retry);
private:


	void scheduleHeartBeat(float dt);


	void	reallyLoginRqst();

	void    kickPlayer();

	void	requestLoginTimeoutCallback(float dt);

	void	showServerStateContent();

	/*
	* 根据登录服务器返回，判定服务器停服，灰度测试，白名单等状态
	* @ret: 是否能继续请求进入游戏服
	*/
	bool	checkServerState(const ::fgame::CSLoginResp& loginResp);

	bool mIsQueryAllStatus;

	bool mIsOther2RequestStatus;

	bool mIsNeedKickPlayer;
	unsigned char m_k[25];
};

#define sgLoginProxy() Singleton<LoginProxy>::Get()
