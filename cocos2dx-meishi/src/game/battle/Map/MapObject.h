#pragma once

#include "cocos2d.h"
#include "MapCustomData.h"
#include "EnvSet.h"

#define MGAME_MAP_OBJ_DEBUG 0

#if (MGAME_MAP_OBJ_DEBUG > 0)
class DebugNode;
#endif


class MapObject : public cocos2d::Ref
{
public:
	enum Type
	{
		eNone = 0,
		eBuilding,
		eWayPoint,
		eObstacle,
	};

public:
	static MapObject* create(int mapUid, Type type, const cocos2d::Point& position, cocos2d::Size& size, float rotAngle);

	inline int getUid() const { return mUid; }
	inline Type getType() const { return mType; }
	inline const cocos2d::Point& getPosition() const { return mPosition; }
	inline const cocos2d::Size& getSize() const { return mSize; }
	inline const float getRotation() const { return mRotation; }

	inline const cocos2d::Vec2& getCordX() const { return mCordX; }
	inline const cocos2d::Vec2& getCordY() const { return mCordY; }

	inline const cocos2d::Point& getLTPoint() const { return mLTPoint; }
	inline const cocos2d::Point& getRTPoint() const { return mRTPoint; }
	inline const cocos2d::Point& getLBPoint() const { return mLBPoint; }
	inline const cocos2d::Point& getRBPoint() const { return mRBPoint; }

	inline float getRadiusA() const { return mRadiusA; }
	inline float getRadiusB() const { return mRadiusB; }

	cocos2d::Point convertWorldToLocal(const cocos2d::Point& pt);
	bool containsPoint(const cocos2d::Point& pt);

	void addAdjcency(MapObject* adjcy);
	const std::vector<MapObject*>& getAdjcencyList() const { return mAdjcencyList; }

	inline bool isVisible() const{ return mVisible; }
	inline void setVisible(bool val) { mVisible = val; }

	inline MapCustomData* getMapCustomData() const { return mCustomData; }
	void setMapCustomData(MapCustomData * customData);

protected:
	MapObject();
	virtual ~MapObject();

	virtual bool init(int mapUid, Type type, const cocos2d::Point& position, cocos2d::Size& size, float rotAngle);
	virtual void dispose();

protected:
	int mUid;
	Type mType;
	cocos2d::Point mPosition;
	cocos2d::Size mSize;
	float mRotation;

	bool mVisible;

	cocos2d::Vec2 mCordX;
	cocos2d::Vec2 mCordY;
	cocos2d::Point mLTPoint;
	cocos2d::Point mRTPoint;
	cocos2d::Point mLBPoint;
	cocos2d::Point mRBPoint;
	float mRadiusA;
	float mRadiusB;

	//cocos2d::Vec2 mRotVec;
	cocos2d::Size mHalfSizeSQ;

	cocos2d::Mat4 mLocalTransform;

	std::vector<MapObject*> mAdjcencyList;

	MapCustomData* mCustomData;

#if (MGAME_MAP_OBJ_DEBUG > 0)
public:
	DebugNode* getDebugNode() const { return mDebugNode; }

private:
	DebugNode* mDebugNode;
#endif
};



#if (MGAME_MAP_OBJ_DEBUG > 0)
class DebugNode : public cocos2d::Node
{
public:
	static DebugNode* create(MapObject* obj);

protected:
	bool init(MapObject* obj);

private:
	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
	void onDraw(const cocos2d::Mat4& transform, uint32_t flags);

	cocos2d::CustomCommand _customCommand;

	MapObject * mObj;
};
#endif
