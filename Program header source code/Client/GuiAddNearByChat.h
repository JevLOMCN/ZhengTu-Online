#pragma once
#include "..\gui\include\guidialog.h"

class CGuiAddNearByChat :
	public CGuiDialog
{
	///显示输入提示
	std::string m_strText;
	///对话框类型
	enumGuiActionType m_action;
	///对话框数据
	void*		m_pData;

	///输入编辑框
	CGuiEditBox* m_pEditBox;
	CGuiListBox* m_pListBox;

public:
	CGuiAddNearByChat(const char* pszText,enumGuiActionType type,void* pData = NULL);

	void OnClose(void);
	void OnCreate();
	//void OnMoneyInputBoxEnter(bool bOk);
	
};
