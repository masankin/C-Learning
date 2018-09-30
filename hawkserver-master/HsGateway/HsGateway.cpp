#include "HsGateway.h"

#define TGW_BUFFER_SIZE 80

namespace Hawk
{
	HsGateway* g_Gateway = 0;

	HsGateway::HsGateway() : HawkGateway()
	{
		HawkAssert(!g_Gateway || g_Gateway == this);
		g_Gateway   = this;
		m_iPlatform = 0;
	}

	HsGateway::~HsGateway()
	{
	}

	Bool HsGateway::Init(const AString& sCfg)
	{
		if (!m_bRunning)
		{
			HawkXmlFile xmlCfg;
			HawkXmlDocument<AString> xmlDoc;
			if (!xmlCfg.Open(sCfg, xmlDoc))
			{
				HawkPrint("Gateway Open Config Error.");
				return false;
			}

			AXmlElement* pRoot  = xmlDoc.GetRoot();
			AXmlElement* pCfg   = pRoot->GetChildren("GwCfg");

			if(!pCfg || !pCfg->GetAttribute("Frontend") || !pCfg->GetAttribute("Backend"))
			{
				HawkPrint("Gateway Config Tag Error.");
				return false;
			}

			AString sFrontend = pCfg->GetAttribute("Frontend")->StringValue();
			AString sBackend  = pCfg->GetAttribute("Backend")->StringValue();
			HawkStringUtil::Replace<AString>(sBackend, "*", "127.0.0.1");

			Int32 iThread     = 4;
			if (pCfg->GetAttribute("Threads"))
				iThread = pCfg->GetAttribute("Threads")->IntValue();
			
			Bool bCrossDomain = false;
			if (pCfg->GetAttribute("CrossDomain"))
				bCrossDomain = pCfg->GetAttribute("CrossDomain")->BoolValue();

			if (pCfg->GetAttribute("Platform"))
				m_iPlatform = pCfg->GetAttribute("Platform")->IntValue();

			AString sProfiler = "";
			if (pCfg->GetAttribute("Profiler"))
				sProfiler = pCfg->GetAttribute("Profiler")->StringValue();

			Int32 iKeepAlive = 0;
			if (pCfg->GetAttribute("KeepAlive"))
				iKeepAlive = pCfg->GetAttribute("KeepAlive")->IntValue();

			if (HawkGateway::Init(sFrontend, sBackend, iThread))
			{
				P_ProtocolManager->SetSizeLimit(PAGE_SIZE);

				if (iKeepAlive > 0)
					SetSessionTimeout(iKeepAlive);

				if (bCrossDomain)
					TurnOnCrossDomain();

				if (sProfiler.size())
					TurnOnProfiler(sProfiler);

				return true;
			}
		}
		return false;
	}

	Bool HsGateway::TrustSession(void* pSession, OctetsStream* pIBuffer)
	{
		if (pSession && pIBuffer)
		{
			//QQ平台网关校验信息过滤
			if (m_iPlatform == 1)
			{
				if (pIBuffer->AvailableSize() >= TGW_BUFFER_SIZE)
				{
					AString sTgw = (Char*)pIBuffer->AvailableData();

#ifdef _DEBUG
					HawkFmtPrint("TGW: %s", sTgw.c_str());
#endif

					Bool bStart = HawkStringUtil::BeginWith<AString>(sTgw, "tgw_l7_forward");
					Bool bEnd   = HawkStringUtil::EndWith<AString>(sTgw, "\r\n\r\n");

					if (bStart && bEnd)
					{
						UInt32 iCrc  = HawkOSOperator::CalcCrc((const UChar*)sTgw.c_str(),(UInt32)sTgw.size());
						UInt32* pCrc = (UInt32*)((Char*)pIBuffer->AvailableData() + (TGW_BUFFER_SIZE - sizeof(UInt32)));
						if (pCrc && iCrc == *pCrc)
						{
							pIBuffer->MoveNonius(TGW_BUFFER_SIZE);
							return true;
						}
					}
				}
				return false;
			}
		}
		return true;
	}
}
