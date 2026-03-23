#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include ".\guiprocess.h"

stPointI CGuiProcess::s_borderTopLeft(5,2);
stPointI CGuiProcess::s_borderRightBottom(4,2);

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiProcess::CGuiProcess(CGuiDialog *pDialog)
:m_nMin(0)
,m_nMax(100)
,m_nValue(0)
{
	m_Type = GUI_CONTROL_PROCESS;
	m_pDialog=pDialog;
	ZeroMemory( &m_strText, sizeof(m_strText) );  
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiProcess::Render( float fElapsedTime )
{

	CGuiControl::Render(fElapsedTime);
	
	m_pDialog->DrawControlBorder(m_rcBoundingBox,m_Type,-1,true);
	
	
	if(m_Elements.size() == 1)
	{
		CGuiElement* pElement = m_Elements[0];

		stRectI rect=m_rcProcess;
		rect.OffsetRect(m_pDialog->GetClientX(),m_pDialog->GetClientY());

		pElement->pBmp->Render(0,rect.left-8,rect.top-3,&m_rcBmpProcess,NULL);

		GUI_CONTROL_STATE iState = GUI_STATE_NORMAL;
		//pElement->FontColor.Blend( iState, fElapsedTime );
		pElement->FontColor.Current = COLOR_ARGB(255,255,255,255);
		 m_pDialog->DrawText( m_strText, pElement, &m_rcBoundingBox, FontEffect_Shadow );

		/*stRectI rcWindow(m_x,m_y,m_width + m_x , m_height + m_y);
		m_pDialog->DrawText( m_strText, pElement, &rcWindow );*/
	}
	
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiProcess::UpdateRects()
{
	CGuiControl::UpdateRects();

	stGuiCtrlBorderInfo* pBorderInfo = m_pDialog->GetCtrlBorderInfo(m_Type);
	if(pBorderInfo)
	{
		CuttRect9(m_rcBoundingBox,pBorderInfo->ptBorderLeftTop,pBorderInfo->ptBorderRightBottom,m_rcRender);
	}

	//CuttRect9(m_rcBoundingBox,s_borderTopLeft,s_borderRightBottom,m_rcRender);

	UpdateProcessRect();
	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiProcess::UpdateProcessRect()
{
	stRectI rect = m_rcRender[0];
	m_rcProcess.left=rect.left;
	m_rcProcess.top=rect.top;
	m_rcProcess.bottom=rect.bottom;
	m_rcProcess.right=rect.left+(int)((m_nValue-m_nMin)*(rect.right-rect.left)/(m_nMax-m_nMin));
	
	m_rcBmpProcess = stRectI(0,0,0,0);
	if(m_Elements.size() == 1)
	{
		rect = m_Elements[0]->rcTexture;
		m_rcBmpProcess = rect;

		m_rcBmpProcess.right = rect.left+(int)((m_nValue-m_nMin)*(rect.right-rect.left)/(m_nMax-m_nMin));
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nPos : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int  CGuiProcess::SetPos(int nPos)
{ 
	m_nValue = nPos;
	if (m_nValue<m_nMin)
		m_nValue=m_nMin;
	if (m_nValue>m_nMax)
		m_nValue=m_nMax;
	UpdateProcessRect();
	return m_nValue; 
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiProcess::SetProcessBitmap(stResourceLocation& rl)
{
	for(int i=0;i<m_Elements.size(); ++i)
		delete m_Elements[i];

	m_Elements.resize(0);
	m_Elements.resize(1);
	

	CGuiElement* pEmt = new CGuiElement;
	
	pEmt->SetTexture(&rl);

	m_Elements[0] = pEmt;
	
	UpdateRects();
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param strText : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
void CGuiProcess::SetText( const char* strText )
{
	if( strText == NULL )
	{
		m_strText[0] = 0;
		return ;
	}

	strncpy( m_strText, strText, MAX_PATH );
	m_strText[MAX_PATH-1] = 0;
	return ;
}
