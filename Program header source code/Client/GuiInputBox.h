/**
*\file		GuiInputBox.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    输入对话框
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief 输入对话框
 * 
 * 输入对话框
 * 
 */
class CGuiInputBox :
	public CGuiDialog
{
	friend void OnInputBoxEnter(void* pDlg,bool bOk);
	///显示输入提示
	std::string m_strText;
	///对话框类型
	enumGuiActionType m_action;
	///对话框数据
	void*		m_pData;
	///银子输入编辑框
	CGuiEditBox* m_pEditBox;
	CGuiComboBox * m_pComboxEdit;
	void OnInitScript();

	char	m_szInputData[512];
private:
	const char* GetInputText();

	void OnNpcDialogInput(int type,const char* pszStr);
public:
	CGuiInputBox(const char* pszText,enumGuiActionType type,void* pData = NULL);
	void OnClose(void);
	void OnCreate();
	void SetDefault(const char* pszDefault);
	void SetNumber(int minNumber,int maxNumber);
	void SetLengthOfText(int iLength);	//设置输入文本的长度
	void OnInputBoxEnter(bool bOk);

	void bind_lua(lua_State* L);
	CGuiEditBox* GetMoneyEditBox(){return m_pEditBox;}
};
