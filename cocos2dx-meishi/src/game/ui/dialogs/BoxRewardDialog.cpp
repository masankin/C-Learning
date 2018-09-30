#include "BoxRewardDialog.h"
#include "game/ui//PageFunction.h"
#include "game/ui//system/Window.h"
#include "SoundManager.h"
#include "game/utils/Comm.h"
#include "net/UserObj.h"
#include "core/stl_helper.h"
#include "game/data/TableManager.h"
#include "GiftAwardDialog.h"

#define ITEM_DISTANCE_H 300
#define ITEM_DISTANCE_V 200

BoxRewardDialog::BoxRewardDialog(void) : mArmBox(0), mCallBack(nullptr), mBoxRewardType(BOX_REWARD_PVE_BASE)
{

}


BoxRewardDialog::~BoxRewardDialog(void)
{
	//SAFE_RELEASE_VECTOR(mArmItemVec);
	//CC_SAFE_DELETE(mArmBox);
}

void BoxRewardDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onOpenBox")
	{
		if (!mbOpenedBox)
			playOpenBoxAni(0);
		
	}
	else if (cName == "OnClose")
	{
		if (mbEnableClose)
			hideItemAni(0);
	}
}

bool BoxRewardDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/BoxRewardDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);

		mNodeBoxAni = mRoot->getChildByName("NodeBoxAni");
		mNodeItemAni = mRoot->getChildByName("NodeItemAni");
		mPanelOpenBox = dynamic_cast<ui::Layout*>(mRoot->getChildByName("PanelOpenBox"));
		mSpriteOpenBox = dynamic_cast<Sprite*>(mRoot->getChildByName("SpriteOpenBox"));

		onLocateClickCallback(mRoot);
		
		ret = true;
	} while (0);


	return ret;
}

BoxRewardDialog* BoxRewardDialog::showBoxRewardDialog(eBoxRewardType eType, int giftId, std::function<void()> callBackFunc)
{
	auto giftData = sgTableManager()->getGiftAward(giftId);
	if (!giftData)
		return nullptr;
	BoxRewardDialog* pDialog = dynamic_cast<BoxRewardDialog*>(sgPageFunction()->createWindow(
		MSG_FUC_RESULT_BOX, "BoxRewardDialog.csb"));
	if (pDialog)
	{
		pDialog->mCallBack = callBackFunc;
		pDialog->mBoxRewardType = eType;
		pDialog->removeAllItems();
		pDialog->mGiftId = giftId;
		pDialog->setShow(true);
		pDialog->updateUI();

		return pDialog;
	}
	return nullptr;
}

BoxRewardDialog* BoxRewardDialog::showBoxRewardDialog(eBoxRewardType eType, std::string strBoxReward, std::function<void()> callBackFunc)
{
	BoxRewardDialog* pDialog = dynamic_cast<BoxRewardDialog*>(sgPageFunction()->createWindow(
		MSG_FUC_RESULT_BOX, "BoxRewardDialog.csb"));
	if (pDialog)
	{
		pDialog->mCallBack = callBackFunc;
		pDialog->mBoxRewardType = eType;
		pDialog->msReward = strBoxReward;
		pDialog->removeAllItems();
		pDialog->setShow(true);
		pDialog->updateUI();
		
		return pDialog;
	}
	return nullptr;
}


void BoxRewardDialog::updateUI()
{
	mbOpenedBox = false;
	mbEnableClose = false;

	mPanelOpenBox->setVisible(false);
	mSpriteOpenBox->setVisible(false);

	mNodeItemAni->removeAllChildren();
	mNodeItemAni->setVisible(false);
	mNodeBoxAni->setVisible(true);
	mArmItemVec.clear();
	initBoxAni();
	displayBox();
}

void BoxRewardDialog::initBoxAni()
{
	mNodeBoxAni->removeAllChildren();
	switch (mBoxRewardType)
	{
	case BOX_REWARD_PVP_NOITEM:
	{
		mArmBox = cocostudio::Armature::create("fla_BaoXiangLB_A");

		auto boneA1 = mArmBox->getBone("BaoXiangLB_a1_ADDITIVE");
		boneA1->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);

		auto boneA2 = mArmBox->getBone("BaoXiangLB_a2_ADDITIVE");
		boneA2->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);

		auto boneLB = mArmBox->getBone("guge_BaoXiangLB_ADDITIVE");
		boneLB->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);

		mNodeBoxAni->addChild(mArmBox);
	}
		break;
	case BOX_REWARD_PVE_BASE:
	{
		mArmBox = cocostudio::Armature::create("fla_BaoXiangLB_B");

		auto boneA1 = mArmBox->getBone("BaoXiangLB_as2_ADDITIVE");
		boneA1->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);

		auto boneA2 = mArmBox->getBone("BaoXiangLB_ab2_ADDITIVE");
		boneA2->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);

		auto boneLB = mArmBox->getBone("guge_BaoXiangLB_ADDITIVE");
		boneLB->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);

		mNodeBoxAni->addChild(mArmBox);
	}
		break;
	case BOX_REWARD_PVE_PERFECT:
	{
		mArmBox = cocostudio::Armature::create("fla_BaoXiangLB_C");

		auto boneA1 = mArmBox->getBone("BaoXiangLB_aaa2_ADDITIVE");
		boneA1->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);

		auto boneA2 = mArmBox->getBone("BaoXiangLB_aaas2_ADDITIVE");
		boneA2->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);

		auto boneLB = mArmBox->getBone("guge_BaoXiangLB_ADDITIVE");
		boneLB->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);

		mNodeBoxAni->addChild(mArmBox);
	}
		break;
	default:
		break;
	}
	
}

void BoxRewardDialog::displayBox()
{
	mArmBox->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
	{
		if (type == cocostudio::COMPLETE && aniName == "born")
		{
			mArmBox->getAnimation()->play("stand",-1,1);
			mPanelOpenBox->setVisible(true);
			mSpriteOpenBox->setVisible(true);
			
			this->scheduleOnce(schedule_selector(BoxRewardDialog::playOpenBoxAni), 5.0f);
		}
	});
	mArmBox->getAnimation()->play("born");
}

void BoxRewardDialog::playOpenBoxAni(float dt)
{
	if (mbOpenedBox || !mArmBox) return;
	mArmBox->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
	{
		if (type == cocostudio::COMPLETE && aniName == "end")
		{
			mArmBox->setVisible(false);
			//playItemAni();
			//todo:
			GiftAwardDialog* pGiftAwardDialog = nullptr;
			switch (mBoxRewardType)
			{
			case BOX_REWARD_PVP_NOITEM:
			{
				pGiftAwardDialog = GiftAwardDialog::showDialogByChests(msReward);
			}
				break;
			case BOX_REWARD_PVE_BASE:
			{
				pGiftAwardDialog = GiftAwardDialog::showDialogByGiftId(mGiftId);
			}
				break;
			case BOX_REWARD_PVE_PERFECT:
			{
				pGiftAwardDialog = GiftAwardDialog::showDialogByGiftId(mGiftId);
			}
				break;
			default:
				break;
			}
			if (mCallBack)
			{
				pGiftAwardDialog->mCallBack = mCallBack;
				mCallBack = nullptr;

			}
			this->setShow(false);
		}
	});
	mArmBox->getAnimation()->play("end");
	mbOpenedBox = true;
	mPanelOpenBox->setVisible(false);
	mSpriteOpenBox->setVisible(false);

}

void BoxRewardDialog::hideItemAni(float dt)
{
	return;
	int count = 0;
	vector<cocostudio::Armature* >::iterator iter =  mArmItemVec.begin();
	for (; iter != mArmItemVec.end(); iter++)
	{
		auto armItem = *iter;
		if (count == 0)
		{ 
			armItem->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
			{
				if (type == cocostudio::COMPLETE && aniName == "end")
				{
					this->setShow(false);
				}
			});
			mbEnableClose = false;
		}
		armItem->getAnimation()->play("end");
		count++;
	}
	
}

void BoxRewardDialog::playItemAni()
{
	return;
	mItemCounter = 0;
	mNodeItemAni->setVisible(true);
	int size = mAwardVec.size();
	std::vector<Node*> mNodeVec;
	mNodeVec.clear();
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

	for(int i= 0; i < count; i++)
	{
		Node* node = Node::create();
		mNodeVec.push_back(node);
	}

	int nodeSize = mNodeVec.size();

	int iCount = 0;
	std::vector<GiftInfo>::iterator it = mAwardVec.begin();
	for (; it != mAwardVec.end(); it++)
	{
		int number = it->mNumber;
		std::string iconName = "";

		const GoodsInfo* lGoodsInfo = sgTableManager()->getGoodsInfo(it->mType, it->mId);
		if (!lGoodsInfo)
		{
			continue;
		}
		iconName = lGoodsInfo->mIcon;

		auto armItem = cocostudio::Armature::create("fla_Buling_WP");
		armItem->setBlendFunc(BlendFunc::COMADDITIVE);
		auto bone = armItem->getBone("guge_BaoXiangLBwp_5");
		Node* pItemNode = CSLoader::getInstance()->createNode("ui/ItemNode.csb");
		pItemNode->getChildByName("Image_1")->setVisible(false);
		auto pIcon = dynamic_cast<ImageView*>(pItemNode->getChildByName("Image_2"));
		auto pNumb = dynamic_cast<Text*>(pItemNode->getChildByName("Text_1"));
		pIcon->loadTexture(iconName, TextureResType::PLIST);
		pNumb->setString(__String::createWithFormat("X%d", number)->getCString());
		bone->addDisplay(pItemNode, 0);
		bone->changeDisplayByIndex(0, true);

		armItem->getAnimation()->setMovementEventCallFunc([this,iCount](cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
		{
			if (type == cocostudio::COMPLETE && aniName == "born")
			{
				pAr->getAnimation()->play("stand", -1, 1);
				
			}
		});

		armItem->setVisible(false);
		mArmItemVec.push_back(armItem);
		
		//mNodeAni->addChild(armItem);
		if (size > 6 && nodeSize == 3)
		{
			int iNum = ceil((float)size / 3);
			if (iCount < 3)
			{
				armItem->setPositionX(ITEM_DISTANCE_H*iCount);
				mNodeVec.at(0)->addChild(armItem);
			}
			
			else if (iCount < 6)
			{
				armItem->setPositionX(ITEM_DISTANCE_H*(iCount - iNum));
				mNodeVec.at(1)->addChild(armItem);
			}
			
			else
			{
				armItem->setPositionX(ITEM_DISTANCE_H*(iCount - 2 * iNum));
				mNodeVec.at(2)->addChild(armItem);
			}
			
		}
		else if (size >= 4 && nodeSize == 2)
		{
			int iNum = ceil(float(size) / 2);
			if (iCount < iNum)
			{
				armItem->setPositionX(ITEM_DISTANCE_H*iCount);
				mNodeVec.at(0)->addChild(armItem);
			}
			else
			{
				armItem->setPositionX(ITEM_DISTANCE_H*(iCount-iNum));
				mNodeVec.at(1)->addChild(armItem);
			}
		}
		else
		{
			armItem->setPositionX(ITEM_DISTANCE_H*iCount);
			mNodeVec.at(0)->addChild(armItem);
		}
		iCount++;
	}

	std::vector<Node*>::iterator iter = mNodeVec.begin();
	for (; iter != mNodeVec.end(); iter++)
	{
		mNodeItemAni->addChild(*iter);
		(*iter)->setPositionX(-((*iter)->getChildrenCount() - 1)*ITEM_DISTANCE_H / 2);
	}

	if (nodeSize == 1)
	{
		mNodeVec.at(0)->setPositionY(0);
	}
	else if (nodeSize == 2)
	{
		mNodeVec.at(0)->setPositionY(ITEM_DISTANCE_H / 2);
		mNodeVec.at(1)->setPositionY(-ITEM_DISTANCE_H / 2);
	}
	else if (nodeSize == 3)
	{
		mNodeVec.at(0)->setPositionY(ITEM_DISTANCE_H / 2);
		mNodeVec.at(1)->setPositionY(-ITEM_DISTANCE_H / 2);
	}
	if (mArmItemVec.size() > 0) {
		float t = 0.3f;
		this->schedule(schedule_selector(BoxRewardDialog::playSigleItem), t);
	}
}

void BoxRewardDialog::playSigleItem(float dt)
{
	return;
	if (mArmItemVec.size() > mItemCounter) {
		//dynamic_cast<cocostudio::Armature*>(mItemAniArray.at(mItemCounter))->getAnimation()->play("born");
		mArmItemVec.at(mItemCounter)->setVisible(true);
		mArmItemVec.at(mItemCounter)->getAnimation()->play("born");

		++mItemCounter;
	}
	else {
		this->unschedule(schedule_selector(BoxRewardDialog::playSigleItem));
		mbEnableClose = true;

	}
}


void BoxRewardDialog::pushBackItem(int type, int id, int number)
{
	return;
	GiftInfo lGiftInfo;
	lGiftInfo.mType = (UserObj::PROPERTYTYPE)type;
	lGiftInfo.mId = id;
	lGiftInfo.mNumber = number;
	mAwardVec.push_back(lGiftInfo);
}

void BoxRewardDialog::removeAllItems()
{
	return;
	mAwardVec.clear();
}
