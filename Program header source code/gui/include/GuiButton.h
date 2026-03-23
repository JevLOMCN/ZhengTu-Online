/**
 * \file      GuiButton.h
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI按钮
 * 
 *	      GUI按钮
 */


#pragma once
#include "GuiStatic.h"
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiButton : public CGuiStatic
{
protected:
	bool m_bChecked;
	UINT m_nButtonGroup;
	stPointF	m_bitmapScale;
	bool m_bFlashState;
	float m_fFlashRate;
public:
	
	virtual void SetCheckedInternal( bool bChecked, bool bFromInput );
	bool GetChecked() { return m_bChecked; }
	void SetChecked( bool bChecked ) { SetCheckedInternal( bChecked, false ); }

	void SetButtonGroup( int nButtonGroup ) { m_nButtonGroup = nButtonGroup; }
	int GetButtonGroup() { return m_nButtonGroup; }

	enum enumButtonType{
		ButtonType_Normal,		// 普通按钮
		ButtonType_CloseBox,	// 关闭按钮
		ButtonType_MinBox,		// 最小化按钮
		ButtonType_MaxBox,		// 最大化按钮
		ButtonType_Bitmap,		// 位图按钮
		ButtonType_Color,		// 颜色选择按钮
		ButtonType_Bitmap2,             // 特殊位图按钮，检测不通过图片，而用boundingbox
	};
	
	enum STYLE{
		GUI_BS_RESULTOK = 0x1,
		GUI_BS_RESULTCANCLE = 0x2,
	};

	static SIZE CalcuSize(const char* pszCaption);

    CGuiButton( CGuiDialog *pDialog = NULL );

    //virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    //virtual void OnHotkey() { m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this ); }
    
    //virtual bool ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt ); }
    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }

    virtual void Render( float fElapsedTime );
	virtual bool ContainsPoint( POINT pt );
	void	SetButtonType(enumButtonType type)
	{
		m_eButtonType = type;
	}

	void PlayFlash(float fRate = 0.7f);
	void StopFlash() { m_bFlashState = false; }
 
	enumButtonType GetButtonType(){ return m_eButtonType;}

	void SetBitmap(stResourceLocation& rl,int frameNum,DWORD* dwState = NULL);
	void SetBitmapScale( const stPointF& scale );

	bool GetBitmapInfo(stResourceLocation& rl,int & frameNum);
protected:
	enumButtonType m_eButtonType;
	void UpdateRects();

	IBitmaps*	m_colorButtonBmp;
	stRectI		m_bmpRect[9];

	DWORD m_BackColor;					//background color
public:
	//-------------------------------------------
	// 注册UI脚本函数
	//-------------------------------------------
	static void bind_lua(lua_State* L);
	
};
