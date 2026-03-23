#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../include/guimenu.h"

static stPointI s_ptBorderGrpLeftTop(4,4);
static stPointI s_ptBorderGrpRightBot(4,4);
static stPointI s_ptBorderLeftTop(4,4);
static stPointI s_ptBorderRightBot(4,4);

static int s_nItemTextX = 28;
static int s_nItemHeight = 21;
static int s_nSpaceHeight = 1;

CGuiElement CGuiMenu::s_aBorderElement[9];
CGuiElement CGuiMenu::s_aItemElement[9];
CGuiElement CGuiMenu::s_aItemSelectElement[9];
CGuiElement CGuiMenu::s_aSpaceElement[9];
CGuiElement CGuiMenu::s_aTextElement[4];
CGuiElement CGuiMenu::s_aCheckElement[9];

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiMenu::CGuiMenu(void)
: m_pOwner(NULL)
{
	m_clientOffsetTopLeft = stPointI(0,0);
	m_clientOffsetRightBottom = stPointI(0,0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiMenu::~CGuiMenu(void)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param cmd : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiMenu::GetMenuIndex(int cmd)
{
	for(size_t i=0;i<m_aItem.size();++i)
	{
		if(m_aItem[i].id == cmd)
			return i;
	}
	return -1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \param cmd : ÃèÊö
 * \param pszText : ÃèÊö
 * \param style : ÃèÊö
 * \param bEnabled : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMenu::SetMenuItem(int index,int cmd,const char* pszText,enumMenuStyle style,bool bEnabled)
{
	if( size_t(index) >= m_aItem.size() )
		return;

	stMenuItem & st = m_aItem[index];
	st.id = cmd;
	st.style = style;
	st.enabled = bEnabled;
	st.checked = false;
	st.text = pszText;
	UpdateRects();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \param bEnabled : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMenu::SetMenuEnabled(int index,bool bEnabled)
{
	if( size_t(index) >= m_aItem.size() )
		return;

	stMenuItem & st = m_aItem[index];
	st.enabled = bEnabled;
	UpdateRects();
}

void CGuiMenu::SetMenuItemChecked(int index,bool bChecked )
{
	if( size_t(index) >= m_aItem.size() )
		return;

	stMenuItem & st = m_aItem[index];
	st.checked = bChecked;
	UpdateRects();
}

void CGuiMenu::SetMenuItemChecked(int index)
{
	for(int i = 0 ; i < m_aItem.size() ; i ++)
	{
		if( i == index )
		{
			m_aItem[i].checked = true;
		}
		else
		{
			m_aItem[i].checked = false;
		}
	}
}

bool CGuiMenu::SetMenuItemEnbale(int cmd,bool bEnable)
{
	for(int i = 0 ; i < m_aItem.size() ; i ++)
	{
		if( m_aItem[i].id == cmd )
		{
			m_aItem[i].enabled = bEnable;
			return true;
		}
	}
	return false;
}

bool CGuiMenu::SetMenuItemCheck(int cmd,bool bCheck)
{
	for(int i = 0 ; i < m_aItem.size() ; i ++)
	{
		if( m_aItem[i].id == cmd )
		{
			m_aItem[i].checked = bCheck;
			return true;
		}
	}
	return false;
}

void CGuiMenu::SetMenuItemCheck(int cmd)
{
	for(int i = 0 ; i < m_aItem.size() ; i ++)
	{
		if( m_aItem[i].id == cmd )
		{
			m_aItem[i].checked = true;
		}
		else
		{
			m_aItem[i].checked = false;
		}
	}
}

void CGuiMenu::ClearAllCheck()
{
	for(int i = 0 ; i < m_aItem.size() ; i ++)
	{		
		m_aItem[i].checked = false;		
	}
}

void CGuiMenu::SetAllCheck()
{
	for(int i = 0 ; i < m_aItem.size() ; i ++)
	{		
		m_aItem[i].checked = true;		
	}
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param cmd : ÃèÊö
 * \param pszText : ÃèÊö
 * \param style : ÃèÊö
 * \param bEnabled : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMenu::AddMenuItem(int cmd,const char* pszText,enumMenuStyle style,bool bEnabled)
{
	for(int i = 0 ; i < m_aItem.size() ; i ++)
	{
		if( m_aItem[i].id == cmd )
			return;
	}
	stMenuItem st;
	st.id = cmd;
	st.style = style;
	st.enabled = bEnabled;
	st.checked = false;
	st.text = pszText;
	m_aItem.push_back(st);
	UpdateRects();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMenu::UpdateRects()
{
	m_width = 156;
	m_height = s_ptBorderLeftTop.y;

	for(size_t i=0;i<m_aItem.size();++i)
	{
		int nHeight;
		if(m_aItem[i].style == MenuStyle_Separate)
			nHeight = s_nSpaceHeight;
		else
			nHeight = s_nItemHeight;

		m_aItem[i].rect.SetRect(
			s_ptBorderLeftTop.x,
			m_height,
			GetWidth() - s_ptBorderRightBot.x,
			m_height + nHeight);

		m_height += nHeight;
	}

	m_height +=  s_ptBorderRightBot.y;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMenu::InitDefaultElements()
{
	stResourceLocation rl;
	rl.SetFileName( GetGuiGraphicPackName() );
	rl.group = 14;
	rl.frame = 0;

	stRectI rcTexture;
	stRectI rects[9];

	//-------------------------------------------
	// ±ß¿ò
	//-------------------------------------------
	rcTexture.SetRect(0,47,159,256);
	CuttRect9(rcTexture,s_ptBorderGrpLeftTop,s_ptBorderGrpRightBot,rects);
	for(size_t i=0;i<9;++i)
	{
		s_aBorderElement[i].SetTexture(&rl,&rects[i]);
		s_aBorderElement[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	}
	//-------------------------------------------
	//-------------------------------------------

	//-------------------------------------------
	// Ñ¡Ïî
	//-------------------------------------------
	rcTexture.SetRect(0,0,153,20);
	CuttRect9(rcTexture,stPointI(23,0),stPointI(0,0),rects);
	for(size_t i=0;i<9;++i)
	{
		s_aItemElement[i].SetTexture(&rl,&rects[i]);
		s_aItemElement[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		s_aItemElement[i].SetFont(0,-1,DT_LEFT|DT_VCENTER);
		s_aItemElement[i].FontColor.Current = -1;
	}
	//-------------------------------------------
	//-------------------------------------------

	//-------------------------------------------
	// Ñ¡ÏîÑ¡ÖĞÊ±
	//-------------------------------------------
	rcTexture.SetRect(0,21,153,42);
	CuttRect9(rcTexture,stPointI(1,1),stPointI(1,1),rects);
	for(size_t i=0;i<9;++i)
	{
		s_aItemSelectElement[i].SetTexture(&rl,&rects[i]);
		s_aItemSelectElement[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	}
	//s_aItemSelectElement[i].SetFont( 0,-1,DT_LEFT | DT_VCENTER );
	//s_aItemSelectElement[i].FontColor.Current = -1;
	//-------------------------------------------
	//-------------------------------------------

	//-------------------------------------------
	// Ñ¡ÏîCheckedÊ±
	//-------------------------------------------
	stResourceLocation rl1;
	rl1.SetFileName( GetGuiGraphicPackName() );
	rl1.group = 0;
	rl1.frame = 1;
	rcTexture.SetRect(60,90,80,110);
	CuttRect9(rcTexture,stPointI(1,1),stPointI(1,1),rects);
	for(size_t i=0;i<9;++i)
	{
		s_aCheckElement[i].SetTexture(&rl1,&rects[i]);
		s_aCheckElement[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	}
	//s_aCheckElement[i].SetFont( 0,-1,DT_LEFT | DT_VCENTER );
	//s_aCheckElement[i].FontColor.Current = -1;
	//-------------------------------------------
	//-------------------------------------------

	//-------------------------------------------
	// ¼ä¸ôÌõ
	//-------------------------------------------
	rcTexture.SetRect(0,44,153,46);
	CuttRect9(rcTexture,stPointI(23,0),stPointI(3,0),rects);
	for(size_t i=0;i<9;++i)
	{
		s_aSpaceElement[i].SetTexture(&rl,&rects[i]);
		s_aSpaceElement[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	}
	//-------------------------------------------
	//-------------------------------------------
	
	s_aTextElement[0].SetFont( 0,-1,DT_LEFT | DT_VCENTER );
	s_aTextElement[0].FontColor.Current = COLOR_ARGB(255,0,255,0);

	s_aTextElement[1].SetFont( 0,-1,DT_LEFT | DT_VCENTER );
	s_aTextElement[1].FontColor.Current = COLOR_ARGB(255,128,128,128);

	s_aTextElement[2].SetFont( 0,-1,DT_LEFT | DT_VCENTER );
	s_aTextElement[2].FontColor.Current = COLOR_ARGB(255,255,255,50);
}

void CGuiMenu::SetMenuItemUseCustomeElement( int index, CGuiElement& customerElement )
{
	if ( (index<0) || (index>=m_aItem.size() ) )
		return;

	s_aTextElement[3].SetFont( 0, -1, DT_LEFT | DT_VCENTER );
	s_aTextElement[3].FontColor = customerElement.FontColor;

	m_aItem[index].useCustomerElement = true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMenu::OnCreate()
{
	CGuiDialog::OnCreate();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMenu::RenderItem(int index)
{
	stMenuItem & st = m_aItem[index];
	stRectI rcBack = st.rect;
	rcBack.OffsetRect(GetX(),GetY());
	stRectI rects[9];


	if(st.style != MenuStyle_Separate && st.enabled && rcBack.PtInRect(Engine_GetCursor()->GetPosition()) )
	{
		CuttRect9(rcBack,stPointI(1,1),stPointI(1,1),rects);
		for( size_t i=0; i<9 ; ++i )
		{
			PatchRenderImage(rects[i],s_aItemSelectElement[i].pBmp,&s_aItemSelectElement[i].rcTexture,-1,Blend_Null,s_PatchAlignFormat[i]);
		}
		stRectI rcText = st.rect;
		rcText.left += s_nItemTextX;
		DrawText(st.text,&s_aTextElement[2],&rcText,FontEffect_None);
	}
	else
	{
		if(st.style == MenuStyle_Separate)
		{
			CuttRect9(rcBack,stPointI(23,0),stPointI(0,0),rects);
			for( size_t i=0; i<9 ; ++i )
			{
				PatchRenderImage(rects[i],s_aSpaceElement[i].pBmp,&s_aSpaceElement[i].rcTexture,-1,Blend_Null,s_PatchAlignFormat[i]);
			}
		}
		else
		{
			CuttRect9(rcBack,stPointI(23,0),stPointI(0,0),rects);
			for( size_t i=0; i<9 ; ++i )
			{
				PatchRenderImage(rects[i],s_aItemElement[i].pBmp,&s_aItemElement[i].rcTexture,-1,Blend_Null,s_PatchAlignFormat[i]);
			}
			stRectI rcText = st.rect;
			rcText.left += s_nItemTextX;
			if(st.enabled)
			{
				if ( m_aItem[index].useCustomerElement )
					DrawText(st.text, &s_aTextElement[3], &rcText, FontEffect_None);
				else
					DrawText(st.text,&s_aTextElement[0],&rcText,FontEffect_None);
			}
			else
				DrawText(st.text,&s_aTextElement[1],&rcText,FontEffect_None);
		}
	}

	if( st.checked )
	{
		stRectI rcCheck(rcBack.left,rcBack.top,rcBack.left + 20,rcBack.top + 20);
		CuttRect9(rcCheck,stPointI(1,1),stPointI(1,1),rects);
		for( size_t i=0; i<9 ; ++i )
		{
			PatchRenderImage(rects[i],s_aCheckElement[i].pBmp,&s_aCheckElement[i].rcTexture,-1,Blend_Null,s_PatchAlignFormat[i]);
		}
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
HRESULT CGuiMenu::OnRender(float fElapsedTime)
{
	stRectI rcBack(GetX(),GetY(),GetWidth() + GetX(),GetHeight() + GetY());
	stRectI rects[9];
	CuttRect9(rcBack,s_ptBorderGrpLeftTop,s_ptBorderGrpRightBot,rects);
	for(size_t i=1;i<9;++i)
	{
		PatchRenderImage(rects[i],s_aBorderElement[i].pBmp,&s_aBorderElement[i].rcTexture,-1,Blend_Null,s_PatchAlignFormat[i]);
	}
	
	for(size_t i=0;i<m_aItem.size();++i)
	{
		RenderItem(i);
	}
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMenu::BeginPopup()
{
	CGuiDialog::BeginPopup();
	stPointI pt = Engine_GetCursor()->GetPosition();
	m_x = pt.x;
	m_y = pt.y - m_height;
	if(m_y < 0) m_y = pt.y;
	if(m_x + GetWidth() > GetDevice()->GetWidth()) m_x = pt.x - GetWidth();

	if(m_x < 0) m_x = 0;
	if(m_y < 0) m_y = 0;

	if(GetGuiManager()->CanHaveFocus(this))
		GetGuiManager()->RequestFocus(this);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMenu::Popup()
{
	BeginPopup();
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
bool CGuiMenu::MsgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if(uMsg == WM_LBUTTONUP)
	{
		for(size_t i=0;i<m_aItem.size();++i)
		{
			stMenuItem & st = m_aItem[i];
			stRectI rcBack = st.rect;
			rcBack.OffsetRect(GetX(),GetY());
			if(st.style != MenuStyle_Separate && st.enabled && rcBack.PtInRect(Engine_GetCursor()->GetPosition()) )
			{
				if(m_pOwner) 
				{
					if(GetGuiManager()->CanHaveFocus(m_pOwner))
						GetGuiManager()->RequestFocus(m_pOwner);
					m_pOwner->SendEvent(EVENT_DIALOG_COMMAND,true,NULL,st.id);
				}
				return true;
				break;
			}
		}
	}
	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);
}