#pragma once
#include "cocos2d.h"
#include "Constant.h"
USING_NS_CC;


/*
军队类：（Formation)
     以map结构保存了所有的队列（FormationRaw）。
     以list结构保存了路径信息（MoveInfo）。
使用：
     create创建并调用start2end()获取整个寻路的点。
	 用寻路的点生成MoveInfo的列表。每次更新整个军队调用nextPosition。
*/

class Solider;
class FormationRow;
class BaseBuild;

typedef std::vector<Solider*> Vect_Soldier;

class Formation : public cocos2d::Ref
{

public:

	struct MoveInof
	{
		MoveInof() : maxDis(0), isEnd(false), isBend(false){};
		float maxDis;
		float cos;
		float sin;
		cocos2d::Vec2 currentPos;
		cocos2d::Vec2 aimPos;
		cocos2d::Vec2 midPos;
		bool isBend;
		float rotation;
		bool isEnd;
	};
	
	Formation() : mSrcBuild(0), mParent(0), mTarBuild(0), mFlage(0), m_iRowSpace(0), mMaxRow(0), mPause(false), m_RowIndex(0), m_time(0){};
	~Formation();
	
	//创建方的阵营
	static Formation* create(eCampType campType, unsigned int num, unsigned id, BaseBuild* srcBuild,float time = 0);
	bool init(eCampType campType, unsigned int num, unsigned id, BaseBuild* srcBuild,float time);
	//创建唯一的Id
	static unsigned int createId();

	void enter(cocos2d::Node*);
	void nextPostion();
	void star2end(const cocos2d::Vec2&, const cocos2d::Vec2&); 

	bool	checkCanCreateRow();

	Vec2 getCenterPos();

	//当前兵数
	int getUnit();
	//期望兵数
	int getExpectUint(){return mExpectNum; }; 

	//检测是否被防御塔攻击
	bool checkBeAttack(BaseBuild* pAttack);
	void removeSoliderWithRect(const Rect& rect);

	int		getBeAttackKey();
	eCampType getTargetCamp();

	void changeTarget(BaseBuild* pOld, BaseBuild* pNew);

	typedef std::map<int, FormationRow*> Map_FormationRow;
	typedef std::list<MoveInof> Vect_MoveInof;

	//根据研究所数量改变  士兵显示
	void  changeSoliderIns();

	void setSrcBuild(BaseBuild* srcBuild);
	void setTarBuild(BaseBuild* tarBuild);

	BaseBuild* getSrcBuild(){return mSrcBuild;};
	BaseBuild* getTarBuild(){return mTarBuild;};

	//暂停时加上暂停时间
	void	addPauseTime(float time);
protected:

	void pushMove(const cocos2d::Vec2 pos);
	void createRow();
	//cocos2d::Vec2 pointOnCubicBezier(cocos2d::Vec2* cp, float t);
private:

	Map_FormationRow _FormationRow;
	Vect_MoveInof _MoveList;
	BaseBuild* mSrcBuild;
	BaseBuild* mTarBuild; 
	eCampType mCamp;
	Node* mParent;
	int mContent;
	int mExpectNum;
	bool mFlage;
	
	std::vector<MoveInof> m_sTempMoveInfo;
	int m_iRowSpace;

	//TODO:set所有FormationRow的速度
	CC_SYNTHESIZE(float, _speed, Speed);
	//队形Id
	CC_SYNTHESIZE(unsigned int, mId, Id);
	// 当前阵型有多少行士兵
	CC_SYNTHESIZE(float, mMaxRow, MaxRow);

	CC_SYNTHESIZE(float, mPreFrame, PreFrame);//提前帧数

	CC_SYNTHESIZE(bool, mPause, Pause);
	

	double m_time;

	int m_RowIndex;

};


/*
队列类：（FormationRow）
保存了当前每一步的MoveInfo。

当一个Row完成当前MoveInfo将其传给后面的Row，并向前面的Row获取下一个MoveInfo
*/
class FormationRow
{
public:

	
	FormationRow() :mCurrentDis(0), next(0), spaceCloum(20), mTarBuild(0), readyEnd(0), mSrcBuild(0), m_IsLastRow(0), m_RowID(0), m_FormationID(0)
	{};
	~FormationRow();;

	void NextPostion();
	void addSoldier(Solider*);
	void enter(cocos2d::Node*);
	bool isNext(){ return next; };
	void setNext(bool n){ next = n; };
	void addMoveInfo(Formation::MoveInof); 
	void setDefault();
	int	 getUnit();

	//检测是否被防御塔攻击
	bool checkBeAttack(BaseBuild* pAttack);
	Formation::MoveInof getMoveInfo();
	void removeSoliderWithRect(const Rect& rect);

	void changeTarget(BaseBuild* pOld, BaseBuild* pNew);


	const Vect_Soldier&  getSoldierList(){ return mSoldierList; };

	
private:


	friend Formation;
	Vect_Soldier mSoldierList;
	float mCurrentDis;
	bool next;
	bool readyEnd;

	Formation::MoveInof mMoveInfo;

	Formation::Vect_MoveInof _MoveInfoList; 
	eCampType mCampType;
	CC_SYNTHESIZE(float, _speed, Speed);
	CC_SYNTHESIZE(float, spaceCloum, SpaceCloum);
	CC_SYNTHESIZE(int, m_IsLastRow, IsLastRow);
	CC_SYNTHESIZE(int, m_RowID, RowID);
	CC_SYNTHESIZE(int, m_FormationID, FormationID);
	CC_SYNTHESIZE(BaseBuild*, mTarBuild, TarBuild);
	CC_SYNTHESIZE(BaseBuild*, mSrcBuild, SrcBuild);
};
