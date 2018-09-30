#include "HsPlayer.h"
#include "HsGame.h"

namespace Hawk
{
	Bool HsPlayer::OnInnerProtocol(SID iSid, Protocol* pProto)
	{
		//»ØÏÔ·þÎñ
		if (pProto->GetType() == ProtocolId::MSG_NOTIFY && ((MsgNotify*)pProto)->m_iMode == HAWK_ERROR)
		{
			SendProtocol(pProto);
			return true;
		}

		return false;
	}
}
