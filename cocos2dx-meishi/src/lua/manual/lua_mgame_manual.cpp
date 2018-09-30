#include "lua_mgame_manual.h"
#include <unordered_map>
#include "CCLuaStack.h"
#include "CCLuaEngine.h"

#include "core/luaHelp/FuncForLua.h"
#include "core/MessageManager.h"
#include "core/SoundManager.h"
#include "ui/PageFunction.h"
#include "core/CVTime.h"
#include "net/NetManager.h"
#include "lua/LuaProxy.h"
#include "net/UserObj.h"
#include "channel/ChannelApi.h"
#include "../LuaFuncHelper.h"
#include "core/ResManager.h"
#include "guide/GuideManager.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#include "game/prop/PropsControl.h"
#include "game/sevenGoal/SevenGoalControl.h"
#include "winner/WinnerControl.h"
#include "ui/dialogs/BackPackDialog.h"
#include "utils/HeadIconUtils.h"
#include "game/battle/BattleRecordsManager.h"
#include "game/topRank/topRankModel_Cpp.h"

int lua_register_getSingleManager(lua_State* tolua_S)
{
	if (NULL == tolua_S)
		return 0;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif

	int argc = 0;
	argc = lua_gettop(tolua_S);

	if (argc == 1)
	{
#if COCOS2D_DEBUG >= 1
		if (!tolua_isstring(tolua_S, 1, 0, &tolua_err))
			goto tolua_lerror;
#endif
		std::string name = (std::string)tolua_tostring(tolua_S, 1, 0);
		
		if (name == "SoundManager")
		{
			auto object = sgSoundManager();
			object_to_luaval<SoundManager>(tolua_S, "SoundManager", (SoundManager*)object);
		}
		else if (name == "PageFunction")
		{
			auto object = sgPageFunction();
			object_to_luaval<PageFunction>(tolua_S, "PageFunction", (PageFunction*)object);
		}
		else if (name == "CVTimeUtils")
		{
			auto object = sgTimeUtils();
			object_to_luaval<CVTimeUtils>(tolua_S, "CVTimeUtils", (CVTimeUtils*)object);
		}
		else if (name == "TimeCalculator")
		{
			auto object = sgTimeCalculator();
			object_to_luaval<TimeCalculator>(tolua_S, "TimeCalculator", (TimeCalculator*)object);
		}
		else if (name == "FuncForLua")
		{
			auto object = sgFuncForLua();
			object_to_luaval<FuncForLua>(tolua_S, "FuncForLua", (FuncForLua*)object);
		}
		else if (name == "NetManager")
		{
			auto object = sgNetManager();
			object_to_luaval<NetManager>(tolua_S, "NetManager", (NetManager*)object);
			return 1;
		}
		else if (name == "UserObj")
		{
			auto object = sgUserObj();
			object_to_luaval<UserObj>(tolua_S, "UserObj", (UserObj*)object);
			return 1;
		}
		else if (name == "ChannelApi")
		{
			auto object = sgChannelApi();
			object_to_luaval<ChannelApi>(tolua_S, "ChannelApi", (ChannelApi*)object);
			return 1;
		}
		else if (name == "LoginProxy")
		{
			auto object = sgLoginProxy();
			object_to_luaval<LoginProxy>(tolua_S, "LoginProxy", (LoginProxy*)object);
			return 1;
		}
		else if (name == "TableManager")
		{
			auto object = sgTableManager();
			object_to_luaval<TableManager>(tolua_S, "TableManager", (TableManager*)object);
			return 1;
		}
		else if (name == "TimeCalculator")
		{
			auto object = sgTimeCalculator();
			object_to_luaval<TimeCalculator>(tolua_S, "TimeCalculator", (TimeCalculator*)object);
			return 1;
		}
		else if (name == "LuaFuncHandlerManager")
		{
			auto object = sgLuaFuncHandlerManager();
			object_to_luaval<LuaFuncHandlerManager>(tolua_S, "LuaFuncHandlerManager", (LuaFuncHandlerManager*)object);
			return 1;
		}
		else if (name == "ResManager")
		{
			auto object = sgResManager();
			object_to_luaval<ResManager>(tolua_S, "ResManager", (ResManager*)object);
			return 1;
		}
		else if (name == "GuideManager")
		{
			auto object = sgGuideManager();
			object_to_luaval<GuideManager>(tolua_S, "GuideManager", (GuideManager*)object);
			return 1;
		}
		else if (name == "WinnerControl")
		{
			auto object = sgWinnerControl();
			object_to_luaval<WinnerControl>(tolua_S, "WinnerControl", (WinnerControl*)object);
			return 1;
		}
		else if (name == "BackPackControl")
		{
			auto object = sgBackPackControl();
			object_to_luaval<BackPackControl >(tolua_S, "BackPackControl", (BackPackControl*)object);
			return 1;
		}
		else if (name == "MessageManager")
		{
			auto object = sgMessageManager();
			object_to_luaval<MessageManager >(tolua_S, "MessageManager", (MessageManager*)object);
			return 1;
		}
		else if (name == "PropsControl")
		{
			auto object = sgPropsControl();
			object_to_luaval<PropsControl >(tolua_S, "PropsControl", (PropsControl*)object);
			return 1;
		}
		else if (name == "RankingControl")
		{
			auto object = sgRankingControl();
			object_to_luaval<RankingControl >(tolua_S, "RankingControl", (RankingControl*)object);
			return 1;
		}
		else if (name == "SevenGoalControl")
		{
			auto object = sgSevenGoalControl();
			object_to_luaval<SevenGoalControl >(tolua_S, "SevenGoalControl", (SevenGoalControl*)object);
			return 1;
		}
		else if (name == "HeadIconUtils")
		{
			auto object = sgHeadIconUtils();
			object_to_luaval<HeadIconUtils >(tolua_S, "HeadIconUtils", (HeadIconUtils*)object);
			return 1;
		}
		else if (name == "BattleRecordsManager")
		{
			auto object = sgBattleRecordsManager();
			object_to_luaval<BattleRecordsManager>(tolua_S, "BattleRecordsManager", (BattleRecordsManager*)object);
			return 1;
		}
		else if (name == "topRankModel_Cpp")
		{
			auto object = sgtopRankModel_Cpp();
			object_to_luaval<topRankModel_Cpp>(tolua_S, "topRankModel_Cpp", (topRankModel_Cpp*)object);
			return 1;
		}
		else{
			luaL_error(tolua_S, "%s unknown cpp manager %s\n", "getSingleManager", name.c_str());
			return 0;
		}

		return 1;
	}
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "getSingleManager", argc, 1);
	return 0;
#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Node_registerScriptHandler'.", &tolua_err);
	return 0;
#endif
}

void lua_register_singleton_free(lua_State* tolua_S)
{

}

//code, len, src, bool bIsCheckRecv = false, bool bIsBlock = false, bool isOnlyOnce = true

int lua_mgame_NetManager_SendPkg(lua_State* tolua_S)
{
	int argc = 0;
	NetManager* cobj = nullptr;
	bool ok = true;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertype(tolua_S, 1, "NetManager", 0, &tolua_err)) goto tolua_lerror;
#endif

	cobj = (NetManager*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
	if (!cobj)
	{
		tolua_error(tolua_S, "invalid 'cobj' in function 'lua_mgame_NetManager_SendPkg'", nullptr);
		return 0;
	}
#endif

	argc = lua_gettop(tolua_S) - 1;
	if (argc == 3)
	{
		unsigned int arg0;
		size_t arg1;
		const char* buffer = NULL;
		ok = false;

		ok &= luaval_to_int32(tolua_S, 2, (int *)&arg0, "NetManager:SendPkg");
		ok &= luaval_to_int32(tolua_S, 3, (int *)&arg1, "NetManager:SendPkg");
		ok &= luaval_to_bin_string(tolua_S, 4, buffer, arg1, "NetManager:SendPkg");


		NetPack msgPack;

		MsgHead& msgHead = *(msgPack.getMsgHead());
		sgNetManager()->GenRqstMsgHead(msgHead, arg0);

		msgPack.getC2SMsg()->ParseFromArray(buffer, arg1);

		cobj->SendPkg(msgPack);
		lua_settop(tolua_S, 1);
		return 1;
	}
	if (argc == 4)
	{
		unsigned int arg0;
		size_t arg1;
		const char* buffer = NULL;
		ok = false;
		bool arg3;
		ok &= luaval_to_int32(tolua_S, 2, (int *)&arg0, "NetManager:SendPkg");
		ok &= luaval_to_int32(tolua_S, 3, (int *)&arg1, "NetManager:SendPkg");
		ok &= luaval_to_bin_string(tolua_S, 4, buffer, arg1, "NetManager:SendPkg");

		ok &= luaval_to_boolean(tolua_S, 5, &arg3, "NetManager:SendPkg");

		NetPack msgPack;

		MsgHead& msgHead = *(msgPack.getMsgHead());
		sgNetManager()->GenRqstMsgHead(msgHead, arg0, arg3);

		msgPack.getC2SMsg()->ParseFromArray(buffer, arg1);

		cobj->SendPkg(msgPack, arg3);
		lua_settop(tolua_S, 1);

		return 1;
	}
	if (argc == 5)
	{
		unsigned int arg0;
		size_t arg1;
		const char* buffer = NULL;
		ok = false;
		bool arg3;
		bool arg4;
		ok &= luaval_to_int32(tolua_S, 2, (int *)&arg0, "NetManager:SendPkg");
		ok &= luaval_to_int32(tolua_S, 3, (int *)&arg1, "NetManager:SendPkg");
		ok &= luaval_to_bin_string(tolua_S, 4, buffer, arg1, "NetManager:SendPkg");

		ok &= luaval_to_boolean(tolua_S, 5, &arg3, "NetManager:SendPkg");
		ok &= luaval_to_boolean(tolua_S, 6, &arg4, "NetManager:SendPkg");
		NetPack msgPack;

		MsgHead& msgHead = *(msgPack.getMsgHead());
		sgNetManager()->GenRqstMsgHead(msgHead, arg0, arg3);

		msgPack.getC2SMsg()->ParseFromArray(buffer, arg1);

		cobj->SendPkg(msgPack, arg3, arg4);
		lua_settop(tolua_S, 1);
	}
	if (argc == 6)
	{
		unsigned int arg0;
		size_t arg1;
		const char* buffer = NULL;
		ok = false;
		bool arg3;
		bool arg4;
		bool arg5;
		ok &= luaval_to_int32(tolua_S, 2, (int *)&arg0, "NetManager:SendPkg");
		ok &= luaval_to_int32(tolua_S, 3, (int *)&arg1, "NetManager:SendPkg");
		ok &= luaval_to_bin_string(tolua_S, 4, buffer, arg1, "NetManager:SendPkg");

		ok &= luaval_to_boolean(tolua_S, 5, &arg3, "NetManager:SendPkg");
		ok &= luaval_to_boolean(tolua_S, 6, &arg4, "NetManager:SendPkg");
		ok &= luaval_to_boolean(tolua_S, 7, &arg5, "NetManager:SendPkg");
		NetPack msgPack;

		MsgHead& msgHead = *(msgPack.getMsgHead());
		sgNetManager()->GenRqstMsgHead(msgHead, arg0, arg3);

		msgPack.getC2SMsg()->ParseFromArray(buffer, arg1);

		cobj->SendPkg(msgPack, arg3, arg4, arg5);
		lua_settop(tolua_S, 1);

		return 1;
	}
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "NetManager:SendPkg", argc, 1);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_mgame_NetManager_SendPkg'.", &tolua_err);
#endif

	return 0;
}

int lua_mgame_NetManager_GenRqstMsgHead(lua_State* tolua_S)
{
	int argc = 0;
	NetManager* cobj = nullptr;
	bool ok = true;
#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertype(tolua_S, 1, "NetManager", 0, &tolua_err)) goto tolua_lerror;
#endif
	cobj = (NetManager*)tolua_tousertype(tolua_S, 1, 0);
#if COCOS2D_DEBUG >= 1
	if (!cobj)
	{
		tolua_error(tolua_S, "invalid 'cobj' in function 'lua_mgame_NetManager_GenRqstMsgHead'", nullptr);
		return 0;
	}
#endif
	argc = lua_gettop(tolua_S) - 1;
	do{
		if (argc == 2) {
			MsgHead* arg0;
			ok &= luaval_to_object<MsgHead>(tolua_S, 2, "MsgHead", &arg0);

			if (!ok) { break; }
			int arg1;
			ok &= luaval_to_int32(tolua_S, 3, (int *)&arg1, "NetManager:GenRqstMsgHead");

			if (!ok) { break; }
			cobj->GenRqstMsgHead(*arg0, arg1);
			lua_settop(tolua_S, 1);
			return 1;
		}
	} while (0);
	ok = true;
	do{
		if (argc == 3) {
			MsgHead* arg0;
			ok &= luaval_to_object<MsgHead>(tolua_S, 2, "MsgHead", &arg0);

			if (!ok) { break; }
			int arg1;
			ok &= luaval_to_int32(tolua_S, 3, (int *)&arg1, "NetManager:GenRqstMsgHead");

			if (!ok) { break; }
			int arg2;
			ok &= luaval_to_int32(tolua_S, 4, (int *)&arg2, "NetManager:GenRqstMsgHead");

			if (!ok) { break; }
			cobj->GenRqstMsgHead(*arg0, arg1, arg2);
			lua_settop(tolua_S, 1);
			return 1;
		}
	} while (0);
	ok = true;
	do{
		if (argc == 4) {
			MsgHead* arg0;
			ok &= luaval_to_object<MsgHead>(tolua_S, 2, "MsgHead", &arg0);

			if (!ok) { break; }
			int arg1;
			ok &= luaval_to_int32(tolua_S, 3, (int *)&arg1, "NetManager:GenRqstMsgHead");

			if (!ok) { break; }
			int arg2;
			ok &= luaval_to_int32(tolua_S, 4, (int *)&arg2, "NetManager:GenRqstMsgHead");

			if (!ok) { break; }
			bool arg3;
			ok &= luaval_to_boolean(tolua_S, 5, &arg3, "NetManager:GenRqstMsgHead");

			if (!ok) { break; }
			cobj->GenRqstMsgHead(*arg0, arg1, arg2, arg3);
			lua_settop(tolua_S, 1);
			return 1;
		}
	} while (0);
	ok = true;
	do{
		if (argc == 5) {
			MsgHead* arg0;
			ok &= luaval_to_object<MsgHead>(tolua_S, 2, "MsgHead", &arg0);

			if (!ok) { break; }
			int arg1;
			ok &= luaval_to_int32(tolua_S, 3, (int *)&arg1, "NetManager:GenRqstMsgHead");

			if (!ok) { break; }
			int arg2;
			ok &= luaval_to_int32(tolua_S, 4, (int *)&arg2, "NetManager:GenRqstMsgHead");

			if (!ok) { break; }
			bool arg3;
			ok &= luaval_to_boolean(tolua_S, 5, &arg3, "NetManager:GenRqstMsgHead");

			if (!ok) { break; }
			bool arg4;
			ok &= luaval_to_boolean(tolua_S, 6, &arg4, "NetManager:GenRqstMsgHead");

			if (!ok) { break; }
			cobj->GenRqstMsgHead(*arg0, arg1, arg2, arg3, arg4);
			lua_settop(tolua_S, 1);
			return 1;
		}
	} while (0);
	ok = true;
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "NetManager:GenRqstMsgHead", argc, 3);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_mgame_NetManager_GenRqstMsgHead'.", &tolua_err);
#endif

	return 0;
}
int lua_mgame_NetManager_RecvPkg(lua_State* tolua_S)
{
	int argc = 0;
	NetManager* cobj = nullptr;
	bool ok = true;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertype(tolua_S, 1, "NetManager", 0, &tolua_err)) goto tolua_lerror;
#endif

	cobj = (NetManager*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
	if (!cobj)
	{
		tolua_error(tolua_S, "invalid 'cobj' in function 'lua_mgame_NetManager_RecvPkg'", nullptr);
		return 0;
	}
#endif

	argc = lua_gettop(tolua_S) - 1;
	if (argc == 2)
	{
		size_t arg0;

		ok = false;

		ok &= luaval_to_int32(tolua_S, 2, (int *)&arg0, "NetManager:RecvPkg");
		const char* buffer = NULL;
		ok &= luaval_to_bin_string(tolua_S, 3, buffer, arg0, "NetManager:RecvPkg");

		if (!ok)
		{
			tolua_error(tolua_S, "invalid arguments in function 'lua_mgame_NetManager_RecvPkg'", nullptr);
			return 0;
		}
		cobj->RecvPkg((void*)buffer, arg0);
		lua_settop(tolua_S, 1);
		return 1;
	}
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "NetManager:RecvPkg", argc, 2);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_mgame_NetManager_RecvPkg'.", &tolua_err);
#endif

	return 0;
}


int lua_mgame_NetPack_SerializeToArray(lua_State* tolua_S)
{
	int argc = 0;
	NetPack* cobj = nullptr;
	bool ok = true;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertype(tolua_S, 1, "NetPack", 0, &tolua_err)) goto tolua_lerror;
#endif

	cobj = (NetPack*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
	if (!cobj)
	{
		tolua_error(tolua_S, "invalid 'cobj' in function 'lua_mgame_NetPack_SerializeToArray'", nullptr);
		return 0;
	}
#endif

	argc = lua_gettop(tolua_S) - 1;
	if (argc == 2)
	{
		size_t arg0;

		ok = false;

		ok &= luaval_to_int32(tolua_S, 2, (int *)&arg0, "NetManager:RecvPkg");
		const char* buffer = NULL;
		ok &= luaval_to_bin_string(tolua_S, 3, buffer, arg0, "NetManager:RecvPkg");

		if (!ok)
		{
			tolua_error(tolua_S, "invalid arguments in function 'lua_mgame_NetManager_RecvPkg'", nullptr);
			return 0;
		}

		bool ret = cobj->SerializeToArray((void*)buffer, arg0);
		tolua_pushboolean(tolua_S, (bool)ret);
		return 1;
	}
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "NetPack:SerializeToArray", argc, 2);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_mgame_NetPack_SerializeToArray'.", &tolua_err);
#endif

	return 0;
}

int lua_register_mgame_NetPack_manual(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "NetPack");
	tolua_cclass(tolua_S, "NetPack", "NetPack", "", nullptr);

	tolua_beginmodule(tolua_S, "NetPack");
	tolua_function(tolua_S, "SerializeToArray", lua_mgame_NetPack_SerializeToArray);
	tolua_endmodule(tolua_S);
	std::string typeName = typeid(NetPack).name();
	g_luaType[typeName] = "NetPack";
	g_typeCast["NetPack"] = "NetPack";
	return 1;
}

int lua_register_mgame_NetManager_manual(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "NetManager");
	tolua_cclass(tolua_S, "NetManager", "NetManager", "cc.Ref", nullptr);

	tolua_beginmodule(tolua_S, "NetManager");
	tolua_function(tolua_S, "SendPkg", lua_mgame_NetManager_SendPkg);
	tolua_function(tolua_S, "GenRqstMsgHead", lua_mgame_NetManager_GenRqstMsgHead);
	//tolua_function(tolua_S, "RecvPkg", lua_mgame_NetManager_RecvPkg);

	tolua_endmodule(tolua_S);
	std::string typeName = typeid(NetManager).name();
	g_luaType[typeName] = "NetManager";
	g_typeCast["NetManager"] = "NetManager";
	return 1;
}

static int lua_register_mgame_LuaProxy_registerHandler(lua_State* tolua_S)
{
	if (NULL == tolua_S)
		return 0;

	int argc = 0;
	Node* self = nullptr;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
	if (!tolua_isusertype(tolua_S, 1, "LuaProxy", 0, &tolua_err)) goto tolua_lerror;
#endif

	self = static_cast<cocos2d::Node*>(tolua_tousertype(tolua_S, 1, 0));
#if COCOS2D_DEBUG >= 1
	if (nullptr == self) {
		tolua_error(tolua_S, "invalid 'self' in function 'tolua_sgame_net_registerScriptHandler'\n", NULL);
		return 0;
	}
#endif

	argc = lua_gettop(tolua_S) - 1;

	if (argc == 2)
	{
#if COCOS2D_DEBUG >= 1
		if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
			goto tolua_lerror;
#endif

		int32_t code = (int32_t)tolua_tonumber(tolua_S, 2, 0);


#if COCOS2D_DEBUG >= 1
		if (!toluafix_isfunction(tolua_S, 3, "LUA_FUNCTION", 0, &tolua_err))
			goto tolua_lerror;
#endif
		LUA_FUNCTION fun = toluafix_ref_function(tolua_S, 3, 0);
		//LuaProxy::getInstance()->registerHandler((short)code, fun);
		LuaProxy::getInstance()->registerHandler(code, fun);
		//ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::NODE);

		return 0;
	}

	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "cc.Node:registerScriptHandler", argc, 1);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'tolua_cocos2d_Node_registerScriptHandler'.", &tolua_err);
	return 0;
#endif
}

int lua_register_mgame_LuaProxy_manual(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "LuaProxy");
	tolua_cclass(tolua_S, "LuaProxy", "LuaProxy", "", nullptr);

	tolua_beginmodule(tolua_S, "LuaProxy");
	tolua_function(tolua_S, "registerHandler", lua_register_mgame_LuaProxy_registerHandler);

	tolua_endmodule(tolua_S);
	std::string typeName = typeid(LuaProxy).name();
	g_luaType[typeName] = "LuaProxy";
	g_typeCast["LuaProxy"] = "LuaProxy";
	return 1;
}

//---------------------------------------------------------  CVTimeUtils  --------------------------------------------------------------------
bool luaval_to_timeval(lua_State* L, int lo, timeval* outValue, const char* funcName)
{
	if (nullptr == L || nullptr == outValue)
		return false;

	bool ok = true;

	tolua_Error tolua_err;
	if (!tolua_istable(L, lo, 0, &tolua_err))
	{
#if COCOS2D_DEBUG >=1
		luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
#endif
		ok = false;
	}


	if (ok)
	{
		lua_pushstring(L, "tv_sec");
		lua_gettable(L, lo);
		outValue->tv_sec = lua_isnil(L, -1) ? 0 : lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_pushstring(L, "tv_usec");
		lua_gettable(L, lo);
		outValue->tv_usec = lua_isnil(L, -1) ? 0 : lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	return ok;
}

int lua_mgame_CVTimeUtils_GetNow(lua_State* tolua_S)
{
	int argc = 0;
	bool ok = true;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertable(tolua_S, 1, "CVTimeUtils", 0, &tolua_err)) goto tolua_lerror;
#endif

	argc = lua_gettop(tolua_S) - 1;

	if (argc == 1)
	{
		timeval arg0;
		ok &= luaval_to_timeval(tolua_S, 2, &arg0, "CVTimeUtils::GetNow");
		if (!ok)
		{
			tolua_error(tolua_S, "invalid arguments in function 'lua_mgame_CVTimeUtils_GetNow'", nullptr);
			return 0;
		}
		timeval& ret = CVTimeUtils::GetNow(arg0);
		return 1;
	}
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "CVTimeUtils:GetNow", argc, 1);
	return 0;
#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_mgame_CVTimeUtils_GetNow'.", &tolua_err);
#endif
	return 0;
}

int lua_mgame_CVTimeUtils_Diff(lua_State* tolua_S)
{
	int argc = 0;
	bool ok = true;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertable(tolua_S, 1, "CVTimeUtils", 0, &tolua_err)) goto tolua_lerror;
#endif

	argc = lua_gettop(tolua_S) - 1;

	if (argc == 2)
	{
		timeval arg0;
		timeval arg1;
		ok &= luaval_to_timeval(tolua_S, 2, &arg0, "CVTimeUtils::Diff");
		ok &= luaval_to_timeval(tolua_S, 3, &arg1, "CVTimeUtils::Diff");

		if (!ok)
		{
			tolua_error(tolua_S, "invalid arguments in function 'lua_mgame_CVTimeUtils_Diff'", nullptr);
			return 0;
		}
		int ret = CVTimeUtils::Diff(arg0, arg1);
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "CVTimeUtils:Diff", argc, 2);
	return 0;
#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_mgame_CVTimeUtils_Diff'.", &tolua_err);
#endif
	return 0;
}

int lua_register_mgame_CVTimeUtils_manual(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "CVTimeUtils");
	tolua_cclass(tolua_S, "CVTimeUtils", "CVTimeUtils", "", nullptr);

	tolua_beginmodule(tolua_S, "CVTimeUtils");
	tolua_function(tolua_S, "GetNow", lua_mgame_CVTimeUtils_GetNow);
	tolua_function(tolua_S, "Diff", lua_mgame_CVTimeUtils_Diff);
	tolua_endmodule(tolua_S);
	std::string typeName = typeid(CVTimeUtils).name();
	g_luaType[typeName] = "CVTimeUtils";
	g_typeCast["CVTimeUtils"] = "CVTimeUtils";
	return 1;
}

//---------------------------------------------------------  TimeCalculator  --------------------------------------------------------------------

int lua_mgame_TimeCalculator_getCurrentTime(lua_State* tolua_S)
{
	int argc = 0;
	TimeCalculator* cobj = nullptr;
	bool ok = true;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertype(tolua_S, 1, "TimeCalculator", 0, &tolua_err)) goto tolua_lerror;
#endif

	cobj = (TimeCalculator*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
	if (!cobj)
	{
		tolua_error(tolua_S, "invalid 'cobj' in function 'lua_mgame_TimeCalculator_getCurrentTime'", nullptr);
		return 0;
	}
#endif

	argc = lua_gettop(tolua_S) - 1;
	if (argc == 0)
	{
		if (!ok)
		{
			tolua_error(tolua_S, "invalid arguments in function 'lua_mgame_TimeCalculator_getCurrentTime'", nullptr);
			return 0;
		}
		unsigned long long ret = cobj->getCurrentTime();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "TimeCalculator:getCurrentTime", argc, 0);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_mgame_TimeCalculator_getCurrentTime'.", &tolua_err);
#endif

	return 0;
}

int lua_mgame_TimeCalculator_getTimeLeft(lua_State* tolua_S)
{
	int argc = 0;
	TimeCalculator* cobj = nullptr;
	bool ok = true;
#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
	if (!tolua_isusertype(tolua_S, 1, "TimeCalculator", 0, &tolua_err)) goto tolua_lerror;
#endif
	cobj = (TimeCalculator*)tolua_tousertype(tolua_S, 1, 0);
#if COCOS2D_DEBUG >= 1
	if (!cobj)
	{
		tolua_error(tolua_S, "invalid 'cobj' in function 'lua_mgame_TimeCalculator_getTimeLeft'", nullptr);
		return 0;
	}
#endif
	argc = lua_gettop(tolua_S) - 1;
	do{
		if (argc == 1) {
			std::string arg0;
			ok &= luaval_to_std_string(tolua_S, 2, &arg0, "TimeCalculator:getTimeLeft");

			if (!ok) { break; }
			double ret = cobj->getTimeLeft(arg0);
			tolua_pushnumber(tolua_S, (lua_Number)ret);
			return 1;
		}
	} while (0);
	ok = true;
	do{
		if (argc == 2) {
			std::string arg0;
			ok &= luaval_to_std_string(tolua_S, 2, &arg0, "TimeCalculator:getTimeLeft");

			if (!ok) { break; }
			double arg1;
			ok &= luaval_to_number(tolua_S, 3, &arg1, "TimeCalculator:getTimeLeft");

			if (!ok) { break; }
			float tmpArg1 = arg1;
			bool ret = cobj->getTimeLeft(arg0, tmpArg1);

			tolua_pushboolean(tolua_S, (bool)ret);
			tolua_pushnumber(tolua_S, tmpArg1);

			return 2;
		}
	} while (0);
	ok = true;
	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "TimeCalculator:getTimeLeft", argc, 2);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_mgame_TimeCalculator_getTimeLeft'.", &tolua_err);
#endif

	return 0;
}

int lua_register_mgame_TimeCalculator_manual(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "TimeCalculator");
	tolua_cclass(tolua_S, "TimeCalculator", "TimeCalculator", "", nullptr);

	tolua_beginmodule(tolua_S, "TimeCalculator");
	tolua_function(tolua_S, "getCurrentTime", lua_mgame_TimeCalculator_getCurrentTime);
	tolua_function(tolua_S, "getTimeLeft", lua_mgame_TimeCalculator_getTimeLeft);
	tolua_endmodule(tolua_S);
	std::string typeName = typeid(TimeCalculator).name();
	g_luaType[typeName] = "TimeCalculator";
	g_typeCast["TimeCalculator"] = "TimeCalculator";
	return 1;
}

static int lua_mgame_LuaFuncHandlerManager_registerScriptHandler(lua_State* tolua_S)
{
	int argc = 0;
	LuaFuncHandlerManager* self = NULL;
	bool ok = true;

#if COCOS2D_DEBUG >= 1
	tolua_Error tolua_err;
	if (!tolua_isusertype(tolua_S, 1, "LuaFuncHandlerManager", 0, &tolua_err)) goto tolua_lerror;
#endif

	self = static_cast<LuaFuncHandlerManager*>(tolua_tousertype(tolua_S, 1, 0));

#if COCOS2D_DEBUG >= 1
	if (nullptr == self) {
		tolua_error(tolua_S, "invalid 'self' in function 'lua_mgame_LuaFuncHandlerManager_registerScriptHandler'\n", NULL);
		return 0;
	}
#endif

	argc = lua_gettop(tolua_S) - 1;

	if (argc == 3)
	{
#if COCOS2D_DEBUG >= 1
		if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err) ||
			!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
			!tolua_isnumber(tolua_S, 4, 0, &tolua_err))
		{
			goto tolua_lerror;
		}
#endif
		LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);

		auto moduleType = static_cast<LuaFuncHandlerManager::ModuleType>((int)tolua_tonumber(tolua_S, 3, 0));
		auto handlerType = (int)tolua_tonumber(tolua_S, 4, 0);

		self->addHandler(moduleType, handler, handlerType);

		CCLOG("[trace]lua_mgame_LuaFuncHandlerManager_registerScriptHandler : registerScriptHandler success.");

		return 0;
	}

	luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "LuaFuncHandlerManager:registerScriptHandler", argc, 1);
	return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
	tolua_error(tolua_S, "#ferror in function 'lua_mgame_LuaFuncHandlerManager_registerScriptHandler'.", &tolua_err);
	return 0;
#endif
}


int lua_register_mgame_LuaFunctionHelper_manual(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "LuaFuncHandlerManager");
	tolua_cclass(tolua_S, "LuaFuncHandlerManager", "LuaFuncHandlerManager", "", nullptr);

	tolua_beginmodule(tolua_S, "LuaFuncHandlerManager");
		tolua_function(tolua_S, "registerScriptHandler", lua_mgame_LuaFuncHandlerManager_registerScriptHandler);
	tolua_endmodule(tolua_S);

	std::string typeName = typeid(TimeCalculator).name();
	g_luaType[typeName] = "LuaFuncHandlerManager";
	g_typeCast["LuaFuncHandlerManager"] = "LuaFuncHandlerManager";
	return 1;
}

TOLUA_API int register_all_mgame_manual(lua_State* tolua_S)
{
	tolua_open(tolua_S);


	tolua_module(tolua_S, nullptr, 0);
	tolua_beginmodule(tolua_S, nullptr);

	lua_register_mgame_NetPack_manual(tolua_S);
	lua_register_mgame_NetManager_manual(tolua_S);
	lua_register_mgame_LuaProxy_manual(tolua_S);
	tolua_endmodule(tolua_S);


	tolua_module(tolua_S, "cc", 0);
	tolua_beginmodule(tolua_S, "cc");

	tolua_function(tolua_S, "getSingleManager", lua_register_getSingleManager);

	//lua_register_singleton_get(tolua_S);
	//lua_register_singleton_free(tolua_S);

	tolua_endmodule(tolua_S);

	lua_register_mgame_CVTimeUtils_manual(tolua_S);
	lua_register_mgame_TimeCalculator_manual(tolua_S);
	lua_register_mgame_LuaFunctionHelper_manual(tolua_S);

	return 1;
}
