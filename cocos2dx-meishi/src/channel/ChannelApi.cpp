
#include "ChannelApi.h"
#include "../core/MessageManager.h"
#include "EnvSet.h"
#include "game/ui/PageFunction.h"
#include "net/UserObj.h"
#include "core/SoundManager.h"
#include "sync/SyncControl.h"
#include "data/TableManager.h"
#include "HelperFunc.h"
#include "CCLuaEngine.h"
#include "GameJniHelper.h"
#include "game/utils/GameDefault.h"
#include "game/utils/Comm.h"
#ifdef MGAME_PLATFORM_MPLUS
#include "game/utils/Comm.h"
#include "friend/FriendControl.h"
#endif


USING_NS_CC;

#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
#include "platform/android/jni/ChannelJni.h"
#endif


/*
ePaySdkType ChannelApi::getPaySdkType()
{
	ePaySdkType pRet;
	switch (mChannelId)
	{
	case 3:
		pRet = E_PAY_JIDI;
		break;
	case 4:
		pRet = E_PAY_DIANXING;
		break;
	default:
		pRet = E_PAY_ONLING;
		break;
	}
	return pRet;
}*/

ChannelApi::ChannelApi() :  mIsOnlineSdk(true), mXGPushToken(""), 
mSession(""), mPayType(ePaySdkType::E_PAY_ONLING), m_sImei(""), m_sVersion(""), mPayToken(""), mChannelId(0), mIsDiffAccount(false), mAccountChoice(0)
{

}

bool ChannelApi::init()
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM

	cocos2d::EventListenerCustom* eventCustomListener = nullptr;

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_LOGIN_EVENT_NAME, CC_CALLBACK_1(ChannelApi::loginCallback, this));

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_LOGOUT_EVENT_NAME, CC_CALLBACK_1(ChannelApi::logoutCallback, this));

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_PAY_EVENT_NAME, CC_CALLBACK_1(ChannelApi::payCallback, this));

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_QUIT_EVENT_NAME, CC_CALLBACK_1(ChannelApi::quitCallback, this));

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_ACCOUNT_EVENT_NAME, CC_CALLBACK_1(ChannelApi::accountCallback, this));

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_PIANTOU_FINISH, CC_CALLBACK_1(ChannelApi::playPiantoFinishCallback, this));

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_WAKEUP_EVENT_NAME, CC_CALLBACK_1(ChannelApi::wakeupCallback, this));

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_PERSON_INFO, CC_CALLBACK_1(ChannelApi::getChannelPersonInfo, this));

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_SHARE_SUCCESS, CC_CALLBACK_1(ChannelApi::shacreSuccessCallback, this));
	
#ifdef MGAME_PLATFORM_MPLUS
	Director::getInstance()->getEventDispatcher()->addCustomEventListener(CHANNEL_NOTIFY_GAMEINFO_EVENT_NAME, CC_CALLBACK_1(ChannelApi::notifyGameInfo, this));
#endif	

#elif CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM
	loginCallback(NULL);
#elif CC_PLATFORM_IOS == CC_TARGET_PLATFORM
    loginCallback(NULL);
#endif

	return true;
}



bool ChannelApi::channelLogin(const std::string& token)
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	return onChannelLoginJNI(token.c_str());
#endif

	return true;
}

void ChannelApi::channelLogout()
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	onChannelLogoutJNI();
#endif
}

void ChannelApi::channelPay(int id, std::string name, float price, std::string describe)
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	onChannelPayJNI(id,name,price,describe);
#endif

	CCLOG("[trace]ChannelApi::channelPay id:%d name:%s, price:%f, desc:%s!", id, name.c_str(), price, describe.c_str());
}

void ChannelApi::channelQuit()
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	onChannelQuitJNI();
#endif

}

void ChannelApi::loginCallback(cocos2d::EventCustom* customData)
{
	std::string szSession = "";
	std::string szXGToken = ""; 
	std::string  szOpenID = "";
	int iChannelID, iPayType;
	std::string szImei = "";
	std::string szVersion = "";

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	if (nullptr == customData)
		return;

	auto info = static_cast<ChannelLoginCallbackInfo*>(customData->getUserData());
	if (info)
	{
		szSession = info->token;
		szXGToken = info->XGPushToken;
		szOpenID = info->uid;

		iChannelID = info->channleId;
		iPayType = info->mobileType;

		szVersion = info->version;
		szImei = info->imei;
	}

#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    szOpenID = HelperFunc::getComputeID()+"21";
	
	iChannelID = ChannelID_Windows;
	iPayType = OperatorID_Windows;

	szVersion = "S_test";
	szImei = "000000";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    iChannelID = ChannelID_Windows;
    iPayType = OperatorID_Windows;
    
    szVersion = "S_test";
    szImei = "000000";

	std::string oId = USER_DEFAULT("USER_OPENID");
    if(oId.empty())
    {
        //创建 随机账号
        long long int r = random(1, 100000000);
        std::string oId =StringUtils::format("%lld",r);
        SAVE_USER_DEFAULT("USER_OPENID",oId );
        //UserDefault::getInstance()->flush();
        szOpenID = oId;
    }else
         szOpenID = oId;
    
#endif

	setSession(szSession);
	setXGPushToken(szXGToken);

	sgUserObj()->setOpenID(szOpenID);
	
	setChannelInfo(iChannelID, iPayType);

	m_sVersion = szVersion;
	m_sImei = szImei;
	CCLOG("[trace]ChannelApi::loginCallback openID is %s  # szOpenID is %s", sgUserObj()->getOpenID().c_str(), szOpenID.c_str());

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	onChannelGetUrlJNI(this->getSDKChargeURL());
#endif

	sgMessageManager()->sendMessage(EMT_SDKLOGIN_SUCCESS,"");
	sgMessageManager()->sendMessage(EMT_GAME_LOGIN_FLOW, StringUtils::format("%d 1 %d", MessageGameLoginFlow::FlowType::ChannelLogin, iChannelID));
}



void ChannelApi::logoutCallback(cocos2d::EventCustom* customData)
{
	if (nullptr == customData)
		return;

#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	auto info = static_cast<ChannelLogoutCallbackInfo*>(customData->getUserData());
	if (info)
	{
		CCLOG("ChannelApi::logoutCallback %s, %s", info->ret ? "True" : "False", info->param1.c_str());

		if(info->ret)
		{
			std::string strParam = StringUtils::format("%d %s", info->ret ? 1 : 0, info->param1.c_str());
			sgMessageManager()->sendMessage(EMT_CHANNEL_LOGOUT, strParam);
#ifdef MGAME_PLATFORM_MPLUS
			//写入登录类类型 CHANNEL_LOGOUT
			SAVE_USER_DEFAULT_INT(CHANNEL_LOGIN_TYPE, TYPE_LOGOUT);
			Director::sharedDirector()->end();
#endif
		}
	}
#endif
}

void ChannelApi::payCallback(cocos2d::EventCustom* customData)
{
	if (nullptr == customData)
		return;

#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	auto info = static_cast<ChannelPayCallbackInfo*>(customData->getUserData());
	if (info && info->result)
	{ 
		if (info->type == 0)
		{
			//单机SDK 
			sgMessageManager()->sendMessage(EMT_SHOP_PAYCOMPLENT,
				StringUtils::format("%d,%s",info->productId,info->orderId.c_str()));
			/*sgSyncControl()->addSyncGetDiamondData(info->orderId, info->productId, 0, info->mobileType);
			sgSyncControl()->sendSyncGetDiamondRqst();*/
		}
	}
#endif
}

void ChannelApi::quitCallback(cocos2d::EventCustom* customData)
{
	if (nullptr == customData)
		return;

#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	auto info = static_cast<ChannelQuitCallbackInfo*>(customData->getUserData());
	if (info)
	{
		CCLOG("ChannelApi::quitCallback %s, %s", info->ret ? "True" : "False", info->param1.c_str());

		Director::getInstance()->end();
	}
#endif
}

void ChannelApi::accountCallback(cocos2d::EventCustom* customData)
{
	if (nullptr == customData)
		return;

#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM

	CCLOG("ChannelApi::accountCallback");

#endif
}


void ChannelApi::wakeupCallback(cocos2d::EventCustom* customData)
{
	CCLOG("[trace]ChannelApi::wakeupCallback  11111");
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	auto info = static_cast<ChannelWakeupInfo*>(customData->getUserData());
	if (info)
	{
		if (info->channelID == 1)
			mChannelId = ChannelID_Tencent_WeiXin;
		else if (info->channelID == 2)
			mChannelId = ChannelID_Tencent_ShouQ;
		else
			mChannelId = 0;
		mIsDiffAccount = info->isDiffAccount;
		mAccountChoice = info->choice;
	}
#endif
}

void ChannelApi::getChannelPersonInfo(cocos2d::EventCustom* customData)
{
	if (nullptr == customData)
		return;


#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	auto info = static_cast<ChannelTencentPersonInfo*>(customData->getUserData());
	if (info)
	{
		CCLOG("[trace]ChannelApi::getChannelPersonInfo %s, %s, %s", info->nickName.c_str(), info->gender.c_str(), info->pictureSmall.c_str());

		sgUserObj()->setHeadIconUrl(info->pictureMiddle);
		sgUserObj()->setChannelPlayerName(info->nickName);
	}
#endif
	
}


void ChannelApi::playPiantoFinishCallback(cocos2d::EventCustom* customData)
{
	if (nullptr == customData)
		return;

#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	sgSoundManager()->setCanPlayMusic(true);
#endif
}

const char* ChannelApi::getDirURL()
{
#define IP_DEFAULT "release"
#define IP_QQ "qq"
#define IP_WX "wechat"
#define IP_MPLUS "mplus"

	if (sgEnvSet("MGAME_OPEN_CHOSE_SERVER"))
	{ 
		static std::string szIP;
		szIP = USER_DEFAULT(USER_SERVER_IP);
		return szIP.c_str();
	}

	if (mChannelId == ChannelID_Tencent_ShouQ)
	{
		const char* pszChannelName = IP_QQ;
		return getUrlByName(pszChannelName);
	}
	else if (mChannelId == ChannelID_Tencent_WeiXin)
	{
		const char* pszChannelName = IP_WX;
		return getUrlByName(pszChannelName);
	}
	else
	{
		const char* pszChannelName = IP_DEFAULT;
		return getUrlByName(pszChannelName);
	}
}

const char* ChannelApi::getDefaultLogin()
{
	return getUrlByName("defaultLogin");
}

const char* ChannelApi::getDefaultLoginDNS()
{
	if (mChannelId == ChannelID_Tencent_ShouQ)
	{
		return getUrlByName("qqLoginDNS");
	}
	else if (mChannelId == ChannelID_Tencent_WeiXin)
	{
		return getUrlByName("wechatLoginDNS");
	}
	else
	{
		return "0.0.0.0:9004";//getUrlByName("qqLoginDNS");
	}
}

const char* ChannelApi::getEmergLoginDNS()
{
	if (mChannelId == ChannelID_Tencent_ShouQ)
	{
		return getUrlByName("qqEmergLoginDNS");
	}
	else if (mChannelId == ChannelID_Tencent_WeiXin)
	{
		return getUrlByName("wechatEmergLoginDNS");
	}
	else
	{
		return "0.0.0.0:9004";//getUrlByName("qqLoginDNS");
	}
}

const char* ChannelApi::getServerStateInfo()
{
	return getUrlByName("srvState");
}

const char* ChannelApi::getUrlByName(const char* sname)
{
	static std::string strUrl_default;
	strUrl_default.clear();

	const char* pszDirCfgJson = "dir_url.json";

	rapidjson::Document json;
	ssize_t size;
	unsigned char *pBytes = FileUtils::getInstance()->getFileData(pszDirCfgJson, "r", &size);
	std::string load_str = std::string((const char*)pBytes, size);
	json.Parse<0>(load_str.c_str());
	CC_SAFE_DELETE_ARRAY(pBytes);

	if (json.HasParseError()) {
		CCLOG("[error]ChannelApi::getUrlByName,json file %s,parse error %s", pszDirCfgJson, json.GetParseError());
		return "";
	}
	if (json.HasMember(sname) && json[sname].IsString()) {
		strUrl_default = json[sname].GetString();
	}
	else if (json.HasMember("default") && json["default"].IsString()){
		strUrl_default = json["default"].GetString();
	}

	CCLOG("[trace]ChannelApi::getUrlByName,in android,channel=[%s],dir=[%s]", sname, strUrl_default.c_str());
	return strUrl_default.c_str();
}

const char* ChannelApi::getSDKChargeURL()
{
	static std::string strUrl;
	strUrl.clear();

	char* pszChannelName = "";
	if (mChannelId == ChannelID_Tencent_ShouQ)
		pszChannelName = "qqSdkurl";
	else if (mChannelId == ChannelID_Tencent_WeiXin)
		pszChannelName = "wechatSdkurl";
	else
		pszChannelName = "qqSdkurl";
	
	const char* pszDirCfgJson = "dir_url.json";

	rapidjson::Document json;
	ssize_t size;
	unsigned char *pBytes = FileUtils::getInstance()->getFileData(pszDirCfgJson, "r", &size);
	std::string load_str = std::string((const char*)pBytes, size);
	json.Parse<0>(load_str.c_str());
	CC_SAFE_DELETE_ARRAY(pBytes);
	
	if (json.HasParseError()) {
		CCLOG("[error]ChannelApi::getSDKChargeURL,json file %s,parse error %s", pszDirCfgJson, json.GetParseError());
		return "";
	}
	if (json.HasMember(pszChannelName) && json[pszChannelName].IsString()) {
		strUrl = json[pszChannelName].GetString();
	}
	else if (json.HasMember("default") && json["default"].IsString()){
		strUrl = "http://203.195.185.157:888/lenovo/";
	}

	CCLOG("[trace]ChannelApi::getSDKChargeURL,in android,channel=[%s],dir=[%s]", pszChannelName, strUrl.c_str());
	return strUrl.c_str();
}

void ChannelApi::callLogin(int type)
{	
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
    /*std::string _addrs = getSDKChargeURL();
	onChannelGetUrlJNI(_addrs);*/
	onChannelCallLoginJNI(type);
#elif CC_PLATFORM_IOS ==  CC_TARGET_PLATFORM || CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM
    sgMessageManager()->sendMessage(EMT_SDKLOGIN_SUCCESS, "");
#endif
}

void ChannelApi::callMoreGame()
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	onChannelCallMoreGameJNI();
#endif
}

void ChannelApi::uploadPlayerInfo(std::string gameId, std::string service, std::string role, std::string grade)
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	onChannelCallUploadPlayerInfoJNI(gameId,service,role,grade);
#endif
}

void ChannelApi::uploadSDKInfo(std::string openId, std::string token)
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	onChannelCallUploadSDKInfo(openId, token);
#endif
}

void ChannelApi::uploadUin(std::string uin)
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	onChannelCallUploadUin(uin);
#endif
}

void ChannelApi::setChannelInfo(int channelId, int operatorId)
{
	mChannelId = channelId;
	mOperatorId = operatorId;
	switch (operatorId)
	{
	case eOperatorID::OperatorId_Default:
	case eOperatorID::OperatorId_OnlineSDK:
		mIsOnlineSdk = true;
		mPayType = E_PAY_ONLING;
		break;

	case eOperatorID::OperatorId_YDJD:
	case eOperatorID::OperatorId_YDMM:
	case eOperatorID::OperatorId_LT:
		mIsOnlineSdk = false;
		mPayType = E_PAY_JIDI;
		break;

	case eOperatorID::OperatorId_DX:
		mIsOnlineSdk = false;
		mPayType = E_PAY_DIANXING;
		break;

	case eOperatorID::OperatorID_Windows:
		mIsOnlineSdk = false;
		mPayType = E_PAY_ONLING;
		break;
	default:
		break;
	}
	mIsOnlineSdk = true;
	mPayType = E_PAY_ONLING;
}

std::string ChannelApi::getPlatformName() const
{
	CCLOG("[trace]ChannelApi::getPlatformName mChannelId:%d!", mChannelId);
	return sgTableManager()->getPlatformName(mChannelId);
}

void ChannelApi::callResLoadOver()
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM 
	onChannelCallLoadResOverJNI();
#endif
}


void ChannelApi::share(int type, std::string title, std::string content, std::string bgImageName, std::string ImagePath, float x, float y, float r)
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM 
	onChannelCallShare(type,title,content,bgImageName,ImagePath,x,y,r);
#endif

#if CC_PLATFORM_WIN32 ==  CC_TARGET_PLATFORM 
	sgMessageManager()->sendMessage(EMT_SHARE_SUCCESS, "");
	const char* pScript = __String::createWithFormat("sgShareControl():sendShareRewardReq()")->getCString();
	LuaEngine::getInstance()->executeString(pScript);
#endif
}

void ChannelApi::shacreSuccessCallback(cocos2d::EventCustom* customData)
{
	if (nullptr == customData)
		return;
	
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM 
	sgMessageManager()->sendMessage(EMT_SHARE_SUCCESS,"");
	const char* pScript = __String::createWithFormat("sgShareControl():sendShareRewardReq()")->getCString();
	LuaEngine::getInstance()->executeString(pScript);
#endif
}
std::string ChannelApi::getSDCardPath()
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM 
  return GameJniHelper::getInstance()->getSDCardPath();
#endif
  return "";
}

#ifdef MGAME_PLATFORM_MPLUS

void ChannelApi::notifyGameInfo(cocos2d::EventCustom* customData)
{
	CCLOG("[trace]ChannelApi::notifyGameInfo  11111");
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	auto info = static_cast<ChannelCallbackExtraInfo*>(customData->getUserData());
	if (info)
	{
		CCLOG("[trace]ChannelApi::notifyGameInfo %d, %s", info->iMessageId, info->param.c_str());

		switch (info->iMessageId)
		{
		case TYPE_LOGIN_FB:
		case TYPE_LOGIN_BINDING:
		{
			std::vector<std::string> v_playerInfo = splitStr(info->param, "--");
			if (v_playerInfo.size() == 2)
			{
				sgMplusControl()->setThirdPartyIconUrl(v_playerInfo[0]);
				sgMplusControl()->setThirdPartyIcon(v_playerInfo[1]);

				//写入本地登录类型为Fb登录
				//sgUserObj()->setLoginType(TYPE_LOGIN_FB);
				SAVE_USER_DEFAULT_INT(CHANNEL_LOGIN_TYPE, TYPE_LOGIN_FB);
			}
			break;
		}
		case TYPE_LOGIN_GUEST:
		{
			//写入本地登录类型
			sgMplusControl()->setThirdPartyIconUrl("");
			sgMplusControl()->setThirdPartyIcon("");
			SAVE_USER_DEFAULT_INT(CHANNEL_LOGIN_TYPE, TYPE_LOGIN_GUEST);
			break;
		}
		case TYPE_FRIEND_LIST:
		{
			sgFriendControl()->addFriendInfo(info->param);

			break;
		}
		case TYPE_SHOW_POPAD:
		{
			sgMplusControl()->sendWatchMovieAward();
			break;
		}
		default:
			break;
		}
	}
#endif
}

void ChannelApi::callGameInfo(int iMessageId, std::string param1, std::string param2)
{
	if (iMessageId == TYPE_SHARE_MSG)
	{
		param1 = getShareMsg(param1, param2, true);
	}
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM

	onChannelGameInfoJNI(iMessageId, param1);
#endif
}

std::string ChannelApi::getShareMsg(std::string param1, std::string param2,bool isAll)
{
	std::string strFilePath = "share.json";
	Data data = FileUtils::getInstance()->getDataFromFile(strFilePath);
	unsigned char* pBytes = data.getBytes();
	size_t size = data.getSize();

	std::string load_str = std::string((const char*)pBytes, size);
	rapidjson::Document json;
	json.Parse<0>(load_str.c_str());
	int ver = 0;
	if (json.HasParseError()) {
		CCLOG("[error]json file %s,parse error %s", strFilePath.c_str(), json.GetParseError());
		return "";
	}
	else {
		const rapidjson::Value& subVal = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(json, param1.c_str());
		if (!isAll)
		{
			std::string description = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(subVal, "description");
			return description;
		}
		std::string name = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(subVal, "name");
		std::string caption = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(subVal, "caption");
		if(param2.empty())
			param2 = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(subVal, "description");

		std::string link = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(subVal, "link");
		std::string picture = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(subVal, "picture");


		param1 = name + "--" + caption + "--" + param2 + "--" + link + "--" + picture;

		return param1;
	}
}

std::string ChannelApi::getShareConfig(std::string param)
{
	std::string strFilePath = "share.json";
	Data data = FileUtils::getInstance()->getDataFromFile(strFilePath);
	unsigned char* pBytes = data.getBytes();
	size_t size = data.getSize();

	std::string load_str = std::string((const char*)pBytes, size);
	rapidjson::Document json;
	json.Parse<0>(load_str.c_str());
	int ver = 0;
	if (json.HasParseError()) {
		CCLOG("[error]json file %s,parse error %s", strFilePath.c_str(), json.GetParseError());
		return "";
	}
	else {
		const rapidjson::Value& subVal = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(json, "config");
		std::string description = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(subVal, param.c_str());
		return description;
	}
	return "";
}




#endif
