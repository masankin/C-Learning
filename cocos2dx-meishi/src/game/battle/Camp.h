/*
* jacey
* 阵营
*/
#pragma once

#include "BattleInfo.h"
#include "Formation.h"
#include "cocos2d.h"
#include "Glyph/Glyph.h"
USING_NS_CC;

class AI;
class GlyphCampAgent;
class Assistant;

class Camp
{
public:
	Camp();
	~Camp();

	bool init(eCampType);
	void update(float deta);
	static Camp*	createCamp(eCampType );

	void load();

	void	addBuild(BuildInfo* ); 
	void	updateBuildInfoById(int id, BuildInfo*);
	void	addGlyphInitAddPeople();

	BaseBuild*	getBuild(int key);
	BaseBuild*	getBuildByPos(Vec2 pt);
	BaseBuild*	getBiludByMinBuild(eBulidType buildType);//获取指定建筑类型最小兵力的建筑

	int getBuildNumByType(eBulidType buildType);

	const MapBulidList&		getBuildList();

	int		getCampMaxProduct();
	int		getCampCurProduct();

	bool	checkIsAI();


	//获取队列Map
	const Vect_Formation& getFormationList() { return mFormationList ; }

	FormationV2* getFormationById(int id);

	void	pushFormation(FormationV2* pForm);

	void	setFormationMoveSpeed(float val);
	float	getFormationMoveSpeed() const;

	void	pauseAllFormation(float time = FLT_MAX);
	void	resumeAllFormation();
	void	pauseFormatiom(int fid, float time = FLT_MAX);
	void	resumeFormatiom(int fid);
	void	freezeAllFormation(float time);

	bool checkBeAttackWithTower(BaseBuild* pAttack);
	void	removeSoliderWithRect(const Rect& rect);

	Solider* getCanBeAttackSoliser(const Point& worldPos, float radiusA, float radiusB);
	Solider* getAliveSoliderInEllipse(const Point& worldPos, float radiusA, float radiusB);
	void getAliveSoliderListInEllipse(const Point& worldPos, float radiusA, float radiusB, std::vector<Solider*> & outList);

	void	occupyBuild(BaseBuild* pBuild, bool isDirty = true);
	void	removeBuild(int BuildId,bool checkDead = true);
	void	insertBuild(BaseBuild* pBuild);



	int		getAllUnit();

	//切换阵型的目标建筑
	void	changeFormationTar(BaseBuild* pOld, BaseBuild* pNew);

	void	addAi();
	void    removeAi();
	 
	void changeToNeutral(Camp* pCamp); 

	// 获取该阵营当前正在攻打的阵营列表
	void	getCurAttCampList(std::set<int>& camplist);


	int	getInstituteNum();

	void gradeSolider();

	bool checkIsMax();

	void addKillPeople(float num);
	void addProductPeople(unsigned int num);
	void addDeadPeople(float num);

	float	 getKillPeople(){ return mKillPeople; };
	int	 getProductPeople(){ return mProductPeople; };
	float	 getDeadPeople(){ return mDeadPeople; };
	int	 getOccupyBuild(){ return mOccupyBuild; }

	bool isHasBuild(){return mBuildList.size()>0?true:false;}

	Glyph* getGlyph(Glyph::eGlyphType type);
	int getAtkLevel();
	int getDefLevel();
	int getSpdLevel();
	int getPplLevel();
	int getMrlLevel();
	//精灵技能加成
	float getAsstAtkAddition();
	void setAsstAtkAddition(float add);
	float getAsstSpeedAddition();
	void setAsstSpeedAddition(float add);

	Assistant* getAssistant() { return mAssistant; }
private:
	
protected:

	// 阵营在资源模式下的积分
	CC_SYNTHESIZE(float, mResScore, ResScore);
	CC_SYNTHESIZE(eCampType, mCampType, CampType);
	CC_SYNTHESIZE(bool, mHasBuild, HasBuild);

	// 阵营拥有的建筑列表
	MapBulidList	mBuildList;
	// 阵营出兵阵型队列
	Vect_Formation  mFormationList;

	//typedef std::map<eCampType, std::set<Formation*>>	CampFormationMap;	//<to camp id, Formation>
	// 阵营产生的阵型列表
	//CampFormationMap	mFormationList;

	float mFormationMoveSpeed;
	
	bool mLive;
	float  mKillPeople;
	int  mProductPeople;
	float  mDeadPeople;
	int  mOccupyBuild;
	float mAsstAtkAddition;
	float mAsstSpeedAddition;

	AI*		mAICom;
	GlyphCampAgent* mGlyphAgent;
	Assistant* mAssistant;
};
