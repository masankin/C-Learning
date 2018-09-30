#include "HsGame.h"

HawkThread* g_DbgThread = 0;

PVoid DoKeyDownExit(void* pArg)
{
	HsGame* pApp = (HsGame*)pArg;
	while (pApp)
	{
		if(HawkOSOperator::WaitKeyboardInput("",false) == 'Q')
		{
			if (pApp->BreakLoop())
				return 0;
		}
	}
	return 0;
}

PVoid DoCoreDump(void* pArg)
{
#ifdef PLATFORM_WINDOWS
	Char* pPtr   = 0; 
	HsGame* pApp = (HsGame*)pArg;	
	while (pApp)
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

void InitDbgThread(HsGame* pGame)
{
#ifdef PLATFORM_WINDOWS
#	ifdef _DEBUG
		g_DbgThread = new HawkThread(DoKeyDownExit);
#	else
		g_DbgThread = new HawkThread(DoCoreDump);
#	endif
#endif

	if (g_DbgThread)
		g_DbgThread->Start(pGame);
}

int main(int argc, Char* argv[])
{
	HawkUtil::Init();

	HsGame* pGame = new HsGame;

#if defined(_DEBUG) && defined(PLATFORM_WINDOWS)
	if (pGame->Init("../Config/gs.cfg"))
#else
	if (pGame->Init("Config/gs.cfg"))
#endif
	{
		InitDbgThread(pGame);

		pGame->Run();
	}
	else
	{
		HawkPrint("GameServer Initialize Failed.");
	}

	HawkUtil::Stop();

	HAWK_RELEASE(g_DbgThread);
	HAWK_RELEASE(pGame);

	HawkUtil::Release();

	return 0;
}
