#pragma once
#include "..\gui\include\guidialog.h"

#define    MAXCHATNUM       5

class CGuiPrivateChatSelect :
	public CGuiDialog
{

	
	std::vector<std::string>  m_chatNameList;
	
public:
	CGuiPrivateChatSelect(void);

	CGuiListBox*    m_pListBox;

	void OnCreate();

	void EndPopup();
	void BeginPopup();

	void RenderBackground(float fElapsedTime);
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void AddWhisperChatName(const char* name);
	bool InitData();//WHISPER
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
	int GetPrivateChatNameNum();

	static void AddWhisperChatUser(const char* user);
};
