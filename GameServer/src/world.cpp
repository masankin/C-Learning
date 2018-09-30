#include "world.h"
#include "IniFile.h"
#include "worldServer.h"
#include "Logger.h"
world::world(void)
{
	m_ptread = nullptr;
	m_bRun = true;
}

world::~world(void)
{
	if (m_ptread != nullptr)
	{
		delete m_ptread;
		m_ptread = nullptr;
	}
}

void world::start()
{
	IniFile::get_instance().Load("server/config.ini");
	Logger::get_instance().init("log");
	worldServer::get_instance().start();

	m_ptread = new thread([=](){
		unsigned long start = 0 ;
		unsigned long tick = 40;
		unsigned long sl = 0;
		while (m_bRun)
		{
			start = GetTickCount();

			worldServer::get_instance().update();

			sl = GetTickCount() - start;
			sl = sl>tick?0:tick-sl;
			//Logger::get_instance().TraceKeyInfo("sleep %d", sl);
			this_thread::sleep_for(chrono::milliseconds(sl));
		}
	});

	m_ptread->join();
}