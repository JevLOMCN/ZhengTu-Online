/**
 * \file      GuiSlider.h
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI刻度条
 * 
 *	      GUI刻度条
 */

#pragma once
#include "guicontrol.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiSlider :
	public CGuiControl
{
public:
    CGuiSlider( CGuiDialog *pDialog = NULL );

    virtual bool ContainsPoint( POINT pt ); 
    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    
    virtual void UpdateRects(); 

    virtual void Render( float fElapsedTime );

    void SetValue( int nValue ) { SetValueInternal( nValue, false ); }
    int  GetValue() { return m_nValue; };

    void SetRange( int nMin, int nMax );
	int GetRangeMin(){ return m_nMin;}
	int GetRangeMax(){ return m_nMax;}
protected:
    void SetValueInternal( int nValue, bool bFromInput );
    int  ValueFromPos( int x ); 

    int m_nValue;

    int m_nMin;
    int m_nMax;

    int m_nDragX;      // Mouse position at start of drag
    int m_nDragOffset; // Drag offset from the center of the button
    int m_nButtonX;

    bool m_bPressed;
    stRectI m_rcButton;
};
