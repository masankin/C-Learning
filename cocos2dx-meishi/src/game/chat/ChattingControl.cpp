
#include "ChattingControl.h"
#include "core/WordChecker.h"
#include "utils/Comm.h"
#include "Constant.h"
#include "core/StringLabel.h"
#include "data/TableManager.h"
#include "MessageManager.h"
#include "core/TimeCalculator.h"
#include "InfoPanel.h"
#include "ui/PageFunction.h"
ChatInfo::ChatInfo()
{
	iconID = "0";
	mContent = "";
	mName = "";
	mUin = 0;
	mMsgType = MSG_TYPE_WORLD; 
	mTitle = 0;
	mContentType = ContentType_Normal;
	mVipLevel = 0;
	Node::onEnter();
}

ChatInfo::~ChatInfo()
{
	Node::onExit();
}

void ChatInfo::update(float delta)
{
	mMsgType = MSG_TYPE_NULL;
}

void ChatInfo::setSchedule()
{
	this->scheduleOnce(schedule_selector(ChatInfo::update),Msg_Invite_Time);
}

ChattingControl::ChattingControl():
mChattingDialog(0),
mChatButton(0),
mFullSrvInviteBatTips(0),
mPVPInviteChatInfoPtr(0)
{

}

ChattingControl::~ChattingControl()
{
	sgUserObj()->saveToFile();
	clearHistory();
	if (mChatButton)
		mChatButton->release();
}


std::string ChattingControl::analyzeExpression(std::string sText)
{
	int tBeginIndex = 0;
	int tEndIndex  = 0; 
	tBeginIndex = sText.find("/",0);
	tEndIndex = sText.find("#",tBeginIndex);
	while(tBeginIndex >= 0 && tEndIndex >= 0)
	{
		if(tBeginIndex >= 0 && tBeginIndex < tEndIndex)
		{
			std::string t0 = sText.substr(0, tBeginIndex);
			std::string t1 = __String::createWithFormat("%s",sText.substr(tBeginIndex + 1, tEndIndex - tBeginIndex - 1).c_str())->getCString();
			std::string t2 = __String::createWithFormat("%s", tEndIndex < sText.size() ?  sText.substr(tEndIndex + 1, sText.size() - tEndIndex - 1).c_str() : "")->getCString();
			sText =t0 + "\\p[emoji_(" + t1 + ").png]"+t2;  
			tBeginIndex = sText.find("/",0);
			tEndIndex = sText.find("#",tBeginIndex);
		} 
	} 

	return sText;
}

void ChattingControl::addMessage(const fgame::CSPvpChatResp& stMsg)
{
	ChatInfo* _chatinfo = new ChatInfo();
	_chatinfo->iconID = stMsg.icon();
	_chatinfo->mMsgType = stMsg.channel();
	_chatinfo->mUin = stMsg.uin();
	_chatinfo->mVipLevel = stMsg.viplevel();

	if (_chatinfo->mUin == sgUserObj()->getUin() && _chatinfo->mContentType != ContentType_Challenge){
		
		if (!sgTimeCalculator()->hasKey(Chat_Text))
		{
			sgTimeCalculator()->createTimeCalcultor(Chat_Text, Msg_Chat_Time);
		}
		//if (sgUserObj()->getChatFreeCount() > 0)
		//{
		//	sgUserObj()->setChatFreeCount(sgUserObj()->getChatFreeCount() - 1);
		//}
			
		if (mChattingDialog)
		{
			mChattingDialog->startUpdateTime();
		}
	}
		

	if (_chatinfo->mMsgType == MSG_TYPE_PVP)
	{
		PvpChat* _ch = sgTableManager()->getPvpChat(stMsg.id());
		if(_ch == NULL){
			delete _chatinfo;
			return;
		}
		else
		{
			_chatinfo->mContent = _ch->mText;
			chatPvpHty.insert(chatPvpHty.begin(),_chatinfo);
		}
	}
	else
	{
		_chatinfo->mName = stMsg.name();
		_chatinfo->mTitle = stMsg.title();

		std::string str = stMsg.content();	
		if (str.find(".gif") == -1)
		{
			str = WordChecker::Get()->replaceSensitiveWord(str,"*",1); 
			str = analyzeExpression(str);				
		}
		else
		{
			_chatinfo->mContentType = ContentType_Emoji;
		}
		_chatinfo->mContent = str;

		chatWorldHty.push_back(_chatinfo);
		if (chatWorldHty.size() > 30){
			CC_SAFE_DELETE(chatWorldHty.front());
			chatWorldHty.erase(chatWorldHty.begin());
		}
		if (mChattingDialog)
		{
			mChattingDialog->addMessage();
		}
	} 
	sgMessageManager()->sendMessage(EMT_CHAT_MSG,StringUtils::format("%d",stMsg.channel()));
}

std::vector<ChatInfo*>* ChattingControl::getchatPvpHty()
{
    return &chatPvpHty;
}
std::vector<ChatInfo*>* ChattingControl::getChatWorldHty()
{
    return &chatWorldHty;
}

void ChattingControl::clearHistory(){

	for (int i = 0; i < chatPvpHty.size(); i++)
	{
		CC_SAFE_DELETE(chatPvpHty.at(i));
	}
	for (int i = 0; i < chatWorldHty.size(); i++)
	{
		CC_SAFE_DELETE(chatWorldHty.at(i));
	}

    chatPvpHty.clear();
    chatWorldHty.clear();
}

void ChattingControl::setExpression(int id, int isBuy)
{
	mChattingDialog->setChannel(true);
	std::string str = StringUtils::format("emoji_(%d).gif",id);
	mChattingProxy.sendChattingRpst(MSG_TYPE_WORLD,str,isBuy);
}

void ChattingControl::sendChallengeMsg(const fgame::CSPersonalRoomBroadcastResp &resp)
{
	ChatInfo* _chatinfo = new ChatInfo();
	_chatinfo->iconID = resp.icon();
	_chatinfo->mContent = sgTextHelper(TEXT_CHAT_INVITE);
	_chatinfo->mContentType = ContentType_Challenge;
	_chatinfo->mName = resp.name();
	_chatinfo->mUin = resp.uin();
	_chatinfo->mMsgType = resp.roomid();
	_chatinfo->mServerNode = resp.servernode();
	_chatinfo->mTitle = resp.title();
	_chatinfo->mVipLevel = resp.viplevel(); 
	_chatinfo->setSchedule();

	chatWorldHty.push_back(_chatinfo);
	if (chatWorldHty.size() > 30){
		CC_SAFE_DELETE(chatWorldHty.front());
		chatWorldHty.erase(chatWorldHty.begin());
	}

	if (mChattingDialog)
	{
		mChattingDialog->addMessage();
	}

	sgMessageManager()->sendMessage(EMT_CHAT_MSG,StringUtils::format("%d",MSG_TYPE_WORLD));
	if (_chatinfo->mUin == sgUserObj()->getUin()) return;
	if (mPVPInviteChatInfoPtr)
	{
		//delete mPVPInviteChatInfoPtr;
		mPVPInviteChatInfoPtr = NULL;
	}
	mPVPInviteChatInfoPtr = _chatinfo;

	sgTimeCalculator()->createTimeCalcultor(TIME_PVP_FullSrv_Invite, Chat_PVP_FullSrv_Invite_TimeOut);

	if (mFullSrvInviteBatTips)
	{
		if (mPVPInviteChatInfoPtr->mUin != sgUserObj()->getUin())
			mFullSrvInviteBatTips->updateUI(mPVPInviteChatInfoPtr);
	}
}

ChattingButton* ChattingControl::getChattingButton(bool flag)
{
	if (!mChatButton)
	{
		mChatButton = ChattingButton::create();
	}
	else
	{
		mChatButton->removeFromParentAndCleanup(false);
	}
	mChatButton->resetUI(flag);

	if (sgEnvSet("MGAME_CHAT_OPEN") == 0)
		mChatButton->setVisible(false);

	return mChatButton;
}


FullSrvInviteBatTips* ChattingControl::getFullSrvInviteBatTips(bool flag)
{
	if (!mFullSrvInviteBatTips)
	{
		mFullSrvInviteBatTips = FullSrvInviteBatTips::create();
	}
	else
	{
		mFullSrvInviteBatTips->removeFromParentAndCleanup(false);
	}

	if (mPVPInviteChatInfoPtr)
	{
		if (mPVPInviteChatInfoPtr->mUin != sgUserObj()->getUin())
		{
			mFullSrvInviteBatTips->updateUI(mPVPInviteChatInfoPtr);
		}
	}
	else
		mFullSrvInviteBatTips->setVisible(false);
	/*if (sgEnvSet("MGAME_CHAT_OPEN"))
		mFullSrvInviteBatTips->setVisible(false);*/

	return mFullSrvInviteBatTips;
}

void ChattingControl::showPlayerInfo(const fgame::CSQueryPlayerInfoResp &resp)
{
	//InfoPanel* pDdialog = dynamic_cast<InfoPanel*>(sgPageFunction()->createWindow(MSG_FUC_INFO,"InfoPanel.csb"));
	InfoPanel* pDdialog = dynamic_cast<InfoPanel*>(sgPageFunction()->getWindowByPageName("InfoPanel.csb"));
	if (pDdialog)
	{
		pDdialog->init(resp);
		pDdialog->setShow(true);
	}
}

void ChattingControl::chatDataReset(int count, int left)
{
	sgUserObj()->setChatFreeCount(count);
	if (left < 0 || left > Msg_Chat_Time) left = Msg_Chat_Time;
	if (count < Msg_Free_Max && left == 0) left = Msg_Chat_Time;
	if (count >= Msg_Free_Max && left != 0) left = 0;

	sgTimeCalculator()->createTimeCalcultor(Chat_Text, left);

	if (mChattingDialog)
	{
		mChattingDialog->startUpdateTime();
	}
	
}

void ChattingControl::calculateFreeCount()
{
	int64_t lastTime = sgUserObj()->getChatTimeLeft();
	if (!lastTime || !CVTimeUtils::IsSameDay(lastTime, time(NULL))){
		//sgUserObj()->setChatFreeCount(Msg_Free_Max);
		sgUserObj()->setChatBuyTip(1);
		sgUserObj()->setChatTimeLeft(time(NULL));
	}

	return;

	long long curTime = (long long)time(NULL);
	unsigned int ltime = curTime - lastTime;

	int count =  (ltime / Msg_Chat_Time);

	if (count > 0)
	{
		int freeConut = count + sgUserObj()->getChatFreeCount();
		freeConut = freeConut > Msg_Free_Max ? Msg_Free_Max : freeConut;
		sgUserObj()->setChatTimeLeft(curTime);
		sgUserObj()->setChatFreeCount(freeConut);
	}

	if (sgUserObj()->getChatFreeCount() < Msg_Free_Max)
	{
		ltime = Msg_Chat_Time - (ltime - count * Msg_Chat_Time);
		if (ltime > 0)
		{
			sgTimeCalculator()->createTimeCalcultor(Chat_Text, ltime);
		}
	}	
}
