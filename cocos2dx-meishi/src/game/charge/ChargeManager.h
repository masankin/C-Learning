#pragma once
#include "Constant.h"
#include "charge/ChargeModes.h"
class ChargeMode;

class ChargeManager
{
public:
	ChargeManager();
	virtual ~ChargeManager();

	ChargeMode* create(int id);

	void addChargeList(ChargeMode* pCharge);
	void saveChargeInfo();
	void loadChargeInfo();

protected:
	// 
	typedef std::map<eChargeMode, ChargeFactory*> ChargeFactoryList;
	ChargeFactoryList mChargeFactoryList;

	typedef std::map<int, ChargeMode*> ChargeList;
	ChargeList mChargeList;
};

#define sgChargeMgr() Singleton<ChargeManager>::Get()
