#pragma once
#include "cocos2d.h"

class BaseSyncData
{
public:
	BaseSyncData(){};
	virtual ~BaseSyncData(){};
	virtual std::string getSyncData();
	virtual void clearData(){};
};


//同步使用道具
class SyncPropUseData: public BaseSyncData
{
public:
	SyncPropUseData();
	~SyncPropUseData();
	void addDPropata(int propId, int count = -1);
	virtual std::string getSyncData();
	void clearData();

};


//同步关卡
class SyncCheckPointData: public BaseSyncData
{
public:
	SyncCheckPointData();
	~SyncCheckPointData();
	void addCheckPointData(int levelId, int star, int passTime);
	virtual std::string getSyncData();
	void clearData();
};



 
