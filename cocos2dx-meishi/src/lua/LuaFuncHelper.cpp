#include "LuaFuncHelper.h"

USING_NS_CC;


int LuaStatePrint(lua_State * luastate)
{
	int nargs = lua_gettop(luastate);

	std::string t;
	for (int i = 1; i <= nargs; i++)
	{
		if (lua_istable(luastate, i))
			t += "table";
		else if (lua_isnone(luastate, i))
			t += "none";
		else if (lua_isnil(luastate, i))
			t += "nil";
		else if (lua_isboolean(luastate, i))
		{
			if (lua_toboolean(luastate, i) != 0)
				t += "true";
			else
				t += "false";
		}
		else if (lua_isfunction(luastate, i))
			t += "function";
		else if (lua_islightuserdata(luastate, i))
			t += "lightuserdata";
		else if (lua_isthread(luastate, i))
			t += "thread";
		else if (lua_isnumber(luastate, i))
		{
			t += StringUtils::format("%f", lua_tonumber(luastate, i));
		}
		else
		{
			const char * str = lua_tostring(luastate, i);
			if (str)
				t += lua_tostring(luastate, i);
			else
				t += lua_typename(luastate, lua_type(luastate, i));
		}
		if (i != nargs)
			t += "\t";
	}
	CCLOG("%s", t.c_str());

	return 0;
}

int LuaStateReleasePrint(lua_State * L)
{
	int nargs = lua_gettop(L);

	std::string t;
	for (int i = 1; i <= nargs; i++)
	{
		if (lua_istable(L, i))
			t += "table";
		else if (lua_isnone(L, i))
			t += "none";
		else if (lua_isnil(L, i))
			t += "nil";
		else if (lua_isboolean(L, i))
		{
			if (lua_toboolean(L, i) != 0)
				t += "true";
			else
				t += "false";
		}
		else if (lua_isfunction(L, i))
			t += "function";
		else if (lua_islightuserdata(L, i))
			t += "lightuserdata";
		else if (lua_isthread(L, i))
			t += "thread";
		else if (lua_isnumber(L, i))
		{
			t += StringUtils::format("%g", lua_tonumber(L, i));
		}
		else
		{
			const char * str = lua_tostring(L, i);
			if (str)
				t += lua_tostring(L, i);
			else
				t += lua_typename(L, lua_type(L, i));
		}
		if (i != nargs)
			t += "\t";
	}
	log("%s", t.c_str());

	return 0;
}


LuaFuncHandlerManager::LuaFuncHandlerManager()
{

}


LuaFuncHandlerManager::~LuaFuncHandlerManager()
{

}


void LuaFuncHandlerManager::bindModule(ModuleType module, void* modulePtr)
{
	CCLOG("[trace]LuaFuncHandlerManager::bindModule : Module[%d], ModulePtr[%p]", module, modulePtr);

	if (nullptr == modulePtr)
	{
		CCLOG("[warn]LuaFuncHelper::bindModule : modulePtr is null, module type is %d", static_cast<int>(module));
		return;
	}

	void * oldModulePtr = nullptr;

	auto moduleItr = mModulePool.find(module);
	if (mModulePool.end() != moduleItr)
	{
		if (moduleItr->second != modulePtr)
		{
			oldModulePtr = moduleItr->second;
			moduleItr->second = modulePtr;
		}
	}
	else
	{
		mModulePool.emplace(std::make_pair(module, modulePtr));
	}

	if (nullptr != oldModulePtr)
	{
		auto handlerMgr = ScriptHandlerMgr::getInstance();

		int begHandlerType = static_cast<int>(cocos2d::ScriptHandlerMgr::HandlerType::EVENT_CUSTOM_ENDED) + 1;
		int endHandlerType = 0;

		switch (module)
		{
		case ModuleType::kCustom: endHandlerType = static_cast<int>(CustomHandlerType::kCustomHandleEnd); break;
		default: return;
		}

		for (int curType = begHandlerType; curType < endHandlerType; ++curType)
		{
			auto handlerType = static_cast<cocos2d::ScriptHandlerMgr::HandlerType>(curType);
			auto funcHandler = handlerMgr->getObjectHandler(oldModulePtr, handlerType);
			handlerMgr->addObjectHandler(modulePtr, funcHandler, handlerType);
		}

		handlerMgr->removeObjectAllHandlers(oldModulePtr);
	}
}


void LuaFuncHandlerManager::unbindModule(ModuleType module)
{
	CCLOG("[trace]LuaFuncHandlerManager::unbindModule : Module[%d]", module);
	auto moduleItr = mModulePool.find(module);
	if (mModulePool.end() != moduleItr)
	{
		ScriptHandlerMgr::getInstance()->removeObjectAllHandlers(moduleItr->second);
		mModulePool.erase(moduleItr);
		moduleItr = mModulePool.end();
	}
}


void LuaFuncHandlerManager::addHandler(ModuleType moduleType, int handler, int moduleHandlerType)
{
	CCLOG("[trace]LuaFuncHandlerManager::addHandler : Module[%d], Handler[%d], HandlerType[%d]", moduleType, handler, moduleHandlerType);

	auto moduleItr = mModulePool.find(moduleType);
	if (mModulePool.end() != moduleItr)
	{
		ScriptHandlerMgr::getInstance()->addObjectHandler(moduleItr->second, handler, static_cast<cocos2d::ScriptHandlerMgr::HandlerType>(moduleHandlerType));
	}
}


int LuaFuncHandlerManager::getHandler(ModuleType moduleType, int moduleHandlerType)
{
	auto moduleItr = mModulePool.find(moduleType);
	if (mModulePool.end() != moduleItr)
	{
		return ScriptHandlerMgr::getInstance()->getObjectHandler(moduleItr->second, static_cast<cocos2d::ScriptHandlerMgr::HandlerType>(moduleHandlerType));
	}

	return 0;
}


void LuaFuncHandlerManager::removeHandler(ModuleType moduleType, int moduleHandlerType)
{
	CCLOG("[trace]LuaFuncHandlerManager::removeHandler : Module[%d], Handler[%d], HandlerType", moduleType, moduleHandlerType);

	auto moduleItr = mModulePool.find(moduleType);
	if (mModulePool.end() != moduleItr)
	{
		ScriptHandlerMgr::getInstance()->removeObjectHandler(moduleItr->second, static_cast<cocos2d::ScriptHandlerMgr::HandlerType>(moduleHandlerType));
	}
}

