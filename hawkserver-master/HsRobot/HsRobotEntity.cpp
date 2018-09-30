#include "HsRobotEntity.h"
#include "HsRobot.h"

namespace Hawk
{
	HsRobotEntity::HsRobotEntity()
	{
	}

	HsRobotEntity::~HsRobotEntity()
	{
	}

	Bool HsRobotEntity::Init()
	{
		if (!m_sSession.Init(g_Robot->m_sSvrAddr))
		{
			HawkFmtError("Robot Session Init Error, Addr: %s", g_Robot->m_sSvrAddr.c_str());
			return false;
		}

		LoginGame sCmd;
		sCmd.m_sName = HawkStringUtil::RandomString<UString>(HawkRand::RandInt(8, 12));
		sCmd.m_sPwd  = HawkStringUtil::RandomString<UString>(HawkRand::RandInt(8, 12));
		m_sSession.SendProtocol(&sCmd);

		//回显消息定时器
		m_sEchoCounter.SetPause(false);
		m_sEchoCounter.SetPeriod(250);

		return true;
	}

	Bool HsRobotEntity::Run()
	{
		while (g_Robot->m_bRunning)
		{
			//更新
			if (!m_sSession.Tick(DEFAULT_SLEEP))
				break;

			//回显消息
			if (m_sEchoCounter.IncCounter(DEFAULT_SLEEP))
			{
				m_sEchoCounter.Reset();
				MsgNotify sCmd;
				sCmd.m_sMsg  = HawkStringUtil::RandomString<UString>(HawkRand::RandInt(32, 64));
				sCmd.m_iMode = HAWK_ERROR;
				m_sSession.SendProtocol(&sCmd);
			}

			//接收协议
			Protocol* pProto = 0;
			while (m_sSession.DecodeProtocol(pProto) && pProto)
			{
#ifdef _DEBUG
				//回显消息显示
				if (pProto->GetType() == ProtocolId::MSG_NOTIFY)
					HawkFmtPrint("Msg: %s", ((MsgNotify*)pProto)->m_sMsg.c_str());
#endif

				OnProtocol(pProto);

				P_ProtocolManager->ReleaseProto(pProto);
			}

			HawkSleep(DEFAULT_SLEEP);
		}
		return true;
	}

	Bool HsRobotEntity::OnProtocol(Protocol* pProto)
	{
		return true;
	}
}
