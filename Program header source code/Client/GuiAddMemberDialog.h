#pragma once
#include "..\gui\include\guidialog.h"

class CGuiAddMemberDialog :
	public CGuiDialog
{
public:
	CGuiAddMemberDialog(enumGuiActionType type,const char* pszTitle);
	~CGuiAddMemberDialog(void);

	void OnClose(void);
	void OnCreate();
	void OnInputBoxEnter(bool bOk);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);

	void UpdateNearCharacterList();
	void SetMemberName(const char* pszName);
	void SetTitle(const char* title);
private:
	CGuiEditBox*		m_pEditBox;
	CGuiListBox*		m_pListBox;
	std::string			m_title;
	enumGuiActionType	m_type;
};
