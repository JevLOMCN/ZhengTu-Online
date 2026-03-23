// ViewGL.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "ViewGL.h"
#include "MainFrm.h"

#include "ViewGLDoc.h"
#include "ViewGLView.h"
#include ".\viewgl.h"
#include "./MainFrm.h"
#include "./ViewGLView.h"

#include "../engine/include/IGraphicCursor.h"
#include "../Engine/include/Fps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CViewGLApp

BEGIN_MESSAGE_MAP(CViewGLApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CViewGLApp 构造

CViewGLApp::CViewGLApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CViewGLApp 对象

CViewGLApp theApp;

// CViewGLApp 初始化

BOOL CViewGLApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	Engine_Init();
	/*Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);*/

	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CViewGLDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CViewGLView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// 启用“DDE 执行”
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当存在后缀时才调用 DragAcceptFiles，
	//  在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生
	// 启用拖/放
	m_pMainWnd->DragAcceptFiles();

	
	if(!GetDevice()->Create(GetViewGLView()->GetSafeHwnd(),GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),32,0,eProgram_ViewGL))
	{
		AfxMessageBox(_T("创建Ｄ３Ｄ设备失败！"));
		return FALSE;
	}
	GetGraphicCursor()->loadCursor("c:\\cur1.tga",CPoint(2,2));
	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CViewGLApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

int CViewGLApp::Run()
{
	// TODO: 在此添加专用代码和/或调用基类

	ASSERT_VALID(this);
	_AFX_THREAD_STATE* pState = AfxGetThreadState();

	// for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;

	CViewGLView* pView = GetViewGLView();
	// acquire and dispatch messages until a WM_QUIT message is received.
	CFps fps(30);
	for (;;)
	{
		// phase1: check to see if we can do idle work
		//while (bIdle &&
		//	!::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE))
		while (!::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE))
		{
			if(bIdle)
			{
				// call OnIdle while in bIdle state
				if (!OnIdle(lIdleCount++))
					bIdle = FALSE; // assume "no idle" state
			}
			Engine_GetFps()->BeginFrame();
			Engine_GetState()->Refresh();
			pView->Draw();
			Engine_GetFps()->EndFrame();
			//fps.RunFrame(Engine_GetState()->GetElapsedTime());
		}

		// phase2: pump messages while available
		do
		{
			// pump message, but quit on WM_QUIT
			if (!PumpMessage())
				return ExitInstance();

			// reset "no idle" state after pumping "normal" message
			//if (IsIdleMessage(&m_msgCur))
			if (IsIdleMessage(&(pState->m_msgCur)))
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}

		} while (::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE));
	}
	return -1;
	//return CWinApp::Run();
}


// CViewGLApp 消息处理程序


int CViewGLApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	GetDevice()->Destroy();
	Engine_Uninit();
	//Gdiplus::GdiplusShutdown(gdiplusToken);
	return CWinApp::ExitInstance();
}

CViewGLView* GetViewGLView()
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	return (CViewGLView*) pMainWnd->m_wndSplitter.GetPane(0,1);
}