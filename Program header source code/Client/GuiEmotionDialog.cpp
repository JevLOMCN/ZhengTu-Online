#include "public.h"
#include "./GameGuiManager.h"
#include "./Game.h"
#include "./GameCursor.h"
#include ".\GuiEmotionDialog.h"
#include ".\gameaccelkeymanager.h"
#include "./GuiFloatAccessKey.h"
#include "../Media/SoundManager.h"
#include "GameScene.h"
#include "MainCharacter.h"

/////////////////////////////////////////////////////////////////////////
// AccessKey 
stPointI	c_pt0AccessBtn(69,483);
int		c_xAccessBtnDiff  = 66;
char		c_AccessKy[ACCESS_NUM]     = {'Q','W','E','R','T',};
/////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//const stRectI	g_rcClip(10,258,410,580);
CToolTips	g_EmotionTips;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ScrollBar 
static int	g_nDeltaPos	= 0;
static int	g_nOldScrollPos = 0;
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
#define		POS_X_GAP	 54    // Line Row Gap
#define		POS_Y_GAP	 44    // Line Col Gap 
#define		NUM_ROW		 6     // 界面内可见部分能容纳的图标为 NUM_ROW * NUM_COL
#define		NUM_COL		 5 
#define		ICON_GRID_WIDTH  32
#define		ICON_GRID_HEIGHT 32
///////////////////////////////////////////////////////////////////////////

CEmotionInfo * GetEmotionInfo()
{
	FUNCTION_BEGIN;

	static CEmotionInfo Info;
	return &Info;

	FUNCTION_END;
}


void CEmotionInfo::LoadFromFile()
{
	
	FUNCTION_BEGIN;

	static bool bLoaded = false;
	if (bLoaded)	return;
	bLoaded = true;

	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\emotiondesc.xml");
	if(!pStream)
	{
		Engine_WriteLog("装载 emotiondesc.xml 失败\n");
		return ;
	}
	if(!LoadXMLFromStream(pStream,doc))
	{
		ClosePackFileForRead(pStream);
		Engine_WriteLog("解析 emotiondesc.xml 失败\n");
		return;
	}
	TiXmlElement* pEmotion = doc.FirstChildElement("emotion");
	if(pEmotion)
	{
		TiXmlElement* pIcon = pEmotion->FirstChildElement("icon");
		while(pIcon)
		{
			int id,animation;
			if(pIcon->QueryIntAttribute("id",&id) != TIXML_SUCCESS)
				continue;
			
			if(pIcon->QueryIntAttribute("animation",&animation) != TIXML_SUCCESS)
                              animation = id;
			
			id  += EMOTION_ICON_ID_BASE;
			stDesc & desc = m_MapEmotionDesc[id];
			desc.animation = animation;

			const char* pMusic = pIcon->Attribute("music");
			if( pMusic )
			{
				desc.strMusic = pMusic;
			}
			const char* pDesc1 = pIcon->Attribute("desc1"); // self to other
			if( pDesc1  )
			{	       
				desc.strDesc1 = pDesc1;
			}
			const char* pDesc2 = pIcon->Attribute("desc2"); // other to me
			if( pDesc2  )
			{	       
				desc.strDesc2 = pDesc2;
			}
			const char* pDesc3 = pIcon->Attribute("desc3"); // other to me
			if( pDesc3  )
			{	       
				desc.strDesc3 = pDesc3;
			}
			pIcon = pIcon->NextSiblingElement("icon");
		}
	}
	ClosePackFileForRead(pStream);

	FUNCTION_END;
}

CEmotionInfo::CEmotionInfo()
{
	FUNCTION_BEGIN;

	m_dwCurIconID = 0;
	m_IconComeFrom = FROM_NONE;
	LoadFromFile();
	ResetAccessKey();	
	
	FUNCTION_END;
}

CEmotionInfo::~CEmotionInfo()
{
	FUNCTION_BEGIN;

	m_MapEmotionDesc.clear();

	FUNCTION_END;
}

void CEmotionInfo::ResetAccessKey()
{
	for( int i = 0 ; i< ACCESS_NUM;i++)
		m_AccessKey[i] = 0 ;

}

void CEmotionInfo::OnPreUseEmotion(DWORD dwIconID)
{
	FUNCTION_BEGIN;

	stPointI ptCursor(0,0);
	//Change mouse icon
	stResourceLocation rl;
	rl.SetFileName(c_IconGL);
	rl.group = EMOTION_GROUP;
	rl.frame = dwIconID - EMOTION_ICON_ID_BASE;	
	IBitmaps * pBmps = GetDevice()->FindBitmaps(&rl);
	if( pBmps )
	{
		IBitmap * pBitmap = pBmps->GetBitmap(0);
		if ( pBitmap )
		{
			ptCursor.x += pBitmap->GetClipWidth()/2;
			ptCursor.y += pBitmap->GetClipHeight()/2;
		}
	}			
	GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorDictate );
	Engine_GetCursor()->SetImage( &rl, ptCursor );	

	m_dwCurIconID = dwIconID;

	FUNCTION_END;
}

bool CEmotionInfo::GetActiveResourceLocation(stResourceLocation & rl)
{
	FUNCTION_BEGIN;

	if( m_dwCurIconID == 0 )
		return false;

	rl.SetFileName(c_IconGL);
	rl.group = EMOTION_GROUP;
	rl.frame = m_dwCurIconID - EMOTION_ICON_ID_BASE;

	return true;

	FUNCTION_END;
}

bool CEmotionInfo::GetResourceLocation(DWORD iIconID, stResourceLocation & rl)
{
	FUNCTION_BEGIN;

	if( iIconID == 0 )
		return false;

	rl.SetFileName(c_IconGL);
	rl.group = EMOTION_GROUP;
	rl.frame = iIconID - EMOTION_ICON_ID_BASE;

	return true;

	FUNCTION_END;
}

DWORD   CEmotionInfo::GetActiveEmotionIcon()
{
	return m_dwCurIconID;
}
void    CEmotionInfo::ClearActiveEmotionIcon()
{
	m_dwCurIconID = 0;
	m_IconComeFrom  = FROM_NONE;
}

void CEmotionInfo::SetAccessKey(int index,DWORD dwKey)
{
	FUNCTION_BEGIN;

        if( index < 0 || index >= ACCESS_NUM)
	     return;
	m_AccessKey[index] = dwKey;

	FUNCTION_END;
}

DWORD CEmotionInfo::GetAccessKey(int index)
{
	FUNCTION_BEGIN;

	if( index < 0 || index >= ACCESS_NUM)
		return 0;
	return m_AccessKey[index];

	FUNCTION_END;
}

std::string  CEmotionInfo::GetCurIconDesc1()
{
	FUNCTION_BEGIN;
     
	return GetIconDesc1(m_dwCurIconID);

	FUNCTION_END;
}

std::string  CEmotionInfo::GetEmotionMusic(DWORD dwIconID)
{
        FUNCTION_BEGIN;

	std::string strReturn = "";
	tMapEmotionDesc::iterator it = m_MapEmotionDesc.find(dwIconID);
	if( it != m_MapEmotionDesc.end() )
	{
		stDesc & desc = it->second;
		strReturn = desc.strMusic;
	}
	return strReturn;

	FUNCTION_END;
}

std::string  CEmotionInfo::GetIconDesc1(DWORD dwIconID)
{
	FUNCTION_BEGIN;

	std::string strReturn = "";
	tMapEmotionDesc::iterator it = m_MapEmotionDesc.find(dwIconID);
	if( it != m_MapEmotionDesc.end() )
	{
		stDesc & desc = it->second;
		strReturn = desc.strDesc1;
	}
	return strReturn;

	FUNCTION_END;
}


std::string  CEmotionInfo::GetIconDesc2(DWORD dwIconID)
{
	FUNCTION_BEGIN;

	std::string strReturn = "";
	tMapEmotionDesc::iterator it = m_MapEmotionDesc.find(dwIconID);
	if( it != m_MapEmotionDesc.end() )
	{
		stDesc & desc = it->second;
		strReturn = desc.strDesc2;
	}
	return strReturn;

	FUNCTION_END;
}


std::string  CEmotionInfo::GetIconDesc3(DWORD dwIconID)
{
	FUNCTION_BEGIN;

	std::string strReturn = "";
	tMapEmotionDesc::iterator it = m_MapEmotionDesc.find(dwIconID);
	if( it != m_MapEmotionDesc.end() )
	{
		stDesc & desc = it->second;
		strReturn = desc.strDesc3;
	}
	return strReturn;

	FUNCTION_END;
}

void CEmotionInfo::RefreshAccessKey()
{
	FUNCTION_BEGIN;

	ResetAccessKey();

	//Update access key btn icon
	for( int i=0;i<ACCESS_NUM;i++ )
	{
		DWORD id;
		enumGameAccelKeyType type;
		BYTE key[4];
		key[0] = VK_MENU;
		key[1] = c_AccessKy[i];
		key[2] = 0;

		stResourceLocation rl;
		if ( GetGameAccelKeyManager()->GetAccessKeyIdAndType( key, (BYTE)EMOTION_ACCESS_KEY_GROUP, type, id ) )
		{
			assert(type == GameAccelKeyType_Face);
			if( type != GameAccelKeyType_Face )
				continue;
			SetAccessKey(i,id);
		}
	}

	FUNCTION_END;
}

void CEmotionInfo::OnUseEmotionOnSome(CCharacter * pChar)
{
	FUNCTION_BEGIN;

	/////////////////////////////////////////////////////////////
	//Send to sever
	void* p = NULL;
	p = malloc( sizeof( stChannelChatUserCmd ) );	
	if( NULL == p )
		return;
	stChannelChatUserCmd* cmd = (stChannelChatUserCmd*) p;

	cmd->byCmd = CHAT_USERCMD;
	cmd->byParam = ALL_CHAT_USERCMD_PARAMETER;
	cmd->dwChannelID = 0;
	cmd->dwType = CHAT_TYPE_EMOTION;
	strncpy(cmd->pstrChat,pChar->GetName(),sizeof(cmd->pstrChat));  ///**< 名字 */ 接收方的名字(不用cmd->pstrName,安全)
	cmd->dwFromID = GetScene()->GetMainCharacter()->GetID();
	cmd->dwSysInfoType = m_dwCurIconID;
	cmd->size = 0;
	SEND_USER_CMD( *cmd);
	free( p );

	FUNCTION_END;
}

void CEmotionInfo::OnReceivedEmotion(stChannelChatUserCmd * pCmd)
{
	FUNCTION_BEGIN;

	CCharacter * pSender = GetScene()->FindCharacterByID(pCmd->dwFromID);
	CCharacter * pReceiver = GetScene()->FindCharacterByName(pCmd->pstrChat);
	CCharacter * pMainPlayer = GetScene()->GetMainCharacter();
	if( NULL == pSender || NULL == pReceiver)
		return;

	DWORD dwIconID = pCmd->dwSysInfoType;

	//// only show message about myself 
	//if( pSender != pMainPlayer && pReceiver != pMainPlayer)
	//	return;

	char szEmotionMsg[512]={0};
	if( pSender == pMainPlayer && pReceiver == pMainPlayer)
		_snprintf(szEmotionMsg,sizeof(szEmotionMsg),"%s",GetIconDesc2(dwIconID).c_str());
	else 
	{
		if( pReceiver ==  pMainPlayer &&  pSender != pMainPlayer)
			_snprintf(szEmotionMsg,sizeof(szEmotionMsg),"%s%s",pSender->GetName(),GetIconDesc3(dwIconID).c_str());
		else
		{
			//return;
		}
	}
	szEmotionMsg[sizeof(szEmotionMsg)-1] = 0;
	GetGameGuiManager()->AddClientSystemMessage(szEmotionMsg,COLOR_ARGB(255,246,0,255));

	pSender->m_BitmapShow.StartShow();
	int nFrame;
	GetAnimation(nFrame,dwIconID);
	pSender->m_BitmapShow.SetFrame(nFrame);

	// Play the sound
	std::string strMusic = GetEmotionMusic(dwIconID);
	if( strMusic != "")
	{
		char szFileName[MAX_PATH];
		sprintf(szFileName,"sound\\emotion\\%s",strMusic.c_str());
		PlayGameSound(szFileName,SoundType_UI);
	}

	FUNCTION_END;
}

CToolTips* CEmotionInfo::GetToolTips(DWORD dwIconID)
{
	FUNCTION_BEGIN;

	g_EmotionTips.Clear();

	g_EmotionTips.SetBkColor(COLOR_ARGB(128,0,0,0));
	g_EmotionTips.SetAlignType( GUI_ALIGN_LEFT );
	g_EmotionTips.SetLineGaps( 3 );
	g_EmotionTips.SetCurColor(COLOR_ARGB(255,255,255,255));

	char szTemp[512];
	_snprintf(szTemp,sizeof(szTemp),"%s",GetIconDesc1(dwIconID).c_str());
	szTemp[sizeof(szTemp)-1]=0;
	g_EmotionTips.AddText(szTemp);
	g_EmotionTips.Resize(); 
	return &g_EmotionTips;

	FUNCTION_END;
}

////////////////////////////////////////////////////////////////////////////
CGuiEmotionDialog::CGuiEmotionDialog(void)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;
	m_dwCurAccessKeyBtn = 0;
	m_dwCurIconID       = 0;

	m_pScrollBar = NULL;

	FUNCTION_END;
}

CGuiEmotionDialog::~CGuiEmotionDialog(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}


void CGuiEmotionDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	m_vecEmotionIcon.clear();
	m_mapEmotionIcon.clear();
	
	FUNCTION_END;
}

void CGuiEmotionDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	/*m_BackElements.resize(2);

	CGuiElement* pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,246,201,3);
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_TOP | DT_LEFT;
	pEmt->iFont = 1;
	m_BackElements[0] = pEmt;

	pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,43,30,2);
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_TOP | DT_LEFT;
	pEmt->iFont = 1;
	m_BackElements[1] = pEmt;*/

	m_dwCurIconID = 0 ; 
	OnResetScreenSize();

	InitIcon();

	InitScrollBar();

	UpdateIcon();

	UpdateSrollRects();

	CreateAccessKey();	

	FUNCTION_END;
}

void CGuiEmotionDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();
	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());
		m_pBtnClose->SetLocation(pt.x,pt.y+26);
	}

	FUNCTION_END;
}


void CGuiEmotionDialog::InitIcon()
{
	FUNCTION_BEGIN;

	m_mapEmotionIcon.clear();
	m_vecEmotionIcon.clear();

	stResourceLocation rl;
	DWORD dwState[] = { GUI_STATE_NORMAL,GUI_STATE_PRESSED,GUI_STATE_DISABLED };
	IBitmapList * pBmpList = GetDevice()->FindResource(c_IconGL);
	//size_t framecount = pBmpList->GetFrameCount(EMOTION_GROUP);  

	const tMapEmotionDesc& map = GetEmotionInfo()->GetEmotionMap();

	for( _CIterator iter = map.begin() ; iter != map.end(); ++iter)
	{
		DWORD dwIconId = (*iter).first;
		int nFrame = dwIconId-EMOTION_ICON_ID_BASE;
		if( pBmpList->IsExist(EMOTION_GROUP,nFrame) )
		{
			DWORD dwNewKey   = nFrame + EMOTION_ICON_ID_BASE;
			DWORD dwNewBtnID = nFrame + EMOTION_BTN_ID_BASE ;
			stEmotionIcon & icon = m_mapEmotionIcon[dwNewKey];
			icon.pButton = GetButton(dwNewBtnID);
			if( NULL == icon.pButton )
			{
				stPointI pt(0,0);
				CGuiButton* pBtn;
				AddButton(dwNewBtnID, "" , pt.x , pt.y , 32 , 32 , 0 , false , &pBtn );
				pBtn->SetButtonType( CGuiButton::ButtonType_Bitmap );
				pBtn->SetBitmap( stResourceLocation(c_IconGL,EMOTION_GROUP,nFrame) , 1 , dwState);

				stRectI rc = pBtn->GetBoundingBox();
				stPointF scale(1.0f,1.0f);
				if( rc.Width ()> ICON_GRID_WIDTH || rc.Height() > ICON_GRID_HEIGHT)
				{
					scale.x = (float)ICON_GRID_WIDTH / (float)rc.Width();
					scale.y = (float)ICON_GRID_HEIGHT / (float)rc.Height();  
					if( scale.x < scale.y )
						scale.y = scale.x;
					else
						scale.x = scale.y;
				}
				pBtn->SetBitmapScale(scale);

				m_mapEmotionIcon[dwNewKey].pButton = pBtn;
				m_vecEmotionIcon.push_back(dwNewKey);
				//m_mapEmotionIcon[dwNewKey].pButton->SetUserData((void *)(dwNewKey));
			}
			m_mapEmotionIcon[dwNewKey].pButton->SetVisible(false);
		}
	}

	FUNCTION_END;
}

void CGuiEmotionDialog::UpdateIcon()
{
        FUNCTION_BEGIN;

	int nCurLine = m_pScrollBar->GetValue();
        int nCurIconIndex = nCurLine  * NUM_COL;

	int _HideStartIndex,_HideEndIndex;
	if( g_nDeltaPos > 0 )
	{
		_HideStartIndex = nCurIconIndex - g_nDeltaPos * NUM_COL;
		_HideEndIndex  = nCurIconIndex; //  not include the last one 
	}
	else 
	{
		_HideStartIndex = nCurIconIndex + NUM_ROW * NUM_COL;
		_HideEndIndex   = _HideStartIndex + ( -g_nDeltaPos) * NUM_COL; // not include the last one 
	}

	if( _HideStartIndex < 0 ) 
		_HideStartIndex = 0;
	if (_HideEndIndex > m_vecEmotionIcon.size()) 
		_HideEndIndex = m_vecEmotionIcon.size();
	for( ;_HideStartIndex < _HideEndIndex;_HideStartIndex++)
	{
		int nIconID = m_vecEmotionIcon[_HideStartIndex];
		tMapEmotionIcon::iterator it1 = m_mapEmotionIcon.find(nIconID);
		if( it1 != m_mapEmotionIcon.end() )
		{
			stEmotionIcon& st = it1->second;
			st.pButton->SetVisible(false);
		}
	}

	stPointI _ptOffsetOfClip(81,196);
	int j = nCurIconIndex;
        while( j < m_vecEmotionIcon.size()  &&  j < (nCurIconIndex + NUM_COL * NUM_ROW) )
	{
		
		int nIconID = m_vecEmotionIcon[j];
		tMapEmotionIcon::iterator it1 = m_mapEmotionIcon.find(nIconID);
		if( it1 != m_mapEmotionIcon.end() )
		{
			stPointI pt;
			pt.x = ((j-nCurIconIndex)%NUM_COL) *  POS_X_GAP + _ptOffsetOfClip.x;
			pt.y = ((j-nCurIconIndex)/NUM_COL) *  POS_Y_GAP + _ptOffsetOfClip.y;
			stEmotionIcon& st = it1->second;
			
                        /////////////////////////////////////////////////////////////////////

			 
			stPointF scale(1.0f,1.0f);

			stResourceLocation rl;
			stRectI rcBitmap(0,0,0,0);
			stRectI rcClip(0,0,0,0);
			int fNum = 0;
			stPointF ptOffset(0.0f,0.0f);
			if ( st.pButton->GetBitmapInfo( rl, fNum ) )
			{
			
				IBitmap * pBitmap = GetDevice()->FindBitmap(&rl); 
				//rcBitmap.right  = pBitmap->GetWidth();
				//rcBitmap.bottom = pBitmap->GetHeight();
				rcClip = pBitmap->GetClipRect();
				if( rcClip.Width() > ICON_GRID_WIDTH || rcClip.Height() > ICON_GRID_HEIGHT)
				{
					scale.x = (float)ICON_GRID_WIDTH / (float)rcClip.Width();
					scale.y = (float)ICON_GRID_HEIGHT / (float)rcClip.Height();  
					if( scale.x < scale.y )
						scale.y = scale.x;
					else
						scale.x = scale.y;
				}		
				st.pButton->SetBitmapScale(scale);
				st.pButton->SetLocation(pt.x + (ICON_GRID_WIDTH - rcClip.Width() * scale.x)/2 - rcClip.left  * scale.x ,pt.y + (ICON_GRID_HEIGHT - rcClip.Height() * scale.y)/2 - rcClip.top * scale.y);
				st.pButton->SetVisible(true);
			}			
		}
		j++;
		
	}

	FUNCTION_END;
}

void CGuiEmotionDialog::InitScrollBar()
{
	FUNCTION_BEGIN;

	//Get scroll bar,must first call InitIcon() to set the scrollpos 
	m_pScrollBar = (CGuiScrollBar*)GetControl( 9, GUI_CONTROL_SCROLLBAR );
	if ( m_pScrollBar )
	{
		m_pScrollBar->SetStyle(CGuiScrollBar::NOSKIN);
		m_pScrollBar->SetRange( 0, (m_vecEmotionIcon.size() + NUM_COL-1) / NUM_COL);
		m_pScrollBar->SetValue( 0 );
		g_nOldScrollPos = 0;
		m_pScrollBar->SetPageSize(NUM_ROW );
		m_pScrollBar->SetTrackRange(0,(m_vecEmotionIcon.size() + NUM_COL-1)/ NUM_COL);
	}

	FUNCTION_END;
}

void CGuiEmotionDialog::UpdateSrollRects()
{
	FUNCTION_BEGIN;

	if( m_pScrollBar )
	{
		stRectI rect(365,177,380,463);
		m_pScrollBar->SetLocation( rect.left, rect.top );
		m_pScrollBar->SetSize( 14, rect.Height() );
	}

	FUNCTION_END;
}

void CGuiEmotionDialog::CreateAccessKey()
{
	FUNCTION_BEGIN;

	DWORD dwState[] = { GUI_STATE_NORMAL,GUI_STATE_PRESSED,GUI_STATE_DISABLED };
	
	for(int i = 0; i< ACCESS_NUM;i++)
	{
		m_AccessKeyBtn[i].pButton = GetButton(i + 100);
		if( m_AccessKeyBtn[i].pButton == NULL)
		{			 
			CGuiButton* pBtn;
			stPointI pt(c_pt0AccessBtn.x + c_xAccessBtnDiff * i,c_pt0AccessBtn.y);
			AddButton(i +  100,"",pt.x,pt.y,32,32,0,false,&pBtn);  // 100~104
			pBtn->SetButtonType(CGuiButton::ButtonType_Bitmap);
			m_AccessKeyBtn[i].pButton = pBtn;
			m_AccessKeyBtn[i].dwKey = 0;
			m_AccessKeyBtn[i].rcClick.left = pt.x ;
			m_AccessKeyBtn[i].rcClick.top = pt.y ;
			m_AccessKeyBtn[i].rcClick.right =  m_AccessKeyBtn[i].rcClick.left + ICON_GRID_WIDTH;
			m_AccessKeyBtn[i].rcClick.bottom = m_AccessKeyBtn[i].rcClick.top + ICON_GRID_HEIGHT;
			pBtn->SetVisible(false);
		}
	}

	for(int i= 0;i<ACCESS_NUM;i++)
	{
		if( GetEmotionInfo()->GetAccessKey(i) != 0)
			__SetAccessKey(GetEmotionInfo()->GetAccessKey(i),100+i);
	}

	FUNCTION_END;
}

void CGuiEmotionDialog::OnResetScreenSize()
{
      FUNCTION_BEGIN;

      m_x = GetDevice()->GetWidth() - GetWidth();
      stRectI rcClip = GetClipRect();
      m_y = (GetDevice()->GetHeight() - rcClip.Height()) /2 - rcClip.top;

      FUNCTION_END;
}

void CGuiEmotionDialog::InitCustomElements()
{
	FUNCTION_BEGIN;

	stResourceLocation rl;
	rl.SetFileName(GetGuiGraphicPackName());
	rl.group = 26;
	rl.frame = 3;

	stRectI rcTexture;
	CGuiElement Element;


	////------------------------------------
	//// Element for the 3D Border ScrollBar(滚动条)
	////------------------------------------
	//{
	//	stRectI rects[9];
	//	rl.group = 11;
	//	rl.frame = 11;
	//	stGuiCtrlBorderInfo * pInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_SCROLLBAR);
	//	pInfo->rcTexture.SetRect(0,0,10,153);
	//	pInfo->ptLeftTop = stPointI(3,15);
	//	pInfo->ptRightBottom = stPointI(3,15);
	//	pInfo->ptBorderLeftTop = stPointI(3,-7);
	//	pInfo->ptBorderRightBottom = stPointI(3,-7);
	//	pInfo->CuttRect9(rects);
	//	m_nSBWidth = pInfo->rcTexture.Width();
	//	for(size_t i=0;i<count_of(rects);++i)
	//	{
	//		pInfo->element[i].SetTexture(&rl,&rects[i]);
	//		pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
	//		pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	//	}
	//}

	//-------------------------------------
	// CGuiScrollBar - Up Arrow
	//-------------------------------------
	rcTexture.SetRect( 0, 212, 14, 230);
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 0, &Element );


	rcTexture.SetRect( 16, 212, 30, 230);
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 1, &Element );


	//-------------------------------------
	// CGuiScrollBar - Down Arrow
	//-------------------------------------
	rcTexture.SetRect( 0, 233,14, 251 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 2, &Element );

	rcTexture.SetRect( 16, 233,30, 251 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 3, &Element );

	//-------------------------------------
	// CGuiScrollBar - Button
	//-------------------------------------
	rcTexture.SetRect( 34, 209, 56, 231 );
	Element.SetTexture( &rl, &rcTexture );

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 4, &Element );

	FUNCTION_END;
}


bool CGuiEmotionDialog::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		{
		
			if(GetGuiManager()->GetMouseOverDlg() != this)
				return false;
                        if(!IsFocus() )
				GetGuiManager()->RequestFocus(this);
	

			if( GetGameCursor()->GetCursorImageType() == eCursorSetAccessInEmotion)
				RestoreCursorImage();

			//stPointI ptCursor(short(LOWORD(lParam)), short(HIWORD(lParam)));
			stPointI ptCursor = Engine_GetCursor()->GetPosition();
			for(int i = 0 ; i < ACCESS_NUM;i++)
			{
				stRectI rect = m_AccessKeyBtn[i].rcClick;
				rect.OffsetRect(GetClientX(),GetClientY());
				if( rect.PtInRect(ptCursor) && !m_AccessKeyBtn[i].pButton->IsVisible())
				{
					 OnGuiEvent(EVENT_BUTTON_CLICKED,100+i,NULL);
                                         break;
				}
			}
		}
		break;
	}
	return  CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);

	FUNCTION_END;
}


bool CGuiEmotionDialog::OnGuiEvent( UINT nEvent,UINT nID,CGuiControl* pControl )
{
	FUNCTION_BEGIN;

	switch( nEvent )
	{
	case EVENT_BUTTON_DRAG_END: // 设置快捷键结束
		{
			if( GetGameCursor()->GetCursorImageType() != eCursorDictate)
				break;
			if(GetGuiManager()->GetMouseOverDlg() != this)
				break;
			stPointI ptCursor = Engine_GetCursor()->GetPosition();
	
			bool bUpdated = false;
			bool bInAccessBtn  = false;
			int  nInBtnAccess = -1;
			for(int i = 0 ; i< ACCESS_NUM ; i++)
			{
				if(m_AccessKeyBtn[i].pButton)
				{
					   stRectI rect = m_AccessKeyBtn[i].rcClick;
					   rect.OffsetRect(GetClientX(),GetClientY());
					   if( rect.PtInRect(ptCursor))
					   {
						   bInAccessBtn = true;
						   nInBtnAccess = 100 + i;
						   if( m_AccessKeyBtn[i].dwKey != 0 )
						   {
							   if(m_AccessKeyBtn[i].dwKey != m_dwCurIconID)
							   {
								   ReplaceAccessKey(m_dwCurIconID,m_AccessKeyBtn[i].dwKey,m_AccessKeyBtn[i].pButton->GetID());
								   bUpdated = true;
							   }								   
						   }
						   else
						   {
							   SetAccessKey(m_dwCurIconID,m_AccessKeyBtn[i].pButton->GetID());
							   bUpdated =  true;
						   }							  
						   break;  
					   }
				   }
			}
			if( m_IconFrom == FROM_ACCESS )
			{
				if( bUpdated || !bInAccessBtn) // 拖近来被重新设置或者从快捷键拖出(目标不在格子里)直接删掉
           				DeleteAccessKey(m_dwCurIconID);
				if( bInAccessBtn)
				{
					if( nInBtnAccess == m_dwCurAccessKeyBtn)
					{ //如果还在格子内，代表是点击
					}
					else
					{
						RestoreCursorImage();
					}
				}
				else
				{
					RestoreCursorImage();
				}
			}
			if( m_IconFrom == FROM_ICON && bInAccessBtn)
			{
				RestoreCursorImage();
			}
		}
		break;
	case EVENT_BUTTON_DRAG_MOVE:   // 设置快捷键开始
		if( GetGameCursor()->GetCursorImageType() == eCursorSetAccessInEmotion)
		{
			// 这里的作法太臭,当程序运行到这代表EVENT_BUTTON_CLICKED刚刚设置完一个快截键,
			// 可以gui底层还是给我发了一个EVENT_BUTTON_DRAG_MOVE,而事实上不需要,所以恢复光标直接跳出
			//RestoreMouseIcon();
			break;
		}
		if ( !GetGameCursor()->IsDrag())
		{
			tMapEmotionIcon::iterator it = m_mapEmotionIcon.find(nID - EMOTION_BTN_ID_BASE + EMOTION_ICON_ID_BASE);
			if ( it != m_mapEmotionIcon.end() )
			{ 
				stEmotionIcon* pEmotion = &(it->second);
				if ( pEmotion)
				{
					if ( (abs(pEmotion->pButton->GetDragOffset().x) >= 5) || (abs(pEmotion->pButton->GetDragOffset().y) >= 5) )
					{
						stResourceLocation rl;
						int fNum = 0;
						if ( pEmotion->pButton->GetBitmapInfo( rl, fNum ) )
						{
							////Get active emotion icon
							m_dwCurIconID = it->first; 
							GetEmotionInfo()->SetCurIconInfo(m_dwCurIconID,FROM_EMOTION);
							m_IconFrom = FROM_ICON;  //从图标上拖动
							SetCursorImage(rl);
							break;
						}
					}
				}
			}
			else
			{
				if( nID >= 100 && nID < 105)
				{
					CGuiButton * pButton = GetButton(nID);
					if( pButton)
					{
						if ( (abs(pButton->GetDragOffset().x) >= 5) || ((pButton->GetDragOffset().y) >= 5) )
						{
							stResourceLocation rl;
							int fNum = 0;
							if ( pButton->GetBitmapInfo( rl, fNum ) )
							{
								////Get active emotion icon
								m_dwCurIconID =  m_AccessKeyBtn[nID-100].dwKey; 
								GetEmotionInfo()->SetCurIconInfo(m_dwCurIconID,FROM_EMOTION);
								assert( m_dwCurIconID != 0 );
								if( m_dwCurIconID == 0 )
									break;
								m_IconFrom = FROM_ACCESS; //从快捷键上拖动
								m_dwCurAccessKeyBtn = nID;
							        SetCursorImage(rl);
								break;
							}
						}
					}
				}
			}

		}	
		break;
	case EVENT_BUTTON_CLICKED:     // 使用快捷键
		{
			if( nID >= 100 && nID < 105)
			{		
				DWORD dwState[] = { GUI_STATE_NORMAL,GUI_STATE_PRESSED,GUI_STATE_DISABLED };
				if( m_AccessKeyBtn[nID-100].dwKey != 0)
				{ 
					if(  m_dwCurIconID != 0 && GetGameCursor()->GetCursorImageType() == eCursorDictate)
					{ //Replace a exist AccessKey
						ReplaceAccessKey(m_dwCurIconID,m_AccessKeyBtn[nID-100].dwKey,m_AccessKeyBtn[nID-100].pButton->GetID());    
						RestoreCursorImage();
					}
					else   if( GetGameCursor()->GetCursorImageType() == eCursorNormal)
					{ //Use a exist AccessKey
						m_dwCurIconID =  m_AccessKeyBtn[nID-100].dwKey;
						GetEmotionInfo()->SetCurIconInfo(m_dwCurIconID,FROM_EMOTION);
						if( m_dwCurIconID == 0) 
							break;
						m_IconFrom = FROM_ACCESS;
						m_dwCurAccessKeyBtn = nID;
						stResourceLocation rl;
						rl.SetFileName(c_IconGL);
						rl.group = EMOTION_GROUP;
						rl.frame = m_dwCurIconID - EMOTION_ICON_ID_BASE;
						SetCursorImage(rl);	
					}
				}
				else
				{//Set a new Accesskey
                                         if( m_dwCurIconID != 0  && GetGameCursor()->GetCursorImageType() == eCursorDictate )
					 {
						 SetAccessKey(m_dwCurIconID,nID);  
						 RestoreCursorImage();
						 GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorSetAccessInEmotion );
					 }
				}	
			}
			else
			{
				tMapEmotionIcon::iterator it = m_mapEmotionIcon.find(nID - EMOTION_BTN_ID_BASE + EMOTION_ICON_ID_BASE);
				if ( it != m_mapEmotionIcon.end() )
				{
					stEmotionIcon* pEmotion = &(it->second);
					stResourceLocation rl;
					int fNum = 0;
					if ( pEmotion && pEmotion->pButton->GetBitmapInfo( rl, fNum ) )
					{
						//Get active emotion icon
						m_dwCurIconID = it->first; 
						GetEmotionInfo()->SetCurIconInfo(m_dwCurIconID,FROM_EMOTION);
						m_IconFrom = FROM_ICON;
						SetCursorImage(rl);
					}
				}
			}
		}
		break;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


void CGuiEmotionDialog::RestoreCursorImage()
{
	FUNCTION_BEGIN;
        
	RestoreMouseIcon();
	m_dwCurIconID = 0;	
	GetEmotionInfo()->SetCurIconInfo(m_dwCurIconID,FROM_EMOTION);
	m_IconFrom = FROM_NONE;

	FUNCTION_END;
}

void CGuiEmotionDialog::SetCursorImage(const stResourceLocation & rl)
{
	FUNCTION_BEGIN;

	//Change mouse icon
	stPointI ptCursor(0,0);
	IBitmaps * pBmps = GetDevice()->FindBitmaps(&rl);
	if( pBmps )
	{
		IBitmap * pBitmap = pBmps->GetBitmap(0);
		if ( pBitmap )
		{
			ptCursor.x += pBitmap->GetClipWidth()/2;
			ptCursor.y += pBitmap->GetClipHeight()/2;
		}
	}

	//ptCursor.x += pEmotion->pButton->GetWidth()/2;
	//ptCursor.y += pEmotion->pButton->GetHeight()/2;

	//GetGameCursor()->SetCursorImageType( eCursorSetAccess );
	GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorDictate );
	Engine_GetCursor()->SetImage( &rl, ptCursor );
      
	FUNCTION_END;
}

void CGuiEmotionDialog::__SetAccessKey(DWORD iIconID,DWORD nBtnID)
{
	FUNCTION_BEGIN;
	assert(nBtnID >= 100 && nBtnID < 100 + ACCESS_NUM);
	if(nBtnID < 100 || nBtnID >= 100 + ACCESS_NUM )
		return;

	if(iIconID == 0 )
		return;
        
	DWORD dwBtnIndex = nBtnID - 100;
	if( dwBtnIndex < 0 || dwBtnIndex >= ACCESS_NUM)
		return;

	// check the all five access to avoid the same accesskey was setted to the more than two buttons z
	for( int __i = 0 ;__i < ACCESS_NUM; __i++)
	{
		if(m_AccessKeyBtn[__i].dwKey == iIconID)
		{
			 
			m_AccessKeyBtn[__i].dwKey = 0;
			if( m_AccessKeyBtn[__i].pButton) 
				m_AccessKeyBtn[__i].pButton->SetVisible(false);

			//Save access key change
			BYTE szAccelKey[4] = {0};
			szAccelKey[0] = VK_MENU;
			szAccelKey[1] = c_AccessKy[__i];
			szAccelKey[2] = 0;
			 
			GetGameAccelKeyManager()->DeleteOneAccelKey(szAccelKey,(BYTE)EMOTION_ACCESS_KEY_GROUP);
		 
		}
	}
	DWORD dwState[] = { GUI_STATE_NORMAL,GUI_STATE_PRESSED,GUI_STATE_DISABLED };
	stResourceLocation rl;
	rl.SetFileName(c_IconGL);
	rl.group = EMOTION_GROUP;
	rl.frame = iIconID - EMOTION_ICON_ID_BASE;
	m_AccessKeyBtn[dwBtnIndex].pButton->SetVisible(true);
	m_AccessKeyBtn[dwBtnIndex].pButton->SetBitmap(rl,1,dwState);
	m_AccessKeyBtn[dwBtnIndex].dwKey = iIconID;
	stRectI rcBtn = m_AccessKeyBtn[dwBtnIndex].rcClick;

	stRectI rcClip(0,0,0,0);
	stRectI rcBitmap(0,0,0,0);
	
	IBitmaps * pBmps = GetDevice()->FindBitmaps(&rl);
	if( pBmps)
	{
		IBitmap * pBmp = pBmps->GetBitmap(0);
		if( pBmp)
		{
			rcClip = pBmp->GetClipRect();
			rcBitmap.right  = pBmp->GetWidth();
			rcBitmap.bottom = pBmp->GetHeight();
		}
	}
	if( rcClip.Width() == 0 || rcClip.Height() == 0)
		rcClip = rcBtn;


	stPointF scale(1.0f,1.0f);
	if( rcClip.Width() > ICON_GRID_WIDTH || rcClip.Height() > ICON_GRID_HEIGHT)
	{
		scale.x = (float)ICON_GRID_WIDTH / (float)rcClip.Width();
		scale.y = (float)ICON_GRID_HEIGHT / (float)rcClip.Height();  
		if( scale.x < scale.y )
			scale.y = scale.x;
		else
			scale.x = scale.y;
	}		
        m_AccessKeyBtn[dwBtnIndex].pButton->SetBitmapScale(scale);
	m_AccessKeyBtn[dwBtnIndex].pButton->SetLocation(rcBtn.left + (ICON_GRID_WIDTH - rcClip.Width() * scale.x) /2  - rcClip.left * scale.x ,  rcBtn.top + (ICON_GRID_HEIGHT - rcClip.Height() * scale.y )/2 - rcClip.top * scale.y);

	FUNCTION_END;
}

void CGuiEmotionDialog::SetAccessKey(DWORD iIconID,DWORD nBtnID)
{
	FUNCTION_BEGIN;

       assert(nBtnID >= 100 && nBtnID < 100 + ACCESS_NUM);
       if(nBtnID < 100 || nBtnID >= 100 + ACCESS_NUM )
	       return;

       if(iIconID == 0 )
	       return;

       __SetAccessKey(iIconID,nBtnID);

       //Save access key change
       stGameAccelKey key;
       key.type = GameAccelKeyType_Face;
       key.dwID = iIconID;
       key.szAccelKey[0] = VK_MENU;
       key.szAccelKey[1] = c_AccessKy[nBtnID-100];
       key.szAccelKey[2] = 0;
       key.mGroup = (BYTE)EMOTION_ACCESS_KEY_GROUP;

       GetGameAccelKeyManager()->SetKey(key);
       GetGameAccelKeyManager()->Save( EMOTION_ACCESS_KEY_GROUP);

       FUNCTION_END;
}

void CGuiEmotionDialog::DeleteAccessKey(DWORD iIconID)
{
	FUNCTION_BEGIN;

	assert(m_dwCurAccessKeyBtn !=0);
	if( m_dwCurAccessKeyBtn == 0)
		return;

	int nBtnIndex = m_dwCurAccessKeyBtn - 100;
	if( nBtnIndex < 0 || nBtnIndex >= ACCESS_NUM)
		return;

	m_AccessKeyBtn[nBtnIndex].dwKey = 0;
	if( m_AccessKeyBtn[nBtnIndex].pButton) 
		m_AccessKeyBtn[nBtnIndex].pButton->SetVisible(false);


	/*BYTE id[4];
	BYTE group;
	enumGameAccelKeyType type = GameAccelKeyType_Face;
	GetGameAccelKeyManager()->DeleteAccelKey(type,iIconID,id,group);*/

	//Save access key change
	BYTE szAccelKey[4] = {0};
	szAccelKey[0] = VK_MENU;
	szAccelKey[1] = c_AccessKy[nBtnIndex];
	szAccelKey[2] = 0;

	GetGameAccelKeyManager()->DeleteOneAccelKey(szAccelKey,(BYTE)EMOTION_ACCESS_KEY_GROUP);


	GetGameAccelKeyManager()->Save( EMOTION_ACCESS_KEY_GROUP);

	FUNCTION_END;
}


void CGuiEmotionDialog::ReplaceAccessKey(DWORD iIconID,DWORD iOldIconID,int nBtnID)
{
	FUNCTION_BEGIN;

	if( nBtnID < 100 && nBtnID >= (100 + ACCESS_NUM) )
		return;

	// check the all five access to avoid the same accesskey was setted to the more than two buttons 
	for( int __i = 0 ;__i < ACCESS_NUM; __i++)
	{
		if(m_AccessKeyBtn[__i].dwKey == iIconID)
		{

			m_AccessKeyBtn[__i].dwKey = 0;
			if( m_AccessKeyBtn[__i].pButton) 
				m_AccessKeyBtn[__i].pButton->SetVisible(false);

			//BYTE id[4];
			//BYTE group;
			//enumGameAccelKeyType type = GameAccelKeyType_Face;
			//GetGameAccelKeyManager()->DeleteAccelKey(type,iIconID,id,group);

			//Save access key change
			BYTE szAccelKey[4] = {0};
			szAccelKey[0] = VK_MENU;
			szAccelKey[1] = c_AccessKy[__i];
			szAccelKey[2] = 0;

			GetGameAccelKeyManager()->DeleteOneAccelKey(szAccelKey,(BYTE)EMOTION_ACCESS_KEY_GROUP);


		}
	}

	DWORD dwState[] = { GUI_STATE_NORMAL,GUI_STATE_PRESSED,GUI_STATE_DISABLED };
	stResourceLocation rl;
	rl.SetFileName(c_IconGL);
	rl.group = EMOTION_GROUP;
	rl.frame = iIconID - EMOTION_ICON_ID_BASE;
	DWORD dwBtnIndex = nBtnID - 100;  
	m_AccessKeyBtn[dwBtnIndex].pButton->SetVisible(true);
	m_AccessKeyBtn[dwBtnIndex].pButton->SetBitmap(rl,1,dwState);
	m_AccessKeyBtn[dwBtnIndex].dwKey = iIconID;

	stRectI rcBtn = m_AccessKeyBtn[dwBtnIndex].rcClick;
	stRectI rcClip(0,0,0,0);
	IBitmaps * pBmps = GetDevice()->FindBitmaps(&rl);
	stRectI rcBitmap(0,0,0,0);
	if( pBmps)
	{
		IBitmap * pBmp = pBmps->GetBitmap(0);
		if( pBmp)
		{
			rcClip = pBmp->GetClipRect();
			rcBitmap.right  = pBmp->GetWidth();
			rcBitmap.bottom = pBmp->GetHeight();
		}
	}
	if( rcClip.Width() == 0 || rcClip.Height() == 0)
		rcClip = rcBtn;

	stPointF scale(1.0f,1.0f);
	if( rcClip.Width() > ICON_GRID_WIDTH || rcClip.Height() > ICON_GRID_HEIGHT)
	{
		scale.x = (float)ICON_GRID_WIDTH / (float)rcClip.Width();
		scale.y = (float)ICON_GRID_HEIGHT / (float)rcClip.Height();  
		if( scale.x < scale.y )
			scale.y = scale.x;
		else
			scale.x = scale.y;
	}		
	m_AccessKeyBtn[dwBtnIndex].pButton->SetBitmapScale(scale);
	m_AccessKeyBtn[dwBtnIndex].pButton->SetLocation(rcBtn.left + (ICON_GRID_WIDTH - rcClip.Width() * scale.x )/2   - rcClip.left * scale.x ,  rcBtn.top + (ICON_GRID_HEIGHT - rcClip.Height() * scale.y )/2  - rcClip.top * scale.y);


	enumGameAccelKeyType type = GameAccelKeyType_Face;
	GetGameAccelKeyManager()->ReplaceAccelKey(type,iOldIconID,iIconID);
	GetGameAccelKeyManager()->Save( EMOTION_ACCESS_KEY_GROUP);
	
	FUNCTION_END;
}

void CGuiEmotionDialog::OnMouseWheel()
{
	FUNCTION_BEGIN;

	if ( abs(g_nDeltaPos) < 1  )
		return;
	if( m_pScrollBar )
	{
		UpdateIcon();
		g_nOldScrollPos = m_pScrollBar->GetValue();
	}

	FUNCTION_END;
}

HRESULT CGuiEmotionDialog::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;

	if ( m_pScrollBar )
	{
		g_nDeltaPos = m_pScrollBar->GetValue() - g_nOldScrollPos;
		OnMouseWheel();
	}

	HRESULT hr =  CGuiDialog::OnRender(fElapsedTime);

	//Draw scroll bar
	if ( m_pScrollBar )
		m_pScrollBar->Render( fElapsedTime );


	if( GetGuiManager()->GetMouseOverDlg() != this)
		return hr;

	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	bool bSetTips = ( (GetGuiManager()->GetMouseOverDlg() == this) && ContainsPoint( ptCursor ) );
	ptCursor.x -= GetClientX();
	ptCursor.y -= GetClientY();
	for(tMapEmotionIcon::iterator it = m_mapEmotionIcon.begin(); it != m_mapEmotionIcon.end(); ++it)
	{
		stEmotionIcon& st = it->second;
		if(st.pButton->IsVisible())
		{
			stRectI rect = st.pButton->GetBoundingBox();
			rect.OffsetRect(GetClientX(),GetClientY());
			if((st.pButton->ContainsPoint(ptCursor))&&!GetGuiManager()->GetToolTips())
			{
				CToolTips* pTips = GetEmotionInfo()->GetToolTips(it->first);
				pTips->RenderTopLeftPrefer(rect,GetDevice()->GetWidth(),GetDevice()->GetHeight());
				//GetGuiManager()->SetToolTips(pTips,pt);
				break;
			}
		}
	}
	if( it == m_mapEmotionIcon.end())
	{
               for(int i = 0 ; i < ACCESS_NUM ; i++ )
	       {
                    stAccessKey & key =  m_AccessKeyBtn[i];
		    if( !key.pButton->IsVisible() )
			    continue;
                    stRectI rect = key.pButton->GetBoundingBox();
		    rect.OffsetRect(GetClientX(),GetClientY());
		    if(key.pButton->ContainsPoint(ptCursor))
		    {
			    CToolTips * pTips = GetEmotionInfo()->GetToolTips(key.dwKey);
			    pTips->RenderTopLeftPrefer(rect,GetDevice()->GetWidth(),GetDevice()->GetHeight());
			    break;
		    }
	       }
	}

#ifdef _DEBUG
	for(int i = 0 ; i< ACCESS_NUM ; i++ )
	{      
		stRectI rect = m_AccessKeyBtn[i].rcClick;
		rect.OffsetRect(GetClientX(),GetClientY());
		GetDevice()->DrawRectangle(&rect,COLOR_ARGB(255,54,241,54));
	}

	for(tMapEmotionIcon::iterator it = m_mapEmotionIcon.begin(); it != m_mapEmotionIcon.end(); ++it)
	{
		stEmotionIcon& st = it->second;
		if(st.pButton->IsVisible())
		{
			stRectI rect = st.pButton->GetBoundingBox();
			rect.OffsetRect(GetClientX(),GetClientY());
			GetDevice()->DrawRectangle(&rect,COLOR_ARGB(255,54,241,54));
		}
	}
	int j = 0;
	stPointI _ptOffsetOfClip(81,196);
	while(   j < NUM_COL * NUM_ROW )
	{
		stPointI pt;
		pt.x = ( j%NUM_COL ) *  POS_X_GAP + _ptOffsetOfClip.x;
		pt.y = ( j/NUM_COL ) *  POS_Y_GAP + _ptOffsetOfClip.y;
		stRectI rect;
		rect.left = pt.x;
		rect.top = pt.y;
		rect.right = rect.left + ICON_GRID_WIDTH;
		rect.bottom =  rect.top + ICON_GRID_HEIGHT;
		rect.OffsetRect(GetClientX(),GetClientY());
		GetDevice()->DrawRectangle(&rect,COLOR_ARGB(255,54,241,54));
           
		j++;
	}
#endif
	return hr;	

	FUNCTION_END;
}



