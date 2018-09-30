#include "PvpRoom.h"
#include "game/data/TableManager.h"
#include "game/pvp/PvpRoomControl.h"
#include "GenerlDialog.h"
#include "TimeCalculator.h"
#include "game/chat/InfoPanel.h"
#include "utils/Comm.h"
#include "game/pvp/PvpControl.h"
#include "core/SoundManager.h"
#include "game/ui/PageFunction.h"
#include "utils/HeadIconUtils.h"

PvpRoom::PvpRoom() :
m_iTime2Start(DEFAULT_TIME_TO_START)
{
}

PvpRoom::~PvpRoom()
{
	sgPvpRoomControl()->setPvpRoom(NULL);
}

bool PvpRoomItem::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PVPRoom_node.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);
		mPlayer = mRoot->getChildByName("Node_player");
		mNone = mRoot->getChildByName("Node_none");
		mClose = mRoot->getChildByName("Node_close");

		mName = dynamic_cast<ui::Text*> (mPlayer->getChildByName("Text_name"));
#ifdef MGAME_PLATFORM_MPLUS
		if (mName) {
			mName->setFontName(DEFAULT_FONT);
		}
#endif
		mRank = dynamic_cast<ui::Text*> (mPlayer->getChildByName("Text_rank"));
		mWin = dynamic_cast<ui::Text*> (mPlayer->getChildByName("Text_win"));
		mCrown = dynamic_cast<Sprite*> (mPlayer->getChildByName("Sprite_crown"));
		mTitle = dynamic_cast<Sprite*> (mPlayer->getChildByName("Sprite_playerTitle")); 
		
		mIcon = dynamic_cast<Button*> (mPlayer->getChildByName("Button_icon"));
		mIcon->addClickEventListener(CC_CALLBACK_1(PvpRoomItem::detailCallback, this));
		mIconBg = dynamic_cast<Sprite*> (mPlayer->getChildByName("Sprite_iconBg"));
		mkillPlayer = dynamic_cast<ui::Button*>(mPlayer->getChildByName("btn_killPlayer"));
		mkillPlayer->addClickEventListener(CC_CALLBACK_1(PvpRoomItem::killoffCallBack, this));

		
		mVipNode = dynamic_cast<Node*>(mPlayer->getChildByName("node_vip"));

		Node *_chatNode = dynamic_cast<Node*>(mPlayer->getChildByName("Node_chat"));
		mChat = dynamic_cast<ui::Text*>(_chatNode->getChildByName("Text_chat"));

	
		ret = true;
	} while (0);

	return ret;
}

void PvpRoomItem::detailCallback(cocos2d::Ref* sender)
{
	CCLOG("[trace]PvpRoomItem::detailCallback,detailFiend");
	int tag = dynamic_cast<Button*>(sender)->getTag();
	FriendSystem::PlayerData* playerData =  &sgPvpRoomControl()->mPlayerList[tag];

	sgUserObj()->setCurInfoUin(playerData->m_uId);
	sgUserObj()->setCurInfoType(0);
	sgPageFunction()->gotoFunction(MSG_FUC_INFO);
}
void PvpRoomItem::killoffCallBack(cocos2d::Ref* sender)
{
	CCLOG("[trace] PvpRoomItem::killoffCallBack ,killoffCallBack");

	//根据索引发包给服务器踢人 
	//房间进入倒计时开始之后，房主无法踢人
	//if (sgTimeCalculator()->hasKey("cp_start"))
	//	sgPvpRoomControl()->showTip(sgTextHelper(TEXT_PVP_ROOM_9));
	//else
	//{
		int uin = dynamic_cast<Button*>(sender)->getTag();
		sgPvpRoomControl()->sendKilloffPlayer(uin);
	//}

}



void PvpRoom::onMenuItemAction(std::string name, cocos2d::Ref* sender)
{
	if (name =="onBack")
	{
		// 退出房间
		CCLOG("[trace] PvpRoom::onMenuItemAction ,onBack");
		sgPvpRoomControl()->sendLeaveRoomRqst();
		sgPvpRoomControl()->deletePlayer(sgUserObj()->getUin());
	}
	else if (name == "onChangeTwo")
	{
		////切换双人模式
		sgPvpRoomControl()->semdChangeRoomModeRqst(ROOMMODE_TWO);
	}
	else if (name == "onChangeThree")
	{
		//切换三人模式
		sgPvpRoomControl()->semdChangeRoomModeRqst(ROOMMODE_THREE);
	}
	else if (name == "onChangeFour")
	{
		//切换四人模式
		sgPvpRoomControl()->semdChangeRoomModeRqst(ROOMMODE_FOUR);
	}
	else if (name == "onStart")
	{
		//开始游戏
		if (sgPvpRoomControl()->getPlayerNum() == 1)
			this->showTextTip(sgTextHelper(TEXT_PVP_ROOM_11));
		else
		{
			CCLOG("[trace] PvpRoom::onMenuItemAction ,onStart");
			sgPvpRoomControl()->sendBeginFightRequest();
		}
		
	}

	else if (name == "onQuickChat")
	{
		//快捷发言
		CCLOG("[trace] PvpRoom::onMenuItemAction ,onQuickChat");
		mQuicChat->setVisible(mQuicChat->isVisible()?false:true);
	}else if (name == "onChat")
	{
		//发言
		CCLOG("[trace] PvpRoom::onMenuItemAction ,onChat");
		mQuicChat->setVisible(false);
		Button * _btn = (Button*)sender;
		sgPvpRoomControl()->sendChatRequest(_btn->getTag());
	}
	else if (name == "onChallenge")
	{
		//全服挑战
		CCLOG("[trace] PvpRoom::onMenuItemAction ,onChallenge");
		sgPvpRoomControl()->sendChallengeRequest();
		sgTimeCalculator()->createTimeCalcultor("cp_challenge", ChallengeTime);
	}
	else if (name == "onInvite")
	{
		//邀请玩家
		CCLOG("[trace] PvpRoom::onMenuItemAction ,onInvite");

		mInvitePage = Window::Create("PVPInvite.csb");
		mInvitePage->setShow(true);
	}
}

bool PvpRoom::init()
{

	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PVPRoom.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		sgPvpRoomControl()->setPvpRoom(this);
		
		//获取四个玩家控件
		for (int i = 0; i < 4; i++)
		{
			Node * temp = mRoot->getChildByName(StringUtils::format("Node_player_%d", i + 1).c_str());
			CC_BREAK_IF(!temp);
			PvpRoomItem * item = PvpRoomItem::create();
			//第一个组件设置房主标识
			if (i == 0)
			{
				item->mCrown->setVisible(true);
				item->mIconBg->setSpriteFrame("ui/PVPRoom/icon_bg_host.png");
			}
				
			temp->addChild((Node*)item);
			m_vPlayer.push_back(item);
		}

		//开始按钮
		mBtnStart = dynamic_cast<Button*>(mRoot->getChildByName("Button_start"));

		//获取模式按钮
		mBtn2 = dynamic_cast<Button *>(mRoot->getChildByName("Button_two"));
		mBtn3 = dynamic_cast<Button *>(mRoot->getChildByName("Button_three"));
		mBtn4 = dynamic_cast<Button *>(mRoot->getChildByName("Button_four"));

		mSpriteBtnBg = dynamic_cast<Sprite*> (mRoot->getChildByName("Sprite_ModeBtnBg"));
		
		//挑战全服
		mChallenge = dynamic_cast<Button*>(mRoot->getChildByName("Button_Chall"));
		mChallengeTime = dynamic_cast<Text*>(mRoot->getChildByName("textTime"));

		Node*  timeNode = dynamic_cast<Node *>(mRoot->getChildByName("Node_timeout"));
		mTime = dynamic_cast<Text*>(timeNode->getChildByName("Text_timeout"));
		
		//初始化快捷聊天内容
		mQuicChat = dynamic_cast<Node*>(mRoot->getChildByName("Node_quickChat"));
		mQuicChat->setVisible(false);
		for (int  i = 0; i < 4; i++)
		{
			//暂定4条内容
			Button *_btn = dynamic_cast<Button*> (mQuicChat->getChildByName(StringUtils::format("Button_%d", i + 1).c_str()));
			_btn->setTag(i + 1);
			Text *  text = dynamic_cast<Text*>(_btn->getChildByName("Text"));

			text->setText(sgTextHelper(5002+ i));
		}
		
		
	ret = true;
	} while (0);

	return ret;
}

void PvpRoom::onShow()
{
	sgSoundManager()->playMusicBg(MUSIC_UI_PVPROOM);
	CCLOG("[trace]PvpRoom::onShow, onShow");
	
	//开启Update
	this->scheduleUpdate();
	mInvitePage = NULL;
	mTime->getParent()->setVisible(false);
	mQuicChat->setVisible(false);

	for (int i = 0; i < 4;i ++)
		m_vPlayer[i]->mChat->getParent()->setVisible(false);

	//房主才显示开始按钮
	mBtnStart->setVisible(sgPvpRoomControl()->getIsHourse());
	//mChallenge->setVisible(false);	//微信版本屏蔽
	mChallenge->setVisible(sgPvpRoomControl()->getIsHourse() ? true : false);
	if (mChallenge->isVisible())
	{
		mChallenge->setTouchEnabled(true);
		mChallenge->setBright(true);
	}
	changeMode();
	showPlayer();
}

void PvpRoom::updateUI()
{

}

void PvpRoom::update(float dt)
{
	if (sgTimeCalculator()->hasKey("cp_start"))
	{
		//显示倒计时
		mTime->getParent()->setVisible(true);
		float _left = sgTimeCalculator()->getTimeLeft("cp_start");
		mTime->setText(StringUtils::format("%d",int(_left)));
		if (_left <= 0)
		{
			mTime->getParent()->setVisible(false);
			sgTimeCalculator()->removeTimeCalcultor("cp_start");
		}
	}
	if (sgTimeCalculator()->hasKey("cp_challenge"))
	{
		//全服挑战按钮禁用
		mChallenge->setTouchEnabled(false);
		mChallenge->setBright(false);
		mChallengeTime->setVisible(true);
		float _left = sgTimeCalculator()->getTimeLeft("cp_challenge");
		mChallengeTime->setText(StringUtils::format(sgTextHelper(TEXT_PVP_ROOM_13).c_str(), (int)_left));
		if (_left <= 0)
		{
			mChallenge->setTouchEnabled(true);
			mChallenge->setBright(true);
			mChallengeTime->setVisible(false);
			sgTimeCalculator()->removeTimeCalcultor("cp_challenge");
		}
	}

}

void PvpRoom::showPlayer()
{

	
	PvpRoomItem* pRoomItem = NULL;
	
	for (int i = 0; i < 4; i++)
	{
		FriendSystem::PlayerData stPlayerInfo;
		pRoomItem = m_vPlayer[i];
		if (i< sgPvpRoomControl()->mPlayerList.size())
			stPlayerInfo = sgPvpRoomControl()->mPlayerList[i];

		//先默认显示玩家状态 ,后续根据条件改变状态
		pRoomItem->mPlayer->setVisible(true);
		pRoomItem->mNone->setVisible(false);

		//如果是房主，如果有玩家，需要显示2，3，4的关闭按钮
		if (sgPvpRoomControl()->getIsHourse() && i > 0)
		{
			pRoomItem->mkillPlayer->setVisible(true);
			pRoomItem->mCrown->setVisible(false);
			pRoomItem->mkillPlayer->setTag(stPlayerInfo.m_uId);
		}		
		else
			pRoomItem->mkillPlayer->setVisible(false);

		if (i >= sgPvpRoomControl()->getPlayerNum())
		{
			pRoomItem->mPlayer->setVisible(false);
			pRoomItem->mNone->setVisible(true);
			pRoomItem->mClose->setVisible(false);
		}
		if (i >= sgPvpRoomControl()->getRoomMode())
		{
			pRoomItem->mPlayer->setVisible(false);
			pRoomItem->mNone->setVisible(false);
			pRoomItem->mClose->setVisible(true);
		}

		if (pRoomItem->mPlayer->isVisible())
		{
			pRoomItem->mName->setText(stPlayerInfo.m_sName);
			sgHeadIconUtils()->changeIcon(pRoomItem->mIcon, stPlayerInfo.m_iIcon);

			/*Size size = SpriteFrameCache::getInstance()->getSpriteFrameByName(icon.c_str())->getOriginalSize();
			pRoomItem->mIcon->setContentSize(size);
			pRoomItem->mIcon->loadTextureNormal(icon.c_str(), TextureResType::PLIST);*/
			pRoomItem->mIcon->setTag(i);
			
			if (stPlayerInfo.m_uId == sgUserObj()->getUin())
				pRoomItem->mIcon->setEnabled(false);
			else
				pRoomItem->mIcon->setEnabled(true);

			int group = TEXT_RANK_GROUP_1 - 1 + stPlayerInfo.m_iGroup;
			CCLOG("[trace]PvpRoom::showPlayer group:%d  stPlayerInfo:group:%d", group, stPlayerInfo.m_iGroup);
			if (stPlayerInfo.m_iGroup == 0)
			{
				group = TEXT_PVP_NO_RANK;
				pRoomItem->mRank->setText(sgTextHelper(group).c_str());
			}
			else
			{
				pRoomItem->mRank->setText(StringUtils::format(sgTextHelper(group).c_str(), stPlayerInfo.m_iOrder).c_str());
			}
			pRoomItem->mWin->setText(StringUtils::format("%.2f%%", stPlayerInfo.m_fWinRate));
			//if ( fgame::CSSycPlayerInfoResp_TitleType_IsValid(stPlayerInfo.m_iTitle))
			/*if (stPlayerInfo.m_iTitle > TitleType::INVALID  && stPlayerInfo.m_iTitle < TitleType::MAX)
			{
				pRoomItem->mTitle->setSpriteFrame(StringUtils::format("ui/general/PlayerTitle_%d.png", stPlayerInfo.m_iTitle));
				pRoomItem->mTitle->setVisible(true);
			}
			else
			{
				pRoomItem->mTitle->setVisible(false);
			}*/

			auto pData = sgTableManager()->getBadgeData(stPlayerInfo.m_iTitle);
			if (pData && pData->getActive() != 0)
			{
				pRoomItem->mTitle->setSpriteFrame(GetPlayerBadgeIconNameById(pData->getId(), true));
				pRoomItem->mTitle->setVisible(true);
				pRoomItem->mTitle->setScale(0.6f);
			}
			else
			{
				pRoomItem->mTitle->setVisible(false);
			}
#ifdef MGAME_PLATFORM_MPLUS
			//vip
			pRoomItem->mVipNode->removeAllChildren();
			if (stPlayerInfo.m_iVipLevel > 0 && stPlayerInfo.m_iVipLevel <= 3)
			{
				pRoomItem->mVipNode->setVisible(true);
				Sprite * left = Sprite::createWithSpriteFrameName(StringUtils::format("ui/general/general_vip%d.png", stPlayerInfo.m_iVipLevel));
				Sprite * right = Sprite::createWithSpriteFrameName(StringUtils::format("ui/general/general_vip%d.png", stPlayerInfo.m_iVipLevel));
				left->setPosition(0, 0);
				right->setFlippedX(true);
				right->setPosition(pRoomItem->mName->getContentSize().width+40,0);
				pRoomItem->mVipNode-> addChild(left);
				pRoomItem->mVipNode->addChild(right);
			}
			else
			{
				pRoomItem->mVipNode->setVisible(false);
			}
#else
			pRoomItem->mVipNode->setVisible(false);
#endif
		}
	}
}


void PvpRoom::showChat(int uin, int id )
{
	int index = 0;
	for (auto it = sgPvpRoomControl()->mPlayerList.begin(); it != sgPvpRoomControl()->mPlayerList.end(); it++)
	{
		if (uin == it->m_uId)
		{
			Button *_btn = dynamic_cast<Button*> (mQuicChat->getChildByName(StringUtils::format("Button_%d", id).c_str()));
			Text *_text = dynamic_cast<Text*>(_btn->getChildByName("Text"));
			m_vPlayer[index]->mChat->setText(_text->getString());
			m_vPlayer[index]->mChat->getParent()->setVisible(true);
			auto delayTime = CCDelayTime::create(3.0);
			auto funcn = CallFuncN::create(CC_CALLBACK_1(PvpRoom::removeChat, this));
			auto sequence = Sequence::create(delayTime, funcn, nullptr);
			m_vPlayer[index]->mChat->getParent()->setVisible(true);
			m_vPlayer[index]->mChat->getParent()->runAction(sequence);
		}
		index++;
	}
	
}

void PvpRoom::removeChat(Node *pSender)
{
	pSender->setVisible(false);
}

void PvpRoom::onClose()
{
	sgSoundManager()->playLastMusicBg();

	CCLOG("[trace] PvpRoom::onClose");
	this->unscheduleUpdate();
	if (sgTimeCalculator()->hasKey("cp_start"))
		sgTimeCalculator()->removeTimeCalcultor("cp_start");
	if (sgTimeCalculator()->hasKey("cp_challenge"))
		sgTimeCalculator()->removeTimeCalcultor("cp_challenge");

	mQuicChat->setVisible(false);
	mChallengeTime->setVisible(false);

	mInvitePage = Window::getWindow("PVPInvite.csb");
	if (mInvitePage && mInvitePage->isShow())
		mInvitePage->setShow(false);

	sgPvpRoomControl()->setIsHourse(false);
	sgPvpRoomControl()->setRoomId(0);
	sgPvpRoomControl()->setPlayerNum(0);
	Window::onClose();
}

void PvpRoom::changeMode()
{
	int mode = sgPvpRoomControl()->getRoomMode();
	bool bTwo = mode == ROOMMODE_TWO;
	bool bThree = mode == ROOMMODE_THREE;
	bool bFour = mode == ROOMMODE_FOUR;

	mBtn2->setBright(bTwo);
	mBtn2->setTouchEnabled(!bTwo);
	mBtn3->setBright(bThree);
	mBtn3->setTouchEnabled(!bThree);
	mBtn4->setBright(bFour);
	mBtn4->setTouchEnabled(!bFour);

	Point p = Point::ZERO;
	if (bTwo)
		p = mBtn2->getPosition();
	else if(bThree)
		p = mBtn3->getPosition();
	else if(bFour)
		p = mBtn4->getPosition();

	mSpriteBtnBg->setPosition(p);
}
