/**
* @file     JungleBattleManager.h
* @brief    打野系统
* @details  
* @author   Alvin
* @date     2015-8-17
*/
#ifndef __JUNGLEBATTLEMANAGER_H__
#define __JUNGLEBATTLEMANAGER_H__
#include "cocos2d.h"
#include "Message.h"
#include "battle/Build.h"
#include "data/Tables.h"
USING_NS_CC;
class JungleBattleInfo
{
public:
	JungleBattleInfo();
	~JungleBattleInfo();

private:
	CC_SYNTHESIZE(int, m_SrcID, SrcID);
	CC_SYNTHESIZE(int, m_DestID, DestID); //仅存储打野建筑ID
	CC_SYNTHESIZE(int, m_FormationID, FormationID);
};

typedef std::pair<int, int> MyKey;

class MyPredicator
{
public:
	MyPredicator(){};
	~MyPredicator(){};

	bool operator()(const MyKey& v1, const MyKey& v2)
	{
		if( v1.first * 1e8 + v1.second  < v2.first * 1e8 + v2.second)
		{
			return false;
		}
		return true;
	}
};


class JungleBattleManager: public MessageHandler
{
public:
	static JungleBattleManager* getInstance();
	JungleBattleManager();
	~JungleBattleManager();

	//释放单例
	static void cleanJungleBattleManager(); 

	bool init();
	void update(float delta);

	//创建打野建筑
	void createJungleBuild(int buildID, int locationID, int buffID);  

	//阵营从打野建筑返回
	void returnFromJungleBuild(int srcID, int destID, int srcCamp, int destCamp);

	//新增阵营Buff
	void addJungleBuildBuff(int camp, int jungleBuildID);

	void onReceiveMassage(GameMessage * message);

	//清理数据
	void clearJungleBattleData(int gateID);

	
private:
	static JungleBattleManager* m_Instance; 
	
	typedef std::map<int, JungleData*>  JungleBuildMap;
	JungleBuildMap m_JungleBuildMap;
	JungleBuildMap m_JungleBuildMap2;

	std::queue<int> m_ClearJungleQuence;

	//最大存在打野建筑数量
	int m_MaxBuildCount;
	//每次刷新打野建筑数量
	int m_RefreshCount;
	//当前打野建筑数量
	int m_JungleBuildCount;
	

	//当前等待刷新时间
	float m_WaitRefreshTime;
	//刷新时间
	float m_RefreshTime;

	
	
	int m_State;

	//存储向打野建筑攻击的军队信息
	typedef std::vector<JungleBattleInfo* > JungleBattleInfoVec;
	JungleBattleInfoVec m_JungleBattleInfoVec;

	//存储受到攻击次数的打野建筑表
	typedef std::map<int, int> JungleBuildRefMap;
	JungleBuildRefMap m_JungleBuildRefMap;

	//打野建筑刷新表
	typedef std::vector<JungleData* > JungleDataVec;
	JungleDataVec m_JungleDataVec; 
private:
	void addJungleBattleInfo(int fromationID, int srcID, int destID);
	void removeJungleBattleInfo(int fromationID,int srcID, int destID, int srcCamp, int destCamp, int buildID, int locationID);
	void closeJungleBattleInfo(int srcID, int srcCamp, int destCamp, int buildID, int locationID);
	void clearJungleBuildRefresh(int buildID, int locationID);
}; 
 
#endif
