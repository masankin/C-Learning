
#include "MapCustomData.h"

USING_NS_CC;

MapBuildingData* MapBuildingData::create()
{
	MapBuildingData* pRet = new MapBuildingData();
	if (pRet)
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		return nullptr;
	}
}

MapBuildingData::MapBuildingData() : mBuildingId(0)
{

}


MapBuildingData::~MapBuildingData()
{

}
