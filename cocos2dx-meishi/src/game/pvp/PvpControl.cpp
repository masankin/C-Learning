#include "PvpControl.h"
#include "pvp/PvpProxy.h"
#include "TimeCalculator.h"
#include "ui/PageFunction.h"
#include "battle/BattleControl.h"
#include "ui/dialogs/GenerlDialog.h"
#include "balance/BalanceControl.h"
#include "GamePrecedure.h"
//#include "battle/Formation.h"
#include "battle/Formation/FormationV2.h"
#include "net/UserObj.h"
#include "game/data/TableManager.h"
#include "prop/PropsControl.h"
#include "EnvSet.h"
#include "game/battle/Build.h"
#include "game/winner/WinnerControl.h"
#include "ranking/RankingControl.h"
#include "sync/SyncControl.h"
#include "battle/Assistant/Assistant.h"
#include <algorithm>

#include "PvpRoomControl.h"

#include "game/daytask/DayTaskControl.h"
#include "core/CVTime.h"

PvpControl::PvpControl() :
mPvpProxy(nullptr), m_ping(0), m_endCout(0), mIsWaiing(0), m_PvpTest(nullptr), m_Lock(false), mIsDead(false), mIsEnd(true), mWinUin(0)
, mMode(0), mIsReConne(false), mPvpStatus(PVP_STATUS_NULL), mIsHoures(false), m_RecoverItemId(0), m_lastOwnerIdx(0), mLastPvpCount(0)
, mStartParkComplete(false), mGlyphLoadComplete(false), mAsstLoadComplete(false)
{
	setIsNoItemOpen(0);
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("eGlyphLoadPvpSuccess", CC_CALLBACK_1(PvpControl::doGlyphLoadPvpSuccess, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("eAsstSavePVPInfo", CC_CALLBACK_1(PvpControl::doAsstLoadPvpSuccess, this));
}

PvpControl::~PvpControl()
{
	if (mPvpProxy)
	{
		delete mPvpProxy;
		mPvpProxy = nullptr;
	}
}

void PvpControl::update(float dt)
{
	
	if (sgTimeCalculator()->hasKey("pvp_ping") && sgTimeCalculator()->getTimeLeft("pvp_ping") <= 0.0f)
	{
		m_ping++;

		CCLOG("[trace]PvpProxy::sendPing  m_ping = %d ", m_ping);
		getPvPProxy()->sendPing();
		sgTimeCalculator()->resetTimeCalcultor("pvp_ping");

		if (m_ping >= 3)
		{
			sgTimeCalculator()->removeTimeCalcultor("pvp_ping");
			m_ping = 0;
		}
	}

	if (sgTimeCalculator()->hasKey(PVP_RECOVER_ITEM_KEY) && sgTimeCalculator()->getTimeLeft(PVP_RECOVER_ITEM_KEY) <= 0.0f)
	{
		sgPropsControl()->cleanCdTime(m_RecoverItemId);
		sgMessageManager()->sendMessage(EMT_PROP_USER, __String::createWithFormat("%d", m_RecoverItemId)->getCString());
		m_RecoverItemId = 0;
		sgTimeCalculator()->removeTimeCalcultor(PVP_RECOVER_ITEM_KEY);
	}

	mStartRsp.set_ts(mStartRsp.ts() + dt);
}

PvpProxy * PvpControl::getPvPProxy()
{
	if (!mPvpProxy)
	{
		mPvpProxy = new PvpProxy();
	}
	return mPvpProxy;
}

void PvpControl::doMatchRsp(const fgame::CSPvpMatchResp& stMsg)
{

	mMatchRsp = stMsg;	
	if (mMatchRsp.match_ret() == 0)
	{
		GenerlDialog::hide();
		std::string szTitle = "Prompt";
		std::string szContent = "Match Failed!";
		GenerlDialog::show(szTitle, szContent);
	}
	else
	{

		GenerlDialog::hide();

		//int _size = mMatchRsp.playinfo_size();
		//for (int i = 0; i != _size; ++i)
		//{
		//	const fgame::CSPlayInfo& playerInfo = mMatchRsp.playinfo(i);
			//CCLOG("[trace] match player:%d uin:%d name:%s", i, playerInfo.uin(), playerInfo.name());
		//}

		// match success, cancel btn can't work.
		PVPMatchLoadingDialog*  matchLoading = dynamic_cast<PVPMatchLoadingDialog*>(
								sgPageFunction()->getWindowByPageName("PVPMatchLoadingDialog.csb"));
		if (matchLoading){
			//matchLoading->setShow(false);
			matchLoading->setCancelBtnEnable(false);
		}
		
		int type = mMatchRsp.type();

		if (mMatchRsp.has_flag())
		{
			setPVPMode(mMatchRsp.flag());
		}

		if (mMatchRsp.has_roomid())
		{
			mPvpProxy->setRoomId(mMatchRsp.roomid());
		}

		startGame(type);

		CCLOG("[trace]PvpControl::doMatchRsp  type = %d!", type);
	}
}

void PvpControl::doStartRsp(const fgame::CSPvpStartResp& stMsg)
{

	mIsDead = false;
	mIsEnd = false;
	mIsReConne = false;
	mWinUin = sgUserObj()->getUin();
	m_endCout = 0;

	PvpRoom * _PvpRoom = sgPvpRoomControl()->getPvpRoom();
	if (_PvpRoom != NULL && _PvpRoom->isShow())
	{
		_PvpRoom->setShow(false);
	}
	
	PvpInviteDialog * _PvpInviteDialog = sgPvpRoomControl()->getPvpInviteDilog();
	if (_PvpInviteDialog != NULL && _PvpInviteDialog->isShow())
	{
		_PvpInviteDialog->setShow(false);
	}

	mStartRsp = stMsg;
	double time = stMsg.ts();
	difTime = stMsg.ts() - sgTimeCalculator()->getCurTime();

	std::string pvpInfoParam = StringUtils::format("%d %d %d", this->getPvpType(), this->getPVPMode(), 1);
	sgMessageManager()->sendMessage(EMT_PVP_INFO, pvpInfoParam);


	//通过开房进来 。露出钻石
	if (getPVPMode() == PVP_MODE_ROOM && getIsHourse())
	{
		std::string param = StringUtils::format("%d %d %d %d %d %s"
			, 1
			, 1
			, eVituralCurrencyType::Diamond
			, -CREATE_PRICE
			, sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
			, "PvpRoomControl::doCreateRoomRsp");
		sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
	}

	mStartParkComplete = true;
	loadGame();
}

void PvpControl::doPingRsp(const fgame::CSPvpPingResp& stMsg)
{
	unsigned int isHost = stMsg.host();
	unsigned int agent = sgUserObj()->getUin();
	
	bool flag = isHost == agent;
	

	if (!sgUserObj()->getIsHost())
	{
		sgUserObj()->setIsHost(flag);
		if (sgBattleControl()->getIsPvp() && sgUserObj()->getIsHost())
		{

			setPvpTestColor(Color4B::YELLOW);
			CCLOG("[trace]PvpControl::doPingRsp  I 'm Host");
			sgBattleControl()->checkAddPvpAi();
			sgTimeCalculator()->createTimeCalcultor("pvp_update", 3);
			
		}
		else
		{
			setPvpTestColor(Color4B::GREEN);
		}

		//sgBalanceControl()->startCheckPoint(sgGamePrecedure()->getCurMapInfo().checkpt);
	}
	else
	{
		if (!flag)
		{
			sgTimeCalculator()->removeTimeCalcultor("pvp_update");
			sgBattleControl()->removeCampAi();
			CCLOG("[trace]PvpControl::doPingRsp  I 'm not Host");
			setPvpTestColor(Color4B::GREEN);
		}
		sgUserObj()->setIsHost(flag);
	}

	mIsWaiing = false;

}


void PvpControl::doMapRsp(const fgame::CSPvpMapResp& stMsg)
{

	int _size = stMsg.data_size();
	for (int i = 0; i != _size; ++i)
	{
		const fgame::CSMapData& mapData = stMsg.data(i);
		int level = mapData.level();
		int BuildId = mapData.buildid();
		int Type = mapData.type();
		int Camp = mapData.camp();
		int num = mapData.num();

		std::set<int> flist;
		int _size = mapData.list_size();
		for (int i = 0; i != _size; ++i)
		{
			const ::fgame::CSFormationList& FormationId = mapData.list(i);
			flist.insert(FormationId.id());
		}

		sgBattleControl()->s2cUpdateBuild(level, BuildId, Type, Camp, num, flist, mIsReConne);
	}
	mMapResp = stMsg;
	mIsReConne = false;

	CCLOG("[trace]PvpControl::doMapRsp  data size = %d ", _size);
}

void PvpControl::doUpgrade(const fgame::CSPvpUpgradeResp& stMsg)
{
	int buildId = stMsg.buildid();
	int uin = stMsg.uin();
	int level = -1;

	if (stMsg.has_level())
	{
		level = stMsg.level();
	}

	if (uin != sgUserObj()->getUin())
	{
		//char params[64] = { 0, };
		//sprintf(params, "%d 1", buildId);

		//sgMessageManager()->sendMessage(EMT_UPGRADE, params);

		BaseBuild* pBuild = sgBattleControl()->getTargetBuild(buildId);
		if (pBuild)
		{
			pBuild->setServerUpLevel(level);
		}

	}


	long time = sgTimeCalculator()->getCurTime() - stMsg.ts();
	mStartRsp.set_ts(time);

	CCLOG("[trace]PvpControl::doUpgrade  buildId = %d ", buildId);
}

void PvpControl::doFight(const fgame::CSPvpFightResp& stMsg)
{

	int src = stMsg.src();
	int dest = stMsg.dest();
	int num = stMsg.num();
	int forId = stMsg.formationid();
	int uin = stMsg.uin();

	float time = sgTimeCalculator()->getCurTime() - getCTime(stMsg.ts());
	difTime = stMsg.ts() - sgTimeCalculator()->getCurTime();

	if (sgUserObj()->getUin() == uin)
	{
		FormationV2* pFormation = sgBattleControl()->getFormationById(forId);
		if (pFormation != nullptr)
		{
			pFormation->resumeByTimer();
		}
	}
	else
	{
		char params[128] = { 0, };
		sprintf(params, "%d %d %d %d %d %f", src, dest, forId, 1, num, 1.0f);
		sgMessageManager()->sendMessage(EMT_MOVE, params);
	}

	CCLOG("[trace]PvpControl::doFight  src = %d,dest = %d ,num = %d , forId =%d,uin = %d ", src, dest, num, forId, uin);
}

void PvpControl::doEnd(const fgame::CSPvpEndResp& stMsg)
{
	if (!sgBattleControl()->getIsPvp())//不是pvp模式 不做处理
	{
		CCLOG("[erro]PvpControl::doEnd ,the end  no enter gamepage");
		return;
	}

	mIsEnd = true;
	mPvpStatus = ePvpStatus::PVP_STATUS_S_END;
	mEndResp = stMsg;
	long time = sgTimeCalculator()->getCurTime() - stMsg.ts();
	mStartRsp.set_ts(time);

	std::string pvpInfoParam = StringUtils::format("%d %d %d", this->getPvpType(), this->getPVPMode(), 2);
	sgMessageManager()->sendMessage(EMT_PVP_INFO, pvpInfoParam);

	if (stMsg.win() == sgUserObj()->getUin())
	{	
		CCLOG("[trace]PvpControl::doEnd  i 'm win");
		sgBalanceControl()->doCheckPtEnd(true);
	}
	else
	{
		CCLOG("[trace]PvpControl::doEnd  i 'm fail");
		sgBalanceControl()->doCheckPtEnd(false);
	}
	sgRankingControl()->requestPlayerRankingInfo();
	sgRankingControl()->requestPlayerSelfRankingList();



	sgRankingControl()->addPvpResult(stMsg);

	//胜利者建筑播放动画
	eCampType eWinCamp = getCampTypeByUid(stMsg.win());
	sgBattleControl()->playPvpCampWinAni(eWinCamp);


	//王者之路	
	if (this->getPVPMode() != PVP_MODE_RAND) return;	
	if (stMsg.win() == sgUserObj()->getUin())
	{
		sgWinnerControl()->setWinnerCount(sgWinnerControl()->getWinnerCount()+1);

		if (sgWinnerControl()->checkCanOpen(WINNER_ROAD) == true)
		{
			int totalNum = sgTableManager()->getWinnerDataNum();
			//当前连胜场数--->实际领取的对应场数奖励
			int canCount = sgWinnerControl()->getWinnerCount() > totalNum ? totalNum : sgWinnerControl()->getWinnerCount();
			const WinnerData* pWinner = sgTableManager()->getWinnerData(canCount);
			if (pWinner)
			{
				int currGold = pWinner->getGoldNum();

				if (canCount > 30 || (canCount >= 5 && canCount % 5 == 0))
				{
					sgWinnerControl()->sendWinnerTip(canCount, currGold);
				}
			}
		}

	}
	else
	{
		sgWinnerControl()->setWinnerCount(0);
	}



}

void PvpControl::doLeave(const fgame::CSPvpLeaveResp& stMsg) 
{
	int uid = stMsg.uin();

	if (uid == sgUserObj()->getUin())
	{
		CCLOG("[erro]PvpControl::doLeave  I 'm leave in game");
		if(sgBattleControl()->getIsPvp())
		GenerlDialog::show("", sgTextHelper(TEXT_SERVER_FORCE), [](Ref* sendr){

			PageFunction::Get()->gotoFunction(MSG_FUC_MAIN_LEVEL);
			PageFunction::Get()->gotoFunction(MSG_FUC_WIN_PVP);
			GenerlDialog::hide();

		}, true);
	}
	else
	{
		CCLOG("[trace]PvpControl::doLeave  uin = %d player leave",uid);
		if (sgBattleControl()->getIsPvp())
		{
			eCampType type = getCampTypeByUid(uid);
			if (type != ECAMP_TYPE_Invalid)
			{
				sgBattleControl()->changeToNeutral(type);
			}
		}
	}
}

void PvpControl::doUseItem(const fgame::CSPvpUseItemResp& stMsg)
{
	int  id = stMsg.itemid();
	int uin = stMsg.uin();

	if (id == sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_EXCHANGE))
	{
		id = stMsg.buildid();
	}
	bool flag = sgPropsControl()->isPropIng();
	int propId = sgPropsControl()->getCurPropId();

	sgPropsControl()->setProp(id, true);
	if (sgPropsControl()->doProp(Vec2(stMsg.x(), stMsg.y()), getCampTypeByUid(uin), true) == false && sgUserObj()->getIsHost())
	{
		mPvpProxy->sendRecoverItem(uin, id);
	}

	if (flag)
	{
		sgPropsControl()->setProp(propId,true);
	}


	CCLOG("[trace]PvpControl::doUseItem  uin = %d item id = %d", uin, id);
}

eCampType PvpControl::getMyCampType()
{

	eCampType type = getCampTypeByUid(sgUserObj()->getUin());

	if (type != ECAMP_TYPE_Invalid)
	{
		return type;
	}

	CCLOG("[error]PvpControl::getMyCampType camp data error!");
	return ECAMP_TYPE_Neutral;
}

eCampType PvpControl::getCampTypeByUid(int uid)
{
	int _size = mStartRsp.points_size();
	for (int i = 0; i != _size; ++i)
	{
		const fgame::CSPlayerPoint& playerPt = mStartRsp.points(i);
		if (playerPt.uin() == uid)
			return (eCampType)playerPt.camp();
	}

	CCLOG("[error]PvpControl::getMyCampType camp data error!");
	return ECAMP_TYPE_Invalid;
}

int PvpControl::getUinByCamp(eCampType type)
{
	int _size = mStartRsp.points_size();
	for (int i = 0; i != _size; ++i)
	{
		const fgame::CSPlayerPoint& playerPt = mStartRsp.points(i);
		if (playerPt.camp() == (int)type)
			return playerPt.uin();
	}

	CCLOG("[error]PvpControl::getUinByCamp the camp = %d not found",type);
	return -1000;
}

int PvpControl::getWinUidByCamp(int camp)
{
	int _size = mStartRsp.points_size();
	for (int i = 0; i != _size; ++i)
	{
		const fgame::CSPlayerPoint& playerPoint = mStartRsp.points(i);

		if (playerPoint.camp() == camp)
		{
			return playerPoint.uin();
		}
		//CCLOG("[trace] match player:%d uin:%d name:%s", i, playerInfo.uin(), playerInfo.name());
	}

	if (EnvSet::leon())
	{
		GenerlDialog::show("getWinUidByCamp", StringUtils::format("[PvpControl] getWinUidByCamp Invalid  camp = %d ",camp));
	}
	return INVALID_CAMP_UIN;
}

bool PvpControl::checkIsAI(eCampType type)
{
	int _size = mStartRsp.points_size();
	for (int i = 0; i != _size; ++i)
	{
		const fgame::CSPlayerPoint& playerPt = mStartRsp.points(i);
		if (((eCampType)playerPt.camp()) == type && playerPt.uin() <= 0)
		{
			return true;
		}

	}
	return false;
}

double PvpControl::getSTime(double ctime)
{
	return ctime + difTime;
}

double PvpControl::getCTime(double stime)
{
	return stime - difTime;
}

bool PvpControl::doErro(int erroCode)
{
	if (erroCode == PVP_NOT_TIME || erroCode == NOT_MATCH_STATE || erroCode == NOT_PVP_SCORE)//不在时间段内 || 取消匹配错误
	{
		sgMessageManager()->sendMessage(EMT_STOP_MATCH, "0");
	}
	else if(erroCode == ENERGY_NOT_ENOUGH)
	{
		sgMessageManager()->sendMessage(EMT_STOP_MATCH, "0");
		sgSyncControl()->sendSyncPlayerInfoRqst();
	}
	else if (erroCode == PVP_NOT_NET)
	{
		if ((!sgPvpControl()->getIsEnd() || mPvpStatus == PVP_STATUS_C_END) && sgBattleControl()->getIsPvp())
		{
			PageFunction::Get()->gotoFunction(MSG_FUC_WIN_PVP_ROOM_BACK);
		}

		return false;
	}
	
	return true;
}

void PvpControl::doStopMatch(const fgame::CSPvpStopMatchResp& stMsg)
{
	CCLOG("[trace]PvpControl::doStopMatch");
	sgMessageManager()->sendMessage(EMT_STOP_MATCH, "0");
}

void PvpControl::doAppearCreep(const fgame::CSPvpAppearCreepResp& stMsg)
{
	int id = stMsg.creepid();
	int pointId = stMsg.dataid();

	CCLOG("[trace]PvpControl::doAppearCreep point id = %d, id = %d",pointId,id);
	sgBattleControl()->getCreepsManager()->syncPvpCreepsInfo(id, pointId);
}

void PvpControl::setPvpTestStr(std::string text)
{
	if (EnvSet::leon())
	{
		if (m_PvpTest /*&& !m_Lock*/)
		{
			m_PvpTest->setText(text);
		}
	}
}

void PvpControl::setPvpTest(ui::Text* pText)
{
	if (pText)
	{
		m_PvpTest = pText;
	}
}

const std::string& PvpControl::getPvpTest()
{
	if (m_PvpTest)
	{
		return m_PvpTest->getString();
	}

	return "";
}

void PvpControl::setPvpTestColor(const Color4B color)
{
	if (m_PvpTest)
	{
		m_PvpTest->setTextColor(color);
	}
}

void PvpControl::doCampDead(const fgame::CSPvpObserverResp& stMsg)
{
	int uin = stMsg.uin();
	eCampType camp = getCampTypeByUid(uin);
	Camp*  pCamp = sgBattleControl()->getTargetCamp(camp);

	if (pCamp)
	{
		pCamp->setHasBuild(false);
		if (pCamp->getCampType() == sgBattleControl()->getOwnCamp() && sgBattleControl()->checkEnmyNum() >= 2)
		{
			sgPvpControl()->setIsDead(true);
			sgBattleControl()->getBattleView()->setEnbelTouch(false);
			sgPageFunction()->gotoFunction(MSG_FUC_WIN_PVP_WATCH);
			sgMessageManager()->sendMessage(eMessageType::EMT_SIMPLIFY_UI, "");

			if (sgPvpControl()->getPVPMode() == PVP_MODE_RAND)
				sgWinnerControl()->setWinnerCount(0);
		}

		Assistant* pAsst = pCamp->getAssistant();
		if (pAsst != nullptr)
		{
			pCamp->getAssistant()->stop();
		}
	}

	CCLOG("[trace] PvpControl::doCampDead  camp = %d ,uin = %d", camp,uin);
}
void PvpControl::startGame(int type)
{
	getPvPProxy()->sendStartRqst(type);
	sgTimeCalculator()->createTimeCalcultor("pvp_ping", 1.0f);
}

void PvpControl::loadGame()
{
	if (mAsstLoadComplete && mGlyphLoadComplete && mAsstLoadComplete)
	{
		if (sgGamePrecedure()->selectPvpMap(mStartRsp.mapid()))
		{
			PageFunction::Get()->gotoFunction(MSG_FUC_PLAYING);
			CCLOG("[trace]PvpControl::doStartRsp  select checkpoint id = %d!", mStartRsp.mapid());
		}
		else
		{
			GenerlDialog::show("error", StringUtils::format("pvp no this checkpoint %d", mStartRsp.mapid()));
		}
		setPvpStatus(PVP_STATUS_MAP);
	}
}

int PvpControl::getKillPeople(int uin)
{
	Camp* pCamp = sgBattleControl()->getTargetCamp(getCampTypeByUid(uin));
	if (pCamp)
	{
		return pCamp->getKillPeople();
	}

	CCLOG("[erro] PvpControl::getKillPeople  dont fint camp uin = %d", uin);

	return 0;
}

int PvpControl::getProductPeople(int uin)
{
	Camp* pCamp = sgBattleControl()->getTargetCamp(getCampTypeByUid(uin));
	if (pCamp)
	{
		return pCamp->getProductPeople();
	}

	CCLOG("[erro] PvpControl::getKillPeople  dont fint camp uin = %d", uin);

	return 0;
}

std::string PvpControl::getPvpPlayerIconId(eCampType camp)
{
	int _size = mStartRsp.points_size();
	for (int i = 0; i != _size; ++i)
	{
		const fgame::CSPlayerPoint& playerPt = mStartRsp.points(i);
		if (playerPt.camp() == camp)
			return playerPt.icon();
	}

	CCLOG("[error]PvpControl::getPvpPlayerIconId camp data error!");
	return 0;
}


int PvpControl::getPvpType()
{
	if (mStartRsp.has_type())
		return mStartRsp.type();

	return 0;
}


std::string PvpControl::getPvpPlayerName(eCampType camp)
{
	std::string name = "pvp name";
	int _size = mStartRsp.points_size();
	for (int i = 0; i != _size; ++i)
	{
		const fgame::CSPlayerPoint& playerPt = mStartRsp.points(i);
		if (playerPt.camp() == camp)
			return playerPt.name();
	}

	CCLOG("[error]PvpControl::getPvpPlayerIconId camp data error!");
	return "error";
}

void PvpControl::doHeartbeat()
{
	if (sgBattleControl()->getIsPvp() && sgTimeCalculator()->hasKey(PVP_END_LOCK_KEY))
	{
		float time = sgTimeCalculator()->getTimeLeft(PVP_END_LOCK_KEY);

		if (mPvpStatus == ePvpStatus::PVP_STATUS_C_END && mIsEnd && time <= 0)
		{
			CCLOG("[trace]PvpControl::doHeartbeat winuin = %d", mWinUin);
			
			sgTimeCalculator()->removeTimeCalcultor(PVP_END_LOCK_KEY);
			mPvpProxy->sendEnd(mWinUin);
		}
	}
}

#define PVP_NO_ITEM_START (20)
#define PVP_NO_ITEM_END (21)
void PvpControl::setIsNoItemOpen(unsigned long long _time)
{
	CVTime targetTime(_time == 0 ? time(0) : _time);
	if (targetTime.GetHour() >= PVP_NO_ITEM_START && targetTime.GetHour() < PVP_NO_ITEM_END)
	{
		if (mIsNoItemOpen != true)
		{
			sgMessageManager()->sendMessage(EMT_STOP_MATCH, "1");
		}
		mIsNoItemOpen = true;
		return;
	}
	if (mIsNoItemOpen != false)
	{
		sgMessageManager()->sendMessage(EMT_STOP_MATCH, "2");
	}
	mIsNoItemOpen = false;
}

void PvpControl::doGlyphLoadPvpSuccess(cocos2d::EventCustom* customData)
{
	mGlyphLoadComplete = true;
	loadGame();
}

void PvpControl::doAsstLoadPvpSuccess(cocos2d::EventCustom* customData)
{
	mAsstLoadComplete = true;
	loadGame();
}

void PvpControl::clean()
{
	setPvpStatus(ePvpStatus::PVP_STATUS_NULL);
	sgTimeCalculator()->removeTimeCalcultor(PVP_END_LOCK_KEY);
	m_RecoverItemId = 0;
	mAsstLoadComplete = false;
	mGlyphLoadComplete = false;
	mAsstLoadComplete = false;
}

void PvpControl::doRecoverItem(const fgame::CSPvpRecoverItemResp& stMsg)
{
	int itemId = stMsg.itemid();
	int uin = stMsg.uin();
	int num = stMsg.num();

	if (uin == sgUserObj()->getUin())
	{
		sgTimeCalculator()->createTimeCalcultor(PVP_RECOVER_ITEM_KEY, PVP_RECOVER_ITEM_TIME);
		sgUserObj()->setProp(itemId, num);
		m_RecoverItemId = itemId;
	}
}

void PvpControl::doUseAsstSkill(const fgame::CSPvpUseSkillResp& stMsg)
{
	bool bOk = stMsg.ok();
	int skillId = stMsg.skillid();
	int campId = stMsg.campid();
	std::vector<int> randList;

	int randSize = stMsg.randomnumbers_size();
	for (int i = 0; i != randSize; ++i)
	{
		int rand = stMsg.randomnumbers(i);
		randList.push_back(rand);
	}

	sgBattleControl()->getAsstBattleCtrl()->castActiveSkill((eCampType)campId, skillId, randList);

	CCLOG("[trace] PvpControl::doUseAsstSkill  bOk = %d ,skillId = %d ,campId = %d ,randomsize = %d", bOk, skillId, campId, randSize);
}

void PvpControl::doNoItemPvpBoxAward(const fgame::CSPvpGetAwardResp& stMsg)
{
	if (!sgBattleControl()->getIsPvp())//不是pvp模式 不做处理
	{
		return;
	}
	mPvpAwardResp = stMsg;
	sgMessageManager()->sendMessage(EMT_PVPRESULT_OPENBOX, "");
}

int PvpControl::getPlayerPoint(eCampType campType)
{
	int _size = mStartRsp.points_size();
	for (int i = 0; i != _size; ++i)
	{
		const fgame::CSPlayerPoint& playerPt = mStartRsp.points(i);
		if (playerPt.camp() == campType)
			return playerPt.point();
	}

	CCLOG("[error]PvpControl::getPvpPlayerIconId camp data error!");
	return 0;
}

