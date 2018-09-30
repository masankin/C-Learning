#ifndef __HS_PLAYER_H__
#define __HS_PLAYER_H__

#include "HsPlayerData.h"
#include "HsPlayerModule.h"

namespace Hawk
{
	/************************************************************************/
	/* 玩家对象封装                                                         */
	/************************************************************************/
	class HsPlayer : public HawkAppObj
	{
	public:
		//构造
		HsPlayer(const XID& sId = XID());

		//析构
		virtual ~HsPlayer();

		//逻辑模块列表
		friend class HsPlayerModule;

		//逻辑模块列表
		typedef vector<HsPlayerModule*> PlayerModules;	

	public:
		//玩家消息处理
		virtual Bool    OnMessage(const HawkMsg& sMsg);
		
		//玩家协议处理
		
		virtual Bool    OnProtocol(SID iSid,Protocol* pProto);	

		//玩家更新
		virtual Bool    OnTick();

		//异常处理
		virtual Bool    OnException(Int32 iCode);

	public:
		//获取会话ID
		virtual SID		GetSid() const;

		//设置会话ID
		virtual Bool	SetSid(SID iSid);

		//玩家发送协议
		virtual Bool    SendProtocol(Protocol* pProto);

		//是否在线
		virtual Bool    IsOnline() const;
		
		//暂停删除计时器
		virtual Bool    PauseDelTimer(Bool bPause);

	protected:
		//注册模块
		virtual Bool    RegisterModules();

		//定时器更新
		virtual Bool	OnInnerTick();	

		//玩家消息处理
		virtual Bool    OnInnerMsg(const HawkMsg& sMsg);

		//玩家协议处理
		virtual Bool	OnInnerProtocol(SID iSid, Protocol* pProto);

	public:
		//玩家登陆完成初始化
		virtual Bool    OnPlayerEnter();

		//玩家退出完成释放
		virtual Bool    OnPlayerLeave();

		//玩家下线消息
		virtual Bool    OnPlayerLogout();

	public:				
		//获取逻辑模块
		HsPlayerModule*	GetModule(const AString& sModuleName);

		//获取玩家数据
		const HsPlayerData* GetData() const;

	protected:
		//会话ID
		SID				m_iSid;
		//玩家数据对象
		HsPlayerData*	m_pData;
		//玩家模块列表
		PlayerModules	m_vModule;
		//自删除定时器
		HawkCounter		m_sDelTimer;
	};
}
#endif
