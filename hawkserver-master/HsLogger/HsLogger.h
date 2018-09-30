#ifndef _HSLOGGER_H_
#define _HSLOGGER_H_

#include "HawkLog.h"

#ifdef PLATFORM_WINDOWS
#	include "HawkWin32.h"
#endif

namespace Hawk
{
	class HsLogger : public HawkLogServer
	{
	public:
		HsLogger();

		virtual ~HsLogger();

	public:
		virtual Bool  Init(const AString& sCfg);
	};
	extern HsLogger* g_Logger;
}
#endif
