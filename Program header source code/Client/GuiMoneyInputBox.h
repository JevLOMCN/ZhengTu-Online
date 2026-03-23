/**
*\file		GuiMoneyInputBox.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    银子输入对话框
*
* 
*
*/


#pragma once

#include "..\gui\include\guidialog.h"

/**
 * \brief 银子输入对话框
 * 
 * 银子输入对话框
 * 
 */
class CGuiMoneyInputBox :
	public CGuiDialog
{
	///显示银子输入提示
	std::string m_strText;
	///对话框类型
	enumGuiActionType m_action;
	///对话框数据
	void*		m_pData;
	///银子输入编辑框
	CGuiEditBox* m_pEditBox[3];
	void OnInitScript();
	void SetNumber(CGuiEditBox* pEditBox,int minNumber,int maxNumber);
	int GetMoneyNum();


public:
	CGuiMoneyInputBox(const char* pszText,enumGuiActionType type,void* pData = NULL);
	void OnClose(void);
	void OnCreate();
	void OnMoneyInputBoxEnter(bool bOk);

	void OnGiveUnionMoney(bool bQuery = true);

	void bind_lua(lua_State* L);
	void SetDefaultMoney(int money);
};
