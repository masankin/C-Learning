#pragma once
#include "Constant.h"
#include "core/CVTime.h"
class ShopInfo;
class ChargeMode
{
public:
	ChargeMode();
	virtual ~ChargeMode();

	virtual bool load(const ShopInfo* pInfo);
	virtual void onBuy();
	virtual void update();

	int getId(){ return mId; };
protected:

	eChargeMode m_Mode;

	int mId;
	int mMum; //赠送次数
	int mGiftNum;//赠送数量
	int mGold;//人民币
	int mManual;//钻石
	int mBuyTime;//购买次数
	

};


class ChargeFactory
{
public:
	virtual ChargeMode* create(const ShopInfo* pInfo) = 0;
};


#define DEFINE_CHARGE_FACTORY(_NAME_)	\
class _NAME_##Factory : public ChargeFactory	\
{\
public:	\
	virtual ChargeMode* create(const ShopInfo* pInfo){ \
\
_NAME_* pret = new _NAME_(); \
if (pret && pret->load(pInfo)){ \
	\
		return pret; \
}\
return NULL; \
	}\
};

class ChargeDay :public ChargeMode
{
public:
	ChargeDay(){};
	~ChargeDay(){};

	virtual bool load(const ShopInfo* pInfo);
	virtual void onBuy();
	virtual void update();

private:

	CVTime stBeing;
	CVTime stNow;

	double mBeingTime;
	double mGetTime;
};
DEFINE_CHARGE_FACTORY(ChargeDay)


class ChargePower :public ChargeMode
{
public:
	ChargePower();
	~ChargePower();

	virtual bool load(const ShopInfo* pInfo);
	virtual void onBuy();
	virtual void update(){};

private:


};

DEFINE_CHARGE_FACTORY(ChargePower)

//赠送标记位
class ChargeFlag :public ChargeMode
{
public:
	ChargeFlag(){};
	~ChargeFlag(){};

	virtual bool load(const ShopInfo* pInfo);
	virtual void onBuy();
	virtual void update(){};

private:


};

DEFINE_CHARGE_FACTORY(ChargeFlag)

//首次购买送金币
class ChargePvpPower :public ChargeMode
{
public:
	ChargePvpPower(){};
	~ChargePvpPower(){};

	virtual bool load(const ShopInfo* pInfo);
	virtual void onBuy();
	virtual void update(){};

private:
};

DEFINE_CHARGE_FACTORY(ChargePvpPower)

class CharggCoin :public ChargeMode
{
public:
	CharggCoin(){};
	~CharggCoin(){};

	virtual bool load(const ShopInfo* pInfo);
	virtual void onBuy();
	virtual void update(){};

private:
};

DEFINE_CHARGE_FACTORY(CharggCoin)
