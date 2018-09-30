#ifndef __GAME_JNI_HELPER_H__
#define __GAME_JNI_HELPER_H__

#include <string>

class GameJniHelper
{
public:
	static GameJniHelper* getInstance();

	void restartGame();
	std::string getCode();
	void reloadAPK(const char * apkName);
	std::string getSDCardPath();

	bool isNetworkConnected();
	bool isWifiConnected();

protected:
	static GameJniHelper* s_instance;
	GameJniHelper(){};
	virtual ~GameJniHelper(){}
};


#endif
