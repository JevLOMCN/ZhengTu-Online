#include "public.h"
#include "./GameHookThread.h"
#include "../engine/include/thread.h"


CThread  g_HookThread;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nCode : ÃèÊö
 * \param wParam : ÃèÊö
 * \param lParam : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
static LRESULT WINAPI MouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	FUNCTION_BEGIN;

	return 0;

	FUNCTION_END;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param nCode : ÃèÊö
* \param wParam : ÃèÊö
* \param lParam : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
static LRESULT WINAPI KeyProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	FUNCTION_BEGIN;

	return 0;

	FUNCTION_END;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nCode : ÃèÊö
 * \param wParam : ÃèÊö
 * \param lParam : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
static LRESULT WINAPI MouseProc1(int nCode,WPARAM wParam,LPARAM lParam)
{
	FUNCTION_BEGIN;

	return 0;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param param : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
unsigned __stdcall GameHookThread( void * param)
{
	FUNCTION_BEGIN;

	CThread* pThread = (CThread*)param;
	HWND hWnd = Engine_GetHWnd();
	HHOOK hook = NULL;
	HHOOK hook1 = NULL;
	HHOOK hookKey = NULL;
	while(!pThread->IsEndThread())
	{
		if(hook)
		{
			UnhookWindowsHookEx(hook);
			hook = NULL;
		}

		hook = SetWindowsHookEx(WH_MOUSE,MouseProc,Engine_GetAppation()->m_hInstance,0);

		if(hook1)
		{
			UnhookWindowsHookEx(hook1);
			hook1 = NULL;
		}

		hook1 = SetWindowsHookEx(WH_MOUSE_LL,MouseProc1,Engine_GetAppation()->m_hInstance,0);
		
		if(hookKey)
		{
			UnhookWindowsHookEx(hookKey);
			hookKey = NULL;
		}

		hookKey = SetWindowsHookEx(WH_KEYBOARD,KeyProc,Engine_GetAppation()->m_hInstance,0);

		Sleep(1000);
		//Sleep(rand() % 2000 + 3000);
	}
	if(hook)
	{
		UnhookWindowsHookEx(hook);
		hook = NULL;
	}
	if(hook1)
	{
		UnhookWindowsHookEx(hook1);
		hook1 = NULL;
	}
	if(hookKey)
	{
		UnhookWindowsHookEx(hookKey);
		hookKey = NULL;
	}
	return 0;

	FUNCTION_END;
}

static CThread sHookThread;
void BeginHookThread()
{
	//sHookThread.BeginThread(GameHookThread,0,&sHookThread);
}

void EndHookThread()
{
	//sHookThread.EndThread();
	//sHookThread.WaitEndThread();
}