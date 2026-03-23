// FileTreeView.cpp : 实现文件
//

#include "stdafx.h"
#include "ViewGL.h"
#include "FileTreeView.h"
#include ".\filetreeview.h"
#include "ViewGLDoc.h"
#include "DlgSaveToTga.h"
#include "SaveHelper.h"
#pragma comment( lib, "ijl15l.lib" )

// CFileTreeView

IMPLEMENT_DYNCREATE(CFileTreeView, CTreeView)

CFileTreeView::CFileTreeView()
{
	m_pSaveHelper = new CSaveHelper();
}

CFileTreeView::~CFileTreeView()
{
	if( m_pSaveHelper )
	{
		delete m_pSaveHelper;
		m_pSaveHelper = NULL;
	}
}

BEGIN_MESSAGE_MAP(CFileTreeView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_RCLICK,OnRightClick)
	ON_COMMAND(ID_EXPORT_TO_TGA, OnExportToTga)
END_MESSAGE_MAP()


// CFileTreeView 诊断

#ifdef _DEBUG
void CFileTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CFileTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG


// CFileTreeView 消息处理程序


void CFileTreeView::OnUpdate(CView* pSender, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pSender == this) return;
	CTreeCtrl& treeCtrl = GetTreeCtrl();
	treeCtrl.DeleteAllItems();
	if(GetDocument()->GetPathName().IsEmpty()) return;

	//CGrpLib& grp = ((CViewGLDoc*)GetDocument())->m_grp;
	IBitmapList* pBitmapList = ((CViewGLDoc*)GetDocument())->m_pBitmapList;
	if(!pBitmapList) return ;
	CString strItem;
	
	HTREEITEM rootItem = treeCtrl.InsertItem(GetDocument()->GetPathName(),1,0);
	treeCtrl.SetItemData(rootItem,4);
	HTREEITEM parent;

	for(size_t i=0;i<pBitmapList->GetGroupCount();++i)
	{
		DWORD dwGroupID = pBitmapList->GetGroupID(i);
		strItem.Format(_T("%04u"),dwGroupID);
		parent = treeCtrl.InsertItem(strItem,1,0,rootItem);
		treeCtrl.SetItemData(parent,5);
		HTREEITEM item;
		for(size_t j=0;j<pBitmapList->GetFrameCount(dwGroupID);++j)
		{
			strItem.Format(_T("%04u"),j);
			IBitmaps* pBitmaps = pBitmapList->GetBitmaps(dwGroupID,j);
			//stAnimation & ani = grp.GetAnimation(dwGroupID,j);
			if(pBitmaps)
			{
				if(pBitmaps->GetFrameCount() == 1) 
				{
					item = treeCtrl.InsertItem(strItem,2,2,parent);
					treeCtrl.SetItemData(item,2);
				}
				else
				{
					HTREEITEM hp = treeCtrl.InsertItem(strItem,1,1,parent);
					treeCtrl.SetItemData(hp,2);
					for(size_t k=0;k<pBitmaps->GetFrameCount();++k)
					{
						strItem.Format(_T("%04u"),k);
						item = treeCtrl.InsertItem(strItem,2,2,hp);
						treeCtrl.SetItemData(item,3);
					}
				}
			}
		}
	}
	treeCtrl.Expand(rootItem,TVE_EXPAND);
}

void CFileTreeView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//选择图片
	*pResult = 0;

	HTREEITEM item = GetTreeCtrl().GetSelectedItem();
	if(item)
	{
		if(GetTreeCtrl().GetChildItem(item)) return;
		DWORD data = GetTreeCtrl().GetItemData(item);
		if(data != 2 && data != 3) return;

		CString strGroup,strFrame,strFrame0;
		HTREEITEM parent = GetTreeCtrl().GetParentItem(item);
		if(!parent) return;
		HTREEITEM parent0 = GetTreeCtrl().GetParentItem(parent);
		
		if(data == 2)
		{	
			strGroup = GetTreeCtrl().GetItemText(parent);
			strFrame = GetTreeCtrl().GetItemText(item);
			strFrame0 = "0";
		}
		else
		{
			strGroup = GetTreeCtrl().GetItemText(parent0);
			strFrame = GetTreeCtrl().GetItemText(parent);
			strFrame0 = GetTreeCtrl().GetItemText(item);
		}
		
		CViewGLDoc* pDoc = (CViewGLDoc*)GetDocument();
		pDoc->SetCurBitmap(atol(strGroup),atol(strFrame),atol(strFrame0));
		pDoc->m_pBitmap = pDoc->m_pBitmapList->GetBitmap(atol(strGroup),atol(strFrame),atol(strFrame0));
		//((CViewGLDoc*)GetDocument())->m_grp.SetCurBlock(atol(strGroup),atol(strFrame),atol(strFrame0));
		pDoc->UpdateAllViews(this);
	}
}

void CFileTreeView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
}

BOOL CFileTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	cs.style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_INFOTIP;
	return CTreeView::PreCreateWindow(cs);
}

int CFileTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_imgList.Create(16,15,ILC_COLOR8|ILC_MASK,0,1);

	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_OPEN_FOLDER));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_CLOSE_FOLDER));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_IMAGE));

	GetTreeCtrl().SetImageList(&m_imgList,LVSIL_NORMAL);

	return 0;
}

void CFileTreeView::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 在此处添加消息处理程序代码
	
}

void CFileTreeView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	static int i = 0;
	TRACE("test:%d\n",i++);
	CTreeView::OnRButtonDown(nFlags, point);
}


void CFileTreeView::DoPopupMenu(UINT nMenuID)
{
	CMenu popMenu;

	popMenu.LoadMenu(nMenuID);

	CPoint posMouse;
	GetCursorPos(&posMouse);

	popMenu.GetSubMenu(0)->TrackPopupMenu(0,posMouse.x,posMouse.y,this);
}

void CFileTreeView::OnRightClick(NMHDR* /*pNotifyStruct*/,LRESULT *result)
{
	CTreeCtrl& ctlTree =  GetTreeCtrl();

	UINT nFlags;
	CPoint curPoint;
	GetCursorPos(&curPoint);
	ScreenToClient(&curPoint);

	// Select the item that is at the point myPoint.
	HTREEITEM hItem = ctlTree.HitTest(curPoint, &nFlags);
	if ((hItem != NULL) && (TVHT_ONITEM & nFlags))
	{
		ctlTree.Select(hItem, TVGN_CARET);
		DoPopupMenu(IDR_MENU1);
	}

	*result = 0;
}


void CFileTreeView::OnExportToTga()
{
	// TODO: 在此添加命令处理程序代码
	IBitmapList* pBitmapList = ((CViewGLDoc*)GetDocument())->m_pBitmapList;
	if(!pBitmapList)return;

	static CDlgSaveToTga s_dlg(AfxGetMainWnd());
	if(s_dlg.DoModal() ==  IDOK)
	{
		if( s_dlg.m_strDstPath.IsEmpty() )
		{
			AfxMessageBox("请选择一个路径!");
			return;
		}
		HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
		if( !hItem ) return;

		SAVE_INFO     saveInfo;
		saveInfo.pBitmapList=pBitmapList;
		DWORD data = GetTreeCtrl().GetItemData(hItem);

		if( data == 4)
		{
			// 根部，保存整个文件包
			saveInfo.itemdata = ITEM_ROOT;

		} 
		else if( data == 5 )  
		{	// 保存某个组
			saveInfo.itemdata = ITEM_GROUP;
			CString strGroup = GetTreeCtrl().GetItemText(hItem);
			saveInfo.group = atol(strGroup);                                
		}
		else if(data == 2) 
		{
			// 保存某动画(可能有一张或者多张图片)
			saveInfo.itemdata = ITEM_FRAME;
			CString strFrame = GetTreeCtrl().GetItemText(hItem);
			saveInfo.frame = atol(strFrame);
			HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
			if( !hParent) return;
			CString strGroup = GetTreeCtrl().GetItemText(hParent);
			saveInfo.group = atol(strGroup);
		}
		else if( data == 3)
		{
			// 保存某动画(多副图片)中的某一张
			saveInfo.itemdata = ITEM_FRAME_FRAME;
			CString strFrameFrame = GetTreeCtrl().GetItemText(hItem);
			saveInfo.frameframe = atol(strFrameFrame);
			HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
			if(!hParent) return;
			CString strFrame = GetTreeCtrl().GetItemText(hParent);
			saveInfo.frame = atol(strFrame);
			HTREEITEM hPParent = GetTreeCtrl().GetParentItem(hParent);
			if(!hPParent) return;
			CString strGroup = GetTreeCtrl().GetItemText(hPParent);
			saveInfo.group = atol(strGroup);                                
		}
		
		strncpy(m_pSaveHelper->m_strFileName,GetDocument()->GetPathName(),MAX_PATH);
		m_pSaveHelper->m_strFileName[MAX_PATH-1] = 0;

        if( m_pSaveHelper->SaveToTga(s_dlg.m_strDstPath,saveInfo,s_dlg.m_bObject) )
		{
			AfxMessageBox(_T("保存成功"));
		}
		else
		{
			AfxMessageBox(_T("保存失败"));
		}
	}
}
