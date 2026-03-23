// ViewGLDoc.cpp :  CViewGLDoc 类的实现
//

#include "stdafx.h"
#include "ViewGL.h"

#include "ViewGLDoc.h"
#include ".\viewgldoc.h"

#include "ViewGLView.h"
#include "FileTreeView.h"
#include "CJShell.h"
#include "DlgSaveToTga.h"
#include "SaveHelper.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CViewGLDoc

IMPLEMENT_DYNCREATE(CViewGLDoc, CDocument)

BEGIN_MESSAGE_MAP(CViewGLDoc, CDocument)
//	ON_COMMAND(ID_SET_BK_COLOR, OnSetBkColor)
ON_COMMAND(ID_SET_BK_COLOR, OnSetBkColor)
ON_COMMAND(ID_SHOW_COLOR, OnShowColor)
ON_UPDATE_COMMAND_UI(ID_SHOW_COLOR, OnUpdateShowColor)
ON_COMMAND(ID_SHOW_ALPHA, OnShowAlpha)
ON_UPDATE_COMMAND_UI(ID_SHOW_ALPHA, OnUpdateShowAlpha)
ON_UPDATE_COMMAND_UI(ID_INDICATOR_CRC,OnUpdateCommandUICRC)
ON_UPDATE_COMMAND_UI(ID_INDICATOR_POSITION,OnUpdateCommandUIPosition)
ON_UPDATE_COMMAND_UI(ID_INDICATOR_BITMAP,OnUpdateCommandUIBitmap)
ON_COMMAND(ID_GRP_SAVE_AS_TGA, OnGrpSaveAsTga)
ON_COMMAND(ID_TEST_SPEED, OnTestSpeed)
ON_COMMAND(ID_SET_BACKIMAGE, OnSetBackimage)
ON_COMMAND(ID_SHOW_CLIP, OnShowClip)
ON_UPDATE_COMMAND_UI(ID_SHOW_CLIP, OnUpdateShowClip)
ON_COMMAND(ID_AUTO_ALARM, OnAutoAlarm)
ON_UPDATE_COMMAND_UI(ID_AUTO_ALARM, OnUpdateAutoAlarm)
ON_COMMAND(ID_AUTO_SHOW, OnAutoShow)
ON_UPDATE_COMMAND_UI(ID_AUTO_SHOW, OnUpdateAutoShow)
END_MESSAGE_MAP()


// CViewGLDoc 构造/析构

IBitmapList* CViewGLDoc::m_pBitmapList = NULL;
CCustomTexture * CViewGLDoc::m_pBackImage = NULL;

CViewGLDoc::CViewGLDoc()
: m_bShowClip(FALSE)
{
	// TODO: 在此添加一次性构造代码
	m_bkColor  = RGB(117,128,138);
	m_bShowColor = TRUE;
	m_bShowAlpha = FALSE;
	m_pBitmap = NULL;

	m_bAutoAlarm = FALSE;
	m_bAutoShow = FALSE;
	m_CurBitmap = stBitmap(0,0,0);
}

CViewGLDoc::~CViewGLDoc()
{
}

BOOL CViewGLDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CViewGLDoc 序列化

void CViewGLDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CViewGLDoc 诊断

#ifdef _DEBUG
void CViewGLDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CViewGLDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CViewGLDoc 命令

BOOL CViewGLDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	//打开文件
	// TODO:  在此添加您专用的创建代码
	//m_grp.Open(lpszPathName);
	m_CurBitmap = stBitmap(0,0,0);
	m_bmpList.clear();
	m_bIsSetCurBitmap = false;
	if(!GetDevice()) return TRUE;
	if(m_pBitmapList)
	{
		GetDevice()->DeleteResource(m_pBitmapList->GetFileName());
		m_pBitmapList = NULL;
	}
	if(m_pBackImage == NULL)
	{
		m_pBackImage = GetD3DDevice()->CreateCustomTexture(1024,1024,D3DFMT_A4R4G4B4);
	}
	m_pBitmap = NULL;
	m_pBitmapList = GetDevice()->FindResource(lpszPathName,
		IBitmapList::flag_AlwaysOpen | IBitmapList::flag_LoadByGroup /*| IBitmapList::flag_MapView*/
		| IBitmapList::flag_LoadByThread | IBitmapList::flag_LoadHdr,Usage_Building);
	UpdateAllViews(NULL);

	if(m_pBitmapList)
	{
		for(size_t i=0;i<m_pBitmapList->GetGroupCount();++i)
		{
			DWORD dwGroupID = m_pBitmapList->GetGroupID(i);
			for(size_t j=0;j<m_pBitmapList->GetFrameCount(dwGroupID);++j)
			{
				IBitmaps* pBitmaps = m_pBitmapList->GetBitmaps(dwGroupID,j);

				if(pBitmaps)
				{
					if(pBitmaps->GetFrameCount() == 1) 
					{
						stBitmap bmp(dwGroupID,j,0);
						m_bmpList.push_back(bmp);
						if(!m_bIsSetCurBitmap)
						{
							m_CurBitmap = bmp;
							m_bIsSetCurBitmap = true;
						}
					}
					else
					{
						for(size_t k=0;k<pBitmaps->GetFrameCount();++k)
						{
							stBitmap bmp(dwGroupID,j,k);
							m_bmpList.push_back(bmp);
							if(!m_bIsSetCurBitmap)
							{
								m_CurBitmap = bmp;
								m_bIsSetCurBitmap = true;
							}
						}
					}
				}
			}
		}
	}

	return TRUE;
}

CViewGLView * CViewGLDoc::GetClientView()
{
	POSITION pos = this->GetFirstViewPosition();
	while(pos)
	{
		CView* pView = this->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CViewGLView)))
			return (CViewGLView*)pView;
	}
	return NULL;
}

CFileTreeView* CViewGLDoc::GetLeftView()
{
	POSITION pos = this->GetFirstViewPosition();
	while(pos)
	{
		CView* pView = this->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CFileTreeView)))
			return (CFileTreeView*)pView;
	}
	return NULL;
}
//void CViewGLDoc::OnSetBkColor()
//{
//	// TODO: 在此添加命令处理程序代码
//	CColorDialog dlg(m_bkColor);
//	if(dlg.DoModal() == IDOK)
//	{
//		m_bkColor = dlg.GetColor();
//	}
//}

void CViewGLDoc::OnSetBkColor()
{
	// TODO: 在此添加命令处理程序代码
	CColorDialog dlg(m_bkColor,0,AfxGetMainWnd());
	if(dlg.DoModal() == IDOK)
	{
		COLORREF color = dlg.GetColor();
		m_bkColor = COLOR_ARGB(255,GetRValue(color),GetGValue(color),GetBValue(color));
		GetClientView()->ResetBitmap();
	}
}

void CViewGLDoc::OnShowColor()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowColor = !m_bShowColor;
	GetClientView()->ResetBitmap();
}

void CViewGLDoc::OnUpdateShowColor(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bShowColor);
}

void CViewGLDoc::OnShowAlpha()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowAlpha = !m_bShowAlpha;
	GetClientView()->ResetBitmap();
}

void CViewGLDoc::OnUpdateShowAlpha(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bShowAlpha);
}

void CViewGLDoc::OnUpdateCommandUICRC(CCmdUI * pUI)
{
	/*
	CGrpBlock* p = m_grp.GetCurBlock();
	if(p)
	{
		TCHAR szCRC[16];
		_stprintf(szCRC,_T("CRC: %X"),p->GetCRC());
		pUI->SetText(szCRC);
	}
	else
	{
		pUI->SetText(_T("CRC: "));
	}
	//*/
}
//
void CViewGLDoc::OnUpdateCommandUIPosition(CCmdUI * pUI)
{
	CViewGLView* pView = GetClientView();
	CPoint pt = pView->GetCursorInBitmap();
	CPoint pt2 = pView->GetCursorInClip();
	
	char szText[100];
	sprintf(szText,"图片坐标:%d,%d  裁减区坐标:%d,%d",pt.x,pt.y,pt2.x,pt2.y);
	pUI->SetText(szText);
}

void CViewGLDoc::OnUpdateCommandUIBitmap(CCmdUI * pUI)
{
	char szText[100];
	sprintf(szText,"Bitmap:[%04u][%04u][%04u]",m_CurBitmap.group,m_CurBitmap.frame,m_CurBitmap.frame0);
	pUI->SetText(szText);
}


void CViewGLDoc::OnGrpSaveAsTga()
{
	// TODO: 在此添加命令处理程序代码
	if( !m_pBitmapList) return;
	static CDlgSaveToTga s_dlg(AfxGetMainWnd());
	if(s_dlg.DoModal() ==  IDOK)
	{
		CSaveHelper saver;
		SAVE_INFO info;
		info.itemdata = ITEM_ROOT;
		info.pBitmapList = m_pBitmapList;
		if(!saver.SaveToTga(s_dlg.m_strDstPath,info,s_dlg.m_bObject))
		{
			AfxMessageBox(_T("保存失败"));
		}
		else
		{
			AfxMessageBox(_T("保存成功"));
		}
	}
	
}

void CViewGLDoc::OnTestSpeed()
{
	// TODO: 在此添加命令处理程序代码
	/*
	DWORD dwTime = m_grp.TestTime();
	CString str;
	str.Format(_T("用时%ums"),dwTime);
	AfxMessageBox(str);
	//*/
}

void CViewGLDoc::OnSetBackimage()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog fdlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT ,_T("*.tga|*.tga||"),AfxGetMainWnd());
	if(fdlg.DoModal() == IDOK)
	{
		if(m_pBackImage)
		{
			m_pBackImage->LoadFromTgaFile(fdlg.GetPathName());
		}
	}
}


void CViewGLDoc::OnShowClip()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowClip =!m_bShowClip;
}

void CViewGLDoc::OnUpdateShowClip(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bShowClip);
}

void CViewGLDoc::OnAutoAlarm()
{
	// TODO: 在此添加命令处理程序代码
	m_bAutoAlarm = !m_bAutoAlarm;
}

void CViewGLDoc::OnUpdateAutoAlarm(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bAutoAlarm);
}

void CViewGLDoc::OnAutoShow()
{
	// TODO: 在此添加命令处理程序代码
	m_bAutoShow = !m_bAutoShow;
	CViewGLView *pView = GetClientView();
	if(pView)
	{
		if(m_bAutoShow)
		{
			pView->BeginAutoShow(pView->m_iSpeed);
		}
		else
		{
			pView->EndAutoShow();
		}
	}
}

void CViewGLDoc::OnUpdateAutoShow(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_bAutoShow);
}

HTREEITEM	CViewGLDoc::GetTreeViewItem(HTREEITEM parent,int child)
{
	CFileTreeView* pTreeView = GetLeftView();
	if(pTreeView && parent)
	{
		CString strChild;
		strChild.Format("%04u",child);
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = pTreeView->GetTreeCtrl().GetChildItem(parent);

		while (hChildItem != NULL)
		{
			if (pTreeView->GetTreeCtrl().GetItemText(hChildItem) == strChild)
			{
				//pTreeView->GetTreeCtrl().SelectSetFirstVisible(hChildItem);
				pTreeView->GetTreeCtrl().EnsureVisible(hChildItem);
				pTreeView->GetTreeCtrl().Select(hChildItem,TVGN_CARET);
				return hChildItem;
			}
			hChildItem = pTreeView->GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	return NULL;
}

void CViewGLDoc::SetTreeViewSelectItem(const stBitmap bitmap)
{
	CFileTreeView* pTreeView = GetLeftView();
	CString strGroup,strFrame,strFrame0;
	strGroup.Format("%04u",bitmap.group);
	strFrame.Format("%04u",bitmap.frame);
	strFrame0.Format("%04u",bitmap.frame0);

	if(pTreeView)
	{
		HTREEITEM hRoot = pTreeView->GetTreeCtrl().GetRootItem();
		HTREEITEM hGroup = GetTreeViewItem(hRoot,bitmap.group);
		HTREEITEM hFrame = GetTreeViewItem(hGroup,bitmap.frame);
		HTREEITEM hFrame0 = GetTreeViewItem(hFrame,bitmap.frame0);					
	}
}

stBitmap* CViewGLDoc::GetPreBitmap()
{
	for(BitmapIt it = m_bmpList.begin();it != m_bmpList.end();++it)
	{
		stBitmap& bmp = *it;
		if(bmp == m_CurBitmap)
		{
			if(it != m_bmpList.begin())
			{
				BitmapIt it1 = it - 1;
				m_CurBitmap = stBitmap(it1->group,it1->frame,it1->frame0);
				SetTreeViewSelectItem(m_CurBitmap);
				return &m_CurBitmap;
			}/*
			else
			{
				it = m_bmpList.end() - 1;
				if(it != m_bmpList.end())
				{

				}
			}*/
			//else
			//{
			//	it = m_bmpList.begin();
			//	m_CurBitmap = stBitmap(it->group,it->frame,it->frame0);
			//	return &m_CurBitmap;
			//}
		}
	}
	return NULL;
}

stBitmap* CViewGLDoc::GetNextBitmap()
{
	for(BitmapIt it = m_bmpList.begin();it != m_bmpList.end();++it)
	{
		stBitmap& bmp = *it;
		if(bmp == m_CurBitmap)
		{
			BitmapIt it1 = it + 1;
			if(it1 != m_bmpList.end())
			{
				m_CurBitmap = stBitmap(it1->group,it1->frame,it1->frame0);
				SetTreeViewSelectItem(m_CurBitmap);
				return &m_CurBitmap;
			}
			else
			{
				it = m_bmpList.begin();
				m_CurBitmap = stBitmap(it->group,it->frame,it->frame0);
				SetTreeViewSelectItem(m_CurBitmap);
				return &m_CurBitmap;
			}
		}
	}
	return NULL;
}