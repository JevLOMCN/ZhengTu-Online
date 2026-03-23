#pragma once


#include "LordStrikeBaseDlg.h"

class CGameBackGoundDlg : public CGuiLordStrikeBaseDlg
{
public:
	CGameBackGoundDlg();
	~CGameBackGoundDlg();

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void ShowStartCardButtons(bool bShow);
	void ShowContestPointButtons(bool bShow);
	void ShowDispenseCardButtons(bool bShow);

	void RefreshPlayerIconButton();
	
	void ConfirmEntryGame();
        /////////////////////////////////////////////////////////////
	void StartGame(); // 真正的开始游戏消息
	void EndGame();
	 /////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
	//  网络消息
	// 通知某个方向的人出牌
	void NotifyPutCard(int  nDirection);

	//通知某人叫分
	void NotifyPointDDZ(int nDirection,int maxPoint);
	
	void SendChat(std::string strChat);
	
	///////////////////////////////////////////////////////////////
	// 更新左上角的主角信息
	void UpdatePlayerScore(stGameResultCommonMiniGameCmd * cmd);

	void EnableDispenseCardButton(bool bCanPut);
	void EnableNoDispenseCardButton(bool bEnable);

	void EndSendNoDispenseCard();
private:
	bool  OnTogglePlayerIcon(UINT nID);
	void  UpdateListBox();


	bool m_bSortUP;


};

