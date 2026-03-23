#ifndef __CJCOLORPOPUP_H__
#define __CJCOLORPOPUP_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CJColorPopup.h : header file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.
// It's free - so you get what you pay for.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:53p $
 * $Revision: 7 $
 * $Archive: /CodeJock/Include/CJColorPopup.h $
 *
 * $History: CJColorPopup.h $
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:53p
 * Updated in $/CodeJock/Include
 * Overrode OnEraseBkgnd(...) and made modifications to OnPaint(...) to
 * handle flicker free drawing.
 * 
 * Modified resource include for static builds.
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:23p
 * Updated in $/CodeJock/Include
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

// CCJColorPopup messages
#define CPN_SELCHANGE        WM_USER + 1001        // Color Picker Selection change
#define CPN_DROPDOWN         WM_USER + 1002        // Color Picker drop down
#define CPN_CLOSEUP          WM_USER + 1003        // Color Picker close up
#define CPN_SELENDOK         WM_USER + 1004        // Color Picker end OK
#define CPN_SELENDCANCEL     WM_USER + 1005        // Color Picker end (cancelled)

// forward declaration
class CColorPicker;

// To hold the Colors and their names
typedef struct {
    COLORREF crColor;
    TCHAR    *szName;
} ColorTableEntry;

/////////////////////////////////////////////////////////////////////////////
// CCJColorPopup window

class  CCJColorPopup : public CWnd
{
// Construction
public:
    CCJColorPopup();
    CCJColorPopup(CPoint p, COLORREF crColor, CWnd* pParentWnd,
                 LPCTSTR szDefaultText = NULL, LPCTSTR szCustomText = NULL);
    virtual void Initialise();

// Attributes
public:

// Operations
public:
    virtual BOOL Create(CPoint p, COLORREF crColor, CWnd* pParentWnd, 
                LPCTSTR szDefaultText = NULL, LPCTSTR szCustomText = NULL);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCJColorPopup)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

// Implementation
public:
	virtual void UpdateFont();
    virtual ~CCJColorPopup();

protected:
    virtual BOOL GetCellRect(int nIndex, const LPRECT& rect);
    virtual void FindCellFromColor(COLORREF crColor);
    virtual void SetWindowSize();
    virtual void CreateToolTips();
    virtual void ChangeSelection(int nIndex);
    virtual void EndSelection(int nMessage);
    virtual void DrawCell(CDC* pDC, int nIndex);

    virtual COLORREF GetColor(int nIndex)              { return m_crColors[nIndex].crColor; }
    virtual LPCTSTR GetColorName(int nIndex)           { return m_crColors[nIndex].szName; }
    virtual int  GetIndex(int row, int col) const;
    virtual int  GetRow(int nIndex) const;
    virtual int  GetColumn(int nIndex) const;

// protected attributes
protected:
    static ColorTableEntry m_crColors[];
    int            m_nNumColors;
    int            m_nNumColumns, m_nNumRows;
    int            m_nBoxSize, m_nMargin;
    int            m_nCurrentSel;
    int            m_nChosenColorSel;
    CString        m_strDefaultText;
    CString        m_strCustomText;
    CRect          m_CustomTextRect, m_DefaultTextRect, m_WindowRect;
    CFont          m_Font;
    CPalette       m_Palette;
    COLORREF       m_crInitialColor, m_crColor;
    CToolTipCtrl   m_ToolTip;
    CWnd*          m_pParent;

    // Generated message map functions
protected:
    //{{AFX_MSG(CCJColorPopup)
    afx_msg void OnNcDestroy();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnQueryNewPalette();
    afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(__CJCOLORPOPUP_H__)
