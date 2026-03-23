#include "public.h"
#include ".\client.h"
#include "../Engine/include/TCPSocket.h"
#include "GameTime.h"
#include "./Game.h"
#include "../EncDec/include/EncDec.h"
#include "./FindDLL.h"

#define HAS_ENCRYPT
//#define RLE_ENCDEC

#define RETURN(Val) { retval = (Val); goto _ret;}

CClient * g_pClient = NULL;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CClient::CClient(void)
{
	FUNCTION_BEGIN;

	m_pSocket = new CTCPSocket;
	//m_pSocket->Create();
	m_recvBuffer.reserve(1024*4);
	m_dataBuffer.reserve(1024*4);
	m_uncompressBuffer.reserve(1024 * 2);
	m_sendBuffer.reserve(1024*2);

	extern std::list<BYTE>	* g_pListLoginKey;
	g_pListLoginKey = &m_loginkey;

	m_pEncrypt = new CEncrypt;
	m_bLoginRecv = true;
	m_bLoginSend = true;
	
	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CClient::~CClient(void)
{
	FUNCTION_BEGIN;

	Destroy();
	delete m_pSocket;
	delete m_pEncrypt;
	FUNCTION_END;
}

typedef unsigned long tPackLengthType;
#define PACKET_MAX_SIZE	0x00ffffff	// 包的最大长度
#define PACKET_FLAG_ENC 0x80000000	// 数据包加密
#define PACKET_FLAG_ZIP 0x40000000	// 数据包压缩

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param param : 描述
 * \return 返回值的描述
 */
unsigned int __stdcall Client_MsgThread(void* param)
{
	unsigned int retval;
	CHECK_RET_ADDR
	//CRYPT_START
	CClient* pClient = (CClient*)param;
	for(;;)
	{
		Sleep(5);
		if(pClient->m_msgThread.IsEndThread())
			break;
		//---------------------------------
		// 读取数据
		//---------------------------------
		{
			CSafeStreamQueue & recvQueue = pClient->m_pSocket->GetRecvQueue();
			recvQueue.Lock();
			stMsg msg;
			for(;;)
			{
				if(pClient->m_msgThread.IsEndThread())
					break;
				int iPopResult = recvQueue.Pop(msg);
				if(iPopResult == CStreamQueue::eErrorOk)
				{
					size_t size1 = pClient->m_recvBuffer.size();
					pClient->m_recvBuffer.resize(size1 + msg.size);
					memcpy(&pClient->m_recvBuffer[size1],msg.buffer,msg.size);
				}
				else break;
			}
			recvQueue.Unlock();
		}
		//---------------------------------

		//---------------------------------
		// 解密
		//---------------------------------
		{
			//TRACE("接收到数据包长度 %d \n ",pClient->m_recvBuffer.size());

#ifdef HAS_ENCRYPT
			if(pClient->m_recvBuffer.size() < 8)
			{
				continue;
			}

			size_t size2 = (pClient->m_recvBuffer.size()) & (~7);
			size_t count = pClient->m_recvBuffer.size()/8;
			if( pClient->m_bLoginRecv )
			{
				RC5_32_KEY key;
				BYTE keyData[16];
				size_t keySize;
				Client_GetKey(pClient->m_loginkey,keyData,keySize);
				pClient->m_pEncrypt->RC5_32_set_key(&key,keyData);
				for(size_t i=0;i<count;++i)
					pClient->m_pEncrypt->RC5_32_decrypt(&pClient->m_recvBuffer[i * 8],&key);
			}
			else
			{
				DES_key_schedule key;
				BYTE keyData[8];
				size_t keySize;
				Client_GetKey(pClient->m_key,keyData,keySize);
				pClient->m_pEncrypt->DES_set_key(keyData,&key);
				for(size_t i=0;i<count;++i)
					pClient->m_pEncrypt->DES_encrypt1(&pClient->m_recvBuffer[i * 8],&key,DES_DECRYPT);
			}

#else
			size_t size2 = (pClient->m_recvBuffer.size());
#endif
			size_t size1 = pClient->m_dataBuffer.size();
			pClient->m_dataBuffer.resize(size1 + size2);
			memcpy(&pClient->m_dataBuffer[size1],&pClient->m_recvBuffer[0],size2);

			size_t size3 = pClient->m_recvBuffer.size() - size2;
			memcpy(&pClient->m_recvBuffer[0],&pClient->m_recvBuffer[size2],size3);
			pClient->m_recvBuffer.resize(size3);

		}
		//---------------------------------

		//---------------------------------
		// 解析
		//---------------------------------
		{
			size_t offset = 0;
			size_t datasize = pClient->m_dataBuffer.size();
			BYTE* data = &pClient->m_dataBuffer[0];
			while(offset + sizeof(tPackLengthType) < datasize)
			{
				tPackLengthType length = *(tPackLengthType*)&data[offset];
				tPackLengthType flags = (length & (~PACKET_MAX_SIZE));
				length &= PACKET_MAX_SIZE;
				Assert(length);
				if(offset + sizeof(tPackLengthType) + length <=  datasize)
				{
					// 有一个完整的数据包
					//TRACE("********** Msg Size %d\n",length);
					BYTE* uncompressData;
					size_t uncompressSize;
					if(flags & PACKET_FLAG_ZIP)
					{
						// 数据包是压缩的
						BYTE* compressData = &data[offset + sizeof(tPackLengthType)];
						size_t compressSize = length;

						if(pClient->m_uncompressBuffer.capacity() < compressSize * 3 / 2)
							pClient->m_uncompressBuffer.reserve(compressSize * 3 / 2);

						for(;;)
						{
							uncompressData = &pClient->m_uncompressBuffer[0];
							uncompressSize = pClient->m_uncompressBuffer.capacity();
#ifdef RLE_ENCDEC
							int iZipResult = RLE_DecodeBytes(compressData,compressSize,uncompressData,uncompressSize);
							if(iZipResult == RLE_ERROR_SIZE)
							{
								// 缓冲区太小
								pClient->m_uncompressBuffer.reserve(pClient->m_uncompressBuffer.capacity() + compressSize * 3 / 2);
								continue;
							}
							if(iZipResult == RLE_ERROR_OK)
								break;
#else
							int iZipResult = uncompress(uncompressData,(uLongf*)&uncompressSize,compressData,compressSize);
							if(iZipResult == Z_BUF_ERROR)
							{
								// 缓冲区太小
								pClient->m_uncompressBuffer.reserve(pClient->m_uncompressBuffer.capacity() + compressSize * 3 / 2);
								continue;
							}
							if(iZipResult == Z_OK)
								break;
#endif
							// 解析出错了，断开连接
							Assert(0);
							goto _parse_error;
						}
					}
					else
					{
						uncompressData = &data[offset + sizeof(tPackLengthType)];
						uncompressSize = length;
					}

					// 将消息放入对列
					pClient->m_msgQueue.Lock();
					pClient->m_msgQueue.Push(uncompressData,uncompressSize);
					pClient->m_msgQueue.Unlock();

					//TRACE("********** Msg uncompressSize %d\n",uncompressSize);
#ifdef HAS_ENCRYPT
					//offset += ((sizeof(tPackLengthType) + length) + 7) & (~7);
					offset += (sizeof(tPackLengthType) + length);
#else
					offset += (sizeof(tPackLengthType) + length);
#endif
				}
				else
				{
					break;
				}
			}

			if(datasize - offset > 1024 * 10)
				goto _parse_error;

			memcpy(&data[0],&data[offset],datasize-offset);
			pClient->m_dataBuffer.resize(datasize-offset);
		}
		//---------------------------------
	}
	RETURN(0);
_parse_error:
	pClient->m_pSocket->Disconnect(SD_BOTH);
	RETURN(-1);
_ret:
	;
	//CRYPT_END
	return retval;
}

//unsigned int _stdcall Client_SendThread(void* pParam)
//{
//	CClient * pClient = (CClient*)pParam;
//	for(;;)
//	{
//		if(pClient->m_sendThread.IsEndThread())
//			break;
//		CSafeStreamQueue& sendQueue = pClient->m_sendQueue;
//
//	}
//}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nPort : 描述
 * \return 返回值的描述
 */
bool CClient::Create(UINT nPort)
{
	FUNCTION_BEGIN;

	if(g_bSinglePlayer)
	{
		m_msgThread.BeginThread(Client_MsgThread,0,this);
		return true;
	}
	else
		return m_pSocket->Create(nPort);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CClient::Destroy()
{
	FUNCTION_BEGIN;

	Disconnect();
	m_pSocket->Close();

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszAddress : 描述
 * \param nHostPort : 描述
 * \return 返回值的描述
 */
bool CClient::Connect(const char* pszAddress,UINT nHostPort)
{
	FUNCTION_BEGIN;

	m_nSequence = rand() % 16;
	
	{
		bool b= m_pSocket->Connect(pszAddress,nHostPort); 
		if( b )m_msgThread.BeginThread(Client_MsgThread,0,this);
		return b;
	}
	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nHow : 描述
 * \return 返回值的描述
 */
void CClient::Disconnect( int nHow )
{
	FUNCTION_BEGIN;

	m_pSocket->Disconnect(nHow);
	m_msgThread.EndThread();
	m_msgThread.WaitEndThread();
	m_recvBuffer.resize(0);
	m_dataBuffer.resize(0);
	m_msgQueue.Clear();

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param buffer : 描述
 * \param bufferSize : 描述
 * \return 返回值的描述
 */
//#pragma optimize( "st", off )
int CClient::Send(char* buffer,size_t bufferSize,bool bSelfRecv)
{
	int retval;
	FUNCTION_BEGIN;
	retval = 0;
	size_t datasize;

	tPackLengthType flags = 0;
	size_t size1 = ( (bufferSize + sizeof(tPackLengthType) + 7) & (~7) );	// 需要加密的话，长度为8的整数倍
#ifdef RLE_ENCDEC
	m_sendBuffer.reserve(size1 * 3/2);
#else
	m_sendBuffer.reserve(size1);
#endif

	if(bufferSize > 32 || g_bSinglePlayer)
	{
		flags |= PACKET_FLAG_ZIP;
		// 压缩
		for(;;)
		{
			BYTE* compressData = &m_sendBuffer[0] + sizeof(tPackLengthType);
			datasize = m_sendBuffer.capacity() - sizeof(tPackLengthType);

#ifdef RLE_ENCDEC
			RLE_EncodeBytes((BYTE*)buffer,bufferSize,compressData,datasize);
			break;
#else
			int zipResult = compress(compressData,(uLongf*)&datasize,(Bytef*)buffer,bufferSize);
			if(zipResult == Z_BUF_ERROR)
			{
				// 缓冲区太小
				m_sendBuffer.reserve(m_sendBuffer.capacity() + bufferSize);
				continue;
			}
			if(zipResult == Z_OK)
				break;
#endif
			Assert(0);
			goto _send_error;
		}
#if defined _DEBUG || defined RELEASE_TEST
		//Engine_WriteLogF("压缩 sourceSize:%d, destSize:%d\n",bufferSize,datasize);
#endif
	}
	else
	{
		datasize = bufferSize;
		BYTE* data = &m_sendBuffer[0] + sizeof(tPackLengthType);
		memcpy(data,buffer,bufferSize);
	}

	
	CHECK_RET_ADDR

	size_t sendsize;
	BYTE*  senddata;

	// 加密
	senddata = &m_sendBuffer[0];

#ifdef HAS_ENCRYPT
	sendsize = ( (datasize + sizeof(tPackLengthType) + 7) & (~7) );	// 需要加密的话，长度为8的整数倍
	*(tPackLengthType*)senddata = (sendsize-sizeof(tPackLengthType)) | flags;
	*(tPackLengthType*)senddata |= PACKET_FLAG_ENC;
	size_t count = sendsize/8;
	if(m_bLoginSend)
	{
		RC5_32_KEY key;
		BYTE keyData[16];
		size_t keySize;
		Client_GetKey(m_loginkey,keyData,keySize);
		m_pEncrypt->RC5_32_set_key(&key,keyData);
		for(size_t i=0;i<count;++i)
		{
			m_pEncrypt->RC5_32_encrypt(&senddata[i*8],&key);
		}
	}
	else
	{
		DES_key_schedule key;
		BYTE keyData[8];
		size_t keySize;
		Client_GetKey(m_key,keyData,keySize);
		m_pEncrypt->DES_set_key(keyData,&key);
		for(size_t i=0;i<count;++i)
			m_pEncrypt->DES_encrypt1(&senddata[i*8],&key,DES_ENCRYPT);
	}
#else
	sendsize = datasize + sizeof(tPackLengthType);
	*(tPackLengthType*)&m_sendBuffer[0] = (datasize-sizeof(tPackLengthType)) | flags;
#endif

	if(bSelfRecv)
	{
		m_pSocket->PushDataToRevQueue(senddata,sendsize);
		RETURN( true );
	}
	else
	{
		RETURN( m_pSocket->Send((char*)senddata,sendsize) );
	}

_send_error:
	Disconnect();
	RETURN(0);
_ret:
	;
	FUNCTION_END;
	return retval;

}
//#pragma optimize( "", on )

static UINT s_nCmdNum = 0;
static DWORD s_dwTime = 0;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwCmdType : 描述
 * \return 返回值的描述
 */
bool CanSendCommand(DWORD dwCmdType)
{
	FUNCTION_BEGIN;
	if(xtimeGetTime() - s_dwTime > 1000)
	{
		//TRACE("****************SendCommandNum = %d ***************\n",s_nCmdNum);
		s_nCmdNum = 0;
		s_dwTime = xtimeGetTime();
	}

	//if(dwCmdType == MAPSCREEN_USERCMD || dwCmdType == LOGON_USERCMD) return TRUE;

	if(s_nCmdNum == 7)
	{
		//TRACE("****************SendNum = 5 Can not send***************\n");
		return false;
	}

	return true;//(4 - s_nCmdNum);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwCmdType : 描述
 * \return 返回值的描述
 */
void WaitCanSendCommand(DWORD dwCmdType)
{
	FUNCTION_BEGIN;

	if( CanSendCommand(dwCmdType) ) 
		return;

	
	DWORD dwTime = xtimeGetTime() - s_dwTime;
	if(dwTime <= 1000){
		//TRACE("****************WaitCanSendCommand %d***************\n",1000 - dwTime);
#ifdef RELEASE_TEST
		printf("****************WaitCanSendCommand %d***************\n",1000 - dwTime);
#endif
		//Sleep(1000 - dwTime);
	}

	/*while(xtimeGetTime() - s_dwTime <= 1000)
	{
		Sleep(0);
	}*/
	
	s_nCmdNum = 0;
	s_dwTime = xtimeGetTime();

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pCmd : 描述
 * \param bufferSize : 描述
 * \return 返回值的描述
 */
#pragma optimize("agst",off)
//__declspec( naked ) bool CClient::SendCommand(stNullUserCmd* pCmd,size_t bufferSize,bool bSelfRecv )
//{
//	int retval;
//	FUNCTION_BEGIN_NEW;
//	_asm push        ebp  
//	_asm mov         ebp,esp
//	_asm push        ecx  
//	_asm push        ecx
//	m_nSequence = ((m_nSequence + 1)&0xf);
//	pCmd->byCmdSequence = (BYTE)m_nSequence;
//	pCmd->dwTimestamp = GetGameTime()->GetTimestamp();
//	s_nCmdNum++;
//	retval = (Send((char*)pCmd,bufferSize,bSelfRecv) > 0);
//
//
//	_asm mov eax , retval
//	_asm pop ecx
//	_asm pop ecx
//	_asm pop ebp
//	FUNCTION_END_NEW;
//	_asm ret 0Ch
//}


#pragma optimize("",on)
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param state : 描述
 * \return 返回值的描述
 */
void CClient::SetState(net::CSocket::enumState state) 
{
	FUNCTION_BEGIN;

	if(g_bSinglePlayer) m_pSocket->SetState(state);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CClient::ResetState()
{
	FUNCTION_BEGIN;

	m_pSocket->SetState(net::CSocket::eSocketState_None);

	FUNCTION_END;
}