#include "charge/ChargeModes.h"
#include "EnvSet.h"
#include "core/TimeCalculator.h"
#include "net/UserObj.h"
#include "data/Tables.h"

#include "core/MessageManager.h"
#include "Constant.h"

ChargeMode::ChargeMode() :m_Mode(CHARGE_TYPE_INVALID), mMum(0), mGiftNum(0), mGold(0), mManual(0), mBuyTime(0), mId(0)
{
}

ChargeMode::~ChargeMode()
{
	
}

bool ChargeMode::load(const ShopInfo* pInfo)
{
	bool ret = false;
	do 
	{
		CC_BREAK_IF(!pInfo);
		CC_BREAK_IF(!(eChargeMode::CHARGE_TYPE_INVALID < pInfo->energy < CHARGE_TYPE_MAX));

		m_Mode = (eChargeMode)pInfo->energy;
		mGold = pInfo->gold;
		mManual = pInfo->manual;
		
		mGiftNum = pInfo->gift;
		mId = pInfo->id;

		ret = true;
	} while (0);

	return ret;
}

void ChargeMode::onBuy()
{

}

void ChargeMode::update()
{

}

bool ChargeDay::load(const ShopInfo* pInfo)
{
	return ChargeMode::load(pInfo);
}

void ChargeDay::onBuy()
{
	if (mBuyTime >= 1)
	{
		mMum += 30;
	}
	mBuyTime++; 
	mMum--;
	mBeingTime = sgTimeCalculator()->getCurrentTime();
	mGetTime = sgTimeCalculator()->getCurrentTime();
	stBeing = sgTimeUtils()->GetDayZero(mBeingTime);
	stNow = sgTimeUtils()->GetDayZero(mBeingTime);

	std::string param = StringUtils::format("%d %d %d %d %d %s"
		, 1
		, 1
		, eVituralCurrencyType::Diamond
		, mManual
		, sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
		, "ChargeDay::onBuy");

	sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
}

void ChargeDay::update()
{
}

ChargePower::ChargePower() 
{

}

ChargePower::~ChargePower()
{

}

bool ChargePower::load(const ShopInfo* pInfo)
{
	return ChargeMode::load(pInfo);

}

void ChargePower::onBuy()
{
	
	if (mBuyTime < mMum)
	{
		//int maxPower = sgPlayerDataMgr()->getPlayerData()->getMaxPower();
		//maxPower += mGiftNum;
		mBuyTime++;
		//sgPlayerDataMgr()->getPlayerData()->setMaxPower(maxPower);
	}
}

bool ChargeFlag::load(const ShopInfo* pInfo)
{
	return ChargeMode::load(pInfo);

}

void ChargeFlag::onBuy()
{
	
	if (mBuyTime < mMum)
	{
		//int flag = sgPlayerDataMgr()->getPlayerData()->getFlagNum();
		//flag += mGiftNum;
		mBuyTime++;
		//sgPlayerDataMgr()->getPlayerData()->setFlagNum(mGiftNum);
	}
}

bool ChargePvpPower::load(const ShopInfo* pInfo)
{
	return ChargeMode::load(pInfo);
}

void ChargePvpPower::onBuy()
{
	
	if (mBuyTime < mMum)
	{
		mBuyTime++;
	}
}

bool CharggCoin::load(const ShopInfo* pInfo)
{
	return ChargeMode::load(pInfo);
}

void CharggCoin::onBuy()
{
	
	if (mBuyTime < mMum)
	{
		std::string param = StringUtils::format("%d %d %d %d %d %s"
			, 1
			, 1
			, eVituralCurrencyType::Diamond
			, mManual + mGiftNum
			, sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
			, "CharggCoin::onBuy");

		sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);

		mBuyTime++;
	}
}
