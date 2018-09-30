#pragma once

#include "cocos2d.h"
#include "Singleton.h"
#include "game/Constant.h"

#ifdef MGAME_PLATFORM_MPLUS

#define CHANNEL_LOGIN_TYPE		"ChannelLoginType"

enum ChannelOperationType
{
	TYPE_LOGOUT = 0,
	TYPE_LOGIN_FB = 1,
	TYPE_LOGIN_GUEST = 2,
	TYPE_LOGIN_BINDING = 3,
	TYPE_FRIEND_LIST = 4,
	TYPE_DOWNLOAD_ICON = 5,
	TYPE_APPLY_FRIEND = 6,
	TYPE_INVITE_FRIEND = 7,
	TYPE_SHARE_MSG = 8,
	TYPE_USER_CENTER = 9,
	TYPE_KAP_COPY = 10,
	TYPE_PLAYER_INFO = 11,
	TYPE_SHOW_POPAD = 12,
	TYPE_REQUEST_HTTP = 13,
	TYPE_SHARE_MSG_TYPE = 14,
};
#endif

class ChannelApi
{
public:
	enum eOperatorID
	{
		OperatorId_Default = 0,
		OperatorId_OnlineSDK = 1,
		OperatorId_YDJD = 2,
		OperatorId_YDMM = 3,
		OperatorId_LT = 4,
		OperatorId_DX = 5,
		OperatorID_Windows = 999,
	};

	enum eChannelID
	{
		ChannelID_Default = 0,

		ChannelID_MoMo = 1,
		ChannelID_QiHu360 = 2,
		ChannelID_XiaoMi = 4,
		ChannelID_Lenovo = 5,
		ChannelID_Lenovo_LeShangDian = 501,
		ChannelID_Lenovo_YouXiZhongXin = 502,
		ChannelID_YiDong = 6,
		ChannelID_YiDongJiDi = 601,
		ChannelID_YiDongMM = 602,
		ChannelID_BaiDu = 7,
		ChannelID_BaiDu_91 = 701,
		ChannelID_BaiDu_ZhuShou = 702,
		ChannelID_BaiDu_DuoKu = 703,
		ChannelID_BaiDu_TieBa = 704,
		ChannelID_Tencent = 8,
		ChannelID_Tencent_YingYongBao = 801,
		ChannelID_Tencent_Brower = 802,
		ChannelID_Tencent_GameCenter = 803,
		ChannelID_Tencent_AppMarket = 804,
		ChannelID_Tencent_ShouQ = 805,
		ChannelID_Tencent_WeiXin = 806,
		ChannelID_UC = 9,
		ChannelID_Oppo = 10,
		ChannelID_Vivo = 11,
		ChannelID_HuaWei = 12,
		ChannelID_CoolPad = 13,
		ChannelID_GioNee = 14,
		ChannelID_AnZhi = 15,
		ChannelID_WaDouJia = 16,
		ChannelID_MuZhiWan = 17,
		ChannelID_IQiYi = 18,
		ChannelID_PPTV = 19,
		ChannelID_SouGou = 20,
		ChannelID_SouGou_ZhuShou = 2001,
		ChannelID_SouGou_ShuRuFa = 2002,
		ChannelID_SouGou_Brower = 2003,
		ChannelID_SouGou_SouSuo = 2004,
		ChannelID_SouGou_AppMarket = 2005,
		ChannelID_SouGou_GameLobby = 2006,
		ChannelID_Sina = 21,
		ChannelID_YouKu = 22,
		ChannelID_DianXin = 23,
		ChannelID_LenovoMM = 24,
		ChannelID_GameCenter = 25,
		ChannelID_MeiTu = 26,
		ChannelID_KuGou = 27,
		ChannelID_WifiKey = 28,
		ChannelID_MeiZu = 29,
		ChannelID_WoShop = 30,
		ChannelID_ZhangMeng = 31,
		ChannelID_SkyMobi = 32,
		ChannelID_SkyMobi_Market = 3201,
		ChannelID_SkyMobi_Game = 3202,
		ChannelID_DangLe = 33,
		ChannelID_4399 = 34,
		ChannelID_AppHui = 35,
		ChannelID_JinRiTouTiao = 36,
		ChannelID_Thunder = 37,
		ChannelID_ZhuoYi = 38,
		ChannelID_Opera = 39,
		ChannelID_XYHelper = 40,
		ChannelID_Letv = 41,
		ChannelID_YiWan = 42,
		ChannelID_YiWan_Push = 4201,
		ChannelID_YiWan_App = 4202,

		ChannelID_AbroadDefault = 100,
		ChannelID_MPlus = 101,

		ChannelID_Windows = 999,
	};

	enum eAccountChoice
	{
		E_CHOICE_NONE = 0,
		E_CHOICE_LOCAL = 1,
		E_CHOICE_OUTSIDE = 2,
	};

public:
	ChannelApi();

	~ChannelApi()
	{

	}

	bool init();
	
	bool channelLogin(const std::string& token);

	void channelLogout();

	void channelQuit();

	const char* getDirURL();
	const char* getDefaultLogin();
	const char* getDefaultLoginDNS();
	const char* getEmergLoginDNS();

	const char* getSDKChargeURL();
	const char* getServerStateInfo();
	std::string getSDCardPath();
	//by junior 
	void callLogin(int type);//通知发起SDK登录
	void channelPay(int id, std::string name, float price, std::string describe);;//通知发起SDK支付
	void callMoreGame();

	void uploadPlayerInfo(std::string gameId, std::string service, std::string role, std::string grade);//有些SDK需要把角色信息传给SDK他们自己的服务器，需在适当的时候调用此接口，所需的SDK有oppo
	void uploadSDKInfo(std::string openId, std::string token);//把SDK的openid和token传到java端
	void uploadUin(std::string uin);//把uin传给java端
	void callResLoadOver();
	void setChannelInfo(int channelId, int operatorId);
	int getChannelId() const { return mChannelId; }
	int getOperatorId() const { return mOperatorId; }

	ePaySdkType getPaySdkType() { return mPayType; }
	bool isOnlineSdk() const { return mIsOnlineSdk; }
	std::string getPlatformName() const;

	std::string getVersion() const { return m_sVersion; }
	std::string getImei() const { return m_sImei; }
	void share(int type, std::string title, std::string content, std::string bgImageName, std::string ImagePath, float x, float y, float r); //weixin or QQ share

#ifdef MGAME_PLATFORM_MPLUS
	//by airel
	void callGameInfo(int iMessageId, std::string param1, std::string param2 = "");
//ex: share.json ----> param1 = message1
	std::string getShareMsg(std::string param1, std::string param2 = "", bool isAll = false);
	std::string getShareConfig(std::string param);
	void notifyGameInfo(cocos2d::EventCustom* customData);  //通知通用信息
#endif
	
private:
	const char* getUrlByName(const char*);

	void loginCallback(cocos2d::EventCustom* customData);	//sdk返回
	void logoutCallback(cocos2d::EventCustom* customData);
	void payCallback(cocos2d::EventCustom* customData);
	void quitCallback(cocos2d::EventCustom* customData);
	void accountCallback(cocos2d::EventCustom* customData);	//发起切换账户
	void playPiantoFinishCallback(cocos2d::EventCustom* customData);	//片头结束回调
	void wakeupCallback(cocos2d::EventCustom* customData);	//平台唤起游戏回调
	void getChannelPersonInfo(cocos2d::EventCustom* customData); //SDK调用返回
	void shacreSuccessCallback(cocos2d::EventCustom* customData); //SDK调用返回
	CC_SYNTHESIZE(std::string, mSession, Session);

	CC_SYNTHESIZE(std::string, mPayToken, PayToken);
	CC_SYNTHESIZE(bool, mIsDiffAccount, IsDiffAccount);
	CC_SYNTHESIZE(int, mAccountChoice, AccountChoice);

	int mChannelId;
	int mOperatorId; // 运营商ID
	ePaySdkType mPayType;
	bool mIsOnlineSdk;

	std::string m_sVersion;
	std::string m_sImei;
	
	//CC_SYNTHESIZE(int, mChannelId, ChannelId);	// 平台类型 
	CC_SYNTHESIZE(std::string, mXGPushToken, XGPushToken); //信鸽推送token 
	//CC_SYNTHESIZE(std::string, mPlatformName, PlatformName);//平台名字
};


#define sgChannelApi() Singleton<ChannelApi>::Get()
