/**
 * \file      GuiRadioButton.h
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI单选框
 * 
 *	      GUI单选框
 */

#pragma once
#include "guicheckbox.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiRadioButton :
	public CGuiCheckBox
{
public:
    CGuiRadioButton( CGuiDialog *pDialog = NULL );

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey() { SetCheckedInternal( true, true, true ); }
    
    void SetChecked( bool bChecked, bool bClearGroup) { SetCheckedInternal( bChecked, bClearGroup, false ); }
    
protected:
	virtual void SetCheckedInternal( bool bChecked, bool bFromInput ){ SetCheckedInternal(bChecked,true,bFromInput);}
    virtual void SetCheckedInternal( bool bChecked, bool bClearGroup, bool bFromInput );
};
