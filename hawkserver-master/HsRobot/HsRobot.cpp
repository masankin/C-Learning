#include "HsRobot.h"

namespace Hawk
{
	HsRobot* g_Robot = 0;

	PVoid hawk_RobotEntityRoutine(void* pArg)
	{
		HsRobotEntity sRobot;
		if (sRobot.Init())
			sRobot.Run();
		return 0;
	}

	HsRobot::HsRobot()
	{
		HawkAssert(!g_Robot || g_Robot == this);
		g_Robot = this;

		#include "ProtocolReg.inl"

		m_iCount   = 0;
		m_bRunning = false;
	}

	HsRobot::~HsRobot()
	{
		for (Size_t i=0;i<m_vThreads.size();i++)
		{
			if (m_vThreads[i])
			{
				m_vThreads[i]->Close();
				HAWK_RELEASE(m_vThreads[i]);
			}			
		}
		m_vThreads.clear();
	}

	Bool HsRobot::Init(const AString& sCfg)
	{
		HawkXmlFile xmlCfg;
		HawkXmlDocument<AString> xmlDoc;
		if (!xmlCfg.Open(sCfg, xmlDoc))
		{
			HawkPrint("Robot Open Config Error.");
			return false;
		}

		AXmlElement* pRoot  = xmlDoc.GetRoot();
		if (pRoot->GetChildAttribute("Robot", "Addr"))
			m_sSvrAddr = pRoot->GetChildAttribute("Robot", "Addr")->StringValue();

		if (pRoot->GetChildAttribute("Robot", "Count"))
			m_iCount = pRoot->GetChildAttribute("Robot", "Count")->IntValue();
		
		return true;
	}

	Bool HsRobot::Run()
	{		
		m_bRunning = true;
		
		for (Int32 i=0;i<m_iCount;i++)
		{
			HawkThread* pThread = new HawkThread(hawk_RobotEntityRoutine);
			pThread->Start();
			m_vThreads.push_back(pThread);
		}

		while (m_bRunning)
		{
			HawkSleep(DEFAULT_SLEEP);
		}

		for (Size_t i=0;i<m_vThreads.size();i++)
		{
			if (m_vThreads[i])
			{
				m_vThreads[i]->Close();
				HAWK_RELEASE(m_vThreads[i]);
			}			
		}
		m_vThreads.clear();

		return true;
	}

	Bool HsRobot::Stop()
	{
		m_bRunning = false;
		return true;
	}
}
