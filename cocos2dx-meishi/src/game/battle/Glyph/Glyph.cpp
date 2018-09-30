#include "Glyph.h"
#include "GlyphData.h"
#include "battle/BattleControl.h"

/**
 * Glyph implementation
 */

Glyph* Glyph::create(eGlyphType type, GlyphBaseData* data)
{
	Glyph* pRet = new Glyph();
	if (pRet && pRet->init(type, data))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

Glyph::Glyph() : mType(E_GLYPH_MIN)
, mData(nullptr)
, mValue(0.0f)
{

}

Glyph::~Glyph()
{
	CC_SAFE_RELEASE(mData);
}


bool Glyph::init(eGlyphType type, GlyphBaseData* data)
{
	if ((type <= E_GLYPH_ATK_BEG || type >= E_GLYPH_ATK_END)
		&& (type <= E_GLYPH_DEF_BEG || type >= E_GLYPH_DEF_END)
		&& (type <= E_GLYPH_SPD_BEG || type >= E_GLYPH_SPD_END)
		&& (type <= E_GLYPH_PPL_BEG || type >= E_GLYPH_PPL_END)
		&& (type <= E_GLYPH_MRL_BEG || type >= E_GLYPH_MRL_END))
	{
		CCLOG("[error]Glyph::init : not glyph type is %d", (int)type);
		return false;
	}

	if (nullptr == data)
	{
		CCLOG("[error]Glyph::init : data is null, type is %d", (int)type);
		return false;
	}

	if (0 == data->mMainAttr)
	{
		CCLOG("[warning]Glyph::init : main attr is 0");
		return false;
	}

	mType = type;
	
	data->retain();
	mData = data;

	if (this->isSpiritAffected())
		mValue = 0;
	else
		mValue = mData->mMainAttr;
		

	return true;
}


Glyph::eGlyphType Glyph::getType() 
{
	return mType;
}


GlyphBaseData* Glyph::getData() 
{
	return mData;
}


bool Glyph::isSpiritAffected() const
{
	if (nullptr == mData)
		return false;

	return mData->mIsSpiritAffected;
}


float Glyph::getDataMainAttr() const
{
	if (mData)
		return mData->mMainAttr;

	return 0.0f;
}


float Glyph::getDataViceAttr() const
{
	if (mData)
		return mData->mViceAttr;

	return 0.0f;
}


bool Glyph::isAffectedByBuildLevel() const
{
	if (mData)
	{
		if (mData->mBuildLevelAffectedAttrs.empty())
			return false;
		else
			return true;
	}

	return false;
}


float Glyph::getDataBuildAffectAttr(int buildLevel) const
{
	if (mData && buildLevel < mData->mBuildLevelAffectedAttrs.size())
	{
		return mData->mBuildLevelAffectedAttrs[buildLevel];
	}

	return 0.0f;
}


std::string Glyph::getDataBattleTip() const
{
	if (mData)
	{
		return mData->mBattleTip;
	}

	return "";
}


float Glyph::getValue() const
{
	auto temp = mValue;
	if (mType == eGlyphType::E_GLYPH_MRL_TIME)
		mValue = 0.0f;

	return temp;
}


void Glyph::onSpiritChange(float spirit)
{
	if (!this->isSpiritAffected())
		return;

	if (mData)
		mValue = mData->mMainAttr * spirit / 100.0f;
}


void Glyph::onIntervalTimeout(eCampType campType)
{
	switch (mType)
	{
	case Glyph::E_GLYPH_ATK_POWER:
		break;
	case Glyph::E_GLYPH_ATK_TIME:
	{
		if (mData)
			mValue += mData->mMainAttr;

		auto strTips = getDataBattleTip();
		sgBattleControl()->addBattleTipsForGlyph(campType, strTips);
	}
	break;
	case Glyph::E_GLYPH_ATK_INSTITUTE:
		break;
	case Glyph::E_GLYPH_ATK_KILL:
		break;
	case Glyph::E_GLYPH_DEF_DEFENSE:
		break;
	case Glyph::E_GLYPH_DEF_TIME:
	{
		if (mData)
			mValue += mData->mMainAttr;

		auto strTips = getDataBattleTip();
		sgBattleControl()->addBattleTipsForGlyph(campType, strTips);
	}
	break;
	case Glyph::E_GLYPH_DEF_INSTITUTE:
		break;
	case Glyph::E_GLYPH_DEF_DEAD:
		break;
	case Glyph::E_GLYPH_SPD_MOVE:
		break;
	case Glyph::E_GLYPH_SPD_UP:
		break;
	case Glyph::E_GLYPH_SPD_CREEPS:
		break;
	case Glyph::E_GLYPH_SPD_ASSAULT:
		break;
	case Glyph::E_GLYPH_SPD_ASSIST:
		break;
	case Glyph::E_GLYPH_SPD_PRODUCE:
		break;
	case Glyph::E_GLYPH_PPL_INIT:
		break;
	case Glyph::E_GLYPH_PPL_UP:
		break;
	case Glyph::E_GLYPH_PPL_OCCUPY:
		break;
	case Glyph::E_GLYPH_PPL_PRODUCE:
		break;
	case Glyph::E_GLYPH_MRL_ADD:
		break;
	case Glyph::E_GLYPH_MRL_TIME:
	{
		if (mData)
			mValue = mData->mMainAttr;

		auto strTips = getDataBattleTip();
		sgBattleControl()->addBattleTipsForGlyph(campType, strTips);
	}
	break;
	default:
		break;
	}
}
