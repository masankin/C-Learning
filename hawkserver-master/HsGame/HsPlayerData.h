#ifndef __HS_PLAYER_DATA_H__
#define __HS_PLAYER_DATA_H__

#include "HawkAppFrame.h"
#include "ErrorCode.h"
#include "GsProtocol.h"

namespace Hawk
{
	/************************************************************************/
	/* �������ͳһ����                                                     */
	/************************************************************************/
	class HsPlayerData : public HawkRefCounter
	{
	public:
		//����
		HsPlayerData();

		//����
		virtual ~HsPlayerData();

	public:
		//����
		virtual Bool    Tick(); 

		//�����иĶ���
		virtual Bool    IsDirty() const;

	protected:
		//�����Ƿ����޸�
		Bool   m_bDirty;
	};
}
#endif
