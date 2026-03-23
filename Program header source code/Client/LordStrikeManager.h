#pragma once

class CLordStrikeManager;
class CLobby;
class CGame;

class CGuiLordStrikeLobbyDlg;
class CGuiLordStrikeRoomDlg;
class CGameBackGoundDlg;

class CGuiLSScoreDlg;
class CGuiLordStrikeSortDlg;
class CGuiLSChangeMoneyDlg;
class CGuiGameHelpDlg;

enum eGameState
{
	eState_Lobby = 1,               // 在大厅里面
	eState_Room   = 2,                // 房间中
	eState_Preparing  = 3,           // 准备
	eState_eStartGame = 4,
	eState_DealCard  = 5,         // 发牌
	eState_DispenseCard = 6, // 出牌
	eState_ContestPoint = 7, // 叫分
	eState_ExposureCard = 8,        // 亮牌
	eState_Playing = 9,      // 出牌中
	eState_Waiting = 10,             // 等待中(这个状态就是一直等,直到有新状态设置)
	eState_EndGame = 11,                 // 游戏结束 
};


struct stMainUserInfo
{
	stMainUserInfo()
	{
		dwMainUserID = 0;
	}
	DWORD        dwMainUserID;  // 不能从rpg游戏中取
	std::string  strRoleName;
	int          nlevel;
	MiniUserData   userdata;
};

class CLordStrikeManager
{
public:
	CLordStrikeManager(void);
	~CLordStrikeManager(void);

	////////////////////////////////////////////////////////////////////
	bool IsLordStrikeMusicOpen();

	/////////////////////////////////////////////////////////////////////
	// singleton
	static CLordStrikeManager * GetSingleton()
	{
		assert(m_singleton != NULL);
		return m_singleton;
	}
	static CLordStrikeManager * m_singleton;
	/////////////////////////////////////////////////////////////////////

       
	void SetMainUserInfo(int level,std::string strRoleName)
	{
	      m_MainUserInfo.strRoleName=  strRoleName;
	      m_MainUserInfo.nlevel = level;
	}
	stMainUserInfo & GetMainUserInfo()
	{
		return m_MainUserInfo;
	}

	/////////////////////////////////////////////////////////////////////
	/// 进出大厅
	CLobby * GetLobby()
	{
		return m_pLobby;
	}

	void EntryLobby(stLoginRetCommonMiniGameCmd * pCmd);
	void LeaveLobby();

	/////////////////////////////////////////////////////////////////////
	CGame * GetGame()
	{
		return m_pGame;
	}
	void SetGameState(eGameState state)
	{
		m_eGameState = state;
	}
	eGameState GetGameState()
	{
		return m_eGameState;
	}
	//////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////
	CGuiLordStrikeLobbyDlg * AddLordStrikeLobbyDlg();
	CGuiLordStrikeRoomDlg *  AddLordStrikeRoomDlg();
	CGameBackGoundDlg *      AddLordStrikeGameBkDlg();
	CGuiLSScoreDlg *         AddLordStrikeLSScoreDlg();
	CGuiLordStrikeSortDlg * AddLordStrikeSortDlg();
	CGuiLSChangeMoneyDlg * AddLordStrikeChangeMoneyDlg();

	CGuiGameHelpDlg * AddGameHelpDlg();

	CGuiLordStrikeLobbyDlg * m_guiLordStrikeLobby;
	CGuiLordStrikeRoomDlg  * m_guiLordStrikeRoom;
	CGameBackGoundDlg * m_guiLordStrikeGameBk;
	CGuiLSScoreDlg  *  m_guiLSScoreDlg;
	CGuiLordStrikeSortDlg * m_guiSortDlg;

	CGuiLSChangeMoneyDlg * m_guiLSChangeMoneyDlg;
	CGuiGameHelpDlg * m_pGameHelpDlg;
        
	///////////////////////////////////////////////////
	void OnUpdate();
        ///////////////////////////////////////////////////

	bool  IsInLordStrike();
	void  SendChat();

	void LogintoLobby();
	///////////////////////////////////////////////////////////////////////


	void UpdateMainUserInfo(stUserDataCommonMiniGameCmd * pCmd);


	void UpdateHideBtn();
	bool OnReOpenLSDialog(UINT nID);
	void ShowAllDialog(bool bShow);

private:
	CLobby  *m_pLobby;
        CGame   *m_pGame;

	eGameState  m_eGameState;
	stMainUserInfo  m_MainUserInfo;

};

#define LSDlg_ButtonSetEnable(id,bEnable)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetEnabled(bEnable);}}
#define LSDlg_ButtonSetEnableDelay(id,bEnable)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetEnableDelay(bEnable,5500);}}
