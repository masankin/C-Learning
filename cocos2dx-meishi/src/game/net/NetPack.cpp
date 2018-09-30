#include "NetPack.h"
#include "EnvSet.h"
#include "NetManager.h"
#include "BlowFishCrypt.h"
#define _NETPACK_CHECKSUM_ 1

bool MsgHead::SerializeToArray(void* data, int size, int plainSize)
{
	char* pBuffer = (char*)(data);

	//1. ms
	size += (NET_HEAD_LEN - 2);//包头长-长度标示

	//2. checksum len
#ifdef _NETPACK_CHECKSUM_

	size += NET_CHECKSUM_LEN;
#endif

	mSize = size;

	//add by weihua
	if (sgNetManager()->getEncryMsg()){
		mReserved = plainSize;
	}
	//1. SIZE
	pBuffer[0] = (size >> 8);
	pBuffer[1] = (size & 0xFF);

	//2. MSGID
	pBuffer[2] = (mMsgID >> 8);
	pBuffer[3] = (mMsgID & 0xFF);

	//3. version
	pBuffer[4] = (mVersion >> 8);
	pBuffer[5] = (mVersion & 0xFF);

	//4. reserved
	pBuffer[6] = (mReserved >> 8);;
	pBuffer[7] = (mReserved & 0xFF);;
	return true;
}

bool MsgHead::ParseFromArray(const char* pMsgBuff)
{
	mSize = (unsigned char)pMsgBuff[1] + (unsigned char)pMsgBuff[0] * pow(2, 8);
	// 注意字节序，高位+低位
	mMsgID = (unsigned char)pMsgBuff[3] + (unsigned char)pMsgBuff[2] * pow(2, 8);
	// 注意字节序，高位+低位
	if (sgNetManager()->getEncryMsg() == true)
	{
		mReserved = (unsigned char)pMsgBuff[5] + (unsigned char)pMsgBuff[4] * pow(2, 8);
	}
	return true;
}

void MsgHead::Clear()
{
	mMsgID = 0;
	mSize = 0;
}

MsgHead::MsgHead()
{
	Clear();

	mVersion = PROTOBUF_CODE;
	mReserved = 1;
}

NetPack::NetPack() : m_decryptBuff(nullptr), m_encryptBuff(nullptr){
}
NetPack::~NetPack(){
	if (m_encryptBuff)
	{
		delete[]m_encryptBuff;
	}
	m_encryptBuff = nullptr;
	if (m_decryptBuff)
	{
		delete[]m_decryptBuff;
	}
	m_decryptBuff = nullptr;
}

unsigned char* NetPack::getEncryptBuf()
{
	if (m_encryptBuff == nullptr)
	{
		m_encryptBuff = new unsigned char[BUF_SIZE]; 
		memset(m_encryptBuff, 0, BUF_SIZE);
	}
	return m_encryptBuff;
}

unsigned char* NetPack::getDecryptBuf()
{
	if (m_decryptBuff == nullptr)
	{
		m_decryptBuff = new unsigned char[BUF_SIZE];
		memset(m_decryptBuff, 0, BUF_SIZE);
	}
	return m_decryptBuff;
}

bool NetPack::ParseFromArray(const char* pMsgBuff, unsigned int iLen)
{
	// 解析消息头
	mMsgHead.ParseFromArray(pMsgBuff);
	bool bRet = false;
	//modify by weihua.cui
	if (sgNetManager()->getEncryMsg() == false)
	{
		// 解析protoBuffer (-4:减去后面追加的版本号和保留字段的长度）
		bRet = mSCMsg.ParseFromArray(pMsgBuff + NET_HEAD_LEN - 4, mMsgHead.mSize - NET_ID_LEN);

		// 解析proto checkSum
		// #ifdef _NETPACK_CHECKSUM_
		// 
		// 	const char* pChecksum = pMsgBuff + NET_ID_LEN + mSCMsg.ByteSize();
		// 	unsigned int checkSum = (unsigned char)pChecksum[3] + (unsigned char)pChecksum[2] * pow(2, 8) +
		// 		(unsigned char)pChecksum[1] * pow(2, 16) + (unsigned char)pChecksum[0] * pow(2, 24);
		// 
		// #endif
	}
	else
	{
		unsigned char* decryptBuf = getDecryptBuf();
		unsigned int decryptSize = mMsgHead.mReserved;
		int encryptSize = mMsgHead.mSize - 4;
	
		

		sgBlowfishManager()->decrypt(encryptSize, (unsigned char*)pMsgBuff + NET_HEAD_LEN - 2, decryptBuf);

		bRet = mSCMsg.ParseFromArray(decryptBuf, decryptSize);
	}
	return bRet;
}

bool NetPack::SerializeToArray(void* data, int size)
{
	int bytes = mCSMsg.ByteSize();
	if (sgNetManager()->getEncryMsg() == false)
	{

		// 序列化消息头
		mMsgHead.SerializeToArray(data, bytes);
		// 序列化protoBuffer
		mCSMsg.SerializeToArray((char*)data + NET_HEAD_LEN, bytes);
	}
	else
	{
		unsigned char* encryptBuf = getEncryptBuf();
		mCSMsg.SerializeToArray(encryptBuf, bytes);
		int encryptSize = sgBlowfishManager()->getEncryptSize(bytes);
		mMsgHead.SerializeToArray(data, encryptSize, bytes);
		sgBlowfishManager()->encrypt(bytes, encryptBuf, (unsigned char*)data + NET_HEAD_LEN);
	}

	int encryptSize = sgBlowfishManager()->getEncryptSize(bytes);
// 序列化proto checkSum
#ifdef _NETPACK_CHECKSUM_

	char* pBuffer = (char*)data + NET_HEAD_LEN - 4;
	unsigned int checkSum = adler32(pBuffer, bytes + 4);
	//CCLOG("[trace] caculate check sum:%u.", checkSum);
	if (sgNetManager()->getEncryMsg() == false)
	{
		pBuffer = (char*)data + NET_HEAD_LEN + bytes;
	}
	else
	{
		int encryptSize = sgBlowfishManager()->getEncryptSize(bytes);
		pBuffer = (char*)data + NET_HEAD_LEN + encryptSize;
	}
	pBuffer[0] = (checkSum >> 24);
	pBuffer[1] = (checkSum >> 16);
	pBuffer[2] = (checkSum >> 8);
	pBuffer[3] = (checkSum & 0xFF);
#endif
	return true;
}

int NetPack::csByteSize() const
{
	int packSize = 0;
	if (sgNetManager()->getEncryMsg() == false)
	{
		packSize = NET_HEAD_LEN + mCSMsg.ByteSize();
	}
	else
	{
		packSize = NET_HEAD_LEN + sgBlowfishManager()->getEncryptSize(mCSMsg.ByteSize());
	}

#ifdef _NETPACK_CHECKSUM_
	packSize += NET_CHECKSUM_LEN;
#endif

	return packSize;
}

int NetPack::scByteSize() const
{
	if (sgNetManager()->getEncryMsg() == false)
	{
		return NET_HEAD_LEN + mSCMsg.ByteSize() - 4;//4bytes(version+reserved)
	}
	else
	{
		return NET_HEAD_LEN + sgBlowfishManager()->getEncryptSize(mSCMsg.ByteSize()) - 2;//4bytes(version+reserved)
	}

}

void NetPack::Clear()
{
	mMsgHead.Clear();
	mSCMsg.Clear();
	mCSMsg.Clear();
}

unsigned int NetPack::adler32(const char* buf, int len) const
{
	//adler32
#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
	/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

	unsigned int s1 = 1;
	unsigned int s2 = 0;
	int k;

	if (buf == NULL) return 1;

	while (len > 0) {
		k = len < NMAX ? len : NMAX;
		len -= k;
		while (k >= 16) {
			DO16(buf);
			buf += 16;
			k -= 16;
		}
		if (k != 0) do {
			s1 += *buf++;
			s2 += s1;
		} while (--k);
		s1 %= BASE;
		s2 %= BASE;
	}
	return (s2 << 16) | s1;
}
