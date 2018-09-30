#pragma once

#include "cocos2d.h"

class MapObject;

class AStarNode : public cocos2d::Ref
{
public:
	static AStarNode* create(MapObject* mapObj);

	inline void calculateG(float parentG, float cost) { mG = parentG + cost; }
	void calculateH(const cocos2d::Point & endPt);

	inline void setParent(AStarNode * parent) { mParent = parent; }
	inline AStarNode * getParent() const { return mParent; }

	inline float getG() const { return mG; }
	inline float getH() const { return mH; }
	inline float getF() const { return mG + mH; }

	MapObject* getObj() const;

protected:
	AStarNode();
	virtual ~AStarNode();

	bool init(MapObject* mapObj);

protected:
	AStarNode * mParent;
	
	float mG;
	float mH;

	MapObject * mMapObj;

private:
	CC_DISALLOW_COPY_AND_ASSIGN(AStarNode);
};
