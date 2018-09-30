#ifndef __HS_PLAYER_MODULE_H__
#define __HS_PLAYER_MODULE_H__

#include "HsPlayerData.h"

namespace Hawk
{
	class HsPlayer;
	/************************************************************************/
	/* ���ģ�����                                                         */
	/************************************************************************/
	class HsPlayerModule : public HawkRefCounter
	{
	public:
		//����
		HsPlayerModule(HsPlayer* pPlayer = 0, const AString& sModuleName = "");

		//����
		virtual ~HsPlayerModule();

	public:
		//��ȡ���ָ��
		HsPlayer*	  GetPlayer();		

		//��ȡģ����
		AString		  GetModuleName() const;

	public:
		//ģ���ڸ���
		virtual Bool  OnTick();

		//����ģ������Ϣ
		virtual Bool  OnMessage(const HawkMsg& sMsg);

		//����ģ����Э��
		virtual Bool  OnProtocol(SID iSid, Protocol* pProto);		

	public:
		//��ҽ����ʼ��
		virtual Bool  OnPlayerEnter();

		//����˳��ͷ�����
		virtual Bool  OnPlayerLeave();

	protected:
		//��Ҷ���
		HsPlayer*  m_pPlayer;
		//ģ����
		AString	   m_sModule;
	};
}
#endif
