#include "WinnerFight.h"
#include "game/ui/PageFunction.h"
#include "game/data/Tables.h"
#include "game/data/TableManager.h"
#include "game/ui/dialogs/GenerlDialog.h"
#include "net/UserObj.h"
#include "game/winner/WinnerControl.h"
#include "SoundManager.h"
#include "WinnerGift.h"
#include "EnvSet.h"

void WinnerFight::onMenuItemAction(cocos2d::Ref* sender)
{
	Widget* pWidgt = dynamic_cast<Widget*>(sender);
	std::string cName = pWidgt->getCallbackName();

	 if (cName == "onAgain" || cName == "onGet")
	{
		if (!sgLoginProxy()->checkLogin())
		{
			return;
		}
	}
	if (cName == "onAgain")
	{
		sgWinnerControl()->getWinnerDialogPtr()->setShow(false);
		sgPageFunction()->gotoFunction(MSG_FUC_WIN_PVP);
	}
	else if (cName == "onGet")
	{
		WinnerSubActivity* winData = sgWinnerControl()->getWinnerSubID(WINNER_FIGHT + 1, pWidgt->getTag());
		if (winData == NULL) return;

		if (!winData->Completed)
		{
			if (pWidgt->getTag() == 7)
			{
				WinnerGift* pDdialog = dynamic_cast<WinnerGift*>(Window::Create("WinnerGift.csb"));
				pDdialog->setActId(WINNER_FIGHT);
				pDdialog->setShow(true);
			}
			else
			{
				TipsManager::showTextTip(sgTextHelper(TEXT_WINNER_NOCOMPLENT));
			}
			return;
		}

		sgWinnerControl()->getWinnerProxy()->sendActGetRqst(WINNER_FIGHT + 1, pWidgt->getTag());
	}
}

bool WinnerFight::init()
{
	bool ret = false;
	do
	{				
		mRoot = CSLoader::createNode("ui/WinnerFight.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);

		this->addChild(mRoot);

		mDayNum_1 = dynamic_cast<Text*>(mRoot->getChildByName("DayNum_1"));
		mDayNum_1->setString("5");

		mWinGoal = dynamic_cast<Text*>(mRoot->getChildByName("WinNum"));
		mWinGoal->setString("5/20");

		mButton_4 = dynamic_cast<Button*>(mRoot->getChildByName("AgainBtn"));
		mButton_4->addClickEventListener(CC_CALLBACK_1(WinnerFight::onMenuItemAction, this));

		mButton_4 = dynamic_cast<Button*>(mRoot->getChildByName("Button_4"));
		mButton_4->setBright(false);

		Node* pNode = mRoot->getChildByName("ProjectNode_1");
		for (int i = 0; i < 7; i++)
		{
			mImageBtn[i] = dynamic_cast<ui::ImageView*>(pNode->getChildByName(StringUtils::format("Image_%d",i+1))); 
			mImageBtn[i]->addClickEventListener(CC_CALLBACK_1(WinnerFight::onMenuItemAction, this));


			cocostudio::Armature* pBtnJewel = cocostudio::Armature::create("fla_baoxiangDH");

			mImageBtn[i]->addChild(pBtnJewel,0,WINNER_FIGHT);		
			pBtnJewel->setPosition(Vec2(mImageBtn[i]->getContentSize().width/2,mImageBtn[i]->getContentSize().height/2));
			pBtnJewel->getAnimation()->play("born");
			pBtnJewel->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(WinnerFight::jewelCallback));

			cocostudio::Armature* pArm = pBtnJewel->getBone("guge_chengjiubxSS")->getChildArmature();
			pArm->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);
			pArm->getAnimation()->play("born");

			if(i < 6){
				mImageBtn[i]->setTouchEnabled(false);
				mImageBtn[i]->getChildByName("Sprite_1")->setZOrder(1);
				mImageBtn[i]->getChildByName("Sprite_2")->setZOrder(1);
				pBtnJewel->setScale(0.6);
			}
		}
		
		ret = true;
	} while (0);

	return ret;
}

WinnerFight::WinnerFight()
{
	mDayNum_1 = NULL;
	mWinGoal = NULL;
	mButton_4 = NULL;
}

WinnerFight::~WinnerFight()
{
}

void WinnerFight::update(float delta)
{

	bool bfound = sgUserObj()->isHaveBadge(TitleType::FIGHTCRAZY);
	mButton_4->setBright(bfound);

	WinnerActivityData* winData = sgWinnerControl()->getWinnerActList(WINNER_FIGHT+1);
	if (winData == NULL) return;
	mDayNum_1->setString(StringUtils::format("%d",winData->DaysRemain));

	int i = 0;
	auto iteor = winData->SubList.begin();
	for (;iteor != winData->SubList.end(); iteor ++)
	{				
		cocostudio::Armature* jewelArm = dynamic_cast<cocostudio::Armature*>(mImageBtn[i]->getChildByTag(WINNER_FIGHT));
		if (iteor->second.Sub_Id == winData->Cur_SubId)
		{
			mWinGoal->setString(StringUtils::format("%d/%d",iteor->second.Value,iteor->second.Request));
			if (iteor->second.Completed)
			{
				if (iteor->second.Submid)
				{
					mImageBtn[i]->setTouchEnabled(false);
					if (delta > 0)
					{
						jewelArm->getAnimation()->play("stand");
					}
					else
					{
						jewelArm->getAnimation()->play("null");
					}												
				}
				else
				{
					mImageBtn[i]->setTouchEnabled(true);
					jewelArm->getAnimation()->play("born");
				}
			}
			else
			{
				mImageBtn[i]->setTouchEnabled(true);
				jewelArm->getAnimation()->play("standc");
			}
		}
		else if(iteor->second.Sub_Id < winData->Cur_SubId)
		{
			mImageBtn[i]->setTouchEnabled(false);
			if (iteor->second.Completed)
			{
				jewelArm->getAnimation()->play("null");
			}
			else
			{
				jewelArm->getAnimation()->play("gray");
				mImageBtn[i]->getChildByName("Sprite_2")->setVisible(true);
			}
		}
		else
		{
			mImageBtn[i]->setTouchEnabled(false);
			jewelArm->getAnimation()->play("standb");
		}
		if (i < winData->SubList.size() - 1)
		{
			if (iteor->second.Completed && iteor->second.Submid)
			{
				mImageBtn[i]->getChildByName("Sprite_1")->setVisible(true);
				mImageBtn[i]->getChildByName("Sprite_2")->setVisible(false);
			}
			else
			{
				mImageBtn[i]->getChildByName("Sprite_1")->setVisible(false);
			}
		}
		else
		{
			mImageBtn[i]->setTouchEnabled(true);
		}
		mImageBtn[i]->setTag(iteor->second.Sub_Id);
		i++;
	}		
}

void WinnerFight::jewelCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (type == cocostudio::COMPLETE && aniName == "stand")
	{
		pAr->getAnimation()->play("null");
	}
}
