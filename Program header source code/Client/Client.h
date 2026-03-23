#pragma once
#include "../engine/include/Socket.h"
struct stNullUserCmd;
class IEncrypt;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CClient
{
	friend unsigned int __stdcall Client_MsgThread(void* param);
	friend unsigned int _stdcall Client_SendThread(void* pParam);
	net::CSocket* m_pSocket;
	CThread m_msgThread;
	CThread m_sendThread;
	std::vector<BYTE>	m_recvBuffer;
	std::vector<BYTE>	m_dataBuffer;
	std::vector<BYTE>	m_uncompressBuffer;
	std::vector<BYTE>	m_sendBuffer;
	CSafeStreamQueue	m_msgQueue;
	CSafeStreamQueue	m_sendQueue;


public:
	DWORD				m_nSequence;
	std::list<BYTE>		m_loginkey;
	std::list<BYTE>		m_key;
	IEncrypt*			m_pEncrypt;
	bool				m_bLoginRecv;
	bool				m_bLoginSend;

	CClient(void);
	~CClient(void);

	virtual bool Create(UINT nPort = 0);
	virtual void Destroy();
	virtual bool Connect(const char* pszAddress,UINT nHostPort);
	virtual void Disconnect(int nHow = SD_BOTH);
	virtual int Send(char* buffer,size_t bufferSize,bool bSelfRecv = false);

	//bool SendCommand(stNullUserCmd* pCmd,size_t bufferSize,bool bSelfRecv  = false);

	virtual bool IsSelfDisconnect() { return m_pSocket->IsSelfDisconnect(); }
	
	virtual bool PopMsg(std::vector<BYTE,std::stack_allocator<BYTE> > & buffer)
	{
		bool result = false;
		
		m_msgQueue.Lock();
		
		stMsg msg;
		
		int e = m_msgQueue.Pop(msg);
		
		if( e == CStreamQueue::eErrorOk )
		{
			buffer.reserve(msg.size);
			memcpy(&buffer[0],msg.buffer,msg.size);
			result = true;
		}

		m_msgQueue.Unlock();

		return result;
	}

	virtual void PushCommand(stNullUserCmd* pCmd,size_t size);
	virtual void SetState(net::CSocket::enumState state);
	virtual net::CSocket::enumState GetState(){ return m_pSocket->GetState();}
	virtual void ResetState();

	virtual DWORD GetTargetIP(){ return m_pSocket->GetTargetIP();}
	virtual DWORD GetTargetPort(){ return m_pSocket->GetTargetPort();}

	virtual size_t getStreamQueueMemorySize()
	{
		return m_pSocket->getStreamQueueMemorySize() + m_msgQueue.getMemorySize() + m_sendQueue.getMemorySize();
	}
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline CClient * GetClient()
{
	extern CClient *g_pClient;
	return g_pClient;
}

#define Client_SetKey( aKey,pData, size)\
{\
	aKey.clear();\
	for(size_t i=0;i<size;++i)\
	{\
		aKey.push_back(pData[i]);\
	}\
}

#define Client_GetKey( aKey,pData,size) \
{\
	size = 0;\
	for(std::list<BYTE>::iterator it = aKey.begin(); it != aKey.end(); ++it)\
		pData[size++] = (*it);\
}

#include "./execryptor.h"
#include "./GameTime.h"
//inline bool CClient::SendCommand(stNullUserCmd* pCmd,size_t bufferSize,bool bSelfRecv)
#define Client_SendCommand(pCmd,bufferSize,bSelfRecv) {\
	(pCmd)->dwTimestamp = GetGameTime()->GetTimestamp();\
	stNullUserCmd * pNumCmd = (stNullUserCmd *)pCmd;\
    if( pNumCmd )\
	{\
		TRACE("cmd:%d,%d\n",pNumCmd->byCmd,pNumCmd->byParam);\
	}\
	GetClient()->Send((char*)(pCmd),bufferSize,bSelfRecv);\
	}

inline void CClient::PushCommand(stNullUserCmd* pCmd,size_t size)
{
	Client_SendCommand(pCmd,size,true);
	//SendCommand(pCmd,size,true);
	/*m_msgQueue.Lock();
	m_msgQueue.Push(pCmd,size);
	m_msgQueue.Unlock();*/
}
extern void WaitCanSendCommand(DWORD dwCmdType);
extern bool CanSendCommand(DWORD dwCmdType);
//#define SEND_USER_CMD( cmd ) GetClient()->SendCommand(&cmd,sizeof(cmd))
#define SEND_USER_CMD(cmd) Client_SendCommand(&(cmd),sizeof(cmd),false)
