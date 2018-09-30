#include "SyncControl.h"
#include <fstream>
#include "ui/dialogs/GenerlDialog.h"
#include "data/TableManager.h"
#include "EnvSet.h"
#include "net/UserObj.h"
#include "MessageManager.h"
#include "CVTime.h"
#include "login/LoginProxy.h"
#include "ranking/RankingControl.h"
#include "achieve/AchieveControl.h"
#include "act/ActControl.h"
#include "core/TimeCalculator.h"
#include "ui/PageFunction.h"
#define WAIT_SAVEDATA_TIMER "WaitSaveData_Timer"
#define SAVE_SYNCSAVEDATA_FILE "sync_savedata.db"  
#define SYNC_loginOT "sync_LoginOT"
#define SYNC_syncDataOT "sync_syncDataOT"
SyncControl::SyncControl(void)
{
	mSyncSaveDataOver = false;  
}


SyncControl::~SyncControl(void)
{
}

SyncProxy* SyncControl::getSyncProxy()
{
	return &m_SyncProxy;
}

void SyncControl::saveData()
{ 
	CCLOG("[trace]SyncControl::saveData save to file");
}

void SyncControl::init()
{  
	loadSyncData();
	
}

void SyncControl::update()
{
	updateLoginTimer();
}



void SyncControl::loadSyncData()
{
}


///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////同步玩家信息////////////

void SyncControl::sendSyncPlayerInfoRqst()
{
	if(sgLoginProxy()->getLoginSuccess() == false)
		return;
	m_SyncProxy.sendSyncPlayerInfoRqst();
}

void SyncControl::doSyncPlayerInfoRsp(const fgame::CSSycPlayerInfoResp& resp)
{
	m_SyncProxy.doSyncPlayerInfoRsp(resp);
	sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, "");
	sgMessageManager()->sendMessage(EMT_ENERGY_CHANGE,"");  
}

///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////// 

void SyncControl::doGmCmdRsp(const fgame::CSGMCmdReq& pcmd)
{
}

void SyncControl::createLoginTimer()
{
	std::string key = StringUtils::format("%s", SYNC_loginOT);
	if(sgTimeCalculator()->hasKey(key))
		sgTimeCalculator()->removeTimeCalcultor(key);
	sgTimeCalculator()->createTimeCalcultor(key, 5.0f);
}

void SyncControl::removeLoginTimer()
{
	std::string key = StringUtils::format("%s", SYNC_loginOT);
	if(sgTimeCalculator()->hasKey(key))
		sgTimeCalculator()->removeTimeCalcultor(key);
}

void SyncControl::updateLoginTimer()
{
	std::string key = StringUtils::format("%s", SYNC_loginOT);
	if(sgTimeCalculator()->hasKey(key) == false)
		return;
	float leftTime = sgTimeCalculator()->getTimeLeft(key);
	if(leftTime <= 0.0f)
	{
		//超时了我靠 
		std::string param = StringUtils::format("%d %d",CS_CMD_ENTER_SERVER,1);
		sgMessageManager()->sendMessage(EMT_RQST_RESULT, param);
		sgTimeCalculator()->removeTimeCalcultor(key);
	}
}


void SyncControl::createSyncTimer()
{
	std::string key = StringUtils::format("%s", SYNC_syncDataOT);
	if(sgTimeCalculator()->hasKey(key))
		sgTimeCalculator()->removeTimeCalcultor(key);
	sgTimeCalculator()->createTimeCalcultor(key, 5.0f); 
}

void SyncControl::removeSyncTimer()
{
	std::string key = StringUtils::format("%s", SYNC_syncDataOT);
	if(sgTimeCalculator()->hasKey(key))
		sgTimeCalculator()->removeTimeCalcultor(key);
}

bool SyncControl::checkSyncSaveDataOver()
{
	return mSyncSaveDataOver;
}

void SyncControl::setSyncSaveDataOver(bool value)
{
	mSyncSaveDataOver = value;
}





