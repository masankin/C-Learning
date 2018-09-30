#include "DayTaskControl.h"
#include "game/net/UserObj.h"
#include "net/NetPack.h"
#include "game/ui/dialogs/DayTaskDialog.h"
#include "game/ui//PageFunction.h"
#include "core/MessageManager.h"
#include "utils/Comm.h"
#include "game/act/PowerGiftControl.h"
#include "game/ui/dialogs/GiftAwardDialog.h"
#include "game/ui/system/ActionTip.h"
#include "game/net/UserObj.h"

#define MISSION_UNCOMPLETE 0
#define MISSION_COMPLETE  1
#define MISSION_GET		2

DayMission::DayMission()
	:mId(0), mValue(0), mRequestValue(1), mIsFinished(false), mIsGet(false)
{

}

DayMission::~DayMission()
{

}

bool DayMission::init()
{
	bool ret = false;
	do {
		ret = true;
	} while (0);
	return ret;
}

DayTaskControl::DayTaskControl() : mProxy(nullptr), mDialog(nullptr)
{
	mMissions.clear();
	VipTipsText[0] = TEXT_VIP1_TIPS;
	VipTipsText[1] = TEXT_VIP2_TIPS;
	VipTipsText[2] = TEXT_VIP3_TIPS;
	mProxy = new DayTaskProxy();
	sgMessageManager()->regisiterMessageHandler(EMT_MISSION_STATE, this);

	syncDay = 0;
}

DayTaskControl::~DayTaskControl()
{
	sgMessageManager()->removeMessageHandler(this);
	if (mProxy)
		delete mProxy;
}

void DayTaskControl::onReceiveMassage(GameMessage * message)
{

}

DayMission* DayTaskControl::getMissionById(int id)
{
	cocos2d::Vector<DayMission*>::iterator iter = mMissions.begin();
	for (; iter != mMissions.end(); iter++) {
		if (id == (*iter)->mId) {
			return *iter;
		}
	}
	return nullptr;
}

DayTaskDataList* DayTaskControl::getAllMissions()
{
	return sgTableManager()->getDayTaskList();
}

int DayTaskControl::getAllMissionCount()
{
	// 
	//return getAllMissions()->size();
	return mMissions.size();
}

void DayTaskControl::syncMissionFromServer(const ::fgame::CSMissionListResp &missionListResp)
{
	mMissions.clear();

	int count = missionListResp.missions_size();
	for (int i = 0; i < count; ++i) {
		DayMission* cacheMission = DayMission::create();
		const fgame::CSMission& mission = missionListResp.missions(i);
		cacheMission->mId = mission.missionid();
		cacheMission->mValue = mission.value();
		cacheMission->mRequestValue = mission.request();
		CCLOG("[trace] syncMissionFromServer missionID = %d   local value = %d,  server value == %d, server request == %d", 
			mission.missionid(), cacheMission->mValue, mission.value(), mission.request());

		if (mission.completed()) {
			// mission completed(get)
			cacheMission->mIsFinished = true;
			cacheMission->mIsGet = true;
			CCLOG("[trace syncMission]  id:%d completed", mission.missionid());
		}
		else if (mission.value() >= cacheMission->mRequestValue)
		{
			//  mission finished(can get)
			cacheMission->mIsFinished = true;
			cacheMission->mIsGet = false;
			CCLOG("[trace syncMission]  id:%d finished", mission.missionid());
		}
		else {
			cacheMission->mIsFinished = false;
			cacheMission->mIsGet = false;
		}
		mMissions.pushBack(cacheMission);
	}

	CVTime pnowTime(sgLoginProxy()->getServerTime());
	syncDay = pnowTime.GetDay();

	checkShowTip();
	refreshDialog();
}

void DayTaskControl::missionUpdateFromServer(const fgame::CSMissionUpdateResp &missionUpdateResp)
{
	int missionId = missionUpdateResp.missionid();
	int value = missionUpdateResp.value();

	CCLOG("[trace]  missionUpdateFromServer  missionId = %d,  value = %d", missionId, value);
	DayMission* mission = getMissionById(missionId);
	if (mission && value >= mission->mRequestValue) {
		// mission completed
		mission->mIsFinished = true;
		sortMission();
		checkShowTip();
	}
}

void DayTaskControl::updateMission(const fgame::CSMission &resqMission)
{
	CCLOG("[trace] updateMission mission id ============== %d",  resqMission.missionid());
	DayMission* mission = getMissionById(resqMission.missionid());
	if (mission) {
		mission->mIsFinished = true;
		mission->mIsGet = true;
		checkShowTip();
	}

	if (resqMission.has_giftid()) {
		CCLOG("[trace]   updateMission  giftid = %d", resqMission.giftid());
		GiftAwardDialog::showDialogByGiftId(resqMission.giftid());
#ifdef MGAME_PLATFORM_MPLUS
		if (sgMplusControl()->getDayTaskShare()){
			sgMplusControl()->setIsCanShare(true);
			sgMplusControl()->setDayTaskShare(false);
		}
#endif
	}
	
	refreshDialog();
}


void DayTaskControl::goMission(int pageId)
{
	//CCLOG("[trace]     mission =================== pageId = %d",  pageId);
	switch (pageId) {
	case 1: 
		hideDialog();
		sgPageFunction()->gotoFunction(MSG_FUC_WIN_SHOP);
		sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, "1");
		break;
	case 2:
		hideDialog();
		sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);
		break;
	case 3:
		hideDialog();
		//if (sgPowerGiftControl()->getIsOpen() == 1)
		//{
			sgPowerGiftControl()->sendGetPowerActivityDataRqst();
			PageFunction::Get()->gotoFunction(MSG_FUC_POWER_GIFT);
		//}
		/*else
		{
			if (sgPowerGiftControl()->getActID() == 1)
			{
				GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_POWER_OFF));
			}
			else
			{
				GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_ENERGY_OFF));
			}
		}*/
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		hideDialog();
		sgPageFunction()->gotoFunction(MSG_FUC_WIN_PVP);
		break;
	case 8:
		// vip level 1 task
		//hideDialog();
		checkVipDask(1);
		break;
	case 9:
		// vip level 2 task
		//hideDialog();
		checkVipDask(2);
		break;
	case 10:
		// vip level 3 task
		//hideDialog();
		checkVipDask(3);
		break;
	default:
		break;
	}
}

void DayTaskControl::refreshDialog()
{
	sortMission();

	auto dialog = getDialog();
	if (dialog) {
		dialog->updateMissions(true);
	}
}

void DayTaskControl::hideDialog()
{
	auto dialog = getDialog();
	if (dialog) {
		dialog->setShow(false);
	}
}

bool DayTaskControl::checkMissionComplete()
{
	cocos2d::Vector<DayMission*>::iterator iter = mMissions.begin();
	for (; iter != mMissions.end(); iter++) {
		//if ((*iter)->mIsFinished && !(*iter)->mIsGet){  // 任务完成，但还没领取
		//	return true;
		//}
		if (!(*iter)->mIsFinished) {
			return true;
		}
	}
	return false;
}

void DayTaskControl::checkShowTip()
{
	if (checkMissionComplete()) {
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 1", MSG_FUC_DAYTASK));
	}
	else {
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 0", MSG_FUC_DAYTASK));
	}
}

// 对任务进行排序（可领 > 未完成 > 已领）
void DayTaskControl::sortMission()
{
	int pos = 1;

	cocos2d::Vector<DayMission*> tempMissions;
	cocos2d::Vector<DayMission*>::iterator iter = mMissions.begin();

	// 可领奖励
	for (; iter != mMissions.end(); iter++) {
		if ((*iter)->mIsFinished && !((*iter)->mIsGet)){
			mMissionSortMap[pos++] = (*iter)->mId;
		}
		else {
			tempMissions.pushBack(*iter);
		}
	}

	// 可领任务
	cocos2d::Vector<DayMission*>::iterator goIter = tempMissions.begin();
	for (; goIter != tempMissions.end();) {
		if (!((*goIter)->mIsFinished)){
			mMissionSortMap[pos++] = (*goIter)->mId;
			goIter = tempMissions.erase(goIter);
		}
		else{
			goIter++;
		}
	}
	// 剩下的
	cocos2d::Vector<DayMission*>::iterator restIter = tempMissions.begin();
	for (; restIter != tempMissions.end(); restIter++) {
		mMissionSortMap[pos++] = (*restIter)->mId;
	}
	tempMissions.clear();

}

int DayTaskControl::getCardDaysLeft()
{
	int days = 0;
	do {
		days = sgUserObj()->getPersonalInfo()->monthcarddays();//sgUserObj()->getMonthCardLeft();
	} while (0);

	return days;
}

int	DayTaskControl::getMissionIdByIndex(int cellIndex)
{
	int index = cellIndex;
	
	std::map<int, int>::iterator missionMap = mMissionSortMap.find(cellIndex);
	if (missionMap != mMissionSortMap.end()) {
		index = missionMap->second;
	}

	CCLOG("[trace]DayTaskControl::getMissionIdByIndex mission id == %d", index);
	return index;
}

void DayTaskControl::trySyncMission()
{
	if (mMissions.size() < 1) {
		if (mProxy) mProxy->sendMissionListReq();
	}
	else{
		CVTime pnowTime(sgLoginProxy()->getServerTime());
		int curDay = pnowTime.GetDay();
		CCLOG("[trace]  curDay === %d,  syncDay == %d", curDay, syncDay);
		if (curDay != syncDay){  // 当天是否已经同步过
			if (mProxy) mProxy->sendMissionListReq();
		}
	}
}
bool DayTaskControl::checkVipDask(int vipLevel)
{
	int currLevel = sgUserObj()->getPersonalInfo()->viplevel();//sgUserObj()->getVipLevel();
	if (currLevel >= vipLevel) {
		return true;
	}
	else {
		sgPageFunction()->gotoFunction(MSG_FUC_VIP);
		hideDialog();
		return false;
	}
}
