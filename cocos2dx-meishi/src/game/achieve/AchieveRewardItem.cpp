#include "AchieveRewardItem.h"
#include "AchieveControl.h"
#include "data/TableManager.h"
#include "login/LoginProxy.h"
#include "ui/PageFunction.h"
#include "MessageManager.h"
#include "core/SoundManager.h"
#include "CCLuaEngine.h"

using namespace cocostudio;
USING_NS_CC;
using namespace ui;
AchieveRewardItem::AchieveRewardItem(AchieveType pType)
	:m_AchieveType(pType),mId(1),m_isTouch(false)
{
}


AchieveRewardItem::~AchieveRewardItem(void)
{
}

AchieveRewardItem* AchieveRewardItem::create(AchieveType pType)
{
	AchieveRewardItem* pRewardItem = new AchieveRewardItem(pType);
	if (pRewardItem && pRewardItem->init())
	{
		pRewardItem->autorelease();
	}else
	{
		CC_SAFE_DELETE(pRewardItem);
	}
	return pRewardItem;
}

bool AchieveRewardItem::init()
{
	if (!Node::init())
	{
		return false;
	}

	Node* pNode = CSLoader::createNode("ui/AchieveItem.csb");
	m_describeLab = dynamic_cast<Text*>(pNode->getChildByName("Text_1_0"));
	m_tasknameLab = dynamic_cast<Text*>(pNode->getChildByName("Text_1"));
	m_progressLoadBar = dynamic_cast<LoadingBar*>(pNode->getChildByName("LoadingBar_1"));
	m_progressText = dynamic_cast<Text*>(pNode->getChildByName("Text_15"));
	m_getBtn = dynamic_cast<Button*>(pNode->getChildByName("Button_7"));
	m_complentplayer = dynamic_cast<Text*>(pNode->getChildByName("Text_1_0_0"));
	mPointBtn = dynamic_cast<ui::Button*>(m_getBtn->getChildByName("Button_3"));
	m_statusText = dynamic_cast<Text*>(m_getBtn->getChildByName("Text_30_0"));
	mPointText = dynamic_cast<Text*>(m_getBtn->getChildByName("Text_30_0_0"));
	mProgressIamge = dynamic_cast<Sprite*>(pNode->getChildByName("Image_6"));

	mPointBtn->setTouchEnabled(false);

	m_getBtn->setSwallowTouches(false);
	m_getBtn->addTouchEventListener(this,toucheventselector(AchieveRewardItem::onTouchEvent));
	addChild(pNode);
	return true;
}

void AchieveRewardItem::onTouchEvent(cocos2d::Ref* ,cocos2d::ui::TouchEventType touchEvent)
{
	if (touchEvent == TOUCH_EVENT_ENDED)
	{
		m_isTouch = true;
	}else
	{
		m_isTouch = false;
	}
}

void AchieveRewardItem::onTouch()
{
	if (!sgAchieveControl()->isCanTouch())
	{
		return;
	}

	sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);

	if (m_isTouch)
	{
		m_isTouch = false;
		if (mRewardStatus == REWARD_BTN_UNREACHABLE)
		{
			if (mGoToLevel == 0)
			{
				sgPageFunction()->gotoFunction(MSG_FUC_WIN_PVP);
				sgAchieveControl()->closeAchievePanel();

				return;
			}

			int pMaxOpenId = sgUserObj()->getMaxOpenGateID();
			/*int pMaxOpenId = sgUserObj()->getMaxOpenCheckPoint();
			const CheckPointTable::CheckPointList pCheckPointList = sgTableManager()->getCheckPoint();
			for (auto it = pCheckPointList.begin(); it!= pCheckPointList.end(); it++)
			{
				if ((*it).second->id % 10 == 0 && (*it).second->id > pMaxOpenId && sgUserObj()->getCheckPointState((*it).second->id) == E_CHECKPOINTSTATE_OPEN)
				{
					pMaxOpenId = (*it).second->id;
					break;
				}
			}*/

			if (mGoToLevel == 1)
			{
				sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);
			}else if (mGoToLevel == 2)
			{
				sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);
				
			}else
			{
				int pGoto = mGoToLevel/1000;

				if(pMaxOpenId < mGoToLevel)
					mGoToLevel = pMaxOpenId;

				/*{
					sgPageFunction()->gotoFunction(MSG_FUC_MINI_LEVEL);

				}
				sgMessageManager()->sendMessage(EMT_ACHIEVE_GOTOLEVEL,
					__String::createWithFormat("%d",mGoToLevel)->getCString());
				*/
				const char* pScript = String::createWithFormat("sgBthControl():gotoMission(%d)", mGoToLevel)->getCString();
				LuaEngine::getInstance()->executeString(pScript);
			}
			sgAchieveControl()->closeAchievePanel();

		}else
		{
			if (sgLoginProxy()->checkLogin(true))
			{
				sgAchieveControl()->setRemCell(dynamic_cast<TableViewCell*>(getParent()) );
				sgAchieveControl()->sendGetAchievementAward(mId);
			}
		}
	}
}

void AchieveRewardItem::updateInfo(const AchievementAward* pAward)
{
	int lmin = 0;
	int lmax = 0;
	int gotoLevel = 0;
	switch (pAward->category)
	{
	case A_Point:
	{
		lmax = 1;
		//if (pAward->point_Type == 1 && sgUserObj()->getCheckPointState(pAward->pointId) == E_CHECKPOINTSTATE_PASS)
		if (pAward->point_Type == 1 )
		{
			RewardItemStatus status = (RewardItemStatus)pAward->mstatus;
			if (status >= REWARD_BTN_GETTED)
			{
				lmin = 1;
			}
		}
		else if (pAward->point_Type == 2)
		{
			lmin = pAward->mProgress;
			lmax = pAward->pvePropCount;
		}
		gotoLevel = pAward->pointId;
	}
	break;
	case A_Star:
	{
		lmin = sgUserObj()->getTotalStars();
		lmax = pAward->getStar;
		gotoLevel = pAward->pointType;
	}
	break;
	case A_AddUp:
	{
		lmin = pAward->mProgress;
		lmax = pAward->addTotalProgress;
	}
	break;
	default:
		break;
	}

	updateInfo(pAward, lmin, lmax, gotoLevel);
}

void AchieveRewardItem::updateInfo(const AchievementAward* pAward, int progressNow, int progressAll, int gotoLevel)
{
	int getPlayeres = pAward->mGetPlayer + pAward->mGetPlayerBegain;
	RewardItemStatus status = (RewardItemStatus)pAward->mstatus;
	mId = pAward->id;
	m_describeLab->setString(pAward->describe);
	m_tasknameLab->setString(pAward->taskName);
	int pShowNow = progressNow > progressAll? progressAll: progressNow;
	m_progressText->setString(__String::createWithFormat("%d/%d",pShowNow,progressAll)->getCString());
	float ploading = (progressNow*1.0)/(progressAll*1.0)>=1?1.0:(progressNow*1.0)/(progressAll*1.0);
	m_progressLoadBar->setPercent(ploading*100);
	if (getPlayeres >=0)
	{
		m_complentplayer->setText(__String::createWithFormat(sgTextHelper(TEXT_ACHIEVE_COMPLENT_PLAYER).c_str(),getPlayeres)->getCString());
	}
	mProgressIamge->setSpriteFrame(StringUtils::format("ui/Achieve/Achieve_level_%d.png",pAward->category));
	m_progressLoadBar->loadTexture(StringUtils::format("ui/general/Ach_Type%d_%d.png",pAward->category,pAward->mIcon),ui::Widget::TextureResType::PLIST);
	m_progressLoadBar->setContentSize(Size(mProgressIamge->getContentSize().height,mProgressIamge->getContentSize().width));

	/*
	if (status !=REWARD_BTN_GETTED )
	{
		if (progressNow >= progressAll)
		{
			status = REWARD_BTN_CANGET;
		}else
			status = REWARD_BTN_UNREACHABLE;
	}else
	{
		if (progressNow< progressAll)
		{
			status = REWARD_BTN_UNREACHABLE;
		}
	}*/

	mPointText->setString(__String::createWithFormat("%d",pAward->Award)->getCString());

	mPointBtn->setPositionX(mPointText->getPositionX()+mPointText->getContentSize().width/2 + 5+mPointBtn->getContentSize().width/2);
	switch (status)
	{
	case REWARD_BTN_UNREACHABLE:
		m_statusText->setString(sgTextHelper(TEXT_ACHIEVE_CANNOTGET));
		m_statusText->enableOutline(Color4B(0, 106, 170, 255), 2);
		mPointText->enableOutline(Color4B(0, 106, 170, 255), 2);
		m_getBtn->setTouchEnabled(true);
		m_getBtn->setBright(true);
		m_getBtn->loadTextureNormal("ui/general/general_bottom-blue-small.png",TextureResType::PLIST);
		mPointBtn->setBright(true);
		break;
	case REWARD_BTN_GETTED:
		m_statusText->setString(sgTextHelper(TEXT_ACHIEVE_GETED));
		m_getBtn->setTouchEnabled(false);
		m_getBtn->setBright(false);
		mPointBtn->setBright(false);
		break;
	case REWARD_BTN_CANGET:
		m_statusText->setString(sgTextHelper(TEXT_ACHIEVE_CANGET));
		m_statusText->enableOutline(Color4B(11, 119, 8,255),2);
		mPointText->enableOutline(Color4B(11, 119, 8, 255), 2);
		m_getBtn->setTouchEnabled(true);
		m_getBtn->setBright(true);
		m_getBtn->loadTextureNormal("ui/general/general_bottom-gre.png",TextureResType::PLIST);
		mPointBtn->setBright(true);
		break;
	case REWARD_BTN_MAX:
		break;
	default:
		break;
	}
	m_getBtn->setSwallowTouches(false);//important

	mRewardStatus = status;
	mGoToLevel = gotoLevel;
}
