
#include "DataEyeAgent.h"
#include "EnvSet.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "platform/android/jni/JniHelper.h"
#include "Android/include/DCAgent.h"
//#include "Android/include/DCAccountType.h"
//#include "Android/include/DCGender.h"
//#include "Android/include/DCTaskType.h"
#include "Android/include/DCJniHelper.h"
#endif


/*
const std::string DataEyeAgent::Level_Id = "CheckPoint_%d";

const std::string DataEyeAgent::EventLevel_Id = "Event_ChekcPoint_%d";
const std::string DataEyeAgent::EventLevel_Key_Uid = "Uid";
const std::string DataEyeAgent::EventLevel_Key_StarCount = "StarCount";
const std::string DataEyeAgent::EventLevel_Key_Result = "Result";
const std::string DataEyeAgent::EventLevel_Key_Perfect = "Perfect";

const std::string DataEyeAgent::EventBoss_Id = "Event_Boss_%d";
const std::string DataEyeAgent::EventBoss_Key_Unlock = "Unlock";
const std::string DataEyeAgent::EventBoss_Key_Uid = "Uid";
const std::string DataEyeAgent::EventBoss_Key_Result = "Result";

const std::string DataEyeAgent::EventChapter_Id = "Event_Chapter_%d";
const std::string DataEyeAgent::EventChapter_Key_Unlock = "Unlock";
const std::string DataEyeAgent::EventChapter_Key_Uid = "Uid";

const std::string DataEyeAgent::EventLostPoint_Id = "Event_Lost_Point";
const std::string DataEyeAgent::EventLostPoint_Key_Step = "Step";

const std::string DataEyeAgent::EventRMBBuy_Id = "Event_RMB_Buy";*/


DataEyeAgent::DataEyeAgent() : mEnable(false)
{
#ifdef MGAME_IN_APP_SDK
	mEnable = true;
#endif // MGAME_IN_APP_SDK
}


DataEyeAgent::~DataEyeAgent()
{

}


bool DataEyeAgent::initilized()
{
	if (!mEnable)
		return true;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCJniHelper::setJVM(cocos2d::JniHelper::getJavaVM());
#endif

	return true;
}


void DataEyeAgent::setVersion(const std::string& version)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAgent::setVersion(version.c_str());
#endif
}


void DataEyeAgent::reportError(const std::string& title, const std::string& error)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAgent::reportError(title.c_str(), error.c_str());
#endif
}


void DataEyeAgent::uploadNow()
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAgent::uploadNow();
#endif
}


std::string DataEyeAgent::getUID()
{
	if (!mEnable)
		return "";

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	return DCAgent::getUID();
#endif

	return "";
}


void DataEyeAgent::accountLogin(const std::string& accountId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAccount::login(accountId.c_str());
#endif
}


void DataEyeAgent::accountLogin(const std::string& accountId, const std::string& gameServer)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAccount::login(accountId.c_str(), gameServer.c_str());
#endif
}


void DataEyeAgent::accountLogout()
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAccount::logout();
#endif
}


std::string DataEyeAgent::accountGetId()
{
	if (!mEnable)
		return "";

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	return DCAccount::getAccountId();
#endif

	return "";
}


void DataEyeAgent::accountSetType(DataEyeAgent::AccountType accountType)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAccount::setAccountType(static_cast<DCAccountType>(accountType));
#endif
}


void DataEyeAgent::accountSetLevel(int level)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAccount::setLevel(level);
#endif
}


void DataEyeAgent::accountSetGender(DataEyeAgent::Gender gender)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAccount::setGender(static_cast<DCGender>(gender));
#endif
}


void DataEyeAgent::accountSetAge(int age)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAccount::setAge(age);
#endif
}


void DataEyeAgent::accountSetGameServer(const std::string& gameServer)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAccount::setGameServer(gameServer.c_str());
#endif
}


void DataEyeAgent::accountAddTag(const std::string& tag, const std::string& subTag)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAccount::addTag(tag.c_str(), subTag.c_str());
#endif
}


void DataEyeAgent::accountRemoveTag(const std::string& tag, const std::string& subTag)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCAccount::removeTag(tag.c_str(), subTag.c_str());
#endif
}


void DataEyeAgent::virtualCurrencyPaymentSuccess(const std::string& orderId, const std::string& iapId, double currencyAmount, const std::string& currencyType, const std::string& paymentType)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCVirtualCurrency::paymentSuccess(orderId.c_str(), iapId.c_str(), currencyAmount, currencyType.c_str(), paymentType.c_str());
#endif
}


void DataEyeAgent::virtualCurrencyPaymentSuccessInLevel(const std::string& orderId, const std::string& iapId, double currencyAmount, const std::string& currencyType, const std::string& paymentSuccess, const std::string& levelId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCVirtualCurrency::paymentSuccessInLevel(orderId.c_str(), iapId.c_str(), currencyAmount, currencyType.c_str(), paymentSuccess.c_str(), levelId.c_str());
#endif
}


void DataEyeAgent::itemBuy(const std::string& itemId, const std::string& itemType, int itemCount, long long virtualCurrency, const std::string& currencyType, const std::string& consumePoint)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCItem::buy(itemId.c_str(), itemType.c_str(), itemCount, virtualCurrency, currencyType.c_str(), consumePoint.c_str());
#endif
}


void DataEyeAgent::itemBuyInLevel(const std::string& itemId, const std::string& itemType, int itemCount, long long virtualCurrency, const std::string& currencyType, const std::string& consumePoint, const std::string& levelId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCItem::buyInLevel(itemId.c_str(), itemType.c_str(), itemCount, virtualCurrency, currencyType.c_str(), consumePoint.c_str(), levelId.c_str());
#endif
}


void DataEyeAgent::itemGet(const std::string& itemId, const std::string& itemType, int itemCount, const std::string& reason)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCItem::get(itemId.c_str(), itemType.c_str(), itemCount, reason.c_str());
#endif
}


void DataEyeAgent::itemGetInLevel(const std::string& itemId, const std::string& itemType, int itemCount, const std::string& reason, const std::string& levelId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCItem::getInLevel(itemId.c_str(), itemType.c_str(), itemCount, reason.c_str(), levelId.c_str());
#endif
}

void DataEyeAgent::itemConsume(const std::string& itemId, const std::string& itemType, int itemCount, const std::string& reason)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCItem::consume(itemId.c_str(), itemType.c_str(), itemCount, reason.c_str());
#endif
}

void DataEyeAgent::itemConsumeInLevel(const std::string& itemId, const std::string& itemType, int itemCount, const std::string& reason, const std::string& levelId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCItem::consumeInLevel(itemId.c_str(), itemType.c_str(), itemCount, reason.c_str(), levelId.c_str());
#endif
}


void DataEyeAgent::taskBegin(const std::string& taskId, DataEyeAgent::TaskType type)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCTask::begin(taskId.c_str(), static_cast<DCTaskType>(type));
#endif
}

void DataEyeAgent::taskComplete(const std::string& taskId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCTask::complete(taskId.c_str());
#endif
}


void DataEyeAgent::taskFail(const std::string& taskId, const std::string& reason)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCTask::fail(taskId.c_str(), reason.c_str());
#endif
}


void DataEyeAgent::onEventBeforeLogin(const std::string& eventId, EventParamMap* map, long long duration)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEventBeforeLogin(eventId.c_str(), map, duration);
#endif
}


void DataEyeAgent::onEventCount(const std::string& eventId, int count)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEventCount(eventId.c_str(), count);
#endif
}


void DataEyeAgent::onEvent(const std::string& eventId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEvent(eventId.c_str());
#endif
}


void DataEyeAgent::onEvent(const std::string& eventId, const std::string& label)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEvent(eventId.c_str(), label.c_str());
#endif
}


void DataEyeAgent::onEvent(const std::string& eventId, DataEyeAgent::EventParamMap* map)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEvent(eventId.c_str(), map);
#endif
}


void DataEyeAgent::onEventDuration(const std::string& eventId, long long duration)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEventDuration(eventId.c_str(), duration);
#endif
}


void DataEyeAgent::onEventDuration(const std::string& eventId, const std::string& label, long long duration)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEventDuration(eventId.c_str(), label.c_str(), duration);
#endif
}


void DataEyeAgent::onEventDuration(const std::string& eventId, DataEyeAgent::EventParamMap* map, long long duration)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEventDuration(eventId.c_str(), map, duration);
#endif
}


void DataEyeAgent::onEventBegin(const std::string& eventId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEventBegin(eventId.c_str());
#endif
}


void DataEyeAgent::onEventBegin(const std::string& eventId, DataEyeAgent::EventParamMap* map)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEventBegin(eventId.c_str(), map);
#endif
}


void DataEyeAgent::onEventEnd(const std::string& eventId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEventEnd(eventId.c_str());
#endif
}


void DataEyeAgent::onEventBegin(const std::string& eventId, DataEyeAgent::EventParamMap* map, const std::string& flag)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEventBegin(eventId.c_str(), map, flag.c_str());
#endif
}


void DataEyeAgent::onEventEnd(const std::string& eventId, const std::string& flag)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCEvent::onEventEnd(eventId.c_str(), flag.c_str());
#endif
}


void DataEyeAgent::coinSetCoinNum(long long total, const std::string& coinType)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCCoin::setCoinNum(total, coinType.c_str());
#endif
}


void DataEyeAgent::coinLost(const std::string& reason, const std::string& coinType, long long lost, long long left)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCCoin::lost(reason.c_str(), coinType.c_str(), lost, left);
#endif
}


void DataEyeAgent::coinLostInLevel(const std::string& reason, const std::string& coinType, long long lost, long long left, const std::string& levelId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCCoin::lostInLevel(reason.c_str(), coinType.c_str(), lost, left, levelId.c_str());
#endif
}


void DataEyeAgent::coinGain(const std::string& reason, const std::string& coinType, long long gain, long long left)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCCoin::gain(reason.c_str(), coinType.c_str(), gain, left);
#endif
}


void DataEyeAgent::coinGainInLevel(const std::string& reason, const std::string& coinType, long long gain, long long left, const std::string& levelId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCCoin::gainInLevel(reason.c_str(), coinType.c_str(), gain, left, levelId.c_str());
#endif
}


void DataEyeAgent::levelsBegin(const std::string& levelId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCLevels::begin(levelId.c_str());
#endif
}


void DataEyeAgent::levelsComplete(const std::string& levelId)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCLevels::complete(levelId.c_str());
#endif
}


void DataEyeAgent::levelsFail(const std::string& levelId, const std::string& failPoint)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCLevels::fail(levelId.c_str(), failPoint.c_str());
#endif
}


void DataEyeAgent::cardsGamePlay(const std::string& roomId, const std::string& roomType, const std::string& coinType, long long lostOrGainCoin, long long taxCoin, long long leftCoin)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCCardsGame::play(roomId.c_str(), roomType.c_str(), coinType.c_str(), lostOrGainCoin, taxCoin, leftCoin);
#endif
}

void DataEyeAgent::cardsGameLost(const std::string& roomId, const std::string& roomType, const std::string& coinType, long long lostCoin, long long leftCoin)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCCardsGame::lost(roomId.c_str(), roomType.c_str(), coinType.c_str(), lostCoin, leftCoin);
#endif
}


void DataEyeAgent::cardsGameGain(const std::string& roomId, const std::string& roomType, const std::string& coinType, long long gainCoin, long long leftCoin)
{
	if (!mEnable)
		return;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	DCCardsGame::gain(roomId.c_str(), roomType.c_str(), coinType.c_str(), gainCoin, leftCoin);
#endif
}
