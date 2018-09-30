#pragma once
#include "cocos2d.h"
#include "Constant.h"

#include "BaseProxy.h"
#include "ui/dialogs/PowerDialog.h"

class PowerGiftProxy : public BaseProxy
{
public:

	PowerGiftProxy();
	~PowerGiftProxy();

	virtual void DoRsp(NetPack& stRsp);
	virtual bool fault(const fgame::CSErrorMsg &err);

	void sendGetPowerActivityDataRqst();
	void sendReceivePowerEnergyRqst(int type);
};

class PowerGiftControl:public Node
{
public:
	PowerGiftControl(void);
	~PowerGiftControl(void);

	CC_SYNTHESIZE(int, mIsOpen, IsOpen);
	CC_SYNTHESIZE(int, m_ActID, ActID);
	CC_SYNTHESIZE(int, m_IsGet, IsGet);//0为领取，1为未领取 
	CC_SYNTHESIZE(int, m_StartTime, ActStartTime);
	CC_SYNTHESIZE(int, m_EndTime, ActEndTime);

	CC_SYNTHESIZE(PowerDialog*, mPowerDialog, PowerDialogPtr);

	void sendGetPowerActivityDataRqst();
	void sendReceivePowerEnergyRqst();

	void updateTime(float dt);
protected:
	PowerGiftProxy mPowerGiftProxy;
};

#define sgPowerGiftControl() Singleton<PowerGiftControl>::Get()
