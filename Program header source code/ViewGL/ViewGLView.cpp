// ViewGLView.cpp : CViewGLView 类的实现
//

#include "stdafx.h"
#include "ViewGL.h"

#include "ViewGLDoc.h"
#include "ViewGLView.h"
#include ".\viewglview.h"
#include "./MainFrm.h"
#include "../engine/include/IGraphicCursor.h"
#include <MMSystem.h>

#pragma  comment(lib,"winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CViewGLView

IMPLEMENT_DYNCREATE(CViewGLView, CScrollView)

BEGIN_MESSAGE_MAP(CViewGLView, CScrollView)
	// 标准打印命
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
//	ON_COMMAND(ID_SET_BK_COLOR, OnSetBkColor)
ON_WM_ERASEBKGND()
//ON_COMMAND(ID_SHOW_CLIP, OnShowClip)
//ON_UPDATE_COMMAND_UI(ID_SHOW_CLIP, OnUpdateShowClip)
ON_WM_TIMER()
//ON_COMMAND(ID_AUTO_SHOW, OnAutoShow)
//ON_UPDATE_COMMAND_UI(ID_AUTO_SHOW, OnUpdateAutoShow)
ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CViewGLView 构造/析构

CViewGLView::CViewGLView()
{
	// TODO: 在此处添加构造代码
	//m_hBmp = NULL;
	m_Timer = 0;
	m_iSpeed = 50;
}

CViewGLView::~CViewGLView()
{
	//EndAutoShow();
	
	//SetTimer(0,1000,NULL);
	//if(m_hBmp) DeleteObject(m_hBmp);
}

BOOL CViewGLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式
	
	return CScrollView::PreCreateWindow(cs);
}

// CViewGLView 绘制
void CViewGLView::OnDraw(CDC* /*pDC*/)
{
}

void CViewGLView::Draw()
{	
	CViewGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	IDevice* pDevice = GetDevice();
	if(!pDevice) return ;
	if(!pDevice->IsCreated()) return;
	if(!pDevice->TestCooperativeLevel())
		return;
	pDevice->BeginScene();
	pDevice->Clear(pDoc->m_bkColor);

	CPoint ptOffset = -GetScrollPosition();

	CRect rcClient;
	GetClientRect(&rcClient);

	if(pDoc->m_pBackImage)
	{
		int nWidth = 1024;//pDoc->m_pBackImage->m_width;
		int nHeight = 1024;//pDoc->m_pBackImage->m_height;
		stRectF rcShow(0,0,nWidth,nHeight);
		rcShow.OffsetRect(ptOffset);
		rcShow &= rcClient;
		rcShow.OffsetRect(-ptOffset);
		pDoc->m_pBackImage->Render(0,0,&rcShow,NULL,NULL,Blend_Null);
	}
	if(pDoc->m_pBitmap)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		DWORD color = -1;
		if(pFrame->m_BlendType == Blend_Monochrome)
			color = 0;
                CRect rcShow(0,0,pDoc->m_pBitmap->GetWidth(),pDoc->m_pBitmap->GetHeight());
		rcShow.OffsetRect(ptOffset);
		rcShow.IntersectRect(rcShow,rcClient);
		rcShow.OffsetRect(-ptOffset);
	        
		pDoc->m_pBitmap->Render(0,0,&rcShow,NULL,color,pFrame->m_BlendType);
		if( pDoc->m_bShowClip)
		{			
			CRect __rcShow(0,0,pDoc->m_pBitmap->GetWidth(),pDoc->m_pBitmap->GetHeight());
			rcShow.OffsetRect(ptOffset);
			static DWORD clrBmp = COLOR_ARGB(255,255,255,255);
			pDevice->DrawRectangle(&rcShow,clrBmp);

			CRect rcClip = pDoc->m_pBitmap->GetClipRect();
			rcClip.OffsetRect(ptOffset);
			//rcClip.IntersectRect(rcClip,rcClient);
			//rcClip.OffsetRect(-ptOffset);
			
			static DWORD clrClip = COLOR_ARGB(255,117,248,54);
			pDevice->DrawRectangle(&rcClip,clrClip);
                            
		        int nHeightBlock = pDoc->m_pBitmap->GetHeightBlockNum();
			int nWidthBlock = pDoc->m_pBitmap->GetWidthBlockNum();
			for(int i = 0 ; i< nHeightBlock; i++)
				for(int j=0;j<nWidthBlock;j++)
				{
					CRect BRect;
					pDoc->m_pBitmap->CalculateBlockClipRect(j,i,&BRect);
					BRect.OffsetRect(ptOffset);

					CPoint pt;
				        pt.x = rcClip.left;
					pt.y = rcClip.top;
					BRect.OffsetRect(pt);
					pt.x = j*256;
					pt.y = i * 256;
					BRect.OffsetRect(pt);
					pDevice->DrawRectangle(&BRect,clrClip);

				}
			if( pDoc->m_bShowAlpha)
			{
				CRect _rcClip = pDoc->m_pBitmap->GetClipRect();
				int _nClipWidth = _rcClip.Width();
				int _nClipHeight = _rcClip.Height();

				int _nBitmapHeight = pDoc->m_pBitmap->GetWidth();
				int _nBitmapWidth = pDoc->m_pBitmap->GetHeight();



				int nPitch = _nClipWidth * 4; // 4 byte per pixel  and nClipWidth pixels 
				DWORD * pBitmapData = new DWORD[_nClipWidth * _nClipHeight]; // pixel * pixel 
				if( !pBitmapData ) 
					goto __end;

				if(!pDoc->m_pBitmap->GetBitmapData(pBitmapData,nPitch)) // 裁剪区数据
				{
					delete [] pBitmapData;
					goto __end;
				}

				//CRgn rgnOK;
				//rgnOK.CreateRectRgn(0,0,0,0);
				CPoint pt(rcClip.left,rcClip.top);

				for(int wpixel = 0 ; wpixel<_nClipWidth; wpixel ++)
				{
					for(int hpixel = 0; hpixel<_nClipHeight; hpixel++)
					{									
						DWORD a = pBitmapData[wpixel + hpixel * _nClipWidth];
						a = (a&0xff000000) >> 24;

						if(a > 0x0f)
						{
							CRgn rgn;
							CRect rect;
							rect.left = wpixel;
							rect.top = hpixel;
							rect.right = wpixel+5;
							rect.bottom = hpixel+5;
							rect.OffsetRect(pt);
							static DWORD _clr = COLOR_ARGB(255,255,255,255);
							pDevice->DrawRectangle(&rect,_clr);
						}
					}
				}

				delete [] pBitmapData;
			}
		}
	}
__end:
	pDevice->EndScene();
	CPoint pt = GetScrollPosition();
	CRect rc;
	GetWindowRect(&rc);
	//CRect rcSrc(pt.x,pt.y,pt.x + rc.Width(),pt.y + rc.Height());
	//CRect rcSrc(0,0,rc.Width(),rc.Height());
	CRect rcDst(0,0,rc.Width(),rc.Height());
	//pDevice->Present(&rcDst,&rcSrc);
	pDevice->Present(&rcDst);
	// TODO: 在此处为本机数据添加绘制代码
	/*
	Gdiplus::Bitmap* bmp = NULL;
	
	if(pDoc->m_grp.GetCurBlock())
		bmp = pDoc->m_grp.GetCurBlock()->GetBitmap();

	if(bmp)
	{
		HRESULT hr;
		CRect rcClip;
		pDC->GetClipBox(&rcClip);
		
		Gdiplus::Graphics grp(pDC->GetSafeHdc());
		
		CRect rc(0,0,pDoc->m_grp.GetCurBlock()->GetWidth(),pDoc->m_grp.GetCurBlock()->GetHeight());
		
		Gdiplus::Region  rgn(Gdiplus::Rect(rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height()));
		
		rgn.Xor(Gdiplus::Rect(0,0,rc.Width(),rc.Height()));

		Gdiplus::SolidBrush blackBrush1( Gdiplus::Color(255, 255, 255, 255) );
		grp.FillRegion(&blackBrush1,&rgn);

		rc.IntersectRect(&rc,&rcClip);
		Gdiplus::SolidBrush blackBrush( Gdiplus::Color(255, GetRValue(pDoc->m_bkColor), GetGValue(pDoc->m_bkColor), GetBValue(pDoc->m_bkColor)) );
		grp.FillRectangle(&blackBrush,rc.left,rc.top,rc.Width(),rc.Height());
		DWORD dwTime1 = timeGetTime();

		Gdiplus::ImageAttributes imAtt;

		//imAtt.SetOutputChannel(Gdiplus::ColorChannelFlagsC, Gdiplus::ColorAdjustTypeBitmap);
		//imAtt.SetOutputChannel(Gdiplus::ColorChannelFlagsM, Gdiplus::ColorAdjustTypeBitmap);
		//imAtt.SetOutputChannel(Gdiplus::ColorChannelFlagsY, Gdiplus::ColorAdjustTypeBitmap);
		//imAtt.SetOutputChannel(Gdiplus::ColorChannelFlagsK, Gdiplus::ColorAdjustTypeBitmap);

		Gdiplus::Point ptDst(rc.left,rc.top);
		Gdiplus::RectF rc3(rc.left,rc.top,rc.Width(),rc.Height());
		Gdiplus::Status status= grp.DrawImage(bmp,
			rc3,
			rc.left,rc.top,rc.Width(),rc.Height(),
			Gdiplus::UnitPixel,&imAtt);
		//hr = grp.DrawImage(bmp,(int)rc.left,(int)rc.top,(int)rc.left,(int)rc.top,(int)rc.Width(),(int)rc.Height(),Gdiplus::UnitPixel);
		if(status != Gdiplus:: Ok)
		{
			TRACE("DrawImage Error : %u\n",status);
		}
		else
		{
			//TRACE("DrawImage time %u\n",timeGetTime() - dwTime1);
		}
	}
	else
	{
		CRect rcClip;
		pDC->GetClipBox(&rcClip);
		Gdiplus::Graphics grp(pDC->GetSafeHdc());
		Gdiplus::SolidBrush blackBrush1( Gdiplus::Color(255, 255, 255, 255) );
		grp.FillRectangle(&blackBrush1,rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height());
	}
	//*/
}

void CViewGLView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// CViewGLView 打印

BOOL CViewGLView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CViewGLView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印前添加额外的初始化
}

void CViewGLView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印后添加清除过程
}


// CViewGLView 诊断

#ifdef _DEBUG
void CViewGLView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CViewGLView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CViewGLDoc* CViewGLView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CViewGLDoc)));
	return (CViewGLDoc*)m_pDocument;
}
#endif //_DEBUG


// CViewGLView 消息处理程序

void CViewGLView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 在此添加专用代码和/或调用基类
	/*CGrpBlock * blk;
	if(blk = GetDocument()->m_grp.GetCurBlock())
	{
		this->SetScrollSizes(MM_TEXT,CSize(blk->GetWidth(),blk->GetHeight()));
	}*/
	CViewGLDoc* pDoc = GetDocument();
	if(pDoc->m_pBitmap)
	{
		this->SetScrollSizes(MM_TEXT,CSize(pDoc->m_pBitmap->GetWidth(),pDoc->m_pBitmap->GetHeight()));
	}
	ResetBitmap();
}

void CViewGLView::ResetBitmap()
{
	//this->Invalidate();
	this->RedrawWindow();
}

//void CViewGLView::OnSetBkColor()
//{
//	// TODO: 在此添加命令处理程序代码
//	
//}

BOOL CViewGLView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CScrollView::OnEraseBkgnd(pDC);
}


//void CViewGLView::OnShowClip()
//{
//	// TODO: 在此添加命令处理程序代码
//	m_bShowClip = !m_bShowClip;
//}

//void CViewGLView::OnUpdateShowClip(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_bShowClip);
//
//}

CPoint CViewGLView::GetCursorInBitmap(void)
{

	CPoint ptCursor;
	::GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);	

        CPoint ptOffset = -GetScrollPosition();
	ptCursor.Offset(ptOffset);

	return ptCursor;
}

CPoint CViewGLView::GetCursorInClip(void)
{

	CViewGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(pDoc)
	{
		CPoint pt = GetCursorInBitmap();
		if(pDoc->m_pBitmap)
		{
			RECT rcClip = pDoc->m_pBitmap->GetClipRect();
			CPoint pt2;
			pt2.x = pt.x - rcClip.left;
			pt2.y = pt.y - rcClip.top;

			CPoint ptOffset = -GetScrollPosition();
			pt2.Offset(ptOffset);
			return pt2;
		}
	}
	return CPoint(0,0);	
}


bool CViewGLView::ShowPreBitmap()
{
	CViewGLDoc* pDoc = GetDocument();
	if(pDoc && pDoc->m_pBitmapList)
	{		
		stBitmap* p = pDoc->GetPreBitmap();
		if(p)
		{
			pDoc->m_pBitmap = pDoc->m_pBitmapList->GetBitmap(p->group,p->frame,p->frame0);
			Invalidate(false);
			return true;
		}		
	}
	return false;
}

bool CViewGLView::ShowNextBitmap()
{
	CViewGLDoc* pDoc = GetDocument();
	if(pDoc && pDoc->m_pBitmapList)
	{		
		stBitmap* p = pDoc->GetNextBitmap();
		if(p)
		{
			pDoc->m_pBitmap = pDoc->m_pBitmapList->GetBitmap(p->group,p->frame,p->frame0);
			Invalidate(false);
			return true;
		}		
	}
	return false;
}

void CViewGLView::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CViewGLDoc* pDoc = GetDocument();
	if(pDoc && pDoc->m_bAutoShow)
	{		
		ShowNextBitmap();
	}

	CScrollView::OnTimer(nIDEvent);
}

void CViewGLView::BeginAutoShow(int frequency)
{
	m_iSpeed = frequency;
	m_Timer = SetTimer(1,frequency,NULL);
}

void CViewGLView::EndAutoShow()
{
	if( m_Timer!= 0 ) 
	{
		KillTimer(m_Timer);
		m_Timer = 0;
	}
}
BOOL CViewGLView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	WNDCLASSEX wc;
	char szClass[MAX_PATH];
	GetClassName(m_hWnd,szClass,MAX_PATH);
	wc.cbSize = sizeof(WNDCLASSEX);
	GetClassInfoEx(AfxGetInstanceHandle(),szClass,&wc);
	DWORD dwStyle,dwStyleEx;
	dwStyle = GetWindowLong(m_hWnd,GWL_STYLE);
	dwStyleEx = GetWindowLong(m_hWnd,GWL_EXSTYLE);
	ShowCursor(TRUE);
	SetCursor(NULL);
	GetGraphicCursor()->showCursor(TRUE);
	//ShowCursor(FALSE);
	//OnDraw(NULL);
	return TRUE;
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}
