#ifndef _HSROBOT_H_
#define _HSROBOT_H_

#include "HsRobotEntity.h"

namespace Hawk
{
	class HsRobot : public HawkRefCounter
	{
	public:
		HsRobot();

		virtual ~HsRobot();

		friend class HsRobotEntity;

	public:
		//初始化机器人系统
		virtual Bool  Init(const AString& sCfg);

		//运行
		virtual Bool  Run();

		//退出
		virtual Bool  Stop();

	protected:
		volatile Bool		m_bRunning;
		AString				m_sSvrAddr;
		Int32				m_iCount;
		vector<HawkThread*> m_vThreads;
	};
	extern HsRobot* g_Robot;
}
#endif
