
#include "CppCallLuaHelper.h"
#include "CCLuaEngine.h"
#include "LuaBasicConversions.h"
#include "core/TimeCalculator.h"

CppCallLuaHelper::CppCallLuaHelper()
{

}

CppCallLuaHelper::~CppCallLuaHelper()
{

}

Window * CppCallLuaHelper::CallOnReceiveMassage(string pageName)
{
	LuaEngine::getInstance()->getLuaStack()->clean();
	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	lua_getglobal(ls, "onReceiveMassage_page");

	/*
	lua_call
	第一个参数:函数的参数个数
	第二个参数:函数返回值个数
	*/

	const char * pstr = pageName.c_str();
	if (!lua_isfunction(ls, 1))
	{
		CCASSERT(0, "get function onReceiveMassage_page null");
		return nullptr;
	}
	lua_pushstring(ls, pstr);	//pageName

	lua_call(ls, 1, 1);
	Window * pPage = *(Window **)lua_touserdata(ls, -1);
	LuaEngine::getInstance()->getLuaStack()->clean();
	return pPage;
}

void CppCallLuaHelper::CallOnReceiveMassage_netProto(int protoType)
{
	LuaEngine::getInstance()->getLuaStack()->clean();
	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	lua_getglobal(ls, "onReceiveMassage_netProto");

	if (!lua_isfunction(ls, 1))
	{
		CCASSERT(0, "get function CallOnReceiveMassage_netProto null");
	}

	//protoType
	lua_pushinteger(ls, protoType);

	/*
	lua_call
	第一个参数:函数的参数个数
	第二个参数:函数返回值个数
	*/
	lua_call(ls, 1, 0);
	LuaEngine::getInstance()->getLuaStack()->clean();
}

void CppCallLuaHelper::onShow(Window* node, bool bShow)
{
	LuaEngine::getInstance()->getLuaStack()->clean();
	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();
	lua_getglobal(ls, "onReceiveMassage_onshow");
	if (!lua_isfunction(ls, 1))
	{
		CCASSERT(0, "get function onReceiveMassage_page null");
		return;
	}

	std::string s = node->getPageName();
	lua_pushstring(ls, s.c_str());	//pageName

	lua_pushboolean(ls, bShow);
	lua_call(ls, 2, 0);
	LuaEngine::getInstance()->getLuaStack()->clean();
	return ;

}


int CppCallLuaHelper::getLuaWindowId(std::string pageName)
{
	LuaEngine::getInstance()->getLuaStack()->clean();
	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	lua_getglobal(ls, "onReceiveMassage_luaId");

	if (!lua_isfunction(ls, 1))
	{
		CCASSERT(0, "get function onReceiveMassage_getLuaWindowId null");
		return 0;
	}

	lua_pushstring(ls, pageName.c_str());	//pageName

	lua_call(ls, 1, 1);

	int  winid = lua_tointeger(ls, -1);
	LuaEngine::getInstance()->getLuaStack()->clean();
	return winid;
}

void CppCallLuaHelper::aniCallback(Window * window, cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	LuaEngine::getInstance()->getLuaStack()->clean();
	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	lua_getglobal(ls, "onReceiveMassage_aniCallback");

	if (!lua_isfunction(ls, 1))
	{
		CCASSERT(0, "get function onReceiveMassage_aniCallback null");
		return;
	}

	std::string s = window->getPageName();

	lua_pushstring(ls, s.c_str());	//pageName
	object_to_luaval<cocostudio::Armature>(ls, "ccs.Armature", (cocostudio::Armature*)pAr);
	lua_pushinteger(ls, type);
	lua_pushstring(ls, aniName.c_str());

	lua_call(ls, 4, 0);
	LuaEngine::getInstance()->getLuaStack()->clean();
}

void CppCallLuaHelper::onFrameEvent(Window * window, cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex)
{
	LuaEngine::getInstance()->getLuaStack()->clean();
	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	lua_getglobal(ls, "onReceiveMassage_onFrameEvent");

	if (!lua_isfunction(ls, 1))
	{
		CCASSERT(0, "get function onReceiveMassage_onFrameEvent null");
		return;
	}

	std::string s = window->getPageName();
	lua_pushstring(ls, s.c_str());	//pageName

	object_to_luaval<cocostudio::Bone>(ls, "ccs.Bone", (cocostudio::Bone*)bone);
	lua_pushstring(ls, evt.c_str());
	lua_pushinteger(ls, originFrameIndex);
	lua_pushinteger(ls, currentFrameIndex);

	lua_call(ls, 5, 0);
	LuaEngine::getInstance()->getLuaStack()->clean();
	return;

}

CppCallLuaHelper::stuTimeRemaining CppCallLuaHelper::GetTimeRemaining(double standardTime, int week, string time, int model)
{
	LuaEngine::getInstance()->getLuaStack()->clean();
	lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();

	lua_getglobal(ls, "GetTimeRemaining");

	if (!lua_isfunction(ls, 1))
	{
		CCASSERT(0, "get function GetTimeRemaining null");
		return stuTimeRemaining();
	}

	/*
	lua_call
	第一个参数:函数的参数个数
	第二个参数:函数返回值个数
	*/
	lua_pushnumber(ls, (lua_Number)(standardTime));
	lua_pushnumber(ls, (lua_Number)(week));
	lua_pushstring(ls, time.c_str());
	lua_pushnumber(ls, (lua_Number)(model));
	lua_call(ls, 4, 2);

	bool bOverTime = lua_toboolean(ls, -1);

	lua_pop(ls, 1);

	size_t l;
	const char * timeRemaining = lua_tolstring(ls, -1, &l);

	stuTimeRemaining timeData(timeRemaining, bOverTime);

	LuaEngine::getInstance()->getLuaStack()->clean();

	return timeData;
}
