#ifndef __HS_GAME_H__
#define __HS_GAME_H__

#include "HsGameDef.h"

namespace Hawk
{
	/************************************************************************/
	/* ��Ϸ�������װ                                                       */
	/************************************************************************/
	class HsGame : public HawkAppFrame
	{
	public:
		//����
		HsGame();

		//����
		virtual ~HsGame();

	public:
		//��Ϸ��ʼ��
		virtual Bool	Init(const AString& sCfgFile);

		//����
		virtual Bool	Run();		

		//ֹͣ
		virtual Bool	Stop();

	protected:
		//����
		virtual Bool    OnTick(UInt32 iPeriod);

		//�쳣����(�洢״̬)
		virtual Bool    OnException(Int32 iCode);

		//��Ϣ
		virtual Bool    OnMessage(const HawkMsg& sMsg);

		//�Ự����(����ģʽ�ص�)
		virtual Bool	OnSessionStart(SID iSid, const AString& sAddr);

		//�ỰЭ��֪ͨ(����ģʽ�ص�)
		virtual Bool	OnSessionProtocol(SID iSid, Protocol* pProto);

		//�Ự����֪ͨ(����ģʽ�ص�)
		virtual Bool	OnSessionClose(SID iSid, const AString& sAddr);

		//Ͷ��Э��
		virtual Bool	PostProtocol(SID iSid, Protocol* pProto);

		//Ͷ����Ϣ
		virtual Bool	DispatchMsg(const XID& sXid, HawkMsg* pMsg);

		//Ӧ�ò㴴������
		HawkAppObj*		AppCreateObj(const XID& sXid);

	protected:
		//ע��������Ͳ�������������
		virtual Bool	InitGameObj();

		//�������
		virtual Bool    TickPlayers();

		//���¹�����
		virtual Bool    TickManagers();
	};

	extern HsGame* g_Game;
}
#endif
