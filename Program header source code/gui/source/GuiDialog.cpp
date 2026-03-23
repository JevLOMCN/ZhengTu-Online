/**
* \file      GuiDialog.cpp
* \version   $Id$
* \author    $$$03π´Àæ@###.com
*           $$$03π´Àæ@###.com
* \date      2006-7-19  21:11:00
* \brief     GUI¥∞ø⁄ª˘¿‡
* 
*	      GUI¥∞ø⁄ª˘¿‡
*/

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../../xml_parse/XmlParseEx.h"
#include "../../engine/include/stringex.h"
#include "../../engine/include/Animation.h"
#include "../include/GuiControl.h"
#include "../../luabind/public.h"

double        CGuiDialog::s_fTimeRefresh = 0.0f;
CGuiControl* CGuiDialog::s_pControlFocus = NULL;        // The control which has focus
CGuiControl* CGuiDialog::s_pControlPressed = NULL;      // The control currently pressed
std::vector< GuiElementHolder* > CGuiDialog::s_DefaultElements;
std::vector< CGuiElement* > CGuiDialog::s_BackElements;
t_GuiMapCtrlBorder CGuiDialog::s_mapCtrlBorder;
CGuiElement CGuiDialog::s_CapElement;  // Element for the caption

stPointI CGuiDialog::s_borderTopLeft(89,96);
stPointI CGuiDialog::s_borderRightBottom(39,20);
stPointI CGuiDialog::s_clientOffsetTopLeft(77,75);
stPointI CGuiDialog::s_clientOffsetRightBottom(8,8);
stRectI  CGuiDialog::s_borderRect(0,0,256,256);

stPointI CGuiDialog::s_smBorderTopLeft(14,13);
stPointI CGuiDialog::s_smBorderRightBottom(14,14);
stPointI CGuiDialog::s_smClientOffsetTopLeft(14,14);
stPointI CGuiDialog::s_smClientOffsetRightBottom(14,14);
stRectI  CGuiDialog::s_smBorderRect(0,0,256,256);

stPointI CGuiDialog::s_ptLeftTopOffset(-24,-81);
CGuiElement CGuiDialog::s_LeftTopElement;

CGuiElement CGuiDialog::s_BackGroundElement;
static CGuiElement s_controlBackGroundElement;
static int s_nCapOffsetY = 0;
static int s_nSBWidth = 22;

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
const int s_nAnchorWidth = 6;

stGuiCtrlBorderInfo::stGuiCtrlBorderInfo()
: ptLeftTop(12,9)
, ptRightBottom(8,10)
, ptBorderLeftTop(12,9)
, ptBorderRightBottom(8,10)
, rcTexture(0, 0, 180, 251)
{
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pRect : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void stGuiCtrlBorderInfo::CuttRect9(RECT* pRect)
{
	::CuttRect9(rcTexture,ptLeftTop,ptRightBottom,pRect);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//Ã“◊”◊∞±∏øÚ“≥√Êµƒ∂‘”⁄≤π∂°√˚◊÷
const char* GetGuiGraphicPackName()
{
	static char szFileName[] = "data\\interfaces.gl";
	return szFileName;
}

//--------------------------------------------------------------------------------------
// CGuiDialog class
//--------------------------------------------------------------------------------------


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
CGuiDialog::CGuiDialog(DWORD dwStyle)
: m_dwStyle(dwStyle)
, m_bBottomMost(false)
, m_bTopMost(false)
, m_pBackImage(NULL)
, m_bClosed(false)
, m_bEnabled(true)
, m_bVisible(true)
, m_pBtnClose(NULL)
, m_minSize(100,50)
, m_maxSize(1280,768)
, m_eAlign(GUI_ALIGN_NONE)
, m_lua(NULL)
, m_dwColor(0xffffffff)
, m_bCloseIsHide(false)
, m_borderTopLeft(s_borderTopLeft)
, m_borderRightBottom(s_borderRightBottom)
, m_clientOffsetTopLeft(s_clientOffsetTopLeft)
, m_clientOffsetRightBottom(s_clientOffsetRightBottom)
, m_bSmDialog(false)
, m_nSBWidth(s_nSBWidth)
, m_ptCaptionOffset(0,15)
, m_colorCaption(COLOR_ARGB(128,0,0,0))
, m_colorBorder(COLOR_ARGB(200,0,0,0))
, m_pAniInfo(NULL)
, m_pBackGroundElement(NULL)
, m_ptBackGroundOffsetTopLeft(20,38)
, m_ptBackGroundOffsetRightBottom(5,6)
, m_nCapOffsetY(s_nCapOffsetY)
, m_pCapElement(NULL)
, m_dwTimeAnchor(-1)
, m_bAnchor(false)
, m_pPopupMenu(NULL)
, m_pLeftTopElement(NULL)
, m_ptLeftTopOffset(s_ptLeftTopOffset)
, m_pIcon(NULL)
, m_ptIcon(8,8)
, m_nShowSoundIndex(0)
, m_nHideSoundIndex(0)
, m_bPlayOpenSound(false)
, m_bPlayCloseSound(false)
, m_pLastFocus(NULL)
{
	m_ID = 0;
	m_x = 0;
	m_y = 0;
	m_width = 0;
	m_height = 0;
	m_bDrag = false;
	m_bSizing = false;
	m_bCaption = false;
	m_bMinimized = false;
	m_szCaption[0] = L'\0';
	m_nCaptionHeight = 0;

	m_colorTopLeft = COLOR_ARGB(255,98,138,206);
	m_colorTopRight = COLOR_ARGB(255,54,105,192);
	m_colorBottomLeft = COLOR_ARGB(255,54,105,192);
	m_colorBottomRight = COLOR_ARGB(255,10,73,179);

	//m_pCallbackEvent = NULL;

	m_fTimeLastRefresh = 0;

	m_pControlMouseOver = NULL;

	m_nDefaultControlID = 0xffff;
	m_bNonUserEvents = false;
	m_bKeyboardInput = true;
	m_bMouseInput = true;

	//InitDefaultElements();
	m_szName[0] = 0;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
CGuiDialog::~CGuiDialog()
{
	int i=0;
	int j=0;
	delete m_lua;
	m_lua = NULL;
	RemoveAllControls();
	delete m_pAniInfo;
	for( int i=0; i < m_Controls.size(); i++ )
	{

		delete m_Controls[i];
	}

	m_Controls.clear();
	//m_Fonts.RemoveAll();
	//m_Textures.RemoveAll();

	for( int i=0; i < m_DefaultElements.size(); i++ )
	{
		GuiElementHolder* pElementHolder = m_DefaultElements[ i ];
		SAFE_DELETE( pElementHolder );
	}

	m_DefaultElements.resize(0);

	for(t_GuiMapCtrlBorder::iterator it = m_mapCtrlBorder.begin(); it != m_mapCtrlBorder.end(); ++it)
	{
		delete it->second;
	}
	m_mapCtrlBorder.clear();

	for(int i = 0; i < m_BackElements.size();++i)
	{
		delete m_BackElements[i];
	}
	m_BackElements.resize(0);

	delete m_pBackGroundElement;
	m_pBackGroundElement = NULL;
	delete m_pCapElement;
	m_pCapElement = NULL;
}

class AAAA{
};
void CGuiDialog::_bind_lua(lua_State* L)
{
	using namespace luabind;
	module(L)
	[
		class_<CGuiDialog>("CGuiDialog")
			.def("GetID",&CGuiDialog::GetID)
			.def("IsFocus",&CGuiDialog::IsFocusInScript)
			.def("GetX",&CGuiDialog::GetX)
			.def("GetY",&CGuiDialog::GetY)
			.def("GetWidth", &CGuiDialog::GetWidth)
			.def("GetHeight", &CGuiDialog::GetHeight)
			.def("GetClientX", &CGuiDialog::GetClientX)
			.def("GetClientY", &CGuiDialog::GetClientY)
			.def("GetClientWidth", &CGuiDialog::GetClientWidth)
			.def("GetClientHeight", &CGuiDialog::GetClientHeight)
			.def("SetEnabled", &CGuiDialog::SetEnabled)
			.def("IsEnabled", &CGuiDialog::IsEnabled)
			.def("SetVisible", &CGuiDialog::SetVisibleInScript)
			.def("IsVisible", &CGuiDialog::IsVisible)
			.def("Close", &CGuiDialog::Close)
			.def("SetZOrder", &CGuiDialog::SetZOrder)
			.def("SetSize", &CGuiDialog::SetSize)
			.def("SetLocation", &CGuiDialog::SetLocation)

			.def("GetCheckedRadioButton",&CGuiDialog::GetCheckedRadioButton)
			.def("CheckRadioButton", &CGuiDialog::CheckRadioButton)
			.def("ClearRadioButtonGroup", &CGuiDialog::ClearRadioButtonGroup)
			.def("ClearButtonGroup", &CGuiDialog::ClearButtonGroup)

			.def("GetControl", &CGuiDialog::GetControlInScript)
			.def("GetEditBox", &CGuiDialog::GetEditBox)
			.def("GetMLEditBox", &CGuiDialog::GetMLEditBox)
			.def("GetButton", &CGuiDialog::GetButton)
			.def("GetCheckBox",&CGuiDialog::GetCheckBox)
			.def("GetRadioButton", &CGuiDialog::GetRadioButton)
			.def("GetComboBox", &CGuiDialog::GetComboBox)
			.def("GetSlider", &CGuiDialog::GetSlider)
			.def("GetListBox", &CGuiDialog::GetListBox)
			.def("GetImage", &CGuiDialog::GetImage)
			.def("GetItemCell", &CGuiDialog::GetItemCell)
			.def("GetTable",&CGuiDialog::GetTable)
			.def("GetProcess", &CGuiDialog::GetProcess)
			.def("GetTab", &CGuiDialog::GetTab)
			.def("GetScrollTab", &CGuiDialog::GetScrollTab)
			.def("GetMLTextBox", &CGuiDialog::GetMLTextBox)
			.def("SetFocus", &CGuiDialog::SetFocus)
			.def("AddControl2TabByID", &CGuiDialog::AddControl2TabByID)
			.def("SetButtonGroup", &CGuiDialog::SetButtonGroup)
	];
}

void CGuiDialog::bind_lua(lua_State * L)
{
	CGuiDialog::_bind_lua(L);
	m_lua->SetGlobal("this",this);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::RemoveControl(CGuiControl* pControl)
{
	Assert(pControl != this->m_pBtnClose);
	// Clean focus first
	ClearFocus();

	// Clear references to this control
	if( s_pControlFocus == pControl )
		s_pControlFocus = NULL;
	if( m_pLastFocus == pControl)
		m_pLastFocus = NULL;
	if( s_pControlPressed == pControl )
		s_pControlPressed = NULL;
	if( m_pControlMouseOver == pControl )
		m_pControlMouseOver = NULL;

	TRACE("RemoveControl %p\n",pControl);

	pControl->m_bDeleted = true;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::RemoveControl( int ID )
{
	for( int i=0; i < m_Controls.size(); i++ )
	{
		CGuiControl* pControl = m_Controls[ i ];
		if( pControl->GetID() == ID )
		{
			// Clean focus first
			ClearFocus();

			// Clear references to this control
			if( s_pControlFocus == pControl )
				s_pControlFocus = NULL;
			if( m_pLastFocus == pControl)
				m_pLastFocus = NULL;
			if( s_pControlPressed == pControl )
				s_pControlPressed = NULL;
			if( m_pControlMouseOver == pControl )
				m_pControlMouseOver = NULL;

			pControl->m_bDeleted = true;
			//SAFE_DELETE( pControl );
			//m_Controls.erase( m_Controls.begin() + i  );

			return;
		}
	}
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::RemoveAllControls()
{
	if( s_pControlFocus && s_pControlFocus->m_pDialog == this )
		s_pControlFocus = NULL;
	if( s_pControlPressed && s_pControlPressed->m_pDialog == this )
		s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;
	m_pLastFocus = NULL;
	for( int i=0; i < m_Controls.size(); i++ )
	{
		m_Controls[i]->m_bDeleted = true;
	}

}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::Refresh()
{
	if( s_pControlFocus )
		s_pControlFocus->OnFocusOut();

	if( m_pControlMouseOver )
		m_pControlMouseOver->OnMouseLeave();

	s_pControlFocus = NULL;
	s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;
	m_pLastFocus = NULL;
	for( int i=0; i < m_Controls.size(); i++ )
	{
		CGuiControl* pControl = m_Controls[ i ];
		if(pControl->m_bDeleted)
		{
		}
		else
		{
			pControl->Refresh();
		}
	}

	if( m_bKeyboardInput )
		FocusDefaultControl();
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
CGuiElement* CGuiDialog::GetCaptionElement()
{
	return &s_CapElement;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param fElapsedTime : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::RenderBackground(float fElapsedTime)
{
	int iFont = GetDevice()->SetFont(1);

	if( !m_bMinimized && !m_pBackImage && (GetStyle() & GUI_WS_NOSKIN) )
	{
		stRectI capRect(m_x,m_y,m_x + m_width,m_y + m_height);
		GetDevice()->FillRect(&capRect,m_colorTopLeft,m_colorTopRight,m_colorBottomLeft,m_colorBottomRight);
	}


	if( !m_bMinimized && !m_pBackImage && ( GetStyle() & GUI_WS_NOSKIN ) )
	{
		stRectI capRect(m_x,m_y ,m_x + m_width,m_y + m_height);
		::DrawRectangle(capRect,3,m_colorBorder);

		capRect.left += m_clientOffsetTopLeft.x;
		capRect.top += m_clientOffsetTopLeft.y;
		capRect.right -= m_clientOffsetRightBottom.x;
		capRect.bottom -= m_clientOffsetRightBottom.y;

		GetDevice()->FillRect(&capRect,m_colorTopLeft,m_colorTopRight,m_colorBottomLeft,m_colorBottomRight);

		capRect.top = m_y;
		capRect.bottom = capRect.top + m_clientOffsetTopLeft.y;
		GetDevice()->FillRect(&capRect,m_colorCaption);
	}


	// If the dialog is minimized, skip rendering
	// its controls.
	if( !m_bMinimized )
	{
		if(m_pBackImage)
		{
			m_pBackImage->Render(fElapsedTime);
		}
		else
		{
			stRectI rectBack(GetX(),GetY(),GetX() + GetWidth(),GetY() + GetHeight());

			if( 0 == (GetStyle() & GUI_WS_NOSKIN) )
			{
				stRectI rect = rectBack;

				// ªÊ÷∆µ◊Œ∆
				rect.left += m_ptBackGroundOffsetTopLeft.x;
				rect.top += m_ptBackGroundOffsetTopLeft.y;
				rect.right -= m_ptBackGroundOffsetRightBottom.x;
				rect.bottom -= m_ptBackGroundOffsetRightBottom.y;
				if(m_pBackGroundElement)
				{
					::PatchRenderImage(rect,m_pBackGroundElement->pBmp,&m_pBackGroundElement->rcTexture,m_dwColor,m_pBackGroundElement->blend,0);
				}
				else
				{
					::PatchRenderImage(rect,s_BackGroundElement.pBmp,&s_BackGroundElement.rcTexture,m_dwColor,Blend_Null,0);
				}

				// ªÊ÷∆µ◊øÚ
				if(m_BackElements.size())
				{
					for(size_t i=0;i<9;++i)
					{
						stRectI rect = m_BackRects[i];
						rect.OffsetRect(m_x,m_y);
						::PatchRenderImage(rect,m_BackElements[i]->pBmp,&m_BackElements[i]->rcTexture,m_dwColor,m_BackElements[i]->blend,s_PatchAlignFormat[i]);
					}
				}
				else
				{
					if(m_bSmDialog) 
					{
						for( size_t i=9;i<18;++i )
						{
							stRectI rect = m_BackRects[i-9];
							rect.OffsetRect(m_x,m_y);
							::PatchRenderImage(rect,s_BackElements[i]->pBmp,&s_BackElements[i]->rcTexture,m_dwColor,s_BackElements[i]->blend,s_PatchAlignFormat[i- 9]);
						}
					}
					else
					{
						for( size_t i=0;i<9;++i )
						{
							stRectI rect = m_BackRects[i];
							rect.OffsetRect(m_x,m_y);
							::PatchRenderImage(rect,s_BackElements[i]->pBmp,&s_BackElements[i]->rcTexture,m_dwColor,s_BackElements[i]->blend,s_PatchAlignFormat[i]);
						}
					}
				}


				// Render the caption if it's enabled.
				if( m_bCaption )
				{	
					CGuiElement* pEmt = (m_pCapElement ? m_pCapElement : &s_CapElement);
					if(pEmt->pBmp && pEmt->pBmp->GetFrameCount())
					{
						stRectI rect = rectBack;
						rect.top += m_nCapOffsetY;
						rect.left += (GetWidth() - pEmt->rcTexture.Width())/2;
						pEmt->pBmp->RenderAni(rect.left,rect.top,xtimeGetTime(),&pEmt->rcTexture,NULL,m_dwColor,pEmt->blend);
					}
					stRectI rect = rectBack;
					if(!m_bSmDialog)
					{
						rect.top += 20;
						rect.left += 77;
						rect.right -= 31;
					}
					else
						rect.top += 18;

					GetDevice()->DrawString(m_szCaption,rect,pEmt->FontColor.Current,DT_CENTER | DT_TOP,FontEffect_Shadow);
				}

			}
			else
			{
				// Render the caption if it's enabled.
				if( m_bCaption )
				{	
					stRectI rect = rectBack;
					rect.top +=  3;
					GetDevice()->DrawString(m_szCaption,rect,COLOR_ARGB(255,255,255,255),DT_CENTER | DT_TOP,FontEffect_Shadow);
				}
			}
		}
	}
	GetDevice()->SetFont(iFont);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param fElapsedTime : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::OnRender( float fElapsedTime )
{   
	RefreshFrame();
	for( int i = 0; i< m_Controls.size(); )
	{
		if( m_Controls[i]->m_bDeleted )
		{
			delete m_Controls[i];
			m_Controls.erase(m_Controls.begin() + i);
		}
		else
		{
			++i;
		}
	}

	if(Engine_GetCursor()->IsLButtonDown() == false && m_dwTimeAnchor != -1 && GetGuiManager()->GetMouseOverDlg() != this && xtimeGetTime() - m_dwTimeAnchor > 400)
	{
		stPointI pt = Engine_GetCursor()->GetPosition();
		stRectI rcClip = GetClipRect();
		rcClip.OffsetRect(m_x, m_y);
		if(!rcClip.PtInRect(pt))
			BerthHide();
	}
	RunAnimation(fElapsedTime);
	// See if the dialog needs to be refreshed
	if( m_fTimeLastRefresh < s_fTimeRefresh )
	{
		m_fTimeLastRefresh = Engine_GetTime();
		Refresh();
	}

	RenderBackground(fElapsedTime);

	// If the dialog is minimized, skip rendering
	// its controls.
	if( !m_bMinimized )
	{

		bool bFocusPanel = (s_pControlFocus && 
			(s_pControlFocus->GetType() == GUI_CONTROL_TAB || s_pControlFocus->GetType() == GUI_CONTROL_PANEL || s_pControlFocus->GetType() == GUI_CONTROL_BUTTON || s_pControlFocus->GetType() == GUI_CONTROL_LISTBOX));
		for( int i=0; i < m_Controls.size(); i++ )
		{
			CGuiControl* pControl = m_Controls[ i ];   

			// Focused control is drawn last
			if( pControl == s_pControlFocus && !bFocusPanel )
				continue;

			if(pControl == m_pBackImage) continue;

			if(pControl->IsVisible())
				pControl->Render(fElapsedTime );
		}

		if( s_pControlFocus != NULL  && s_pControlFocus->m_pDialog == this && s_pControlFocus != m_pBackImage && !bFocusPanel && s_pControlFocus->IsVisible())
			s_pControlFocus->Render(fElapsedTime );
	}

	if( !m_pBackImage && (GetStyle() & GUI_WS_NOSKIN) == 0 && !m_bSmDialog )
	{
		// ªÊ÷∆◊Û…œΩ«Õº∆¨
		CGuiElement* pEmt = (m_pLeftTopElement ? m_pLeftTopElement : &s_LeftTopElement);
		if(pEmt->pBmp && pEmt->pBmp->GetFrameCount())
		{
			pEmt->pBmp->RenderAni(m_x + m_ptLeftTopOffset.x,m_y + m_ptLeftTopOffset.y,xtimeGetTime(),&pEmt->rcTexture,NULL,m_dwColor,pEmt->blend);
		}
	}
	if(m_pIcon)
	{
		m_pIcon->RenderAni(m_x + m_ptIcon.x,m_y + m_ptIcon.y,xtimeGetTime(),(DWORD)-1);
	}
	return S_OK;
}

void CGuiDialog::RefreshFrame()
{
	if(m_bPlayOpenSound)
	{
		if(m_nShowSoundIndex)
			Engine_GetCallBack()->PlayUISound(m_nShowSoundIndex);
		m_bPlayOpenSound = false;
	}
	if(m_bPlayCloseSound)
	{
		if(m_nHideSoundIndex)
			Engine_GetCallBack()->PlayUISound(m_nHideSoundIndex);
		m_bPlayOpenSound = false;
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::OnHide()
{

}

void CGuiDialog::OnCompleteAniEvent()
{

}
/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nEvent : √Ë ˆ
* \param nID : √Ë ˆ
* \param pControl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
bool CGuiDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent)
	{
	case EVENT_DIALOG_COMMAND:
		{
			switch(nID)
			{
			case ID_EDIT_CLEAR:
				if(s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetType() == GUI_CONTROL_EDITBOX
					&& ( 0 == (((CGuiEditBox*)s_pControlFocus)->GetStyle() & CGuiEditBox::GUI_ES_READONLY)) )
				{
					((CGuiEditBox*)s_pControlFocus)->DeleteSelectionText();
					((CGuiEditBox*)s_pControlFocus)->OnTextChange();
					return true;
				}
				if(s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetType() == GUI_CONTROL_MLEDITBOX
					&& ( 0 == (((CGuiMLEditBox*)s_pControlFocus)->GetStyle() & CGuiMLEditBox::GUI_ES_READONLY)) )
				{
					((CGuiMLEditBox*)s_pControlFocus)->DeleteSelectionText();
					//((CGuiEditBox*)s_pControlFocus)->OnTextChange();
					return true;
				}
				break;
			case ID_EDIT_COPY:
				if(s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetType() == GUI_CONTROL_EDITBOX)
				{
					((CGuiEditBox*)s_pControlFocus)->CopyToClipboard();
					return true;
				}
				if(s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetType() == GUI_CONTROL_MLEDITBOX)
				{
					((CGuiMLEditBox*)s_pControlFocus)->CopyToClipboard();
					return true;
				}
				if (s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetType() == GUI_CONTROL_MLTEXTBOX)
				{
					((CGuiMLTextBox*)s_pControlFocus)->CopyToClipboard();
					return true;
				}
				break;
			case ID_EDIT_CUT:
				if( s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetType() == GUI_CONTROL_EDITBOX 
					&& ( 0 == (((CGuiEditBox*)s_pControlFocus)->GetStyle() & CGuiEditBox::GUI_ES_READONLY)) )
				{
					((CGuiEditBox*)s_pControlFocus)->CopyToClipboard();
					((CGuiEditBox*)s_pControlFocus)->DeleteSelectionText();
					((CGuiEditBox*)s_pControlFocus)->OnTextChange();
					return true;
				}
				if(s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetType() == GUI_CONTROL_MLEDITBOX
					&& ( 0 == (((CGuiMLEditBox*)s_pControlFocus)->GetStyle() & CGuiMLEditBox::GUI_ES_READONLY)) )
				{
					((CGuiMLEditBox*)s_pControlFocus)->CopyToClipboard();
					((CGuiMLEditBox*)s_pControlFocus)->DeleteSelectionText();
					//((CGuiMLEditBox*)s_pControlFocus)->OnTextChange();
					return true;
				}
				break;
			case ID_EDIT_PASTE:
				if(s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetType() == GUI_CONTROL_EDITBOX
					&& ( 0 == (((CGuiEditBox*)s_pControlFocus)->GetStyle() & CGuiEditBox::GUI_ES_READONLY)) )
				{
					if(((CGuiEditBox*)s_pControlFocus)->PasteFromClipboard())
					{
						((CGuiEditBox*)s_pControlFocus)->OnTextChange();
					}
					return true;
				}
				if(s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetType() == GUI_CONTROL_MLEDITBOX
					&& ( 0 == (((CGuiMLEditBox*)s_pControlFocus)->GetStyle() & CGuiMLEditBox::GUI_ES_READONLY)) )
				{
					((CGuiMLEditBox*)s_pControlFocus)->PasteFromClipboard();
					return true;
				}
				break;
			case ID_EDIT_SELECT_ALL:
				if(s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetType() == GUI_CONTROL_EDITBOX)
				{
					((CGuiEditBox*)s_pControlFocus)->SelectAll();
					return true;
				}
				if(s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetType() == GUI_CONTROL_MLEDITBOX)
				{
					((CGuiMLEditBox*)s_pControlFocus)->SelectAll();
					return true;
				}
				break;
			}
		}
	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
		case GUI_CLOSEBOX_ID:
			if(m_bCloseIsHide) 
			{
				SetVisible(false);
				OnHide();
			}
			else	Close();
			return true;
		}
		break;
	}
	return false;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nEvent : √Ë ˆ
* \param bTriggeredByUser : √Ë ˆ
* \param pControl : √Ë ˆ
* \param dwID : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------

bool CGuiDialog::SendEvent( UINT nEvent, bool bTriggeredByUser, CGuiControl* pControl ,DWORD dwID)
{
	// If no callback has been registered there's nowhere to send the event to
	//if( m_pCallbackEvent == NULL )
	//	return;

	// Discard events triggered programatically if these types of events haven't been
	// enabled
	if( !bTriggeredByUser && !m_bNonUserEvents )
	{
		return false;
	}

	if(dwID == -1)
		dwID = (int)(pControl ? pControl->GetID() : -1);

	//if(m_lua && m_lua->IsExistFunction("OnGuiEvent"))
	if(m_lua)
	{
		bool bResult = false;
		bResult = m_lua->Call<bool,int,int,CGuiControl*>("OnGuiEvent",false,(int)nEvent,(int)dwID,pControl);
		
		if(bResult) return true;
	}

	return OnGuiEvent(nEvent,dwID,pControl);
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param index : √Ë ˆ
* \param strFaceName : √Ë ˆ
* \param height : √Ë ˆ
* \param weight : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::SetFont( UINT index, const char* strFaceName, LONG height, LONG weight )
{
	/*
	// Make sure the list is at least as large as the index being set
	UINT i;
	for( i=m_Fonts.size(); i <= index; i++ )
	{
	m_Fonts.Add( -1 );
	}

	int iFont = GetGlobalDialogResourceManager()->AddFont( strFaceName, height, weight );
	m_Fonts.SetAt( index, iFont );
	//*/

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
bool CGuiDialog::OnContextMenu()
{
	if( m_pPopupMenu )
	{

		{
			m_pPopupMenu->Popup();
			return true;
		}
	}
	return false;
}

RECT CGuiDialog::GetClipRect()
{
	if(m_pBackImage)
	{
		CAnimation * pAni = m_pBackImage->GetImage(0);
		if(pAni) 
		{
			IBitmaps* pBmp = pAni->GetImages();
			if(pBmp && pBmp->GetBitmapCount())
			{
				stRectI rc = pBmp->GetBitmapByIndex(0)->GetClipRect();
				rc.OffsetRect(m_pBackImage->GetLocation());
				return rc;
			}
		}
	}
	return stRectI(0,0,GetWidth(),GetHeight());
}

void CGuiDialog::BerthShow()
{
	if(m_bAnchor)
	{
		m_bAnchor = false;
		const int mindis = s_nAnchorWidth;
		stGuiAnimationInfo st;
		st.ptDst = GetLocation();
		st.fTime = 0.4f;
		st.dwColorDst = GetColor();
		st.eAniComplete = GuiAniEvent_StopAnchor;

		stRectI clipRect = GetClipRect();
		if(m_x == - clipRect.right + mindis)
		{
			st.ptDst.x = -clipRect.left + 1;
		}
		else if(m_x  == GetDevice()->GetWidth()  - mindis - clipRect.left)
		{
			st.ptDst.x = GetDevice()->GetWidth() - clipRect.right - 1;
		}
		else if(m_y == - clipRect.bottom + mindis)
		{
			st.ptDst.y = -clipRect.top + 1;
		}
		else if(m_y == GetDevice()->GetHeight()  - mindis - clipRect.top)
		{
			st.ptDst.y = GetDevice()->GetHeight() - clipRect.bottom - 1;
		}

		BeginAnimation(&st);
	}
}

void CGuiDialog::BerthHide()
{
	const int mindis = s_nAnchorWidth;
	m_dwTimeAnchor = -1;
	stGuiAnimationInfo st;
	st.ptDst = GetLocation();
	st.fTime = 0.5;
	st.dwColorDst = GetColor();
	st.eAniComplete = GuiAniEvent_Anchor; 
	stRectI clipRect = GetClipRect();
	if(m_x == -clipRect.left)
	{
		st.ptDst.x = - clipRect.right + mindis;
	}
	else if(m_x + clipRect.right == GetDevice()->GetWidth())
	{
		st.ptDst.x = GetDevice()->GetWidth()  - mindis - clipRect.left;
	}
	else if(m_y == -clipRect.top)
	{
		st.ptDst.y = - clipRect.bottom + mindis;
	}
	else if(m_y + clipRect.bottom == GetDevice()->GetHeight())
	{
		st.ptDst.y = GetDevice()->GetHeight() - mindis - clipRect.top;
	}
	else return;
	BeginAnimation(&st);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param hWnd : √Ë ˆ
* \param uMsg : √Ë ˆ
* \param wParam : √Ë ˆ
* \param lParam : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
bool CGuiDialog::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	//if(!m_bEnable || !m_bVisible || m_bClosed) return false;

	static bool s_bTransactKeyDown = false;
	if(!IsEnabled()) return false;

	if(uMsg == WM_LBUTTONDOWN)
	{
		int iiiiii=0;
	}

	bool bHandled = false;
	bool bHandled2 = false;
	static POINT ptDrag;
	static enumSizingType s_eSizingType;
	static POINT ptSizing;

	// If caption is enable, check for clicks in the caption area.
	if(m_bAnchor && uMsg == WM_MOUSEMOVE && GetGuiManager()->GetMouseOverDlg() == this)
	{
		/*POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
		stRectI rect(m_x,m_y ,m_x + GetWidth(),m_y + GetHeight());
		if(rect.PtInRect(mousePoint))*/
		{
			//BerthShow();

			m_bAnchor = false;
			const int mindis = s_nAnchorWidth;
			stGuiAnimationInfo st;
			st.ptDst = GetLocation();
			st.fTime = 0.4f;
			st.dwColorDst = GetColor();
			st.eAniComplete = GuiAniEvent_StopAnchor;

			stRectI clipRect = GetClipRect();
			if(m_x == - clipRect.right + mindis)
			{
				st.ptDst.x = -clipRect.left;
			}
			else if(m_x  == GetDevice()->GetWidth()  - mindis - clipRect.left)
			{
				st.ptDst.x = GetDevice()->GetWidth() - clipRect.right ;
			}
			else if(m_y == - clipRect.bottom + mindis)
			{
				st.ptDst.y = -clipRect.top ;
			}
			else if(m_y == GetDevice()->GetHeight()  - mindis - clipRect.top)
			{
				st.ptDst.y = GetDevice()->GetHeight() - clipRect.bottom;
			}

			BeginAnimation(&st);
		}
	}

	if(m_bDrag)
	{
		if( uMsg == WM_LBUTTONUP )
		{
			//ReleaseCapture();
			m_bDrag = false;
			//m_bMinimized = !m_bMinimized;
			return true;
		}
		else if( uMsg == WM_MOUSEMOVE)
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			int old_x = m_x;
			int old_y = m_y;
			m_x = mousePoint.x + ptDrag.x;
			m_y = mousePoint.y + ptDrag.y;
			if(old_x != m_x || old_y != m_y)
			{
				m_dwTimeAnchor = -1;
				m_bAnchor = false;
				if(GetStyle() & GUI_WS_AUTOANCHOR)
				{
					if ( 1 )			//Autosnap
					{
						const int mindis = 20;
						// ◊‘∂ØÕ£øøµƒ¥∞ø⁄
						stRectI clipRect = GetClipRect();
						if( m_x != old_x)
						{
							if( abs( m_x + clipRect.left ) < mindis )
							{
								m_x = -clipRect.left;
								m_dwTimeAnchor = xtimeGetTime();
							}
							else
							{
								int right = m_x + clipRect.right;
								if( abs( right - GetDevice()->GetWidth() ) < mindis) 
								{
									right = GetDevice()->GetWidth();
									m_x = right - clipRect.right;
									m_dwTimeAnchor = xtimeGetTime();
								}
							}
						}

						if( m_y != old_y)
						{
							if( abs( m_y + clipRect.top ) < mindis )
							{
								m_y = -clipRect.top;
								m_dwTimeAnchor = xtimeGetTime();
							}
							else
							{
								int bottom = m_y + clipRect.bottom;
								if( abs( bottom - GetDevice()->GetHeight() ) < mindis) 
								{
									bottom = GetDevice()->GetHeight() ;
									m_y = bottom - clipRect.bottom;
									m_dwTimeAnchor = xtimeGetTime();
								}
							}
						}
					}

				}

				SendEvent(EVENT_DIALOG_MOVE,true,NULL);
			}
		}
		return true;
	}
	else if(m_bSizing)
	{
		if( uMsg == WM_LBUTTONUP )
		{
			m_bSizing = false;
			return true;
		}
		else if( uMsg == WM_MOUSEMOVE )
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			stPointI pt(mousePoint.x + ptSizing.x,mousePoint.y + ptSizing.y);
			if(s_eSizingType & eSizingType_Top)
			{
				int bottom = m_y + m_height;
				m_y = pt.y;
				m_height = bottom - m_y;
				if(m_height < m_minSize.y)
				{	
					m_y = bottom - m_minSize.y;
					m_height = m_minSize.y;
				}
				else if(m_height > m_maxSize.y)
				{
					m_y = bottom - m_maxSize.y;
					m_height = m_maxSize.y;
				}
			}
			else if(s_eSizingType & eSizingType_Bottom)
			{
				m_height = pt.y - m_y;
				if(m_height < m_minSize.y)
				{
					m_height = m_minSize.y;
				}
				else if(m_height > m_maxSize.y)
				{
					m_height = m_maxSize.y;
				}
			}

			if(s_eSizingType & eSizingType_Left)
			{
				int right = m_x + m_width;
				m_x = pt.x;
				m_width = right - m_x;
				if(m_width < m_minSize.x)
				{	
					m_x = right - m_minSize.x;
					m_width = m_minSize.x;
				}
				else if(m_width > m_maxSize.x)
				{
					m_x = right - m_maxSize.x;
					m_width = m_maxSize.x;
				}
			}
			else if(s_eSizingType & eSizingType_Right)
			{
				m_width = pt.x - m_x;
				if(m_width < m_minSize.x)
				{
					m_width = m_minSize.x;
				}
				else if(m_width > m_maxSize.x)
				{
					m_width = m_maxSize.x;
				}
			}
			SendEvent(EVENT_DIALOG_RESIZE,true,NULL);
			UpdateLocation();
		}
		return true;
	}
	else
	{
		if( uMsg == WM_LBUTTONDOWN)
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			DWORD dwHiTest = OnHiTest(mousePoint);
			if(dwHiTest != HTERROR && dwHiTest != HTTRANSPARENT)
			{
				if(GetGuiManager()->CanHaveFocus(this))
					GetGuiManager()->RequestFocus(this);
				bHandled2 = true;
			}
		}
	}

	// If the dialog is minimized, don't send any messages to controls.
	if( m_bMinimized )
		return bHandled;

	// If a control is in focus, it belongs to this dialog, and it's enabled, then give
	// it the first chance at handling the message.
	if( s_pControlFocus && 
		s_pControlFocus->m_pDialog == this && 
		s_pControlFocus->IsEnabled() )
	{
		// If the control MsgProc handles it, then we don't.
		if( s_pControlFocus->MsgProc( uMsg, wParam, lParam ) )
			return true;
	}

	switch( uMsg )
	{
	case WM_ACTIVATEAPP:
		// Call OnFocusIn()/OnFocusOut() of the control that currently has the focus
		// as the application is activated/deactivated.  This matches the Windows
		// behavior.
		if( s_pControlFocus && 
			s_pControlFocus->m_pDialog == this && 
			s_pControlFocus->IsEnabled() )
		{

#ifndef _DEBUG
			if( wParam )
				s_pControlFocus->OnFocusIn();
			else
				s_pControlFocus->OnFocusOut();
#endif

		}
		break;
	case WM_CHAR:
		if(s_bTransactKeyDown)
			return true;
		/*for( int i=0; i < m_Controls.size(); i++ )
		{
		CGuiControl* pControl = m_Controls[ i ];
		if(!pControl->m_bDeleted && pControl->GetHotkey().IsKeyDown() )
		{
		pControl->OnHotkey();
		return true;
		}
		}*/
		break;
		// Keyboard messages
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{

			if(uMsg == WM_KEYUP)
			{
				s_bTransactKeyDown = false;
				return true;
			}
#ifdef _DEBUG
			if(uMsg == WM_KEYDOWN)
			{
				//TRACE("WM_KEYDOWN wParam:%u\n",wParam);
				if(wParam == 83 && GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_CONTROL) < 0)
				{
					// Ctrl + Shift + S
					char szFileName[MAX_PATH]="";
					char szFileTitle[MAX_PATH]="";
					OPENFILENAME fn;
					memset(&fn,0,sizeof(OPENFILENAME));
					fn.lStructSize = sizeof(OPENFILENAME);
					fn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER;
					fn.lpstrDefExt = "xml";
					fn.lpstrFilter = "*.xml\0*.xml";
					fn.lpstrFile = szFileName;
					fn.nMaxFile = MAX_PATH;
					fn.lpstrTitle = "«Î—°‘ÒŒƒº˛√˚";
					fn.hwndOwner = Engine_GetHWnd();
					fn.hInstance = ::GetModuleHandle(NULL);
					fn.lpstrFileTitle = szFileTitle;
					fn.nMaxFileTitle = MAX_PATH;
					if(GetSaveFileName(&fn))
					{
						SaveGuiToXML(szFileName);
					}
					else
					{
						DWORD dw = CommDlgExtendedError();
						TRACE("GetSaveFileName Error : %u\n",dw);
					}
					SetCurrentDirectory(Engine_GetAppation()->GetAppPath());
				}
				if(wParam == 75 && GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_CONTROL) < 0)
				{
					// ±£¥Ê
					// Ctrl + Shift + K
					char szFileName[MAX_PATH]="";
					char szFileTitle[MAX_PATH]="";
					OPENFILENAME fn;
					memset(&fn,0,sizeof(OPENFILENAME));
					fn.lStructSize = sizeof(OPENFILENAME);
					fn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER;
					fn.lpstrDefExt = "xml";
					fn.lpstrFilter = "*.xml\0*.xml";
					fn.lpstrFile = szFileName;
					fn.nMaxFile = MAX_PATH;
					fn.lpstrTitle = "«Î—°‘Ò±£¥Êµƒ∆§∑ÙŒƒº˛√˚";
					fn.hwndOwner = Engine_GetHWnd();
					fn.hInstance = ::GetModuleHandle(NULL);
					fn.lpstrFileTitle = szFileTitle;
					fn.nMaxFileTitle = MAX_PATH;
					if(GetSaveFileName(&fn))
					{
						SaveDefaultElementToXML(szFileName);
					}
					else
					{
						DWORD dw = CommDlgExtendedError();
						TRACE("GetSaveFileName Error : %u\n",dw);
					}
					SetCurrentDirectory(Engine_GetAppation()->GetAppPath());
				}
			}
#endif
			// If a control is in focus, it belongs to this dialog, and it's enabled, then give
			// it the first chance at handling the message.
			if( s_pControlFocus && 
				s_pControlFocus->m_pDialog == this && 
				s_pControlFocus->IsEnabled() )
			{
				if( s_pControlFocus->HandleKeyboard( uMsg, wParam, lParam ) )
					return true;
			}

			// Not yet handled, see if this matches a control's hotkey
			// Activate the hotkey if the focus doesn't belong to an
			// edit box.
			{
				for( int i=0; i < m_Controls.size(); i++ )
				{
					CGuiControl* pControl = m_Controls[ i ];
					if( !pControl->m_bDeleted && pControl->GetHotkey().IsKeyDown() )
					{
						//if(uMsg != WM_KEYDOWN)
						//	pControl->OnHotkey();

						//if( wParam == VK_RETURN /*|| wParam == VK_ESCAPE*/ )
						if(uMsg == WM_KEYDOWN)
						{
							pControl->OnHotkey();
							s_bTransactKeyDown = true;
						}

						return true;
					}
				}
			}

			// Not yet handled, check for focus messages
			if( uMsg == WM_KEYDOWN )
			{
				// If keyboard input is not enabled, this message should be ignored
				if( !m_bKeyboardInput )
					return bHandled;

				switch( wParam )
				{
				case VK_RIGHT:
				case VK_DOWN:
					if( s_pControlFocus != NULL )
					{
						OnCycleFocus( true );
						return true;
					}
					break;

				case VK_LEFT:
				case VK_UP:
					if( s_pControlFocus != NULL )
					{
						OnCycleFocus( false );
						return true;
					}
					break;

				case VK_TAB: 
					if( s_pControlFocus == NULL )
					{
						return FocusDefaultControl();
					}
					else
					{
						bool bShiftDown = ((GetAsyncKeyState( VK_SHIFT ) & 0x8000) != 0);
						return OnCycleFocus( !bShiftDown );
					}
				}
			}

			break;
		}


		// Mouse messages
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		{

			// If not accepting mouse input, return false to indicate the message should still 
			// be handled by the application (usually to move the camera).
			if( !m_bMouseInput )
				return bHandled;

			if( !Engine_GetCursor()->IsNormal() && !Engine_GetCursor()->IsDrag() )
				break;

			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			mousePoint.x -= m_x + m_clientOffsetTopLeft.x;
			mousePoint.y -= m_y + m_clientOffsetTopLeft.y;

			// If caption is enabled, offset the Y coordinate by the negative of its height.
			if( m_bCaption )
				mousePoint.y -= m_nCaptionHeight;

			bool bHandleDisable = false;
#ifdef _DEBUG
			if(GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_CONTROL) < 0)
				bHandleDisable = true;
#endif
			if((uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK) && (!s_pControlFocus || 
				s_pControlFocus->m_pDialog != this || s_pControlFocus->GetType() != GUI_CONTROL_COMBOBOX) )
			{
				CGuiControl* pControl = GetControlAtPoint( mousePoint );
				if( pControl != NULL && (bHandleDisable || pControl->IsEnabled()) )
				{
					bool b = pControl->HandleMouse( uMsg, mousePoint, wParam, lParam );
					if( b )
					{
						return true;
					}
				}
			}
			// If a control is in focus, it belongs to this dialog, and it's enabled, then give
			// it the first chance at handling the message.
			if( s_pControlFocus && 
				s_pControlFocus->m_pDialog == this && 
				(s_pControlFocus->IsEnabled() || bHandleDisable))
			{
				if( s_pControlFocus->HandleMouse( uMsg, mousePoint, wParam, lParam ) )
					return true;
			}

			// Not yet handled, see if the mouse is over any controls
			CGuiControl* pControl = GetControlAtPoint( mousePoint );
			if( pControl != NULL /*&& (bHandleDisable || pControl->IsEnabled())*/ )
			{
				bool b = pControl->HandleMouse( uMsg, mousePoint, wParam, lParam );
				if( b )
					return true;
			}
			else
			{
				// Mouse not over any controls in this dialog, if there was a control
				// which had focus it just lost it
				if( uMsg == WM_LBUTTONDOWN && 
					s_pControlFocus && 
					s_pControlFocus->GetType() != GUI_CONTROL_EDITBOX && 
					s_pControlFocus->GetType() != GUI_CONTROL_MLEDITBOX &&
					s_pControlFocus->m_pDialog == this )
				{
					s_pControlFocus->OnFocusOut();
					s_pControlFocus = NULL;
				}
			}

			// Still not handled, hand this off to the dialog. Return false to indicate the
			// message should still be handled by the application (usually to move the camera).
			switch( uMsg )
			{
			case WM_MOUSEMOVE:
				OnMouseMove( mousePoint );
				return bHandled;
				break;
			case WM_RBUTTONUP:
				if(GetGuiManager()->GetMouseOverDlg() == this && OnContextMenu())
					bHandled =  true;
				break;
			}

			break;
		}
	}
	if(!bHandled)
	{
		if( uMsg == WM_LBUTTONDOWN /*|| uMsg == WM_LBUTTONDBLCLK*/  )
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			DWORD dwHiTest = OnHiTest(mousePoint);
			if(dwHiTest == HTCAPTION)
			{
				if(GetGuiManager()->CanHaveFocus(this))
				{
					if(0 ==(GetStyle() & GUI_WS_NOMOVE))
					{
						m_bDrag = true;
						//SetCapture( Engine_GetHwnd() );
						ptDrag.x = m_x - mousePoint.x;
						ptDrag.y = m_y - mousePoint.y;
						return true;
					}
					return false;
				}
			}
			enumSizingType sizingType = HiTestCodeToSizingType(dwHiTest);
			if(sizingType != eSizingType_None)
			{
				s_eSizingType = sizingType;
				if(GetGuiManager()->CanHaveFocus(this))
				{
					m_bSizing = true;
					stPointI pt2(0,0);
					if(sizingType & eSizingType_Left)
					{
						pt2.x = m_x;
					}
					else if(sizingType & eSizingType_Right)
					{
						pt2.x = m_x + m_width;
					}

					if(sizingType & eSizingType_Top)
					{
						pt2.y = m_y;
					}
					else if(sizingType & eSizingType_Bottom)
					{
						pt2.y = m_y + m_height;
					}
					ptSizing.x = pt2.x - mousePoint.x;
					ptSizing.y = pt2.y - mousePoint.y;
					return true;
				}
			}
		}
	}
	return bHandled ;//|| bHandled2;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pt : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
CGuiControl* CGuiDialog::GetControlAtPoint( POINT pt )
{
	// Search through all child controls for the first one which
	// contains the mouse point
	for( int i=m_Controls.size()-1; i >=0; i-- )
	{
		CGuiControl* pControl = m_Controls[ i ];

		if( pControl == NULL || pControl->m_bDeleted)
		{
			continue;
		}

		// We only return the current control if it is visible
		// and enabled.  Because GetControlAtPoint() is used to do mouse
		// hittest, it makes sense to perform this filtering.
		bool bHandleDisable = true;
#ifdef _DEBUG
		if(GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_CONTROL) < 0)
			bHandleDisable = true;
#endif
		if( pControl->ContainsPoint( pt ) && ( bHandleDisable || pControl->IsEnabled()) && pControl->IsVisible() )
		{
			return pControl;
		}
	}

	return NULL;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
bool CGuiDialog::GetControlEnabled( int ID )
{
	CGuiControl* pControl = GetControl( ID );
	if( pControl == NULL )
		return false;

	return pControl->IsEnabled();
}



/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param bEnabled : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::SetControlEnabled( int ID, bool bEnabled )
{
	CGuiControl* pControl = GetControl( ID );
	if( pControl == NULL )
		return;

	pControl->SetEnabled( bEnabled );
}

void CGuiDialog::SetControlEnabled(GUI_CONTROL_TYPE nControlType, bool bEnabled)
{
	// Try to find the control with the given ID
	for (size_t i = 0; i < m_Controls.size(); i++)
	{
		CGuiControl* pControl = m_Controls[i];
		if (!pControl->m_bDeleted && pControl->GetType() == nControlType)
		{
			pControl->SetEnabled(bEnabled);
		}
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pt : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::OnMouseUp( POINT pt )
{
	s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pt : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::OnMouseMove( POINT pt )
{
	// Figure out which control the mouse is over now
	CGuiControl* pControl = GetControlAtPoint( pt );

	// If the mouse is still over the same control, nothing needs to be done
	if( pControl == m_pControlMouseOver )
		return;

	// Handle mouse leaving the old control
	if( m_pControlMouseOver )
		m_pControlMouseOver->OnMouseLeave();

	// Handle mouse entering the new control
	m_pControlMouseOver = pControl;
	if( pControl != NULL )
		m_pControlMouseOver->OnMouseEnter();
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nControlType : √Ë ˆ
* \param iElement : √Ë ˆ
* \param pElement : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::SetCustomElement( UINT nControlType, UINT iElement, CGuiElement* pElement )
{
	// If this Element type already exist in the list, simply update the stored Element
	for( int i=0; i < m_DefaultElements.size(); i++ )
	{
		GuiElementHolder* pElementHolder = m_DefaultElements[ i ];

		if( pElementHolder->nControlType == nControlType &&
			pElementHolder->iElement == iElement )
		{
			pElementHolder->Element = *pElement;
			return S_OK;
		}
	}

	// Otherwise, add a new entry
	GuiElementHolder* pNewHolder;
	pNewHolder = new GuiElementHolder;
	if( pNewHolder == NULL )
		return E_OUTOFMEMORY;

	pNewHolder->nControlType = nControlType;
	pNewHolder->iElement = iElement;
	pNewHolder->Element = *pElement;

	m_DefaultElements.push_back( pNewHolder );
	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nControlType : √Ë ˆ
* \param iElement : √Ë ˆ
* \param pElement : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::SetDefaultElement( UINT nControlType, UINT iElement, CGuiElement* pElement )
{
	// If this Element type already exist in the list, simply update the stored Element
	for( int i=0; i < s_DefaultElements.size(); i++ )
	{
		GuiElementHolder* pElementHolder = s_DefaultElements[ i ];

		if( pElementHolder->nControlType == nControlType &&
			pElementHolder->iElement == iElement )
		{
			pElementHolder->Element = *pElement;
			return S_OK;
		}
	}

	// Otherwise, add a new entry
	GuiElementHolder* pNewHolder;
	pNewHolder = new GuiElementHolder;
	if( pNewHolder == NULL )
		return E_OUTOFMEMORY;

	pNewHolder->nControlType = nControlType;
	pNewHolder->iElement = iElement;
	pNewHolder->Element = *pElement;

	s_DefaultElements.push_back( pNewHolder );
	return S_OK;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nControlType : √Ë ˆ
* \param iElement : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
CGuiElement* CGuiDialog::GetDefaultElement( UINT nControlType, UINT iElement )
{
	for( int i=0; i < m_DefaultElements.size(); i++ )
	{
		GuiElementHolder* pElementHolder = m_DefaultElements[ i ];

		if( pElementHolder->nControlType == nControlType &&
			pElementHolder->iElement == iElement )
		{
			return &pElementHolder->Element;
		}
	}

	for( int i=0; i < s_DefaultElements.size(); i++ )
	{
		GuiElementHolder* pElementHolder = s_DefaultElements[ i ];

		if( pElementHolder->nControlType == nControlType &&
			pElementHolder->iElement == iElement )
		{
			return &pElementHolder->Element;
		}
	}

	return NULL;
}



/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param strText : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param bIsDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::AddStatic( int ID, const char* strText, int x, int y, int width, int height, bool bIsDefault, CGuiStatic** ppCreated )
{
	HRESULT hr = S_OK;

	CGuiStatic* pStatic = new CGuiStatic( this );

	if( ppCreated != NULL )
		*ppCreated = pStatic;

	if( pStatic == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pStatic );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pStatic->SetID( ID ); 
	pStatic->SetText(strText ? strText : "");
	pStatic->SetLocation( x, y );
	pStatic->SetSize( width, height );
	pStatic->m_bIsDefault = bIsDefault;

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param rl : √Ë ˆ
* \param imgRect : √Ë ˆ
* \param color : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SetBackImage(const stResourceLocation& rl,const RECT * imgRect,DWORD color,int x,int y,bool bReAlign,int width,int height)
{
	if(!m_pBackImage)
	{
		CGuiImage::STYLE style = CGuiImage::FULLSIZE_IMAGE;
		if(imgRect && width > 0 && height > 0 && (width != (imgRect->right-imgRect->left) || height != (imgRect->bottom-imgRect->top)) )
		{
			style = CGuiImage::STRETCH_IMAGE;
		}
		AddImage(0,x,y,rl,imgRect,color,width,height,style,&m_pBackImage);
	}
	else
	{
		m_pBackImage->SetLocation(x,y);
		m_pBackImage->SetImage(rl,imgRect,color);
	}
	m_pBackImage->m_bIsBack = true;
	m_pBackImage->SetZOrder(GUI_CONTROL_BOTTOM);
	m_width = m_pBackImage->GetX() + m_pBackImage->GetWidth();
	m_height = m_pBackImage->GetY() + m_pBackImage->GetHeight();
	m_pBackImage->SetColor(ColorBlend(color,GetColor()));
	UpdateLocation(bReAlign);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::AddItemCell(int ID,int x,int y,CGuiItemCell** ppCreated)
{
	HRESULT hr = S_OK;
	CGuiItemCell* pImage = new CGuiItemCell( this );
	if(ppCreated != NULL)
		*ppCreated = pImage;

	if(pImage == NULL)
		return E_OUTOFMEMORY;

	hr = AddControl(pImage);
	if(FAILED(hr)) 
		return hr;
	pImage->SetID( ID );
	pImage->SetLocation(x,y);
	pImage->SetSize(0,0);
	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param cellWidth : √Ë ˆ
* \param cellHeight : √Ë ˆ
* \param colCount : √Ë ˆ
* \param rowCount : √Ë ˆ
* \param showRowCount : √Ë ˆ
* \param bVirtual : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::AddTable(int ID,int x,int y,int cellWidth,int cellHeight,int colCount,int rowCount,int showRowCount,bool bVirtual ,CGuiTable** ppCreated)
{
	HRESULT hr = S_OK;
	CGuiTable* pImage = new CGuiTable( this );

	if(ppCreated != NULL)
		*ppCreated = pImage;

	if(pImage == NULL)
		return E_OUTOFMEMORY;
	pImage->m_bVirtual = bVirtual;
	pImage->m_cellWidth = cellWidth;
	pImage->m_cellHeight = cellHeight;
	pImage->m_rowCount = rowCount;
	pImage->m_colCount = colCount;
	pImage->m_showRowCount = showRowCount;
	pImage->SetID( ID );
	hr = AddControl(pImage);
	pImage->SetLocation(x,y);

	//pImage->SetSize(colCount * cellWidth,showRowCount*cellHeight);

	if(FAILED(hr)) 
		return hr;

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param rl : √Ë ˆ
* \param imgRect : √Ë ˆ
* \param dwColor : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param dwStyle : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::AddImage(int ID,int x,int y,const stResourceLocation & rl,const RECT * imgRect,DWORD dwColor,int width,int height,DWORD dwStyle,CGuiImage ** ppCreated)
{
	HRESULT hr = S_OK;
	CGuiImage* pImage = new CGuiImage( this );

	if(ppCreated != NULL)
		*ppCreated = pImage;

	if(pImage == NULL)
		return E_OUTOFMEMORY;

	hr = AddControl(pImage);
	if(FAILED(hr)) 
		return hr;
	pImage->SetID( ID );
	pImage->SetLocation(x,y);
	pImage->SetSize(width,height);
	pImage->SetStyle(dwStyle);
	return pImage->SetImage(rl,imgRect,dwColor);
}
//--------------------------------------------------------------------------------------

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param strText : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param nHotkey : √Ë ˆ
* \param bIsDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::AddButton( int ID, const char* strText, int x, int y, int width, int height, const char* nHotkey, bool bIsDefault, CGuiButton** ppCreated )
{
	HRESULT hr = S_OK;

	CGuiButton* pButton = new CGuiButton( this );

	if( ppCreated != NULL )
		*ppCreated = pButton;

	if( pButton == NULL )
		return E_OUTOFMEMORY;

	pButton->SetText(strText ? strText : "");
	hr = AddControl( pButton );
	if( FAILED(hr) )
		return hr;
	//pButton->SetTextColor(COLOR_ARGB(255,204,165,10));

	// Set the ID and list index
	pButton->SetID( ID ); 

	pButton->SetLocation( x, y );
	pButton->SetSize( width, height );
	if(nHotkey)
		pButton->SetHotkey( nHotkey );
	pButton->m_bIsDefault = bIsDefault;

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param style : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::AddTab( int ID,int x,int y,int width,int height,DWORD style,CGuiTab** ppCreated)
{
	HRESULT hr = S_OK;
	CGuiTab* pTab = new CGuiTab(this);
	if(ppCreated != NULL)
		*ppCreated = pTab;

	if(pTab == NULL) return E_OUTOFMEMORY;

	hr = AddControl(pTab);
	if(FAILED(hr))
		return hr;

	pTab->SetStyle(style);
	pTab->SetID(ID);
	pTab->SetLocation(x,y);
	pTab->SetSize(width,height);

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param isDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::AddScrollTab(int ID,int x,int y,int width,int height,bool isDefault,CGuiScrollTab** ppCreated )
{
	HRESULT hr = S_OK;

	CGuiScrollTab* pScrollTab = new CGuiScrollTab( this );

	if( ppCreated != NULL )
		*ppCreated = pScrollTab;

	if( pScrollTab == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pScrollTab );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pScrollTab->SetID( ID ); 
	pScrollTab->SetLocation( x, y );
	pScrollTab->SetSize( width, height );


	pScrollTab->m_bIsDefault = isDefault;

	return S_OK;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param nMin : √Ë ˆ
* \param nMax : √Ë ˆ
* \param nPos : √Ë ˆ
* \param isDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::AddProcess(int ID,int x,int y,int width,int height,int nMin,int nMax,int nPos,bool isDefault,CGuiProcess** ppCreated )
{
	HRESULT hr = S_OK;

	CGuiProcess* pProcess = new CGuiProcess( this );

	if( ppCreated != NULL )
		*ppCreated = pProcess;

	if( pProcess == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pProcess );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pProcess->SetID( ID ); 
	pProcess->SetLocation( x, y );
	pProcess->SetSize( width, height );
	pProcess->SetRange(nMin,nMax);
	pProcess->SetPos(nPos);

	pProcess->m_bIsDefault = isDefault;

	/**
	* \brief ºÚ∂Ã√Ë ˆ
	* 
	* œÍœ∏√Ë ˆ
	* 
	* \param ID : √Ë ˆ
	* \param strText : √Ë ˆ
	* \param x : √Ë ˆ
	* \param y : √Ë ˆ
	* \param width : √Ë ˆ
	* \param height : √Ë ˆ
	* \param bChecked : √Ë ˆ
	* \param nHotkey : √Ë ˆ
	* \param bIsDefault : √Ë ˆ
	* \param ppCreated : √Ë ˆ
	* \return ∑µªÿ÷µµƒ√Ë ˆ
	*/
	return S_OK;
}
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::AddCheckBox( int ID, const char* strText, int x, int y, int width, int height, bool bChecked, const char* nHotkey, bool bIsDefault, CGuiCheckBox** ppCreated )
{
	HRESULT hr = S_OK;

	CGuiCheckBox* pCheckBox = new CGuiCheckBox( this );

	if( ppCreated != NULL )
		*ppCreated = pCheckBox;

	if( pCheckBox == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pCheckBox );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pCheckBox->SetID( ID ); 
	pCheckBox->SetText( strText );
	pCheckBox->SetLocation( x, y );
	pCheckBox->SetSize( width, height );
	if(nHotkey)
		pCheckBox->SetHotkey( nHotkey );
	pCheckBox->m_bIsDefault = bIsDefault;
	pCheckBox->SetChecked( bChecked );

	return S_OK;
}



/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param nButtonGroup : √Ë ˆ
* \param strText : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param bChecked : √Ë ˆ
* \param nHotkey : √Ë ˆ
* \param bIsDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::AddRadioButton( int ID, UINT nButtonGroup, const char* strText, int x, int y, int width, int height, bool bChecked, const char* nHotkey, bool bIsDefault, CGuiRadioButton** ppCreated )
{
	HRESULT hr = S_OK;

	CGuiRadioButton* pRadioButton = new CGuiRadioButton( this );

	if( ppCreated != NULL )
		*ppCreated = pRadioButton;

	if( pRadioButton == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pRadioButton );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pRadioButton->SetID( ID ); 
	pRadioButton->SetText( strText );
	pRadioButton->SetButtonGroup( nButtonGroup );
	pRadioButton->SetLocation( x, y );
	pRadioButton->SetSize( width, height );
	if(nHotkey)
		pRadioButton->SetHotkey( nHotkey );
	pRadioButton->SetChecked( bChecked,true );
	pRadioButton->m_bIsDefault = bIsDefault;
	pRadioButton->SetChecked( bChecked ,true);

	return S_OK;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param nHotkey : √Ë ˆ
* \param bIsDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::AddComboBox( int ID, int x, int y, int width, int height, const char* nHotkey, bool bIsDefault, CGuiComboBox** ppCreated )
{
	HRESULT hr = S_OK;

	CGuiComboBox* pComboBox = new CGuiComboBox( this );

	if( ppCreated != NULL )
		*ppCreated = pComboBox;

	if( pComboBox == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pComboBox );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pComboBox->SetID( ID ); 
	pComboBox->SetLocation( x, y );
	pComboBox->SetSize( width, height );
	if(nHotkey)
		pComboBox->SetHotkey( nHotkey );
	pComboBox->m_bIsDefault = bIsDefault;

	return S_OK;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param min : √Ë ˆ
* \param max : √Ë ˆ
* \param pagesize : √Ë ˆ
* \param bIsDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::AddScrollBar( int ID, int x, int y, int width, int height, int min, int max, int pagesize, bool bIsDefault, CGuiScrollBar** ppCreated ,int nStyle)
{
	HRESULT hr = S_OK;

	CGuiScrollBar* pScrollBar = new CGuiScrollBar( this );

	if( ppCreated != NULL )
		*ppCreated = pScrollBar;

	if( pScrollBar == NULL )
		return E_OUTOFMEMORY;

	
	hr = AddControl( pScrollBar );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index

	pScrollBar->SetID( ID ); 
	pScrollBar->SetLocation( x, y );
	pScrollBar->SetSize( width, height );
	pScrollBar->m_bIsDefault = bIsDefault;
	pScrollBar->SetTrackRange(min,max);
	pScrollBar->SetTrackPos( min );
	pScrollBar->SetPageSize(pagesize);

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param min : √Ë ˆ
* \param max : √Ë ˆ
* \param value : √Ë ˆ
* \param bIsDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::AddSlider( int ID, int x, int y, int width, int height, int min, int max, int value, bool bIsDefault, CGuiSlider** ppCreated )
{
	HRESULT hr = S_OK;

	CGuiSlider* pSlider = new CGuiSlider( this );

	if( ppCreated != NULL )
		*ppCreated = pSlider;

	if( pSlider == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pSlider );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pSlider->SetID( ID ); 
	pSlider->SetLocation( x, y );
	pSlider->SetSize( width, height );
	pSlider->m_bIsDefault = bIsDefault;
	pSlider->SetRange( min, max );
	pSlider->SetValue( value );

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param strText : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param dwStyle : √Ë ˆ
* \param isDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::AddMLTextBox(int ID,const char* strText,int x,int y,int width,int height,DWORD dwStyle,bool isDefault,CGuiMLTextBox** ppCreated)
{
	HRESULT hr = S_OK;
	CGuiMLTextBox * pMLTextBox = new CGuiMLTextBox(this,dwStyle);
	if(ppCreated ) *ppCreated = pMLTextBox;

	if(!pMLTextBox) return E_OUTOFMEMORY;

	hr = AddControl(pMLTextBox);
	if(FAILED(hr)) return hr;

	pMLTextBox->SetID(ID);
	pMLTextBox->SetLocation(x,y);
	pMLTextBox->SetSize(width,height);
	pMLTextBox->m_bIsDefault = isDefault;

	if(strText)
	{
		pMLTextBox->SetText(strText);
	}

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param strText : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param dwStyle : √Ë ˆ
* \param isDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::AddMLEditBox(int ID,const char* strText,int x,int y,int width,int height,DWORD dwStyle,bool isDefault,CGuiMLEditBox** ppCreated)
{
	HRESULT hr = S_OK;
	CGuiMLEditBox * pMLEditBox = new CGuiMLEditBox(this,dwStyle);
	if(ppCreated ) *ppCreated = pMLEditBox;

	if(!pMLEditBox) return E_OUTOFMEMORY;

	hr = AddControl(pMLEditBox);
	if(FAILED(hr)) return hr;

	pMLEditBox->SetID(ID);
	pMLEditBox->SetLocation(x,y);
	pMLEditBox->SetSize(width,height);
	pMLEditBox->m_bIsDefault = isDefault;

	if(strText)
	{
		pMLEditBox->SetText(strText);
	}

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param strText : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param bIsDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::AddEditBox( int ID, const char* strText, int x, int y, int width, int height, bool bIsDefault, CGuiEditBox** ppCreated )
{
	HRESULT hr = S_OK;

	CGuiEditBox *pEditBox = new CGuiEditBox( this );

	if( ppCreated != NULL )
		*ppCreated = pEditBox;

	if( pEditBox == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pEditBox );
	if( FAILED(hr) )
		return hr;

	// Set the ID and position
	pEditBox->SetID( ID ); 
	pEditBox->SetLocation( x, y );
	pEditBox->SetSize( width, height );
	pEditBox->m_bIsDefault = bIsDefault;

	if( strText )
		pEditBox->SetText( strText );
	if( CGuiDialog::s_pControlFocus == NULL )
		this->SetFocus(pEditBox);
	return S_OK;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param strText : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param bIsDefault : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
/*
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::AddIMEEditBox( int ID, const char* strText, int x, int y, int width, int height, bool bIsDefault, CGuiImeEditBox** ppCreated )
{
HRESULT hr = S_OK;
CGuiImeEditBox *pEditBox = new CGuiImeEditBox( this );

if( ppCreated != NULL )
*ppCreated = pEditBox;

if( pEditBox == NULL )
return E_OUTOFMEMORY;

hr = AddControl( pEditBox );
if( FAILED(hr) )
return hr;

// Set the ID and position
pEditBox->SetID( ID ); 
pEditBox->SetLocation( x, y );
pEditBox->SetCount( width, height );
pEditBox->m_bIsDefault = bIsDefault;

if( strText )
pEditBox->SetText( strText );

return S_OK;
}
//*/


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param dwStyle : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::AddListBox( int ID, int x, int y, int width, int height, DWORD dwStyle, CGuiListBox** ppCreated )
{
	HRESULT hr = S_OK;
	CGuiListBox *pListBox = new CGuiListBox( this );

	if( ppCreated != NULL )
		*ppCreated = pListBox;

	if( pListBox == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pListBox );
	if( FAILED(hr) )
		return hr;

	// Set the ID and position
	pListBox->SetID( ID );
	pListBox->SetLocation( x, y );
	pListBox->SetSize( width, height );
	pListBox->SetStyle( dwStyle );

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \param dwStyle : √Ë ˆ
* \param ppCreated : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::AddTree( int ID, int x, int y, int width, int height, DWORD dwStyle, CGuiTree** ppCreated )
{
	HRESULT hr = S_OK;
	CGuiTree *pTree = new CGuiTree( this );

	if( ppCreated != NULL )
		*ppCreated = pTree;

	if( pTree == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pTree );
	if( FAILED(hr) )
		return hr;

	// Set the ID and position
	pTree->SetID( ID );
	pTree->SetLocation( x, y );
	pTree->SetSize( width, height );
	pTree->SetStyle( dwStyle );

	return S_OK;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::OnCreate()
{
	if(m_bSmDialog)
	{
		m_ptBackGroundOffsetTopLeft = stPointI(13,13);
		m_ptBackGroundOffsetRightBottom = stPointI(13,13);
	}

	if( 0 == (GetStyle() & GUI_WS_NOSKIN) )
	{
	}

	if( GetStyle() & GUI_WS_CLOSEBOX )
	{
		// ”–πÿ±’∞¥≈•
		Assert(m_pBtnClose == NULL);
		AddButton(GUI_CLOSEBOX_ID,NULL,0,0,25,25,0,false,&m_pBtnClose);
		m_pBtnClose->SetButtonType(CGuiButton::ButtonType_CloseBox);
		m_pBtnClose->SetHotkey("ESC");
		InitControl(m_pBtnClose,GUI_CONTROL_CLOSEBOX);
	}
	UpdateLocation();
	SendEvent(EVENT_DIALOG_CREATE,true,NULL);
	if(m_dwStyle & GUI_WS_RESIZEING)
	{
		SendEvent(EVENT_DIALOG_RESIZE,true,NULL);
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::InitControl( CGuiControl* pControl )
{
	if( pControl == NULL )
		return E_INVALIDARG;

	/**
	* \brief ºÚ∂Ã√Ë ˆ
	* 
	* œÍœ∏√Ë ˆ
	* 
	* \param pControl : √Ë ˆ
	* \param iControlType : √Ë ˆ
	* \return ∑µªÿ÷µµƒ√Ë ˆ
	*/
	return InitControl(pControl,pControl->GetType());
}
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::InitControl( CGuiControl* pControl ,int iControlType)
{
	if( pControl == NULL )
		return E_INVALIDARG;

	int i0 = 0;
	int i1 = 100;
	if(iControlType == GUI_CONTROL_BUTTON)
	{
		if(strlen(pControl->GetText()) > 4)
			i0 = 3;
		else i1 = 3;
	}
	if( iControlType == GUI_CONTROL_TABLE)
	{
		if(((CGuiTable*)pControl)->m_bVirtual)
			return S_OK;
	}
	// Look for a default Element entries
	for(int i= i0;i < i1;i++)
	{
		CGuiElement* pElement = GetDefaultElement(iControlType,i);
		if(!pElement) break;
		pControl->SetElement( i - i0, pElement );
	}

	return ( pControl->OnInit() );
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::AddControl( CGuiControl* pControl )
{
	HRESULT hr = S_OK;

	hr = InitControl( pControl );
	if( FAILED(hr) )
		return DXTRACE_ERR( "CGuiDialog::InitControl", hr );

	// Add to the list
	m_Controls.push_back( pControl );

	return S_OK;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
CGuiControl* CGuiDialog::GetControl( int ID )
{
	// Try to find the control with the given ID
	for( size_t i=0; i < m_Controls.size(); i++ )
	{
		CGuiControl* pControl = m_Controls[ i ];

		if( !pControl->m_bDeleted && pControl->GetID() == ID )
		{
			return pControl;
		}
	}

	// Not found
	return NULL;
}



/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \param nControlType : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
CGuiControl* CGuiDialog::GetControl( int ID, UINT nControlType )
{
	// Try to find the control with the given ID
	for( size_t i=0; i < m_Controls.size(); i++ )
	{
		CGuiControl* pControl = m_Controls[ i ];
		if( !pControl->m_bDeleted && pControl->GetID() == ID && pControl->GetType() == nControlType )
		{
			return pControl;
		}
	}

	// Not found
	return NULL;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
CGuiControl* CGuiDialog::GetNextControl( CGuiControl* pControl )
{
	int index = ControlToIndex(pControl);

	// Cycle through dialogs in the loop to find the next control. Note
	// that if only one control exists in all looped dialogs it will
	// be the returned 'next' control.
	int oldIx = index;
	for(;;)
	{
		index ++;
		if( index >= (int)m_Controls.size() )
		{
			index = 0;
		}
		if(m_Controls[index]->m_bDeleted)
		{
		}
		else
		{
			break;
		}
		if(index == oldIx) return NULL;
	}

	return m_Controls[ index ];
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
CGuiControl* CGuiDialog::GetPrevControl( CGuiControl* pControl )
{
	int index = ControlToIndex(pControl);

	// Cycle through dialogs in the loop to find the next control. Note
	// that if only one control exists in all looped dialogs it will
	// be the returned 'next' control.
	int oldIx = index;
	for(;;)
	{
		index --;

		if( index < 0 )
		{
			index = m_Controls.size()-1;
		}

		if(m_Controls[index]->m_bDeleted)
		{
		}
		else
		{
			break;
		}

		if(index == oldIx) return NULL;
	}

	return m_Controls[ index ];  
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nButtonGroup : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::ClearRadioButtonGroup( UINT nButtonGroup )
{
	// Find all radio buttons with the given group number
	for( size_t i=0; i < m_Controls.size(); i++ )
	{
		CGuiControl* pControl = m_Controls[ i ];

		if( !pControl->m_bDeleted && pControl->GetType() == GUI_CONTROL_RADIOBUTTON )
		{
			CGuiRadioButton* pRadioButton = (CGuiRadioButton*) pControl;

			if( pRadioButton->GetButtonGroup() == nButtonGroup )
				pRadioButton->SetChecked( false, false );
		}
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nButtonGroup : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::ClearButtonGroup( UINT nButtonGroup )
{
	// Find all radio buttons with the given group number
	for( size_t i=0; i < m_Controls.size(); i++ )
	{
		CGuiControl* pControl = m_Controls[ i ];

		if( !pControl->m_bDeleted && pControl->GetType() == GUI_CONTROL_BUTTON )
		{
			CGuiButton* pRadioButton = (CGuiButton*) pControl;

			if( pRadioButton->GetButtonGroup() == nButtonGroup )
				pRadioButton->SetCheckedInternal(false, false );
		}
	}
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ID : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::ClearComboBox( int ID )
{
	CGuiComboBox* pComboBox = GetComboBox( ID );
	if( pComboBox == NULL )
		return;

	pComboBox->RemoveAllItems();
}




/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::RequestFocus( CGuiControl* pControl )
{
	if(!GetGuiManager()->CanHaveFocus(this)) return;

	if( s_pControlFocus == pControl )
		return;

	if( !pControl->CanHaveFocus() )
		return;
	GetGuiManager()->RequestFocus(this);

	if( s_pControlFocus )
		s_pControlFocus->OnFocusOut();

	pControl->OnFocusIn();
	Assert(pControl->m_bDeleted == false);
	s_pControlFocus = pControl;
	if(pControl->CanHaveInputFocus())
		m_pLastFocus = pControl;
}

bool CGuiDialog::ResetFocus()
{
	if(!GetGuiManager()->CanHaveFocus(this)) return false;
	if(m_pLastFocus) 
	{
		if(s_pControlFocus)
			s_pControlFocus->OnFocusOut();
		s_pControlFocus = m_pLastFocus;
		s_pControlFocus->OnFocusIn();
		return true;
	}
	CGuiControl* pControl = NULL;
	for( int i=0; i < m_Controls.size();++i )
	{
		pControl = m_Controls[i];

		// If the dialog accepts keybord input and the control can have focus then
		// move focus
		if( !pControl->m_bDeleted && pControl->m_pDialog->m_bKeyboardInput && pControl->CanHaveInputFocus() )
		{
			if(s_pControlFocus)
				s_pControlFocus->OnFocusOut();
			s_pControlFocus = pControl;
			m_pLastFocus = pControl;
			s_pControlFocus->OnFocusIn();
			return true;
		}
	}
	return false;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pRect : √Ë ˆ
* \param color : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::DrawRectangle(const RECT* pRect, DWORD color)
{
	stRectI rcScreen = *pRect;
	rcScreen.OffsetRect( m_x, m_y );

	// If caption is enabled, offset the Y position by its height.
	rcScreen.OffsetRect( m_clientOffsetTopLeft.x,m_clientOffsetTopLeft.y );

	if(GetDevice()->DrawRectangle(&rcScreen,color))
		return S_OK;

	return S_FALSE;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pRect : √Ë ˆ
* \param color : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::FillRect(const RECT *pRect,DWORD color)
{
	stRectI rcScreen = *pRect;
	rcScreen.OffsetRect( m_x, m_y );
	rcScreen.OffsetRect( m_clientOffsetTopLeft.x,m_clientOffsetTopLeft.y );

	if(GetDevice()->FillRect(&rcScreen,color))
	{
		return S_OK;
	}

	return S_FALSE;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pRect : √Ë ˆ
* \param color : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::DrawRect( const RECT* pRect, DWORD color )
{
	stRectI rcScreen = *pRect;
	rcScreen.OffsetRect( m_x, m_y );

	// If caption is enabled, offset the Y position by its height.
	rcScreen.OffsetRect( m_clientOffsetTopLeft.x,m_clientOffsetTopLeft.y );
	if(GetDevice()->FillRect(&rcScreen,ColorBlend(m_dwColor,color)))
	{
		return S_OK;
	}
	return S_FALSE;

	return S_OK;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param apPoints : √Ë ˆ
* \param nNumPoints : √Ë ˆ
* \param color : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::DrawPolyLine( POINT* apPoints, UINT nNumPoints, DWORD color )
{
	if(GetDevice()->DrawPolyLine(apPoints,nNumPoints,color))
		return S_OK;
	return S_FALSE;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pElement : √Ë ˆ
* \param prcDest : √Ë ˆ
* \param dwAlignFormat : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
HRESULT CGuiDialog::PatchDrawSprite(CGuiElement* pElement, RECT* prcDest,DWORD dwAlignFormat)
{
	// No need to draw fully transparent layers
	if( pElement->TextureColor.Current.a < 1.0f/16.0f )
		return S_OK;

	if( pElement->pBmp == NULL)
		return S_OK;

	stRectI & rcTexture = pElement->rcTexture;

	stRectI rcScreen = *prcDest;
	rcScreen.OffsetRect(m_x,m_y);

	// If caption is enabled, offset the Y position by its height.
	rcScreen.OffsetRect( m_clientOffsetTopLeft.x,m_clientOffsetTopLeft.y );

	/*
	stPointF scale((float) rcScreen.Width() / rcTexture.Width(),(float) rcScreen.Height() / rcTexture.Height());
	//stPointF scale(1.0,1.0f);
	if(pElement->pBmp->Render(rcScreen.left,rcScreen.top,&rcTexture,&scale,ColorBlend(m_dwColor,(DWORD)pElement->TextureColor.Current)))
	return S_OK;
	//*/
	PatchRenderImage(rcScreen,pElement->pBmp,&rcTexture,ColorBlend(m_dwColor,(DWORD)pElement->TextureColor.Current),pElement->blend,dwAlignFormat);
	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pElement : √Ë ˆ
* \param prcDest : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::DrawSprite( CGuiElement* pElement, RECT* prcDest )
{
	// No need to draw fully transparent layers
	if( pElement->TextureColor.Current.a < 1.0f/16.0f )
		return S_OK;

	if( pElement->pBmp == NULL)
		return S_OK;

	stRectI & rcTexture = pElement->rcTexture;

	stRectI rcScreen = *prcDest;
	rcScreen.OffsetRect(m_x,m_y);

	// If caption is enabled, offset the Y position by its height.
	rcScreen.OffsetRect(m_clientOffsetTopLeft.x,m_clientOffsetTopLeft.y);

#ifdef _DEBUG
	GetDevice()->DrawRectangle(&rcScreen,COLOR_ARGB(255,0,0,250));
#endif

	if(rcScreen.Width() == rcTexture.Width() && rcScreen.Height() == rcTexture.Height())
	{
		
		
		if(pElement->pBmp->RenderAni(rcScreen.left,rcScreen.top,xtimeGetTime(),&rcTexture,NULL,ColorBlend(m_dwColor,(DWORD)pElement->TextureColor.Current),pElement->blend))
			return S_OK;
	}
	else
	{
		stPointF scale((float) rcScreen.Width() / rcTexture.Width(),(float) rcScreen.Height() / rcTexture.Height());
		if(pElement->pBmp->RenderAni(rcScreen.left,rcScreen.top,xtimeGetTime(),&rcTexture,&scale,ColorBlend(m_dwColor,(DWORD)pElement->TextureColor.Current),pElement->blend))
			return S_OK;
	}

	
	return S_FALSE;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param strText : √Ë ˆ
* \param pElement : √Ë ˆ
* \param prcDest : √Ë ˆ
* \param nCount : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::CalcTextRect( const char* strText, CGuiElement* pElement, RECT* prcDest, int nCount )
{
	int iFont = GetDevice()->SetFont(pElement->iFont);
	GetDevice()->GetStringRect(strText,nCount,*prcDest,pElement->dwTextFormat);
	GetDevice()->SetFont(iFont);
	return S_OK;
	/*
	HRESULT hr = S_OK;

	DXUTFontNode* pFontNode = GetFont( pElement->iFont );
	DWORD dwTextFormat = pElement->dwTextFormat | DT_CALCRECT;
	// Since we are only computing the rectangle, we don't need a sprite.
	hr = pFontNode->pFont->DrawText( NULL, strText, nCount, prcDest, dwTextFormat, pElement->FontColor.Current );
	if( FAILED(hr) )
	return hr;
	//*/

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param strText : √Ë ˆ
* \param pElement : √Ë ˆ
* \param prcDest : √Ë ˆ
* \param fontEffect : √Ë ˆ
* \param nCount : √Ë ˆ
* \param pData : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::DrawText( const WCHAR* strText, CGuiElement* pElement, RECT* prcDest, enumFontEffect fontEffect, int nCount ,DWORD* pData)
{
	HRESULT hr = S_OK;

	// No need to draw fully transparent layers
	if( pElement->FontColor.Current.a < 1.0f/16.0f )
		return S_OK;

	stRectI rcScreen = *prcDest;
	rcScreen.OffsetRect( m_x, m_y );

	// If caption is enabled, offset the Y position by its height.
	rcScreen.OffsetRect(m_clientOffsetTopLeft.x,m_clientOffsetTopLeft.y);

	int iFont = GetDevice()->SetFont(pElement->iFont);

	GetDevice()->DrawString(strText,nCount,rcScreen,pElement->FontColor.Current,pElement->dwTextFormat,fontEffect,COLOR_ARGB(DWORD(pElement->FontColor.Current.a * 255), 0, 0, 0),pData);

	GetDevice()->SetFont(iFont);

	return S_OK;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param strText : √Ë ˆ
* \param pElement : √Ë ˆ
* \param prcDest : √Ë ˆ
* \param fontEffect : √Ë ˆ
* \param nCount : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiDialog::DrawText( const char* strText, CGuiElement* pElement, RECT* prcDest, enumFontEffect fontEffect, int nCount )
{
	HRESULT hr = S_OK;

	// No need to draw fully transparent layers
	if( pElement->FontColor.Current.a < 1.0f/16.0f )
		return S_OK;

	stRectI rcScreen = *prcDest;
	rcScreen.OffsetRect( m_x, m_y );

	// If caption is enabled, offset the Y position by its height.
	rcScreen.OffsetRect(m_clientOffsetTopLeft.x,m_clientOffsetTopLeft.y);

	int iFont = GetDevice()->SetFont(pElement->iFont);

	//bShadow = false;

	GetDevice()->DrawString(strText,nCount,rcScreen,pElement->FontColor.Current,pElement->dwTextFormat,fontEffect,COLOR_ARGB(DWORD(pElement->FontColor.Current.a * 255), 0, 0, 0));

	GetDevice()->SetFont(iFont);

	return S_OK;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param colorTopLeft : √Ë ˆ
* \param colorTopRight : √Ë ˆ
* \param colorBottomLeft : √Ë ˆ
* \param colorBottomRight : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::SetBackgroundColors( DWORD colorTopLeft, DWORD colorTopRight, DWORD colorBottomLeft, DWORD colorBottomRight )
{
	m_colorTopLeft = colorTopLeft;
	m_colorTopRight = colorTopRight;
	m_colorBottomLeft = colorBottomLeft;
	m_colorBottomRight = colorBottomRight;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::ClearFocus()
{
	if( s_pControlFocus )
	{
		s_pControlFocus->OnFocusOut();
		s_pControlFocus = NULL;
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SetFocus(CGuiControl* pControl)
{
	if(!IsFocus())
	{
		return;
	}
	if(pControl && pControl->m_pDialog != this) return;

	ClearFocus();

	Assert(pControl->m_bDeleted == false);
	s_pControlFocus = pControl;
	if(pControl->CanHaveInputFocus())
		m_pLastFocus = pControl;
	if(s_pControlFocus)
		s_pControlFocus->OnFocusIn();
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
bool CGuiDialog::FocusDefaultControl()
{
	// Check for default control in this dialog
	for( size_t i=0; i < m_Controls.size(); i++ )
	{
		CGuiControl* pControl = m_Controls[ i ];
		if( !pControl->m_bDeleted && pControl->m_bIsDefault && pControl->m_pDialog->m_bKeyboardInput && pControl->CanHaveInputFocus())
		{
			// Remove focus from the current control
			ClearFocus();

			// Give focus to the default control
			s_pControlFocus = pControl;
			m_pLastFocus = pControl;
			s_pControlFocus->OnFocusIn();
			return true;
		}
	}
	return false;
}



/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param bForward : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
bool CGuiDialog::OnCycleFocus( bool bForward )
{
	// This should only be handled by the dialog which owns the focused control, and 
	// only if a control currently has focus
	if( s_pControlFocus == NULL || s_pControlFocus->m_pDialog != this )
		return false;

	CGuiControl* pControl = s_pControlFocus;
	for( int i=0; i < 0xffff; i++ )
	{
		pControl = (bForward) ? GetNextControl( pControl ) : GetPrevControl( pControl );

		// If we've gone in a full circle then focus doesn't change
		if( pControl == s_pControlFocus )
			return false;

		// If the dialog accepts keybord input and the control can have focus then
		// move focus
		if( !pControl->m_bDeleted && pControl->m_pDialog->m_bKeyboardInput && pControl->CanHaveInputFocus() )
		{
			s_pControlFocus->OnFocusOut();
			s_pControlFocus = pControl;
			m_pLastFocus = pControl;
			s_pControlFocus->OnFocusIn();
			return true;
		}
	}
	return false;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::OnMouseEnter( CGuiControl* pControl )
{
	if( pControl == NULL )
		return;

	//pControl->m_bMouseOver = true;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::OnMouseLeave( CGuiControl* pControl )
{
	if( pControl == NULL )
		return;

	for( int i=0; i < m_Controls.size(); i++ )
	{
		CGuiControl* pControl = m_Controls[ i ];
	}

	//pControl->m_bMouseOver = false;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::UninitDefaultElement()
{
	for(size_t i=0; i < s_DefaultElements.size(); i++ )
	{
		GuiElementHolder* pElementHolder = s_DefaultElements[ i ];
		SAFE_DELETE( pElementHolder );
	}

	s_DefaultElements.resize(0);

	for(size_t i=0; i < s_BackElements.size(); i++ )
	{
		CGuiElement* pElement = s_BackElements[ i ];
		SAFE_DELETE( pElement );
	}

	s_BackElements.resize(0);

	for(t_GuiMapCtrlBorder::iterator it = s_mapCtrlBorder.begin(); it != s_mapCtrlBorder.end(); ++it)
	{
		delete it->second;
	}
	s_mapCtrlBorder.clear();
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::InitCustomElements()
{
	SetFont( 0, "Arial", 14, FW_NORMAL );
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param parent : √Ë ˆ
* \param guiColor : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
inline void GuiElementBlendColorToXML(TiXmlElement* parent,GuiBlendColor* guiColor)
{
	TiXmlElement* blendColor = parent->InsertEndChild(TiXmlElement("blendColor"))->ToElement();
	XML_SetColorAttribute(blendColor,"defaultColor",guiColor->defaultColor);
	for(size_t i = 0;i < count_of(guiColor->States); ++i)
	{
		TiXmlElement* item = blendColor->InsertEndChild(TiXmlElement("state"))->ToElement();
		item->SetAttribute("state",GetGuiControlStateDesc(i));
		XML_SetColorAttribute(item,"color",guiColor->States[i]);
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param parent : √Ë ˆ
* \param pElement : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
inline void GuiElementToXML(TiXmlElement* parent,CGuiElement* pElement)
{
	IBitmaps* pBmp = pElement->pBmp;
	if(pBmp)
	{
		TiXmlElement* image = parent->InsertEndChild(TiXmlElement("image"))->ToElement();
		image->SetAttribute("graphicPack",pBmp->GetPackName());
		image->SetAttribute("group",(int)pBmp->GetGroup());
		image->SetAttribute("frame",(int)pBmp->GetFrame());
		char szRect[64];
		pElement->rcTexture.ToString(szRect,sizeof(szRect));
		image->SetAttribute("rect",szRect);
		GuiElementBlendColorToXML(image,&pElement->TextureColor);
	}
	TiXmlElement* font = parent->InsertEndChild(TiXmlElement("font"))->ToElement();
	font->SetAttribute("font",pElement->iFont);
	XML_SetAttribute(font,"format",pElement->dwTextFormat,16);
	GuiElementBlendColorToXML(font,&pElement->FontColor);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param parent : √Ë ˆ
* \param guiColor : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
inline void GuiElementBlendColorFromXML(TiXmlElement* parent,GuiBlendColor* guiColor)
{
	TiXmlElement* blendColor = parent->FirstChildElement("blendColor");
	if(!blendColor) return;
	guiColor->defaultColor = XML_GetColorAttribute(blendColor,"defaultColor",0xffffffff);
	TiXmlElement* state = blendColor->FirstChildElement("state");
	while(state)
	{
		const char* szState = state->Attribute("state");
		if(szState)
		{
			int iState = DescToGuiControlState(szState);
			guiColor->States[iState] = XML_GetColorAttribute(state,"color",0);
		}
		state = state->NextSiblingElement("state");
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param parent : √Ë ˆ
* \param pElement : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
inline void GuiElementFromXML(TiXmlElement* parent,CGuiElement* pElement)
{
	TiXmlElement* image = parent->FirstChildElement("image");
	if(image)
	{
		const char* szGraphicPack = image->Attribute("graphicPack");
		const char * szRect = image->Attribute("rect");
		if(szGraphicPack && szRect)
		{
			int iGroup,iFrame;
			image->Attribute("group",&iGroup);
			image->Attribute("frame",&iFrame);
			stResourceLocation rl;
			if(strncpy(rl.szFileName,szGraphicPack,sizeof(rl.szFileName)))
			{
				rl.group = iGroup;
				rl.frame = iFrame;
				stRectI rect;
				rect.FromString(szRect);
				DWORD defaultColor = 0xffffffff;
				TiXmlElement* blendColor = image->FirstChildElement("blendColor");
				if(blendColor)
					defaultColor = XML_GetColorAttribute(blendColor,"defaultColor",0xffffffff);
				pElement->SetTexture(&rl,&rect,defaultColor);
				GuiElementBlendColorFromXML(image,&pElement->TextureColor);
			}
		}
	}
	TiXmlElement* font = parent->FirstChildElement("font");
	if(font)
	{
		int iFont;
		DWORD defaultColor = 0xffffffff;
		TiXmlElement* blendColor = font->FirstChildElement("blendColor");
		if(blendColor)
			defaultColor = XML_GetColorAttribute(blendColor,"defaultColor",0xffffffff);

		font->Attribute("font",&iFont);
		pElement->dwTextFormat = DT_LEFT | DT_TOP;
		DWORD dwTextFormat;
		XML_QueryAttribute(font,"format",16,(int*)&dwTextFormat);
		pElement->SetFont(iFont,defaultColor,dwTextFormat);
		GuiElementBlendColorFromXML(font,&pElement->FontColor);
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pStream : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
bool CGuiDialog::LoadDefaultElementFromXML(Stream* pStream)
{
	TiXmlDocument doc;

	if(!LoadXMLFromStream(pStream,doc))
		return false;

	TiXmlElement* skin = doc.FirstChildElement("skin");
	TiXmlElement* defaltElements = skin->FirstChildElement("defaltElements");

	TiXmlElement* dialog = defaltElements->FirstChildElement("dialog");
	if(dialog)
	{
		TiXmlElement* caption = dialog->FirstChildElement("caption");
		GuiElementFromXML(caption,&s_CapElement);
		// Pre-blend as we don't need to transition the state
		s_CapElement.TextureColor.Blend( GUI_STATE_NORMAL, 10.0f );
		s_CapElement.FontColor.Blend( GUI_STATE_NORMAL, 10.0f );
	}

	TiXmlElement* controls = defaltElements->FirstChildElement("controls");
	TiXmlElement* control = controls->FirstChildElement("control");
	while(control)
	{
		int iControlType;

		const char* szControlType = control->Attribute("type");
		if(szControlType)
		{
			iControlType = DescToGuiControlType(szControlType);
			TiXmlElement* item = control->FirstChildElement("element");
			while(item)
			{
				int iElement;
				item->Attribute("index",&iElement);

				GuiElementHolder Element;
				Element.nControlType = iControlType;
				Element.iElement = iElement;
				GuiElementFromXML(item,&Element.Element);
				SetDefaultElement(iControlType,iElement,&Element.Element);
				item = item->NextSiblingElement("element");
			}
		}
		control = control->NextSiblingElement("control");
	}

	return true;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pszZipPackName : √Ë ˆ
* \param pszFileName : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
bool CGuiDialog::LoadDefaultElementFromXML(const char* pszZipPackName,const char* pszFileName)
{
	Stream* pStream = OpenPackFileForRead(pszZipPackName,pszFileName);
	if(!pStream) return false;
	bool b = LoadDefaultElementFromXML(pStream);
	ClosePackFileForRead(pStream);
	return b;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pszFileName : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SaveDefaultElementToXML(const char* pszFileName)
{
	TiXmlDocument doc; 
	doc.InsertEndChild(TiXmlDeclaration("1.0","gb2312",""));

	TiXmlElement* skin = XML_FindAndAddChildNode(&doc,"skin");
	TiXmlElement* defaltElements = XML_FindAndAddChildNode(skin,"defaltElements");

	TiXmlElement* dialog = XML_FindAndAddChildNode(defaltElements,"dialog");

	TiXmlElement* caption = XML_FindAndAddChildNode(dialog,"caption");
	GuiElementToXML(caption,&s_CapElement);

	TiXmlElement* controls = XML_FindAndAddChildNode(defaltElements,"controls");

	TiXmlElement* control = NULL;
	int iControlType;
	for(size_t i = 0;i < s_DefaultElements.size();++i)
	{
		GuiElementHolder* pElementHolder = s_DefaultElements[i];

		if(i == 0 || pElementHolder->nControlType != iControlType)
		{
			iControlType = pElementHolder->nControlType;
			control = controls->InsertEndChild(TiXmlElement("control"))->ToElement();
			control->SetAttribute("type",GetGuiControlTypeDesc(pElementHolder->nControlType));
		}

		TiXmlElement* item = control->InsertEndChild(TiXmlElement("element"))->ToElement();
		item->SetAttribute("index",(int)pElementHolder->iElement);
		CGuiElement* pElement = &pElementHolder->Element;
		GuiElementToXML(item,pElement);
	}
	doc.SaveFile(pszFileName);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param p1 : √Ë ˆ
* \param p2 : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
int compareGuiElementHolder(const void *p1, const void *p2) 
{
	GuiElementHolder* pElementHolder1 = *(GuiElementHolder**)p1;
	GuiElementHolder* pElementHolder2 = *(GuiElementHolder**)p2;
	return ((pElementHolder1->nControlType << 16) |(pElementHolder1->iElement))
		- ((pElementHolder2->nControlType << 16) |(pElementHolder2->iElement));
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiDialog::InitDefaultElements()
{
	//if(LoadDefaultElementFromXML("data\\gui.pak","gui\\DefaultSkin.xml"))
	//	return;

	const char* szGrpFileName = GetGuiGraphicPackName();

	CGuiElement Element;
	stRectI rcTexture;
	stRectI rects[9];

	stResourceLocation rl;
	rl.SetFileName(GetGuiGraphicPackName());
	rl.group = 0;
	rl.frame = 0;

	//-------------------------------------
	// Element for the caption ±ÍÃ‚øÚ
	//-------------------------------------
	rl.group = 0;
	rl.frame = 3;
	//rcTexture.SetRect( 138, 60, 256, 86 );
	//s_CapElement.SetTexture( &rl, &rcTexture );
	s_CapElement.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	s_CapElement.FontColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	s_CapElement.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER );
	s_CapElement.TextureColor.Current = COLOR_ARGB(255,255,255,255);
	s_CapElement.FontColor.Current = COLOR_ARGB(255,255,255,255);

	//--------------------------------------
	// µ◊Œ∆
	//--------------------------------------
	rl.group = 0;
	rl.frame = 8;
	rcTexture.SetRect(0,0,768,768);
	s_BackGroundElement.SetTexture(&rl,&rcTexture);
	s_BackGroundElement.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	s_BackGroundElement.FontColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	s_BackGroundElement.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER );
	s_BackGroundElement.TextureColor.Current = COLOR_ARGB(255,255,255,255);
	s_BackGroundElement.FontColor.Current = COLOR_ARGB(255,255,255,255);

	//--------------------------------------
	// øÿº˛µ◊Œ∆
	//--------------------------------------
	rl.group = 0;
	rl.frame = 11;
	rcTexture.SetRect(0,0,768,768);
	s_controlBackGroundElement.SetTexture(&rl,&rcTexture);
	s_controlBackGroundElement.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	s_controlBackGroundElement.FontColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	s_controlBackGroundElement.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER );
	s_controlBackGroundElement.TextureColor.Current = COLOR_ARGB(255,255,255,255);
	s_controlBackGroundElement.FontColor.Current = COLOR_ARGB(255,255,255,255);

	//--------------------------------------
	// ◊Û…œΩ«Õº∆¨
	//--------------------------------------
	rl.group = 0;
	rl.frame = 9;
	rcTexture.SetRect(0,0,147,378);
	s_LeftTopElement.SetTexture(&rl,&rcTexture);
	s_LeftTopElement.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	s_LeftTopElement.FontColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	s_LeftTopElement.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER );
	s_LeftTopElement.TextureColor.Current = COLOR_ARGB(255,255,255,255);
	s_LeftTopElement.FontColor.Current = COLOR_ARGB(255,255,255,255);

	//-------------------------------------
	// Element for the back Õ‚øÚ
	//-------------------------------------
	s_BackElements.resize( 18 );
	{
		rl.group = 0;
		rl.frame = 2;

		CuttRect9(s_borderRect,s_borderTopLeft,s_borderRightBottom,rects);

		for(size_t i=0;i<count_of(rects);++i)
		{
			CGuiElement* pEmt = new CGuiElement;
			pEmt->SetTexture(&rl,&rects[i]);
			pEmt->FontColor.Current = COLOR_ARGB(255,255,255,255);
			pEmt->TextureColor.Current = COLOR_ARGB(255,255,255,255);
			s_BackElements[i] = pEmt;
		}
	}

	//-------------------------------------
	// Element for the back
	//-------------------------------------
	{
		rl.group = 0;
		rl.frame = 10;

		CuttRect9(s_smBorderRect,s_smBorderTopLeft,s_smBorderRightBottom,rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
			CGuiElement* pEmt = new CGuiElement;
			pEmt->SetTexture(&rl,&rects[i]);
			pEmt->FontColor.Current = COLOR_ARGB(255,255,255,255);
			pEmt->TextureColor.Current = COLOR_ARGB(255,255,255,255);
			s_BackElements[i + 9] = pEmt;
		}
	}

	//------------------------------------
	// Element for the 3D Border EditBox(±‡º≠øÚ)
	//------------------------------------
	{
		rl.group = 0;
		rl.frame = 3;
		stGuiCtrlBorderInfo * pInfo = GetDefaultCtrlBorderInfo(GUI_CONTROL_EDITBOX);
		pInfo->rcTexture.SetRect(0,167,193,191);
		pInfo->ptLeftTop = stPointI(4,5);
		pInfo->ptRightBottom = stPointI(5,5);
		pInfo->ptBorderLeftTop = stPointI(4,5);
		pInfo->ptBorderRightBottom = stPointI(5,5);
		pInfo->CuttRect9(rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
			pInfo->element[i].SetTexture(&rl,&rects[i]);
			pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
			pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}
	}


	//------------------------------------
	// Element for the 3D Border MLEditBox(∂‡––±‡º≠øÚ)
	//------------------------------------
	{
		rl.group = 0;
		rl.frame = 3;
		stGuiCtrlBorderInfo * pInfo = GetDefaultCtrlBorderInfo(GUI_CONTROL_MLEDITBOX);
		pInfo->rcTexture.SetRect(0,0,224,156);
		pInfo->ptLeftTop = stPointI(13,34);
		pInfo->ptRightBottom = stPointI(12,12);
		pInfo->ptBorderLeftTop = stPointI(13,16);
		pInfo->ptBorderRightBottom = stPointI(12,12);
		pInfo->CuttRect9(rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
			pInfo->element[i].SetTexture(&rl,&rects[i]);
			pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
			pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}
	}

	//------------------------------------
	// Element for the 3D Border MLTextBox(∂‡––Œƒ±æøÚ)
	//------------------------------------
	{
		rl.group = 0;
		rl.frame = 3;
		stGuiCtrlBorderInfo * pInfo = GetDefaultCtrlBorderInfo(GUI_CONTROL_MLTEXTBOX);
		pInfo->rcTexture.SetRect(0,0,224,156);
		pInfo->ptLeftTop = stPointI(13,34);
		pInfo->ptRightBottom = stPointI(12,12);
		pInfo->ptBorderLeftTop = stPointI(13,16);
		pInfo->ptBorderRightBottom = stPointI(12,12);
		pInfo->CuttRect9(rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
			pInfo->element[i].SetTexture(&rl,&rects[i]);
			pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
			pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}
	}

	//------------------------------------
	// Element for the 3D Border ListBox(¡–±ÌøÚ)
	//------------------------------------
	{
		rl.group = 0;
		rl.frame = 3;
		stGuiCtrlBorderInfo * pInfo = GetDefaultCtrlBorderInfo(GUI_CONTROL_LISTBOX);
		pInfo->rcTexture.SetRect(0,0,224,156);
		pInfo->ptLeftTop = stPointI(13,34);
		pInfo->ptRightBottom = stPointI(12,12);
		pInfo->ptBorderLeftTop = stPointI(13,16);
		pInfo->ptBorderRightBottom = stPointI(12,12);
		pInfo->CuttRect9(rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
			pInfo->element[i].SetTexture(&rl,&rects[i]);
			pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
			pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}
	}

	//------------------------------------
	// Element for the 3D Border ListBox(√Ê∞Â)
	//------------------------------------
	{
		rl.group = 0;
		rl.frame = 3;
		stGuiCtrlBorderInfo * pInfo = GetDefaultCtrlBorderInfo(GUI_CONTROL_PANEL);
		pInfo->rcTexture.SetRect(0,0,224,156);
		pInfo->ptLeftTop = stPointI(13,34);
		pInfo->ptRightBottom = stPointI(12,12);
		pInfo->ptBorderLeftTop = stPointI(13,16);
		pInfo->ptBorderRightBottom = stPointI(12,12);
		pInfo->CuttRect9(rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
			pInfo->element[i].SetTexture(&rl,&rects[i]);
			pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
			pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}
	}

	//------------------------------------
	// Element for the 3D Border ScrollBar(πˆ∂ØÃı)
	//------------------------------------
	{
		rl.group = 0;
		rl.frame = 3;
		stGuiCtrlBorderInfo * pInfo = GetDefaultCtrlBorderInfo(GUI_CONTROL_SCROLLBAR);
		pInfo->rcTexture.SetRect(225,0,247,136);
		pInfo->ptLeftTop = stPointI(0,19);
		pInfo->ptRightBottom = stPointI(0,19);
		pInfo->ptBorderLeftTop = stPointI(3,3);
		pInfo->ptBorderRightBottom = stPointI(3,3);
		pInfo->CuttRect9(rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
			pInfo->element[i].SetTexture(&rl,&rects[i]);
			pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
			pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}
	}

	//------------------------------------
	// Element for the 3D Border ComboBox(◊È∫œøÚ)
	//------------------------------------
	{
		rl.group = 0;
		rl.frame = 3;
		stGuiCtrlBorderInfo * pInfo = GetDefaultCtrlBorderInfo(GUI_CONTROL_COMBOBOX);
		pInfo->rcTexture.SetRect(0,167,193,191);
		pInfo->ptLeftTop = stPointI(4,5);
		pInfo->ptRightBottom = stPointI(5,5);
		pInfo->ptBorderLeftTop = stPointI(4,5);
		pInfo->ptBorderRightBottom = stPointI(5,5);
		pInfo->CuttRect9(rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
			pInfo->element[i].SetTexture(&rl,&rects[i]);
			pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
			pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}
	}

	//------------------------------------
	// Element for the 3D Border Tab(TAB)
	//------------------------------------
	{
		rl.group = 0;
		rl.frame = 7;
		stGuiCtrlBorderInfo * pInfo = GetDefaultCtrlBorderInfo(GUI_CONTROL_TAB);
		/*pInfo->rcTexture.SetRect(0,52,256,94);
		pInfo->ptLeftTop = stPointI(4,4);
		pInfo->ptRightBottom = stPointI(4,0);
		pInfo->ptBorderLeftTop = stPointI(3,2);
		pInfo->ptBorderRightBottom = stPointI(3,0);
		pInfo->CuttRect9(rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
		pInfo->element[i].SetTexture(&rl,&rects[i]);
		pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
		pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}*/
	}



	//------------------------------------
	// Element for the 3D Border Process(Ω¯∂»Ãı)
	//------------------------------------
	{

		rl.group = 19;
		rl.frame = 6;
		stGuiCtrlBorderInfo * pInfo = GetDefaultCtrlBorderInfo(GUI_CONTROL_PROCESS);
		pInfo->rcTexture.SetRect(0,0,108,15);
		pInfo->ptLeftTop = CGuiProcess::s_borderTopLeft;
		pInfo->ptRightBottom = CGuiProcess::s_borderRightBottom;
		pInfo->ptBorderLeftTop = stPointI(5,2);
		pInfo->ptBorderRightBottom = stPointI(4,2);
		pInfo->CuttRect9(rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
			pInfo->element[i].SetTexture(&rl,&rects[i]);
			pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
			pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}
	}




	//------------------------------------
	// Element for the 3D Border ScrollTab
	//------------------------------------
	{
		rl.group = 19;
		rl.frame = 0;
		stGuiCtrlBorderInfo * pInfo = GetDefaultCtrlBorderInfo(GUI_CONTROL_SCROLLTAB);
		pInfo->rcTexture.SetRect(0,0,261,19);
		pInfo->ptLeftTop = CGuiScrollTab::s_borderTopLeft;
		pInfo->ptRightBottom = CGuiScrollTab::s_borderRightBottom;
		pInfo->ptBorderLeftTop = stPointI(4,2);
		pInfo->ptBorderRightBottom = stPointI(4,2);
		pInfo->CuttRect9(rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
			pInfo->element[i].SetTexture(&rl,&rects[i]);
			pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
			pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}
	}



	rl.group = 0;
	rl.frame = 1;
	//-------------------------------------
	// CGuiStatic
	//-------------------------------------
	Element.SetFont( 0 ,COLOR_ARGB(255,255,255,255),DT_TOP | DT_LEFT);
	Element.FontColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );

	// Assign the Element
	SetDefaultElement( GUI_CONTROL_STATIC, 0, &Element );

	//-------------------------------------
	// CGuiEditBox
	//-------------------------------------
	Element.SetFont( 0 ,COLOR_ARGB(255,255,255,255),DT_TOP | DT_LEFT | DT_SINGLELINE);
	Element.FontColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );

	// Assign the Element
	SetDefaultElement( GUI_CONTROL_EDITBOX, 0, &Element );

	//-------------------------------------
	// CGuiMLEditBox
	//-------------------------------------
	Element.SetFont( 0 ,COLOR_ARGB(255,255,255,255),DT_TOP | DT_LEFT);
	Element.FontColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );

	// Assign the Element
	SetDefaultElement( GUI_CONTROL_MLEDITBOX, 0, &Element );

	//-------------------------------------
	// CGuiButton - Button –°µƒ
	//-------------------------------------
	rcTexture.SetRect( 0, 0, 70, 29 );
	Element.SetTexture( &rl, &rcTexture );
	Element.SetFont( 0 );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	Element.FontColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255,204,165,10);
	Element.FontColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.FontColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(255,204,165,10);
	// Assign the Element
	SetDefaultElement( GUI_CONTROL_BUTTON, 0, &Element );

	rcTexture.SetRect( 70, 0, 140, 29 );
	Element.SetTexture( &rl, &rcTexture );
	Element.SetFont( 0 );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = 0;

	Element.FontColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.FontColor.States[ GUI_STATE_PRESSED ] = 0;
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.FontColor.States[ GUI_STATE_FOCUS ] = 0;
	// Assign the Element
	SetDefaultElement( GUI_CONTROL_BUTTON, 1, &Element );

	rcTexture.SetRect( 140, 0, 210, 29 );
	Element.SetTexture( &rl, &rcTexture );
	Element.SetFont( 0 );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(0, 0, 0, 0);

	Element.FontColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.FontColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.FontColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	// Assign the Element
	SetDefaultElement( GUI_CONTROL_BUTTON, 2, &Element );


	//-------------------------------------
	// CGuiButton - Button ¥Ûµƒ
	//-------------------------------------
	rcTexture.SetRect( 0, 29, 88, 58 );
	Element.SetTexture( &rl, &rcTexture );
	Element.SetFont( 0 );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	Element.FontColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255,204,165,10);
	Element.FontColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.FontColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(255,204,165,10);
	// Assign the Element
	SetDefaultElement( GUI_CONTROL_BUTTON, 3, &Element );

	rcTexture.SetRect( 88, 29, 176, 58 );
	Element.SetTexture( &rl, &rcTexture );
	Element.SetFont( 0 );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = 0;

	Element.FontColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.FontColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.FontColor.States[ GUI_STATE_FOCUS ] = 0;
	// Assign the Element
	SetDefaultElement( GUI_CONTROL_BUTTON, 4, &Element );


	rcTexture.SetRect( 0, 58, 88, 87 );
	Element.SetTexture( &rl, &rcTexture );
	Element.SetFont( 0 );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(0, 0, 0, 0);

	Element.FontColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.FontColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.FontColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	Element.FontColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(0, 0, 0, 0);
	// Assign the Element
	SetDefaultElement( GUI_CONTROL_BUTTON, 5, &Element );


	//-------------------------------------
	// CGuiCheckBox - Box
	//-------------------------------------
	rcTexture.SetRect( 38, 91, 59, 112 );
	Element.SetTexture( &rl, &rcTexture );
	Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
	Element.FontColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	// Assign the Element
	SetDefaultElement( GUI_CONTROL_CHECKBOX, 0, &Element );


	//-------------------------------------
	// CGuiCheckBox - Check
	//-------------------------------------
	rcTexture.SetRect( 59, 91, 78, 112 );
	Element.SetTexture( &rl, &rcTexture );

	// Assign the Element
	SetDefaultElement( GUI_CONTROL_CHECKBOX, 1, &Element );


	//-------------------------------------
	// CGuiRadioButton - Box
	//-------------------------------------
	rcTexture.SetRect( 0, 91, 21, 112 );
	Element.SetTexture( &rl, &rcTexture );
	Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
	Element.FontColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	// Assign the Element
	SetDefaultElement( GUI_CONTROL_RADIOBUTTON, 0, &Element );


	//-------------------------------------
	// CGuiRadioButton - Check
	//-------------------------------------
	rcTexture.SetRect( 21, 91, 38, 112 );
	Element.SetTexture( &rl, &rcTexture );

	// Assign the Element
	SetDefaultElement( GUI_CONTROL_RADIOBUTTON, 1, &Element );


	//-------------------------------------
	// CGuiComboBox - Main
	//-------------------------------------
	{
		rl.group = 0;
		rl.frame = 3;
		Element.SetFont(0);
		Element.FontColor.Current = (DWORD)-1;
		// Assign the Element
		SetDefaultElement( GUI_CONTROL_COMBOBOX, 0, &Element );

		//-------------------------------------
		// CGuiComboBox - Button
		//-------------------------------------
		rcTexture.SetRect( 194,168, 216, 191 );
		Element.SetTexture( &rl, &rcTexture );
		Element.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255,255,255,255);
		Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255,255,255,255);
		Element.TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(255,255,255,255);
		Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255,255,255,255);

		// Assign the Element
		SetDefaultElement( GUI_CONTROL_COMBOBOX, 1, &Element );
	}

	rl.group = 0;
	rl.frame = 1;
	//-------------------------------------
	// CGuiSlider - Track
	//-------------------------------------
	rcTexture.SetRect( 2, 124, 137, 135 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 255, 255, 255);

	// Assign the Element
	SetDefaultElement( GUI_CONTROL_SLIDER, 0, &Element );

	//-------------------------------------
	// CGuiSlider - Button
	//-------------------------------------
	rcTexture.SetRect( 162, 119, 173, 141 );
	Element.SetTexture( &rl, &rcTexture );

	// Assign the Element
	SetDefaultElement( GUI_CONTROL_SLIDER, 1, &Element );


	{
		rl.group = 0;
		rl.frame = 3;



		//-------------------------------------
		// CGuiScrollBar - Up Arrow
		//-------------------------------------
		rcTexture.SetRect( 0, 211,15, 230);
		Element.SetTexture( &rl, &rcTexture );
		Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);

		// Assign the Element
		SetDefaultElement( GUI_CONTROL_SCROLLBAR, 0, &Element );

		//-------------------------------------
		// CGuiScrollBar - Up Arrow Click
		//-------------------------------------
		rcTexture.SetRect( 16, 211,31, 230);
		Element.SetTexture( &rl, &rcTexture );
		Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);

		// Assign the Element
		SetDefaultElement( GUI_CONTROL_SCROLLBAR, 1, &Element );


		//-------------------------------------
		// CGuiScrollBar - Down Arrow
		//-------------------------------------
		rcTexture.SetRect( 0, 232,15, 251 );
		Element.SetTexture( &rl, &rcTexture );
		Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);

		// Assign the Element
		SetDefaultElement( GUI_CONTROL_SCROLLBAR, 2, &Element );


		//-------------------------------------
		// CGuiScrollBar - Down Arrow Click
		//-------------------------------------
		rcTexture.SetRect( 16, 232,31, 251 );
		Element.SetTexture( &rl, &rcTexture );
		Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);

		// Assign the Element
		SetDefaultElement( GUI_CONTROL_SCROLLBAR, 3, &Element );


		//-------------------------------------
		// CGuiScrollBar - Button
		//-------------------------------------
		rcTexture.SetRect( 33, 209,	 56, 231 );
		Element.SetTexture( &rl, &rcTexture );

		// Assign the Element
		SetDefaultElement( GUI_CONTROL_SCROLLBAR, 4, &Element );
	}


	//-------------------------------------
	// CGuiScrollTab
	//-------------------------------------
	Element.SetFont( 0 ,COLOR_ARGB(255,255,255,255),DT_TOP | DT_LEFT | DT_SINGLELINE);
	Element.FontColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB( 200, 200, 200, 200 );

	// Assign the Element
	SetDefaultElement( GUI_CONTROL_SCROLLTAB, 0, &Element );


	rl.frame = 7;


	//-------------------------------------
	// CGuiPanel
	//-------------------------------------
	// Element assignment:
	//   0 - text area
	//   1 - top left border
	//   2 - top border
	//   3 - top right border
	//   4 - left border
	//   5 - right border
	//   6 - lower left border
	//   7 - lower border
	//   8 - lower right border
	//rl.group = 0;
	//rl.frame = 3;
	//stRectI totalRect(0,0,224,156);
	//CuttRect9(totalRect,CGuiPanel::s_borderTopLeft,CGuiPanel::s_borderRightBottom,rects);
	//Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_CENTER | DT_VCENTER);
	//
	//// Assign the style
	//int i=0;
	//Element.SetTexture( &rl, &rects[i++] );
	//SetDefaultElement( GUI_CONTROL_PANEL, 0, &Element );

	//Element.SetTexture( &rl, &rects[i++] );
	//SetDefaultElement( GUI_CONTROL_PANEL, 1, &Element );

	//Element.SetTexture( &rl, &rects[i++] );
	//SetDefaultElement( GUI_CONTROL_PANEL, 2, &Element );

	//Element.SetTexture( &rl, &rects[i++] );
	//SetDefaultElement( GUI_CONTROL_PANEL, 3, &Element );

	//Element.SetTexture( &rl, &rects[i++] );
	//SetDefaultElement( GUI_CONTROL_PANEL, 4, &Element );

	//Element.SetTexture( &rl, &rects[i++] );
	//SetDefaultElement( GUI_CONTROL_PANEL, 5, &Element );
	//
	//Element.SetTexture( &rl, &rects[i++] );
	//SetDefaultElement( GUI_CONTROL_PANEL, 6, &Element );

	//Element.SetTexture( &rl, &rects[i++] );
	//SetDefaultElement( GUI_CONTROL_PANEL, 7, &Element );

	//Element.SetTexture( &rl, &rects[i++] );
	//SetDefaultElement( GUI_CONTROL_PANEL, 8, &Element );



	//-------------------------------------
	// CGuiProcess
	//-------------------------------------
	/*

	rcTexture.SetRect( 0,52,256,94 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);

	SetDefaultElement( GUI_CONTROL_PROCESS, 0, &Element );
	*/



	rl.frame = 9;
	Element.SetTexture( &rl, &stRectI(0,0,355,218),COLOR_ARGB(128,255,255,255) );
	Element.TextureColor.Current = COLOR_ARGB(128,255,255,255);
	//SetDefaultElement( GUI_CONTROL_PANEL, 9, &Element );
	//-------------------------------------
	// CGuiTabItem
	//-------------------------------------
	// Element assignment:
	//   0 - text area
	//   1 - top left border
	//   2 - top border
	//   3 - top right border
	//   4 - left border
	//   5 - right border
	//   6 - lower left border
	//   7 - lower border
	//   8 - lower right border
	stRectI totalRect;
	rl.group = 0;
	rl.frame = 3;
	totalRect.SetRect(139,202,210,231);
	CuttRect9(totalRect,CGuiTab::s_borderTopLeft,CGuiTab::s_borderRightBottom,rects);
	int i = 0;
	// ≤ª—°÷–µƒÕº∆¨
	Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_CENTER | DT_VCENTER);
	Element.FontColor.States[GUI_STATE_NORMAL] = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	Element.FontColor.States[GUI_STATE_PRESSED] = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	Element.FontColor.States[GUI_STATE_MOUSEOVER] = D3DCOLOR_ARGB( 255, 255, 255, 255 );


	// Assign the style
	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_PRESSED] = 0;
	Element.TextureColor.States[GUI_STATE_MOUSEOVER] = 0;

	SetDefaultElement( GUI_CONTROL_TAB, 0, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_PRESSED] = 0;
	Element.TextureColor.States[GUI_STATE_MOUSEOVER] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 1, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_PRESSED] = 0;
	Element.TextureColor.States[GUI_STATE_MOUSEOVER] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 2, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_PRESSED] = 0;
	Element.TextureColor.States[GUI_STATE_MOUSEOVER] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 3, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_PRESSED] = 0;
	Element.TextureColor.States[GUI_STATE_MOUSEOVER] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 4, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_PRESSED] = 0;
	Element.TextureColor.States[GUI_STATE_MOUSEOVER] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 5, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_PRESSED] = 0;
	Element.TextureColor.States[GUI_STATE_MOUSEOVER] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 6, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_PRESSED] = 0;
	Element.TextureColor.States[GUI_STATE_MOUSEOVER] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 7, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_PRESSED] = 0;
	Element.TextureColor.States[GUI_STATE_MOUSEOVER] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 8, &Element );

	//----------------------------------------------------
	//----------------------------------------------------
	// —°÷–µƒÕº∆¨
	i = 0;
	totalRect.SetRect(65,201,136,233);
	CuttRect9(totalRect,CGuiTab::s_borderTopLeft,CGuiTab::s_borderRightBottom,rects);
	Element.SetFont( 0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), DT_CENTER | DT_VCENTER);
	Element.FontColor.States[GUI_STATE_NORMAL] = 0;

	// Assign the style
	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_NORMAL] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 9, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_NORMAL] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 10, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_NORMAL] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 11, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_NORMAL] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 12, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_NORMAL] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 13, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_NORMAL] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 14, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_NORMAL] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 15, &Element );

	Element.SetTexture( &rl, &rects[i++] );
	Element.TextureColor.States[GUI_STATE_NORMAL] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 16, &Element );

	Element.SetTexture( &rl, &rects[i++]);
	Element.TextureColor.States[GUI_STATE_NORMAL] = 0;
	SetDefaultElement( GUI_CONTROL_TAB, 17, &Element );
	//----------------------------------------------------


	//-------------------------------------
	// CGuiListBox - Main
	//-------------------------------------

	Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP | DT_SINGLELINE );
	Element.FontColor.Current = COLOR_ARGB(255,255,255,255);
	// Assign the Element
	SetDefaultElement( GUI_CONTROL_LISTBOX, 0, &Element );

	//--------------------------------------
	// CGuiTable - Texture
	//--------------------------------------

	rl.group = 0;
	rl.frame = 4;

	CuttRect9(CGuiTable::s_ctrlBorderRect,CGuiTable::s_ctrlBorderTopLeft,CGuiTable::s_ctrlBorderRightBottom,rects);
	for(size_t i=0;i<9;++i)
	{
		Element.SetTexture( &rl, &rects[i] );
		Element.TextureColor.Current = (DWORD)-1;
		SetDefaultElement( GUI_CONTROL_TABLE , i , &Element);
	}
	//-------------------------------------

	//-------------------------------------
	// CloseBox
	//-------------------------------------
	rl.group = 0;
	rl.frame = 1;
	Element.SetFont(0);
	rcTexture.SetRect(109,64,141,97);
	Element.SetTexture(&rl,&rcTexture);
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = 0;
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = 0;
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = 0;
	SetDefaultElement(GUI_CONTROL_CLOSEBOX,0,&Element);

	rcTexture.SetRect(109,64,141,97);
	Element.SetTexture(&rl,&rcTexture);
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = 0;
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	SetDefaultElement(GUI_CONTROL_CLOSEBOX,1,&Element);

	rcTexture.SetRect(141,64,173,97);
	Element.SetTexture(&rl,&rcTexture);
	Element.TextureColor.States[ GUI_STATE_NORMAL ] = 0;
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(255, 255, 255, 255);
	Element.TextureColor.States[ GUI_STATE_MOUSEOVER ] = 0;
	Element.TextureColor.States[ GUI_STATE_FOCUS ] = 0;
	SetDefaultElement(GUI_CONTROL_CLOSEBOX,2,&Element);
	//-------------------------------------------------

	qsort(&s_DefaultElements[0],s_DefaultElements.size(),sizeof(GuiElementHolder*),compareGuiElementHolder);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \param index : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SetControlZOrder(CGuiControl* pControl,int index)
{
	int oldIndex = FindControl(pControl);
	Assert(oldIndex != -1);
	if(oldIndex == -1) return;
	if(oldIndex == index) return;
	if(oldIndex < index)
	{
		memmove(&m_Controls[oldIndex],&m_Controls[oldIndex+1],sizeof(CGuiControl*) * (index - oldIndex));
	}
	else
	{
		memmove(&m_Controls[index + 1],&m_Controls[index],sizeof(CGuiControl*) * (oldIndex - index));
	}
	m_Controls[index] = pControl;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
int  CGuiDialog::FindControl(CGuiControl * pControl)
{
	for(size_t i=0 ; i < m_Controls.size(); ++i)
	{
		if(m_Controls[i] == pControl)
			return i;
	}
	return -1;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pControl : √Ë ˆ
* \param pInsertAfter : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SetControlZOrder(CGuiControl* pControl,CGuiControl * pInsertAfter)
{
	switch((int)(pInsertAfter))
	{
	case GUI_CONTROL_TOP:
		SetControlZOrder(pControl,m_Controls.size()-1);
		return;
	case GUI_CONTROL_BOTTOM:
		SetControlZOrder(pControl,0);
		return;
	}
	int index = FindControl(pInsertAfter);
	Assert(index != -1);
	if(index == -1)
		return;

	SetControlZOrder(pControl,index);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pInsertAfter : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SetZOrder(CGuiDialog * pInsertAfter)
{
	GetGuiManager()->SetDialogZOrder(this,pInsertAfter);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::OnClose()
{

}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::Close(void)
{
	if(m_bClosed) return;
	if(m_pPopupMenu) 
	{
		m_pPopupMenu->Close();
		m_pPopupMenu = NULL;
	}
	m_bClosed = true;
	m_bEnabled = false;
	m_bVisible = false;
	m_bPlayCloseSound = false;
	m_bPlayOpenSound = false;
	OnHide();
	GetGuiManager()->OnCloseDlg(this);
	OnClose();
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ptScreen : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
BOOL CGuiDialog::ContainsPoint(POINT  ptScreen)
{
	if(IsInRect(ptScreen))
	{
		if(m_pBackImage)
		{
			POINT pt = {ptScreen.x - (m_x + m_pBackImage->GetX()), ptScreen.y - (m_y + m_pBackImage->GetY())};
			if(m_pBackImage->ContainsPoint(pt))
				return TRUE;
		}
		else if((GetStyle() & GUI_WS_NOSKIN) == 0)
		{
			ptScreen.x -= GetX();
			ptScreen.y -= GetY();
			int i0,i1;
			if(m_bSmDialog) 
			{
				i0 = 9;
				i1 = 18;
			}
			else
			{
				i0 = 0;
				i1 = 9;
			}

			stRectI rect = m_BackRects[0];
			if(rect.PtInRect(ptScreen)) return TRUE;

			stRectI rectBack(0,0,GetWidth(),GetHeight());
			rectBack.left += m_ptBackGroundOffsetTopLeft.x;
			rectBack.right += m_ptBackGroundOffsetTopLeft.y;
			rectBack.right -= m_ptBackGroundOffsetRightBottom.x;
			rectBack.bottom -= m_ptBackGroundOffsetRightBottom.y;

			if(rectBack.PtInRect(ptScreen)) return TRUE;

			for(int i=i0 + 1;i<i1;++i)
			{
				stRectI rect = m_BackRects[i%9];

				if(GetPatchImageAlpha(ptScreen,rect,s_BackElements[i]->pBmp,&s_BackElements[i]->rcTexture,s_PatchAlignFormat[i%9]) != 0)
					return TRUE;
				//::PatchRenderImage(rect,s_BackElements[i]->pBmp,&s_BackElements[i]->rcTexture,-1,Blend_Null,s_PatchAlignFormat[i%9]);
			}

			if( !m_pBackImage && (GetStyle() & GUI_WS_NOSKIN) == 0 && !m_bSmDialog )
			{
				// ªÊ÷∆◊Û…œΩ«Õº∆¨
				CGuiElement* pEmt = (m_pLeftTopElement ? m_pLeftTopElement : &s_LeftTopElement);
				if(pEmt->pBmp && pEmt->pBmp->GetFrameCount())
				{
					stPointI pt1 = ptScreen;
					pt1.x += m_ptLeftTopOffset.x;
					pt1.y += m_ptLeftTopOffset.y;
					if( pEmt->pBmp->GetBitmap(0)->GetAlpha(pt1) != 0) 
						return TRUE;
				}
			}
		}
		else return TRUE;


	}
	return FALSE;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ptScreen : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
BOOL CGuiDialog::IsInRect(POINT ptScreen)
{
	return (ptScreen.x >= m_x && ptScreen.x < m_x + m_width && ptScreen.y >= m_y && ptScreen.y < m_y + m_height);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param ptScreen : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
DWORD CGuiDialog::OnHiTest(POINT ptScreen)
{
	if(!IsInRect(ptScreen))
		return HTERROR;
	if(!ContainsPoint(ptScreen)) 
		return HTTRANSPARENT;
	if( ( GetStyle() & GUI_WS_RESIZEING ) || ( GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_CONTROL) < 0) )
	{
		const int borderWidth = 3;
		stRectI rect(m_x,m_y,m_x + m_width,m_y + m_height);

		if(rect.right - ptScreen.x <= 12 && rect.bottom - ptScreen.y <= 12)
		{
			return HTBOTTOMRIGHT;
		}

		stRectI rect2 = rect;
		rect2.InflateRect(-borderWidth-1,-borderWidth-1);
		if(rect.PtInRect(ptScreen) && !rect2.PtInRect(ptScreen))
		{
			if(ptScreen.x - rect.left <= borderWidth)
			{
				if(ptScreen.y - rect.top <= borderWidth)
					return HTTOPLEFT;
				else if(rect.bottom - ptScreen.y <= borderWidth)
					return HTBOTTOMLEFT;
				return HTLEFT;
			}
			else if(rect.right - ptScreen.x <= borderWidth)
			{
				if(ptScreen.y - rect.top <= borderWidth)
					return HTTOPRIGHT;
				else if(rect.bottom - ptScreen.y <= borderWidth)
					return HTBOTTOMRIGHT;
				return HTRIGHT;
			}
			else
			{
				if(ptScreen.y - rect.top <= borderWidth)
					return HTTOP;
				else if(rect.bottom - ptScreen.y <= borderWidth)
					return HTBOTTOM;
			}
		}
	}

	if(m_bCaption && m_nCaptionHeight)
	{
		if(ptScreen.y - m_y < m_nCaptionHeight) return HTCAPTION;
		return HTCLIENT;
	}
	return HTCAPTION;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pszFileName : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
// ±£¥Ê¥∞ø⁄≈‰÷√Œ™XMLŒƒº˛
HRESULT CGuiDialog::SaveGuiToXML(const char* pszFileName)
{
	TiXmlDocument doc;
	if(!doc.LoadFile(pszFileName))
		doc.InsertEndChild(TiXmlDeclaration("1.0","gb2312",""));

	TiXmlElement* gui = XML_FindAndAddChildNode							(&doc,"gui");

	/*
	const char* szComment1 = "\r\n\
	enum GUI_CONTROL_TYPE \r\n\
	{ \r\n\
	GUI_CONTROL_BUTTON		=	0, \r\n\
	GUI_CONTROL_STATIC		=	1, \r\n\
	GUI_CONTROL_CHECKBOX	=	2,\r\n\
	GUI_CONTROL_RADIOBUTTON	=	3,\r\n\
	GUI_CONTROL_COMBOBOX	=	4,\r\n\
	GUI_CONTROL_SLIDER		=	5,\r\n\
	GUI_CONTROL_EDITBOX		=	6,\r\n\
	GUI_CONTROL_MLEDITBOX	=	7,\r\n\
	GUI_CONTROL_LISTBOX		=	8,\r\n\
	GUI_CONTROL_SCROLLBAR	=	9,\r\n\
	GUI_CONTROL_IMAGE		=	10,\r\n\
	GUI_CONTROL_ITEMCELL	=	11,\r\n\
	GUI_CONTROL_TABLE		=	12,\r\n\
	\r\n\
	// Ãÿ ‚øÿº˛\r\n\
	GUI_CONTROL_CLOSEBOX	=	-1;\r\n\
	};\r\n\
	\r\n\
	enum GUI_CONTROL_STATE\r\n\
	{\r\n\
	GUI_STATE_NORMAL	=	0,\r\n\
	GUI_STATE_DISABLED	=	1,\r\n\
	GUI_STATE_HIDDEN	=	2,\r\n\
	GUI_STATE_FOCUS		=	3,\r\n\
	GUI_STATE_MOUSEOVER	=	4,\r\n\
	GUI_STATE_PRESSED	=	5,\r\n\
	};";

	TiXmlComment* pComment = XML_FindAndAddComment(gui);
	pComment->SetValue(szComment1);
	//*/

	TiXmlElement* dialog = XML_FindAndAddMatchingChildNode(gui,"dialog","id",GetID());

	if(strcmp(GetName(),""))
		dialog->SetAttribute("name",GetName());

	XML_SetAttribute(dialog,"style",GetStyle(),16);
	dialog->SetAttribute("caption",m_szCaption);

	stRectI dlgRect(m_x,m_y,m_x + m_width,m_y + m_height);
	char szRect[64];
	dlgRect.ToString(szRect,sizeof(szRect));
	dialog->SetAttribute("rect",szRect);
	/*
	dialog->SetAttribute("x",m_x);
	dialog->SetAttribute("y",m_y);
	dialog->SetAttribute("width",m_width);
	dialog->SetAttribute("height",m_height);
	//*/

	XML_SetColorAttribute(dialog,"colortopleft",m_colorTopLeft);
	XML_SetColorAttribute(dialog,"colortopright",m_colorTopRight);
	XML_SetColorAttribute(dialog,"colorbottomleft",m_colorBottomLeft);
	XML_SetColorAttribute(dialog,"colorbottomright",m_colorBottomRight);
	XML_SetAttribute(dialog,"align",m_eAlign,16);
	TiXmlElement* controls = XML_FindAndAddChildNode(dialog,"controls");

	const char* szScript = this->GetScript();
	if(szScript)
	{
		TiXmlElement* script = XML_FindAndAddChildNode(dialog,"script");
		XML_SetNodeText(script,szScript);
	}

	for(size_t i=0;i<m_Controls.size();++i)
	{
		CGuiControl* pControl = m_Controls[i];
		if(pControl->GetID() == GUI_CLOSEBOX_ID) continue;
		TiXmlElement* item = XML_FindAndAddMatchingChildNode(controls,"item","id",pControl->GetID());

		item->SetAttribute("type",GetGuiControlTypeDesc(pControl->GetType()));
		XML_SetAttribute(item,"style",pControl->GetStyle(),16);

		/*
		item->SetAttribute("x",pControl->GetX());
		item->SetAttribute("y",pControl->GetY());
		item->SetAttribute("width",pControl->GetWidth());
		item->SetAttribute("height",pControl->GetHeight());
		//*/
		stRectI controlRect(pControl->GetX(),pControl->GetY(),pControl->GetX() + pControl->GetWidth(),pControl->GetY() + pControl->GetHeight());
		controlRect.ToString(szRect,sizeof(szRect));
		item->SetAttribute("rect",szRect);
		item->SetAttribute("hotkey",pControl->GetHotkey().GetAccelKey().c_str());
		item->SetAttribute("isdefault",(int)pControl->m_bIsDefault);
		//XML_SetAttribute(item,"textformat",pControl->GetTextFromat(),16);

		XML_SetColorAttribute(item,"color",pControl->GetColor());
		XML_SetColorAttribute(item,"textcolor",pControl->GetTextColor());

		switch(pControl->GetType())
		{
		case GUI_CONTROL_BUTTON:
			{
				XML_SetNodeText(item,pControl->GetText());
				CGuiButton* pBtn = (CGuiButton*)pControl;
				if(pBtn->GetButtonType() == CGuiButton::ButtonType_Bitmap && pBtn->GetElementCount() > 0 && pBtn->GetElement(0)->pBmp )
				{
					IBitmaps* pBmp = pBtn->GetElement(0)->pBmp;
					item->SetAttribute("pack",pBmp->GetPackName());
					item->SetAttribute("group",(int)pBmp->GetGroup());
					item->SetAttribute("frame",(int)pBmp->GetFrame());
					item->SetAttribute("framenum",(int)pBtn->GetElementCount());
				}
			}
			break;
		case GUI_CONTROL_STATIC: 
			XML_SetNodeText(item,pControl->GetText());
			break;
		case GUI_CONTROL_CHECKBOX:
			XML_SetNodeText(item,pControl->GetText());
			break;
		case GUI_CONTROL_RADIOBUTTON:
			{
				CGuiRadioButton* pRadio = (CGuiRadioButton*)pControl;
				item->SetAttribute("group",pRadio->GetButtonGroup());
				XML_SetNodeText(item,pControl->GetText());
			}
			break;
		case GUI_CONTROL_COMBOBOX:
			{
				CGuiComboBox* pComboBox = (CGuiComboBox*)pControl;
				if(pComboBox->GetItemCount())
				{
					TiXmlElement* datas = XML_FindAndAddChildNode(item,"datas");
					datas->Clear();
					for(int i=0;i<pComboBox->GetItemCount();++i)
					{
						TiXmlElement* item = XML_FindAndAddMatchingTextChildNode(datas,"item",pComboBox->GetItemText(i));
						//item->SetAttribute("data",(int)pComboBox->GetItemData(i));
					}
				}
			}
			break;
		case GUI_CONTROL_SLIDER:
			{
				CGuiSlider* pSlider = (CGuiSlider*)pControl;
				item->SetAttribute("min",pSlider->GetRangeMin());
				item->SetAttribute("max",pSlider->GetRangeMax());
			}
			break;
		case GUI_CONTROL_EDITBOX:
			{
				CGuiEditBox* pEdit = (CGuiEditBox*)pControl;
				item->SetAttribute("maxCharCount",pEdit->GetMaxCharCount());
				if( pEdit->GetStyle() & CGuiEditBox::GUI_ES_NUMBER )
				{
					item->SetAttribute("minNumber",pEdit->GetMinNumber());
					item->SetAttribute("maxNumber",pEdit->GetMaxNumber());
				}
				XML_SetColorAttribute(item,"caretcolor",pEdit->GetCaretColor());
			}
			break;
		case GUI_CONTROL_MLEDITBOX:
			break;
		case GUI_CONTROL_MLTEXTBOX:
			break;
		case GUI_CONTROL_LISTBOX:
			{
				CGuiListBox* pListBox = (CGuiListBox*)pControl;
				if(pListBox->GetItemCount())
				{
					TiXmlElement* datas = XML_FindAndAddChildNode(item,"datas");
					datas->Clear();
					for(int i=0;i<pListBox->GetItemCount();++i)
					{
						XML_FindAndAddMatchingTextChildNode(datas,"item",pListBox->GetItemText(i));
					}
				}
			}
			break;
		case GUI_CONTROL_TAB:
			{
				CGuiTab* pListBox = (CGuiTab*)pControl;
				if(pListBox->GetItemCount())
				{
					TiXmlElement* datas = XML_FindAndAddChildNode(item,"datas");
					datas->Clear();
					for(int i=0;i<pListBox->GetItemCount();++i)
					{
						XML_FindAndAddMatchingTextChildNode(datas,"item",pListBox->GetItemText(i));
					}
				}
			}
			break;
		case GUI_CONTROL_SCROLLBAR:
			{
				CGuiScrollBar* pScroll = (CGuiScrollBar*)pControl;
				item->SetAttribute("min",pScroll->GetTrackStart());
				item->SetAttribute("max",pScroll->GetTrackEnd());
				item->SetAttribute("pagesize",pScroll->GetPageSize());
			}
			break;
		case GUI_CONTROL_SCROLLTAB:
			{
				CGuiScrollTab* pListBox = (CGuiScrollTab*)pControl;
				if(pListBox->GetItemCount())
				{
					TiXmlElement* datas = XML_FindAndAddChildNode(item,"datas");
					datas->Clear();
					for(int i=0;i<pListBox->GetItemCount();++i)
					{
						XML_FindAndAddMatchingTextChildNode(datas,"item",pListBox->GetItemText(i));
					}
				}
			}
			break;
		case GUI_CONTROL_IMAGE:
			{
				CGuiImage* pImage = (CGuiImage*)pControl;
				if(pImage->GetImgPackName())
				{
					item->SetAttribute("pack",pImage->GetImgPackName());
					item->SetAttribute("group",pImage->GetImgGroup());
					item->SetAttribute("frame",pImage->GetImgFrame());
					item->SetAttribute("backgroud",(int)pImage->m_bIsBack);
				}
			}
			break;
		case GUI_CONTROL_TABLE:
			{
				CGuiTable* pTable = (CGuiTable*)pControl;
				item->SetAttribute("cellWidth",pTable->m_cellWidth);
				item->SetAttribute("cellHeight",pTable->m_cellHeight);
				item->SetAttribute("rowCount",pTable->m_rowCount);
				item->SetAttribute("showRowCount",pTable->m_showRowCount);
				item->SetAttribute("colCount",pTable->m_colCount);
				item->SetAttribute("virtual",(int)pTable->m_bVirtual);
				XML_SetRectAttribute(item,"itemrect",&pTable->m_itemRect);
			}
			break;
		}
	}

	if(doc.SaveFile(pszFileName))
		return S_OK;

	return S_FALSE;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
CGuiDialog* CGuiDialog::GetNextDialog()
{
	return GetGuiManager()->GetNextDialog(this);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
CGuiDialog* CGuiDialog::GetPrevDialog()
{
	return GetGuiManager()->GetPrevDialog(this);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
bool CGuiDialog::IsFocus()
{
	return GetGuiManager()->IsFocus(this);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pt : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::ClientToScreen(POINT* pt)
{
	pt->x += m_x;
	pt->y += m_y;
	if(m_bCaption)
		pt->y += m_nCaptionHeight;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param rc : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::ClientToScreen(RECT* rc)
{
	rc->left += m_x;
	rc->right += m_x;
	rc->top += m_y;
	rc->bottom += m_y;
	if(m_bCaption)
	{
		rc->top += m_nCaptionHeight;
		rc->bottom += m_nCaptionHeight;
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pt : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::ScreenToClient(POINT* pt)
{
	pt->x -= m_x;
	pt->y -= m_y;
	if(m_bCaption)
		pt->y -= m_nCaptionHeight;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param rc : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::ScreenToClient(RECT* rc)
{
	rc->left -= m_x;
	rc->right -= m_x;
	rc->top -= m_y;
	rc->bottom -= m_y;
	if(m_bCaption)
	{
		rc->top -= m_nCaptionHeight;
		rc->bottom -= m_nCaptionHeight;
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::OnSetCloseLocation()
{
	if(m_pBtnClose)
	{
		if(m_pBackImage)
		{
			m_pBtnClose->SetLocation( GetWidth() - 31, 88);
			m_pBtnClose->SetSize(32,33);
		}
		else if(m_bSmDialog)
		{
			m_pBtnClose->SetLocation( - m_clientOffsetTopLeft.x + GetWidth() - 31, - m_clientOffsetTopLeft.y - 1 );
			m_pBtnClose->SetSize(32,33);
		}
		else
		{
			m_pBtnClose->SetLocation( - m_clientOffsetTopLeft.x + GetWidth() - 31, - m_clientOffsetTopLeft.y + 9);
			m_pBtnClose->SetSize(32,33);
		}
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::UpdateClientRect()
{
	if(m_pBackImage)
	{
		m_borderTopLeft = stPointI(0,0);
		m_borderRightBottom = stPointI(0,0);
		m_clientOffsetTopLeft = stPointI(0,0);
		m_clientOffsetRightBottom = stPointI(0,0);
		m_width = m_pBackImage->GetX() + m_pBackImage->GetWidth();
		m_height = m_pBackImage->GetY() + m_pBackImage->GetHeight();
	}
	else if(m_bSmDialog)
	{
		m_borderTopLeft = s_smBorderTopLeft;
		m_borderRightBottom = s_smBorderRightBottom;
		m_clientOffsetTopLeft = s_smClientOffsetTopLeft;
		m_clientOffsetRightBottom = s_smClientOffsetRightBottom;
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::UpdateLocation(bool bReAlign)
{
	if(bReAlign)
	{
		if(m_eAlign & GUI_ALIGN_TOP)
		{
			m_y = 0;
		}
		else if(m_eAlign & GUI_ALIGN_VCENTER)
		{
			m_y = (GetDevice()->GetHeight()-GetHeight())/2;
		}
		else if(m_eAlign & GUI_ALIGN_BOTTOM)
		{
			m_y = GetDevice()->GetHeight() - GetHeight();
		}

		if(m_eAlign & GUI_ALIGN_LEFT)
		{
			m_x = 0;
		}
		else if(m_eAlign & GUI_ALIGN_HCENTER)
		{
			m_x = (GetDevice()->GetWidth() - GetWidth())/2;
		}
		else if(m_eAlign & GUI_ALIGN_RIGHT)
		{
			m_x = GetDevice()->GetWidth() - GetWidth();
		}
	}

	UpdateClientRect();

	stRectI rc1(0,0,GetWidth(),GetHeight());
	CuttRect9(rc1,m_borderTopLeft,m_borderRightBottom,m_BackRects);
	for(size_t i=0;i<9;++i)
	{
		m_BackRects[i] &= rc1;
	}
	if(m_pBackImage) 
	{
		m_clientOffsetTopLeft = stPointI(0,0);
	}
	OnSetCloseLocation();
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param script : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SetScript(const char* script)
{
#ifdef _DEBUG
	m_strScript = script;
#endif
	TRACE(script);
	TRACE("\n");
	if(str_isempty(script)) return;
	if(m_lua == NULL)
	{
		m_lua = new CLuaState();
		bind_lua(*m_lua);
		CGuiControl::RegisterScript(*m_lua);
		GuiScriptInit(*m_lua);
	}
	m_lua->DoString(script);
	OnInitScript();
	//m_lua->Do();
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::OnInitScript()
{
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nIDFirstButton : √Ë ˆ
* \param nIDLastButton : √Ë ˆ
* \param nIDCheckButton : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::CheckRadioButton(int nIDFirstButton,int nIDLastButton,int nIDCheckButton )
{
	CGuiCheckBox* pCheckBox = GetRadioButton(nIDCheckButton);
	if(pCheckBox) pCheckBox->SetChecked(true);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nIDFirstButton : √Ë ˆ
* \param nIDLastButton : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
int CGuiDialog::GetCheckedRadioButton(int nIDFirstButton,int nIDLastButton )
{
	for(int nID = nIDFirstButton; nID <= nIDLastButton;nID++)
	{
		CGuiCheckBox* pCheckBox = GetRadioButton(nID);
		if(pCheckBox && pCheckBox->GetChecked())
		{
			return nID - nIDFirstButton;
		}
	}
	return 0;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param b : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SetEnabled(bool b)
{
	m_bEnabled = b;
	if(!b)
	{
		if(IsFocus()) GetGuiManager()->ResetFocus();
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param b : √Ë ˆ
* \param bResetZOrder : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SetVisible(bool b,bool bResetZOrder)
{
	if(m_bVisible != b)
	{
		m_bVisible = b;
		if(b)
		{
			if(bResetZOrder)
			{
				if(!m_bTopMost && !m_bBottomMost)
					SetZOrder(GUI_DIALOG_TOP);
			}
			GetGuiManager()->ResetFocus();
			m_bPlayCloseSound = false;
			m_bPlayOpenSound = true;
			SendEvent(EVENT_DIALOG_SHOW,true,NULL);
		}
		else
		{
			if(this == GetGuiManager()->GetMouseDlg())
				GetGuiManager()->SetMouseDlg(NULL);
			if(this == GetGuiManager()->GetMouseOverDlg())
				GetGuiManager()->UpdateMouseOverDlg();

			if(IsFocus()) GetGuiManager()->ResetFocus();
			if(m_bPlayOpenSound)
			{
				m_bPlayOpenSound = false;
				m_bPlayCloseSound = false;
			}
			else
			{
				m_bPlayCloseSound = true;
			}
			SendEvent(EVENT_DIALOG_HIDE,true,NULL);
		}
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param rect : √Ë ˆ
* \param dwControlType : √Ë ˆ
* \param color : √Ë ˆ
* \param bDrawCenter : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::DrawControlBorder(const RECT & rect,DWORD dwControlType,DWORD color,bool bDrawCenter)
{
	// No need to draw fully transparent layers
	if( 0 == (color & 0xff000000))
		return ;

	if(color == -1) color = m_dwColor;

	stRectI rcScreen = rect;
	rcScreen.OffsetRect(m_x,m_y);

	// If caption is enabled, offset the Y position by its height.
	rcScreen.OffsetRect(m_clientOffsetTopLeft.x,m_clientOffsetTopLeft.y);

	stGuiCtrlBorderInfo* pInfo = GetCtrlBorderInfo(dwControlType);
	if(!pInfo) return;

	stRectI rects[9];
	CuttRect9(rcScreen,pInfo->ptLeftTop,pInfo->ptRightBottom,rects);
	int count = 9;
	int i;
	if( bDrawCenter ) i=0;
	else i = 1;

	if( dwControlType != GUI_CONTROL_EDITBOX && dwControlType != GUI_CONTROL_TABLE && dwControlType != GUI_CONTROL_TAB && dwControlType != GUI_CONTROL_SCROLLTAB)
	{
		stRectI rc1 = rcScreen;
		rc1.InflateRect(-3,-3);
		PatchRenderImage(rc1,s_controlBackGroundElement.pBmp,&s_controlBackGroundElement.rcTexture,color);
	}

	for( ;i<count;++i )
	{
		::PatchRenderImage(rects[i],pInfo->element[i].pBmp,&pInfo->element[i].rcTexture,color,pInfo->element[i].blend,s_PatchAlignFormat[i]);
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param width : √Ë ˆ
* \param height : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SetSize( int width, int height )
{ 
	m_width = width; m_height = height;  
	SendEvent(EVENT_DIALOG_RESIZE,true,NULL);
	UpdateLocation();
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param x : √Ë ˆ
* \param y : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SetLocation( int x, int y ) 
{ 
	m_x = x;
	m_y = y; 
	UpdateLocation();
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
DWORD CGuiDialog::GenerateControlID()
{
	int maxID = 0;
	for(size_t i=0;i<m_Controls.size();++i)
	{
		if(m_Controls[i]->GetID() > maxID)
		{
			maxID = m_Controls[i]->GetID();
		}
	}

	return getMax((long)(maxID+1),(long)1000);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param color : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::SetColor(DWORD color)
{ 
	m_dwColor = color;
	if(m_pBackImage)
	{
		m_pBackImage->SetColor(color);
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param iCtrlType : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
stGuiCtrlBorderInfo * CGuiDialog::GetDefaultCtrlBorderInfo(int iCtrlType)
{
	t_GuiMapCtrlBorder::iterator it = s_mapCtrlBorder.find(iCtrlType);
	if(it != s_mapCtrlBorder.end())
	{
		return it->second;
	}
	return (s_mapCtrlBorder[iCtrlType] = new stGuiCtrlBorderInfo);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param iCtrlType : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
stGuiCtrlBorderInfo * CGuiDialog::GetCustomCtrlBorderInfo(int iCtrlType)
{
	t_GuiMapCtrlBorder::iterator it = m_mapCtrlBorder.find(iCtrlType);
	if(it != m_mapCtrlBorder.end())
	{
		return it->second;
	}
	return (m_mapCtrlBorder[iCtrlType] = new stGuiCtrlBorderInfo);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param iCtrlType : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
stGuiCtrlBorderInfo * CGuiDialog::GetCtrlBorderInfo(int iCtrlType)
{
	t_GuiMapCtrlBorder::iterator it = m_mapCtrlBorder.find(iCtrlType);
	if(it == m_mapCtrlBorder.end())
	{
		it = s_mapCtrlBorder.find(iCtrlType);
		if(it == s_mapCtrlBorder.end())
			return NULL;
	}
	return it->second;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pInfo : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
bool CGuiDialog::BeginAnimation(stGuiAnimationInfo* pInfo)
{
	if(m_pAniInfo == NULL)
		m_pAniInfo = new stGuiAnimationInfoEx;

	m_pAniInfo->pos = GetLocation();
	memcpy(m_pAniInfo,pInfo,sizeof(*pInfo));
	m_pAniInfo->fRunTime = 0;
	m_pAniInfo->dx = (pInfo->ptDst.x - m_pAniInfo->pos.x)/pInfo->fTime;
	m_pAniInfo->dy = (pInfo->ptDst.y - m_pAniInfo->pos.y)/pInfo->fTime;

	int a,r,g,b,a1,r1,g1,b1;
	DWORD color = GetColor();

	a = COLOR_GET_ALPHA(color);
	r = COLOR_GET_R(color);
	g = COLOR_GET_G(color);
	b = COLOR_GET_B(color);

	a1 = COLOR_GET_ALPHA(pInfo->dwColorDst);
	r1 = COLOR_GET_R(pInfo->dwColorDst);
	g1 = COLOR_GET_G(pInfo->dwColorDst);
	b1 = COLOR_GET_B(pInfo->dwColorDst);

	m_pAniInfo->a = (float)a;
	m_pAniInfo->r = (float)r;
	m_pAniInfo->g = (float)g;
	m_pAniInfo->b = (float)b;

	m_pAniInfo->da = (a1 - a)/pInfo->fTime;
	m_pAniInfo->dr = (r1 - r)/pInfo->fTime;
	m_pAniInfo->dg = (g1 - g)/pInfo->fTime;
	m_pAniInfo->db = (b1 - b)/pInfo->fTime;

	return true;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param fElaspedTime : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::RunAnimation(float fElaspedTime)
{
	if(!m_pAniInfo) return;

	m_pAniInfo->fRunTime += fElaspedTime;
	if(m_pAniInfo->fRunTime >= m_pAniInfo->fTime)
	{
		m_x = m_pAniInfo->ptDst.x;
		m_y = m_pAniInfo->ptDst.y;
		SetColor(m_pAniInfo->dwColorDst);
		switch(m_pAniInfo->eAniComplete)
		{
		case GuiAniEvent_Close:
			Close();
			break;
		case GuiAniEvent_Hide:
			SetVisible(false);
			break;
		case GuiAniEvent_Anchor:
			m_bAnchor = true;
			break;
		case GuiAniEvent_StopAnchor:
			m_dwTimeAnchor = xtimeGetTime();
			break;
		case GuiAniEvent_RunFun:
			OnCompleteAniEvent();
			break;
		}
		delete m_pAniInfo;
		m_pAniInfo = NULL;
	}
	else
	{
		m_pAniInfo->pos.x += fElaspedTime * m_pAniInfo->dx;
		m_pAniInfo->pos.y += fElaspedTime * m_pAniInfo->dy;

		m_pAniInfo->a += fElaspedTime * m_pAniInfo->da;
		if(m_pAniInfo->a < 0) m_pAniInfo->a = 0;
		else if(m_pAniInfo->a > 255) m_pAniInfo->a = 255;

		m_pAniInfo->r += fElaspedTime * m_pAniInfo->dr;
		if(m_pAniInfo->r < 0) m_pAniInfo->r = 0;
		else if(m_pAniInfo->r > 255) m_pAniInfo->r = 255;

		m_pAniInfo->g += fElaspedTime * m_pAniInfo->dg;
		if(m_pAniInfo->g < 0) m_pAniInfo->g = 0;
		else if(m_pAniInfo->g > 255) m_pAniInfo->g = 255;

		m_pAniInfo->b += fElaspedTime * m_pAniInfo->db;
		if(m_pAniInfo->b < 0) m_pAniInfo->b = 0;
		else if(m_pAniInfo->b > 255) m_pAniInfo->b = 255;

		//SetLocation(m_pAniInfo->pos.x,m_pAniInfo->pos.y);
		m_x = (int)m_pAniInfo->pos.x;
		m_y = (int)m_pAniInfo->pos.y;
		DWORD a,r,g,b;
		a = (DWORD)m_pAniInfo->a;
		r = (DWORD)m_pAniInfo->r;
		g = (DWORD)m_pAniInfo->g;
		b = (DWORD)m_pAniInfo->b;
		SetColor(COLOR_ARGB(a,r,g,b));

		SendEvent(EVENT_DIALOG_MOVE,true,NULL);
	}
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::OnResetScreenSize()
{
	if(m_x >= GetDevice()->GetWidth()) m_x = GetDevice()->GetWidth() - GetWidth();
	if(m_y >= GetDevice()->GetHeight()) m_y = GetDevice()->GetHeight() - GetHeight();
	UpdateLocation();
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::EndPopup(void)
{
	SetVisible(false);
	//GetGuiManager()->SetPopupDlg(NULL);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
void CGuiDialog::BeginPopup(void)
{
	GetGuiManager()->SetPopupDlg(this);
	SetVisible(true,false);
	SetZOrder(GUI_DIALOG_TOP);
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
CGuiMenu* CGuiDialog::GetPopupMenu()
{
	return m_pPopupMenu;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pMenu : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
bool CGuiDialog::SetPopupMenu(CGuiMenu* pMenu)
{
	if(!pMenu)
	{
		if(m_pPopupMenu) m_pPopupMenu->SetOwner(NULL);
		m_pPopupMenu = NULL;
	}
	else
	{
		m_pPopupMenu = pMenu;
		m_pPopupMenu->SetOwner(this);
	}

	return true;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param rl : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
bool CGuiDialog::SetIcon(stResourceLocation * rl)
{
	m_pIcon = GetDevice()->FindBitmaps(rl);
	return m_pIcon != NULL;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pTab : √Ë ˆ
* \param frame : √Ë ˆ
* \param ctrlID : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
bool CGuiDialog::AddControl2Tab(CGuiTab *pTab,int frame,int ctrlID)
{
	Assert(pTab);
	CGuiControl* pControl = GetControl(ctrlID);
	if(!pControl) return false;

	pTab->AddControl(frame,pControl);
	return true;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param tabID : √Ë ˆ
* \param frame : √Ë ˆ
* \param ctrlID : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
bool CGuiDialog::AddControl2TabByID(int tabID,int frame,int ctrlID)
{
	//TRACE("[%d][%d][%d]\n",tabID,frame,ctrlID);
	CGuiTab* pTab = GetTab(tabID);
	if(pTab) return AddControl2Tab(pTab,frame,ctrlID);
	return false;
}


int CGuiDialog::ControlToIndex(CGuiControl* pControl)
{
	if(pControl == NULL) return m_Controls.empty() ? -1 : 0;
	for(int i=0;(size_t)i<m_Controls.size();++i)
	{
		if(m_Controls[i] == pControl) return i;
	}
	return -1;
}

void CGuiDialog::SetButtonGroup(int button,int group)
{
	CGuiButton* pBtn = GetButton(button);
	if(pBtn) pBtn->SetButtonGroup(group);
}

lua_State* CGuiDialog::GetScriptEngine()
{
	if(m_lua)
		return *m_lua;
	return NULL;
}