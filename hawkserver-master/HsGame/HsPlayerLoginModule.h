#ifndef __HS_PLAYER_LOGINMODULE_H__
#define __HS_PLAYER_LOGINMODULE_H__

#include "HsPlayerModule.h"

namespace Hawk
{
	/************************************************************************/
	/* 玩家登陆模块功能                                                     */
	/************************************************************************/
	class HsPlayerLoginModule : public HsPlayerModule
	{
	public:
		//构造
		HsPlayerLoginModule(HsPlayer* pPlayer = 0, const AString& sModuleName = "");

		//析构
		virtual ~HsPlayerLoginModule();

	public:
		//处理模块内协议
		virtual Bool  OnProtocol(SID iSid, Protocol* pProto);

		//玩家进入初始化
		virtual Bool  OnPlayerEnter();

		//玩家退出释放数据
		virtual Bool  OnPlayerLeave();
	};
}
#endif
