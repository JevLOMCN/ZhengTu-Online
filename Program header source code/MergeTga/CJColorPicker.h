#ifndef __CJCOLORPICKER_H__
#define __CJCOLORPICKER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CJColorPicker.h : header file
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
 * $Date: 10/31/99 11:50p $
 * $Revision: 6 $
 * $Archive: /CodeJock/Include/CJColorPicker.h $
 *
 * $History: CJColorPicker.h $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:50p
 * Updated in $/CodeJock/Include
 * Overrode OnEraseBkgnd(...), OnPaint() and made modifications to
 * DrawItem(...) for flicker free drawing.
 * 
 * Modified resource include for static builds.
 * 
 * Added method CheckTextColor(...) which tests if the intensity of the
 * color is greater as 128. If the intensity < 128 => color is dark, so
 * the text must be light. Stephane Routelous
 * [routelous@cad-cam-concept.de]
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * Removed un-necessary calls to GetParent(), a call is made only once at
 * initialization, to ensure we are working with a valid handle.
 * 
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:22p
 * Updated in $/CodeJock/Include
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "CJColorPopup.h"

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker window

void AFXAPI DDX_CJColorPicker(CDataExchange *pDX, int nIDC, COLORREF& crColor);

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker window

#define CP_MODE_TEXT 1  // edit text Color
#define CP_MODE_BK   2  // edit background Color (default)

class  CCJColorPicker : public CButton
{
// Construction
public:
    CCJColorPicker();
    DECLARE_DYNCREATE(CCJColorPicker);

// Attributes
public:
    virtual COLORREF GetColor();
    virtual void     SetColor(COLORREF crColor); 

    virtual void     SetDefaultText(LPCTSTR szDefaultText);
    virtual void     SetCustomText(LPCTSTR szCustomText);

    virtual void     SetTrackSelection(BOOL bTracking = TRUE)  { m_bTrackSelection = bTracking; }
    virtual BOOL     GetTrackSelection()                       { return m_bTrackSelection; }

    virtual void     SetSelectionMode(UINT nMode)              { m_nSelectionMode = nMode; }
    virtual UINT     GetSelectionMode()                        { return m_nSelectionMode; };

    virtual void     SetBkColor(COLORREF crColorBk);
    virtual COLORREF GetBkColor()                             { return m_crColorBk; }
    
    virtual void     SetTextColor(COLORREF crColorText);
    virtual COLORREF GetTextColor()                           { return m_crColorText;}

	virtual void	 CheckTextColor(COLORREF crColor);

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCJColorPicker)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CCJColorPicker();

protected:
    void SetWindowSize();

// protected attributes
protected:
    BOOL     m_bActive,                // Is the dropdown active?
             m_bTrackSelection;        // track Color changes?
    COLORREF m_crColorBk;
    COLORREF m_crColorText;
    UINT     m_nSelectionMode;
    CRect    m_ArrowRect;
    CString  m_strDefaultText;
    CString  m_strCustomText;
	bool	 m_bLBtnDown;
	CWnd*	 m_pParentWnd;				// holds a pointer to the parent window.

    // Generated message map functions
protected:
    //{{AFX_MSG(CCJColorPicker)
    afx_msg BOOL OnClicked();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
    afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
    afx_msg LONG OnSelEndCancel(UINT lParam, LONG wParam);
    afx_msg LONG OnSelChange(UINT lParam, LONG wParam);

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(__CJCOLORPICKER_H__)
