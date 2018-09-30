#include "VipControl.h"
#include "game/net/UserObj.h"
#include "game/ui/dialogs/GenerlDialog.h"
#include "game/ui/PageFunction.h"
#include "MessageManager.h"
#include "channel/ChannelApi.h"
VipControl::VipControl() :mSelectBuyID(1), mSelectVip(1)
{
	
}

VipControl::~VipControl()
{

}



void VipControl::sendBuyVipRequest(int vipID, int buyID)
{


	VipData * vipData = sgTableManager()->getVipData(vipID);
	sVipType vipType = vipData->getVipType(buyID);

	//荣誉或者钻石

	if (vipType.mPayType == 1)
	{
		//荣誉购买 
		//if (sgUserObj()->getHonorCount() < vipType.mPrice)
		if (sgUserObj()->getPersonalInfo()->honorcount() < vipType.mPrice)
		{
			// change by jim, unify tips ui
			//GeneralDialogOther::show(sgTextHelper(TEXT_VIP8));
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_VIP8));
			return;
		}
	}
	else if (vipType.mPayType == 2)
	{
		//钻石购买 
		//if (sgUserObj()->getGoldCoin() < vipType.mPrice)
		if (sgUserObj()->getPersonalInfo()->diamond() < vipType.mPrice)
		{
			// change by jim, unify tips ui
			//GeneralDialogOther::show(sgTextHelper(TEXT_BUY_ITEM_NO_EN), GeneralDialogOther::E_GENERAL_TYPE_CONFIRM, [](Ref* sendr){
			//	GeneralDialogOther::hide();
			//	PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
			//	sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, "1");
			//});
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_BUY_ITEM_NO_EN), [](Ref* sendr){
				GenerlDialog::hide();
				PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
				sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, "1");
			},false);
			return;
		}

	}

	//二级确认框
	string contentText;
	//if (sgUserObj()->getVipLevel() == 0)
	if (sgUserObj()->getPersonalInfo()->viplevel() == 0)
	{
		//0级
		if (vipType.mPayType == 1)
		{
			contentText = StringUtils::format(sgTextHelper(TEXT_VIP22).c_str(), vipType.mPrice, vipData->getName().c_str());
		}
		else if (vipType.mPayType == 2)
		{
			contentText = StringUtils::format(sgTextHelper(TEXT_VIP9).c_str(), vipType.mPrice, vipData->getName().c_str());
		}
		else if (vipType.mPayType == 3)
		{
			contentText = StringUtils::format(sgTextHelper(TEXT_VIP23).c_str(), vipType.mPrice, vipData->getName().c_str());;
		}


	}
	else
	{
		int level = sgUserObj()->getPersonalInfo()->viplevel();
		//if (vipID > sgUserObj()->getVipLevel())
		if (vipID > level)
		{
			contentText = StringUtils::format(sgTextHelper(TEXT_VIP15).c_str(), vipData->getName().c_str());
		}
		//else if (vipID <= sgUserObj()->getVipLevel())
		else if (vipID <= level)
		{
			//int lv = sgUserObj()->getVipLevel();
			VipData  *tempVip = sgTableManager()->getVipData(level);
			contentText = StringUtils::format(sgTextHelper(TEXT_VIP14).c_str(), tempVip->getName().c_str(), vipData->getName().c_str());
		}
	}

	// change by jim , unify tips ui
	//GeneralDialogOther::show(contentText.c_str(), GeneralDialogOther::E_GENERAL_TYPE_CONFIRM, CC_CALLBACK_1(VipControl::buyVip, this, vipType, buyID, vipID));
	GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), contentText.c_str(), CC_CALLBACK_1(VipControl::buyVip, this, vipType, buyID, vipID));
}

void VipControl::doBuyVipResp(const fgame::CSBuyVipResp& resp)
{
	CCLOG("[trace]VipControl::doBuyVipResp vipLevel:%d----remainTime:%s", resp.viplevel(),resp.remaintime().c_str());
	int level = resp.viplevel();
	sgUserObj()->getPersonalInfo()->set_viplevel(level);//sgUserObj()->setVipLevel(level);
	sgUserObj()->initVipTime(resp.remaintime());

	//购买成功
	if (resp.type() == 1)
		TipsManager::showTextTip(sgTextHelper(TEXT_BUY_ITEM_SUCCESS));

	sgMessageManager()->sendMessage(EMT_BUY_VIP, StringUtils::format("%d", level));
}

void VipControl::buyVip(Ref* sendr, sVipType vipType, int buyID, int vipID)
{
	//GeneralDialogOther::hide();
	GenerlDialog::hide();
	if (vipType.mPayType == 3)
	{
		//现金
		VipData * vipData = sgTableManager()->getVipData(vipID);
		CCLOG("[trace]VipControl::buyVip ,shopId:%d,vipName:%s,vipPrice:%d",vipType.mShopID,vipData->getName().c_str(),vipType.mPrice);

		std::string name = sgUserObj()->getPlayerName();
		int	pid = sgUserObj()->getUin();
		int gold = sgUserObj()->getPersonalInfo()->diamond();// sgUserObj()->getGoldCoin();
		std::string productDescribe = StringUtils::format("%d#%d#%s", gold, pid, name.c_str());
		sgChannelApi()->channelPay(vipType.mShopID, vipData->getName(), vipType.mPrice, productDescribe);
	}
	else
		mVipProxy.sendBuyVipRequest(vipID, vipType.mPrice, vipType.mPayType, buyID);
}

void VipControl::sendVipTimeRequest()
{
	mVipProxy.sendVipTimeRequest();
}

