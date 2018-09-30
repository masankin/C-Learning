#pragma once

#include "net/socket.h"
#include "net/concurrency.h"
#include "cocos2d.h"
#include "net/recvbuffer.h"

using namespace GameCom;

class Session
{
public:

	struct PacketData
	{
		void* buffer;
		int length;
	};

protected:

	GameCom::AsyncSocket mSocket;

 	// 消息接收/发送缓存
 	RecvBuffer* mRecvBuff;


	bool mConnectionOk;

	std::string mIp;
	int mPort;

public:

	Session();
	~Session();

	void setConnOk(bool connOK) { mConnectionOk = connOK; };
	bool getConnOk(){ return mConnectionOk; }

	void connect(const std::string& ip, int port);
	void reconnect();
	void disconnect();
	void sendPacket(PacketData data, int opcode);
	void recvPkg();

	std::string getCurrConnIp() { return std::string(mIp + ":").append(__String::createWithFormat("%d",mPort)->getCString()); }

	RecvBuffer* getRecvBuffer(){ return mRecvBuff; }
};

#define sgSession() Singleton<Session>::Get()
