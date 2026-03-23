#include "../../engine/include/engine.h"
#include "../include/cursor.h"
#include "../include/GuiManager.h"
#include "../../engine/include/IGraphicCursor.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CCursor* g_pCursor = NULL;
CCursor* Engine_GetCursor()
{
	return g_pCursor;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CCursor::CCursor(void)
: m_eType(CursorType_Normal)
, m_ptHotPoint(0,0)
, m_ptPosition(0,0)
, m_ptPositionOnWnd(0,0)
, m_hCursor(NULL)
{
	g_pCursor = this;
	m_bHardwareCursor = false;
	mImageResource.szFileName[0] = 0;
	mImageResource.frame = 0;
	mImageResource.group = 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CCursor::~CCursor(void)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CCursor::Run(float fElapsedTime)
{
	UpdateCursorPosition();
	GetGraphicCursor()->run();
}

void CCursor::UpdateCursorPosition()
{
	GetCursorPos(&m_ptPosition);

	m_ptPosition.x -= GetDevice()->GetWndX();
	m_ptPosition.y -= GetDevice()->GetWndY();
	m_ptPositionOnWnd = m_ptPosition;
	GetDevice()->ConverMousePoint(m_ptPosition);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CCursor::Render(float fElapsedTime)
{
	if(GetDevice()->IsSupportHardwareCursor())
	{
		if(m_bHardwareCursor)
			return;
	}else{
		if(Engine_GetAppation()->GetConfigBase()->bHardwareCursor)
			return;
	}

	stPointI pt = m_ptPosition;
	pt -= m_ptHotPoint;
	m_image.Render( pt.x,pt.y );
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param uMsg : ÃèÊö
 * \param wParam : ÃèÊö
 * \param lParam : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CCursor::MsgProc( UINT uMsg,WPARAM wParam,LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		//m_ptPosition.x = GET_X_LPARAM(lParam);
		//m_ptPosition.y = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONDOWN:
		m_bLButtonDown = true;
		break;
	case WM_LBUTTONUP:
		m_bLButtonDown = false;
		break;
	case WM_RBUTTONDOWN:
		m_bRButtonDown = true;
		break;
	case WM_RBUTTONUP:
		m_bRButtonDown = false;
		break;
	case WM_SETCURSOR:
		{
			DWORD hit = (lParam & 0xffff);
			if(GetDevice()->IsSupportHardwareCursor()){
				// ÊÇÖ§³ÖÓ²¼þÊó±êµÄ
				if(m_bHardwareCursor && GetGuiManager()->IsShowCursor()){
					if(HTCAPTION == hit || HTCLIENT == hit){
						SetCursor(NULL);
						GetGraphicCursor()->showCursor(true);
						return true;
					}else{
						GetGraphicCursor()->showCursor(false);
					}
				}else{
					GetGraphicCursor()->showCursor(false);
					if(hit == HTCLIENT){
						SetCursor(NULL);
						return true;
					}
				}
				return false;
			}else{
				// Èç¹û²»Ö§³ÖÓ²¼þÊó±ê£¬ÇÒÑ¡ÔñÁËÊ¹ÓÃÓ²¼þÊó±êÔòÊ¹ÓÃÆÕÍ¨µÄ¼ýÍ·
				if(hit == HTCLIENT){
					if(!Engine_GetAppation()->GetConfigBase()->bHardwareCursor){
						SetCursor(NULL);
						return true;
					}
				}
				return false;
			}
		}
		break;
	}
	return false;
}

void CCursor::SetImage(const stResourceLocation * rl,stPointI ptHotPoint)
{
	mImageResource = *rl;
	/*m_hCursor = GetCursorHandle(rl);
	if(m_hCursor){
		ShowCursor(TRUE);
		SetCursor(m_hCursor);
	}else{
		ShowCursor(FALSE);
		SetCursor(NULL);
		}*/
	OnSetHardwareCursor(rl,ptHotPoint);
	m_image.Create(rl);
	m_ptHotPoint = ptHotPoint;
}

void	CCursor::ResetCursor()
{
	OnSetHardwareCursor(&mImageResource,m_ptHotPoint);
	/*m_hCursor = GetCursorHandle(&mImageResource);
	if(m_hCursor){
		ShowCursor(TRUE);
		SetCursor(m_hCursor);
	}else{
		ShowCursor(FALSE);
		SetCursor(NULL);
	}*/
}