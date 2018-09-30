#ifndef cocos2d_dcjnihelper_h
#define cocos2d_dcjnihelper_h
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
#include <jni.h>
#endif
#include <string>
#include <map>

using namespace std;

typedef struct _DCJniMethodInfo
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	JNIEnv* env;
	jclass classID;
	jmethodID methodID;
#endif
}DCJniMethodInfo;

class DCJniHelper
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
private:
	static JavaVM* m_jvm;
private:
    static JNIEnv* getEnv();
    static JNIEnv* cacheEnv(JavaVM* jvm);
    static jclass getClassId(const char* className);
#endif
public:
	
    static bool getStaticMethodInfo(DCJniMethodInfo& info, const char* className, const char* methodName, const char* methodParam);
	
    static bool getMethodInfo(DCJniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
    static jobject cMapToJMap(std::map<string, string>* map);
	
	static void setJVM(JavaVM* jvm);
	
    static string jstring2string(jstring jstr);
#endif
};
#endif
