#include "LuaProxy.h"

#include "CCLuaStack.h"
#include "CCLuaEngine.h"
#include "net/NetManager.h"

LuaProxy::LuaProxy()
{
}

LuaProxy::~LuaProxy()
{
}
LuaProxy* LuaProxy::s_luaProxy = nullptr;

LuaProxy* LuaProxy::getInstance()
{
	if (s_luaProxy == nullptr)
	{
		s_luaProxy = new LuaProxy;
	}
	return s_luaProxy;
}

void LuaProxy::registerHandler(short code, LUA_FUNCTION handler)
{
	m_handlers[code] = handler;
	sgNetManager()->RegCmdListener(code, this);
}

LUA_FUNCTION LuaProxy::getRegisterHandler(short code)
{
	return m_handlers[code];
}

void LuaProxy::DoRsp(NetPack& stRsp)
{
	const int & msgId = stRsp.getMsgHead()->mMsgID;
	const fgame::S2ClientMsg* msg = stRsp.getS2CMsg();
	int handler = getRegisterHandler(msgId);
	if (handler != 0 )
	{
		msg->SerializeToArray(m_buf, msg->ByteSize());
		LuaStack* stack = LuaEngine::getInstance()->getLuaStack();

		const char*  functionName = "NetManager_receiveData";
		lua_getglobal(stack->getLuaState(), functionName);       /* query function by name, stack: function */
		if (!lua_isfunction(stack->getLuaState(), -1))
		{
			CCLOG("[LUA ERROR] name '%s' does not represent a Lua function", functionName);
			lua_pop(stack->getLuaState(), 1);
			return ;
		}
		stack->pushInt(msgId);
		stack->pushString(m_buf, msg->ByteSize());
		stack->executeFunction(2);
	}
	else
	{
		CCLOG("[error] %dhandler not found", msgId);
	}
}
