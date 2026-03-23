/**
*\file		GuiChatInvite.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2005-7-5   17:12:00 中国标准时间
*\brief	    组聊邀请或聊天选择对话框
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"


struct tChatInvite;

/**
 * \brief 组聊邀请或聊天选择对话框
 * 
 * 组聊邀请或聊天选择对话框
 * 
 */
class CGuiChatInvite :
	public CGuiDialog
{
	///输入聊天名字的选择
	CGuiRadioButton*	m_pRadio1;
	///选择社会关系聊天者的选择
	CGuiRadioButton*	m_pRadio2;
	///对应的私聊对话框
	CGuiDialog*			m_pDlg;
	///同意组聊请求
	CGuiRadioButton*	m_pRadio3;
	///拒绝组聊请求
	CGuiRadioButton*	m_pRadio4;
	///请求保留X分钟
	CGuiRadioButton*	m_pRadio5;
	///选择附近玩家聊天者
	CGuiRadioButton*	m_pRadio6;
	///提示信息
	CGuiStatic*			m_pLabel;

	tChatInvite*		m_pChatInvite;
public:
	CGuiChatInvite(tChatInvite* pInvite);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	void SetTitleInfo(const char* title);
};
