/**
 * \file      GuiStatic.h
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI¾²Ì¬¿Ø¼þ
 * 
 *	      GUI¾²Ì¬¿Ø¼þ
 */

#pragma once

#include "GuiControl.h"
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiStatic : public CGuiControl
{
public:
    CGuiStatic( CGuiDialog *pDialog = NULL );

    virtual void Render( float fElapsedTime );
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey();

    HRESULT GetTextCopy( char* strDest, UINT bufferCount );
    const char* GetText() { return m_strText; }
    void SetText( const char* strText );
	void SetBackGroundColor(DWORD color);
	DWORD GetBackGroundColor() { return m_color; }
	
	POINT GetDragOffset();

	virtual void    SetSoundIndex(int soundindex)
	{
		m_nSoundIndex = soundindex;
	}
protected:
    char m_strText[MAX_PATH*2];      // Window text  
	DWORD m_color;					//background color
	stPointI m_ptDragStart;
	stPointI m_ptDragEnd;
	bool m_bDrawBackGround;
	bool m_bPressed;

	int m_nSoundIndex;
};
