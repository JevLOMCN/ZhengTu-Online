#include "../../engine/include/engine.h"
#include "..\include\guiTypes.h"

stPointI CGuiPanel::s_borderTopLeft(13,34);
stPointI CGuiPanel::s_borderRightBottom(12,12);

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiPanel::CGuiPanel(CGuiDialog* pDialog)
: CGuiControl(pDialog)
{
	m_nBorder = 16;
	m_Type = GUI_CONTROL_PANEL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiPanel::~CGuiPanel(void)
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
void CGuiPanel::Render( float fElapsedTime )
{
	CGuiControl::Render(fElapsedTime);
	m_pDialog->DrawControlBorder(m_rcBoundingBox,GUI_CONTROL_PANEL,-1,true);
	//for( int e = m_Elements.size() - 1; e > 0; --e )
	//{
	//	stRectI rc = m_rcRender[e];
	//	CGuiElement* pElement = m_Elements[ e ];
	//	pElement->TextureColor.Blend( GUI_STATE_NORMAL, fElapsedTime,0.0f );
	//	//m_pDialog->DrawSprite( pElement, &rc );
	//	m_pDialog->PatchDrawSprite(pElement,&rc,s_PatchAlignFormat[e]);
	//}

	//if(m_Elements.size() > 9)
	//{
	//	CGuiElement* pElement = m_Elements[9];
	//	//pElement->TextureColor.Blend( GUI_STATE_NORMAL, fElapsedTime,0.0f );
	//	stRectI rect = m_rcRender[0];
	//	m_pDialog->DrawSprite(pElement,&rect);
	//}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
// 1 2 3
// 4 0 5
// 6 7 8
void CGuiPanel::UpdateRects()
{
	CGuiControl::UpdateRects();

	CuttRect9(m_rcBoundingBox,s_borderTopLeft,s_borderRightBottom,m_rcRender);
}