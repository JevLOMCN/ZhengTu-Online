// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "ViewGL.h"

#include "MainFrm.h"
#include "resource.h"
#include "ViewGLDoc.h"
#include "ViewGLView.h"
#include "FileTreeView.h"
#include ".\mainfrm.h"
#include ".\tooldlg.h"

#include "tgaCore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//extern BOOL InitD3DDevice(HWND hWnd);
//extern BOOL DestroyD3DDevice();
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_COV_TOOL, OnFileCovTool)
	ON_CBN_SELCHANGE(ID_BLEND_TYPE,OnBlendType)
	ON_WM_DESTROY()
	ON_COMMAND(ID_32793, On32793)
	ON_COMMAND(ID_PRE_BITMAP, OnPreBitmap)
	ON_COMMAND(ID_NEXT_BITMAP, OnNextBitmap)
	ON_NOTIFY(NM_RELEASEDCAPTURE, ID_SLIDER_SPEED, OnNMReleasedcaptureSlider1)
//	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CRC,
	ID_INDICATOR_POSITION,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_BITMAP,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
: m_BlendType(Blend_Null)
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	WNDCLASSEX wc;
	char szClass[MAX_PATH];
	GetClassName(m_hWnd,szClass,MAX_PATH);
	wc.cbSize = sizeof(WNDCLASSEX);
	GetClassInfoEx(AfxGetInstanceHandle(),szClass,&wc);
	DWORD dwStyle,dwStyleEx;
	dwStyle = GetWindowLong(m_hWnd,GWL_STYLE);
	dwStyleEx = GetWindowLong(m_hWnd,GWL_EXSTYLE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	// TODO: 如果不需要工具栏可停靠，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	int ix = m_wndToolBar.CommandToIndex(ID_BLEND_TYPE);
	m_wndToolBar.SetButtonInfo(ix,ID_BLEND_TYPE,TBBS_SEPARATOR,150);
	CRect rect;
	m_wndToolBar.GetItemRect(ix,&rect);
	//rect.top = 0;
	rect.bottom = rect.top + 200;

	m_wndBlendType.Create(CBS_DROPDOWNLIST|WS_VISIBLE|WS_TABSTOP|WS_HSCROLL|WS_VSCROLL,rect,&m_wndToolBar,ID_BLEND_TYPE);
	m_wndBlendType.SetFont(m_wndToolBar.GetFont(),TRUE);
	m_wndBlendType.AddString(_T("Blend_Null"));
	m_wndBlendType.AddString(_T("Blend_Add"));
	m_wndBlendType.AddString(_T("Blend_Add2"));
	m_wndBlendType.AddString(_T("Blend_AlphaAdd"));
	m_wndBlendType.AddString(_T("Blend_AlphaAdd2"));
	m_wndBlendType.AddString(_T("Blend_Bright"));
	m_wndBlendType.AddString(_T("Blend_IgnoreTexelColor"));
	m_wndBlendType.AddString(_T("Blend_ColorAdd"));
	m_wndBlendType.AddString(_T("Blend_AlphaAdd3"));
	m_wndBlendType.AddString(_T("Blend_AlphaSub"));
	m_wndBlendType.AddString(_T("Blend_Monochrome"));
	m_wndBlendType.SetCurSel(0);

	ix = m_wndToolBar.CommandToIndex(ID_SLIDER_SPEED);
	m_wndToolBar.SetButtonInfo(ix,ID_SLIDER_SPEED,TBBS_SEPARATOR,150);

	m_wndToolBar.GetItemRect(ix,&rect);
	rect.bottom = rect.top + 200;
	m_wndSlider.Create(TBS_HORZ|TBS_NOTICKS|TBS_TOP|TBS_LEFT|WS_VISIBLE ,rect,&m_wndToolBar,ID_SLIDER_SPEED);
	m_wndSlider.SetRange(0,100);	
	m_wndSlider.SetPos(100);

	//m_wndSplitter.GetPane(0,1)->Invalidate(true);

	/*if(!InitD3DDevice(::GetDesktopWindow()))
	{
		return -1;
	}*/
	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	m_wndSplitter.CreateStatic(this,1,2);
	m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CFileTreeView),CSize(100,200),pContext);
	m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CViewGLView),CSize(0,0),pContext);
	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序


void CMainFrame::OnFileCovTool()
{
	// TODO: 在此添加命令处理程序代码
	CToolDlg dlg;
	dlg.DoModal();
}


void CMainFrame::OnDestroy()
{
	//DestroyD3DDevice();
	CFrameWnd::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

void CMainFrame::OnBlendType()
{
	m_BlendType = (BlendType)m_wndBlendType.GetCurSel();
	GetViewGLView()->RedrawWindow();
}



void CMainFrame::On32793()
{
	// TODO: 在此添加命令处理程序代码
	CString strFile = "E:\\TESt\\1_70001.tga";

	// tga1 
	CGraphicsFileData tga;
	tga.LoadTGAFile(strFile);

	CGraphicsFileData::stFileData * pData = (CGraphicsFileData::stFileData *)tga.m_pstFileData;

	int nTgaHeight = tga.GetHeight();
	int nTgaWidth  = tga.GetWidth();


	int index = 0;
	 bool b_1 = true;
	for(size_t i=0;i<nTgaHeight;++i)
	{
		// long rowStride = (nTgaWidth2 + 3) & (~3); // DWORD aligned
		for(size_t j=0;j<nTgaWidth;++j)
		{
			UINT a = pData[index].A;
			index++;
			if( a > 0x0f)
			{
				    TRACE("第一个alpha点:%d,%d\n",i,j);
				    b_1 = false;
			}

		}
	}

}

void CMainFrame::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	GetViewGLView()->EndAutoShow();
	GetViewGLView()->BeginAutoShow( (101 - m_wndSlider.GetPos()) * 50);
}

void CMainFrame::OnPreBitmap()
{
	if(GetViewGLView()->ShowPreBitmap())
	{				
		//UpdateDialogControls()
	}
}

void CMainFrame::OnNextBitmap()
{
	if(GetViewGLView()->ShowNextBitmap())
	{
		
	}
}

//BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	ShowCursor(TRUE);
//	return CFrameWnd::OnSetCursor(pWnd, nHitTest, message);
//}
