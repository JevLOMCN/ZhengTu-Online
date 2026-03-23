/**
*\file		GuiFace.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2005-7-5   17:12:00 中国标准时间
*\brief	    聊天表情对话框
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief 聊天表情对话框
 * 
 * 聊天表情对话框
 * 
 */
class CGuiFace :
	public CGuiDialog
{
	struct stFace {
		char     esc[32];
		char     allesc[512];
		WORD     showid;
		stRectI  rect;
		int      charnum;
	};
	///表情列表
	std::vector<stFace> m_faceList;
	///当前鼠标位置
	stPointI     m_ptMouse;
	///当前选择的表情ID
	int         m_nSelect;
	///表情提示类
	CToolTips      tips;
	///聊天表情对话框背景区域
	stRectI     m_clientRect;
	
	///客户端区域宽度
	int		m_iScreenWidth;

	void  TipsRender();
	void  FillFaceList();
	void  ResetSelectFace();

public:
	CGuiFace(void);
	
	HRESULT OnRender( float fElapsedTime );  
	void OnCreate(void);
	void InitCustomElements();
	void RenderBackground(float fElapsedTime);
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void OnResetScreenSize();

	stRectI*   GetFaceRect(){ return &m_clientRect;}
	CGuiEditBox*       m_pEditBox;

	int GetScreenWidth() { return m_iScreenWidth; }

	void EndPopup();
	void BeginPopup();
};

void OnOffChatFaceSelect(CGuiEditBox* pEditBox);