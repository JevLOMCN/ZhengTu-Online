#pragma once


// CFileTreeView 视图

class CSaveHelper;
class CFileTreeView : public CTreeView
{
	DECLARE_DYNCREATE(CFileTreeView)

protected:
	CFileTreeView();           // 动态创建所使用的受保护的构造函数
	virtual ~CFileTreeView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	CImageList m_imgList;
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnInitialUpdate();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	void OnRightClick(NMHDR *pNotifyStruct,LRESULT *result);
private:
	void DoPopupMenu(UINT nMenuID);
public:
	afx_msg void OnExportToTga();
	CSaveHelper   *   m_pSaveHelper;
};


