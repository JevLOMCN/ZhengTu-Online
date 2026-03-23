#pragma once
#include "..\gui\include\guidialog.h"

class CGuiFriendLevelup :
	public CGuiDialog
{
public:
	CGuiFriendLevelup();
	~CGuiFriendLevelup();

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	void OnCreate();
	void OnSetCloseLocation();
	HRESULT OnRender(float fElapsedTime);

	void  SetInfo(DWORD playerid,const char *name,DWORD playerlevel,DWORD type);

	DWORD m_dwPlayerID;
	CGuiStatic*	m_pStatic;
};
