#include <sstream>
#include "StarModes.h"
#include "TimeCalculator.h"
#include "MessageManager.h"
#include "data/TableManager.h"
#include "battle/BattleControl.h"
#include "battle/Build.h"
#include "battle/Camp.h"
#include "BattleInfo.h"
#include "HelperFunc.h"

BaseStar::~BaseStar()
{
	sgMessageManager()->removeMessageHandler(this);
}

bool BaseStar::init(std::stringstream& ssparams)
{
	ssparams >> param1;

	registerMessageType();
	return true;
}

void BaseStar::registerMessageType()
{
	sgMessageManager()->regisiterMessageHandler(EMT_SUCC_CHECKSTAR, this);
	sgMessageManager()->regisiterMessageHandler(EMT_BUILD_DESTORY, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PRODUCT, this);
	sgMessageManager()->regisiterMessageHandler(EMT_UPGRADE, this);
}

std::string BaseStar::getStarTextByTag(eTextTagStar eTextTag)
{
	std::string szTemp = "";
	char szText[128] = { 0, };
	switch (mEndSuccess)
	{
	case  -1:
		snprintf(szText, 128, sgTextHelper(eTextTag).c_str(), param1);
		break;
	case 0:
		szTemp = /*"  " + sgTextHelper(TEXT_STAR_FAILED) + */sgTextHelper(eTextTag);
		snprintf(szText, 128, szTemp.c_str(), param1);
		break;
	case 1:
		szTemp = /*"  " + sgTextHelper(TEXT_STAR_SUCCESS) +*/ sgTextHelper(eTextTag);
		snprintf(szText, 128, szTemp.c_str(), param1);
		break;
	default:
		break;
	}
	return szText;
}

void BaseStar::setEndTag(int statu)
{
	if (mEndSuccess == statu/* || mEndSuccess != -1*/)
		return;

	mEndSuccess = statu;

	char params[128] = { 0, };
	sprintf(params, "%d %d", mEndSuccess,getStarTag());
	sgMessageManager()->sendMessage(EMT_STAR_STATU, params);
}

//======================================================================================
StarSuccessInN::~StarSuccessInN()
{
	sgTimeCalculator()->removeTimeCalcultor("star_sucinN");
}

bool StarSuccessInN::init(std::stringstream& ssparams)
{
	BaseStar::init(ssparams);

	return true;
}

void StarSuccessInN::update()
{
	if (mEndSuccess == -1)
	{
		float _left = sgTimeCalculator()->getTimeLeft("star_sucinN");
		if (_left <= 0.0f)
		{
			setEndTag(0);
		}
	}
}

void StarSuccessInN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		MessageCheckStar* pMes = dynamic_cast<MessageCheckStar*>(message);
		if (pMes && pMes->mPlayerSuccess == 1)
		{
			if (mEndSuccess == -1)
				setEndTag(1);
		}
	}
}

std::string StarSuccessInN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_SuccessN);
}

void StarSuccessInN::startGameTime()
{
	sgTimeCalculator()->createTimeCalcultor("star_sucinN", param1);
}

//======================================================================================

void EnemyBuildsLN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		const CampList& camplist = sgBattleControl()->getCampList();
		CampList::const_iterator it = camplist.begin();
		for (; it != camplist.end(); ++it)
		{
			if (it->first != ECAMP_TYPE_Player && it->first != ECAMP_TYPE_Neutral && it->second->getBuildList().size() > param1)
			{
				setEndTag(0);
				break;
			}
		}
	}
	else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		MessageCheckStar* pCheckStar = dynamic_cast <MessageCheckStar*>(message);
		if (pCheckStar && pCheckStar->mPlayerSuccess == 1)
		{
			if (mEndSuccess == -1)
				setEndTag(1);
		}
	}
	
}

std::string EnemyBuildsLN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_EnemyBuildsLN);
}


//======================================================================================

void StarLastNeutralBuilds::onReceiveMassage(GameMessage * message)
{
	Camp* pNeutral = sgBattleControl()->getTargetCamp(ECAMP_TYPE_Neutral);
	if (!pNeutral)
		return;

	int nBuildSize = pNeutral->getBuildList().size();

	if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		if (nBuildSize < param1)
		{
			setEndTag(0);
		}
	}else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		MessageCheckStar* pMes = dynamic_cast<MessageCheckStar*>(message);
		if (pMes && pMes->mPlayerSuccess == 1)
		{
			if (mEndSuccess == -1 && nBuildSize >= param1)
				setEndTag(1);
		}
	}
}

std::string StarLastNeutralBuilds::getStarText()
{
	return getStarTextByTag(TEXT_STAR_EneBuild_LastLN);
}

//======================================================================================
bool StarGetTarBuilds::init(std::stringstream& ssparams)
{
	registerMessageType();

	for (int i = 0; i != 5; ++i)
		param[i] = 0;

	std::string szTemp;
	ssparams >> szTemp;

	HelperFunc::replaceAllCharacters(szTemp, "+", "\t");

	std::stringstream params(szTemp);
	for (int i = 0; i != 5; ++i)
		params >> param[i];
 
	return true;
}

std::string StarGetTarBuilds::getStarText()
{
	return getStarTextByTag(TEXT_STAR_GetTar_Builds);
}

void StarGetTarBuilds::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		MessageCheckStar* pMes = dynamic_cast<MessageCheckStar*>(message);
		if (pMes && pMes->mPlayerSuccess == 1)
		{
			bool bOwnTags = true; // 是否占领全部标记建筑

			for (int i = 0; i != 5; ++i)
			{
				int insID = param[i];

				BaseBuild* pBuildIns = sgBattleControl()->getTargetBuildByIns(insID);
				if (!pBuildIns)
					continue;

				if (pBuildIns->getCamp() != ECAMP_TYPE_Player)
				{
					bOwnTags = false;
					break;
				}
			}

			if (bOwnTags)
				setEndTag(1);
			else
				setEndTag(0);
		}
	}
}

void StarOwnPeopleMEneN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_PRODUCT)
	{
		MessageProduct* pMes = dynamic_cast<MessageProduct*>(message);
		if (pMes)
		{
			if (mEndSuccess != -1)
				return;

			Camp* pOwnCamp = sgBattleControl()->getTargetCamp(ECAMP_TYPE_Player);
			if (!pOwnCamp)
				return;

			int iOwnPro = pOwnCamp->getAllUnit();

			// 比兵力最多的AI阵营 兵力多出N时，达成评星
			int	iSecPro = 0;
			eCampType eSecCamp = ECAMP_TYPE_Invalid;
			for (int i = 2; i != 5; i++)	// ECAMP_TYPE_AI - ECAMP_TYPE_AI3
			{
				eCampType etype = (eCampType)i;

				Camp* pcamp = sgBattleControl()->getTargetCamp(etype);
				if (!pcamp)
					continue;

				int ipro = pcamp->getAllUnit();
				if (iSecPro < ipro)
				{
					iSecPro = ipro;
					eSecCamp = etype;
				}
			}

			if (iOwnPro - iSecPro > param1)
				setEndTag(1);
		}
	}else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		if (mEndSuccess == -1)
			setEndTag(0);
	}
}

std::string StarOwnPeopleMEneN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_People_M_ENE_N);
}

void StarOwnPeopleLN::onReceiveMassage(GameMessage * message)
{
	int curMine = sgBattleControl()->getTargetCamp(ECAMP_TYPE_Player)->getAllUnit();

	if (message->getMessageType() == EMT_PRODUCT)
	{
		MessageProduct* pMes = dynamic_cast<MessageProduct*>(message);
		if (pMes)
		{
			if (mEndSuccess != -1)
				return;

			if (curMine > param1)
				setEndTag(0);
		}
	}else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		MessageCheckStar* pCheckStar = dynamic_cast <MessageCheckStar*>(message);
		if (pCheckStar && pCheckStar->mPlayerSuccess == 1)
		{
			if (mEndSuccess == -1 && curMine <= param1)
				setEndTag(1);
		}
	}
}

std::string StarOwnPeopleLN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_People_LN);
}

void StarOwnPeopleMN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_PRODUCT)
	{
		MessageProduct* pMes = dynamic_cast<MessageProduct*>(message);
		if (pMes)
		{
			Camp* pCamp = sgBattleControl()->getTargetCamp(sgBattleControl()->getOwnCamp());
			if (pCamp)
			{
				int curMine = pCamp->getAllUnit();
				if (mEndSuccess != 1)
				{
					if (curMine > param1)
					{
						setEndTag(1);
					}
				}

			}

		}	
	}
}

std::string StarOwnPeopleMN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_People_MN);
}

void StarEnePeopleLN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_PRODUCT)
	{
		MessageProduct* pMes = dynamic_cast<MessageProduct*>(message);
		{
			if (mEndSuccess != -1)
				return;

			BaseBuild* pBuild = sgBattleControl()->getTargetBuild(pMes->getBuildKey());
			if (!pBuild)
				return;

			if (pBuild->getCamp() == ECAMP_TYPE_Player || pBuild->getCamp() == ECAMP_TYPE_Neutral)
				return;

			// 兵力增长的建筑的阵营人口超过指定值，判定失败
			Camp* pTargetCamp = sgBattleControl()->getTargetCamp(pBuild->getCamp());
			if (!pTargetCamp)
				return;

			int curAI = pTargetCamp->getAllUnit();
			if (curAI > param1)
				setEndTag(0);
		}
	}else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		if (mEndSuccess == -1)
			setEndTag(1);
	}
}

std::string StarEnePeopleLN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_EnePeo_LN);
}

void StarEnePeopleMN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_PRODUCT)
	{
		MessageProduct* pMes = dynamic_cast<MessageProduct*>(message);
		{
			if (mEndSuccess != -1)
				return;

			BaseBuild* pBuild = sgBattleControl()->getTargetBuild(pMes->getBuildKey());
			if (!pBuild)
				return;

			if (pBuild->getCamp() == ECAMP_TYPE_Player || pBuild->getCamp() == ECAMP_TYPE_Neutral)
				return;

			// 兵力增长的建筑的阵营人口超过指定值，判定成功
			Camp* pTargetCamp = sgBattleControl()->getTargetCamp(pBuild->getCamp());
			if (!pTargetCamp)
				return;

			int curAI = pTargetCamp->getAllUnit();
			if (curAI > param1)
				setEndTag(1);
		}
	}
	else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		if (mEndSuccess == -1)
			setEndTag(0);
	}
}

std::string StarEnePeopleMN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_EnePeo_MN);
}

void StarIniBuildLostLN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		MessageBuildDestroy* pDestroy = dynamic_cast<MessageBuildDestroy*>(message);
		if (pDestroy)
		{
			if (pDestroy->getInitCamp() != ECAMP_TYPE_Player)
				return;

			MapBulidList::iterator it = mMineBuilds.find(pDestroy->getKeyID());
			if (it != mMineBuilds.end())
			{
				mMineBuilds.erase(it);
				mInitBuildLost++;

				if (mInitBuildLost >= param1)
					setEndTag(0);
			}
		}
	}else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		if (mEndSuccess == -1)
			setEndTag(1);
	}
}

std::string StarIniBuildLostLN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_BuildLost_LN);
}

bool StarIniBuildLostLN::init(std::stringstream& ssparams)
{
	BaseStar::init(ssparams);

	const MapBulidList& buildlist = sgBattleControl()->getBuildList();
	MapBulidList::const_iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		if (it->second->getBaseCamp() == ECAMP_TYPE_Player)
			mMineBuilds.insert(std::make_pair(it->first, it->second));
	}
	return true;
}

void StarOwnBuildMN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		const MapBulidList& buildlist = sgBattleControl()->getTargetCamp(sgBattleControl()->getOwnCamp())->getBuildList();
		if (buildlist.size() >= param1)
			setEndTag(1);
		else
			setEndTag(0);
	}
}

std::string StarOwnBuildMN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_OwnBuild_MN);
}

void StarSuccLastPeopleMN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		MessageCheckStar* pSuccess = dynamic_cast<MessageCheckStar*>(message);
		if (pSuccess)
		{
			int curOwnPeop = sgBattleControl()->getTargetCamp(ECAMP_TYPE_Player)->getAllUnit();
			if (curOwnPeop > param1)
				setEndTag(1);
			else
				setEndTag(0);
		}
	}
}

std::string StarSuccLastPeopleMN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_SuccOwnPeop_MN);
}

void StarMonsterMN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		MessageBuildDestroy* pDestory = dynamic_cast<MessageBuildDestroy*>(message);
		if (pDestory)
		{
			if (pDestory->getToCamp() != ECAMP_TYPE_Player)
				return;

			int keyID = pDestory->getKeyID();
			BaseBuild* pBuild = sgBattleControl()->getTargetBuild(keyID);
			if (!pBuild)
				return;

			if (pBuild->getBaseCamp() == ECAMP_TYPE_Creeps)
				mMonsterTimes++;
		}
	}else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		if (mMonsterTimes >= param1)
			setEndTag(1);
		else
			setEndTag(0);
	}
}

std::string StarMonsterMN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_KillMonster_MN);
}

bool StarMonsterMN::init(std::stringstream& ssparams)
{
	BaseStar::init(ssparams);

	mMonsterTimes = 0;
	return true;
}

bool StarNoBuildUpgrade::init(std::stringstream& ssparams)
{
	BaseStar::init(ssparams);

	const MapBulidList& buildlist = sgBattleControl()->getBuildList();
	MapBulidList::const_iterator it = buildlist.begin();
	for (; it != buildlist.end(); ++it)
	{
		if (it->second->getBaseCamp() == ECAMP_TYPE_Player)
			mMineBuilds.insert(std::make_pair(it->first, it->second));
	}
	return true;
}

void StarNoBuildUpgrade::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_UPGRADE)
	{
		MessageUpgrade* pUpgrade = dynamic_cast<MessageUpgrade*>(message);
		if (pUpgrade)
		{
			int iKeyid = pUpgrade->getBuild();
			
			BaseBuild* pTarBuild = sgBattleControl()->getTargetBuild(iKeyid);
			if (!pTarBuild)
				return;

			MapBulidList::iterator it = mMineBuilds.find(iKeyid);
			if (it != mMineBuilds.end())
				setEndTag(0);
		}
	}else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		if (mEndSuccess == -1)
			setEndTag(1);
	}
}

std::string StarNoBuildUpgrade::getStarText()
{
	return getStarTextByTag(TEXT_STAR_NoInitBuild_Upgrade);
}


bool StarNoDestroyGetTar::init(std::stringstream& ssparams)
{
	registerMessageType();

	for (int i = 0; i != 5; ++i)
		param[i] = 0;

	for (int i = 0; i != 5; ++i)
		param2[i] = 0;

	
	std::string _param1, _param2;
	ssparams >> _param1 >> _param2;

	std::stringstream ssparam1(_param1), ssparam2(_param2);
	for (int i = 0; i != 5; ++i)
		ssparam1 >> param[i];

	for (int i = 0; i != 5; ++i)
		ssparam2 >> param2[i];
	return true;
}

void StarNoDestroyGetTar::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		MessageBuildDestroy* pDestory = dynamic_cast<MessageBuildDestroy*>(message);
		if (pDestory)
		{
			BaseBuild* pBuild = sgBattleControl()->getTargetBuild(pDestory->getKeyID());
			if (!pBuild)
				return;

			int insID = pBuild->getBuildInfo().insID;
			// 保护的建筑组被占领过，直接判定未达成
			for (int i = 0; i != 5; i++)
			{
				if (param[i] == insID)
				{
					setEndTag(0);
					break;;
				}
			}
		}
	}else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		if (mEndSuccess != -1)
			return;

		// 另外，抢占的建筑全部拿下，判定未达成
		bool bAllOwned = true;
		for (int i = 0; i != 5; ++i)
		{
			if (param2[i] == 0)
				continue;

			BaseBuild* pBuild = sgBattleControl()->getTargetBuildByIns(param2[i]);
			if (!pBuild || pBuild->getCamp() != sgBattleControl()->getOwnCamp())
				bAllOwned = false;
		}

		if (bAllOwned)
			setEndTag(1);
	}
}

std::string StarNoDestroyGetTar::getStarText()
{
	return getStarTextByTag(TEXT_STAR_NoDestroyGetTar);
}

bool StarBuildNoDestroy::init(std::stringstream& ssparams)
{
	registerMessageType();

	for (int i = 0; i != 5; ++i)
		ssparams >> param[i];

	//check
	for (int i = 0; i != 5; ++i)
	{
		int insID = param[i];
		BaseBuild* pBuildIns = sgBattleControl()->getTargetBuildByIns(insID);
		if (!pBuildIns)
		{
			CCLOG("[fault]StarBuildNoDestroy::init no build, insid:%d.", insID);
			continue;
		}

		eCampType ecamp = pBuildIns->getCamp();
		if (ecamp != ECAMP_TYPE_Neutral || ecamp != ECAMP_TYPE_Player)
		{
			CCLOG("[fault]StarBuildNoDestroy::init no build, insid:%d.", insID);
			continue;
		}
	}
	return true;
}

void StarBuildNoDestroy::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_BUILD_DESTORY)
	{
		MessageBuildDestroy* pDestory = dynamic_cast<MessageBuildDestroy*>(message);
		if (pDestory)
		{
			int validID = -1;

			for (int i = 0; i != 5; ++i)
			{
				if (param[i] == pDestory->getKeyID())
				{
					validID = param[i];
					break;
				}
			}

			if (validID == -1)
				return;

			BaseBuild* pBuild = sgBattleControl()->getTargetBuild(pDestory->getKeyID());
			if (!pBuild)
				return;

			if (pBuild->getCamp() != ECAMP_TYPE_Player)
				setEndTag(0);
		}
	}else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		if (mEndSuccess == -1)
			setEndTag(1);
	}
}

std::string StarBuildNoDestroy::getStarText()
{
	return getStarTextByTag(TEXT_STAR_Build_NoDestroy);
}

void StarNoPeopleLN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_PRODUCT)
	{
		MessageProduct* pProduct = dynamic_cast<MessageProduct*>(message);
		if (pProduct)
		{
			if (mEndSuccess != -1)
				return;

			BaseBuild* pBuild = sgBattleControl()->getTargetBuild(pProduct->getBuildKey());
			if (!pBuild)
				return;

			Camp* pProCamp = sgBattleControl()->getTargetCamp(pBuild->getCamp());
			if (pProCamp->getAllUnit() > param1)
				setEndTag(0);
		}
	}
	else if (message->getMessageType() == EMT_SUCC_CHECKSTAR)
	{
		if (mEndSuccess == -1)
			setEndTag(1);
	}
}

std::string StarNoPeopleLN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_OwnPeople_LN);
}

void StarMoraleMN::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_MORALESHOW)
	{
		MessageMoraleShow* pMorale = dynamic_cast<MessageMoraleShow*>(message);
		if (pMorale)
		{
			eCampType eCamp = (eCampType)pMorale->getCampType();
			if (eCamp != ECAMP_TYPE_Player)
				return;

			int iMoraleVal = pMorale->getMoraleValue();

			if (iMoraleVal > param1)
				setEndTag(1);
			else
				setEndTag(0);
		}
	}
}

std::string StarMoraleMN::getStarText()
{
	return getStarTextByTag(TEXT_STAR_MORALE_MN);
}
