#include "WinnerProxy.h"

#include "net/UserObj.h"
#include "net/NetManager.h"
#include "EnvSet.h"

#include "ui/dialogs/GenerlDialog.h"
#include "channel/ChannelApi.h"
#include "game/ui/PageFunction.h"
#include "WinnerControl.h"
#include "data/TableManager.h"
#include "MessageManager.h"
#include "ui/dialogs/GiftAwardDialog.h"

WinnerProxy::WinnerProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_STREAK_WIN_VALUE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_STREAK_WIN_GET, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_ACTIVITY_LIST, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_ACTIVITY_UPDATE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_ACTIVITY_SUBMIT, this);
}

WinnerProxy::~WinnerProxy()
{

}

bool WinnerProxy::fault(const fgame::CSErrorMsg &err)
{
	if (err.errcode() == 10001)
	{
		CCLOG("[trace]WinnerProxy::fault !");
	}
	return true;
}

void WinnerProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]WinnerProxy::DoRsp error!");		
	}
	else if(stRsp.getS2CMsg()->has_streakwinvalue_resp())
	{
		const ::fgame::CSStreakWinValueResp& winnerRsp = stRsp.getS2CMsg()->streakwinvalue_resp();

		sgWinnerControl()->setWinnerCount(winnerRsp.streakwinvalue());
		sgWinnerControl()->setIsGet((bool)winnerRsp.ret());
		if (winnerRsp.has_getwinvalue() && winnerRsp.getwinvalue() != 0)
		{
			sgWinnerControl()->setGetWinnerCount(winnerRsp.getwinvalue());
			sgWinnerControl()->setIsGet(true);
		}
		else
		{
			sgWinnerControl()->setGetWinnerCount(0);
		}
		if (sgWinnerControl()->getWinnerDialogPtr())
		{
			sgWinnerControl()->getWinnerDialogPtr()->updateUI();
		}
		//sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d 0",MSG_FUC_WINNER));
	}
	else if(stRsp.getS2CMsg()->has_streakwinget_resp())
	{
		const ::fgame::CSStreakWinGetResp& winnerRsp = stRsp.getS2CMsg()->streakwinget_resp();
		if (winnerRsp.ret())
		{
			std::string param = StringUtils::format("%d %d %d %d %d %s"
				, 1
				, 1
				, eVituralCurrencyType::Diamond
				, winnerRsp.diamondvalue()
				, sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
				, "WinnerProxy::DoRsp");
			sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
			//GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_WINNER_SUCCESS));

			sgWinnerControl()->setIsGet(true);
			sgWinnerControl()->setGetWinnerCount(sgWinnerControl()->getWinnerCount());
		
			if (sgWinnerControl()->getWinnerDialogPtr())
			{
				sgWinnerControl()->getWinnerDialogPtr()->updateUI(true);
			}	
			sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d 0",MSG_FUC_WINNER));
		}
		//else
		//{
			//GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_WINNER_FAIL));
		//}
	}
	else if(stRsp.getS2CMsg()->has_activitylist_resp())
	{
		const ::fgame::CSActivityListResp& winnerRsp = stRsp.getS2CMsg()->activitylist_resp();
		sgWinnerControl()->mWinnerActList.clear();
		
		for (int i = 0; i < winnerRsp.activitylist_size(); i++)
		{
			WinnerActivityData ActData;
			const ::fgame::CSActivityData& pdata = winnerRsp.activitylist(i);
			ActData.Act_type = pdata.act_type();
			ActData.Cur_SubId = pdata.cur_subid();
			ActData.DaysRemain = pdata.daysremain();
			
			for (int j = 0; j < pdata.sublist_size(); j++)
			{
				const ::fgame::CSSubActivity& subAct = pdata.sublist(j);
				WinnerSubActivity subData;
				subData.Act_Id = subAct.act_id();
				subData.Completed = subAct.completed();
				subData.GiftId = subAct.giftid();
				subData.Request = subAct.request();
				subData.Sub_Id = subAct.sub_id();
				subData.Submid = subAct.submit();
				subData.Value = subAct.value();

				ActData.SubList.insert(std::make_pair(subData.Sub_Id,subData));
			}
			sgWinnerControl()->mWinnerActList.insert(std::make_pair(ActData.Act_type,ActData));
			if (ActData.Act_type == WINNER_ROAD +1)
			{
				sgWinnerControl()->sendWinnerValueRqst();
			}
		}
		if (sgWinnerControl()->getWinnerDialogPtr())
		{
			sgWinnerControl()->getCanGet();
			sgWinnerControl()->getWinnerDialogPtr()->setWinnerBtn();
		}
		else
		{
			sgWinnerControl()->setBtnRefresh(true);
		}
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 1", MSG_FUC_WINNER));
	}
	else if(stRsp.getS2CMsg()->has_activityupdate_resp())
	{
		const ::fgame::CSActivityUpdateResp& winnerRsp = stRsp.getS2CMsg()->activityupdate_resp();
		auto pAct = sgWinnerControl()->getWinnerSubID(winnerRsp.activity().act_id(),winnerRsp.activity().sub_id());
		if (pAct == NULL) return;

		pAct->Completed = winnerRsp.activity().completed();
		pAct->Submid = winnerRsp.activity().submit();
		pAct->Value = winnerRsp.activity().value();

		if (sgWinnerControl()->getWinnerDialogPtr())
		{
			sgWinnerControl()->getCanGet();
			sgWinnerControl()->getWinnerDialogPtr()->updateUI(false);
		}

		if (pAct->Completed && !pAct->Submid)
		{
			sgWinnerControl()->sendTaskComplete(winnerRsp.activity().act_id() - 1);
			sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d 0",MSG_FUC_WINNER));
		}	
	}
	else if(stRsp.getS2CMsg()->has_activitysubmit_resp())
	{
		const ::fgame::CSActivitySubmitResp& winnerRsp = stRsp.getS2CMsg()->activitysubmit_resp();
		auto pAct = sgWinnerControl()->getWinnerSubID(winnerRsp.act_id(),winnerRsp.sub_id());
		if (pAct == NULL) return;
		pAct->Submid = true;
		GiftAwardDialog::showDialogByGiftId(pAct->GiftId);

		if (sgWinnerControl()->getWinnerDialogPtr())
		{
			sgWinnerControl()->getCanGet();
			sgWinnerControl()->getWinnerDialogPtr()->updateUI(true);
		}
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d 0",MSG_FUC_WINNER));
	}
}

void WinnerProxy::sendGetRqst()
{
	unsigned int uCmd = fgame::CS_CMD_STREAK_WIN_GET;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	::fgame::CSStreakWinGetReq* pWinRqst = msgPack.getC2SMsg()->mutable_streakwinget_req();	 

	pWinRqst->set_streakwinvalue(sgWinnerControl()->getWinnerCount());

	sgNetManager()->SendPkg(msgPack);
}

void WinnerProxy::sendWinnerValueRqst()
{
	unsigned int uCmd = fgame::CS_CMD_STREAK_WIN_VALUE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	::fgame::CSStreakWinValueReq* pWinRqst = msgPack.getC2SMsg()->mutable_streakwinvalue_req();	 

	pWinRqst->set_uin(sgUserObj()->getUin());

	sgNetManager()->SendPkg(msgPack);
}

void WinnerProxy::sendActGetRqst(int actId,int subId)
{
	unsigned int uCmd = fgame::CS_CMD_ACTIVITY_SUBMIT;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	::fgame::CSActivitySubmitReq* pWinRqst = msgPack.getC2SMsg()->mutable_activitysubmit_req();	 

	pWinRqst->set_act_id(actId);
	pWinRqst->set_sub_id(subId);

	sgNetManager()->SendPkg(msgPack);
}

void WinnerProxy::sendWinnerListRqst()
{
	unsigned int uCmd = fgame::CS_CMD_ACTIVITY_LIST;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	::fgame::CSActivityListReq* pWinRqst = msgPack.getC2SMsg()->mutable_activitylist_req();	 

	sgNetManager()->SendPkg(msgPack);
}
