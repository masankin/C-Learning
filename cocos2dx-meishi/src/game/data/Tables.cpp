#include "Tables.h"
#include "utils/Comm.h"
#include "HelperFunc.h"
#include "CVTime.h"

void BuildAttr::readline(std::stringstream& _stream)
{
	_stream >> id
		>> type
		>> level
		>> proSpeed
		>> proLimit
		>> holdLimit
		>> addDef
		>> attRadius
		>> attSpeed
		>> attCount
		>> upNeedPro
		>> upNeedTime
		>> upAddMorale
		>> holdAddMorale
		>> lostAddMorale
		>> moveSpeed
		>> exchangeRate;
}

BuildAttr* BuildAttr::create()
{
	BuildAttr* pret = new BuildAttr();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

const BuildAttr* BuildAttrTable::getBuildAttrByID(int id)
{
	BuildAttrList::iterator it = mBuildAttrList.find(id);
	if (it != mBuildAttrList.end())
	{
		return it->second;
	}
	return NULL;
}

void BuildIns::readline(std::stringstream& _stream)
{
	_stream >> id
		>> gateID
		>> type
		>> levelLimit
		>> initPeople
		>> initCamp
		>> initExpress
		>> aiType
		>> isModesTag
		>> isGuardTag
		>> scoreSpeed
		>> creepsPoint;
}

BuildIns* BuildIns::create()
{
	BuildIns* pret = new BuildIns();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

const BuildIns* BuildInsTable::getBuildInsByID(int id)
{
	BuildInsMap::iterator it = mBuildInsMap.find(id);
	if (it != mBuildInsMap.end())
	{
		return it->second;
	}
	return NULL;
}

 const BuildInsVec BuildInsTable::getBuildIndsByGateID(int gateID)
{
	BuildInsVec tVec;
	BuildInsMap::iterator it = mBuildInsMap.begin();
	for (; it != mBuildInsMap.end(); it++)
	{
		BuildIns* tData = it->second;
		if(tData->gateID == gateID)
		{
			tVec.push_back(tData);
		}
	}
	return tVec;
}
 
 

void CheckPoint::readline(std::stringstream& _stream)
{	
	_stream >> id
		>> levelName
		>> checkType
		>> star1
		>> star2
		>> star3
		>> timeLimit
		>> tool1
		>> tool2
		>> tool3
		>> viewUpgrade
		>> viewEnemyInfo
		>> viewHelpInfo
		>> bgMap
		>> obstacleMap
		>> tileMap
		>> preCheckPt
		>> preNeedStar
		>> consumeEnergy
		>> dailyTimes
		>> levelType
		>> branches
		>> levelLogo
		>> baseBonus
		>> perfectBonus
		>> passDes
		>> passName
		>> failCount
		>> tryCount
		>> spirit
		>> firstEX
		>> maxStarEX
		>> repeatEX
		>> owner
		>> nextStage
		>> shareID
		>> tipsProp
		>> isSkip
		>> menuSkipCount
		>> failSkipCount
		>> skipValue
		>> hasClearanceTip
		>> clearanceTipImg
		>> clearanceTip;

	skipValueList = splitStr(skipValue, "#");
}

CheckPoint* CheckPoint::create()
{
	CheckPoint* pret = new CheckPoint();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

const CheckPoint* CheckPointTable::getCheckPointByID(int id)
{
	CheckPointList::iterator it = mCheckPointList.find(id);
	if (it != mCheckPointList.end())
	{
		return it->second;
	}
	return NULL;
}

const CheckPoint* CheckPointTable::getCheckPointByMapName(std::string szMap)
{
	MapCPointList::iterator it = mMapCPList.find(szMap);
	if (it != mMapCPList.end())
	{
		return it->second;
	}
	return NULL;
}

InsistuteAddAttr* InsistuteAddAttr::create()
{
	InsistuteAddAttr* pret = new InsistuteAddAttr();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void InsistuteAddAttr::readline(std::stringstream& _stream)
{
	_stream >> id
		>> addDefence
		>> addAttack;
}

const InsistuteAddAttr* InsistuteAddAttrTable::getInsistuteAddAttrByID(int id)
{
	InsistuteAddAttrList::iterator it = mInsistuteAddAttrList.find(id);
	if (it != mInsistuteAddAttrList.end())
	{
		return it->second;
	}
	return NULL;
}

StateAction* StateAction::create()
{
	StateAction* pret = new StateAction();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void StateAction::readline(std::stringstream& _stream)
{
	actionList.clear();
	for (int i = 0; i < 5;++i)
		actionList.push_back("");
	
	_stream >> id
		>> state
		>> actionList[0]
		>> actionList[1]
		>> actionList[2]
		>> actionList[3]
		>> actionList[4]
		>> time;

	//删除空的行为动作
	for (auto it = actionList.begin(); it != actionList.end();)
	{
		if (*it == "-1")
			it=actionList.erase(it);
		else
			it++;
	}

}

const StateAction* StateActionTable::getStateActionByID(int id)
{
	StateActionList::iterator it = mStateActionList.find(id);
	if (it != mStateActionList.end())
	{
		return it->second;
	}
	return NULL;
}

TextHelper* TextHelper::create()
{
	TextHelper* pret = new TextHelper();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void TextHelper::readline(std::stringstream& _stream)
{
	_stream >> id
		>> text;
}

const TextHelper* TextHelperTable::getTextHelperByID(int id)
{
	TextHelperList::iterator it = mTextHelperList.find(id);
	if (it != mTextHelperList.end())
	{
		return it->second;
	}
	return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MusicInfo::readline(std::stringstream& _stream)
{
	_stream >> id
		>> description
		>> wait
		>> interval
		>> durationTime
		>> volume
		>> fileName;
}

MusicInfo* MusicInfo::create()
{
	MusicInfo* pret = new MusicInfo();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

const MusicInfo* MusicTable::getMusicInfoByID(int id)
{
	//CCLOG("[trace]MusicTable::getMusicInfoByID id:%d!", id);
	MusicList::iterator it = mMusicList.find(id);
	if (it != mMusicList.end())
	{
		return it->second;
	}
	CCLOG("[error]MusicTable::getMusicInfoByID id:%d return NULL!", id);
	return NULL;
}

PropInfo* PropInfo::create()
{
	PropInfo* pret = new PropInfo();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void PropInfo::readline(std::stringstream& _stream)
{
	int type;
	_stream >> id
		>> type
		>> range
		>> duration
		>> cdtime
		>> icon
		>> priority
		>> beginCd
		>> useRange
		>> isPve
		>> display
		>> headText
		>> name
		>> explain1
		>> explain2
		>> params;

	this->type = (ePropType)type;
	if (explain2 == "0")
	{
		explain2 = "";
	}
}

const PropInfo* PropTable::getPropInfoByID(int id)
{
	PropList::iterator it = mPropList.find(id);
	if (it != mPropList.end())
	{
		return it->second;
	}
	return NULL;
}


 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GuideData* GuideData::create()
{
	GuideData* pret = new GuideData();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

bool GuideData::init()
{
	return true;
}
GuideData::GuideData()
{
	m_GroupFinish = false;
	m_NodeID = 0;
	m_ID = 0;
	m_GateGuide = 0;
	m_GuideID = 0;  
	m_GroupID = 0.00f; 
	m_Type = 0;
	m_Step = 1;
	m_Skip = 0;
	m_GuideMode = 0;
	m_GuideTextID = 0;
	m_GuideNodeID = "";
	m_GuideNodePos = ccp(0,0);
	m_DragNodeID  = "";
	m_DragNodePos = ccp(0,0);
	m_IsForce = 1;
	m_Timer = 0.0f;
	m_NextGuideID = 0;
	m_PreGuideID = 0;
	m_GateID = 0;
	m_Level = 1;
	m_SoldierNum = 0;
	m_TargetID = 0;
	m_IsFinish = false;
	m_WaitTime = 0.0f;
	m_IsShow = true; 
	m_GuideFrameSize = "";
	m_ExtraFunc = 0;
	m_ExtraParam = 0;
	m_NpcTurn = 0;
	m_NpcPos = "";
	m_IsPause = 0;
	m_IsBattleGuide = 0;
	m_IsFingerAni = 0;
	m_PointPos = "";
	m_PointAngle = 0.0;
	m_CanRollback = 0;
	m_TouchAtOnce = 1;
	m_OperatorType = 0;
	m_EnableFocus = 1;
	m_RookieGateID = 1010;
	m_SaveAtOnce = 1; 
	m_TouchAtOnce = 1;
}

GuideData::GuideData(const GuideData* copy)
{	
	m_GroupFinish = copy->getGroupFinish();

	m_ID = copy->getID();
	m_GateGuide = copy->getGataGuide();
	m_NodeID = copy->getNodeID(); 
	m_GuideID = copy->getGuideID();
	m_GroupID = copy->getGroupID();
	m_Type = copy->getType();
	m_Step = copy->getStep();
	m_Skip = copy->getSkip();
	m_GuideMode = copy->getGuideMode();	
	m_GuideTextID = copy->getGuideTextID();	

	m_FocusType = copy->getFocusType();
	m_GuideNodeID = copy->getGuideNodeID();
	m_GuideNodePos = copy->getGuideNodePos();
	m_DragNodeID = copy->getDragNodeID();
	m_DragNodePos = copy->getDragNodePos();
	m_IsForce = copy->getIsForce();
	m_Timer = copy->getTimer();	
	m_SoldierNum = copy->getSoldierNum();
	m_TargetID = copy->getTargetID();
	m_IsShowHand = copy->getIsShowHand();
	
	m_NextGuideID = copy->getNextGuideID();
	m_PreGuideID = copy->getPreGuideID();
	m_GateID = copy->getGateID();	
	m_Level = copy->getLevel();
	m_IsFinish = copy->getIsFinish();
	m_WaitTime = copy->getWaitTime();	
	m_IsShow = copy->getIsShow();
	m_imgFile = copy->getImgFile();
	m_GuideFrameSize = copy->getGuideFrameSize();
	m_ExtraFunc = copy->getExtraFunc();
	m_ExtraParam = copy->getExtraParam();
	
	m_NpcTurn = copy->getNpcTurn();
	m_NpcPos = copy->getNpcPos();	
	m_IsPause = copy->getIsPause();
	m_IsBattleGuide = copy->getIsBattleGuide();	
	m_IsFingerAni = copy->getIsFingerAni();
	m_PointPos = copy->getPointPos();
	m_PointAngle = copy->getPointAngle();
	m_TouchAtOnce = copy->getTouchAtOnce();
	m_EnableFocus = copy->getEnableFocus();
	m_OperatorType = copy->getOperatorType();

	m_CanRollback = copy->getCanRollback();		
	m_RookieGateID = copy->getRookieGateID();	
	m_SaveAtOnce = copy->getSaveAtOnce();	
	
	
}

GuideData::~GuideData()
{

} 

void GuideData::readline(std::stringstream& _stream)
{
	std::string pos1Str;
	std::string pos2Str;  
	_stream
		>> m_ID
		>> m_GateGuide
		>> m_NodeID
		>> m_GuideID
		>> m_GroupID
		>> m_Type
		>> m_Step
		>> m_Skip
		>> m_GuideMode
		>> m_GuideTextID
		>> m_FocusType
		>> m_GuideNodeID
		>> pos1Str
		>> m_DragNodeID
		>> pos2Str
		>> m_IsForce
		>> m_Timer
		>> m_NextGuideID
		>> m_PreGuideID
		>> m_GateID 
		>> m_Level
		>> m_SoldierNum
		>> m_TargetID
		>> m_IsShowHand
		>> m_WaitTime
		>> m_IsShow
		>> m_imgFile
		>> m_GuideFrameSize
		>> m_ExtraFunc
		>> m_ExtraParam
		>> m_NpcTurn
		>> m_NpcPos
		>> m_IsPause
		>> m_IsBattleGuide
		>> m_IsFingerAni
		>> m_PointPos
		>> m_PointAngle
		>> m_TouchAtOnce
		>> m_EnableFocus 
		>> m_OperatorType
		>> m_CanRollback
		>> m_RookieGateID
		>> m_SaveAtOnce;

	std::vector<std::string > tPos1Vec = splitStr(pos1Str, "#");	
	if(tPos1Vec.size() == 2)
	{
		std::string pos1X = tPos1Vec.at(0);
		std::string pos1Y = tPos1Vec.at(1);
		m_GuideNodePos = ccp(atof(pos1X.c_str()), atof(pos1Y.c_str()));
	}

	std::vector<std::string > tPos2Vec = splitStr(pos2Str, "#");
	if(tPos2Vec.size() == 2)
	{
		std::string pos2X = tPos2Vec.at(0);
		std::string pos2Y = tPos2Vec.at(1);
		m_DragNodePos = ccp(atof(pos2X.c_str()), atof(pos2Y.c_str())); 
	}
} 

const GuideData* GuideDataTable::getGuideDataByID(int id)
{
	GuideDataVec::iterator it = m_GuideDataVec.begin();
	for(; it != m_GuideDataVec.end(); it++)
	{
		if ((*it)->getGuideID() == id)
		{
			return *it;
		}
	} 
	return NULL;
}
 
GuideDataVec GuideDataTable::getGuideDataVec()
{
	return m_GuideDataVec;
}

AchievementAward* AchievementAward::create()
{
	AchievementAward* pPoint = new AchievementAward();
	if (pPoint && pPoint->init())
	{
		return pPoint;
	}
	CC_SAFE_DELETE(pPoint);
	return NULL;
}

void AchievementAward::readline(std::stringstream& _stream)
{
	mstatus = 0;
	std::string pCondtion ="";
	_stream>>id
		>> module
		>>category
		>>pCondtion
		>>Award
		>>describe
		>>taskName
		>>mGetPlayerBegain
		>>mIcon;
	mGetPlayer = 0;
	mProgress = 0;
	switch (category)
	{
	case 1:
	{
		std::vector<std::string > pconditionVec = splitStr(pCondtion, "#");
		if (pconditionVec.size() == 3)
		{
			point_Type = std::atoi(pconditionVec[0].c_str());
			switch (point_Type)
			{
			case 1:
				pointId = std::atoi(pconditionVec[2].c_str());
				break;
			case 2:
				pvePropCount = std::atoi(pconditionVec[2].c_str());
				break;
			default:
				break;
			}
		}
	}
		
		break;
	case 2:
		{
			std::vector<std::string > pconditionVec = splitStr(pCondtion, "#");
			if (pconditionVec.size() == 2)
			{
				pointType = std::atoi(pconditionVec[0].c_str());
				getStar     = std::atoi(pconditionVec[1].c_str());
			}
		}
		break;
	case 3:
		{
			std::vector<std::string > pconditionVec = splitStr(pCondtion, "#");
			if (pconditionVec.size() == 3)
			{
				addUpType = std::atoi(pconditionVec[0].c_str());
				addTotalProgress = std::atoi(pconditionVec[2].c_str());
				if (addUpType == 1)
				{
					fightCount = std::atoi(pconditionVec[1].c_str());
					danGrad = std::atoi(pconditionVec[2].c_str());
				}else if (addUpType == 2)
				{
					pvpModule = std::atoi(pconditionVec[1].c_str());
					pvpCount = std::atoi(pconditionVec[2].c_str());
				}
			}
		}
		break;
	default:
		break;
	}
}

AchievementAward* AchievementAwardTable::getAchievementByKey(AchieveCategory categroy, int key)
{
	switch (categroy)
	{
	case A_Point:
		{
			if (m_AchievePointMap.size()>key)
			{
				return m_AchievePointMap[key];
			}
			
			cocos2d::log("[AchievementAwardTable]: getAchievePointByKey not find value by key:%d",key);
			return NULL;
		}
		break;
	case A_Star:
		{
			if (m_AchieveStarMap.size()>key)
			{
				return m_AchieveStarMap[key];
			}

			cocos2d::log("[AchievementAwardTable]: getAchieveStarByKey not find value by key:%d",key);
			return NULL;
		}
		break;
	case A_AddUp:
		{
			if (m_AchieveAddUpMap.size()>key)
			{
				return m_AchieveAddUpMap[key];
			}
			cocos2d::log("[AchievementAwardTable]: getAchieveAddUpByKey not find value by key:%d",key);
			return NULL;
		}
		break;
	case A_Last:
		{
			if (m_AchieveLastMap.size()>key)
			{
				return m_AchieveLastMap[key];
			}
			cocos2d::log("[AchievementAwardTable]: m_AchieveLastMap not find value by key:%d",key);
			return NULL;
		}
		break;
	default:
		break;
	}
	return NULL;
}

AchievementAward* AchievementAwardTable::getAchievementById(int id)
{
	for (auto it = m_AchievePointMap.begin(); it != m_AchievePointMap.end(); it++)
	{
		if ((*it)->id == id)
		{
			return (*it);
		}
	}
	for (auto it = m_AchieveStarMap.begin(); it != m_AchieveStarMap.end(); it++)
	{
		if ((*it)->id == id)
		{
			return (*it);
		}
	}
	for (auto it = m_AchieveAddUpMap.begin(); it != m_AchieveAddUpMap.end(); it++)
	{
		if ((*it)->id == id)
		{
			return (*it);
		}
	}
	/*for (auto it = m_AchieveLastMap.begin(); it!=m_AchieveLastMap.end(); it++)
	{
		if ((*it)->id == id)
		{
			return (*it);
		}
	}*/
	return NULL;
}


int AchievementAwardTable::getAchievementCount(AchieveCategory category)
{
	int pRet = 0;
	switch (category)
	{
	case A_Point:
		pRet =  m_AchievePointMap.size();
		break;
	case A_Star:
		pRet = m_AchieveStarMap.size();
		break;
	case A_AddUp:
		pRet = m_AchieveAddUpMap.size();
		break;
	case A_Last:
		pRet = m_AchieveLastMap.size();
	default:
		break;
	}
	return pRet;
}

bool AchievementAwardTable::isAchieveMentAwardComplent(AchieveModule lmodule, bool isLast /*= false*/)
{
	bool pRet = false;
	for(auto it = m_AchieveLastMap.begin(); it!=m_AchieveLastMap.end(); it++)
	{
		if ((*it)->module == lmodule)
		{
			pRet = true;
			break;
		}
	}
	if (isLast&& m_AchieveLastMap.size()>0)
	{
		pRet = true;
	}
	return pRet;
}

void AchievementAwardTable::sortAchievementAward(std::map<int,AchievementAward::AchieveInfo>& pStatusMap)
{
	std::vector<int> pnogetPointVec;
	std::vector<int> pgetedPointVec;
	std::vector<int> pnogetStarVec;
	std::vector<int> pgetedStarVec;
	std::vector<int> pnogetAddUpVec;
	std::vector<int> pgetedAddupVec;
	for (auto it = pStatusMap.begin(); it!=pStatusMap.end(); it++)
	{
		AchieveCategory pCategory = getAchieveCategory((*it).first,(*it).second.mCompleteCount,  (*it).second.mReceivePlayerCount);
		if (it->second.mstatus == 2)//2未完成 1未领取 0 已领取
		{
			continue;
		}
		if ( pCategory == A_Point)
		{
			if ((*it).second.mstatus == 1)
			{
				pnogetPointVec.push_back((*it).first);
			}else
			{
				pgetedPointVec.push_back((*it).first);
			}
		}else if (pCategory == A_Star)
		{
			if ((*it).second.mstatus == 1)
			{
				pnogetStarVec.push_back((*it).first);
			}else
			{
				pgetedStarVec.push_back((*it).first);
			}
		}else if (pCategory == A_AddUp)
		{
			if ((*it).second.mstatus == 1)
			{
				pnogetAddUpVec.push_back((*it).first);
			}else
			{
				pgetedAddupVec.push_back((*it).first);
			}
		}
	}
	sortAchieveByCategory(A_Point,pnogetPointVec,pgetedPointVec);
	sortAchieveByCategory(A_Star,pnogetStarVec,pgetedStarVec);
	sortAchieveByCategory(A_AddUp,pnogetAddUpVec,pgetedAddupVec);
	m_AchieveLastMap = m_AchieveComplentMap;
	/*for (auto it = m_AchieveNoComplentMap.begin(); it != m_AchieveNoComplentMap.end(); it++)
	{
		m_AchieveLastMap.push_back(*it);
	}*/
	m_AchieveComplentMap.clear();
	m_AchieveNoComplentMap.clear();
}

void AchievementAwardTable::sortAchieveByCategory(AchieveCategory category, std::vector<int> nogetVec,std::vector<int> getedVec)
{
	std::vector<AchievementAward*> pAwardVec;
	std::vector<AchievementAward*> pAchieveVec;
	switch (category)
	{
	case A_NULL:
		break;
	case A_Point:
		pAwardVec = m_AchievePointMap;
		break;
	case A_Star:
		pAwardVec = m_AchieveStarMap;
		break;
	case A_AddUp:
		pAwardVec = m_AchieveAddUpMap;
		break;
	default:
		break;
	}

	if (pAwardVec.size()>0)
	{
		pAchieveVec.clear();
		for (int i =0; i<nogetVec.size(); i++)
		{
			for (auto it1 = pAwardVec.begin(); it1!=pAwardVec.end(); it1++)
			{
				if (nogetVec[i] == (*it1)->id)
				{
					(*it1)->mstatus = 2;
					pAchieveVec.push_back(*it1);
					m_AchieveComplentMap.push_back(*it1);
					pAwardVec.erase(it1);
					break;
				}
			}
		}
		for (auto it1 = pAwardVec.begin(); it1!=pAwardVec.end();it1++)
		{
			bool isContinue = false;
			for (int i =0; i<getedVec.size(); i++)
			{
				if ((*it1)->id == getedVec[i])
				{
					isContinue = true;
					break;
				}
			}
			if (!isContinue)
			{
				(*it1)->mstatus = 0;
				pAchieveVec.push_back(*it1);
			}

		}
		for (auto it1 = pAwardVec.begin(); it1!=pAwardVec.end();it1++)
		{
			for (int i =0; i<getedVec.size(); i++)
			{
				if ((*it1)->id == getedVec[i])
				{
					(*it1)->mstatus = 1;
					pAchieveVec.push_back(*it1);
					m_AchieveNoComplentMap.push_back(*it1);
					break;
				}
			}
		}
	}
	switch (category)
	{
	case A_NULL:
		break;
	case A_Point:
		 m_AchievePointMap = pAchieveVec;
		break;
	case A_Star:
		m_AchieveStarMap = pAchieveVec;
		break;
	case A_AddUp:
		m_AchieveAddUpMap = pAchieveVec;
		break;
	default:
		break;
	}
}

AchieveCategory AchievementAwardTable::getAchieveCategory(int id, int progress ,int getPlayeres)
{
	
	for (auto it = m_AchievePointMap.begin(); it!=m_AchievePointMap.end(); it++)
	{
		if ((*it)->id == id)
		{
			(*it)->mProgress = progress;
			if (getPlayeres >= 0)
			{
				(*it)->mGetPlayer = getPlayeres;
			}
			return A_Point;
		}
	}
	
	for (auto it = m_AchieveStarMap.begin(); it!=m_AchieveStarMap.end(); it++)
	{
		if ((*it)->id == id)
		{
			(*it)->mProgress = progress;
			if (getPlayeres >= 0)
			{
				(*it)->mGetPlayer = getPlayeres;
			}
			return A_Star;
		}
	}

	for (auto it = m_AchieveAddUpMap.begin(); it!=m_AchieveAddUpMap.end(); it++)
	{
		if ((*it)->id == id)
		{
			(*it)->mProgress = progress;
			if (getPlayeres >= 0)
			{
				(*it)->mGetPlayer = getPlayeres;
			}
			return A_AddUp;
		}
	}
	
	return A_NULL;
}

GiftAward* GiftAward::create()
{
	GiftAward* pret = new GiftAward();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

GiftAward::GiftAward()
	:id(0),
	gold(0),
	manual(0),
	energy(0),
	roomcard(0),
	mHonor(0),
	mBageId(0),
	mTitle(""),
	mOrign("")
{
}
GiftAward::~GiftAward()
{

}

void GiftAward::readline(std::stringstream& _stream)
{
	std::string  tempArr[5];
	_stream >> id
		>> gold
		>> manual
		>> energy
		>> roomcard
		>> mHonor
		>> mBageId
		>> tempArr[0]
		>> tempArr[1]
		>> tempArr[2]
		>> tempArr[3]
		>> tempArr[4]
		>> mTitle
		>> mOrign;

	for (int i = 0; i < 5;i++)
	{
		if (tempArr[i].empty() || tempArr[i] == "0")
		{
			continue;
		}
		auto tempStr = splitStr(tempArr[i], "+");
		if (tempStr.size()==2)
		{
			itemMap.insert(std::make_pair(atoi(tempStr[0].c_str()), atoi(tempStr[1].c_str())));
			itemIdList.push_back(atoi(tempStr[0].c_str()));
			itemCountList.push_back(atoi(tempStr[1].c_str()));
		}
	}
	if (gold > 0)
	{
		GiftInfo lGiftInfo;
		lGiftInfo.mType = UserObj::PROPERTYTYPE::PT_DIAMOND;
		lGiftInfo.mId = 0;
		lGiftInfo.mNumber = gold;
		mGiftData.push_back(lGiftInfo);
	}
	if (manual > 0)
	{
		GiftInfo lGiftInfo;
		lGiftInfo.mType = UserObj::PROPERTYTYPE::PT_PVEPOWER;
		lGiftInfo.mId = 0;
		lGiftInfo.mNumber = manual;
		mGiftData.push_back(lGiftInfo);
	}
	if (energy > 0)
	{
		GiftInfo lGiftInfo;
		lGiftInfo.mType = UserObj::PROPERTYTYPE::PT_PVPPOWER;
		lGiftInfo.mId = 0;
		lGiftInfo.mNumber = energy;
		mGiftData.push_back(lGiftInfo);
	}
	if (roomcard > 0)
	{
		GiftInfo lGiftInfo;
		lGiftInfo.mType = UserObj::PROPERTYTYPE::ROOM_TICKET;
		lGiftInfo.mId = 0;
		lGiftInfo.mNumber = roomcard;
		mGiftData.push_back(lGiftInfo);
	}
	if (mHonor > 0)
	{
		GiftInfo lGiftInfo;
		lGiftInfo.mType = UserObj::PROPERTYTYPE::Honor;
		lGiftInfo.mId = 0;
		lGiftInfo.mNumber = mHonor;
		mGiftData.push_back(lGiftInfo);
	}
	if (mBageId > 0)
	{
		GiftInfo lGiftInfo;
		lGiftInfo.mType = UserObj::PROPERTYTYPE::TITLE_ADD;
		lGiftInfo.mId = mBageId;
		lGiftInfo.mNumber = 1;
		mGiftData.push_back(lGiftInfo);
	}
	for (auto it : itemMap)
	{
		GiftInfo lGiftInfo;
		int id = it.first;
		if (id <100001)//道具
		{
			lGiftInfo.mType = UserObj::PROPERTYTYPE::PT_ITEM;
		}
		else
		{
			lGiftInfo.mType = UserObj::PROPERTYTYPE::STUFF;
		}
		lGiftInfo.mId = id;
		lGiftInfo.mNumber = it.second;
		mGiftData.push_back(lGiftInfo);
	}
	
}

GiftAward* GiftTable::getGiftAwardById(int id)
{
		GiftAwardVec::iterator it = mGiftAwadrVec.begin();
	for (; it != mGiftAwadrVec.end(); it++)
	{
		if ((*it)->id == id)
		{
			return *it;
		}
	}
	return NULL;
}

AchievePointAward* AchievePointAward::create()
{
	AchievePointAward* pPoint = new AchievePointAward();
	if (pPoint && pPoint->init())
	{
		return pPoint;
	}
	CC_SAFE_DELETE(pPoint);
	return NULL;
}

void AchievePointAward::readline(std::stringstream& _stream)
{
	_stream >> id
		>> achievePoint
		>> giftId
		>> icon;
}

AchievePointAward* AchievePointAwardTable::getAchievePointAward(int id)
{
	auto it = mAwardPointMap.find(id);
	if (it== mAwardPointMap.end())
	{
		return NULL;
	}
	return (*it).second;
	

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JungleData* JungleData::create()
{
	JungleData* pret = new JungleData();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

bool JungleData::init()
{
	return true;
}
JungleData::JungleData()
{
	m_ID = 0; 
	m_GateID = 0;
	m_BuildID = 0;
	m_LocationID = 0;
	m_RefreshWeight = 0;
	m_BuildID = 0;
	m_RefreshCount = 0;
	m_TotalCount = 0;
	m_RefreshTime = 0; 
	m_RandRangeStar = 0;
	m_RandRangeEnd = 0;
	m_AnimationName = "";
}

JungleData::~JungleData()
{

} 

void JungleData::readline(std::stringstream& _stream)
{
	_stream 
		>> m_ID
		>> m_GateID
		>> m_BuildID
		>> m_LocationID
		>> m_RefreshWeight 
		>> m_BuffID
		>> m_RefreshCount
		>> m_TotalCount
		>> m_RefreshTime
		>> m_AnimationName;
	m_RandRangeStar = 0;
	m_RandRangeEnd = 0;
} 

const JungleData* JungleDataTable::getJungleDataByID(int buildID)
{
	JungleDataVec::iterator it = m_JungleDataVec.begin();
	for(; it != m_JungleDataVec.end(); it++)
	{
		if ((*it)->getBuildID() == buildID)
		{
			return *it;
		}
	} 
	return NULL;
}

JungleDataVec JungleDataTable::getJungleDataVec()
{
	return m_JungleDataVec;
}

JungleDataVec JungleDataTable::getJungleDataVecByID(int gateID)
{
	JungleDataVec tVec;
	JungleDataVec::iterator tIter = m_JungleDataVec.begin();
	for(; tIter != m_JungleDataVec.end(); tIter++)
	{
		if((*tIter)->getGateID() == gateID)
		{
			tVec.push_back(*tIter);
		} 
	}
	return tVec;
}


/////////////////////////////////////////
MoraleDefenceData::MoraleDefenceData()
{
	m_ID = 0;
	m_MoraleStart = 0;
	m_MoraleEnd = 0;
	m_AddDefenceMoral = 0;
}

MoraleDefenceData::~MoraleDefenceData()
{

}

MoraleDefenceData* MoraleDefenceData::create()
{
	MoraleDefenceData* pret = new MoraleDefenceData();
	if (pret)
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void MoraleDefenceData::readline(std::stringstream& _stream)
{
	_stream 
		>> m_ID
		>> m_MoraleStart
		>> m_MoraleEnd
		>> m_AddDefenceMoral;
}

///////////////////

MoraleDefenceData* MoraleDefenceDataTable::getMoraleDefenceDataByRange(float moraleValue)
{ 
	MoraleDefenceDataVec::iterator tIter = m_MoraleDefenceDataVec.begin();
	for(; tIter != m_MoraleDefenceDataVec.end(); tIter++)
	{
		MoraleDefenceData* tData = *tIter;
		if(moraleValue >= tData->getMoraleStart() && moraleValue < tData->getMoraleEnd())
		{
			return tData;
		}
	}
	return NULL;
}

MoraleDefenceDataVec MoraleDefenceDataTable::getMoraleDefenceDataVec()
{
	return m_MoraleDefenceDataVec;
}


///////////////////////////////
MoraleBuffData::MoraleBuffData()
{
	m_ID = 0;
	m_MoraleStart = 0;
	m_MoraleEnd = 0; 
}

MoraleBuffData::~MoraleBuffData()
{

}

MoraleBuffData* MoraleBuffData::create()
{
	MoraleBuffData* tPt = new MoraleBuffData();
	if(tPt)
	{
		return tPt;
	}
	CC_SAFE_DELETE(tPt);
	return NULL;
}

void MoraleBuffData::readline(std::stringstream& _stream)
{
	_stream 
		>> m_ID
		>> m_MoraleStart
		>> m_MoraleEnd
		>> m_BuffID
		>> m_Level;
}

MoraleBuffDataVec MoraleBuffDataTable::getMoraleBuffDataVec()
{
	return m_MoraleBuffDataVec;
}

MoraleBuffDataVec MoraleBuffDataTable::getMoraleBuffDataVecByRange(float moraleValue)
{
	MoraleBuffDataVec tVec;
	MoraleBuffDataVec::iterator tIter = m_MoraleBuffDataVec.begin();
	for(; tIter != m_MoraleBuffDataVec.end(); tIter++)
	{
		MoraleBuffData* tData = *tIter;
		if(moraleValue >= tData->getMoraleStart() && moraleValue <  tData->getMoraleEnd())
		{
			tVec.push_back(tData);
		}
	}
	return tVec;
}

int MoraleBuffDataTable::getMoraleValueLevel(float moraleValue)
{ 
	MoraleBuffDataVec::iterator tIter = m_MoraleBuffDataVec.begin();
	for(; tIter != m_MoraleBuffDataVec.end(); tIter++)
	{
		MoraleBuffData* tData = *tIter;
		if(moraleValue >= tData->getMoraleStart() && moraleValue <  tData->getMoraleEnd())
		{
			return tData->getLevel();
		}
	}
	return 0;
}

//////////////////////////////////////////////
BuffData::BuffData()
{
	m_ID = 0;
	m_BuffType = 0;
	m_AddValue = 0;
	m_AddRate = 0;
	m_CDTime = 0;
	m_mutex = 0;
	m_targetType = 0;
}

BuffData::BuffData(const BuffData* copy)
{
	m_ID = copy->m_ID;
	m_BuffType = copy->m_BuffType;
	m_AddValue = copy->m_AddValue;
	m_AddRate = copy->m_AddRate;
	m_CDTime = copy->m_CDTime;
	m_mutex = copy->m_mutex;
	m_targetType = copy->m_targetType;
}

BuffData::~BuffData()
{

}

BuffData* BuffData::create()
{
	BuffData* tPt = new BuffData();
	if(NULL != tPt)
	{
		return tPt;
	}
	CC_SAFE_DELETE(tPt);
	return NULL;
}

void BuffData::readline(std::stringstream& _stream)
{
	_stream 
		>> m_ID
		>> m_BuffType
		>> m_AddValue
		>> m_AddRate
		>> m_CDTime
		>> m_targetType
		>> m_targetNum
		>> m_mutex;
}


BuffData* BuffDataTable::getBuffDataByID(int id)
{
	 BuffDataMap::iterator tIter = m_BuffDataMap.find(id);
	 if(tIter != m_BuffDataMap.end())
	 {
		 return tIter->second;
	 }
	 return NULL;
}

/////////////////////////////////////////
WinnerData::WinnerData()
{
	m_ID = 0;
	m_GoldNum = 0;
}

WinnerData::~WinnerData()
{

}

WinnerData* WinnerData::create()
{
	WinnerData* pret = new WinnerData();
	if (pret)
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void WinnerData::readline(std::stringstream& _stream)
{
	_stream 
		>> m_ID
		>> m_GoldNum;
}

WinnerData* WinnerDataTable::getWinnerData(int id)
{ 
	WinnerDataVec::iterator tIter = m_WinnerDataVec.begin();
	for(; tIter != m_WinnerDataVec.end(); tIter++)
	{
		WinnerData* tData = *tIter;
		if(tData && id == tData->getID())
		{
			return tData;
		}
	}
	return NULL;
}

int WinnerDataTable::getWinnerDataNum()
{
	return m_WinnerDataVec.size();
}

////////////////////////////////////

ActLogin* ActLoginTable::getActLogin(int id)
{
	auto it = mActLoginMap.find(id);
	if (it == mActLoginMap.end())
	{
		return NULL;
	}
	return (*it).second;
}

void ActLogin::readline(std::stringstream& _stream)
{
	_stream>>mId
		>>mDay
		>>mGiftSpe
		>>mGiftNor;
}
ActLogin* ActLogin::create()
{
	ActLogin* pActLogin = new ActLogin();
	if (pActLogin && pActLogin->init())
	{
		return pActLogin;
	}
	CC_SAFE_DELETE(pActLogin);
	return NULL;
}

/////////////////////////////////////////
NoticeData::NoticeData()
{
	m_ID = 0;
	m_NoticeCount = 0;
	m_Content = "";
	m_Title = "";
}

NoticeData::~NoticeData()
{

}

NoticeData* NoticeData::create()
{
	NoticeData* pret = new NoticeData();
	if (pret)
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void NoticeData::readline(std::stringstream& _stream)
{
	_stream 
		>> m_ID
		>> m_User
		>> m_Type
		>> m_Urgent
		>> m_NoticeCount
		>> m_TimeGap
		>> m_Title
		>> m_Content
		>> m_MsgType
		>> m_Value
		>> m_Text;

}

NoticeDataVec* NoticeDataTable::getNoticeDataVec()
{ 
	return &m_NoticeDataVec;
}

NoticeData* NoticeDataTable::getNoticeData(int id)
{
	NoticeDataVec::iterator iter = m_NoticeDataVec.begin();
	for (; iter != m_NoticeDataVec.end(); iter++)
	{
		if ((*iter)->getID() == id)
			return *iter;
	}
	return nullptr;
	
}

void ShopInfo::readline(std::stringstream& _stream)
{
	std::string pSdk;
	std::string ldescribe;
	_stream >> id
		>> type
		>> gold
		>> goldStr
		>> manual
		>> img
		>> icon
		>> energy
		>> gift
		>> giftCout
		>> text
		>> pSdk
		>> ldescribe
		>> name
		>> startTime
		>> endTime
		>> leftCount
		>> giftId
		>> discount
		>> priceGold
		>> giftIcon
		>> giftType;

	isShow = false;
	isFirst = true;
	/*std::vector<std::string > tTempStrVec = splitStr(endTime, "#");
	std::vector<int> str;
	if (tTempStrVec.size() == 2)
	{
	std::string closeTime = tTempStrVec.at(1).substr(0, tTempStrVec.at(1).length() - 6);
	str[0] = atoi( closeTime.substr(0, 4).c_str());
	str[1] = atoi( closeTime.substr(4, 2).c_str());
	str[2] = atoi(closeTime.substr(6, 2).c_str());
	int64_t lNowTime = sgUserObj()->getNowTime();
	CVTime lNow_Time(lNowTime);
	CVTime lendTime(str[0],str[1],str[2],lNow_Time.GetHour(),lNow_Time.GetMinute(),lNow_Time.GetSecond());
	lendTime.GetTime();
	}*/

	std::vector<std::string > pVec = splitStr(pSdk, "#");	
	if(pVec.size() == 3)
	{
		for (int i =0; i<pVec.size(); i++)
		{
			mSdkIsOpen.push_back(atoi(pVec[i].c_str()));
		}
	}
	if (text == "0")
	{
		text = "";
	}
}

ShopInfo* ShopInfo::create()
{
	ShopInfo* pret = new ShopInfo();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

const ShopList& ShopTable::getShopList()
{
	return mShopList;
}

const ShopInfo* ShopTable::getShopInfoByID(int id)
{
	ShopList::iterator it = mShopList.find(id);
	if (it != mShopList.end())
	{
		return it->second;
	}

	return nullptr;
}

void ShopTable::updateShopInfo(std::map<int,ShopInfo> shopMap)
{
	for (auto giftit = mGiftList.begin(); giftit != mGiftList.end(); giftit++)
	{
		giftit->second->isShow = false;
	}
	
	for (auto it = shopMap.begin(); it != shopMap.end(); it++)
	{
		ShopInfo lshopInfo1 = (it->second);
		auto shopit = mShopList.find(it->first);
		if (shopit != mShopList.end())
		{
			ShopInfo* lshopInfo = (*shopit).second;
			lshopInfo->isFirst = lshopInfo1.isFirst;
		}
		else
		{
			if (lshopInfo1.type == ShopInfo::Shop_Gift)
			{
				auto giftit = mGiftList.find(it->first);
				if (giftit != mGiftList.end())
				{
					ShopInfo* lgiftShopInfo = giftit->second;
					lgiftShopInfo->giftId = lshopInfo1.giftId;
					lgiftShopInfo->startTime = lshopInfo1.startTime;
					lgiftShopInfo->endTime = lshopInfo1.endTime;
					lgiftShopInfo->leftCount = lshopInfo1.leftCount;
					lgiftShopInfo->name = lshopInfo1.name;
					lgiftShopInfo->discount = lshopInfo1.discount;
					lgiftShopInfo->priceGold = lshopInfo1.priceGold;
					lgiftShopInfo->giftIcon = lshopInfo1.giftIcon;
					lgiftShopInfo->giftType = lshopInfo1.giftType;
					lgiftShopInfo->isShow = true;
				}
				else
				{
					ShopInfo* lgiftShopInfo = ShopInfo::create();
					lgiftShopInfo->gold = 0;
					lgiftShopInfo->id = lshopInfo1.id;
					lgiftShopInfo->type = ShopInfo::ShopType::Shop_Gift;
					lgiftShopInfo->giftId = lshopInfo1.giftId;
					lgiftShopInfo->startTime = lshopInfo1.startTime;
					lgiftShopInfo->endTime = lshopInfo1.endTime;
					lgiftShopInfo->leftCount = lshopInfo1.leftCount;
					lgiftShopInfo->name = lshopInfo1.name;
					lgiftShopInfo->discount = lshopInfo1.discount;
					lgiftShopInfo->priceGold = lshopInfo1.priceGold;
					lgiftShopInfo->giftIcon = lshopInfo1.giftIcon;
					lgiftShopInfo->giftType = lshopInfo1.giftType;
					lgiftShopInfo->isShow = true;
					mGiftList.insert(std::make_pair(it->first, lgiftShopInfo));
				}
				
			}
		}
	}
}

ItemBuyInfo* ItemBuyInfo::create()
{
	ItemBuyInfo* pret = new ItemBuyInfo();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void ItemBuyInfo::readline(std::stringstream& _stream)
{
	_stream >> id
		>> PropId
		>> num
		>> moneynum
		>> icon
		>> energy
		>> text;
}

const ItemBuyList& ItemBuyTable::getItemBuyList()
{
	return mItemBuyList;
}

const ItemBuyInfo* ItemBuyTable::getItemBuyInfoByID(int id)
{
	ItemBuyList::iterator it = mItemBuyList.find(id);
	if (it != mItemBuyList.end())
	{
		return it->second;
	}

	return nullptr;
}

/////////////////////////////////////////
RechargeData::RechargeData()
{
	mID = 0;
	mStage = 0;
	mStartTime = "";
	mEndTime = "";
	mRechargeType = 0;
	mRechargeNum = 0;
	mReward = 0;
	mIsGet = 2;
}

RechargeData::~RechargeData()
{

}

RechargeData* RechargeData::create()
{
	RechargeData* pret = new RechargeData();
	if (pret)
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void RechargeData::readline(std::stringstream& _stream)
{
	_stream 
		>> mID
		>> mStage
		>> mStartTime
		>> mEndTime
		>> mRechargeType
		>> mRechargeNum
		>> mReward;
	mIsGet = false;
}

RechargeList* RechargeActivityTable::getRechargeList()
{ 
	return &mRechargeList;
}

RechargeData* RechargeActivityTable::getRechargeData(int id)
{ 
	RechargeList::iterator it = mRechargeList.find(id);
	if (it != mRechargeList.end())
	{
		return it->second;
	}
	return NULL;
}

/////////////////////////////////////////
PowerBuy::PowerBuy()
{
	mID = 0;
	mPowerNum = 0;
	mEnergyNum = 0;
}

PowerBuy::~PowerBuy()
{

}

PowerBuy* PowerBuy::create()
{
	PowerBuy* pret = new PowerBuy();
	if (pret)
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void PowerBuy::readline(std::stringstream& _stream)
{
	_stream 
		>> mID
		>> mPowerNum
		>> mEnergyNum;
}

PowerBuyList* PowerBuyTable::getPowerBuyList()
{ 
	return &mPowerBuyList;
}

PowerBuy* PowerBuyTable::getPowerBuy(int id)
{ 
	PowerBuyList::iterator it = mPowerBuyList.find(id);
	if (it != mPowerBuyList.end())
	{
		return it->second;
	}
	return NULL;
}

////////////////////////////////////////
DayTaskData::DayTaskData()
{
}

DayTaskData::~DayTaskData()
{

}

DayTaskData* DayTaskData::create()
{
	DayTaskData* data = new DayTaskData();
	if (data)
	{
		return data;
	}
	CC_SAFE_DELETE(data);
	return NULL;
}

void DayTaskData::readline(std::stringstream& _stream)
{
	_stream
		>> mID
		>> mType
		>> mHardLevel
		>> mRequire
		>> mValue
		>> mGiftId
		>> mTitle
		>> mDesc;
}

 DayTaskData* DayTaskTable::getDayTaskById(int id)
{
	DayTaskDataList::iterator it = mDayTaskDataList.find(id);
	if (it != mDayTaskDataList.end())
	{
		return it->second;
	}
	return NULL;
}


/////////////////////////////////野怪点/////////////////////////////////////////

CreepsPoint* CreepsPoint::create()
{
	CreepsPoint* pret = new CreepsPoint();
	if (pret)
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return nullptr;
}


CreepsPoint::CreepsPoint() : mID(0)
, mGroup(0)
, mProb(0.0f)
, mInterval(0.0f)
{

}


CreepsPoint::~CreepsPoint()
{
	
}


void CreepsPoint::readline(std::stringstream& _stream)
{
	std::string strIds;
	
	_stream >> mID
		>> mGroup
		>> mProb
		>> mInterval
		>> strIds;

	mCreepsList = std::move(HelperFunc::tokenize_2int(strIds, "+"));
}


void CreepsPointTable::purge()
{
	for (auto itr = mTable.begin(); itr != mTable.end(); ++itr)
	{
		if (itr->second)
			delete itr->second;
	}
}


CreepsPoint* CreepsPointTable::getCreepsPoint(int id)
{
	auto itr = mTable.find(id);
	if (mTable.end() != itr)
		return itr->second;

	return nullptr;
}


//////////////////////////////////野怪////////////////////////////////////////

Creeps* Creeps::create()
{
	Creeps* pret = new Creeps();
	if (pret)
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return nullptr;
}


Creeps::Creeps() : mID(0)
, mProb(0.0f)
, mType(0)
, mBuffId(0)
{
	
}


Creeps::~Creeps()
{
	
}


void Creeps::readline(std::stringstream& _stream)
{
	std::string strIds;

	_stream >> mID
		>> mType
		>> mProb
		>> mBuffId
		>> mEffectAni;
}


void CreepsTable::purge()
{
	for (auto itr = mTable.begin(); itr != mTable.end(); ++itr)
	{
		if (itr->second)
			delete itr->second;
	}
}


Creeps* CreepsTable::getCreeps(int id)
{
	auto itr = mTable.find(id);
	if (mTable.end() != itr)
		return itr->second;

	return nullptr;
}

NewPlayGiftData::NewPlayGiftData()
{
	id = 0;			//ID
	type = 0;
	moneyType = 0;
	price = 0;
	giftId = 0;		//礼包id
}

NewPlayGiftData::~NewPlayGiftData()
{

}

NewPlayGiftData* NewPlayGiftData::create()
{
	NewPlayGiftData* pret = new NewPlayGiftData();
	if (pret)
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}


void NewPlayGiftData::readline(std::stringstream& _stream)
{
	_stream >> id
		>> type
		>> moneyType
		>> price
		>> giftId;
}

NewPlayGiftList* NewPlayGiftTable::getNewPlayGiftList()
{
	return &mNewPlayGiftList;
}


NewPlayGiftData* NewPlayGiftTable::getNewPlayGiftDataByType(int type)
{
	NewPlayGiftList::iterator it = mNewPlayGiftList.begin();
	if (it != mNewPlayGiftList.end() && (*(it->second)).type == type)
	{
		return it->second;
	}
	return NULL;
}

void RecourcePathItem::readline(std::stringstream& _stream)
{
	std::string str;
	_stream
		>> itemID
		>> path;
}

void RecourcePath::purge()
{
	RecourcePathList::iterator itr = mRecourcePathList.begin();
	while (itr != mRecourcePathList.end())
	{
		delete itr->second;
		itr->second = NULL;
		++itr;
	}
	mRecourcePathList.clear();
}


/*void RecourcePath::initSearchPath()
{
	const std::vector<std::string>& paths = cocos2d::FileUtils::getInstance()->getSearchPaths();
	if (!paths.empty())
	{
		mOriSearchPath.assign(paths.begin(), paths.end());
	}
	cocos2d::FileUtils::getInstance()->setSearchPaths(mOriSearchPath);
	cocos2d::FileUtils::getInstance()->purgeCachedEntries();
}

void RecourcePath::setOriginalSearchPath()
{
	RecourcePath::RecourcePathListIterator itr = RecourcePath::Get()->getRecourcePathIterator();
	while (itr.hasMoreElements())
	{
		std::string seachPath = itr.getNext()->path;
		cocos2d::FileUtils::getInstance()->addSearchPath(seachPath.c_str());
	}
	cocos2d::FileUtils::getInstance()->purgeCachedEntries();
}

void RecourcePath::setAdditionalSearchPath()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	mAdditionalSearchPath = "data/Resources";
#else
	mAdditionalSearchPath = "_additionalSearchPath";
#endif

	std::string searchpath = cocos2d::FileUtils::getInstance()->getWritablePath() + "/" + mAdditionalSearchPath;

	RecourcePath::RecourcePathListIterator itr = RecourcePath::Get()->getRecourcePathIterator();
	while (itr.hasMoreElements())
	{
		std::string path = searchpath + "/" + itr.getNext()->path;
		cocos2d::FileUtils::getInstance()->addSearchPath(path.c_str());
	}
	cocos2d::FileUtils::getInstance()->purgeCachedEntries();
}
*/

/////////////////////////////////////////
PvpChat::PvpChat()
{
	mID = 0;
	mType = 0;
	mTitle = "";
	mText = "";
	mTime = 5;
}

void PvpChat::readline(std::stringstream& _stream)
{
	_stream 
		>> mID
		>> mType
		>> mTitle
		>> mText
		>> mTime;
}

PvpChatMap* PvpChatTable::getPvpChatMap()
{ 
	return &mPvpChatMap;
}

PvpChat* PvpChatTable::getPvpChat(int id)
{ 
	PvpChatList::iterator it = mPvpChatList.find(id);
	if (it != mPvpChatList.end())
	{
		return it->second;
	}
	return NULL;
}

std::vector<PvpChat*>* PvpChatTable::getChatType(int type)
{
	PvpChatMap::iterator it = mPvpChatMap.find(type);
	if (it != mPvpChatMap.end())
	{
		return &it->second;
	}
	return NULL;
}


ErrorCode* ErrorCode::create()
{
	ErrorCode* pret = new ErrorCode();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void ErrorCode::readline(std::stringstream& _stream)
{
	_stream >> id
		>> text;
}

const ErrorCode* ErrorCodeTable::getErrorCodeByID(int id)
{
	ErrorCodeList::iterator it = mErrorCodeList.find(id);
	if (it != mErrorCodeList.end())
	{
		return it->second;
	}
	return NULL;
}

PayLimit* PayLimit::create()
{
	PayLimit* pret = new PayLimit();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void PayLimit::readline(std::stringstream& _stream)
{
	_stream>>id
		>>type
		>>l_day
		>>l_month;
}

 const PayLimit* PayLimitTable::getPayLimit()
{
	int pOpertatorID = sgChannelApi()->getOperatorId();
	if (pOpertatorID == 2)
	{
		pOpertatorID = 3;
	}

	auto pRet = mLimitMap.find(pOpertatorID);
	if(pRet!=mLimitMap.end())
	{
		return (*pRet).second;
	}
	return NULL;
}

 Bible* Bible::create()
 {
	 Bible* pret = new Bible();
	 if (pret && pret->init())
	 {
		 return pret;
	 }
	 CC_SAFE_DELETE(pret);
	 return NULL;
 }

 void Bible::readline(std::stringstream& _stream)
 {
	 _stream >> id
		 >> title
		 >> mark
		 >> icon
		 >> loop
		 >> level;
 }

 Bible* BibleTable::getBible(int id)
 {
	 auto itr = mBibleMap.find(id);
	 if (mBibleMap.end() != itr)
		 return itr->second;

	 return nullptr;
 }

 CrazyWeekInfo* CrazyWeekInfo::create()
 {
	 CrazyWeekInfo* info = new CrazyWeekInfo();
	 if (info && info->init()){
		 return info;
	 }
	 CC_SAFE_DELETE(info);
	 return nullptr;
 }

 void CrazyWeekInfo::readline(std::stringstream& _stream)
 {
	 _stream >> id 
		 >> pvpWinCount
		 >> pvpCount
		 >> rewardId 
		 >> condition 
		 >> subCondition; 
 }

 const CrazyWeekInfo* CrazyWeekTable::getCrazyWeekInfoById(int id)
 {
	 auto itr = mCrazyWeekMap.find(id);
	 if (mCrazyWeekMap.end() != itr) 
	 {
		 return itr->second;
	 }

	 return nullptr;
 }


 /////////////////////////////ExchangeItem/////////////////////////////////////////////

 ExchangeItem* ExchangeItem::create()
 {
	 ExchangeItem* pret = new ExchangeItem();
	 if (pret)
	 {
		 return pret;
	 }
	 CC_SAFE_DELETE(pret);
	 return nullptr;
 }


 ExchangeItem::ExchangeItem() 
	 : mID(0)
 //, mType(0)
 //, mItemId(0)
 //, mValue(0)
 , mGiftId(201)
 , mDailyCount(0)
 , mNeedScore(0)
 , mFreshTime(0)
 {

 }


 ExchangeItem::~ExchangeItem()
 {

 }


 void ExchangeItem::readline(std::stringstream& _stream)
 {
	 _stream >> mID
		 >> mGiftId
		 >> mDailyCount
		 >> mNeedScore
		 >> mFreshTime;
 }


 void ExchangeItemTable::purge()
 {
	 for (auto itr = mTable.begin(); itr != mTable.end(); ++itr)
	 {
		 if (itr->second)
			 delete itr->second;
	 }
 }


 ExchangeItem* ExchangeItemTable::getExchangeItem(int id)
 {
	 auto itr = mTable.find(id);
	 if (mTable.end() != itr)
		 return itr->second;

	 return nullptr;
 }

 int ExchangeItemTable::getExchangeItemCount()
 {
	 return mTable.size();
 }


 /////////////////////////////ExchangeItem/////////////////////////////////////////////

 GradeData* GradeData::create()
 {
	 GradeData* pret = new GradeData();
	 if (pret)
	 {
		 return pret;
	 }
	 CC_SAFE_DELETE(pret);
	 return nullptr;
 }


 GradeData::GradeData() : mID(0)
	 , mGroup(0)
	 , mMinPoint(0)
	 , mMaxPoint(0)
	 , mRandWinScore(0)
	 , mRandLoseScore(0)
 {

 }


 GradeData::~GradeData()
 {

 }


 void GradeData::readline(std::stringstream& _stream)
 {
	 std::string strIds;

	 _stream >> mID
		 >> mName
		 >> mGroup
		 >> mMinPoint
		 >> mMaxPoint
		 >> mRandWinScore
		 >> mRandLoseScore;
 }



 void GradeDataTable::purge()
 {
	 for (auto pData : mTable)
	 {
		 if (pData)
			 delete pData;
	 }
 }


 GradeData* GradeDataTable::getGradeData(int point)
 {
	 for (auto rItr = mTable.rbegin(); rItr != mTable.rend(); ++rItr)
	 {
		 auto pData = *rItr;
		 if (pData && point >= pData->getMinPoint())
			 return pData;
	 }

	 return nullptr;
 }

 GradeData* GradeDataTable::getGradeByGrade(int id)
 {
	 for (auto Itr = mTable.begin(); Itr != mTable.end(); ++Itr)
	 {
		 auto pData = *Itr;
		 if (pData && id == pData->getGroup())
			 return pData;
	 }
	 return NULL;
 }

 /////////////////////////////ActInform/////////////////////////////////////////////

 ActInformData* ActInformData::create()
 {
	 ActInformData* pret = new ActInformData();
	 if (pret)
	 {
		 return pret;
	 }
	 CC_SAFE_DELETE(pret);
	 return nullptr;
 }

 ActInformData::ActInformData() : mID(0), mType(0), mActType(0), mShowType1(0), mShowType2(0), mTControlType(0)
 {

 }

 ActInformData::~ActInformData()
 {

 }

 void ActInformData::readline(std::stringstream& _stream)
 {
	_stream >> mID
		 >> mActName
		 >> mType
		 >> mActType
		 >> mShowType1
		 >> mShowType2
		 >> mTControlType
		 >> mOpenTime
		 >> mCloseTime
		 >> mContent
		 >> mPageId;
 }

 ActInformData* ActInformTable::getActInformData(int id)
 {
	 ActInformList::iterator iter = mActInformList.begin();
	 for (; iter != mActInformList.end(); iter++)
	 {
		 if ((*iter)->mID == id)
			 return *iter;
	 }
	 return nullptr;
 }

 void VipData::readline(std::stringstream& _stream)
 {
	 
	 std::string temp_Price;
	 std::string temp_PayType;
	 std::string temp_Time;
	 std::string temp_Describe;
	 std::string temp_ShopID;
	 _stream >> mID
		 >> mName
		 >> mHIcon
		 >> temp_Price
		 >> temp_PayType
		 >> temp_Time
		 >> mPowerSlot
		 >> mEnergySlot
		 >> mAddHonor
		 >> mShopDiscount
		 >> temp_Describe
		 >> temp_ShopID;
	 std::vector<std::string> vPrice = splitStr(temp_Price, "#");
	 std::vector<std::string> vPayType = splitStr(temp_PayType, "#");
	 std::vector<std::string> vTime = splitStr(temp_Time, "#");
	 std::vector<std::string> vShopID = splitStr(temp_ShopID, "#");
	  mDescribeID = splitStr(temp_Describe, "#");
	 for (int i = 0;i < 2; i++)
	 {
		 m_sVipType.mPrice =atoi(vPrice[i].c_str());
		 m_sVipType.mPayType = atoi(vPayType[i].c_str());
		 m_sVipType.mTime = atoi(vTime[i].c_str());
		 m_sVipType.mShopID = atoi(vShopID[i].c_str());
		 mVipTypeMap.insert(std::make_pair(i+1, m_sVipType));
	 }
 }

 VipData* VipData::create()
 {
	 VipData* pret = new VipData();
	 if (pret)
	 {
		 return pret;
	 }
	 CC_SAFE_DELETE(pret);
	 return nullptr;
 }

sVipType VipData::getVipType(int id)
 {
	 auto it = mVipTypeMap.find(id);
	 if (it != mVipTypeMap.end())
	 {
		 return it->second;
	 }
	 CCLOG("[fault]VipData::getVipType ,no fount id:%d",id);

	 return sVipType();
 }

 void VipDataTable::purge()
 {
	 for (auto pData : mTable)
	 {
		 if (pData)
			 delete pData;
	 }
 }

 VipData* VipDataTable::getVipData(int id)
 {
	 for (auto pData : mTable)
	 {
		 if (pData)
		 {
			 if (pData->getID() == id)
				 return pData;
		 }
	 }
	 CCLOG("[fault]VipDataTable :getVipData fault ,id:%d",id);
	 return nullptr;
 }

 void LevelData::readline(std::stringstream& _stream)
 {
	 _stream >> mLevel
		 >> mLevelEX;

 }

 LevelData* LevelData::create()
 {
	 LevelData* pret = new LevelData();
	 if (pret)
	 {
		 return pret;
	 }
	 CC_SAFE_DELETE(pret);
	 return nullptr;
 }

 int LevelTable::getLevelData(int level)
 {
	 LevelDataMap::iterator it= mLevelDataMap.find(level);
	 if (it != mLevelDataMap.end())
	 {
		 if (it->first == level)
			 return it->second;

	 }
	 CCLOG("[fault]LevelTable::getLevelData ,don't Find level:%d data",level);
	 return 0;
 }


 ///////////////////

 void GlyphStuffData::readline(std::stringstream& _stream)
 {
	 _stream >> mStuffID
		 >> mStuffName
		 >> mStuffIcon
		 >> mStuffClass
		 >> mStuffDesc;

 }

 GlyphStuffData* GlyphStuffData::create()
 {
	 GlyphStuffData* pret = new GlyphStuffData();
	 if (pret)
	 {
		 return pret;
	 }
	 CC_SAFE_DELETE(pret);
	 return nullptr;
 }

 GlyphStuffData* GlyphStuffTable::getGlyphStuffData(int id)
 {
	 GlyphStuffList::iterator it = mGlyphStuffList.find(id);
	 if (it != mGlyphStuffList.end())
	 {		 
		 return it->second;
	 } 
	 return NULL;
 }


 void PlayerInfo::readline(std::stringstream& _stream)
 {
	 _stream >> mInviteLevel;
 }

 PlayerInfo* PlayerInfo::create()
 {
	 PlayerInfo* pret = new PlayerInfo();
	 if (pret)
	 {
		 return pret;
	 }
	 CC_SAFE_DELETE(pret);
	 return nullptr;
 }

 Intelligent* Intelligent::create()
 {
	 Intelligent* pret = new Intelligent();
	 if (pret && pret->init())
	 {
		 return pret;
	 }
	 CC_SAFE_DELETE(pret);
	 return NULL;
 }

 void Intelligent::readline(std::stringstream& _stream)
 {
	 _stream >> id
		     >> priority
			 >> isable;
 }

 const Intelligent* IntelligentTable::getIntelligent(int id)
 {
	 auto itr = mIntelligentMap.find(id);
	 if (mIntelligentMap.end() != itr)
		 return itr->second;

	 return nullptr;
 }


 //////////////////////////////////徽章////////////////////////////////////////
 BadgeData* BadgeData::create()
 {
	 BadgeData* pret = new BadgeData();
	 if (pret)
	 {
		 return pret;
	 }
	 CC_SAFE_DELETE(pret);
	 return nullptr;
 }

  
 BadgeData::BadgeData() : mId(0)
 {

 }


 BadgeData::~BadgeData()
 {

 }


 void BadgeData::readline(std::stringstream& _stream)
 {
	 std::string strIds;

	 _stream >> mId
		 >> mActive
		 >> mName
		 >> mAlias
		 >> mIntroduce
		 >> mJumpPoint
		 >> mIcon
		 >> mPath;
 }



 void BadgeDataTable::purge()
 {
	 for (auto pData : mCache)
	 {
		 if (pData)
			 delete pData;
	 }
 }


 BadgeData* BadgeDataTable::getBadge(int id) const
 {
	 for (auto pData : mCache)
	 {
		 if (pData && pData->getId() == id)
			 return pData;
	 }

	 return nullptr;
 }

  GoodsInfo* GoodsInfo::create()
 {
	 GoodsInfo* pret = new GoodsInfo();
	 if (pret)
	 {
		 return pret;
	 }
	 CC_SAFE_DELETE(pret);
	 return NULL;
 }

  void GoodsInfo::readline(std::stringstream& _stream)
 {
	 _stream >> mType
		 >> mName
		 >> mIcon
		 >> mIconBig;
	 mIconSmall = mIcon;
 }

  GoodsInfo * GoodsInfoTable::getGoodsInfo(int type)
  {
	  auto it = mGoodsInfoMap.find(type);
	  if (it != mGoodsInfoMap.end())
	  {
		  return it->second;
	  }

	  return NULL;
  }

  NameArea* NameArea::create()
  {
	  NameArea* pret = new NameArea();
	  if (pret)
	  {
		  return pret;
	  }
	  CC_SAFE_DELETE(pret);
	  return NULL;
  }

  void NameArea::readline(std::stringstream& _stream)
  {
	  _stream >> mID
		  >> mType
		  >> mName;

  }

  NameArea * NameAreaTable::getNameArea(int id)
  {
	  auto it = mNameAreaMap.find(id);
	  if (it != mNameAreaMap.end())
	  {
		  return it->second;
	  }
	  return NULL;
  }


  

  ScoreInfo* ScoreInfo::create()
  {
	  ScoreInfo* pret = new ScoreInfo();
	  if (pret)
	  {
		  return pret;
	  }
	  CC_SAFE_DELETE(pret);
	  return NULL;
  }

  void ScoreInfo::readline(std::stringstream& _stream)
  {
	  _stream >> id
		  >> useScore
		  >> fightScore
		  >> percentScore;
  }

  ScoreInfo* ScoreInfoTable::getScoreInfo(int id)
  {
	  auto it = mScoreInfoMap.find(id);
	  if (it != mScoreInfoMap.end())
	  {
		  return it->second;
	  }
	  return NULL;
  }

  SevenDayMission::SevenDayMission()
  {

  }

  SevenDayMission::~SevenDayMission()
  {

  }

  SevenDayMission* SevenDayMission::create()
  {
	  SevenDayMission* pret = new SevenDayMission();
	  if (pret)
	  {
		  return pret;
	  }
	  CC_SAFE_DELETE(pret);
	  return NULL;
  }

  void SevenDayMission::readline(std::stringstream& _stream)
  {
	  _stream >> mID
		  >> mType
		  >> mCondition
		  >> mPara
		  >> mReward;
  }

  SevenDayMission* SevenDayMissionTable::getDataByDay(int day)
  {
	  for (std::map<int, SevenDayMission*>::iterator iter = mScoreInfoMap.begin(); iter != mScoreInfoMap.end(); iter++)
	  {
		  if (day == iter->second->getID())	//id这一列就是天
		  {
			  return iter->second;
		  }
	  }

	  return NULL;
  }

  void SevenDayMissionTable::readline(std::stringstream& _stream)
  {
	  SevenDayMission* pInfo = SevenDayMission::create();
	  pInfo->readline(_stream);
	  mScoreInfoMap.insert(std::make_pair(pInfo->getID(), pInfo));
  }
