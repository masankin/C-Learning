#ifndef __HS_GAMEDEF_H__
#define __HS_GAMEDEF_H__

#include "HawkAppFrame.h"

#ifdef PLATFORM_WINDOWS
#	include "HawkWin32.h"
#endif

namespace Hawk
{
	//��Ϸ��������
	enum
	{
		HS_PLAYER_CACHE_TIME = 300000,
		HS_OBJ_TICK_PERIOD	 = 200,
	};

	//��������������
	enum
	{
		HSO_UNKNOWN = 0,

		//������������
		HSO_MANAGER,

		//��Ϸ����
		HSO_PLAYER,
	};

	//ȫ�ֹ���������ID����
	enum
	{
		HMI_NONE = 0,

		HMI_APP,
	};

	//��������Ϣ����
	enum
	{
		HSM_UNKNOWN = 0,

		//�ͷŶ���
		HSM_DELOBJ,

		//���ӶϿ�
		HSM_DISCONNECT,
	};
}
#endif
