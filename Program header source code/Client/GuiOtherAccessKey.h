/**
 *\file		GuiOtherAccessKey.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:13:00 中国标准时间
 *\brief	浮动快捷键界面
 *
 * 
 *
*/

#pragma once
#include "..\gui\include\guidialog.h"

#define OTHER_ACCESS_KEY_GROUP	254
#define OTHER_ACCESS_KEY_NUM	6
#define EMOTION_ACCESS_KEY_GROUP 100


extern int g_nOtherAccessBtnLeft;
extern int g_nOtherAccessBtnTop ;	
extern int g_OtherAccessBtnSpace ;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiOtherAccessKey :
	public CGuiDialog
{
public:
	///快捷键按钮
	CGuiButton* m_pBtnAccessKey[OTHER_ACCESS_KEY_NUM];
	///快捷键tips
	CToolTips* m_activeSkillToolTips;
	///鼠标指向的快捷键
	int m_mouseOverAccessKeyNo;
	bool m_bPretendedHide;
public:
	CGuiOtherAccessKey(void);
	void OnClose(void);
	void OnCreate();
	void OnInitScript();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	void ResetDlgLocation();
	void OnResetScreenSize();
	HRESULT OnRender( float fElapsedTime );
	HRESULT RenderAccessKeyNotes(float fElapsedTime);
	HRESULT RenderAccessKeyBtnToolTips(float fElapsedTime);
	HRESULT RenderAccessKeyBtnDisableStatus(float fElapsedTime);

	//Access key about
	int  GetMouseOverAccessKeyNo();
	int GetCurAccessKeyNo() { return m_mouseOverAccessKeyNo; }
	void UpdateKeyTooltips( const BYTE& btnID );
	void RemoveAccessKeyBtnInfo( BYTE key );
	void DeleteMouseOverAccessKey();
	void DeleteAccessKey( BYTE key );
	void RefreshKeyGroupState();
	bool SetAccessKey( const int& keyID );
	void OnAccessBtnSelectSkill( const int& btnID, const int& skillId );
	bool OnFindAccessSkill();
	void EditAccessKeyBtn(const DWORD& id);

	void __SetEmotionAccessBtn(stResourceLocation & rl,int keyID);
};