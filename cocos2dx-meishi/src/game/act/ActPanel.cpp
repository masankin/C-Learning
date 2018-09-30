#include "ActPanel.h"
#include "ActLoginLayout.h"
#include "ActItem.h"
#include "ActControl.h"
#include "MessageManager.h"
#include "ActFirstCharge.h"
#include "ActRecharge.h"
#include "guide/GuideManager.h"
#include "ActDailyCharge.h"
#include "ui/PageFunction.h"
#include "data/MailData.h"

using namespace cocos2d;

ActPanel::ActPanel()
	:mSelectBtn(ACT_LOGIN)
{
	mHasReceive = false;
	sgMessageManager()->regisiterMessageHandler(EMT_GOTO_ACTLAB, this);

}


ActPanel::~ActPanel()
{
	sgActControl()->setActPanel(NULL);
	sgMessageManager()->removeMessageHandler(this);
	for (auto it = mActLayerMap.begin(); it!= mActLayerMap.end(); it++)
	{
		CC_SAFE_RELEASE_NULL((*it).second);
	}
	mActLayerMap.clear();
}

bool ActPanel::init()
{
	if (!ActionWindow::init())
	{
		return false;
	}

	mRoot = CSLoader::getInstance()->createNode("ui/ActNode.csb");
	mRoot->setAnchorPoint(Vec2(0.5, 0.5));
	setAnimation("fla_fulixin");
	
	setPopType(E_POP_TYPE_NONE);

	
	mActContent = dynamic_cast<Node*>(mRoot->getChildByName("Node_1"));
	mArmatureBg->setZOrder(-2);
	changeBone(mArmatureBg,"fulixin_qiandao",mActContent);
	changeBone(mArmatureBg,"fulixin_guanbianniu",mRoot->getChildByName("BtnBack"));
	changeBone(mArmatureBg,"fulixin_di",mRoot->getChildByName("Sprite_1"));
	changeBone(mArmatureBg,"fulixin_gazi",mRoot->getChildByName("Sprite_2"));
	changeBone(mArmatureBg,"fulixin_daiziaia",mRoot->getChildByName("Sprite_3"));

	int i = 1;
	std::map<int,bool>::iterator itr = sgActControl()->getActTypeEff()->begin();
	for (; itr != sgActControl()->getActTypeEff()->end() ; itr++)
	{
		int type = itr->first;
		if (type > ACT_RECHARGE)
		{
			break;
		}

		if (type == ACT_FIRST_CHARGE) {
			continue;
		}

		std::string sName = StringUtils::format("ProjectNode_%d",i);
		Node* pNode1 = dynamic_cast<Node*>(mRoot->getChildByName(sName));
		ActItem* pItem1 = ActItem::createByType((ACTTYPE)type);
		pItem1->initWithNode(pNode1);

		mActItemMap.insert(std::make_pair(type,pItem1));
		pNode1->setVisible(true);
		pNode1->setZOrder(-1);
	
		if (i == 1)
		{
			changeBone(mArmatureBg,"fulixin_A",pNode1);
		}
		else if (i == 2)
		{
			changeBone(mArmatureBg,"fulixin_B",pNode1);
		}
		else if (i == 3)
		{
			changeBone(mArmatureBg,"fulixin_C",pNode1);
		}

		i++;
		
		if (i > 3) {
			break;
		}
	}
	mSiginFlagLeft = dynamic_cast<Button*>(mRoot->getChildByName("sigin_flag1"));
	changeBone(mArmatureBg,"fulixin_anniub",mSiginFlagLeft);
	mSiginFlagRight = dynamic_cast<Button*>(mRoot->getChildByName("sigin_flag2"));
	changeBone(mArmatureBg,"fulixin_anniuad",mSiginFlagRight);
	

	onLocateClickCallback(mRoot);
	this->addChild(mRoot);
	
	mGuideNode = Node::create();
	addChild(mGuideNode);
	mGuideNode->setTag(9999);
	
	Node* mUINode1 = dynamic_cast<Node* >(mRoot->getChildByTag(80));	
	CCASSERT(mUINode1 != NULL, "who delete my guide node !");
	mGuideNode->setPosition(mUINode1->getPosition() + ccp(640,320));


	mGuideNode2 = Node::create();
	addChild(mGuideNode2);
	mGuideNode2->setTag(10000);
	mGuideNode2->setPosition(mUINode1->getPosition() + ccp(640,320));
	sgActControl()->setActPanel(this);
	return true;
}

void ActPanel::onEnter()
{
	ActionWindow::onEnter();

	if (sgActControl()->getBtnRefresh())
	{
		setActPanelBtn();
		sgActControl()->setBtnRefresh(false);
	}
	else
	{
		setTypeIdx(mSelectBtn);
	}
}

void ActPanel::onExit()
{
	ActionWindow::onExit(); 
}

 

void ActPanel::setTypeIdx(int pTypeId)
{
	mActItemMap[mSelectBtn]->setSelect(false);
	mSelectBtn = pTypeId;
	mActItemMap[mSelectBtn]->setSelect(true);
	updateContent();	
}

void ActPanel::updateContent()
{
	ActItem* pItem = mActItemMap[mSelectBtn];
	pItem->updateUI();

	ActLayout* pActLayout = getActLayout((ACTTYPE)mSelectBtn);
	if (pActLayout== NULL)
	{
		pActLayout = createActLayout(ACTTYPE(mSelectBtn));
		pActLayout->setAnchorPoint(Vec2(0,0));
		CC_SAFE_RETAIN(pActLayout);
		mActLayerMap.insert(std::make_pair(mSelectBtn,pActLayout));
		
	}
	if(ACTTYPE(mSelectBtn) == ACT_LOGIN)
	{
		mSiginFlagLeft->setVisible(true);
		mSiginFlagRight->setVisible(true);

		if (sgActControl()->getActProxy()->getSevenDaySiginList().getlist_size() && 
			sgActControl()->getActProxy()->getSevenDaySiginList().getlist(0).isget() == false)
		{  
			CCLOG("[trace]ActPanel::updateContent");
		} 
		mHasReceive = true;
	}else
	{
		mSiginFlagLeft->setVisible(false);
		mSiginFlagRight->setVisible(false);
	}
	
	mActContent->removeAllChildren();
	mActContent->addChild(pActLayout);
	pActLayout->updateInfo();
}

void ActPanel::onMenuItemAction(std::string pName, cocos2d::Ref* pRef)
{
	if (pName == "onBack")
	{
		this->setShow(false);
		sgPageFunction()->enableOptionButton(true);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
	}else if (pName == "onActSelect")
	{
		int iType = (dynamic_cast<ActItem*>(dynamic_cast<Node*>(pRef)->getParent()->getChildByTag(100)))->getActType();
		if (mSelectBtn != iType)
		{
			switch (iType)
			{
			case ACT_LOGIN:
				sgActControl()->sendQueryActLogin();
			default:
				break;
			}
			setTypeIdx(iType);
		}
	} else if (pName == "onLeft")
	{
		ActLayout* pActLayout = getActLayout((ACTTYPE)mSelectBtn);
		if (pActLayout)
		{
			pActLayout->tableViewMove(false);
		}
	}
	else if (pName == "onRight")
	{
		ActLayout* pActLayout = getActLayout((ACTTYPE)mSelectBtn);
		if (pActLayout)
		{
			pActLayout->tableViewMove(true);
		}
			
	}


}
ActLayout* ActPanel::createActLayout(ACTTYPE pType)
{
	ActLayout* pRet = NULL; 
	switch (pType)
	{
	case ACT_NULL:
		break;
	case ACT_LOGIN:
		pRet = ActLoginLayout::create();		
		break;
	case ACT_FIRST_CHARGE:
		//pRet = ActFirstCharge::create();
		break;
	case ACT_DAILY_CHARGE:
		pRet = ActDailyCharge:: create();
		break;
	case ACT_RECHARGE:
		pRet = ActRecharge::create();
		break;
	default:
		break;
	}

	return pRet;
}

ActLayout* ActPanel::getActLayout(ACTTYPE pType)
{
	ActLayout* pRet = NULL;
	auto it = mActLayerMap.find(pType);
	if (it != mActLayerMap.end() && (*it).second )
	{
		pRet = (*it).second;
	}
	return pRet;
}

void ActPanel::setShow(bool b)
{
	ActionWindow::setShow(b);
	if (b)
	{
		if(ACTTYPE(mSelectBtn) == ACT_LOGIN)
		{
			sgActControl()->sendQueryActLogin();
		}
	}
}

void ActPanel::updateUI(int type)
{
	setTypeIdx(type);
}

void ActPanel::setActPanelBtn()
{
	int i = 1;
	for (; i <= 3; i++)
	{
		std::string sName = StringUtils::format("ProjectNode_%d",i);
		Node* pNode1 = mRoot->getChildByName(sName);
		pNode1->removeChildByTag(100);
		pNode1->setVisible(false);
	}
	i = 1;
	mActItemMap.clear();
	std::map<int,bool>::iterator itr = sgActControl()->getActTypeEff()->begin();
	for (; itr != sgActControl()->getActTypeEff()->end() ; itr++)
	{
		int type = itr->first;
		if (type > ACT_RECHARGE)
		{
			break;
		}
		std::string sName = StringUtils::format("ProjectNode_%d",i);
		Node* pNode1 = mRoot->getChildByName(sName);
		ActItem* pItem1 = ActItem::createByType((ACTTYPE)type);
		pItem1->initWithNode(pNode1);

		mActItemMap.insert(std::make_pair(type,pItem1));
		pNode1->setVisible(true);
		pNode1->setZOrder(-1);

		i++;

	}
	mSelectBtn = ACT_LOGIN;
	setTypeIdx(mSelectBtn);
}

void ActPanel::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_GOTO_ACTLAB)
	{
		MessageGotoActTab* pMsg = dynamic_cast<MessageGotoActTab*>(message);
		if (!pMsg)
			return;
		if (pMsg->getPage() == MSG_FUC_ACT)
		{
			int actType = pMsg->getLab();

			auto itr = sgActControl()->getActTypeEff()->find(actType);
			if (itr == sgActControl()->getActTypeEff()->end()) return;

			if (actType == ACT_LOGIN)
			{
				sgActControl()->sendQueryActLogin();
			}
			// 过滤首冲
			if (actType != ACT_FIRST_CHARGE) {
				setTypeIdx(actType);
			}
		}
	}
}
