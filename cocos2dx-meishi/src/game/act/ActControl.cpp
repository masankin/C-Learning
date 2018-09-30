#include "ActControl.h"
#include "ui/PageFunction.h"
#include "guide/GuideManager.h"
#include "game/net/UserObj.h"
#include "data/MailData.h"
#include "core/CVTime.h"
#include "ui/dialogs/GiftAwardDialog.h"
#include "login/LoginProxy.h"
#include "core/SoundManager.h"
#include "ActFirstCharge.h"
#include "game/utils/GameDefault.h"
ActControl::ActControl(void)
	:mActPanel(NULL),
	mActLoginPanel(NULL),
	mIsCanAutoShow(true),
	mDailyCount(0),
	mRechargeCount(0),
	mRechargeId(2),
	mDailyId(0),
	mFirstId(1),
	mIsNewerGiftOpened(false),
	mQueryLoginDay(0),
	mBtnRefresh(false),
	mIsShowFirstCharge(false),
	mIsChristmasActOpen(false),
	mIsTopRankActOpen(false)
{
	mActTypeEff.insert(std::make_pair(ACT_LOGIN,false));
	//mActTypeEff.insert(std::make_pair(ACT_FIRST_CHARGE,false));
	//mActTypeEff.insert(std::make_pair(ACT_RECHARGE,false));
}


ActControl::~ActControl(void)
{
}

ActProxy* ActControl::getActProxy()
{
	return &mActProxy;
}

void ActControl::sendQueryActLogin()
{
	if (!isSameDay(mQueryLoginDay))
	{
		mActProxy.sendQueryActLogin();
	}
}

void ActControl::sendGetActLogin()
{
	mActProxy.sendGetActLogin();
}

void ActControl::doRueryActLogin(const fgame::CSSevenDaySignListResp& resp)
{
	CVTime pnowTime(sgLoginProxy()->getServerTime());
	mQueryLoginDay = pnowTime.GetDay();

	bool pIsShow = false;
	for (int i = 0; i<resp.nowday(); i++)
	{
		if (resp.getlist(i).isget() == false)
		{
			pIsShow = true;
			break;
		}
	}
	
	setActCanGetByType(ACT_LOGIN,pIsShow);
	if (mActPanel && mActPanel->isShow())
	{
		mActPanel->updateUI(ACT_LOGIN);
	}
}

void ActControl::doGetActLogin(const fgame::CSSevenDaySignResp& resp)
{
	mQueryLoginDay = 0;
	if (resp.giftid()>0)
	{	
		sgSoundManager()->playMusicEffect(SOUND_ITEM_GAIN);
		GiftAwardDialog::showDialogByGiftId(resp.giftid());
#ifdef MGAME_PLATFORM_MPLUS
		sgMplusControl()->setIsCanShare(true);
#endif
	}
	if (mActPanel && mActPanel->isShow())
	{
		sendQueryActLogin();
	}else
	{
		if (mActLoginPanel && mActLoginPanel->isShow())
		{
			sendQueryActLogin();
		}
	}
}

void ActControl::doRechargeCheck()
{
	mDailyId = mRechargeId = 0;

	//bool firstCharge = false;
	//std::map<int,bool>::iterator it = mActTypeEff.find(ACT_FIRST_CHARGE);
	//if (it != mActTypeEff.end())
	//{
	//	firstCharge = true;	
	//}
	int id[2] = {0,0};
	RechargeList::iterator _iter = sgTableManager()->getRechargeList()->begin();
	for (; _iter != sgTableManager()->getRechargeList()->end(); _iter++)
	{
		bool flag = _iter->second->getIsGet() == 1? true : false;
		switch (_iter->second->getRechargeType())
		{
		case ACT_FIRST_CHARGE:
			{
				mFirstId = _iter->second->getID();
				if (mIsShowFirstCharge)
				{
					//setActCanGetByType(ACT_FIRST_CHARGE,flag);
					// 显示首冲红点
					sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d %d", MSG_FUC_FIRST_CHARGE, flag ? 1 : 0));
				}
				break;
			}
		case ACT_DAILY_CHARGE:
			{
				id[0] = _iter->second->getID();
				if(mDailyId != 0) continue;
				if (flag || _iter->second->getRechargeNum() > mDailyCount)
				{
					mDailyId = _iter->second->getID();
					setActCanGetByType(ACT_DAILY_CHARGE,flag);
				}
				break;
			}
		case ACT_RECHARGE:
			{
				id[1] = _iter->second->getID();
				if (mRechargeId != 0) continue;
				if (flag || _iter->second->getRechargeNum() > mRechargeCount)
				{
					mRechargeId = _iter->second->getID();
					setActCanGetByType(ACT_RECHARGE,flag);
					sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d %d", MSG_FUC_TOTAL_CHARGE, flag ? 1 : 0));
				}
				break;
			}
		default:
			break;
		}
	}
	if(mDailyId == 0)
	{
		mDailyId = id[0];
	}

	if(mRechargeId == 0)
	{
		mRechargeId = id[1];
	}
}

bool ActControl::getCurrRechargeData(int type)
{
	RechargeList::iterator _iter = sgTableManager()->getRechargeList()->begin();
	for (; _iter != sgTableManager()->getRechargeList()->end(); _iter++)
	{
		if (_iter->second->getRechargeType() != type) continue;
		bool flag = _iter->second->getIsGet() == 1? true : false;
		int id = _iter->second->getID();
		switch (type)
		{
		case ACT_FIRST_CHARGE:
			{
				if (id > mFirstId)
				{
					mFirstId = id;
					setActCanGetByType(type,flag);
					return true;
				}
				break;
			}
		case ACT_DAILY_CHARGE:
			{
				if (id > mDailyId)
				{
					mDailyId = id;
					setActCanGetByType(type,flag);
					return true;
				}
				break;
			}
		case ACT_RECHARGE:
			{
				if (id > mRechargeId)
				{
					mRechargeId = id;
					setActCanGetByType(type,flag);
					return true;
				}
				break;
			}
		default:
			break;
		}
	}
	return false;
}

bool ActControl::checkCurrActCanGet()
{
	std::map<int,bool>::iterator itr = mActTypeEff.begin();
	for (; itr != mActTypeEff.end() ; itr++)
	{
		if (itr->second)
		{
			return true;
		}
	}
	return false;
}

bool ActControl::checkActCanGetByType(int type)
{
	std::map<int,bool>::iterator itr = mActTypeEff.find(type);
	if (itr != mActTypeEff.end())
	{
		if (itr->second)
		{
			return true;
		}
	}

	
	return false;
}

void ActControl::setActCanGetByType(int type,bool _get)
{
	std::map<int,bool>::iterator itr = mActTypeEff.find(type);
	if (itr != mActTypeEff.end())
	{
		itr->second = _get;
	}

	sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d 0",MSG_FUC_ACT));
}


bool ActControl::isSameDay(int day)
{
	if (sgLoginProxy()->getServerTime() == 0)
	{
		return false;
	}
	CVTime pnowTime(sgLoginProxy()->getServerTime());
	return pnowTime.GetDay()==day;
}

void ActControl::clearActTypeEff()
{
	for (auto it = mActTypeEff.begin(); it!=mActTypeEff.end();)
	{
		if (it->first == ACT_LOGIN)
		{
			it++;
		}else
		{
			it = mActTypeEff.erase(it);
		}
	}
}

void ActControl::setIsNewYearIconOpen(bool isOpen)
{
	sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", MSG_FUC_REDPACKET, isOpen ? 1 : 0));
	//auto m = UserDefault::getInstance()->getXMLFilePath();
	bool bFirstOpenNewYear = USER_DEFAULT_BOOL("isFirstOpenNewYear",false);
	if (isOpen && !bFirstOpenNewYear)
	{
		SAVE_USER_DEFAULT_BOOL("isFirstOpenNewYear", true);
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 1", MSG_FUC_REDPACKET));//开启红点
	}
}

void ActControl::setIsValentineIconOpen(bool isOpen)
{
	sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", MSG_FUC_VALENTINE, isOpen ? 1 : 0));
}

void ActControl::updateFirstChargeView()
{
	ActFirstCharge* dialog = dynamic_cast<ActFirstCharge*>(sgPageFunction()->getWindowByPageName("ActFirstCharge.csb"));
	if (dialog && dialog->isShow()){
		//matchLoading->setShow(false);
		dialog->updateBtn();
	}
}
