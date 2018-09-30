#include "ActProxy.h"
#include "net/NetManager.h"
#include "ActControl.h"
#include "data/TableManager.h"
#include "net/UserObj.h"
#include "MessageManager.h"
#include "ui/dialogs/GiftAwardDialog.h"
#include "game/act/ActCrazyWeekControl.h"
#include "data/MailData.h"
#include "ui/dialogs/GenerlDialog.h"

#define NEWER_GIFT_TYPE 1

ActProxy::ActProxy(void)
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_SEVEN_DAY_SIGN_LIST,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_SEVEN_DAY_SIGN,this);

	sgNetManager()->RegCmdListener(fgame::CS_CMD_GET_OPEN_ACTIVITY,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_GET_ACTIVITY_DATA,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_RECEIVE_ACTIVITY_REWARD,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_BUY_NEWBIE_GIFT, this);

}


ActProxy::~ActProxy(void)
{
}

void ActProxy::DoRsp(NetPack& stRsp)
{
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]ActProxy::DoRsp error!");

	}
	else if (stRsp.getS2CMsg()->has_sevendaysignlist_resp())//查询
	{
		CCLOG("[trace]ActProxy has_sevendaysignlist_resp");
		mSevenDaySiginList = stRsp.getS2CMsg()->sevendaysignlist_resp();
		sgActControl()->doRueryActLogin(stRsp.getS2CMsg()->sevendaysignlist_resp());

	}else if (stRsp.getS2CMsg()->has_sevendaysign_resp())//领取
	{
		CCLOG("[trace]ActProxy has_sevendaysign_resp");	
		sgActControl()->doGetActLogin(stRsp.getS2CMsg()->sevendaysign_resp());
	}
	else if(stRsp.getS2CMsg()->has_getopenactivity_resp())
	{
		const ::fgame::CSGetOpenActivityResp& pRsp = stRsp.getS2CMsg()->getopenactivity_resp();

		sgActControl()->clearActTypeEff();
		sgActControl()->setIsNewerGiftOpened(false);
		sgActCrazyWeekControl()->showCrazyWeekIcon(false);
		sgActControl()->setIsNewYearIconOpen(false);
		sgActControl()->setIsShowFirstCharge(false);
		sgActControl()->setIsChristmasActOpen(false);
		sgActControl()->setIsValentineIconOpen(false);
		sgActControl()->setIsTopRankActOpen(false);

		for (int i = 0; i < pRsp.openactivityinfo_size(); i++)
		{
			if (pRsp.openactivityinfo(i).type() == ACT_NEWER_GIFT)
			{ 
				sgActControl()->setIsNewerGiftOpened(true);
			}
			else if (pRsp.openactivityinfo(i).type() == ACT_CRAZY_WEEK){
				//CCLOG("[jim]  crazyWeek active .............. ");
				sgActCrazyWeekControl()->sendGetCrazyWeekInfoReq();
			}
			else if (pRsp.openactivityinfo(i).type() == ACT_CHRISTMAS){
				CCLOG("[trace][jim]................. Merry Christmas.....");
				sgActControl()->setIsChristmasActOpen(true);
			}
			else if (pRsp.openactivityinfo(i).type() == ACT_NEW_YEAR)
			{
				//crystal 2015/12/17
				//sgActControl()->setIsNewYearIconOpen(true);
			}
			else if (pRsp.openactivityinfo(i).type() == ACT_FIRST_CHARGE) {
				CCLOG("[trace] act first charge ............... ");
				sgActControl()->setIsShowFirstCharge(true);
			}
			else if (pRsp.openactivityinfo(i).type() == ACT_VALENTINE)
			{
				//crystal 2015/1/12
				//sgActControl()->setIsValentineIconOpen(true);
			}
			else if (pRsp.openactivityinfo(i).type() == ACT_TOP_RANK_STAR)
			{
				//显示巅峰排行按钮
				sgActControl()->setIsTopRankActOpen(true);
			}
			else if (pRsp.openactivityinfo(i).type() == ACT_TOP_RANK_WIN)
			{
				//显示巅峰排行按钮
				sgActControl()->setIsTopRankActOpen(true);
			}
			else if (pRsp.openactivityinfo(i).type() == ACT_TOP_RANK_WINSTREAK)
			{
				//显示巅峰排行按钮
				sgActControl()->setIsTopRankActOpen(true);
			}
			else if (pRsp.openactivityinfo(i).type() != ACT_LOGIN)
			{
				sgActControl()->getActTypeEff()->insert(std::make_pair(pRsp.openactivityinfo(i).type(), false));
			}

			CCLOG("[trace] open active type == %d ", pRsp.openactivityinfo(i).type());
		}

		ActPanel* pActPanel = sgActControl()->getActPanel();
		if(pActPanel != NULL)
		{
			if(pActPanel->isShow())
			{
				pActPanel->setActPanelBtn();
			}
			else
			{
				sgActControl()->setBtnRefresh(true);
			}
		}
		// 主界面首冲图标
		sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", MSG_FUC_FIRST_CHARGE, sgActControl()->getIsShowFirstCharge()));
		sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", MSG_FUC_CHRISTMAS, sgActControl()->getIsChristmasActOpen()));

		//主界面巅峰排行图标
		sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", MSG_FUC_TOP_RANK, sgActControl()->getIsTopRankActOpen()));
	}
	else if(stRsp.getS2CMsg()->has_getactivitydata_resp())
	{
		const ::fgame::CSGetActivityDataResp& pRsp = stRsp.getS2CMsg()->getactivitydata_resp();
		sgActControl()->setDailyCount(pRsp.daycount());
		sgActControl()->setRechargeCount(pRsp.rechargecount());
		sgTableManager()->getRechargeList()->clear();
		for (int i = 0; i < pRsp.activityinfo_size(); i++)
		{
			const fgame::CSActivityInfo& pInfo = pRsp.activityinfo(i);
			auto pItor = sgTableManager()->getRechargeList()->find(pInfo.id());
			if (pItor != sgTableManager()->getRechargeList()->end())
			{
				pItor->second->setID(pInfo.id());
				pItor->second->setStageId(pInfo.stage());
				pItor->second->setRechargeType(pInfo.type());
				pItor->second->setIsGet(pInfo.status());
				pItor->second->setReward(pInfo.gift());
				pItor->second->setStartTime(pInfo.starttime());
				pItor->second->setEndTime(pInfo.endtime());
				pItor->second->setRechargeNum(pInfo.recharge_num());
			}
			else
			{
				RechargeData* pData = RechargeData::create();
				if (pData)
				{
					pData->setID(pInfo.id());
					pData->setStageId(pInfo.stage());
					pData->setRechargeType(pInfo.type());
					pData->setIsGet(pInfo.status());
					pData->setReward(pInfo.gift());
					pData->setStartTime(pInfo.starttime());
					pData->setEndTime(pInfo.endtime());
					pData->setRechargeNum(pInfo.recharge_num());
				}
				sgTableManager()->getRechargeList()->insert(std::make_pair(pData->getID(),pData));	
			}				
		}
		sgActControl()->doRechargeCheck();
	}
	else if(stRsp.getS2CMsg()->has_receiveactivityreward_resp())
	{
		const ::fgame::CSReceiveActivityRewardResp& pRsp = stRsp.getS2CMsg()->receiveactivityreward_resp();
		if (pRsp.ret() == 0)
		{
			RechargeData* pData = sgTableManager()->getRechargeData(pRsp.id());
			if (pData)
			{
				pData->setIsGet(0);
				GiftAwardDialog::showDialogByGiftId(pData->getReward());
			}
			sgActControl()->getCurrRechargeData(pRsp.type());
			if (pRsp.type() == ACT_FIRST_CHARGE) {
				// 首冲领取成功,首冲入口消失, 刷新首冲界面
				sgActControl()->setIsShowFirstCharge(false);
				sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", MSG_FUC_FIRST_CHARGE, 0));
				sgActControl()->updateFirstChargeView();
			}
			else {
				if (sgActControl()->getActPanel())
				{
					sgActControl()->getActPanel()->updateUI(pRsp.type());
				}
				
			}
		}
	}
	else if (stRsp.getS2CMsg()->has_buynewbiegift_resp())
	{
		const ::fgame::CSBuyNewbieGiftResp& pRsp = stRsp.getS2CMsg()->buynewbiegift_resp();
		if (pRsp.ret() == 0)
		{
			GenerlDialog::hide();
			NewPlayGiftData* pData = sgTableManager()->getNewPlayGiftDataByType(NEWER_GIFT_TYPE);

			GiftAwardDialog* pDialog = GiftAwardDialog::showDialogByGiftId(pData->giftId); 
			pDialog->setbGuideCallBack(true);
			std::string param = StringUtils::format("%d %d %d %d %d %s"
				, 1
				, 1
				, eVituralCurrencyType::Diamond
				, -pData->price
				, sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
				, "ActProxy::DoRsp");

			sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);

			sgActControl()->setIsNewerGiftOpened(false);
			//sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", MSG_FUC_NEWER_GIFT, sgActControl()->getIsNewerGiftOpened()));
			sgMessageManager()->sendMessage(EMT_UPDATE_SHOPLIST, "");
		}
	}
}


void ActProxy::sendQueryActLogin()
{
	unsigned int uCmd = fgame::CS_CMD_SEVEN_DAY_SIGN_LIST;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSSevenDaySignListReq* pRqst = msgPack.getC2SMsg()->mutable_sevendaysignlist_req();

	sgNetManager()->SendPkg(msgPack);
}

void ActProxy::sendGetActLogin()
{
	unsigned int uCmd = fgame::CS_CMD_SEVEN_DAY_SIGN;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSSevenDaySignReq* pRqst = msgPack.getC2SMsg()->mutable_sevendaysign_req();

	sgNetManager()->SendPkg(msgPack);
}

void ActProxy::sendGetOpenActivity()
{
	unsigned int uCmd = fgame::CS_CMD_GET_OPEN_ACTIVITY;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSGetOpenActivityReq* pRqst = msgPack.getC2SMsg()->mutable_getopenactivity_req();

	sgNetManager()->SendPkg(msgPack);

	sgActControl()->doRechargeCheck();
}

void ActProxy::sendGetActivityData()
{
	unsigned int uCmd = fgame::CS_CMD_GET_ACTIVITY_DATA;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSGetActivityDataReq* pRqst = msgPack.getC2SMsg()->mutable_getactivitydata_req();

	sgNetManager()->SendPkg(msgPack);
}

void ActProxy::sendReceiveActivityReward(int _type)
{
	unsigned int uCmd = fgame::CS_CMD_RECEIVE_ACTIVITY_REWARD;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSReceiveActivityRewardReq* pRqst = msgPack.getC2SMsg()->mutable_receiveactivityreward_req();
	pRqst->set_type(_type);
	int id = sgActControl()->getRechargeId();

	if (_type == ACT_FIRST_CHARGE)
	{
		id = sgActControl()->getFirstId();
	}
	else if (_type == ACT_DAILY_CHARGE)
	{
		id = sgActControl()->getDailyId();
	}
	pRqst->set_id(id);

	RechargeData* pData = sgTableManager()->getRechargeData(id);
	if (pData)
	{
		pRqst->set_stage(pData->getStageId());
	}
	else
	{
		pRqst->set_stage(1);
	}

	sgNetManager()->SendPkg(msgPack);
}

void ActProxy::sendBuyActNewerGift(int _id)
{
	unsigned int uCmd = fgame::CS_CMD_BUY_NEWBIE_GIFT;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSBuyNewbieGiftReq* pRqst = msgPack.getC2SMsg()->mutable_buynewbiegift_req();
	pRqst->set_id(_id);

	sgNetManager()->SendPkg(msgPack);
}

fgame::CSSevenDaySignListResp& ActProxy::getSevenDaySiginList()
{
	return mSevenDaySiginList;
}
