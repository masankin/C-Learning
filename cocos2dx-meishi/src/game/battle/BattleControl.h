/*
* jacey
* 战斗控制类
*/

#pragma once

#include "BattleView.h"
#include "BattleInfo.h"
#include "MessageManager.h"
#include "CreepsManager.h"
#include "Glyph/GlyphBattleManager.h"
#include "buff/BuffView.h"
#include "Assistant/AsstBattleControl.h"
class Solider;
class FormationV2;

class BattleControl : public MessageHandler
{
public:

	BattleControl();
	~BattleControl();

	// override begin

	void onReceiveMassage(GameMessage * message);


	// override end

	
	void init();
	void destroy();
	void update(float dt);

	void	loadMapData(int id);
	CC_SYNTHESIZE(BattleView* , mBattleView, BattleView);
	
	const CampList&	getCampList();
	Camp*	getTargetCamp(eCampType);
	void	getEnemyCamp(CampList&);
	void	getEnemyCamp(CampList& ,eCampType);
	FormationV2 *getEnemyMaxPeopleFormation(eCampType campType); //获取敌方最多小兵的阵营
	bool	getHasNeutralCamp();
	int		getValidCampSize();
	bool	isBorning(){ return isBorn; };
	void	setBorning(){ isBorn = true; };

	CreepsManager* getCreepsManager() { return &mCreepsMgr; }

	BaseBuild*	getTargetBuild(int key);
	BaseBuild*	getTargetBuild(Vec2 pt);
	BaseBuild*	getTargetBuildTouch(Vec2 pt);
	BaseBuild*	getTargetBuildByIns(int insKey);
	// @bEnemy 是否找敌对建筑
	BaseBuild*	getNearestBuild(int key, bool bEnemy);
	const MapBulidList& getBuildList() { return mBuildList; }
	void	getTargetBuildList(eCampType, MapBulidList&);
	void	getTargetActivedBuildList(eCampType, MapBulidList&);
	void	getEnemyBuildList(eCampType, MapBulidList&);
	void	getEnemyBuildsMaxPeople(eCampType, MapBulidList&);
	int		getEnemyBuildsMaxPeople(eCampType);
	int		getCampOccupyNum(eCampType camp);
	bool	getIfOtherTwoInBattle(eCampType );
	int	getInstituteNum(eCampType type);

	const BaseBuild* getCampBaseBuild(eCampType eType, int buildID);
	const BuildInfo& getCampBuildInfo(eCampType eType, int buildID);

	GlyphBattleManager* getGlyphBattleMgr();
	AsstBattleControl* getAsstBattleCtrl();

	int		getCampInstituteNum(eCampType eType);

	void	doCampBuildUpgrade(int buildID);
	void	doBuildSoliderMove(GameMessage* message);
	void	doBuildUpgrade(GameMessage* message);
	void	doBuildSoliderFight(GameMessage* message);
	void	doChangeBuildType(int buildId, eBulidType type, bool isNew);
	void	doBuildSoliderTrans(GameMessage* message);
	void	doBuildAttack(GameMessage* message);
	void	doBuildChange(GameMessage* message);
	void	doBuildFullPeopel(GameMessage* message);
	void	doBuildFreeze(GameMessage* message);
	void	doCallCook();
	void	doInvisibility(GameMessage* message);
	void	doOil(GameMessage* message);
	void	doGamePause(bool stop);
	void	doJungleMessage(GameMessage* message);
	void	doShowMorale(GameMessage* message);
	void	doReducePeople(GameMessage* message);
	void	doFormationOver(GameMessage* message);
	void	doCampBuffShow(GameMessage* message);

	void	doBombHitSolider(Solider* pSolider, Point pos);
	void	doBombAoeHitSolider(Solider* pSolider, Point pos, eCampType campType);
	void	doBombIceHitSolider(int formationID , Point pos);
	void	doBuildSoliderDepart(GameMessage* message);
	void	doCreepsReturnSolider(int creepsBuildId, eCampType originCampType, int originBildUid, int fid, int totalCount);
	void	doReturnSoliderArrived(int creepsBuildId, eCampType originCampType, int originBuildUid, int fid, int count);

	void    CheckAndAddBuild(Solider* soldier,BaseBuild* srcBuild,BaseBuild* tarBuild,float speed);

	bool	checkBuildBeAttack(int buildKey, eCampType campType);

	void cleanGame();

	void	setCurPause(bool bPause){ isBorn = !bPause; }

	//计时器控制
	void enableTimeCalculate(bool start); 



	void createJungleBuild(int destroyID, int buildID, int locationID, int buffID);

	void clearJungleBuild(int id);

	void returnFromJungleBuild(int srcID, int destID);

	void closeJungleBuild(int srcID, int srcCamp, int destCamp, int buildID, int locationID);

	void changeToNeutral(eCampType type);

	FormationV2* getFormationById(int id);
	void getApproachingBuildFormations(int buildUid, std::vector<FormationV2*>& outList, const std::function<bool(const FormationV2*)>& filter = nullptr);


	//pvp相关

	//服务器同步mapdata @param 
	void s2cUpdateBuild(int level, int build, int type, int camp, int num, std::set<int>& fList,bool reconn);

	bool isPvpAndHost();

	int checkEnmyNum();

	eCampType getPVPWinCamp();

	void checkAddPvpAi();
	void removeCampAi();
	void playPvpCampWinAni(eCampType type);

	//初始化buff ICON
	void initCampBuffView(Node* topNode, eCampType camp);
	void clearCampBuffShow(eCampType camp);

	//地沟油道具使用点
	void pushOilPoint(const OilPoint& point);
	const OilPointMap& getOilPointList();
	void updateOil();

	void stopBornAni();

	void addBattleTipsForGlyph(eCampType campType, const std::string& battleTips);
	void	starAddCreepsPoint();

protected:
	void cleanData();
	// 建立build key为索引的map列表
	void	createBuildList();

	void    updateCamps();

	// pvp重新分配阵营
	void	resolvePvpCamp();
	 
	//添加buff ICON
	void startCampBuffShow(eCampType camp, eBuffType buffType, float time);
	void addBuffTips(eCampType camp, eBuffType buffType, int insID);
	void updateBuffTips();
	void updateCampBuffShow();
	void clearCampBuffShow();

	void doGuideExtraAction(MessageGuideTips* pMsg);
	void doAttackGuide(BaseBuild* pSrcBuild, BaseBuild* pDstBuild);
	void delayActiveGuide(float dt, int location);
	void delayActiveGamePageGuide(float dt, int location);

	void doSetTurretStep(GameMessage * message);	//设置建筑step，战斗回放专用
protected:

	MapBulidList	mBuildList;

	CampList	mCampList;
	CC_SYNTHESIZE(eCampType, mOwnCamp, OwnCamp);
	CC_SYNTHESIZE(int, mPercent, OutputPercent);
	CC_SYNTHESIZE(bool, mIsPvp, IsPvp);

	CreepsManager mCreepsMgr;
	GlyphBattleManager mGlyphBattleMgr;
	AsstBattleControl mAsstBattleCtrl;

	//建筑是否完成出生动画
	bool isBorn;
	bool mEnableGameRun;

	bool m_bCanPlayWalkSound;

	//buff效果相关
	typedef std::vector<BuffIconNode*> BuffIconNodeVec;
	typedef std::map<eCampType, BuffIconNodeVec> CampBuffNodeMap;
	CampBuffNodeMap mCampBuffNodeMap; 

	std::vector<std::string> mBuffTipsVec;
	OilPointMap mOilPointList;

	vector<int> m_vFormationIdlist;

	std::list<std::string> mBattleTipsList;
private:  

	float mTimeElapsed;
};
#define sgBattleControl() Singleton<BattleControl>::Get()





