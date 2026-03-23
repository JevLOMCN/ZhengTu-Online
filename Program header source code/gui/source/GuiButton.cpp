/**
 * \file      GuiButton.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI°´Å¥
 * 
 *	      GUI°´Å¥
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../../luabind/public.h"

void CGuiButton:: bind_lua(lua_State* L)
{
	using namespace luabind;
	module(L)
	[
		class_<CGuiButton,CGuiControl>("CGuiButton")
			.def("GetChecked",&CGuiButton::GetChecked)
			.def("SetChecked",&CGuiButton::SetChecked)
			.def("SetButtonGroup",&CGuiButton::SetButtonGroup)
			.def("GetButtonGroup",&CGuiButton::GetButtonGroup)
	];
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
// CGuiButton class
//--------------------------------------------------------------------------------------
CGuiButton::CGuiButton( CGuiDialog *pDialog )
: m_eButtonType(ButtonType_Normal)
, m_bChecked(false)
, m_nButtonGroup(0)
, m_fFlashRate(0.7f)
, m_bFlashState(false)
{
    m_Type = GUI_CONTROL_BUTTON;
    m_pDialog = pDialog;

    m_bPressed = false;

	m_bitmapScale.x = 1.0f;
	m_bitmapScale.y = 1.0f;

	stResourceLocation rl;
	rl.SetFileName( "data\\interfaces.gl" );
	rl.group = 0;
	rl.frame = 28;
	m_colorButtonBmp = GetDevice()->FindBitmaps( &rl );

	if(m_colorButtonBmp)
	{	
		stPointI ptBorderIncise(10,10);
		IBitmap* pBmp = m_colorButtonBmp->GetBitmap(0);
		if(pBmp)
		{
			stRectI rcTexture = pBmp->GetClipRect();
			CuttRect9(rcTexture,ptBorderIncise,ptBorderIncise,m_bmpRect);		
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param scale : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiButton::SetBitmapScale( const stPointF& scale )
{
	m_bitmapScale = scale;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiButton::Render( float fElapsedTime )
{
	stRectI oldClip;
	stRectI rc = m_rectClip;
	rc.OffsetRect(m_pDialog->GetClientX(),m_pDialog->GetClientY());

	oldClip = GetDevice()->SetClipRect(&rc);

	stRectI rcWindow;



	CGuiControl::Render(fElapsedTime);

	rcWindow.SetRect(m_x,m_y,m_width + m_x , m_height + m_y);
	if(m_eButtonType == ButtonType_Color)
	{	
		rcWindow.OffsetRect(m_pDialog->GetClientX(),m_pDialog->GetClientY());
		stPointI ptBorderIncise(10,10);
		stRectI renderRect[9];
		CuttRect9(rcWindow,ptBorderIncise,ptBorderIncise,renderRect);
		for(size_t i = 0;i < 9; ++i)
		{
			PatchRenderImage(renderRect[i],m_colorButtonBmp,&m_bmpRect[i],GetBackGroundColor(),Blend_Null,DT_LEFT | DT_BOTTOM);
		}
		GetDevice()->SetClipRect(&oldClip);
		return ;
	}


    int nOffsetX = 0;
    int nOffsetY = 0;

    GUI_CONTROL_STATE iState = GUI_STATE_NORMAL;

	if( GetGuiManager()->GetMouseOverDlg() == m_pDialog )
		int iii = 0;

    if( m_bVisible == false )
    {
        iState = GUI_STATE_HIDDEN;
    }
    else if( m_bEnabled == false )
    {
        iState = GUI_STATE_DISABLED;
    }
    else if( m_bPressed || m_bChecked)
    {
        iState = GUI_STATE_PRESSED;
    }
    else if( m_bMouseOver && GetGuiManager()->GetMouseOverDlg() == m_pDialog)
    {
		//if(!m_ToolTips.Empty())
		//{
		//	//GetGuiManager()->SetToolTips(&m_ToolTips,stPointI(m_pDialog->GetClientX() + m_rcBoundingBox.left - 20,m_pDialog->GetClientY() + m_rcBoundingBox.top - 20));
		//	stPointI pt;
		//	pt.x = m_pDialog->GetClientX() + m_rcBoundingBox.left - m_ToolTips.GetWidth();
		//	pt.y = m_pDialog->GetClientY() + m_rcBoundingBox.top-m_ToolTips.GetHeight();

		//	if(pt.x < 0)
		//		pt.x = m_pDialog->GetClientX() + m_rcBoundingBox.right;
		//	if(pt.y < 0)
		//		pt.y = m_pDialog->GetClientY() + m_rcBoundingBox.bottom;

		//	GetGuiManager()->SetToolTips(&m_ToolTips, pt);
		//}

        iState = GUI_STATE_MOUSEOVER;
    }
    else if( m_bHasFocus )
    {
        iState = GUI_STATE_FOCUS;
    }

	float fBlendRate = 0.0f;

    /*OffsetRect( &rcWindow, nOffsetX, nOffsetY );
	if(m_eButtonType == ButtonType_Bitmap && m_Elements.size())
	{
		CGuiElement* pElement = m_Elements[ 0 ];
		if(pElement->pBmp)
		{
			stPointI pt = pElement->pBmp->GetClipOffset();
			rcWindow.left -= pt.x;
			rcWindow.top -= pt.y;
		}

	}*/
	for(int i=0;i<m_Elements.size();++i)
	{
		CGuiElement* pElement = m_Elements[ i ];
		
		if(pElement->pBmp && pElement->pBmp->GetFrameCount())
		{
			if(m_eButtonType == ButtonType_Bitmap || m_eButtonType == ButtonType_Bitmap2)
			{
				// Blend current color
				rcWindow.right = rcWindow.left + (long)(pElement->pBmp->GetBitmap(0)->GetWidth()*m_bitmapScale.x);
				rcWindow.bottom = rcWindow.top + (long)(pElement->pBmp->GetBitmap(0)->GetHeight()*m_bitmapScale.y);
			}
			pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );

			//use flash
			static DWORD dstColor = 0;
			if ( m_bFlashState )		//
			{
				if ( (pElement->TextureColor.Current.a <0.1f) && (dstColor == 0) )
					dstColor = -1;
				else if ( (pElement->TextureColor.Current.a > 0.9f) && (dstColor == -1) )
					dstColor = 0;

				pElement->TextureColor.Blend( dstColor, fElapsedTime, true, m_fFlashRate );
			}
			m_pDialog->DrawSprite( pElement, &rcWindow );
		}

		pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );
		
		if(strcmp(m_strText, "5") == 0)
			pElement=pElement;

		m_pDialog->DrawText( m_strText, pElement, &rcWindow );
	}
	GetDevice()->SetClipRect(&oldClip);
}

void CGuiButton::PlayFlash( float fRate /* = 0.7f */ )
{
	m_bFlashState = true;
	m_fFlashRate = fRate;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiButton::UpdateRects()
{
	/*if(GetButtonType() == ButtonType_Normal || GetButtonType() == ButtonType_CloseBox || m_Elements.size() <= 0 || m_Elements[0]->pBmp == NULL)
	{
		m_rcBoundingBox.SetRect( m_x, m_y, m_x + m_width, m_y + m_height );
	}
	else*/
	CGuiControl::UpdateRects();
	if(GetButtonType() == ButtonType_Normal || GetButtonType() == ButtonType_CloseBox || m_Elements.size() <= 0 || m_Elements[0]->pBmp == NULL || m_Elements[0]->pBmp->GetFrameCount() == 0)
	{
		if(m_Elements.size() && m_Elements[0]->pBmp)
		{
			m_width = m_Elements[0]->rcTexture.Width();
			m_height = m_Elements[0]->rcTexture.Height();
			m_rcBoundingBox.right = m_x + m_width;
			m_rcBoundingBox.bottom = m_y + m_height;
		}
		m_rcBoundingBox.SetRect( m_x, m_y, m_x + m_width, m_y + m_height );
	}
	else
	{
		stPointI ptOffset = m_Elements[0]->pBmp->GetBitmap(0)->GetClipOffset();
		int width = (int)(m_Elements[0]->pBmp->GetBitmap(0)->GetClipWidth() * m_bitmapScale.x);
		int height = (int)(m_Elements[0]->pBmp->GetBitmap(0)->GetClipHeight() * m_bitmapScale.y);
		if( GetButtonType() == ButtonType_Bitmap2)
			m_rcBoundingBox.SetRect(m_x + (long)(ptOffset.x * m_bitmapScale.x),m_y + (long)(ptOffset.y * m_bitmapScale.y),m_x + (long)(ptOffset.x * m_bitmapScale.x) + width,m_y + (long)(ptOffset.y * m_bitmapScale.y) + height);
		else 
			m_rcBoundingBox.SetRect(m_x + ptOffset.x,m_y + ptOffset.y,m_x + ptOffset.x + width,m_y + ptOffset.y + height);
	}
	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rl : ÃèÊö
 * \param frameNum : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiButton::GetBitmapInfo(stResourceLocation& rl,int & frameNum)
{	
	if(GetButtonType() != ButtonType_Bitmap && GetButtonType() != ButtonType_Bitmap2)
		return false;
	if(m_Elements.size() == 0) return false;
	if(m_Elements[0]->pBmp == NULL) return false;
	rl.SetFileName(m_Elements[0]->pBmp->GetPackName());
	rl.group = m_Elements[0]->pBmp->GetGroup();
	rl.frame = m_Elements[0]->pBmp->GetFrame();
	frameNum = m_Elements.size();
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rl : ÃèÊö
 * \param frameNum : ÃèÊö
 * \param dwState : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiButton::SetBitmap(stResourceLocation& rl,int frameNum,DWORD* dwState)
{
	if(GetButtonType() != ButtonType_Bitmap && GetButtonType() != ButtonType_Bitmap2) return;

	for(int i=0;i<m_Elements.size(); ++i)
		delete m_Elements[i];
	m_Elements.resize(0);
	m_Elements.resize(frameNum);
	DWORD astate[] ={GUI_STATE_NORMAL,GUI_STATE_MOUSEOVER,GUI_STATE_PRESSED,GUI_STATE_DISABLED,99};
	bool bHasMouseOver  = true;

	if(dwState)
	{
		bHasMouseOver = false;
		for(int i=0;i<frameNum;++i)
		{
			if(dwState[i] == GUI_STATE_MOUSEOVER)
			{
				bHasMouseOver = true;
				break;
			}
		}
	}

	if(frameNum == 1) bHasMouseOver = false;

	for(int i=0;i<frameNum;++i)
	{
		CGuiElement* pEmt = new CGuiElement;
		stResourceLocation rl0 = rl;
		rl0.frame += i;
		pEmt->SetTexture(&rl0);
		pEmt->SetFont(0);
		int state = GUI_STATE_NORMAL;
		if(dwState )
			state = dwState[i];
		else if( i < count_of(astate))
			state = astate[i];

		switch(state)
		{

		case 99:	// Òş²Ø
			pEmt->TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(0, 255, 255, 255);
			pEmt->TextureColor.States[ GUI_STATE_PRESSED ] = 0;
			pEmt->TextureColor.States[ GUI_STATE_MOUSEOVER ] = 0;
			pEmt->TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(0, 255, 255, 255);

			if(frameNum == 1)
			{
				pEmt->TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 255, 255, 255);	
			}

			if(bHasMouseOver == false)
			{
				CGuiElement* pEmtAdd = new CGuiElement;
				*pEmtAdd = *pEmt;
				m_Elements.push_back(pEmtAdd);
				pEmtAdd->blend = Blend_ColorAdd;
				pEmtAdd->TextureColor.States[ GUI_STATE_NORMAL ] = 0;
				pEmtAdd->TextureColor.States[ GUI_STATE_PRESSED ] = 0;
				pEmtAdd->TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 20, 20, 20);
				pEmtAdd->TextureColor.States[ GUI_STATE_FOCUS ] = 0;
				pEmtAdd->TextureColor.States[ GUI_STATE_DISABLED] = 0;
			}
			break;
		case GUI_STATE_NORMAL:	// Normal
			pEmt->TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
			pEmt->TextureColor.States[ GUI_STATE_PRESSED ] = 0;
			pEmt->TextureColor.States[ GUI_STATE_MOUSEOVER ] = 0;
			pEmt->TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);

			if(frameNum == 1)
			{
				pEmt->TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);	
			}

			if(bHasMouseOver == false)
			{
				CGuiElement* pEmtAdd = new CGuiElement;
				*pEmtAdd = *pEmt;
				m_Elements.push_back(pEmtAdd);
				pEmtAdd->blend = Blend_ColorAdd;
				pEmtAdd->TextureColor.States[ GUI_STATE_NORMAL ] = 0;
				pEmtAdd->TextureColor.States[ GUI_STATE_PRESSED ] = 0;
				pEmtAdd->TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 20, 20, 20);
				pEmtAdd->TextureColor.States[ GUI_STATE_FOCUS ] = 0;
				pEmtAdd->TextureColor.States[ GUI_STATE_DISABLED] = 0;
			}
			break;
		case GUI_STATE_MOUSEOVER:	// Focus
			if(!pEmt->pBmp || pEmt->pBmp->GetFrameCount() == 0 || pEmt->pBmp->GetBitmap(0)->IsNull())
			{
				if(m_Elements.size())
				{
					*pEmt = *(m_Elements[0]);
					pEmt->blend = Blend_ColorAdd;
					pEmt->TextureColor.States[ GUI_STATE_NORMAL ] = 0;
					pEmt->TextureColor.States[ GUI_STATE_PRESSED ] = 0;
					pEmt->TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 20, 20, 20);
					pEmt->TextureColor.States[ GUI_STATE_FOCUS ] = 0;
					pEmt->TextureColor.States[ GUI_STATE_DISABLED] = 0;
				}
			}
			else
			{
				pEmt->TextureColor.States[ GUI_STATE_DISABLED] = 0;
				pEmt->TextureColor.States[ GUI_STATE_NORMAL ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_PRESSED ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
				pEmt->TextureColor.States[ GUI_STATE_FOCUS ] = 0;//D3DCOLOR_ARGB(255, 255, 255, 255);
			}
			break;
		case GUI_STATE_PRESSED:	// Pressed
			if(!pEmt->pBmp || pEmt->pBmp->GetFrameCount() == 0 || pEmt->pBmp->GetBitmap(0)->IsNull())
			{
				pEmt->TextureColor.States[ GUI_STATE_NORMAL ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_PRESSED ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_MOUSEOVER ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_FOCUS ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_DISABLED] = 0;
				
				if(m_Elements.size())
				{
					CGuiElement* pEmt2 = m_Elements[0];
					pEmt2->TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
				}
			}
			else
			{
				pEmt->TextureColor.States[ GUI_STATE_NORMAL ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
				pEmt->TextureColor.States[ GUI_STATE_MOUSEOVER ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_FOCUS ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_DISABLED] = 0;
			}
			break;
		case GUI_STATE_DISABLED:	// Disabled
			if(!pEmt->pBmp || pEmt->pBmp->GetFrameCount() == 0 || pEmt->pBmp->GetBitmap(0)->IsNull())
			{
				pEmt->TextureColor.States[ GUI_STATE_NORMAL ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_PRESSED ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_MOUSEOVER ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_FOCUS ] = 0;

				if(m_Elements.size())
				{
					CGuiElement* pEmt2 = m_Elements[0];
					pEmt2->TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
				}
			}
			else
			{
				if(m_Elements.size())
				{
					CGuiElement* pEmt2 = m_Elements[0];
					pEmt2->TextureColor.States[ GUI_STATE_DISABLED ] = 0;
				}
				pEmt->TextureColor.States[ GUI_STATE_NORMAL ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_PRESSED ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
				pEmt->TextureColor.States[ GUI_STATE_MOUSEOVER ] = 0;
				pEmt->TextureColor.States[ GUI_STATE_FOCUS ] = 0;
			}
			break;
		default:
			break;
		}
		//pEmt->FontColor = pEmt->TextureColor;
		m_Elements[i] = pEmt;
	}
	UpdateRects();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszCaption : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
SIZE CGuiButton::CalcuSize(const char* pszCaption)
{
	SIZE sz = { strlen(pszCaption) * 6 + 18,22};
	return sz;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiButton::ContainsPoint( POINT pt )
{
	if(!m_rectClip.PtInRect(pt)) return false;
	if(m_eButtonType == ButtonType_Bitmap )
	{		
		if(m_Elements.size() && m_Elements[0]->pBmp && m_Elements[0]->pBmp->GetFrameCount())
		{
			pt.x -= m_x;
			pt.y -= m_y;

			IBitmap* pBmp = m_Elements[0]->pBmp->GetBitmap(0);
			return pBmp->IsAt(pt,false,false);
		}
	}
	return CGuiControl::ContainsPoint(pt);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bChecked : ÃèÊö
 * \param bFromInput : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiButton::SetCheckedInternal( bool bChecked, bool bFromInput ) 
{ 
	
	if( bChecked && bChecked != m_bChecked && m_nButtonGroup != 0 )
		m_pDialog->ClearButtonGroup( m_nButtonGroup );

	m_bChecked = bChecked; 
	if(bFromInput)
		m_pDialog->SendEvent( EVENT_CHECKBOX_CHANGED, bFromInput, this ); 
}