#include "PvpRoomControl.h"
#include "game/net/UserObj.h"
#include "TimeCalculator.h"
#include "PvpControl.h"
#include "game/sync/SyncControl.h"
#include "ui/dialogs/GenerlDialog.h"
#include "game/data/TableManager.h"
#include "chat/ChattingControl.h"
#include "friend/FriendControl.h"
#include "ui/PageFunction.h"

PvpRoomControl::PvpRoomControl() :mRoomID(0), mIsHourse(false), mPlayerNum(0), mPvpRoom(NULL), mRoomMode(ROOMMODE_FOUR), mPvpInvieDialog(NULL)
{
	mPlayerList.clear();
}

PvpRoomControl::~PvpRoomControl()
{

}

void PvpRoomControl::sendCreateRoomRqst()
{	
	CCLOG("[trace]PvpRoomControl::sendCreateRoomRqst");
	g_PvpRoomProxy.sendCreateRoomRequest();
}

void PvpRoomControl::doCreateRoomRsp(const fgame::CSPersonalRoomCreateResp& resp)
{
	CCLOG("[trace]PvpRoomControl::doCreateRoomRsp, roomId:%d", resp.roomid());
	if (resp.roomid() > 0)
	{

		sgSyncControl()->sendSyncPlayerInfoRqst();
		sgMessageManager()->sendMessage(EMT_ENERGY_CHANGE, "");

		//好友容错处理 ，如果好友数量为0 ，请求下获取好友  
		if (sgFriendControl()->getFriendList().size() == 0)
			sgFriendControl()->getProxy()->sendFriendListRequest();

		mPlayerList.clear();
		FriendSystem::PlayerData sPlayerInfo;
		sPlayerInfo.m_uId = sgUserObj()->getUin();
		sPlayerInfo.m_iGroup = sgUserObj()->getRankGroup();
		sPlayerInfo.m_iIcon = sgUserObj()->getPersonalInfo()->iconid();//sgUserObj()->getPlayerIconID();
		sPlayerInfo.m_sName = sgUserObj()->getPlayerName();
		sPlayerInfo.m_iOrder = sgUserObj()->getPersonalInfo()->rank();//sgUserObj()->getRanking();
		sPlayerInfo.m_fWinRate = sgUserObj()->getPersonalInfo()->winrate();//sgUserObj()->getWinRate();
		sPlayerInfo.m_iTitle = sgUserObj()->getPersonalInfo()->playertitle();//sgUserObj()->getPlayerTitle();
		sPlayerInfo.m_iVipLevel = sgUserObj()->getPersonalInfo()->viplevel();//sgUserObj()->getVipLevel();
		mPlayerList.push_back(sPlayerInfo);

		setRoomId(resp.roomid());
		setIsHourse(true);
		sgPvpControl()->setIsHourse(true);
		setPlayerNum(1);
		setRoomMode(ROOMMODE_FOUR);
		sgPageFunction()->gotoFunction(MSG_FUC_PVPROOM);
	}
	else
		CCLOG("[trace]PvpRoomControl::doCreateRoomRsp ,createRoom Fail");

}

void PvpRoomControl::doEnterRoomRsp(const fgame::CSPersonalRoomEnterResp& resp)
{
	CCLOG("[trace]PvpRoomControl::doEnterRoomRsp, roomId:%d", resp.roomid());
	if (resp.roomid() > 0)
	{
		//进入房间成功
		CCLOG("[trace]PvpRoomControl::doEnterRoomRsp, roomInfo Size :%d", resp.players_size());
		
		//自己进入房间，需清空玩家列表
		if (resp.players_size()>1)
			mPlayerList.clear();

		sgPvpControl()->getPvPProxy();

		//好友容错处理 ，如果好友数量为0 ，请求下获取好友   
		if (sgFriendControl()->getFriendList().size() == 0)
		sgFriendControl()->getProxy()->sendFriendListRequest();
	
		FriendSystem::PlayerData sPlayerInfo;
		vector <FriendSystem::PlayerData> tempList;
		for (int i = 0; i != resp.players_size(); ++i)
		{
			const ::fgame::CSPersonalRoomPlayerInfo& roomInfo = resp.players(i);
			sPlayerInfo.m_uId = roomInfo.uin();
			
			//sPlayerInfo.m_sName = getOtherName(roomInfo.name());
			sPlayerInfo.m_sName = roomInfo.name();
			CCLOG("[trace]PvpRoomControl::doEnterRoomRsp name:%s", sPlayerInfo.m_sName.c_str());

			sPlayerInfo.m_iIcon = roomInfo.icon();
			sPlayerInfo.m_iGroup = roomInfo.group();
			sPlayerInfo.m_iOrder = roomInfo.rank();
			sPlayerInfo.m_fWinRate = (float)roomInfo.winrate();
			sPlayerInfo.m_iIndex = roomInfo.index();
			sPlayerInfo.m_iScore = roomInfo.score();
			sPlayerInfo.m_iTitle = roomInfo.title();
			sPlayerInfo.m_iLevel = roomInfo.level();
			sPlayerInfo.m_iVipLevel = roomInfo.viplevel();
			//如果是房主，直接插入 //如果是其他玩家刚进入。则需要排序
			
			if (getIsHourse())
				mPlayerList.push_back(sPlayerInfo);
			else
				tempList.push_back(sPlayerInfo);
		}
		
		if (getIsHourse())
		{
			//如果自己是房主，且有玩家进入 
			setPlayerNum(getPlayerNum() + 1);
			if(mPvpRoom)
				mPvpRoom->showPlayer();
		}
		else
		{
			//如果不是房主。则进入房间.显示玩家,并且刷新服务器上的时间
			if (mPlayerList.size() == 0)
			{
				for (int i = 0; i < tempList.size(); i++)
				{
					for (int j = 0; j < tempList.size(); j++)
					{
						if (tempList[j].m_iIndex == i + 1)
							mPlayerList.push_back(tempList.at(j));
					}
				}
				setRoomMode((eRoomMode)resp.roomtype());
				setRoomId(resp.roomid());
				setPlayerNum(resp.players_size());
				setIsHourse(false);
				sgPvpControl()->setIsHourse(false);
				sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
				sgPageFunction()->gotoFunction(MSG_FUC_PVPROOM);
			}
			else
			{
				setPlayerNum(getPlayerNum() + 1);
				mPlayerList.push_back(tempList.at(0));
				if(mPvpRoom)
					mPvpRoom->showPlayer();
			}
		}
	}
	else
		CCLOG("[trace]PvpRoomControl::doEnterRoomRsp ,enterRoom Fail");
}



void PvpRoomControl::semdChangeRoomModeRqst(eRoomMode mode)
{
	if (sgTimeCalculator()->hasKey("cp_start"))
	{
		if(mPvpRoom)
			mPvpRoom->showTextTip(sgTextHelper(TEXT_PVP_ROOM_10));
	}
	else
	{
		CCLOG("[trace]PvpRoomControl::semdChangeRoomModeRqst");

		//房主才能切换
		if (!sgPvpRoomControl()->getIsHourse())
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_PVP_ROOM_1));
			return;
		}
			

		if (sgPvpRoomControl()->mPlayerList.size() > mode)
		if (mPvpRoom)
		{
			mPvpRoom->showTextTip(sgTextHelper(TEXT_PVP_ROOM_8));
			return;
		}
				
		g_PvpRoomProxy.sendChangeRoomModeRequest(mode);
	}
}

void PvpRoomControl::doChangeRoomModeRsp(const fgame::CSPersonalRoomModeResp &resp)
{
	CCLOG("[trace]PvpRoomControl::doChangeRoomModeRsp, mode:%d",resp.mode());
	if (resp.mode() > 0)
	{
		//改变房间模式成功
		setRoomMode((eRoomMode)resp.mode());

		//如果房间不满人 移除倒计时
		if (getPlayerNum() != getRoomMode())
		{
			if (sgTimeCalculator()->hasKey("cp_start"))
			{
				sgTimeCalculator()->removeTimeCalcultor("cp_start");
				if(mPvpRoom)
					mPvpRoom->getTimeNode()->getParent()->setVisible(false);
			}
		}

		//重新刷新玩家
		if(mPvpRoom)
		{
			mPvpRoom->changeMode();
			mPvpRoom->showPlayer();
		}	
	}
}

void PvpRoomControl::sendLeaveRoomRqst()
{
	CCLOG("[trace]PvpRoomControl::sendLeaveRoomRqst");
	g_PvpRoomProxy.sendLeaveRequest(getRoomId());
}

void PvpRoomControl::doLeaveRoomRsp(const fgame::CSPersonalRoomLeaveResp &resp)
{
	CCLOG("[trace]PvpRoomControl::doLeaveRoomRsp, uin:%llu", resp.uin());
	deletePlayer(resp.uin());
}

void PvpRoomControl::sendKilloffPlayer(int uin)
{
	CCLOG("[trace]PvpRoomControl::sendKilloffPlayer");
	g_PvpRoomProxy.sendKickOffRequest(uin);
}

void PvpRoomControl::doKilloffPlayerRsp(const fgame::CSPersonalRoomKickResp & resp)
{
	CCLOG("[trace]PvpRoomControl::doKilloffPlayer, uin:%llu", resp.uin());
	deletePlayer(resp.uin());
}

void PvpRoomControl::sendInvitePlayer(int uin)
{
	CCLOG("[trace]PvpRoomControl::sendInvitePlayer");
	g_PvpRoomProxy.sendInviteFriendRequest(uin);
}

void PvpRoomControl::doInvitePlayerRsp(const fgame::CSPersonalRoomInviteResp &resp)
{
	CCLOG("[trace]PvpRoomControl::doInvitePlayer, uin:%llu", resp.uin());
	//GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_PVP_ROOM_4));
	if(mPvpRoom)
		mPvpRoom->showTextTip(sgTextHelper(TEXT_PVP_ROOM_4));
}

void PvpRoomControl::sendChallengeRequest()
{
	CCLOG("[trace]PvpRoomControl::sendChallengeRequest");
	g_PvpRoomProxy.sendChallengeRequest();
}

void PvpRoomControl::doChallengeRsp(const fgame::CSPersonalRoomBroadcastResp &resp)
{
	CCLOG("[trace]PvpRoomControl::doChallengeRsp, uin:%llu,name:%s,roomID:%d,time:%llu", resp.uin(), resp.name().c_str(),resp.roomid(),resp.time());
	sgChattingControl()->sendChallengeMsg(resp);
	if (getIsHourse() && mPvpRoom  && resp.uin() == sgUserObj()->getUin())
		mPvpRoom->showTextTip(sgTextHelper(TEXT_PVP_ROOM_6));
	
}

void PvpRoomControl::sendChatRequest(int id )
{
	CCLOG("[trace]PvpRoomControl::sendChatRequest");
	g_PvpRoomProxy.sendChatRequest(id);
}

void PvpRoomControl::doChatRsp(const fgame::CSPersonalRoomChatResp& resp)
{
	CCLOG("[trace]PvpRoomControl::doChatRsp, uin:%llu ,id:%d", resp.uin(),resp.id());
	if(mPvpRoom)
		mPvpRoom->showChat(resp.uin(), resp.id());
}

void PvpRoomControl::sendBeginFightRequest()
{
	CCLOG("[trace]PvpRoomControl::sendBeginFightRequest");
	g_PvpRoomProxy.sendBeginFightRequest();
}

void PvpRoomControl::doBeginFightRsp(const fgame::CSPersonalRoomBeginFightResp &resp)
{
	CCLOG("[trace]PvpRoomControl::doBeginFightRsp, mapId:%d", resp.roomid());
	if(mPvpRoom)
		mPvpRoom->setShow(false);
}

void PvpRoomControl::deletePlayer(int uin)
{
	if (mPlayerList.size() == 0 || mPlayerList.empty() )
		return;

	for (auto it = mPlayerList.begin(); it != mPlayerList.end(); it++)
	{
		if (uin == it->m_uId)
		{
			if (sgTimeCalculator()->hasKey("cp_start"))
			{
				sgTimeCalculator()->removeTimeCalcultor("cp_start");
				if(mPvpRoom)
					mPvpRoom->getTimeNode()->getParent()->setVisible(false);
			}

			if (it->m_uId != sgUserObj()->getUin())
			{
				//其他玩家离开房间。移除对应玩家
				mPlayerList.erase(it);
				setPlayerNum(getPlayerNum() - 1);
				//并重新刷新玩家
				if(mPvpRoom)
					mPvpRoom->showPlayer();
	
				break;
			}
			else if (it->m_uId == sgUserObj()->getUin())
			{
				//自己被T啦
				if(mPvpRoom)
					mPvpRoom->setShow(false);
				// change by jim , unify tips dialog
				//GeneralDialogOther::show(sgTextHelper(TEXT_PVP_ROOM_5));
				GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_PVP_ROOM_5));
				mPlayerList.erase(it);
				break;
			}

		}

	}
}

void  PvpRoomControl::sendEnterRoomRequest(int roomid,std::string serverNode)
{
	g_PvpRoomProxy.sendEnterRoomRequest(roomid,serverNode);
};

void PvpRoomControl::doCountDownRsp(const fgame::CSPersonalRoomCountDownResp &resp)
{
	CCLOG("[trace]PvpRoomControl::doCountDownRsp, roomId:%d,seconds:%d", resp.roomid(), resp.seconds());
	sgTimeCalculator()->createTimeCalcultor("cp_start",resp.seconds());
}

void PvpRoomControl::showTip(std::string str)
{
	if(mPvpRoom)
		mPvpRoom->showTextTip(str.c_str());
}
