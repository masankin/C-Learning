#include "FriendControl.h"
#include "MessageManager.h"
#include "core/stl_helper.h"
#include "net/UserObj.h"
#include "battle/BattleControl.h"
#include "ui/dialogs/GenerlDialog.h"
#include "data/TableManager.h"
#include "pvp/PvpRoomControl.h"
#include "ui/PageFunction.h"
#include "utils/Comm.h"

FriendControl::FriendControl() :
m_pPvpInviteTip(NULL), mSeachType(0)
{
	SAFE_RELEASE_VECTOR(m_vFriendList);
	SAFE_RELEASE_VECTOR(m_vApplicationList);
	SAFE_RELEASE_VECTOR(m_vPkList);
	SAFE_RELEASE_VECTOR(m_vSearchList);
	SAFE_RELEASE_VECTOR(m_vBlackList);

	m_pProxy = new FriendProxy();
	m_pInviterData = new FriendSystem::PlayerData();
	m_pPkPlayerData = new FriendSystem::PlayerData();
}

FriendControl::~FriendControl()
{
	if (m_pProxy)
		delete m_pProxy;
}

void FriendControl::updateFriendList(const fgame::CSFriendGetInfoResp& stMsg)
{
	//if (m_vFriendList.size() > 0)
	//	return;
	//SAFE_RELEASE_VECTOR(m_vFriendList);
	int size = stMsg.friends_size();
	if (size != 0)
	{
		SAFE_RELEASE_VECTOR(m_vFriendList);
		for (int i = 0; i < size; ++i)
		{
			auto playerInfo = stMsg.friends(i);

			FriendSystem::PlayerData* data = new FriendSystem::PlayerData();
			data->m_uId = playerInfo.uin();
			data->m_sName = playerInfo.nickname();
			data->m_iIcon = playerInfo.icon();
			data->m_iOrder = playerInfo.rank();
			data->m_fWinRate = playerInfo.winrate();
			data->m_iTime = playerInfo.lastonlinetime();
			data->m_iGroup = playerInfo.group();
			data->m_iPlayerLevel = playerInfo.explevel();
			data->m_iExp = playerInfo.exp();
			if (playerInfo.has_lastofflinetime())
				data->m_iLogoutTime = playerInfo.lastofflinetime();
			if (playerInfo.has_score())
				data->m_iScore = playerInfo.score();
			if (playerInfo.has_title())
				data->m_iTitle = playerInfo.title();
			if (playerInfo.has_level())
				data->m_iLevel = playerInfo.level();
#ifdef MGAME_PLATFORM_MPLUS
			if (playerInfo.has_openid())
				data->m_sOpenId = playerInfo.openid();
#endif
			
			m_vFriendList.push_back(data);
		}
	}
#ifdef MGAME_PLATFORM_MPLUS
	sgChannelApi()->callGameInfo(TYPE_FRIEND_LIST, "");
#endif
	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_MINE_GET).asString());
}

void FriendControl::updateFriendList(const fgame::CSFriendConfirmAddResp& stMsg)
{
	int size = stMsg.newfriend_size();
	for (int i = 0; i < size; ++i)
	{
		auto playerInfo = stMsg.newfriend(i);

		//容错性处理
		bool isFriend = false;
		for (auto it = m_vFriendList.begin(); it != m_vFriendList.end(); it++)
		{
			if (playerInfo.uin() == (*it)->m_uId)
			{
				isFriend = true;
				break;
			}
		}

		if (isFriend)
			break;

		FriendSystem::PlayerData* data = new FriendSystem::PlayerData();
		data->m_uId = playerInfo.uin();
		data->m_sName = playerInfo.nickname();
		data->m_iIcon = playerInfo.icon();
		data->m_iOrder = playerInfo.rank();
		data->m_fWinRate = playerInfo.winrate();
		data->m_iTime = playerInfo.lastonlinetime();
		data->m_iGroup = playerInfo.group();
		data->m_iExp = playerInfo.exp();
		data->m_iPlayerLevel = playerInfo.explevel();
		if (playerInfo.has_lastofflinetime())
			data->m_iLogoutTime = playerInfo.lastofflinetime();
		if (playerInfo.has_score())
			data->m_iScore = playerInfo.score();
		if (playerInfo.has_title())
			data->m_iTitle = playerInfo.title();
		if (playerInfo.has_level())
			data->m_iLevel = playerInfo.level();

		m_vFriendList.push_back(data);
	}

	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_MINE_ADD).asString());
}

void FriendControl::updateFriendList(const fgame::CSFriendRequestRemoveResp& stMsg)
{
	int iUin = stMsg.uin();

	int size = m_vFriendList.size();
	for (auto it = m_vFriendList.begin(); it != m_vFriendList.end(); it++)
	{
		if (iUin == (*it)->m_uId)
		{
			m_vFriendList.erase(it);
			sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_MINE_REMOVE).asString());
			return;
		}
	}

	//getProxy()->sendFriendListRequest();
}

void FriendControl::deleteFriendItem(int uin)
{
	int size = m_vFriendList.size();
	for (auto it = m_vFriendList.begin(); it != m_vFriendList.end(); it++)
	{
		if (uin == (*it)->m_uId)
		{
			m_vFriendList.erase(it);
			sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_MINE_REMOVE).asString());
			getProxy()->sendSplitUpRequest(uin);
			return;
		}
	}
}

void FriendControl::updateSearchList(const fgame::CSFriendQueryResp& stMsg)
{
	SAFE_RELEASE_VECTOR(m_vSearchList);
	int size = stMsg.players_size();
	for (int i = 0; i < size; ++i)
	{
		auto playerInfo = stMsg.players(i);

		if (stMsg.players(0).uin() == sgUserObj()->getUin())
			break;
		
		FriendSystem::PlayerData* data = new FriendSystem::PlayerData();
		data->m_uId = playerInfo.uin();
		data->m_sName = playerInfo.nickname();
		data->m_iIcon = playerInfo.icon();
		data->m_iOrder = playerInfo.rank();
		data->m_fWinRate = playerInfo.winrate();
		data->m_iTime = playerInfo.lastonlinetime();
		data->m_iGroup = playerInfo.group();
		data->m_iPlayerLevel = playerInfo.explevel();
		data->m_iExp = playerInfo.exp();
		if (playerInfo.has_lastofflinetime())
			data->m_iLogoutTime = playerInfo.lastofflinetime();
		if (playerInfo.has_score())
			data->m_iScore = playerInfo.score();
		if (playerInfo.has_title())
			data->m_iTitle = playerInfo.title();
		if (playerInfo.has_level())
			data->m_iLevel = playerInfo.level();

		m_vSearchList.push_back(data);
	}

	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(m_vSearchList.size() > 0 ? E_FRIEND_SEARCH : E_FRIEND_FIND_NONE).asString());
}

void FriendControl::clearSearchList()
{
	SAFE_RELEASE_VECTOR(m_vSearchList);
	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_SEARCH).asString());
}

void FriendControl::updateApplicationList(const fgame::CSFriendGetInfoResp& stMsg)
{
	//if (m_vApplicationList.size() > 0)
	//	return;
	int size = stMsg.requests_size();
	if (!(size == 0 && m_vApplicationList.size() > 0))
	{
		SAFE_RELEASE_VECTOR(m_vApplicationList);
		for (int i = 0; i < size; ++i)
		{
			auto playerInfo = stMsg.requests(i);

			FriendSystem::PlayerData* data = new FriendSystem::PlayerData();
			data->m_uId = playerInfo.info().uin();
			data->m_sName = playerInfo.info().nickname();
			data->m_iIcon = playerInfo.info().icon();
			data->m_iOrder = playerInfo.info().rank();
			data->m_fWinRate = playerInfo.info().winrate();
			data->m_iTime = playerInfo.time();
			data->m_iGroup = playerInfo.info().group();
			data->m_iExp = playerInfo.info().exp();
			data->m_iPlayerLevel = playerInfo.info().explevel();
			if (playerInfo.info().has_score())
				data->m_iScore = playerInfo.info().score();
			if (playerInfo.info().has_title())
				data->m_iTitle = playerInfo.info().title();
			if (playerInfo.info().has_level())
				data->m_iLevel = playerInfo.info().level();

			m_vApplicationList.push_back(data);
		}
	}

	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_APPLIACTION).asString());
	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(m_vApplicationList.size() == 0 ? E_FRIEND_APPLIACTION_NONE : E_FRIEND_APPLIACTION_HAVE).asString());
}

void FriendControl::updateApplicationList(const fgame::CSFriendRequestAddResp& stMsg)
{
	if (stMsg.has_info())
	{
		auto playerInfo = stMsg.info().info();

		FriendSystem::PlayerData* data = new FriendSystem::PlayerData();
		data->m_uId = playerInfo.uin();
		data->m_sName = playerInfo.nickname();
		data->m_iIcon = playerInfo.icon();
		data->m_iOrder = playerInfo.rank();
		data->m_fWinRate = playerInfo.winrate();
		data->m_iTime = stMsg.info().time();
		data->m_iGroup = playerInfo.group();
		data->m_iPlayerLevel = playerInfo.explevel();
		data->m_iExp = playerInfo.exp();
		if (playerInfo.has_score())
			data->m_iScore = playerInfo.score();
		if (playerInfo.has_title())
			data->m_iTitle = playerInfo.title();
		if (playerInfo.has_level())
			data->m_iLevel = playerInfo.level();

		//去重
		int size = m_vApplicationList.size();
		for (auto it = m_vApplicationList.begin(); it != m_vApplicationList.end(); it++)
		{
			if (data->m_uId == (*it)->m_uId)
			{
				m_vApplicationList.erase(it);
				break;
			}
		}

		m_vApplicationList.push_back(data);

		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_APPLIACTION).asString());
		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_APPLIACTION_HAVE).asString());
	}	
}

void FriendControl::deleteApplicationItem(int index)
{
	vector<FriendSystem::PlayerData*>::iterator it = m_vApplicationList.begin() + index;
	m_vApplicationList.erase(it);

	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_APPLIACTION).asString());
	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(m_vApplicationList.size() == 0 ? E_FRIEND_APPLIACTION_NONE : E_FRIEND_APPLIACTION_HAVE).asString());
}

void FriendControl::clearApplicationList()
{
	SAFE_RELEASE_VECTOR(m_vApplicationList);
}

void FriendControl::updatePkList(const fgame::CSFriendGetInfoResp& stMsg)
{
	//if (m_vPkList.size() > 0)
	//	return;
	int size = stMsg.pk_size();
	if (!(size == 0 && m_vPkList.size() > 0))
	{
		SAFE_RELEASE_VECTOR(m_vPkList);
		for (int i = 0; i < size; ++i)
		{
			auto playerInfo = stMsg.pk(i);

			FriendSystem::PlayerData* data = new FriendSystem::PlayerData();
			data->m_uId = playerInfo.info().uin();
			data->m_sName = playerInfo.info().nickname();
			data->m_iIcon = playerInfo.info().icon();
			data->m_iOrder = playerInfo.info().rank();
			data->m_fWinRate = playerInfo.info().winrate();
			data->m_iTime = playerInfo.time();
			data->m_iGroup = playerInfo.info().group();
			data->m_iRoom = playerInfo.roomid();
			data->m_iExp = playerInfo.info().exp();
			data->m_iPlayerLevel = playerInfo.info().explevel();
			if (playerInfo.info().has_score())
				data->m_iScore = playerInfo.info().score();
			if (playerInfo.info().has_title())
				data->m_iTitle = playerInfo.info().title();
			if (playerInfo.info().has_level())
				data->m_iLevel = playerInfo.info().level();

			m_vPkList.push_back(data);
		}
	}

	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_PK).asString());
	if (m_vPkList.size() == 0)
	{
		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_PK_NONE).asString());
		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(m_vApplicationList.size() == 0 ? E_FRIEND_APPLIACTION_NONE : E_FRIEND_APPLIACTION_HAVE).asString());
	}
	else
	{
		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_PK_HAVE).asString());
	}
}

void FriendControl::updatePkList(const fgame::CSPersonalRoomInviteResp& stMsg)
{
	//向全局发送一个约战消息
	getProxy()->sendPKListRequest();
	showPvpInviteTip(stMsg.info());
}

void FriendControl::updatePkList(const fgame::CSPersonalRoomApplyOrRefuseInviteResp& stMsg)
{
	if (stMsg.isapply())
	{
		reomovePvpInviteTip();
		deletePkItem(m_pPkPlayerData->m_uId, m_pPkPlayerData->m_iRoom);
		getProxy()->sendPKListRequest();
		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_AGREE_PK).asString());
	}
}

void FriendControl::deletePkItem(int uin, int roomId)
{
	int size = m_vPkList.size();
	for (auto it = m_vPkList.begin(); it != m_vPkList.end(); it++)
	{
		if (uin == (*it)->m_uId && roomId == (*it)->m_iRoom)
		{
			m_vPkList.erase(it);
			break;
		}
	}

	if (m_vPkList.size() == 0)
	{
		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_PK_NONE).asString());
		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(m_vApplicationList.size() == 0 ? E_FRIEND_APPLIACTION_NONE : E_FRIEND_APPLIACTION_HAVE).asString());
	}
	else
	{
		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_PK_HAVE).asString());
	}
}

void FriendControl::deletePkItem(int roomId)
{
	int size = m_vPkList.size();
	if (size > 0)
	{
		for (auto it = m_vPkList.begin(); it != m_vPkList.end();)
		{
			if (roomId == (*it)->m_iRoom)
			{
				getProxy()->sendPkConfirmRequest(false, (*it)->m_iRoom, (*it)->m_uId);
				it = m_vPkList.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_PK).asString());

	if (sgFriendControl()->getPkList().size() == 0)
		sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_PK_NONE).asString());
}

void FriendControl::sendAddFriendRequest(int index)
{
	m_pProxy->sendAddFriendRequest(m_vSearchList.at(index)->m_uId);
}

void FriendControl::sendAddConfirmRequest(bool isAgree, int index)
{
	m_pProxy->sendAddConfirmRequest(isAgree, m_vApplicationList.at(index)->m_uId);
}

void FriendControl::sendPkConfirmRequest(bool isAgree, int index)
{
	m_pProxy->sendPkConfirmRequest(isAgree, m_vPkList.at(index)->m_iRoom, m_vPkList.at(index)->m_uId);
}

bool FriendControl::isFriend(int uin)
{
	int size = m_vFriendList.size();
	if (size > 0)
	{
		for (int i = 0; i < size; ++i)
		{
			if (uin == m_vFriendList.at(i)->m_uId)
				return true;
		}
	}

	return false;
}

bool FriendControl::hasFriend()
{
	return m_vFriendList.size() > 0;
}

void FriendControl::showPvpInviteTip(const ::fgame::FriendPkRequestInfo& stMsg)
{
	reomovePvpInviteTip();

	if (!sgBattleControl()->getIsPvp()
		&& sgPvpRoomControl()->getPlayerNum() == 0
		//&& !(sgUserObj()->getPlayerLevel()< sgTableManager()->getPlayerInfo()->getInviteLevel()
		&& !(sgUserObj()->getPersonalInfo()->explevel() < sgTableManager()->getPlayerInfo()->getInviteLevel()
		&& sgUserObj()->getMaxOpenGateID() <= sgEnvSet("PVP_ROOM_LEVEL")//sgUserObj()->getMaxOpenCheckPoint() <= 1140
		&& (sgPageFunction()->getCurrentUIType() == MSG_FUC_MINI_LEVEL || sgPageFunction()->getCurrentUIType() == MSG_FUC_PLAYING)))
	{

		m_pPvpInviteTip = CSLoader::createNode("PVPInviteTip_node.csb");
		dynamic_cast<ui::Button*>(m_pPvpInviteTip->getChildByName("Button_Agree"))->addClickEventListener(CC_CALLBACK_1(FriendControl::onPvpInvite, this));
		Director::getInstance()->getRunningScene()->addChild(m_pPvpInviteTip, 100);

		auto playerData = stMsg.info();
		m_pInviterData->m_uId = playerData.uin();
		m_pInviterData->m_sName = playerData.nickname();
		m_pInviterData->m_iRoom = stMsg.roomid();

		auto pText = dynamic_cast<ui::Text*>(m_pPvpInviteTip->getChildByName("Text_name"));
#ifdef MGAME_PLATFORM_MPLUS
		if (pText){
			pText->setFontName(DEFAULT_FONT);
		}
#endif // MGAME_PLATFORM_MPLUS

		pText->setString(m_pInviterData->m_sName);

		m_pPvpInviteTip->stopAllActions();
		Size winSize = Director::getInstance()->getWinSize();
		m_pPvpInviteTip->setPosition(Vec2(winSize.width + 161, winSize.height - 100));
		m_pPvpInviteTip->runAction(Sequence::create(
			MoveBy::create(.5f, Vec2(-322, 0)),
			DelayTime::create(3.0f),
			MoveBy::create(.5f, Vec2(322, 0)),
			CallFunc::create(CC_CALLBACK_0(FriendControl::reomovePvpInviteTip, this)),
			NULL));
	}
}

void FriendControl::reomovePvpInviteTip()
{
	if (m_pPvpInviteTip)
	{
		m_pPvpInviteTip->removeFromParent();
		m_pPvpInviteTip = NULL;
	}
}

void FriendControl::onPvpInvite(cocos2d::Ref* sender)
{
	std::string content = StringUtils::format(sgTextHelper(TEXT_PVP_ROOM_ACCEPT_CONFIRM).c_str(), m_pInviterData->m_sName.c_str());
	// change by jim, unify the tips ui
	// GeneralDialogOther::show(content, GeneralDialogOther::E_GENERAL_TYPE_PK_CONFIRM ,CC_CALLBACK_1(FriendControl::acceptPvpInviteCallback, this));
	GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), content, CC_CALLBACK_1(FriendControl::acceptPvpInviteCallback, this), false, true, E_GENERAL_TYPE_PK_CONFIRM);
}

void FriendControl::acceptPvpInviteCallback(cocos2d::Ref* sender)
{
	GenerlDialog::hide();
	sgFriendControl()->setPkPlayerData(m_pInviterData);
	getProxy()->sendPkConfirmRequest(true, m_pInviterData->m_iRoom, m_pInviterData->m_uId);
}

void FriendControl::setPkPlayerData(FriendSystem::PlayerData* data)
{
	m_pPkPlayerData->m_uId = data->m_uId;
	m_pPkPlayerData->m_sName = data->m_sName;
	m_pPkPlayerData->m_iIcon = data->m_iIcon;
	m_pPkPlayerData->m_iOrder = data->m_iOrder;
	m_pPkPlayerData->m_fWinRate = data->m_fWinRate;
	m_pPkPlayerData->m_iTime = data->m_iTime;
	m_pPkPlayerData->m_iGroup = data->m_iGroup;
	m_pPkPlayerData->m_iRoom = data->m_iRoom;
	m_pPkPlayerData->m_iScore = data->m_iScore;
	m_pPkPlayerData->m_iTitle = data->m_iTitle;
	m_pPkPlayerData->m_iLevel = data->m_iLevel;
	m_pPkPlayerData->m_iExp = data->m_iExp;
	m_pPkPlayerData->m_iPlayerLevel = data->m_iPlayerLevel;
}

void FriendControl::updateFriendList( const fgame::CSFriendQueryResp& stMsg )
{
	
	int size = stMsg.players_size();
	if (size == 0)
		return;

	const fgame::FriendInfo &playerInfo = stMsg.players(0);
	for (auto it = m_vFriendList.begin(); it != m_vFriendList.end();it ++)
	{
		if ((*it)->m_uId != playerInfo.uin())
			continue;
		
		(*it)->m_uId = playerInfo.uin();
		(*it)->m_sName = playerInfo.nickname();
		(*it)->m_iIcon = playerInfo.icon();
		(*it)->m_iOrder = playerInfo.rank();
		(*it)->m_fWinRate = playerInfo.winrate();
		(*it)->m_iTime = playerInfo.lastonlinetime();
		(*it)->m_iGroup = playerInfo.group();
		(*it)->m_iPlayerLevel = playerInfo.explevel();
		(*it)->m_iExp = playerInfo.exp();
		if (playerInfo.has_lastofflinetime())
			(*it)->m_iLogoutTime = playerInfo.lastofflinetime();
		if (playerInfo.has_score())
			(*it)->m_iScore = playerInfo.score();
		if (playerInfo.has_title())
			(*it)->m_iTitle = playerInfo.title();
		if (playerInfo.has_level())
			(*it)->m_iLevel = playerInfo.level();
	}

	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_INFO_PANLE).asString());
}

#ifdef MGAME_PLATFORM_MPLUS

void FriendControl::updateFriendList(const fgame::CSSycFriendListResp& stMsg)
{
	int size = stMsg.friends_size();
	if (size != 0)
	{
		for (int i = 0; i < size; ++i)
		{
			auto playerInfo = stMsg.friends(i);

			for (auto it = m_vFriendList.begin(); it != m_vFriendList.end(); it++)
			{
				if (playerInfo.openid().compare((*it)->m_sOpenId.c_str()) == 0)
				{
					(*it)->m_uId = playerInfo.uin();
					(*it)->m_sName = playerInfo.nickname();
					(*it)->m_iIcon = playerInfo.icon();
					(*it)->m_iOrder = playerInfo.rank();
					(*it)->m_fWinRate = playerInfo.winrate();
					(*it)->m_iTime = playerInfo.lastonlinetime();
					(*it)->m_iGroup = playerInfo.group();
					(*it)->m_iExp = playerInfo.exp();
					(*it)->m_iPlayerLevel = playerInfo.explevel();
					if (playerInfo.has_lastofflinetime())
						(*it)->m_iLogoutTime = playerInfo.lastofflinetime();
					if (playerInfo.has_score())
						(*it)->m_iScore = playerInfo.score();
					if (playerInfo.has_title())
						(*it)->m_iTitle = playerInfo.title();
					if (playerInfo.has_level())
						(*it)->m_iLevel = playerInfo.level();
				}
			}
		}
	}

	sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_MINE_GET).asString());
}

void FriendControl::addFriendInfo(const std::string extStr)
{
	int _size = atoi(CommFunc::getStrValue(extStr, "BzSize").c_str());

	if (!_size)
		return;

	std::vector<std::string> _vOpenId;

	char valueBegin[16] = { NULL, };
	char valueEnd[16] = { NULL, };

	for (int i = 1; i <= _size; i++)
	{
		sprintf(valueBegin, "<BzBegin%d>", i);
		sprintf(valueEnd, "<BzEnd%d>", i);
		std::string userStr = CommFunc::getStrValue(extStr, valueBegin, valueEnd);

		std::string sOpenId = CommFunc::getStrValue(userStr, "BzUId");

		std::string icon = CommFunc::getStrValue(userStr, "BzJpg");

		CCLOG("[airel]=============== get friend info = %s", sOpenId.c_str());
		bool flag = false;
		for (int i = 0; i < m_vFriendList.size(); ++i)
		{
			if (m_vFriendList[i]->m_sOpenId.compare(sOpenId.c_str()) == 0)
			{
				flag = true;
				m_vFriendList[i]->m_bTPFriend = true;
				m_vFriendList[i]->m_sThirdPartyIcon = icon;
			}
		}
		if (!flag)
		{
			_vOpenId.push_back(sOpenId);
			FriendSystem::PlayerData* data = new FriendSystem::PlayerData();
			data->m_sOpenId = sOpenId;
			data->m_sThirdPartyIcon = icon;
			data->m_bTPFriend = true;
			m_vFriendList.push_back(data);
		}
	}

	//获取到平台好友 不在好友列表的需要去后台请求好友的游戏数据
	if (_vOpenId.size() > 0)
		m_pProxy->sendSycFriendListRequest(_vOpenId);
	return;
}

#endif
