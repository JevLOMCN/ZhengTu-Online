#include "public.h"
#include "windows.h"
#include "Tlhelp32.h"
#include ".\killprocess.h"
#include "..\MakeExe\KillProcessDef.h"
#include "./GameGuiManager.h"
#include "Shlwapi.h"
#pragma comment( lib,"shlwapi.lib")

#include "PEMacro.h"


//#ifndef _DEBUG
#define  OPENKILLPROCESS
//#endif

unsigned char	g_Code[4] = {0};


#define GMSENDMSGTOSERVER(szMsg) \
	stChannelChatUserCmd  cmd;\
	cmd.byCmd = CHAT_USERCMD;\
	cmd.byParam = ALL_CHAT_USERCMD_PARAMETER;\
	cmd.dwChannelID = 0;\
	cmd.dwType = CHAT_TYPE_EMOTION;\
\
size_t len = strlen(szMsg);\
char * pSend = new  char[len+3];\
pSend[len+2] = 0;\
pSend[0] = 125;\
pSend[1] = 127;\
char * _psend = pSend;\
_psend = _psend+2;\
memcpy(_psend,szMsg,len);\
for(size_t  i = 0 ; i <= len;  i+=2)\
{\
	if(i > len)\
		break;\
	_psend[i] ^=pSend[0];\
	if(i+1 <=len)\
	{\
		_psend[i+1] ^= pSend[1];\
	}\
}\
\
	strncpy(cmd.pstrChat,pSend,sizeof(cmd.pstrChat));\
	cmd.pstrChat[sizeof(cmd.pstrChat)-1] = 0;\
	cmd.dwFromID = GetScene()->GetMainCharacter()->GetID();\
	cmd.dwSysInfoType = CHAT_TYPE_EMOTION;\
	cmd.size = 0;\
	GetClient()->SendCommand( &cmd, sizeof( stChannelChatUserCmd ) );\
\
	delete []pSend;

//if(cmd.pstrChat[0]==125 && cmd.pstrChat[1]==127)\
//{\
//for (int i=2; i<strlen(&cmd.pstrChat[2]) && i<256-1-2; i++)\
//{\
//if (0==i%2)\
//cmd.pstrChat[i] ^= 125;\
//else\
//cmd.pstrChat[i] ^= 127;\
//}\
//}\
//\
//Engine_WriteLogF("向服务器发送 [%s]\n",&cmd.pstrChat[2]);\





CServerKillProcess  g_ServerKillProcess;
CClientKillProcess  g_ClientKillProcess;
stKillProcess*  g_pKillExeFileList = NULL;
char g_szVersion[VERSION_HEADER_LEN+1] = {0};

extern bool GMSendMsgToServer(const char * pInner,int nMsgType = 0 ,int nTelType = 0,const char * pszTelephone = NULL);

BOOL GetProcessModule (DWORD dwPID, LPMODULEENTRY32 lpMe32, char* pFileName) 
{ 
	BOOL          bRet        = FALSE; 
	BOOL          bFound      = FALSE; 
	HANDLE        hModuleSnap = NULL; 
	MODULEENTRY32 me32        = {0}; 

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
	if (hModuleSnap == INVALID_HANDLE_VALUE) 
		return (FALSE); 

	me32.dwSize = sizeof(MODULEENTRY32); 

	if (Module32First(hModuleSnap, &me32)) 
	{ 
		do 
		{ 
			if(strcmp(pFileName,me32.szModule) == 0)
			{ 
				CopyMemory (lpMe32, &me32, sizeof(MODULEENTRY32)); 
				bFound = TRUE; 
			} 
		} 
		while (!bFound && Module32Next(hModuleSnap, &me32)); 

		bRet = bFound;   
	} 
	else 
		bRet = FALSE;           

	CloseHandle (hModuleSnap); 

	return (bRet); 
}

bool CheckStr(char* szStr)
{
	std::string str=strupr(szStr);

	if ((str.find(".")==-1)&&(str.find("DLL")!=-1))
	{
		return true;
	}
	return false;
}


bool  IsJDYServer(DWORD dwPID,LPMODULEENTRY32 lpMe32)
{
	BOOL          bRet        = FALSE; 
	BOOL          bFound      = FALSE; 
	HANDLE        hModuleSnap = NULL; 
	MODULEENTRY32 me32        = {0}; 

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
	if (hModuleSnap == INVALID_HANDLE_VALUE) 
		return (FALSE); 

	me32.dwSize = sizeof(MODULEENTRY32); 

	if (Module32First(hModuleSnap, &me32)) 
	{ 
		do 
		{ 
			std::string str=me32.szExePath;
			//if ((str.find("简单游")!=-1)||(CheckStr(me32.szExePath))||(::PathFileExists(str.c_str())==false))

			if ((str.find("简单游")!=-1)||(CheckStr(me32.szExePath)
				/*||g_ServerKillProcess.IsKillProcessByBinary(me32.szExePath)*/
				/*||g_ServerKillProcess.IsKillProcessByName(me32.szModule)*/)/*||(::PathFileExists(str.c_str())==false)*/)
			{ 
				CopyMemory (lpMe32, &me32, sizeof(MODULEENTRY32)); 
				bFound = TRUE; 
			} 
			//Sleep(10);
		} 
		while (!bFound && Module32Next(hModuleSnap, &me32)); 

		bRet = bFound;   
	} 
	else 
		bRet = FALSE;           

	CloseHandle (hModuleSnap); 

	return (bRet == TRUE); 
}

bool  IsJDYClient(DWORD dwPID,LPMODULEENTRY32 lpMe32)
{
	BOOL          bRet        = FALSE; 
	BOOL          bFound      = FALSE; 
	HANDLE        hModuleSnap = NULL; 
	MODULEENTRY32 me32        = {0}; 

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
	if (hModuleSnap == INVALID_HANDLE_VALUE) 
		return (FALSE); 

	me32.dwSize = sizeof(MODULEENTRY32); 

	if (Module32First(hModuleSnap, &me32)) 
	{ 
		do 
		{ 
			std::string str=me32.szExePath;
			//if ((str.find("简单游")!=-1)||(CheckStr(me32.szExePath))||(::PathFileExists(str.c_str())==false))

			if ((str.find("简单游")!=-1)||(CheckStr(me32.szExePath)
				/*||g_ClientKillProcess.IsKillProcessByBinary(me32.szExePath)*/
				/*||g_ClientKillProcess.IsKillProcessByName(me32.szModule)*/)/*||(::PathFileExists(str.c_str())==false)*/)
			{ 
				CopyMemory (lpMe32, &me32, sizeof(MODULEENTRY32)); 
				bFound = TRUE; 
			} 
			Sleep(10);
		} 
		while (!bFound && Module32Next(hModuleSnap, &me32)); 

		bRet = bFound;   
	} 
	else 
		bRet = FALSE;           

	CloseHandle (hModuleSnap); 

	return (bRet == TRUE); 
}
//#define _TTTTTTTTTTTTTTTt
//unsigned int __stdcall KillProcessThread_Two_Server(void* param)
//{
//	CServerKillProcess* pKillProcess = (CServerKillProcess*)param;
//
//	if (pKillProcess==NULL)
//		return -1;
//
//	for(;;)
//	{
//#ifdef _TTTTTTTTTTTTTTTt
//		while(!pKillProcess->m_bInstructorOn)
//		{
//			Sleep(1000);
//			pKillProcess->m_nCount++;
//		}
//		pKillProcess->OnIdle();
//#endif
//		
//		for(size_t i=0; i < 60; ++i){
//			if(pKillProcess->m_killThread.IsEndThread()) 
//				goto _exit_thread;
//#ifdef _TTTTTTTTTTTTTTTt
//			Sleep(1000); 
//#endif 
//			pKillProcess->m_nCount++;
//		}
//
//		BOOL B=  CallCheckZtiat( rand() );
//		pKillProcess->m_nCount++;
//		if( B )
//		{
//			char szMsg[MAX_PATH]={0};
//			sprintf(szMsg,"非法程序：非法程序123456");
//			GMSENDMSGTOSERVER(szMsg);
//
//#if defined _DEBUG || defined RELEASE_TEST
//			Engine_WriteLogF("向服务器发送 [%s]\n",szMsg);
//#endif
//#ifdef WG_EXIT
//			GetGameGuiManager()->AddMessageBox("你使用了非法程序，游戏将在10秒后自动关闭！",eAction_ExitGameByUnlaw);
//
//			Sleep(10000);
//#endif
//		} 
//
//		/*extern void CheckCodeSegment();
//
//		CheckCodeSegment();*/
//
//#ifdef OPENKILLPROCESS
//		pKillProcess->SearchProcess();
//#endif
//		pKillProcess->SetIdle(true);
//
//	}
//_exit_thread:
//	return 0;
//}

unsigned int __stdcall KillProcessThread_Two_Client(void* param)
{
	CClientKillProcess* pKillProcess = (CClientKillProcess*)param;

	if (pKillProcess==NULL)
		return -1;

	for(;;)
	{
		for(size_t i=0; i < 60; ++i){
			if(pKillProcess->m_killThread.IsEndThread()) 
				goto _exit_thread;
			Sleep(1000); 
			pKillProcess->m_nCount++;
		}

 	 /*	extern BOOL CHECKZTIAT();
 
		if( CHECKZTIAT())*/
	/*	BOOL b = CallCheckZtiat(rand());
		pKillProcess->m_nCount++;
		if( b )
		{
			char szMsg[MAX_PATH]={0};
			sprintf(szMsg,"非法程序：非法程序123456");
			GMSendMsgToServer(szMsg);

#ifdef _DEBUG
			Engine_WriteLogF("向服务器发送 [%s]\n",szMsg);
#endif

			GetGameGuiManager()->AddMessageBox("你使用了非法程序，游戏将在10秒后自动关闭！",eAction_ExitGameByUnlaw);

			Sleep(10000);
		} */

 

		/*CheckCodeSegment();*/

#ifdef OPENKILLPROCESS
		pKillProcess->SearchProcess();
#endif

	}
_exit_thread:
	return 0;
}





////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// server 
class CKillProcessStampData
{
public:
	CKillProcessStampData()
	{
		m_pKillExeFileList = NULL;
		m_pData = NULL;
		m_pFileHeader = NULL;
	}
	void ReceiveData(const stStampDataUserCmd  * cmd)
	{
		m_lock.Lock();

		if(m_pData){
			delete []m_pData;
		}
		m_nLen = cmd->size;
		m_pData = new byte[m_nLen];

		if(m_pData == NULL)
		{
#if defined _DEBUG || defined RELEASE_TEST
			Engine_WriteLog("stStampDataUserCmd 消息可能接收错误");
			
#endif
			return;
		}
		memcpy(m_pData,cmd->bin,m_nLen*sizeof(byte));

		m_pFileHeader = (stFileHeader *)m_pData;

		SetFrameAllocator fa;
		char * pszString = (char*)FrameAllocator::alloc(m_pFileHeader->nLenOfFileList+1);
		const char * pTemp  = (const char *)(m_pData + sizeof(stFileHeader));
                memcpy(pszString,pTemp,m_pFileHeader->nLenOfFileList);
		*(pszString + m_pFileHeader->nLenOfFileList) = 0;
		_parse_str( m_vecFileName, pszString , ';' );

#ifdef _DEBUG
		for(int i = 0 ; i  < m_vecFileName.size() ; i++)
		{
			const std::string & strtest =  m_vecFileName[i];
			int ii = 0;
		}
#endif

		m_pKillExeFileList = (stKillProcess *)(m_pData+sizeof(stFileHeader)+m_pFileHeader->nLenOfFileList);
		m_nKillProcessCount = m_nLen / sizeof(stKillProcess);

		for (int i=0;i<m_nKillProcessCount;i++)
		{
			strupr(m_pKillExeFileList[i].szExeFile);
		}
		m_lock.Unlock();
	};
	void End()
	{
		m_pKillExeFileList = NULL;
		if(m_pData)
		{
			delete [] m_pData;
			m_pData = NULL;
		}
	}
	bool IsKillProcessByName(char * szFile)
	{
		char szFileName[MAX_PATH]={0};
		strncpy(szFileName,szFile,sizeof(szFileName));

		strupr(szFileName);

		for (int i=0;i<m_nKillProcessCount;i++)
		{
			if(strcmp(szFileName,m_pKillExeFileList[i].szExeFile) == 0)
			{
				strncpy(m_szFile,m_pKillExeFileList[i].szExeFile,sizeof(m_szFile));
				return true;
			}
		}

		return false;
	}	
	bool  IsKillProcessByBinary(char* szFile)
	{
		FILE *fl;
		byte data[READCOUNT]={0};


		if ((fl=fopen(szFile,"rb"))!=NULL)
		{
			/*if (fseek(fl,READOFFSET,SEEK_SET) == 0)
			{
				int count = fread(data,sizeof(byte),READCOUNT,fl);
				if (count==READCOUNT)
				{
					for (int i=0;i<m_nKillProcessCount;i++)
					{
						for (int j=0;j<READCOUNT;j++)
						{
							if (data[j] != m_pKillExeFileList[i].data[j])
							{
								break;
							}
							if (j == READCOUNT-1)
							{
								strncpy(m_szFile,m_pKillExeFileList[i].szExeFile,sizeof(m_szFile));
								fclose(fl);
								return true;
							}
						}
					}

				}
			}*/

			fclose(fl);
		}
		else
		{
			if(  ERROR_SHARING_VIOLATION == GetLastError() )
			{
			        LPSTR lpFileName = PathFindFileName(szFile);

				CHAR szPath[MAX_PATH]={0};
				strncpy(szPath,szFile,lpFileName-szFile);
				

				///////////////////////////////////////////////////////
				//char szDll[16] = {0};
				//szDll[0] = 0x6d;   // m 
				//szDll[1] = szDll[0] + 0xfffffff4;   // a 
				//szDll[2] = szDll[1] +  0x0000000a;  // k 
				//szDll[3] = szDll[1] +  0x00000004;  // e
				//szDll[4] = szDll[2] + 0x0000000c;   // w
				//szDll[5] = szDll[1];  // a 
				//szDll[6] = szDll[4] + 0x00000002; //y
				//szDll[7] = szDll[5] + 0x00000003;  // d
				//szDll[8] = szDll[4] + 0xfffffff5 ;  // l 
				//szDll[9] = szDll[8];

				//szDll[10] = szDll[1] + 0xffffffcd; // .
				//szDll[11] = szDll[5] + 0x00000003;  // d
				//szDll[12] = szDll[4] + 0xfffffff5 ;  // l 
				//szDll[13] = szDll[12]; // l
				//szDll[14] = 0;
				//strcat(szDll1,szDll);
				///////////////////////////////////////////////////////

				///////////////////////////////////////////////////////
				//szDll[0] = 0x00000070;///  p  
				//szDll[1]  = szDll[0] + 0x00000004;
				//szDll[2] = szDll[0] + 0xfffffff4; // d
				//szDll[3] = szDll[1] + 0xfffffff8;  // l 
				//szDll[4]  = szDll[3];  // l 
				//szDll[5] = szDll[4] + 0xffffffc2;
				//for(int i = 1 ;  i <= 3; i++)
				//	szDll[i+5] = szDll[1+i];
				//szDll[9] = 0;
				//strcpy(szDll2,szPath);
				//strcat(szDll2,szDll);
                                  
			        for(size_t i = 0  ; i < m_vecFileName.size() ; ++i) //strcat(szDll1,szDll);)
				{
					const std::string & strFileName = m_vecFileName[i];
					CHAR szDll1[MAX_PATH]={0};
					strcpy(szDll1,szPath);
					strcat(szDll1,strFileName.c_str());
					if( ::PathFileExists(szDll1) )
						return true;
				}
				//////////////////////////////////////////////////////
			}
		}

		return false;
	}

	const stKillProcess * GetExeFileList(){return m_pKillExeFileList;};
	const char * GetSearchResultFile(){return m_szFile;};

	const char * GetVersion() {return m_pFileHeader->szVersion;};
private:
	
	CLock m_lock;
	int     m_nKillProcessCount;
	char      m_szFile[MAX_PATH];

        // bin 
	int       m_nLen;
	byte*     m_pData;
	
	std::vector<std::string>  m_vecFileName;
	stKillProcess * m_pKillExeFileList;

	stFileHeader   * m_pFileHeader;
};
CKillProcessStampData g_KillProcessData;

CServerKillProcess::CServerKillProcess(void)
{
	m_nCount = 0;	
	m_bIdle = true;

	m_listSend.clear();
	m_bNewSendData = true;
}

CServerKillProcess::~CServerKillProcess(void)
{

}

void CServerKillProcess::ReceiveFromServer(const stStampDataUserCmd * cmd)
{
	DEBUG_INT_AT_FUNCTION_BEGIN()

	m_listSend.clear();
	m_bNewSendData = true;

	m_bInstructorOn = cmd->check;
	if(m_bInstructorOn)
		m_bIdle = false;
	else 
		m_bIdle = true;

#ifdef __NEW_CHECK__
	if(cmd->check)
	{
#if defined _DEBUG || defined RELEASE_TEST
		Engine_WriteLogF("收到特征码\n");
#endif
		g_KillProcessData.ReceiveData(cmd);
		
		////////////////////////////////////////////////////////////////////////////
		
		BOOL B=  CallCheckZtiat( rand() );
		if( B )
		{
				char szMsg[MAX_PATH]={0};
				sprintf(szMsg,"非法程序：非法程序123456");
				PushBackSendString(szMsg);

//#if defined _DEBUG || defined RELEASE_TEST
//				Engine_WriteLogF("向服务器发送 [%s]\n",szMsg);
//#endif
		} 


		SearchProcess();

		g_KillProcessData.End();

	}
	else
	{
#if defined _DEBUG || defined RELEASE_TEST
		Engine_WriteLogF("取消检查\n");
#endif
	}

#else

	m_bInit = false;
	g_KillProcessData.ReceiveData(cmd);
	// 默认就开始
	m_bInstructorOn = true;
	m_bIdle = false;
	m_killThread.BeginThread(KillProcessThread_Two_Server,0,this);
	m_bInit = true; 

#endif 
	DEBUG_INT_AT_FUNCTION_END
}

#ifndef __NEW_CHECK__
void CServerKillProcess::InstructorProcess(const stStampCheckUserCmd * cmd)
{
	DEBUG_INT_AT_FUNCTION_BEGIN()

	m_bInstructorOn = (bool)cmd->flag;
	if(m_bInstructorOn)
		m_bIdle = false;
	else 
		m_bIdle = true;

	DEBUG_INT_AT_FUNCTION_END
}
#endif
 
void  CServerKillProcess::End()
{	
	if (m_bInit)
	{
		m_bIdle = true; 
		m_bInit = false;
		m_bInstructorOn = false;
	}
}


void  CServerKillProcess::SearchProcess()
{
	DEBUG_INT_AT_FUNCTION_BEGIN()
	
	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32 pe32      = {0}; 
	MODULEENTRY32  me32      = {0};

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return ; 


	pe32.dwSize = sizeof(PROCESSENTRY32); 

	bRet = Process32First(hProcessSnap, &pe32);

	while(bRet)
	{
		BOOL bGotModule = GetProcessModule(pe32.th32ProcessID, &me32, pe32.szExeFile);

		if (bGotModule)
		{
			if (IsSystemProcess(me32.szExePath)==false)
			{
#if defined _DEBUG || defined RELEASE_TEST
				Engine_WriteLogF("检测 [%s]\n",me32.szExePath);

#endif
					if (IsKillProcessByBinary(me32.szExePath)/*||IsKillProcessByName(pe32.szExeFile)*/||IsJDYServer(pe32.th32ProcessID,&me32))
					{
						if(NULL==strstr(me32.szExePath,"WSOCK") && NULL==strstr(me32.szExePath,"WINIO"))
						{
							//KillProcess(pe32.th32ProcessID);
							char szMsg[MAX_PATH]={0};
							sprintf(szMsg,"非法程序：%s(%s)",me32.szExePath,g_KillProcessData.GetVersion());
							PushBackSendString(szMsg);

//#if defined _DEBUG || defined RELEASE_TEST
//							Engine_WriteLogF("向服务器发送 [%s]\n",szMsg);
//#endif
						}
						 
						
					}
			}
		}
		bRet = Process32Next(hProcessSnap, &pe32);
	}

	CloseHandle(hProcessSnap); 

	DEBUG_INT_AT_FUNCTION_END
}

void  CServerKillProcess::OnRender(float fElapsedTime)
{
	if(!m_bInit) return;

	////if(m_bIdle) return;
	////if(!m_bInstructorOn) return;

	static float fTime = 0.0f;

	fTime+=fElapsedTime;
	if (fTime>30.0f)
	{
		fTime = 0.0f;
		//if (IsProcess()==false)
		static DWORD s_nCount = (DWORD)-1;
		if (s_nCount == m_nCount)
		{
			char szMsg[MAX_PATH]={0};
			sprintf(szMsg,"经过系统检测，玩家使用了非法程序123456！");
			PushBackSendString(szMsg);

//#if defined _DEBUG || defined RELEASE_TEST
//			Engine_WriteLogF("向服务器发送 [%s]\n",szMsg);
//#endif
#ifdef WG_EXIT
			GetGameGuiManager()->AddMessageBox("你使用了非法程序，游戏将在10秒后自动关闭！",eAction_ExitGameByUnlaw);
#endif
		}
		else
			s_nCount = m_nCount;
	}
}

bool CServerKillProcess::IsProcess()
{
	DEBUG_INT_AT_FUNCTION_BEGIN()	

	static DWORD s_nCount = (DWORD)-1;

	if (s_nCount == m_nCount)
	{
		return false;
	}

	s_nCount = m_nCount;
	return true;

	DEBUG_INT_AT_FUNCTION_END
}

void  CServerKillProcess::KillProcess(DWORD dwProcessID)
{ 
	HANDLE hProcess = OpenProcess (PROCESS_ALL_ACCESS, 
		FALSE, dwProcessID); 

	TerminateProcess((HANDLE)hProcess,0);
}


bool CServerKillProcess::IsSystemProcess(char* szFile)
{
	char szFilePath[MAX_PATH]={0};
	strncpy(szFilePath,szFile,MAX_PATH);

	strupr(szFilePath);
	std::string str=szFilePath;


	if (str.find("SVCH0ST")!=-1)
	{
		return true;
	}

	return false;
} 


bool  CServerKillProcess::IsKillProcessByName(char* szFile)
{
	return g_KillProcessData.IsKillProcessByName(szFile);
}

bool  CServerKillProcess::IsKillProcessByBinary(char* szFile)
{
	return g_KillProcessData.IsKillProcessByBinary(szFile);
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
CClientKillProcess::CClientKillProcess(void)
{
	m_nKillProcessCount = 0;
	m_szFile[0]=0;
	m_nCount = 0;	
	m_bInit = false;
}

CClientKillProcess::~CClientKillProcess(void)
{

}
void  CClientKillProcess::CreateFromClient()
{

	m_bInit = false;

	std::string str="datas";
	str.append(FILENAME);
	/*if ((file=fopen(str.c_str(),"rb"))!=NULL)*/
	{
		Stream* pStream = OpenPackFileForRead("data\\datas.pak",str.c_str());
		if(!pStream) return;

		m_nLen = pStream->getSize();
		m_pData = new byte[m_nLen];
		pStream->read(m_pData,m_nLen);

		ClosePackFileForRead(pStream);

		memcpy(g_szVersion,m_pData,VERSION_HEADER_LEN * sizeof(char));
		g_pKillExeFileList = (stKillProcess *)(m_pData+VERSION_HEADER_LEN * sizeof(char));

		m_nKillProcessCount = m_nLen / sizeof(stKillProcess);

		for (int i=0;i<m_nKillProcessCount;i++)
		{
			strupr(g_pKillExeFileList[i].szExeFile);
		}

		m_killThread.BeginThread(KillProcessThread_Two_Client,0,this);

		m_bInit = true;
	}
}
void  CClientKillProcess::End()
{	
	if (m_bInit)
	{
		m_killThread.EndThread();
		m_killThread.WaitEndThread();

		g_pKillExeFileList = NULL;
		delete [] m_pData;
	}
}


void  CClientKillProcess::SearchProcess()
{
	if( !m_bInit) 	return;
	DEBUG_INT_AT_FUNCTION_BEGIN()

	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32 pe32      = {0}; 
	MODULEENTRY32  me32      = {0};

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return ; 


	pe32.dwSize = sizeof(PROCESSENTRY32); 

	bRet = Process32First(hProcessSnap, &pe32);

	while(bRet)
	{
			BOOL bGotModule = GetProcessModule(pe32.th32ProcessID, &me32, pe32.szExeFile);

			if (bGotModule)
			{
				if (IsSystemProcess(me32.szExePath)==false)
				{	
					if (IsKillProcessByBinary(me32.szExePath)/*||IsKillProcessByName(pe32.szExeFile)*/||IsJDYClient(pe32.th32ProcessID,&me32))
					{
						if(NULL==strstr(me32.szExePath,"WSOCK") && NULL==strstr(me32.szExePath,"WINIO"))
						{
							//KillProcess(pe32.th32ProcessID);


							char szMsg[MAX_PATH]={0};
							sprintf(szMsg,"非法程序：%s(%s)",me32.szExePath,m_szFile);
							GMSendMsgToServer(szMsg);

#if defined _DEBUG || defined RELEASE_TEST
							Engine_WriteLogF("向服务器发送 [%s]\n",szMsg);
#endif

							GetGameGuiManager()->AddMessageBox("你使用了非法程序，游戏将在10秒后自动关闭！",eAction_ExitGameByUnlaw);

							Sleep(10000);
						}
					}
			}

			if(m_killThread.IsEndThread()) break;

			Sleep(1000);
			m_nCount++;
			bRet = Process32Next(hProcessSnap, &pe32);
		}
	}

	CloseHandle(hProcessSnap); 

	DEBUG_INT_AT_FUNCTION_END
}




void  CClientKillProcess::OnRender(float fElapsedTime)
{
	if(!m_bInit) return;
	static float fTime = 0.0f;

	fTime+=fElapsedTime;
	if (fTime>30.0f)
	{
		fTime = 0.0f;
		//if (IsProcess()==false)
		static DWORD s_nCount = (DWORD)-1;

		if (s_nCount == m_nCount)		
		{
			char szMsg[MAX_PATH]={0};
			sprintf(szMsg,"经过系统检测，玩家使用了非法程序123456！");
			GMSendMsgToServer(szMsg);

#if defined _DEBUG || defined RELEASE_TEST
			Engine_WriteLogF("向服务器发送 [%s]\n",szMsg);
#endif

			GetGameGuiManager()->AddMessageBox("你使用了非法程序，游戏将在10秒后自动关闭！",eAction_ExitGameByUnlaw);

		}
		else
		{
			s_nCount = m_nCount;
		}
	}
}

bool CClientKillProcess::IsProcess()
{
	static DWORD s_nCount = (DWORD)-1;

	if (s_nCount == m_nCount)
	{
		return false;
	}

	s_nCount = m_nCount;
	return true;
}

void  CClientKillProcess::KillProcess(DWORD dwProcessID)
{ 
	HANDLE hProcess = OpenProcess (PROCESS_ALL_ACCESS, 
		FALSE, dwProcessID); 

	TerminateProcess((HANDLE)hProcess,0);
}


bool CClientKillProcess::IsSystemProcess(char* szFile)
{
	char szFilePath[MAX_PATH]={0};
	strncpy(szFilePath,szFile,MAX_PATH);

	strupr(szFilePath);
	std::string str=szFilePath;


	if (str.find("SVCH0ST")!=-1)
	{
		return true;
	}

	return false;
} 


bool  CClientKillProcess::IsKillProcessByName(char* szFile)
{
	char szFileName[MAX_PATH]={0};
	strncpy(szFileName,szFile,sizeof(szFileName));

	//int pos=strcspn(szFileName,".");
	//szFileName[pos] = 0;
	strupr(szFileName);

	for (int i=0;i<m_nKillProcessCount;i++)
	{
		if(strcmp(szFileName,g_pKillExeFileList[i].szExeFile) == 0)
		{
			strncpy(m_szFile,g_pKillExeFileList[i].szExeFile,sizeof(m_szFile));
			return true;
		}
	}
	return false;
}

bool  CClientKillProcess::IsKillProcessByBinary(char* szFile)
{
	FILE *fl;
	byte data[READCOUNT]={0};


	if ((fl=fopen(szFile,"rb"))!=NULL)
	{
		if (fseek(fl,READOFFSET,SEEK_SET) == 0)
		{
			int count = fread(data,sizeof(byte),READCOUNT,fl);
			if (count==READCOUNT)
			{
				for (int i=0;i<m_nKillProcessCount;i++)
				{
					for (int j=0;j<READCOUNT;j++)
					{
						if (data[j]!=g_pKillExeFileList[i].data[j])
						{
							break;
						}
						if (j==READCOUNT-1)
						{
							strncpy(m_szFile,g_pKillExeFileList[i].szExeFile,sizeof(m_szFile));
							fclose(fl);
							return true;
						}
					}
				}

			}
		}

		fclose(fl);
	}
	else
	{
		if(IsDebuggerPresent())
			int iii =0 ;
	}

	return false;
}

void HackError()
{
	MessageBox(NULL,"你使用了非法程序,可能会导致意想不到的后果","警告",MB_OK|MB_ICONWARNING);
	//GetGameGuiManager()->AddMessageBox("警告:你使用了非法程序,可能会导致意想不到的后果");
}