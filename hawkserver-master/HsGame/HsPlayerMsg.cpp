#include "HsPlayer.h"
#include "HsGame.h"

namespace Hawk
{
	Bool HsPlayer::OnInnerTick()
	{
		//ɾ����ʱ��
		if (m_sDelTimer.IncCounter(HS_OBJ_TICK_PERIOD))
		{
			PauseDelTimer(true);

			//֪ͨAPPɾ���Լ�
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
		//�ر�ɾ����ʱ��
		PauseDelTimer(true);

		//֪ͨ��ÿ��ģ��
		for (Size_t i=0;i<m_vModule.size();i++)
		{
			if (m_vModule[i])
				m_vModule[i]->OnPlayerEnter();
		}

		//�󶨶���ͻỰID
		g_Game->BindSidXid(m_iSid, m_sXid);
		return true;
	}

	Bool HsPlayer::OnPlayerLeave()
	{
		//֪ͨ��ÿ��ģ��
		for (Size_t i=0;i<m_vModule.size();i++)
		{
			if (m_vModule[i])
				m_vModule[i]->OnPlayerLeave();
		}

		//����ỰID�Ͷ���İ�
		g_Game->UnbindSidXid(m_iSid);

		//����ɾ����ʱ��
		PauseDelTimer(false);

		return true;
	}

	Bool HsPlayer::OnPlayerLogout()
	{
		//����״̬���˳�
		if (IsOnline())
			OnPlayerLeave();

		return true;
	}
}
