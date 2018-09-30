#ifndef __HS_GAME_H__
#define __HS_GAME_H__

#include "HsGameDef.h"

namespace Hawk
{
	/************************************************************************/
	/* 游戏启动类封装                                                       */
	/************************************************************************/
	class HsGame : public HawkAppFrame
	{
	public:
		//构造
		HsGame();

		//析构
		virtual ~HsGame();

	public:
		//游戏初始化
		virtual Bool	Init(const AString& sCfgFile);

		//运行
		virtual Bool	Run();		

		//停止
		virtual Bool	Stop();

	protected:
		//更新
		virtual Bool    OnTick(UInt32 iPeriod);

		//异常处理(存储状态)
		virtual Bool    OnException(Int32 iCode);

		//消息
		virtual Bool    OnMessage(const HawkMsg& sMsg);

		//会话开启(网关模式回调)
		virtual Bool	OnSessionStart(SID iSid, const AString& sAddr);

		//会话协议通知(网关模式回调)
		virtual Bool	OnSessionProtocol(SID iSid, Protocol* pProto);

		//会话结束通知(网关模式回调)
		virtual Bool	OnSessionClose(SID iSid, const AString& sAddr);

		//投递协议
		virtual Bool	PostProtocol(SID iSid, Protocol* pProto);

		//投递消息
		virtual Bool	DispatchMsg(const XID& sXid, HawkMsg* pMsg);

		//应用层创建对象
		HawkAppObj*		AppCreateObj(const XID& sXid);

	protected:
		//注册对象类型并创建基础对象
		virtual Bool	InitGameObj();

		//更新玩家
		virtual Bool    TickPlayers();

		//更新管理器
		virtual Bool    TickManagers();
	};

	extern HsGame* g_Game;
}
#endif
