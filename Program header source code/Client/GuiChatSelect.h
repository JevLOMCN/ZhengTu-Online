/**
 *\file		GuiChatSelect.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:11:00 中国标准时间
 *\brief	聊天界面
 *
 * 
 *
*/

#pragma once
#include "..\gui\include\guidialog.h"
#include "./GuiButtonList.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
#define CHAT_TYPE_NUM	9

class CGuiButtonList;

class CGuiChatSelect :
	public CGuiDialog
{
	///屏幕宽度
	int m_iScreenWidth;
	///聊天编辑框
	CGuiEditBox* m_pEditChat;
	///聊天类型选择按钮
	CGuiButtonList m_aChatBtnList;
	//CGuiButton* m_pBtnChatType[8];

	BOOL ContainsPoint(POINT  ptScreen);

	bool CheckIndentify();

	
	struct stChatInfo
	{
		char szChatStr[MAX_CHATINFO];
	};
	struct stChatTypeInfo{
		std::list<stChatInfo>	m_strChatHistory; // size() <= 4
	};

	std::map<int,stChatTypeInfo> m_MapHistory;
	enumChatType m_curChatType;
	
	//void OnFilter();
public:

	void SetChatType(enumChatType iChatType);
	CGuiChatSelect(void);
	void OnClose(void);
	void OnCreate();
	HRESULT OnRender(float fElapsedTime);
	void OnInitScript();
	void OnResetScreenSize();
	void EndPopup();
	void BeginPopup();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void OnCompleteAniEvent();

	void SendChat();
	void AfterSendChat();
	CGuiEditBox* GetEditChat() { return m_pEditChat; }

	void InsertPrivateChat(const char* name);
	void ChatEditSetFocus();

	bool IsChatTypeBtnListIsPop() { return m_aChatBtnList.IsPop(); }
	void TurnOffChatTypeBtnList() { if ( m_aChatBtnList.IsPop() ) m_aChatBtnList.TurnOffBtnList(); }
private:
	void InternalSendChat();
};

