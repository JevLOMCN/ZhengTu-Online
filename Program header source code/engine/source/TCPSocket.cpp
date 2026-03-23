/**
 * \file      TCPSocket.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#include "../include/engine.h"
#include "../include/tcpsocket.h"
#include "../include/Thread.h"

std::list<BYTE>	* g_pListLoginKey = NULL;
CThread::t_start_address g_tcpSocket_RecvThread;
CThread::t_start_address g_tcpSocket_SendThread;

size_t	gSocketRecvNum = 0;
size_t  gSocketSendNum = 0;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
unsigned int __stdcall TCPSocket_RecvThread(void* param)
{
	CTCPSocket* pSocket = (CTCPSocket*)param;
	for(;;)
	{
		if(pSocket->m_recvThread.IsEndThread()) break;
		char buffer[1024 * 10];
		int iResult = pSocket->RecvData(buffer,sizeof(buffer));
		if(iResult == 0) continue;
		if(iResult < 0) break;

#if defined _DEBUG || defined RELEASE_TEST
		//TRACE("ÊÕµ½Êı¾İ°ü£ºSize = %u\n",iResult);
		gSocketRecvNum += iResult;
#endif

		pSocket->GetRecvQueue().Lock();
		pSocket->GetRecvQueue().Push(buffer,iResult);
		pSocket->GetRecvQueue().Unlock();

		Sleep(5);
	}
	pSocket->m_state = net::CSocket::eSocketState_Disconnect;
	return 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
unsigned int __stdcall TCPSocket_SendThread(void* param)
{
	CTCPSocket* pSocket = (CTCPSocket*)param;
	for(;;)
	{
		stMsg msg;
		int iPopResult; 
		
		if(pSocket->m_sendThread.IsEndThread()) break;

		pSocket->GetSendQueue().Lock();
		iPopResult = pSocket->GetSendQueue().Pop(msg);
		if(iPopResult == CStreamQueue::eErrorOk)
		{
			int iResult = pSocket->SendData(msg.buffer,msg.size);
			if(iResult != msg.size) 
			{
				pSocket->GetSendQueue().Unlock();
				break;
			}
		}
		pSocket->GetSendQueue().Unlock();

		if(iPopResult != CStreamQueue::eErrorOk) Sleep(5);
	}
	pSocket->m_state = net::CSocket::eSocketState_Disconnect;
	return 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTCPSocket::CTCPSocket(void)
: m_bSelfDisconnect(false)
{
	g_tcpSocket_RecvThread = TCPSocket_RecvThread;
	g_tcpSocket_SendThread = TCPSocket_SendThread;
	m_recvQueue.Init(1024*8);
	m_sendQueue.Init(1024*8);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTCPSocket::~CTCPSocket(void)
{
	Disconnect();
	Close();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nHow : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTCPSocket::Disconnect( int nHow )
{
	if( m_socket != INVALID_SOCKET )
	{
		shutdown( m_socket, nHow );
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	m_recvThread.EndThread();
	m_sendThread.EndThread();
	
	m_recvThread.WaitEndThread();
	m_sendThread.WaitEndThread();

	m_recvQueue.Lock();
	m_recvQueue.Clear();
	m_recvQueue.Unlock();

	m_sendQueue.Lock();
	m_sendQueue.Clear();
	m_sendQueue.Unlock();

	m_bSelfDisconnect = true;
	m_dwTargetIP = -1;
	m_dwTargetPort = -1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTCPSocket::Close()
{
	Disconnect();
	if(m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nPort : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CTCPSocket::Create(UINT nPort )
{
	Close();

	m_socket = socket ( AF_INET, SOCK_STREAM, 0 );
	if ( m_socket == INVALID_SOCKET )
	{
		// ²»ÒªÊä³öĞÅÏ¢£¬ÈİÒ×±»¸ú×Ù
		//appDump << "socket function failed! code:" << WSAGetLastError() << "\n";
		return false;
	}
	if(nPort)
	{
		SOCKADDR_IN sockAddr;
		memset(&sockAddr,0,sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		sockAddr.sin_port = htons((u_short)nPort);
		if(SOCKET_ERROR == bind(m_socket,(sockaddr*)&sockAddr,sizeof(sockAddr)))
		{
			//appDump << "bind function failed! code:" << WSAGetLastError() << "\n";
			return false;
		}
	}
	if(!ApplySocketBufferSize())
		return false;
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszAddress : ÃèÊö
 * \param nHostPort : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
#pragma optimize( "", off )
bool CTCPSocket::Connect(const char* pszAddress,UINT nHostPort)
{
	if(m_socket == INVALID_SOCKET && !Create())
	{
		return false;
	}

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;

	sockAddr.sin_port = htons((u_short)nHostPort);
	sockAddr.sin_addr.S_un.S_addr		=  inet_addr( pszAddress );

	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(pszAddress);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
			return false;
	}
	
	if(SOCKET_ERROR == connect(m_socket,(sockaddr*)&sockAddr,sizeof(sockAddr)))
	{
		//appDump << "connect function failed! code:" << WSAGetLastError() << "\n";
		TRACE("connect function failed ! code: %u\n",WSAGetLastError());
		return false;
	}

	if(!m_recvThread.BeginThread(g_tcpSocket_RecvThread,0,this)) return false;

	if(!m_sendThread.BeginThread(g_tcpSocket_SendThread,0,this)) return false;

	m_state = net::CSocket::eSocketState_Connection;

	m_dwTargetIP = sockAddr.sin_addr.S_un.S_addr;
	m_dwTargetPort = nHostPort;
	m_bSelfDisconnect = false;
	return true;
}
#pragma optimize( "", on )

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param buffer : ÃèÊö
 * \param bufferSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CTCPSocket::RecvData(char* buffer,size_t bufferSize)
{
	int iResult = recv(m_socket,buffer,bufferSize,0);

	if(iResult == 0) 
	{
		return -1; 
	}

	if(iResult == SOCKET_ERROR) 
	{
		DWORD dwCode = WSAGetLastError();
		if(dwCode == WSAEINPROGRESS || dwCode == WSAEMSGSIZE)
			return 0;
		return -1;
	}
	return iResult;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param buffer : ÃèÊö
 * \param bufferSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CTCPSocket::SendData(char* buffer,size_t bufferSize)
{
	int iSendSize = 0;
	while(bufferSize)
	{
		int iResult = send(m_socket,buffer + iSendSize,bufferSize,0);
		if(iResult == SOCKET_ERROR)
		{
			DWORD dwCode = WSAGetLastError();
			if(dwCode != WSAEINPROGRESS && dwCode != WSAEMSGSIZE)
				return iSendSize;
		}
		else
		{
			iSendSize += iResult;
			bufferSize -= iResult;
#if defined _DEBUG || defined RELEASE_TEST
			gSocketSendNum += iResult;
#endif
		}
	}
	return iSendSize;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param buffer : ÃèÊö
 * \param bufferSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CTCPSocket::Recv(char* buffer,size_t bufferSize)
{
	CSafeStreamQueue & recvQueue = GetRecvQueue();
	stMsg msg;
	recvQueue.Lock();
	int result = recvQueue.Pop(msg,bufferSize);
	int ret;
	if(result = CStreamQueue::eErrorOk)
	{
		memcpy(buffer,msg.buffer,msg.size);
		ret = msg.size;
	}
	recvQueue.Unlock();
	if(result == CStreamQueue::eErrorMsgSize)
		ret = -1;
	else if(result == CStreamQueue::eErrorNoData)
		ret = 0;
	return ret;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param buffer : ÃèÊö
 * \param bufferSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CTCPSocket::Send(char* buffer,size_t bufferSize)
{
	CSafeStreamQueue & sendQueue = GetSendQueue();
	sendQueue.Lock();
	sendQueue.Push(buffer,bufferSize);
	sendQueue.Unlock();
	return bufferSize;
}