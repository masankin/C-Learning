#include "Formation.h"
#include "game/battle/Solider.h"
#include "Map/MapHandler.h"
#include "GamePrecedure.h"
#include "game/battle/Build.h"
#include "battle/BattleControl.h"
#include "battle/SoliderView.h"
#include "BuildView.h"
#include "core/TimeCalculator.h"
#include "stl_helper.h"
#include "MoraleManager.h"
USING_NS_CC;
#define row_max_people 6
#define  FRAME_RATE 0.01 
Formation::~Formation()
{ 
	//CC_SAFE_DELETE(mSrcBuild);
	CCLOG("===========~Formation()===========");
	SAFE_RELEASE_MAP(_FormationRow);	
}


//创建方的阵营
//@param campType 创建方的阵营
//@param num 创建方的数量
//@param id 创建方的Id
//@param srcBuild 产生阵型的建筑
Formation* Formation::create(eCampType campType, unsigned int num, unsigned id, BaseBuild* pSrc, float time /* = 0 */)
{
	Formation* pFormation = new Formation();
	if (pFormation && pFormation->init(campType, num, id, pSrc, time))
	{
		pFormation->autorelease();
		return pFormation;
	}

	CC_SAFE_DELETE(pFormation);
	return nullptr;
}

bool Formation::init(eCampType campType, unsigned int num, unsigned id, BaseBuild* pSrc,float time)
{
	bool ret = false;
	do
	{
		//_speed = 2.5;
		CC_BREAK_IF(num <= 0);
		CC_BREAK_IF(!pSrc);
		mId = id; 
		//mSrcBuild = pSrc->copyWithTypeWithoutView(pSrc->getBuildType());  
		mSrcBuild = pSrc;
		mContent = num; 
		mExpectNum = num;
		mCamp = campType;

		mMaxRow = num / row_max_people;
		int _val2 = num % row_max_people;
		if (_val2 > 0)
		{
			mMaxRow += 1;
		}

		m_time = sgTimeCalculator()->getCurTime();
		//createRow(MoveInof());
		ret = true;
	} while (0);

	return ret;
}

unsigned int Formation::createId()
{
	static int id =  cocos2d::random(1, 100000);
	double time = sgTimeCalculator()->getSystemTime();
	time = time - (long)time;
	id += time * 100000;
	return id;
}

void Formation::enter(Node* sender)
{
	Map_FormationRow::iterator it = _FormationRow.begin();
	for (; it != _FormationRow.end(); it++)
	{
		it->second->enter(sender);
	}

	mParent = sender;
}

//整个队列的下一步
void Formation::nextPostion()
{
	if (mPause && sgBattleControl()->getIsPvp() && sgTimeCalculator()->getTimeLeft(StringUtils::format("pvp_formation%d", mId)) == 0)
	{
		mPause = false;
		sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("pvp_formation%d", mId));
	}
	
	if (mPause)
	{
		m_time = sgTimeCalculator()->getCurTime();
		return;
	};

	int cout = 0;

	double time = sgTimeCalculator()->getCurTime() - m_time;

	if (time >= FRAME_RATE)
	{
		double offset = 0;
		cout = (time / FRAME_RATE);
		offset -= time - cout * FRAME_RATE;

		m_time = sgTimeCalculator()->getCurTime() - offset;
	}

	for (int i = 0; i < cout; i++)
	{
		Map_FormationRow::iterator it = _FormationRow.begin();
		for (; it != _FormationRow.end();)
		{
			if (it->second->isNext())
			{
				if (it->first < _FormationRow.size())
				{
					MoveInof info = it->second->getMoveInfo();
					if (it->first == _FormationRow.size() - 1)
					{
						if (mSrcBuild && mContent > 0 && checkCanCreateRow())
						{
							createRow();
							Map_FormationRow::iterator newRowit = _FormationRow.find(it->first + 1);
							if (newRowit != _FormationRow.end())
							{
								newRowit->second->addMoveInfo(info);
								newRowit->second->setDefault();
							}
						}
					}

					Map_FormationRow::iterator next = _FormationRow.find(it->first + 1);
					if (next != _FormationRow.end() && next->second->_MoveInfoList.size() < 1)
					{
						next->second->addMoveInfo(info);
						//next->second->NextPostion();
					}
				}
			}

			it->second->NextPostion();
			it++;
		}

	}
	
}

void Formation::pushMove(const Vec2 _postion)
{
	MoveInof info;
	info.aimPos = _postion;

	if (_MoveList.size() <= 0)
	{
		info.currentPos = _postion;
		_MoveList.push_front(info);
		return;
	}
	if (_MoveList.size() == 1)
	{
		MoveInof info2 = _MoveList.front();
		if (info2.maxDis == 0)
		{
			_MoveList.pop_front();
			info = info2;
			info.aimPos = _postion;
		}
		else
			info.currentPos = _MoveList.front().aimPos;
	}
	else
		info.currentPos = _MoveList.front().aimPos;

	Vec2 pos = info.aimPos - info.currentPos;
	int distant = sqrt(pos.x * pos.x + pos.y * pos.y);
	info.maxDis = distant;
	info.cos = pos.x / (float)distant;
	info.sin = pos.y / (float)distant;
	info.rotation = atan2(pos.x, pos.y);

	_MoveList.push_front(info);

}

void Formation::star2end(const Vec2& star, const Vec2& end)
{
	// process start position . By Evix
	auto direction = end - star;
	auto radius = 20.0f;
	auto angle = CC_RADIANS_TO_DEGREES(direction.getAngle());
	if (angle > -30.0f && angle <= 30.0f)
		radius = 40.0f;
	else if (angle > 30 && angle < 150.0f)
		radius = 20.0f;
	else if (angle > -150 && angle <= 30.0f)
		radius = 30.0f;
	else
		radius = 40.0f;

	auto realStartPoint = star + direction.getNormalized() * radius;
	// End. by Evix

	std::list<Vec2> positions = sgMapHandler()->searchPath(realStartPoint, end);
	//std::list<Vec2> positions = sgMapHandler()->SearchPath(star, end);

	if (positions.size() <= 0)return;

	Vec2 pos = positions.front();
	positions.pop_front();

	while (positions.size() > 0)
	{
		this->pushMove(pos);
		pos = positions.front();
		positions.pop_front();
	}
	MoveInof info;
	info.currentPos = end;
	info.aimPos = end;
	info.maxDis = -1;
	info.isEnd = true;
	_MoveList.push_front(info);

	createRow();
	if (_MoveList.size() > 0)
	{
		Map_FormationRow::iterator fIt = _FormationRow.find(0);
		if (fIt != _FormationRow.end())
		{
			do
			{
				MoveInof info = _MoveList.back();
				_MoveList.pop_back();
				fIt->second->addMoveInfo(info);

			} while (_MoveList.size() > 0);

			fIt->second->setDefault();
		}
	}


	//_MoveList.pop_back();
	nextPostion();
}

void Formation::setSrcBuild(BaseBuild* pBuild)
{
	if (pBuild)
	{
		mSrcBuild = pBuild;
		for (auto item : _FormationRow)
		{
			item.second->setSrcBuild(pBuild);
		}
	}
}

void Formation::setTarBuild(BaseBuild* pBuild)
{
	if (pBuild)
	{
		mTarBuild = pBuild;
		for (auto item : _FormationRow)
		{
			item.second->setTarBuild(pBuild);
		}
	}
}

int Formation::getUnit()
{
	int num = 0;
	for (auto item : _FormationRow)
	{
		num += item.second->getUnit();
	}
	return num;
}

void Formation::createRow()
{
	//mContent = sgBattleControl()->getTargetBuild(mSrcBuild->getKeyID())->getOwnPeople();
	int num = mSrcBuild->getFormationRow(mContent); 
	/*char tParam[64] = {0,};
	sprintf(tParam, "%d %d %d", mSrcBuild->getKeyID(), mSrcBuild->getBaseCamp(),  -1 * num);
	sgMessageManager()->sendMessage(EMT_REDUCEPEOPLE, tParam);*/
	if(num <= 0)
	{ 
		Map_FormationRow::iterator iter = _FormationRow.find(_FormationRow.size() - 1);
		if(iter != _FormationRow.end())
		{
			iter->second->setIsLastRow(1);
		}
		return;
	}
	FormationRow* pRow = new FormationRow();
	pRow->setRowID(m_RowIndex);
	m_RowIndex++;
	pRow->setFormationID(getId());
	if(mContent <= 0)
	{
		pRow->setIsLastRow(1); 
		//CCLOG("###########FormationID is %d          rowID is %d", getId(), pRow->getRowID());
	}
	else
	{
		pRow->setIsLastRow(0); 
	}
	
	//CCLOG("FormationID  is %d, and content is %d", getId(),mContent);
	for (int c = 0; c < num; c++)
	{

		//pRow->addSoldier(Soldier::create());//TODO:需要从Itemmanager 获得	

		auto pos_Src = mSrcBuild->getPosition();
		auto pos_Tar = mTarBuild->getPosition();

		bool isFlipX = false;
		int zOrder = EBVZ_BOTTOM_EFFECT + 1;

		if(pos_Src.x > pos_Tar.x)
		{
			isFlipX = true;
		}
		
		if (pos_Src.y > pos_Tar.y)
		{
			zOrder = num - c ;
		}

		//pRow->addSoldier(Solider::create(mCamp, mTarBuild->getCamp(), isFlipX, zOrder));
	}

	if (mParent)
	{
		pRow->enter(mParent);
	}
	if (mTarBuild)
	{


		pRow->setTarBuild(mTarBuild);
	}
	if (mSrcBuild)
	{
		if (mContent <= 0)
			mSrcBuild->changeBuildAni(BUILD_ANI_OUTPUT_END);

		pRow->setSrcBuild(mSrcBuild);
	}
	pRow->mCampType = mCamp;
	pRow->setSpeed(_speed);

	_FormationRow.insert(std::make_pair(_FormationRow.size(), pRow)); 


	//发送阵营攻击打野建筑的消息 
	if(getTarBuild()->getBaseCamp() == ECAMP_TYPE_Creeps)
	{
		char szParam2[64] = { 0, };
		sprintf(szParam2, "%d %d %d %d %d %d %d %d", 
			getSrcBuild()->getKeyID(),
			getTarBuild()->getKeyID(),
			getSrcBuild()->getCamp(),
			getTarBuild()->getCamp(), 
			getId(),
			getSrcBuild()->getBuildInfo().insID,
			getTarBuild()->getBuildInfo().locationID,
			E_JUNGLE_ATTACK);
		sgMessageManager()->sendMessage(EMT_JUNGLE, szParam2);
	} 
	
}

//检测是否被防御塔攻击
//@param pAttack 攻击方防御塔
//@return true 击中
bool Formation::checkBeAttack(BaseBuild* pAttack)
{
	Map_FormationRow::iterator it = _FormationRow.begin();

	for (; it != _FormationRow.end(); it++)
	{
		if (it->second->checkBeAttack(pAttack))
		{
			return true;
		}
	}
	return false;
}

int Formation::getBeAttackKey()
{
	if (!mTarBuild)
	{
		CCLOG("[error]Formation::getBeAttackKey unknow build!");
		return -1;
	}

	// 同阵营转移兵力
	if (mSrcBuild->getCamp() == mTarBuild->getCamp())
		return -1;

	return mTarBuild->getKeyID();
}

bool Formation::checkCanCreateRow()
{
	return _FormationRow.size() < mMaxRow ? true : false;
}


void Formation::changeTarget(BaseBuild* pOld, BaseBuild* pNew)
{
	if (!pOld || !pNew)return;

	if (mSrcBuild == pOld)
		mSrcBuild = pNew;

	if (mTarBuild == pOld)
		mTarBuild = pNew;

	Map_FormationRow::iterator it = _FormationRow.begin();
	for (; it != _FormationRow.end(); it++)
	{
		it->second->changeTarget(pOld,pNew);
	}
}

void Formation::removeSoliderWithRect(const Rect& rect)
{
	Map_FormationRow::iterator it = _FormationRow.begin();
	for (; it != _FormationRow.end(); it++)
	{
		it->second->removeSoliderWithRect(rect);
	}
}


Vec2 Formation::getCenterPos()
{
	Vec2 pos = Vec2::ZERO;
	int cout = 0;
	Map_FormationRow::iterator it = _FormationRow.begin();
	for (; it != _FormationRow.end(); it++)
	{
		Vect_Soldier::const_iterator sit = it->second->getSoldierList().begin();
		for (; sit != it->second->getSoldierList().end(); sit++)
		{
			pos += (*sit)->getPosition();
			cout++;
		}
	}
	pos = (pos / cout);
	return pos;
}

eCampType Formation::getTargetCamp()
{
	return mTarBuild->getBuildInfo().camp;
}

void Formation::changeSoliderIns()
{
// 	Map_FormationRow::iterator it = _FormationRow.begin();
// 	for (; it != _FormationRow.end(); it++)
// 	{
// 		Vect_Soldier::const_iterator sit = it->second->getSoldierList().begin();
// 		for (; sit != it->second->getSoldierList().end(); sit++)
// 		{
// 			(*sit)->getView()->refresh();
// 		}
// 	}
}

void Formation::addPauseTime(float time)
{
	m_time + time;
}


void FormationRow::NextPostion()
{

	if (mMoveInfo.maxDis == 0)
	{
		if (_MoveInfoList.size() <= 0)
			return;

		mMoveInfo = _MoveInfoList.back();
		_MoveInfoList.pop_back();
		next = false;
		mCurrentDis = 0;
		return;
	}
	if (mCurrentDis >= mMoveInfo.maxDis)
	{
		if (mMoveInfo.isEnd && !readyEnd)
		{
			int liveNum = 0;
			for (size_t i = 0; i < mSoldierList.size(); i++)
			{
				if (mSoldierList[i]->getLive())
				{
					liveNum++;
				}
			}
			if (liveNum >= 0)
			{
				//CCLOG("******FormationID is %d and rowID is %d          isLastRow is %d",m_FormationID, m_RowID, m_IsLastRow);
				mTarBuild->calculate(mSrcBuild,mCampType, liveNum, m_IsLastRow, m_FormationID); 
			}
				


			//for (auto it : mSoldierList)
			//{
			//	it->setTargetCamp(mTarBuild->getCamp());
			//	it->remove();
			//	//it->removeSoldier(NULL);
			//}

			Vect_Soldier::iterator it = mSoldierList.begin();
			for (; it != mSoldierList.end(); )
			{
				Solider* pSolider = (*it);
				pSolider->remove();
				it = mSoldierList.erase(it);
				pSolider->release();
				continue;
			}

			mSoldierList.clear();
			readyEnd = true;
		}

		next = true;
		return;
	}

	float x = _speed * mMoveInfo.cos;
	float y = _speed * mMoveInfo.sin;
	const int num = mSoldierList.size();

	for (int i = 0; i < num; i++)
	{
		Solider* pSoldier = mSoldierList.at(i);
		Vec2 soldierPos = pSoldier->getPosition();
		pSoldier->setTargetPosition(Vec2(x, y));

		sgBattleControl()->CheckAndAddBuild(pSoldier, mSrcBuild, mTarBuild, _speed);

		//其他士兵对当前士兵的作用力
		for (int j = 0; j < num; j++)
		{
			//不是同一个士兵
			if (i == j || abs(i - j) != 1)
			{
				continue;
			}

			Solider* pOtherSoldier = mSoldierList.at(j);
			Vec2 othersoldierPos = pOtherSoldier->getPosition();

			//其他士兵对当前士兵施加作用力
			pSoldier->addFs(othersoldierPos, i - j, _speed);
		}


		pSoldier->setPosition(pSoldier->getRealNextPosition());
	}

	//for (int t = 0 ; t< mSoldierList.size() ; t++)
	//{
	//	Solider* pSoldier = mSoldierList.at(t);
	//	//pSoldier->setPosition(pSoldier->getRealNextPosition());
	//}

	mCurrentDis = mCurrentDis + _speed;
}

void FormationRow::addSoldier(Solider* soldier)
{
	if (soldier)
	{
		mSoldierList.push_back(soldier);
	}
}

void FormationRow::addMoveInfo(Formation::MoveInof info)
{
	_MoveInfoList.push_front(info);
}

Formation::MoveInof FormationRow::getMoveInfo()
{
	Formation::MoveInof temp = mMoveInfo;
	if (_MoveInfoList.size() > 0)
	{
		mMoveInfo = _MoveInfoList.back();
		_MoveInfoList.pop_back();
	}
	next = false;
	mCurrentDis = 0;
	return temp;
}

void FormationRow::enter(cocos2d::Node* parent)
{
	for (auto it : mSoldierList)
	{
		it->add(parent);
	}
	
	parent->sortAllChildren();
}

 

//检测是否被防御塔攻击
//@param pAttack 攻击方防御塔
//@return true 攻击成功
//TODO:炮塔攻击
bool FormationRow::checkBeAttack(BaseBuild* pAttack)
{
	Vect_Soldier::iterator it = mSoldierList.begin();

	for (; it != mSoldierList.end(); it++)
	{

		if (pAttack->getBuildType() == EBULID_TYPE_TURRET)
		{

			const BuildAttr* pAttr = sgTableManager()->getBuildAttr(pAttack->getAttrID());
			
			Vec2 attack = pAttack->getPosition();
			Vec2 defense = (*it)->getPosition();
			Vec2 pos = defense - attack;
			//float distant = sqrt(pos.x * pos.x + pos.y * pos.y);

			Size winSize = CCDirector::getInstance()->getWinSize();
			float halfWidth = pAttr->attRadius;
			float halfHeight = (halfWidth / winSize.width) * winSize.height;

			//if (distant < pAttr->attRadius && (*it)->getLive())
			if ((pow(pos.x, 2) / pow(halfWidth, 2) + pow(pos.y, 2) / pow(halfHeight, 2)) <= 1.0f && (*it)->getLive())
			{
				Solider* pSolider = (*it);

				Sprite* pBullet = Sprite::create("bulletNor.png");
				pBullet->setPosition(attack);
				EaseSineIn* pAction = EaseSineIn::create(JumpBy::create(0.3f, pos, 100, 1));
				(*it)->setBullet(pBullet);
				pBullet->runAction(Sequence::create(pAction, CallFunc::create(CC_CALLBACK_0(Solider::remove, *it)), nullptr));
				sgBattleControl()->getBattleView()->addChild(pBullet);
				mSoldierList.erase(it);

				pAttack->getBuildView()->playAni(BUILD_ANI_TURRET_ATTACK);

				//炮塔攻击增加士气 
				sgMoraleManager()->registMoraleMessage(pAttack->getCamp(), 1.0f, E_MORALECOMPUTE_DEFENCE);

				return true;
			}
		}
	}
	return false;

}

void FormationRow::setDefault()
{
	NextPostion();
	//设置初始位置
	float x = mMoveInfo.currentPos.x + _speed * mMoveInfo.cos;
	float y = mMoveInfo.currentPos.y + _speed * mMoveInfo.sin;
	int spaceCloum = 1;
	for (unsigned int i = 0; i < mSoldierList.size(); i++)
	{
		Solider* pSoldier = mSoldierList.at(i);
		pSoldier->setPosition(Vec2(x + i * spaceCloum * mMoveInfo.sin, y - i * spaceCloum* mMoveInfo.cos));
		//pSoldier->setPosition(x,y);
		//pSoldier->setDirectionWithRotiation(mMoveInfo.rotation);
	}

}

int FormationRow::getUnit()
{
	return mSoldierList.size();
}

void FormationRow::changeTarget(BaseBuild* pOld, BaseBuild* pNew)
{
	if (!pOld || !pNew)return;

	if (mSrcBuild == pOld)
		mSrcBuild = pNew;

	if (mTarBuild == pOld)
		mTarBuild = pNew;
}

void FormationRow::removeSoliderWithRect(const Rect& rect)
{
	Vect_Soldier::iterator it = mSoldierList.begin();

	for (; it != mSoldierList.end();)
	{
		Solider* pSolider = (*it);
		if (pSolider && rect.containsPoint(pSolider->getPosition()))
		{

			SoliderView* pView = pSolider->getView();
			auto dealy = DelayTime::create(1.5);

			auto pos=sgBattleControl()->getBattleView()->getFeidieArm()->getPosition();

			auto moveto = MoveTo::create(0.5, Point(pos.x, pos.y + 220));

			auto rotale = RotateBy::create(0.5, 720);

			Spawn* spawn = Spawn::create(moveto, rotale, nullptr);

			auto pCallBack = CallFunc::create(pView, CC_CALLFUNC_SELECTOR(Node::removeFromParent));

			auto pSeque = Sequence::create(dealy, spawn, pCallBack, nullptr);

			pView->runAction(pSeque);

			sgGamePrecedure()->addDieEffect(pView);

			it = mSoldierList.erase(it);

			pSolider->release();

			continue;
		}

		it++;
	}
}

FormationRow::~FormationRow()
{
	Vect_Soldier::iterator it = mSoldierList.begin();
	for (; it != mSoldierList.end(); it++)
		(*it)->release();
}




//cocos2d::Vec2 Formation::pointOnCubicBezier(Vec2* cp, float t)
//{
//	float ax, bx, cx; float ay, by, cy;
//	float tSquared, tCubed; Vec2 result;
//	/* 计算多项式系数 */
//	cx = 3.0 * (cp[1].x - cp[0].x);
//	bx = 3.0 * (cp[2].x - cp[1].x) - cx;
//	ax = cp[3].x - cp[0].x - cx - bx;
//	cy = 3.0 * (cp[1].y - cp[0].y);
//	by = 3.0 * (cp[2].y - cp[1].y) - cy;
//	ay = cp[3].y - cp[0].y - cy - by;
//	/* 计算t位置的点值 */
//	tSquared = t * t;
//	tCubed = tSquared * t;
//	result.x = (ax * tCubed) + (bx * tSquared) + (cx * t) + cp[0].x;
//	result.y = (ay * tCubed) + (by * tSquared) + (cy * t) + cp[0].y;
//	return result;
//}

