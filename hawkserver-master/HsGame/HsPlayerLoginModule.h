#ifndef __HS_PLAYER_LOGINMODULE_H__
#define __HS_PLAYER_LOGINMODULE_H__

#include "HsPlayerModule.h"

namespace Hawk
{
	/************************************************************************/
	/* ��ҵ�½ģ�鹦��                                                     */
	/************************************************************************/
	class HsPlayerLoginModule : public HsPlayerModule
	{
	public:
		//����
		HsPlayerLoginModule(HsPlayer* pPlayer = 0, const AString& sModuleName = "");

		//����
		virtual ~HsPlayerLoginModule();

	public:
		//����ģ����Э��
		virtual Bool  OnProtocol(SID iSid, Protocol* pProto);

		//��ҽ����ʼ��
		virtual Bool  OnPlayerEnter();

		//����˳��ͷ�����
		virtual Bool  OnPlayerLeave();
	};
}
#endif
