
#pragma once

#include <map>
#include "Constant.h"
#include "battle/Map/MapObject.h"
#include "data/TableManager.h"

#include "cocos2d.h"
USING_NS_CC;

struct BuildInfo
{
	MapObject* mapObj;

	//int x;
	//int y;
	//int width;
	//int height;

	int locationID;
	int id;			// BuildAttr.csv key id
	int unit;
	eCampType camp;
	eExpressType express;
	int type;		// eBuildType
	int insID;		// BuildIns.csv key id
	int levelLimit;  //¿ÉÒÔ´ïµ½×î¸ßµÄµÈ¼¶
	void init(int insKey)
	{
		const BuildIns* pInfo = sgTableManager()->getBuildIns(insKey);
		if (!pInfo)
		{
			CCLOG("[error] BuildInfo::init no insKey:%d.", insKey);
			return;
		}

		id = pInfo->type;
		type = id / 1000;
		insID = insKey;
		camp = (eCampType)pInfo->initCamp;
		express = (eExpressType)pInfo->initExpress;
		unit = pInfo->initPeople;
		levelLimit = pInfo->levelLimit;
	} 

	void init(BuildIns* pInfo)
	{
		if (!pInfo)
		{
			CCLOG("[error] BuildInfo::init no insKey:%d.", pInfo->id);
			return;
		}

		id = pInfo->type;
		type = id / 1000;
		insID = pInfo->id;
		camp = (eCampType)pInfo->initCamp;
		express = (eExpressType)pInfo->initExpress;
		unit = pInfo->initPeople;
	}
};

class BaseBuild;
class Camp;
typedef std::map<int, BaseBuild*> MapBulidList;	//<id, BuildBase*>
typedef std::map<eCampType, Camp*> CampList;	//<camp id, camp>


class Policy;
typedef std::map<eCampType, Policy*>	CampAIPolicyMap;	//<camp id, policy>

class FormationV2;
typedef cocos2d::Map<unsigned int, FormationV2*> Vect_Formation;	//<formation id, formation>



struct OilPoint
{
	OilPoint() :mId(0), mRadiusA(0), mRadiusB(0), mPos(Vec2::ZERO), mSpeed(0.5f)
	{}

	unsigned int mId;
	int mRadiusA;
	int mRadiusB;
	float mSpeed;
	Vec2 mPos;
};

typedef std::map<unsigned int, OilPoint> OilPointMap;

