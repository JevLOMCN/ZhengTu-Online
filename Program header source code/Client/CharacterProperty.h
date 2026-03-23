/**
 *\file		CharacterProperty.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:10:00 中国标准时间
 *\brief	人物状态绘制
 *
 * 
 *
*/

#pragma once
#include "..\gui\include\guidialog.h"

class CGuiMenu;
class CCharacter;
class CMainCharacter;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiCharacterProperty :
	public CGuiDialog
{
	///是否需要显示详细信息
	bool m_bShowDetail;
	///激活的菜单命令，已不用
	int	m_activeCMD;
	///提示，已不用
	CToolTips*	m_pDetail;
	///命令菜单
	CGuiMenu*	m_menu;

	CGuiMenu*	m_CartoonPetMenu;

	CToolTips	m_AllTips; //sky 气泡
	enum enumBarType{
		BarTypeVert,
		BarTypeHoriz,
	};

	struct stBarShowInfo{
		stRectI rcClip;
		enumBarType type; 
		IBitmap* pBmp;
	};
	std::vector<stBarShowInfo> m_aBar;
	void RenderBar(CGuiCharacterProperty::stBarShowInfo& bar,uint64_t iCur,uint64_t iMax,int index);
	BOOL ContainsPoint(POINT  ptScreen);
	

	bool CanFollow();
	bool CanChatPrivate();
	bool CanExchange();
	bool CanMakeGroupNormal();
	bool CanMakeGroupHonor();
	bool CanLookAt();
	bool CanFight();
	bool CanRequestShop();


	stResourceLocation rlTeam;
	IBitmaps * m_pTeamHeadIcon;

	HRESULT RenderTeamFlag();
	HRESULT RenderLevel();
	HRESULT RenderDetail();
	void RefreshMenuStatus();

public:

	CGuiCharacterProperty(void);
	void OnClose(void);
	void OnCreate();
	void OnInitScript();
	void OnResetScreenSize();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool OnContextMenu();
	
	void InitIcon();
	void InitDetail();
	void UpdateDetail();
	bool CanTreateCMD();
	int GetActiveCMD() { return m_activeCMD; }
	HRESULT OnRender( float fElapsedTime );
	void SetName( const char* pName, DWORD dwColor = 0xffffffff );

	void CopyNameToClipboard();
};


bool CanMakeGroupWithMe(int iLevel,int iMode);
