#include "PowerGiftControl.h"
#include "net/UserObj.h"
#include "net/NetManager.h"
#include "MessageManager.h"
#include "data/TableManager.h"


PowerGiftProxy::PowerGiftProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_GET_POWER_ENERGY_ACTIVITY_DATA, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_RECEIVE_POWER_ENERGY_REWARD, this);
}

PowerGiftProxy::~PowerGiftProxy()
{

}

bool PowerGiftProxy::fault(const fgame::CSErrorMsg &err)
{
	if (err.errcode() == 10001)
	{
		CCLOG("[trace]PowerGiftProxy::fault !");
	}
	return true;
}

void PowerGiftProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]PowerGiftProxy::DoRsp error!");		
	}
	else if(stRsp.getS2CMsg()->has_receivepowerenergy_resp())
	{
		//쥴혤
		const ::fgame::CSReceivePowerEnergyResp& pRsp = stRsp.getS2CMsg()->receivepowerenergy_resp();
		if (pRsp.ret() == 0)
		{
			sgPowerGiftControl()->setIsGet(0);
			if (sgPowerGiftControl()->getPowerDialogPtr())
			{
				sgPowerGiftControl()->getPowerDialogPtr()->setIsGet(1);
				sgPowerGiftControl()->getPowerDialogPtr()->onShow();
#ifdef MGAME_PLATFORM_MPLUS
				sgPowerGiftControl()->getPowerDialogPtr()->showShareTips();
#endif		
				sgPowerGiftControl()->getPowerDialogPtr()->showTextTip(sgTextHelper(TEXT_POWER_GET));
			}
			sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d 0",MSG_FUC_POWER_GIFT));
		}

	}
	else if(stRsp.getS2CMsg()->has_getpowerenergyactivity_resp())
	{
		//삿혤삶땡珂쇌
		const ::fgame::CSGetPowerEnergyActivityResp& pRsp = stRsp.getS2CMsg()->getpowerenergyactivity_resp();
		sgPowerGiftControl()->setIsOpen(pRsp.isopen());
		sgPowerGiftControl()->setActID(pRsp.id());
		sgPowerGiftControl()->setIsGet(pRsp.status());
		sgPowerGiftControl()->setActStartTime(pRsp.starttime());
		sgPowerGiftControl()->setActEndTime(pRsp.endtime());
		if (sgPowerGiftControl()->getPowerDialogPtr())
		{
			sgPowerGiftControl()->getPowerDialogPtr()->onShow();
		}	
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d %d",MSG_FUC_POWER_GIFT,pRsp.status()));
		//sgMessageManager()->sendMessage(EMT_BTN_CONTROL,StringUtils::format("%d %d",MSG_FUC_POWER_GIFT,pRsp.isopen() != 0 ? 1: 0));
	}
}

void PowerGiftProxy::sendGetPowerActivityDataRqst()
{
	unsigned int uCmd = fgame::CS_CMD_GET_POWER_ENERGY_ACTIVITY_DATA;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	::fgame::CSGetPowerEnergyActivityDataReq* pWinRqst = msgPack.getC2SMsg()->mutable_getpowerenergyactivitydata_req();	 

	sgNetManager()->SendPkg(msgPack);
}

void PowerGiftProxy::sendReceivePowerEnergyRqst(int type)
{

	unsigned int uCmd = fgame::CS_CMD_RECEIVE_POWER_ENERGY_REWARD;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	::fgame::CSReceivePowerEnergyReq* pWinRqst = msgPack.getC2SMsg()->mutable_receivepowerenergy_req();	 

	pWinRqst->set_id(sgPowerGiftControl()->getActID());
	pWinRqst->set_type(type);

	sgNetManager()->SendPkg(msgPack);
}


PowerGiftControl::PowerGiftControl(void)
{
	mIsOpen = 0;
	mPowerDialog = NULL;
	m_IsGet = 0;
	m_StartTime = 0;
	m_EndTime = 0;
	m_ActID = 1;
	Node::onEnter();
	schedule(schedule_selector(PowerGiftControl::updateTime), 1.0);
}


PowerGiftControl::~PowerGiftControl(void)
{
	unschedule(schedule_selector(PowerGiftControl::updateTime));
	Node::onExit();
}


void PowerGiftControl::updateTime(float dt)
{
	if (sgPowerGiftControl()->getActEndTime() > 0)
	{
		int _t = sgPowerGiftControl()->getActEndTime() - (int)dt;
		sgPowerGiftControl()->setActEndTime(_t);

		if(_t <= 0)
		{
			_t = 0;
			sendGetPowerActivityDataRqst();
		}
	}

	//if(m_IsGet == 0) return;

	if (sgPowerGiftControl()->getActStartTime() > 0)
	{
		int _t = sgPowerGiftControl()->getActStartTime() - (int)dt;
		sgPowerGiftControl()->setActStartTime(_t);

		if(_t <= 0)
		{
			_t = 0;
			sendGetPowerActivityDataRqst();
		}
		if (mPowerDialog)
		{
			const char* cstr = __String::createWithFormat("%02d:%02d",_t/60,_t%60)->getCString();
			mPowerDialog->mTimeText1->setText(cstr);
			//mPowerDialog->mTimeText2->setText(cstr);
		}	
		return;
	}
}

void PowerGiftControl::sendGetPowerActivityDataRqst()
{
	mPowerGiftProxy.sendGetPowerActivityDataRqst();
}

void PowerGiftControl::sendReceivePowerEnergyRqst()
{
	mPowerGiftProxy.sendReceivePowerEnergyRqst(m_ActID);
}
