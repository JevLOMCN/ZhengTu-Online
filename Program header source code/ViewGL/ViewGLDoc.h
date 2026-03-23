// ViewGLDoc.h :  CViewGLDoc 类的接口
//


#pragma once

//#include "GrpLib.h"

#include "../RenderD3D/include/RenderD3D.h"
#include "../RenderD3D/include/D3DDevice.h"
#include "../RenderD3D/include/CustomTexture.h"

class CViewGLView;
class CFileTreeView;

#include <vector>
class stBitmap
{
public:
	int group;
	int frame;
	int frame0;

	stBitmap()
	{
		group = frame = frame0 = 0;
	}
	stBitmap(int a,int b,int c)
	{
		group = a;
		frame = b;
		frame0 =c;
	}
	stBitmap(const stBitmap& b)
	{
		*this = b;
	}
	stBitmap& operator = (const stBitmap& bmp)
	{
		group = bmp.group;
		frame = bmp.frame;
		frame0 = bmp.frame0;
		return *this;
	}

	bool operator == (const stBitmap bmp)
	{
		return ((group == bmp.group) && (frame == bmp.frame) && (frame0 == bmp.frame0));
	}
};

typedef std::vector<stBitmap> BitmapVec;
typedef BitmapVec::iterator   BitmapIt;

class CViewGLDoc : public CDocument
{
protected: // 仅从序列化创建
	CViewGLDoc();
	DECLARE_DYNCREATE(CViewGLDoc)

// 属性
public:
	static IBitmapList* m_pBitmapList;
	IBitmap*	 m_pBitmap;
	static CCustomTexture * m_pBackImage;
	//CGrpLib m_grp;
	BOOL	m_bShowColor;
	BOOL	m_bShowAlpha;

	DWORD m_bkColor;

	BOOL  m_bShowClip;

	BOOL m_bAutoAlarm;

	BOOL m_bAutoShow;


// 操作
public:

// 重写
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
private:
	BitmapVec m_bmpList;
	stBitmap m_CurBitmap;
	BOOL	m_bIsSetCurBitmap;

public:
	//stBitmap GetCurrentBitmap() {return m_CurBitmap;};
	stBitmap* GetNextBitmap();
	stBitmap* GetPreBitmap();
	void SetCurBitmap(int group,int frame,int frame0 = 0)
	{
		m_CurBitmap = stBitmap(group,frame,frame0);
	}
// 实现
public:
	virtual ~CViewGLDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CViewGLView * GetClientView();
	CFileTreeView* GetLeftView();
protected:
	HTREEITEM	GetTreeViewItem(HTREEITEM parent,int child);

	void SetTreeViewSelectItem(const stBitmap bitmap);
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
//	afx_msg void OnSetBkColor();
	afx_msg void OnSetBkColor();
	afx_msg void OnShowColor();
	afx_msg void OnUpdateShowColor(CCmdUI *pCmdUI);
	afx_msg void OnShowAlpha();
	afx_msg void OnUpdateShowAlpha(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCommandUICRC(CCmdUI * pUI);
	afx_msg void OnUpdateCommandUIPosition(CCmdUI * pUI);
	afx_msg void OnUpdateCommandUIBitmap(CCmdUI * pUI);
	afx_msg void OnGrpSaveAsTga();
	afx_msg void OnTestSpeed();
	afx_msg void OnSetBackimage();
	afx_msg void OnShowClip();
	afx_msg void OnUpdateShowClip(CCmdUI *pCmdUI);
	afx_msg void OnAutoAlarm();
	afx_msg void OnUpdateAutoAlarm(CCmdUI *pCmdUI);
	afx_msg void OnAutoShow();
	afx_msg void OnUpdateAutoShow(CCmdUI *pCmdUI);
};


