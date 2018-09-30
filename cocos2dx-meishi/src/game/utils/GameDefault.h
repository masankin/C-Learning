//
// GameDefault.h

#pragma once

#include "cocos2d.h"
USING_NS_CC;

#define USER_DEFAULT(__KEY__)  GameDefault::sharedGameDefault()->getStringForKey(__KEY__);
#define SAVE_USER_DEFAULT(__KEY__,__VALUE__) GameDefault::sharedGameDefault()->setStringForKey(__KEY__,__VALUE__);

#define USER_DEFAULT_INT(__KEY__) GameDefault::sharedGameDefault()->getIntegerForKey(__KEY__)
#define SAVE_USER_DEFAULT_INT(__KEY__,__VALUE__) GameDefault::sharedGameDefault()->setIntegerForKey(__KEY__,__VALUE__);

#define USER_DEFAULT_FLOAT(__KEY__) GameDefault::sharedGameDefault()->getFloatForKey(__KEY__)
#define SAVE_USER_DEFAULT_FLOAT(__KEY__,__VALUE__) GameDefault::sharedGameDefault()->setFloatForKey(__KEY__, __VALUE__);

#define USER_DEFAULT_BOOL(__KEY__,__DEFAULT__) GameDefault::sharedGameDefault()->getBoolForKey(__KEY__,__DEFAULT__);
#define SAVE_USER_DEFAULT_BOOL(__KEY__,__VALUE__) GameDefault::sharedGameDefault()->setBoolForKey(__KEY__,__VALUE__);

class GameDefault// : public UserDefault
{
public:
    
    GameDefault();
    ~GameDefault();
    
    static GameDefault* sharedGameDefault();
    
    
    
    std::string getStringForKey(const char* pKey);
    void        setStringForKey(const char* pKey, const std::string & value);
    
    
    float       getFloatForKey(const char* pKey);
    void        setFloatForKey(const char* pKey, float value);
    
    
    int         getIntegerForKey(const char* pKey);
    void        setIntegerForKey(const char* pKey, int value);
    
    
    bool        getBoolForKey(const char* pKey,bool value);
    void        setBoolForKey(const char* pKey, bool value);
    
    
    
private:
    
    static GameDefault* m_spGameDefault;
};
