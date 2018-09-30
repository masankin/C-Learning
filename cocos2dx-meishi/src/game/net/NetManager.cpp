#include "NetManager.h"
#include "Constant.h"
#include "HelperFunc.h"
#include "game/data/TableManager.h"
#include "TimeCalculator.h"
#include "game/ui/PageFunction.h"
#include "ui/dialogs/GenerlDialog.h"
#include "pvp/PvpControl.h"
#include "battle/BattleControl.h"
#include "core/TimeCalculator.h"


#include "zlib.h"


NetManager::NetManager() : mEncryMsg(false)
{
	connectingData = new ConnectingEvent();
	//connectingData->autorelease();
}

NetManager::~NetManager() 
{
	connectingData->release();
} 

void NetManager::RecvPkg(void* pBufffer, int len)
{
	sgSession()->recvPkg();

	unsigned char* pPacket = NULL;
	RecvBuffer* pRecvBuff = sgSession()->getRecvBuffer();

	while (pPacket = pRecvBuff->getFirstPacketPtr())
	{
		NetPack netpack;
		bool bParseRes = netpack.ParseFromArray((const char*)pPacket, 0);
		if (!bParseRes)
		{
			int msgID = netpack.getMsgHead()->mMsgID;
			CCLOG("[error]%s,parse VLogicSvr2CliMsg fail! cmd=%d", __FUNCTION__, msgID);
#ifndef MGAME_IN_APP_SDK
			std::string szTitle = "fault error";
			std::string szContent = "proto parse fail! cmd=" + HelperFunc::int2String(msgID);
			GenerlDialog::show(szTitle, szContent);
#endif
			break;
		}
		else{

			int msgID = netpack.getMsgHead()->mMsgID;
			// 判断网络回包，取消菊花用
			if (connectingData->getCurrTaskId() == msgID){
				connectingData->setIsAccepted(true);
			}
			sgLoginProxy()->setIsAccepted(true);
			DoOneCmd(netpack);

			std::map<int, std::list<NetPack> >::iterator targetIter = m_stMsgCacheMap.find(msgID);

			if (targetIter != m_stMsgCacheMap.end()) {
				std::list<NetPack> pNetPack = targetIter->second;
				pNetPack.pop_front();
				if (pNetPack.size() == 0)
				{
					m_stMsgCacheMap.erase(targetIter);
				}
			}
			pRecvBuff->removeFirstPacket(2 + netpack.getMsgHead()->mSize);
		}
	}
}

void NetManager::SendPkg(NetPack& stRqst, bool bIsCheckRecv, bool bIsBlock, bool isOnlyOnce)
{
	int nMsgID = stRqst.getMsgHead()->mMsgID;
	// 协议只发给对应的服务器
	if (checkCmd2Server(nMsgID) == false)
		return;
	if (bIsCheckRecv)
	{
		auto it =  m_stMsgCacheMap.find(nMsgID);
		if (it == m_stMsgCacheMap.end())
		{
			std::list<NetPack> pNetPack;
			pNetPack.push_back(stRqst);
			m_stMsgCacheMap.insert(std::make_pair(nMsgID,pNetPack));
		}else
		{
			if (isOnlyOnce)
			{
				return;
			}
			it->second.push_back(stRqst);
			//return;//
		}
	}

	int bufsize = stRqst.csByteSize();
	char* pSendBuf = new char[bufsize];
	memset(pSendBuf, 0, bufsize);

	//memset(m_acCodeBuff, 0, sizeof(char)* BUF_SIZE);

	bool bCodeRslt = stRqst.SerializeToArray(pSendBuf, sizeof(pSendBuf));
	if (!bCodeRslt)
	{
		CCLOG("[error]fail to encode message %d!", nMsgID);
		return;
	}

	Session::PacketData data;
	data.buffer = pSendBuf;
	data.length = bufsize;

	// 设置连网锁屏
	if (bIsBlock) {
		connectingData->resetEvent();
		connectingData->setCurrTaskId(nMsgID);
		sgPageFunction()->gotoFunction(MSG_FUC_CONNECTING);
	}

	sgSession()->sendPacket(data, nMsgID);
}

void NetManager::SendCachePack()
{
	for (auto it = m_stMsgCacheMap.begin(); it!= m_stMsgCacheMap.end(); it++)
	{
		std::list<NetPack> pNetPack = it->second;
		for (auto netit = pNetPack.begin(); netit!=pNetPack.end(); netit++)
		{
			SendPkg(*netit);
		}
		
	}
}
void NetManager::clearCachePack()
{
	m_stMsgCacheMap.clear();
}

int NetManager::RegCmdListener(const int iCmd, BaseProxy *pstProxy, bool isUpdateFunc)
{
	if(NULL == pstProxy) {
		CCLOG("[error]NetManager::RegCmdListener, Fail to reg cmd target %d, pstProxy is null", iCmd);
        return -1;
	}

	std::map<int, BaseProxy*>::const_iterator targetIter = m_stCmdProxyMap.find(iCmd);
	if (targetIter == m_stCmdProxyMap.end())
    {
        //reg
        m_stCmdProxyMap[iCmd] = pstProxy;
        CCLOG("[trace]reg cmd %d -> %s success", iCmd, typeid(*pstProxy).name());
		return 0;
    }

    if (targetIter != m_stCmdProxyMap.end() && pstProxy != targetIter->second)
    {
        CCLOG("[warning]cmd %d's function now is %s,tobe reg is %s", iCmd, typeid(*((targetIter->second))).name(), typeid(*pstProxy).name());
        if (isUpdateFunc)
        {
            m_stCmdProxyMap[iCmd] = pstProxy;
            CCLOG("[trace]update cmd %d -> %s", iCmd, typeid(*pstProxy).name());
        }
    }
    return 0;
}

int NetManager::InitNetConnection(const std::string &strIP, unsigned short uPort) 
{
	sgSession()->connect(strIP, uPort);
	return 0;
} 

void NetManager::DoOneCmd(NetPack& stRsp)
{
	const int iCmd = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]NetManager::DoOneCmd cmdid=%d,errorcode=%d", iCmd, stRsp.getS2CMsg()->error_msg().errcode());

		unsigned int uRqstCmdID = stRsp.getMsgHead()->mMsgID;
		std::map<int, BaseProxy*>::iterator rqstTargetIter = m_stCmdProxyMap.find(uRqstCmdID);
		if (rqstTargetIter != m_stCmdProxyMap.end())
		{
            BaseProxy *pListener = rqstTargetIter->second;
            if (NULL != pListener)
            {
				if (pListener->fault(stRsp.getS2CMsg()->error_msg()))
				{
					int erroCode = stRsp.getS2CMsg()->error_msg().errcode();

					const ErrorCode* pText = sgTableManager()->getErrorCodeByID(erroCode);
					if (pText)
						GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), pText->text);
					else
						CCLOG("[warning]NetManager::DoOneCmd,not find erroCode:%d", erroCode);
				}
			}
		}
		return;
	}

	std::map<int, BaseProxy*>::iterator targetIter = m_stCmdProxyMap.find(iCmd);
    if (targetIter == m_stCmdProxyMap.end())
    {
		CCLOG("[warning]NetManager::DoOneCmd,not find cmd %d's function", iCmd);
    }
    else
    {
		BaseProxy *pListener = targetIter->second;
        if (NULL != pListener)
        {
			if (iCmd != fgame::CS_CMD_HEART_BEAT)
				CCLOG("[trace]NetManager::DoOneCmd recv packet. cmd:%d size:%d!", iCmd, stRsp.scByteSize());
            pListener->DoRsp(stRsp);		
        }
    }
}

void NetManager::GenRqstMsgHead(MsgHead& stHead, const int iCmd, const int uUin, bool bIsRestSeq/* = false*/,  bool bIsUpdateSeq/* = true*/)
{ 
	/*
	* 消息格式
	|					|					|									  |
	|	   size(2)		|		msgid(2)	|			protoMsg(N)				  |
	|___________________|___________________|_____________________________________|
	*@size = msgid(2) + protoMsg(N)
	*@msgid 消息id 
	*/

	stHead.mMsgID = iCmd; 
}

void NetManager::GenRqstMsgHead(MsgHead& stHead, const int iCmd)
{
    GenRqstMsgHead(stHead, iCmd, 0, false);
}

void NetManager::delCmdListener(const int iCmd)
{
    if(m_stCmdProxyMap.count(iCmd)) 
        m_stCmdProxyMap.erase(iCmd);
}

bool NetManager::IsConnect() 
{
	return sgSession()->getConnOk();
}

void NetManager::DisConnect()
{
	sgSession()->disconnect();
	//sgLoginProxy()->setServerState(LoginProxy::E_SS_INVALID);
}

void NetManager::ReConnect()
{
	sgSession()->reconnect();
}

bool NetManager::checkCmd2Server(int cmdid)
{
	bool bCheck = false;

	// 当前服务器状态, 过滤心跳包
	LoginProxy::eServerState ess = sgLoginProxy()->getServerState();
	if (ess == LoginProxy::E_SS_INVALID || cmdid == fgame::CS_CMD_HEART_BEAT)
		return true;

	// 协议归属服务器
	LoginProxy::eServerState e2s = LoginProxy::E_SS_INVALID;
	switch (cmdid / 100)
	{
	case 2:
		bCheck = true;
		e2s = LoginProxy::E_SS_CENTER;
		break;
	default:
	{
	   if (cmdid == fgame::CS_CMD_LOGIN || cmdid == fgame::CS_CMD_REQUEST_PKEY || cmdid == fgame::CS_CMD_SEND_CLIENT_KEY)
	   {
		   if (ess == LoginProxy::E_SS_CENTER || ess == LoginProxy::E_SS_LOGIN)
		   {
			   bCheck = true;
			   e2s = LoginProxy::E_SS_LOGIN;
		   }
	   }
	   else{
		   if (ess == LoginProxy::E_SS_LOGIN || ess == LoginProxy::E_SS_GAME)
		   {
			   bCheck = true;
			   e2s = LoginProxy::E_SS_GAME;
		   }
	   }
	   break;
	}
	}

	if (bCheck == false)
		CCLOG("[error]NetManager::checkCmd2Server curState:%d cmdid:%d-tarState:%d!", (int)ess, cmdid, (int)e2s);
	return bCheck;
}
