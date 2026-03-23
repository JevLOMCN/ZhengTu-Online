#pragma once
#include "..\gui\include\guidialog.h"

class CGuiKingSelectDialog :
	public CGuiDialog
{
public:
	CGuiKingSelectDialog(void);
	~CGuiKingSelectDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	void OnSetCloseLocation();

	bool OnIDOk();
	bool OnIDCancel();

	void SetText(const char* pszText);
	void ClearList();
	void AddListItem(const char *szText, void *pData  ,DWORD color  = -1,IBitmaps* icon = NULL);
	int GetCurSelectItem();
	void* GetCurSelectItemData();
	const char* GetCurItemText();

	void ClearListForeigner();
	void AddForeigner(const std::string& text);

	CGuiListBox	 *	mx_pList;				//列表
	char			m_strText[MAX_PATH];	//题头
	CGuiEditBox*	m_editboxPos;
	CGuiListBox*	m_listboxForeigner;

	DWORD	m_dwThisId;
	static std::vector<std::string>	m_vForeigner;
};
