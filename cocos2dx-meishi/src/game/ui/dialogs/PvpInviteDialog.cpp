#include "PvpInviteDialog.h"
#include "game/pvp/PvpRoomControl.h"
#include "friend/FriendControl.h"
#include "GenerlDialog.h"
#include "game/data/TableManager.h"
#include "game/chat/InfoPanel.h"
#include "utils/Comm.h"
#include "TimeCalculator.h"
#include "game/ui/PageFunction.h"
#include "utils/HeadIconUtils.h"
PvpInviteDialog::PvpInviteDialog() :mFriendNum(12)
{
	// 
}

PvpInviteDialog::~PvpInviteDialog()
{

}
bool PvpInviteItem::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PVPInvite_node.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);

		mName = dynamic_cast<Text *>(mRoot->getChildByName("Text_name"));
#ifdef MGAME_PLATFORM_MPLUS
		if (mName) {
			mName->setFontName(DEFAULT_FONT);
		}
#endif
		mWin = dynamic_cast<Text *>(mRoot->getChildByName("Text_win"));
		mPlayerTitle = dynamic_cast<Sprite *>(mRoot->getChildByName("Sprite_playerTitle"));
		mRank = dynamic_cast<Text *>(mRoot->getChildByName("Text_rank"));
		mBtnIcon = dynamic_cast<Button *>(mRoot->getChildByName("Button_icon"));
		mBtnInvite = dynamic_cast<Button *>(mRoot->getChildByName("Button_invite"));

		
		mBtnInvite->addClickEventListener(CC_CALLBACK_1(PvpInviteItem::inviteFirendCallback,this));
		mLimitLevel = dynamic_cast<Text *>(mRoot->getChildByName("text_tip")->getChildByName("text_limitLevel"));
		PlayerInfo * playInfo = sgTableManager()->getPlayerInfo();
		mLimitLevel->setText(StringUtils::format("%d",playInfo->getInviteLevel()));
		 
		ret = true; 
	} while (0);

	return ret;
}

void PvpInviteItem::inviteFirendCallback(Ref * pSender)
{
	CCLOG("[trace]PvpInviteItem::inviteFirendCallback,inviteFriend");
	Button * btn = dynamic_cast<Button*>(pSender);
	//
	btn->setBright(false);
	btn->setTouchEnabled(false);
	sgPvpRoomControl()->sendInvitePlayer(btn->getTag());


}

void PvpInviteDialog::detailCallback(Ref * pSender)
{
	CCLOG("[trace]PvpInviteItem::detailCallback,detailFiend");
	int tag = dynamic_cast<Button*>(pSender)->getTag();

	sgUserObj()->setCurInfoUin(mPlayerInfo.at(tag)->m_uId);
	sgUserObj()->setCurInfoType(0);
	sgPageFunction()->gotoFunction(MSG_FUC_INFO);
}

bool PvpInviteDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/PVPInvite.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);
		
		mTime = dynamic_cast<Text *> (mRoot->getChildByName("Text_timeout"));

		mListView = dynamic_cast<ListView *>(mRoot->getChildByName("ListView"));
		
		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);
	return ret;
}

void PvpInviteDialog::onMenuItemAction(std::string name, cocos2d::Ref* sender)
{
	if (name == "onClose")
	{
		this->setShow(false);
		CCLOG("[trace]PvpInviteDialog::onMenuItemAction ,onClose");
	}
}

void PvpInviteDialog::onShow()
{

	mPlayerInfo.clear();
	mListView->removeAllItems();
	//初始化listView 
	FriendSystem::PlayerData  *stPlayerData;
	for (int i = 0; i < sgFriendControl()->getFriendList().size(); ++i)
	{
		stPlayerData = sgFriendControl()->getFriendList()[i];
		
		PvpInviteItem * item = PvpInviteItem::create();
		
		item->mBtnInvite->setTag(stPlayerData->m_uId);
		item->mBtnIcon->setTag(i);
		item->mBtnIcon->addClickEventListener(CC_CALLBACK_1(PvpInviteDialog::detailCallback, this));
		mPlayerInfo.push_back(stPlayerData);

		pushListItem(item, stPlayerData);
	}

}

void PvpInviteDialog::updateUI()
{

}

void PvpInviteDialog::pushListItem(PvpInviteItem* item, FriendSystem::PlayerData *playerData)
{

	item->setPosition(Vec2(320, 60));

	Widget* pWidget = Widget::create();
	pWidget->setContentSize(Size(640, 160));
	pWidget->addChild((Node*)item);
	mListView->pushBackCustomItem(pWidget);
#ifdef MGAME_PLATFORM_MPLUS
	FriendSystem::setPlayerIconBtn(item->mBtnIcon,playerData->m_iIcon,playerData->m_sThirdPartyIcon);
#else
	sgHeadIconUtils()->changeIcon(item->mBtnIcon, playerData->m_iIcon);
#endif

	//item->mBtnIcon->setPositionX(item->mBtnIcon->getPositionX() - 20);
	item->mName->setText(playerData->m_sName);
	item->mWin->setText(StringUtils::format("%.2f%%",playerData->m_fWinRate));

	int group = TEXT_RANK_GROUP_1 - 1 + playerData->m_iGroup;
	if (playerData->m_iGroup == 0)
	{
		group = TEXT_PVP_NO_RANK;
		item->mRank->setText(sgTextHelper(group).c_str());
	}
	else
	{
		item->mRank->setText(StringUtils::format(sgTextHelper(group).c_str(), playerData->m_iOrder).c_str());
	}

	//if (fgame::CSSycPlayerInfoResp_TitleType_IsValid(playerData->m_iTitle))
// 	if (playerData->m_iTitle > TitleType::INVALID  && playerData->m_iTitle < TitleType::MAX)
// 	{
// 		item->mPlayerTitle->setSpriteFrame(StringUtils::format("ui/general/PlayerTitle_%d.png", playerData->m_iTitle));
// 		item->mPlayerTitle->setVisible(true);
// 	}
// 	else
// 	{
// 		item->mPlayerTitle->setVisible(false);
// 	}

	auto pData = sgTableManager()->getBadgeData(playerData->m_iTitle);
	if (pData && pData->getActive() != 0)
	{
		item->mPlayerTitle->setSpriteFrame(GetPlayerBadgeIconNameById(pData->getId(), true));
		item->mPlayerTitle->setVisible(true);
		item->mPlayerTitle->setScale(0.6f);
	}
	else
	{
		item->mPlayerTitle->setVisible(false);
	}

	//邀请等级限制
	PlayerInfo *playInfo = sgTableManager()->getPlayerInfo();
	bool bLimit = playerData->m_iPlayerLevel >=playInfo->getInviteLevel();
	item->mLimitLevel->getParent()->setVisible(!bLimit);
	item->mBtnInvite->setVisible(bLimit);
}
