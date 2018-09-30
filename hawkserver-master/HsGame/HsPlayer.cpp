#include "HsPlayer.h"
#include "HsGame.h"
#include "HsPlayerLoginModule.h"

namespace Hawk
{
	HsPlayer::HsPlayer(const XID& sId) : HawkAppObj(sId), m_sDelTimer(HS_PLAYER_CACHE_TIME)
	{
		RegisterModules();

		m_pData = new HsPlayerData;
	}

	HsPlayer::~HsPlayer()
	{
		for (Size_t i=0;i<m_vModule.size();i++)
		{
			HAWK_RELEASE(m_vModule[i]);
		}
		m_vModule.clear();

		HAWK_RELEASE(m_pData);
	}

	SID HsPlayer::GetSid() const
	{
		return m_iSid;
	}

	Bool HsPlayer::SetSid(SID iSid)
	{
		m_iSid = iSid;
		return m_iSid != 0;
	}

	Bool HsPlayer::PauseDelTimer(Bool bPause)
	{
		m_sDelTimer.Reset();
		m_sDelTimer.SetPause(bPause);
		return true;
	}

	Bool HsPlayer::SendProtocol(Protocol* pProto)
	{
		return g_Game->SendProtocol(m_iSid, pProto);
	}

	Bool HsPlayer::IsOnline() const
	{
		return true;
	}	

	Bool HsPlayer::OnMessage(const HawkMsg& sMsg)
	{
		if (OnInnerMsg(sMsg))
			return true;

		for (Size_t i=0;i<m_vModule.size();i++)
		{
			if (m_vModule[i] && m_vModule[i]->OnMessage(sMsg))
				return true;
		}

		return false;
	}

	Bool HsPlayer::OnProtocol(SID iSid,Protocol* pProto)
	{
		if (OnInnerProtocol(iSid, pProto))
			return true;

		for (Size_t i=0;i<m_vModule.size();i++)
		{
			if (m_vModule[i] && m_vModule[i]->OnProtocol(iSid, pProto))
				return true;
		}

		return false;
	}

	Bool HsPlayer::OnTick()
	{
		OnInnerTick();

		for (Size_t i=0;i<m_vModule.size();i++)
		{
			if (m_vModule[i])
				m_vModule[i]->OnTick();
		}

		return true;
	}

	Bool HsPlayer::OnException(Int32 iCode)
	{
		//ÔÚÏß×´Ì¬¼´ÍË³ö
		if (IsOnline())
			OnPlayerLeave();

		return true;
	}

	const HsPlayerData* HsPlayer::GetData() const
	{
		return m_pData;
	}

	HsPlayerModule*	HsPlayer::GetModule(const AString& sModuleName)
	{
		for (Size_t i=0;i<m_vModule.size();i++)
		{
			if (m_vModule[i] && m_vModule[i]->GetModuleName() == sModuleName)
				return m_vModule[i];
		}
		return 0;
	}

	Bool HsPlayer::RegisterModules()
	{
		#define REGISTER_MODULE(module_name) { m_vModule.push_back(new HsPlayer##module_name(this, #module_name)); }

		REGISTER_MODULE(LoginModule);

		#undef  REGISTER_MODULE
		return true;
	}
}
