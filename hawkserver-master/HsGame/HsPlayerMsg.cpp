#include "HsPlayer.h"
#include "HsGame.h"

namespace Hawk
{
	Bool HsPlayer::OnInnerTick()
	{
		//删除定时器
		if (m_sDelTimer.IncCounter(HS_OBJ_TICK_PERIOD))
		{
			PauseDelTimer(true);

			//通知APP删除自己
			HawkMsg* pMsg = P_MsgManager->GetMsg(HSM_DELOBJ);
			if (pMsg)
			{			
				pMsg->Params.Push<XID>(m_sXid);
				g_Game->PostMsg(XID(HSO_MANAGER, HMI_APP), pMsg);
			}			
		}

		return true;
	}

	Bool HsPlayer::OnInnerMsg(const HawkMsg& sMsg)
	{
		if (sMsg.Msg == HSM_DISCONNECT)
		{
			OnPlayerLogout();
			return true;
		}

		return false;
	}
	
	Bool HsPlayer::OnPlayerEnter()
	{
		//关闭删除计时器
		PauseDelTimer(true);

		//通知到每个模块
		for (Size_t i=0;i<m_vModule.size();i++)
		{
			if (m_vModule[i])
				m_vModule[i]->OnPlayerEnter();
		}

		//绑定对象和会话ID
		g_Game->BindSidXid(m_iSid, m_sXid);
		return true;
	}

	Bool HsPlayer::OnPlayerLeave()
	{
		//通知到每个模块
		for (Size_t i=0;i<m_vModule.size();i++)
		{
			if (m_vModule[i])
				m_vModule[i]->OnPlayerLeave();
		}

		//解除会话ID和对象的绑定
		g_Game->UnbindSidXid(m_iSid);

		//开启删除计时器
		PauseDelTimer(false);

		return true;
	}

	Bool HsPlayer::OnPlayerLogout()
	{
		//在线状态即退出
		if (IsOnline())
			OnPlayerLeave();

		return true;
	}
}
