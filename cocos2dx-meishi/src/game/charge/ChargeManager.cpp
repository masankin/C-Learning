#include "charge/ChargeManager.h"
#include "EnvSet.h"
#include "core/TimeCalculator.h"
#include "net/UserObj.h"

#include "data/TableManager.h"
#include "data/Tables.h"
#include "stl_helper.h"

ChargeMode* ChargeManager::create(int id)
{
	const ShopInfo* pInfo = sgTableManager()->getShopInfoById(id);
	if (pInfo)
	{
		ChargeList::iterator cIt = mChargeList.find(id);
		if (mChargeList.end() != cIt)
		{
			return cIt->second;
		}

		ChargeFactoryList::iterator it = mChargeFactoryList.find((eChargeMode)pInfo->energy);
		if (it != mChargeFactoryList.end())
		{
			ChargeMode* pCharge = it->second->create(pInfo);
			addChargeList(pCharge);
			return pCharge;
		}
	}

	return nullptr;
}

ChargeManager::ChargeManager()
{
	mChargeFactoryList.insert(std::make_pair(eChargeMode::CHARGE_TYPE_DAY, new ChargeDayFactory()));
	mChargeFactoryList.insert(std::make_pair(eChargeMode::CHARGE_TYPE_P_POWER, new ChargePvpPowerFactory()));
	mChargeFactoryList.insert(std::make_pair(eChargeMode::CHARGE_TYPE_POWER, new ChargePowerFactory()));
	mChargeFactoryList.insert(std::make_pair(eChargeMode::CHARGE_TYPE_FLAG, new ChargeFlagFactory()));
	mChargeFactoryList.insert(std::make_pair(eChargeMode::CHARGE_TYPE_COIN, new CharggCoinFactory()));
	
}

ChargeManager::~ChargeManager()
{
	SAFE_RELEASE_MAP(mChargeFactoryList);
}

void ChargeManager::addChargeList(ChargeMode* pCharge)
{
	if (pCharge)
	{
		mChargeList.insert(std::make_pair(pCharge->getId(), pCharge));
	}
}

void ChargeManager::saveChargeInfo()
{

}

void ChargeManager::loadChargeInfo()
{

}
