#include "DCJniHelper.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
#include <pthread.h>
#include <android/log.h>

#define  LOG_TAG    "DCJniHelper"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static pthread_key_t m_key;

JavaVM* DCJniHelper::m_jvm = NULL;


void DCJniHelper::setJVM(JavaVM* jvm)
{
    pthread_t thisthread = pthread_self();
    LOGD("DCJniHelper::setJVM(%p), pthread_self() = %ld", jvm, thisthread);
    m_jvm = jvm;
    pthread_key_create(&m_key, NULL);
}
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
JNIEnv* DCJniHelper::getEnv()
{
    JNIEnv* env = (JNIEnv*)pthread_getspecific(m_key);
    if (NULL == env)
    {
        env = DCJniHelper::cacheEnv(m_jvm);
    }
    return env;
}

JNIEnv* DCJniHelper::cacheEnv(JavaVM* jvm)
{
	if(jvm == NULL)
	{
		LOGE("please call DCJniHelper::setJVM() first!!!!");
		return NULL;
	}
    JNIEnv* env = NULL;
    jint ret = jvm->GetEnv((void**)&env, JNI_VERSION_1_4);
    
    switch (ret)
    {
        case JNI_OK:
            pthread_setspecific(m_key, env);
            return env;
        case JNI_EDETACHED:
            if (jvm->AttachCurrentThread(&env, NULL) < 0)
            {
                LOGE("Failed to get the environment using AttachCurrentThread()");
                return NULL;
            }
            else
            {
                pthread_setspecific(m_key, env);
                return env;
            }
        case JNI_EVERSION:
            LOGE("JNI interface version 1.4 not supported");
        default:
            LOGE("Failed to get the environment using GetEnv()");
            return NULL;
    }
}

jclass DCJniHelper::getClassId(const char* className)
{
    if (NULL == className) {
        return NULL;
    }
    
    JNIEnv* env = DCJniHelper::getEnv();
    jclass _clazz = (jclass)env->FindClass(className);
    if (NULL == _clazz)
    {
        LOGE("Classloader failed to find class of %s", className);
        env->ExceptionClear();
    }
    return _clazz;
}
#endif

bool DCJniHelper::getStaticMethodInfo(DCJniMethodInfo& info, const char* className, const char* methodName, const char* methodParam)
{
    if ((NULL == className) ||
        (NULL == methodName) ||
        (NULL == methodParam)) {
        return false;
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    try
	{
		JNIEnv *env = DCJniHelper::getEnv();
		if (!env) {
			LOGE("Failed to get JNIEnv");
			return false;
		}
		
		jclass classID = DCJniHelper::getClassId(className);
		if (!classID) {
			LOGE("Failed to find class %s", className);
			env->ExceptionClear();
			return false;
		}
		
		jmethodID methodID = env->GetStaticMethodID(classID, methodName, methodParam);
		if (! methodID) {
			LOGE("Failed to find static method id of %s", methodName);
			env->ExceptionClear();
			return false;
		}
		
		info.classID = classID;
		info.env = env;
		info.methodID = methodID;
	}
	catch(exception e)
	{
		CCLOG("TRY CATCH EXCEPTION E");
	}
#endif
    return true;
}

bool DCJniHelper::getMethodInfo(DCJniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
{
    if ((NULL == className) ||
        (NULL == methodName) ||
        (NULL == paramCode)) {
        return false;
    }
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    try
	{
		JNIEnv *env = DCJniHelper::getEnv();
		if (!env) {
			LOGE("Failed to get JNIEnv");
			return false;
		}
		
		jclass classID = DCJniHelper::getClassId(className);
		if (! classID) {
			LOGE("Failed to find class %s", className);
			env->ExceptionClear();
			return false;
		}
		
		jmethodID methodID = env->GetMethodID(classID, methodName, paramCode);
		if (! methodID) {
			LOGE("Failed to find static method id of %s", methodName);
			env->ExceptionClear();
			return false;
		}
		
		methodinfo.classID = classID;
		methodinfo.env = env;
		methodinfo.methodID = methodID;
	}
	catch(exception e)
	{
		CCLOG("TRY CATCH EXCEPTION E");
	}
#endif
    return true;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
jobject DCJniHelper::cMapToJMap(std::map<string, string>* map)
{
    if (NULL == map)
    {
        return NULL;
    }
	DCJniMethodInfo methodInfo;
    if(!DCJniHelper::getMethodInfo(methodInfo, "java/util/HashMap", "<init>", "()V"))
    {
        return NULL;
    }
	jobject obj = methodInfo.env->NewObject(methodInfo.classID, methodInfo.methodID);

    if(!DCJniHelper::getMethodInfo(methodInfo, "java/util/HashMap", "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"))
    {
        return NULL;
    }
	std::map<string, string>::iterator it;
	for(it = map->begin(); it != map->end(); ++it)
	{
		jstring key = methodInfo.env->NewStringUTF(it->first.c_str());
		jstring value = methodInfo.env->NewStringUTF(it->second.c_str());
		methodInfo.env->CallObjectMethod(obj, methodInfo.methodID, key, value);
		methodInfo.env->DeleteLocalRef(key);
		methodInfo.env->DeleteLocalRef(value);
	}
	return obj;
}

string DCJniHelper::jstring2string(jstring jstr)
{
    if (NULL == jstr)
    {
        return NULL;
    }
    JNIEnv* env = DCJniHelper::getEnv();
    if (!env)
    {
        return NULL;
    }
    const char* chars = env->GetStringUTFChars(jstr, NULL);
    std::string ret(chars);
    env->ReleaseStringUTFChars(jstr, chars);
    return ret;
}
#endif
