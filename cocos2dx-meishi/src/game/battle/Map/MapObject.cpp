#include "MapObject.h"

USING_NS_CC;

MapObject* MapObject::create(int mapUid, Type type, const cocos2d::Point& position, cocos2d::Size& size, float rotAngle)
{
	MapObject* pRet = new MapObject();
	if (pRet && pRet->init(mapUid, type, position, size, rotAngle))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return pRet;
	}
}


MapObject::MapObject() : mUid(0)
, mType(Type::eNone)
, mRotation(0.0f)

, mVisible(true)

, mRadiusA(0.0f)
, mRadiusB(0.0f)

, mCustomData(nullptr)

{
#if (MGAME_MAP_OBJ_DEBUG > 0)
	mDebugNode = nullptr;
#endif
}


MapObject::~MapObject()
{
	this->dispose();
}


bool MapObject::init(int mapUid, Type type, const cocos2d::Point& position, cocos2d::Size& size, float rotAngle)
{
	if (0 == mapUid)
		return false;

	if (Type::eNone == type)
	{
		CCLOG("[error]MapObject::init Object type is none");
		return false;
	}

	mUid = mapUid;
	mType = type;

	Point pivot(position.x, position.y + size.height);
	Point center(position.x + size.width / 2.0f, position.y + size.height / 2.0f);

	float rotAngleByRadian = CC_DEGREES_TO_RADIANS(rotAngle);
	mPosition = center.rotateByAngle(pivot, -rotAngleByRadian);
	mSize = size;
	mRotation = rotAngle;

	mRadiusA = size.width / 2.0f;
	mRadiusB = size.height / 2.0f;

	mHalfSizeSQ = Size(mRadiusA * mRadiusA, mRadiusB * mRadiusB);
	//mRotVec = Vec2::forAngle(rotAngleByRadian);

	mCordY = Point(0.0f, 1.0f).rotate(Vec2::forAngle(-rotAngleByRadian));
	mCordX = mCordY.getRPerp();

	mLTPoint = mPosition + (-mCordX * mRadiusA + mCordY * mRadiusB);
	mRTPoint = mPosition + (mCordX * mRadiusA + mCordY * mRadiusB);
	mLBPoint = mPosition + (-mCordX * mRadiusA - mCordY * mRadiusB);
	mRBPoint = mPosition + (mCordX * mRadiusA - mCordY * mRadiusB);

	mLocalTransform.translate(mPosition.x, mPosition.y, 0.0f);
	mLocalTransform.rotateZ(-rotAngleByRadian);
	mLocalTransform.inverse();

#if (MGAME_MAP_OBJ_DEBUG > 0)
	mDebugNode = DebugNode::create(this);
	if (mDebugNode)
	{
		mDebugNode->setPosition(this->getPosition());
		mDebugNode->setRotation(this->getRotation());

		mDebugNode->retain();
	}

#endif

	CCLOG("[trace]MapObject::init Object(%d) type %f, position[%f, %f], size[%f, %f], rotation %f", mUid, mType, mPosition.x, mPosition.y, mSize.width, mSize.height, mRotation);

	return true;
}


void MapObject::dispose()
{
	mUid = 0;
	mType = Type::eNone;

	mPosition = Point::ZERO;
	mSize = Size::ZERO;
	mRotation = 0.0f;

	mVisible = false;

	mCordX = Vec2::ZERO;
	mCordY = Vec2::ZERO;

	mLTPoint = Point::ZERO;
	mRTPoint = Point::ZERO;
	mLBPoint = Point::ZERO;
	mRBPoint = Point::ZERO;

	mRadiusA = 0.0f;
	mRadiusB = 0.0f;

	//mRotVec = Vec2::ZERO;
	mHalfSizeSQ = Size::ZERO;

	mLocalTransform.setIdentity();

	mAdjcencyList.clear();

	if (mCustomData)
		mCustomData->release();
	mCustomData = nullptr;

#if (MGAME_MAP_OBJ_DEBUG > 0)
	if (mDebugNode)
		mDebugNode->release();
#endif
}


void MapObject::addAdjcency(MapObject* adjcy)
{
	if (nullptr == adjcy)
		return;

	for (auto & obj : mAdjcencyList)
	{
		if (nullptr != obj && obj->getUid() == adjcy->getUid())
		{
			CCLOG("[warn]MapObject::addAdjcency obj is existed, uid : %d", adjcy->getUid());
			return;
		}
	}

	mAdjcencyList.push_back(adjcy);
}


Point MapObject::convertWorldToLocal(const cocos2d::Point& pt)
{
	//return mPosition + (pt - mPosition).rotate(mRotVec);
	Vec3 temp(pt.x, pt.y, 0.0f);
	mLocalTransform.transformPoint(&temp);

	return Point(temp.x, temp.y);
}


bool MapObject::containsPoint(const cocos2d::Point& pt)
{
	Point localPos = std::move(convertWorldToLocal(pt));
	
	//if ((powf(localPos.x - mPosition.x, 2) / mHalfSizeSQ.width + powf(localPos.y - mPosition.y, 2) / mHalfSizeSQ.height) <= 1.0f)
	//	return true;

	switch (mType)
	{
	case MapObject::eNone:
		break;
	case MapObject::eBuilding:
	{		
		if ((localPos.x * localPos.x / mHalfSizeSQ.width + localPos.y * localPos.y / mHalfSizeSQ.height) <= 1.0f)
		return true;
	}
	break;

	case MapObject::eWayPoint:
	{

	}
	break;

	case MapObject::eObstacle:
	{
		if (Rect(-mRadiusA, -mRadiusB, mSize.width, mSize.height).containsPoint(localPos))
			return true;
	}
	break;

	default:
		break;
	}
	
	return false;
}


void MapObject::setMapCustomData(MapCustomData * customData)
{
	if (mCustomData)
		mCustomData->release();

	mCustomData = customData;

	if (mCustomData)
		mCustomData->retain();
}


#if (MGAME_MAP_OBJ_DEBUG > 0)

DebugNode* DebugNode::create(MapObject* obj)
{
	auto pRet = new DebugNode();
	if (pRet && pRet->init(obj))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return pRet;
	}
}


bool DebugNode::init(MapObject* obj)
{
	if (nullptr == obj)
		return false;

	mObj = obj;

	return true;
}


void DebugNode::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(DebugNode::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}

void DebugNode::onDraw(const Mat4 &transform, uint32_t flags)
{
	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);


	// draw 4 small points
	//Vec2 points[] = { Vec2(60, 60), Vec2(70, 70), Vec2(60, 70), Vec2(70, 60) };

	auto& size = mObj->getSize();
	Vec2 point(0, 0);
	DrawPrimitives::setPointSize(4);
	DrawPrimitives::setDrawColor4B(128, 255, 255, 255);
	DrawPrimitives::drawPoints(&point, 1);

	switch (mObj->getType())
	{
	case MapObject::eBuilding:
	{
		DrawPrimitives::drawCircle(point, size.width / 2.0f, 360.0f, 180, false, 1.0f, size.height / size.width);
	}
	break;

	case MapObject::eWayPoint:
	{
		DrawPrimitives::drawCircle(point, size.width / 2.0f, 360.0f, 180, false, 1.0f, size.height / size.width);
	}
	break;

	case MapObject::eObstacle:
	{
		DrawPrimitives::drawRect(Point(-mObj->getRadiusA(), -mObj->getRadiusB()), Point(mObj->getRadiusA(), mObj->getRadiusB()));
	}
	break;

	default:
		break;
	}

	//end draw
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

#endif
