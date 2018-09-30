#include "GameJniHelper.h"
#include "Cocos2d.h"

USING_NS_CC;

#if( CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>
#endif


GameJniHelper* GameJniHelper::s_instance = nullptr;

GameJniHelper* GameJniHelper::getInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new GameJniHelper;
	}
	return s_instance;
}

void GameJniHelper::restartGame() {
	CCLOG("[trace]GameInf::doRestartGame");
#if( CC_TARGET_PLATFORM ==CC_PLATFORM_ANDROID)  
	JniMethodInfo methodInfo; //用于获取函数体  
	bool isHave = JniHelper::getStaticMethodInfo(methodInfo, "com/liuyi/channel/JavaHelper", "restartGame", "()V");
	if (isHave) {
		methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
	}
	else  {
		CCLOG("[error]GameInf::doRestartGame,get jni method fail!!!");
	}
#endif 
}


void GameJniHelper::reloadAPK(const char * apkName) {
	CCLOG("[trace]GameInf::reloadAPK");
#if( CC_TARGET_PLATFORM ==CC_PLATFORM_ANDROID)  
	JniMethodInfo methodInfo; //用于获取函数体  
	bool isHave = JniHelper::getStaticMethodInfo(methodInfo, "com/liuyi/channel/JavaHelper", "reloadAPK", "(Ljava/lang/String;)V");
	if (isHave) {
		jstring stringArg1 = methodInfo.env->NewStringUTF(apkName);
		methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID,stringArg1);
	}
	else  {
		CCLOG("[error]GameInf::reloadAPK,get jni method fail!!!");
	}
#endif 
}

#ifdef __cplusplus
extern "C"{
#endif

std::string GameJniHelper::getCode() {

#if( CC_TARGET_PLATFORM ==CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo; //用于获取函数体  
	bool isHave = JniHelper::getStaticMethodInfo(methodInfo, "com/liuyi/channel/JavaHelper", "getCode", "()Ljava/lang/String;");
	if (isHave) {

		jobject jObj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
		jstring jstr = (jstring)(jObj);
		std::string str = JniHelper::jstring2string(jstr);
		return str;
	}
	else  {
		CCLOG("[error]GameInf::getCode,get jni method fail!!!");
	}
#endif
	return "";
}


std::string GameJniHelper::getSDCardPath() {

#if( CC_TARGET_PLATFORM ==CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo; //用于获取函数体  
	bool isHave = JniHelper::getStaticMethodInfo(methodInfo, "com/liuyi/channel/JavaHelper", "getSDCardPath", "()Ljava/lang/String;");
	if (isHave) {
		jobject jObj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
		jstring jstr = (jstring)(jObj);
		std::string str = JniHelper::jstring2string(jstr);
		return str;
	}
	else  {
		CCLOG("[error]GameInf::getSDCardPath,get jni method fail!!!");
	}
#endif
	return "";
}
#ifdef __cplusplus
}

bool GameJniHelper::isNetworkConnected()
{
#if( CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)  
	//1. 获取activity静态对象
	cocos2d::JniMethodInfo methodInfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(methodInfo,
		"com/liuyi/channel/NetworkManager",
		"getInstance",
		"()Ljava/lang/Object;");

	jobject activityObj;

	if (isHave)
	{
		//调用静态函数getInstance，获取java类对象。
		activityObj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
	}

	//2. 查找isNetworkConnected接口，获取其函数信息，并用jobj调用
	isHave = cocos2d::JniHelper::getMethodInfo(methodInfo, "com/liuyi/channel/NetworkManager", "isNetworkConnected", "()Z");

	if (!isHave)
	{
		CCLOG("jni:isNetworkConnected 函数不存在");
	}
	else
	{
		//调用此函数
		bool bNetConnected = methodInfo.env->CallBooleanMethod(activityObj, methodInfo.methodID);
		return bNetConnected;
	}
#endif 

	return true;
}

bool GameJniHelper::isWifiConnected()
{
#if( CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)  
	//1. 获取activity静态对象
	cocos2d::JniMethodInfo methodInfo;
	bool isHave = cocos2d::JniHelper::getStaticMethodInfo(methodInfo,
		"com/liuyi/channel/NetworkManager",
		"getInstance",
		"()Ljava/lang/Object;");

	jobject activityObj;

	if (isHave)
	{
		//调用静态函数getInstance，获取java类对象。
		activityObj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
	}

	//2. 查找isWifiConnected接口，获取其函数信息，并用jobj调用
	isHave = cocos2d::JniHelper::getMethodInfo(methodInfo, "com/liuyi/channel/NetworkManager", "isWifiConnected", "()Z");

	if (!isHave)
	{
		CCLOG("jni:isWifiConnected 函数不存在");
	}
	else
	{
		//调用此函数
		bool bWifiConnected = methodInfo.env->CallBooleanMethod(activityObj, methodInfo.methodID);

		return bWifiConnected;
	}
#endif 

	return true;
}

#endif
