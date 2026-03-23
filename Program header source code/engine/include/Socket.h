/**
 * \file      Socket.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#pragma once

#include "Thread.h"
#include "StreamQueue.h"

namespace net{
class CSocket
{
protected:
	SOCKET m_socket;
	int	   m_nSendBufferSize;
	int	   m_nRecvBufferSize;
	DWORD  m_dwTargetIP;
	DWORD  m_dwTargetPort;

	bool ApplySocketBufferSize();

	CThread m_recvThread;
	CThread m_sendThread;
	
	CSafeStreamQueue m_recvQueue;
	CSafeStreamQueue m_sendQueue;
public:
	enum enumState{
		eSocketState_None,
		eSocketState_Connection,
		eSocketState_Disconnect,
	};
	DWORD GetTargetIP() { return m_dwTargetIP;}
	DWORD GetTargetPort(){ return m_dwTargetPort;}
protected:
	CSafeStreamQueue & GetSendQueue() { return m_sendQueue; }
	enumState		 m_state;
public:
	

	CSafeStreamQueue & GetRecvQueue() { return m_recvQueue; }

	CSocket(void);
	virtual ~CSocket(void);

	virtual bool IsSelfDisconnect() = 0;
	virtual bool Create(UINT nPost = 0) = 0;
	virtual void Close() = 0;
	virtual bool Connect(const char* pszAddress,UINT nHostPort) = 0;
	virtual void Disconnect(int nHow = SD_BOTH) = 0;
	virtual int Recv(char* buffer,size_t bufferSize) = 0;
	virtual int Send(char* buffer,size_t bufferSize) = 0;
	enumState GetState(){ return m_state; }
	void	  SetState(enumState state) { m_state = state; }
	void PushDataToRevQueue(BYTE* pData,size_t size);
	size_t getStreamQueueMemorySize()
	{
		return m_sendQueue.getMemorySize() + m_recvQueue.getMemorySize();
	}
};
extern DWORD AddressToIP(const char* pszAddress);
};
