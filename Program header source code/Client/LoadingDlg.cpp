#include "public.h"
#include "./resource.h"
#include "./Game.h"
#include "./GameAppation.h"
#include "../engine/include/Thread.h"
#include <commctrl.h>

static HWND g_hLoadingDlg = NULL;
static CThread g_thread;

#define WM_ENDLOADINGDLG (WM_USER + 100)

static BOOL CALLBACK LoadingDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case WM_INITDIALOG:
		{
			g_hLoadingDlg = hwndDlg;
			stRectI rectWindow;
			GetWindowRect(hwndDlg,&rectWindow);

			/*HWND hAni = GetDlgItem(hwndDlg,IDC_LOADING);
			if(hAni){
				if( ::SendMessage(hAni, ACM_OPEN, (WPARAM)GetModuleHandle(NULL), (LPARAM)"loading.avi")){
				}else{
					ShowWindow(hAni,SW_HIDE);
				}
			}*/
			int cx = GetSystemMetrics(SM_CXSCREEN);
			int cy = GetSystemMetrics(SM_CYSCREEN);
			int x = (cx - rectWindow.Width())/2;
			int y = (cy - rectWindow.Height())/2;
			//MoveWindow(hwndDlg,x,y,rectWindow.Width(),rectWindow.Height(),TRUE);
#if defined _DEBUG || defined RELEASE_TEST
			SetWindowPos(hwndDlg,HWND_TOP,x,y,rectWindow.Width(),rectWindow.Height(),0);
#else
			SetWindowPos(hwndDlg,HWND_TOPMOST,x,y,rectWindow.Width(),rectWindow.Height(),0);
#endif
		}
		break;
	case WM_PAINT:
		{
			stRectI rectWindow;
			GetWindowRect(hwndDlg,&rectWindow);
			int cx = GetSystemMetrics(SM_CXSCREEN);
			int cy = GetSystemMetrics(SM_CYSCREEN);
			int x = (cx - rectWindow.Width())/2;
			int y = (cy - rectWindow.Height())/2;
			MoveWindow(hwndDlg,x,y,rectWindow.Width(),rectWindow.Height(),FALSE);
		}
		break;
	case WM_ENDLOADINGDLG:
		EndDialog(hwndDlg,1);
		return TRUE;
		break;
	}
	return FALSE;

}

unsigned int  __stdcall LoadingDlgThread( void * param )
{
	return DialogBox( Engine_GetAppation()->m_hInstance, MAKEINTRESOURCE(IDD_LOADINGDLG) , NULL,LoadingDlgProc );
}

void ShowLoadingDlg()
{
#ifndef _DEBUG
	g_thread.BeginThread(LoadingDlgThread);
#endif
}

void CloseLoadingDlg()
{
#ifndef _DEBUG
	if(g_hLoadingDlg)
	{
		SendMessage(g_hLoadingDlg,WM_ENDLOADINGDLG,0,0);
		SetForegroundWindow(Engine_GetAppation()->GetWnd());
		g_hLoadingDlg = NULL;
	}
#endif
}