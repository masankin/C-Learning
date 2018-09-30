#ifndef _HSGATEWAY_H_
#define _HSGATEWAY_H_

#include "HawkGateway.h"

#ifdef PLATFORM_WINDOWS
#	include "HawkWin32.h"
#endif

namespace Hawk
{
	class HsGateway : public HawkGateway
	{
	public:
		HsGateway();

		virtual ~HsGateway();

	public:
		virtual Bool  Init(const AString& sCfg);

		virtual Bool  TrustSession(void* pSession, OctetsStream* pIBuffer);

	protected:
		Int32  m_iPlatform;
	};
	extern HsGateway* g_Gateway;
}
#endif
