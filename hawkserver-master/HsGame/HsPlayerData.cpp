#include "HsPlayerData.h"
#include "HsGame.h"

namespace Hawk
{
	HsPlayerData::HsPlayerData()
	{
		m_bDirty = false;
	}

	HsPlayerData::~HsPlayerData()
	{
	
	}

	Bool HsPlayerData::Tick()
	{
		return true;
	}

	Bool HsPlayerData::IsDirty() const
	{
		return m_bDirty;
	}
}
