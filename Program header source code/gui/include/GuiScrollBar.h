/**
* \file      GuiScrollBar.h
* \version   $Id$
* \author    $$$03公司@###.com
*           $$$03公司@###.com
* \date      2006-7-19  21:11:00
* \brief     GUI滚动条
* 
*	      GUI滚动条
*/

#pragma once
#include "guicontrol.h"

/**
* \brief 简短描述
* 
* 详细描述
* 
*/
class CGuiScrollBar :
	public CGuiControl
{
public:
	CGuiScrollBar( CGuiDialog *pDialog = NULL );
	virtual ~CGuiScrollBar();

	virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

	virtual void Render( float fElapsedTime );
	virtual void    UpdateRects();
	void SetCanHaveFocus(bool b){ m_bCanHasFocus = b;}
	void SetTrackRange( int nStart, int nEnd );
	int GetTrackPos() { return m_nPosition; }
	void SetTrackPos( int nPosition ) { m_nPosition = nPosition; Cap(); UpdateThumbRect(); }
	int GetPageSize() { return m_nPageSize; }
	void SetPageSize( int nPageSize ) { m_nPageSize = nPageSize; Cap(); UpdateThumbRect(); }
	void SetLineSize(int nLineSize) { m_nLineSize = nLineSize; }
	int GetLineSize() { return m_nLineSize;}

	void UpALine();
	void DownALine();

	virtual void SetValue( int nValue ) { SetTrackPos( nValue); }
	virtual int  GetValue() { return GetTrackPos(); };

	virtual void SetRange( int nMin, int nMax ){ SetTrackRange(nMin,nMax); }
	virtual int GetRangeMin(){ return GetTrackStart();}
	virtual int GetRangeMax(){ return GetTrackEnd();}

	void Scroll( int nDelta );    // Scroll by nDelta items (plus or minus)
	void ShowItem( int nIndex );  // Ensure that item nIndex is displayed, scroll if necessary

	int GetTrackStart(){ return m_nStart;}
	int GetTrackEnd(){ return m_nEnd; }

	bool IsKeepScroll();

	enum STYLE{
		HORIZONTAL = 0x1,
		NOSKIN = 0x8,
		HORIZONTAL2 = 0x10, // 为了不影响一起的混乱(以前这个style在scrollbar中没用,所以专门再用一个HORIZONTAL2)^.^
	};
protected:
	// ARROWSTATE indicates the state of the arrow buttons.
	// CLEAR            No arrow is down.
	// CLICKED_UP       Up arrow is clicked.
	// CLICKED_DOWN     Down arrow is clicked.
	// HELD_UP          Up arrow is held down for sustained period.
	// HELD_DOWN        Down arrow is held down for sustained period.
	enum ARROWSTATE { CLEAR, CLICKED_UP, CLICKED_DOWN, HELD_UP, HELD_DOWN };

	void UpdateThumbRect();
	void Cap();  // Clips position at boundaries. Ensures it stays within legal range.

	bool m_bShowThumb;
	bool m_bCanHasFocus;
	bool CanHaveFocus();
	DWORD m_dwStyle;
	stRectI m_rcUpButton;
	stRectI m_rcDownButton;
	stRectI m_rcTrack;
	stRectI m_rcThumb;
	int m_nPosition;  // Position of the first displayed item
	int m_nPageSize;  // How many items are displayable in one page
	int m_nLineSize;
	int m_nStart;     // First item
	int m_nEnd;       // The index after the last item
	POINT m_LastMouse;// Last mouse position
	ARROWSTATE m_Arrow; // State of the arrows
	double m_dArrowTS;  // Timestamp of last arrow event.


	//int m_ThumbOffsetY;
	//int m_ThumbOffsetX;

	//bool s_bDrag;
	//CGuiScrollBar * s_dragScr;

};
