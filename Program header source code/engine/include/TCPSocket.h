/**
 * \file      TCPSocket.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#pragma once
#include "socket.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CTCPSocket :
	public net::CSocket
{
	friend unsigned int __stdcall TCPSocket_RecvThread(void* param);
	friend unsigned int __stdcall TCPSocket_SendThread(void* param);

protected:
	bool m_bSelfDisconnect;				// 自己断开的
public:
	
protected:
public:
	CTCPSocket(void);
	~CTCPSocket(void);

	bool IsSelfDisconnect() { return m_bSelfDisconnect;}
	bool Create(UINT nPort = 0);
	void Close();
	bool Connect(const char* pszAddress,UINT nHostPort);
	void Disconnect(int nHow = SD_BOTH);
	int RecvData(char* buffer,size_t bufferSize);
	int SendData(char* buffer,size_t bufferSize);

	virtual int Recv(char* buffer,size_t bufferSize);
	virtual int Send(char* buffer,size_t bufferSize);
};
