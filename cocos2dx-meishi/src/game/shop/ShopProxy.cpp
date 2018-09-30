#include "ShopProxy.h"
#include "net/NetManager.h"
#include "data/TableManager.h"
#include "ui/dialogs/GiftAwardDialog.h"
#include "core/MessageManager.h"
#include "ui/dialogs/GenerlDialog.h"
ShopProxy::ShopProxy(void)
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_SHOP_BUY,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_SHOP_LIST,this);
}


ShopProxy::~ShopProxy(void)
{
}

bool ShopProxy::fault(const fgame::CSErrorMsg &err)
{
	return true;
}

void ShopProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		sendShopList();
		CCLOG("[trace]SyncProxy::DoRsp error!");

	}	
	else if (stRsp.getS2CMsg()->has_shopbuy_resp())
	{
		GenerlDialog::hide();
		int lshopId = stRsp.getS2CMsg()->shopbuy_resp().shopid();
		const ShopList lshopList =  sgTableManager()->getGiftList();
		auto shopit =  lshopList.find(lshopId);
		if (shopit != lshopList.end())
		{
			GiftAwardDialog::showDialogByGiftId(shopit->second->giftId);
		}
		sendShopList();
	}else if (stRsp.getS2CMsg()->has_shoplist_resp())
	{
		const ::fgame::CSShopListResp& lshopList = stRsp.getS2CMsg()->shoplist_resp();
		if (lshopList.diamondlist_size()>0 || lshopList.giftlist_size()>0)
		{
			mShopList.clear();
			for (int i =0; i<lshopList.diamondlist_size(); i++)
			{
				ShopInfo lshopInfo;
				lshopInfo.id = lshopList.diamondlist(i).shopid();
				lshopInfo.type = ShopInfo::ShopType::Shop_Diamond;
		
				lshopInfo.isFirst = lshopList.diamondlist(i).first();
				
				mShopList.insert(std::make_pair(lshopInfo.id,lshopInfo));
			}
			for (int i = 0; i < lshopList.giftlist_size(); i++)
			{
				ShopInfo lshopInfo;
				lshopInfo.id = lshopList.giftlist(i).shopid();
				lshopInfo.type = ShopInfo::ShopType::Shop_Gift;

				lshopInfo.giftId = lshopList.giftlist(i).giftid();
				lshopInfo.startTime = StringUtils::format("%d", lshopList.giftlist(i).starttime());
				lshopInfo.endTime = StringUtils::format("%d", lshopList.giftlist(i).endtime());
				lshopInfo.leftCount = lshopList.giftlist(i).leftcount();
				lshopInfo.name = lshopList.giftlist(i).giftname();
				lshopInfo.discount = std::atoi(lshopList.giftlist(i).discount().c_str());
				lshopInfo.priceGold = lshopList.giftlist(i).price();
				lshopInfo.giftIcon = lshopList.giftlist(i).gifticon();
				lshopInfo.giftType = 1;
				lshopInfo.isShow = true;
				mShopList.insert(std::make_pair(lshopInfo.id, lshopInfo));
			}
			
			sgTableManager()->updateShopInfo(mShopList);
		}
		sgMessageManager()->sendMessage(EMT_UPDATE_SHOPLIST,"");
	}
}

void ShopProxy::sendShopBuy(int id)
{
	unsigned int uCmd = fgame::CS_CMD_SHOP_BUY;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSShopBuyReq* pRqst = msgPack.getC2SMsg()->mutable_shopbuy_req();
	pRqst->set_shopid(id);

	sgNetManager()->SendPkg(msgPack);
}

void ShopProxy::sendShopList()
{
	unsigned int uCmd = fgame::CS_CMD_SHOP_LIST;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSShopListReq* pRqst = msgPack.getC2SMsg()->mutable_shoplist_req();

	sgNetManager()->SendPkg(msgPack);
}
