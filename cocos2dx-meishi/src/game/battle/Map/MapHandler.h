#pragma once

#include <deque>
#include <unordered_map>


#include "Singleton.h"
#include "BattleInfo.h"

#include "cocos2d.h"

USING_NS_CC;
 
class AStarNode;
class MapObject;

class MapHandler
{
public:

	MapHandler();
	~MapHandler();

	void createMap(std::string szFile);
	void cleanMapData();

	TMXTiledMap* getTitleMap() const { return mTileMap; }

	std::list<Vec2> searchPath(const Vec2& being, const Vec2& end);
	std::list<Point> searchPath(MapObject* pBegObj, MapObject* pEndObj);
	std::list<Point> searchPath(int begObjId, int endObjId);

	MapObject* getMapObject(int objUid) const;
	const std::map<int, MapObject*>& getMapObjectList() const { return mMapObjectList; }

	const CampList&	getCampList() { return mCampList; }

protected:
	bool parseMapData(TMXTiledMap* tileMap);
	void loadBuildInfo();

	Vec2 tileCoordForPosition(const Vec2&);
	Vec2 tileConvertGlPosition(const Vec2&);

	bool checkCollision(const Vec2&) const{ return false; }
	bool checkRoadCanPass(Vec2 begin, Vec2 end, int count) {return false;}

protected:
	TMXTiledMap* mTileMap;

	std::map<int, MapObject*>  mMapObjectList;

	CampList mCampList;
	
};

#define  sgMapHandler() Singleton<MapHandler>::Get()
