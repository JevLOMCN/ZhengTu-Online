#pragma once
#include "..\gui\include\guidialog.h"

#define ID_BOX_PART_NUM	4
#define ID_BOX_CHAR_NUM	5

class CGuiInputBoxFiveDialog :
	public CGuiDialog
{
public:
	CGuiInputBoxFiveDialog(const char* pszTitle);
	~CGuiInputBoxFiveDialog(void);

	void OnClose(void);
	void OnCreate();
	void OnInputBoxEnter(bool bOk);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

private:
	const char* GetText(const int index);

	bool VerifyEditBox(const int index);

private:
	std::string			m_strTitle;
	CGuiEditBox*		m_pEditBox[ID_BOX_PART_NUM];
};
