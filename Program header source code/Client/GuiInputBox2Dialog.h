#pragma once
#include "..\gui\include\guidialog.h"

class CGuiInputBox2Dialog :
	public CGuiDialog
{
public:
	CGuiInputBox2Dialog(const char* pszText1,const char* pszText2,enumGuiActionType type,void* pData = NULL);
	~CGuiInputBox2Dialog(void);

	void OnClose(void);
	void OnCreate();
	void OnInputBoxEnter(bool bOk);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void SetTextLength(int length1,int length2);

	void bind_lua(lua_State* L);

private:
	const char* GetText1();
	const char* GetText2();
#ifdef __SPRING__
	bool OnSendBlessMsg(bool bOk);
#endif
private:
	enumGuiActionType	m_action;
	std::string			m_strText1,m_strText2;
	void*				m_pData;
	CGuiEditBox*		m_pEditBox1;
	CGuiEditBox*		m_pEditBox2;	

};
