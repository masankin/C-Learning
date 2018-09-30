#include "WinnerGift.h"
#include "game/winner/WinnerControl.h"
#include "SoundManager.h"
#include "data/TableManager.h"
#include "WinnerControl.h"

WinnerGift::WinnerGift()
{

}

WinnerGift::~WinnerGift()
{

}

bool WinnerGift::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/WinnerGift.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		ret = true;
	} while (0);


	return ret;
}

void WinnerGift::onMenuItemAction(std::string cbName, cocos2d::Ref* sender)
{
	if (cbName == "onBack")
	{
		this->setShow(false);
	}
}

void WinnerGift::onShow()
{
	ActionWindow::onShow();
}

#define  Gift_Num (4)
void WinnerGift::setActId(int actId)
{
	WinnerActivityData* winData = sgWinnerControl()->getWinnerActList(actId+1);
	if (winData == NULL) return;

	int i = 1;			
	auto iteor = winData->SubList.begin();
	for (;iteor != winData->SubList.end(); iteor ++)
	{				
		Node* pNode = mRoot->getChildByName(StringUtils::format("ProjectNode_%d",i));
		i++;

		ui::Text* pText = dynamic_cast<ui::Text*>(pNode->getChildByName("Text_Day"));
		pText->setString(StringUtils::format("%d",iteor->second.Sub_Id));

		const GiftAward* pGiftAward = sgTableManager()->getGiftAward(iteor->second.GiftId);
		if (!pGiftAward) return;

		int iSize = 1;
		int iNum = 0;
		std::string sIconStr ="";

		if (iteor->second.Sub_Id == 7)
		{
			sIconStr = StringUtils::format("ui/winner/Winner_%d_Btn.png", winData->Act_type - 1);

			Sprite* pSpr = dynamic_cast<Sprite*>(pNode->getChildByName("Sprite_1"));
			pSpr->setSpriteFrame(sIconStr);
			ui::Text* pNum = dynamic_cast<ui::Text*>(pNode->getChildByName("Text_1"));

			pNum->setString(sgTextHelper(TEXT_WINNER_FAIL + winData->Act_type));
			pSpr->setScale(0.24);

			iSize = 2;
		}
		else
		{
			pNode->getChildByName("Sprite_1")->setVisible(true);
			pNode->getChildByName("Text_1")->setVisible(true);
		}
		
		if (pGiftAward->mGiftData.size() == 0)
		{
			return;
		}
		for (int i = 0; i < pGiftAward->mGiftData.size(); i++)
		{
			GiftInfo lGiftInfo = pGiftAward->mGiftData.at(i);
			const GoodsInfo* lGoodsInfo = sgTableManager()->getGoodsInfo(lGiftInfo.mType, lGiftInfo.mId);
			if (!lGoodsInfo)
			{
				break;
			}
			iNum = lGiftInfo.mNumber;
			sIconStr = lGoodsInfo->mIcon;

			Sprite* pSpr = dynamic_cast<Sprite*>(pNode->getChildByName(StringUtils::format("Sprite_%d", iSize)));
			pSpr->setSpriteFrame(sIconStr);

			ui::Text* pNum = dynamic_cast<ui::Text*>(pNode->getChildByName(StringUtils::format("Text_%d", iSize)));
			pNum->setString(StringUtils::format("x%d", iNum));
			Size lSize = pSpr->getContentSize();
			pSpr->setScale(30.0/lSize.width,30.0/lSize.height);
			iSize++;
		}
		for (int j = iSize; j < Gift_Num; j++)
		{
			pNode->getChildByName(StringUtils::format("Sprite_%d",j))->setVisible(false);
			pNode->getChildByName(StringUtils::format("Text_%d",j))->setVisible(false);
		}
	}
}
