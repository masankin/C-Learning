
#include <fstream>
#include "BattleControl.h"
#include "BattleRecordsManager.h"
#include "Formation/FormationV2.h"
#include "Build.h"
#include "core/TimeCalculator.h"
#include "BattleControl.h"
#include "BattleInfo.h"
#include "GamePrecedure.h"
#include "prop/PropsControl.h"

#define TEMP_BATTLE_RECORDS_DATA "tmpbattleRecord.db"
#define BATTLE_RECORDS_DATA "battleRecords%d.db"

BattleRecordsManager::BattleRecordsManager() :mCommondInterval(0.0f), mTimeElapsed(0.0f), mIsPlayback(false), mIsBeganPlayback(false), mLastPlaybackPoint(0), mCommondIndex(0)
{
	mBattleRecord_proto.Clear();

	sgMessageManager()->regisiterMessageHandler(EMT_MOVE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_UPGRADE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_CHANGE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_CHANGEOK, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_ADDTIME, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_FULLPEOP, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_FREEZE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_COOKING, this);
	sgMessageManager()->regisiterMessageHandler(EMT_RECORD_CREEPS_BORN, this);
	sgMessageManager()->regisiterMessageHandler(EMT_RECORD_OUT_PERCENT, this);
	sgMessageManager()->regisiterMessageHandler(EMT_RECORD_TURRET_UPDATE, this);
}

BattleRecordsManager::~BattleRecordsManager()
{
	sgMessageManager()->removeMessageHandler(this);
}

void BattleRecordsManager::update(float dt)
{
	if (!sgGamePrecedure()->getGameIng())
	{
		return;
	}

	mCommondInterval += dt;

	runPlayBack(dt);

	//默认1秒后取消箭头指示
	if (sgTimeCalculator()->getTimeLeft("timePlayback") == 0)
	{
		sgBattleControl()->getBattleView()->resetPainting();
	}
}

void BattleRecordsManager::resetCommondInterval()
{
	if (mIsPlayback == false)
	{
		mBattleRecord_proto.Clear();
	}

	mCommondInterval = 0.0f;
}

void BattleRecordsManager::runPlayBack(float dt)
{
	if (false == getIsBeganPlayBack())
	{
		return;
	}

	int size = mBattleRecord_proto.mvecbattlerecords_size();
	while (mCommondIndex < size)
	{
		const fgame::BattleRecord &record = mBattleRecord_proto.mvecbattlerecords(mCommondIndex);

		mTimeElapsed += dt;
		float timee = record.mtimeinterval();
		if (mTimeElapsed >= timee)
		{
			char params[128];
			memset(params, 0, 128);

			switch (record.mcommondtype())
			{
			case PVE_RECORD_MOVE:
			{
				//箭头
				if (sgBattleControl()->getTargetBuildByIns(record.msrc())->getCamp() == ECAMP_TYPE_Player)
				{
					auto posSrc = sgBattleControl()->getTargetBuildByIns(record.msrc())->getPosition();
					auto posDest = sgBattleControl()->getTargetBuildByIns(record.mdest())->getPosition();

					sgBattleControl()->getBattleView()->setPaintingPos(posSrc, posDest);

					sgTimeCalculator()->createTimeCalcultor("timePlayback", 0.7f);
				}

				//发指令
				sprintf(params, "%d %d %d %d %d", record.msrc(), record.mdest(), FormationV2::NewId(), record.mnumtype(), record.mnum());
				sgMessageManager()->sendMessage(EMT_MOVE, params);
			}
			break;
			case PVE_RECORD_UPGRADE:
			{
				sprintf(params, "%d", record.mbuildid());
				sgMessageManager()->sendMessage(EMT_UPGRADE, params);
			}
			break;
			case PVE_RECORD_PROP_CHANGE:
			{
				sprintf(params, "%d %d", record.mbuildid(), record.mentype());
				sgMessageManager()->sendMessage(EMT_PROP_CHANGE, params);
			}
			break;
			case PVE_RECORD_PROP_CHANGEOK:
			{
				sprintf(params, "%d %d %d", record.mkeyid(), record.mchangetype(), record.misnew());
				sgMessageManager()->sendMessage(EMT_PROP_CHANGEOK, params);
			}
			break;
			case PVE_RECORD_PROP_ADDTIME:
			{
				sprintf(params, "%f", record.mduration());
				//sgPropsControl()->setCdTime(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_TIME));
				sgMessageManager()->sendMessage(EMT_PROP_ADDTIME, params);
			}
			break;
			case PVE_RECORD_PROP_FULLPEOP:
			{
				sprintf(params, "%d %d", record.mbuildid(), record.mcamp());
				sgMessageManager()->sendMessage(EMT_PROP_FULLPEOP, params);
			}
			break;
			case PVE_RECORD_PROP_FREEZE:
			{
				sgMessageManager()->sendMessage(EMT_PROP_FREEZE, "");
			}
			break;
			case PVE_RECORD_PROP_COOKING:
			{
				sgPropsControl()->doCookingSolider(Vec2(record.mx(), record.my()), ECAMP_TYPE_Player);
			} 
			break;
			case PVE_RECORD_CREEPS_BORN:
			{
				sgBattleControl()->getCreepsManager()->creepsBorn(record.mcreepsid(), record.mhittedcreepsid());
			}
			break;
			case PVE_RECORD_OUT_PERCENT:
			{
				//改变比例显示
				sprintf(params, "%d", record.moutpercent());
				sgMessageManager()->sendMessage(EMT_BUTTON_OUT_PERCENT, params);
			}
			case PVE_RECORD_TURRET_UPDATE:
			{
				sprintf(params, "%d %d", record.mbuildid(), record.mturretstep());
				sgMessageManager()->sendMessage(EMT_RECORD_EXECUTE_TURRET_UPDATE, params);
			}
			break;
			default:
				break;
			}

			//build校准
			int buildSize = record.mvectorbuildinfo_size();

			for (int i = 0; i < buildSize; i++)
			{
				const fgame::BuildInfoRecord &vectorBuildInfo = record.mvectorbuildinfo(i);

				BaseBuild * build = sgBattleControl()->getTargetBuildByIns(vectorBuildInfo.mbuildid());

				if (!build)
				{
					CCLOG("[error]BattleRecordsManager::runPlayBack error buildId, please check battleRecords config file");
					return;
				}

				int nowCamp = build->getCamp();
				int targetCamp = vectorBuildInfo.mcamp();

				if (nowCamp != targetCamp)
				{
					//校准阵营
					build->recordSetCamp(targetCamp);
				}

				if (build->getLevel() != vectorBuildInfo.mlevel())
				{
					//校准等级
					build->recordSetLevel(vectorBuildInfo.mlevel());
				}

				if (build->getOwnPeople() != vectorBuildInfo.mpeoplecount())
				{
					int owner = build->getOwnPeople();
					int owner2 = vectorBuildInfo.mpeoplecount();
					//校准人口
					build->recordSetPeopleCount(vectorBuildInfo.mpeoplecount());
				}
			}

			mTimeElapsed = 0.0f;
			mCommondIndex += 1;
		}

		return;
	}
}

void BattleRecordsManager::setIsPlayback(bool b)
{
	if (b == true)
	{
		mCommondIndex = 0;
	}

	mIsPlayback = b;
}

bool BattleRecordsManager::getIsPlayback()
{
	return mIsPlayback;
}

void BattleRecordsManager::setIsBeganPlayback(bool b)
{
	mIsBeganPlayback = b;
}

bool BattleRecordsManager::getIsBeganPlayBack()
{
	return mIsBeganPlayback;
}

int BattleRecordsManager::getLastPlaybackPoint()
{
	return mLastPlaybackPoint;
}

void BattleRecordsManager::setLastPlaybackPoint(int point)
{
	mLastPlaybackPoint = point;
}

void BattleRecordsManager::setNowBuildInfo(fgame::BattleRecord* battleRecord)
{
	//--get 信息 
	MapBulidList buildList = sgBattleControl()->getBuildList();

	for (MapBulidList::iterator iter = buildList.begin(); iter != buildList.end(); iter++)
	{
		int buildId = iter->second->getBuildInfo().insID;
		int camp = iter->second->getCamp();
		int level = iter->second->getLevel();
		int peopleCount = iter->second->getOwnPeople();

		fgame::BuildInfoRecord* pBuildInfo = battleRecord->add_mvectorbuildinfo();
		pBuildInfo->set_mbuildid(buildId);
		pBuildInfo->set_mcamp(camp);
		pBuildInfo->set_mlevel(level);
		pBuildInfo->set_mpeoplecount(peopleCount);
	}
}

bool BattleRecordsManager::saveTempPlayerData(std::string path)
{
	int iProtoSize = mBattleRecord_proto.ByteSize();
	char* buffer = new char[iProtoSize];
	mBattleRecord_proto.SerializeToArray(buffer, iProtoSize);
	std::fstream* dbSaveHand = new std::fstream(path.c_str(), std::fstream::binary | std::ios::out);
	if (dbSaveHand)
	{
		dbSaveHand->seekp(0, std::ios::beg);
		dbSaveHand->write(buffer, iProtoSize);
		dbSaveHand->flush();
		dbSaveHand->close();
	}
	delete[] buffer;

	mBattleRecord_proto.Clear();
	return true;
}

void BattleRecordsManager::saveBattleRecords(int pointId)
{
	//写入本地
	std::string filePath = "battleRecords/";
	//tmpPath
	std::string tempPath = filePath + TEMP_BATTLE_RECORDS_DATA;

	if (saveTempPlayerData(tempPath))
	{
		bool canRename = true;

		char buff[128];
		memset(buff, 0, 128);
		sprintf(buff, BATTLE_RECORDS_DATA, pointId);

		//dbPath
		std::string dbPath = filePath + buff;

		//isFileExist返回值不正确
		ssize_t bufferSize;
		auto pBuffer = CCFileUtils::sharedFileUtils()->getFileData(dbPath.c_str(), "rb", &bufferSize);
		if (pBuffer)
		//if (FileUtils::getInstance()->isFileExist(dbPath))
		{
			if (remove(dbPath.c_str()) != 0)
			{
				canRename = false;
			}
		}

		if (canRename)
		{
			int renameRet = rename(tempPath.c_str(), dbPath.c_str());
			if (renameRet != 0)
				CCLOG("[error]UserObj::savePlayerData rename result = %d", renameRet);
		}
	}
}

bool BattleRecordsManager::loadBattleRecords(int pointId)
{
	//获得路径
	char buff[128];
	memset(buff, 0, 128);
	sprintf(buff, BATTLE_RECORDS_DATA, pointId);

	std::string filePath = "battleRecords/";
	filePath += buff;
	filePath = FileUtils::getInstance()->fullPathForFilename(filePath);

	//读文件
	ssize_t bufferSize;
	auto pBuffer = CCFileUtils::sharedFileUtils()->getFileData(filePath.c_str(), "rb", &bufferSize);
	if (!pBuffer)
	{
		return false;
	}

	//填充protobuf
	mBattleRecord_proto.Clear();
	bool bParse = mBattleRecord_proto.ParseFromArray(pBuffer, bufferSize);
	if (!bParse)
	{
		return false;
	}

	mLastPlaybackPoint = pointId;

	return true;
}

void BattleRecordsManager::onReceiveMassage(GameMessage * message)
{
	bool isPvp = sgBattleControl()->getIsPvp();
	if (getIsPlayback() || sgBattleControl()->getIsPvp())
	{
		return;
	}

	fgame::BattleRecord *battleRecord = NULL;

	switch (message->getMessageType())
	{
	case EMT_MOVE:
	{
		MessageMove* pMes = dynamic_cast<MessageMove*>(message);

		int src = pMes->getFromBuild();
		int dest = pMes->getToBuild();
		int formation = pMes->getFormationId();
		int numType = pMes->getNumType();
		int num = pMes->getNumValue();

		battleRecord = mBattleRecord_proto.add_mvecbattlerecords();

		//赋值
		battleRecord->set_mtimeinterval(mCommondInterval);
		battleRecord->set_mcommondtype(PVE_RECORD_MOVE);
		battleRecord->set_msrc(src);
		battleRecord->set_mdest(dest);
		battleRecord->set_mnumtype(numType);
		battleRecord->set_mnum(num);

		setNowBuildInfo(battleRecord);

		mCommondInterval = 0;
	}
		break;
	case EMT_UPGRADE:
	{
		MessageUpgrade* pMes = dynamic_cast<MessageUpgrade*>(message);

		int buildId = pMes->getBuild();

		battleRecord = mBattleRecord_proto.add_mvecbattlerecords();

		//赋值
		battleRecord->set_mtimeinterval(mCommondInterval);
		battleRecord->set_mcommondtype(PVE_RECORD_UPGRADE);
		battleRecord->set_mbuildid(buildId);

		setNowBuildInfo(battleRecord);

		mCommondInterval = 0;
	}
		break;
	case EMT_PROP_CHANGE:
	{
		MessageBuildChange* pMes = dynamic_cast<MessageBuildChange*>(message);

		int buildId = pMes->getBuild();

		battleRecord = mBattleRecord_proto.add_mvecbattlerecords();

		//赋值
		battleRecord->set_mtimeinterval(mCommondInterval);
		battleRecord->set_mcommondtype(PVE_RECORD_PROP_CHANGE);
		battleRecord->set_mbuildid(buildId);

		setNowBuildInfo(battleRecord);

		mCommondInterval = 0;
	}
		break;
	case EMT_PROP_CHANGEOK:
	{
		MessageBuildChangeOk* pMes = dynamic_cast<MessageBuildChangeOk*>(message);

		int keyId = pMes->getBuild();
		int changeType = pMes->getChangeType();
		int isNew = pMes->getisNew();

		battleRecord = mBattleRecord_proto.add_mvecbattlerecords();

		//赋值
		battleRecord->set_mtimeinterval(mCommondInterval);
		battleRecord->set_mcommondtype(PVE_RECORD_PROP_CHANGEOK);
		battleRecord->set_mkeyid(keyId);
		battleRecord->set_mchangetype(changeType);
		battleRecord->set_misnew(isNew);

		setNowBuildInfo(battleRecord);

		mCommondInterval = 0;
	}
		break;
	case EMT_PROP_ADDTIME:
	{
		MessageAddTime* pMes = dynamic_cast<MessageAddTime*>(message);
		
		float duration = pMes->time;

		battleRecord = mBattleRecord_proto.add_mvecbattlerecords();

		//赋值
		battleRecord->set_mtimeinterval(mCommondInterval);
		battleRecord->set_mcommondtype(PVE_RECORD_PROP_ADDTIME);
		battleRecord->set_mduration(duration);

		setNowBuildInfo(battleRecord);

		mCommondInterval = 0;
	}
		break;
	case EMT_PROP_FULLPEOP:
	{
		MessageFullPeopel* pMes = dynamic_cast<MessageFullPeopel*>(message);

		int buildId = pMes->getBuildId();
		int camp = pMes->getCamp();

		battleRecord = mBattleRecord_proto.add_mvecbattlerecords();

		//赋值
		battleRecord->set_mtimeinterval(mCommondInterval);
		battleRecord->set_mcommondtype(PVE_RECORD_PROP_FULLPEOP);
		battleRecord->set_mbuildid(buildId);
		battleRecord->set_mcamp(camp);

		setNowBuildInfo(battleRecord);

		mCommondInterval = 0;
	}
		break;
	case EMT_PROP_FREEZE:
	{
		//no params
		battleRecord = mBattleRecord_proto.add_mvecbattlerecords();

		//赋值
		battleRecord->set_mtimeinterval(mCommondInterval);
		battleRecord->set_mcommondtype(PVE_RECORD_PROP_FREEZE);

		setNowBuildInfo(battleRecord);

		mCommondInterval = 0;
	}
		break;
	case EMT_PROP_COOKING:
	{
		MessageCooking* pMes = dynamic_cast<MessageCooking*>(message);

		float x = pMes->x;
		float y = pMes->y;

		battleRecord = mBattleRecord_proto.add_mvecbattlerecords();

		//赋值
		battleRecord->set_mtimeinterval(mCommondInterval);
		battleRecord->set_mcommondtype(PVE_RECORD_PROP_COOKING);
		battleRecord->set_mx(x);
		battleRecord->set_my(y);

		setNowBuildInfo(battleRecord);

		mCommondInterval = 0;
	}
		break;
	case EMT_RECORD_CREEPS_BORN:	//新增消息类型
	{
		MessageRecordCreepsBorn* pMes = dynamic_cast<MessageRecordCreepsBorn*>(message);

		int creepsId = pMes->getCreepsId();
		int hittedCreepsID = pMes->getHittedCreepsID();
		unsigned int uHitIndex = pMes->getHitIndex();

		battleRecord = mBattleRecord_proto.add_mvecbattlerecords();

		//赋值
		battleRecord->set_mtimeinterval(mCommondInterval);
		battleRecord->set_mcommondtype(PVE_RECORD_CREEPS_BORN);
		battleRecord->set_mcreepsid(creepsId);
		battleRecord->set_mhittedcreepsid(hittedCreepsID);
		battleRecord->set_mhitindex(uHitIndex);

		setNowBuildInfo(battleRecord);

		mCommondInterval = 0;
	}
		break;
	case EMT_RECORD_OUT_PERCENT:
	{
		MessageRecordOutPercent* pMes = dynamic_cast<MessageRecordOutPercent*>(message);

		int outPercent = pMes->getOutPercent();

		battleRecord = mBattleRecord_proto.add_mvecbattlerecords();

		//赋值
		battleRecord->set_mtimeinterval(mCommondInterval);
		battleRecord->set_mcommondtype(PVE_RECORD_OUT_PERCENT);
		battleRecord->set_moutpercent(outPercent);

		mCommondInterval = 0;
	}
	break;
	case EMT_RECORD_TURRET_UPDATE:
	{
		MessageRecordTurretUpdate* pMes = dynamic_cast<MessageRecordTurretUpdate*>(message);

		int mBuildId = pMes->getBuildId();
		int mStep	 = pMes->getStep();

		battleRecord = mBattleRecord_proto.add_mvecbattlerecords();

		//赋值
		battleRecord->set_mtimeinterval(mCommondInterval);
		battleRecord->set_mcommondtype(PVE_RECORD_TURRET_UPDATE);
		battleRecord->set_mbuildid(mBuildId);
		battleRecord->set_mturretstep(mStep);
	
		mCommondInterval = 0;
	}
	break;
	default:
		break;
	}
}
