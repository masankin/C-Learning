#include "MapHandler.h"
#include "MapObject.h"
#include "AStarNode.h"
#include "HelperFunc.h"

#include "astar_algorithm.h"

#include "battle/Build.h"
#include "battle/Camp.h"
#include "data/TableManager.h"


MapHandler::MapHandler() : mTileMap(nullptr)
{

}


MapHandler::~MapHandler()
{
	cleanMapData();
}


void MapHandler::createMap(std::string szFile)
{
	//szFile = "pvp_4_1.tmx";
	//szFile = "testmap2.tmx";

	mTileMap = TMXTiledMap::create(szFile);
	if (nullptr == mTileMap)
	{
		CCLOG("[error]MapHandler::createMap Create tilemap falied, filename is %s", szFile.c_str());
		return;
	}

	mTileMap->retain();

	parseMapData(mTileMap);

	loadBuildInfo();
}


void MapHandler::cleanMapData()
{
	CCLOG("[trace] MapHandler::cleanMapData called.");

	for (auto it = mCampList.begin(); it != mCampList.end(); it++)
	{
		delete it->second;
	}
	mCampList.clear(); 

	for (auto it = mMapObjectList.begin(); it != mMapObjectList.end(); ++it)
	{
		MapObject* obj = it->second;
		if (obj)
			obj->release();			
	}
	mMapObjectList.clear();

	if (mTileMap)
		mTileMap->release();
	mTileMap = nullptr;
}


bool MapHandler::parseMapData(TMXTiledMap* tileMap)
{
	if (nullptr == tileMap)
		return false;

	auto objGroup = tileMap->getObjectGroup("objects");
	if (nullptr == objGroup)
		return false;

	ValueVector& aryGroup = objGroup->getObjects();
	int arySize = aryGroup.size();

	CCLOG("[trace]MapHandler::parseMapData object group size :%d", arySize);

	std::vector< std::pair<MapObject*, std::string> > objsAdjList;

	// Get info from object layer
	for (int index = 0; index < arySize; ++index)
	{
		ValueMap& objData = aryGroup.at(index).asValueMap();

		int objId = objData["ObjUid"].asInt();
		std::string objAdjStr = objData["ObjAdjList"].asString();
		float objRot = objData["rotation"].asFloat();
		std::string objTypeName = objData["type"].asString();

		MapCustomData* customData = nullptr;
		MapObject::Type objType = MapObject::eNone;
		if ("BD" == objTypeName)
		{
			objType = MapObject::eBuilding;

			MapBuildingData * pBuildingData = MapBuildingData::create();
			if (nullptr != pBuildingData)
			{
				pBuildingData->setBuildingId(objData["BuildId"].asInt());
			}
			customData = pBuildingData;
		}
		else if ("WP" == objTypeName)
		{
			objType = MapObject::eWayPoint;
		}
		else if ("OC" == objTypeName)
		{
			objType = MapObject::eObstacle;
		}

		Point objPos;
		objPos.x = objData["x"].asFloat();
		objPos.y = objData["y"].asFloat();

		Size objSize;
		objSize.width = objData["width"].asFloat();
		objSize.height = objData["height"].asFloat();

		MapObject* mapObj = MapObject::create(objId, objType, objPos, objSize, objRot);
		if (nullptr == mapObj)
		{
			CCLOG("[error]MapHandler::parseMapData create failed, map object uid : %d", objId);
			continue;
		}

		mapObj->retain();
		mapObj->setMapCustomData(customData);
		mMapObjectList.insert(std::make_pair(mapObj->getUid(), mapObj));
		objsAdjList.push_back(std::make_pair(mapObj, objAdjStr));
	}

	// Build Adjacency list
	bool autoPath = tileMap->getProperty("AutoPath").asBool();
	if (autoPath)
	{
		for (unsigned int i = 0; i < objsAdjList.size(); ++i)
		{
			auto & srcMapObj = objsAdjList[i].first;
			if (nullptr == srcMapObj)
				continue;

			for (unsigned int j = 0; j < objsAdjList.size(); ++j)
			{
				auto & dstMapObj = objsAdjList[j].first;
				if (nullptr == dstMapObj || dstMapObj == srcMapObj)
					continue;

				srcMapObj->addAdjcency(dstMapObj);
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < objsAdjList.size(); ++i)
		{
			auto & srcMapObj = objsAdjList[i].first;
			if (nullptr == srcMapObj)
				continue;

			std::string& strAdjcencyList = objsAdjList[i].second;
			if (strAdjcencyList.empty())
				continue;

			std::vector<int> adjList = std::move(HelperFunc::tokenize_2int(strAdjcencyList, ","));
			for (auto uid : adjList)
			{
				MapObject* dstMapObj = this->getMapObject(uid);
				if (nullptr == dstMapObj || dstMapObj == srcMapObj)
					continue;

				srcMapObj->addAdjcency(dstMapObj);
			}
		}
	}

	return true;
}


void MapHandler::loadBuildInfo()
{
	mCampList.insert(std::make_pair(eCampType::ECAMP_TYPE_Neutral, Camp::createCamp(ECAMP_TYPE_Neutral)));

	for (auto it = mMapObjectList.begin(); it != mMapObjectList.end(); ++it)
	{
		MapObject * obj = it->second;
		if (nullptr == obj)
			continue;

		if (MapObject::Type::eBuilding != obj->getType())
			continue;

		MapBuildingData * pBuildingData = dynamic_cast<MapBuildingData*>(obj->getMapCustomData());
		if (nullptr == pBuildingData)
			continue;

		const BuildIns* pBuildInsData = sgTableManager()->getBuildIns(pBuildingData->getBuildingId());
		if (!pBuildInsData)
			continue;

		BuildInfo info;
		info.init(pBuildInsData->id);
		info.mapObj = obj;

		Camp* pCamp = nullptr;

		CampList::iterator itCamp = mCampList.find(info.camp);
		if (itCamp != mCampList.end())
		{
			pCamp = itCamp->second;
		}
		else
		{
			pCamp = Camp::createCamp(info.camp);
			mCampList.insert(std::make_pair(info.camp, pCamp));
		}
		

		if (pCamp)
			pCamp->addBuild(&info);
	}
}


cocos2d::Vec2 MapHandler::tileCoordForPosition(const Vec2& ptPos)
{
	Vec2 pt;
	const Size& tileSize = mTileMap->getTileSize();
	const Size& mapSize = mTileMap->getMapSize();

	pt.x = ptPos.x / tileSize.width;
	pt.y = (tileSize.height * mapSize.height - ptPos.y) / tileSize.height;

	pt.x = (int)pt.x;
	pt.y = (int)pt.y;


	return pt;
}


cocos2d::Vec2 MapHandler::tileConvertGlPosition(const Vec2& ptPos)
{
	const Size& tileSize = mTileMap->getTileSize();
	const Size& mapSize = mTileMap->getMapSize();

	Vec2 pt;
	pt.x = ptPos.x * tileSize.width;
	pt.y = tileSize.height * mapSize.height - tileSize.height * ptPos.y;

	return pt;
}


std::list<Vec2> MapHandler::searchPath(const Vec2& _being, const Vec2& _end)
{
	st_StarNode* node_list = NULL;
	Vec2 stat = tileCoordForPosition(_being);
	Vec2 end = tileCoordForPosition(_end);

	const Size& mapSize = mTileMap->getMapSize();
	node_list = a_star_search(mapSize.width, mapSize.height, stat.x, stat.y, end.x, end.y);

	std::list<Vec2> list;
	if (node_list != NULL)
	{
		st_StarNode* p = node_list;
		while (p)
		{
			Vec2 point = tileConvertGlPosition(Vec2(p->i, p->j));
			list.push_front(point);

			p = p->parent;
		}

		Vec2 begin = list.front();
		Vec2 end = list.back();
		int count = list.size();
		if (checkRoadCanPass(begin,end,count))
		{
			Vec2 dis = (end - begin) / count;
			list.clear();
			for (int i = 0 ; i <  count ; i++)
			{
				Vec2 pStep = begin + dis * i;
				list.push_back(pStep);
			}
		}
	}

	//CCLOG("[trace] SearchPath list size: %d", list.size());
	return list;
}


std::list<Point> MapHandler::searchPath(int begObjId, int endObjId)
{
	MapObject* pBegObj = this->getMapObject(begObjId);
	MapObject* pEndObj = this->getMapObject(endObjId);

	return this->searchPath(pBegObj, pEndObj);
}


std::list<Point> MapHandler::searchPath(MapObject* pBegObj, MapObject* pEndObj)
{
	std::list<Point> path;

	if (nullptr == pBegObj || nullptr == pEndObj)
		return path;


	cocos2d::Point begPos = pBegObj->getPosition();
	cocos2d::Point endPos = pEndObj->getPosition();

	std::deque<AStarNode*> openTable;
	std::unordered_map<int, AStarNode*> closeTable;

	AStarNode* pBegNode = AStarNode::create(pBegObj);
	if (nullptr == pBegNode)
	{
		CCLOG("[trace]MapHandler:SearchPath begin node is invalid!");
		return path;
	}

	pBegNode->retain();
	pBegNode->calculateH(endPos);

	// 把起始点加入open列表中
	openTable.push_back(pBegNode);

	// 循环直到OPEN列表为空或者终点加入CLOSE列表中
	std::unordered_map<int, AStarNode*>::iterator endndNodeItr = closeTable.end();
	while (!openTable.empty() && ((endndNodeItr = closeTable.find(pEndObj->getUid())) == closeTable.end()))
	{
		// 调整堆
		std::make_heap(openTable.begin(), openTable.end(), [](const AStarNode* n1, const AStarNode* n2)->bool {

			if (n1->getF() > n2->getF())
				return true;

			return false;
		});

		// 堆顶是最小F值，取出
		auto curNode = *openTable.begin();
		openTable.pop_front();

		MapObject * curObj = curNode->getObj();
		if (nullptr == curObj)
			break;

		// 把取出的放进close列表
		closeTable.insert(std::make_pair(curObj->getUid(), curNode));

		const std::vector<MapObject*>& adjcencyObjList = curObj->getAdjcencyList();
		for (auto adjcencyObj : adjcencyObjList)
		{
			if (nullptr == adjcencyObj)
				continue;

			auto adjcencyObjID = adjcencyObj->getUid();

			// 1：邻居已经在CLOSE表中了，那么不需要考虑
			if (closeTable.end() != closeTable.find(adjcencyObjID))
				continue;

			// 2：邻居是障碍物，不需要考虑
			if (MapObject::Type::eObstacle == adjcencyObj->getType())
				continue;

			//if (!adjcencyObj->isVisible())
			//	continue;

			auto openItr = std::find_if(openTable.begin(), openTable.end(), [&adjcencyObjID](const AStarNode* nodeCheck)->bool {
				if (nullptr == nodeCheck)
					return false;

				if (nullptr == nodeCheck->getObj())
					return false;

				return adjcencyObjID == nodeCheck->getObj()->getUid();
			});


			float costToAdjcency = curObj->getPosition().getDistance(adjcencyObj->getPosition());

			if (openItr == openTable.end())
			{
				//3：邻居不在OPEN表中，那么将邻居加入OPEN

				auto newNode = AStarNode::create(adjcencyObj);
				if (newNode)
				{
					newNode->retain();
					newNode->setParent(curNode);
					newNode->calculateG(curNode->getG(), costToAdjcency);
					newNode->calculateH(endPos);

					openTable.push_back(newNode);
				}

			}
			else
			{
				//4：邻居在OPEN表中，对比从当前点出发到此邻居的G值和此邻居原本的G值
				// 如果从当前点出发到此邻居的G值更小，证明通过当前点到此邻居是最佳路径（同一点的H值相同），更换父节点。
				// 否则有可能证明通过当前点不是最佳路径，需要继续筛选，那么什么不做，继续下一步操作（继续调整和增加邻居 或者 继续下一轮筛选）

				auto & openNode = *openItr;
				if (curNode->getG() + costToAdjcency < openNode->getG())
				{
					openNode->calculateG(curNode->getG(), costToAdjcency);
					openNode->setParent(curNode);
				}
			}
		}
	}

	endndNodeItr = closeTable.find(pEndObj->getUid());
	if (endndNodeItr != closeTable.end())
	{
		AStarNode * endNode = endndNodeItr->second;
		std::vector<cocos2d::Point> pathReverseTilePoints;
		for (AStarNode * node = endNode; node != nullptr; node = node->getParent())
		{
			if (nullptr == node)
				continue;

			if (nullptr == node->getObj())
				continue;

			auto pos = node->getObj()->getPosition();
			pathReverseTilePoints.push_back(pos);
		}

		for (auto rTilePointItr = pathReverseTilePoints.rbegin(); rTilePointItr != pathReverseTilePoints.rend(); ++rTilePointItr)
		{
			path.push_back(*rTilePointItr);
		}
	}

	for (auto& node : openTable)
	{
		node->release();
	}

	for (auto& nodePair : closeTable)
	{
		nodePair.second->release();
	}

	return path;
}


MapObject* MapHandler::getMapObject(int objUid) const
{
	auto findItr = mMapObjectList.find(objUid);
	if (mMapObjectList.end() != findItr)
		return findItr->second;

	return nullptr;
}
