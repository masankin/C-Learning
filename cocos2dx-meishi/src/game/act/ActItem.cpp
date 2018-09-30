#include "ActItem.h"
#include "cocostudio/CocoStudio.h"
#include "act/ActControl.h"
using namespace ui;

#define BUTTON_TIP_TAG 100

ActItem::ActItem(ACTTYPE pType)
{
	mType = pType;
}


ActItem::~ActItem(void)
{
}

bool ActItem::init()
{
	if (!Layout::init())
	{
		return false;
	}

	return true;
}

ActItem* ActItem::createByType(ACTTYPE pType)
{
	ActItem* pItem = new ActItem(pType);
	if (pItem && pItem->init())
	{
		pItem->autorelease();
	}else
	{
		CC_SAFE_DELETE(pItem);
	}
	return pItem;
}

void ActItem::initWithNode(Node* mNode)
{
	m_itemNode = mNode;
	mNode->addChild(this);
	this->setTag(100);
	mActImage = dynamic_cast<ui::ImageView*>(m_itemNode->getChildByName("Image_6"));
	mTitle = dynamic_cast<Sprite*>(m_itemNode->getChildByName("Sprite_1"));

	updateUI();
	setSelect(false);
}

void ActItem::updateUI()
{
	if (sgActControl()->checkActCanGetByType(mType))
	{
		if (mTitle->getChildByTag(BUTTON_TIP_TAG) == nullptr)
		{
			cocostudio::Armature* pTipArm = cocostudio::Armature::create("fla_HongDian");
			mTitle->addChild(pTipArm,1,BUTTON_TIP_TAG);
			pTipArm->setPosition(0,mTitle->getContentSize().height);
			pTipArm->getAnimation()->play("play");
		}	
	}
	else
	{
		mTitle->removeChildByTag(BUTTON_TIP_TAG);
	}
}

void ActItem::setSelect(bool pBool)
{
	mTitle->setSpriteFrame(StringUtils::format("ui/act/act_%d_%d.png", mType, pBool == true ? 1 : 0));
	mActImage->loadTexture(StringUtils::format("ui/act/act_btn_click_%d.png", pBool == true ? 1 : 0), cocos2d::ui::Widget::TextureResType::PLIST);
	if (pBool)
	{
		mTitle->setPositionY(5);
		mActImage->setScale(0.8);
	}
	else
	{
		mTitle->setPositionY(0);
		mActImage->setScale(1.0);
	}
	
	
}
