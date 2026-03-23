/**
*\file		GuiWorldBoard.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2005-7-5   17:12:00 中国标准时间
*\brief	    公告查看,写入对话框
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief 公告查看,写入对话框
 * 
 * 公告查看,写入对话框
 * 
 */
class CGuiWorldBoard :
	public CGuiDialog
{
	///要查看的公告内容
	std::string       m_strText;
	///公告的标题
	std::string       m_strTitle;
	///公告的类型名字
	std::string       m_strCaption;
	///公告的类型
	enumGuiActionType  m_action;
	///公告的数据
	void*		       m_pData;
	///公告编辑框控件
	CGuiMLEditBox*     m_pEditBox;
	///是查看还是编辑
	bool              m_bEdit;


	bool OnWorldBoardEnter();
	bool SendBoard(byte state);

public:
	CGuiWorldBoard(const char* pszCaption,const char* pszTitle,const char* pszText,
		enumGuiActionType type,bool bEdit,void* pData = NULL);
	
	void OnClose(void);
	void OnCreate();
	

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

};
