#pragma once
#include "guicontrol.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiPanel :
	public CGuiControl
{
	stRectI m_rcRender[9];
	int m_nBorder;
public:
	static stPointI s_borderTopLeft;
	static stPointI s_borderRightBottom;

	CGuiPanel(CGuiDialog* pDialog);
	~CGuiPanel(void);
	void Render( float fElapsedTime );
	void UpdateRects();
};
