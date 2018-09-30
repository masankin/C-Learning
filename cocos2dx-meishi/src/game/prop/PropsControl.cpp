#include "PropsControl.h"
#include "game/battle/BattleControl.h"
#include "game/battle/Build.h"
#include "game/battle/BuildView.h"
#include "game/battle/BattleRecordsManager.h"

#include "TimeCalculator.h"
#include "SoundManager.h"
#include "EnvSet.h"
#include "net/NetManager.h"
#include "prop/PropItem.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"
#include "game/ui//PageFunction.h"
#include "net/UserObj.h" 
#include "game/guide/GuideManager.h"
#include "sync/SyncControl.h"
#include "ui/dialogs/BackPackDialog.h"
#include "shop/ShopPanel.h"
#include "ui/dialogs/GiftAwardDialog.h"


const float PropsControl::PropTipsTime = 15;

const std::string PropsControl::PropTipsTimeKey = "updatePropTips";

PropsControl::PropsControl() :mPropIng(false), mCurProp(PROP_TYPE_NULL)
, mInformTextBottom(nullptr), mInformTextHead(nullptr), mCancel(0), isExchange(false), mNeedTipsProp(false), mUsePropNum(0)
{
	mCdList.insert(std::make_pair(ePropType::PROP_TYPE_EXCHANGE_B, false));
	mCdList.insert(std::make_pair(ePropType::PROP_TYPE_EXCHANGE_I, false));
	mCdList.insert(std::make_pair(ePropType::PROP_TYPE_EXCHANGE_T, false));
	mCdList.insert(std::make_pair(ePropType::PROP_TYPE_TIME, false));
	mCdList.insert(std::make_pair(ePropType::PROP_TYPE_ADDPEOPEL, false));
	mCdList.insert(std::make_pair(ePropType::PROP_TYPE_COOKING, false));
	mCdList.insert(std::make_pair(ePropType::PROP_TYPE_FREEZE, false));
	mCdList.insert(std::make_pair(ePropType::PROP_TYPE_INVISIBILITY_CLOAK, false));

	for (int i = 1; i <= 3; i++)
	{
		PropItem* pPropItem = PropItem::create();
		if (pPropItem)
		{
			pushPropItem((ePropPos)i, pPropItem);
			CC_SAFE_RETAIN(pPropItem);
		}
	}

	sgMessageManager()->regisiterMessageHandler(EMT_PROP_USER, this);
	sgMessageManager()->regisiterMessageHandler(EMT_GUIDE, this);
}

PropsControl::~PropsControl()
{
	sgMessageManager()->removeMessageHandler(this);
	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end();)
	{
		CC_SAFE_RELEASE(it->second);
	}
	mPropItemList.clear();
}

void PropsControl::doExchange(int buildId, eCampType useCamp)
{

	bool isNew = useCamp == sgBattleControl()->getOwnCamp() && isExchange;//是否是综合转换卷
	if (isNew)
	{
		isExchange = false;
	}


	char params[64] = { 0, };
	sprintf(params, "%d %d %d %d", buildId, useCamp, isNew ? 1 : 0, m_Id);
	sgMessageManager()->sendMessage(EMT_PROP_CHANGE, params);
}

void PropsControl::doFullPeopol(int buildId, eCampType eCamp)
{
	char params[64] = { 0, };
	sprintf(params, "%d %d", buildId, eCamp);
	sgMessageManager()->sendMessage(EMT_PROP_FULLPEOP, params);
}

void PropsControl::doCookingSolider(const Vec2& pos, eCampType useCamp)
{
	char params[64] = { 0, };
	sprintf(params, "%f %f %d", pos.x, pos.y, useCamp);
	mTouchPos = pos;
	sgMessageManager()->sendMessage(EMT_PROP_COOKING, params);

	if (useCamp == sgBattleControl()->getOwnCamp())
	{
		sgPropsControl()->setCdTime(m_Id);
	}
}

void PropsControl::doOil(const Vec2& pos, eCampType useCamp)
{
	char params[64] = { 0, };
	sprintf(params, "%f %f %d", pos.x, pos.y, useCamp);
	//mTouchPos = pos;
	sgMessageManager()->sendMessage(EMT_PROP_Oil, params);

	if (useCamp == sgBattleControl()->getOwnCamp())
	{
		sgPropsControl()->setCdTime(m_Id);
	}
}

void PropsControl::doAddTime(eCampType useCamp)
{
	const PropInfo* pCheckPt = sgTableManager()->getPropInfoById(m_Id);
	if (pCheckPt&& pCheckPt->type == PROP_TYPE_TIME)
	{
		char params[64] = { 0, };
		sprintf(params, "%f", pCheckPt->duration);
		if (useCamp == sgBattleControl()->getOwnCamp())
		{
			setCdTime(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_TIME));
		}
		sgMessageManager()->sendMessage(EMT_PROP_ADDTIME, params);
	}

}

void PropsControl::doFreeze(eCampType useCamp)
{
	sgMessageManager()->sendMessage(EMT_PROP_FREEZE, StringUtils::format("%d", useCamp));
	if (useCamp == sgBattleControl()->getOwnCamp())
	{
		setCdTime(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_FREEZE));
	}
}


void PropsControl::doInvisibility(int buildId, eCampType useCamp)
{
	char params[64] = { 0, };
	sprintf(params, "%d %d", buildId, useCamp);
	sgMessageManager()->sendMessage(EMT_PROP_INVISIBILITY, params);
}


void PropsControl::update()
{
	chechCdTimeOver();
	updatePropTips();
}

void PropsControl::setCdTime(int id)
{

	const PropInfo* pProp = sgTableManager()->getPropInfoById(id);
	if (pProp)
	{

		sgTimeCalculator()->createTimeCalcultor(StringUtils::format("cdTime%d", pProp->type), pProp->cdtime);
		if (sgBattleControl()->getIsPvp() == false)
		{
			sendUsePropRqst(pProp->id, 1);
			mCdList[pProp->type] = true;
		}

		updatePropItem(pProp->id);
		sgMessageManager()->sendMessage(EMT_PROP_USER, StringUtils::format("%d", pProp->id));

		std::string param = StringUtils::format("%d %d %d %d %d %d %s"
			, eItemOperation::Use
			, m_Id
			, 0
			, 1
			, 0
			, eVituralCurrencyType::Diamond
			, "");

		sgMessageManager()->sendMessage(EMT_ITEM_OPERATION, param);
	}

}

void PropsControl::chechCdTimeOver()
{

	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		const PropInfo* pProp = sgTableManager()->getPropInfoById(it->second->getPropId());
		if (!pProp) continue;;

		float time = sgTimeCalculator()->getTimeLeft(StringUtils::format("cdTime%d", pProp->type));
		if (time == 0)
		{
			mCdList[pProp->type] = false;
			it->second->setCdPercentage(0);
			sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("cdTime%d", pProp->type));

		}
		else if (time > 0)
		{
			if (pProp)
			{

				float percent = time / pProp->cdtime * 100;
				it->second->setCdPercentage(percent);
			}
		}
	}


}


bool PropsControl::checkIsCdTime(int propId)
{
	const PropInfo* pProp = sgTableManager()->getPropInfoById(propId);
	if (pProp)
	{
		float time = sgTimeCalculator()->getTimeLeft(StringUtils::format("cdTime%d", pProp->type));
		return time > 0;
	}

	return false;
}


bool PropsControl::doProp(const Vec2& pos, eCampType useCamp, bool isNet /* = false */)
{
	if (useCamp == sgBattleControl()->getOwnCamp())
	{
		showOrHideAimEffect(false);
	}

	mPropIng = false;
	sgGuideManager()->unRegistGuideMessage();


	BaseBuild* pBuild = sgBattleControl()->getTargetBuild(pos);
	if (!checkPropValid(pBuild, useCamp))
		return false;

	if (!isNet && sgBattleControl()->getIsPvp())
	{
		if (ePropType::PROP_TYPE_EXCHANGE_B <= mCurProp && mCurProp <= PROP_TYPE_EXCHANGE_T && isExchange)
		{
			sgPvpControl()->getPvPProxy()->sendUseItem(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_EXCHANGE), pos, m_Id);
		}
		else
		{
			sgPvpControl()->getPvPProxy()->sendUseItem(m_Id, pos, 0);
		}
		return false;
	}

	if (useCamp == sgBattleControl()->getOwnCamp() && !sgBattleRecordsManager()->getIsPlayback())
	{
		mUsePropNum++;
	}

	switch (mCurProp)
	{
	case PROP_TYPE_EXCHANGE_B:
		doExchange(pBuild->getKeyID(), useCamp);
		break;
	case PROP_TYPE_EXCHANGE_I:
		doExchange(pBuild->getKeyID(), useCamp);
		break;
	case PROP_TYPE_EXCHANGE_T:
		doExchange(pBuild->getKeyID(), useCamp);
		break;
	case PROP_TYPE_ADDPEOPEL:
		doFullPeopol(pBuild->getKeyID(), useCamp);
		break;
	case PROP_TYPE_COOKING:
		doCookingSolider(pos, useCamp);
		break;
	case PROP_TYPE_FREEZE:
		doFreeze(useCamp);
		break;
	case PROP_TYPE_TIME:
		doAddTime(useCamp);
		break;
	case PROP_TYPE_INVISIBILITY_CLOAK:
		doInvisibility(pBuild->getKeyID(), useCamp);
		break;
	case PROP_TYPE_CHAIN_OIL:
		doOil(pos, useCamp);
		break;
	default:
		break;
	}

	return true;
}

bool PropsControl::setProp(int propId, bool isNet /* = false */)
{
	if (mPropIng) return false;

	m_Id = propId;
	mCurProp = getCurPropType();
	mPropIng = true;

	if (sgGamePrecedure()->getEnterGame() && !isNet)
	{
		prePropEvet();
	}
	return true;
}


PropProxy* PropsControl::getPropProxy()
{
	return &m_PropProxy;
}

void PropsControl::sendBuyPropRqst(int itemid, int price, int count)
{
	getPropProxy()->sendBuyPropRqst(itemid, price, count);
}

void PropsControl::sendUsePropRqst(int itemid, int count)
{
	if (sgNetManager()->IsConnect())
	{
		getPropProxy()->sendUsePropRqst(itemid, count);
	}
	else
	{
	}

}

void PropsControl::sendOpenChestRqst(int itemid, int count)
{
	if (count > 0)
		getPropProxy()->sendOpenChestRqst(itemid, count);
}

void PropsControl::doBuyPropRsp(const fgame::CSBuyItemResp& mBuyRsp)
{
	cocos2d::log("[CSBuyItemResp:] Ret:%d, ItemId:%d, Count:%d", mBuyRsp.ret(), mBuyRsp.itemid(), mBuyRsp.count());
	if (mBuyRsp.ret() == 0)
	{
		sgMessageManager()->removeMessageHandler(EMT_SHOP_AFTER_BUYDIAMOND);

		sgSoundManager()->playMusicEffect(SOUND_ITEM_GAIN);
		if (mBuyRsp.itemid() == PVP_ENERGY_TYPE)
		{
			PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());//sgUserObj()->getEnergyBuyCount());
			if (pPowerBuy)
			{
				//获取精力动画
				//Size winSize = Director::getInstance()->getWinSize();
				//sgPageFunction()->getActionPowerLayer()->addItem("ui/general/general_vigour.png", Vec2(winSize / 2), 50, sgUserObj()->getPvpPower()); 

				std::string param = StringUtils::format("%d %d %d %d %d %s"
					, 1
					, 1
					, eVituralCurrencyType::Diamond
					, -pPowerBuy->getEnergyNum()
					, sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
					, "PropsControl::doBuyPropRsp EnergyType");
				sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);


				//sgUserObj()->setEnergyBuyCount(sgUserObj()->getEnergyBuyCount() >= 50 ? 50 : (sgUserObj()->getEnergyBuyCount() + 1));
				int energyBuyCount = sgUserObj()->getPersonalInfo()->energybuycount();
				sgUserObj()->getPersonalInfo()->set_energybuycount(energyBuyCount >= 50 ? 50 : energyBuyCount + 1);

				//pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getEnergyBuyCount());
				//if (pPowerBuy)
				//{
				//	GenerlDialog::setContentText(__String::createWithFormat(sgTextHelper(TEXT_BUY_ITEM_JINGLI_SURE).c_str(),pPowerBuy->getEnergyNum())->getCString());
				//}			
			}
		}
		else if (mBuyRsp.itemid() == PVE_POWER_TYPE)
		{
			//获取体力动画
			//Size winSize = Director::getInstance()->getWinSize();
			//sgPageFunction()->getActionPowerLayer()->addItem("ui/general/general_Energy.png", Vec2(winSize / 2), 50, sgUserObj()->getPersonalInfo()->power());//sgUserObj()->getPvePower());
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eBthChangePower");

			std::string param = StringUtils::format("%d %d %d %d %d %s"
				, 1
				, 1
				, eVituralCurrencyType::Diamond
				, -mBuyRsp.price()
				, sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
				, "PropsControl::doBuyPropRsp PowerType");
			sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);


			//选关界面体力不足购买体力后直接进入游戏
			/*LevelDialog* pLevelDialog = dynamic_cast<LevelDialog*>(sgPageFunction()->getWindowByPageName("LevelDialog.csb"));
			if (pLevelDialog && pLevelDialog->isShow())
			{
			sgMessageManager()->sendMessage(EMT_CLOSESHOP, "");

			sgUserObj()->sendEnterPoint(sgGamePrecedure()->getCurMapInfo().checkpt);
			}*/
		}
		else
		{
			//获取道具
			const PropInfo* pPorp = sgTableManager()->getPropInfoById(mBuyRsp.itemid());
			if (pPorp)
			{
				auto pItemIcon = Sprite::createWithSpriteFrameName(pPorp->icon.c_str());
				pItemIcon->setScale(.5f);
				Size winSize = Director::getInstance()->getWinSize();
				sgPageFunction()->getActionPackageLayer()->addItem(pItemIcon, Vec2(winSize.width / 2 - 197, winSize.height / 2 + 19), 0, 0);

			}

			std::string param = StringUtils::format("%d %d %d %d %d %d %s"
				, eItemOperation::Buy
				, mBuyRsp.itemid()
				, 0
				, mBuyRsp.count()
				, eVituralCurrencyType::Diamond
				, mBuyRsp.price()
				, "ShopDetailed::onMenuItemAction");
			sgMessageManager()->sendMessage(EMT_ITEM_OPERATION, param);

			sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);

			sgMessageManager()->sendMessage(EMT_PROP_USER, __String::createWithFormat("%d", mBuyRsp.itemid())->getCString());
			//sgUserObj()->getSyncPropData()->addDPropata(mBuyRsp.itemid(),1);
		}


		ShopDetailed*  pWindow = dynamic_cast<ShopDetailed*>(Window::Create("ShopDetailed.csb"));
		if (pWindow && pWindow->isShow())
		{
			if (sgGamePrecedure()->getEnterGame())
			{
				pWindow->setShow(false);
			}
			pWindow->doFinishGuide();
		}
		pWindow->showTextTip(sgTextHelper(TEXT_BUY_ITEM_SUCCESS));

		BackPackDialog* pDialog = dynamic_cast<BackPackDialog*>(sgPageFunction()->getWindowByPageName("backpackDialog.csb"));
		if (pDialog&&pDialog->isShow())
		{
			sgMessageManager()->sendMessage(EMT_CLOSESHOP, "");

		}



	}
	else if (mBuyRsp.ret() == 6002)
	{
		//肮脏的代码
		sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, "");
		GenerlDialog::show(sgTextHelper(TEXT_BUY_ITEM_TITLE), sgTextHelper(TEXT_COMMTIPS_SHOP_TEXT1), true);
	}
}

void PropsControl::doUsePropRsp(const fgame::CSUseItemResp& mUseRsp)
{
	cocos2d::log("[CSUseItemResp:] Ret:%d", mUseRsp.ret());
}

void PropsControl::clean()
{
	mPropIng = false;
	mNeedTipsProp = false;
	mCurProp = PROP_TYPE_NULL;
	m_Id = 0;

	CdList::iterator it = mCdList.begin();
	for (; it != mCdList.end(); it++)
	{
		it->second = false;
	}

	PropItemList::iterator pIt = mPropItemList.begin();

	for (; pIt != mPropItemList.end(); pIt++)
	{
		pIt->second->setCdPercentage(0);
		pIt->second->cleanData();
	}

	if (mInformTextBottom)
	{
		mInformTextBottom->setVisible(false);
	}
	if (mInformTextHead)
	{
		mInformTextHead->setVisible(false);
	}
	if (mCancel)
	{
		mCancel->setVisible(false);
	}
}

void PropsControl::cleanCdTime()
{
	PropItemList::iterator pIt = mPropItemList.begin();

	for (; pIt != mPropItemList.end(); pIt++)
	{
		pIt->second->setCdPercentage(0);
	}

	CdList::iterator it = mCdList.begin();
	for (; it != mCdList.end(); it++)
	{
		it->second = false;
	}

	showOrHideAimEffect(false);
}

void PropsControl::cleanCdTime(int propId)
{
	const PropInfo* pProp = sgTableManager()->getPropInfoById(propId);
	PropItem* pItem = getPropItem(pProp->id);
	if (pProp != nullptr && pItem != nullptr)
	{
		sgTimeCalculator()->removeTimeCalcultor(StringUtils::format("cdTime%d", pProp->type));
		mCdList[pProp->type] = false;
		updatePropItem(pProp->id);
		pItem->setCdPercentage(10);
	}

}

int PropsControl::getCurPropId()
{
	return m_Id;
}

ePropType PropsControl::getCurPropType()
{
	const PropInfo * pProp = sgTableManager()->getPropInfoById(m_Id);

	if (pProp)
	{
		return pProp->type;
	}

	return ePropType::PROP_TYPE_NULL;
}

void PropsControl::pushPropItem(ePropPos pos, PropItem* pItem)
{
	PropItemList::iterator it = mPropItemList.find(pos);
	if (it != mPropItemList.end())
	{
		it->second = pItem;
	}
	else
	{
		mPropItemList.insert(std::make_pair(pos, pItem));
	}
}

int PropsControl::getPosPropId(ePropPos pos)
{
	PropItemList::iterator it = mPropItemList.find(pos);
	if (it != mPropItemList.end())
	{
		return it->second->getPropId();
	}
	return 0;
}

void PropsControl::updatePropItem(int propId)
{
	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		if (it->second->getPropId() == propId)
		{
			it->second->updatePropNum();
			it->second->setCdPercentage(100);
		}

	}
}

void PropsControl::removePropItem(int propId)
{
	PropItem*  pItem = sgPropsControl()->getPropItem(propId);
	if (pItem)
	{
		pItem->cleanData();
	}

	std::vector<int> list;
	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		int id = it->second->getPropId();
		if (id != 0)
		{
			list.push_back(id);
		}
	}

	PropItemList::iterator pIt = mPropItemList.begin();
	for (; pIt != mPropItemList.end(); pIt++)
		pIt->second->cleanData();

	for (int i = 1; i <= list.size(); i++)
	{
		PropItem* pItem = getPropItem((ePropPos)i);
		if (pItem)
		{
			int id = list.at((i - 1));
			sgUserObj()->setPropItem(i, id);
			pItem->updateUI(id);
		}
	}

}

void PropsControl::addPropItem(int propId)
{
	PropItem* pItem = getPropItem(propId);
	if (pItem)
		pItem->cleanData();

	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		if (it->second->getPropId() == 0)
		{
			it->second->updateUI(propId);
			if (!sgBattleControl()->getIsPvp())
			{
				// 				if(sgGuideManager()->checkGuideFinish(17) == false)
				// 					return;
				sgUserObj()->setPropItem(it->first, propId);
			}
			break;
		}
	}



}

PropItem* PropsControl::getPropItem(ePropPos pos)
{
	PropItemList::iterator it = mPropItemList.find(pos);
	if (it != mPropItemList.end())
	{
		return it->second;
	}
	return nullptr;
}

PropItem* PropsControl::getPropItem(int id)
{
	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		if (it->second->getPropId() == id)
		{
			return it->second;
		}
	}
	return nullptr;
}

void PropsControl::updatePropItemShow()
{
	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		bool ret = checkPropInLevel(it->first) && it->second->getPropId() != 0;
		it->second->setVisible(ret);
	}

}

void PropsControl::updatePropTips()
{
	if (mNeedTipsProp && sgTimeCalculator()->getTimeLeft(PropsControl::PropTipsTimeKey) == 0)
	{
		int pos = cocos2d::random(1, 3);
		auto pItem = getPropItem((ePropPos)pos);
		if (pItem && pItem->getPropId() != 0)
		{
			int propNum = sgUserObj()->getProp(pItem->getPropId());
			if (propNum == 0)
			{
				pItem->playPropBuyTips();
			}
			else
			{
				pItem->playPropUseTips();
			}
		}

		sgTimeCalculator()->resetTimeCalcultor(PropsControl::PropTipsTimeKey);
	}
}

void PropsControl::prePropEvet()
{
	mAimList.clear();

	if (ePropType::PROP_TYPE_TIME == mCurProp || ePropType::PROP_TYPE_FREEZE == mCurProp)
	{
		doProp(Vec2(0, 0), sgBattleControl()->getOwnCamp());
		return;
	}
	else if (ePropType::PROP_TYPE_ADDPEOPEL == mCurProp)
	{
		const MapBulidList& buildList = sgBattleControl()->getBuildList();
		for (MapBulidList::const_iterator it = buildList.begin(); it != buildList.end(); it++)
		{
			if (it->second->getCamp() == sgBattleControl()->getOwnCamp()
				&& (it->second->getInviInfo() == nullptr || it->second->getInviInfo()->mUseProp))
			{
				mAimList.push_back(it->first);
			}
		}
	}
	else if (ePropType::PROP_TYPE_EXCHANGE_B == mCurProp)
	{
		const MapBulidList& buildList = sgBattleControl()->getBuildList();
		for (MapBulidList::const_iterator it = buildList.begin(); it != buildList.end(); it++)
		{
			if (it->second->getCamp() == sgBattleControl()->getOwnCamp()
				&& it->second->getBuildType() != EBULID_TYPE_BARRACK
				&& (it->second->getInviInfo() == nullptr || it->second->getInviInfo()->mUseProp))
			{
				mAimList.push_back(it->first);
			}
		}
	}
	else if (ePropType::PROP_TYPE_EXCHANGE_I == mCurProp)
	{
		const MapBulidList& buildList = sgBattleControl()->getBuildList();
		for (MapBulidList::const_iterator it = buildList.begin(); it != buildList.end(); it++)
		{
			if (it->second->getCamp() == sgBattleControl()->getOwnCamp()
				&& it->second->getBuildType() != EBULID_TYPE_INSISTUTE
				&& (it->second->getInviInfo() == nullptr || it->second->getInviInfo()->mUseProp))
			{
				mAimList.push_back(it->first);
			}
		}
	}
	else if (ePropType::PROP_TYPE_EXCHANGE_T == mCurProp)
	{
		const MapBulidList& buildList = sgBattleControl()->getBuildList();
		for (MapBulidList::const_iterator it = buildList.begin(); it != buildList.end(); it++)
		{
			if (it->second->getCamp() == sgBattleControl()->getOwnCamp()
				&& it->second->getBuildType() != EBULID_TYPE_TURRET
				&& (it->second->getInviInfo() == nullptr || it->second->getInviInfo()->mUseProp))
			{
				mAimList.push_back(it->first);
			}
		}
	}
	else if (ePropType::PROP_TYPE_INVISIBILITY_CLOAK == mCurProp)
	{
		const MapBulidList& buildList = sgBattleControl()->getBuildList();
		for (MapBulidList::const_iterator it = buildList.begin(); it != buildList.end(); it++)
		{
			if (it->second->getCamp() == sgBattleControl()->getOwnCamp()
				&& (it->second->getInviInfo() == nullptr || it->second->getInviInfo()->mUseProp))
			{
				mAimList.push_back(it->first);
			}
		}
	}
	else if (ePropType::PROP_TYPE_EXCHANGE == mCurProp)
	{
		//兵营转换卷
		Window* pWindow = Window::Create("PropExchangeView");
		if (pWindow)
		{
			mPropIng = false;
			pWindow->setShow(true);
		}

		isExchange = true;
		return;
	}

	showOrHideAimEffect(true);

}

void PropsControl::showOrHideAimEffect(bool v)
{

	const PropInfo * pInfo = sgTableManager()->getPropInfoById(m_Id);
	if (!pInfo)
		return;

	for (int i = 0; i < mAimList.size(); i++)
	{
		int buildId = mAimList.at(i);
		BaseBuild* pBuild = sgBattleControl()->getTargetBuild(buildId);
		if (pBuild)
		{
			BuildView* pView = pBuild->getBuildView();
			if (pView)
			{
				if (v)
					pView->addPropAimEffect();
				else
					pView->RemovePropAimEffect();
			}
		}
	}


	mCancel->setVisible(v);

	if (mInformTextBottom)
	{
		if (v)
		{
			mInformTextBottom->stopAllActions();
			auto pScale = ScaleBy::create(1.0, 1.1);
			auto pAction = Sequence::create(pScale, pScale->reverse(), nullptr);
			mInformTextBottom->runAction(RepeatForever::create(pAction));

			std::string text = pInfo->explain2;
			if (pInfo->useRange == 1 && mAimList.size() <= 0)
				text = sgTextHelper(TEXT_COMMTIPS_PROP_AIM_TEXT);


			mInformTextBottom->setText(text);
			mInformTextBottom->setVisible(true);
		}
		else
		{
			mInformTextBottom->setScale(1);
			mInformTextBottom->stopAllActions();
			mInformTextBottom->setVisible(false);
		}
	}

	if (mInformTextHead && pInfo->headText != "0")
	{
		if (v)
		{
			mInformTextHead->stopAllActions();
			auto pScale = ScaleBy::create(1.0, 1.1);
			auto pAction = Sequence::create(pScale, pScale->reverse(), nullptr);
			mInformTextHead->runAction(RepeatForever::create(pAction));
			mInformTextHead->setText(pInfo->headText);
			mInformTextHead->setVisible(true);
		}
		else
		{
			mInformTextHead->setScale(1);
			mInformTextHead->stopAllActions();
			mInformTextHead->setVisible(false);
		}
	}

	if (!v)
	{
		updatePropItemShow();
	}
	else
	{
		PropItemList::iterator it = mPropItemList.begin();
		for (; it != mPropItemList.end(); it++)
		{
			it->second->setVisible(false);
		}
	}

}

void PropsControl::stopPropTipsAni()
{
	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		if (it->second->getPropId() != 0)
		{
			it->second->stopPropTipsAni();
		}
	}
}

void PropsControl::propEvetCancel()
{
	showOrHideAimEffect(false);
	mPropIng = false;
}

void PropsControl::textCallBack()
{
	mInformTextHead->setScale(1);
	mInformTextHead->stopAllActions();
	mInformTextHead->setVisible(false);
}

void PropsControl::showNoticeText(std::string str)
{
	if (mInformTextHead)
	{
		auto pScale = ScaleBy::create(1.0, 1.1);
		auto pAction = Sequence::create(pScale, pScale->reverse(), CallFunc::create(CC_CALLBACK_0(PropsControl::textCallBack, this)), nullptr);
		mInformTextHead->runAction(pAction);
		mInformTextHead->setText(str);
		mInformTextHead->setVisible(true);
	}
}

void PropsControl::onReceiveMassage(GameMessage * message)
{ 
	if (message->getMessageType() == EMT_PROP_USER)
	{
		if (sgGamePrecedure()->getEnterGame())
		{
			int id = dynamic_cast<MessagePropUserData*>(message)->getPropID();
			PropItem* pPropItem = getPropItem(id);
			if (pPropItem)
			{
				//mCdList[pPropItem->getItemType()] = false;
				pPropItem->update(nullptr);
			}
			if (mNeedTipsProp)
			{
				stopPropTipsAni();
				mNeedTipsProp = false; //购买或使用道具后 不在提示
			}
		}
	}
	if (message->getMessageType() == EMT_GUIDE)
	{
		MessageGuideData* pMessage = dynamic_cast<MessageGuideData*>(message);
		if (!pMessage)
			return;
		if (pMessage->getState() == E_GUIDEMESSAGE_OVER) 
		{
			//显示道具栏
			setPropItemShow(true);
		}
		else if (pMessage->getState() == E_GUIDEMESSAGE_START)
		{
			//隐藏道具栏
			setPropItemShow(false);
		}
	}
}

void PropsControl::hideOtherItem(int id)
{
	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		if (it->second->getPropId() != id)
		{
			it->second->setVisible(false);
		}
	}
}

bool PropsControl::checkPropInLevel(ePropPos pos)
{
	int tool = 0;
	const CheckPoint* pCheckPt = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);
	switch (pos)
	{
	case ePropPos::E_POS_LEFT:
		tool = (ePropType)pCheckPt->tool1;
		break;
	case ePropPos::E_POS_MID:
		tool = (ePropType)pCheckPt->tool2;
		break;
	case ePropPos::E_POS_RIGHT:
		tool = (ePropType)pCheckPt->tool3;
		break;
	default:
		break;
	}
	return (tool != 0 ? true : false);
}

bool PropsControl::hasPropInLevel()
{
	for (int i = 1; i <= 3; i++)
	{
		if (sgPropsControl()->checkPropInLevel((ePropPos)i))
		{
			return true;
		}
	}
	return false;
}

int PropsControl::getSelectPropNum()
{
	int num = 0;

	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		if (it->second->getPropId() != 0)
		{
			num++;
		}
	}

	return num;
}

void PropsControl::init()
{
	if (sgBattleControl()->getIsPvp())
	{
		initPvpProp();
		mNeedTipsProp = false;
	}
	else
	{
		initNeedTipProp();
		sgTimeCalculator()->createTimeCalcultor(PropsControl::PropTipsTimeKey, PropsControl::PropTipsTime);
		mNeedTipsProp = sgGamePrecedure()->getCurMapInfo().bTipsProp;
	}
	updatePropItemShow();
	mUsePropNum = 0;
}

void PropsControl::initPropCd()
{

	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		if (it->second->getPropId() != 0)
		{

			const PropInfo* pProp = sgTableManager()->getPropInfoById(it->second->getPropId());
			if (pProp && pProp->beginCd == 1)
			{
				mCdList[pProp->type] = true;
				sgTimeCalculator()->createTimeCalcultor(StringUtils::format("cdTime%d", pProp->type), pProp->cdtime);
			}
		}
	}
}

void PropsControl::initPvpProp()
{
	clean();

	const CheckPoint* pCheckPt = sgTableManager()->getCheckPointByID(sgGamePrecedure()->getCurMapInfo().checkpt);

	if (pCheckPt)
	{
		PropItemList::iterator it = mPropItemList.begin();
		for (; it != mPropItemList.end(); it++)
		{
			int tool = 0;
			switch (it->first)
			{
			case ePropPos::E_POS_LEFT:
				tool = (ePropType)pCheckPt->tool1;
				break;
			case ePropPos::E_POS_MID:
				tool = (ePropType)pCheckPt->tool2;
				break;
			case ePropPos::E_POS_RIGHT:
				tool = (ePropType)pCheckPt->tool3;
				break;
			default:
				break;
			}
			if (tool != 0)
			{
				it->second->updateUI(tool);
			}
		}
	}

}

void PropsControl::initNeedTipProp()
{
	if (!hasPropInLevel() || sgGuideManager()->checkOpenPropControl(sgGamePrecedure()->getCurMapInfo().checkpt) == false)
		return;

	int needId[3];
	needId[0] = sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_ADDPEOPEL);
	needId[1] = sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_COOKING);
	needId[2] = sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_FREEZE);

	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		if (it->second->getPropId() == 0)
		{
			for (int i = 0; i < 3; i++)
			{
				int id = needId[i];
				if (id == 0)
					continue;;

				auto pItem = getPropItem(id);
				if (pItem == nullptr)
				{
					it->second->updateUI(id);
					it->second->setIsTipsProp(true);
					needId[i] = 0;
					break;
				}
			}
		}
	}
}

void PropsControl::initSaveProp()
{
	PropItemList::iterator it = mPropItemList.begin();
	for (; it != mPropItemList.end(); it++)
	{
		int id = sgUserObj()->getPropItem(it->first);
		if (id != 0)
		{
			it->second->updateUI(id);
		}
	}
}

bool PropsControl::checkPropValid(BaseBuild* pBuild, eCampType useCamp)
{
	const PropInfo* pInfo = sgTableManager()->getPropInfoById(m_Id);
	if (!pInfo)
	{
		CCLOG("[trace]PropsControl::checkValidByPos: prop id = %d  is Valid ", m_Id);
		return false;
	}

	if (pInfo->useRange == 1 && pBuild == nullptr)
	{
		CCLOG("[trace]PropsControl::checkValidByPos: cant fount  Target Build");
		return false;
	}

	if (pInfo->useRange == 1 && pBuild->getCamp() != useCamp)
	{
		CCLOG("[trace]PropsControl::checkValidByPos: useCamp = %d  build Camp = %d", useCamp, pBuild->getCamp());
		return false;
	}

	if (pBuild != nullptr && pBuild->getInviInfo() != nullptr && !pBuild->getInviInfo()->mUseProp)
	{
		CCLOG("[trace]PropsControl::checkValidByPos: the build set can't use prop ");
		return false;
	}

	//各个道具的判断
	switch (pInfo->type)
	{
	case ePropType::PROP_TYPE_EXCHANGE_B:
		if (eBulidType::EBULID_TYPE_BARRACK == pBuild->getBuildType())
			return false;
		break;

	case ePropType::PROP_TYPE_EXCHANGE_I:
		if (eBulidType::EBULID_TYPE_INSISTUTE == pBuild->getBuildType())
			return false;
		break;

	case ePropType::PROP_TYPE_EXCHANGE_T:
		if (eBulidType::EBULID_TYPE_TURRET == pBuild->getBuildType())
			return false;
		break;
	default:
		break;
	}

	if (useCamp == sgBattleControl()->getOwnCamp() && checkIsCdTime(m_Id))
	{
		CCLOG("[trace]PropsControl::checkValidByPos: prop id = %d   cd  time", m_Id);
		return false;
	}

	return true;
}

void PropsControl::doOpenChestResp(const fgame::CSOpenChestResp& openChestResp)
{
	std::string sub_str = "";
	std::string total_str = "";
	for (int i = 0; i < openChestResp.updatedata_size(); i++)
	{
		const fgame::PropertyUpdateData& pUpdate = openChestResp.updatedata(i);

		if (pUpdate.uptype() == UserObj::PROPERTYTYPE::PT_PVEPOWER )
		{
			// 打开体力提示
			CCLOG(" [trace] open pve power ...............");
		}
		else if (pUpdate.uptype() == UserObj::PROPERTYTYPE::PT_PVPPOWER){
			// 精力药水提示
			CCLOG(" [trace] open pvp poer .................");
		}
		else if (pUpdate.uptype() == UserObj::PROPERTYTYPE::ASSISTANT)  {
			// 将军
			sgUserObj()->setShowGiftId(pUpdate.value2());
			sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "AssitantArmDialog.csb", 1);
			break;
		}
		else {
			// type + id + count
			sub_str = StringUtils::format("%d+%d+%d#", pUpdate.uptype(), pUpdate.value2(), pUpdate.value());
			total_str = StringUtils::format("%s%s", total_str.c_str(), sub_str.c_str());
		}
	}

	if (!total_str.empty()) {
		GiftAwardDialog::showDialogByChests(total_str);
	}

	CCLOG(" [trace] openChest Resp  items_str === %s", total_str.c_str());
}

void PropsControl::initGuideProp(Node* pRootNode, int index)
{
	GuidePropItem* pPropItem = GuidePropItem::create();
	pRootNode->addChild(pPropItem);
	pPropItem->setVisible(false);
	pPropItem->setName(StringUtils::format("GuidePropItem%d", index));
	pPropItem->setScale(0.8f);
	pPropItem->setIsLoad(false);
	pPropItem->setCount(0);
	mGuidePropItemList.push_back(pPropItem);
	mSelectGuideProp = 0;
}

void PropsControl::addGuideProp(int propID)
{
	for (int i = 0; i < mGuidePropItemList.size(); i++)
	{
		GuidePropItem* pPropItem = mGuidePropItemList.at(i);
		if (pPropItem && !pPropItem->getIsLoad())
		{
			pPropItem->loadPropIcon(propID);
			pPropItem->setIsLoad(true);
			mSelectGuideProp = i;
			return;
		}
	}
}

void PropsControl::clearGuideProp()
{
	mGuidePropItemList.clear();
	mSelectGuideProp = 0;
}

void PropsControl::doUseGuideProp(Point pos)
{
	if (mGuidePropItemList.size() <= mSelectGuideProp)
		return;
	GuidePropItem* pPropItem = mGuidePropItemList.at(mSelectGuideProp);
	if (!pPropItem->getIsLoad() || pPropItem->getCount() <= 0)
		return;
	BaseBuild* pBuild = sgBattleControl()->getTargetBuild(pos);
	int type = pPropItem->getType();
	switch (type)
	{
	case PROP_TYPE_COOKING:
		sgPropsControl()->doCookingSolider(pos, sgBattleControl()->getOwnCamp());
		break;
	case PROP_TYPE_ADDPEOPEL:
		if (pBuild)
			doFullPeopol(pBuild->getKeyID(), sgBattleControl()->getOwnCamp());
		break;
	default:
		break;
	}
	pPropItem->setUsedFlag();
	sgGuideManager()->unRegistGuideMessage();
}

GuidePropItem* PropsControl::getGuideProp(int index)
{
	if (mGuidePropItemList.size() > index)
		return mGuidePropItemList.at(index);
	return NULL;
}


void PropsControl::setPropItemShow(bool show)
{
	PropItemList::iterator iter = mPropItemList.begin();
	for (; iter != mPropItemList.end(); iter++)
	{
		PropItem* pItem = iter->second;
		if (pItem->open)
			pItem->setVisible(show);
	}
}

int PropsControl::getUsePropNum()
{
	return mUsePropNum;
}







