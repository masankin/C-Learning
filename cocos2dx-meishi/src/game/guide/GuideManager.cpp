#include "GuideManager.h"

#include "GamePrecedure.h"
#include "ClickGuideLayer.h"
#include "DragGuideLayer.h"
#include "FocusGuideLayer.h" 
#include "DescGuideLayer.h"
#include "DialogGuideLayer.h"
#include "battle/BattleControl.h" 
#include "EnvSet.h"
#include "game/net/UserObj.h"
#include "ui/PageFunction.h" 
#include "Constant.h" 
#include "core/TimeCalculator.h"
#include "battle/BattleRecordsManager.h"
#define  GUIDELOCK_KEY "GuideLock"
#define  GuideLoc_Divide 50 
#define  GuideMaxGroup 1000
#define  GuideTimer "GuideTimer"
GuideAchiveCondition::GuideAchiveCondition()
{
	m_ParentNode = NULL;
	m_LevelID = 0;
	m_GuideLocation = 0;
	m_GateID = 0;
	m_GuideID = 0;
	m_HighestLayer = false;
	m_GuideLayerPos = ccp(0, 0);
	
}

GuideAchiveCondition::~GuideAchiveCondition()
{
	
}

void GuideAchiveCondition::setSceneNode(Node* pNode)
{
	m_SceneNode = pNode; 
}


void GuideAchiveCondition::setParentNode(Node* pNode)
{
	m_ParentNode = pNode; 
}

void GuideAchiveCondition::releaseNode()
{
	//m_ParentNode->autorelease();
	//m_SceneNode->autorelease();
}


void GuideManager::clearGuideData()
{

}



void GuideManager::onReceiveMassage(GameMessage * message)
{
	
}

GuideManager::GuideManager()
{   
	m_IsRunning = false; 
	m_GuideLock = false;
	mGuideLayer = NULL;
	m_CloseGuide = false;
	m_CurGuideID = 0; 
	m_CurGuideGroup = 0;  
	m_IsRookie = false;
	m_OpenRookieCamp = false; 
	setDragCondition(0);
	setExtraCondition(0);
	setExtraCondition2(0);
}

GuideManager::~GuideManager()
{
	for(int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);
		if(NULL != pData)
		{
			delete pData;
			pData = NULL;
		}	
	}
	m_TotalGuideDataVec.clear();

	while(m_GuideAchiveCoditionQueue.empty() == false)
	{
		GuideAchiveCondition* pCondition = m_GuideAchiveCoditionQueue.front();
		if(NULL != pCondition)
		{
			delete pCondition;
			pCondition = NULL;
		}
		m_GuideAchiveCoditionQueue.pop();
	}
}

bool GuideManager::registGuideMessage(int gateID, int levelID, int guideLocation, Node* parentNode, Node* sceneNode, bool highestLayer, Point guideLayerPos)
{ 	  
	
	if (m_CloseGuide)
		return false;
	GuideAchiveCondition* pCondition = new GuideAchiveCondition();
	pCondition->setParentNode(parentNode);
	pCondition->setGateID(gateID);
	pCondition->setGuideLocation(guideLocation);
	pCondition->setLevelID(levelID);
	pCondition->setSceneNode(sceneNode);  
	pCondition->setHighestLayer(highestLayer);
	pCondition->setGuideLayerPos(guideLayerPos);
	//准备同一帧执行，但是onenter时候把节点加载getRunningScene上遍历会报错，需要改底层
	//待优化 
	bool ret = startGuide(pCondition);
	if(ret == true)
	{  
		/*pCondition->getSceneNode()->retain();
		pCondition->getParentNode()->retain();*/
		sendGuideMessage(gateID, guideLocation, E_GUIDEMESSAGE_START);
		sgGuideManager()->setGuideLock(true); 
		m_GuideAchiveCoditionQueue.push(pCondition); 
		CCLOG("[trace]###GuideManager::registGuideMessage");
	} 
	return ret;
}

bool GuideManager::unRegistGuideMessage(bool force, int operatorType)
{
	if (m_CloseGuide)
		return false; 
	return setCurGuideFinish(force, operatorType);
}

const GuideData* GuideManager::getGuideDataByGuideID(int guideID)
{
	for (int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);
		if (pData->getGuideID() == guideID)
			return pData;
	} 
	CCLOG("[error]GuideManager::getGuideDataByGuideID  NULL, guideID is %d", guideID);
	return NULL;
}

const GuideData* GuideManager::getGuideDataByGroup(int groupID)
{
	for (int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);
		if (pData->getGataGuide() !=0 && pData->getGroupID() == groupID)
			return pData;
	}
	CCLOG("[error]GuideManager::getGuideDataByGroup  NULL, groupID is %d", groupID);
	return NULL;
}



void GuideManager::saveGuideData(int id)
{  
	std::vector<GuideData* >::iterator iter = m_TotalGuideDataVec.begin();
	int groupID = 0;
	for(; iter != m_TotalGuideDataVec.end(); iter++)
	{
		GuideData* pData = *iter;
		if (pData->getGuideID() != id)
			continue;
		if (pData->getSaveAtOnce() != 1)
			continue;
		groupID = pData->getGroupID();
		if (pData->getGataGuide() != 2)
		{
			setCurGuideGroup(0);
			sgUserObj()->setGuideFinish(groupID);
		}
		resetGroupState(groupID, true);
		break; 
	}
}

void GuideManager::rollbackGuide(bool isGateGuide)
{
	
	if (mGuideLayer)
	{
		if (mGuideLayer->getReferenceCount() > 1)
			mGuideLayer->release();
		mGuideLayer->removeFromParentAndCleanup(true);
	}
		
	for(int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData  = m_TotalGuideDataVec.at(i);
		if(!pData)
			return;
		if (isGateGuide)
		{
			if (pData->getGataGuide() == 0)
				continue;
		}
		if (pData->getCanRollback() == 1 && pData->getGroupID() == m_CurGuideGroup)
			pData->setIsFinish(false);
	}
	m_GuideLock = false; 
	m_IsRunning = false;
	if(m_GuideAchiveCoditionQueue.size())
	{
		GuideAchiveCondition* pCondition = m_GuideAchiveCoditionQueue.front();
		pCondition->releaseNode();
		delete pCondition;
		pCondition = NULL;
		m_GuideAchiveCoditionQueue.pop(); 
	}	 
	m_CurGuideID = 0;
	mGuideLayer = NULL; 
}

void GuideManager::initGuideData()
{  

	m_TotalGuideDataVec.clear();
	std::vector<const GuideData* > tVec = sgTableManager()->getGuideDataVec(); 
	if (tVec.size() > 0)
	{
		for(int i = 0; i < tVec.size(); i++)
		{
			GuideData* pData = new GuideData(tVec.at(i));

			m_TotalGuideDataVec.push_back(pData);
		}
	}
	else
		CCLOG("[error]GuideManager::initGuideData guideDataVec is empty"); 

	std::map<int, bool> idMap;
	for(int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);  
		if(!pData)
			return;    
		//if (pData->getSaveAtOnce() == 1)
		{
			bool isFinish = sgUserObj()->getGuideFinish(pData->getGroupID());
			if (pData->getGroupID() == GuideGroup_Glyph && sgUserObj()->getHaveGlyph())
			{
				isFinish = true;
			}
			const CheckPoint* pCheckPoint = sgTableManager()->getCheckPointByID(pData->getGateID());
			if (sgUserObj()->getMaxOpenGateID() > 0 && pCheckPoint->levelType != E_Level_Trans)
			{
				if (pData->getLevel() <= sgUserObj()->getPersonalInfo()->explevel())
				{
					if (pData->getGateID() < sgUserObj()->getMaxOpenGateID())
					{
						isFinish = true;
					}
					else if (pData->getGateID() <= sgUserObj()->getMaxOpenGateID() && pData->getSkip() == 1)
					{
						isFinish = true;
					}

				}				
			}
			for (int j = 0; j < m_TotalGuideDataVec.size(); j++)
			{
				GuideData* pData2 = m_TotalGuideDataVec.at(j);
				if (pData2 && pData->getGroupID() == pData2->getGroupID())
				{ 
					pData2->setIsFinish(isFinish);
					pData2->setGroupFinish(isFinish); 
					idMap.insert(std::make_pair(pData2->getGroupID(), pData2->getIsFinish()));
				} 
			}
			if (pData->getGroupID() <= GuideMaxGroup)
				resetGroupState(pData->getGroupID(), isFinish); 
		}
		 
	}    
	m_GuideGroupStateMap.size();
	sgUserObj()->syncGuideFinish(idMap);
	m_IsRookie = checkRookieFlag();
}

bool GuideManager::init()
{
	bool isRet = false;
	do 
	{  
		mGuideLayer = NULL;
		isRet = true; 
	} while (0);
	return isRet;
} 

void GuideManager::update(float delta)
{  
	if (m_CloseGuide){ return; }
	if (m_IsRunning == false && m_GuideAchiveCoditionQueue.empty() == false)
	{ 
		if (checkGuideActive())
		{
			GuideAchiveCondition* pCondition = m_GuideAchiveCoditionQueue.front();
			CCLOG("[trace]GuideManager::update active id is %d", pCondition->getGuideID());
			doGuide(pCondition);
			m_IsRunning = true;

			PauseDialog* pauseDialog = dynamic_cast<PauseDialog*>(Window::getWindow("PauseDialog.csb"));
			if (pauseDialog && pauseDialog->isShow())
				pauseDialog->setShow(false);
		} 
	}
	if(mGuideLayer)
		mGuideLayer->update(delta);
}
  
 
bool GuideManager::startGuide(GuideAchiveCondition* conditionNode)
{     
	GuideData* pGuideData = searchGuideData(conditionNode); 
	if(!pGuideData)
		return false;
	addGuideTimer(pGuideData->getTimer());
	return true;
}

GuideData* GuideManager::searchGuideData(GuideAchiveCondition* condition)
{ 
	GuideData* pData = getGuideActiveID(condition);
	if (pData)
	{ 
		condition->setGuideID(pData->getGuideID());
		return pData;
	}
	else
	{  
		delete condition;
		condition = NULL; 
		return NULL;
	}  
}

GuideData* GuideManager::getGuideActiveID(GuideAchiveCondition* condition)
{
	//定位ID范围
	//查找最小未触发ID 
	for(int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{ 
		GuideData* pData = m_TotalGuideDataVec.at(i);
		if (!pData)
			continue; 
		const CheckPoint* pCP = sgTableManager()->getCheckPointByID(pData->getGateID());
		if (pCP && pCP->levelType != E_Level_Trans)
		{
			//如果等级过低则不启动引导
			if (condition->getLevelID() < pData->getLevel())
				continue;

			//如果关卡ID太小则不启动引导
			if (condition->getGateID() < pData->getGateID())
				continue; 
		}
		//不触发已完成引导
		if (pData->getIsFinish())
			continue;

		if (checkGuideCondition(pData, condition) && m_CurGuideID != pData->getGuideID())
		{
			if (checkGuideFinishByID(pData->getPreGuideID()) || 0 == pData)
			{
				return pData;
			}
		}
	}
	return NULL;
}

void GuideManager::doGuide(GuideAchiveCondition* condition)
{ 
	CCLOG("[trace]GuideManager::doGuide -- start");
	mGuideLayer = NULL;
	m_CurGuideID = condition->getGuideID();
	Node* pParentNode = condition->getParentNode();
	Node* pSceneNode = condition->getSceneNode();
	 
	if (!pSceneNode || !pParentNode)
		return;

	bool hasCreateLayer = false; 
	for (int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);
		if (pData->getGuideID() != m_CurGuideID)
			continue;
		if (hasCreateLayer == false)
		{
			hasCreateLayer = true;
			initGuideLayer(pData, condition); 
			
		}
		if (!mGuideLayer)
		{
			CCLOG("[error]GuideManager::doGuide -- mGuideLayer is null 1");
			m_IsRunning = false;
			condition->releaseNode();
			delete condition;
			condition = NULL;
			m_GuideAchiveCoditionQueue.pop();
			return;
		}
		mGuideLayer->addGuideData(pData); 
		if (pData->getOperatorType() == E_GuideOperator_Drag)
			m_StopDragFlag = false;  
	} 
	if (!mGuideLayer)
	{
		CCLOG("[error]GuideManager::doGuide -- mGuideLayer is null 2");
		return;
	}
	CCLOG("[trace]GuideManager::doGuide -- addChild() guideID is %d", m_CurGuideID); 
	pSceneNode->addChild(mGuideLayer, TAG_GUIDELAYER, TAG_GUIDELAYER);
		
}

BaseGuideLayer* GuideManager::initGuideLayer(GuideData* pData, GuideAchiveCondition* conditionNode)
{
	if(pData->getGuideMode() == E_GUIDE_CLICK)
	{
		mGuideLayer = ClickGuideLayer::create(); 
	}
	else if(pData->getGuideMode() == E_GUIDE_DRAG)
	{
		mGuideLayer = DragGuideLayer::create(); 
	} 
	else if(pData->getGuideMode() == E_GUIDE_FOCUS)
	{
		mGuideLayer = FocusGuideLayer::create();
	} 
	else if(pData->getGuideMode() == E_GUIDE_DIALOG)
	{
		mGuideLayer = DescGuideLayer::create();
		mGuideLayer->setGuideType(atoi(pData->getDragNodeID().c_str()));
	} 
	else if(pData->getGuideMode() ==  E_GUIDE_NOMAL)
	{
		mGuideLayer = DialogGuideLayer::create();
	}
	else if(pData->getGuideMode() == E_GUIDE_ANIMATION)
	{
		mGuideLayer = AnimationGuideLayer::create();
	}
	else if (pData->getGuideMode() == E_GUIDE_STORY)
	{
		mGuideLayer = StoryDialogGuideLayer::create();
	}
	if (!mGuideLayer)
		return NULL;
	Point pos = ccp(0,0);
	std::vector<std::string> posVec = splitStr(pData->getPointPos(), "#");
	if(posVec.size() == 2)
	{
		int x = atoi(posVec.at(0).c_str());
		int y = atoi(posVec.at(1).c_str());
		pos = ccp(x,y);
	}
	float angle = (pData->getPointAngle()); 
	int type = pData->getIsFingerAni();
	mGuideLayer->initDialogUI(type,angle, pos);
	mGuideLayer->initGuideInfo(m_CurGuideID, 
		pData->getIsPause(), 
		conditionNode->getSceneNode(), 
		conditionNode->getParentNode(), 
		conditionNode->getGuideLayerPos());
	m_CurGuideGroup = pData->getGroupID();  

	mGuideLayer->retain();
	return mGuideLayer;
}

bool GuideManager::checkGuideCondition(GuideData* pData, GuideAchiveCondition* conditionNode)
{ 
	//如果引导类型对不上则不启动引导
	if (!pData || !conditionNode || conditionNode->getGuideLocation() != pData->getType())
		return false; 

	if (pData->getRookieGateID() == conditionNode->getGateID() && pData->getRookieGateID() != 0)
	{
		setGroupFinishByID(pData->getGroupID());
		return false;
	}

	//如果正在回放则不启动引导
	if (sgBattleRecordsManager()->getIsPlayback())
		return false;  
	if (conditionNode->getGuideLocation() == E_GUIDELOC_MAINUI || conditionNode->getGuideLocation() == E_GUIDELOC_RENAMEFINISH)
	{ 
		const CheckPoint* pCP = sgTableManager()->getCheckPointByID(pData->getGateID());
		if (pCP && pCP->levelType == E_Level_Trans)
			return true;
		if (!checkGuideFinishByGroup(GuideGroup_SkipRookieCamp))
			return false;
		if (conditionNode->getGateID() == 0 || conditionNode->getGateID() >= pData->getGateID())
			return true;
	} 
	else if (conditionNode->getGuideLocation() == E_GUIDELOC_GLYPH)
	{
		if (conditionNode->getGateID() >= pData->getGateID())
			return true;
	}
	else if (conditionNode->getGuideLocation() == E_GUIDELOC_SOLDIERNUM)
	{
		if (m_ExtraCondition == pData->getTargetID())
		{
			if (m_ExtraCondition2 >= pData->getSoldierNum()
				&& conditionNode->getGateID() == pData->getGateID())
				return true;
		}
		
	}
	else
	{
		if (conditionNode->getGateID() == pData->getGateID())
			return true;
	}  
	return false;
}
 

void GuideManager::setGuideLock(bool lock)
{
	m_GuideLock = lock;
}

bool GuideManager::getGuideLock()
{
	return m_GuideLock;
} 
 

bool GuideManager::checkNeedPower(int gateID)
{
	return true;
	for(int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData  = m_TotalGuideDataVec.at(i); 
		if(gateID != pData->getGateID() )
			continue;
		//
		if (pData->getType() == E_GUIDELOC_MAINUI
			|| pData->getType() == E_GUIDELOC_PVPUI
			|| pData->getType() == E_GUIDELOC_SHOP
			|| pData->getType() == E_GUIDELOC_GLYPH
			|| pData->getType() == E_GUIDELOC_SELECTGATE
			)
		{
			return true;
		}
			

		int pointState = sgUserObj()->getGateState(gateID);		
		bool ret = checkGuideFinishByID(pData->getGuideID());
		if (ret && pointState == E_CHECKPOINTSTATE_PASS)
		{ 
			return true;
		}
		return false;
	}
	return true;
}
 

//bool GuideManager::checkGuideGroupFinish(int group)
//{
//	for(int i = 0; i < m_TotalGuideDataVec.size(); i++)
//	{
//		if(NULL != m_TotalGuideDataVec.at(i) 
//			&& m_TotalGuideDataVec.at(i)->getGroupID() == group 
//			&& m_TotalGuideDataVec.at(i)->getSaveAtOnce() == 1)
//		{
//			return m_TotalGuideDataVec.at(i)->getIsFinish();
//		}
//	} 
//	return true;
//}
 

 

bool GuideManager::checkGuideFinishByGroup(int group)
{
	std::map<int, bool>::iterator iter = m_GuideGroupStateMap.find(group);
	if (iter != m_GuideGroupStateMap.end())
		return iter->second;
	return false;
}

Layer* GuideManager::getGuideLayer()
{
	return mGuideLayer;
}

bool GuideManager::checkGuideFinishByID(int guideID)
{  
	//查询数据比较该引导是否完成
	//完成的话就保存进度
	//同时更新数值		
	for(int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);
		if (pData && pData->getGuideID() == guideID)
		{
			return pData->getIsFinish();
		}
	} 
	return true;
}


bool GuideManager::checkOpeartor(int operatorType)
{ 
// 	if(m_CurGuideID == 0)
// 		return true;
	for(int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);
		if(pData->getGuideID() == m_CurGuideID && pData->getOperatorType() == operatorType)
			return true;
	} 
	return false;
}


bool GuideManager::checkDragOperator()
{
	if(m_CurGuideID == 0)
		return true;
	for(int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);
		if(pData->getGuideID() == m_CurGuideID && pData->getOperatorType() != E_GuideOperator_DouClick)
			return true;
	} 
	return false;
}


void GuideManager::gotoNextGuide(GuideAchiveCondition* conditionNode)
{
	m_GuideLock = true; 
	doGuide(conditionNode);
}

bool GuideManager::setCurGuideFinish(bool force, int operatorType)
{  
	//查询数据比较该引导是否完成
	//完成的话就保存进度
	//同时更新数值
	bool finish = false;
	int finishIndex = 0;
	//将该组动作全部设置完成
	for(int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);
		bool condition1 = (pData->getGuideID() == m_CurGuideID ? true: false);

		if (!condition1)
			continue;

		bool condition2 = (pData->getIsFinish() == false ? true : false);
		bool condition3 = (checkGuideFinishByID(pData->getPreGuideID())  ? true : false);
		bool condition4 = (pData->getOperatorType() == operatorType ? true : false); 
		if (force)
		{
			condition2 = true;
			condition3 = true;
			condition4 = true;
		}
		if(condition1 && condition2 && condition3 && condition4)
		{ 
			pData->setIsFinish(true);
			finishIndex = i;
			finish = true;   
			break;
		}
	} 
	if(finish == true)
	{   
		mGuideLayer->setIsReady(false);
		GuideData* pData = m_TotalGuideDataVec.at(finishIndex); 

		//保存引导数据
		saveGuideData(pData->getGuideID());
		setCurGuideID(0);
		setGuideLock(false);

		
		setDragCondition(0);
		setExtraCondition(0);
		setExtraCondition2(0);
		if (pData->getGataGuide() != 0
			&& !checkGuideFinishByGroup(pData->getGroupID()))
			setStopDragFlag(true);
		else
			setStopDragFlag(false);


		//显式执行引导结束回调方法
		int refCnt = mGuideLayer->getReferenceCount();
		if (mGuideLayer)
			mGuideLayer->onGuideCallback(true);
		if (mGuideLayer->getReferenceCount() >= 1)
			mGuideLayer->release();
		mGuideLayer = NULL;
		
		//发送OSS数据 都定死ID了怎么OSS还接到小于1000的，法克
		//我就说不是我的锅，法克
		int nodeID = pData->getNodeID();
		if (nodeID < 1000)
		{
			CCLOG("[error]GuideManager::setCurGuideFinish error guide NodeID: %d   guideID is %d", nodeID, pData->getID());
#ifndef MGAME_IN_APP_SDK
			std::string szTitle = sgTextHelper(TEXT_COMMTIPS_TIPS);
			std::string szContent = StringUtils::format(sgTextHelper(TEXT_COMMTIPS_GLYPGNOOPEN).c_str(), nodeID);
			GenerlDialog::show(szTitle, szContent);
#endif
		}  
		else
			m_GuideProxy.sendGuide2Oss(nodeID);

		//执行下一步引导
		GuideAchiveCondition* pCondition = m_GuideAchiveCoditionQueue.front();
		if(pData->getNextGuideID() != 0)
		{ 
			pCondition->setGuideID(pData->getNextGuideID()); 
			gotoNextGuide(pCondition);
		}
		else
		{  
			m_IsRunning = false; 
			pCondition->releaseNode();
			delete pCondition;
			pCondition = NULL;
			m_GuideAchiveCoditionQueue.pop();   
			if (pData->getGataGuide() != 0)
				sendGuideMessage(pData->getGateID(), pData->getType(), E_GUIDEMESSAGE_OVER);
			
		}   
		CCLOG("[trace]GuideManager::setCurGuideFinish curID is %d and nextID is %d", pData->getGuideID(), pData->getNextGuideID());
	}
	return finish;
}


bool GuideManager::checkRookieFlag()
{
	std::map<int, bool>::iterator iter = m_GuideGroupStateMap.begin();
	for (iter; iter != m_GuideGroupStateMap.end(); iter++)
	{
		if (iter->second == true)
			return false;
	} 
	return true;
}

bool GuideManager::checkGuideTypeFinished(int type)
{
	for (int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);
		if (pData->getType() == type && !pData->getIsFinish())
			return false;			
	}
	return true;
}

void GuideManager::setCurGuideGroupFinish()
{
	setGroupFinishByID(getCurGuideGroup());
}

void GuideManager::setGroupFinishByID(int group, bool serverSave)
{
	for (int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);
		if (pData->getGroupID() == group)
			pData->setIsFinish(true);
	}
	std::map<int, bool>::iterator iter = m_GuideGroupStateMap.find(group);
	if (iter != m_GuideGroupStateMap.end())
		iter->second = true;
	else
		m_GuideGroupStateMap.insert(std::make_pair(group, true));
	if (serverSave)
		sgUserObj()->setGuideFinish(group);
}

const GuideData* GuideManager::getCurGuideData()
{
	for (int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		const GuideData* pData = m_TotalGuideDataVec.at(i);
		if (pData->getGuideID() == m_CurGuideID)
			return pData;
	} 
	return NULL;
}


void GuideManager::rollbackRookieGuide()
{
	for (int i = 0; i < m_TotalGuideDataVec.size(); i++)
	{
		GuideData* pData = m_TotalGuideDataVec.at(i);
		if (pData->getCanRollback() == 2)
		{
			pData->setIsFinish(false);
			resetGroupState(pData->getGroupID(), false);
		} 
	}
}

void GuideManager::resetGroupState(int group, bool state)
{
	std::map<int, bool>::iterator iter = m_GuideGroupStateMap.find(group);
	if (iter != m_GuideGroupStateMap.end())
		iter->second = state;
	else
		m_GuideGroupStateMap.insert(std::make_pair(group, state));
}

void GuideManager::addGuideTimer(float time)
{
	sgTimeCalculator()->createTimeCalcultor(GuideTimer, time);
}

bool GuideManager::checkGuideActive()
{
	if (!sgTimeCalculator()->hasKey(GuideTimer))
		return true;
	float leftTime = sgTimeCalculator()->getTimeLeft(GuideTimer);
	if (leftTime > 0.001)
		return false;
	removeGuideTimer();
	return true;
}

void GuideManager::removeGuideTimer()
{
	sgTimeCalculator()->removeTimeCalcultor(GuideTimer);
}

void GuideManager::setStopDragFlag(bool flag)
{
	m_StopDragFlag = flag;
}

bool GuideManager::getStopDragFlag()
{
	if (m_CloseGuide)
		return false;
	return m_StopDragFlag;
}

bool GuideManager::checkIsReady()
{ 
	if (mGuideLayer)
		return mGuideLayer->getIsReady();
	return false;
}

void GuideManager::resetGuideDragFlag(int gateID)
{
	m_StopDragFlag = false;
	std::vector<GuideData* >::iterator iter = m_TotalGuideDataVec.begin();
	for (; iter != m_TotalGuideDataVec.end(); iter++)
	{
		GuideData* pData = *iter; 
		if (pData->getIsFinish())
			continue; 
		if (pData->getGateID() == gateID 
			&& pData->getGataGuide() != 0)
		{
			m_StopDragFlag = true;
			break;
		}
	} 
}

bool GuideManager::checkOpenPropControl(int gateID)
{  
	bool isOpenPropControlFlag = true;
	const CheckPoint* pPoint = sgTableManager()->getCheckPointByID(gateID);
	if (pPoint->levelType == E_Level_Trans)
	{
		const GuideData* pData = getGuideDataByGroup(GuideGroup_RookieProp);
		if (!pData)
			return isOpenPropControlFlag;
		if (gateID < pData->getGateID())
			isOpenPropControlFlag = false;
	}
	else if (pPoint->levelType != E_Level_Trans)
	{
		const GuideData* pData = getGuideDataByGroup(GuideGroup_PveProp);
		if (!pData)
			return isOpenPropControlFlag;
		if (gateID < pData->getGateID())
			isOpenPropControlFlag = false;
		else if (gateID == pData->getGateID())
			isOpenPropControlFlag = checkGuideFinishByGroup(GuideGroup_PveProp);
		else if (gateID > pData->getGateID() && checkHaveGateGuide(gateID))
			isOpenPropControlFlag = false;
	}
	return isOpenPropControlFlag;
}


bool GuideManager::checkOpenSoldierControl(int gateID)
{
	bool isSoldierControlFlag = true;
	const CheckPoint* pPoint = sgTableManager()->getCheckPointByID(gateID);
	if (pPoint->levelType == E_Level_Trans)
	{
		const GuideData* pData = getGuideDataByGroup(GuideGroup_RookieSoldierNum);
		const GuideData* pData2 = getGuideDataByGroup(GuideGroup_RookieStable);
		if (!pData ||  !pData2)
			return isSoldierControlFlag;
		if (gateID != pData->getGateID() && gateID != pData2->getGateID())
			isSoldierControlFlag = false;		
	}
	else if (pPoint->levelType != E_Level_Trans)
	{
		const GuideData* pData = getGuideDataByGroup(GuideGroup_PveSoldierNum);
		const GuideData* pData2 = getGuideDataByGroup(GuideGroup_RookieStable);
		if (!pData || !pData2)
			return isSoldierControlFlag;
		if (gateID < pData->getGateID())
			isSoldierControlFlag = false;
		else if (gateID == pData->getGateID() || gateID == pData2->getGateID())
			isSoldierControlFlag = true;
		else if (gateID > pData->getGateID() && checkHaveGateGuide(gateID))
			isSoldierControlFlag = false;
	}
	return isSoldierControlFlag;
}

bool GuideManager::checkHaveGateGuide(int gateID)
{
	std::vector<GuideData* >::iterator iter = m_TotalGuideDataVec.begin();
	for (; iter != m_TotalGuideDataVec.end(); iter++)
	{
		GuideData* pData = *iter;
		if (pData->getIsFinish())
			continue;
		if (pData->getGateID() == gateID
			&& pData->getGataGuide() != 0
			&& !pData->getIsFinish())
		{
			return true;
		}
	}
	return false;
}

void GuideManager::sendGuideMessage(int gateID, int locationID, int state)
{ 
	string msgStr = StringUtils::format("%d %d %d", gateID, locationID, state);
	sgMessageManager()->sendMessage(EMT_GUIDE, msgStr);
}
 
 

