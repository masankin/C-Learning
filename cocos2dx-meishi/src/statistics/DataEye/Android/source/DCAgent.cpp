#include "DCAgent.h"
#include "DCJniHelper.h"

#define DCAGENT_CLASS              "com/dataeye/DCAgent"
#define DCACCOUNT_CLASS            "com/dataeye/DCAccount"
#define DCVIRTUALCURRENCY_CLASS    "com/dataeye/DCVirtualCurrency"
#define DCCOIN_CLASS               "com/dataeye/DCCoin"
#define DCEVENT_CLASS              "com/dataeye/DCEvent"
#define DCCARDGAME_CLASS           "com/dataeye/plugin/DCCardsGame"
#define DCLEVELS_CLASS             "com/dataeye/plugin/DCLevels"
#define DCCONFIGPARAMS_CLASS       "com/dataeye/DCCocos2dConfigParams"
#define DCITEM_CLASS               "com/dataeye/DCItem"
#define DCTASK_CLASS               "com/dataeye/DCTask"

using namespace cocos2d;

void DCAgent::setVersion(const char* version)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCAGENT_CLASS, "setVersion", "(Ljava/lang/String;)V"))
	{
		jstring jVersion = methodInfo.env->NewStringUTF(version);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jVersion);
        methodInfo.env->DeleteLocalRef(jVersion);
	}
#endif
}

void DCAgent::reportError(const char* title, const char* error)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCAGENT_CLASS, "reportError", "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring jTitle = methodInfo.env->NewStringUTF(title);
        jstring jError = methodInfo.env->NewStringUTF(error);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jTitle, jError);
        methodInfo.env->DeleteLocalRef(jTitle);
		methodInfo.env->DeleteLocalRef(jError);
	}
#endif
}

void DCAgent::uploadNow()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCAGENT_CLASS, "uploadNow", "()V"))
	{
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
	}
#endif
}

const char* DCAgent::getUID()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	static std::string ret = "";
    DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCAGENT_CLASS, "getUID", "()Ljava/lang/String;"))
	{
		jstring jUID = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
		ret = DCJniHelper::jstring2string(jUID);
		return ret.c_str();
	}
#endif
	return NULL;
}

void DCAccount::login(const char* accountId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCACCOUNT_CLASS, "login", "(Ljava/lang/String;)V"))
	{
		jstring jaccountId = methodInfo.env->NewStringUTF(accountId);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jaccountId);
		methodInfo.env->DeleteLocalRef(jaccountId);
	}
#endif
}

void DCAccount::login(const char* accountId, const char* gameServer)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCACCOUNT_CLASS, "login", "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring jaccountId = methodInfo.env->NewStringUTF(accountId);
		jstring jgameServer = methodInfo.env->NewStringUTF(gameServer);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jaccountId, jgameServer);
		methodInfo.env->DeleteLocalRef(jaccountId);
		methodInfo.env->DeleteLocalRef(jgameServer);
	}
#endif
}

void DCAccount::logout()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCACCOUNT_CLASS, "logout", "()V"))
	{
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
	}
#endif
}

const char* DCAccount::getAccountId()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	static std::string ret = "";
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCACCOUNT_CLASS, "getAccountId", "()Ljava/lang/String;"))
	{
		jstring jAccountId = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
		ret = DCJniHelper::jstring2string(jAccountId);
		return ret.c_str();
	}
#endif
	return NULL;
}

void DCAccount::setAccountType(DCAccountType accountType)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCACCOUNT_CLASS, "setAccountType", "(I)V"))
	{
		jint jType = accountType;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jType);
	}
#endif
}

void DCAccount::setLevel(int level)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCACCOUNT_CLASS, "setLevel", "(I)V"))
	{
		jint jLevel = level;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jLevel);
	}
#endif
}

void DCAccount::setGender(DCGender gender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	DCJniMethodInfo mi;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCACCOUNT_CLASS, "setGender", "(I)V"))
	{
		jint jGender = gender;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jGender);
	}
#endif
}

void DCAccount::setAge(int age)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCACCOUNT_CLASS, "setAge", "(I)V"))
	{
		jint jAge = age;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jAge);
	}
#endif
}

void DCAccount::setGameServer(const char* gameServer)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCACCOUNT_CLASS, "setGameServer", "(Ljava/lang/String;)V"))
	{
		jstring jgameServer = methodInfo.env->NewStringUTF(gameServer);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jgameServer);
		methodInfo.env->DeleteLocalRef(jgameServer);
	}
#endif
}

void DCAccount::addTag(const char* tag, const char* subTag)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
    DCJniMethodInfo methodInfo;
    if(DCJniHelper::getStaticMethodInfo(methodInfo, DCACCOUNT_CLASS, "addTag", "(Ljava/lang/String;Ljava/lang/String;)V"))
    {
        jstring jtag = methodInfo.env->NewStringUTF(tag);
        jstring jsubTag = methodInfo.env->NewStringUTF(subTag);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jtag, jsubTag);
        methodInfo.env->DeleteLocalRef(jtag);
        methodInfo.env->DeleteLocalRef(jsubTag);
    }
#endif
}

void DCAccount::removeTag(const char* tag, const char* subTag)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
    DCJniMethodInfo methodInfo;
    if(DCJniHelper::getStaticMethodInfo(methodInfo, DCACCOUNT_CLASS, "removeTag", "(Ljava/lang/String;Ljava/lang/String;)V"))
    {
        jstring jtag = methodInfo.env->NewStringUTF(tag);
        jstring jsubTag = methodInfo.env->NewStringUTF(subTag);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jtag, jsubTag);
        methodInfo.env->DeleteLocalRef(jtag);
        methodInfo.env->DeleteLocalRef(jsubTag);
    }
#endif
}

void DCVirtualCurrency::paymentSuccess(const char* orderId, const char* iapId, double currencyAmount, const char* currencyType, const char* paymentType)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCVIRTUALCURRENCY_CLASS, "paymentSuccess",
		"(Ljava/lang/String;Ljava/lang/String;DLjava/lang/String;Ljava/lang/String;)V"))
	{
		jstring jorderId = methodInfo.env->NewStringUTF(orderId);
		jstring jiapId = methodInfo.env->NewStringUTF(iapId);
		jdouble jCurrencyAmount = currencyAmount;
		jstring jcurrencyType = methodInfo.env->NewStringUTF(currencyType);
		jstring jpaymentType = methodInfo.env->NewStringUTF(paymentType);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jorderId, jiapId, jCurrencyAmount,
				jcurrencyType, jpaymentType);
		methodInfo.env->DeleteLocalRef(jorderId);
		methodInfo.env->DeleteLocalRef(jiapId);
		methodInfo.env->DeleteLocalRef(jcurrencyType);
		methodInfo.env->DeleteLocalRef(jpaymentType);
	}
#endif
}

void DCVirtualCurrency::paymentSuccessInLevel(const char* orderId, const char* iapId, double currencyAmount, const char* currencyType, const char* paymentType, const char* levelId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
    DCJniMethodInfo methodInfo;
    if(DCJniHelper::getStaticMethodInfo(methodInfo, DCVIRTUALCURRENCY_CLASS, "paymentSuccessInLevel",
                                        "(Ljava/lang/String;Ljava/lang/String;DLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    {
        jstring jorderId = methodInfo.env->NewStringUTF(orderId);
		jstring jiapId = methodInfo.env->NewStringUTF(iapId);
        jdouble jcurrencyAmount = currencyAmount;
        jstring jcurrencyType = methodInfo.env->NewStringUTF(currencyType);
        jstring jpaymentType = methodInfo.env->NewStringUTF(paymentType);
        jstring jlevelId = methodInfo.env->NewStringUTF(levelId);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jorderId, jiapId, jcurrencyAmount,
                                             jcurrencyType, jpaymentType, jlevelId);
        methodInfo.env->DeleteLocalRef(jorderId);
		methodInfo.env->DeleteLocalRef(jiapId);
        methodInfo.env->DeleteLocalRef(jcurrencyType);
        methodInfo.env->DeleteLocalRef(jpaymentType);
        methodInfo.env->DeleteLocalRef(jlevelId);
    }
#endif
}

void DCItem::buy(const char* itemId, const char* itemType, int itemCount, long long virtualCurrency, const char* currencyType, const char* consumePoint)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCITEM_CLASS, "buy", "(Ljava/lang/String;Ljava/lang/String;IJLjava/lang/String;Ljava/lang/String;)V"))
	{
		jstring jItemId = methodInfo.env->NewStringUTF(itemId);
		jstring jItemType = methodInfo.env->NewStringUTF(itemType);
		jstring jCurrencyType = methodInfo.env->NewStringUTF(currencyType);
		jstring jConsumePoint = methodInfo.env->NewStringUTF(consumePoint);
		jint jItemCount = itemCount;
		jlong jVirtualCurrency = virtualCurrency;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jItemId, jItemType, jItemCount, jVirtualCurrency, jCurrencyType, jConsumePoint);
		methodInfo.env->DeleteLocalRef(jItemId);
		methodInfo.env->DeleteLocalRef(jItemType);
		methodInfo.env->DeleteLocalRef(jCurrencyType);
		methodInfo.env->DeleteLocalRef(jConsumePoint);
	}
#endif
}

void DCItem::buyInLevel(const char* itemId, const char* itemType, int itemCount, long long virtualCurrency, const char* currencyType, const char* consumePoint, const char* levelId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
    DCJniMethodInfo methodInfo;
    if(DCJniHelper::getStaticMethodInfo(methodInfo, DCITEM_CLASS, "buyInLevel", "(Ljava/lang/String;Ljava/lang/String;IJLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    {
        jstring jItemId = methodInfo.env->NewStringUTF(itemId);
        jstring jItemType = methodInfo.env->NewStringUTF(itemType);
        jstring jCurrencyType = methodInfo.env->NewStringUTF(currencyType);
        jstring jConsumePoint = methodInfo.env->NewStringUTF(consumePoint);
        jstring jLevelId = methodInfo.env->NewStringUTF(levelId);
        jint jItemCount = itemCount;
        jlong jVirtualCurrency = virtualCurrency;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jItemId, jItemType, jItemCount, jVirtualCurrency, jCurrencyType, jConsumePoint, jLevelId);
        methodInfo.env->DeleteLocalRef(jItemId);
        methodInfo.env->DeleteLocalRef(jItemType);
        methodInfo.env->DeleteLocalRef(jCurrencyType);
        methodInfo.env->DeleteLocalRef(jConsumePoint);
        methodInfo.env->DeleteLocalRef(jLevelId);
    }
#endif
}

void DCItem::get(const char* itemId, const char* itemType, int itemCount, const char* reason)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCITEM_CLASS, "get", "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)V"))
	{
		jstring jItemId = methodInfo.env->NewStringUTF(itemId);
		jstring jItemType = methodInfo.env->NewStringUTF(itemType);
		jstring jReason = methodInfo.env->NewStringUTF(reason);
		jint jItemCount = itemCount;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jItemId, jItemType, jItemCount, jReason);
		methodInfo.env->DeleteLocalRef(jItemId);
		methodInfo.env->DeleteLocalRef(jItemType);
		methodInfo.env->DeleteLocalRef(jReason);
	}
#endif
}

void DCItem::getInLevel(const char* itemId, const char* itemType, int itemCount, const char* reason, const char* levelId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
    DCJniMethodInfo methodInfo;
    if(DCJniHelper::getStaticMethodInfo(methodInfo, DCITEM_CLASS, "getInLevel", "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;)V"))
    {
        jstring jItemId = methodInfo.env->NewStringUTF(itemId);
        jstring jItemType = methodInfo.env->NewStringUTF(itemType);
        jstring jReason = methodInfo.env->NewStringUTF(reason);
        jstring jLevelId = methodInfo.env->NewStringUTF(levelId);
        jint jItemCount = itemCount;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jItemId, jItemType, jItemCount, jReason, jLevelId);
        methodInfo.env->DeleteLocalRef(jItemId);
        methodInfo.env->DeleteLocalRef(jItemType);
        methodInfo.env->DeleteLocalRef(jReason);
        methodInfo.env->DeleteLocalRef(jLevelId);
    }
#endif
}

void DCItem::consume(const char* itemId, const char* itemType, int itemCount, const char* reason)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCITEM_CLASS, "consume", "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)V"))
	{
		jstring jItemId = methodInfo.env->NewStringUTF(itemId);
		jstring jItemType = methodInfo.env->NewStringUTF(itemType);
		jstring jReason = methodInfo.env->NewStringUTF(reason);
		jint jItemCount = itemCount;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jItemId, jItemType, jItemCount, jReason);
		methodInfo.env->DeleteLocalRef(jItemId);
		methodInfo.env->DeleteLocalRef(jItemType);
		methodInfo.env->DeleteLocalRef(jReason);
	}
#endif
}

void DCItem::consumeInLevel(const char* itemId, const char* itemType, int itemCount, const char* reason, const char* levelId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
    DCJniMethodInfo methodInfo;
    if(DCJniHelper::getStaticMethodInfo(methodInfo, DCITEM_CLASS, "consumeInLevel", "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;)V"))
    {
        jstring jItemId = methodInfo.env->NewStringUTF(itemId);
        jstring jItemType = methodInfo.env->NewStringUTF(itemType);
        jstring jReason = methodInfo.env->NewStringUTF(reason);
        jstring jLevelId = methodInfo.env->NewStringUTF(levelId);
        jint jItemCount = itemCount;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jItemId, jItemType, jItemCount, jReason, jLevelId);
        methodInfo.env->DeleteLocalRef(jItemId);
        methodInfo.env->DeleteLocalRef(jItemType);
        methodInfo.env->DeleteLocalRef(jReason);
        methodInfo.env->DeleteLocalRef(jLevelId);
    }
#endif
}

void DCTask::begin(const char* taskId, DCTaskType taskType)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCTASK_CLASS, "begin", "(Ljava/lang/String;I)V"))
	{
		jstring jTaskId = methodInfo.env->NewStringUTF(taskId);
		jint jTaskType = taskType;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jTaskId, jTaskType);
		methodInfo.env->DeleteLocalRef(jTaskId);
	}
#endif
}

void DCTask::complete(const char* taskId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCTASK_CLASS, "complete", "(Ljava/lang/String;)V"))
	{
		jstring jTaskId = methodInfo.env->NewStringUTF(taskId);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jTaskId);
		methodInfo.env->DeleteLocalRef(jTaskId);
	}
#endif
}

void DCTask::fail(const char* taskId, const char* reason)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCTASK_CLASS, "fail", "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring jTaskId = methodInfo.env->NewStringUTF(taskId);
		jstring jReason = methodInfo.env->NewStringUTF(reason);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jTaskId, jReason);
		methodInfo.env->DeleteLocalRef(jTaskId);
		methodInfo.env->DeleteLocalRef(jReason);
	}
#endif
}

void DCEvent::onEventBeforeLogin(const char* eventId, map<string, string>* map, long long duration)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEventBeforeLogin", "(Ljava/lang/String;Ljava/util/Map;J)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		jobject jmap = DCJniHelper::cMapToJMap(map);
		jlong jDuration = duration;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId, jmap, jDuration);
		methodInfo.env->DeleteLocalRef(jEventId);
		methodInfo.env->DeleteLocalRef(jmap);
	}
#endif
}

void DCEvent::onEventCount(const char* eventId, int count)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEventCount", "(Ljava/lang/String;I)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		jint jcount = count;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId, jcount);
		methodInfo.env->DeleteLocalRef(jEventId);
	}
#endif
}

void DCEvent::onEvent(const char* eventId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEvent", "(Ljava/lang/String;)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId);
		methodInfo.env->DeleteLocalRef(jEventId);
	}
#endif
}

void DCEvent::onEvent(const char* eventId, const char* label)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEvent", "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		jstring jLabel = methodInfo.env->NewStringUTF(label);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId, jLabel);
		methodInfo.env->DeleteLocalRef(jEventId);
		methodInfo.env->DeleteLocalRef(jLabel);
	}
#endif
}

void DCEvent::onEvent(const char* eventId, map<string, string>* map)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEvent", "(Ljava/lang/String;Ljava/util/Map;)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		jobject jmap = DCJniHelper::cMapToJMap(map);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId, jmap);
		methodInfo.env->DeleteLocalRef(jEventId);
		methodInfo.env->DeleteLocalRef(jmap);
	}
#endif
}

void DCEvent::onEventDuration(const char* eventId, long long duration)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEventDuration", "(Ljava/lang/String;J)V"))
	{
		jlong jduration = duration;
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId, jduration);
		methodInfo.env->DeleteLocalRef(jEventId);
	}
#endif
}

void DCEvent::onEventDuration(const char* eventId, const char* label, long long duration)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEventDuration", "(Ljava/lang/String;Ljava/lang/String;J)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		jstring jLabel = methodInfo.env->NewStringUTF(label);
		jlong jduration = duration;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId, jLabel, jduration);
		methodInfo.env->DeleteLocalRef(jEventId);
		methodInfo.env->DeleteLocalRef(jLabel);
	}
#endif
}

void DCEvent::onEventDuration(const char* eventId, map<string, string>* map, long long duration)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEventDuration", "(Ljava/lang/String;Ljava/util/Map;J)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		jobject jmap = DCJniHelper::cMapToJMap(map);
		jlong jduration = duration;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId, jmap, jduration);
		methodInfo.env->DeleteLocalRef(jEventId);
		methodInfo.env->DeleteLocalRef(jmap);
	}
#endif
}

void DCEvent::onEventBegin(const char* eventId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEventBegin", "(Ljava/lang/String;)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId);
		methodInfo.env->DeleteLocalRef(jEventId);
	}
#endif
}

void DCEvent::onEventBegin(const char* eventId, map<string, string>* map)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEventBegin", "(Ljava/lang/String;Ljava/util/Map;)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		jobject jmap = DCJniHelper::cMapToJMap(map);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId, jmap);
		methodInfo.env->DeleteLocalRef(jEventId);
		methodInfo.env->DeleteLocalRef(jmap);
	}
#endif
}

void DCEvent::onEventBegin(const char* eventId, map<string, string>* map, const char* flag)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEventBegin", "(Ljava/lang/String;Ljava/util/Map;Ljava/lang/String;)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		jobject jmap = DCJniHelper::cMapToJMap(map);
		jstring jFlag = methodInfo.env->NewStringUTF(flag);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId, jmap, jFlag);
		methodInfo.env->DeleteLocalRef(jEventId);
		methodInfo.env->DeleteLocalRef(jmap);
		methodInfo.env->DeleteLocalRef(jFlag);
	}
#endif
}

void DCEvent::onEventEnd(const char* eventId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEventEnd", "(Ljava/lang/String;)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId);
		methodInfo.env->DeleteLocalRef(jEventId);
	}
#endif
}

void DCEvent::onEventEnd(const char* eventId, const char* flag)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCEVENT_CLASS, "onEventEnd", "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring jEventId = methodInfo.env->NewStringUTF(eventId);
		jstring jFlag = methodInfo.env->NewStringUTF(flag);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventId, jFlag);
		methodInfo.env->DeleteLocalRef(jEventId);
		methodInfo.env->DeleteLocalRef(jFlag);
	}
#endif
}

void DCCoin::setCoinNum(long long coinNum, const char* coinType)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCOIN_CLASS, "setCoinNum", "(JLjava/lang/String;)V"))
	{
		jlong jCoinNum = coinNum;
        jstring jCoinType = methodInfo.env->NewStringUTF(coinType);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jCoinNum, jCoinType);
        methodInfo.env->DeleteLocalRef(jCoinType);
	}
#endif
}

void DCCoin::lost(const char* id, const char* coinType, long long lost, long long left)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCOIN_CLASS, "lost", "(Ljava/lang/String;Ljava/lang/String;JJ)V"))
	{
		jlong jLost = lost;
		jlong jLeft = left;
		jstring jId = methodInfo.env->NewStringUTF(id);
        jstring jCoinType = methodInfo.env->NewStringUTF(coinType);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jId, jCoinType, jLost, jLeft);
		methodInfo.env->DeleteLocalRef(jId);
        methodInfo.env->DeleteLocalRef(jCoinType);
	}
#endif
}

void DCCoin::lostInLevel(const char* id, const char* coinType, long long lost, long long left, const char* levelId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
    DCJniMethodInfo methodInfo;
    if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCOIN_CLASS, "lostInLevel", "(Ljava/lang/String;Ljava/lang/String;JJLjava/lang/String;)V"))
    {
        jlong jLost = lost;
        jlong jLeft = left;
        jstring jId = methodInfo.env->NewStringUTF(id);
        jstring jCoinType = methodInfo.env->NewStringUTF(coinType);
        jstring jLevelId = methodInfo.env->NewStringUTF(levelId);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jId, jCoinType, jLost, jLeft, jLevelId);
        methodInfo.env->DeleteLocalRef(jId);
        methodInfo.env->DeleteLocalRef(jCoinType);
        methodInfo.env->DeleteLocalRef(jLevelId);
    }
#endif
}

void DCCoin::gain(const char* id, const char* coinType, long long gain, long long left)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCOIN_CLASS, "gain", "(Ljava/lang/String;Ljava/lang/String;JJ)V"))
	{
		jlong jGain = gain;
		jlong jLeft = left;
		jstring jId = methodInfo.env->NewStringUTF(id);
        jstring jCoinType = methodInfo.env->NewStringUTF(coinType);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jId, jCoinType, jGain, jLeft);
		methodInfo.env->DeleteLocalRef(jId);
        methodInfo.env->DeleteLocalRef(jCoinType);
	}
#endif
}

void DCCoin::gainInLevel(const char* id, const char* coinType, long long gain, long long left, const char* levelId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
    DCJniMethodInfo methodInfo;
    if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCOIN_CLASS, "gainInLevel", "(Ljava/lang/String;Ljava/lang/String;JJLjava/lang/String;)V"))
    {
        jlong jGain = gain;
        jlong jLeft = left;
        jstring jId = methodInfo.env->NewStringUTF(id);
        jstring jCoinType = methodInfo.env->NewStringUTF(coinType);
        jstring jLevelId = methodInfo.env->NewStringUTF(levelId);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jId, jCoinType, jGain, jLeft, jLevelId);
        methodInfo.env->DeleteLocalRef(jId);
        methodInfo.env->DeleteLocalRef(jCoinType);
        methodInfo.env->DeleteLocalRef(jLevelId);
    }
#endif
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
extern "C"
{
    JNIEXPORT void JNICALL Java_com_dataeye_DCCocos2dConfigParams_updateComplete(JNIEnv *, jobject obj)
    {
		// NotificationCenter::getInstance()->postNotification(DCCONFIGPARAMS_UPDATE_COMPLETE);
        CCNotificationCenter::sharedNotificationCenter()->postNotification(DCCONFIGPARAMS_UPDATE_COMPLETE);
    }
}
#endif

void DCConfigParams::update()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCONFIGPARAMS_CLASS, "update", "()V"))
	{
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
	}
#endif
}

const char* DCConfigParams::getParameterString(const char* key, const char* defaultValue)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
    static std::string ret = "";
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCONFIGPARAMS_CLASS, "getParameterString", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"))
	{
		jstring jkey = methodInfo.env->NewStringUTF(key);
		jstring jdefaultValue = methodInfo.env->NewStringUTF(defaultValue);
		jstring result = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jkey, jdefaultValue);
		ret = DCJniHelper::jstring2string(result);
		methodInfo.env->DeleteLocalRef(jkey);
		methodInfo.env->DeleteLocalRef(jdefaultValue);
		return ret.c_str();
	}
#endif
    return defaultValue;
}

int DCConfigParams::getParameterInt(const char* key, int defaultValue)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCONFIGPARAMS_CLASS, "getParameterInt", "(Ljava/lang/String;I)I"))
	{
		jstring jkey = methodInfo.env->NewStringUTF(key);
		jint jdefaultValue = defaultValue;
		jint result = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID, jkey, jdefaultValue);
		methodInfo.env->DeleteLocalRef(jkey);
		return (int)result;
	}
#endif
    return defaultValue;
}

long long DCConfigParams::getParameterLong(const char* key, long long defaultValue)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCONFIGPARAMS_CLASS, "getParameterLong", "(Ljava/lang/String;J)J"))
	{
		jstring jkey = methodInfo.env->NewStringUTF(key);
		jlong jdefaultValue = defaultValue;
		jlong result = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID, jkey, jdefaultValue);
		methodInfo.env->DeleteLocalRef(jkey);
		return (long long)result;
	}
#endif
    return defaultValue;
}

bool DCConfigParams::getParameterBool(const char* key, bool defaultValue)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCONFIGPARAMS_CLASS, "getParameterBoolean", "(Ljava/lang/String;Z)Z"))
	{
		jstring jkey = methodInfo.env->NewStringUTF(key);
		jboolean jdefaultValue = defaultValue;
		jboolean result = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, jkey, jdefaultValue);
		methodInfo.env->DeleteLocalRef(jkey);
		return (bool)result;
	}
#endif
    return defaultValue;
}

void DCLevels::begin(const char* levelId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCLEVELS_CLASS, "begin", "(Ljava/lang/String;)V"))
	{
		jstring jLevelId = methodInfo.env->NewStringUTF(levelId);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jLevelId);
		methodInfo.env->DeleteLocalRef(jLevelId);
	}
#endif
}

void DCLevels::complete(const char* levelId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCLEVELS_CLASS, "complete", "(Ljava/lang/String;)V"))
	{
		jstring jLevelId = methodInfo.env->NewStringUTF(levelId);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jLevelId);
		methodInfo.env->DeleteLocalRef(jLevelId);
	}
#endif
}

void DCLevels::fail(const char* levelId, const char* failPoint)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCLEVELS_CLASS, "fail", "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring jLevelId = methodInfo.env->NewStringUTF(levelId);
		jstring jFailPoint = methodInfo.env->NewStringUTF(failPoint);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jLevelId, jFailPoint);
		methodInfo.env->DeleteLocalRef(jLevelId);
		methodInfo.env->DeleteLocalRef(jFailPoint);
	}
#endif
}

void DCCardsGame::play(const char* roomId, const char* roomType, const char* coinType, long long loseOrGain, long long tax, long long left)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCARDGAME_CLASS, "play", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JJJ)V"))
	{
		jstring jRoomId = methodInfo.env->NewStringUTF(roomId);
		jstring jId = methodInfo.env->NewStringUTF(roomType);
		jstring jCoinType = methodInfo.env->NewStringUTF(coinType);
		jlong jLoseOrGain = loseOrGain;
		jlong jTax = tax;
		jlong jLeft = left;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jRoomId, jId, jCoinType, jLoseOrGain, jTax, jLeft);
		methodInfo.env->DeleteLocalRef(jRoomId);
		methodInfo.env->DeleteLocalRef(jId);
		methodInfo.env->DeleteLocalRef(jCoinType);
	}
#endif
}

void DCCardsGame::lost(const char* roomId, const char* roomType, const char* coinType, long long lost, long long left)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCARDGAME_CLASS, "lost", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JJ)V"))
	{
		jstring jRoomId = methodInfo.env->NewStringUTF(roomId);
		jstring jId = methodInfo.env->NewStringUTF(roomType);
		jstring jCoinType = methodInfo.env->NewStringUTF(coinType);
		jlong jLost = lost;
		jlong jLeft = left;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jRoomId, jId, jCoinType, jLost, jLeft);
		methodInfo.env->DeleteLocalRef(jRoomId);
		methodInfo.env->DeleteLocalRef(jId);
		methodInfo.env->DeleteLocalRef(jCoinType);
	}
#endif
}

void DCCardsGame::gain(const char* roomId, const char* roomType, const char* coinType, long long gain, long long left)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	DCJniMethodInfo methodInfo;
	if(DCJniHelper::getStaticMethodInfo(methodInfo, DCCARDGAME_CLASS, "gain", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JJ)V"))
	{
		jstring jRoomId = methodInfo.env->NewStringUTF(roomId);
		jstring jId = methodInfo.env->NewStringUTF(roomType);
		jstring jCoinType = methodInfo.env->NewStringUTF(coinType);
		jlong jGain = gain;
		jlong jLeft = left;
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jRoomId, jId, jCoinType, jGain, jLeft);
		methodInfo.env->DeleteLocalRef(jRoomId);
		methodInfo.env->DeleteLocalRef(jId);
		methodInfo.env->DeleteLocalRef(jCoinType);
	}
#endif
}



