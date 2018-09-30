#ifndef __BATTLE_RECORDS_MANAGER_H__
#define __BATTLE_RECORDS_MANAGER_H__

#include "Singleton.h"
#include<string>
#include<sstream>
#include <vector>
#include "net/proto/BattleRecord.pb.h"
#include "game/Message.h"

using namespace std;

struct BuildInfoRecord
{
	int mBuildId;
	int mCamp;
	int mLevel;
	int mPeopleCount;

	BuildInfoRecord(int buildId, int camp, int level, int peopleCount)
	{
		mBuildId = buildId;
		mCamp = camp;
		mLevel = level;
		mPeopleCount = peopleCount;
	}
};

//-------------------------------------------class BattleRecordsManager----------------------------------------
class BattleRecordsManager:MessageHandler
{
public:
	BattleRecordsManager();
	~BattleRecordsManager();

	std::vector<std::string> mBattleRecords;

	void update(float dt);

	void resetCommondInterval();

	void runPlayBack(float dt);

	void setIsPlayback(bool b);
	bool getIsPlayback();

	void setIsBeganPlayback(bool);
	bool getIsBeganPlayBack();

	int getLastPlaybackPoint();
	void setLastPlaybackPoint(int point);

	void saveBattleRecords(int pointId);
	bool loadBattleRecords(int pointId);

	virtual void onReceiveMassage(GameMessage * message);
private:
	float mCommondInterval;	//每一次pve操作的时间间隔
	float mTimeElapsed;

	bool mIsPlayback;
	bool mIsBeganPlayback;

	int mLastPlaybackPoint;

	int mCommondIndex;

	//proto数据
	fgame::BattleRecords mBattleRecord_proto;
	bool saveTempPlayerData(std::string path);
	void setNowBuildInfo(fgame::BattleRecord* battleRecord);	//兵力，类型等信息
};

#define sgBattleRecordsManager() Singleton<BattleRecordsManager>::Get()

#endif
