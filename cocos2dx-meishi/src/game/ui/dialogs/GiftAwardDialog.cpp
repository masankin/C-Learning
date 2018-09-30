#include "GiftAwardDialog.h"

#include "guide/GuideManager.h"
#include "game/utils/Comm.h"
#include "net/UserObj.h"
#include "data/MailData.h"
#include "game/ui/PageFunction.h"
#include "ui/PageFunction.h"
#define ITEM_DISTANCE_H 300
#define ITEM_DISTANCE_V 200

GiftAwardDialog::GiftAwardDialog(void) 
:mbGuideCallBack(false), 
mAniParentNode(nullptr), 
mIsForce(false), 
mItemCounter(0), 
mArmItemBg(nullptr),
mArmItem(nullptr), 
mTarget(nullptr), 
mFun(nullptr),
mIsShowAward(true),
mGiftID2(0),
mTitleLabel(nullptr),
mCallBack(nullptr)
{
	mItemAniArray.clear();
}


GiftAwardDialog::~GiftAwardDialog(void)
{
	CC_SAFE_DELETE(mArmItem);
}

bool GiftAwardDialog::init()
{
	if (!Window::init())
	{
		return false;
	}
	mRoot = CSLoader::getInstance()->createNode("ui/GiftAwardNode.csb");
	mRoot->setAnchorPoint(Vec2(0.5, 0.5));
	onLocateClickCallback(mRoot);
	this->addChild(mRoot);

	mPanelNext = dynamic_cast<ui::Layout*>(mRoot->getChildByName("PanelNext"));
	mPanelNext->setEnabled(false);
	mAniParentNode = mRoot->getChildByName("NodeAni");
	mTitleLabel = dynamic_cast<ui::Text*>(mRoot->getChildByName("TitleLabel"));
	//mArmItemBg = cocostudio::Armature::create("fla_lingqu_ADDITIVE");
	//mArmItemBg->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);
	//mAniParentNode->addChild(mArmItemBg);
	//mArmItem = cocostudio::Armature::create("fla_lingqu");
	//mAniParentNode->addChild(mArmItem);
	mRoot->getChildByName("Sprite_5")->setVisible(false);

	mIsShowDailyMission = false;
	return true;
}

void GiftAwardDialog::onMenuItemAction(std::string pName, cocos2d::Ref* pRef)
{
	if (mIsForce)
		return;

	if (pName == "onNext")
	{
		//bPanelNextTouched = true;
		//playTouchNext();
		//extraWork();
		playItemHide(0.0);
		mPanelNext->setEnabled(false);
	}
}

GiftAwardDialog* GiftAwardDialog::showDialogByGiftId(int giftId, bool isShow)
{
	auto giftData = sgTableManager()->getGiftAward(giftId);
	if (giftData == NULL)
		return nullptr;

	GiftAwardDialog* pDialog = dynamic_cast<GiftAwardDialog*>(sgPageFunction()->createWindow(MSG_FUC_GIFTDIALOG,"GiftAwardNode.csb"));
	if (pDialog)
	{
		pDialog->removeAllItems();

		pDialog->mGiftID = giftId;
		pDialog->mGiftID2 = 0;
			

		if (giftData->mTitle.size() != 0) {
			pDialog->mTitleLabel->setVisible(true);
			pDialog->mTitleLabel->setString(giftData->mTitle);
		}
		else{
			pDialog->mTitleLabel->setVisible(false);
		}

		//////////////////////////////////////////////////////////////////////////
		if (giftData->mGiftData.size() == 0)
		{
			return nullptr;
		}
		for (int i = 0; i < giftData->mGiftData.size(); i++)
		{
			GiftInfo lGiftInfo = giftData->mGiftData.at(i);
			
			pDialog->pushBackItem(lGiftInfo.mType, lGiftInfo.mId, lGiftInfo.mNumber);
		}

		pDialog->mIsShowAward = isShow;
		//dynamic_cast<Sprite*>(pDialog->mRoot->getChildByName("Sprite_5"))->setVisible(isShow);
		pDialog->mIsShowDailyMission = !isShow;
		pDialog->setShow(true);

		return pDialog;
	}
	return nullptr;
}

GiftAwardDialog* GiftAwardDialog::showDialogByGiftIds(int gift_id1, int gift_id2)
{
	GiftAwardDialog* dialog = GiftAwardDialog::showDialogByGiftId(gift_id1);
	if (dialog) {
		dialog->mGiftID2 = gift_id2;
		return dialog;
	}
	return nullptr;
}

GiftAwardDialog* GiftAwardDialog::showDialogByChests(std::string chestsStr)
{
	GiftAwardDialog* pDialog = nullptr;
	std::vector<std::string> lChestsVec = splitStr(chestsStr, "#");
	for (int i = 0; i < lChestsVec.size(); i++)
	{
		if (lChestsVec[i] == "")
		{
			break;
		}
		std::vector<std::string> lGodsVec = splitStr(lChestsVec[i], "+");
		if (lGodsVec.size() > 0)
		{
			int ltype = atoi(lGodsVec[0].c_str());
			if (!pDialog){
				pDialog = dynamic_cast<GiftAwardDialog*>(sgPageFunction()->createWindow(MSG_FUC_GIFTDIALOG, "GiftAwardNode.csb"));
				if (pDialog)
					pDialog->removeAllItems();
			}
			if (pDialog)
				pDialog->pushBackItem(ltype, atoi(lGodsVec[1].c_str()), atoi(lGodsVec[2].c_str()));
		}
	}

	if (pDialog)
	{
		pDialog->setShow(true);
		return pDialog;
	}
	return nullptr;
	
}

void GiftAwardDialog::showDialogByAward(std::map<int,int> pAwardMap)
{
	//邮件那用到，类型还未统一
	if (pAwardMap.size() < 1) return;
	GiftAwardDialog* pDialog = nullptr;
	for (auto it = pAwardMap.begin(); it != pAwardMap.end(); it++)
	{
		int type;
		int id = it->first;
		do
		{
			type = sgMailData()->getGoodsType(id);
		} while (0);
		if (!pDialog){
			pDialog = dynamic_cast<GiftAwardDialog*>(sgPageFunction()->createWindow(MSG_FUC_GIFTDIALOG, "GiftAwardNode.csb"));
			if (pDialog)
				pDialog->removeAllItems();
		}
		if (pDialog)
			pDialog->pushBackItem(type, id, it->second);
	}
	 
	if (pDialog)
	{
		pDialog->setShow(true);
	}
}


void GiftAwardDialog::onShow()
{
	if (mIsShowAward)
		playAllItemAni2();
	mIsShowAward = true;
}


void GiftAwardDialog::playAllItemAni()
{
	bPanelNextTouched = false;
	mPanelNext->setEnabled(true);

	if (mAwardVec.size() <= 0)
	{
		mAwardVec.clear();
		this->setShow(false);
		if (mbGuideCallBack)
		{
			
			mbGuideCallBack = false;
			if (mTarget && mFun && mGiftID != 0)
			{
				(mTarget->*mFun)(mGiftID);
			}
		}
#ifdef MGAME_PLATFORM_MPLUS
		else
		{
			if (sgMplusControl()->getIsCanShare())
			{
				GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_MPLUSHELP_SHARING), [](Ref* sendr){
					GenerlDialog::hide();
					sgChannelApi()->callGameInfo(TYPE_SHARE_MSG_TYPE, "5");
				}, false);
				sgMplusControl()->setIsCanShare(false);
			}
		}
#endif



		return;

	}
	
	std::vector<GiftInfo>::iterator it = mAwardVec.end();
	it--;
	int number = it->mNumber;
	std::string iconName = "";

	const GoodsInfo* lGoodsInfo = sgTableManager()->getGoodsInfo(it->mType, it->mId);
	if (!lGoodsInfo)
	{
		return;
	}
	iconName = lGoodsInfo->mIcon;

	Node* pItemNode = CSLoader::getInstance()->createNode("ui/ItemNode.csb");
	pItemNode->getChildByName("Image_1")->setVisible(false);
	auto pIcon = dynamic_cast<ImageView*>(pItemNode->getChildByName("Image_2"));
	auto pNumb = dynamic_cast<Text*>(pItemNode->getChildByName("Text_1"));
	pIcon->loadTexture(iconName, TextureResType::PLIST);
	pNumb->setString(__String::createWithFormat("X%d", number)->getCString());
	auto bone = mArmItem->getBone("chengjiu_a2");
	bone->addDisplay(pItemNode, 0);
	mArmItem->getAnimation()->play("play");
	mArmItemBg->getAnimation()->play("play");
	mPanelNext->setEnabled(true);
	mArmItem->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID) {
		if (movementType == cocostudio::MovementEventType::COMPLETE)
		{
			this->playAllItemAni();
		}
	});
	
	mAwardVec.pop_back();
}

Vec2 GiftAwardDialog::getPosByIndex(int idx)
{
	// test
	int itemCount = mAwardVec.size();
	// 根据物品个数计算物品出现的位置
	return Vec2(-((itemCount - 1)*ITEM_DISTANCE_H / 2) + (idx - 1)*ITEM_DISTANCE_H, 0);
}

void GiftAwardDialog::extraWork()
{
	mAwardVec.clear();
	mItemCounter = 0;
	mItemAniArray.clear();
	mAniParentNode->removeAllChildrenWithCleanup(true);

	if (mbGuideCallBack)
	{

		mbGuideCallBack = false;
		if (mTarget && mFun && mGiftID != 0)
		{
			(mTarget->*mFun)(mGiftID);
		}
	}
#ifdef MGAME_PLATFORM_MPLUS
	else
	{
		if (sgMplusControl()->getIsCanShare())
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_MPLUSHELP_SHARING), [](Ref* sendr){
				GenerlDialog::hide();
				sgChannelApi()->callGameInfo(TYPE_SHARE_MSG_TYPE, "5");
			}, false);
			sgMplusControl()->setIsCanShare(false);
		}
	}
#endif

	if (mCallBack)
	{
		mCallBack();
		mCallBack = nullptr;
	}
	this->setShow(false);

}

void GiftAwardDialog::playAllItemAni2()
{
	mAniParentNode->removeAllChildrenWithCleanup(true);

	mPanelNext->setEnabled(false);
	std::vector<Node*> mNodeVec;
	mNodeVec.clear();
	int size = mAwardVec.size();
	int count = 0;
	if (size > 0 && size < 4)
		count = 1;
	else if (size >= 4 && size <= 6)
	{
		count = 2;
	}
	else if (size > 6)
	{
		count = 3;
	}

	for (int i = 0; i < count; i++)
	{
		Node* node = Node::create();
		mNodeVec.push_back(node);
	}
	int nodeSize = mNodeVec.size();
	int iCount = 0;
	std::vector<GiftInfo>::iterator iter = mAwardVec.begin();
	for (; iter != mAwardVec.end(); ++iter) {
		int number = iter->mNumber;
		std::string iconName = "";
		const GoodsInfo* lGoodsInfo = sgTableManager()->getGoodsInfo(iter->mType, iter->mId);
		if (!lGoodsInfo) break;

		iconName = lGoodsInfo->mIconBig;
		Node* rewardNode = getItemNode(iconName, number);
		auto itemAni = cocostudio::Armature::create("fla_Buling_WP");
		//itemAni->setPosition(getPosByIndex(++i));
		itemAni->setBlendFunc(BlendFunc::COMADDITIVE);

		if (size > 6 && nodeSize == 3)
		{
			int iNum = ceil((float)size / 3);
			if (iCount < 3)
			{
				itemAni->setPositionX(ITEM_DISTANCE_H*iCount);
				mNodeVec.at(0)->addChild(itemAni);
			}

			else if (iCount < 6)
			{
				itemAni->setPositionX(ITEM_DISTANCE_H*(iCount - iNum));
				mNodeVec.at(1)->addChild(itemAni);
			}

			else
			{
				itemAni->setPositionX(ITEM_DISTANCE_H*(iCount - 2 * iNum));
				mNodeVec.at(2)->addChild(itemAni);
			}

		}
		else if (size >= 4 && nodeSize == 2)
		{
			int iNum = ceil((float)size / 2);
			if (iCount < iNum)
			{
				itemAni->setPositionX(ITEM_DISTANCE_H*iCount);
				mNodeVec.at(0)->addChild(itemAni);
			}
			else
			{
				itemAni->setPositionX(ITEM_DISTANCE_H*(iCount - iNum));
				mNodeVec.at(1)->addChild(itemAni);
			}
		}
		else
		{
			itemAni->setPositionX(ITEM_DISTANCE_H*iCount);
			mNodeVec.at(0)->addChild(itemAni);
		}

		
		iCount++;
		//mAniParentNode->addChild(itemAni);
		auto bone = itemAni->getBone("guge_BaoXiangLBwp_5");
		bone->addDisplay(rewardNode, 0);
		//itemAni->getAnimation()->play("born");
		mItemAniArray.pushBack(itemAni);
		//mItemAniArray.push_back(itemAni);
		itemAni->setVisible(false);
		itemAni->getAnimation()->setMovementEventCallFunc(this,movementEvent_selector(GiftAwardDialog::itemAniCall));
	}

	std::vector<Node*>::iterator iter2 = mNodeVec.begin();
	for (; iter2 != mNodeVec.end(); iter2++)
	{
		mAniParentNode->addChild(*iter2);
		(*iter2)->setPositionX(-((*iter2)->getChildrenCount() - 1)*ITEM_DISTANCE_H / 2);
	}

	if (nodeSize == 1)
	{
		mNodeVec.at(0)->setPositionY(0);
	}
	else if (nodeSize == 2)
	{
		mNodeVec.at(0)->setPositionY(ITEM_DISTANCE_V / 2);
		mNodeVec.at(1)->setPositionY(-ITEM_DISTANCE_V / 2);
	}
	else if (nodeSize == 3)
	{
		mNodeVec.at(0)->setPositionY(ITEM_DISTANCE_V );
		mNodeVec.at(1)->setPositionY(0);
		mNodeVec.at(2)->setPositionY(-ITEM_DISTANCE_V);
	}

	if (mItemAniArray.size() > 0) {
		float t = 0.3f;
		if (mIsShowDailyMission)
		{
			playSigleItem(0.0);
			t = 1.0f;
			mIsShowDailyMission = false;
		}
		
		this->schedule(schedule_selector(GiftAwardDialog::playSigleItem), t);
	}
	else {
		mPanelNext->setEnabled(true);
	}
}

void GiftAwardDialog::itemAniCall(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (type == cocostudio::MovementEventType::COMPLETE && aniName == "born")
	{
		pAr->getAnimation()->play("stand");

		if (mItemCounter == mItemAniArray.size() && mIsForce ) {
			this->scheduleOnce(schedule_selector(GiftAwardDialog::playItemHide), 0.5f);
		}
	}

	if (type == cocostudio::MovementEventType::COMPLETE && aniName == "end" && mItemCounter > 0)
	{
		//CCLOG("[trace]  mItemCounter ==                %d", mItemCounter);
		extraWork();	
	}
}

void GiftAwardDialog::playItemHide(float dt)
{
	auto iter = mItemAniArray.begin();
	for (; iter != mItemAniArray.end(); ++iter)
	{
		(*iter)->getAnimation()->play("end");
	}
}

void GiftAwardDialog::showGiftTitle()
{
	//ui::Text* title = ui::Text::create();
}

void GiftAwardDialog::playSigleItem(float dt)
{
	if (mItemAniArray.size() > mItemCounter) {
		//dynamic_cast<cocostudio::Armature*>(mItemAniArray.at(mItemCounter))->getAnimation()->play("born");
		mItemAniArray.at(mItemCounter)->setVisible(true);
		mItemAniArray.at(mItemCounter)->getAnimation()->play("born");

		++mItemCounter;
	} else {
		this->unschedule(schedule_selector(GiftAwardDialog::playSigleItem));
		mPanelNext->setEnabled(true);

		if (sgUserObj()->getIsAfterAwardOpenAgain())
		{
			extraWork();
			sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "MissionDialog.csb", 1);
			sgUserObj()->setIsAfterAwardOpenAgain(false);
		}
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("eCloseMissionCompleteDialog", NULL);

	}
}

Node* GiftAwardDialog::getItemNode(std::string iconName, int count)
{
	Node* pItemNode = CSLoader::getInstance()->createNode("ui/ItemNode.csb");
	pItemNode->getChildByName("Image_1")->setVisible(false);
	auto pIcon = dynamic_cast<ImageView*>(pItemNode->getChildByName("Image_2"));
	auto pNumb = dynamic_cast<Text*>(pItemNode->getChildByName("Text_1"));
	pIcon->loadTexture(iconName, TextureResType::PLIST);
	pIcon->setContentSize(Size(180, 180));
	pNumb->setString(__String::createWithFormat("X%d", count)->getCString());

	return pItemNode;
}

void GiftAwardDialog::pushBackItem(int type, int id ,int number)
{
	GiftInfo lGiftInfo;
	lGiftInfo.mType = (UserObj::PROPERTYTYPE)type;
	lGiftInfo.mId = id;
	lGiftInfo.mNumber = number;
	mAwardVec.push_back(lGiftInfo);
}

void GiftAwardDialog::removeAllItems()
{
	//mItemList->removeAllItems();
	mItemCounter = 0;
	mAwardVec.clear();
}

void GiftAwardDialog::playTouchNext()
{
	/*
	mPanelNext->setEnabled(true); 
	this->playAllItemAni();
	*/
}

void GiftAwardDialog::setbGuideCallBack(bool bGuide, Node* target, SEL_CallFuncID fun, bool isForce)
{ 
	mbGuideCallBack = bGuide;
	mTarget = target;
	mFun = fun;
	mIsForce = isForce;

	//Node* pTips = mRoot->getChildByName("Sprite_5");
	//if (!pTips)
	//	return;
	//if (mIsForce)
	//	pTips->setVisible(false);
	//else
	//	pTips->setVisible(true);

}

void GiftAwardDialog::clearGuideCallback()
{
	mbGuideCallBack = false;
	mTarget = NULL;
	mFun = NULL;
}
