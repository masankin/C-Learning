#pragma once

#include "circuitqueue.h"
#include "net/NetPack.h"

class RecvBuffer
{

private:
	CircuitQueue<unsigned char>	*m_pQueue;



public:

	RecvBuffer() : m_pQueue(NULL)
	{

	}

	~RecvBuffer()
	{
		CCLOG("[trace]RecvBuffer destroy...");
		destroy();
	}


	inline void		destroy()
	{
		if (m_pQueue)
			delete m_pQueue;
	}

	inline void	create(int nBufferSize)
	{
		m_pQueue = new CircuitQueue<unsigned char>;
		m_pQueue->create(nBufferSize);
	}

	inline void	completion(int nBytesRecvd)
	{
		m_pQueue->enqueue(NULL, nBytesRecvd);
		CCLOG("[trace]RecvBuffer::completion end, bytes:%d!", nBytesRecvd);
	}

	inline void	getRecvParam(unsigned char **ppRecvPtr, int &nLength)
	{
		*ppRecvPtr = m_pQueue->getWritePtr();
		nLength = m_pQueue->getWritableLen();
	}

	inline unsigned char* getFirstPacketPtr()
	{
		MsgHead msgpeek;
		if (!m_pQueue->peek((unsigned char*)&msgpeek, sizeof(MsgHead)))
			return NULL;

		MsgHead msgHead;
		msgHead.ParseFromArray((const char*)&msgpeek);

		if (m_pQueue->getUsed() < (int)(2 + msgHead.mSize))
			return NULL;

		//if (m_pQueue->getTailUsed() < (int)(msgHead.mSize + sizeof(MsgHead)))
		//	m_pQueue->copyHeader2Extra(msgHead.mSize + sizeof(MsgHead) - m_pQueue->getTailUsed());

		return  m_pQueue->getReadPtr();
	}

	inline void	removeFirstPacket(unsigned short wSize)
	{
		m_pQueue->dequeue(NULL, wSize);
	}
};
