#ifndef __HS_PLAYER_DATA_H__
#define __HS_PLAYER_DATA_H__

#include "HawkAppFrame.h"
#include "ErrorCode.h"
#include "GsProtocol.h"

namespace Hawk
{
	/************************************************************************/
	/* 玩家数据统一管理                                                     */
	/************************************************************************/
	class HsPlayerData : public HawkRefCounter
	{
	public:
		//构造
		HsPlayerData();

		//析构
		virtual ~HsPlayerData();

	public:
		//更新
		virtual Bool    Tick(); 

		//数据有改动吗
		virtual Bool    IsDirty() const;

	protected:
		//数据是否有修改
		Bool   m_bDirty;
	};
}
#endif
