/**
 *\file		GuiChatOutput.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:10:00 中国标准时间
 *\brief	聊天输出界面
 *
 * 
 *
*/

#pragma once
#include "..\gui\include\guidialog.h"


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiChatOutput :
	public CGuiDialog
{
	///聊天输出框
	CGuiMLTextBox* m_pEditOutput[3];
	char          m_szPrivateName[MAX_NAMESIZE+10];
	byte          m_eState;

	bool AddNineChatMessage(const stChannelChatUserCmd* & pCmd);
	void AddItemChatMessage(const stChannelChatUserCmd* & pCmd);
	void AddCommChatMessage(const stChannelChatUserCmd* & pCmd);

	void ResetChat();

    bool m_bEnableScroll[3];
	bool m_bOnlyGmMsg[3];

	void AppendParagraph(TiXmlElement * page,const stChannelChatUserCmd* pCmd);

	CGuiMenu*       m_pMenu;
	void UpdateMakeGroupMenuState(CGuiMenu * pMenu,int * pBtn,int BtnSize);
	std::string m_strChatUser;
	bool   m_bPopMenu;

	CGuiTab*		m_pTab;
	CGuiMenu*		m_pConfigMenu[2];
public:
	CGuiChatOutput(void);
	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void OnInitScript();
	void RenderBackground(float fElapsedTime);
	HRESULT OnRender(float fElapsedTime);
	void InitCustomElements();
	void AddMessage(const char* pszString,DWORD dwColor);
	void AddChatMessage(stChannelChatUserCmd*  pCmd);
	void OpenMediaExchangeHyperLink( const DWORD& dwID );
	void OnResetScreenSize();

	void UpdateMediaExchangeList();
	bool OnContextMenu();

	const char* GetPrivateName(){ return m_szPrivateName;}

	static void AddChatUserCmd(stChannelChatUserCmd*  pCmd);

	typedef std::map<int,bool>	tMapChatConfig;

private:
	void OnMenuCmd(UINT nID);
	void OnButtonClick(UINT nID);
	bool IsCanputType(int type,int index);

	tMapChatConfig	m_map1;
	tMapChatConfig	m_map2;
	DWORD			m_tBeginTime;
	DWORD			m_tBeginOutTime;
};


extern void  InsertPrivateChat(const char* name);