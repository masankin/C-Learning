#ifndef __HS_PLAYER_H__
#define __HS_PLAYER_H__

#include "HsPlayerData.h"
#include "HsPlayerModule.h"

namespace Hawk
{
	/************************************************************************/
	/* ��Ҷ����װ                                                         */
	/************************************************************************/
	class HsPlayer : public HawkAppObj
	{
	public:
		//����
		HsPlayer(const XID& sId = XID());

		//����
		virtual ~HsPlayer();

		//�߼�ģ���б�
		friend class HsPlayerModule;

		//�߼�ģ���б�
		typedef vector<HsPlayerModule*> PlayerModules;	

	public:
		//�����Ϣ����
		virtual Bool    OnMessage(const HawkMsg& sMsg);
		
		//���Э�鴦��
		
		virtual Bool    OnProtocol(SID iSid,Protocol* pProto);	

		//��Ҹ���
		virtual Bool    OnTick();

		//�쳣����
		virtual Bool    OnException(Int32 iCode);

	public:
		//��ȡ�ỰID
		virtual SID		GetSid() const;

		//���ûỰID
		virtual Bool	SetSid(SID iSid);

		//��ҷ���Э��
		virtual Bool    SendProtocol(Protocol* pProto);

		//�Ƿ�����
		virtual Bool    IsOnline() const;
		
		//��ͣɾ����ʱ��
		virtual Bool    PauseDelTimer(Bool bPause);

	protected:
		//ע��ģ��
		virtual Bool    RegisterModules();

		//��ʱ������
		virtual Bool	OnInnerTick();	

		//�����Ϣ����
		virtual Bool    OnInnerMsg(const HawkMsg& sMsg);

		//���Э�鴦��
		virtual Bool	OnInnerProtocol(SID iSid, Protocol* pProto);

	public:
		//��ҵ�½��ɳ�ʼ��
		virtual Bool    OnPlayerEnter();

		//����˳�����ͷ�
		virtual Bool    OnPlayerLeave();

		//���������Ϣ
		virtual Bool    OnPlayerLogout();

	public:				
		//��ȡ�߼�ģ��
		HsPlayerModule*	GetModule(const AString& sModuleName);

		//��ȡ�������
		const HsPlayerData* GetData() const;

	protected:
		//�ỰID
		SID				m_iSid;
		//������ݶ���
		HsPlayerData*	m_pData;
		//���ģ���б�
		PlayerModules	m_vModule;
		//��ɾ����ʱ��
		HawkCounter		m_sDelTimer;
	};
}
#endif
