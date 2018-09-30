#ifndef __LUAFUNCHELPER_H__
#define __LUAFUNCHELPER_H__

#include "CCLuaEngine.h"
#include "Singleton.h"

extern int LuaStatePrint(lua_State * luastate);
extern int LuaStateReleasePrint(lua_State * luastate);


class LuaFuncHandlerManager
{
public:
	enum class ModuleType : int
	{
		kNone = 0,
		kCustom = 1,
		kMax,
	};

	enum class CustomHandlerType : int
	{
		kCustomHandleBegin = static_cast<int>(cocos2d::ScriptHandlerMgr::HandlerType::EVENT_CUSTOM_ENDED),

		kGetGlyphData,
		kGetGlyphStuffData,
		kGetMissionData,
		kGetPveResultValue,
		kGetBthTotalScore,
		kGetBthRankList,
		kGetMissionSkipCount,
		kGetAssistantData,

		kCustomHandleEnd,
	};

	typedef std::map <ModuleType, void*> ModulePool;

public:
	LuaFuncHandlerManager();
	virtual ~LuaFuncHandlerManager();

	void bindModule(ModuleType module, void* modulePtr);
	void unbindModule(ModuleType module);
	
	void addHandler(ModuleType moduleType, int handler, int moduleHandlerType);
	int getHandler(ModuleType moduleType, int moduleHandlerType);
	void removeHandler(ModuleType moduleType, int moduleHandlerType);

protected:

	ModulePool mModulePool;
};
#define sgLuaFuncHandlerManager() Singleton<LuaFuncHandlerManager>::Get()

#endif
