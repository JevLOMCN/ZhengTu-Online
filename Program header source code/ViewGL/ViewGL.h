// ViewGL.h : ViewGL 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号
#include "../Engine/include/engine.h"
class CViewGLView;
// CViewGLApp:
// 有关此类的实现，请参阅 ViewGL.cpp
//

class CViewGLApp : public CWinApp
{
public:
	CViewGLApp();

	ULONG_PTR gdiplusToken;
// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	int Run();
};

extern CViewGLApp theApp;
extern CViewGLView* GetViewGLView();
