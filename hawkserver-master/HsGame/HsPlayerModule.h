#ifndef __HS_PLAYER_MODULE_H__
#define __HS_PLAYER_MODULE_H__

#include "HsPlayerData.h"

namespace Hawk
{
	class HsPlayer;
	/************************************************************************/
	/* 玩家模块基类                                                         */
	/************************************************************************/
	class HsPlayerModule : public HawkRefCounter
	{
	public:
		//构造
		HsPlayerModule(HsPlayer* pPlayer = 0, const AString& sModuleName = "");

		//析构
		virtual ~HsPlayerModule();

	public:
		//获取玩家指针
		HsPlayer*	  GetPlayer();		

		//获取模块名
		AString		  GetModuleName() const;

	public:
		//模块内更新
		virtual Bool  OnTick();

		//处理模块内消息
		virtual Bool  OnMessage(const HawkMsg& sMsg);

		//处理模块内协议
		virtual Bool  OnProtocol(SID iSid, Protocol* pProto);		

	public:
		//玩家进入初始化
		virtual Bool  OnPlayerEnter();

		//玩家退出释放数据
		virtual Bool  OnPlayerLeave();

	protected:
		//玩家对象
		HsPlayer*  m_pPlayer;
		//模块名
		AString	   m_sModule;
	};
}
#endif
