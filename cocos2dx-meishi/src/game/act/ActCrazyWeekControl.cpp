#include "ActCrazyWeekControl.h"
#include "net/NetManager.h"
#include "core/MessageManager.h"
#include "game/ui/dialogs/GiftAwardDialog.h"
#include "game/net/UserObj.h"

CrazyWeekProxy::CrazyWeekProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_GET_ACTIVITY_CRAZY_DETAIL, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_ACTIVITY_CRAZY_USAGE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_ACTIVITY_CRAZY_USABLE, this);
}

CrazyWeekProxy::~CrazyWeekProxy()
{

}

void CrazyWeekProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]CrazyWeek::DoRsp error!");
	}
	else if (stRsp.getS2CMsg()->has_activitycrazydtls_resp())
	{
		// 活动周信息
		sgActCrazyWeekControl()->syncCrazyWeekInfo(stRsp.getS2CMsg()->activitycrazydtls_resp());
	}
	else if (stRsp.getS2CMsg()->has_activitycrazyusable_resp())
	{
		// 完成活动任务
		sgActCrazyWeekControl()->updateCrazyWeekInfo(stRsp.getS2CMsg()->activitycrazyusable_resp());
	}
	else if (stRsp.getS2CMsg()->has_activitycrazyusage_resp())
	{
		// 领取活动奖励
		sgActCrazyWeekControl()->doReward(stRsp.getS2CMsg()->activitycrazyusage_resp());
	}
}

bool CrazyWeekProxy::fault(const fgame::CSErrorMsg &err)
{
	return true;
}

void CrazyWeekProxy::sendGetCrazyWeekInfoReq()
{
	unsigned int uCmd = fgame::CS_CMD_GET_ACTIVITY_CRAZY_DETAIL;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);
	::fgame::CSActivityCrazyDtlsReq* pCrazyReq = msgPack.getC2SMsg()->mutable_activitycrazydtls_req();
	sgNetManager()->SendPkg(msgPack);
}

void CrazyWeekProxy::sendGetRewardReq(int rewardId)
{
	unsigned int uCmd = fgame::CS_CMD_ACTIVITY_CRAZY_USAGE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	::fgame::CSActivityCrazyUsageReq* pReq = msgPack.getC2SMsg()->mutable_activitycrazyusage_req();
	pReq->set_rewardid(rewardId);
	sgNetManager()->SendPkg(msgPack);
}


ActCrazyWeekControl::ActCrazyWeekControl()
	:mCurrDay(1),
	mRewardId(502),
	mCrazyPoint(0),
	mExtraHonourPoint(0),
	mConditionCompleted(0),
	mSubConditionCompleted(0),
	mRewardState(0),
	mTimeString("19870101")
{

}

ActCrazyWeekControl::~ActCrazyWeekControl()
{

}

void ActCrazyWeekControl::sendGetCrazyWeekInfoReq()
{
	mCrazyWeekProxy.sendGetCrazyWeekInfoReq();
}

void ActCrazyWeekControl::sendGetRewardReq()
{
	mCrazyWeekProxy.sendGetRewardReq(mRewardId);
}

void ActCrazyWeekControl::showCrazyWeekIcon(bool flag)
{
	CCLOG("[jim]  show crazyWeek icon");
	sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", MSG_FUC_CRAZYWEEK, flag?1:0));
	mIsCrazyWeekOpen = flag;
	sgUserObj()->setIsOpenCrazyWeek(flag);
}

void ActCrazyWeekControl::showCrazyWeekTip(bool flag)
{
	bool isShow = false;
	if (mRewardState == 1) {
		isShow = true;
	}
	sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d %d", MSG_FUC_CRAZYWEEK, isShow ? 1 : 0));
	sgMessageManager()->sendMessage(EMT_CRAZY_WEEK, "1");
}

void ActCrazyWeekControl::syncCrazyWeekInfo(const fgame::CSActivityCrazyDtlsResp &dtlsResp)
{
	if (dtlsResp.has_points()) {
		mCrazyPoint = dtlsResp.points();
	}
	if (dtlsResp.has_score()) {
		mExtraHonourPoint = dtlsResp.score();
	}
	if (dtlsResp.has_rewardid()) {
		mRewardId = dtlsResp.rewardid();
	}
	if (dtlsResp.has_usable()) {
		mRewardState = dtlsResp.usable();
	}
	if (dtlsResp.has_conditionstate()) {
		mConditionCompleted = dtlsResp.conditionstate();
	}
	else{
		mConditionCompleted = 0;
	}
	if (dtlsResp.has_subconditionstate()) {
		mSubConditionCompleted = dtlsResp.subconditionstate();
	}
	else {
		mSubConditionCompleted = 0;
	}
	if (dtlsResp.has_crazytime()){
		mTimeString = dtlsResp.crazytime();
	}
	if (dtlsResp.has_day()) {
		int activityDay = dtlsResp.day();
		if (activityDay > 7 && activityDay < 1) {
			activityDay = 1;
		}
		mCurrDay = activityDay;
		showCrazyWeekIcon(true);
		//CCLOG(" server CurrDay ============= %d ",  mCurrDay);
		showCrazyWeekTip();
	}

	//CCLOG("[jim syncCrazyWeekInfo] mExtraHonourPoint == %d", mExtraHonourPoint);
	//CCLOG("[jim syncCrazyWeekInfo] Day == %d", mCurrDay);
	//CCLOG("[jim syncCrazyWeekInfo] mCrazyPoint == %d", mCrazyPoint);
}

void ActCrazyWeekControl::updateCrazyWeekInfo(const fgame::CSActivityCrazyUsableResp &usableResp)
{
	if (usableResp.has_conditionstate()){
		mConditionCompleted = usableResp.conditionstate();
	}
	else{
		mConditionCompleted = 0;
	}
	if (usableResp.has_subconditionstate()) {
		mSubConditionCompleted = usableResp.subconditionstate();
	}
	else {
		mSubConditionCompleted = 0;
	}

	if (usableResp.has_usable()){
		mRewardState = usableResp.usable();
	}

	if (usableResp.has_extrascore()) {
		//CCLOG("[jim]  addPoint == %d",  usableResp.extrascore());
		mExtraHonourPoint += usableResp.extrascore();
		//CCLOG("[jim] after mExtraHonourPoint == %d", mExtraHonourPoint);
	}

	showCrazyWeekTip();
}

void ActCrazyWeekControl::doReward(const fgame::CSActivityCrazyUsageResp &usageResp)
{
	//if (usageResp.has_rewardpackage()) {
		//const fgame::CSGiftPackage & rewardResp = usageResp.rewardpackage();

	if (usageResp.has_points()) {
		mCrazyPoint = usageResp.points();
	}

	// 奖励领取
	if (usageResp.has_giftid() && usageResp.giftid() > 0) {
		GiftAwardDialog::showDialogByGiftId(usageResp.giftid());
	}

	//	std::map<int, int> awardMap;
	//	if (rewardResp.has_power()) {
	//		if (rewardResp.power() > 0) {
	//			awardMap.insert(pair<int, int>(A_PVEPOWER, rewardResp.power()));
	//		}
	//	}
	//	if (rewardResp.has_energy()) {
	//		if (rewardResp.energy() > 0) {
	//			awardMap.insert(pair<int, int>(A_PVPPOWER, rewardResp.energy()));
	//		}
	//	}
	//	if (rewardResp.has_diamond()) {
	//		std::string param = StringUtils::format("%d %d %d %d %d %s"
	//			, 1
	//			, 1
	//			, eVituralCurrencyType::Diamond
	//			, rewardResp.diamond()
	//			, sgUserObj()->getGoldCoin()
	//			, "CrazyWeek::doReward");

	//		sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
	//		if (rewardResp.diamond() > 0) {
	//			awardMap.insert(pair<int, int>(A_DIAMOND, rewardResp.diamond()));
	//		}
	//	}

	//	int itemCount = rewardResp.items_size();
	//	for (int i = 0; i < itemCount; i++) {
	//		const fgame::CSItem itemInfo = rewardResp.items(i);
	//		if (itemInfo.count() > 0) {
	//			awardMap.insert(pair<int, int>(itemInfo.itemid(), itemInfo.count()));
	//		}
	//	}

	//	GiftAwardDialog::showDialogByAward(awardMap);
	//}
	mRewardState = 2;
	showCrazyWeekTip();

	//CCLOG("[jim doReward] mExtraHonourPoint == %d", mExtraHonourPoint);
	//CCLOG("[jim doReward] Day == %d", mCurrDay);
	//CCLOG("[jim doReward] mCrazyPoint == %d", mCrazyPoint);
}
