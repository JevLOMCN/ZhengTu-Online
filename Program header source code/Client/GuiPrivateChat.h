/**
*\file		GuiPrivateChat.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    私人聊天对话框
*
* 
*
*/


#pragma once
#include "..\gui\include\guidialog.h"


#include <vector>
#include <string>


class CChatChannel;
class CGuiPrivateChat;


class CGuiChatInvateDialog :
	public CGuiDialog
{
	void*			m_pData;
	CGuiListBox*    m_pListBox;
	byte            m_eState;
	CGuiMenu*		m_ChatMenu;

public:

	enum
	{
		eChat,
		eTong,
		eSept,
	};
	CGuiChatInvateDialog(byte state);
	void SetData(void* pData);
	void* GetData();
	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void InsertPrivatName(const char* pStr);
	void SetCloseIsHide(bool hide);

	static void OnChannelClosed(CChatChannel* pChannel);
	static void AddPrivateChatName(const char* name);
private:
	void OnPopupChatMenu();
	bool OnContextMenu();
	bool OnMenuCmdChat(UINT nID);
};

//
class CGuiChatDialog : public CGuiDialog
{
public:
	CGuiChatDialog();
	virtual ~CGuiChatDialog();
	void OnClose();

	void OnCreate();
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void AddMessage(const char* msg,DWORD color = COLOR_ARGB(255,255,255,255) );
	void AddMessage(TiXmlElement* page);

	virtual void UpdateDialogTitle();			
	void AddChatUser(const char* name,DWORD color = COLOR_ARGB(255,255,255,255));			
	void RemoveAllUser();						
	void RemoveAllMsg();						
	void LoadChatMsg( const char* pszFileName );		
	void ScrollToTail();						

private:
	void OnPopupChatMenu();
	bool OnContextMenu();
	bool OnMenuCmdChat(UINT nID);

	void SendChat();
protected:
	///聊天频道
	CChatChannel*	m_pChannel;

	///聊天输入框
	CGuiEditBox*	m_pEditInput;

	///聊天输出框
	CGuiMLTextBox*	m_pEditOutput;

	///聊天对象框
	CGuiListBox*	m_pListUser;

	//
	CGuiMenu*		m_ChatMenu;
};

/**
 * \brief 私人聊天对话框
 * 
 * 私人聊天对话框
 * 
 */
class CGuiPrivateChat :
	public CGuiChatDialog
{
public:
	CGuiPrivateChat(CChatChannel* pChannel);
	~CGuiPrivateChat();

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void UpdateDialogTitle();
private:
	bool CanInvite();
};

//家族帮会聊天
class CGuiFUChatDialog : public CGuiChatDialog
{
public:
	CGuiFUChatDialog(DWORD chatType,CChatChannel* pChannel);

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void UpdateDialogTitle();
private:
	DWORD m_dwChatType;
};

//玩家列表
class CGuiUserListDialog : public CGuiDialog
{
public:
	CGuiUserListDialog();

	void OnCreate();
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose();
	bool OnContextMenu();
	HRESULT OnRender( float fElapsedTime );
	//
	void UpdateUserList();
	void SetChannel(CChatChannel* pChannel) { m_pChannel = pChannel; }

	static void OnChannelClosed(CChatChannel* pChannel);
	static void OnRelationUpdate();

private:
	void UpdateNearCharacterList();
private:
	///聊天频道
	CChatChannel*	m_pChannel;

	CGuiMenu*		m_pMenu;

	CGuiListBox*	m_pUserList[4];
};