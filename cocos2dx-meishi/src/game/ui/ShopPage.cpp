//
//#include "ui/UIHelper.h"
//#include "GameConfig.h"
//#include "GamePrecedure.h"
//#include "ShopPage.h"
//#include "GamePrecedure.h"
//
//
//void ShopPage::onMenuItemAction(std::string cbName, cocos2d::Ref* pSender)
//{
//	if (cbName == "onRight")
//	{
//		Layout* pInner = mScrollView->getInnerContainer();
//		float x = -1 * pInner->getPositionX();
//		x += lookSize.width;
//		float w = pInner->getSize().width - lookSize.width;
//		float percent = abs(x / w)*100.0f;
//		mScrollView->scrollToPercentHorizontal(percent,0.5,true);
//	}
//	else if (cbName == "onLeft")
//	{
//		Layout* pInner = mScrollView->getInnerContainer();
//		float x = -1 * pInner->getPositionX();
//		x -= lookSize.width;
//		x = MAX(0, x);
//		float w = pInner->getSize().width - lookSize.width;
//		float percent = abs(x / w)*100.0f;
//		mScrollView->scrollToPercentHorizontal(percent, 0.5, true);
//	}
//}
//
//void ShopPage::onScrollViewCallBack(Ref*, ScrollviewEventType type)
//{
//	if (type == SCROLLVIEW_EVENT_SCROLLING)
//	{
//		Size size = lookSize;
//		Layout* pInner = mScrollView->getInnerContainer();
//		float x = -1 * pInner->getPositionX() ;
//		float w = pInner->getSize().width - size.width;
//		CCLOG("x = %f,w = %f", x, w);
//		float percent = (x / w)*100.0f;
//		if (percent > 100.0f &&percent < 0) return;
//	}
//}
//
//void ShopPage::initCell(Node* pSender, LevelInfo info)
//{
//	if (pSender)
//	{
//		Node* mLevelBg = pSender->getChildByName("mLevelBg");
//		Button* mBuyBtn = dynamic_cast<Button*>(pSender->getChildByName("mBuyBtn"));
//		mBuyBtn->setTag(info.mapId);
//		mBuyBtn->addClickEventListener(CC_CALLBACK_1(ShopPage::onBuyLevel, this));
//
//		Sprite* bg = Sprite::createWithSpriteFrameName(info.levelIcon);
//		mLevelBg->addChild(bg);
//	}
//}
//
//void ShopPage::initUi()
//{
//
//		//mScrollView = static_cast<ui::ScrollView*>(this->getChildByName("mScrollView"));
//		//mScrollView->setTouchEnabled(false);
//		//lookSize = mScrollView->getInnerContainerSize();
//		//Size size = lookSize;
//
//		//mLevelInfoList = sgGamePrecedure()->getLevelInfoList();
//		//int page = mLevelInfoList.size() % 6 == 0 ? mLevelInfoList.size() / 6 : mLevelInfoList.size() / 6 + 1;
//		//float offsetX = size.width / 3;
//		//float offsetY = size.height / 2;
//		//mScrollView->setInnerContainerSize(Size(size.width * page, size.height));
//		//size.width = mScrollView->getInnerContainerSize().width;
//		//mScrollView->addEventListenerScrollView(this, scrollvieweventselector(ShopPage::onScrollViewCallBack));
//		//
//		//for (size_t i = 0; i < page; i++)
//		//{
//
//		//	Vec2 star = Vec2(i * lookSize.width + offsetX / 2, offsetY / 2);
//		//	for (size_t r = 0; r < 2; r++)
//		//	{
//		//		for (size_t c = 0; c < 3; c++)
//		//		{
//		//			int index = i*6 + r * 3 + c;
//		//			if (index < mLevelInfoList.size())
//		//			{
//		//				Node* cell = CSLoader::createNode("LevelButton.csb");
//		//				cell->setAnchorPoint(Vec2(0.5, 0.5));
//
//		//				float x = c * offsetX + star.x;
//		//				float y = size.height - r * offsetY - star.y;
//		//				cell->setPosition(Vec2(x, y));
//		//				cell->setTag(index);
//
//		//				MapLevelInfo::iterator it = mLevelInfoList.find(index+1);
//		//				if (it != mLevelInfoList.end())
//		//				{
//		//					this->initCell(cell, it->second);
//		//					mScrollView->addChild(cell);
//		//				}
//		//			}
//		//			
//		//		}
//		//	}
//		//}
//}
//
//void ShopPage::onBuyLevel(Ref* sender)
//{
//	int tag = dynamic_cast<Node*>(sender)->getTag();
//	sgGamePrecedure()->selectMapWithLevel(tag);
//}
