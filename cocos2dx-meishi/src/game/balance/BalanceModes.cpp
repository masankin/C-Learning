#include "BalanceModes.h"
#include "battle/BattleControl.h"
#include "battle/Camp.h"
#include "battle/Build.h"
#include "TimeCalculator.h"
#include "balance/BalanceControl.h"
#include "prop/PropsControl.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"
#include "net/UserObj.h"
#include "net/UserObj.h"
#include "ui/PageFunction.h"
#include "winner/WinnerControl.h"
#include "core/SoundManager.h"

bool BaseMode::init()
{
	sgMessageManager()->regisiterMessageHandler(EMT_FIGHT, this);
	sgMessageManager()->regisiterMessageHandler(EMT_BUILD_DESTORY, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_ADDTIME, this);

	return true;
}

void BaseMode::start(int cp)
{
	init();

	const CheckPoint* pCP = sgTableManager()->getCheckPointByID(cp);
	if (!pCP)
	{
		CCAssert(0, "[falt]ModeDestory::start table data error!");
		return;
	}
	mCheckPt = cp;
}

void BaseMode::doFail()
{
	
	if (!sgBattleControl()->getIsPvp())
	{
		clearData();
		sgBalanceControl()->doCheckPtEnd(false);

	}
	else
	{
		doPvpFail();
	}

}

void BaseMode::doSuccess()
{

	if (!sgBattleControl()->getIsPvp())
	{
		sgBalanceControl()->doCheckPtEnd(true);
		clearData();
	}
	else
	{
		doPvpSuccess();
	}
}

bool BaseMode::doPvpFail()
{
	getBuildListSize();


	float _left = getTimeLeft();//时间到达
	if (_left <= 0.0f)
	{
		sgPvpControl()->getPvPProxy()->sendEnd(FIGHT_END_NUM);//pvp 时间到了
		clearData();
		return true;
	}


	if (sgBattleControl()->checkEnmyNum() <= 1 && mMineBuildSize <= 0)//失败
	{
		eCampType camp = sgBattleControl()->getPVPWinCamp();
		if (camp == ECAMP_TYPE_Invalid) return false;

		int uid = sgPvpControl()->getWinUidByCamp(camp);
		if (uid != INVALID_CAMP_UIN)
			sgPvpControl()->getPvPProxy()->sendEnd(uid);
		clearData();
		return true;
	}


	/*if (!sgPvpControl()->getIsDead() && sgBattleControl()->checkEnmyNum() >= 2 && mMineBuildSize <= 0)
	{
		sgPvpControl()->setIsDead(true);
		sgBattleControl()->getBattleView()->setEnbelTouch(false);
		sgPageFunction()->gotoFunction(MSG_FUC_WIN_PVP_WATCH);
		sgMessageManager()->sendMessage(eMessageType::EMT_SIMPLIFY_UI, "");

		if (sgPvpControl()->getPVPMode() == PVP_MODE_RAND)
			sgWinnerControl()->setWinnerCount(0);
	}*/

	return false;
}

bool BaseMode::doPvpSuccess()
{
	clearData();
	sgPvpControl()->getPvPProxy()->sendEnd(sgUserObj()->getUin());
	return true;
}
void BaseMode::update()
{
	if (hasTimeLimit())
	{
		float _left = getTimeLeft();
		if (_left <= 0.0f)
		{
			doFail();
		}
	}
}

void BaseMode::clearData()
{
	sgTimeCalculator()->removeTimeCalcultor("cp_time");
	sgMessageManager()->removeMessageHandler(this);

}

bool BaseMode::hasTimeLimit()
{
	if (sgTimeCalculator()->hasKey("cp_time"))
		return true;

	return false;
}

float BaseMode::getTimeLeft()
{
	return sgTimeCalculator()->getTimeLeft("cp_time");
}

void BaseMode::getBuildListSize()
{
	int num = 0;
	// 建筑相关
	const MapBulidList& playerBuildlist = sgBattleControl()->getBuildList();

	MapBulidList::const_iterator it = playerBuildlist.begin();
	for (; it != playerBuildlist.end(); it++)
	{
		if (it->second->getCamp() == sgBattleControl()->getOwnCamp())
		{
			num++;
		}
	}

	mMineBuildSize = num;


	MapBulidList enemyBuildlist;
	sgBattleControl()->getEnemyBuildList(sgBattleControl()->getOwnCamp(), enemyBuildlist);

	// 该值在>2人对战无效
	mEneBuildSize = enemyBuildlist.size();
}

void BaseMode::getFormationListSize()
{
	// 己方阵型大小
	Camp* pOwnCamp = sgBattleControl()->getTargetCamp(sgBattleControl()->getOwnCamp());
	if (!pOwnCamp)
		return;

	mMineFormationSize = pOwnCamp->getFormationList().size();

	// 敌方阵型大小
	CampList camplist;
	sgBattleControl()->getEnemyCamp(camplist);

	mEneFormationSize = 0;
	CampList::iterator it = camplist.begin();
	for (; it != camplist.end(); ++it)
	{
		mEneFormationSize += it->second->getFormationList().size();
	}
} 

void BaseMode::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_PROP_ADDTIME)
	{
		sgSoundManager()->playMusicEffect(SOUND_PROP_ADDTIME);
		
		MessageAddTime* pMes = dynamic_cast<MessageAddTime*>(message);
		if (pMes)
		{
			sgTimeCalculator()->addTimeByKey("cp_time", pMes->time);
			sgBattleControl()->getBattleView()->playPropAni(PROP_TYPE_TIME);
		}
	}
}

void BaseMode::starGameTime()
{
	const CheckPoint* pCP = sgTableManager()->getCheckPointByID(mCheckPt);
	if (!pCP)
	{
		CCAssert(0, "[falt]ModeDestory::start table data error!");
		return;
	}

	if (pCP->timeLimit > 0 && !hasTimeLimit())
		sgTimeCalculator()->createTimeCalcultor("cp_time", pCP->timeLimit);
}



bool ModeDestory::init()
{
	BaseMode::init();
	m_PlayerOccupyTags = 0;
	m_NonPlayerOccupyTags = 0;
	mOneOccupy = false;
	mOwnCamp = ECAMP_TYPE_Invalid;
	getBuildListSize();
	getFormationListSize();
	return true;
}

void ModeDestory::onReceiveMassage(GameMessage * message)
{
	BaseMode::onReceiveMassage(message);
	if (message->getMessageType() == EMT_FIGHT)
	{
		getFormationListSize();
	}
	else if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		getBuildListSize();
		getTargetBuildsSize();
	}
	if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		MessageBuildDestroy* pdestroy = dynamic_cast<MessageBuildDestroy*>(message);
		if (pdestroy)
		{
			// 一方建筑和兵力都被干掉了
			getBuildListSize();
			if (mEneBuildSize <= 0)
				doSuccess();
			else if (mMineBuildSize <= 0)
				doFail();

			// 是否摧毁了标记建筑
			bool bDestoryTagBuild = false;
			std::vector<int>::iterator tt = TagBuildList.begin();
			for (; tt != TagBuildList.end(); ++tt)
			{
				if (*tt == pdestroy->getKeyID())
					bDestoryTagBuild = true;
			}

			if (bDestoryTagBuild == false)
				return;

			// 所有标记建筑是否被一方占领
			bool oneOccupy = true;
			eCampType ownCamp = ECAMP_TYPE_Invalid;
			std::vector<int>::iterator it = TagBuildList.begin();
			for (; it != TagBuildList.end(); ++it)
			{
				BaseBuild* pBuild = sgBattleControl()->getTargetBuild(*it);
				if (!pBuild)
					continue;

				if (ownCamp == ECAMP_TYPE_Invalid)
				{
					ownCamp = pBuild->getCamp();
					continue;
				}

				if (pBuild->getCamp() != ownCamp)
				{
					oneOccupy = false;
					break;
				}
			}

			if (oneOccupy == true)
			{
				mOwnCamp = ownCamp;
				mOneOccupy = true;
				if (ownCamp == sgBattleControl()->getOwnCamp())
					doSuccess();
				else
					doFail();
			}
		}
	}
}

void ModeDestory::getTargetBuildsSize()
{
	const MapBulidList& buildlist = sgBattleControl()->getBuildList();

	MapBulidList::const_iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		int insKey = it->second->getBuildInfo().insID;

		if (sgTableManager()->getBuildIns(insKey)->isModesTag == 1)
		{
			TagBuildList.push_back(it->second->getKeyID());
		}
	}

}

bool ModeDestory::doPvpFail()
{
	if (!BaseMode::doPvpFail())
	{
		if (mOwnCamp != ECAMP_TYPE_Invalid)
		{
			int uid = sgPvpControl()->getWinUidByCamp(mOwnCamp);
			if (uid != INVALID_CAMP_UIN)
			{
				sgPvpControl()->getPvPProxy()->sendEnd(uid);
				clearData();
				return true;
			}
		}
	}
	return false;
}

bool ModeDestory::doPvpSuccess()
{
	return BaseMode::doPvpSuccess();

}

bool ModeOccupy::init()
{
	BaseMode::init();
	mOneOccupy = false;
	mOwnCamp = ECAMP_TYPE_Invalid;
	getTargetBuildsSize();
	return true;
}

void ModeOccupy::onReceiveMassage(GameMessage * message)
{
	BaseMode::onReceiveMassage(message);

	if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		MessageBuildDestroy* pdestroy = dynamic_cast<MessageBuildDestroy*>(message);
		if (pdestroy)
		{
			// 一方建筑和兵力都被干掉了
			getBuildListSize();
			if (mEneBuildSize <= 0)
				doSuccess();
			else if (mMineBuildSize <= 0)
				doFail();

			// 是否摧毁了标记建筑
			bool bDestoryTagBuild = false;
			std::vector<int>::iterator tt = TagBuildList.begin();
			for (; tt != TagBuildList.end(); ++tt)
			{
				if (*tt == pdestroy->getKeyID())
					bDestoryTagBuild = true;
			}

			if (bDestoryTagBuild == false)
				return;

			// 所有标记建筑是否被一方占领
			bool oneOccupy = true;
			eCampType ownCamp = ECAMP_TYPE_Invalid;
			std::vector<int>::iterator it = TagBuildList.begin();
			for (; it != TagBuildList.end(); ++it)
			{
				BaseBuild* pBuild = sgBattleControl()->getTargetBuild(*it);
				if (!pBuild)
					continue;

				if (ownCamp == ECAMP_TYPE_Invalid)
				{
					ownCamp = pBuild->getCamp();
					continue;
				}

				if (pBuild->getCamp() != ownCamp)
				{
					oneOccupy = false;
					break;
				}
			}

			if (oneOccupy == true)
			{
				mOwnCamp = ownCamp;
				mOneOccupy = true;
				if (ownCamp == sgBattleControl()->getOwnCamp())
					doSuccess();
				else
					doFail();
			}
		}
	} 
}

void ModeOccupy::getTargetBuildsSize()
{
	const MapBulidList& buildlist = sgBattleControl()->getBuildList();

	MapBulidList::const_iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		int insKey = it->second->getBuildInfo().insID;

		if (sgTableManager()->getBuildIns(insKey)->isModesTag == 1)
		{
			TagBuildList.push_back(it->second->getKeyID());
		}
	}
}

bool ModeOccupy::doPvpFail()
{
	if (!BaseMode::doPvpFail())
	{
			if (mOwnCamp != ECAMP_TYPE_Invalid)
			{
				int uid = sgPvpControl()->getWinUidByCamp(mOwnCamp);
				if (uid != INVALID_CAMP_UIN)
				{
					clearData();
					sgPvpControl()->getPvPProxy()->sendEnd(uid);
					return true;
				}
			}
	}
	return false;
}

bool ModeOccupy::doPvpSuccess()
{
	return BaseMode::doPvpSuccess();
}

bool ModeSurvival::init()
{
	BaseMode::init();
	getBuildListSize();

	return true;
}

void ModeSurvival::update()
{
	if (sgTimeCalculator()->hasKey("cp_time"))
	{
		float _left = sgTimeCalculator()->getTimeLeft("cp_time");
		if (_left <= 0.0f)
		{
			doSuccess();
		}
	}
}

void ModeSurvival::onReceiveMassage(GameMessage * message)
{
	BaseMode::onReceiveMassage(message);

	if (message->getMessageType() == EMT_FIGHT)
	{
		getFormationListSize();
	}
	else if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		getBuildListSize();
	} 

	// 胜负判定
	if (mEneBuildSize <= 0 && mEneFormationSize <= 0)
		doSuccess();
	else if (mMineBuildSize <= 0 && mMineFormationSize <= 0)
		doFail();
}

bool ModeResoure::init()
{
	BaseMode::init();
	ScoreBuildList.clear();
	getTargetBuildsSize();

	return true;
}

void ModeResoure::update()
{
	if (ScoreBuildList.empty())
		return;

	// 遍历所有的积分建筑
	std::map<int, std::string>::iterator it = ScoreBuildList.begin();
	for (; it != ScoreBuildList.end(); ++it)
	{
		bool bHasKey = sgTimeCalculator()->hasKey(it->second);
		if (!bHasKey)
			continue;

		float fLeftTime = sgTimeCalculator()->getTimeLeft(it->second);
		if (fLeftTime <= 0.0f)
		{
			eCampType eBuildCamp = sgBattleControl()->getTargetBuild(it->first)->getCamp();
			if (eBuildCamp == ECAMP_TYPE_Neutral)
				continue;

			Camp* pTarCamp = sgBattleControl()->getTargetCamp(eBuildCamp);
			float curScore = pTarCamp->getResScore();
			// 每个定时器结束，增加阵营1点积分
			pTarCamp->setResScore(curScore + 1);
			sgTimeCalculator()->resetTimeCalcultor(it->second);

			sgMessageManager()->sendMessage(EMT_RESMODE_SCORE, "");

			// 某个阵营的积分达到300，结束关卡
			if (pTarCamp->getResScore() >= 300.0f)
			{
				doEndMode(eBuildCamp);
				return;
			}
		}
	}
}

void ModeResoure::onReceiveMassage(GameMessage * message)
{
	BaseMode::onReceiveMassage(message);
	if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		MessageBuildDestroy* pdestroy = dynamic_cast<MessageBuildDestroy*>(message);
		if (pdestroy)
		{
			getBuildListSize();
			if (mMineBuildSize <= 0)
			{
				doEndMode(ECAMP_TYPE_Invalid);
				
				// 主机不return，会继续以主机为基准同步其他客户端
				if (!sgUserObj()->getIsHost())
					return;
			}
			
			// 只有初始阵营为中立的建筑需要创建定时器
			if (pdestroy->getInitCamp() != ECAMP_TYPE_Neutral)
				return;

			// 创建该建筑的积分定时器
			int keyID = pdestroy->getKeyID();
			std::map<int, std::string>::iterator it = ScoreBuildList.find(keyID);
			if (it == ScoreBuildList.end())
				return;

			BaseBuild* pDestroyBuild = sgBattleControl()->getTargetBuild(keyID);
			if (!pDestroyBuild)
				return;

			int insID = pDestroyBuild->getBuildInfo().insID;
			const BuildIns* pIns = sgTableManager()->getBuildIns(insID);
			if (!pIns || pIns->isModesTag == 0)
				return;

			sgTimeCalculator()->createTimeCalcultor(it->second, pIns->scoreSpeed);
		}
	} 
}

void ModeResoure::getTargetBuildsSize()
{
	const MapBulidList& buildlist = sgBattleControl()->getBuildList();

	MapBulidList::const_iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		int insKey = it->second->getBuildInfo().insID;
		const BuildIns* pIns = sgTableManager()->getBuildIns(insKey);

		if (pIns && pIns->isModesTag == 1)
		{
			std::string szKey = CCString::createWithFormat("score_%d", it->second->getKeyID())->getCString();
			// 建筑属于有效阵营，开始积分定时器
			eCampType eBuildCamp = it->second->getBuildInfo().camp;
			if (eBuildCamp != ECAMP_TYPE_Neutral && eBuildCamp < ECAMP_TYPE_Creeps)
			{
				sgTimeCalculator()->createTimeCalcultor(szKey, pIns->scoreSpeed);
			}

			ScoreBuildList.insert(std::make_pair(it->second->getKeyID(), szKey));
		}
	}	
} 

void ModeResoure::doFail()
{
	// pvp: 失败统一弹 提示框（是否继续观看比赛 or 退出游戏-重选主机）
	// pve: AI失败，不做处理

	BaseMode::doFail();

	if (sgBattleControl()->getIsPvp())
	{
		// 提示, 如主机选择退出，重选主机
		// sgUserObj()->getIsHost()
		PageFunction::Get()->gotoFunction(MSG_FUC_FAIL_PROMPT);
 	}
}

void ModeResoure::doEndMode(eCampType eSucc)
{

	eCampType eOwnType = sgBattleControl()->getOwnCamp();
	if (eOwnType != eSucc || eSucc == ECAMP_TYPE_Invalid)
	{
		doFail();
	}
	else
	{
		doSuccess();
	}
}



bool ModeGuard::init()
{
	BaseMode::init();
	getTargetBuildsSize();
	return true;
}

void ModeGuard::update()
{
	if (hasTimeLimit())
	{
		float _left = getTimeLeft();
		if (_left <= 0.0f)
		{
			doSuccess();
		}
	}
}

void ModeGuard::onReceiveMassage(GameMessage * message)
{
	BaseMode::onReceiveMassage(message);
	if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		MessageBuildDestroy* pdestroy = dynamic_cast<MessageBuildDestroy*>(message);
		if (pdestroy)
		{
			// 一方建筑和兵力都被干掉了
			getBuildListSize();
			if (mEneBuildSize <= 0)
				doSuccess();
			else if (mMineBuildSize <= 0)
				doFail();

			// 标记建筑同时全部被（任意方）占领，判定失败
			bool bAllOccupy = false;
			std::vector<int>::iterator tt = TagBuildList.begin();
			for (; tt != TagBuildList.end(); ++tt)
			{
				int key = *tt;
				eCampType eBuildCamp = sgBattleControl()->getTargetBuild(key)->getCamp();
				if (eBuildCamp == sgBattleControl()->getOwnCamp())
					break;

				bAllOccupy = true;
			}

			if (bAllOccupy)
				doFail();
		}
	} 
}

void ModeGuard::getTargetBuildsSize()
{
	const MapBulidList& buildlist = sgBattleControl()->getBuildList();

	MapBulidList::const_iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		int insKey = it->second->getBuildInfo().insID;

		if (sgTableManager()->getBuildIns(insKey)->isGuardTag == 1)
		{
			TagBuildList.push_back(it->second->getKeyID());
		}
	}
}
