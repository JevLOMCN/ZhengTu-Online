#pragma once

#include "..\gui\include\guidialog.h"

class CGuiRightFadeNotifyDialog :
	public CGuiDialog
{
public:

	void OnClose(void);
	void OnResetScreenSize();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnCreate();
	void OnSetCloseLocation();
	
	void SetInfo(int index, char* c,DWORD color = COLOR_ARGB(255,255,255,255) );

	virtual HRESULT OnRender( float fElapsedTime );  

private:
	CGuiTab* m_tab;
	CGuiMLTextBox* m_multiTextBox[5];

	time_t	m_lastCreateTime;

};


