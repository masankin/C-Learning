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
		//��ʼ��
		virtual Bool Init();

		//����
		virtual Bool Run();

	protected:
		//���յ�Э��
		virtual Bool OnProtocol(Protocol* pProto);

	protected:
		//�����˻Ự
		HawkSession  m_sSession;
		//������Ϣ��ʱ��
		HawkCounter  m_sEchoCounter;
	};
}
#endif
