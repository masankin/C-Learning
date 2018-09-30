/**
* @file     GuideManager.h
* @brief    新手引导管理类
* @details  
* @author   Alvin
* @date     2015-7-28
*/
#ifndef __GUIDEMANAGER_H__
#define __GUIDEMANAGER_H__
#include "cocos2d.h" 
#include "cocos-ext.h" 
#include "battle/ArrowWidget.h"
#include "cocostudio/CCArmature.h"
#include "BaseGuideLayer.h" 
#include "Message.h"
#include "GuideProxy.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocostudio;

 
typedef void (CCObject::*SEL_CallFuncByBool)(bool isOk);
#define callfuncByBool_selector(_SELECTOR) (SEL_CallFuncByBool)(&_SELECTOR)
 
enum GuideEspecialID
{
	GuideID_PveProp = 1000,			//PVE道具引导ID 
	GuideID_Morale = 900,			//士气引导ID
	GuideID_PveSoldierBar = 830,		//兵力条引导ID	
	GuideID_RookieSoldierBar = 330, //训练营兵力条引导
	GuideID_Glyph = 68,				//符文引导ID  
	GuideID_Institute = 1300,			//抽卡引导

};

enum GuideEspecialGroup
{
	GuideGroup_Glyph = 20,//符文引导组
	GuideGroup_SkipRookieCamp = 33,  //新手训练营是否跳过引导组 
	GuideGroup_SelectRookieCamp = 34,	//选择新手训练营
	GuideGroup_RookieSoldierNum = 50, //训练营兵力条引导引导组
	GuideGroup_PveSoldierNum = 90, //PVE兵力条引导组
	GuideGroup_RookieProp = 70,	 //训练营道具引导组  
	GuideGroup_PveProp = 110,      //PVE道具引导组 
	GuideGroup_RookieStable = 120,  //训练营马厩引导
	GuideGroup_PveStable = 130,  //PVE马厩引导 
	
};



class GuideAchiveCondition
{
public: 
	GuideAchiveCondition();
	~GuideAchiveCondition();
	void setSceneNode(Node* pNode);
	Node* getSceneNode(){ return m_SceneNode; }
	void setParentNode(Node* pNode);
	Node* getParentNode(){ return m_ParentNode; }
	void releaseNode();
private:
	CC_SYNTHESIZE(int, m_GateID, GateID);					//触发关卡
	CC_SYNTHESIZE(int, m_LevelID, LevelID);					//触发等级
	CC_SYNTHESIZE(int, m_GuideLocation, GuideLocation);		//引导所在界面 
	CC_SYNTHESIZE(int, m_GuideID, GuideID);				    //计算的引导点
	CC_SYNTHESIZE(bool, m_HighestLayer, HighestLayer);		//是否当前场景最高层
	CC_SYNTHESIZE(Point, m_GuideLayerPos, GuideLayerPos);
	Node* m_ParentNode;										//引导点父节点
	Node* m_SceneNode;										//场景节点
};



class GuideManager: public MessageHandler
{
public:
	GuideManager();
	~GuideManager();
	
	bool registGuideMessage(
		int gateID, int levelID, int guideLocation, 
		Node* parentNode, Node* sceneNode, 
		bool highestLayer = true, Point guideLayerPos = ccp(0,0)); 
	bool unRegistGuideMessage(bool force = false, int operatorType = 0); 

	const GuideData* getGuideDataByGuideID(int guideID);
	const GuideData* getGuideDataByGroup(int groupID);

	void saveGuideData(int id);
	
	void rollbackGuide(bool gateGuide = true);

	static void cleanGuideManager();

	void clearGuideData();

	void onReceiveMassage(GameMessage * message);

	bool init();
	
	void update(float delta); 
	
	bool getIsRunning() { return m_IsRunning;}

	
	bool checkGuideFinishByID(int id);		 //查询相关ID是否完成
	bool checkGuideFinishByGroup(int group); //查询相关组是否完成

	bool checkOpeartor(int operatorType);

	bool checkDragOperator();

	Layer* getGuideLayer();
	bool getGuideLock();
	void setGuideLock(bool lock);
	 

	bool checkNeedPower(int gateID); //查询是否需要消耗体力  

	bool checkRookieFlag(); //获取新手标识

	bool checkGuideTypeFinished(int type); //查询该类型引导是否完成

	void setCurGuideGroupFinish();
	void setGroupFinishByID(int group, bool serverSave = true); 

	const GuideData* getCurGuideData();

	void rollbackRookieGuide(); //回滚新手训练营引导状态

	bool checkIsReady();

	void resetGuideDragFlag(int gateID);

	bool checkOpenSoldierControl(int gateID); //是否开启兵力条

	bool checkOpenPropControl(int gateID); //是否开启道具栏

	bool checkHaveGateGuide(int gateID); //判断是否存在引导，去除已完成引导

	bool getStopDragFlag();
	void setStopDragFlag(bool flag);
private:
	//执行引导--由消息驱动
	bool startGuide(GuideAchiveCondition* conditionNode);

	GuideData* getGuideActiveID(GuideAchiveCondition* conditionNode);

	//执行引导--由内部关联驱动
	void doGuide(GuideAchiveCondition* conditionNode);

	//创建引导显示对象
	BaseGuideLayer* initGuideLayer(GuideData* tData, GuideAchiveCondition* conditionNode);

	//检查触发条件
	bool checkGuideCondition(GuideData* tData, GuideAchiveCondition* conditionNode); 
	//查找最小的符合条件的引导信息
	GuideData* searchGuideData(GuideAchiveCondition* condition);
	
	//跳转下一次引导
	void gotoNextGuide(GuideAchiveCondition* conditionNode);
	//设置完成
	bool setCurGuideFinish(bool force = false, int operatorType = 0); 
	 
	void resetGroupState(int group, bool state);

	void addGuideTimer(float time);
	bool checkGuideActive();
	void removeGuideTimer();

	void sendGuideMessage(int gateID, int locationID, int state); 
	
private:
	BaseGuideLayer* mGuideLayer;

	CC_SYNTHESIZE(int, m_CurGuideGroup, CurGuideGroup);
	CC_SYNTHESIZE(int, m_CurGuideID, CurGuideID);
	CC_SYNTHESIZE(bool, m_IsRookie, IsRookie);
	CC_SYNTHESIZE(int, m_ExtraCondition, ExtraCondition);
	CC_SYNTHESIZE(int, m_ExtraCondition2, ExtraCondition2);
	CC_SYNTHESIZE(int, m_DragCondition, DragCondition);
	CC_SYNTHESIZE(bool, m_OpenRookieCamp, OpenRookieCamp);
	 
	CC_SYNTHESIZE(bool, m_CloseGuide, CloseGuide);
	bool m_GuideLock;

	bool m_StopDragFlag;

	std::vector<GuideData* > m_TotalGuideDataVec;  

	void initGuideData(); 
	bool m_IsRunning;

	
	std::queue<GuideAchiveCondition* > m_GuideAchiveCoditionQueue;

	GuideProxy m_GuideProxy;

	std::map<int, bool> m_GuideGroupStateMap; 
 };


 #define sgGuideManager() Singleton<GuideManager>::Get()

 
#endif
