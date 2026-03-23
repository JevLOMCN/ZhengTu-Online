#pragma once
#include "..\gui\include\guidialog.h"

class CGuiNameMoneyInputBoxDialog :
	public CGuiDialog
{
public:
	CGuiNameMoneyInputBoxDialog(const char* pszText1,const char* pszText2,enumGuiActionType type,void* pData = NULL);
	~CGuiNameMoneyInputBoxDialog(void);

	void OnClose(void);
	void OnCreate();
	void OnMoneyInputBoxEnter(bool bOk);
	void OnInitScript();
private:
	///显示银子输入提示
	std::string			m_strText1;
	std::string			m_strText2;

	///对话框类型
	enumGuiActionType	m_action;
	///对话框数据
	void*				m_pData;

	CGuiEditBox*		m_pEditBox[3];
	CGuiEditBox*		m_pNameEdit;

private:
	int GetInputMoney();
	const char* GetInputName();
public:
	void bind_lua(lua_State* L);
};
