#include "WinnerControl.h"
#include "net/UserObj.h"
#include "MessageManager.h"
#include "achieve/AchieveTip.h"
#include "data/TableManager.h"

#define WINNER_COUNT (20)
WinnerControl::WinnerControl(void)
	:mWinnerDialog(NULL)
{
	m_IsGet = false;
	m_WinnerCount = 0;
	m_GetCount = 0;
	mBtnRefresh = false;
	setWinnerRoadOpen(0);
}


WinnerControl::~WinnerControl(void)
{
}


void WinnerControl::sendGetRqst()
{
	mWinnerProxy.sendGetRqst();
}

void WinnerControl::sendWinnerValueRqst()
{
	mWinnerProxy.sendWinnerValueRqst();
}

void WinnerControl::setWinnerCount(int val)
{
	m_WinnerCount = val;
// 	if (m_WinnerCount >= WINNER_COUNT)
// 	{
// 		sgUserObj()->addBadge(TitleType::WINNER);
// 	}
	sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d 0",MSG_FUC_WINNER));
}

bool WinnerControl::getCanGet()
{
	bool flag = false;
	std::map<int, WinnerActivityData>::iterator mData = mWinnerActList.begin();
	for(; mData != mWinnerActList.end();mData++)
	{
		auto mSubData = mData->second.SubList.find(mData->second.Cur_SubId);
		if (mSubData != mData->second.SubList.end())
		{
			if (mSubData->second.Completed && !mSubData->second.Submid)
			{
				mData->second.IsCanGet = true;
				flag = true;
			}
			else
			{
				mData->second.IsCanGet = false;
			}
		}
		else if (mData->first == WINNER_ROAD + 1)
		{
			if (m_WinnerCount && !m_IsGet)
			{
				flag = true;
				mData->second.IsCanGet = true;
			}
			else
			{
				mData->second.IsCanGet = false;
			}
		}
	}

	return flag;
}

bool WinnerControl::getWinnerActOpen(int actID)
{
	std::map<int, WinnerActivityData>::iterator mData = mWinnerActList.find(actID);
	if (mData != mWinnerActList.end())
		return true;
	return false;
}

WinnerActivityData* WinnerControl::getWinnerActList(int actId)
{
	std::map<int, WinnerActivityData>::iterator mData = mWinnerActList.find(actId);
	if (mData != mWinnerActList.end())
	{
		return &mData->second;
	}

	return NULL;
}

WinnerSubActivity* WinnerControl::getWinnerSubID(int actId,int subId)
{
	std::map<int, WinnerActivityData>::iterator mData = mWinnerActList.find(actId);
	if (mData != mWinnerActList.end())
	{
		std::map<int, WinnerSubActivity>::iterator subData = mData->second.SubList.find(subId);
		if (subData != mData->second.SubList.end())
		{
			return &subData->second;
		}
	}

	return NULL;
}

void WinnerControl::sendTaskComplete(int actId)
{
	//±íÏÖ
	AchieveTip* pAchieve = AchieveTip::create();
	Director::getInstance()->getNotificationNode()->addChild(pAchieve);

	std::string icon = StringUtils::format("ui/general/Winner_%d_Btn.png",actId);
	pAchieve->updateUI(icon, sgTextHelper(TEXT_WINNER_CONFIRM + actId), sgTextHelper(TEXT_WINNER_COMPLENT));
}

void WinnerControl::sendWinnerTip(int cout, int gif)
{
	AchieveTip* pAchieve = AchieveTip::create();
	Director::getInstance()->getNotificationNode()->addChild(pAchieve);

	std::string icon = StringUtils::format("ui/general/Winner_%d_Btn.png", WINNER_ROAD + 1);
	std::string content;

	if (cout <= 30)
	{
		content = StringUtils::format(sgTextHelper(TEXT_WINNER_CONTENT_1).c_str(),cout, gif);
	}
	else
	{
		content = StringUtils::format(sgTextHelper(TEXT_WINNER_CONTENT_2).c_str(), gif);
	}

	pAchieve->updateUI(icon, sgTextHelper(TEXT_COMM_TIPS), content);

}

bool WinnerControl::checkCanOpen(int type)
{
	auto iteor = mWinnerActList.find(type + 1);
	if (iteor != mWinnerActList.end())
	{
		return true;
	}

	return false;
}

#define WINNER_ROAD_START (10)
#define WINNER_ROAD_END (22)
void WinnerControl::setWinnerRoadOpen(unsigned long long _time)
{
	CVTime targetTime(_time == 0 ? time(0) : _time);
	if (targetTime.GetHour() >= WINNER_ROAD_START && targetTime.GetHour() < WINNER_ROAD_END)
	{
		if (mWinnerRoadOpen != true)
		{
			mWinnerRoadOpen = true;
			if (mWinnerDialog)
			{				
				mWinnerDialog->getWinPanel(WINNER_ROAD)->update(3.0);
			}
		}
		return;
	}
	if (mWinnerRoadOpen != false)
	{
		mWinnerRoadOpen = false;
		if (mWinnerDialog)
		{
			mWinnerDialog->getWinPanel(WINNER_ROAD)->update(3.0);
		}
	}
}
