#pragma once
#include "..\gui\include\guidialog.h"

class CGuiSelectDialog :
	public CGuiDialog
{
public:
	CGuiSelectDialog(void);
	~CGuiSelectDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	void OnSetCloseLocation();

	bool OnIDOk();
	bool OnIDCancel();

	void SetText(const char* pszText);
	void AddListItem(const char *szText, void *pData  ,DWORD color  = -1,IBitmaps* icon = NULL);
	int GetCurSelectItem();
	void* GetCurSelectItemData();
	const char* GetCurItemText();

	CGuiListBox	 *	m_pList;				//列表
	char			m_strText[MAX_PATH];	//题头
};
