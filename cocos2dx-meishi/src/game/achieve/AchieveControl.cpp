#include "AchieveControl.h"
#include "login/LoginProxy.h"
#include "MessageManager.h"
#include "data/TableManager.h"
#include "AchieveTip.h"
#include "core/SoundManager.h"

AchieveControl::AchieveControl(void)
	:mAchievePanel(NULL)
	,mGetCell(NULL)
	,mCanTouch(true)
	,mIsPvpFight(false)
{
}


AchieveControl::~AchieveControl(void)
{
}

AchieveProxy* AchieveControl::getAchieveProxy()
{
	return &m_AchieveProxy;
}

void AchieveControl::sendQueryAchievementStatus()
{
	m_AchieveProxy.sendQueryAchievementStatus();
}
bool AchieveControl::isSendQueryAchievementStatus()
{
	bool pRet = false;
	if (!m_getAchievementResp.has_achievementlevel()||mIsPvpFight)
	{
		pRet = true;
	}
	return pRet;
}

void AchieveControl::sendGetAchievementAward(int id)
{
	mCanTouch = false;
	m_AchieveProxy.sendGetAchievementAward(id);
}

void AchieveControl::sendReceiveAchievementRewardReq(int id)
{
	m_AchieveProxy.sendReceiveAchievementRewardReq(id);
}

void AchieveControl::doQueryAchievementStatus(const fgame::CSGetAchievementResp& resp)
{
	mIsPvpFight = false;
	m_getAchievementResp = resp;
	//sgUserObj()->setAchieveLv(resp.achievementlevel());
	sgUserObj()->getPersonalInfo()->set_achievementlevel(resp.achievementlevel());
	updateUI();
	
}

void AchieveControl::doReceiveAchievement(const fgame::CSReceiveAchievementResp& recResp)
{
	int pRecId = 0;
	if (recResp.ret() == 0)
	{
		pRecId = recResp.achievementid();
		if (mAchievePanel)
			mAchievePanel->showAward(pRecId, true);
		if (mGetCell)
		{
			mRecId = pRecId;
			if (mAchievePanel)
				mAchievePanel->removeTableViewCell(mGetCell);
		}
	}else
	{
		mCanTouch = true;
		if (mAchievePanel)
			mAchievePanel->showAward(recResp.ret(), false);
	}
		
	
}

void AchieveControl::doReceiveAward(const fgame::CSReceiveAchievementRewardResp& resp)
{
	sgSoundManager()->playMusicEffect(SOUND_ITEM_GAIN);
	if (resp.ret() == 0)
	{
		if (mAchievePanel)
			mAchievePanel->showAwardByLv(resp.achievementlevel());
	}else
	{
		if (mAchievePanel)
			mAchievePanel->showAward(resp.ret(),false);
	}
}

void AchieveControl::scrolView(int percnt)
{
	if (mAchievePanel)
	{
		mAchievePanel->scrolViewSlider(percnt);
	}
	
}

void AchieveControl::setRemCell(TableViewCell* pCell)
{
	mGetCell = pCell;
}

void AchieveControl::setCanTouch(bool pb)
{
	mCanTouch = pb;
}

void AchieveControl::closeAchievePanel()
{
	if (mAchievePanel && mAchievePanel->isShow())
	{
		mAchievePanel->setShow(false);
	}
}

void AchieveControl::updateLocalData(std::list<int> pComplentList)
{
	for (auto it = pComplentList.begin(); it != pComplentList.end(); it++)
	{
		for (int i = 0; i<m_getAchievementResp.data_size(); i++)
		{
			if ((*it)== m_getAchievementResp.data(i).id())
			{
				if (m_getAchievementResp.data(i).status() != 2)
				{
					continue;
				}
				m_getAchievementResp.mutable_data(i)->set_status(1);
			}
		}
	}

	updateUI();
}

void AchieveControl::TipAchieveComplent(std::list<int> pComplentList)
{
	if(pComplentList.size()>0)
	{
		log("[trace] AchieveControl::TipAchieveComplent");
		//±íÏÖ
		AchieveTip* pAchieve = AchieveTip::create();
		Director::getInstance()->getNotificationNode()->addChild(pAchieve);
		pAchieve->updateUI(pComplentList);
	}

	updateLocalData(pComplentList);
}

void AchieveControl::updateUI()
{
	sortAchieve();
	checkComplentTip();
	if (mAchievePanel&& mAchievePanel->isShow())
	{
		mAchievePanel->updateUI(m_getAchievementResp);
	}
}

void AchieveControl::sortAchieve()
{
	std::map<int ,AchievementAward::AchieveInfo> pStatusMap;
	for (int i = 0; i< m_getAchievementResp.data_size(); i++)
	{
		AchievementAward::AchieveInfo pAchieveInfo;
		pAchieveInfo.mstatus = m_getAchievementResp.data(i).status();
		pAchieveInfo.mReceivePlayerCount = m_getAchievementResp.data(i).receiveplayercount();
		if (m_getAchievementResp.data(i).has_completecount())
		{
			pAchieveInfo.mCompleteCount = m_getAchievementResp.data(i).completecount();
		}else
			pAchieveInfo.mCompleteCount = 0;

		pStatusMap.insert(std::make_pair(m_getAchievementResp.data(i).id(),pAchieveInfo));
	}
	if (m_getAchievementResp.data_size()>0)
	{
		sgTableManager()->sortAchievementAward(pStatusMap);
	}
}

void AchieveControl::setAchieveGeted()
{
	for(int i = 0; i<m_getAchievementResp.data_size(); i++)
	{
		if(m_getAchievementResp.data(i).id() == mRecId)
		{
			m_getAchievementResp.mutable_data(i)->set_status(0);
			m_getAchievementResp.mutable_data(i)->set_receiveplayercount(m_getAchievementResp.data(i).receiveplayercount()+1);
			m_getAchievementResp.set_achievementpointcount(m_getAchievementResp.achievementpointcount() + sgTableManager()->getAchievementById(mRecId)->Award);
			break;
		}
	}

	updateUI();
}

void AchieveControl::checkComplentTip()
{
	if (sgTableManager()->getAchievementAwardCount(A_Last)>0)
	{
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 1", MSG_FUC_ACHIEVE));
	}
	else
	{
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 0", MSG_FUC_ACHIEVE));
	}
}
