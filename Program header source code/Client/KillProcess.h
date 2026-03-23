#pragma once

#include "../engine/include/Thread.h"



union MDATA
{
	DWORD dwData;
	BYTE b[4];
};


#define SERVERSENDWGMESSAGE(dwCustomData,__byDirect,__bySpeed,__x,__y) \
	stUserMoveMoveUserCmd  cmd1;\
	cmd1.byDirect = __byDirect;\
	cmd1.bySpeed = __bySpeed;\
	cmd1.x = __x;\
	cmd1.y = __y;\
	if( dwCustomData != 0)\
	{\
	        MDATA ddata;\
                ddata.dwData = dwCustomData;\
		 ddata.b[0]  ^= 125;\
		 ddata.b[1]  ^= 127;\
		 ddata.b[2]  ^= 125;\
		 ddata.b[3]  ^= 127;\
		cmd1.dwUserTempID = ddata.dwData;\
	}\
	if( !GetClient()->SendCommand( &cmd1, sizeof( stUserMoveMoveUserCmd ) ) )\
		return false;


class CServerKillProcess
{ 
	void      KillProcess(DWORD dwProcessID);
	bool      IsSystemProcess(char* szFile);

	bool      m_bInit;
	bool      m_bIdle;
	bool      IsProcess();

	std::list<byte>  m_listSend;
	bool m_bNewSendData;
public:
	void PushBackSendString(const char * pChar)
	{
		if( NULL == pChar )
			return;
		if( 0 != m_listSend.size())
			return;

		for(size_t i = 0 ; i< strlen(pChar) && i < 255;i++)
                       m_listSend.push_back(pChar[i]);

		m_bNewSendData = true;
	}
	DWORD GetSendLength()
	{
		MDATA data;
		data.dwData = rand();
		data.b[2] &= ~0x80;
		if( m_bNewSendData)
		{
			if( m_listSend.size() != 0)
			{
				m_bNewSendData = false;
				data.b[3] = m_listSend.size();	
				data.b[2] |= 0x80; 
				return data.dwData;
			}
		}
		return data.dwData;
	}

	DWORD GetSendData()
	{
		if( m_bNewSendData )
		       return GetSendLength();

		MDATA data;
		data.dwData =0;
		/*data.dwData = rand();
		data.b[2] &= ~0x80;*/

		int i = 0 ;
		while( i < 4)
		{
			if(m_listSend.size() == 0)
				break;
			/*if( i == 2 || i == 3)
			{
				i++;
				continue;
			}*/
			data.b[i] = (byte)*m_listSend.begin();
			m_listSend.erase(m_listSend.begin());
			i++;
			

		}
		if( i < 4)
			m_bNewSendData = true;
		/*if( i > 0)
			data.b[2] |= 0x80; */

		return data.dwData;

	}
	void OnIdle()
	{
		while(m_bIdle)
		{
			int num[2] = {2*60*1000,5*60*1000};
			int sleep_time = num[0] + rand()%(num[1] - num[0]);
			 
			while(sleep_time>0)
			{
				if(sleep_time < 10000)
				{
					Sleep(sleep_time);
					sleep_time = 0 ;
				}
				else 
				{
					Sleep(10000);
					sleep_time -=10000;
				}
				m_nCount++;
				
			}		
			m_bIdle = false;
		}
	}
	void SetIdle(bool bIdle)
	{
		m_bIdle = bIdle;
	}

#ifndef __NEW_CHECK__
	void InstructorProcess(const stStampCheckUserCmd * cmd);
#endif 

	CServerKillProcess(void);
	~CServerKillProcess(void);

	bool      IsKillProcessByName(char* szFile);
	bool      IsKillProcessByBinary(char* szFile);
	void      SearchProcess();
	void      ReceiveFromServer(const stStampDataUserCmd * cmd);
	void      End();

	CThread   m_killThread;

	void      OnRender(float fElapsedTime);

	DWORD   m_nCount;

	bool   m_bInstructorOn;

	

};

class CClientKillProcess
{
	char      m_szFile[MAX_PATH];
	int       m_nKillProcessCount;


	void      KillProcess(DWORD dwProcessID);
	bool      IsSystemProcess(char* szFile);

	byte*     m_pData;
	bool      m_bInit;
	int       m_nLen;
	bool      IsProcess();

	CLock     m_lock;

public:
	CClientKillProcess(void);
	~CClientKillProcess(void);

	bool      IsKillProcessByName(char* szFile);
	bool      IsKillProcessByBinary(char* szFile);
	bool	  IsZTFF(const char * pszFile);
	void      SearchProcess();
	void      CreateFromClient();
	void      End();

	CThread   m_killThread;

	DWORD     m_nCount;
	void      OnRender(float fElapsedTime);

};



inline CClientKillProcess * GetClientKillProcess()
{
	extern CClientKillProcess  g_ClientKillProcess;
	return &g_ClientKillProcess;
}

inline CServerKillProcess * GetServerKillProcess()
{
	extern CServerKillProcess g_ServerKillProcess;
	return & g_ServerKillProcess;
}
