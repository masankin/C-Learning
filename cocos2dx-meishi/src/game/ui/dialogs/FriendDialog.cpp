#include "FriendDialog.h"
#include "core/SoundManager.h"
#include "MessageManager.h"
#include "GenerlDialog.h"
#include "data/TableManager.h"
#include "core/CVTime.h"
#include "chat/InfoPanel.h"
#include "login/LoginProxy.h"
#include "utils/Comm.h"
#include "PvpDialog.h"
#include "game/ui/PageFunction.h"
#include "game/prop/PropsControl.h"
#include "utils/HeadIconUtils.h"
using namespace std;

#define TabCount 4
#define FriendTabIndex 0
#define SearchTabIndex 1
#define ApplyTabIndex 2
#define PkTabIndex 3
#define FriendMaxCount 45
#define FriendListTagBegin 1000
#define SearchListTagBegin 2000
#define SearchListButtonTagBegin 3000
#define ApplyListTagBegin 4000
#define ApplyListButtonNoTagBegin 5000
#define ApplyListButtonYesTagBegin 6000
#define PkListTagBegin 7000
#define PkListTagButtonNoBegin 8000
#define PkListTagButtonYesBegin 9000

#define Day2Second 86400
#define Hour2Second 3600

#define CountToRefresh 5

const int iSelectButtonBgPosY[TabCount] = { 340, 240, 145, 50 };

FriendDialog::FriendDialog() :
	m_bNewApply(false),
	m_bNewPk(false),
	m_sRealSearchText(""),
	m_iSelectTabIndex(0),
	m_iSelectItemIndex(0),
	m_iClickFriendTabCount(0),
	m_iClickApplyTabCount(0),
	m_iClickPkTabCount(0)
{
	sgMessageManager()->regisiterMessageHandler(EMT_FRIEND_UPDATE, this);
}

FriendDialog::~FriendDialog()
{
	sgMessageManager()->removeMessageHandler(this);
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
}

void FriendDialog::onMenuItemAction(std::string name, cocos2d::Ref* sender)
{
	auto pSender = dynamic_cast<Node*> (sender);
	 m_iTag = pSender->getTag();

	if (name == "onBack")
	{
		this->setShow(false);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
	}
	else if (name == "onInput")
	{
		CCLOG("[edward]===============input");
		m_pSearchBox->touchDownAction(m_pSearchBox, Widget::TouchEventType::ENDED);
	}
	else if (name == "onSearch")
	{
		CCLOG("[edward]===============search");
		if (m_sRealSearchText.empty())
		{
			m_pSearchBox->touchDownAction(m_pSearchBox, Widget::TouchEventType::ENDED);
		}
		else
		{
			onSearch();
		}
	}
	else if (name == "onFriendList")
	{
		CCLOG("[edward]===============friend list");
		m_iClickFriendTabCount++;
		changeTab(FriendTabIndex);
		if (sgFriendControl()->getFriendList().size() == 0)
		{
			sgFriendControl()->getProxy()->sendFriendListRequest();
		}
		else
		{
			updateFriendList();
			if (m_iClickFriendTabCount % CountToRefresh == 0)
			{
				sgFriendControl()->getProxy()->sendFriendListRequest();
			}
		}
	}
	else if (name == "onSearchList")
	{
		CCLOG("[edward]===============search list");
		//sgFriendControl()->getProxy()->sendSearchRequest(1000002);
		//sgFriendControl()->getProxy()->sendAddFriendRequest(1000003);
		//sgFriendControl()->getProxy()->sendAddConfirmRequest(true, 1000002);
		//sgFriendControl()->getProxy()->sendSplitUpRequest(Value(m_sRealSearchText).asInt());
		//暂时做清空处理 TODO
		changeTab(SearchTabIndex);
		sgFriendControl()->clearSearchList();
	}
	else if (name == "onApplyList")
	{
		CCLOG("[edward]===============apply list");
		m_iClickApplyTabCount++;
		changeTab(ApplyTabIndex);
		if (sgFriendControl()->getApplicationList().size() == 0)
		{
			sgFriendControl()->getProxy()->sendApplicationListRequest();
		}
		else
		{
			updateApplicationList();
			if (m_iClickApplyTabCount % CountToRefresh == 0)
			{
				sgFriendControl()->getProxy()->sendApplicationListRequest();
			}
		}
	}
	else if (name == "onPkList")
	{
		CCLOG("[edward]===============pk list");
		m_iClickPkTabCount++;
		changeTab(PkTabIndex);
		if (sgFriendControl()->getPkList().size() == 0)
		{
			sgFriendControl()->getProxy()->sendPKListRequest();
		}
		else
		{
			updatePkList();
			if (m_iClickPkTabCount % CountToRefresh == 0)
			{
				sgFriendControl()->getProxy()->sendPKListRequest();
			}
		}
	}
	else if (name == "onBatchAccept")
	{
		CCLOG("[edward]===============Batch Accept");
		onBatchEvent(true);
	}
	else if (name == "onBatchReject")
	{
		CCLOG("[edward]===============Batch Reject");
		onBatchEvent(false);
	}
	else if (name == "onAdd")
	{
		CCLOG("[edward]===============add");
		if (sgFriendControl()->getFriendList().size() >= FriendMaxCount)
		{
			CCLOG("[edward]=============== full ");
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_FRIEND_MAX));
		}
		else if (sgFriendControl()->isFriend(sgFriendControl()->getSearchList().at(m_iTag - SearchListButtonTagBegin)->m_uId))
		{
			CCLOG("[edward]=============== true friend ");
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_FRIEND_IS_FRIEND));
		}
		else
		{
			auto pButton = dynamic_cast<Button*> (pSender);
			pButton->setEnabled(false);
			pButton->setBright(false);
			//Sprite* pSprite = Sprite::createWithSpriteFrameName("ui/Friend/btn_g_1.png");
			((Sprite*)(pButton->getChildByName("Sprite_add")))->setSpriteFrame("ui/Friend/btn_g_1.png");
			sgFriendControl()->sendAddFriendRequest(m_iTag - SearchListButtonTagBegin);
		}
	}
	else if (name == "onApplyNo")
	{
		CCLOG("[edward]===============apply no");
		sgFriendControl()->sendAddConfirmRequest(false, m_iTag - ApplyListButtonNoTagBegin);
		deleteApplicationItem(m_iTag - ApplyListButtonNoTagBegin);
	}
	else if (name == "onApplyYes")
	{
		CCLOG("[edward]===============apply yes");
		if (sgFriendControl()->getFriendList().size() >= FriendMaxCount)
		{
			CCLOG("[edward]=============== full ");
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_FRIEND_MAX));
		}
		else if (sgFriendControl()->isFriend(sgFriendControl()->getApplicationList().at(m_iTag - ApplyListButtonYesTagBegin)->m_uId))
		{
			CCLOG("[edward]=============== true friend ");
			sgFriendControl()->sendAddConfirmRequest(false , m_iTag - ApplyListButtonYesTagBegin);
			deleteApplicationItem(m_iTag - ApplyListButtonYesTagBegin);
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_FRIEND_IS_FRIEND));
		}
		else
		{
			sgFriendControl()->sendAddConfirmRequest(true, m_iTag - ApplyListButtonYesTagBegin);
			deleteApplicationItem(m_iTag - ApplyListButtonYesTagBegin);
		}
	}
	else if (name == "onPkNo")
	{
		CCLOG("[edward]===============pk no");
		sgFriendControl()->setPkPlayerData(sgFriendControl()->getPkList().at(m_iTag - PkListTagButtonNoBegin));
		// change by jim, unify tips ui
		//GeneralDialogOther::show(sgTextHelper(TEXT_PVP_ROOM_REJECT_CONFIRM), GeneralDialogOther::E_GENERAL_TYPE_CONFIRM, CC_CALLBACK_1(FriendDialog::rejcectPvpInviteCallback, this));
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_PVP_ROOM_REJECT_CONFIRM), CC_CALLBACK_1(FriendDialog::rejcectPvpInviteCallback, this));
	}
	else if (name == "onPkYes")
	{
		CCLOG("[edward]===============pk yes");
		sgFriendControl()->setPkPlayerData(sgFriendControl()->getPkList().at(m_iTag - PkListTagButtonYesBegin));
		std::string inviteName = sgFriendControl()->getPkPlayerData()->m_sName;
		std::string content = StringUtils::format(sgTextHelper(TEXT_PVP_ROOM_ACCEPT_CONFIRM).c_str(), inviteName.c_str());
		// change by jim, unify tips ui
		// GeneralDialogOther::show(content, GeneralDialogOther::E_GENERAL_TYPE_PK_CONFIRM, CC_CALLBACK_1(FriendDialog::acceptPvpInviteCallback, this));
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), content, CC_CALLBACK_1(FriendDialog::acceptPvpInviteCallback, this));
	}
	else if (name == "onFriendItem")
	{
		CCLOG("[edward]===============friend item");
		//showDetail(sgFriendControl()->getFriendList().at(m_iTag - FriendListTagBegin), E_TYPE_FRIEND_MINE);
		//?????????????
		FriendSystem::PlayerData*  player = sgFriendControl()->getFriendList().at(m_iTag - FriendListTagBegin);
		sgFriendControl()->getProxy()->sendSearchRequest(player->m_uId,player->m_sName,1);
	}
	else if (name == "onSearchItem")
	{
		CCLOG("[edward]===============search item");
		m_iSelectItemIndex = m_iTag - SearchListTagBegin;
		showDetail(sgFriendControl()->getSearchList().at(m_iTag - SearchListTagBegin));
	}
	else if (name == "onApplyItem")
	{
		CCLOG("[edward]===============apply item");
		showDetail(sgFriendControl()->getApplicationList().at(m_iTag - ApplyListTagBegin));
	}
#ifdef MGAME_PLATFORM_MPLUS
	else if (name == "onInvite")
	{
		sgChannelApi()->callGameInfo(TYPE_INVITE_FRIEND, "");
	}
#endif
	
}

void FriendDialog::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_FRIEND_UPDATE)
	{
		MessageFriend* pMes = dynamic_cast<MessageFriend*>(message);
		switch (pMes->getListId())
		{
		case E_FRIEND_MINE_GET:
			updateFriendList();
			break;
		case E_FRIEND_MINE_REMOVE:
		case E_FRIEND_MINE_ADD:
			{
				changeFriendCount(sgFriendControl()->getFriendList().size());
				updateFriendList();
			}
			break;
		case E_FRIEND_APPLIACTION:
			updateApplicationList();
			break;
		case E_FRIEND_BLACK:
			break;
		case E_FRIEND_PK:
			updatePkList();
			break;
		case E_FRIEND_SEARCH:
			{
				changeTab(SearchTabIndex);
				updateSearchList();
			}
			break;
		case E_FRIEND_FIND_NONE:
			{
				updateSearchList();
				GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_FRIEND_FIND_NONE));
			}
			break;
		case E_FRIEND_AGREE_PK:
			{
				sgFriendControl()->reomovePvpInviteTip();
				this->setShow(false);
			}
		case E_FRIEND_ADD_REQUEST_DONE:
			{
				if (m_pListView[SearchTabIndex]->getChildrenCount() > 0)
				{
					auto pButton = dynamic_cast<ui::Button*>(m_pListView[SearchTabIndex]->getItem(m_iSelectItemIndex)->getChildByName("Button_add"));
					pButton->setEnabled(false);
					pButton->setBright(false);
					((Sprite*)(pButton->getChildByName("Sprite_add")))->setSpriteFrame("ui/Friend/btn_g_1.png");
				}
			}
			break;
		case  E_FRIEND_INFO_PANLE:
		    {
				showDetail(sgFriendControl()->getFriendList().at(m_iTag - FriendListTagBegin), E_TYPE_FRIEND_MINE);
				updateFriendList();
			}
			break;
		}
	}
}

bool FriendDialog::init()
{
	if (!Window::init())
		return false;

	mRoot = CSLoader::createNode("ui/FriendPanel.csb");

	mRoot->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(mRoot);

	string sTabName[TabCount] = { "Button_mine", "Button_search", "Button_apply", "Button_pk" };
	string sPanelName[TabCount] = { "Panel_1", "Panel_2", "Panel_3", "Panel_4" };
	for (int i = 0; i < TabCount; i++)
	{
		m_pTabButton[i] = dynamic_cast<Button*> (mRoot->getChildByName("Image_2")->getChildByName(sTabName[i]));
		m_pPanel[i] = dynamic_cast<Layout*> (mRoot->getChildByName(sPanelName[i]));
		m_pListView[i] = dynamic_cast<ListView*> (m_pPanel[i]->getChildByName("ListView_1"));
	}
	m_pSelectButtonBg = dynamic_cast<Sprite*> (mRoot->getChildByName("Image_2")->getChildByName("Sprite_selectBg"));
	m_pBatchAcceptButton = dynamic_cast<Button*> (mRoot->getChildByName("Button_batchAccept"));
	m_pBatchRejectButton = dynamic_cast<Button*> (mRoot->getChildByName("Button_batchReject"));

	auto pMaxText = dynamic_cast<Text*> (mRoot->getChildByName("Text_max"));
	pMaxText->setString("/" + Value(FriendMaxCount).asString());

	m_pSearchTextTield = dynamic_cast<TextField*> (mRoot->getChildByName("Node_search")->getChildByName("TextField_search"));
	m_pSearchTextTield->setMaxLength(12);
#ifdef MGAME_PLATFORM_MPLUS
	if (m_pSearchTextTield) {
		m_pSearchTextTield->setFontName(DEFAULT_FONT);
	}
#endif

	auto editBoxSize = Size(0, 0);
	m_pSearchBox = ui::EditBox::create(editBoxSize, "ui/main/main_reddot.png", TextureResType::PLIST);
	m_pSearchBox->setVisible(false);
	m_pSearchBox->setReturnType(ui::EditBox::KeyboardReturnType::SEARCH);
	m_pSearchBox->setDelegate(this);
	mRoot->addChild(m_pSearchBox);

	//申请列表红点提示
	m_pNewApplyTipNode = cocostudio::Armature::create("fla_HongDian");
	m_pNewApplyTipNode->getAnimation()->play("play");
	mRoot->getChildByName("Image_2")->addChild(m_pNewApplyTipNode);

	auto applyTitlePosition = m_pTabButton[ApplyTabIndex]->getPosition();
	m_pNewApplyTipNode->setPosition(applyTitlePosition + Vec2(35, 20));
	//m_pNewApplyTipNode->getAnimation()->play("play");
	//m_pNewApplyTipNode->setScale(.5f);
	m_pNewApplyTipNode->setVisible(false);

	//约战列表红点提示

	m_pNewPkTipNode = cocostudio::Armature::create("fla_HongDian");
	m_pNewPkTipNode->getAnimation()->play("play");

	mRoot->getChildByName("Image_2")->addChild(m_pNewPkTipNode);
	auto pkTitlePosition = m_pTabButton[PkTabIndex]->getPosition();
	m_pNewPkTipNode->setPosition(pkTitlePosition + Vec2(35, 20));
	//m_pNewPkTipNode->getAnimation()->play("play");
	m_pNewPkTipNode->setScale(.5f);
	m_pNewPkTipNode->setVisible(false);

	setPopType(E_POP_TYPE_BIG);
	onLocateClickCallback(mRoot);

#ifdef MGAME_PLATFORM_MPLUS
	dynamic_cast<ui::Text*>(mRoot->getChildByName("mIdLabel"))->setText(StringUtils::format("%d", sgUserObj()->getNewUin()));
#else
	//mRoot->getChildByName("mIdLabel")->setVisible(false);
#endif

	return true;
}

void FriendDialog::onShow()
{
	Window::onShow();

	//sgFriendControl()->getProxy()->sendFriendListRequest();
	changeTab(FriendTabIndex);
	if (sgFriendControl()->getFriendList().size() == 0)
	{
		sgFriendControl()->getProxy()->sendFriendListRequest();
	}
	else
	{
		updateFriendList();
	}
	schedule(schedule_selector(FriendDialog::update), 1.0f);
}

void FriendDialog::onClose()
{
	m_sRealSearchText.clear();
	m_pSearchTextTield->setText("");
	unschedule(schedule_selector(FriendDialog::update));

	Window::onClose();
}

void FriendDialog::update(float dt)
{
	//申请列表红点提示
	if (!m_bNewApply && sgFriendControl()->getApplicationList().size() > 0)
	{
		m_bNewApply = true;
		m_pNewApplyTipNode->setVisible(true);
	}

	//约战列表红点提示
	if (!m_bNewPk && sgFriendControl()->getPkList().size() > 0)
	{
		m_bNewPk = true;
		m_pNewPkTipNode->setVisible(true);
	}
}

void FriendDialog::changeTab(int index)
{
	for (int i = 0; i < TabCount; i++)
	{
		if (i == index)
		{
			m_pTabButton[i]->setEnabled(false);
			m_pTabButton[i]->setBright(false);
			m_pPanel[i]->setVisible(true);
		}
		else
		{
			m_pTabButton[i]->setEnabled(true);
			m_pTabButton[i]->setBright(true);
			m_pPanel[i]->setVisible(false);
		}
	}
	
	m_iSelectTabIndex = index;
	m_pSelectButtonBg->setPositionY(iSelectButtonBgPosY[index]);
	enableBatchButton(index == ApplyTabIndex && sgFriendControl()->getApplicationList().size() > 0 ? true : false);
}

void FriendDialog::changeFriendCount(int count)
{
	auto pText = dynamic_cast<Text*> (mRoot->getChildByName("Text_count"));
	pText->setString(Value(count).asString());
}

void FriendDialog::enableBatchButton(bool visible)
{
	m_pBatchAcceptButton->setVisible(visible);
	m_pBatchRejectButton->setVisible(visible);
}

void FriendDialog::addFriendItem(int index)
{
	//FriendSystem::PlayerData* data = sgFriendControl()->getApplicationList().at(index);
	//int size = sgFriendControl()->getFriendList().size();

	//auto pNode = dynamic_cast<Button*> (CSLoader::createNode("ui/FriendNode_mine.csb")->getChildByName("Button_1"));
	//pNode->removeFromParentAndCleanup(false);
	//onLocateClickCallback(pNode);
	//pNode->setTag(FriendListTagBegin + size - 1);

	//auto headButton = pNode->getChildByName("Button_head");
	//dynamic_cast<Sprite*>(headButton->getChildByName("Sprite_head"))->setSpriteFrame("ui/Icon/Icon_bz.png");

	//dynamic_cast<Text*>(pNode->getChildByName("Text_name"))->setString(data->m_sName);
	//dynamic_cast<Text*>(pNode->getChildByName("Text_rank"))->setString(Value(data->m_iOrder).asString());
	//dynamic_cast<Text*>(pNode->getChildByName("Text_winRate"))->setString(Value(data->m_iWinRate).asString() + "%");
	//dynamic_cast<Text*>(pNode->getChildByName("Text_time"))->setString(time2Chinese(data->m_iTime));

	//m_pListView[FriendTabIndex]->pushBackCustomItem(pNode);
}

void FriendDialog::updateFriendList()
{
	if (m_iSelectTabIndex != FriendTabIndex)
		return;

	m_pListView[FriendTabIndex]->removeAllItems();

	vector<FriendSystem::PlayerData*> vec = sgFriendControl()->getFriendList();
	int size = vec.size();
	for (int i = 0; i < size; i++)
	{
		auto pNode = dynamic_cast<Button*> (CSLoader::createNode("ui/FriendNode_mine.csb")->getChildByName("Button_1"));
		pNode->removeFromParentAndCleanup(false);
		pNode->setTag(FriendListTagBegin + i);

		auto headButton = pNode->getChildByName("Button_head");
		auto sprIcon = dynamic_cast<Sprite*>(headButton->getChildByName("Sprite_head"));
#ifdef MGAME_PLATFORM_MPLUS
		FriendSystem::setPlayerIconSpr(sprIcon, vec.at(i)->m_iIcon, vec.at(i)->m_sThirdPartyIcon);
		if (vec.at(i)->m_bTPFriend)
			sprIcon->getChildByName("Sprite_1")->setVisible(true);
		else
			sprIcon->getChildByName("Sprite_1")->setVisible(false);
#else
		sgHeadIconUtils()->changeIcon(sprIcon, vec.at(i)->m_iIcon);
#endif

		auto pText = dynamic_cast<Text*>(pNode->getChildByName("Text_name"));
#ifdef MGAME_PLATFORM_MPLUS
		if (pText) {
			pText->setFontName(DEFAULT_FONT);
		}
#endif // MGAME_PLATFORM_MPLUS
		pText->setString(vec.at(i)->m_sName);

		string rankText = "";
		if (vec.at(i)->m_iOrder > 0 && vec.at(i)->m_iGroup > 0)
		{
			rankText = StringUtils::format(sgTextHelper(TEXT_RANK_GROUP_1 - 1 + vec.at(i)->m_iGroup).c_str(), vec.at(i)->m_iOrder);
		}
		else
		{
			rankText = sgTextHelper(TEXT_PVP_NO_RANK);
		}
		dynamic_cast<Text*>(pNode->getChildByName("Text_rank"))->setString(rankText);
		dynamic_cast<Text*>(pNode->getChildByName("Text_winRate"))->setString(StringUtils::format("%.1f%%", vec.at(i)->m_fWinRate));
		dynamic_cast<Text*>(pNode->getChildByName("Text_time"))->setString(LogintTime2Chinese(vec.at(i)->m_iTime, vec.at(i)->m_iLogoutTime));

		m_pListView[FriendTabIndex]->pushBackCustomItem(pNode);
	}
	onLocateClickCallback(m_pListView[FriendTabIndex]);

	if (m_pPanel[FriendTabIndex]->getChildByName("Text_Tip"))
		m_pPanel[FriendTabIndex]->getChildByName("Text_Tip")->setVisible(size == 0 ? true : false);

	changeFriendCount(size);
}

void FriendDialog::updateSearchList()
{
	if (m_iSelectTabIndex != SearchTabIndex)
		return;

	m_pListView[SearchTabIndex]->removeAllItems();

	vector<FriendSystem::PlayerData*> vec = sgFriendControl()->getSearchList();
	int size = vec.size();
	for (int i = 0; i < size; i++)
	{
		auto pNode = dynamic_cast<Button*> (CSLoader::createNode("ui/FriendNode_search.csb")->getChildByName("Button_1"));
		pNode->removeFromParentAndCleanup(false);
		pNode->setTag(SearchListTagBegin + i);

		auto headButton = pNode->getChildByName("Button_head");

		auto sprIcon = dynamic_cast<Sprite*>(headButton->getChildByName("Sprite_head"));
		sgHeadIconUtils()->changeIcon(sprIcon, vec.at(i)->m_iIcon);
		

		auto pText = dynamic_cast<Text*>(pNode->getChildByName("Text_name"));		
#ifdef MGAME_PLATFORM_MPLUS
		if (pText) {
			pText->setFontName(DEFAULT_FONT);
		}
#endif // MGAME_PLATFORM_MPLUS
		pText->setString(vec.at(i)->m_sName);
		string rankText = "";
		if (vec.at(i)->m_iOrder > 0 && vec.at(i)->m_iGroup > 0)
		{
			rankText = StringUtils::format(sgTextHelper(TEXT_RANK_GROUP_1 - 1 + vec.at(i)->m_iGroup).c_str(), vec.at(i)->m_iOrder);
		}
		else
		{
			rankText = sgTextHelper(TEXT_PVP_NO_RANK);
		}
		dynamic_cast<Text*>(pNode->getChildByName("Text_rank"))->setString(rankText);
		dynamic_cast<Text*>(pNode->getChildByName("Text_winRate"))->setString(StringUtils::format("%.1f%%", vec.at(i)->m_fWinRate));
		pNode->getChildByName("Button_add")->setTag(SearchListButtonTagBegin + i);

		m_pListView[SearchTabIndex]->pushBackCustomItem(pNode);
	}

	if (m_pPanel[SearchTabIndex]->getChildByName("Text_Tip"))
		m_pPanel[SearchTabIndex]->getChildByName("Text_Tip")->setVisible(size == 0 ? true : false);

	onLocateClickCallback(m_pListView[SearchTabIndex]);
}

void FriendDialog::updateApplicationList()
{
	if (m_iSelectTabIndex != ApplyTabIndex)
		return;

	m_pListView[ApplyTabIndex]->removeAllItems();

	vector<FriendSystem::PlayerData*> vec = sgFriendControl()->getApplicationList();
	int size = vec.size();
	for (int i = size - 1; i >= 0; i--)
	{
		auto pNode = dynamic_cast<Button*> (CSLoader::createNode("ui/FriendNode_apply.csb")->getChildByName("Button_1"));
		pNode->removeFromParentAndCleanup(false);
		pNode->setTag(ApplyListTagBegin + i);

		auto headButton = pNode->getChildByName("Button_head");

		auto sprIcon = dynamic_cast<Sprite*>(headButton->getChildByName("Sprite_head"));
		sgHeadIconUtils()->changeIcon(sprIcon, vec.at(i)->m_iIcon);


		auto pText = dynamic_cast<Text*>(pNode->getChildByName("Text_name"));
		if (pText) {
#ifdef MGAME_PLATFORM_MPLUS	
			pText->setFontName(DEFAULT_FONT);
#endif
			pText->setString(vec.at(i)->m_sName);
		}
		dynamic_cast<Text*>(pNode->getChildByName("Text_time"))->setString(time2Chinese(vec.at(i)->m_iTime));
		pNode->getChildByName("Button_no")->setTag(ApplyListButtonNoTagBegin + i);
		pNode->getChildByName("Button_yes")->setTag(ApplyListButtonYesTagBegin + i);

		m_pListView[ApplyTabIndex]->pushBackCustomItem(pNode);
	}

	if (m_pPanel[ApplyTabIndex]->getChildByName("Text_Tip"))
		m_pPanel[ApplyTabIndex]->getChildByName("Text_Tip")->setVisible(size == 0 ? true : false);

	onLocateClickCallback(m_pListView[ApplyTabIndex]);
	//容错处理
	enableBatchButton(size > 0 ? true : false);
}

void FriendDialog::deleteApplicationItem(int index)
{
	sgFriendControl()->deleteApplicationItem(index);
	//m_pListView[ApplyTabIndex]->removeChildByTag(ApplyListTagBegin + index);

	if (sgFriendControl()->getApplicationList().size() == 0)
	{
		m_bNewApply = false;
		m_pNewApplyTipNode->setVisible(false);
		enableBatchButton(false);

		if (m_pPanel[ApplyTabIndex]->getChildByName("Text_Tip"))
			m_pPanel[ApplyTabIndex]->getChildByName("Text_Tip")->setVisible(true);
	}
}

void FriendDialog::updatePkList()
{
	if (m_iSelectTabIndex != PkTabIndex)
		return;

	m_pListView[PkTabIndex]->removeAllItems();

	vector<FriendSystem::PlayerData*> vec = sgFriendControl()->getPkList();
	int size = vec.size();
	for (int i = size - 1; i >= 0; i--)
	{
		auto pNode = dynamic_cast<Button*> (CSLoader::createNode("ui/FriendNode_pk.csb")->getChildByName("Button_1"));
		pNode->removeFromParentAndCleanup(false);
		pNode->setTag(PkListTagBegin + i);

		auto headButton = pNode->getChildByName("Button_head");

		auto sprIcon = dynamic_cast<Sprite*>(headButton->getChildByName("Sprite_head"));
		sgHeadIconUtils()->changeIcon(sprIcon, vec.at(i)->m_iIcon);

		auto pText = dynamic_cast<Text*>(pNode->getChildByName("Text_name"));
#ifdef MGAME_PLATFORM_MPLUS
		if (pText) {
			pText->setFontName(DEFAULT_FONT);
		}
#endif // MGAME_PLATFORM_MPLUS
		pText->setString(vec.at(i)->m_sName);

		dynamic_cast<Text*>(pNode->getChildByName("Text_time"))->setString(time2Chinese(vec.at(i)->m_iTime));
		pNode->getChildByName("Button_no")->setTag(PkListTagButtonNoBegin + i);
		pNode->getChildByName("Button_yes")->setTag(PkListTagButtonYesBegin + i);

		m_pListView[PkTabIndex]->pushBackCustomItem(pNode);
	}

	if (m_pPanel[PkTabIndex]->getChildByName("Text_Tip"))
		m_pPanel[PkTabIndex]->getChildByName("Text_Tip")->setVisible(size == 0 ? true : false);

	onLocateClickCallback(m_pListView[PkTabIndex]);

	if (sgFriendControl()->getPkList().size() == 0)
	{
		m_bNewPk = false;
		m_pNewPkTipNode->setVisible(false);
		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_PK_NONE).asString());
	}
}

void FriendDialog::deletePkItem(int uin, int roomId)
{
	sgFriendControl()->deletePkItem(uin, roomId);
	//m_pListView[ApplyTabIndex]->removeChildByTag(PkListTagBegin + index);

	if (sgFriendControl()->getPkList().size() == 0)
	{
		m_bNewPk = false;
		m_pNewPkTipNode->setVisible(false);
		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_PK_NONE).asString());

		if (m_pPanel[PkTabIndex]->getChildByName("Text_Tip"))
			m_pPanel[PkTabIndex]->getChildByName("Text_Tip")->setVisible(true);
	}
}

void FriendDialog::showDetail(FriendSystem::PlayerData* data, int type)
{
	sgUserObj()->setCurInfoUin(data->m_uId);
	sgUserObj()->setCurInfoType(type);
	sgPageFunction()->gotoFunction(MSG_FUC_INFO);
	//addChild(pDdialog);
}

void FriendDialog::onBatchEvent(bool isAgree)
{
	int size = sgFriendControl()->getApplicationList().size();
	if (isAgree && size + sgFriendControl()->getFriendList().size() > FriendMaxCount)
	{
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_FRIEND_MAX));
		return;
	}

	if (size > 0)
	{
		vector<int> uins;

		for (int i = 0; i < size; i++)
		{
			uins.push_back(sgFriendControl()->getApplicationList().at(i)->m_uId);
		}
		sgFriendControl()->getProxy()->sendAddConfirmRequest(isAgree, uins);

		sgFriendControl()->clearApplicationList();
		m_pListView[ApplyTabIndex]->removeAllItems();
	}

	if (sgFriendControl()->getApplicationList().size() == 0)
	{
		m_bNewApply = false;
		m_pNewApplyTipNode->setVisible(false);
		enableBatchButton(false);

		if (m_pPanel[ApplyTabIndex]->getChildByName("Text_Tip"))
			m_pPanel[ApplyTabIndex]->getChildByName("Text_Tip")->setVisible(true);
	}
}

void FriendDialog::editBoxReturn(cocos2d::ui::EditBox* editBox)
{
	m_sRealSearchText = m_pSearchBox->getText();
	m_pSearchTextTield->setString(m_sRealSearchText);

	onSearch();
}

bool FriendDialog::checkCharacter(const std::string str)
{
	if (str.empty()) return false;

	int i = str.find(' ');
	if (i != -1)return false;

	int len = 0;
	int length = str.length();

	for (unsigned int i = 0; i < length; i++)
	{
		if (str.at(i) < 33 || str.at(i) > 126)
			len += 1;
		if (len)
		{
			int num = (i + 1 - len) + (len / 3);
			if (num >= 6){
				if (i + 1 == length){
					return true;
				}
				return false;
			}
		}
		else if (length > 12)
		{
			return false;
		}
	}
	return true;
}

std::string FriendDialog::time2Chinese(int target)
{
	int currentTime = sgLoginProxy()->getServerTime();
	CVTime nowTime(currentTime == 0 ? time(0) : currentTime);
	CVTime targetTime(target);

	int dis = ::abs(nowTime.GetTime() - targetTime.GetTime());
	if (dis > Day2Second * 30 * 6)
	{
		return sgTextHelper(TEXT_FRIEND_LONGTIME);
	}
	else if (dis > Day2Second * 30)
	{
		return String::createWithFormat(sgTextHelper(TEXT_FRIEND_MONTH).c_str(), dis / Day2Second / 30)->getCString();
	}
	else if (dis > Day2Second)
	{
		return String::createWithFormat(sgTextHelper(TEXT_FRIEND_DAY).c_str(), dis / Day2Second)->getCString();
	}
	else if (dis > Hour2Second)
	{
		return String::createWithFormat(sgTextHelper(TEXT_FRIEND_DAY).c_str(), dis / Hour2Second)->getCString();
	}

	return sgTextHelper(TEXT_FRIEND_RECENTLY);
}

std::string FriendDialog::LogintTime2Chinese(int loginTime, int logoutTime)
{
	if (loginTime > logoutTime)
	{
		return sgTextHelper(TEXT_FRIEND_ONLINE);
	}
	else
	{
		return time2Chinese(logoutTime);
	}
}

void FriendDialog::acceptPvpInviteCallback(cocos2d::Ref* sender)
{
	
	//if (sgUserObj()->getPvpPower() < PVP_NEED_PSYCH)
	if (sgUserObj()->getPersonalInfo()->energy() < PVP_NEED_PSYCH)
	{
		PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());//sgUserObj()->getEnergyBuyCount());
		if (pPowerBuy)
		{
			GenerlDialog::show(sgTextHelper(TEXT_BUY_ITEM__JINGLI),
				__String::createWithFormat(sgTextHelper(TEXT_BUY_ITEM_JINGLI_SURE).c_str(), pPowerBuy->getEnergyNum())->getCString(), CC_CALLBACK_1(FriendDialog::onClickBuyEngerCallBack,this));
		}
	}
	else
	{
		//GeneralDialogOther::hide();
		GenerlDialog::hide();
		FriendSystem::PlayerData* pData = sgFriendControl()->getPkPlayerData();
		sgFriendControl()->getProxy()->sendPkConfirmRequest(true, pData->m_iRoom, pData->m_uId);
	}
}

void FriendDialog::rejcectPvpInviteCallback(cocos2d::Ref* sender)
{
	//GeneralDialogOther::hide();
	GenerlDialog::hide();

	FriendSystem::PlayerData* pData = sgFriendControl()->getPkPlayerData();
	sgFriendControl()->getProxy()->sendPkConfirmRequest(false, pData->m_iRoom, pData->m_uId);
	deletePkItem(pData->m_uId, pData->m_iRoom);
	updatePkList();
}

void FriendDialog::onSearch()
{
	if (m_sRealSearchText.empty())
		return;

	if (sgUserObj()->getNewUin() == Value(m_sRealSearchText).asInt()
		|| sgUserObj()->getPlayerName() == m_sRealSearchText)
	{
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_FRIEND_FIND_NONE));
		return;
	}

	sgFriendControl()->getProxy()->sendSearchRequest(sgUserObj()->convertToUin(Value(m_sRealSearchText).asInt()), m_sRealSearchText);
}

void FriendDialog::onClickBuyEngerCallBack(cocos2d::Ref*)
{
	//if (sgUserObj()->getPvpPower() >= POWER_BUY_MAX)
	if (sgUserObj()->getPersonalInfo()->energy() > POWER_BUY_MAX)
	{
		GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS),
			__String::createWithFormat("%s%s", sgTextHelper(TEXT_ACT_JINGLI).c_str(), sgTextHelper(TEXT_BUY_POWER_MAX).c_str())->getCString());
		return;
	}
	PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());// sgUserObj()->getEnergyBuyCount());
	if (pPowerBuy)
	{
		//if (sgUserObj()->getGoldCoin() >= pPowerBuy->getEnergyNum())
		if (sgUserObj()->getPersonalInfo()->diamond() >= pPowerBuy->getEnergyNum())
		{
			sgPropsControl()->sendBuyPropRqst(PVP_ENERGY_TYPE, pPowerBuy->getEnergyNum(), 1);
		}
		else
		{
			PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
		}
		GenerlDialog::hide();
	}
}
