#ifndef __LUA_PROXY_H__ 
#define __LUA_PROXY_H__

#include "BaseProxy.h"




class LuaProxy :public BaseProxy
{
public:
	enum {eBufSize=65535};

	static LuaProxy* getInstance();

	void registerHandler(short code, int handler);
	int getRegisterHandler(short code);

	virtual void DoRsp(NetPack& stRsp);
protected:
	static LuaProxy* s_luaProxy;
	LuaProxy();
	virtual ~LuaProxy();

	std::unordered_map<short, int> m_handlers;
	char m_buf[eBufSize];
};


#endif
 
