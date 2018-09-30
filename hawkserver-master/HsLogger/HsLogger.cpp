#include "HsLogger.h"

namespace Hawk
{
	HsLogger* g_Logger = 0;

	HsLogger::HsLogger() : HawkLogServer()
	{
		HawkAssert(!g_Logger || g_Logger == this);
		g_Logger = this;
	}

	HsLogger::~HsLogger()
	{
	}

	Bool HsLogger::Init(const AString& sCfg)
	{
		if (!m_bRunning)
		{
			HawkXmlFile xmlCfg;
			HawkXmlDocument<AString> xmlDoc;
			if (!xmlCfg.Open(sCfg, xmlDoc))
			{
				HawkPrint("LogServer Open Config Error.");
				return false;
			}

			AXmlElement* pRoot  = xmlDoc.GetRoot();
			AXmlElement* pCfg   = pRoot->GetChildren("LgCfg");
			AXmlElement* pDbCfg = pRoot->GetChildren("DbCfg");

			if(!pCfg || !pDbCfg || !pCfg->GetAttribute("Addr"))
			{
				HawkPrint("LogServer Config Tag Error.");
				return false;
			}

			AString sAddr = pCfg->GetAttribute("Addr")->StringValue();

			if (pRoot->GetAttribute("Console"))
				EnableConsole(pRoot->GetAttribute("Console")->BoolValue());

			HawkDatabase::DBConn sConn(HawkDatabase::HDB_MYSQL);

			if (pDbCfg->GetAttribute("Host"))
				sConn.SetHost(pDbCfg->GetAttribute("Host")->StringValue());

			if (pDbCfg->GetAttribute("Port"))
				sConn.SetPort(pDbCfg->GetAttribute("Port")->IntValue());

			if (pDbCfg->GetAttribute("User"))
				sConn.SetUser(pDbCfg->GetAttribute("User")->StringValue());

			if (pDbCfg->GetAttribute("Pwd"))
				sConn.SetPwd(pDbCfg->GetAttribute("Pwd")->StringValue());

			sConn.SetDBName(pDbCfg->GetAttribute("DB")->StringValue());
			
			return HawkLogServer::Init(sAddr, sConn);
		}
		return false;
	}
}
