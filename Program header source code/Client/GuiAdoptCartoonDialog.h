#pragma once
#include "..\gui\include\guidialog.h"

class CGuiAdoptCartoonDialog :
	public CGuiDialog
{
public:
	CGuiAdoptCartoonDialog(void);
	~CGuiAdoptCartoonDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);

	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool OnContextMenu();

	void UpdateCartoonPetList();
	void SwitchFromWorldConn();
private:
	const char* GetSelectCharacterName();

	bool OnAdoptCartoonPet();
	bool OnReturnCartoonPet();
	bool OnSwitchToWorldConn(UINT nID);

	bool OnUpdateButtons();

	void OnPopupChatMenu();
	void OnMenuCmdChat(UINT nID);

	CGuiListBox*	m_guiListBoxCartoon;
	CGuiMenu*		m_ChatMenu;
};
