#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CGuiFloatControl :
	public CGuiDialog
{
	BOOL ContainsPoint( POINT ptScreen );
public:
	bool m_bPretendedHide;
	CGuiFloatControl(void);
	void OnClose(void);
	void OnInitScript();
	void OnCreate();
	HRESULT OnRender( float fElapsedTime );

	void OnResetScreenSize();
};
