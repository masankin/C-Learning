#include "VipTimeDialog.h"
#include "ExRichText.h"
#include "game/data/TableManager.h"
#include "game/utils/Comm.h"
#include "vip/VipControl.h"
#include "GenerlDialog.h"
#include "game/ui/PageFunction.h"
VipTimeDialog::VipTimeDialog()
{
	sgMessageManager()->regisiterMessageHandler(EMT_BUY_VIP, this);
}

VipTimeDialog::~VipTimeDialog()
{

}

bool VipTimeDialog::init()
{

	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/VipTimeEnd.csb");
		CC_BREAK_IF(!mRoot);
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		Node *tempNode = dynamic_cast<Node*>(mRoot->getChildByName("node_TextContent"));
		//mVipDay = dynamic_cast<Text*>(tempNode->getChildByName("text_day"));
		mVipName = dynamic_cast<Text*>(tempNode->getChildByName("text_name"));

		mNode1 = dynamic_cast<Node*>(mRoot->getChildByName("node1"));
		mNode2 = dynamic_cast<Node*>(mRoot->getChildByName("node2"));
		mNode3 = dynamic_cast<Node*>(mRoot->getChildByName("node3"));
		
		mVip1Price = dynamic_cast<Text*>(mNode1->getChildByName("text_price"));
		mVip2Price1 = dynamic_cast<Text*>(mNode2->getChildByName("node1")->getChildByName("text_price"));
		mVip2Price2 = dynamic_cast<Text*>(mNode2->getChildByName("node2")->getChildByName("text_price"));
		mVip3Price1 = dynamic_cast<Text*>(mNode3->getChildByName("node1")->getChildByName("text_price"));
		mVip3Price2 = dynamic_cast<Text*>(mNode3->getChildByName("node2")->getChildByName("text_price"));

		ret = true;
	} while (0);

	return ret;
}

void VipTimeDialog::onMenuItemAction(std::string name, cocos2d::Ref* sender)
{
	if (name == "onClose")
	{
		setShow(false);
	}
	else if (name == "onBuyVIP")
	{	
		Button *btn = dynamic_cast<Button*>(sender);
		bool bTypeshow = btn->getName() == "btn7";
		int buyID = bTypeshow ? 1 : 2;
		sgVipControl()->sendBuyVipRequest(sgUserObj()->getPersonalInfo()->viplevel(), buyID);//sgUserObj()->getVipLevel(), buyID);
	}
}


void VipTimeDialog::onShow()
{
	CCLOG("[trace]VipTimeDialog::onShow, onShow");
	refreshContent(sgUserObj()->getPersonalInfo()->viplevel());//sgUserObj()->getVipLevel());

}


void VipTimeDialog::refreshContent(int id)
{
	//CVTimeSpan time = sgUserObj()->getVipTime(sgUserObj()->getVipLevel());
	if (id <= 0 || id > 3)
		return;

	mNode1->setVisible(id == 1);
	mNode2->setVisible(id == 2);
	mNode3->setVisible(id == 3);

	VipData *vipData = sgTableManager()->getVipData(id);
	sVipType  vipType1 = vipData->getVipType(1);
	sVipType  vipType2 = vipData->getVipType(2);
	mVipName->setString(vipData->getName());
	switch (id)
	{
	case 1:
	{
			  mVip1Price->setText(StringUtils::format("%d", vipType1.mPrice));
	}
		break;

	case 2:
	{
			  mVip2Price1->setText(StringUtils::format("%d", vipType1.mPrice));
			  mVip2Price2->setText(StringUtils::format("%d", vipType2.mPrice));
	}
		break;
	case 3:
	{
			  mVip3Price1->setText(StringUtils::format("%d", vipType1.mPrice));
			  mVip3Price2->setText(StringUtils::format("%d", vipType2.mPrice));
	}
		break;
	}
}

void VipTimeDialog::onReceiveMassage(GameMessage * message)
{
	CCLOG("[trace]VipTimeDialog::onReceiveMassage EMT_BUY_VIP");

	if (message->getMessageType() == EMT_BUY_VIP)
		this->setShow(false);
}



