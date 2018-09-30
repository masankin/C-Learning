#include "AStarNode.h"
#include "MapObject.h"

AStarNode::AStarNode() : mParent(nullptr)
, mMapObj(nullptr)

, mG(0)
, mH(0)
{

}


AStarNode::~AStarNode()
{

}


AStarNode * AStarNode::create(MapObject* mapObj)
{
	AStarNode *ret = new AStarNode();
	if (ret && ret->init(mapObj))
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		delete ret;
		ret = nullptr;
		return ret;
	}
}


bool AStarNode::init(MapObject* mapObj)
{
	if (nullptr == mapObj)
		return false;

	mMapObj = mapObj;

	return true;
}


void AStarNode::calculateH(const cocos2d::Point & endPt)
{
	mH = mMapObj->getPosition().getDistance(endPt);
}


MapObject* AStarNode::getObj() const
{
	return mMapObj;
}
