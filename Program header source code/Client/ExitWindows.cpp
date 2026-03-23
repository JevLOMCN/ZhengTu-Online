/**********************************************************************************
/* File Name		:	ExitWindows.cpp
/*																				   
/* Author			:	Jais T Mathew (jaistmathew@yahoo.com.com)
/*																				   
/* Project			:	NetWork developement Toolkit
/*																				   
/* Module			:	Node Manager
/*																				   
/* Purpose			:	Declaration of the CExitWindows class
/*						Abstract functions for system management. Implements the 
/*						all of mixed enviroument.
/*																				   
/* Copy Right		:	Copyright ?ktssoftwares.Com
/*																				   
/* Notes			:															   
/*																				   
/* History			:															   
/*		Created		:   14 Dec 2002
/*		Version		:	1.0.0.0													   
/*																				   
/*		Updated #1	:	6 May 2002															   
/*																				   
/**********************************************************************************/

#include "public.h"
#include "ExitWindows.h"

/*
***********************************************************************************
	Purpose				:	Identifying which operating system is running on the machine. 

	Values	Populating	:	m_nVersionType is pupulating as WINNT if it is WINDOWS NT 4.0 or above
							else it will be WIN9X if os it 95 or above
*************************************************************************************
*/

CExitWindows::CExitWindows()
{
	//	OSVERSIONINFO osvi;
		
		m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		
		BOOL bval=GetVersionEx(&m_osvi);
		
		if(m_osvi.dwPlatformId==VER_PLATFORM_WIN32_NT)
		{
			m_nVersionType=WINNT;
		}
		else if(m_osvi.dwPlatformId=VER_PLATFORM_WIN32_WINDOWS)
		{
			m_nVersionType=WIN9X;
		}	


}

/*
***********************************************************************************
	Purpose				:	It will restart the Machine forcefully with 
							out allowing  save

	Values	Populating	:	return values will be FALSE in many cases. 
							1.if AdjustProcessTokenPrivilege() failed.
							2.Restart failed.
*************************************************************************************
*/
BOOL CExitWindows::ForceReStart()
{
	return ExitWindowsExt(EWX_REBOOT|EWX_FORCE,0);
}
/*
***********************************************************************************
	Purpose				:	It will logoff current user forcefully with 
							out allowing  save. this codeis common for both NT and 9X Os

	Values	Populating	:	return values will be FALSE in many cases. 
							1.Logoff failed.
*************************************************************************************
*/

BOOL CExitWindows::ForceLogOff()
{
		return ExitWindowsEx(EWX_LOGOFF|EWX_FORCE,0);
}
BOOL CExitWindows::ForceShutDown()
{	
	return ExitWindowsExt(EWX_SHUTDOWN|EWX_FORCE,0);
}

/************************************************************************************
	Purpose			:Adjusting the Privilage of the Process to Shut Down the machine.

	Return Values	:	 if failed	OPENING_PROCESS_TOKEN_FAILED,
									ADJUST_PRIVILEGE_FAILED,
									ADJUST_TOCKEN_FAILED
						
						 if Success	ADJUST_TOCKEN_SUCESS
**************************************************************************************/

BOOL CExitWindows::AdjustProcessTokenPrivilege()
{
	HANDLE hToken; 

	TOKEN_PRIVILEGES tkp; 
 
	if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
			return OPENING_PROCESS_TOKEN_FAILED;
	}
 
	if(!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid))
	{
			return ADJUST_PRIVILEGE_FAILED;
	}
 
	tkp.PrivilegeCount = 1;

	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES)NULL, 0); 
 
	if (GetLastError() != ERROR_SUCCESS) 
	{
			return ADJUST_TOCKEN_FAILED;
	}
	

	return ADJUST_TOCKEN_SUCESS;
}

/************************************************************************************
	Purpose				:	This function is declared to customize the purpose of the class. 
							You can use this function for any kind of operation related to Exit 
							Windows by passing Values to the parameter with out bothering about 
							the os and Token Adjusting. The second parameter is reserved for 
							future use.

	Values	Populating	:	return values will be FALSE in many cases. 
							1.If AdjustProcessTokenPrivilege() failed.
							2.If operation failed.
**************************************************************************************/

BOOL CExitWindows::ExitWindowsExt(UINT nFlag, DWORD dwType)
{
	int iRetval(0);

	switch(m_nVersionType)
	{
	case WINNT:
		{
			if((iRetval=AdjustProcessTokenPrivilege())==ADJUST_TOCKEN_SUCESS)
			{
				return ExitWindowsEx(nFlag,dwType);
			}
			else
			{
				return iRetval;
			}
			break;
		}
	case WIN9X:
		{
			return ExitWindowsEx(nFlag,dwType);
			break;
		}
	}
	return FALSE;
}

UINT CExitWindows::GetOsVersion()
{
	return m_nVersionType;
}
OSVERSIONINFO CExitWindows::GetOsVersionInfo()const
{
	return m_osvi;
}
/*
***********************************************************************************
	Purpose				:	It will shutdown the Machine,, after prompting save

	Values	Populating	:	return values will be FALSE in many cases. 
							1.if AdjustProcessTokenPrivilege() failed.
							2.ShutDown failed.
*************************************************************************************
*/
BOOL CExitWindows::ShutDown()
{
	return ExitWindowsExt(EWX_SHUTDOWN,0);
}
/*
***********************************************************************************
	Purpose				:	It will restart the machine after prompting save

	Return Value		:	return values will be FALSE in many cases. 
							1.if AdjustProcessTokenPrivilege() failed.
							2.restart failed.
*************************************************************************************
*/
BOOL CExitWindows::Restart()
{
	return ExitWindowsExt(EWX_REBOOT,0);
}

/*
***********************************************************************************
	Purpose				:	It will logoff the machine after prompting save

	Return Value		:	return values will be FALSE if restart failed. 

*************************************************************************************
*/
BOOL CExitWindows::LogOff()
{
	return ExitWindows(EWX_LOGOFF,0);
}
/*
***********************************************************************************
	Purpose				:	It using to set the Application to reserve last shutdown 
							range. This is possible only in NT.

	Return Value		:	return values will be FALSE if setting  failed.. 

*************************************************************************************
*/
BOOL CExitWindows::SetItAsLastShutDownProcess()
{
	if(!SetProcessShutdownParameters(0x100,0))
		return FALSE;
	

	return TRUE;
}