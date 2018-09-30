#include "HonorShopControl.h"
#include "game/net/NetManager.h"
#include "game/ui/dialogs/GiftAwardDialog.h"
#include "game/data/TableManager.h"
#include "core/TimeCalculator.h"

HonorShopItemData::HonorShopItemData()
	:mItemId(1),
	mAllLeftCount(0),
	mExchangeLeftCount(0),
	mRefreshTime(1)
{

}

HonorShopItemData::~HonorShopItemData()
{

}

bool HonorShopItemData::init()
{
	bool ret = false;

	do {
		ret = true;
	} while (0);

	return ret;
}

HonorShopProxy::HonorShopProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_EXCHANGE_ITEM, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_UPDATE_EXCHANGE_LIST, this);
}

HonorShopProxy::~HonorShopProxy()
{

}

void HonorShopProxy::DoRsp(NetPack &stRsp)
{
	//unsigned long msgId = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg()){
		CCLOG("[trace]  HonorShop  S2CMsg error!");
	}
	else if (stRsp.getS2CMsg()->has_exchangeitem_resp()){
		CCLOG("[trace]  ExchangeItem ................ resp");
		sgHonorShopControl()->doUpdateExchangeItem(stRsp.getS2CMsg()->exchangeitem_resp());
	}
	else if (stRsp.getS2CMsg()->has_updateexchangelist_resp()) {
		CCLOG("[trace]  updateExchangeList ============== resp");
		sgHonorShopControl()->doSyncExchangeInfo(stRsp.getS2CMsg()->updateexchangelist_resp());
	}

}

bool HonorShopProxy::fault(const fgame::CSErrorMsg &err)
{
	bool ret = false;


	return ret;
}

void HonorShopProxy::sendExchangeReq(int itemId)
{
	unsigned int uCmd = fgame::CS_CMD_EXCHANGE_ITEM;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSExchangeItemReq *pReq = msgPack.getC2SMsg()->mutable_exchangeitem_req();

	pReq->set_itemid(itemId);

	sgNetManager()->SendPkg(msgPack,false, false, true);
}

void HonorShopProxy::sendGetExchangeInfoReq()
{
	unsigned int uCmd = fgame::CS_CMD_UPDATE_EXCHANGE_LIST;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSUpdateExchangeListReq *pReq = msgPack.getC2SMsg()->mutable_updateexchangelist_req();
	
	sgNetManager()->SendPkg(msgPack, false,false, true);
}

HonorShopControl::HonorShopControl()
{

}

HonorShopControl::~HonorShopControl()
{

}

void HonorShopControl::sendExchangeItemReq(int itemId)
{
	sgHonorShopProxy()->sendExchangeReq(itemId);
}

void HonorShopControl::sendGetExchangeInfoReq()
{
	sgHonorShopProxy()->sendGetExchangeInfoReq();
}

void HonorShopControl::try2SyncExchangeInfo()
{
	//if (mShopDataList.size() == 0) {
		sendGetExchangeInfoReq();
	//}
}

void HonorShopControl::doSyncExchangeInfo(const fgame::CSUpdateExchangeListResp &listResp)
{
	clearShopData();
	int itemCount = listResp.exchangeitem_size();
	CCLOG("[trace]    itemCOun ==== %d", itemCount);
	/*  test
	for (int i = 1; i < 8; i++) {
		HonorShopItemData* data = HonorShopItemData::create();
		//const fgame::ExchangeItemInfo &info = listResp.exchangeitem(i);
		data->setItemId(i);
		data->setExchangeLeftCount(i);
		data->setRefreshTime(i * 60);
		////data->setAllLeftCount(info.limitcount());
		std::string time_key = StringUtils::format("HONOR_ITEM_KAY_%d", data->getItemId());
		sgTimeCalculator()->createTimeCalcultor(time_key, data->getRefreshTime());
		CCLOG("  itemid === %d,  count == %d,  time  == %d, all === %d", data->getItemId(), data->getExchangeLeftCount(), data->getRefreshTime(), data->getAllLeftCount());
		addItem(data);
	}
	*/

	for (int i = 0; i < itemCount; i++) {
		HonorShopItemData* data = HonorShopItemData::create();
		const fgame::ExchangeItemInfo &info = listResp.exchangeitem(i);
		data->setItemId(info.itemid());
		data->setExchangeLeftCount(info.buyleftcount());
		data->setRefreshTime(info.refreshtime());
		////data->setAllLeftCount(info.limitcount());
		if (data->getRefreshTime() > 0) {
			std::string time_key = StringUtils::format("HONOR_ITEM_KAY_%d", data->getItemId());
			sgTimeCalculator()->createTimeCalcultor(time_key, data->getRefreshTime());
			CCLOG("  itemid === %d,  count == %d,  time  == %d, all === %d", data->getItemId(), data->getExchangeLeftCount(), data->getRefreshTime(), data->getAllLeftCount());
			addItem(data);
		}
	}

	if (listResp.has_honorcount()) {
		sgUserObj()->getPersonalInfo()->set_honorcount(listResp.honorcount());//sgUserObj()->setHonorCount(listResp.honorcount());
		CCLOG("   count === %d",  listResp.honorcount());
	}

	sgMessageManager()->sendMessage(EMT_UPDATE_EXCHANGE, StringUtils::format("%d", 0));
}

void HonorShopControl::doUpdateExchangeItem(const fgame::CSExchangeItemResp &itemResp)
{
	if (itemResp.has_ret()) {
		if (itemResp.ret() != 0) {
			CCLOG("[trace]  doUpdateExchangeItem code === %d", itemResp.ret());
			sgMessageManager()->sendMessage(EMT_UPDATE_EXCHANGE, StringUtils::format("%d", itemResp.ret()));
			return;
		}
	}

	if (itemResp.has_honorcount()) {
		sgUserObj()->getPersonalInfo()->set_honorcount(itemResp.honorcount());//sgUserObj()->setHonorCount(itemResp.honorcount());
	}

	if (itemResp.has_exchangeitem()) {
		/////// 쉽쟨쥴혤
		auto cacheData = getShopDataById(itemResp.exchangeitem().itemid());
		if (cacheData) {
			cacheData->setExchangeLeftCount(itemResp.exchangeitem().buyleftcount());
			cacheData->setRefreshTime(itemResp.exchangeitem().refreshtime());
		}

		auto resData = sgTableManager()->getExchangeItem(itemResp.exchangeitem().itemid());
		if (resData){
			GiftAwardDialog::showDialogByGiftId(resData->mGiftId);
		}
	}

	sgMessageManager()->sendMessage(EMT_UPDATE_EXCHANGE, StringUtils::format("%d", 0));
}

void HonorShopControl::addItem(HonorShopItemData *data)
{
	mShopDataList.pushBack(data);
	//CCLOG("  shopDataList ===== %d",mShopDataList.size());
}

void HonorShopControl::removeItemById(int itemId)
{
	auto iter = mShopDataList.begin();
	for (; iter != mShopDataList.end(); ++iter) {
		if ((*iter)->getItemId() == itemId){
			mShopDataList.erase(iter);
			// refresh dialog
			sgMessageManager()->sendMessage(EMT_UPDATE_EXCHANGE, StringUtils::format("%d", 0));
			break;
		}
	}
}

HonorShopItemData* HonorShopControl::getShopDataById(int itemId)
{
	cocos2d::Vector<HonorShopItemData*>::iterator iter = mShopDataList.begin();
	for (; iter != mShopDataList.end(); iter++) {
		CCLOG("  item iD == %d",  (*iter)->getItemId());
		if ((*iter)->getItemId() == itemId) {
			return (*iter);
		}
	}

	return nullptr;
}

int	HonorShopControl::getItemIdByIndex(int index)
{
	int item_id = 0;
	if (mShopDataList.size() >= index) {
		HonorShopItemData* item = mShopDataList.at(index);
		item_id = item->getItemId();
	}

	return item_id;
}

bool HonorShopControl::checkHonorCountEnough(int itemId)
{
	int ret = false;
	do {
		const ExchangeItem *itemInfo = sgTableManager()->getExchangeItem(itemId);
		if (!itemInfo) break;
		//if (sgUserObj()->getHonorCount() >= itemInfo->mNeedScore) {
		if (sgUserObj()->getPersonalInfo()->honorcount() >= itemInfo->mNeedScore) {
			ret = true;
		}
	} while (0);

	return ret;
}

