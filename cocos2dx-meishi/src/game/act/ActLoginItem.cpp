#include "ActLoginItem.h"
#include "data/TableManager.h"
#include "ActControl.h"
#include "Constant.h"
#include "MessageManager.h"
#include "guide/GuideManager.h"
#include "SoundManager.h"

//using namespace cocostudio;
using namespace ui;

ActLoginItem::ActLoginItem(void)
	:mEffectNode(nullptr)
{
}


ActLoginItem::~ActLoginItem(void)
{
}

bool ActLoginItem::init()
{
	if (!Node::init())
	{
		return false;
	}
	Node* pNode = CSLoader::getInstance()->createNode("ui/ActLoginAwardItem.csb");

	pNode->setPosition(Vec2(125,225));

	mDay = dynamic_cast<Text*>(pNode->getChildByName("Text_1"));
	mDayStr = mDay->getString();

	Node* projectNode = dynamic_cast<Node*>(pNode->getChildByName("ProjectNode_1"));
	mNum = dynamic_cast<Text*>(projectNode->getChildByName("Text_1"));//////
	projectNode->getChildByName("Image_1")->setVisible(false);
	mName = dynamic_cast<Text*>(pNode->getChildByName("Text_18_0"));

	mIcon = dynamic_cast<ImageView*>(projectNode->getChildByName("Image_2"));/////
	mBtnName = dynamic_cast<ImageView*>(pNode->getChildByName("Image_47"));
	mBtnName1 = dynamic_cast<ImageView*>(pNode->getChildByName("Image_47_1"));
	mTip = dynamic_cast<ImageView*>(pNode->getChildByName("Image_47_0"));

	/// 添加特效节点
	mEffectNode = dynamic_cast<Node*>(projectNode->getChildByName("EffectNode"));
	if (mEffectNode) {
		//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_QDtexiao.png", "fla_QDtexiao.plist", "fla_QDtexiao.xml");
		auto effect = cocostudio::CCArmature::create("fla_QDtexiao_ADDITIVE");
		effect->setBlendFunc(BlendFunc::ADDITIVE);
		effect->getAnimation()->play("play1");
		mEffectNode->addChild(effect);
		mEffectNode->setVisible(false);
	}

	dynamic_cast<Widget*>(projectNode->getChildByName("Panel_1"))->setSwallowTouches(false);
	////

	mBtn = dynamic_cast<Button*>(pNode->getChildByName("Button_3"));
	mBtn->setSwallowTouches(false);
	mBtn->addClickEventListener(std::bind(&ActLoginItem::onClickCallBack, this, std::placeholders::_1));
	addChild(pNode);
	
	return true;
}

void ActLoginItem::onClickCallBack(Ref*)
{
	if (sgGuideManager()->getGuideLock() == true)
		return;

	sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);
	mIsOnTouch = true;
	sgActControl()->sendGetActLogin();
	//完成新手引导点击领取福利界面
	sgGuideManager()->unRegistGuideMessage();
	CCLOG("[GUIDE]### END to get Fuli message");
}

void ActLoginItem::onTouch()
{
	if (mIsOnTouch&& mBtn->isTouchEnabled())
	{
		mIsOnTouch = false;
		sgActControl()->sendGetActLogin();
	}
}

Node* ActLoginItem::getGuideNode()
{
	return mBtn;
}

void ActLoginItem::updateInfo(int day ,bool isGet, bool isCanGet, int giftId)
{
	mDay->setString(__String::createWithFormat(mDayStr.c_str(),day)->getCString());
	if (isGet)
	{
		mBtnStatus = BUTTON_GETED;
	}else if (isCanGet)
	{
		mBtnStatus = BUTTON_CANGET;
	}else
	{
		mBtnStatus = BUTTON_NOTCANGET;
	}

	if (mEffectNode){
		mEffectNode->setVisible(false);
	}

	switch (mBtnStatus)
	{
	case ActLoginItem::BUTTON_GETED:
		{
			mBtn->setVisible(false);
			mBtn->setTouchEnabled(false);
			mBtn->setBright(false);
			mBtnName->setVisible(false);
			mBtnName1->setVisible(false);
			mTip->setVisible(true);
		}
		break;
	case ActLoginItem::BUTTON_CANGET:
		{
			mBtn->setVisible(true);
			mBtn->setTouchEnabled(true);
			mBtn->setBright(true);
			mBtnName->setVisible(true);
			mBtnName1->setVisible(false);
			mTip->setVisible(false);
			if (mEffectNode){
				mEffectNode->setVisible(true);
			}
		}
		break;
	case ActLoginItem::BUTTON_NOTCANGET:
		{
			mBtn->setTouchEnabled(false);
			mBtn->setBright(false);
			mBtn->setVisible(true);
			mBtnName->setVisible(false);
			mBtnName1->setVisible(true);
			mTip->setVisible(false);
		}
		break;
	default:
		break;
	}
	mBtn->setSwallowTouches(false);
	const GiftAward* pGiftAward = sgTableManager()->getGiftAward(giftId);
	int pSize = 0;
	std::string pIconStr ="";
	std::string pName = "";
	int pTextId = 0;
	if (pGiftAward->mGiftData.size() == 0)
	{
		return;
	}
	GiftInfo lGiftInfo = pGiftAward->mGiftData.at(0);
	const GoodsInfo* lGoodsInfo = sgTableManager()->getGoodsInfo(lGiftInfo.mType, lGiftInfo.mId);
	pSize = lGiftInfo.mNumber;
	pIconStr = lGoodsInfo->mIconSmall;
	pName = lGoodsInfo->mName;

	mIcon->loadTexture(pIconStr,TextureResType::PLIST);
	mNum->setString(__String::createWithFormat("%d",pSize)->getCString());
	mName->setString(pName);
	if(mBtnStatus == BUTTON_GETED)
	{
		//完成新手引导点击领取福利界面
		sgGuideManager()->unRegistGuideMessage();
		CCLOG("[GUIDE]### END to get Fuli message"); 
	}
}

