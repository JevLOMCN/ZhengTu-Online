/**
*\file		GuiFace.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2005-7-5   17:12:00 中国标准时间
*\brief	    聊天表情对话框
*
* 
*
*/



#include "public.h"
#include "./GameAppation.h"
#include "./GameGuiManager.h"
#include "./GuiChatSelect.h"
#include ".\guiface.h"

///对话框左上角坐标
const stPointI  clientOffsetTopLeft(8,8);
///对话框右上角坐标
const stPointI  clientOffsetRightBottom(11,11);
///表情宽度
const int       s_faceWidth=16;
///表情高度
const int       s_faceHeight=16;
///选择区域宽度
const int       s_Width=25;
///选择区域高度
const int       s_Height=25;
///表情之间的空隙
const int       s_offsetFrame=2;
///表情行高
const int nFaceOffset=25;
///表情相对对话框偏移位置
const stPointI	dlgOffset(15,-50);

/**
 * \brief 弹出对话框
 * 
 * 弹出对话框
 * 
 * \return 无
 */
void CGuiFace::BeginPopup()
{
	FUNCTION_BEGIN;

	CGuiDialog::BeginPopup();

	stGuiAnimationInfo st;
	st.dwColorDst = GetColor();
	st.ptDst = GetLocation();
	st.fTime = 0.3f;
	st.eAniComplete = GuiAniEvent_None;
	BeginAnimation( &st );

	FUNCTION_END;
}

/**
 * \brief 关闭对话框
 * 
 * 关闭对话框
 * 
 * \return 无
 */
void CGuiFace::EndPopup()
{
	FUNCTION_BEGIN;

	CGuiDialog::EndPopup();
	
	stGuiAnimationInfo st;
	st.dwColorDst = GetColor();
	st.ptDst = GetLocation();
	st.ptDst.y += 100;
	st.fTime = 0.3f;
	st.eAniComplete = GuiAniEvent_Hide;
	BeginAnimation( &st );

	if ( m_pEditBox->m_pDialog )
	{
		GetGuiManager()->RequestFocus( m_pEditBox->m_pDialog);
		m_pEditBox->m_pDialog->SetFocus( m_pEditBox );
	}
	FUNCTION_END;
}

/**
 * \brief 聊天表情对话框开关
 * 
 * 聊天表情对话框开关
 * 
 * \param pEditBox : 绑定聊天表情对话框的聊天输入框
 * \return 无
 */
void OnOffChatFaceSelect(CGuiEditBox* pEditBox)
{
	FUNCTION_BEGIN;


	if (!GetGameGuiManager()->m_guiFace)
		GetGameGuiManager()->AddFace();

	if(GetGameGuiManager()->m_guiFace->IsVisible())
	{
		GetGameGuiManager()->m_guiFace->EndPopup();
	}
	else
	{
		GetGameGuiManager()->m_guiFace->m_pEditBox=pEditBox;
		stPointI ptMouse=Engine_GetCursor()->GetPosition();
		(ptMouse.x+GetGameGuiManager()->m_guiFace->GetFaceRect()->right)>Engine_GetAppation()->GetConfigBase()->screenWidth?ptMouse.x=ptMouse.x-GetGameGuiManager()->m_guiFace->GetFaceRect()->right:ptMouse.x=ptMouse.x;
		(ptMouse.y+GetGameGuiManager()->m_guiFace->GetFaceRect()->bottom)>Engine_GetAppation()->GetConfigBase()->screenHeight?ptMouse.y=ptMouse.y-GetGameGuiManager()->m_guiFace->GetFaceRect()->bottom:ptMouse.y=ptMouse.y;

		(ptMouse.y+GetGameGuiManager()->m_guiFace->GetFaceRect()->bottom+nFaceOffset)>Engine_GetAppation()->GetConfigBase()->screenHeight?ptMouse.y-=nFaceOffset:ptMouse.y=ptMouse.y;
		if ( GetGameGuiManager()->m_guiFace->GetScreenWidth() == 1024 )
		{
			GetGameGuiManager()->m_guiFace->SetLocation(ptMouse.x+dlgOffset.x,ptMouse.y+dlgOffset.y);
		}
		else if ( GetGameGuiManager()->m_guiFace->GetScreenWidth() == 1280 )
		{
			GetGameGuiManager()->m_guiFace->SetLocation(ptMouse.x+dlgOffset.x-264,ptMouse.y+dlgOffset.y);
		}
		else
		{
			GetGameGuiManager()->m_guiFace->SetLocation(ptMouse.x+dlgOffset.x,ptMouse.y+dlgOffset.y);
		}

		//GetGameGuiManager()->m_guiFace->SetVisible( true );
		GetGameGuiManager()->m_guiFace->BeginPopup();
	}

	FUNCTION_END;
}


/**
 * \brief 构造函数
 * 
 * 构造函数
 * 
 * \return 无
 */
CGuiFace::CGuiFace(void)
:m_pEditBox(NULL)
,m_clientRect(0,0,0,0)
{
	FUNCTION_BEGIN;

	m_nSelect=-1;
	m_bCloseIsHide = true;
	m_faceList.clear();

	m_iScreenWidth = 1024;

	m_bSmDialog = true;

	FUNCTION_END;
}


/**
 * \brief 对话框创建
 * 
 * 对话框创建
 * 
 * \return 无
 */
void CGuiFace::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	
	m_bCaption=false;
	m_clientOffsetTopLeft=clientOffsetTopLeft;
	m_clientOffsetRightBottom=clientOffsetRightBottom;
	
	FillFaceList();

	OnResetScreenSize();

	FUNCTION_END;
}

/**
 * \brief 重新刷新客户区域大小
 * 
 * 重新刷新客户区域大小
 * 
 * \return 无
 */
void CGuiFace::OnResetScreenSize()
{
	FUNCTION_BEGIN;

	int iScreenWidth = GetDevice()->GetWidth();
	if(iScreenWidth == m_iScreenWidth) return;
	m_iScreenWidth = iScreenWidth;	

	FUNCTION_END;
}

/**
 * \brief 设置对话框皮肤
 * 
 * 设置对话框皮肤
 * 
 * \return 无
 */
void CGuiFace::InitCustomElements()
{
	FUNCTION_BEGIN;

	m_BackElements.resize(2);
	CGuiElement* pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,255,0,0);
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	m_BackElements[0] = pEmt;

	pEmt = new CGuiElement;
	pEmt->FontColor.Current = COLOR_ARGB(255,255,255,255);
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	m_BackElements[1] = pEmt;

	

	FUNCTION_END;
}

/**
 * \brief 刷新对话框背景
 * 
 * 刷新对话框背景
 * 
 * \param fElapsedTime : 刷新时间
 * \return 无
 */
void CGuiFace::RenderBackground(float fElapsedTime)
{
	FUNCTION_BEGIN;

	stRectI rect=m_clientRect;
	rect.OffsetRect(m_x+5,m_y+5);

	GetDevice()->FillRect(&rect,m_BackElements[0]->TextureColor.Current);

	FUNCTION_END;
}

/**
 * \brief 刷新数据和图片
 * 
 * 刷新数据和图片
 * 
 * \param fElapsedTime : 刷新时间
 * \return 刷新数据和图片
 */
HRESULT CGuiFace::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;

	ResetSelectFace();

	HRESULT  hr = CGuiDialog::OnRender(fElapsedTime);
	WCHAR  showFace[2]={0};
	
	for (int i=0;i<m_faceList.size();i++)
	{
		showFace[0]=m_faceList[i].showid;
		DrawText( (char*)showFace,m_BackElements[1],&(m_faceList[i].rect));
	}
	
	TipsRender();
	return hr;

	FUNCTION_END;
}

/**
 * \brief 刷新所有聊天表情数据
 * 
 * 刷新所有聊天表情数据
 * 
 * \return 无
 */
void  CGuiFace::FillFaceList()
{
	FUNCTION_BEGIN;

	stFace  face;
	stRectI rect;
	int     nCount = GetGameApplication()->GetPicCount();
	

	for (int i=0;i<nCount;i++)
	{
		memset(&face,0,sizeof(stFace));
		if (GetGameApplication()->SearchPicChar(face.esc,face.allesc," ",i))
		{
			int row=m_faceList.size()/10;
			int col=m_faceList.size()%10;
			rect.left=s_Width*col;
			rect.top=s_Height*row;
			rect.right=rect.left+s_faceWidth;
			rect.bottom=rect.top+s_faceHeight;
			face.rect=rect;

			GetGameApplication()->ConverPicChar(face.esc,face.showid,face.charnum);
			m_faceList.push_back(face);

			if (rect.right>m_clientRect.right)
				m_clientRect.right=rect.right+s_offsetFrame;
			if (rect.bottom>m_clientRect.bottom)
				m_clientRect.bottom=rect.bottom+s_offsetFrame;
		}
	}
	
	m_clientRect.bottom+=clientOffsetRightBottom.y;
	m_clientRect.right+=clientOffsetRightBottom.x;

	this->SetSize(m_clientRect.right,m_clientRect.bottom);
	

	FUNCTION_END;
}


/**
 * \brief 聊天表情提示显示
 * 
 * 聊天表情提示显示
 * 
 * \return 无
 */
void  CGuiFace::TipsRender()
{
	FUNCTION_BEGIN;

	tips.Clear();
	tips.SetCurColor(COLOR_ARGB(255,255,255,255));

	if (m_nSelect>=0)
	{
		stRectI  rect(m_faceList[m_nSelect].rect.left-s_offsetFrame,
			m_faceList[m_nSelect].rect.top-s_offsetFrame,
			m_faceList[m_nSelect].rect.right+s_offsetFrame,
			m_faceList[m_nSelect].rect.bottom+s_offsetFrame);
		rect.OffsetRect(m_x + m_clientOffsetTopLeft.x,m_y + m_clientOffsetTopLeft.y);
		GetDevice()->DrawRectangle(&rect,m_BackElements[0]->FontColor.Current);
		tips.AddText((m_faceList[m_nSelect]).allesc);

		tips.Resize();

		stPointI pt(Engine_GetCursor()->GetPosition());

		int xPos=(pt.x+28+tips.GetWidth()>=1024)?1024-tips.GetWidth():pt.x+28  ;
		tips.Render(xPos,pt.y+28);
	}

	FUNCTION_END;
}

/**
* \brief 重新设置当前选中的聊天表情
* 
* 重新设置当前选中的聊天表情
* 
* \return 无
*/
void  CGuiFace::ResetSelectFace()
{
	FUNCTION_BEGIN;

	m_nSelect=-1;

	for (int i=0;i<m_faceList.size();i++)
	{
		if (m_faceList[i].rect.PtInRect(m_ptMouse))
		{
			m_nSelect=i;
			return;
		}
	}

	FUNCTION_END;
}

/**
 * \brief 对话框消息处理函数
 * 
 * 对话框消息处理函数
 * 
 * \param hWnd : 
 * \param uMsg : 
 * \param wParam : 
 * \param lParam : 
 * \return 返回值的描述
 */
bool CGuiFace::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	switch(uMsg) {
	case WM_MOUSEMOVE:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			mousePoint.x -= m_x + m_clientOffsetTopLeft.x;
			mousePoint.y -= m_y + m_clientOffsetTopLeft.y;
			m_ptMouse=mousePoint;
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if (m_nSelect>=0)
			{
				WCHAR  showFace[2]={0};
				showFace[0]=m_faceList[m_nSelect].showid;
				m_pEditBox->SetSelText((char*)showFace);

				//Set chat edit box focus
				{
					GetGuiManager()->RequestFocus(m_pEditBox->m_pDialog);
					m_pEditBox->m_pDialog->SetFocus((CGuiControl*)m_pEditBox);
				}
					
			}
		}
		break;
	
	}
	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);

	FUNCTION_END;
}