#pragma once

#include "net/proto/ClientSvrMsg.pb.h"

#include "cocos2d.h"
USING_NS_CC;

#define NET_ID_LEN 2
#define NET_HEAD_LEN 8
#define NET_CHECKSUM_LEN 4

class MsgHead
{
public:
	MsgHead();

	void Clear();
	bool	ParseFromArray(const char* pMsgBuff);

	//@size proto的长度
	bool	SerializeToArray(void* data, int size, int plainSize = 0);


	unsigned short mSize;
	unsigned short mMsgID;
	unsigned short mVersion;	//协议版本
	unsigned short mReserved; //保留字段

};


class NetPack
{
protected:

	MsgHead mMsgHead;
	fgame::S2ClientMsg	mSCMsg;
	fgame::C2ServerMsg mCSMsg;

	enum {
		BUF_SIZE = 65535,
	};
	unsigned char* m_encryptBuff;
	unsigned char* m_decryptBuff;
public:

	NetPack();
	~NetPack();

	unsigned char* getEncryptBuf();
	unsigned char* getDecryptBuf();

	MsgHead* getMsgHead() { return &mMsgHead; }

	fgame::S2ClientMsg*		getS2CMsg(){ return &mSCMsg; }
	fgame::C2ServerMsg*		getC2SMsg(){ return &mCSMsg; }
	

	int csByteSize() const;
	int scByteSize() const;

	void Clear();

	bool	ParseFromArray(const char* pMsgBuff, unsigned int iLen);
	bool	SerializeToArray(void* data, int size);

protected:

	unsigned int adler32(const char* buf, int len) const;
};
