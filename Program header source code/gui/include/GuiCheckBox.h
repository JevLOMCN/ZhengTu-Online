/**
 * \file      GuiCheckBox.h
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI¼ì²é¿ò
 * 
 *	      GUI¼ì²é¿ò
 */

#pragma once
#include "guibutton.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiCheckBox :
	public CGuiButton
{
public:
	enum STYLE
	{
		GUI_ES_NOSKIN	= 0x8,
	};

    CGuiCheckBox( CGuiDialog *pDialog = NULL );

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey() { SetCheckedInternal( !m_bChecked, true ); }

    virtual bool ContainsPoint( POINT pt ); 
    virtual void UpdateRects(); 

    virtual void Render(float fElapsedTime );
    
protected:

    stRectI m_rcButton;
    stRectI m_rcText;
};
