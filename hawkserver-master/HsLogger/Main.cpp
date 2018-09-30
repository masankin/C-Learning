#include "HsLogger.h"

HawkThread* g_DbgThread = 0;

PVoid DoKeyDownExit(void* pArgs)
{
	HsLogger* pApp = (HsLogger*)pArgs;
	while (pApp)
	{
		if(HawkOSOperator::WaitKeyboardInput("",false) == 'Q')
		{
			pApp->Stop();
			return 0;
		}
	}
	return 0;
}

PVoid DoCoreDump(void* pArgs)
{
#ifdef PLATFORM_WINDOWS
	Char* pPtr = 0; 
	while (true)
	{
		if(_getch() == 'd')
		{			
			__try
			{
				*pPtr = 0;
			}
			__except(HawkHandleException(GetExceptionInformation())) 
			{
			}
		}
	}
#endif
	return 0;
}

void InitDbgThread(HsLogger* pApp)
{
#ifdef PLATFORM_WINDOWS
#	ifdef _DEBUG
		g_DbgThread = new HawkThread(DoKeyDownExit);
#	else
		g_DbgThread = new HawkThread(DoCoreDump);
#	endif
#endif

	if (g_DbgThread)
		g_DbgThread->Start(pApp);
}

int main(int argc, Char* argv[])
{
	HawkUtil::Init();

	g_Logger = new HsLogger;

#if defined(_DEBUG) && defined(PLATFORM_WINDOWS)
	if (g_Logger->Init("../Config/SvrCfg.xml"))
#else
	if (g_Logger->Init("Config/SvrCfg.xml"))
#endif
	{
		HawkPrint("==============================================================");
		HawkPrint("                     LogServer is Running");
		HawkPrint("==============================================================");

		InitDbgThread(g_Logger);

		g_Logger->Run();
		g_Logger->Stop();
	}
	else
	{
		HawkPrint("LogServer Initialize Failed.");
	}

	HawkUtil::Stop();

	HAWK_RELEASE(g_DbgThread);
	HAWK_RELEASE(g_Logger);

	HawkUtil::Release();
	return 0;
}
