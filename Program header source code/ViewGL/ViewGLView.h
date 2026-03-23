// ViewGLView.h : CViewGLView 类的接口
//


#pragma once


class CViewGLView : public CScrollView
{
protected: // 仅从序列化创建
	CViewGLView();
	DECLARE_DYNCREATE(CViewGLView)

// 属性
public:
	CViewGLDoc* GetDocument() const;
	//HBITMAP	m_hBmp;
	CPoint GetCursorInBitmap(void);
	CPoint GetCursorInClip(void);
	int m_Timer;
	int			m_iSpeed;
// 操作
public:

// 重写
	public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	void Draw();
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CViewGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void ResetBitmap();
	void BeginAutoShow(int frequency = 1000);
	void EndAutoShow();

	bool ShowPreBitmap();
	bool ShowNextBitmap();
protected:
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
//	afx_msg void OnSetBkColor();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg void OnShowClip();
//	afx_msg void OnUpdateShowClip(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
//	afx_msg void OnAutoShow();
//	afx_msg void OnUpdateAutoShow(CCmdUI *pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

#ifndef _DEBUG  // ViewGLView.cpp 的调试版本
inline CViewGLDoc* CViewGLView::GetDocument() const
   { return reinterpret_cast<CViewGLDoc*>(m_pDocument); }
#endif

