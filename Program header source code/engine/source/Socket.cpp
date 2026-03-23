/**
 * \file      Socket.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#include "../include/engine.h"
#include "../include/socket.h"

namespace net{

DWORD AddressToIP(const char* pszAddress)
{
	u_long uIP		=  inet_addr( pszAddress );

	if (uIP == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(pszAddress);
		if (lphost != NULL)
			uIP = *(u_long*)lphost->h_addr;
		else
			return INADDR_NONE;
	}
	return uIP;
}

CSocket::CSocket(void)
: m_nSendBufferSize(-1)
, m_nRecvBufferSize(-1)
, m_socket(INVALID_SOCKET)
, m_state(eSocketState_None)
, m_dwTargetIP(-1)
, m_dwTargetPort(-1)
{
}

CSocket::~CSocket(void)
{
}

bool CSocket::ApplySocketBufferSize()
{
	if( m_nRecvBufferSize != -1 )
	{
		// 设置套接口的接收缓冲区
		if( SOCKET_ERROR == setsockopt( m_socket, SOL_SOCKET, SO_RCVBUF, (char *) &m_nRecvBufferSize, sizeof( m_nRecvBufferSize ) ))
		{
			//appDump << "setsockopt function failed! code:" << WSAGetLastError() << "\n";
			return false;
		}
	}
	else
	{
		int iSize = sizeof(m_nRecvBufferSize);
		getsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char*) &m_nRecvBufferSize,&iSize);
	}

	if( m_nSendBufferSize != -1 )
	{
		// 设置套接口的发送缓冲区
		if( SOCKET_ERROR == setsockopt( m_socket, SOL_SOCKET, SO_SNDBUF, (char *) &m_nSendBufferSize, sizeof( m_nSendBufferSize ) ))
		{
			//appDump << "setsockopt function failed! code:" << WSAGetLastError() << "\n";
			return false;
		}
	}
	else
	{
		int iSize = sizeof(m_nSendBufferSize);
		getsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char*) &m_nSendBufferSize,&iSize);
	}
	return true;
}

void CSocket::PushDataToRevQueue(BYTE* pData,size_t size)
{
	GetRecvQueue().Lock();
	GetRecvQueue().Push(pData,size);
	GetRecvQueue().Unlock();
}

};