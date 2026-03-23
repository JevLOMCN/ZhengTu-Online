#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../../xml_parse/XmlParseEx.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param time : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
extern void PlayUISound(DWORD dwSoundID);
const int s_nSBSpace = 8;
const float g_fShowTime = 300.0f;
const float g_fBlendTime = 5.0f;
const int s_nSBTopOffset = 13;
const int s_nSBBottomOffset = 8;
float getLastTime(float time)
{
	return g_fShowTime + g_fBlendTime - time;
}


int g_GMFilter = 255;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
float getLinkShowTime()
{
	return (g_fShowTime - 1.0f);
}
int   getAlpha(float time)
{
	return (time < g_fShowTime) ? 255 : (int)(getLastTime(time) * 255 / g_fBlendTime);
}
struct stFadeoutElement{
	enumFontEffect eFontEffect;
	DWORD         dwShowColor;
	DWORD         dwShadowColor;
	stFadeoutElement()
	{
		eFontEffect = FontEffect_None;
		dwShadowColor = 0xff000000;
	}
};

void DrawBackGround(const RECT &rc)
{
	static IBitmaps * pBmps = NULL;
	int x = rc.left, y = rc.top;
	if( !pBmps )
	{
		stResourceLocation rl;
		rl.SetFileName("data\\interfaces.gl"); 
		rl.group = 27;
		rl.frame = 2;
		pBmps = GetDevice()->FindBitmaps(&rl);
	}
	if(pBmps && pBmps->GetFrameCount())
	{	
		RECT rcClip  = pBmps->GetBitmap(0)->GetClipRect();
		
		x -= rcClip.left;
		y -= rcClip.top;

	/*	rcClip.top += rc.top;
		rcClip.bottom += rc.top-6;
		rcClip.left += rc.left;
		rcClip.right += rc.left-6;
		GetDevice()->FillRect(&rcClip,COLOR_ARGB(255,0,0,0));*/

		pBmps->Render(0,x,y);
	}
}

CGuiMLTextBox::NodeHyperlinkBase::NodeHyperlinkBase()
{
	//colorNormal = COLOR_ARGB(255,0,15,251);
	colorNormal = COLOR_ARGB(255,190,228,38);
	colorOver = COLOR_ARGB(255,255,255,255);
	colorDown = COLOR_ARGB(255,255,0,0);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param st : ÃèÊö
* \param style : ÃèÊö
* \param dwBlend : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void DoFadeout(stFadeoutElement& st,DWORD style,DWORD dwBlend)
{
	if (style & CGuiMLTextBox::GUI_TS_AUTOFADEOUT)
	{
		st.eFontEffect = FontEffect_Border;
		st.dwShowColor = ColorBlend(st.dwShowColor,dwBlend);
		st.dwShadowColor = ColorBlend(st.dwShadowColor,dwBlend);
	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
CGuiMLTextBox::CGuiMLTextBox(CGuiDialog* pDialog,DWORD dwStyle)
: CGuiControl(pDialog)
, m_nLineHeight(12)
, m_VScrollBar(pDialog)
, m_nSelect(-1)
, m_bFilter(false)
, m_bMouseDrag(false)
{
	m_VScrollBar.SetCanHaveFocus(false);
	m_nSBWidth = pDialog->m_nSBWidth;
	//m_nBorder = 5;  // Default border width
	m_nSpacing = 0;  // Default spacing
	m_Type = GUI_CONTROL_MLTEXTBOX;

	m_SelBkColor = D3DCOLOR_ARGB( 255, 0, 0, 255 );
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
CGuiMLTextBox::~CGuiMLTextBox(void)
{
	Clear();
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param ptOffset : ÃèÊö
* \param prect : ÃèÊö
* \param pTextBox : ÃèÊö
* \param dwBlend : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CGuiMLTextBox::NodeText::Draw(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend)
{
	if (!IsShow(ptOffset,prect))
		return false;

	stRectI rc = rect;
	DWORD format = 0;
	stFadeoutElement st;
	st.dwShowColor = color;
	/*st.dwShadowColor = COLOR_ARGB(255,255-COLOR_GET_R(color),255-COLOR_GET_G(color),
		255-COLOR_GET_B(color));*/

	DoFadeout(st,pTextBox->GetStyle(),dwBlend);
	rc.OffsetRect(ptOffset);

	GetDevice()->DrawString(text.c_str(),rc,st.dwShowColor,format,st.eFontEffect,st.dwShadowColor,(data.empty() ? NULL : &data[0]));
	return false;
}

bool CGuiMLTextBox::NodeUnknown::DrawSelBkColor( POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox )
{
	if (!IsShow(ptOffset,prect))
		return false;
	if ( !bSelected )
		return false;

	stRectI rc = rect;
	rc.OffsetRect(ptOffset);

	GetDevice()->FillRect( &rc, pTextBox->GetSelBkColor() );
	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param ptOffset : ÃèÊö
* \param prect : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CGuiMLTextBox::NodeUnknown::IsShow(POINT ptOffset,const RECT * prect)
{
	stRectI rc = rect;
	rc.OffsetRect(ptOffset);
	stRectI rc1 = rc & *prect;
	if(rc1.IsRectEmpty()) return false;
	return true;
}


bool CGuiMLTextBox::NodeUnknown::IsInRect(POINT ptOffset)
{
	stPointI pt(Engine_GetCursor()->GetPosition());
	stRectI rc = rect;
	rc.OffsetRect(ptOffset);
	return rc.PtInRect(pt);
}

bool CGuiMLTextBox::NodeImage::Draw(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend)
{
	if(!pImage) return false;
	if (!IsShow(ptOffset,prect))
		return false;

	stRectI rc = rect;
	rc.OffsetRect(ptOffset);

	int w = pImage->GetBitmap(0)->GetWidth();
	int h = pImage->GetBitmap(0)->GetHeight();
	stRectI clip(0,0,min(w,rc.Width()),min(h,rc.Height()));
	pImage->RenderAni(rc.left,rc.top,xtimeGetTime(),&clip,NULL);

	return true;
}

bool CGuiMLTextBox::NodeImage::HandleMouse( UINT uMsg, POINT ptOffset, WPARAM wParam, CGuiMLTextBox* pTextBox )
{
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		if(group == -1) return false;
		stPointI ptCursor = Engine_GetCursor()->GetPosition();
		if(pTextBox->m_pDialog->IsFocus())
		{
			stRectI rc = rect;
			rc.OffsetRect(ptOffset);
			if(rc.PtInRect(ptCursor))
			{
				pTextBox->UncheckImageNode(group);
				check = true;
				//pTextBox->m_pDialog->SendEvent(EVENT_SHELLOBJECT_CLICKED,true,pTextBox,objid);
				return OnClick(pTextBox);
			}
		}
		break;
	}
	return false;
}

bool CGuiMLTextBox::NodeImage::OnClick(CGuiMLTextBox* pTextBox)
{
	return false;
}

bool CGuiMLTextBox::NodeObject::OnClick(CGuiMLTextBox* pTextBox)
{
	pTextBox->m_pDialog->SendEvent(EVENT_SCRIPT_BUTTON_CLICK,true,pTextBox,(DWORD)(LPVOID)scriptFunction.c_str());
	return true;
}

bool CGuiMLTextBox::NodeObject::Draw(POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend)
{
	if (!IsShow(ptOffset,prect))
		return false;
	stRectI rc = rect;
	rc.OffsetRect(ptOffset);
	//DWORD color;
	//if(!check)
	//	color = COLOR_ARGB(255,50,50,50);
	//else
	//	color = COLOR_ARGB(255,128,128,128);
	//GetDevice()->FillRect(&rc,color);
	if(check)
	{
		GetDevice()->FillRect(&rc,COLOR_ARGB(255,110, 207, 241));
	}

	DrawBackGround(rc);
	NodeImage::Draw(ptOffset,prect,pTextBox,dwBlend);

	stRectI rc2 = rc;

	int w = 0;
	int h = 0;
	if(pImage)
	{
		w = pImage->GetBitmap(0)->GetWidth();
		h = pImage->GetBitmap(0)->GetHeight();
	}
	rc.left += w;
	
	GetDevice()->DrawString(strText.c_str(),rc,textColor,DT_VCENTER | DT_LEFT);

	stPointI ptCursor = Engine_GetCursor()->GetPosition();

	if( pTextBox->m_pDialog->IsFocus() && rc2.PtInRect(ptCursor) )
	{
		stRectI rcBound = pTextBox->m_rcBoundingBox; 
		stPointI pt(rc.right, rc.top - toolTips.GetHeight());
        
		if( pt.x + toolTips.GetWidth() > GetDevice()->GetWidth() )
			pt.x = rc2.left - toolTips.GetWidth();
		if(pt.y < 0)
			pt.y = rc2.top;
		if( pt.y + toolTips.GetHeight() > GetDevice()->GetHeight() )
			pt.y = rc2.top - toolTips.GetHeight();
	
		GetGuiManager()->SetToolTips(&toolTips,pt);
	}

	return true;
}

bool CGuiMLTextBox::NodeObject::HandleMouse( UINT uMsg, POINT ptOffset, WPARAM wParam, CGuiMLTextBox* pTextBox )
{
	return NodeImage::HandleMouse(uMsg,ptOffset,wParam,pTextBox);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param ptOffset : ÃèÊö
* \param prect : ÃèÊö
* \param color : ÃèÊö
* \param pTextBox : ÃèÊö
* \param dwBlend : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CGuiMLTextBox::NodeSubLink::Draw(POINT ptOffset,const RECT * prect,DWORD color,CGuiMLTextBox* pTextBox,DWORD dwBlend)
{
	if (!IsShow(ptOffset,prect))
		return false;

	stRectI rc = rect;
	//DWORD format = DT_UNDERLINE | DT_SINGLELINE;
	stFadeoutElement st;
	st.dwShowColor = color;
	/*st.dwShadowColor = COLOR_ARGB(255,255-COLOR_GET_R(color),255-COLOR_GET_G(color),
		255-COLOR_GET_B(color));*/

	DoFadeout(st,pTextBox->GetStyle(),dwBlend);
	rc.OffsetRect(ptOffset);

	GetDevice()->DrawString(text.c_str(),rc,st.dwShowColor,m_dwFormat,st.eFontEffect,st.dwShadowColor);
	return false;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param ptOffset : ÃèÊö
* \param prect : ÃèÊö
* \param pTextBox : ÃèÊö
* \param dwBlend : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CGuiMLTextBox::NodeHyperlinkBase::Draw( POINT ptOffset ,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend)
{
	DWORD color = colorNormal;
	bool  bShow = false;
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	if(pTextBox->m_pDialog->IsFocus() && PtInRect(prect,ptCursor))
	{
		for(size_t i=0;i<subNodes.size();++i)
		{
			stRectI rc = subNodes[i]->rect;
			rc.OffsetRect(ptOffset);
			if(rc.PtInRect(ptCursor))
			{
				color = colorOver;
				bShow = true;
				if(Engine_GetCursor()->IsLButtonDown())
					color = colorDown;

				//»»Êó±êicoHand
				//stResourceLocation rl("data\\cursor.gl",0,14);
				//Engine_GetCursor()->SetImage( &rl, stPointI(12,1) );
				break;
			}
			else
			{
				//»»Êó±êicoHand
				//stResourceLocation rl("data\\cursor.gl",0,0);
				//Engine_GetCursor()->SetImage( &rl, stPointI(1,1) );
			}
		}
	}

	for(size_t i=0;i<subNodes.size();++i)
		subNodes[i]->Draw(ptOffset,prect,color,pTextBox,dwBlend);

	return bShow;
}

bool CGuiMLTextBox::NodeHyperlinkBase::DrawSelBkColor( POINT ptOffset,const RECT * prect,CGuiMLTextBox* pTextBox )
{
	for(size_t i=0;i<subNodes.size();++i)
		subNodes[i]->DrawSelBkColor( ptOffset, prect, pTextBox );

	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param ptOffset : ÃèÊö
* \param prect : ÃèÊö
* \param pTextBox : ÃèÊö
* \param dwBlend : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CGuiMLTextBox::NodeButton::Draw( POINT ptOffset ,const RECT * prect,CGuiMLTextBox* pTextBox,DWORD dwBlend)
{
	if (!IsShow(ptOffset,prect))
		return false;

	if(bDown)
	{
		rect.OffsetRect(1,1);
	}
	stRectI rc = rect;

	rc.OffsetRect(ptOffset);

	int w = 0;
	int h = 0;
	if(pImage)
	{
		w = pImage->GetBitmap(0)->GetWidth();
		h = pImage->GetBitmap(0)->GetHeight();
	}

	bool bFill = false;

	if(pTextBox->m_pDialog->IsFocus())
	{
		if(bDown) bFill = true;
		else
		{
			if(!Engine_GetCursor()->IsLButtonDown())
			{
				stPointI ptCursor = Engine_GetCursor()->GetPosition();
				stRectI rc = rect;
				rc.OffsetRect(ptOffset);
				if(rc.PtInRect(ptCursor))
					bFill = true;
			}
		}
	}

	if(bFill)
		GetDevice()->FillRect(&rc,COLOR_ARGB(90,255,245,0),COLOR_ARGB(150,200,200,0),COLOR_ARGB(90,255,180,80),COLOR_ARGB(150,200,245,0));

	NodeImage::Draw(ptOffset,prect,pTextBox,dwBlend);
	rc.left += w;

	GetDevice()->DrawString(strText.c_str(),rc,textColor,DT_VCENTER | DT_LEFT);

	if(bDown)
	{
		rect.OffsetRect(-1,-1);
	}
	return true;
}

bool CGuiMLTextBox::NodeButton::HandleMouse( UINT uMsg, POINT ptOffset, WPARAM wParam, CGuiMLTextBox* pTextBox )
{
	CGuiDialog * pDlg = pTextBox->m_pDialog;
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		{
			if(pDlg->IsFocus())
			{
				stPointI ptCursor = Engine_GetCursor()->GetPosition();
				stRectI rc = rect;
				rc.OffsetRect(ptOffset);
				if(rc.PtInRect(ptCursor))
				{
					bDown = true;
					return true;
				}
			}
		}
		break;
	case WM_LBUTTONUP:
		if(bDown)
		{
			if(pDlg->IsFocus())
			{
				stPointI ptCursor = Engine_GetCursor()->GetPosition();
				stRectI rc = rect;
				rc.OffsetRect(ptOffset);
				if(rc.PtInRect(ptCursor))
				{
					bDown = false;
					if(sound != -1)
						Engine_GetCallBack()->PlayUISound(sound);
					pDlg->SendEvent(EVENT_ITEM_CLICKED,true,pTextBox,id);
					return true;
				}
			}
			bDown = false;
		}
		break;
	}
	return false;
}


bool CGuiMLTextBox::NodeButton::OnClick(CGuiMLTextBox* pTextBox)
{
	return false;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param ptOffset : ÃèÊö
* \param prect : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CGuiMLTextBox::NodeHyperlinkBase::IsShow(POINT ptOffset,const RECT * prect)
{
	for(size_t i=0;i<subNodes.size();++i)
	{
		if (subNodes[i]->IsShow(ptOffset,prect))
			return true;
	}
	return false;
}

bool CGuiMLTextBox::NodeHyperlinkBase::IsInRect(POINT ptOffset)
{
	for(size_t i=0;i<subNodes.size();++i)
	{
		if (subNodes[i]->IsInRect(ptOffset))
			return true;
	}
	return false;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param uMsg : ÃèÊö
* \param ptOffset : ÃèÊö
* \param wParam : ÃèÊö
* \param pTextBox : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CGuiMLTextBox::NodeHyperlinkBase::HandleMouse( UINT uMsg, POINT ptOffset, WPARAM wParam, CGuiMLTextBox* pTextBox )
{
	switch(uMsg)
	{
	case WM_RBUTTONDOWN:
		{
			stPointI ptCursor = Engine_GetCursor()->GetPosition();
			//if(pTextBox->m_pDialog->IsFocus())
			{
				for(size_t i=0;i<subNodes.size();++i)
				{
					stRectI rc = subNodes[i]->rect;
					rc.OffsetRect(ptOffset);
					if(rc.PtInRect(ptCursor))
					{
						DWORD dwHiTest = pTextBox->m_pDialog->OnHiTest(ptCursor);
						if(dwHiTest != HTERROR && dwHiTest != HTTRANSPARENT)
						{
							if(GetGuiManager()->CanHaveFocus(pTextBox->m_pDialog))
								GetGuiManager()->RequestFocus(pTextBox->m_pDialog);
							return OnRightClick(pTextBox);
						}
						//pTextBox->m_pDialog->SendEvent(EVENT_SHELLOBJECT_CLICKED,true,pTextBox,objid);
						//return true;
					}
				}
			}
		}
		break;
	case WM_LBUTTONDOWN:
		stPointI ptCursor = Engine_GetCursor()->GetPosition();
		if(pTextBox->m_pDialog->IsFocus())
		{
			for(size_t i=0;i<subNodes.size();++i)
			{
				stRectI rc = subNodes[i]->rect;
				rc.OffsetRect(ptOffset);
				if(rc.PtInRect(ptCursor))
				{
					return OnClick(pTextBox);
					//pTextBox->m_pDialog->SendEvent(EVENT_SHELLOBJECT_CLICKED,true,pTextBox,objid);
					//return true;
				}
			}
		}
		break;
	}
	return false;
}

bool CGuiMLTextBox::NodeHyperlink::OnClick( CGuiMLTextBox* pTextBox )
{
	if(strstr(addr.c_str(),"http") == addr.c_str())
	{
		GotoURL(addr.c_str(),SW_SHOW);
		return true;
	}
	else if(!addr.empty())
	{
		pTextBox->m_strNewDoc = addr;
		return true;
	}
	return false;
}

bool CGuiMLTextBox::NodeShell::OnClick( CGuiMLTextBox* pTextBox )
{
	pTextBox->m_pDialog->SendEvent(EVENT_SHELLOBJECT_CLICKED,true,pTextBox,objid);
	return true;
}

bool CGuiMLTextBox::NodeChatUser::OnClick( CGuiMLTextBox* pTextBox )
{
        return  pTextBox->m_pDialog->SendEvent(EVENT_ITEM_CLICKED,true,pTextBox,(DWORD)strName);
	//return true;
}
bool CGuiMLTextBox::NodeChatUser::OnRightClick( CGuiMLTextBox* pTextBox )
{
	pTextBox->m_pDialog->SendEvent(EVENT_ITEM_RIGHT_CLICKED,true,pTextBox,(DWORD)strName);
	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param text : ÃèÊö
* \param color : ÃèÊö
* \param ptOffset : ÃèÊö
* \param aNode : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
template < class _TNode>
void CGuiMLTextBox::AppendText( const char* text,DWORD color,stPointI & ptOffset,std::vector<_TNode * > & aNode,int curLineHeight )
{
	int rowWidth = m_rcText.Width();
	stPointI pt = ptOffset;
	stPointI pt0 = ptOffset;
	const char* p = text;
	const char* p0 = text;
	int iLineHeight = curLineHeight;
	while(*p)
	{
		int ch=0;
		int add;
		bool bNewLine = false;
		bool bTab = false;
		if( (*p) & 0x80 )
		{
			if( p[1] == 0 )
				break;
			ch = *(WORD*)p;
			add = 2;
		}
		else
		{
			size_t charnum;
			if( IsNewLine(p,charnum) )
			{
				add = charnum;
				bNewLine = true;
			}
			else
			{
				if (*p == '\t')
				{
					bTab = true;
				}
				ch = *p;
				add = 1;
			}
		}

		int cx;
		if(bTab)	cx = 4 * GetDevice()->GetFontWidth(GetFont());
		else		cx = GetDevice()->GetAFontWidth(ch,GetFont());

		if( pt.x + cx > rowWidth || bNewLine)
		{
			if( p != p0 || bNewLine)
			{
				_TNode * node = new _TNode;

				node->rect.SetRect(pt0.x,pt.y,pt.x,pt.y + m_nLineHeight);

				node->color = color;

				size_t slen = p - p0;

				node->text.append(p0,slen);

				if( GetStyle() & GUI_TS_MULTIMEDIA )
				{
					node->data.resize(slen);
					InitStringData(node->text.c_str(),&node->data[0],-1,color);
				}
				aNode.push_back(node);
			}

			pt.y += iLineHeight;
			ptOffset.y += iLineHeight;
			if(iLineHeight == curLineHeight) iLineHeight = m_nLineHeight;

			ptOffset.x = 0;
			pt.x = 0;
			pt0 = pt;

			if(bNewLine) p += add;
			p0 = p;
			continue;
		}

		pt.x += cx;
		p += add;
	}

	if(p != p0)
	{
		_TNode * node = new _TNode;

		node->rect.SetRect(pt0.x,pt.y,pt.x,pt.y + m_nLineHeight);
		ptOffset.x = node->rect.right;

		node->color = color;

		size_t slen = p - p0;

		node->text.append(p0,slen);

		if( GetStyle() & GUI_TS_MULTIMEDIA )
		{
			node->data.resize(slen);
			InitStringData(node->text.c_str(),&node->data[0],-1,color);
		}

		aNode.push_back(node);
	}
}

void CGuiMLTextBox::UpdateNodeLocation(std::vector<NodeUnknown*> & aNodes,int iLineHeight)
{
	for(size_t i=0;i<aNodes.size();++i)
	{
		int h = aNodes[i]->rect.Height();
		aNodes[i]->rect.top = aNodes[i]->rect.top + (iLineHeight - h)/2;
		aNodes[i]->rect.bottom = aNodes[i]->rect.top + h;
	}
}

bool CGuiMLTextBox::AppendImageNode(CGuiMLTextBox::NodeImage * node,const char* pszText,const stResourceLocation * rl,stPointI & ptOffset,int iLineHeight,bool bSingleLine)
{
	bool result = false;
	IBitmaps* pBmps = GetDevice()->FindBitmaps(rl);
	int w = 0,h = 0;
	node->pImage = NULL;
	if(pBmps && pBmps->GetFrameCount())
	{
		node->pImage = pBmps;
		w = pBmps->GetBitmap(0)->GetWidth();
		h = pBmps->GetBitmap(0)->GetHeight() + 6;
		result = true;
	}

	int fontWidth = GetDevice()->GetFontWidth(GetFont()) * strlen(pszText);
	w += fontWidth;

	if(!bSingleLine && ptOffset.x + w > m_rcText.Width())
	{
		ptOffset.y += iLineHeight;
		ptOffset.x = 0;
	}
	int lineHeight = max(h,m_nLineHeight);
	node->rect.SetRect(ptOffset.x,ptOffset.y,ptOffset.x + w,ptOffset.y + lineHeight);
	ptOffset.x += w;

	return result;
}

void CGuiMLTextBox::UncheckImageNode(int group)
{
	for(size_t i=0;i<m_aImage.size();++i)
	{
		if(m_aImage[i]->group == group)
		{
			m_aImage[i]->check = false;
		}
	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param page : ÃèÊö
* \param pData : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CGuiMLTextBox::ParseAParagraph(TiXmlElement* page,void* pData)
{
	stPointI ptOffset = GetTailOffset();
	ptOffset.x = 0;
	//ptOffset.y += m_nLineHeight;
	TiXmlElement* sub = page->FirstChildElement();
	stParagraph& pp = *new stParagraph;
	std::vector<NodeText*> aNodeText;
	std::vector<NodeButton*> aNodeButton;

	std::vector<NodeUnknown*> aLineNode;

	int maxLineHeight = 0;

	int curLineHeight = m_nLineHeight;
	pp.data = pData;
	bool bChangeLineHeight = false;
	while(sub)
	{
		const char* text = XML_GetNodeText(sub);
		if( strcmp("shell",sub->Value()) == 0 )
		{
			int objid;
			if( text && sub->QueryIntAttribute( "obj",&objid ) == TIXML_SUCCESS )
			{
				NodeShell * node = new NodeShell;
				node->objid = objid;
				stPointI ptOldOffset = ptOffset;
				AppendText(text,COLOR_ARGB(255,204,165,10),ptOffset,node->subNodes,curLineHeight);
				if( node->subNodes.empty() )
				{
					delete node;
				}
				else
				{
					pp.nodes.push_back(node);
					if(ptOldOffset.y != ptOffset.y) curLineHeight = 0;
					curLineHeight = max(curLineHeight,m_nLineHeight);

					for(size_t i=0;i<node->subNodes.size();++i)
					{
						if(aLineNode.size() && aLineNode.back()->rect.top != node->subNodes[i]->rect.top)
						{
							UpdateNodeLocation(aLineNode,maxLineHeight);
							maxLineHeight = 0;
							aLineNode.resize(0);
						}
						aLineNode.push_back(node->subNodes[i]);
						maxLineHeight = max(maxLineHeight,node->subNodes[i]->rect.Height());
					}
				}
			}
		}
		else if( strcmp("name",sub->Value()) == 0 )
		{
			if( text )
			{
				DWORD color;
				color = XML_GetColorAttribute(sub,"color",-1);

				NodeChatUser * node = new NodeChatUser;
				strncpy(node->strName,text,sizeof(node->strName));
				stPointI ptOldOffset = ptOffset;
				AppendText(text,color,ptOffset,node->subNodes,curLineHeight);
				if( node->subNodes.empty() )
				{
					delete node;
				}
				else
				{
					node->colorNormal = color;
					node->SetFormat(0);

					pp.nodes.push_back(node);
					if(ptOldOffset.y != ptOffset.y) curLineHeight = 0;
					curLineHeight = max(curLineHeight,m_nLineHeight);

					for(size_t i=0;i<node->subNodes.size();++i)
					{
						if(aLineNode.size() && aLineNode.back()->rect.top != node->subNodes[i]->rect.top)
						{
							UpdateNodeLocation(aLineNode,maxLineHeight);
							maxLineHeight = 0;
							aLineNode.resize(0);
						}
						aLineNode.push_back(node->subNodes[i]);
						maxLineHeight = max(maxLineHeight,node->subNodes[i]->rect.Height());
					}
				}
			}
		}
		else if( strcmp("a",sub->Value()) == 0 )
		{
			const char* addr;
			const char* text;
			DWORD colorNormal;
			DWORD colorOver;
			DWORD colorDown;
			text = XML_GetNodeText(sub);
			addr = sub->Attribute("href");
			if(text)
			{
				if(addr == NULL) addr = "";

				NodeHyperlink * node = new NodeHyperlink;

				colorNormal = XML_GetColorAttribute(sub,"color",node->colorNormal);
				colorOver = XML_GetColorAttribute(sub,"colorover",node->colorOver);
				colorDown = XML_GetColorAttribute(sub,"colordown",node->colorDown);

				stPointI ptOldOffset = ptOffset;
				AppendText(text,COLOR_ARGB(255,204,165,10),ptOffset,node->subNodes,curLineHeight);
				if( node->subNodes.empty() )
				{
					delete node;
				}
				else
				{
					pp.nodes.push_back(node);

					node->addr = addr;
					node->colorNormal = colorNormal;
					node->colorOver = colorOver;
					node->colorDown = colorDown;

					if(ptOldOffset.y != ptOffset.y) curLineHeight = 0;
					curLineHeight = max(curLineHeight,m_nLineHeight);

					for(size_t i=0;i<node->subNodes.size();++i)
					{
						if(aLineNode.size() && aLineNode.back()->rect.top != node->subNodes[i]->rect.top)
						{
							UpdateNodeLocation(aLineNode,maxLineHeight);
							maxLineHeight = 0;
							aLineNode.resize(0);
						}
						aLineNode.push_back(node->subNodes[i]);
						maxLineHeight = max(maxLineHeight,node->subNodes[i]->rect.Height());
					}
				}
			}
		}
		else if( strcmp("button",sub->Value()) == 0 )
		{
			int objid;
			int soundindex;
			const char * text;
			const char * icon;
			icon = sub->Attribute("icon");
			text = XML_GetNodeText(sub);
			
			if( text && sub->QueryIntAttribute( "id",&objid ) == TIXML_SUCCESS )
			{
				stResourceLocation rl(icon);
				NodeButton * node = new NodeButton;
				stPointI pt = ptOffset;
				stPointI ptOldOffset = ptOffset;
				AppendImageNode(node,text,&rl,pt,curLineHeight,true);
				node->id = objid;
				node->bDown = false;
				node->strText = text;
				node->rect.right = m_rcText.Width();
				ptOffset.y += node->rect.Height();
                ptOffset.x = 0;
				node->textColor = XML_GetColorAttribute( sub,"textcolor",COLOR_ARGB(255,231,190,0) );
				if(sub->QueryIntAttribute( "sound",&soundindex ) != TIXML_SUCCESS)
					soundindex = -1;
				node->sound = soundindex;

				pp.nodes.push_back(node);

				int lineHeight = node->rect.Height();
				if( aLineNode.size() && aLineNode.back()->rect.top != node->rect.top )
				{
					UpdateNodeLocation(aLineNode,maxLineHeight);
					maxLineHeight = 0;
					aLineNode.resize(0);
				}
				aLineNode.push_back(node);
				maxLineHeight = max(maxLineHeight,lineHeight);

				if(ptOldOffset.y != ptOffset.y) curLineHeight = 0;
				curLineHeight = max(curLineHeight,lineHeight);
			}
		}
		else if( strcmp("image",sub->Value() ) == 0 )
		{
			const char* pack;
			int group;
			pack = sub->Attribute("path");
			if(sub->QueryIntAttribute("group",&group) != TIXML_SUCCESS)
				group = -1;
			stResourceLocation rl(pack);
			NodeImage * node = new NodeImage;
			
			stPointI ptOldOffset = ptOffset;
			if(AppendImageNode(node,"",&rl,ptOffset,curLineHeight))
			{
				pp.nodes.push_back(node);
				if(group != -1)
					m_aImage.push_back(node);
				node->group = group;
				node->check = false;

				int lineHeight = node->rect.Height();
				if( aLineNode.size() && aLineNode.back()->rect.top != node->rect.top )
				{
					UpdateNodeLocation(aLineNode,maxLineHeight);
					maxLineHeight = 0;
					aLineNode.resize(0);
				}
				aLineNode.push_back(node);
				maxLineHeight = max(maxLineHeight,lineHeight);

				if(ptOldOffset.y != ptOffset.y) curLineHeight = 0;
				curLineHeight = max(curLineHeight,lineHeight);
			}
			else
				delete node;
		}
		else if( strcmp("object",sub->Value()) == 0 )
		{
			int id;
			int group;
			const char* text;
			const char* scriptFun;
			stResourceLocation rl;
			DWORD textColor = 0xffffffff;
			if( sub->QueryIntAttribute("group",&group) != TIXML_SUCCESS )
				group = -1;
			if(sub->QueryIntAttribute("id",&id) == TIXML_SUCCESS && Engine_GetCallBack()->GetObjectImageLocation(id,&rl))
			{
				text = XML_GetNodeText(sub);
				if(text == NULL) text = "";

				textColor = XML_GetColorAttribute( sub,"textcolor",-1 );
				NodeObject * node = new NodeObject;
				stPointI ptOldOffset = ptOffset;
				if(AppendImageNode(node,text,&rl,ptOffset,curLineHeight))
				{
					// get the tips
					Engine_GetCallBack()->SetObjectTooltips(id,node->toolTips);

					pp.nodes.push_back(node);
					if(group != -1)
						m_aImage.push_back(node);
					node->strText = text;
					scriptFun = sub->Attribute("OnClick");
					if(scriptFun) node->scriptFunction = scriptFun;
					node->group = group;
					node->check = false;
					node->textColor = textColor;
					node->rect.bottom += 6;
					int lineHeight = node->rect.Height();
					if(aLineNode.size() && aLineNode.back()->rect.top != node->rect.top)
					{
						UpdateNodeLocation(aLineNode,maxLineHeight);
						maxLineHeight = 0;
						aLineNode.resize(0);
					}
					aLineNode.push_back(node);
					maxLineHeight = max(maxLineHeight,lineHeight);

					if(ptOldOffset.y != ptOffset.y) curLineHeight = 0;
					curLineHeight = max(curLineHeight,lineHeight);
				}
				else
					delete node;
			}
		}
		else if(strcmp("n",sub->Value()) == 0)
		{
			DWORD color;
			color = XML_GetColorAttribute(sub,"color",-1);
			int taskID;
			const char* taskVar;
			if(sub->QueryIntAttribute("task",&taskID) != TIXML_SUCCESS)
				taskID = -1;
			taskVar = sub->Attribute("var");
			std::string strTaskVar;
			if(taskID != -1 && taskVar)
			{
				strTaskVar = Engine_GetCallBack()->GetTaskVariable(taskID,taskVar);				
				text = strTaskVar.c_str();
			}
			if(text)
			{
				aNodeText.resize(0);
				stPointI ptOldOffset = ptOffset;
				AppendText(text,color,ptOffset,aNodeText,curLineHeight);
				if(aNodeText.empty())
				{
				}
				else
				{
					pp.nodes.insert(pp.nodes.end(),aNodeText.begin(),aNodeText.end());
					if(ptOldOffset.y != ptOffset.y) curLineHeight = 0;
					curLineHeight = max(curLineHeight,m_nLineHeight);

					for(size_t i=0;i<aNodeText.size();++i)
					{
						NodeText *  p = aNodeText[i];
						if(aLineNode.size() && aLineNode.back()->rect.top != aNodeText[i]->rect.top)
						{
							UpdateNodeLocation(aLineNode,maxLineHeight);
							maxLineHeight = 0;
							aLineNode.resize(0);
						}
						aLineNode.push_back(aNodeText[i]);
						maxLineHeight = max(maxLineHeight,aNodeText[i]->rect.Height());
					}
				}
			}
		}
		sub = sub->NextSiblingElement();
	}

	if(pp.nodes.empty())
	{
		delete &pp;
		return false;
	}

	if(!aLineNode.empty())
	{
		UpdateNodeLocation(aLineNode,maxLineHeight);
	}

	pp.time=0.0f;
	
	if( pData && (*(int *)pData) == g_GMFilter )  
		pp.bIsGmMsg = true;
	else
		pp.bIsGmMsg = false;
	m_aParagraph.push_back(&pp);

		
	return true;
}

TiXmlElement& CGuiMLTextBox::GetXmlBody()
{
	TiXmlElement* body = m_xmlDoc.FirstChildElement("body");
	if(!body) body = m_xmlDoc.InsertEndChild(TiXmlElement("body"))->ToElement();
	return *body;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param page : ÃèÊö
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::AddHeadParagraph( TiXmlElement* page,void* pData )
{
	TiXmlElement* pBody = &GetXmlBody();
	if(!pBody) return;

	TiXmlElement* pFrist = pBody->FirstChildElement("p");
	if( pFrist )
	{
		pBody->InsertBeforeChild( pFrist,*page );
	}
	else
	{
		pBody->InsertEndChild( *page );
	}
	ClearParagraph();
	ParseXml();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param page : ÃèÊö
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::_AppendParagraph(TiXmlElement* page,void* pData)
{
	if(ParseAParagraph(page,pData))
		GetXmlBody().InsertEndChild(*page);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param page : ÃèÊö
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::AppendParagraph(TiXmlElement* page,void* pData)
{
	if(ParseAParagraph(page,pData))
		GetXmlBody().InsertEndChild(*page);
	m_VScrollBar.SetPageSize(m_rcText.Height());
	m_VScrollBar.SetTrackRange(0,GetTextHeight());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param text : ÃèÊö
 * \param color : ÃèÊö
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::AppendParagraph(const char* text,DWORD color,void* pData)
{
	if(text[0] == 0) return;
	if(strstr(text,"<p>") == text)
	{
		TiXmlElement page("p");
		page.Parse(text,NULL,TIXML_ENCODING_UNKNOWN);
		_AppendParagraph(&page,pData);
		m_VScrollBar.SetPageSize(m_rcText.Height());
		m_VScrollBar.SetTrackRange(0,GetTextHeight());
	}
	else
	{
		TiXmlElement page("p");
		TiXmlElement* node = page.InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetColorAttribute(node,"color",color);
		XML_SetNodeText(node,text);
		_AppendParagraph(&page,pData);
		m_VScrollBar.SetPageSize(m_rcText.Height());
		m_VScrollBar.SetTrackRange(0,GetTextHeight());
	}
}


void CGuiMLTextBox::AppendParagraphButton(const char* text,DWORD color,DWORD objid,const char* icon,int soundindex ,void* pData)
{
	if(text[0] == 0) return;

	if(strstr(text,"<p>") == text)
	{
		TiXmlElement page("p");
		page.Parse(text,NULL,TIXML_ENCODING_UNKNOWN);
		_AppendParagraph(&page,pData);
	}
	else
	{
		TiXmlElement page("p");
		TiXmlElement* node = page.InsertEndChild(TiXmlElement("button"))->ToElement();
		node->SetAttribute( "id", objid );
		node->SetAttribute("icon",icon);
		node->SetAttribute("sound",soundindex);
		char szTemp[32];
		_snprintf(szTemp,32,"%d,%d,%d,%d",COLOR_GET_ALPHA(color),COLOR_GET_R(color),COLOR_GET_G(color),COLOR_GET_B(color));
		szTemp[31] = 0;
		node->SetAttribute("textcolor",szTemp);
		TiXmlText* itemName = node->InsertEndChild( TiXmlText( text) )->ToText();
		_AppendParagraph(&page,pData);
	}
	m_VScrollBar.SetPageSize(m_rcText.Height());
	m_VScrollBar.SetTrackRange(0,GetTextHeight());
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
POINT CGuiMLTextBox::GetTailOffset()
{
	if(!m_aParagraph.empty())
	{
		return m_aParagraph.back()->GetTailOffset();
	}
	return stPointI(0,0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiMLTextBox::OnInit()
{
	m_pDialog->InitControl( &m_VScrollBar ); 
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::UpdateRects(void)
{
	if(m_nLineHeight < GetDevice()->GetFontHeight(GetFont()))
		m_nLineHeight = GetDevice()->GetFontHeight(GetFont());

	CGuiControl::UpdateRects();

	stRectI rect = m_rcBoundingBox;

	if(m_VScrollBar.IsVisible())
		rect.right -= m_nSBWidth + s_nSBSpace;
	stRectI rcText;

	// Update the render rectangles
	if(GetStyle() & GUI_TS_NOSKIN)
	{
		if(0 == (GetStyle() & GUI_TS_SCROLLBAR_HAS_SKIN))
			m_VScrollBar.SetStyle(CGuiScrollBar::NOSKIN);
		CuttRect9(rect,stPointI(0,0),stPointI(0,0),m_rcRender);
		rcText = rect;
		stGuiCtrlBorderInfo* pBorderInfo = m_pDialog->GetCtrlBorderInfo( m_Type );
		if(0 == (GetStyle() & GUI_TS_SCROLLBAR_HAS_SKIN))
		if( pBorderInfo )
		{
			rcText.top += pBorderInfo->ptBorderLeftTop.x;
			rcText.bottom -= pBorderInfo->ptBorderRightBottom.y;
		}
	}
	else
	{
		stGuiCtrlBorderInfo* pBorderInfo = m_pDialog->GetCtrlBorderInfo( m_Type );
		rcText = rect;
		if( pBorderInfo )
		{
			rcText.left += pBorderInfo->ptBorderLeftTop.x;
			rcText.top += pBorderInfo->ptBorderLeftTop.y;
			//rcText.right -= pBorderInfo->ptBorderRightBottom.x;
			rcText.bottom -= pBorderInfo->ptBorderRightBottom.y;
			CuttRect9(rect,pBorderInfo->ptLeftTop,pBorderInfo->ptRightBottom,m_rcRender);
		}
	}

	if(GetStyle() & GUI_TS_NOSKIN)
	{
		if(GetStyle() & GUI_TS_LEFTSCROLL)
		{
			rcText.left = rcText.left + m_nSBWidth + 12;
			m_VScrollBar.SetLocation( m_rcBoundingBox.left+5, m_rcBoundingBox.top );
			m_VScrollBar.SetSize( m_nSBWidth, m_rcBoundingBox.Height()-10 );
		}
		else
		{
			m_VScrollBar.SetLocation( m_rcBoundingBox.right - m_nSBWidth - s_nSBSpace, m_rcBoundingBox.top );
			m_VScrollBar.SetSize( m_nSBWidth, m_rcBoundingBox.Height());
		}
	}
	else
	{
		if(GetStyle() & GUI_TS_LEFTSCROLL)
		{
			rcText.left = rcText.left + m_nSBWidth + 12;
			m_VScrollBar.SetLocation( m_rcBoundingBox.left+5, m_rcBoundingBox.top );
			m_VScrollBar.SetSize( m_nSBWidth, m_rcBoundingBox.Height()-10 );
		}
		else
		{
			m_VScrollBar.SetLocation( m_rcBoundingBox.right - m_nSBWidth - s_nSBSpace, m_rcBoundingBox.top + s_nSBTopOffset );
			m_VScrollBar.SetSize( m_nSBWidth, m_rcBoundingBox.Height() - (s_nSBTopOffset + s_nSBBottomOffset));
		}
	}

	InflateRect( &rcText, -m_nSpacing, -m_nSpacing );

	if(m_rcText == rcText)
		return;

	bool bWidthChange = (rcText.Width() != m_rcText.Width());

	m_rcText = rcText;

	if(!m_rcText.IsRectEmpty() && bWidthChange)
	{
		RefreshText();
	}

	m_VScrollBar.SetPageSize(m_rcText.Height());
	m_VScrollBar.SetLineSize(m_nLineHeight);
	m_VScrollBar.SetTrackRange(0,GetTextHeight());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::ParseXml()
{
	TiXmlElement* body = m_xmlDoc.FirstChildElement("body");
	if(!body) return;

	TiXmlElement* page = body->FirstChildElement("p");
	while(page)
	{
		ParseAParagraph(page,NULL);
		page = page->NextSiblingElement("p");
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param text : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::SetText(const char* text)
{
	m_xmlDoc.Clear();
	ClearParagraph();
	if(text[0] == 0) return;
	if(strstr(text,"<?xml ") != text)
	{
		TiXmlElement* p = GetXmlBody().InsertEndChild(TiXmlElement("p"))->ToElement();
		TiXmlElement* node = p->InsertEndChild(TiXmlElement("n"))->ToElement();
		XML_SetNodeText(node,text);
	}
	else
	{
		m_xmlDoc.Parse(text);
	}
	if(m_xmlDoc.Error())
	{
		m_xmlDoc.Clear();
	}
	else
	{
		ParseXml();
	}
	m_VScrollBar.SetPageSize(m_rcText.Height());
	m_VScrollBar.SetTrackRange(0,GetTextHeight());
}

void CGuiMLTextBox::SetXMLDoc(TiXmlDocument & doc)
{
	m_xmlDoc = doc;
	ClearParagraph();
	ParseXml();
	m_VScrollBar.SetPageSize(m_rcText.Height());
	m_VScrollBar.SetTrackRange(0,GetTextHeight());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::RefreshText()
{
	ClearParagraph();
	ParseXml();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::ClearParagraph()
{
	for(size_t i=0; i<m_aParagraph.size(); ++i)
	{
		delete m_aParagraph[i];
	}
	m_aParagraph.resize(0);
	m_aImage.resize(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::Clear()
{
	ClearParagraph();
	m_xmlDoc.Clear();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::Render( float fElapsedTime )
{
	if(!(m_dwStyle & GUI_TS_NOSKIN))
	{
		stRectI rect = m_rcBoundingBox;
		//rect.right -= m_nSBWidth + s_nSBSpace;
		m_pDialog->DrawControlBorder(rect,m_Type);	
	}

	int iFont = GetDevice()->SetFont(GetFont());
	stRectI rcText = m_rcText;
	rcText.OffsetRect(m_pDialog->GetClientX(),m_pDialog->GetClientY());
	
	stPointI ptOffset(rcText.left,rcText.top );

	ptOffset.y -= m_VScrollBar.GetTrackPos();

	stRectI clipOld = GetDevice()->SetClipRect(&rcText);
	for(size_t i=0;i<m_aParagraph.size();++i)
	{
		if( m_bFilter)
		{
			if(m_aParagraph[i]->bIsGmMsg)
				m_aParagraph[i]->Draw(ptOffset,&rcText,this);
			else
				continue;
		}
		else
			m_aParagraph[i]->Draw(ptOffset,&rcText,this);
	}
	GetDevice()->SetClipRect(&clipOld);
	m_VScrollBar.Render(fElapsedTime);
	GetDevice()->SetFont(iFont);
}

bool CGuiMLTextBox::LoadDoc(const char* fileName)
{
	Stream* str = OpenPackFileForRead2(fileName);
	if(str)
	{
		char * text = new char[str->getSize() + 1];
		str->read(text,str->getSize());
		text[str->getSize()] = 0;
		SetText(text);
		delete [] text;
		ClosePackFileForRead(str);

		m_strCurDir = GetFileDir(fileName);
		return true;
	}
	return false;
}

const char* CGuiMLTextBox::NodeHyperlinkBase::GetTextContent()
{
	static std::string str;
	str.clear();
	for( size_t i=0;i<subNodes.size();++i )
	{
		if ( subNodes[i]->bSelected )
		{
			if ( subNodes[i]->GetTextContent() )
			{
				std::string strSub = subNodes[i]->GetTextContent();
				str += strSub;
			}
		}
	}

	return str.c_str();
}

void CGuiMLTextBox::CopyToClipboard()
{
	//Copy sel nodes to clipboard
	if( OpenClipboard( NULL ) )
	{
		EmptyClipboard();

		int nLength = 10000;

		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, sizeof(char) * nLength );
		if( hBlock )
		{
			char *pszText  = (char*)GlobalLock( hBlock );
			if( pszText )
			{
				pszText[0] = 0L;

				for( size_t i=0;i<m_aParagraph.size();++i )
				{
					for( size_t k=0;k<m_aParagraph[i]->nodes.size();++k )
					{
						NodeBase* node = m_aParagraph[i]->nodes[k];
						if ( node && node->bSelected )
						{
							if ( node->GetTextContent() )
							{
								int totalLen = strlen(pszText) + strlen(node->GetTextContent());
								if ( totalLen >= nLength )
									break;
								strcat( pszText, node->GetTextContent() );
							}
						}
					}
				}

				pszText[strlen(pszText)] = 0L;
				GlobalUnlock( hBlock );
			}

			SetClipboardData( CF_TEXT, hBlock );
		}
		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if( hBlock )
			GlobalFree( hBlock );
	}
}

bool CGuiMLTextBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			if ( (GetKeyState(VK_CONTROL) < 0) && (wParam == 'C') )
			{
				CopyToClipboard();
				return true;
			}
		}
		break;
	}

	return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param uMsg : ÃèÊö
 * \param pt : ÃèÊö
 * \param wParam : ÃèÊö
 * \param lParam : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
bool CGuiMLTextBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if(CGuiControl::HandleMouse(uMsg,pt,wParam,lParam))
		return true;

	if( !m_bEnabled || !m_bVisible )
		return false;

	// First acquire focus
	if( WM_LBUTTONDOWN == uMsg )
		if( !m_bHasFocus )
			m_pDialog->RequestFocus( this );

	if( m_VScrollBar.HandleMouse( uMsg, pt,wParam, lParam ) )
	{
		if(GetStyle() & GUI_TS_AUTOFADEOUT)
		{
			stRectI rcText = m_rcText;
			rcText.OffsetRect(m_pDialog->GetClientX(),m_pDialog->GetClientY());
			stPointI ptOffset(rcText.left,rcText.top);
			ptOffset.y -= m_VScrollBar.GetTrackPos();

			stRectI clipOld = GetDevice()->SetClipRect(&rcText);
			float   fTime = 0.0f;
			bool    bFind = false;
			for(int i=m_aParagraph.size()-1;i>=0;--i)
			{
				if (m_aParagraph[i]->IsShow(ptOffset,&rcText))
				{
					m_aParagraph[i]->time = fTime;
					fTime+=1.0f;
					bFind = true;
				}
				else if (bFind)
					break;
				
			}
			GetDevice()->SetClipRect(&clipOld);
		}
		return true;
	}

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
		{
			if( !ContainsPoint( pt ) )
				return false;
			if ( uMsg == WM_LBUTTONDOWN )
			{
				m_bMouseDrag = true;
				m_ptStartSel = pt;
				m_ptEndSel = pt;
			}
			if ( (m_bMouseDrag && (uMsg == WM_MOUSEMOVE)) )
				m_ptEndSel = pt;
			if ( uMsg == WM_LBUTTONUP )
				m_bMouseDrag = false;

			stRectI rcText = m_rcText;
			rcText.OffsetRect(m_pDialog->GetClientX(),m_pDialog->GetClientY());
			stPointI ptOffset(rcText.left,rcText.top);
			ptOffset.y -= m_VScrollBar.GetTrackPos();

			//Sel range
			stRectI rcSel;
			if ( (m_ptStartSel.x > m_ptEndSel.x)
				|| (m_ptStartSel.y > m_ptEndSel.y ) )
			{
				rcSel.left = m_ptEndSel.x;
				rcSel.top = m_ptEndSel.y;
				rcSel.right = m_ptStartSel.x;
				rcSel.bottom = m_ptStartSel.y;
			}
			else
			{
				rcSel.left = m_ptStartSel.x;
				rcSel.top = m_ptStartSel.y;
				rcSel.right = m_ptEndSel.x;
				rcSel.bottom = m_ptEndSel.y;
			}
			rcSel.OffsetRect( m_pDialog->GetClientX(), m_pDialog->GetClientY() );
			for(size_t i=0;i<m_aParagraph.size();++i)
			{
				if( m_bFilter){ 
					if( !m_aParagraph[i]->bIsGmMsg )	continue;
				}
				if (uMsg == WM_LBUTTONDBLCLK)
				{
					if(m_aParagraph[i]->IsInRect(ptOffset))
					{
						//m_pDialog->SendEvent( EVENT_ITEM_CLICKED, true, this );
						m_nSelect = i;
					}
				}
				else
				{
					if ( uMsg == WM_MOUSEMOVE && (GetStyle() & GUI_TS_SELECT_ENABLE) )
						m_aParagraph[i]->RefreshSelNode( ptOffset, &rcSel );

					if(m_aParagraph[i]->HandleMouse(uMsg,ptOffset,wParam,this))
					{
						if(!m_strNewDoc.empty())
						{
							if( !m_pDialog->SendEvent(EVENT_HYPERLINK_CLICKED,true,this,(DWORD)(LPVOID)m_strNewDoc.c_str()) )
							{
								char szFileName[MAX_PATH];
								sprintf(szFileName,"%s\\%s",m_strCurDir.c_str(),m_strNewDoc.c_str());
								LoadDoc(szFileName);
							}
							m_strNewDoc = "";
						}
						return true;
					}
				}
			}
			if ( GetStyle() & GUI_TS_SELECT_ENABLE )
			{
				if ( (uMsg == WM_MOUSEMOVE) || (WM_LBUTTONDOWN) || (WM_LBUTTONUP) )
					return true;
			}
			
			return false;
		}
		break;

	}

	return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nLineHeight : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::SetLineHeight(int nLineHeight)
{
	m_nLineHeight = nLineHeight;
	UpdateRects();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::ScrollToTail()
{
	//if(m_pDialog->IsFocus())
	{
		if(m_VScrollBar.IsKeepScroll()) return;
	}
	m_VScrollBar.SetTrackPos(m_VScrollBar.GetTrackEnd()-1);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::RemoveFirstParagraph()
{
	TiXmlElement* pBody = &GetXmlBody();
	if(!pBody) return;

	TiXmlElement* p = pBody->FirstChildElement();
	Assert(p);
	if(p)
		pBody->RemoveChild(p);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLTextBox::Reset()
{
	RefreshText();
	m_VScrollBar.SetPageSize(m_rcText.Height());
	m_VScrollBar.SetTrackRange(0,GetTextHeight());
}

bool CGuiMLTextBox::CanHaveFocus() 
{ 
	return (m_bVisible && m_bEnabled);
}