#include "HsPlayerLoginModule.h"
#include "HsGame.h"
#include "HsPlayer.h"

namespace Hawk
{
	HsPlayerLoginModule::HsPlayerLoginModule(HsPlayer* pPlayer, const AString& sModuleName) : HsPlayerModule(pPlayer, sModuleName)
	{

	}

	HsPlayerLoginModule::~HsPlayerLoginModule()
	{

	}

	Bool HsPlayerLoginModule::OnProtocol(SID iSid, Protocol* pProto)
	{
		ProtoType eType = pProto->GetType();
		if (eType == ProtocolId::LOGIN_GAME)
		{
			//设置玩家会话ID
			m_pPlayer->SetSid(iSid);
			//通知玩家进入游戏
			m_pPlayer->OnPlayerEnter();
			return true;
		}

		return false;
	}

	Bool HsPlayerLoginModule::OnPlayerEnter()
	{
		return true;
	}

	Bool HsPlayerLoginModule::OnPlayerLeave()
	{
		return true;
	}
}
