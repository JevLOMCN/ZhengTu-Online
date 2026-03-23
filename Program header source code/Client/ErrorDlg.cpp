#include "public.h"
#include "./ErrorDlg.h"
#include "./resource.h"
#include "./Game.h"
#include "./GameAppation.h"
#include<wininet.h>
#pragma comment(lib,"Wininet.lib")

static const char* sStrMsg = NULL;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pStr : 描述
 * \return 返回值的描述
 */
std::string StrToHttpMsg(const char* pStr)
{
	std::string msg;
	while(*pStr)
	{
		char temp[8];
		if(*pStr & 0x80)
		{
			if(*(pStr+1) == 0) break;
			sprintf(temp,"%02x",(int)(BYTE)*pStr);
			msg += '%';
			msg += temp;
			pStr++;
			sprintf(temp,"%02x",(int)(BYTE)*pStr);
			msg += '%';
			msg += temp;
			pStr++;
		}
		else if( *pStr < 48 || *pStr > 122 )
		{
			sprintf(temp,"%02x",(int)(BYTE)*pStr);
			msg += '%';
			msg += temp;
			pStr++;
		}
		else
		{
			msg += *pStr++;
		}
	}
	return msg;

}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  发送错误报告。
*				  格式：	提交人帐号=account.com&Bug类别=系统问题&角色名字=playerName&日志=log&描述=userDesc
*/
void  SendError(const char* account,const char* playerName,const char* log,const char* userDesc,bool bDebug)
{

	const char * szAddr = GetGameApplication()->GetConfig()->szBugAddr;
	const char * szObj = GetGameApplication()->GetConfig()->szBugObj;

	//const char * szAddr = "192.168.2.11";
//	const char * szObj = "/ztgame/webreg/bugreport.php";
    const char* headMsg="Content-Type: application/x-www-form-urlencoded";

	std::string  SendMsg;

	SendMsg.reserve(8192);

	SendMsg = "__EVENTTARGET=1&";
	SendMsg += StrToHttpMsg("Bug类别");
	SendMsg += "=";
	if( bDebug )
	   SendMsg += StrToHttpMsg("调试");
	else
	   SendMsg += StrToHttpMsg("程序异常");

	SendMsg += "&";
	SendMsg += StrToHttpMsg("提交人帐号");
	SendMsg += "=";
	SendMsg += StrToHttpMsg(account);
	SendMsg += "&";
	SendMsg += StrToHttpMsg("角色名字");
	SendMsg += "=";
	SendMsg += StrToHttpMsg(playerName);
	SendMsg += "&";
	SendMsg += StrToHttpMsg("日志");
	SendMsg += "=";
	SendMsg += StrToHttpMsg(log);
	SendMsg += "&";
	SendMsg += StrToHttpMsg("描述");
	SendMsg += "=";
	SendMsg += StrToHttpMsg(userDesc);

	 HINTERNET hOpen, hConnect;
	 if ( !(hOpen = InternetOpen("mircosotIE", INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0)))
	 {
		 return ;
	 }
	 if (!(hConnect = InternetConnect(hOpen, szAddr ,INTERNET_DEFAULT_HTTP_PORT, "","",INTERNET_SERVICE_HTTP, 0, NULL) ))
	 {
	     InternetCloseHandle(hOpen);
		 return ;
	 }
    
	 LPCTSTR  bstrRemoteFile = szObj;
	 HINTERNET hRequest = HttpOpenRequest(hConnect, "POST",(LPCTSTR)bstrRemoteFile,NULL, NULL, NULL,0,0);

	 if (!hRequest)
	 {
		 InternetCloseHandle(hConnect);
		 InternetCloseHandle(hOpen);
		 return ;
	 }
	 if(!HttpSendRequest(hRequest,headMsg,strlen(headMsg),(void*)SendMsg.c_str(),SendMsg.length()))
	 {
		 InternetCloseHandle(hRequest);
		 InternetCloseHandle(hConnect);
		 InternetCloseHandle(hOpen);
		 return ;
	 }
	 InternetCloseHandle(hRequest);
	 InternetCloseHandle(hConnect);
	 InternetCloseHandle(hOpen);

}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszUserDesc : 描述
 * \return 返回值的描述
 */
void SendError(const char* pszUserDesc,bool bDebug = false)
{
	extern const char* c_szFileName;
	char m_pszUserDesc[1024]={0};

	CIni ini(c_szFileName);
	strncpy(m_pszUserDesc,ini.GetString("Server","servername","").c_str(),sizeof(m_pszUserDesc));

	strncat(m_pszUserDesc,pszUserDesc,sizeof(m_pszUserDesc));

	SendError(CAccoutSaveInfo::GetSingleton()->GetUserID().c_str(),CAccoutSaveInfo::GetSingleton()->GetUserName().c_str(),sStrMsg,m_pszUserDesc,bDebug);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param hwndDlg : 描述
 * \param message : 描述
 * \param wParam : 描述
 * \param lParam : 描述
 * \return 返回值的描述
 */
static BOOL CALLBACK ErrorDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDOK:
			{
				HWND hwnd = GetDlgItem(hwndDlg,IDC_EDIT_DESC);
				char szUserDesc[1024];
				GetWindowText(hwnd,szUserDesc,1024);
				szUserDesc[1023] = 0;
				SendError(szUserDesc);
				EndDialog(hwndDlg,IDOK);
			}
			return TRUE;
			break;
		case IDCANCEL:
			EndDialog(hwndDlg,IDCANCEL);
			return TRUE;
			break;
		}
		break;
	}
	return FALSE;

}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void ShowErrorDlg(const char* msg)
{
	sStrMsg = msg;
	DialogBox(Engine_GetAppation()->m_hInstance,MAKEINTRESOURCE(IDD_DIALOG_ERROR),NULL,ErrorDlgProc);
}