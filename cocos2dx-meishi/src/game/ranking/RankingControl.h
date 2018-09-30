#pragma once

#include "Constant.h"
#include "Singleton.h"
#include "RankingProxy.h"
#include "net/NetPack.h"

class RankingProxy;
class RankingDialog;
class RankingControl
{
public:
	struct PlayerData
	{
		PlayerData() : id(0), rankingId(0), point(0), winRate(0), score(0), vipLevel(0)
		{

		}

		~PlayerData()
		{

		}

		uint64 id;
		int rankingId;
		std::string name;
		int point;
		float winRate;
		int score;
		int vipLevel;
	};

	struct PvpPlayerResultData
	{
		PvpPlayerResultData() : uin(0), addPoint(0), addScore(0)
		{

		}

		uint64 uin;
		std::string name;
		int addPoint; //增加的分数
		int addScore; // 增加的荣誉
	};

	struct PvpResultData
	{
		PvpResultData() : winUin(0)
		{

		}

		uint64 winUin;
		std::vector<PvpPlayerResultData*> resultList;
	};

	struct stuRankWeekAwardinfo
	{
		bool Status;
		int PvpCount;
		int GroupId;
		int Rank;

		stuRankWeekAwardinfo()
		{
			Status = false;
			PvpCount = 0;
			GroupId = 0;
			Rank = 0;
		}
	};
	stuRankWeekAwardinfo mRankWeekAwardinfo;

	struct stuRankWeekAward
	{
		bool Success;
		int  Error;

		stuRankWeekAward()
		{
			Success = false;
			Error = 0;
		}
	};
	stuRankWeekAward mRankWeekAward;

	// 利用map的排序和唯一性
	typedef std::map<int, PlayerData*> RankingList;
	typedef std::map<eRankingGroupType, RankingList> GroupList;
	typedef std::list<PvpResultData*> PvpRoundResultList;

	bool getStatus(){ return mRankWeekAwardinfo.Status; };
	int getPvpCount(){ return mRankWeekAwardinfo.PvpCount; };
	int getGroupId(){ return mRankWeekAwardinfo.GroupId; };
	int getRank(){ return mRankWeekAwardinfo.Rank; };

	void setPvpCount(int count){ mRankWeekAwardinfo.PvpCount = count; };
	void setGroupId(int groupId){ mRankWeekAwardinfo.GroupId = groupId; };
	void setRank(int rank){ mRankWeekAwardinfo.Rank = rank; };


	bool getSuccess(){ return mRankWeekAward.Success; };
	int getError(){ return mRankWeekAward.Error; };
	
public:

	RankingControl();
	~RankingControl();

	void cleanup();

	void setView(RankingDialog* dialog) { mView = dialog; }
	RankingDialog* getView() const { return mView; }

	RankingProxy *	getProxy()const  { return mProxy; }

	// 请求指定榜Top 50数据
	void requestRankingList(eRankingGroupType groupType);
	// 请求个人信息
	void requestPlayerRankingInfo();
	// 请求个人pvp对战列表
	void requestPlayerPvpResultInfo();
	// 请求个人在排行榜中的排行数据
	void requestPlayerSelfRankingList();
	// 请求排行奖励信息
	void requestRankWeekAwardInfo();
	// 请求领取排行奖励
	void requestRankWeekAward();

	void doNetGetRankingListRsp(const fgame::CSPvpRankingGetListResp& stMsg);
	void doNetPlayerRankingDataRsp(const fgame::CSPvpRankingPlayerInfoResp& stMsg);
	void doNetPlayerPvpResultRsp(const fgame::CSPvpResultResp& stMsg);
	void doNetGetSelfRankingResp(const fgame::CSPvpSelfRankListResp& stMsg);
	void doNetRankWeekAwardInfoResp(const fgame::CSRankWeekRewardInfoResp& stMsg);
	void doNetRankWeekAwardResp(const fgame::CSRankWeekRewardRewardResp stMsg);

	void addPvpResult(const fgame::CSPvpEndResp& pvpResult);
	void giveUp(int point);
	
	const PvpRoundResultList& getRoundResultList() const { return mPvpResultList; }

	//unsigned int getPlayerRoundCount() const { return mPlayerRoundCount; }
	//eRankingGroupType getPlayerGroupType() const { return mPlayerGroupType; }
	//int getPlayerPoint() const { return mPlayerRankingData.point; }
	//float getPlayerWinRate() const { return mPlayerRankingData.winRate; }

	int getPlayerCurrentRanking() const { return mPlayerRankingData.rankingId; }
	int getPlayerPrevRanking() const { return mPlayerPrevRankingId; }

	int getPlayerScore() const { return mPlayerRankingData.score; }

	bool mbLockDialogClose;
	bool getbLockDialogClose(){ return mbLockDialogClose; };
	void setbLockDialogClose(bool b){ mbLockDialogClose = b; };

	double mAwardTime;
	double getAwardTime(){ return mAwardTime; };
	void   setAwardTime(double time){ mAwardTime = time; };

protected:
	static eRankingGroupType PointToGroup(int point);
	static void CleanRankingList(RankingList& rankingList);

	void cleanRoundList();

	void rebuildRakingList(eRankingGroupType groupType);

	void onScheduleCleanGroupTopNCache(float dt, eRankingGroupType groupType);
	void onScheduleRequestPlayerResultInfo(float dt);

protected:
	static const int TopNSize = 50;

	RankingProxy* mProxy;
	RankingDialog* mView;

	GroupList mGroupsCache; // 真正排行榜（只负责保存数据，不操作数据） 由mGroupsTopNCache、mSelfRankingList、mPlayerRankingData中三部分数据组成
	GroupList mGroupsTopNCache; // 保存服务器反馈的前N名数据 
	RankingList mSelfRankingList; // 保存自己积分周边的玩家排名数据

	//eRankingGroupType mPlayerGroupType;
	//unsigned int mPlayerRoundCount;
	int mPlayerPrevRankingId;

	PlayerData mPlayerRankingData; // 保存一份自己本地的排行榜“假”数据

	PvpRoundResultList mPvpResultList;
};

#define sgRankingControl() Singleton<RankingControl>::Get()
