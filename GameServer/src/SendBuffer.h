#ifndef _SENDBUFFER_H_
#define _SENDBUFFER_H_

#include <windows.h>
#include "CircuitQueue.h"
#include "net_msg.h"

//=============================================================================================================================
/// 클라이언트로 보낼 패킷을 쌓아둘 버퍼
//	send 호출 후 GQCS에서 completion이 들어오면 Completion(int)를 호출해 주면 된다.
//=============================================================================================================================
class SendBuffer
{
public:
	SendBuffer()			{ m_pQueue = NULL; }
	virtual ~SendBuffer()	{ if( m_pQueue ) delete m_pQueue; }

	inline VOID Create( int nBufferSize, DWORD dwExtraBuffeSize ) {
		if( m_pQueue ) delete m_pQueue;
		m_pQueue = new CircuitQueue<BYTE>;
		m_pQueue->Create( nBufferSize, dwExtraBuffeSize );
		m_bComplete = TRUE; }
	inline VOID Clear() { m_pQueue->Clear(); m_bComplete = TRUE; }

	inline VOID Completion( int nBytesSend ) { 	m_pQueue->Dequeue( NULL, nBytesSend ); m_bComplete = TRUE; }

	// 버퍼가 비어있거나 보내기 불가능한 상태(이미 send를 걸어놓은 상태)이면 FALSE를 리턴한다.
	inline BOOL	GetSendParam( BYTE **ppSendPtr, int &nLength ) {
		if( !IsReadyToSend() ) { nLength = 0; return FALSE; }
		*ppSendPtr	= m_pQueue->GetReadPtr();
		nLength		= m_pQueue->GetReadableLen();
		m_bComplete = FALSE;
		return TRUE; }

	inline bool Write( tagPACKET_HEADER *pHeader, BYTE *pMsg )	{
		if( !m_pQueue->Enqueue( (BYTE*)pHeader, sizeof(PACKET_HEADER) ) )	return false;
		if( !m_pQueue->Enqueue( pMsg, pHeader->size ) )						return false;
		return true; }

	inline bool Write( BYTE *pMsg, WORD wSize ) {
		if( !m_pQueue->Enqueue( pMsg, wSize ) ) return false;
		return true; }

	inline DWORD GetLength() { return m_pQueue->GetLength(); }

private:
	inline BOOL			IsReadyToSend() { return ( m_bComplete && m_pQueue->GetLength() > 0 ) ? TRUE : FALSE; }

private:
	BOOL				m_bComplete;
	CircuitQueue<BYTE>	*m_pQueue;
};

#endif
