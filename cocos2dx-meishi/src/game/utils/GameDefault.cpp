
//  GameDefault.cpp

#include "GameDefault.h"
#include "game/net/UserObj.h"
GameDefault* GameDefault::m_spGameDefault = NULL;

GameDefault::GameDefault()
{
    
    
}

GameDefault::~GameDefault()
{
    
    
}

GameDefault* GameDefault::sharedGameDefault()
{
    if (! m_spGameDefault)
    {
        m_spGameDefault = new GameDefault();
    }
    
    return m_spGameDefault;
}


void GameDefault::setStringForKey(const char* pKey, const std::string & value)
{
    /*
     *  1. 保存全局属性，如账号密码，服务器信息
     *  2. 保存某一账号属性,前面+账号做前缀
     */
    
    do
    {
        // 如果是全局信息
        std::string szKey = std::string(pKey);
        if (szKey.find("USER") != std::string::npos) {
            
            UserDefault::getInstance()->setStringForKey(pKey, value);
            break;
        }
        
        szKey = UserDefault::getInstance()->getStringForKey("USER_OPENID");
        szKey += "_";
        szKey += pKey;
        UserDefault::getInstance()->setStringForKey(szKey.c_str(), value);
        
    }while (0);
    
    UserDefault::getInstance()->flush();
    return;
}


std::string GameDefault::getStringForKey(const char* pKey)
{
    std::string szKey = std::string(pKey);
    if (szKey.find("USER") != std::string::npos) {
        
        return UserDefault::getInstance()->getStringForKey(pKey);
    }
    
    szKey = UserDefault::getInstance()->getStringForKey("USER_OPENID");
    szKey += "_";
    szKey += pKey;
    return UserDefault::getInstance()->getStringForKey(szKey.c_str());
}

float GameDefault::getFloatForKey(const char* pKey)
{
    std::string szKey = std::string(pKey);
    if (szKey.find("USER") != std::string::npos) {
        
        return UserDefault::getInstance()->getFloatForKey(pKey);
    }
    
	szKey = UserDefault::getInstance()->getStringForKey("USER_OPENID");
    szKey += "_";
    szKey += pKey;
    return UserDefault::getInstance()->getFloatForKey(szKey.c_str());
}

void GameDefault::setFloatForKey(const char* pKey, float value)
{
    do
    {
        // 如果是全局信息
        std::string szKey = std::string(pKey);
        if (szKey.find("USER") != std::string::npos) {
            
            UserDefault::getInstance()->setFloatForKey(pKey, value);
            break;
        }
        
        szKey = UserDefault::getInstance()->getStringForKey("USER_OPENID");
        szKey += "_";
        szKey += pKey;
        UserDefault::getInstance()->setFloatForKey(szKey.c_str(), value);
        
    }while (0);
    
    UserDefault::getInstance()->flush();
    return;
}


int GameDefault::getIntegerForKey(const char* pKey)
{
    std::string szKey = std::string(pKey);
    if (szKey.find("USER") != std::string::npos) {
        
        return UserDefault::getInstance()->getIntegerForKey(pKey);
    }
    
    szKey = UserDefault::getInstance()->getStringForKey("USER_OPENID");
    szKey += "_";
    szKey += pKey;
    return UserDefault::getInstance()->getIntegerForKey(szKey.c_str());
}

void GameDefault::setIntegerForKey(const char* pKey, int value)
{
    do
    {
        // 如果是全局信息
        std::string szKey = std::string(pKey);
        if (szKey.find("USER") != std::string::npos) {
            
            UserDefault::getInstance()->setIntegerForKey(pKey, value);
            break;
        }
        
        szKey = UserDefault::getInstance()->getStringForKey("USER_OPENID");
        szKey += "_";
        szKey += pKey;
        UserDefault::getInstance()->setIntegerForKey(szKey.c_str(), value);
        
    }while (0);
    
    UserDefault::getInstance()->flush();
    return;
}


bool GameDefault::getBoolForKey(const char* pKey,bool value)
{
    std::string szKey = std::string(pKey);
    if (szKey.find("USER") != std::string::npos) {
        
        return UserDefault::getInstance()->getBoolForKey(pKey,value);
    }
    
    szKey = UserDefault::getInstance()->getStringForKey("USER_OPENID");
    szKey += "_";
    szKey += pKey;
    return UserDefault::getInstance()->getBoolForKey(szKey.c_str(),value);
}

void GameDefault::setBoolForKey(const char* pKey, bool value)
{
    do
    {
        // 如果是全局信息
        std::string szKey = std::string(pKey);
        if (szKey.find("USER") != std::string::npos) {
            
            UserDefault::getInstance()->setBoolForKey(pKey, value);
            break;
        }
        
        szKey = UserDefault::getInstance()->getStringForKey("USER_OPENID");
        szKey += "_";
        szKey += pKey;
        UserDefault::getInstance()->setBoolForKey(szKey.c_str(), value);
        
    }while (0);
    
    UserDefault::getInstance()->flush();
    return;
}
