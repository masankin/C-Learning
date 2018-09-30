#include "ShopLayer.h"
#include "ui/system/Window.h"

using namespace cocos2d;
using namespace cocostudio;

ShopLayer::ShopLayer(void) :mIsShow(false)
{
}


ShopLayer::~ShopLayer(void)
{
	for (auto it = mContentVec.begin(); it != mContentVec.end(); it++)
	{
		CC_SAFE_RELEASE(*it);
	}
}

bool ShopLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Node* pLayer= CSLoader::createNode("ui/ShopLayer.csb");
	for (int i = 4; i<=6; i++)
	{
		Node* lprojectNode =  dynamic_cast<Node*>(pLayer->getChildByName(__String::createWithFormat("ProjectNode_%d",i)->getCString()));
		CC_SAFE_RETAIN(lprojectNode);
		mContentVec.push_back(lprojectNode);
	}

	addChild(pLayer);

	return true;
}

void ShopLayer::setBuyButton(std::vector<Button*> butVec)
{
	for (auto it = butVec.begin(); it!=butVec.end(); it++)
	{
		BuyButtonInfo pInfo;
		pInfo.mPrice = dynamic_cast<Text*>((*it)->getChildByName("price_txt"));
		pInfo.mIcon = dynamic_cast<Sprite*>((*it)->getChildByName("icon_image"));
		mBuyBtnInfoVec.push_back(pInfo);
	}
}

void ShopLayer::onShow()
{
	mIsShow = true;
	mShowLeft = 0;
	for (int i = 0; i<3; i++)
	{
		mBuyBtnInfoVec[i].mIcon->getParent()->setVisible(true);
	}
	if (getShowItemSize()<3)
	{
		for (int i = 2; i>=0; i--)
		{
			if (i>getShowItemSize()-1)
			{
				mBuyBtnInfoVec[i].mIcon->getParent()->setVisible(false);
			}
			
		}
	}

	updateContent();
}

void ShopLayer::onClose()
{
	mIsShow = false;
}

void ShopLayer::moveLeft()
{
	int i = 3;
	mShowLeft -= 3;
	updateContent();
}

void ShopLayer::moveRight()
{
	int i = 3;
	mShowLeft += 3;
	updateContent();
}

void ShopLayer::updateContent()
{
}
