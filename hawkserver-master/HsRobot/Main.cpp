#include "HsRobot.h"

HawkThread* g_DbgThread = 0;

PVoid DoKeyDownExit(void* pArgs)
{
	HsRobot* pApp = (HsRobot*)pArgs;
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

void InitDbgThread(HsRobot* pApp)
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

	g_Robot = new HsRobot;

#if defined(_DEBUG) && defined(PLATFORM_WINDOWS)
	if (g_Robot->Init("../Config/SvrCfg.xml"))
#else
	if (g_Robot->Init("Config/SvrCfg.xml"))
#endif
	{
		HawkPrint("==============================================================");
		HawkPrint("                     Robot is Running");
		HawkPrint("==============================================================");

		InitDbgThread(g_Robot);

		g_Robot->Run();
		g_Robot->Stop();
	}
	else
	{
		HawkPrint("Robot Initialize Failed.");
	}

	HawkUtil::Stop();

	HAWK_RELEASE(g_DbgThread);
	HAWK_RELEASE(g_Robot);

	HawkUtil::Release();
	return 0;
}
