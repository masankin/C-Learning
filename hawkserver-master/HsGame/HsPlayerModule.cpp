#include "HsPlayerModule.h"
#include "HsGame.h"
#include "HsPlayer.h"

namespace Hawk
{
	HsPlayerModule::HsPlayerModule(HsPlayer* pPlayer, const AString& sModuleName) : m_pPlayer(pPlayer), m_sModule(sModuleName)
	{
	}

	HsPlayerModule::~HsPlayerModule()
	{

	}

	HsPlayer* HsPlayerModule::GetPlayer()
	{
		return m_pPlayer;
	}

	AString HsPlayerModule::GetModuleName() const
	{
		return m_sModule;
	}

	Bool HsPlayerModule::OnMessage(const HawkMsg& sMsg)
	{
		return false;
	}

	Bool HsPlayerModule::OnProtocol(SID iSid, Protocol* pProto)
	{
		return false;
	}

	Bool HsPlayerModule::OnTick()
	{
		return true;
	}

	Bool HsPlayerModule::OnPlayerEnter()
	{
		return true;
	}

	Bool HsPlayerModule::OnPlayerLeave()
	{
		return true;
	}
}
