#include "Session.h"

#include "net/NetManager.h"

#include "cocos2d.h"
USING_NS_CC;

using namespace GameCom;

Session::Session()
{
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData))
	{
		WSACleanup();
	}
#endif 

	mRecvBuff = new RecvBuffer();
	mRecvBuff->create(1024 * 1024);
}

Session::~Session()
{
	if (mRecvBuff)
		delete mRecvBuff;
#ifdef WIN32
	WSACleanup();
#endif
}

void Session::connect(const std::string& ip, int port)
{
	mIp = ip;
	mPort = port; 

	mSocket.onClose();

	if (!mSocket.onCreate())
	{
		CCLOG("[error]Session::connect ip:%s port:%d socket create error!", ip.c_str(), port);
		return ;
	}

	if (!mSocket.onConnect(mIp.c_str(), mPort))
	{
		CCLOG("[error]Session::connect ip:%s port:%d socket create error!", ip.c_str(), port);
		return;
	}

	setConnOk(true);
	CCLOG("[trace]Session::connect ip:%s port:%d success!", ip.c_str(), port);
}

void Session::reconnect()
{
	connect(mIp, mPort);
}

void Session::sendPacket(PacketData data, int opcode)
{
	int iRet = mSocket.onSend(data.buffer, data.length);
	if (iRet == -1)
	{
		CCLOG("[warn]Session::sendPacket ipcode:%d send error:%d!", opcode, iRet);
		return;
	}else if (iRet == -2)
	{
		setConnOk(false);
		CCLOG("[warn]Session::sendPacket socket exception,disconnect!");
	}

	CCLOG("[trace]Session::sendPacket, msgID:%d len:%d send:%d!", opcode, data.length, iRet);
}

void Session::disconnect()
{
	mSocket.onClose();
	setConnOk(false);
	CCLOG("[trace]Session::disconnect ip:%s port:%d success!", mIp.c_str(), mPort);
}

void Session::recvPkg()
{
	unsigned char* mBuffer = NULL;
	int nLast = 0;

	
	mRecvBuff->getRecvParam(&mBuffer, nLast);

	int nRet = mSocket.onReceive(mBuffer, nLast, 0);
	if (nRet > 0)
	{
		mRecvBuff->completion(nRet);
		CCLOG("[trace]Session::recvPkg socket recv len:%d success!", nRet);
	}
}
