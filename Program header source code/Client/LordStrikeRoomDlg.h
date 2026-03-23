#pragma once


#include "LordStrikeBaseDlg.h"

#define ID_COL_TABLE_NUM     2 
#define ID_ROW_TABLE_NUM     3


class CGuiLordStrikeRoomDlg : public CGuiLordStrikeBaseDlg
{
public:
	CGuiLordStrikeRoomDlg();
	~CGuiLordStrikeRoomDlg();

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	bool MsgProc(HWND hWnd,UINT uMsg,WPARAM wParam, LPARAM lParam);

	void SendChat(std::string strChat);
	 
	void UpdateTable();
	void NewTable(int nMaxTable);

	void UpdatePlayerScore(stGameResultCommonMiniGameCmd * cmd);

private:
	

	// table 
	void InitTable();
	void ClearAllTable();

        //scroll bar 
	void    InitScrollBar();
	void	UpdateSrollRects();
	CGuiScrollBar*	      m_pLeftVertScrollBar;
	
	void InitTableButtons();
	void ShowTableButtons(bool bShow);
	void UpdateTableButtons();
	void OnTableButtonClick(UINT nID);

	void UpdateListBox();


	struct stButtonInfo
	{     
		CGuiButton * pDownButton;
		CGuiStatic * pDownStatic;
		CGuiImage  * pDownImage;
		CGuiImage * pDownPrepareImage;
		

		CGuiButton * pTableButton;
		CGuiStatic * pTableIDStatic;

		CGuiButton * pLeftButton1;
		CGuiStatic * pLeftStatic1;
		CGuiImage * pLeftImage1;
	        CGuiImage * pLeftPrepareImage1;

	 
		CGuiButton * pRightButton1;
		CGuiStatic * pRightStatic1;
		CGuiImage * pRightImage1;
		CGuiImage * pRightPrepareImage1;


		stButtonInfo()
		{
			pDownButton = NULL;	pDownStatic = NULL;	pDownImage = NULL;  pDownPrepareImage = NULL;
			pTableButton = NULL;	pTableIDStatic = NULL;	
			pLeftButton1 = NULL;pLeftStatic1 = NULL;pLeftImage1 = NULL;pLeftPrepareImage1 = NULL;
			pRightButton1 = NULL;	pRightStatic1 = NULL;pRightImage1= NULL;pRightPrepareImage1= NULL;
		}
	};

	stButtonInfo m_ButtonInfo[ID_COL_TABLE_NUM * ID_ROW_TABLE_NUM];
	struct stTableInfo
	{
		DWORD tableid;// 桌子索引 0~max
		std::vector<MiniUserData *> VecUserData;  // 桌子上的玩家列表
		std::vector<MiniSeatData *> vecSeat;
                
		bool IsSeatClosed(BYTE seat)
		{
			for(int k = 0 ; k < vecSeat.size(); k++)
			{
				MiniSeatData * pdata = vecSeat[k];
				if( pdata->seatID.seat == seat)
				{
					if(pdata->state==1)//0关闭 1打开
						return false;
					else 
						return true;
				}
			}
			return false;
		}
		bool IsTableStartState()
		{
			for(int k = 0 ; k < VecUserData.size(); ++k)
			{
				const MiniUserData * ptemp = VecUserData[k];
				if(ptemp->state == MUS_PLAY)
				{
					return true;
				}
			}
			return false;
		}
// 为了方便规定seat左上角为0,逆时针转累加
		int GetLeftIndex(){ 
			return GetIndex(1);
		}

		int GetDownIndex(){ 
			return GetIndex(2);
		}
		int GetRightIndex(){ 
			return GetIndex(3);
		}
	
		int GetIndex(int nSeat){
			for(int k = 0 ; k < VecUserData.size(); ++k)
			{
				MiniUserData * ptemp = VecUserData[k];
				if(ptemp->pos.seat ==nSeat)
				{
					return k;
				}
			}
			return -1;
		}
		MiniUserState GetUserState(int seat )
		{
			for(int k = 0 ; k < VecUserData.size(); ++k)
			{
				MiniUserData * ptemp = VecUserData[k];
				if(ptemp->pos.seat ==seat )
				{
					return (MiniUserState)ptemp->state;
				}
			}
			return MUS_NOTPLAY;
		}
		

	};
	typedef std::vector<stTableInfo>            tVecTable;
	tVecTable m_VecTable;        // 桌子id信息

	void OnLeftMouseWheel();


	bool LoginGame(UINT nControlID);

	

public:
	void UpdateSeatState(const MiniSeatData & seat);

	DWORD m_dwCreateTime;
	BOOL  m_bOrderReset;


};
 