
#ifndef __CPP_CALL_LUA_HELPER_H__
#define __CPP_CALL_LUA_HELPER_H__

#include "ui/system/Window.h"
#include "Singleton.h"
#include <string>

using namespace std;

class CppCallLuaHelper
{
public:
	CppCallLuaHelper();
	~CppCallLuaHelper();

	struct stuTimeRemaining
	{
		string mFormat;
		bool mbOverTime;

		stuTimeRemaining()
		{
			mFormat = "";
			mbOverTime = false;
		}

		stuTimeRemaining(string _format, bool overTime)
		{
			mFormat = _format;
			mbOverTime = overTime;
		}
	};

	Window * CallOnReceiveMassage(string pageName);

	void CallOnReceiveMassage_netProto(int protoType);

	void onShow(Window* window,  bool bShow);

	int getLuaWindowId(std::string pageName);

	void aniCallback(Window * window, cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);

	void onFrameEvent(Window * window, cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex);

	stuTimeRemaining GetTimeRemaining(double standard, int week, string time, int model = E_NORMAL);
private:

};

#endif

#define sgCppCallLuaHelper() Singleton<CppCallLuaHelper>::Get()
