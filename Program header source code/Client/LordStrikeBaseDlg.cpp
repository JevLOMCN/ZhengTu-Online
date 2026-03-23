#include "public.h"
#include "miniusercommand.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "GameGuiManager.h"
#include "LordStrikeBaseDlg.h"
#include "guiface.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
 


#define ID_MLTEXTBOX  9 
#define ID_STATIC_ROLE_NAME    15
#define ID_STATIC_LEVEL        16
#define ID_STATIC_TOTAL_SCORE  17
#define ID_STATIC_MONEY        18 
#define ID_STATIC_SORT         19 
#define ID_STATIC_WIN          20
#define ID_STATIC_FAIL         21 



#define ID_BUTTON_MIN  31




//////////////////////////////////////////////////////////////////////////
CGuiLordStrikeBaseDlg::CGuiLordStrikeBaseDlg()
: m_pRightHorzScrollBar(NULL)
{
	InitScrollBarValue();
}

CGuiLordStrikeBaseDlg::~CGuiLordStrikeBaseDlg()
{

}


void CGuiLordStrikeBaseDlg::OnCreate(void)
{
	CGuiDialog::OnCreate();


	m_pRightHorzScrollBar = (CGuiScrollBar *)GetControl(ID_PLAYER_SCROLLBAR,GUI_CONTROL_SCROLLBAR);
	//assert(m_pRightHorzScrollBar!=NULL);

	UpdateSrollRects();

	InitListBox();


	m_x = (GetDevice()->GetWidth() - GetWidth())/2;
	m_y = (GetDevice()->GetHeight() - GetHeight())/2;

         CGuiStatic * pStatic = GetStatic(ID_STATIC_SORT);
	 if(pStatic)
		 pStatic->SetText("-");


	 //stGuiCtrlBorderInfo* pBorderInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_SCROLLBAR);
	 //pBorderInfo->ptLeftTop = stPointI(19,19);
	 //pBorderInfo->ptRightBottom = stPointI(19,19);

}


CGuiButton * CGuiLordStrikeBaseDlg::__AddButton(int nBtnID)
{
	CGuiButton * pButton  = GetButton(nBtnID);
	if(!pButton)
	{
		AddButton(nBtnID, "" , 0 , 0 , 32 , 32 , 0 , false , &pButton );
		pButton->SetButtonType( CGuiButton::ButtonType_Bitmap );	
	}
	assert(pButton != NULL);
	if(pButton)
	{
		//pButton->SetLocation(TABLE_OFFSET_X + i%3 * TABLE_GAP_X - 50,TABLE_OFFSET_Y + i/3 * TABLE_GAP_Y);
		//pButton->SetEnabled(false);
		pButton->SetVisible(false);
	}
	return pButton;
}

CGuiStatic * CGuiLordStrikeBaseDlg::__AddStatic(int nStaticID)
{
	CGuiStatic* pStatic = GetStatic(nStaticID);
	if(!pStatic)
	{
		AddStatic(nStaticID , "" ,  0,  0, 100 , 32  , false , &pStatic );
	}
	assert(pStatic!=NULL);
	if(pStatic)
	{
		pStatic->SetVisible(false);
	}
	return pStatic;
}

CGuiImage * CGuiLordStrikeBaseDlg::__AddImage(int nImageID)
{
	CGuiImage* pImg = GetImage(nImageID);
	if(!pImg)
	{
		AddImage(nImageID,0,0,stResourceLocation("data\\interfaces",1,242),NULL,-1,-1,-1,CGuiImage::FULLSIZE_IMAGE,&pImg);
	}
	assert(pImg!=NULL);
	if(pImg)
	{
		pImg->SetVisible(false);
	}
	return pImg;
}

void CGuiLordStrikeBaseDlg::__SetButtonBitmap(CGuiButton * pButton,stResourceLocation & rl,int x,int y,stPointF & scale)
{
	DWORD dwState[] = { GUI_STATE_NORMAL,GUI_STATE_PRESSED,GUI_STATE_DISABLED };

	pButton->SetBitmap(rl,1,dwState);
	pButton->SetVisible(true);
	pButton->SetEnabled(true);
	pButton->SetLocation(x,y);
	pButton->SetBitmapScale(scale);

}

void CGuiLordStrikeBaseDlg::__SetStatic(CGuiStatic * pStatic,std::string strText,int x,int y )
{
         pStatic->SetText(strText.c_str());
	 pStatic->SetVisible(true);
	 pStatic->SetLocation(x,y);
}


void CGuiLordStrikeBaseDlg::__SetImage(CGuiImage * pImage,stResourceLocation & rl,int x,int y )
{
	pImage->SetImage(rl);
	pImage->SetVisible(true);
	pImage->SetLocation(x,y);
	pImage->SetLocation(x,y);
}



void CGuiLordStrikeBaseDlg::InitScrollBar()
{
	FUNCTION_BEGIN;


	m_pRightHorzScrollBar = (CGuiScrollBar*)GetControl( ID_PLAYER_SCROLLBAR, GUI_CONTROL_SCROLLBAR );
	if ( m_pRightHorzScrollBar )
	{
		m_pRightHorzScrollBar->SetStyle(m_pRightHorzScrollBar->GetStyle() | CGuiScrollBar::NOSKIN);
		//m_pRightHorzScrollBar->SetRange( 0, (m_vecEmotionIcon.size() + NUM_COL-1) / NUM_COL);
		m_pRightHorzScrollBar->SetValue( 0 );
		m_nOldRightHorzScrollPos = 0;
		//m_pRightHorzScrollBar->SetPageSize(NUM_ROW );
		//m_pRightHorzScrollBar->SetTrackRange(0,(m_vecEmotionIcon.size() + NUM_COL-1)/ NUM_COL);
	}

	FUNCTION_END;
}

void CGuiLordStrikeBaseDlg::UpdateSrollRects()
{
	FUNCTION_BEGIN;

	if(m_pRightHorzScrollBar)
	{
		stRectI rect(767,454,993,470);
		m_pRightHorzScrollBar->SetLocation( rect.left, rect.top );
		m_pRightHorzScrollBar->SetSize( rect.Width(), rect.Height() );
	}

	FUNCTION_END;
}


void CGuiLordStrikeBaseDlg::InitScrollBarValue()
{
	m_nRightHorzDeltaPos    = 0;
	m_nOldRightHorzScrollPos = 0;
}


void CGuiLordStrikeBaseDlg::OnMouseWheel()
{
	FUNCTION_BEGIN;
 
	if(m_pRightHorzScrollBar){
		m_nRightHorzDeltaPos = m_pRightHorzScrollBar->GetValue() - m_nOldRightHorzScrollPos;	 
	}

	if( abs(m_nRightHorzDeltaPos) >= 1)
	{
		if( m_pRightHorzScrollBar )
		{
			m_ListBoxHelper.Adjust();
			m_nOldRightHorzScrollPos = m_pRightHorzScrollBar->GetValue();
		}
	}


	FUNCTION_END;
}

HRESULT CGuiLordStrikeBaseDlg::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;


	OnMouseWheel();

	HRESULT hr =  CGuiDialog::OnRender(fElapsedTime);


	if ( m_pRightHorzScrollBar )
		m_pRightHorzScrollBar->Render( fElapsedTime );

	m_ListBoxHelper.Render();

	if( GetGuiManager()->GetMouseOverDlg() != this)
		return hr;

	if(GetGameGuiManager()->m_guiFace && GetGameGuiManager()->m_guiFace->IsVisible())
	{
		 __UpdateFaceDlgLocation();
	}

	return hr;	

	FUNCTION_END;
}


#define MAX_TEXT_LINE  200
void CGuiLordStrikeBaseDlg::AddChat(const char * pstr,DWORD clr)
{
	CGuiMLTextBox * pMLTExtBox = GetMLTextBox(ID_MLTEXTBOX);
	if(pMLTExtBox)
	{
		pMLTExtBox->AppendParagraph(pstr,clr);

		if(pMLTExtBox->GetParagraphCount() > MAX_TEXT_LINE )
		{
			for ( int j = 0; j < 16; j++ )
			{
				pMLTExtBox->RemoveFirstParagraph();
			}
			pMLTExtBox->Reset();
		}		
		pMLTExtBox->ScrollToTail();
	}
}

void CGuiLordStrikeBaseDlg::SendEditBox()
{
	CGuiEditBox * pEditBox = GetEditBox(ID_EDIT_CHAR);

	if(pEditBox)
	{
		const char* pszText = pEditBox->GetText();

		char szCharText[512]={0};
		strncpy(szCharText,pszText,sizeof(szCharText));
		str_trim_left((char *)szCharText);
		str_trim_right((char *)szCharText);
		if(strlen(szCharText) == 0){						
			return;   // 如果为空就直接返回
		}
		std::string strChat = szCharText;
		SendChat(strChat);

		pEditBox->ClearText();
	}
}

bool CGuiLordStrikeBaseDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
			case VK_RETURN:
				{
					SendEditBox();
				}
				break;
		}
		break;
	}

	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);

}


bool CGuiLordStrikeBaseDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID)
			{
			case ID_BUTTON_FACE:
				{
					if (!GetGameGuiManager()->m_guiFace)
						GetGameGuiManager()->AddFace();

					if(GetGameGuiManager()->m_guiFace->IsVisible())
					{
						GetGameGuiManager()->m_guiFace->EndPopup();
					}
					else
					{
						GetGameGuiManager()->m_guiFace->m_pEditBox=GetEditBox(ID_EDIT_CHAR);
						__UpdateFaceDlgLocation();
						GetGameGuiManager()->m_guiFace->BeginPopup();
					}

					//OnOffChatFaceSelect(GetEditBox(ID_EDIT_CHAR));
				}
				break;
			case ID_BUTTON_SEND:
				{
					SendEditBox();
				}
				break;
			case ID_BUTTON_MIN:
				{

					CLordStrikeManager::GetSingleton()->ShowAllDialog(false);
					CLordStrikeManager::GetSingleton()->UpdateHideBtn();
				}
				break;
			}
		}
		break;
	}
	
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}


void CGuiLordStrikeBaseDlg::__UpdateFaceDlgLocation()
{
	if( !GetGameGuiManager()->m_guiFace)
		return;
        
	CGuiButton * pButton = GetButton(ID_BUTTON_FACE);
	if( !pButton ) 
		return;


	
	stPointI pt(900,715); //clip rect of button

	int x = pt.x + GetClientX();
	int y = pt.y + GetClientY();

	stRectI * pRect = GetGameGuiManager()->m_guiFace->GetFaceRect();

	x -= pRect->Width();
	y -= pRect->Height();

	GetGameGuiManager()->m_guiFace->SetLocation(x,y);       
		
}

void CGuiLordStrikeBaseDlg::OnClose(void)
{
	return CGuiDialog::OnClose();
}


void CListBoxHelper::Init(CGuiListBox * __pListBox,CGuiScrollBar * __pScrollBar,CGuiLordStrikeBaseDlg * __pDlg)
{
	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");
	rl.group = 132;
	rl.frame = 1;
	pBmpHeading  = GetDevice()->FindBitmaps(&rl);
	if(pBmpHeading)
	{
		IBitmap * pBmp = pBmpHeading->GetBitmap(0);
		if(pBmp)
		{
			rcHeading = pBmp->GetClipRect();
		}
	}

	pListBox = __pListBox;
	pScrollBar = __pScrollBar;
	pDialog = __pDlg;

	rcClipListBox = stRectI(765,247,974,449);

	if ( pScrollBar && pListBox && pDialog)
	{
		pScrollBar->SetStyle(pScrollBar->GetStyle() | CGuiScrollBar::NOSKIN | CGuiScrollBar::HORIZONTAL2);

		pListBox->SetStyle(CGuiListBox::USECLIP | pListBox->GetStyle());
		pListBox->SetListBoxClipRect(rcClipListBox);

		pScrollBar->SetRange( 0, rcHeading.Width());
		pScrollBar->SetValue( 0 );
		//m_nRightHorzDeltaPos    = 0;
		//m_nOldRightHorzScrollPos = 0;
		__pDlg->InitScrollBarValue();

		pScrollBar->SetPageSize(pListBox->GetWidth());
		pScrollBar->SetTrackRange(0,rcHeading.Width());		
	}
}
void CListBoxHelper::Adjust()
{
	if(pListBox && pScrollBar)
		pListBox->SetOffset(pScrollBar->GetValue(),0);

}

void CListBoxHelper::Render()
{
	if(pDialog && pListBox)
	{
		stRectI rcClip = rcClipListBox;
		OffsetRect(&rcClip,pDialog->GetClientX(),pDialog->GetClientY());
		rcClip.top -=rcHeading.Height();
		stRectI rcOldClip = GetDevice()->SetClipRect(&rcClip);

		ptRenderBmp.x = pDialog->GetClientX() + rcClipListBox.left - pListBox->GetOffset().x;
		ptRenderBmp.y = pDialog->GetClientY() + rcClipListBox.top - pListBox->GetOffset().y - rcHeading.Height();

		pBmpHeading->Render(0,ptRenderBmp.x,ptRenderBmp.y);

		GetDevice()->SetClipRect(&rcOldClip);
	}
}


void CGuiLordStrikeBaseDlg::InitListBox()
{
//#ifdef _DEBUG
//	m_listUserInfo.clear();
//
//	for(size_t i = 0 ; i < 50; i++)
//	{
//		CUserInfo info;
//
//		char szName[30];
//
//		// 角色名
//		sprintf(szName,"玩家%d",i);
//		szName[29] = 0 ;
//		info.strRoleName = szName;
//
//		/// 等级
//		info.level = i;
//
//		/// 积分 
//		info.nTotalPoint = i * 100;
//
//		/// 排名
//		info.nSort = i;
//
//		info.numWin = i ;  ///胜
//		info.numFail= i ; /// 负
//		info.numPeace = i ; /// 和
//
//		// 国家
//		sprintf(szName,"国家%d",i);
//		szName[29] = 0 ;
//		info.strCountry = szName; 
//
//
//		sprintf(szName,"官职%d",i);
//		info.strCaption = szName; // 官职
//
//		m_listUserInfo.push_back(info);
//	}
//#endif 
//
//	CGuiListBox * pListBox = GetListBox(ID_LISTBOX_PLAYER);
//	assert(pListBox != NULL);
//	if(!pListBox)
//		return;
//
//
//	pListBox->RemoveAllItems();
//	for(std::list<CUserInfo>::iterator iter = m_listUserInfo.begin(); iter != m_listUserInfo.end(); ++iter)
//	{
//		pListBox->AddItem("",NULL);
//		int append_index = pListBox->GetItemCount()-1;
//
//		const CUserInfo & info = *iter;
//		pListBox->SetItemText2(append_index,0,info.strRoleName.c_str(),84) ; // 角色名
//
//		// 等级
//		char szName[20];
//		sprintf(szName,"%d",info.level);
//		szName[19] = 0;
//		pListBox->SetItemText2(append_index,1,szName,45);  // 等级
//
//		// 积分
//		sprintf(szName,"%d",info.nTotalPoint);
//		szName[19] = 0;
//		pListBox->SetItemText2(append_index,2,szName,40);  /// 积分
//
//
//		sprintf(szName,"%d",info.nSort);
//		szName[19] = 0;
//		pListBox->SetItemText2(append_index,3,szName,43); ///排名
//
//		sprintf(szName,"%d",info.numWin);
//		szName[19] = 0;
//		pListBox->SetItemText2(append_index,4,szName,30); ///胜
//
//
//		sprintf(szName,"%d",info.numFail);
//		szName[19] = 0;
//		pListBox->SetItemText2(append_index,5,szName,30); ///负
//
//
//		sprintf(szName,"%d",info.numPeace);
//		szName[19] = 0;
//		pListBox->SetItemText2(append_index,6,szName,30); ///和
//
//		pListBox->SetItemText2(append_index,7,info.strCountry.c_str(),58); ///国家
//
//		pListBox->SetItemText2(append_index,8,info.strCaption.c_str(),36); ///官职
//
//	}

	m_ListBoxHelper.Init( GetListBox(ID_LISTBOX_PLAYER), m_pRightHorzScrollBar,this);
}

void CGuiLordStrikeBaseDlg::InitCustomElements()
{
	FUNCTION_BEGIN;

	stResourceLocation rl;
	CGuiElement Element;
	stRectI rcTexture;

	rl.SetFileName("data\\interfaces.gl");
	rl.group = 132;
	rl.frame = 21;


	//-------------------------------------
	// CGuiScrollBar - left Arrow
	//-------------------------------------
	rcTexture.SetRect( 0, 0,17, 13);
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 5, &Element );

	//-------------------------------------
	// CGuiScrollBar - left Arrow Click
	//-------------------------------------
	rcTexture.SetRect( 0, 14,17, 30);
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 6, &Element );


	//-------------------------------------
	// CGuiScrollBar - right Arrow
	//-------------------------------------
	rcTexture.SetRect( 19, 0,36, 13 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 7, &Element );


	//-------------------------------------
	// CGuiScrollBar - right Arrow Click
	//-------------------------------------
	rcTexture.SetRect( 19, 14,36, 30 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_PRESSED ] = D3DCOLOR_ARGB(0, 0, 0, 0);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 8, &Element );


	//-------------------------------------
	// CGuiScrollBar - Button
	//-------------------------------------
	rcTexture.SetRect( 17, 31,40, 54 );
	Element.SetTexture( &rl, &rcTexture );

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 9, &Element );

	FUNCTION_END;
}

void CGuiLordStrikeBaseDlg::UpdatePlayerRoleNameAndLevel(std::string strRole,int nLevel)
{
	CGuiStatic * pStatic = NULL;
	pStatic = GetStatic(ID_STATIC_ROLE_NAME);
	if(pStatic)
	{ 
	               pStatic->SetText(strRole.c_str());      
	}

	pStatic = GetStatic(ID_STATIC_LEVEL);
	if(pStatic)
	{
		//char szTest[40];
		//_snprintf(szTest,40,"%d",nLevel);
		//
		//szTest[39] = 0;
		pStatic->SetText("-");  
	}
}

void CGuiLordStrikeBaseDlg::UpdateMainPlayerScore(int nScore)
{
	CGuiStatic * pStatic = GetStatic(ID_STATIC_TOTAL_SCORE);
	if(pStatic)
	{
		char szTest[40];
		_snprintf(szTest,40,"%d",nScore);
		szTest[39] = 0;
		pStatic->SetText(szTest);   
	}  

}

void CGuiLordStrikeBaseDlg::UpdateMainPlayerScore(const MiniGameScore & score)
{
	CGuiStatic * pStatic = GetStatic(ID_STATIC_TOTAL_SCORE);
	if(pStatic)
	{
		char szTest[40];
		_snprintf(szTest,40,"%d",score.score);
		szTest[39] = 0;
		pStatic->SetText(szTest);   
	}  

	pStatic = GetStatic(ID_STATIC_MONEY);
	if(pStatic)
	{
		char szTest[40];
		_snprintf(szTest,40,"%d",score.money);
		szTest[39] = 0;
		pStatic->SetText(szTest);   
	}  

	

	pStatic = GetStatic(ID_STATIC_WIN);
	if(pStatic)
	{
		char szTest[40];
		_snprintf(szTest,40,"%d",score.win);
		szTest[39] = 0;
		pStatic->SetText(szTest);   
	}

	pStatic = GetStatic(ID_STATIC_FAIL);            
	if(pStatic)
	{
		char szTest[40];
		_snprintf(szTest,40,"%d",score.lose);
		szTest[39] = 0;
		pStatic->SetText(szTest);   
	}
}
