#include "TableManager.h"
#include "game/battle/BattleControl.h"
#include "utils/Comm.h"
#include "EnvSet.h"
#include <map>
#include "MessageManager.h"
#include "net/UserObj.h"

class CsvLoadTask : public ThreadTask
{
public:

	CsvLoadTask() : mTargetTable(NULL){}

	virtual int run()
	{
		//sgTableManager()->lockTask();

		if (!mTargetTable)
			return -1;

		mTargetTable->init();
		//sgTableManager()->unlockTask();
		sgTableManager()->setLoadState(LS_COMPLETE);
		return 0;
	}

	TableReader* mTargetTable;
};

TableManager::TableManager() : mCurrentTask(NULL), mLoadState(LS_COMPLETE)
{
	mGuideDataTable = NULL; 
	//mMusicTable = new MusicTable();
}

TableManager::~TableManager()
{
	
}

void TableManager::loadAllTables()
{
	queueAllTables();
}

void TableManager::addCsvLoadTask(TableReader* pTargetTable)
{
	if (!pTargetTable)
		return;

	CsvLoadTask* pCsvTask = new CsvLoadTask();
	pCsvTask->mTargetTable = pTargetTable;

	mTaskList.push_back(pCsvTask);
}

void TableManager::update(float delta)
{
	//asyncLoadCsv();
	syncLoadCsv();
} 


void TableManager::queueAllTables()
{
#define ADD_CSV_TASK(tar_table, tab_type) \
	tar_table = new tab_type(); \
	addCsvLoadTask(tar_table);

	// 所有表数据压入到队列(表构造时不做解析操作！！！)
	//ADD_CSV_TASK(mResPathTable, RecourcePath);
	ADD_CSV_TASK(mBuildAttrTable, BuildAttrTable);
	ADD_CSV_TASK(mBuildInsTable, BuildInsTable);
	ADD_CSV_TASK(mCheckPointTable, CheckPointTable);
	//ADD_CSV_TASK(mMusicTable, MusicTable);
	ADD_CSV_TASK(mStateActionTable, StateActionTable);
	//ADD_CSV_TASK(mTextHelperTable, TextHelperTable);
	ADD_CSV_TASK(mInsistuteAddAttrTable, InsistuteAddAttrTable);
	ADD_CSV_TASK(mPropTable, PropTable);
	//ADD_CSV_TASK(mGuideDataTable, GuideDataTable);
	ADD_CSV_TASK(mAchievementAwardTable, AchievementAwardTable);
	ADD_CSV_TASK(mGiftTable, GiftTable);
	ADD_CSV_TASK(mAchievePointAwardTable, AchievePointAwardTable);
	ADD_CSV_TASK(mJungleDataTable, JungleDataTable);
	ADD_CSV_TASK(mMoralDefenceDataTable, MoraleDefenceDataTable);
	ADD_CSV_TASK(mMoralBuffDataTable, MoraleBuffDataTable);
	ADD_CSV_TASK(mBuffDataTable, BuffDataTable);
	ADD_CSV_TASK(mWinnerDataTable, WinnerDataTable);
	ADD_CSV_TASK(mShopTable, ShopTable);
	ADD_CSV_TASK(mItemBuyTable, ItemBuyTable);
	//ADD_CSV_TASK(mRechargeActivityTable, RechargeActivityTable);
	mRechargeActivityTable = new RechargeActivityTable();
	ADD_CSV_TASK(mPowerBuyTable, PowerBuyTable);
	ADD_CSV_TASK(mDayTaskTable, DayTaskTable);
	ADD_CSV_TASK(mNewPlayGiftTable, NewPlayGiftTable);
	ADD_CSV_TASK(mNoticeDataTable, NoticeDataTable);
	ADD_CSV_TASK(mCreepsPointTable, CreepsPointTable);
	ADD_CSV_TASK(mCreepsTable, CreepsTable);
	ADD_CSV_TASK(mActLoginTabel, ActLoginTable);
	ADD_CSV_TASK(mPvpChatTable,PvpChatTable);
	//ADD_CSV_TASK(mPlatformTable, PlatformTable);
	ADD_CSV_TASK(mErrorCodeTable, ErrorCodeTable);
	ADD_CSV_TASK(mPayLimitTable, PayLimitTable);
	ADD_CSV_TASK(mBibleTable, BibleTable);
	ADD_CSV_TASK(mCrazyWeekTable, CrazyWeekTable);
	ADD_CSV_TASK(mExchangeItemTable, ExchangeItemTable);
	ADD_CSV_TASK(mGradeDataTable, GradeDataTable);
	ADD_CSV_TASK(mActInformTable, ActInformTable);

	ADD_CSV_TASK(mVipDataTable, VipDataTable);
	ADD_CSV_TASK(mLevelTable, LevelTable);
	ADD_CSV_TASK(mGlyphStuffTable, GlyphStuffTable);

	ADD_CSV_TASK(mPlayerInfoTable, PlayerInfoTable);

	ADD_CSV_TASK(mIntelligentTable, IntelligentTable);
	ADD_CSV_TASK(mBadgeDataTable, BadgeDataTable);
	ADD_CSV_TASK(mGoodsInfoTable, GoodsInfoTable);
	ADD_CSV_TASK(mNameAreaTable, NameAreaTable);
	ADD_CSV_TASK(mScoreInfoTable, ScoreInfoTable);
	ADD_CSV_TASK(mSevenDayMissionTable, SevenDayMissionTable);


	Director::getInstance()->getScheduler()->scheduleUpdate(this, -1, false);
}

void TableManager::unloadAllTables()
{
	//CC_SAFE_DELETE(mResPathTable);
	CC_SAFE_DELETE(mBuildAttrTable);
	CC_SAFE_DELETE(mBuildInsTable);
	CC_SAFE_DELETE(mCheckPointTable);
	//CC_SAFE_DELETE(mMusicTable);
	CC_SAFE_DELETE(mStateActionTable);
	//CC_SAFE_DELETE(mTextHelperTable);
	CC_SAFE_DELETE(mInsistuteAddAttrTable);
	CC_SAFE_DELETE(mPropTable);
	//CC_SAFE_DELETE(mGuideDataTable);
	CC_SAFE_DELETE(mAchievementAwardTable);
	CC_SAFE_DELETE(mGiftTable);
	CC_SAFE_DELETE(mAchievePointAwardTable);
	CC_SAFE_DELETE(mJungleDataTable);
	CC_SAFE_DELETE(mMoralDefenceDataTable);
	CC_SAFE_DELETE(mMoralBuffDataTable);
	CC_SAFE_DELETE(mBuffDataTable);
	CC_SAFE_DELETE(mWinnerDataTable);
	CC_SAFE_DELETE(mShopTable);
	CC_SAFE_DELETE(mItemBuyTable);
	CC_SAFE_DELETE(mRechargeActivityTable);
	CC_SAFE_DELETE(mPowerBuyTable);
	CC_SAFE_DELETE(mDayTaskTable);
	CC_SAFE_DELETE(mNewPlayGiftTable);
	CC_SAFE_DELETE(mNoticeDataTable);
	CC_SAFE_DELETE(mCreepsPointTable);
	CC_SAFE_DELETE(mCreepsTable);
	CC_SAFE_DELETE(mActLoginTabel);
	CC_SAFE_DELETE(mPvpChatTable);
	CC_SAFE_DELETE(mPlatformTable);
	CC_SAFE_DELETE(mErrorCodeTable);
	CC_SAFE_DELETE(mPayLimitTable);
	CC_SAFE_DELETE(mBibleTable);
	CC_SAFE_DELETE(mCrazyWeekTable);
	CC_SAFE_DELETE(mExchangeItemTable);
	CC_SAFE_DELETE(mGradeDataTable);
	CC_SAFE_DELETE(mActInformTable);
	CC_SAFE_DELETE(mVipDataTable);
	CC_SAFE_DELETE(mLevelTable);
	CC_SAFE_DELETE(mPlayerInfoTable);
	CC_SAFE_DELETE(mIntelligentTable);
	CC_SAFE_DELETE(mBadgeDataTable);
	CC_SAFE_DELETE(mGoodsInfoTable);
	CC_SAFE_DELETE(mNameAreaTable);
	CC_SAFE_DELETE(mScoreInfoTable);
	CC_SAFE_DELETE(mSevenDayMissionTable);
}

const BuildAttr* TableManager::getBuildAttr(int id)
{
	const BuildAttr* pBuildAttr = mBuildAttrTable->getBuildAttrByID(id);
	if (!pBuildAttr)
	{
		CCLOG("[error] TableManager::getBuildAttr not find build:%d!", id);
		return NULL;
	}
	return  pBuildAttr;
}

const BuildIns* TableManager::getBuildIns(int id)
{
	const BuildIns* pBuildIns = mBuildInsTable->getBuildInsByID(id);
	if (!pBuildIns)
	{
		CCLOG("[error] TableManager::BuildIns not find build:%d!", id);
		return NULL;
	}
	return pBuildIns;
}

const BuildInsVec TableManager::getBuildInsByGateID(int gateID)
{
	return mBuildInsTable->getBuildIndsByGateID(gateID);
}


const CheckPoint* TableManager::getCheckPointByID(int id)
{
	const CheckPoint* pCheckPt = mCheckPointTable->getCheckPointByID(id);
	if (!pCheckPt)
	{
		CCLOG("[error] TableManager::getCheckPointByID not find CheckPoint:%d!", id);
		return NULL;
	}
	return pCheckPt;
}

const CheckPoint* TableManager::getMapCPoint(int type, int mapid)
{
	char szMapName[64] = { 0, };
	sprintf(szMapName, "pvp_%d_%d", type, mapid);

	const CheckPoint* pCheckPt = mCheckPointTable->getCheckPointByMapName(szMapName);
	if (!pCheckPt)
	{
		CCLOG("[error] TableManager::getMapCPoint not find CheckPoint:%s!", szMapName);
		return NULL;
	}
	return pCheckPt;
}

const MusicInfo* TableManager::getMusicInfoByID(int id)
{
	const MusicInfo* pCheckPt = mMusicTable->getMusicInfoByID(id);
	if (!pCheckPt)
	{
		CCLOG("[error] TableManager::getMusicByID not find CheckPoint:%d!", id);
		return NULL;
	}
	return pCheckPt;
}

const StateAction* TableManager::getStateActionByID(int id)
{
	const StateAction* pStateAct = mStateActionTable->getStateActionByID(id);
	if (!pStateAct)
	{
		CCLOG("[error] TableManager::getStateActionByID not find StateAction:%d!", id);
		return NULL;
	}
	return pStateAct;
}

const TextHelper* TableManager::getTextHelperByID(int id)
{
	const TextHelper* pTextHelper = mTextHelperTable->getTextHelperByID(id);
	if (!pTextHelper)
	{
		CCLOG("[error] TableManager::getTextHelperByID not find TextHelper:%d!", id);
		return NULL;
	}
	return pTextHelper;
}

const InsistuteAddAttr* TableManager::getInsistuteAddAttr(int id)	
{
	const InsistuteAddAttr* pData = mInsistuteAddAttrTable->getInsistuteAddAttrByID(id);
	if (!pData)
	{
		//CCLOG("[error] TableManager::getInsistuteAddAttr not find InsistuteAddAttr:%d!", id);
		return NULL;
	}
	return pData;
}

std::string TableManager::getAnimationNameByCamp(eCampType type)
{
	switch (type)
	{
	case ECAMP_TYPE_Neutral:
		return "fla_Fantuan";
		break;
	case ECAMP_TYPE_Player:
		return "fla_baozi";
		break;
	case ECAMP_TYPE_Player1:
		return "pvp_fla_baozi";
		break;
	case ECAMP_TYPE_AI:
		return "fla_zongzi";
		break;
	case ECAMP_TYPE_Player2:
		return "pvp_fla_zongzi";
		break;
	case ECAMP_TYPE_AI2:
		return "fla_hanbao";
		break;
	case ECAMP_TYPE_Player3:
		return "pvp_fla_hanbao";
		break;
	case ECAMP_TYPE_AI3:	
		return "fla_dangao";
		break;
	case ECAMP_TYPE_Player4:
		return "pvp_fla_dangao";
		break;
	case ECAMP_TYPE_Creeps:
		return "fla_lajiao";
		break;	
	case ECAMP_TYPE_Invalid:
		break;
	default:
		return "fla_zongzi";
		break;
	} 
}


std::string TableManager::getFlagNameByCamp(eCampType type)
{
	switch (type)
	{
	case ECAMP_TYPE_Player:
	case ECAMP_TYPE_AI:
	case ECAMP_TYPE_AI2:
	case ECAMP_TYPE_AI3:
	case ECAMP_TYPE_Player1:
		return "fla_QiZhiShengJi";
		break;
	case ECAMP_TYPE_Player2:
		return "fla_QiZhiShengJiLv";
		break;
	case ECAMP_TYPE_Player3:
		return "fla_QiZhiShengJiZi";
		break;
	case ECAMP_TYPE_Player4:
		return "fla_QiZhiShengJiHuang";
		break;
	default:
		return "fla_QiZhiShengJi";
		break;
	}
}

std::string TableManager::getAnimationNameByExpress(eExpressType type)
{
	switch (type)
	{
	case EXP_TYPE_Neutral:
		return "fla_Fantuan";
		break;
	case EXP_TYPE_Player:
		return "fla_baozi";
		break;
	case EXP_TYPE_Player1:
		return "fla_Redbaozi";
		break;
	case EXP_TYPE_AI:
		return "fla_zongzi";
		break;
	case EXP_TYPE_Player2:
		return "fla_Grbaozi";
		break;
	case EXP_TYPE_AI2:
		return "fla_hanbao";
		break;
	case EXP_TYPE_Player3:
		return "fla_Ppbaozi";
		break;
	case EXP_TYPE_AI3:
		return "fla_dangao";
		break;
	case EXP_TYPE_Player4: 
		return "fla_Yebaozi";
		break;
	case EXP_TYPE_LaJiao:
		return "fla_lajiao";
		break;
	case EXP_TYPE_DaCong:
		return "fla_dacong";
		break;
	case EXP_TYPE_JiDan:
		return "fla_jidan";
		break;
	case EXP_TYPE_NaiLao:
		return "fla_nailao";
		break;
	case EXP_TYPE_SuanTou:
		return "fla_suantou";
		break;
	case EXP_TYPE_XiaMi1:
		return "fla_xiami";
		break;
	case EXP_TYPE_Invalid:
		break;
	default:
		break;
	}
}

std::vector<int> TableManager::getNextCheckPoints(int cpCur, int cpStar)
{
	std::vector<int> cps;
	int curOpen = sgUserObj()->getMaxOpenCheckPoint();
	// 获得前置关卡为@cpCur的关卡list
	CheckPointTable::CheckPointListIterator it = mCheckPointTable->getCheckPointIterator();
	while (it.hasMoreElements())
	{
		CheckPoint* pcp = dynamic_cast<CheckPoint*>(it.getNext());
		if (pcp->id!= 1010 && pcp->preCheckPt == 0)
		{
			break;
		}
		if (pcp->id % 10 == 0)
		{
			if (pcp->preCheckPt == cpCur && pcp->preNeedStar <= cpStar)
			{
				cps.push_back(pcp->id);
			}
		}
		else
		{
			if ((pcp->preCheckPt == cpCur || curOpen > pcp->preCheckPt) && pcp->preNeedStar <= cpStar)
			{
				cps.push_back(pcp->id);
			}
		}
	}

	////TODO: 
	//const CheckPoint* pLast = mCheckPointTable->getCheckPointByID(1152);
	//if (pLast && pLast->preNeedStar <= cpStar)
	//{
	//	cps.push_back(pLast->id);
	//}

	return cps;
}


int TableManager::getWolrdAllStar(int wolrd)
{

	int star = 0;

	CheckPointTable::CheckPointListIterator it = mCheckPointTable->getCheckPointIterator();
	while (it.hasMoreElements())
	{
		CheckPoint* pcp = dynamic_cast<CheckPoint*>(it.getNext());
		if (pcp->id /1000 >wolrd)
				break;

		if (pcp->id /1000 < wolrd)
				continue;

		if (pcp)
		{
			if (pcp->star1 != "0")
			{
				star++;
			}
			if (pcp->star2 != "0")
			{
				star++;
			}
			if (pcp->star3 != "0")
			{
				star++;
			}
		}
	}
	return star;
}

int TableManager::getCheckPointStars(int cpCur)
{
	const CheckPoint* pCP = sgTableManager()->getCheckPointByID(cpCur);
	if (!pCP)
		return 0;

	int ret = 0;
	if (pCP->star1 != "0")
		ret++;

	if (pCP->star2 != "0")
		ret++;

	if (pCP->star3 != "0")
		ret++;
	return ret;
}

//const PropInfo* TableManager::getPropInfoByType(ePropType type)
//{
//	int id = 10000 + (int)type;
//	const PropInfo* pCheckPt = mPropTable->getPropInfoByID(id);
//	if (!pCheckPt)
//	{
//		CCLOG("[error] TableManager::getPropInfoID not find PropInfo:%d!", type);
//		return NULL;
//	}
//	return pCheckPt;
//}

const GuideDataVec TableManager::getGuideDataVec()
{
	const GuideDataVec vec;
	if(NULL != mGuideDataTable)
	{
		return mGuideDataTable->getGuideDataVec();
	} 
	//返回空vector
	return vec;
}

const GuideData* TableManager::getGuideDataByID(int guideID)
{
	if(NULL != mGuideDataTable)
	{
		return mGuideDataTable->getGuideDataByID(guideID);
	} 
	return NULL;
}

const AchievementAward* TableManager::getAchievementAwardByID(AchieveCategory category,int id)
{
	return mAchievementAwardTable->getAchievementByKey(category,id);
}
const AchievementAward* TableManager::getAchievementById(int id)
{
	return mAchievementAwardTable->getAchievementById(id);
}

int TableManager::getAchievementAwardCount(AchieveCategory category)
{
	return mAchievementAwardTable->getAchievementCount(category);
}

bool TableManager::isAchieveMentAwardComplent(AchieveModule module, bool isLast /*= false*/)
{
	return mAchievementAwardTable->isAchieveMentAwardComplent(module, isLast);
}

void TableManager::sortAchievementAward(std::map<int,AchievementAward::AchieveInfo>& pStatusMap)
{
	mAchievementAwardTable->sortAchievementAward(pStatusMap);
}

const GiftAward * TableManager::getGiftAward(int id)
{
	return  mGiftTable->getGiftAwardById(id);
}
const AchievePointAward* TableManager::getAchievePointAwardById(int id)
{
	return mAchievePointAwardTable->getAchievePointAward(id);
}
const JungleDataVec TableManager::getJungleDataVec()
{
	return mJungleDataTable->getJungleDataVec();
}

const JungleDataVec TableManager::getJungleDataVecByID(int gateID)
{
	return mJungleDataTable->getJungleDataVecByID(gateID);
}

const JungleData* TableManager::getJungleDataByID(int buildID)
{
	return mJungleDataTable->getJungleDataByID(buildID);
}

const MoraleDefenceDataVec TableManager::getMoraleDefenceDataVec()
{
	return mMoralDefenceDataTable->getMoraleDefenceDataVec();
}

const MoraleDefenceData* TableManager::getMoraleDefenceDataByRange(float moraleValue)
{
	return mMoralDefenceDataTable->getMoraleDefenceDataByRange(moraleValue);
}



const MoraleBuffDataVec TableManager::getMoraleBuffDataVec()
{
	return mMoralBuffDataTable->getMoraleBuffDataVec();
}

const MoraleBuffDataVec TableManager::getMoraleBuffDataVecByRange(float moraleValue)
{
	return mMoralBuffDataTable->getMoraleBuffDataVecByRange(moraleValue);
}

const int TableManager::getMoraleLevel(float moraleValue)
{
	return mMoralBuffDataTable->getMoraleValueLevel(moraleValue);
}

const BuffData* TableManager::getBuffDataByID(int buffID)
{
	return mBuffDataTable->getBuffDataByID(buffID);
}

const WinnerData * TableManager::getWinnerData(int id)
{
	return  mWinnerDataTable->getWinnerData(id);
}
const ActLogin* TableManager::getActLogin(int day)
{
	return mActLoginTabel->getActLogin(day);
}

int TableManager::getWinnerDataNum()
{
	return mWinnerDataTable->getWinnerDataNum();
}

const ShopInfo* TableManager::getShopInfoById(int id)
{

	const ShopInfo* pShopInfo = mShopTable->getShopInfoByID(id);
	if (!pShopInfo)
	{
		CCLOG("[error] TableManager::getShopInfoById not find ShopInfo:%d!", id);
		return NULL;
	}
	return pShopInfo;

}

void  TableManager::updateShopInfo(std::map<int,ShopInfo> shopMap)
{
	mShopTable->updateShopInfo(shopMap);
}

const ItemBuyInfo* TableManager::getItemBuyById(int id)
{
	const ItemBuyInfo* pInfo = mItemBuyTable->getItemBuyInfoByID(id);
	if (!pInfo)
	{
		CCLOG("[error] TableManager::getItemBuyById not find ItemBuyInfo:%d!", id);
		return NULL;
	}
	return pInfo;
}

const PropInfo * TableManager::getPropInfoById(int id)
{
	return mPropTable->getPropInfoByID(id);
}

int TableManager::getPropType(int id)
{
	const PropInfo* pInfo = mPropTable->getPropInfoByID(id);
	if (!pInfo)
		return -1;
	if (pInfo->isPve)
		return 1;
	return 0;
}

int TableManager::getPropIdByType(ePropType type)
{
	auto plist = getPropInfoList();
	int isPve = sgBattleControl()->getIsPvp() ? 0 : 1;

	for (PropList::const_iterator it = plist.begin(); it != plist.end();it++)
	{
		if (it->second->type == type && it->second->isPve == isPve)
		{
			return it->second->id;
		}
	}

	CCLOG("[error] TableManager::getPropIdByType can't find this type :%d!", type);
	return -1;
}

const PropList& TableManager::getPropInfoList(bool display)
{
	if (display)
		return mPropTable->getDisplayPropList();
	return mPropTable->getPropList();
}

RechargeList* TableManager::getRechargeList()
{
	return mRechargeActivityTable->getRechargeList();
}

RechargeData* TableManager::getRechargeData(int id)
{
	return mRechargeActivityTable->getRechargeData(id);
}

PowerBuy* TableManager::getPowerBuy(int id)
{
	return mPowerBuyTable->getPowerBuy(id);
}

DayTaskData* TableManager::getDayTaskDataById(int id)
{
	return mDayTaskTable->getDayTaskById(id);
}

DayTaskDataList* TableManager::getDayTaskList()
{
	return mDayTaskTable->getDayTaskList();
}

NoticeDataVec* TableManager::getNoticeDataVec()
{ 
	return mNoticeDataTable->getNoticeDataVec();
}

NoticeData* TableManager::getNoticeData(int id)
{
	return mNoticeDataTable->getNoticeData(id);
}

void TableManager::loadNoticeTable()
{
	mNoticeDataTable = new NoticeDataTable();
	mNoticeDataTable->init();
}


const CreepsPoint* TableManager::getCreepsPoint(int id)
{
	const CreepsPoint* pRet = mCreepsPointTable->getCreepsPoint(id);
	if (!pRet)
	{
		CCLOG("[error] TableManager::getCreepsPoint not find CreepsPoint:%d!", id);
		return NULL;
	}
	return pRet;
}


const Creeps* TableManager::getCreeps(int id)
{
	const Creeps* pRet = mCreepsTable->getCreeps(id);
	if (!pRet)
	{
		CCLOG("[error] TableManager::getCreeps not find Creeps:%d!", id);
		return NULL;
	}
	return pRet;
}

void TableManager::loadShopTable()
{

	mShopTable = new ShopTable();
	mShopTable->init();
}

void TableManager::loadGuideTable()
{
	mGuideDataTable = new GuideDataTable();
	mGuideDataTable->init();
}

NewPlayGiftList* TableManager::getNewPlayGiftList()
{
	return mNewPlayGiftTable->getNewPlayGiftList();
}

NewPlayGiftData* TableManager::getNewPlayGiftDataByType(int type)
{
	return mNewPlayGiftTable->getNewPlayGiftDataByType(type);
}

// RecourcePath::RecourcePathListIterator TableManager::getRecourcePathIterator()
// {
// 	return mResPathTable->getRecourcePathIterator();
// }

PvpChatMap* TableManager::getPvpChatMap()
{ 
	return mPvpChatTable->getPvpChatMap();
}

PvpChat* TableManager::getPvpChat(int id)
{ 
	return mPvpChatTable->getPvpChat(id);
}

std::vector<PvpChat*>* TableManager::getChatType(int type)
{
	return mPvpChatTable->getChatType(type);
}

std::string TableManager::getPlatformName(int id)
{
	return mPlatformTable->getPlatformName(id);
}

const ErrorCode* TableManager::getErrorCodeByID(int id)
{
	const ErrorCode* pTextHelper = mErrorCodeTable->getErrorCodeByID(id);
	if (!pTextHelper)
	{
		CCLOG("[error] TableManager::getTextHelperByID not find TextHelper:%d!", id);
		return NULL;
	}
	return pTextHelper;
}

void TableManager::syncLoadCsv()
{
	if (!mTaskList.empty())
	{
		CsvLoadTask* pTask = mTaskList.front();
		mTaskList.pop_front();

		pTask->mTargetTable->init();
		setLoadState(LS_LOADING);
		CC_SAFE_DELETE(pTask);
	}
	else{

		// .csv加载完成
		setLoadState(LS_OVER);
		Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
	}
}

void TableManager::asyncLoadCsv()
{
	if (getLoadState() != LS_COMPLETE)
		return;

	CC_SAFE_DELETE(mCurrentTask);

	if (!mTaskList.empty())
	{
		mCurrentTask = mTaskList.front();
		mTaskList.pop_front();

		setLoadState(LS_LOADING);
		//mThread.execute(mCurrentTask);
	}
	else{

		setLoadState(LS_OVER);
		Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
	}
}

const PayLimit* TableManager::getPayLimit()
{
	return mPayLimitTable->getPayLimit();
}

void TableManager::loadErrorCodeCsv()
{
	mErrorCodeTable = new ErrorCodeTable();

	mErrorCodeTable->init();
}



void TableManager::initLoadTables()
{
#define LOAD_CSV(tar_table, tab_type) \
	tar_table = new tab_type(); \
	tar_table->init();

	LOAD_CSV(mTextHelperTable, TextHelperTable);
	LOAD_CSV(mPlatformTable, PlatformTable);
	LOAD_CSV(mErrorCodeTable, ErrorCodeTable);
	LOAD_CSV(mMusicTable, MusicTable);
	LOAD_CSV(mGuideDataTable, GuideDataTable);
}


Bible* TableManager::getBible(int id)
{
	return mBibleTable->getBible(id);
}

BibleList* TableManager::getBibleList()
{
	return mBibleTable->getBibleList();
}

const Intelligent* TableManager::getIntelligent(int id)
{
	return mIntelligentTable->getIntelligent(id);
}

void TableManager::loadPlatformCsv()
{
	mPlatformTable = new PlatformTable();
	mPlatformTable->init();
}

const CrazyWeekInfo* TableManager::getCrazyWeekInfoById(int id)
{
	return mCrazyWeekTable->getCrazyWeekInfoById(id);
}


const ExchangeItem* TableManager::getExchangeItem(int id)
{
	return mExchangeItemTable->getExchangeItem(id);
}

int	TableManager::getExchangeItemCount()
{
	return mExchangeItemTable->getExchangeItemCount();
}


const GradeData* TableManager::getGradeData(int point)
{
	return mGradeDataTable->getGradeData(point);
}

const GradeData* TableManager::getGradeByGrade(int gradeId)
{
	return mGradeDataTable->getGradeByGrade(gradeId);
}

void TableManager::loadActInformTable()
{
	mActInformTable = new ActInformTable();
	mActInformTable->init();
}


ActInformList TableManager::getActInformList()
{
	return mActInformTable->getActInformList();
}


ActInformData* TableManager::getActInformData(int id)
{
	return mActInformTable->getActInformData(id);
}

ActInformData* TableManager::getActInformDataByIdx(int idx)
{
	int num = 0;
	ActInformList pActInformList = mActInformTable->getActInformList();
	auto iter = pActInformList.begin();
	for (; iter != pActInformList.end(); iter++)
	{
		if (num == idx)
			return *iter;
		num++;
	}
	return nullptr;
}

ActInformList TableManager::getActInformListByType(int iType)
{
	int num = 0;
	ActInformList pActInformList;
	pActInformList.clear();
	ActInformList pTmpActInformList = mActInformTable->getActInformList();
	ActInformList::iterator iter = pTmpActInformList.begin();
	for (; iter != pTmpActInformList.end(); iter++)
	{
		if ((*iter)->mType == iType)
			pActInformList.push_back(*iter);
	}
	return pActInformList;
}

VipData* TableManager::getVipData(int id)
{
	return mVipDataTable->getVipData(id);
}

int  TableManager::getLevelExp(int level)
{
	return mLevelTable->getLevelData(level);
}

GlyphStuffData* TableManager::getGlyphStuffData(int id)
{
	return mGlyphStuffTable->getGlyphStuffData(id);
}

const GlyphStuffList& TableManager::getGlyphStuffMap()
{
	return mGlyphStuffTable->getGlyphStuffList();
}

const BadgeData* TableManager::getBadgeData(int id)
{
	return mBadgeDataTable->getBadge(id);
}

const GoodsInfo* TableManager::getGoodsInfo(int type, int id /*= 0*/)
{
	GoodsInfo* lGoodsInfo = mGoodsInfoTable->getGoodsInfo(type);
	if (!lGoodsInfo)
	{
		return NULL;
	}
	if (type == UserObj::PROPERTYTYPE::PT_ITEM)
	{
		const PropInfo *lPropInfo = getPropInfoById(id);
		lGoodsInfo->mName = lPropInfo->name;
		lGoodsInfo->mIcon = lPropInfo->icon;
		std::vector<std::string > pIconVec = splitStr(lPropInfo->icon, ".");
		lGoodsInfo->mIconBig = lPropInfo->icon;
		if (pIconVec.size()>0)
		{
			lGoodsInfo->mIconSmall = pIconVec[0] + "_small.png";
		}
		
	}
	else if (type == UserObj::PROPERTYTYPE::STUFF)
	{
		GlyphStuffData* lstuffData = getGlyphStuffData(id);
		lGoodsInfo->mName = lstuffData->mStuffName;
		lGoodsInfo->mIcon = lstuffData->mStuffIcon;
		lGoodsInfo->mIconBig = lGoodsInfo->mIcon;//default

		// add big image path	
		std::vector<std::string > pIconVec = splitStr(lGoodsInfo->mIcon, ".");
		if (pIconVec.size() > 0)
		{
			lGoodsInfo->mIconBig = pIconVec[0] + "_b.png";
		}	
		lGoodsInfo->mIconSmall = lGoodsInfo->mIcon;
	}
	else if (type == UserObj::PROPERTYTYPE::TITLE_ADD)
	{
		const BadgeData* pBadgeData = getBadgeData(id);
		lGoodsInfo->mName = pBadgeData->getName();
		lGoodsInfo->mIcon = pBadgeData->getPath();
		lGoodsInfo->mIconBig = lGoodsInfo->mIcon;
	}
	else if (type == UserObj::PROPERTYTYPE::GLYPH)
	{ 
		GlyphStuffData* pGlyphStuffData = getGlyphStuffData(id);
		lGoodsInfo->mName = pGlyphStuffData->mStuffName;
		lGoodsInfo->mIcon = pGlyphStuffData->getIcon();
		lGoodsInfo->mIconBig = lGoodsInfo->mIcon;
	}
	return lGoodsInfo;
}

SevenDayMission* TableManager::getSevenDayMissionByDay(int day)
{
	return mSevenDayMissionTable->getDataByDay(day);
}
