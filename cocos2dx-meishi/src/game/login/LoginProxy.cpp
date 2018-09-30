#include "LoginProxy.h"

#include "net/UserObj.h"
#include "net/NetManager.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"
#include "EnvSet.h"
#include "sync/SyncControl.h"

#include "ui/dialogs/GenerlDialog.h"
#include "channel/ChannelApi.h"
#include "game/ui/PageFunction.h"
#include "game/notice/NoticeControl.h"
#include "MessageManager.h"
#include "battle/BattleControl.h"
#include "channel/ChannelApi.h"
#include "ranking/RankingControl.h"
#include "achieve/AchieveControl.h"
#include "act/ActControl.h"
#include "game/daytask/DayTaskControl.h"
#include "battle/BattleControl.h"
#include "shop/ShopProxy.h"
#include "update/UpdateMgrImp.h"
#include "update/UpdateHelper.h"
#include <stdio.h>
#include "RSACrypt.h"
#include "BlowFishCrypt.h"

#include "SrvDownNoticeDialog.h"

#include "friend/FriendControl.h"
#include "winner/WinnerControl.h"
#include "data/MailData.h"
#include "mail/MailProxy.h"
#include "act/PowerGiftControl.h"
#include "core/ResManager.h"
#include "update/HttpProxy.h"
#include "core/GameJniHelper.h"
#include "guide/GuideManager.h"
#include "game/sevenGoal/SevenGoalControl.h"
#include "game/utils/GameDefault.h"

#ifdef WIN32
#include <winsock2.h>    
#include <mstcpip.h> 
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#endif

LoginProxy::LoginProxy():
mGameServer(""), mServerState(E_SS_INVALID),
	mSendStart(0), mCurrSignalMs(0),mCurrSignal(0),mConnTimes(CONNET_COUNT),mServerTime(0),mGameServerName(""),mRetryLoginServer(0)
	, mIsQueryAllStatus(false), mIsOther2RequestStatus(false), mReloginShowing(false), mIsNeedKickPlayer(false), mIsUpdateResource(true)
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_REG, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_LOGIN, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_ENTER_SERVER, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_LOGOUT, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_DIRECT_ENTER, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_QUEUE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_CANENTER, this);
	sgNetManager()->RegCmdListener(fgame::CS_GET_SERVER, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_HEART_BEAT, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_KICK_PLAYER, this);

	//add by weihua
	sgNetManager()->RegCmdListener(fgame::CS_CMD_REQUEST_PKEY, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_SEND_CLIENT_KEY, this);
	mLoginSuccess = false;
	mIsAccepted = true;
}

LoginProxy::~LoginProxy()
{
}

bool LoginProxy::fault(const fgame::CSErrorMsg &err)
{
	if (err.errcode() == 10001)
	{
		CCLOG("[trace]LoginProxy::fault register account error!account exist!");
	}
	return true;
}

void LoginProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]LoginProxy::DoRsp error!");
		
	}
	else if (stRsp.getS2CMsg()->has_reg_resp())
	{
		const ::fgame::CSRegResp& regRsp = stRsp.getS2CMsg()->reg_resp();

		long long _uin = regRsp.uin();
		CCLOG("[trace]LoginProxy::DoRsp register success!uin:%lld!", _uin);

	}
	else if (stRsp.getS2CMsg()->has_login_resp())
	{
		setServerState(E_SS_LOGIN);

		const ::fgame::CSLoginResp& loginRsp = stRsp.getS2CMsg()->login_resp();
		sgUserObj()->setUin(loginRsp.uin());
		//if (loginRsp.has_openid())
			//sgUserObj()->setOpenID(loginRsp.openid());
		if (loginRsp.has_authtoken())
			sgChannelApi()->setPayToken(loginRsp.authtoken());

		// 服务器维护相关判定
		if (checkServerState(loginRsp) == false)
			return;

#ifdef MGAME_IN_APP_SDK
		//by carl
		if (sgChannelApi()->getChannelId() == ChannelApi::eChannelID::ChannelID_CoolPad)
		{
			sgChannelApi()->uploadSDKInfo(sgUserObj()->getOpenID(), sgChannelApi()->getPayToken());
		}
		sgChannelApi()->uploadUin(__String::createWithFormat("%d", sgUserObj()->getUin())->getCString());
#endif

		CCLOG("[trace]LoginProxy::DoRsp uin is %d, openid is %s, paytoken is %s", sgUserObj()->getUin(), sgUserObj()->getOpenID().c_str(), sgChannelApi()->getPayToken().c_str());

		mGameServer = loginRsp.gameserveraddr();
		sgMessageManager()->sendMessage(EMT_GAME_LOGIN_FLOW, StringUtils::format("%d 1 %s", MessageGameLoginFlow::FlowType::EnterLoginSvr, mGameServer.c_str()));

		sendEnterServerRqst();
	}
	else if (stRsp.getS2CMsg()->has_enterserver_resp())
	{
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(LoginProxy::updateRetryEnterServer),sgLoginProxy());
		const ::fgame::CSEnterServerResp& enterRsp = stRsp.getS2CMsg()->enterserver_resp();
		if (enterRsp.has_gameservername())
		{
			sgUserObj()->setGameServerId(enterRsp.gameservername());
		}

		if (enterRsp.has_ts())
		{
			sgTimeCalculator()->initServerTime(enterRsp.ts());
		}

		std::string strFailureReson;
		if (enterRsp.has_forceexit())
		{
			strFailureReson = "ForceExit";

			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_SERVER_FORCE));  
			CCLOG("[trace]LoginProxy has_forceexit == true");
		}
		else if (enterRsp.overload() == 1)
		{
			strFailureReson = "OverLoad";

			int num = enterRsp.wait_num();
			sgUserObj()->setWaitNum(num);  
			CCLOG("[trace]LoginProxy enterRsp.overload() == 1");
		}
		else
		{
			strFailureReson.clear();

			loginSuccess();
		}

		if (strFailureReson.empty())
		{
			if (enterRsp.has_gameservername())
				sgMessageManager()->sendMessage(EMT_GAME_LOGIN_FLOW, StringUtils::format("%d 1 %s", MessageGameLoginFlow::FlowType::EnterGameSvr, enterRsp.gameservername().c_str()));
			else
				sgMessageManager()->sendMessage(EMT_GAME_LOGIN_FLOW, StringUtils::format("%d 1", MessageGameLoginFlow::FlowType::EnterGameSvr));
		}
		else
		{
			sgMessageManager()->sendMessage(EMT_GAME_LOGIN_FLOW, StringUtils::format("%d 0 %s", MessageGameLoginFlow::FlowType::EnterGameSvr, strFailureReson.c_str()));
		}

		CCLOG("[trace]LoginProxy::EnterServerResp loginSuccess");
	}
	else if (stRsp.getS2CMsg()->has_canenter_req())
	{
		const ::fgame::CSCanEnterReq& queueRsp = stRsp.getS2CMsg()->canenter_req();

		sendDirectEnterRqst();
	}
	else if (stRsp.getS2CMsg()->has_directenter_resp())
	{
		const ::fgame::CSDircetEnterResp& dircetRsp = stRsp.getS2CMsg()->directenter_resp();
		if(dircetRsp.ret())
		{
			loginSuccess(); 
		}
	}
	else if (stRsp.getS2CMsg()->has_queue_req())
	{
		const ::fgame::CSQueueReq& queueRsp = stRsp.getS2CMsg()->queue_req();

		int num = queueRsp.remain_num();
		sgUserObj()->setWaitNum(num);

	}
	else if(stRsp.getS2CMsg()->has_getserver_resp())
	{
		/*setServerState(E_SS_CENTER);

		sgLoginProxy()->setRetryLoginServer(0);

		const ::fgame::CSGetServerResp& serverRsp = stRsp.getS2CMsg()->getserver_resp();
		
		if (serverRsp.has_whiteupdate())
		{
			//灰度测试中
			if (serverRsp.whiteupdate() == 0)
			{
				//非白名单用户，不更新
				setIsUpdateResource(false);
			}	
			else
			{       
				//白名单用户，更新
				setIsUpdateResource(true);
			}
		}
		else
		{
			//非灰度测试，所有人都更新
			setIsUpdateResource(true);
		}

		
		mGameServerName = serverRsp.gameservername();
		sgUserObj()->setLoginServerDir(serverRsp.loginserver());

		InitNetConnection(sgUserObj()->getLoginServerDir());

		sgMessageManager()->sendMessage(EMT_GAME_LOGIN_FLOW, StringUtils::format("%d 1 %s", MessageGameLoginFlow::FlowType::EnterCenterSvr, serverRsp.loginserver().c_str()));

		std::string localKey = "key.bin";
		if (FileUtils::getInstance()->isFileExist(localKey))
		{
			//跳过第一步 直接第二步
			sendClientKey();
		}
		else
		{
			sendPKeyRest();
		}*/
	}
	else if (stRsp.getS2CMsg()->has_requestpkey_resp())
	{
		std::string keyPath = UpdateMgrImp::getInstance()->GetExtractPath() + "/Resources/" + "key.bin";
		std::string dir = UpdateMgrImp::getInstance()->GetExtractPath() + "/Resources/";
		UpdateHelper::MakeDirs(dir.c_str());
		const ::fgame::CSRequestPKeyResp& keyRsp = stRsp.getS2CMsg()->requestpkey_resp();
		std::string s = keyRsp.pkey();
		FILE* f = fopen(keyPath.c_str(), "wb");
		if (f == nullptr)
		{
			CCLOG("[error]openg key file error!");
			return;
		}

		int nwrite = fwrite(s.c_str(), s.size(), 1, f);
		if (nwrite != 1)
		{
			CCLOG("[error]write file error");
			return;
		}
		fclose(f);
		sendClientKey();
	}
	else if (stRsp.getS2CMsg()->has_sendclientkey_resp())
	{
		const ::fgame::CSSendClientKeyResp& sendKeyRsp = stRsp.getS2CMsg()->sendclientkey_resp();
		const std::string& s = sendKeyRsp.s_key();
		const std::string& payload = sendKeyRsp.payload(); 
		
		unsigned char key[32] = "\0";
		memcpy((void*)&key[0], (void*)m_k, 24);
		memcpy((void*)&key[24], s.c_str(), 8);

		unsigned char* decrypt = new unsigned char[payload.size() + 1];
		memset(decrypt, 0, payload.size() + 1);
		sgBlowfishManager()->initKey(key, 32);
		sgBlowfishManager()->decrypt(payload.size(), (unsigned char*)payload.c_str(), decrypt);
		//delete[] decrypt;

		sgNetManager()->setEncryMsg(true);
		reallyLoginRqst();
	}
	else if (stRsp.getS2CMsg()->has_heartbeat_resp())
	{
		mCurrSignalMs = sgTimeCalculator()->getCurrentTime()  - mSendStart;
		mCurrSignalMs  = (mCurrSignalMs <= 0 || mCurrSignalMs > 5000)? (rand()%333) : mCurrSignalMs;
		mSendStart = 0;
		mConnTimes = CONNET_COUNT;
		currSignalNum();
		sgPvpControl()->doHeartbeat();
		if (stRsp.getS2CMsg()->heartbeat_resp().has_ts())
		{
			setServerTime(stRsp.getS2CMsg()->heartbeat_resp().ts());
			sgPvpControl()->setIsNoItemOpen(mServerTime);
			sgWinnerControl()->setWinnerRoadOpen(mServerTime);
		}
		if (sgLoginProxy()->getReloginShowing())
		{
			sgNetManager()->SendCachePack();
			ReLoginDialog::hide();
		}
	}
	else if (stRsp.getS2CMsg()->has_kickplayer_req())
	{
		kickPlayer();
	}
}

void LoginProxy::reallyLoginRqst()
{
	unsigned int uCmd = fgame::CS_CMD_LOGIN;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	::fgame::CSLoginReq* pLoginRqst = msgPack.getC2SMsg()->mutable_login_req();

	pLoginRqst->set_openid(sgUserObj()->getOpenID());
	pLoginRqst->set_channelid(sgChannelApi()->getChannelId());
	pLoginRqst->set_token(sgChannelApi()->getSession().c_str());
	pLoginRqst->set_secretkey(sgChannelApi()->getXGPushToken().c_str());
	pLoginRqst->set_gameservername(mGameServerName);

	CCLOG("[trace]LoginProxy::reallyLoginRqst Uin:%d, OpenID:%s, ChannelID:%d, token:%s", sgUserObj()->getUin(), sgUserObj()->getOpenID().c_str(), sgChannelApi()->getChannelId(), sgChannelApi()->getSession().c_str());

	sgNetManager()->SendPkg(msgPack);
	mRetry = 0;
}

void LoginProxy::requestLoginTimeoutCallback(float dt)
{
	++mRetryLoginServer;
	CCLOG("[trace]LoginProxy::requestLoginTimeoutCallback : Request Times = %d", mRetryLoginServer);
	if (mRetryLoginServer <= 2)
	{
		::fgame::LoginData* pLoginData = sgUserObj()->getSaveData()->mutable_logindata();
		int iSize = pLoginData->loginsrvlist_size();
		if (iSize > 0)
		{
			int channelId = sgChannelApi()->getChannelId();
			//1、连接失败 从缓存中删除登录服ip（本地网络OK下才删除）
			std::string szLoginAddr = sgUserObj()->getLoginServerDir();
			sgUserObj()->dltLoginSrvFromList(channelId, szLoginAddr);

			//2、缓存中随机另一个ip，尝试连接		循环两次
			std::string szOther = sgUserObj()->sltLoginSrvRand(channelId);
			if (szOther.empty())
			{
				// 报错 
				GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_SRV_CONN_FAIL));
				return;
			}
			sgUserObj()->setLoginServerDir(szOther);

			CCLOG("[trace]LoginProxy::requestLoginTimeoutCallback : LoginSrv from cache, %s", szOther.c_str());
		}
	}
	else if (mRetryLoginServer == 3)
	{
		std::string szDns;
		int port = 0;

		std::string szLoginDns = sgChannelApi()->getEmergLoginDNS();
		if (szLoginDns.find(":") != std::string::npos)
		{
			port = atoi(szLoginDns.substr(szLoginDns.find(":") + 1).c_str());
			szDns = szLoginDns.substr(0, szLoginDns.find(":"));
		}

		hostent* phost = gethostbyname(szDns.c_str());
		if (phost)
		{
			string szLoginAddr = StringUtils::format("%s:%d", inet_ntoa(*((struct in_addr*)phost->h_addr)), port);
			sgUserObj()->setLoginServerDir(szLoginAddr);

			CCLOG("[trace]LoginProxy::requestLoginTimeoutCallback : analyze DNS success, %s", szLoginAddr.c_str());
		}
	}

	if (mRetryLoginServer <= 3)
	{
		sgNetManager()->setEncryMsg(false);
		requestLoginKey();
	}
	else
	{
		showServerStateContent();
	}

	
}


void LoginProxy::sendRegisterRqst()
{
	unsigned int uCmd = fgame::CS_CMD_REG;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSRegReq* pRegRqst = msgPack.getC2SMsg()->mutable_reg_req();
	
	pRegRqst->set_account("1001");
	pRegRqst->set_pwd("000");

	sgNetManager()->SendPkg(msgPack);
}

void LoginProxy::sendEnterServerRqst()
{
	InitNetConnection(mGameServer);

	CCLOG("[trace]LoginProxy::sendEnterServerRqst mGameServer = %s", mGameServer.c_str());

	unsigned int uCmd = fgame::CS_CMD_ENTER_SERVER;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSEnterServerReq* pEnterRqst = msgPack.getC2SMsg()->mutable_enterserver_req();

	pEnterRqst->set_uin(sgUserObj()->getUin());
	pEnterRqst->set_openid(sgUserObj()->getOpenID());
	pEnterRqst->set_retry(mRetry);
	pEnterRqst->set_imei(sgChannelApi()->getImei().c_str());
	pEnterRqst->set_version(sgChannelApi()->getVersion().c_str());
	if(sgSyncControl()->checkSyncSaveDataOver() == false)
	{
		sgNetManager()->setEncryMsg(false);
		sgNetManager()->SendPkg(msgPack);
		sgNetManager()->setEncryMsg(true);
		return;
	}
	bool flag = (bool)mRetry;
	if (!sgBattleControl()->getIsPvp() && sgGamePrecedure()->getEnterGame())
	{
		flag = false;
	}
	sgNetManager()->setEncryMsg(false);
	sgNetManager()->SendPkg(msgPack,false);
	sgNetManager()->setEncryMsg(true);
}

void LoginProxy::sendDirectEnterRqst()
{
	sgUserObj()->setWaitNum(0);
	unsigned int uCmd = fgame::CS_CMD_DIRECT_ENTER;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSDirectEnterReq* pDirectRqst = msgPack.getC2SMsg()->mutable_directenter_req();
	pDirectRqst->set_uin(sgUserObj()->getUin());

	sgNetManager()->SendPkg(msgPack);
}

void LoginProxy::sendGetServerRqst(bool block)
{
	/*Director::getInstance()->getScheduler()->schedule(schedule_selector(LoginProxy::scheduleLoginServer),
		sgLoginProxy(),
		sgNetManager()->getConnectingEvent()->getMaxWaitingTime()
		,3,0,false);

	sgNetManager()->setEncryMsg(false);
	InitNetConnection(sgChannelApi()->getDirURL());

	unsigned int uCmd = fgame::CS_GET_SERVER;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSGetServerReq* pDirectRqst = msgPack.getC2SMsg()->mutable_getserver_req();
	pDirectRqst->set_openid(sgUserObj()->getOpenID());
	pDirectRqst->set_gameservername(sgUserObj()->getGameServerName());
	pDirectRqst->set_lastts(sgUserObj()->getLastServerTime());
	pDirectRqst->set_uin(sgChannelApi()->isOnlineSdk() ? sgUserObj()->getUin() : 0);
	pDirectRqst->set_version(sgChannelApi()->getVersion().c_str());
	
	if(sgSyncControl()->checkSyncSaveDataOver() == false)
	{ 
		sgNetManager()->SendPkg(msgPack); 
		return;
	}
	sgNetManager()->SendPkg(msgPack, false , block);
	CCLOG("[trace]LoginProxy::sendGetServerRqst openID is %s",sgUserObj()->getOpenID().c_str());*/
}
void LoginProxy::sendPKeyRest()
{
	unsigned int uCmd = fgame::CS_CMD_REQUEST_PKEY;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);
	fgame::CSRequestPKeyReq* pKeyReq = msgPack.getC2SMsg()->mutable_requestpkey_req();
	pKeyReq->set_dummy("111");
	pKeyReq->set_not_used(1);
	sgNetManager()->SendPkg(msgPack, false);
}

void LoginProxy::sendClientKey()
{
	unsigned int uCmd = fgame::CS_CMD_SEND_CLIENT_KEY;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSSendClientKeyReq* pKeyReq = msgPack.getC2SMsg()->mutable_sendclientkey_req();
	const int clientKeyLen = 24;
	memset(m_k, 0, clientKeyLen + 1);
	for (int i = 0; i < clientKeyLen; i++)
	{
		m_k[i] = (unsigned char)cocos2d::random(1, 255);
	}

	Data publicData = FileUtils::getInstance()->getDataFromFile("key.bin");

	sgRSACryptManager()->initPublicKey(publicData.getBytes(), publicData.getSize());
	const int encryptLen = sgRSACryptManager()->getEncryptSize();
	unsigned char* encryptKey = new unsigned char[encryptLen];
	memset(encryptKey, 0, encryptLen);
	sgRSACryptManager()->encrypt(clientKeyLen, m_k, encryptKey);

	pKeyReq->set_client_key((char*)encryptKey, encryptLen);

	sgNetManager()->SendPkg(msgPack, false);
	//delete[] encryptKey;
}

void LoginProxy::loginSuccess()
{
	if (getServerState() != E_SS_GAME)
		PageFunction::Get()->gotoFunction(MSG_FUC_UPDATE);

	setServerState(E_SS_GAME);

	//static bool bEnterGame = false;
	//if (!bEnterGame)
	//{
	//	PageFunction::Get()->gotoFunction(MSG_FUC_UPDATE);
	//	bEnterGame = true;
	//}

	//启动定时发送心跳
	if (Director::getInstance()->getScheduler()->isScheduled(schedule_selector(LoginProxy::updateHearBeat),sgLoginProxy())==false)
	{

		sendHeartBeat();
		Director::getInstance()->getScheduler()->schedule(schedule_selector(LoginProxy::updateHearBeat),
			sgLoginProxy(),HEARBEAT_TICK,CC_REPEAT_FOREVER,0.0,false);
	}
	mRetry = 1;
	mConnTimes = CONNET_COUNT;
	mSendStart = 0;

	//告知syncPage登陆成功
	
	std::string param = StringUtils::format("%d %d", CS_CMD_ENTER_SERVER, 0);
	sgMessageManager()->sendMessage(EMT_RQST_RESULT, param); 
	mLoginSuccess = true;	 
	std::string param2 = StringUtils::format("%d", 1);
	sgMessageManager()->sendMessage(EMT_GETSAVEDATAOVER, param2); 
	
	sendOtherRqst();

	sgPvpControl()->setIsNoItemOpen(0);
}	

bool LoginProxy::checkLogin(bool show)
{
	if (mRetry == 0)
	{
		if(show || sgChannelApi()->isOnlineSdk())
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_LOGIN_AUTO));
			sgMessageManager()->sendMessage(EMT_CHANNEL_LOGIN, "1");
		}		
		return false;
	}
	int num = sgUserObj()->getWaitNum();
	if (num)
	{
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), StringUtils::format(sgTextHelper(TEXT_LOGIN_QUEUE).c_str(), num));
		return false;
	}
	return true;
}

void LoginProxy::InitNetConnection(std::string addrs)
{
	sgNetManager()->DisConnect();

	int nPort = 0;
	std::string szip;

	if (std::string::npos != addrs.find(":"))
	{
		nPort = atoi(addrs.substr(addrs.find(":") + 1).c_str());
		szip = addrs.substr(0, addrs.find(":"));
	}

	CCASSERT(nPort != 0, "port must be a value");
	sgNetManager()->InitNetConnection(szip, nPort);
}

void LoginProxy::sendHeartBeat()
{
	unsigned int uCmd = fgame::CS_CMD_HEART_BEAT;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSHeartBeatReq* pHeartRqst = msgPack.getC2SMsg()->mutable_heartbeat_req();
	sgNetManager()->SendPkg(msgPack);

	setSendStart(sgTimeCalculator()->getCurrentTime());
	setIsAccepted(false);
}

void LoginProxy::updateHearBeat(float dt)
{
	if (sgLoginProxy()->getConnTimes() <= 0)
	{
		CCLOG("[trace]LoginProxy::updateHearBeat sRetry = %d", sgLoginProxy()->getRetry());
		if (sgLoginProxy()->getRetry() == 0) return;
		
		//重连不上
		sgLoginProxy()->setRetry(0);
		sgLoginProxy()->setLoginSuccess(false);
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(LoginProxy::updateHearBeat), sgLoginProxy());
		
		ReLoginDialog::showButton();
		return;
	}

	unsigned long long istart =sgLoginProxy()->getSendStart();
	if (istart != 0 && !sgLoginProxy()->getIsAccepted())
	{		 
		sgLoginProxy()->setConnTimes(sgLoginProxy()->getConnTimes() - 1);
		//没收到心跳返回
		if(!sgNetManager()->IsConnect() ||sgLoginProxy()->getConnTimes() <= CONNET_COUNT/2)
		{
			//连接已经断开的情况
			//关闭Socket 重连逻辑服
			if (!(sgGamePrecedure()->getEnterGame()&& !sgBattleControl()->getIsPvp()))
			{
				ReLoginDialog::show();
			}
			
			sgLoginProxy()->sendEnterServerRqst();
			sgPvpControl()->setIsReConne(true);
		}	
		 CCLOG("[trace]LoginProxy::updateHearBeat sgLoginProxy()->getConnTimes() = %d", sgLoginProxy()->getConnTimes());
	}
	sgLoginProxy()->sendHeartBeat();	 
	
}

#define _SignalMs1 (1300)
#define _SignalMs2 (800)
#define _SignalMs3 (400)

void LoginProxy::currSignalNum()
{
	int _Signal = 0;
	if (mCurrSignalMs > _SignalMs1)
	{
		_Signal = 0;
	}
	else if (mCurrSignalMs >= _SignalMs2 &&
		mCurrSignalMs < _SignalMs1)
	{
		_Signal = 1;
	}
	else if (mCurrSignalMs >= _SignalMs3 &&
		mCurrSignalMs < _SignalMs2)
	{
		_Signal = 2;
	}
	else if (mCurrSignalMs < _SignalMs3)
	{
		_Signal = 3;
	}

	if (!sgBattleControl()->getIsPvp())
	{
		mCurrSignal = _Signal;
		return;
	}
	if (_Signal != mCurrSignal)
	{
		sgMessageManager()->sendMessage(EMT_SIGNAL_CHANGE, "1");
	}
	else
	{
		sgMessageManager()->sendMessage(EMT_SIGNAL_CHANGE, "");
	}
	mCurrSignal = _Signal;
	return;
}


void LoginProxy::onCallBack(cocos2d::Ref* ref)
{
	GenerlDialog::hide();
	sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
}

void LoginProxy::onRetryEnterServer()
{
	mRetry = 1;
	sgLoginProxy()->sendEnterServerRqst();
	sgPvpControl()->setIsReConne(true);
	Director::getInstance()->getScheduler()->schedule(schedule_selector(LoginProxy::updateRetryEnterServer),
		sgLoginProxy(),HEARBEAT_TICK,0,0.0,false);
}

void LoginProxy::updateRetryEnterServer(float dt)
{
	ReLoginDialog::showButton(false);
}

void LoginProxy::sendOtherRqst()
{
	sgNetManager()->SendCachePack();
	ReLoginDialog::hide();
	CCLOG("[trace]LoginProxy::sendOtherRqst");
	sgSyncControl()->sendSyncPlayerInfoRqst();

	if (!mIsQueryAllStatus)
	{
		mIsQueryAllStatus = true;
		//sgUserObj()->sendGetCheckPtData();
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBthGetMissions");
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBthGetMissionsRank");
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eAsstLoginSync");

		sgRankingControl()->requestPlayerPvpResultInfo();
		sgRankingControl()->requestPlayerRankingInfo();
		sgRankingControl()->requestPlayerSelfRankingList();

#ifdef MGAME_PLATFORM_MPLUS
//请求任务列表
sgDayTaskControl()->getProxy()->sendMissionListReq();
#endif
		sgUserObj()->sendGetNewBieList();
		
		
	}else
	{
		if (sgPageFunction()->getCurrentUIType() == MSG_FUC_LOGIN)//登陆界面重连
			sgMessageManager()->sendMessage(EMT_GAMESERVERLOGIN_SUCCESS, "");
	}
}

void LoginProxy::sendOther2Rqst()
{
	if (!mIsOther2RequestStatus)
	{
		mIsOther2RequestStatus = true;
		sgActControl()->getActProxy()->sendGetOpenActivity();
		sgActControl()->getActProxy()->sendGetActivityData();
		sgActControl()->sendQueryActLogin();
		sgWinnerControl()->getWinnerProxy()->sendWinnerListRqst();
		sgPowerGiftControl()->sendGetPowerActivityDataRqst();
		sgAchieveControl()->sendQueryAchievementStatus();

		sgMailProxy()->sendGetMail(sgMailData()->getMailNo());
		sgFriendControl()->getProxy()->sendAllListRequest();
		
		
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eCanRewardRedPacketReq", NULL);
	}
	
}
void LoginProxy::isLogin()
{
	sendHeartBeat();
	Director::getInstance()->getScheduler()->schedule(schedule_selector(LoginProxy::scheduleHeartBeat),
		sgLoginProxy(),1.0,0,0.0,false);
}

void LoginProxy::scheduleHeartBeat(float dt)
{
	
	if (sgLoginProxy()->getLoginSuccess() && sgLoginProxy()->getIsAccepted() == false)
	{
		ReLoginDialog::show();
		sgLoginProxy()->sendEnterServerRqst();
		CCLOG("[trace]AppDelegate::applicationWillEnterForeground reconnect gameserver!");
	}
}

void LoginProxy::kickPlayer()
{
	// 玩家还在加载资源时
	//float fPropgress = sgResManager()->getProgress();

	if (sgPageFunction()->getCurrentUIType() > MSG_FUC_LOGIN)
	{
		mIsQueryAllStatus = false;
		mIsOther2RequestStatus = false;
		sgActControl()->setQueryLoginDay(0); //签到界面刷新
		
		sgPageFunction()->gotoFunction(MSG_FUC_LOGIN);
		sgMessageManager()->sendMessage(EMT_BUTTON_LOGINSCENE_UNVISIBLE, "");
		sgPageFunction()->gotoFunction(MSG_FUC_KICKPLAYER);
		sgNoticeControl()->setNewNotice(true);

		sgNoticeControl()->setNewActInform(true);

		sgUserObj()->cleanCheckPtDataMap();			//清除玩家关卡记录，只为了判断是否发送刷新全关卡消息

	}
	else
	{
		mIsNeedKickPlayer = true;
	}

	Director::getInstance()->getScheduler()->unschedule(schedule_selector(LoginProxy::updateHearBeat), sgLoginProxy());
	setServerState(E_SS_INVALID);
	sgNetManager()->setEncryMsg(false);
}

void LoginProxy::doResLoadOver()
{
	if (mIsNeedKickPlayer)
	{
		kickPlayer();
		mIsNeedKickPlayer = false;
	}
}

void LoginProxy::requestLoginKey()
{
	// Evix 是否重新请求秘钥？
	if (sgNetManager()->getEncryMsg() == true)
	{
		CCLOG("[trace]LoginProxy::requestLoginKey : Request login without encrpt key");
		if (!sgSession()->getConnOk() || sgUserObj()->getLoginServerDir().compare(sgSession()->getCurrConnIp().c_str()) != 0)
		{
			InitNetConnection(sgUserObj()->getLoginServerDir());
		}
		sgLoginProxy()->reallyLoginRqst();
		return;
	}

	// 本地没有key，请求服务器
	InitNetConnection(sgUserObj()->getLoginServerDir());

	if (!sgSession()->getConnOk())
	{
		CCLOG("[trace]LoginProxy::requestLoginKey : Connection is failed.");
		// 本地网络未连接
		if (false == GameJniHelper::getInstance()->isNetworkConnected())
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_NET_ERROR_TIP));
			return;
		}
		else
		{
			// 服务器问题，连接不上 
			requestLoginTimeoutCallback(0.0f);
		}
		return;
	}
	//连上了置0,不然下次会直接弹服务器维护中
	mRetryLoginServer = 0;

	std::string localKey = "key.bin";
	if (FileUtils::getInstance()->isFileExist(localKey))
	{
		CCLOG("[trace]LoginProxy::requestLoginKey : Send client key.");

		//跳过第一步 直接第二步
		sendClientKey();
	}
	else
	{
		CCLOG("[trace]LoginProxy::requestLoginKey : Send private key.");

		sendPKeyRest();
	}
}

void LoginProxy::analysisLoginDNS()
{
	if (0 == sgEnvSet("MGAME_OPEN_CHOSE_SERVER"))
	{
		std::string szDns;
		int port = 0;

		std::string szLoginDns = sgChannelApi()->getDefaultLoginDNS();
		if (szLoginDns.find(":") != std::string::npos)
		{
			port = atoi(szLoginDns.substr(szLoginDns.find(":") + 1).c_str());
			szDns = szLoginDns.substr(0, szLoginDns.find(":"));
		}

		hostent* phost = gethostbyname(szDns.c_str());
		if (phost)
		{
			string szLoginAddr = StringUtils::format("%s:%d", inet_ntoa(*((struct in_addr*)phost->h_addr)), port);
			sgUserObj()->setLoginServerDir(szLoginAddr);

			CCLOG("[trace]LoginProxy::analysisLoginDNS : analyze DNS success, %s", szLoginAddr.c_str());
		}
	}
}

bool LoginProxy::checkServerState(const ::fgame::CSLoginResp& loginResp)
{
	// 服务器停机状态
	if (loginResp.has_stopgameflag() == 1)
	{
		//1、不在停服白名单中， 去请求服务器状态页面
		//2、否则，进入游戏
		if (loginResp.stopgameflag() == 1)
		{
			showServerStateContent();
			CCLOG("[trace]LoginProxy::checkServerState : No in white list.");
			return false;
		}
	}
	else
	{
		// 设置灰度发布相关
		if (loginResp.has_grayrelease())
		{
			//灰度测试中
			if (loginResp.grayrelease() == 0)
			{
				//非白名单用户，不更新
				setIsUpdateResource(false);
				CCLOG("[trace]LoginProxy::checkServerState : No update.");
			}
			else
			{
				//白名单用户，更新
				setIsUpdateResource(true);
				CCLOG("[trace]LoginProxy::checkServerState : update.");
			}
		}
		else
		{
			//非灰度测试，所有人都更新
			setIsUpdateResource(true);
			CCLOG("[trace]LoginProxy::checkServerState : update all.");
		}
	}

	//登录服连接成功，缓存起来
	std::string szLoginAddr = sgUserObj()->getLoginServerDir();
	sgUserObj()->addLoginSrv2List(sgChannelApi()->getChannelId(), szLoginAddr);
	CCLOG("[trace]LoginProxy::checkServerState : Save login server = %s.", szLoginAddr.c_str());
	return true;
}

LoginProxy::serverState LoginProxy::getServerStateInfo()
{
	serverState stuState(0, "");

	int channelId = sgChannelApi()->getChannelId();
	string openId = sgUserObj()->getOpenID();
	string url_serverState = sgChannelApi()->getServerStateInfo();// "http://10.10.2.92/get_server.php?openid=%s&channelid=%d";

	char url_buff[256];
	memset(url_buff, 0, 256);
	sprintf(url_buff, url_serverState.c_str(), openId.c_str(), channelId);

	//下载serverState.json
	bool isSuccess = UpdateMgrImp::getInstance()->DownloadServerStateFile(url_buff);

	//解析serverState.json
	if (true == isSuccess)
	{
		//read file
		rapidjson::Document jsonFile;
		ssize_t size = 0;
		std::string load_str;

		unsigned char* titlech = FileUtils::getInstance()->getFileData("serverState.json", "r", &size);
		load_str = std::string((const char*)titlech, size);
		jsonFile.Parse<0>(load_str.c_str());

		if (jsonFile.HasParseError())
		{
			return stuState;	//false
		}

		stuState.m_state = DICTOOL->getIntValue_json(jsonFile, "ret");
		stuState.m_content = DICTOOL->getStringValue_json(jsonFile, "content");

		return stuState;
	}
	else
	{
		return stuState;
	}
}

void LoginProxy::showServerStateContent()
{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//	sgChannelApi()->channelLogout();
//	sgChannelApi()->setChannelInfo(0, 1);
//#endif
	serverState state = getServerStateInfo();
	if (state.m_content != "OK")
	{
		SrvDownNoticeDialog* pSrvDownNoticeDialog = dynamic_cast<SrvDownNoticeDialog*>(sgPageFunction()->createWindow(MSG_FUC_SRVDOWN_NOTICE, "SrvDownNoticeDialog.csb"));
		if (pSrvDownNoticeDialog)
		{
			pSrvDownNoticeDialog->setContent(state.m_content);
		}
	}
}

void LoginProxy::sendLogOut()
{
	unsigned int uCmd = fgame::CS_CMD_LOGOUT;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSLogoutReq* pRqst = msgPack.getC2SMsg()->mutable_logout_req();
	pRqst->set_uin(sgUserObj()->getUin());

	sgNetManager()->SendPkg(msgPack);

	Director::getInstance()->getScheduler()->unschedule(schedule_selector(LoginProxy::updateHearBeat), sgLoginProxy());
	setServerState(E_SS_INVALID);
	sgNetManager()->setEncryMsg(false);

	mIsQueryAllStatus = false;
	mIsOther2RequestStatus = false;
	sgActControl()->setQueryLoginDay(0); //签到界面刷新
	sgNoticeControl()->setNewActInform(true);
	sgUserObj()->clearGateState();
	sgRankingControl()->cleanup();

	mRetry = 0;
}
