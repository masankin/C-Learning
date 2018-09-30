#pragma once

#include "cocos2d.h"

class MapCustomData : public cocos2d::Ref
{
protected:
	MapCustomData() {}
	virtual ~MapCustomData() {}
};



class MapBuildingData : public MapCustomData
{
public:
	static MapBuildingData* create();

	inline void setBuildingId(int val) { mBuildingId = val; }
	inline int getBuildingId() const { return mBuildingId; }

protected:
	MapBuildingData();
	virtual ~MapBuildingData();

protected:
	int mBuildingId;
};
