
#include <map>
#include <string>

#include "Statistics.h"
#include "cocos2d.h"
#include "StatisticsCommon.h"

#include "DataEye/DataEyeAgent.h"
#include "TalkingData/TalkingDataAgent.h"

#include "data/TableManager.h"
#include "channel/ChannelApi.h"
#include "balance/BalanceControl.h"
#include "game/net/UserObj.h"
#include "game/utils/GameDefault.h"


USING_NS_CC;

Statistics::Statistics() : mDataEyeAgent(nullptr)
, mTalkingDataAgent(nullptr)
{

}


Statistics::~Statistics()
{
	if (nullptr != mDataEyeAgent)
	{
		delete mDataEyeAgent;
		mDataEyeAgent = nullptr;
	}

	if (nullptr != mTalkingDataAgent)
	{
		delete mTalkingDataAgent;
		mTalkingDataAgent = nullptr;
	}
}


bool Statistics::init()
{
	bool isDataEyeActive = false;
	mDataEyeAgent = new DataEyeAgent();
	if (nullptr != mDataEyeAgent)
	{
		CCLOG("[trace]Statistics::init : create DataEye agent success.");
		isDataEyeActive = mDataEyeAgent->initilized();
	}
	else
	{
		CCLOG("[error]Statistics::init : create DataEye agent failed, Statistics shutdown.");
	}

	bool isTalkingActive = false;
	mTalkingDataAgent = new TalkingDataAgent();
	if (nullptr != mTalkingDataAgent)
	{
		CCLOG("[trace]Statistics::init : create TalkingData agent success.");
		isTalkingActive = mTalkingDataAgent->initilized();
	}
	else
	{
		CCLOG("[trace]Statistics::init : create TalkingData agent failed, Statistics shutdown.");
	}

	if (!isTalkingActive && !isTalkingActive)
	{
		CCLOG("[trace]Statistics::init : no statics sdk active");
		return false;
	}


	sgMessageManager()->regisiterMessageHandler(EMT_CHECKPOINT_START, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CHECKPOINT_END, this);

	sgMessageManager()->regisiterMessageHandler(EMT_CHANNEL_LOGIN, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CHANNEL_LOGOUT, this);

	sgMessageManager()->regisiterMessageHandler(EMT_OPEN_UI, this);
	sgMessageManager()->regisiterMessageHandler(EMT_SHOP_PAYCOMPLENT, this);

	sgMessageManager()->regisiterMessageHandler(EMT_GUIDE_STEP_OVER, this);

	sgMessageManager()->regisiterMessageHandler(EMT_ITEM_OPERATION, this);

	sgMessageManager()->regisiterMessageHandler(EMT_MONEY_CHANGE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PVP_INFO, this);

	sgMessageManager()->regisiterMessageHandler(EMT_GAME_LOGIN_FLOW, this);

	return true;
}


void Statistics::onReceiveMassage(GameMessage * message)
{
	if (nullptr == message)
		return;

	switch (message->getMessageType())
	{
	case eMessageType::EMT_CHECKPOINT_START:
	{
		this->doCheckPointStart(dynamic_cast<MessageCheckPointStart*>(message));
	}
	break;

	case eMessageType::EMT_CHECKPOINT_END:
	{
		this->doCheckPointOver(dynamic_cast<MessageCheckPointEnd*>(message));
	}
	break;

	case eMessageType::EMT_CHANNEL_LOGIN:
	{
		this->doChannelLogin(dynamic_cast<MessageChannelLogin*>(message));
	}
	break;

	case eMessageType::EMT_CHANNEL_LOGOUT:
	{
		this->doChannelLogout(dynamic_cast<MessageChannelLogout*>(message));
	}
	break;

	case eMessageType::EMT_OPEN_UI:
	{
		this->doOpenUI(dynamic_cast<MessageOpenUI*>(message));
	}
	break;

	case eMessageType::EMT_GUIDE_STEP_OVER:
	{
		this->doGuideStepOver(dynamic_cast<MessageGuideStepOver*>(message));
	}
	break;

	case eMessageType::EMT_SHOP_PAYCOMPLENT:
	{
		this->doRMBBuy(dynamic_cast<MessageShopPayComplent*>(message));
	}
	break;

	case eMessageType::EMT_ITEM_OPERATION:
	{
		this->doItemOperation(dynamic_cast<MessageItemOperation*>(message));
	}
	break;

	case eMessageType::EMT_MONEY_CHANGE:
	{
		this->doMoneyChange(dynamic_cast<MessageMoneyChange*>(message));
	}
	break;


	case eMessageType::EMT_PVP_INFO:
	{
		this->doPVPInfo(dynamic_cast<MessagePVPInfo*>(message));
	}
	break;

	case eMessageType::EMT_GAME_LOGIN_FLOW:
	{
		this->doGameLoginFlow(dynamic_cast<MessageGameLoginFlow*>(message));
	}
	break;


	default:
		break;
	}
}


void Statistics::doCheckPointStart(MessageCheckPointStart * pMsg)
{
	if (nullptr == pMsg)
		return;

	CCLOG("[trace]Statistics::doCheckPointStart : Level=%d", pMsg->mCpIndex);

	mDataEyeAgent->levelsBegin(StringUtils::format(STATISTICS_LEVEL_ID, pMsg->mCpIndex));
	mTalkingDataAgent->missionBegin(StringUtils::format(STATISTICS_LEVEL_ID, pMsg->mCpIndex));

	//mDataEyeAgent->onEventBegin(StringUtils::format(STATISTICS_EVENT_LEVEL_ID, pMsg->mCpIndex));

	//int lpData = sgUserObj()->getOneLostPointData(eLostPointStep::eEnterCheckPoint2);
	int iDataIndex = pMsg->mCpIndex * 2;
	int lpData = sgUserObj()->getOneLostPointData(iDataIndex);
	if (lpData != 1)
	{
		DataEyeAgent::EventParamMap eventAttrLostPoint;
		eventAttrLostPoint[STATISTICS_EVENT_LOST_POINT_KEY_STEP] = StringUtils::format("Start Check Point %d", pMsg->mCpIndex);
		mDataEyeAgent->onEvent(STATISTICS_EVENT_LOST_POINT_ID, &eventAttrLostPoint);
		mTalkingDataAgent->onEvent(STATISTICS_EVENT_LOST_POINT_ID, &eventAttrLostPoint);

		sgUserObj()->updateLostPointData(iDataIndex, true);
	}
}


void Statistics::doCheckPointOver(MessageCheckPointEnd * pMsg)
{
	if (nullptr == pMsg)
		return;

	CCLOG("[trace]Statistics::doCheckPointOver : Level=%d, Result=%d, Reason=%s", pMsg->mCpIndex, pMsg->mSuccess, pMsg->mReason.c_str());

	auto cpData = sgTableManager()->getCheckPointByID(pMsg->mCpIndex);
	if (nullptr == cpData)
	{
		CCLOG("Statistics::onReceiveMassage, Check point data is null.");
		return;
	}

	std::vector<std::string> starConditions;
	starConditions.reserve(3);
	if (!cpData->star1.empty())
		starConditions.push_back(cpData->star1);
	if (!cpData->star2.empty())
		starConditions.push_back(cpData->star2);
	if (!cpData->star3.empty())
		starConditions.push_back(cpData->star3);

	std::string result;

	std::string levelID = StringUtils::format(STATISTICS_LEVEL_ID, pMsg->mCpIndex);
	if (pMsg->mSuccess > 0)
	{
		mDataEyeAgent->levelsComplete(levelID.c_str());
		mTalkingDataAgent->missionBegin(StringUtils::format(STATISTICS_LEVEL_ID, pMsg->mCpIndex));
		result = "Success";
	}
	else
	{
		mDataEyeAgent->levelsFail(levelID.c_str(), pMsg->mReason.c_str());
		mTalkingDataAgent->missionFailed(StringUtils::format(STATISTICS_LEVEL_ID, pMsg->mCpIndex), pMsg->mReason.c_str());
		result = "Failure";
	}

	std::string uid = mDataEyeAgent->getUID();

	DataEyeAgent::EventParamMap eventAttrLevel;
	eventAttrLevel[STATISTICS_EVENT_LEVEL_KEY_UID] = uid;
	eventAttrLevel[STATISTICS_EVENT_LEVEL_KEY_RESULT] = result;


	if (starConditions.empty())
	{
		eventAttrLevel[STATISTICS_EVENT_LEVEL_KEY_PERFECT] = "Empty";

		eventAttrLevel[STATISTICS_EVENT_LEVEL_KEY_STARCOUNT] = "Empty";
	}
	else
	{
		int starCnt = sgBalanceControl()->getStarNum();
		if (static_cast<std::size_t>(starCnt) >= starConditions.size())
			eventAttrLevel[STATISTICS_EVENT_LEVEL_KEY_PERFECT] = "Yes";
		else
			eventAttrLevel[STATISTICS_EVENT_LEVEL_KEY_PERFECT] = "No";

		if (pMsg->mSuccess > 0)
			eventAttrLevel[STATISTICS_EVENT_LEVEL_KEY_STARCOUNT] = StringUtils::format("%d", starCnt);
	}


	mDataEyeAgent->onEvent(StringUtils::format(STATISTICS_EVENT_LEVEL_ID, pMsg->mCpIndex).c_str(), &eventAttrLevel);
	mTalkingDataAgent->onEvent(StringUtils::format(STATISTICS_EVENT_LEVEL_ID, pMsg->mCpIndex).c_str(), &eventAttrLevel);

	if (cpData->levelType == E_Level_Boss)
	{
		//DataEyeEventMapType eventAttrBoss;
		//eventAttrBoss[EVENT_BOSS_COMPLETE_KEY_UID] = uid;
		//eventAttrBoss[EVENT_BOSS_COMPLETE_KEY_CPID] = levelID;
		//eventAttrBoss[EVENT_BOSS_COMPLETE_KEY_RESULT] = result;
		//DCEvent::onEvent(EVENT_BOSS_COMPLETE_ID, &eventAttrLevel);

		DataEyeAgent::EventParamMap eventAttrBoss;
		eventAttrBoss[STATISTICS_EVENT_BOSS_KEY_UID] = uid;
		eventAttrBoss[STATISTICS_EVENT_BOSS_KEY_RESULT] = result;

		mDataEyeAgent->onEvent(StringUtils::format(STATISTICS_EVENT_BOSS_ID, pMsg->mCpIndex).c_str(), &eventAttrBoss);
		mTalkingDataAgent->onEvent(StringUtils::format(STATISTICS_EVENT_BOSS_ID, pMsg->mCpIndex).c_str(), &eventAttrBoss);
	}

	/*eLostPointStep lpStep = eNone;
	switch (pMsg->mCpIndex)
	{
	case 1010: lpStep = eLostPointStep::eCompleteCheckPoint1;	break;
	case 1020: lpStep = eLostPointStep::eCompleteCheckPoint2;	break;
	case 1030: lpStep = eLostPointStep::eCompleteCheckPoint3;	break;
	case 1040: lpStep = eLostPointStep::eCompleteCheckPoint4;	break;
	case 1050: lpStep = eLostPointStep::eCompleteCheckPoint5;	break;
	case 1060: lpStep = eLostPointStep::eCompleteCheckPoint6;	break;
	case 1070: lpStep = eLostPointStep::eCompleteCheckPoint7;	break;
	case 1080: lpStep = eLostPointStep::eCompleteCheckPoint8;	break;
	case 1090: lpStep = eLostPointStep::eCompleteCheckPoint9;	break;
	case 1100: lpStep = eLostPointStep::eCompleteCheckPoint10;	break;
	default:
		break;
	}*/

	//int lpData = sgUserObj()->getOneLostPointData(lpStep);

	int iDataIndex = pMsg->mCpIndex * 2;
	int lpData = sgUserObj()->getOneLostPointData(iDataIndex);
	if (lpData != 1)
	{
		if (pMsg->mSuccess > 0)
		{
			sgUserObj()->updateLostPointData(iDataIndex, true);
			
			DataEyeAgent::EventParamMap eventAttrLostPoint;
			eventAttrLostPoint[STATISTICS_EVENT_LOST_POINT_KEY_STEP] = StringUtils::format("Complete Check Point %d", pMsg->mCpIndex);
			mDataEyeAgent->onEvent(STATISTICS_EVENT_LOST_POINT_ID, &eventAttrLostPoint);
			mTalkingDataAgent->onEvent(STATISTICS_EVENT_LOST_POINT_ID, &eventAttrLostPoint);
		}
		else
		{
			sgUserObj()->updateLostPointData(iDataIndex, false);
		}
	}
}


void Statistics::doChannelLogin(MessageChannelLogin* pMsg)
{
	if (nullptr == pMsg)
		return;

	CCLOG("[trace]Statistics::doChannelLogin : Result=%d, Uid=%s, Token=%s, ChannelId=%d", pMsg->mResult, pMsg->mUid.c_str(), pMsg->mToken.c_str(), pMsg->channelId);


	//DCLevels::begin(StringUtils::format(LEVEL_ID_FORMAT, msg->mCpIndex).c_str());

	//DCEvent::onEvent(StringUtils::format(EVENT_LEVEL_ID_FORMAT, msg->mCpIndex).c_str());

	/*int lpData = sgUserObj()->getOneLostPointData(eLostPointStep::Login);
	if (lpData != 1)
	{
		DataEyeAgent::EventParamMap eventAttrLostPoint;
		eventAttrLostPoint[STATISTICS_EVENT_LOST_POINT_KEY_STEP] = "Channel Login";
		mDataEyeAgent->onEvent(STATISTICS_EVENT_LOST_POINT_ID, &eventAttrLostPoint);

		sgUserObj()->updateLostPointData(eLostPointStep::Login, true);
	}*/

	int lpData = sgUserObj()->getOneLostPointData(eLostPointStep::FirstLogin);
	if (lpData != 1)
	{
		if (pMsg->mResult > 0)
		{
			sgUserObj()->updateLostPointData(eLostPointStep::FirstLogin, true);

			DataEyeAgent::EventParamMap eventAttrLostPoint;
			eventAttrLostPoint[STATISTICS_EVENT_LOST_POINT_KEY_STEP] = "Channel Login";
			mDataEyeAgent->onEvent(STATISTICS_EVENT_LOST_POINT_ID, &eventAttrLostPoint);
			mTalkingDataAgent->onEvent(STATISTICS_EVENT_LOST_POINT_ID, &eventAttrLostPoint);
		}
		else
		{
			sgUserObj()->updateLostPointData(eLostPointStep::FirstLogin, false);
		}
	}
}


void Statistics::doChannelLogout(MessageChannelLogout* pMsg)
{
	if (nullptr == pMsg)
		return;
}


void Statistics::doOpenUI(MessageOpenUI* pMsg)
{
	if (nullptr == pMsg)
		return;

	CCLOG("[trace]Statistics::doOpenUI : PageType=%d", pMsg->mPageType);


	if (pMsg->mPageType == MSG_FUC_MINI_LEVEL)
	{
		int lpData = sgUserObj()->getOneLostPointData(FirstSelectCheckPoint);
		if (lpData != 1)
		{
			DataEyeAgent::EventParamMap eventAttrLostPoint;
			eventAttrLostPoint[STATISTICS_EVENT_LOST_POINT_KEY_STEP] = "Enter SelectLevels";
			mDataEyeAgent->onEvent(STATISTICS_EVENT_LOST_POINT_ID, &eventAttrLostPoint);
			mTalkingDataAgent->onEvent(STATISTICS_EVENT_LOST_POINT_ID, &eventAttrLostPoint);

			sgUserObj()->updateLostPointData(FirstSelectCheckPoint, true);
		}
	}

	std::string strEventUIOpenID;
	auto pUserProfile = GameDefault::sharedGameDefault();
	if (pMsg->mPageType == MSG_FUC_LOGO)
	{
		strEventUIOpenID = StringUtils::format(STATISTICS_EVENT_OPEN_UI_ONCE_ID, "Logo");
	}
	else if (pMsg->mPageType == MSG_FUC_LOGIN)
	{
		strEventUIOpenID = StringUtils::format(STATISTICS_EVENT_OPEN_UI_ONCE_ID, "Login");
	}
	else if (pMsg->mPageType == MSG_FUC_UPDATE)
	{
		strEventUIOpenID = StringUtils::format(STATISTICS_EVENT_OPEN_UI_ONCE_ID, "Update");
	}
	else if (pMsg->mPageType == MSG_FUC_MAIN_LEVEL)
	{
		strEventUIOpenID = StringUtils::format(STATISTICS_EVENT_OPEN_UI_ONCE_ID, "SelectWorld");
	}

	if (!strEventUIOpenID.empty())
	{
		std::string strProfileKey = "Statistics-" + strEventUIOpenID;
		if (!pUserProfile->getBoolForKey(strProfileKey.c_str(),false))
		{
			mDataEyeAgent->onEvent(strEventUIOpenID);
			mTalkingDataAgent->onEvent(strEventUIOpenID);

			pUserProfile->setBoolForKey(strProfileKey.c_str(), true);
		}
	}

	DataEyeAgent::EventParamMap eventAttrOpenUIAll;
	auto strOpenAllUIAttrKey= StringUtils::format("%d", pMsg->mPageType);
	eventAttrOpenUIAll[strOpenAllUIAttrKey] = strOpenAllUIAttrKey;
	mDataEyeAgent->onEvent(STATISTICS_EVENT_OPEN_UI_ALL_ID, &eventAttrOpenUIAll);
	mTalkingDataAgent->onEvent(STATISTICS_EVENT_OPEN_UI_ALL_ID, &eventAttrOpenUIAll);
}


void Statistics::doGuideStepOver(MessageGuideStepOver* pMsg)
{
	if (nullptr == pMsg)
		return;

	CCLOG("[trace]Statistics::doGuideStepOver : Step=%d", pMsg->mStep);

	int lpData = sgUserObj()->getOneLostPointData(FirstGuideOver);
	if (lpData != 1)
	{
		if (pMsg->mStep == 0)
		{
			sgUserObj()->updateLostPointData(FirstGuideOver, true);

			DataEyeAgent::EventParamMap eventAttrLostPoint;
			eventAttrLostPoint[STATISTICS_EVENT_LOST_POINT_KEY_STEP] = "First Guide Over";
			mDataEyeAgent->onEvent(STATISTICS_EVENT_LOST_POINT_ID, &eventAttrLostPoint);
			mTalkingDataAgent->onEvent(STATISTICS_EVENT_LOST_POINT_ID, &eventAttrLostPoint);
		}
		else
		{
			sgUserObj()->updateLostPointData(FirstGuideOver, false);
		}

	}

}


void Statistics::doRMBBuy(MessageShopPayComplent* pMsg)
{
	if (nullptr == pMsg)
		return;
	
	CCLOG("[trace]Statistics::doRMBBuy : OrderId=%s, ProductID=%d", pMsg->getOrderId().c_str(), pMsg->getProductId());

	auto shopList = sgTableManager()->getShopList();
	auto it = shopList.find(pMsg->getProductId());
	if (it == shopList.end())
		return;

	ShopInfo* pInfo = it->second;
	if (nullptr == pInfo)
		return;

	std::string strProductName = StringUtils::format("%d Diamond", pInfo->manual);
	std::string strPayTypename = "Default";
	switch (sgChannelApi()->getPaySdkType())
	{
	case E_PAY_JIDI: strPayTypename = "YD LT DuanDai";	break;
	case E_PAY_DIANXING: strPayTypename = "DX DuanDai";  break;
	case E_PAY_ONLING: strPayTypename = "OnlineSDK Pay"; break;
	}

#ifdef MGAME_PLATFORM_MPLUS
	std::string strCurrencyType = STATISTICS_CURRENCY_NAME_TWD;
#else
	std::string strCurrencyType = STATISTICS_CURRENCY_NAME_CNY;
#endif

	mDataEyeAgent->virtualCurrencyPaymentSuccess(pMsg->getOrderId(), strProductName, pInfo->gold, strCurrencyType, strPayTypename);
	mTalkingDataAgent->virtualCurrencyChargeRequest(pMsg->getOrderId(), strProductName, pInfo->gold, strCurrencyType, pInfo->manual, strPayTypename);
	mTalkingDataAgent->virtualCurrencyChargeSuccess(pMsg->getOrderId());
}


void Statistics::doItemOperation(MessageItemOperation* pMsg)
{
	if (nullptr == pMsg)
		return;

	CCLOG("[trace]Statistics::doItemOperation : Opera=%d, ItemId=%d, ItemType=%d, ItemCnt=%d, VCType=%d, VCCount=%d, Param=%s"
		, pMsg->getOperationType()
		, pMsg->getItemId()
		, pMsg->getItemType()
		, pMsg->getItemCnt()
		, pMsg->getVCType()
		, pMsg->getVCCnt()
		, pMsg->getParam().c_str());

	std::string strItemName = StringUtils::format("%d", pMsg->getItemId());
	std::string strItemType;
	switch (pMsg->getItemType())
	{
	case Defualt: strItemType = STATISTICS_ITEM_TYPE_DEFAULT; break;
	}


	switch (pMsg->getOperationType())
	{
	case eItemOperation::Buy:
	{
		std::string strVCType;
		switch (pMsg->getVCType())
		{
		case Diamond:	strVCType = STATISTICS_VITURALCURRENCY_NAME_DIAMOND;	break;
		}

		mDataEyeAgent->itemBuy(strItemName, strItemType, pMsg->getItemCnt(), pMsg->getVCCnt(), strVCType, pMsg->getParam());
		mTalkingDataAgent->itemPurchase(strItemName, pMsg->getItemCnt(), pMsg->getVCCnt());
	}
	break;

	case eItemOperation::Get:
	{
		mDataEyeAgent->itemGet(strItemName, strItemType, pMsg->getItemCnt(), pMsg->getParam());
		//mTalkingDataAgent->itemUse(strItemName, pMsg->getItemCnt());
	}
	break;

	case eItemOperation::Use:
	{
		mDataEyeAgent->itemConsume(strItemName, strItemType, pMsg->getItemCnt(), pMsg->getParam());
		mTalkingDataAgent->itemUse(strItemName, pMsg->getItemCnt());
	}
	break;

	default:
		break;
	}
}


void Statistics::doMoneyChange(MessageMoneyChange* pMsg)
{
	if (nullptr == pMsg)
		return;

	CCLOG("[trace]Statistics::doMoneyChange : Record=%d, Opera=%d, VCType=%d, ChangeCount=%d, CurCount=%d, Reason=%s"
		, pMsg->getRecordData()
		, pMsg->getChangeOperation()
		, pMsg->getVCType()
		, pMsg->getChangeCnt()
		, pMsg->getCurrentCnt()
		, pMsg->getReason().c_str());

	if (pMsg->getRecordData() <= 0)
		return;

	std::string strVCName = "Default";
	switch (pMsg->getVCType())
	{
	case Diamond:	strVCName = STATISTICS_VITURALCURRENCY_NAME_DIAMOND; break;
	}

	if (pMsg->getChangeOperation() == 1)
	{
		auto changeCount = pMsg->getChangeCnt();
		if (changeCount > 0)
		{
			mDataEyeAgent->coinGain(pMsg->getReason(), strVCName, abs(changeCount), pMsg->getChangeCnt());
			mTalkingDataAgent->virtualCurrencyReward(abs(changeCount), pMsg->getReason());
		}
		else
		{
			mDataEyeAgent->coinLost(pMsg->getReason(), strVCName, abs(changeCount), pMsg->getChangeCnt());
		}
	}
	else if (pMsg->getChangeOperation() == 2)
	{
		mDataEyeAgent->coinSetCoinNum(pMsg->getCurrentCnt(), strVCName);
	}
	
}


void Statistics::doPVPInfo(MessagePVPInfo* pMsg)
{
	if (nullptr == pMsg)
		return;

	CCLOG("[trace]Statistics::doPVPInfo : Type=%d, Mode=%d, Operate=%d"
		, pMsg->getPVPType()
		, pMsg->getPVPMode()
		, pMsg->getOperation());

	std::string strEventID;

	if (1 == pMsg->getPVPMode())
		strEventID = StringUtils::format(STATISTICS_EVENT_PVP_MATCHMODE_ID, pMsg->getPVPType());
	else if (2 == pMsg->getPVPMode())
		strEventID = StringUtils::format(STATISTICS_EVENT_PVP_ROOMMODE_ID, pMsg->getPVPType());
	else if (3 == pMsg->getPVPMode())
		strEventID = StringUtils::format(STATISTICS_EVENT_PVP_NOITEMMODE_ID, pMsg->getPVPType());

	if (1 == pMsg->getOperation())
	{
		mDataEyeAgent->onEventBegin(strEventID);
		mTalkingDataAgent->onEvent(strEventID + "_Begin");
	}
	else if (2 == pMsg->getOperation())
	{
		mDataEyeAgent->onEventEnd(strEventID);
		mTalkingDataAgent->onEvent(strEventID + "_End");
	}
}


void Statistics::doGameLoginFlow(MessageGameLoginFlow* pMsg)
{
	if (nullptr == pMsg)
		return;

	CCLOG("[trace]Statistics::doGameLoginFlow : Type=%d, Result=%d, Param=%s"
		, pMsg->mFlowType
		, pMsg->mResult
		, pMsg->mParam1.c_str());


	auto pUserProfile = GameDefault::sharedGameDefault();

	DataEyeAgent::EventParamMap eventLoginFlow;
	eventLoginFlow[STATISTICS_EVENT_LOGIN_FLOW_KEY_RESULT] = pMsg->mResult != 0 ? "Success" : "Failure";

	std::string strEventID;
	switch (pMsg->mFlowType)
	{
	case MessageGameLoginFlow::FlowType::ChannelLogin:
	{
		strEventID = StringUtils::format(STATISTICS_EVENT_LOGIN_FLOW_ID, "ChannelLogin");
		eventLoginFlow["ChannelID"] = pMsg->mParam1;
	}
	break;

	case MessageGameLoginFlow::FlowType::EnterCenterSvr:
	{
		strEventID = StringUtils::format(STATISTICS_EVENT_LOGIN_FLOW_ID, "EnterCenterSvr");
		eventLoginFlow["LoginServer"] = pMsg->mParam1;
	}
	break;

	case MessageGameLoginFlow::FlowType::EnterLoginSvr:
	{
		strEventID = StringUtils::format(STATISTICS_EVENT_LOGIN_FLOW_ID, "EnterLoginSvr");
		eventLoginFlow["GameServer"] = pMsg->mParam1;
	}
	break;

	case MessageGameLoginFlow::FlowType::EnterGameSvr: 
	{
		strEventID = StringUtils::format(STATISTICS_EVENT_LOGIN_FLOW_ID, "EnterGameSvr");
		eventLoginFlow["Note"] = pMsg->mParam1;
	}
	break;

	default:
		break;
	}

	if (!strEventID.empty())
	{
		std::string strProfileKey = "Statistics-" + strEventID;
		if (!pUserProfile->getBoolForKey(strProfileKey.c_str(),false))
		{
			mDataEyeAgent->onEvent(strEventID, &eventLoginFlow);
			mTalkingDataAgent->onEvent(strEventID, &eventLoginFlow);

			pUserProfile->setBoolForKey(strProfileKey.c_str(), true);
		}
	}
}
