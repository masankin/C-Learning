
#include "TalkingDataAgent.h"
#include "TalkingData/include/TalkingData.h"
#include "EnvSet.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include "TalkingData/android/TDGAJniHelper.h"
#endif


TalkingDataAgent::TalkingDataAgent() : mEnable(false)
, mTDAccount(nullptr)
{
#ifdef MGAME_IN_APP_SDK
	mEnable = true;
#endif // MGAME_IN_APP_SDK

}


TalkingDataAgent::~TalkingDataAgent()
{

}


bool TalkingDataAgent::initilized()
{
	if(!mEnable)
		return true;


#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

	TDGAJniHelper::setJavaVM(cocos2d::JniHelper::getJavaVM());

	mTDAccount = TDCCAccount::setAccount(TDCCTalkingDataGA::getDeviceId());

#endif
	
	return true;
}


void TalkingDataAgent::onStart(const std::string& appId, const std::string& channelId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCTalkingDataGA::onStart(appId.c_str(), channelId.c_str());
#endif

}

void TalkingDataAgent::onEvent(const std::string& eventId, TalkingDataAgent::EventParamMap* map/* = NULL*/)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCTalkingDataGA::onEvent(eventId.c_str(), map);
#endif

}


void TalkingDataAgent::setLatitude(double latitude, double longitude)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#endif

}


std::string TalkingDataAgent::getDeviceId()
{
	if (!mEnable)
		return "";

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	return TDCCTalkingDataGA::getDeviceId();
#endif

	return "";
}


void TalkingDataAgent::onKill()
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCTalkingDataGA::onKill();
#endif

}


void TalkingDataAgent::setVerboseLogDisabled()
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCTalkingDataGA::setVerboseLogDisabled();
#endif

}


void TalkingDataAgent::setVerboseLogEnabled()
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCTalkingDataGA::setVerboseLogEnabled();
#endif

}


void TalkingDataAgent::setAccount(const std::string& accountId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	mTDAccount = TDCCAccount::setAccount(accountId.c_str());
#endif

}


void TalkingDataAgent::accountSetName(const std::string& accountName)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	if (nullptr != mTDAccount)
		mTDAccount->setAccountName(accountName.c_str());
#endif

}


void TalkingDataAgent::accountSetType(TalkingDataAgent::AccountType accountType)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	if (nullptr != mTDAccount)
		mTDAccount->setAccountType(static_cast<TDCCAccount::TDCCAccountType>(accountType));
#endif

}


void TalkingDataAgent::accountSetLevel(int level)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	if (nullptr != mTDAccount)
		mTDAccount->setLevel(level);
#endif

}


void TalkingDataAgent::accountSetGender(TalkingDataAgent::Gender gender)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	if (nullptr != mTDAccount)
		mTDAccount->setGender(static_cast<TDCCAccount::TDCCGender>(gender));
#endif

}


void TalkingDataAgent::accountSetAge(int age)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	if (nullptr != mTDAccount)
		mTDAccount->setAge(age);
#endif

}


void TalkingDataAgent::accountSetGameServer(const std::string& gameServer)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	if (nullptr != mTDAccount)
		mTDAccount->setGameServer(gameServer.c_str());
#endif

}


void TalkingDataAgent::missionBegin(const std::string& missionId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCMission::onBegin(missionId.c_str());
#endif

}


void TalkingDataAgent::missionCompleted(const std::string& missionId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#endif

}


void TalkingDataAgent::missionFailed(const std::string& missionId, const std::string& failedCause)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCMission::onFailed(missionId.c_str(), failedCause.c_str());
#endif

}


void TalkingDataAgent::itemPurchase(const std::string& item, int number, double price)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCItem::onPurchase(item.c_str(), number, price);
#endif

}


void TalkingDataAgent::itemUse(const std::string& item, int number)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCItem::onUse(item.c_str(), number);
#endif

}


void TalkingDataAgent::virtualCurrencyChargeRequest(const std::string& orderId, const std::string& iapId, double currencyAmount, const std::string& currencyType, double virtualCurrencyAmount, const std::string& paymentType)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCVirtualCurrency::onChargeRequest(orderId.c_str(), iapId.c_str(), currencyAmount, currencyType.c_str(), virtualCurrencyAmount, paymentType.c_str());
#endif

}


void TalkingDataAgent::virtualCurrencyChargeSuccess(const std::string& orderId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCVirtualCurrency::onChargeSuccess(orderId.c_str());
#endif

}


void TalkingDataAgent::virtualCurrencyReward(double currencyAmount, const std::string& reason)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	TDCCVirtualCurrency::onReward(currencyAmount, reason.c_str());
#endif
}
