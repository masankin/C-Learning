#include <sstream>

#include "GamePrecedure.h"
#include "BalanceControl.h"
#include "data/TableManager.h"

#include "TimeCalculator.h"
#include "balance/BalanceModes.h"
#include "stl_helper.h"
#include "MessageManager.h"
#include "battle/BattleControl.h"
#include "balance/StarModes.h"
#include "HelperFunc.h"
#include "pvp/PvpControl.h"
#include "net/UserObj.h"
#include "EnvSet.h"
#include "sync/SyncControl.h"
#include "guide/GuideManager.h"
#include "battle/BattleRecordsManager.h"

BalanceControl::BalanceControl() :mCurMode(0), mStartTime(0), mCurCheckPoint(0), mCurSuccess(false)
{

	mModeList.insert(std::make_pair(BALANCE_TAG_DESTORY, new ModeDestory()));
	mModeList.insert(std::make_pair(BALANCE_TAG_OCCUPY, new ModeOccupy()));
	mModeList.insert(std::make_pair(BALANCE_TAG_SURVIVAL, new ModeSurvival()));
	mModeList.insert(std::make_pair(BALANCE_TAG_RESOURCE, new ModeResoure()));
	mModeList.insert(std::make_pair(BALANCE_TAG_GUARD, new ModeGuard()));

	mStarFactoryList.insert(std::make_pair(STAR_TAG_SUCCESS_IN_N, new StarSuccessInNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_ENE_BUILD_L_N, new EnemyBuildsLNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_BUILD_NODESTROY, new StarBuildNoDestroyFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_NEUTRAL_N, new StarLastNeutralBuildsFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_NO_PEOP_MN, new StarNoPeopleLNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_TAR_BUILD, new StarGetTarBuildsFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_MORE_ENE_N, new StarOwnPeopleMEneNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_PEOPLE_L_N, new StarOwnPeopleLNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_PEOPLE_M_N, new StarOwnPeopleMNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_ENE_PEOPLE_L_N, new StarEnePeopleLNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_ENE_PEOPLE_M_N, new StarEnePeopleMNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_BUILD_LOST, new StarIniBuildLostLNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_MY_BUILD_M_N, new StarOwnBuildMNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_SUCC_LAST_MN, new StarSuccLastPeopleMNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_MONSTER_TIMES, new StarMonsterMNFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_BUILD_UPGRADE, new StarNoBuildUpgradeFactory()));
	mStarFactoryList.insert(std::make_pair(STAR_TAG_OCampNoDestroy_GetTar, new StarNoDestroyGetTarFactory()));

	mScoreList.insert(std::make_pair(SCORE_TIME, 0));
	mScoreList.insert(std::make_pair(SCORE_PEOPLE, 0));
	mScoreList.insert(std::make_pair(SCORE_BOMB, 0));
	mScoreList.insert(std::make_pair(SCORE_FREEZE, 0));
	mScoreList.insert(std::make_pair(SCORE_EXCHANGE, 0));
	mScoreList.insert(std::make_pair(SCORE_INVISIBILITY, 0));
	mScoreList.insert(std::make_pair(SCORE_SPEED, 0));

	sgMessageManager()->regisiterMessageHandler(EMT_CHECKPOINT_END, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_FULLPEOP, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_COOKING, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_FREEZE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_CHANGE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_INVISIBILITY, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_Oil, this);
}

BalanceControl::~BalanceControl()
{
	SAFE_RELEASE_MAP(mModeList);
	SAFE_RELEASE_MAP(mStarFactoryList);

	sgMessageManager()->removeMessageHandler(this);
}

void BalanceControl::update()
{
	if (mCurMode)
		mCurMode->update();


	// 评星规则更新检测
	if (!sgBattleControl()->getIsPvp())
	{
		StarList::iterator it = mCPStarList.begin();
		for (; it != mCPStarList.end(); ++it)
		{
			it->second->update();
		}
	}

}

void BalanceControl::startCheckPoint(int cp)
{

	mStartTime = sgTimeCalculator()->getCurrentTime();
	mCurCheckPoint = cp;

	const CheckPoint* pCP = sgTableManager()->getCheckPointByID(cp);
	if (!pCP)
	{
		CCAssert(0, "[fault]BalanceControl::startCheckPoint table data error!");
		return;
	}

	if (!mCurMode)
	{
		BalanceModeList::iterator it = mModeList.find((eBalanceTag)pCP->checkType);
		if (it != mModeList.end())
		{
			sgPvpControl()->setPvpTestStr("enter checkpoint init mode");

			mCurMode = it->second;
			mCurMode->start(cp);

			CCLOG("[trace]BalanceControl::startCheckPoint new checkpoint:%d checkType:%d!", cp, pCP->checkType);
		}
		else{
			CCLOG("[error]BalanceControl::startCheckPoint ERROR checkType:%d!", pCP->checkType);
		}
	}
	else{
		CCLOG("[trace]BalanceControl::startCheckPoint mCurMode==NULL!");
	}
	 
	if (!sgBattleControl()->getIsPvp())
		initCheckPtStarList(cp);
	else
	{
		SAFE_RELEASE_MAP(mCPStarList);
	}
 
	char params[128] = { 0, };
	sprintf(params, "%d", cp);
	sgMessageManager()->sendMessage(EMT_CHECKPOINT_START, params);
}

void BalanceControl::doCheckPtEnd(bool bSuccess, bool bReturn /* = false */)
{
	CCLOG("[trace] BalanceControl::doCheckPtEnd success:%d!", bSuccess == true ? 1 : 0);

	mCurSuccess = bSuccess;


	if (!sgBattleControl()->getIsPvp())
	{
		// PVE结算

		char params[128] = { 0, };

		// 失败时直接清理所有评星，胜利时发起评星命令
		if (!bSuccess)
		{
			doCheckPtFail();
		}
		else{

			sprintf(params, "%d", bSuccess == true ? 1 : 0);
			sgMessageManager()->sendMessage(EMT_SUCC_CHECKSTAR, params);
		}

		sgGamePrecedure()->setEnterGame(false);

	}



	if (mCurMode)
	{
		char params[128] = { 0, };

		std::string reason;
		if (!bSuccess)
		{
			if (bReturn)
				reason = "return";
			else
				reason = "Unknown";
		}

		memset(params, 0, sizeof(params));
		sprintf(params, "%d %d %s", mCurMode->getCheckPointIndex(), bSuccess == true ? 1 : 0, reason.c_str());
		sgMessageManager()->sendMessage(EMT_CHECKPOINT_END, params);
	}


	const CheckPoint* pCheck = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);

	auto pInfo = sgTableManager()->getScoreInfo(eBalanceScore::SCORE_TIME);
	if (!bReturn && pInfo && mCurSuccess && pCheck)
	{
		float fGameTime = sgBalanceControl()->getTimeLeft();
		int score = pInfo->useScore * (600.0f / pCheck->timeLimit) * fGameTime;
		addScore(eBalanceScore::SCORE_TIME, score);
	}
}
 

void BalanceControl::initCheckPtStarList(int cp)
{
	SAFE_RELEASE_MAP(mCPStarList);

	const CheckPoint* pCP = sgTableManager()->getCheckPointByID(cp);
	if (!pCP)
	{
		CCAssert(0, "[fault]BalanceControl::initCheckPtStarList table data error!");
		return;
	}

	do 
	{
		std::vector<std::string> starList;

		if (!pCP->star1.empty())
			starList.push_back(pCP->star1);

		if (!pCP->star2.empty())
			starList.push_back(pCP->star2);

		if (!pCP->star3.empty())
			starList.push_back(pCP->star3);

		std::vector<std::string>::iterator it = starList.begin();
		for (; it != starList.end(); ++it)
		{
			std::string szTemp = *it;
			HelperFunc::replaceAllCharacters(szTemp, "#", "\t");

			std::stringstream ssTemp(szTemp);

			int starid;
			//std::string params;

			ssTemp >> starid;// >> params;
			StarFactoryList::iterator it = mStarFactoryList.find(starid);
			if (it != mStarFactoryList.end())
			{
				BaseStar* pStarMode = it->second->create(ssTemp);
				if (pStarMode)
				{
					mCPStarList.insert(std::make_pair(starid, pStarMode));
				}
			}
		}
	} while (0);
}

int BalanceControl::getStarNum()
{
	int iRet = 0;

	StarList::iterator it = mCPStarList.begin();
	for (; it != mCPStarList.end(); ++it)
	{
		if (it->second->getEndTag() == 1)
			iRet++;
	}
	return iRet;
}

const ::BalanceControl::StarList& BalanceControl::getStarList()
{
	return mCPStarList;
}

void BalanceControl::doCheckPtFail()
{
	StarList::iterator it = mCPStarList.begin();
	for (; it != mCPStarList.end(); ++it)
	{
		it->second->setEndTag(0);
	}
}

float BalanceControl::getTimeLeft() const
{
	if (mCurMode)
		return mCurMode->getTimeLeft();
	else
		return -1;
}

void BalanceControl::clean()
{
	// 清空通关模式数据
	mCurMode = nullptr;
	mStartTime = 0;

	BalanceModeList::iterator it = mModeList.begin();
	for (; it != mModeList.end(); ++it)
	{
		it->second->clearData();
	}

	ScoreList::iterator sIt = mScoreList.begin();
	for (; sIt != mScoreList.end(); ++sIt)
	{
		sIt->second = 0;
	}

	// 清空评星数据
	SAFE_RELEASE_MAP(mCPStarList);
}

void BalanceControl::doSaveStars()
{
	if (!mCurSuccess && sgBattleControl()->getIsPvp() == false)
	{
		int pTime = sgTimeCalculator()->getCurrentTime() - mStartTime;

		int cp = sgGamePrecedure()->getCurMapInfo().checkpt;
		sgUserObj()->updateCheckPoint(cp, -1, pTime / 1000);
		return;
	}
	if (!mCurSuccess || sgBattleControl()->getIsPvp())
		return;


	int pTime = sgTimeCalculator()->getCurrentTime() - mStartTime;
	//sgUserObj()->getSyncCheckPointData()->addCheckPointData(mCurCheckPoint,mCurSuccess==true?getStarNum():-1,pTime/1000);
	

	int stars = sgBalanceControl()->getStarNum();
	int cp = sgGamePrecedure()->getCurMapInfo().checkpt;
	const CheckPoint* pCP = sgTableManager()->getCheckPointByID(cp);
	if (!pCP)
		return;
	 
	sgUserObj()->updateCheckPoint(cp, stars, pTime / 1000);
	// 开启后续关卡
	/*int curOpen = sgUserControl()->getMaxOpenCheckPoint();
	if(cp > curOpen)
	{
	sgUserObj()->setMaxOpenCheckPoint(cp);
	}*/

	//sgSyncControl()->addSyncCheckPointData(cp, stars, pTime/1000);

	/*int allStar = sgUserObj()->getTotalStars(); 
	std::vector<int> openList = sgTableManager()->getNextCheckPoints(pCP->id, allStar);
	if (openList.size()>0)
	{
		int pOpenCheckPoint = 0;
		for (unsigned int i = 0; i < openList.size(); i++)
		{
			int checkPoint = openList.at(i);
			if (checkPoint > pOpenCheckPoint && checkPoint/1000<3)
			{
				pOpenCheckPoint = checkPoint;
			}
		}
		if (sgUserObj()->getCheckPointState(pOpenCheckPoint) == E_CHECKPOINTSTATE_CLOSE)
		{
			sgUserObj()->updateCheckPoint(cp, stars,pTime/1000, pOpenCheckPoint);
			return;
		}
	}
	
	int oldStars = sgUserObj()->getCheckPointStar(cp);
	//if(oldStars < stars || sgUserObj()->getCheckPointState(cp) == E_CHECKPOINTSTATE_OPEN)
	{
		//更新关卡为通关状态
		sgUserObj()->updateCheckPoint(cp, stars,pTime/1000); 
	}*/
}

void BalanceControl::onReceiveMassage(GameMessage * message)
{

#define ADDSCORE(_type) \
	auto pInfo = sgTableManager()->getScoreInfo(_type);\
	if (pInfo)\
	{\
		addScore(_type, pInfo->useScore); \
	}

	// 自己抛出EMT_CHECKPOINT_END事件，响应时存储评星。
	if (message->getMessageType() == EMT_CHECKPOINT_END)
	{
		MessageCheckPointEnd* pMes = dynamic_cast<MessageCheckPointEnd*>(message);
		if (pMes)
		{
			if (!sgBattleRecordsManager()->getIsPlayback() && pMes->mReason != "return")
			{
				doSaveStars();
			}
		}
	}
	else if (message->getMessageType() == EMT_PROP_FULLPEOP)
	{
		MessageFullPeopel* pMes = dynamic_cast<MessageFullPeopel*>(message);
		if (pMes && pMes->getCamp() == sgBattleControl()->getOwnCamp())
		{
			ADDSCORE(SCORE_PEOPLE)
		}
	}
	else if (message->getMessageType() == EMT_PROP_COOKING)
	{
		MessageCooking* pMes = dynamic_cast<MessageCooking*>(message);
		if (pMes && pMes->camp == sgBattleControl()->getOwnCamp())
		{
			ADDSCORE(SCORE_BOMB)
		}
	}
	else if (message->getMessageType() == EMT_PROP_FREEZE)
	{
		MessageFreeze* pMes = dynamic_cast<MessageFreeze*>(message);
		if (pMes && pMes->camp == sgBattleControl()->getOwnCamp())
		{
			ADDSCORE(SCORE_FREEZE)
		}
		
	}
	else if (message->getMessageType() == EMT_PROP_CHANGE)
	{
		MessageBuildChange* pMes = dynamic_cast<MessageBuildChange*>(message);
		if (pMes && pMes->getCamp() == sgBattleControl()->getOwnCamp())
		{
			ADDSCORE(SCORE_EXCHANGE)
		}
	}
	else if (message->getMessageType() == EMT_PROP_INVISIBILITY)
	{
		MessageInvisibility* pMes = dynamic_cast<MessageInvisibility*>(message);
		if (pMes && pMes->camp == sgBattleControl()->getOwnCamp())
		{
			ADDSCORE(SCORE_INVISIBILITY)
		}
	}
	else if (message->getMessageType() == EMT_PROP_Oil)
	{
		MessageOil* pMes = dynamic_cast<MessageOil*>(message);
		if (pMes && pMes->camp == sgBattleControl()->getOwnCamp())
		{
			ADDSCORE(SCORE_SPEED)
		}
	}

	

}

BaseStar* BalanceControl::getTargetStar(int tag)
{
	StarList::iterator it = mCPStarList.find(tag);
	if (it != mCPStarList.end())
	{
		return it->second;
	}
	return NULL;
}

void BalanceControl::starGameTime()
{
	if (mCurMode)
	{
		mCurMode->starGameTime();
	}
	StarList::iterator it = mCPStarList.begin();
	for (; it != mCPStarList.end(); ++it)
	{
		it->second->startGameTime();
	}
	mStartTime = sgTimeCalculator()->getCurrentTime();
}

void BalanceControl::addScore(eBalanceScore type, int scroe)
{
	ScoreList::iterator it = mScoreList.find(type);
	if (it != mScoreList.end())
	{
		it->second += scroe;
	}
}

int BalanceControl::getGameScore()
{
	int scroe = 0;
	ScoreList::iterator it = mScoreList.begin();
	for (; it != mScoreList.end();it++)
	{
		scroe += it->second;
	}

	return scroe;
}

int BalanceControl::getScore(eBalanceScore type)
{
	ScoreList::iterator it = mScoreList.find(type);
	if (it != mScoreList.end())
	{
		return it->second;
	}

	return 0;
}

int BalanceControl::getTimeScore()
{
	int scroe = getScore(eBalanceScore::SCORE_TIME);
	return scroe;
}

int BalanceControl::getPropScore()
{
	int scroe = 0;
	ScoreList::iterator it = mScoreList.begin();
	for (; it != mScoreList.end(); it++)
	{
		if (it->first != eBalanceScore::SCORE_TIME)
		{
			scroe += it->second;
		}
	}
	return scroe;
}

long long BalanceControl::getBattleTime()
{
	return sgTimeCalculator()->getCurrentTime() - mStartTime;
}
