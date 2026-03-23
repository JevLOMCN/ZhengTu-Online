/**
 * \file      GuiManager.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2005-07-05 17:02:57
 * \brief     GUI¹ÜÀíÀà
 * 
 *	      GUI¹ÜÀíÀà
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../include/GuiFloatItem.h"
#include "../../xml_parse/XmlParseEx.h"
#include "../include/ToolTips.h"

#pragma comment (lib , "Imm32.lib")

//-------------------------------------------------------------
// Ä¬ÈÏµã»÷ÉùÒô
int g_defaultButtonClickSound = 0;
int g_defaultTabClickSound    = 0;
int g_defaultDlgOpenSound     = 0;
int g_defaultDlgHideSound     = 0;
//-------------------------------------------------------------
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiManager* GetGuiManager()
{
	static CGuiManager manager;
	return &manager;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiManager::CGuiManager(void)
: m_pFocusDlg(NULL)
, m_pMouseDlg(NULL)
, m_pToolTips(NULL)
, m_pPopupDlg(NULL)
, m_ptToolTips(0,0)
, m_pEditMenu(NULL)
, m_bEnableIme(true)
{
	//CGuiDialog::InitDefaultElements();

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiManager::~CGuiManager(void)
{
	CloseAllDialog();
	CGuiDialog::UninitDefaultElement();
}

void CGuiManager::InitCreate()
{
	CGuiDialog::InitDefaultElements();
	CGuiMenu::InitDefaultElements();
	CToolTips::InitBorderTexture();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::Init()
{
	//CGuiDialog::SaveDefaultElementToXML("c:\\GameSkin.xml");
	m_pEditMenu = CreateMenu( NULL );
	m_pEditMenu->AddMenuItem(ID_EDIT_SELECT_ALL,"È«Ñ¡\tCtrl+A");
	m_pEditMenu->AddMenuItem(-1,"",CGuiMenu::MenuStyle_Separate);
	m_pEditMenu->AddMenuItem(ID_EDIT_CUT,"¼ôÇĞ\tCtrl+X");
	m_pEditMenu->AddMenuItem(ID_EDIT_COPY,"¸´ÖÆ\tCtrl+C");
	m_pEditMenu->AddMenuItem(ID_EDIT_PASTE,"Õ³Ìû\tCtrl+V");
	m_pEditMenu->AddMenuItem(ID_EDIT_CLEAR,"É¾³ı");
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDlg : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::AppendDialog(CGuiDialog* pDlg)
{
	m_Dialogs.push_back(pDlg);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pOwner : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiMenu* CGuiManager::CreateMenu(CGuiDialog* pOwner)
{
	CGuiMenu* pDlg = new CGuiMenu;
	HRESULT hr = AddDialog(this->GenerateDialogID(),0,0,0,0,GUI_WS_TOPMOST|GUI_WS_NOMOVE|GUI_WS_NOSKIN,NULL,NULL,pDlg);
	if(FAILED(hr))
	{
		delete pDlg;
		return NULL;
	}
	pDlg->SetVisible(false);
	return pDlg;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ID : ÃèÊö
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \param width : ÃèÊö
 * \param height : ÃèÊö
 * \param dwStyle : ÃèÊö
 * \param pszCaption : ÃèÊö
 * \param pszName : ÃèÊö
 * \param pDialog : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
static bool s_bLoadDlg = false;
HRESULT CGuiManager::AddDialog(int ID,int x,int y,int width,int height,DWORD dwStyle,const char* pszCaption,const char* pszName,CGuiDialog* pDialog)
{
	//Backup old focus dialog and corresponding control
	CGuiDialog* pCurFocusDialog = NULL;
	CGuiControl* pCurFocusControl = NULL;
	if ( m_pFocusDlg )
	{
		pCurFocusDialog = m_pFocusDlg;
		pCurFocusControl = m_pFocusDlg->GetControlFocus();	
	}
	
	//Add this dlg
	CGuiDialog * pDlg = pDialog;
	pDlg->m_ID = ID;
	pDlg->m_dwStyle = dwStyle;
	pDlg->InitCustomElements();
	pDlg->SetLocation(x,y);
	pDlg->SetSize(width,height);
	if(pszCaption && pszCaption[0])
	{
		pDlg->m_bCaption = true;
		pDlg->SetCaptionText(pszCaption);
	}
	else
	{
		pDlg->m_bCaption = false;
		pDlg->SetCaptionHeight(0);
	}

	if( (dwStyle & GUI_WS_TOPMOST) || (m_Dialogs.size() <= 0) )
	{
		AppendDialog(pDlg);
	}
	else if( dwStyle & GUI_WS_BOTTOMMOST)
	{
		InsertDialog(pDlg,0);
	}
	else
	{
		int index = GetLastTopMostDlgIndex();
		if(index == -1) AppendDialog(pDlg);
		else InsertDialog(pDlg,index+1);
	}

	if(dwStyle & GUI_WS_TOPMOST)
		pDlg->m_bTopMost = true;
	else if(dwStyle & GUI_WS_BOTTOMMOST)
		pDlg->m_bBottomMost = true;

	if(pszName)
		pDlg->SetName(pszName);

	if(pDlg == m_Dialogs[m_Dialogs.size()-1]) RequestFocus(pDlg);

	if(!s_bLoadDlg)
		pDlg->OnCreate();

	//Reset focus to old edit control
	if ( pCurFocusDialog && pCurFocusControl && (pCurFocusControl->GetType() == GUI_CONTROL_EDITBOX) )
	{
		RequestFocus( pCurFocusDialog );
		pCurFocusDialog->SetFocus( pCurFocusControl );
	}

	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ID : ÃèÊö
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \param width : ÃèÊö
 * \param height : ÃèÊö
 * \param dwStyle : ÃèÊö
 * \param pszCaption : ÃèÊö
 * \param pszName : ÃèÊö
 * \param ppCteated : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiManager::AddDialog(int ID,int x,int y,int width,int height,DWORD dwStyle,const char* pszCaption,const char* pszName,CGuiDialog** ppCteated)
{
	CGuiDialog * pDlg = new CGuiDialog(dwStyle);
	HRESULT hr = AddDialog(ID,x,y,width,height,dwStyle,pszCaption,pszName,pDlg);
	if(ppCteated)
		*ppCteated = pDlg;

	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiManager::GetLastTopMostDlgIndex()
{
	for(int i=m_Dialogs.size()-1; i>= 0; --i)
	{
		CGuiDialog* pDlg = m_Dialogs[i];
		if(!pDlg->m_bTopMost) break;
	}
	if(i + 1 >= m_Dialogs.size()) return -1;
	return i + 1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiManager::GetFirstTopMostDlgIndex()
{
	if(m_Dialogs.size() <= 0) return -1;
	if(m_Dialogs[m_Dialogs.size()-1]->m_bTopMost) return m_Dialogs.size()-1;
	return -1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiManager::GetLastBottomMostDlgIndex()
{
	for(int i=0;i<m_Dialogs.size();++i)
	{
		CGuiDialog* pDlg = m_Dialogs[i];
		if(!pDlg->m_bBottomMost) break;
	}
	return i - 1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiManager::GetFirstBottomMostDlgIndex()
{
	if(m_Dialogs.size() <= 0) return -1;
	if(m_Dialogs[0]->m_bBottomMost) return 0;
	return -1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiDialog * CGuiManager::GetTopDlg()
{
	return IndexToDialog(m_Dialogs.size()-1);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiDialog * CGuiManager::GetBottomDlg()
{
	return IndexToDialog(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDlg : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiManager::CanHaveFocus(CGuiDialog * pDlg)
{
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDlg : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::RequestFocus(CGuiDialog * pDlg)
{
	if(m_pFocusDlg == pDlg) 
	{
		//pDlg->ResetFocus();
		return;
	}

	Assert(pDlg);
	if(m_pFocusDlg) m_pFocusDlg->OnFocusOut();
	m_pFocusDlg = pDlg;
	pDlg->OnFocusIn();
	pDlg->ResetFocus();
	if(!pDlg->m_bTopMost && !pDlg->m_bBottomMost)
		pDlg->SetZOrder(GUI_DIALOG_TOP);
	//if(CGuiDialog::s_pControlFocus != pDlg)
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDlg : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::RemoveDlg(CGuiDialog * pDlg)
{
	int index = DialogToIndex(pDlg);
	Assert(index >= 0 && index < m_Dialogs.size());
	
	if(index < 0 || index >= m_Dialogs.size())
	{
		return;
	}
	
	delete pDlg;
	
	m_Dialogs.erase(m_Dialogs.begin() + index);
	
	if(pDlg == m_pFocusDlg) 
	{
		m_pFocusDlg = NULL;
		ResetFocus();
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::Render(float fElapsedTime)
{
	static HIMC hImc = NULL;
	CGuiControl* pControl = CGuiDialog::GetControlFocus();
	if(!pControl || !pControl->IsVisible() || !pControl->IsEnabled() || 
		!pControl->m_pDialog->IsVisible() || !pControl->m_pDialog->IsEnabled() || 
		( pControl->GetType() != GUI_CONTROL_EDITBOX 
		  && pControl->GetType() != GUI_CONTROL_MLEDITBOX ))
	{
		if(m_bEnableIme)
		{
			m_bEnableIme = false;
			HWND hWnd = Engine_GetHWnd();
			// Get input context for backup. 
			hImc = ImmGetContext(hWnd);

			// Remove association the testing 
			if (hImc)
				ImmAssociateContext(hWnd, NULL);

			// Release input context
			ImmReleaseContext(hWnd, hImc);

			::SetFocus(hWnd);
		}
	}
	else
	{
		if(!m_bEnableIme)
		{
			m_bEnableIme = true;
			HWND hWnd = Engine_GetHWnd();
			if(hImc)
			{
				ImmAssociateContext(hWnd, hImc);
			}
			::SetFocus(hWnd);
		}
	}

	for(int i=0;i<m_Dialogs.size();)
	{
		CGuiDialog* pDlg = m_Dialogs[i];
		if(pDlg->IsClosed())
		{
			RemoveDlg(pDlg);
		}
		else
		{
			if(pDlg->IsVisible() && pDlg != m_pMouseDlg)
				pDlg->OnRender(fElapsedTime);
			++i;
		}
	}

	if(m_pToolTips) 
	{
		if(m_ptToolTips.x < 0) 
			m_ptToolTips.x = 0;
		else if(m_ptToolTips.x + m_pToolTips->GetWidth() > GetDevice()->GetWidth())
			m_ptToolTips.x = GetDevice()->GetWidth() - m_pToolTips->GetWidth();
		
		if(m_ptToolTips.y < 0) 
			m_ptToolTips.y = 0;
		else if(m_ptToolTips.y + m_pToolTips->GetHeight() > GetDevice()->GetHeight())
			m_ptToolTips.y = GetDevice()->GetHeight() - m_pToolTips->GetHeight();

		m_pToolTips->Render(m_ptToolTips.x,m_ptToolTips.y);
		m_pToolTips = NULL;
	}

	if(IsShowCursor())
		Engine_GetCursor()->Render(fElapsedTime);
	else if(m_pMouseDlg)
		m_pMouseDlg->OnRender(fElapsedTime);
}

bool CGuiManager::IsShowCursor()
{
	return m_pMouseDlg == NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pMouseDlg : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::SetMouseDlg(CGuiDialog* pMouseDlg)
{
	m_pMouseDlg = pMouseDlg;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hWnd : ÃèÊö
 * \param uMsg : ÃèÊö
 * \param wParam : ÃèÊö
 * \param lParam : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiManager::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(uMsg == WM_MOUSEWHEEL)
	{
		int xPos,yPos;
		xPos = GET_X_LPARAM(lParam); 
		yPos = GET_Y_LPARAM(lParam); 
		stRectI rect;
		GetClientRect(Engine_GetHWnd(),&rect);
		ClientToScreen(Engine_GetHWnd(),(LPPOINT)&rect);
		xPos -= rect.left;
		yPos -= rect.top;
		lParam = MAKELPARAM(xPos,yPos);

	}

	if(uMsg == WM_RBUTTONUP)
	{
		int iiiii=0;
	}
	if(uMsg == WM_LBUTTONDOWN)
	{
		::SetCapture(Engine_GetHWnd());
	}
	else if(uMsg == WM_LBUTTONUP)
	{
		::ReleaseCapture();
	}
	else if(uMsg == WM_MOUSEMOVE)
	{
		UpdateMouseOverDlg();
		if(m_pMouseOverDlg == NULL)
		{
			SetToolTips(NULL,stPointI(0,0));
		}
		if(wParam & MK_LBUTTON)
		{
			bool bResetCursor = false;
			stPointI pt( (short)LOWORD(lParam) , (short)HIWORD(lParam) );

			if(pt.x < 0) {
				pt.x = 0;
				bResetCursor = true;
			}
			
			if(pt.y < 0) {
				pt.y = 0;
				bResetCursor = true;
			}
			
			if(pt.x >= GetDevice()->GetWidth()){
				pt.x = GetDevice()->GetWndWidth()-1;
				bResetCursor = true;
			}
			
			if(pt.y >= GetDevice()->GetHeight()){
				pt.y = GetDevice()->GetWndHeight()-1;
				bResetCursor = true;
			}

			//if(bResetCursor = true;)
			if(bResetCursor)
			{
				stPointI pt2 = pt;
				GetDevice()->ConverMousePoint(pt2);
				lParam = ( ((DWORD)(short)pt2.y) << 16) | ( (DWORD)(short)pt2.x );
				::ClientToScreen(Engine_GetHWnd(),&pt);
				SetCursorPos(pt.x,pt.y);

			}
		}
	}
	
	if(uMsg == WM_CHAR)
	{
		int iiiiii=0;
	}
	//if(Engine_GetCursor()->MsgProc(uMsg,wParam,lParam))
	//	return true;

	if(m_pFocusDlg && (m_pFocusDlg->GetStyle() & GUI_WS_MODAL))
	{
		return m_pFocusDlg->MsgProc(hWnd,uMsg,wParam,lParam);
	}

	if(uMsg == WM_MOUSEMOVE)
	{
		
		bool b = false;
		if(m_pFocusDlg && m_pFocusDlg->MsgProc(hWnd,uMsg,wParam,lParam))
			return true;
		if(m_pMouseOverDlg && m_pMouseOverDlg != m_pFocusDlg)
			b = m_pMouseOverDlg->MsgProc(hWnd,uMsg,wParam,lParam);
		return b;
	}
	else if(uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN)
	{
		CGuiDialog* pPopupDlg = m_pPopupDlg;

		if(pPopupDlg && pPopupDlg != m_pMouseOverDlg)
		{
			pPopupDlg->EndPopup();
			m_pPopupDlg = NULL;
			return true;
		}

		bool b = false;
		if(m_pMouseOverDlg && m_pMouseOverDlg != m_pFocusDlg && m_pMouseOverDlg->MsgProc(hWnd,uMsg,wParam,lParam))
			b = true;
		if(!b && m_pFocusDlg)
			b = m_pFocusDlg->MsgProc(hWnd,uMsg,wParam,lParam);
		/*if(pPopupDlg && pPopupDlg != m_pMouseOverDlg)
		{
			pPopupDlg->EndPopup();
			m_pPopupDlg = NULL;
			b = true;
		}*/
		return b;
	}
	else
	{
		bool b = false;
		CGuiDialog* pPopupDlg = m_pPopupDlg;
		if(m_pFocusDlg && m_pFocusDlg->IsEnabled() && m_pFocusDlg->IsVisible() && !m_pFocusDlg->IsClosed())
		{
			if(m_pFocusDlg->MsgProc(hWnd,uMsg,wParam,lParam))
				b =  true;
		}
		if(pPopupDlg && ( uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONUP ) ) 
		{
			pPopupDlg->EndPopup();
			m_pPopupDlg = NULL;
			b =  true;
		}
		if(b) return true;
	}
	
	static std::vector<CGuiDialog*> aDialogs;
	aDialogs = m_Dialogs;
	for(int i=aDialogs.size()-1;i>=0;--i)
	{
		if( i >= aDialogs.size()) break;
	
		CGuiDialog* pDlg = aDialogs[i];
		/*if(uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN)
		{
			if(pDlg->IsEnabled() && pDlg->IsVisible() && !pDlg->IsClosed())
				if(pDlg->MsgProc(hWnd,uMsg,wParam,lParam))
					return true;
		}
		else*/
		{
			if(pDlg != m_pFocusDlg)
			{
				if(pDlg->IsEnabled() && pDlg->IsVisible() && !pDlg->IsClosed())
					if(pDlg->MsgProc(hWnd,uMsg,wParam,lParam))
						return true;
			}
		}
	}
	return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDlg : ÃèÊö
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::InsertDialog(CGuiDialog* pDlg,int index)
{
	if(index >= m_Dialogs.size())
	{
		m_Dialogs.push_back(pDlg);
	}
	else
	{
		Assert(index < m_Dialogs.size());

		if(index >= m_Dialogs.size()) return ;
		
		m_Dialogs.push_back(NULL);
		memmove(&m_Dialogs[index+1],&m_Dialogs[index],sizeof(CGuiDialog*) * (m_Dialogs.size() - index-1));
		m_Dialogs[index] = pDlg;
	}
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDlg : ÃèÊö
 * \param pInsertAfter : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::SetDialogZOrder(CGuiDialog * pDlg,CGuiDialog * pInsertAfter)
{
	int index = DialogToIndex(pDlg);
	
	m_Dialogs.erase(m_Dialogs.begin() + index);

	int indexInsert;
	switch(int(pInsertAfter))
	{
	case GUI_DIALOG_TOP:
		{
			if(pDlg->m_bTopMost)
			{
				m_Dialogs.push_back(pDlg);
			}
			else if(pDlg->m_bBottomMost)
			{
				indexInsert = GetLastBottomMostDlgIndex();
				if(indexInsert == -1) indexInsert = 0;
				InsertDialog(pDlg,indexInsert);
			}
			else
			{
				indexInsert = GetLastTopMostDlgIndex();
				if(indexInsert == -1) m_Dialogs.push_back(pDlg);
				else InsertDialog(pDlg,indexInsert);
			}
		}
		break;
	case GUI_DIALOG_BOTTOM:
		{
			if(pDlg->m_bTopMost)
			{
				indexInsert = GetLastTopMostDlgIndex();
				if(indexInsert == -1) m_Dialogs.push_back(pDlg);
				else InsertDialog(pDlg,indexInsert);
			}
			else if(pDlg->m_bBottomMost)
			{
				InsertDialog(pDlg,0);
			}
			else
			{
				indexInsert = GetLastBottomMostDlgIndex();
				indexInsert++;
				if(indexInsert >= m_Dialogs.size()) m_Dialogs.push_back(pDlg);
				else InsertDialog(pDlg,indexInsert);
			}
		}
		break;
	case GUI_DIALOG_TOPMOST:
		{
			m_Dialogs.push_back(pDlg);
			pDlg->m_bTopMost = true;
			pDlg->m_bBottomMost = false;
		}
		break;
	case GUI_DIALOG_NOMOST:
		{
			if(pDlg->m_bTopMost)
			{
				indexInsert = GetLastTopMostDlgIndex();
				if(indexInsert == -1) m_Dialogs.push_back(pDlg);
				else InsertDialog(pDlg,indexInsert);;
			}
			else if(pDlg->m_bBottomMost)
			{
				indexInsert = GetLastBottomMostDlgIndex();
				indexInsert++;
				if(indexInsert >= m_Dialogs.size()) m_Dialogs.push_back(pDlg);
				else InsertDialog(pDlg,indexInsert);
			}
			pDlg->m_bTopMost = false;
			pDlg->m_bBottomMost = false;
		}
		break;
	case GUI_DIALOG_BOTTOMMOST:
		{
			InsertDialog(pDlg,0);
			pDlg->m_bTopMost = false;
			pDlg->m_bBottomMost = true;
		}
		break;
	default:
		{
			InsertDialog(pDlg,index);
		}
		break;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDlg : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::OnCloseDlg(CGuiDialog* pDlg)
{
	if(CGuiDialog::s_pControlFocus && CGuiDialog::s_pControlFocus->m_pDialog == pDlg)
	{
		if(CGuiDialog::s_pControlFocus) CGuiDialog::s_pControlFocus->OnFocusOut();
		CGuiDialog::s_pControlFocus = NULL;
	}
	if(pDlg == m_pMouseDlg)
		m_pMouseDlg = NULL;
	if(pDlg == m_pMouseOverDlg)
	{
		m_pMouseOverDlg = NULL;
		UpdateMouseOverDlg();
	}
	if(pDlg == m_pFocusDlg)
	{
		m_pFocusDlg = NULL;
		ResetFocus();
	}
	if(pDlg == m_pPopupDlg)
	{
		m_pPopupDlg = NULL;
	}
	if(pDlg == m_pEditMenu)
	{
		Assert(0);
		m_pEditMenu = NULL;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiManager::ResetFocus()
{
	m_pFocusDlg = NULL;
	if(CGuiDialog::s_pControlFocus) CGuiDialog::s_pControlFocus->OnFocusOut();
	CGuiDialog::s_pControlFocus = NULL;
	for(int i=m_Dialogs.size()-1;i>=0;--i)
	{
		CGuiDialog* pDlg = m_Dialogs[i];
		if(pDlg->IsEnabled() && pDlg->IsVisible() && !pDlg->IsClosed())
		{
			m_pFocusDlg = pDlg;
			m_pFocusDlg->ResetFocus();
			break;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::CloseAllDialog()
{
	m_pFocusDlg = NULL;
	m_pEditMenu = NULL;
	m_pPopupDlg = NULL;
	for(int i=0;i<m_Dialogs.size();++i)
	{
		m_Dialogs[i]->Close();
	}
	for(int i=0;i<m_Dialogs.size();++i)
	{
		delete m_Dialogs[i];
	}
	m_Dialogs.resize(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszZipPackName : ÃèÊö
 * \param pszFileName : ÃèÊö
 * \param pCreated : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiManager::LoadGuiFromXML(const char* pszZipPackName,const char* pszFileName,CGuiDialog** pCreated)
{
	Stream* pStream = OpenPackFileForRead(pszZipPackName,pszFileName);
	if(!pStream) return S_FALSE;
	HRESULT hr = LoadGuiFromXML(pStream,pCreated);
	ClosePackFileForRead(pStream);
	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszFileName : ÃèÊö
 * \param pCreated : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiManager::LoadGuiFromXML(const char* pszFileName,CGuiDialog** pCreated)
{
	FileStream stream;
	stream.open(pszFileName,FileStream::Read);
	return LoadGuiFromXML(&stream,pCreated);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszZipPackName : ÃèÊö
 * \param pszFileName : ÃèÊö
 * \param pDialog : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiManager::LoadGuiFromXML(const char* pszZipPackName,const char* pszFileName,CGuiDialog* pDialog)
{
	Stream* pStream = OpenPackFileForRead(pszZipPackName,pszFileName);
	if(!pStream) return S_FALSE;
	HRESULT hr = LoadGuiFromXML(pStream,pDialog);
	ClosePackFileForRead(pStream);
	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszFileName : ÃèÊö
 * \param pDialog : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiManager::LoadGuiFromXML(const char* pszFileName,CGuiDialog* pDialog)
{
	FileStream stream;
	stream.open(pszFileName,FileStream::Read);
	return LoadGuiFromXML(&stream,pDialog);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pStream : ÃèÊö
 * \param pDialog : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiManager::LoadGuiFromXML(Stream* pStream,CGuiDialog* pDialog)
{
	CGuiDialog *pDlg;
	TiXmlDocument doc;

	if(!LoadXMLFromStream(pStream,doc))
		return S_FALSE;

	TiXmlElement* gui = doc.FirstChildElement("gui");
	if(!gui) return S_FALSE;

	TiXmlElement* dialog = gui->FirstChildElement("dialog");
	if(!dialog) return S_FALSE;

	int x,y,width,height;
	int id;
	int style;
	int type;
	int isdefault;
	int align;
	int nIcon = -1;
	const char * hotkey;
	int colortopleft, colortopright, colorbottomleft, colorbottomright;
	const char* pszCaption;
	const char* pszName;
	int  showsoundindex,hidesoundindex;
	if(dialog->QueryIntAttribute("id",&id) != TIXML_SUCCESS)
		return S_FALSE;

	if(XML_QueryAttribute(dialog,"style",16,&style) != TIXML_SUCCESS)
		style = 0;

	if(XML_QueryAttribute(dialog,"align",16,&align) != TIXML_SUCCESS)
		align = GUI_ALIGN_NONE;
	if(dialog->QueryIntAttribute("icon",&nIcon) != TIXML_SUCCESS)
		nIcon = -1;
	colortopleft = XML_GetColorAttribute(dialog,"colortopleft",-1);
	colortopright = XML_GetColorAttribute(dialog,"colortopright",-1);
	colorbottomleft = XML_GetColorAttribute(dialog,"colorbottomleft",-1);
	colorbottomright = XML_GetColorAttribute(dialog,"colorbottomright",-1);

	if(dialog->QueryIntAttribute("showsoundindex",&showsoundindex) != TIXML_SUCCESS)
		showsoundindex = g_defaultDlgOpenSound;         // Ä¬ÈÏµÄ
	if(dialog->QueryIntAttribute("hidesoundindex",&hidesoundindex) != TIXML_SUCCESS)
		hidesoundindex = g_defaultDlgHideSound;         // Ä¬ÈÏµÄ

	/*
	if(dialog->QueryIntAttribute("x",&x) != TIXML_SUCCESS)
	x = 0;
	if(dialog->QueryIntAttribute("y,",&y) != TIXML_SUCCESS)
	y = 0;
	if(dialog->QueryIntAttribute("width",&width) != TIXML_SUCCESS)
	width = 200;
	if(dialog->QueryIntAttribute("height",&height) != TIXML_SUCCESS)
	height = 200;
	//*/

	const char* pszRect = dialog->Attribute("rect");
	stRectI dlgRect;
	if(pszRect) dlgRect.FromString(pszRect);
	else dlgRect.SetRect(0,0,200,200);
	x = dlgRect.left;
	y = dlgRect.top;
	width = dlgRect.Width();
	height = dlgRect.Height();

	int iFont;

	pszCaption = dialog->Attribute("caption");
	pszName = dialog->Attribute("name");

	s_bLoadDlg = true;
	HRESULT hr;
	hr = AddDialog(id,x,y,width,height,style,pszCaption,pszName,pDialog);
	s_bLoadDlg = false;
	if(FAILED(hr)) return S_FALSE;

	pDlg = pDialog;
	pDlg->SetAlign((GUI_ALIGN_TYPE)align);
	pDlg->SetBackgroundColors(colortopleft,colortopright,colorbottomleft,colorbottomright);
	//fancui
	pDlg->SetShowSoundIndex(showsoundindex);
	pDlg->SetHideSoundIndex(hidesoundindex);
	TiXmlElement* controls = dialog->FirstChildElement("controls");
	if(!controls)
		return S_OK;

	TiXmlElement* item = controls->FirstChildElement("item");

	while(item)
	{
		const char* szControlType = NULL;
		if( item->QueryIntAttribute("id",&id) != TIXML_SUCCESS || ((szControlType = item->Attribute("type")) == NULL ) )
		{
			RemoveDlg(pDlg);
			return S_FALSE;
		}
		type = DescToGuiControlType(szControlType);

		/*
		if(item->QueryIntAttribute("x",&x) != TIXML_SUCCESS)
		x = 0;
		if(item->QueryIntAttribute("y",&y) != TIXML_SUCCESS)
		y = 0;
		if(item->QueryIntAttribute("width",&width) != TIXML_SUCCESS)
		width = 50;
		if(item->QueryIntAttribute("height",&height) != TIXML_SUCCESS)
		height = 50;
		//*/
		pszRect = item->Attribute("rect");
		stRectI controlRect;
		if(pszRect) controlRect.FromString(pszRect);
		else controlRect.SetRect(0,0,50,50);
		x = controlRect.left;
		y = controlRect.top;
		width = controlRect.Width();
		height = controlRect.Height();

		const char* pszToopTips  = NULL;

		TiXmlElement* pXmlTip = item->FirstChildElement("Tip");

		if(pXmlTip)
		{
			pszToopTips = XML_GetNodeText(pXmlTip);
		}

		DWORD dwTextFormat = DT_CENTER  | DT_VCENTER;
		
		if(XML_QueryAttribute(item,"textformat",16,(int*)&dwTextFormat) != TIXML_SUCCESS)
			dwTextFormat = DT_CENTER | DT_VCENTER;

		hotkey = item->Attribute("hotkey");
		if(item->QueryIntAttribute("isdefault",&isdefault) != TIXML_SUCCESS)
			isdefault = 0;

		if(XML_QueryAttribute(item,"style",16,&style) != TIXML_SUCCESS)
			style = 0;

		if(item->QueryIntAttribute("font",&iFont) != TIXML_SUCCESS)
			iFont = -1;
		
		//if(!GetDevice()->IsValidFont(iFont)) iFont = 0;

		pszCaption = XML_GetNodeText(item);

		int checked;
		int group;
		if(item->QueryIntAttribute("checked",&checked) != TIXML_SUCCESS)
			checked = 0;
		if(item->QueryIntAttribute("group",&group) != TIXML_SUCCESS)
			group = 0;

		int color,textcolor;

		color = XML_GetColorAttribute(item,"color",-1);
		textcolor = XML_GetColorAttribute(item,"textcolor",0);

		const char* szPack;
		int frame;

		szPack = item->Attribute("pack");
		if(item->QueryIntAttribute("frame",&frame) != TIXML_SUCCESS)
			frame = 0;
		
		CGuiControl* pControl = NULL;
		switch(type)
		{
		case GUI_CONTROL_BUTTON:
			{
				CGuiButton* pBtn;
				if(SUCCEEDED(pDlg->AddButton(id,pszCaption,x,y,width,height,hotkey,isdefault != 0,&pBtn)))
				{
					int bitmapbutton = 0;
					int framenum = 3;
					int soundindex=0;
					if(item->QueryIntAttribute("bitmapbutton",&bitmapbutton) != TIXML_SUCCESS)
						bitmapbutton = 0;

					if(item->QueryIntAttribute("framenum",&framenum) != TIXML_SUCCESS)
						framenum = 3;
					
					if(bitmapbutton)
					{
						pBtn->SetButtonType(CGuiButton::ButtonType_Bitmap);
						pBtn->SetBitmap(stResourceLocation(szPack,group,frame),framenum);
					}
					if(style == 5)
					{
						pBtn->SetButtonType(CGuiButton::ButtonType_Color);
						pBtn->SetSize(width,height);
					}

					if(item->QueryIntAttribute("sound",&soundindex) != TIXML_SUCCESS)
						soundindex = g_defaultButtonClickSound;
					pBtn->SetSoundIndex(soundindex);
					pControl = pBtn;
				}
			}
			break;
		case GUI_CONTROL_STATIC:
			{
				CGuiStatic * pStatic;
				if(SUCCEEDED(pDlg->AddStatic(id,pszCaption,x,y,width,height,isdefault != 0,&pStatic)))
				{
					pControl = pStatic;
				}
			}
			break;
		case GUI_CONTROL_CHECKBOX:
			{
				CGuiCheckBox* pCheckBox;
				if(SUCCEEDED(pDlg->AddCheckBox(id,pszCaption,x,y,width,height,checked != 0,hotkey,isdefault != 0,&pCheckBox)))
				{
					pControl = pCheckBox;
					pCheckBox->SetChecked(checked != 0);
				}
			}
			break;
		case GUI_CONTROL_RADIOBUTTON:
			{
				CGuiRadioButton* pRadio;
				if(SUCCEEDED(pDlg->AddRadioButton(id,group,pszCaption,x,y,width,height,checked != 0,hotkey,isdefault != 0,&pRadio)))
				{
					pControl = pRadio;
					pRadio->SetButtonGroup(group);
					pRadio->SetChecked(checked != 0,true);
				}
			}
			break;
		case GUI_CONTROL_COMBOBOX:
			{
				CGuiComboBox* pCombo;
				if(SUCCEEDED(pDlg->AddComboBox(id,x,y,width,height,hotkey,isdefault != 0,&pCombo)))
				{
					TiXmlElement* datas = item->FirstChildElement("datas");
					if(datas)
					{
						TiXmlElement* item = datas->FirstChildElement("item");
						while(item)
						{
							int data;
							if(item->QueryIntAttribute("data",&data) != TIXML_SUCCESS)
							{
								data = 0;
							}
							pCombo->AddItem(XML_GetNodeText(item),(void*)data);
							item = item->NextSiblingElement("item");
						}
					}

					pControl = pCombo;
					//pCombo->SetTextColor(textcolor);
				}
			}
			break;
		case GUI_CONTROL_SLIDER:
			{
				int min,max;
				if(item->QueryIntAttribute("min",&min) != TIXML_SUCCESS)
					min = 0;
				if(item->QueryIntAttribute("max",&max) != TIXML_SUCCESS)
					max = 100;
				pDlg->AddSlider(id,x,y,width,height,min,max,min + (max - min)/2,isdefault != 0,(CGuiSlider**)&pControl);
			}
			break;
		case GUI_CONTROL_EDITBOX:
			{
				if(iFont == -1) iFont = 0;
				CGuiEditBox* pEdit;
				if(SUCCEEDED(pDlg->AddEditBox(id,pszCaption,x,y,width,height,isdefault != 0,&pEdit)))
				{
					pControl = pEdit;
					int maxCharCount;
					if(item->QueryIntAttribute("maxCharCount",&maxCharCount) == TIXML_SUCCESS)
						pEdit->SetMaxCharCount(maxCharCount);
					if(style & CGuiEditBox::GUI_ES_NUMBER)
					{
						int minNumber;
						int maxNumber;
						if(item->QueryIntAttribute("minNumber",&minNumber) == TIXML_SUCCESS)
							pEdit->SetMinNumber(minNumber);
						if(item->QueryIntAttribute("maxNumber",&maxNumber) == TIXML_SUCCESS)
							pEdit->SetMaxNumber(maxNumber);
					}
					pEdit->SetCaretColor(XML_GetColorAttribute(item,"caretcolor",pEdit->GetCaretColor()));
				}
			}
			break;
		case GUI_CONTROL_MLEDITBOX:
			{
				if(iFont == -1) iFont = 0;
				CGuiMLEditBox* pEdit;
				if(SUCCEEDED(pDlg->AddMLEditBox(id,pszCaption,x,y,width,height,CGuiMLEditBox::AUTONEWLINE,isdefault != 0,&pEdit)))
				{
					pControl = pEdit;
					pEdit->SetCaretColor(XML_GetColorAttribute(item,"caretcolor",pEdit->GetCaretColor()));
					int iLineHeight = 12;
					if(item->QueryIntAttribute("lineHeight",&iLineHeight) == TIXML_SUCCESS)
						pEdit->SetLineHeight(iLineHeight);
					int maxCharCount;
					if(item->QueryIntAttribute("maxCharCount",&maxCharCount) == TIXML_SUCCESS)
						pEdit->SetMaxCharCount(maxCharCount);
				}
			}
			break;
		case GUI_CONTROL_MLTEXTBOX:
			{
				if(iFont == -1) iFont = 0;
				CGuiMLTextBox* pText;
				if(SUCCEEDED(pDlg->AddMLTextBox(id,pszCaption,x,y,width,height,0,isdefault != 0,&pText)))
				{
					pControl = pText;
					int iLineHeight = 12;
					if(item->QueryIntAttribute("lineHeight",&iLineHeight) == TIXML_SUCCESS)
						pText->SetLineHeight(iLineHeight);
				}
			}
			break;
		case GUI_CONTROL_LISTBOX:
			{
				CGuiListBox* pList;
				if(SUCCEEDED(pDlg->AddListBox(id,x,y,width,height,style,&pList)))
				{
					int height = 0;
					if(item->QueryIntAttribute("itemheight",&height) == TIXML_SUCCESS)
						pList->SetTextHeight(height);
					TiXmlElement* datas = item->FirstChildElement("datas");
					if(datas)
					{
						TiXmlElement* item = datas->FirstChildElement("item");
						while(item)
						{
							int data;
							if(item->QueryIntAttribute("data",&data) != TIXML_SUCCESS)
							{
								data = 0;
							}
							pList->AddItem(XML_GetNodeText(item),(void*)data);
							item = item->NextSiblingElement("item");
						}
					}
					pControl = pList;
				}
			}
			break;
		case GUI_CONTROL_TREE:
			{
				CGuiTree* pTree;
				if(SUCCEEDED(pDlg->AddTree(id,x,y,width,height,style,&pTree)))
				{
					TiXmlElement* datas = item->FirstChildElement("datas");
					if(datas)
					{
						
						TiXmlElement* item = datas->FirstChildElement("item");
						/*
						while(item)
						{
							pTree->AddItem(XML_GetNodeText(item),COLOR_ARGB(255,255,255,255));
							item = item->NextSiblingElement("item");
						}
						*/
					//	AddTreeItems(datas->FirstChildElement("item"));
						pTree->LoadItemsFromXML(item);

					}
					pControl = pTree;
				}
			}
			break;
		case GUI_CONTROL_TAB:
			{
				CGuiTab* pList;
				if(SUCCEEDED(pDlg->AddTab(id,x,y,width,height,style,&pList)))
				{
					int align = 0;
					if(item->QueryIntAttribute("align",&align) != TIXML_SUCCESS)
						align = 0;
					pList->SetHeaderAlign((CGuiTab::enumHeaderAlign)align);

					int soundindex;
					if(item->QueryIntAttribute("sound",&soundindex) != TIXML_SUCCESS)
						soundindex = g_defaultTabClickSound;
					pList->SetSoundIndex(soundindex);


					TiXmlElement* datas = item->FirstChildElement("datas");
					if(datas)
					{
						TiXmlElement* item = datas->FirstChildElement("item");
						while(item)
						{
							int data;
							if(item->QueryIntAttribute("data",&data) != TIXML_SUCCESS)
							{
								data = 0;
							}
							pList->AddItem(XML_GetNodeText(item),(void*)data);
							item = item->NextSiblingElement("item");
						}
					}
					pControl = pList;
				}
			}
			break;
		case GUI_CONTROL_SCROLLBAR:
			{
				int min,max,pagesize;
				if(item->QueryIntAttribute("min",&min) != TIXML_SUCCESS)
					min = 0;
				if(item->QueryIntAttribute("max",&max) != TIXML_SUCCESS)
					max = 100;
				if(item->QueryIntAttribute("pagesize",&pagesize) != TIXML_SUCCESS)
					pagesize = (max-min)/10;
				if(pagesize == 0) pagesize = 1;
				CGuiScrollBar* pScroll;
				int style;
				if(item->QueryIntAttribute("style",&style) != TIXML_SUCCESS)
					style = 0;
				if(SUCCEEDED(pDlg->AddScrollBar(id,x,y,width,height,min,max,pagesize,isdefault != 0,&pScroll,style)))
				{
					pControl = pScroll;
				}
			}
			break;
		case GUI_CONTROL_IMAGE:
			{
				int isback;
				if(item->QueryIntAttribute("backgroud",&isback) != TIXML_SUCCESS)
					isback = false;
				if(szPack)
				{
					stResourceLocation rl;
					if( strncpy(rl.szFileName,szPack,sizeof(rl.szFileName)) )
					{
						rl.group = group;
						rl.frame = frame;
						stRectI imageRect;
						const char* pszRect = item->Attribute("imagerect");
						if(pszRect) imageRect.FromString(pszRect);
						if(isback)
						{
							if(width == 0) width = -1;
							if(height == 0) height = -1;
							pDlg->SetBackImage(rl,pszRect ? &imageRect : NULL,-1,0,0,true,width,height);
						}
						else
							pDlg->AddImage(id,x,y,rl,pszRect ? &imageRect : NULL,color,width,height,style,(CGuiImage**)&pControl);
					}
				}
			}
			break;
		case GUI_CONTROL_PROCESS:
			{
				int nMin,nMax,nPos;

				if(item->QueryIntAttribute("min",&nMin) != TIXML_SUCCESS)
					nMin = 0;

				if(item->QueryIntAttribute("max",&nMax) != TIXML_SUCCESS)
					nMax = 0;

				if(item->QueryIntAttribute("pos",&nPos) != TIXML_SUCCESS)
					nPos = 0;
				

				CGuiProcess* pProcess;
				if(SUCCEEDED(pDlg->AddProcess(id,x,y,width,height,nMin,nMax,nPos,isdefault != 0,&pProcess)))
				{
					pProcess->SetProcessBitmap(stResourceLocation(szPack,group,frame));
					pControl = pProcess;
				}

			}
			break;
		case GUI_CONTROL_SCROLLTAB:
			{
				CGuiScrollTab* pList;
				if(SUCCEEDED(pDlg->AddScrollTab(id,x,y,width,height,isdefault != 0,&pList)))
				{
					TiXmlElement* datas = item->FirstChildElement("datas");
					if(datas)
					{
						TiXmlElement* item = datas->FirstChildElement("item");
						while(item)
						{
							int data;
							if(item->QueryIntAttribute("data",&data) != TIXML_SUCCESS)
							{
								data = 0;
							}
							pList->AddItem(XML_GetNodeText(item),(void*)data);
							item = item->NextSiblingElement("item");
						}
					}
					pControl = pList;
				}

			}
			break;
		case GUI_CONTROL_TABLE:
			{
				int cellWidth,cellHeight;
				int rowCount,showRowCount,colCount;
				
				int isvirtual = false;
				
				if(item->QueryIntAttribute("virtual",&isvirtual) != TIXML_SUCCESS)
					isvirtual = 0;

				if(item->QueryIntAttribute("cellWidth",&cellWidth) != TIXML_SUCCESS)
					cellWidth = 0;
				if(item->QueryIntAttribute("cellHeight",&cellHeight) != TIXML_SUCCESS)
					cellHeight = 0;

				if(item->QueryIntAttribute("rowCount",&rowCount) != TIXML_SUCCESS)
					rowCount = 0;
				if(item->QueryIntAttribute("showRowCount",&showRowCount) != TIXML_SUCCESS)
					showRowCount = 0;
				if(item->QueryIntAttribute("colCount",&colCount) != TIXML_SUCCESS)
					colCount = 0;

				pDlg->AddTable(id,x,y,cellWidth,cellHeight,colCount,rowCount,showRowCount,isvirtual != 0,(CGuiTable**)&pControl);
				if(pControl)
				{
					CGuiTable* pTable = (CGuiTable*)pControl;
					if(isvirtual)
					{
						pTable->SetSize(width,height);
						pTable->m_itemRect = XML_GetRectAttribute(item,"itemrect");
						if(pTable->m_itemRect.IsRectEmpty())
						{
							pTable->m_itemRect = stRectI(0,0,pTable->GetWidth(),pTable->GetHeight());
						}
					}
				}
			}
		}
		if(pControl)
		{
			if(pszToopTips) pControl->SetToolTips(pszToopTips);

			pControl->SetStyle(style,true);
			if(hotkey)
				pControl->SetHotkey(hotkey);
			if(textcolor != 0)
				pControl->SetTextColor(textcolor);
			if(iFont == -1) iFont = 1;
			pControl->SetFont(iFont);
			//pControl->SetTextFormat(dwTextFormat);
		}
		item = item->NextSiblingElement("item");
	}
	TiXmlElement* script = dialog->FirstChildElement("script");
	if(script)
	{
		const char* szScript = XML_GetNodeText(script);
		pDlg->SetScript(szScript);
	}
	TiXmlElement* popupmenu = dialog->FirstChildElement("popupmenu");
	if(popupmenu)
	{
		CGuiMenu* pMenu = GetGuiManager()->CreateMenu(pDlg);
		TiXmlElement* item = popupmenu->FirstChildElement("item");
		while(item)
		{
			int cmd;
			const char* text;
			int separate;
			if(item->QueryIntAttribute("cmd",&cmd) != TIXML_SUCCESS)
				cmd = -1;
			text = item->Attribute("text");
			if(text == NULL)
				text = "";
			if(item->QueryIntAttribute("separate",&separate) != TIXML_SUCCESS)
				separate = 0;
			CGuiMenu::enumMenuStyle style = CGuiMenu::MenuStyle_Command;
			if(separate) style = CGuiMenu::MenuStyle_Separate;
				item =  item->NextSiblingElement("item");
			pMenu->AddMenuItem(cmd,text,style);
		}
		pDlg->SetPopupMenu(pMenu);
	}
	pDlg->OnCreate();
	if(nIcon != -1)
	{
		stResourceLocation rl(GetGuiGraphicPackName(),22,nIcon);
		pDlg->SetIcon(&rl);
	}
	ResetFocus();
	//pDlg->SetAlign((GUI_ALIGN_TYPE)align);
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pStream : ÃèÊö
 * \param pCreated : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiManager::LoadGuiFromXML(Stream* pStream,CGuiDialog** pCreated)
{
	CGuiDialog *pDlg = new CGuiDialog(0);
	HRESULT hr = LoadGuiFromXML(pStream,pDlg);
	if(pCreated) *pCreated = pDlg;
	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nID : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiDialog* CGuiManager::GetDialogByID(UINT nID)
{
	for(int i=0;i<m_Dialogs.size();++i)
	{
		if(m_Dialogs[i]->IsClosed()) continue;
		if(m_Dialogs[i]->GetID() == nID) return m_Dialogs[i];
	}
	return NULL;
}

CGuiDialog* CGuiManager::GetDialogByActionType(int type)
{
	for(int i=0;i<m_Dialogs.size();++i)
	{
		if(m_Dialogs[i]->IsClosed()) continue;
		if( (m_Dialogs[i])->GetActionType() == type ) return m_Dialogs[i];
	}
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szName : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiDialog* CGuiManager::GetDialogByName(const char* szName)
{
	for(int i=0;i<m_Dialogs.size();++i)
	{
		if(m_Dialogs[i]->IsClosed()) continue;
		if(strcmp(m_Dialogs[i]->GetName() , szName) == 0) return m_Dialogs[i];
	}
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDialog : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiDialog* CGuiManager::GetNextDialog(CGuiDialog* pDialog)
{
	int i= DialogToIndex(pDialog);
	return IndexToDialog(i+1);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDialog : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiDialog* CGuiManager::GetPrevDialog(CGuiDialog* pDialog)
{
	int i= DialogToIndex(pDialog);
	return IndexToDialog(i-1);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiFloatItem* CGuiManager::GetFloatItem()
{
	if(GetMouseDlg() == ::GetGuiFloatItem())
		return (CGuiFloatItem*)GetMouseDlg();
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::OnScreenSize()
{
	for(int i=0;i<m_Dialogs.size();++i)
	{
		if(!m_Dialogs[i]->IsClosed())
			m_Dialogs[i]->OnResetScreenSize();
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::UpdateMouseOverDlg()
{
	m_pMouseOverDlg = GetDialogByPosition(Engine_GetCursor()->GetPosition());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptScreen : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiDialog* CGuiManager::GetDialogByPosition(POINT ptScreen)
{
	//*
	//if(m_pMouseDlg) return m_pMouseDlg;
	for(int i=m_Dialogs.size()-1;i>=0;--i)
	{
		if( i >= m_Dialogs.size()) break;

		CGuiDialog* pDlg = m_Dialogs[i];
		if(pDlg->GetID() ==	20 )
			int jj = 0;
		if( strcmp(pDlg->GetName(),"TeamDialog") == 0)
			int iii = 0;
		if(pDlg->IsVisible() && !pDlg->IsClosed())
		{
			DWORD dwHit = pDlg->OnHiTest(ptScreen);
			if(dwHit != HTERROR && dwHit != HTTRANSPARENT)
			{
				return pDlg;
			}
		}
	}
	return NULL;
	//*/
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiManager::GenerateDialogID()
{
	int maxID = 0;
	for(int i=0;i<m_Dialogs.size();++i)
	{
		if(m_Dialogs[i]->GetID() > maxID)
		{
			maxID = m_Dialogs[i]->GetID();
		}
	}
	return getMax((long)GUI_USER_ID,(long)(maxID+1));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDlg : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::SetPopupDlg(CGuiDialog* pDlg)
{
	if(m_pPopupDlg) m_pPopupDlg->EndPopup();
	m_pPopupDlg = pDlg;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDlg : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiManager::PopupEditMenu(CGuiDialog* pDlg)
{
	if(!m_pEditMenu)
	{
		Assert(0); return;
	}
	m_pEditMenu->SetOwner(pDlg);
	m_pEditMenu->Popup();
}