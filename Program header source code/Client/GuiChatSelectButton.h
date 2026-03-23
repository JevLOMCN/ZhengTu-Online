#pragma once
#include "..\gui\include\guidialog.h"
#include "./GuiButtonList.h"



class CGuiChatSelectButton :
	public CGuiDialog
{
	///聊天类型选择按钮
	CGuiButtonList m_aChatBtnList;

	BOOL ContainsPoint(POINT  ptScreen);

public:
	CGuiChatSelectButton(void);

	void OnClose(void);
	void OnCreate();
	HRESULT OnRender(float fElapsedTime);

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	bool IsChatTypeBtnListIsPop() { return m_aChatBtnList.IsPop(); }
	void TurnOffChatTypeBtnList() { if ( m_aChatBtnList.IsPop() ) m_aChatBtnList.TurnOffBtnList(); }
	
	void BeginPopup();
	void EndPopup();

	void OnResetScreenSize();

	void ChangeActiveChatModeBtn(int type);
	void JumpToChatMode(int mode);

	bool TreateAllChatModeAccessKey(long key);
};

void OnOffChatSelectButton(stPointI pt);
void ChangeActiveChatModeBtn();
bool IsChatTypeClosed(DWORD type);