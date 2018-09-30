#include "PvpProxy.h"
#include "net/NetManager.h"
#include "net/UserObj.h"
#include "pvp/PvpControl.h"
#include "core/TimeCalculator.h"
#include "game/battle/Build.h"
#include "game/battle/BattleControl.h"
#include "game/balance/BalanceControl.h"
#include "game/ui/PageFunction.h"
#include "game/daytask/DayTaskControl.h"
#include "game/ranking/RankingControl.h"
#include "lua/LuaFuncHelper.h"
#include "game/battle/Camp.h"
#include "game/prop/PropsControl.h"

PvpProxy::PvpProxy() :mMsgNum(1000)
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_PING, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_MATCH, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_START, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_FIGHT, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_UPGRADE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_END, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_MAP, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_LEAVE, this);
	
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_USE_ITEM, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_STOP_MATCH, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_APPEAR_CREEP, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_OBSERVER, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_RECOVER_ITEM, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_GET_AWARD, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVP_USE_AS_SKILL, this);
	//sgMessageManager()->regisiterMessageHandler(EMT_MOVE, this);
	//sgMessageManager()->regisiterMessageHandler(EMT_UPGRADE, this);
}

PvpProxy::~PvpProxy()
{

}

void PvpProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		const fgame::CSErrorMsg& pMsg = stRsp.getS2CMsg()->error_msg();
		sgPvpControl()->doErro(pMsg.errcode());
		CCLOG("[trace]PvpProxy::DoRsp error!");
	}


	//战斗外接收
	if (stRsp.getS2CMsg()->has_match_resp())
	{
		sgPvpControl()->doMatchRsp(stRsp.getS2CMsg()->match_resp());
		return;

	}else if (stRsp.getS2CMsg()->has_start_resp())
	{
		sgPvpControl()->doStartRsp(stRsp.getS2CMsg()->start_resp());
		return;
	}
	else if (stRsp.getS2CMsg()->has_stopmatch_resp())
	{
		sgPvpControl()->doStopMatch(stRsp.getS2CMsg()->stopmatch_resp());
		return;
	}
	else if (stRsp.getS2CMsg()->has_leave_resp())
	{
		sgPvpControl()->doLeave(stRsp.getS2CMsg()->leave_resp());
		return;
	}
	


	// 战斗内接收
	if (!sgBattleControl()->getIsPvp())
	{
		return;
	}


	if (stRsp.getS2CMsg()->has_ping_resp())
	{
		sgPvpControl()->doPingRsp(stRsp.getS2CMsg()->ping_resp());

	}
	else if (stRsp.getS2CMsg()->has_map_resp())
	{
		sgPvpControl()->doMapRsp(stRsp.getS2CMsg()->map_resp());
	}
	else if (stRsp.getS2CMsg()->has_upgrade_resp())
	{
		sgPvpControl()->doUpgrade(stRsp.getS2CMsg()->upgrade_resp());
	}
	else if (stRsp.getS2CMsg()->has_fight_resp())
	{
		sgPvpControl()->doFight(stRsp.getS2CMsg()->fight_resp());
	}
	else if (stRsp.getS2CMsg()->has_end_resp())
	{
		sgPvpControl()->doEnd(stRsp.getS2CMsg()->end_resp());
	}
	else if (stRsp.getS2CMsg()->has_pvpuseitem_resp())
	{
		sgPvpControl()->doUseItem(stRsp.getS2CMsg()->pvpuseitem_resp());
	}
	else if (stRsp.getS2CMsg()->has_appearcreep_resp())
	{
		sgPvpControl()->doAppearCreep(stRsp.getS2CMsg()->appearcreep_resp());
	}
	else if (stRsp.getS2CMsg()->has_pvpobserver_resp())
	{
		sgPvpControl()->doCampDead(stRsp.getS2CMsg()->pvpobserver_resp());
	}
	else if (stRsp.getS2CMsg()->has_recoveritem_resp())
	{
		sgPvpControl()->doRecoverItem(stRsp.getS2CMsg()->recoveritem_resp());
	}
	else if (stRsp.getS2CMsg()->has_pvpgetaward_resp())
	{
		sgPvpControl()->doNoItemPvpBoxAward(stRsp.getS2CMsg()->pvpgetaward_resp());
	}
	else if (stRsp.getS2CMsg()->has_pvpuseskill_resp())
	{
		sgPvpControl()->doUseAsstSkill(stRsp.getS2CMsg()->pvpuseskill_resp());
	}
	sgPvpControl()->doHeartbeat();//任何战斗内回包都是一个心跳包 此时网络正常

}

void PvpProxy::sendMatchRqst(int type,int mode,int score)
{
	if (sgPvpControl()->getIsWaiing())
		return;

	unsigned int uCmd = fgame::CS_CMD_PVP_MATCH;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpMatchReq* pMatchRqst = msgPack.getC2SMsg()->mutable_match_req();
	pMatchRqst->set_type(type);
	pMatchRqst->set_mode(mode);
	//pMatchRqst->set_score(score);
	sgPvpControl()->setIsWaiing(true);

	CCLOG("[trace]PvpProxy::sendMatchRqst  type = %d", type);
	sgNetManager()->SendPkg(msgPack);
}

void PvpProxy::sendStartRqst(int type)
{
	unsigned int uCmd = fgame::CS_CMD_PVP_START;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	mMsgNum = cocos2d::random(1000, 9999);
	fgame::CSPvpStartReq* pStartRqst = msgPack.getC2SMsg()->mutable_start_req();
	pStartRqst->set_type(type);
	pStartRqst->set_msgno(mMsgNum);
	pStartRqst->set_roomid(mRoomId);

	CCLOG("[trace]PvpProxy::sendStartRqst  type = %d ", type);
	sgNetManager()->SendPkg(msgPack);
}

void PvpProxy::sendPing()
{
	unsigned int uCmd = fgame::CS_CMD_PVP_PING;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpPingReq* pPingRqst = msgPack.getC2SMsg()->mutable_ping_req();
	pPingRqst->set_ts(sgPvpControl()->getStartRsp()->ts());
	pPingRqst->set_msgno(++mMsgNum);
	pPingRqst->set_roomid(mRoomId);

	sgNetManager()->SendPkg(msgPack);
}

void PvpProxy::sendMap(const MapBulidList& builList)
{
	if (sgPvpControl()->getIsEnd())
		return;

	std::string testStr;
	int count = 0;


	unsigned int uCmd = fgame::CS_CMD_PVP_MAP;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);
	fgame::CSPvpMapReq* mMapRsp = msgPack.getC2SMsg()->mutable_map_req();
	mMapRsp->set_msgno(++mMsgNum);
	mMapRsp->set_roomid(mRoomId);

	MapBulidList::const_iterator it = builList.begin();
	for (; it != builList.end(); it++)
	{
		fgame::CSMapData* mMapData = mMapRsp->add_data();
		mMapData->set_buildid(it->first);
		mMapData->set_camp(it->second->getCamp());

		if (!it->second->isCurUpgrade() && it->second->getServerUpLevel() == 0)
		{
			mMapData->set_level(it->second->getLevel());
		}
		else
		{
			//level == 0 的数据作为 非法数据，此数据对接受者无效
			mMapData->set_level(0);
		}

		mMapData->set_num(it->second->getOwnPeople());
		mMapData->set_type(it->second->getBuildType());

		const std::set<int>& fList = it->second->getTarFormationIdList();
		for (auto sIt : fList)
		{
			fgame::CSFormationList* formId = mMapData->add_list();
			formId->set_id(sIt);

			count++;
		}

	}

	testStr = StringUtils::format(" \n Formation data  = %d", count);
	sgPvpControl()->setPvpTestStr(testStr);

	float time = sgTimeCalculator()->getCurTime();
	mMapRsp->set_ts(sgPvpControl()->getSTime(time));

	CCLOG("[trace]PvpProxy::sendMap  builList size = %d ", builList.size());
	sgNetManager()->SendPkg(msgPack);


}

void PvpProxy::sendFight(int src, int des, int num, int fId)
{
	if (sgPvpControl()->getIsEnd())
		return;

	unsigned int uCmd = fgame::CS_CMD_PVP_FIGHT;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpFightReq* pFightRqst = msgPack.getC2SMsg()->mutable_fight_req();
	pFightRqst->set_src(src);
	pFightRqst->set_dest(des);
	pFightRqst->set_num(num);
	pFightRqst->set_formationid(fId);
	pFightRqst->set_uin(sgUserObj()->getUin());
	pFightRqst->set_msgno(++mMsgNum);
	pFightRqst->set_roomid(mRoomId);

	double time = sgTimeCalculator()->getCurTime();
	pFightRqst->set_ts(sgPvpControl()->getSTime(time));

	CCLOG("[trace]PvpProxy::sendFight  src = %d,dest = %d ,num = %d , forId =%d ", src, des, num, fId);
	sgNetManager()->SendPkg(msgPack);
}

void PvpProxy::sendUpgrade(int buildId, int level)
{
	if (sgPvpControl()->getIsEnd())
		return;

	unsigned int uCmd = fgame::CS_CMD_PVP_UPGRADE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpUpgradeReq* UpgradeRqst = msgPack.getC2SMsg()->mutable_upgrade_req();
	UpgradeRqst->set_buildid(buildId);
	UpgradeRqst->set_uin(sgUserObj()->getUin());
	UpgradeRqst->set_level(level);
	UpgradeRqst->set_msgno(++mMsgNum);
	UpgradeRqst->set_roomid(mRoomId);


	float time = sgTimeCalculator()->getCurTime();
	UpgradeRqst->set_ts(sgPvpControl()->getSTime(time));

	CCLOG("[trace]PvpProxy::sendUpgrade  buildId = %d,level = %d ", buildId, level);
	sgNetManager()->SendPkg(msgPack);
}

void PvpProxy::sendEnd(int uid)
{
	//if (!sgNetManager()->IsConnect())
	//{
	//	GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_NETWORK_ERROR),[this](Ref* sendr){
	//		sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
	//		GenerlDialog::hide();
	//	},true);

	//	return;
	//}

	//・￠?íend?ó?? ・à?1?à′?・￠?í
	//上报每日任务相关数据
	sgUserObj()->sendDailyMissionData(true);

	sgTimeCalculator()->createTimeCalcultor(PVP_END_LOCK_KEY, PVP_END_LOCK_TIME);

	unsigned int uCmd = fgame::CS_CMD_PVP_END;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpEndReq* pEndRqst = msgPack.getC2SMsg()->mutable_end_req();
	float time = sgTimeCalculator()->getCurTime();
	pEndRqst->set_ts(sgPvpControl()->getSTime(time));
	pEndRqst->set_win(uid);
	pEndRqst->set_msgno(++mMsgNum);
	pEndRqst->set_roomid(mRoomId);


	if (sgTimeCalculator()->hasKey("pvp_update"))
	{
		sgTimeCalculator()->removeTimeCalcultor("pvp_update");
	}

	sgPvpControl()->setIsEnd(true); 
	if (sgPvpControl()->getPvpStatus() != ePvpStatus::PVP_STATUS_S_END)
	{
		sgPvpControl()->setPvpStatus(ePvpStatus::PVP_STATUS_C_END);
	}

	sgPvpControl()->setPvpTestStr(StringUtils::format("send start win uin = %d", uid));
	sgPvpControl()->setWinUin(uid);

	CCLOG("[trace]PvpProxy::sendEnd  win uin = %d ", uid);
	sgNetManager()->SendPkg(msgPack);

}

void PvpProxy::sendLeave()
{
	unsigned int mode = 2;
	float gameTime = sgBalanceControl()->getTimeLeft();
	float allTime = 900.0f;
	
	const CheckPoint* pCheck = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);
	if (pCheck)
	{
		allTime = pCheck->timeLimit;
	}
	
	bool needAddHonor = false;
	if (sgPvpControl()->getIsDead())
	{
		needAddHonor = true;
	}
	else
	{
		if (gameTime > (allTime - 120))
		{
			mode = 1;
		}
		else
		{
			needAddHonor = true;
		}
	}

	if (sgPvpControl()->getPVPMode() != 3)//从当前模式为无道具场
	{
		if (needAddHonor)
		{
			auto pGradeData = sgTableManager()->getGradeData(sgUserObj()->getPersonalInfo()->score());//sgUserObj()->getPvpPoint());
			if (nullptr != pGradeData)
			{
				//sgUserObj()->setHonorCount(sgUserObj()->getHonorCount() + pGradeData->getRandLoseScore());
				sgUserObj()->getPersonalInfo()->set_honorcount(sgUserObj()->getPersonalInfo()->honorcount() + pGradeData->getRandLoseScore());
			}
		}
	}
	else
	{
		//sgUserObj()->setHonorCount(sgUserObj()->getHonorCount() - 50);
		sgUserObj()->getPersonalInfo()->set_honorcount(sgUserObj()->getPersonalInfo()->honorcount() - 50);
	}

	

	sgRankingControl()->giveUp(-40);

	std::string pvpInfoParam = StringUtils::format("%d %d %d", sgPvpControl()->getPvpType(), sgPvpControl()->getPVPMode(), 2);
	sgMessageManager()->sendMessage(EMT_PVP_INFO, pvpInfoParam);

	unsigned int uCmd = fgame::CS_CMD_PVP_LEAVE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	CCLOG("[trace]PvpProxy::sendLeave  ");
	fgame::CSPvpLeaveReq* pLeaveRqst = msgPack.getC2SMsg()->mutable_leave_req();
	pLeaveRqst->set_msgno(++mMsgNum);
	pLeaveRqst->set_roomid(mRoomId);
	pLeaveRqst->set_mode(mode);


	sgNetManager()->SendPkg(msgPack);
}

bool PvpProxy::fault(const fgame::CSErrorMsg &err)
{
	return sgPvpControl()->doErro(err.errcode());
}

void PvpProxy::sendUseItem(int id, const Vec2 pos, int buildId)
{
	unsigned int uCmd = fgame::CS_CMD_PVP_USE_ITEM;
	NetPack msgPack;
	
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpUseItemReq* pUseItemRqst = msgPack.getC2SMsg()->mutable_pvpuseitem_req();
	pUseItemRqst->set_itemid(id);
	pUseItemRqst->set_x(pos.x);
	pUseItemRqst->set_y(pos.y);
	pUseItemRqst->set_buildid(buildId);
	pUseItemRqst->set_ts(sgTimeCalculator()->getSystemTime());
	pUseItemRqst->set_msgno(++mMsgNum);
	pUseItemRqst->set_roomid(mRoomId);


	CCLOG("[trace]PvpProxy::sendUseItem  use Item id = %d ,buildId = %d", id, buildId);
	sgNetManager()->SendPkg(msgPack);

}

void PvpProxy::sendStopMatch(int mode)
{
	unsigned int uCmd = fgame::CS_CMD_PVP_STOP_MATCH;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpStopMatchReq* pUseItemRqst = msgPack.getC2SMsg()->mutable_stopmatch_req();
	pUseItemRqst->set_type(mode);

	CCLOG("[trace]PvpProxy::sendStopMatch  sendStopMatch");
	sgNetManager()->SendPkg(msgPack);

}

void PvpProxy::sendAppearCreep(int id, int dataId)
{
	unsigned int uCmd = fgame::CS_CMD_PVP_APPEAR_CREEP;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpAppearCreepReq* pCreepRqst = msgPack.getC2SMsg()->mutable_appearcreep_req();
	pCreepRqst->set_creepid(id);
	pCreepRqst->set_dataid(dataId);
	pCreepRqst->set_ts(sgTimeCalculator()->getSystemTime());
	pCreepRqst->set_msgno(++mMsgNum);
	pCreepRqst->set_roomid(mRoomId);

	CCLOG("[trace]PvpProxy::sendAppearCreep  creepid = %d ,dataid = %d",id,dataId);
	sgNetManager()->SendPkg(msgPack);
}

void PvpProxy::sendDeadCamp(eCampType type)
{
	int uin = sgPvpControl()->getUinByCamp(type);

	unsigned int uCmd = fgame::CS_CMD_PVP_OBSERVER;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpObserverReq* pObserverRqst = msgPack.getC2SMsg()->mutable_pvpobserver_req();
	pObserverRqst->set_uin(uin);
	pObserverRqst->set_msgno(++mMsgNum);
	pObserverRqst->set_roomid(mRoomId);

	CCLOG("[trace]PvpProxy::sendDeadCamp  camp = %d ,uin = %d  dead", type, uin);
	sgNetManager()->SendPkg(msgPack);
}

void PvpProxy::sendRecoverItem(int uin, int itemId)
{
	unsigned int uCmd = fgame::CS_CMD_PVP_RECOVER_ITEM;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpRecoverItemReq* pRecoverRqst = msgPack.getC2SMsg()->mutable_recoveritem_req();
	pRecoverRqst->set_uin(uin);
	pRecoverRqst->set_itemid(itemId);

	sgNetManager()->SendPkg(msgPack);
	CCLOG("[trace]PvpProxy::sendRecoverItem  uin = %d ,itemId = %d", uin, itemId);
}

void PvpProxy::sendAsstSkill(int campId, int skillId, int rollNum)
{
	unsigned int uCmd = fgame::CS_CMD_PVP_USE_AS_SKILL;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPvpUseSkillReq* pSkillRqst = msgPack.getC2SMsg()->mutable_pvpuseskill_req();

	pSkillRqst->set_campid(campId);
	pSkillRqst->set_skillid(skillId);
	pSkillRqst->set_rollnum(rollNum);

	sgNetManager()->SendPkg(msgPack);
	CCLOG("[trace]PvpProxy::sendAsstSkill  campId = %d ,skillId = %d, rollNum = %d", campId, skillId, rollNum);
	
}
