#ifndef _HSROBOTENTITY_H_
#define _HSROBOTENTITY_H_

#include "HawkUtil.h"
#include "HawkGateway.h"

#ifdef PLATFORM_WINDOWS
#	include "HawkWin32.h"
#endif

#include "GsProtocol.h"

namespace Hawk
{
	class HsRobotEntity : public HawkRefCounter
	{
	public:
		HsRobotEntity();

		virtual ~HsRobotEntity();

	public:
		//初始化
		virtual Bool Init();

		//运行
		virtual Bool Run();

	protected:
		//接收到协议
		virtual Bool OnProtocol(Protocol* pProto);

	protected:
		//机器人会话
		HawkSession  m_sSession;
		//回显消息定时器
		HawkCounter  m_sEchoCounter;
	};
}
#endif
