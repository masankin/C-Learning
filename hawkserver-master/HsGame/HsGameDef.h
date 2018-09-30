#ifndef __HS_GAMEDEF_H__
#define __HS_GAMEDEF_H__

#include "HawkAppFrame.h"

#ifdef PLATFORM_WINDOWS
#	include "HawkWin32.h"
#endif

namespace Hawk
{
	//游戏常量定义
	enum
	{
		HS_PLAYER_CACHE_TIME = 300000,
		HS_OBJ_TICK_PERIOD	 = 200,
	};

	//服务器对象类型
	enum
	{
		HSO_UNKNOWN = 0,

		//单例对象类型
		HSO_MANAGER,

		//游戏对象
		HSO_PLAYER,
	};

	//全局管理器对象ID定义
	enum
	{
		HMI_NONE = 0,

		HMI_APP,
	};

	//服务器消息类型
	enum
	{
		HSM_UNKNOWN = 0,

		//释放对象
		HSM_DELOBJ,

		//连接断开
		HSM_DISCONNECT,
	};
}
#endif
