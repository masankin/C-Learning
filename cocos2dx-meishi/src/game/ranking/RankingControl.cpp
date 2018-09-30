
#include <algorithm>

#include "RankingControl.h"
#include "ui/dialogs/RankingDialog.h"
#include "data/TableManager.h"
#include "game/net/UserObj.h"
#include "game/pvp/PvpControl.h"
#include "core/MessageManager.h"
eRankingGroupType RankingControl::PointToGroup(int point)
{
	auto pGradeData = sgTableManager()->getGradeData(point);
	if (pGradeData)
		return pGradeData->getGroup();

	return RANKINGGROUP_DEFAULT;
}


void RankingControl::CleanRankingList(RankingList& rankingList)
{
	for (auto itr = rankingList.begin(); itr != rankingList.end(); ++itr)
	{
		auto& playerInfo = itr->second;
		if (playerInfo)
		{
			delete playerInfo;
			playerInfo = nullptr;
		}
	}
	rankingList.clear();
}


RankingControl::RankingControl() : mProxy(nullptr)
, mView(nullptr)
//, mPlayerGroupType(eRankingGroupType::RANKINGGROUP_BRONZE)
, mPlayerPrevRankingId(0)

//, mPlayerRoundCount(0)
, mbLockDialogClose(false)
, mAwardTime(0.0f)
{
	mGroupsTopNCache.clear();

	mProxy = new RankingProxy();

	mPvpResultList.clear();
}

RankingControl::~RankingControl()
{
	this->cleanup();

	if (nullptr != mProxy)
	{
		delete mProxy;
		mProxy = nullptr;
	}
}

void RankingControl::cleanup()
{
	for (int groupType = RANKINGGROUP_DEFAULT; groupType <= RANKINGGROUP_KING; ++groupType)
	{
		std::string scheduleKey = StringUtils::format("RankingControl::onScheduleCleanGroupTopNCache %d", groupType);
		Director::getInstance()->getScheduler()->unschedule(scheduleKey, this);
	}

	std::string scheduleKey = "RankingControl::onScheduleRequestPlayerResultInfo";
	Director::getInstance()->getScheduler()->unschedule(scheduleKey, this);

	RankingControl::CleanRankingList(mSelfRankingList);

	for (auto gourpItr = mGroupsTopNCache.begin(); gourpItr != mGroupsTopNCache.end(); ++gourpItr)
	{
		auto &rankingList = gourpItr->second;
		RankingControl::CleanRankingList(rankingList);
	}
	mGroupsTopNCache.clear();

	mGroupsCache.clear();

	this->cleanRoundList();
}


void RankingControl::requestRankingList(eRankingGroupType groupType)
{
	bool needReqTopN = false;
	auto groupItr = mGroupsTopNCache.find(groupType);
	if (groupItr == mGroupsTopNCache.end())
	{
		needReqTopN = true;
	}
	else
	{
		if (groupItr->second.empty())
			needReqTopN = true;
	}

	if (needReqTopN)
	{
		if (mProxy)
			mProxy->sendRankingListReq(groupType, RankingControl::TopNSize);
	}
	else
	{
		this->rebuildRakingList(groupType);
		if (mView && mView->isShow())
			mView->updateRankingList(groupType, mGroupsCache[groupType]);
	}

}


void RankingControl::doNetGetRankingListRsp(const fgame::CSPvpRankingGetListResp& stMsg)
{
	RankingList newList;
	int playerOrder = -1;
	for (int i = 0; i < stMsg.playerlist_size(); ++i)
	{
		auto& playerInfo = stMsg.playerlist(i);

		PlayerData* pData = new PlayerData();
		if (pData)
		{
			pData->id = playerInfo.id();
			pData->name = playerInfo.name();
			pData->rankingId = playerInfo.order();
			pData->point = playerInfo.point();
			pData->winRate = playerInfo.winrate();
			pData->vipLevel = playerInfo.viplevel();

			newList.insert(std::make_pair(pData->rankingId, pData));
		}
	}

	auto groupType = static_cast<eRankingGroupType>(stMsg.groundid());

	if (newList.empty())
	{
		CCLOG("[trace]RankingControl::doNetRankingRsp : ranking list is empty");
	}
	else
	{
		auto groupItr = mGroupsTopNCache.find(groupType);
		if (groupItr != mGroupsTopNCache.end())
		{
			auto & oldList = groupItr->second;
			RankingControl::CleanRankingList(oldList);
			oldList.swap(newList);
		}
		else
		{
			auto rlt = mGroupsTopNCache.insert(std::make_pair(groupType, std::move(newList)));
			groupItr = rlt.first;
		}

		std::string scheduleKey = StringUtils::format("RankingControl::onScheduleCleanGroupTopNCache %d", (int)groupType);
		Director::getInstance()->getScheduler()->unschedule(scheduleKey, this);

		Director::getInstance()->getScheduler()->schedule(std::bind(&RankingControl::onScheduleCleanGroupTopNCache, this, std::placeholders::_1, groupType)
			, this
			, 10 * 60.0f
			, 0
			, 0
			, false
			, scheduleKey);


		this->rebuildRakingList(groupType);
	}

	if (mView && mView->isShow())
		mView->updateRankingList(groupType, mGroupsCache[groupType]);
}



void RankingControl::requestPlayerRankingInfo()
{
	if (mProxy)
		mProxy->sendPlayerRankInfoReq(sgUserObj()->getUin());
}


void RankingControl::doNetPlayerRankingDataRsp(const fgame::CSPvpRankingPlayerInfoResp& stMsg)
{
	if (stMsg.rankinfo().id() == sgUserObj()->getUin())
	{
		//mPlayerRoundCount = stMsg.totalround();

		//if (stMsg.rankinfo().has_group())
		//	mPlayerGroupType = static_cast<eRankingGroupType>(stMsg.rankinfo().group());

		//mPlayerRankingData.id = stMsg.rankinfo().id();
		//mPlayerRankingData.name = stMsg.rankinfo().name();
		//mPlayerRankingData.point = stMsg.rankinfo().point();
		//mPlayerRankingData.winRate = stMsg.rankinfo().winrate();
		//mPlayerRankingData.score = stMsg.rankinfo().score();
		//mPlayerRankingData.rankingId = stMsg.rankinfo().order();

		sgPvpControl()->setLastPvpCount(sgUserObj()->getPersonalInfo()->pvpround());

		sgUserObj()->getPersonalInfo()->set_pvpround(stMsg.totalround());//sgUserObj()->setRoundCount(stMsg.totalround());
		sgUserObj()->setScore(stMsg.rankinfo().point());
		sgUserObj()->getPersonalInfo()->set_winrate(stMsg.rankinfo().winrate());//sgUserObj()->setWinRate(stMsg.rankinfo().winrate());
		sgUserObj()->getPersonalInfo()->set_rank(stMsg.rankinfo().order());//sgUserObj()->setRanking(stMsg.rankinfo().order());
		sgUserObj()->getPersonalInfo()->set_honorcount(stMsg.rankinfo().score());//sgUserObj()->setHonorCount(stMsg.rankinfo().score());

		//if (0 == mPlayerGroupType)
		//	mPlayerGroupType = eRankingGroupType::RANKINGGROUP_BRONZE;
	}
}


void RankingControl::requestPlayerPvpResultInfo()
{
	if (mProxy)
		mProxy->sendPlayerPvpResultReq();

	std::string scheduleKey = "RankingControl::onScheduleRequestPlayerResultInfo";
	Director::getInstance()->getScheduler()->unschedule(scheduleKey, this);
	Director::getInstance()->getScheduler()->schedule(std::bind(&RankingControl::onScheduleRequestPlayerResultInfo, this, std::placeholders::_1)
		, this
		, 20 * 60.0f
		, 0
		, 0
		, false
		, scheduleKey);

}


void RankingControl::doNetPlayerPvpResultRsp(const fgame::CSPvpResultResp& stMsg)
{
	this->cleanRoundList();

	mPlayerPrevRankingId = INT_MAX;

	for (int roundIndex = 0; roundIndex < stMsg.result_size(); ++roundIndex)
	{
		this->addPvpResult(stMsg.result(roundIndex));
	}
}


void RankingControl::requestPlayerSelfRankingList()
{
	if (mProxy)
		mProxy->sendSelfRankingListReq();
}

void RankingControl::requestRankWeekAwardInfo()
{
	if (mProxy)
		mProxy->sendRankWeekAwardInfoReq();
}

void RankingControl::requestRankWeekAward()
{
	if (mProxy)
		mProxy->sendRankWeekAwardReq();
}

void RankingControl::doNetGetSelfRankingResp(const fgame::CSPvpSelfRankListResp& stMsg)
{
	RankingControl::CleanRankingList(mSelfRankingList);

	for (int i = 0; i < stMsg.playerlist_size(); ++i)
	{
		auto& playerInfo = stMsg.playerlist(i);

		PlayerData* pData = new PlayerData();
		if (pData)
		{
			pData->id = playerInfo.id();
			pData->name = playerInfo.name();
			pData->rankingId = playerInfo.order();
			pData->point = playerInfo.point();
			pData->winRate = playerInfo.winrate();
			pData->vipLevel= playerInfo.viplevel();

			//data->online = playerInfo.online();

			mSelfRankingList.insert(std::make_pair(pData->rankingId, pData));
		}
	}
}

void RankingControl::doNetRankWeekAwardInfoResp(const fgame::CSRankWeekRewardInfoResp& stMsg)
{
	mRankWeekAwardinfo.Status = stMsg.status();
	mRankWeekAwardinfo.PvpCount = stMsg.pvpcount();
	mRankWeekAwardinfo.GroupId = stMsg.groupid();
	mRankWeekAwardinfo.Rank = stMsg.rank();

	if (mView && mView->isShow())
	{
		mView->setRankAwardInfo(mRankWeekAwardinfo.Status);
	}
}

void RankingControl::doNetRankWeekAwardResp(const fgame::CSRankWeekRewardRewardResp stMsg)
{
	mRankWeekAward.Success = stMsg.success();
	mRankWeekAward.Error = stMsg.error();
}

static bool SortRankingPlayerData(const RankingControl::PlayerData* p1, const RankingControl::PlayerData* p2)
{
	if (p1->point > p2->point)
	{
		return true;
	}
	else if (p1->point == p2->point)
	{
		if (p1->winRate > p2->winRate)
			return true;
	}

	return false;
}

void RankingControl::rebuildRakingList(eRankingGroupType groupType)
{
	RankingList * pTopNRankingList = nullptr;
	auto groupItr = mGroupsTopNCache.find(groupType);
	if (groupItr != mGroupsTopNCache.end())
	{
		pTopNRankingList = &groupItr->second;
	}

	if (nullptr == pTopNRankingList)
	{
		CCLOG("[trace]RankingControl::rebuildRakingList : Top n ranking list is null");
		return;
	}

	if (pTopNRankingList->empty())
	{
		CCLOG("[trace]RankingControl::rebuildRakingList : Top n ranking list is empty");
		return;
	}


	RankingList * pRankingList = &mGroupsCache[groupType];
	if (nullptr == pRankingList)
	{
		CCLOG("[trace]RankingControl::rebuildRakingList : Ranking list is null");
		return;
	}

	pRankingList->clear();

	mPlayerRankingData.id = sgUserObj()->getUin();
	mPlayerRankingData.name = sgUserObj()->getPlayerName();
	mPlayerRankingData.rankingId = sgUserObj()->getPersonalInfo()->rank();//sgUserObj()->getRanking();
	mPlayerRankingData.point = sgUserObj()->getPersonalInfo()->score();//sgUserObj()->getPvpPoint();
	mPlayerRankingData.winRate = sgUserObj()->getPersonalInfo()->winrate();//sgUserObj()->getWinRate();
	mPlayerRankingData.score = sgUserObj()->getPersonalInfo()->honorcount();//sgUserObj()->getHonorCount();
	mPlayerRankingData.vipLevel = sgUserObj()->getPersonalInfo()->viplevel();//sgUserObj()->getVipLevel();


	// 判断是否是自己所在榜，如果不是则直接插入TopN列表(需剔除自己)；如果是则特殊处理
	if (groupType != sgUserObj()->getRankGroup())
	{
		// pRankingList->insert(pTopNRankingList->begin(), pTopNRankingList->end());

		std::vector<PlayerData*> sortList;
		sortList.reserve(pTopNRankingList->size());
		for (auto & e : *pTopNRankingList)
		{
			auto pPlayerData = e.second;
			if (nullptr == pPlayerData)
				continue;

			if (pPlayerData->id != mPlayerRankingData.id)
			{
				sortList.push_back(pPlayerData);
			}
		}

		// 排序
		std::sort(sortList.begin(), sortList.end(), SortRankingPlayerData);

		// 从新赋予排行序号
		for (unsigned int index = 0; index < sortList.size(); ++index)
		{
			// 这个地方把替换本地玩家假数据的排行序号重新赋值
			sortList[index]->rankingId = 1 + index;
		}

		// 插入到榜中
		for (auto pPlayerData : sortList)
		{
			pRankingList->emplace(std::make_pair(pPlayerData->rankingId, pPlayerData));
		}
	}
	else
	{
		// 判断自己周边排行列表是否为空
		if (mSelfRankingList.empty())
		{
			CCLOG("[warn]RankingControl::rebuildRakingList : no self ranking list is empty.");

			pRankingList->insert(pTopNRankingList->begin(), pTopNRankingList->end());
		}
		else
		{
			// 利用Map的有序性,获取“自己周边排行列表最小排行”
			int minSelfListRankingId = mSelfRankingList.begin()->first;

			if (minSelfListRankingId > RankingControl::TopNSize)
			{
				// 如果“自己周边排行列表最小排行”大于TopNSize名次，那么假榜需分两段处理

				// 第一段，把TopN排行榜重新赋予排名（有可能之前的排序过，打乱了他们排名）,并排除自己
				// 由于原先Map的key就是TopN排行榜玩家的排名序号，按照这个顺去重新排名即可
				int topNIndex = 0;
				for (auto & e : *pTopNRankingList)
				{
					auto pPlayerData = e.second;
					if (nullptr == pPlayerData)
						continue;

					if (pPlayerData->id != mPlayerRankingData.id)
					{
						pPlayerData->rankingId = ++topNIndex;
						pRankingList->emplace(std::make_pair(pPlayerData->rankingId, pPlayerData));
					}
				}

				//pRankingList->insert(pTopNRankingList->begin(), pTopNRankingList->end());

				// 第二段，把自己周边排行列表放在一个列表，把自己排行数据换成本地排行假数据，然后重新排序，最后插入榜单

				bool needPushSelf = true;
				std::vector<PlayerData*> sortList;
				sortList.reserve(mSelfRankingList.size());
				for (auto & e : mSelfRankingList)
				{
					auto pPlayerData = e.second;
					if (nullptr == pPlayerData)
						continue;

					if (pPlayerData->id != mPlayerRankingData.id) 
					{
						sortList.push_back(pPlayerData);
					}
					else // 替换本地玩家假数据 
					{
						sortList.push_back(&mPlayerRankingData);
						needPushSelf = false;
					}
				}

				if (needPushSelf)
					sortList.push_back(&mPlayerRankingData);

				// 排序
				std::sort(sortList.begin(), sortList.end(), SortRankingPlayerData);

				// 从新赋予排行序号
				for (unsigned int index = 0; index < sortList.size(); ++index)
				{
					// 这个地方把替换本地玩家假数据的排行序号重新赋值
					sortList[index]->rankingId = minSelfListRankingId + index;
				}

				// 插入到榜中
				for (auto pPlayerData : sortList)
				{
					pRankingList->emplace(std::make_pair(pPlayerData->rankingId, pPlayerData));
				}
			}
			else
			{
				// 如果“自己周边排行列表最小排行”小于于TopNSize名次，那么证明TopN列表
				// 和自己周边排行列表可以放在一起排序，并且排名是从1开始。但必须要先剔除
				// 重复的数据

				// 先缓存排行榜前N的数据
				std::map<uint64, PlayerData*> tempPlayerRankingList;
				for (auto & e : *pTopNRankingList)
				{
					auto pPlayerData = e.second;
					if (nullptr == pPlayerData)
						continue;

					tempPlayerRankingList.emplace(std::make_pair(pPlayerData->id, pPlayerData));
				}

				std::map<uint64, PlayerData*>::iterator tempListItr = tempPlayerRankingList.end();
				// 再用自己周边排行列表更新缓存
				for (auto & e : mSelfRankingList)
				{
					auto pPlayerData = e.second;
					if (nullptr == pPlayerData)
						continue;

					tempListItr = tempPlayerRankingList.find(pPlayerData->id);
					if (tempListItr != tempPlayerRankingList.end())
					{
						tempListItr->second = pPlayerData; // 更换新数据
					}
					else
					{
						tempPlayerRankingList.emplace(std::make_pair(pPlayerData->id, pPlayerData));
					}
				}

				// 最后把自己的数据剔除，换成自己的本地的假数据
				tempListItr = tempPlayerRankingList.find(mPlayerRankingData.id);
				if (tempListItr != tempPlayerRankingList.end())
				{
					tempListItr->second = &mPlayerRankingData;
				}
				else
				{
					tempPlayerRankingList.emplace(std::make_pair(mPlayerRankingData.id, &mPlayerRankingData));
				}

				// 放到一个列表里面，排序
				std::vector<PlayerData*> sortList;
				sortList.reserve(tempPlayerRankingList.size());
				for (auto &e : tempPlayerRankingList)
				{
					sortList.push_back(e.second);
				}

				// 排序
				std::sort(sortList.begin(), sortList.end(), SortRankingPlayerData);

				// 从新赋予排行序号
				for (unsigned int index = 0; index < sortList.size(); ++index)
				{
					// 这个地方把替换本地玩家假数据的排行序号重新赋值
					sortList[index]->rankingId = 1 + index;
				}

				// 插入到榜中
				for (auto *pPlayerData : sortList)
				{
					pRankingList->emplace(std::make_pair(pPlayerData->rankingId, pPlayerData));
				}
			}
		}
	}

	// 排序后再赋予新排名
	//sgUserObj()->setRanking(mPlayerRankingData.rankingId);
	sgUserObj()->getPersonalInfo()->set_rank(mPlayerRankingData.rankingId);
	sgMessageManager()->sendMessage(EMT_RANK_CHANGE, "");
}


void RankingControl::giveUp(int point)
{
	//++mPlayerRoundCount;

	int roundCount = sgUserObj()->getPersonalInfo()->pvpround();//sgUserObj()->getRoundCount();
	sgPvpControl()->setLastPvpCount(roundCount);
	sgUserObj()->getPersonalInfo()->set_pvpround(++roundCount);//sgUserObj()->setRoundCount(++roundCount);

	mPlayerRankingData.point += point;
	if (mPlayerRankingData.point < 1200)
		mPlayerRankingData.point = 1200;

	//sgUserObj()->setPvpPoint(mPlayerRankingData.point);

	//mPlayerGroupType = RankingControl::PointToGroup(mPlayerRankingData.point);
}


void RankingControl::addPvpResult(const fgame::CSPvpEndResp& pvpResult)
{
	PvpResultData* pRoundData = new PvpResultData();
	if (pRoundData)
	{
		pRoundData->winUin = pvpResult.win();
		for (int playerIndex = 0; playerIndex < pvpResult.result_size(); ++playerIndex)
		{
			auto& playerInfo = pvpResult.result(playerIndex);

			PvpPlayerResultData* pPlayerData = new PvpPlayerResultData();
			if (pPlayerData)
			{
				pPlayerData->uin = playerInfo.uin();
				pPlayerData->name = playerInfo.name();
				pPlayerData->addPoint = playerInfo.addpoint();
				pPlayerData->addScore = playerInfo.addscore();

				pRoundData->resultList.push_back(pPlayerData);

				if (playerInfo.uin() == sgUserObj()->getUin())
				{
					mPlayerPrevRankingId = mPlayerRankingData.rankingId;
					mPlayerRankingData.rankingId = playerInfo.newrank();

					//mPlayerPrevRankingId = mPlayerRankingData.rankingId;

					// 这个玩家排行还不是真正的排行序号，正在的在rebuildRakingList中赋值
					// mPlayerRankingData.rankingId = playerInfo.newrank(); 

					sgUserObj()->getPersonalInfo()->set_rank(mPlayerRankingData.rankingId);//sgUserObj()->setRanking(mPlayerRankingData.rankingId);
					sgUserObj()->getPersonalInfo()->set_winrate(playerInfo.winrate());//sgUserObj()->setWinRate(playerInfo.winrate());
					sgUserObj()->setScore(playerInfo.prevpoint() + playerInfo.addpoint());
					sgUserObj()->getPersonalInfo()->set_honorcount(playerInfo.prevscore() + playerInfo.addscore());//sgUserObj()->setHonorCount(playerInfo.prevscore() + playerInfo.addscore());
				}
			}
		}

		mPvpResultList.push_back(pRoundData);

		if (mPvpResultList.size() > 20)
		{
			auto pRemoveData = mPvpResultList.front();
			mPvpResultList.pop_front();
			if (pRemoveData)
			{
				delete pRemoveData;
				pRemoveData = nullptr;
			}
		}
	}
}


void RankingControl::onScheduleCleanGroupTopNCache(float dt, eRankingGroupType groupType)
{
	auto groupItr = mGroupsTopNCache.find(groupType);
	if (groupItr != mGroupsTopNCache.end())
	{
		RankingControl::CleanRankingList(groupItr->second);
	}

	mGroupsCache[groupType].clear();
}


void RankingControl::cleanRoundList()
{
	for (auto roundItr = mPvpResultList.begin(); roundItr != mPvpResultList.end(); ++roundItr)
	{
		auto& pRoundData = *roundItr;
		if (pRoundData)
		{
			auto & resultList = pRoundData->resultList;
			for (auto resultItr = resultList.begin(); resultItr != resultList.end(); ++resultItr)
			{
				auto & pPlayerData = *resultItr;
				if (pPlayerData)
				{
					delete pPlayerData;
					pPlayerData = nullptr;
				}
			}
			resultList.clear();

			delete pRoundData;
			pRoundData = nullptr;
		}
	}
	mPvpResultList.clear();
}


void RankingControl::onScheduleRequestPlayerResultInfo(float dt)
{
	if (mProxy)
		mProxy->sendPlayerPvpResultReq();
}
