#include "GlyphData.h"

/**
 * GlyphBaseData implementation
 */


GlyphBaseData* GlyphBaseData::create()
{
	auto pRet = new GlyphBaseData();
	if (pRet)
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}


GlyphBaseData::GlyphBaseData() : mIsSpiritAffected(false)
, mMainAttr(0.0f)
, mViceAttr(0.0f)
, mLevel(0)
{

}


GlyphBaseData::~GlyphBaseData()
{
}
