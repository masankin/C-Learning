#include "VipDialog.h"
#include "ExRichText.h"

#include "game/utils/Comm.h"
#include "GenerlDialog.h"
#include "game/ui/PageFunction.h"

VipDialog::VipDialog() 
{
	sgMessageManager()->regisiterMessageHandler(EMT_BUY_VIP, this);
}

VipDialog::~VipDialog()
{
	sgMessageManager()->removeMessageHandler(this);
}

bool VipDialog::init()
{

	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/VIPDialog.csb");
		CC_BREAK_IF(!mRoot);
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		setAnimation("fla_VIPmianban");

		Button* pClose = dynamic_cast<Button*>(mRoot->getChildByName("btn_close"));
		changeBone(mArmatureBg, "VIPmianban_cxzcz", pClose);

		setPopType(E_POP_TYPE_NONE);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		mSelectBtnVec.clear();
		for (int i = 1; i <= 3; ++i)
		{
			Button *btn = dynamic_cast<Button*>(mRoot->getChildByName(StringUtils::format("btn_%d",i)));
			mSelectBtnVec.pushBack(btn);
			switch (i)
			{
			case 1:
				changeBone(mArmatureBg, "guge_baiyin", btn);
				break;
			case 2:
				changeBone(mArmatureBg, "guge_huangjin", btn);
				break;
			case 3:
				changeBone(mArmatureBg, "gugezuanshi", btn);
				break;
			}
		}

		mContent = dynamic_cast<ui::ScrollView*>(mRoot->getChildByName("text_content"));
		changeBone(mArmatureBg, "guge_xinxi", mContent);

		Text *temp_time = dynamic_cast<Text*>(mRoot->getChildByName("text_time"));
		mTime = dynamic_cast<Text*>(temp_time->getChildByName("time"));
		changeBone(mArmatureBg, "guge_shijian", temp_time);

		mSprite_Icon = dynamic_cast<Sprite*>(mRoot->getChildByName("sprite_icon"));
		changeBone(mArmatureBg, "guge_touxiang", mSprite_Icon);
		mSprite_Icon->setZOrder(mArmatureBg->getZOrder()-1);

		//buy Node
		Node *temp_node = mRoot->getChildByName("node_buy");
		changeBone(mArmatureBg, "guge_goumai", temp_node);

		mSprite_bg = dynamic_cast<Sprite*>(mRoot->getChildByName("Sprite_bg"));
		mSprite_bg->setZOrder(mArmatureBg->getZOrder() - 1);
		auto armature1 = mArmatureBg->getBone("guge_goumaia")->getChildArmature();
		if (armature1 != NULL)
			changeBone(armature1, "VIPmianban_GouMai_dxcv", mSprite_bg);

		mSprite_title = dynamic_cast<Sprite*>(mRoot->getChildByName("Sprite_title"));
		mSprite_title->setZOrder(mArmatureBg->getZOrder() - 1);
		auto armature2 = mArmatureBg->getBone("guge_goumaib")->getChildArmature();
		if (armature2 != NULL)
			changeBone(armature2, "VIPmianban_GouMai_dvxcvd", mSprite_title);

		mBuy1Node = temp_node->getChildByName("node1");
		mBuy2Node = temp_node->getChildByName("node2");

		mText1_price = dynamic_cast<Text*>(mBuy1Node->getChildByName("text_price"));
		mText2_price = dynamic_cast<Text*>(mBuy2Node->getChildByName("text_price"));
		mSprite2_price = dynamic_cast<Sprite*>(mBuy2Node->getChildByName("sprite_price"));
		mSprite2_gouhao1 = dynamic_cast<Sprite*>(mBuy2Node->getChildByName("btn7")->getChildByName("sprite"));
		mSprite2_gouhao2 = dynamic_cast<Sprite*>(mBuy2Node->getChildByName("btn30")->getChildByName("sprite"));

		mPlayTag = false;
		ret = true;
	} while (0);

	return ret;
}

void VipDialog::onMenuItemAction(std::string name, cocos2d::Ref* sender)
{
	if (name == "onClose")
	{
		setShow(false);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
	}
	else if (name == "onBuy")
	{	
		int selectID = sgVipControl()->getSelectVip();
		int buyID = sgVipControl()->getSelectBuyID();
		sgVipControl()->sendBuyVipRequest(selectID, buyID);
	}
	else if (name == "onSelectBuy")
	{
		//选择VIP购买档次，刷新属性和界面
		Node * node = dynamic_cast<Node*>(sender);
		bool bTypeshow = node->getName() == "btn7";
		mSprite2_gouhao1->setVisible(bTypeshow);
		mSprite2_gouhao2->setVisible(!bTypeshow);
		int buyID = bTypeshow ? 1 : 2;
		int selectID = sgVipControl()->getSelectVip();
		VipData * vipData = sgTableManager()->getVipData(selectID);
		sgVipControl()->setSelectBuyID(buyID);
		sVipType vipType = vipData->getVipType(buyID);

		std::string path;
		switch (vipType.mPayType)
		{
		case 1:
			path = "ui/general/general_honor_icon.png";
			break;
		case 2:
			path = "ui/general/general_diamonds.png";
			break;
		case 3:
			path = "ui/general/general_rmb_mark.png";
			break;

		}
		mSprite2_price->setSpriteFrame(path);

		mText2_price->setText(StringUtils::format("%d", vipType.mPrice));
		
	}
	else if (name == "onSelect")
	{
		//更换选择VIP
		mPlayTag = true; 
		Node * node = dynamic_cast<Node*>(sender);
		UpdateUI(node->getTag());
	}
}


void VipDialog::onShow()
{

	CCLOG("[trace]VipDialog::onShow, onShow");
	mPlayTag = false;
	int level = sgUserObj()->getPersonalInfo()->viplevel();//sgUserObj()->getVipLevel();

	if (level<1 || level >3)
		level = 0;
	else
		level -= 1;

	int tag = mSelectBtnVec.at(level)->getTag();
	UpdateUI(tag);

	sgVipControl()->sendVipTimeRequest();

}

void VipDialog::UpdateUI(int tag)
{
	refreshBtn(tag);
	sgVipControl()->setSelectVip(tag - 1000);
	sgVipControl()->setSelectBuyID(1);
	refreshContent(sgVipControl()->getSelectVip());
	refreshVIPTime(sgVipControl()->getSelectVip());
	refreshBuyNode(sgVipControl()->getSelectVip());
}

void VipDialog::refreshBtn(int tag)
{
	//刷新VIP按钮高亮
	for (int i = 0; i < mSelectBtnVec.size(); ++ i)
	{
		Button * btn = mSelectBtnVec.at(i);
		bool b = tag == btn->getTag()? true :false;	
		btn->getChildByName("select")->setVisible(b);
		btn->getChildByName("no")->setVisible(!b);
	}
}

void VipDialog::refreshContent(int id)
{
	//刷新VIP内容
	mContent->removeAllChildren();
	mContent->jumpToTop();
	VipData * vipData = sgTableManager()->getVipData(id);
	if (vipData  == nullptr)
		return;

	string szResult = "";
	std::vector<std::string> describeID = vipData->getDescribeID();
	for (int i = 0; i < describeID.size(); ++i)
	{
		int textId = atoi(describeID[i].c_str());
		std::string szDesc = getRichFormatText(sgTextHelper(textId));
		switch (textId)
		{
		case TEXT_VIP1:
			szDesc = StringUtils::format(szDesc.c_str(), vipData->getPowerSlot());
			break;
		case  TEXT_VIP2:
		case  TEXT_VIP4:
			szDesc = StringUtils::format(szDesc.c_str(), vipData->getName().c_str());
			break;
		case  TEXT_VIP3:
			szDesc = StringUtils::format(szDesc.c_str(), vipData->getAddHonor());
			break;
		case  TEXT_VIP5:
			szDesc = StringUtils::format(szDesc.c_str(), vipData->getShopDiscount());
			break;
		case  TEXT_VIP6:
		{
		 	  if (id > 1)
			 {
				VipData *tempData = sgTableManager()->getVipData(id - 1);
				szDesc = StringUtils::format(szDesc.c_str(), tempData->getName().c_str());
			 }
		}
			break;
		case  TEXT_VIP7:
			szDesc = StringUtils::format(szDesc.c_str(), vipData->getEnergySlot());
			break;
		}

		szResult += szDesc;
		szResult += "<br/>";
	}	
	
	ExRichText *richText = ExRichText::create();
	richText->ignoreContentAdaptWithSize(false);
	richText->setSize(Size(550,350));
	richText->setLimitWidth(550);
	richText->setVerticalSpace(5);
	richText->setText(szResult);
	
	int line = richText->getLineNum();

	richText->setPosition(Point(280, line * 35 + 30));
	mContent->ignoreContentAdaptWithSize(false);
	mContent->setInnerContainerSize(Size(280, line * 35 + 30));
	mContent->addChild(richText);

	mSprite_Icon->setSpriteFrame(vipData->getHIcon());

}

void VipDialog::refreshBuyNode(int id)
{
	//刷新VIP购买节点
	VipData * vipData = sgTableManager()->getVipData(id);
	bool bTypeShow = id == 1 ? true : false;
	mBuy1Node->setVisible(bTypeShow);
	mBuy2Node->setVisible(!bTypeShow);

	int buyID = sgVipControl()->getSelectBuyID();
	sVipType vipType = vipData->getVipType(buyID);

	if (mPlayTag)
	{
		auto armature1 = mArmatureBg->getBone("guge_goumaia")->getChildArmature();
		if (armature1 != NULL)
			armature1->getAnimation()->play("play");

		auto armature2 = mArmatureBg->getBone("guge_goumaib")->getChildArmature();
		if (armature2 != NULL)
			armature2->getAnimation()->play("play");
	}

	string bgPath = StringUtils::format("ui/VIP/vip%d.png", id);
	string titlePath = StringUtils::format("ui/VIP/buy%d.png", id);
	mSprite_bg->setSpriteFrame(bgPath);
	mSprite_title->setSpriteFrame(titlePath);

	if (bTypeShow)
	{
		mText1_price->setText(StringUtils::format("%d",vipType.mPrice));
	}
	else
	{
		string spricePaht = id == 2 ? "ui/general/general_diamonds.png" : "ui/general/general_rmb_mark.png";
		string priceStr = StringUtils::format("%d", vipType.mPrice);
		mSprite2_price->setSpriteFrame(spricePaht);
		mText2_price->setText(priceStr);
		mSprite2_gouhao1->setVisible(true);
		mSprite2_gouhao2->setVisible(false);
	}

}

void VipDialog::onReceiveMassage(GameMessage * message)
{
	CCLOG("[trace]VipDialog::onReceiveMassage EMT_BUY_VIP");

	if (message->getMessageType() == EMT_BUY_VIP)
		refreshVIPTime(sgVipControl()->getSelectVip());

}

void VipDialog::refreshVIPTime(int id)
{
	CVTimeSpan time = sgUserObj()->getVipTime(id);
	if (time.GetTotalSeconds() == 0 )
	{
		//未开启
		string timeName;
		timeName = sgTextHelper(TEXT_VIP24);
		mTime->setText(timeName.c_str());
	}
	else
	{
		string timeName;
		//if (id < sgUserObj()->getVipLevel())
		if (id < sgUserObj()->getPersonalInfo()->viplevel())
			timeName = sgTextHelper(TEXT_VIP_TIME2);
		else
			timeName = sgTextHelper(TEXT_VIP_TIME1);

		mTime->setText(StringUtils::format(timeName.c_str(), time.GetDays(), time.GetHours()));

	}
}




