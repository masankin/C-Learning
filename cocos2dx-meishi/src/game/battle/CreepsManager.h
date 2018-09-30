#pragma once

#include <map>
#include <set>

#include "cocos2d.h"
#include "data/TableManager.h"

class CreepsManager
{
public :
	typedef std::function<bool(const Creeps*)> RefreshCallBack;


public:
	CreepsManager();
	~CreepsManager();

	void cleanup();
	void update(float dt);

	void creepsBorn(int creepsId, int hittedCreepsId);	//Õ½¶·»Ø·Å×¨ÓÃ

	void addCreepsPoint(int id, const RefreshCallBack& callback);
	void resetCreepPoint(int id);
	void occupyCreepPoint(int id);

	void syncPvpCreepsInfo(int creepsPtId, int creepsId);

protected:
	struct CreepsPointInfo
	{
		CreepsPointInfo(const CreepsPoint* pData, const RefreshCallBack& callback);
		~CreepsPointInfo();
		bool init();

		const CreepsPoint* mData;
		RefreshCallBack mCallback;
		std::string mTimerKeyName;
		std::vector<const Creeps*> mCreepsList;
		int mHittedCreepsID;
	};

	struct GroupInfo
	{
		GroupInfo();
		~GroupInfo();

		int mGroupId;
		std::string mTimerKey;
		std::vector<CreepsPointInfo*> mCreepsPtList;
	};

	bool refreshGroup(GroupInfo* pInfo);

	CreepsPointInfo* getCreepsPoint(int id) const;
	bool refreshCreepsPoint(CreepsPointInfo* pPointInfo);

	bool isCreepsExist(int creepsId) const;
	int getExistCreepsCount() const;
	void addCreepsToExistingList(int creepsId);
	void removeCreepsFromExistingList(int creepsId);

protected:
	std::map<int, GroupInfo*> mGroupList;

	std::map<int, CreepsPointInfo*> mCreepsPointList;

	std::set<int> mExistingCreepsList;

};
