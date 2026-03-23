#include "public.h"
#include "miniusercommand.h"
#include "GameGuiManager.h"
#include "../Media/SoundManager.h"
#include ".\lordstrikeGameBkdlg.h"
#include "LordStrikeGame.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "LordStrikeCard.h"
#include "Country.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define ID_GAME_BK_BUTTON_CLOSE 1 

#define ID_PLAYER_SCROLLBAR  2 
#define ID_LISTBOX_PLAYER    8 


#define ID_BUTTON_PLAYICON       100
#define ID_STATIC_PLAYICON       1000
#define ID_STATIC_PREPARESTATIC  2000

 
// BUTTON ID
#define ID_BUTTON_PREPARE_BASE       200

// BUTTON POSITION 
#define ID_BUTTON_PREPARE_BASE_X     332
#define ID_BUTTON_Y     560



// BUTTON ID
#define ID_BUTTON_DISPENSE_CARD_BASE       300 
// BUTTON POSITION 
#define ID_BUTTON_DISPENSE_CARD_BASE_X     236 
#define ID_BUTTON_DISPENSE_CARD_BASE_GAP   85 


// BUTTON ID
#define ID_BUTTON_CONTEST_POINT_BASE       400
// BUTTON POSITION 
#define ID_BUTTON_CONTEST_POINT_BASE_X     236 
#define ID_BUTTON_CONTEST_POINT_BASE_GAP   85 


#define ID_BUTTON_HELP      32
#define ID_BUTTON_OPTION    33
#define ID_BUTTON_GIVEUP    34

CGameBackGoundDlg::CGameBackGoundDlg()
: m_bSortUP(false)
{
}

CGameBackGoundDlg::~CGameBackGoundDlg()
{
}

void CGameBackGoundDlg::OnCreate(void)
{
	CGuiLordStrikeBaseDlg::OnCreate();

	CGuiButton * pButn = GetButton(ID_BUTTON_OPTION);
	if(pButn)
	   pButn->SetEnabled(false);
}

void CGameBackGoundDlg::ConfirmEntryGame()
{
	ShowStartCardButtons(true);
}

// 一局游戏结束
void CGameBackGoundDlg::EndGame()
{
	ShowDispenseCardButtons(false);
	ShowStartCardButtons(true);
}


// 游戏开始
void CGameBackGoundDlg::StartGame()
{
	ShowContestPointButtons(false);
	ShowStartCardButtons(false);

	m_bSortUP = false;
}

// 禁桌，踢人
bool  CGameBackGoundDlg::OnTogglePlayerIcon(UINT nID)
{
	if(nID >= ID_BUTTON_PLAYICON)
	{
		int nButtonIndex = nID - ID_BUTTON_PLAYICON;
		if(nButtonIndex < 0 || nButtonIndex >= 3)  // 0  1 2 
			return false;

                int nDirection = nButtonIndex +1;

		stGameInfo & gameinfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();

 		int nseat = DirectionToSeat(nDirection,gameinfo.seatid.seat);
		if( nseat == gameinfo.seatid.seat)
		{// 不会甚至自己
                      return true;
		}

		std::list<MiniUserData *>::iterator iter = gameinfo.mListUserData.end();
		for(; iter != gameinfo.mListUserData.end();iter++)
		{
			MiniUserData * puserdata = *iter;
                        if(puserdata->pos.seat == nseat)
			{
				stKickUserCommonMiniGameCmd cmd;
				cmd.gameID = gameinfo.seatid;
				cmd.userID = puserdata->id;
				SEND_USER_CMD(cmd);
				return true;
			}
		}
		if( iter == gameinfo.mListUserData.end())
		{      //这个位置上没有人
			//打开/关闭某个位子
			for(int j = 0; j < gameinfo.mSeatList.size(); j++)
			{
				MiniSeatData * pseatdata = gameinfo.mSeatList[j];
				if(pseatdata->seatID.seat == nseat)
				{
					//BYTE state;//0关闭 1打开
					 stToggleSeatCommonMiniGameCmd cmd;
					 cmd.seatID = pseatdata->seatID;
					 cmd.open = !pseatdata->state;
					 SEND_USER_CMD(cmd);
					 return true;
				}
			}
			if(gameinfo.mSeatList.size() == j)
			{ // 还没有初始化的位置,默认打开
				stToggleSeatCommonMiniGameCmd cmd;
				cmd.seatID.type = gameinfo.seatid.type;
				cmd.seatID.room = gameinfo.seatid.room;
				cmd.seatID.game = gameinfo.seatid.game;
				cmd.seatID.seat = nseat;
				cmd.open = 0;
				SEND_USER_CMD(cmd);
				return true;
			}
		}
	}
	return false;
}

bool CGameBackGoundDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			if(OnTogglePlayerIcon(nID))
			{
                    break;
			}
			switch(nID)
			{
			case ID_BUTTON_PREPARE_BASE:
				{// 开始游戏
					stGameInfo & gameinfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
					stToggleReadyCommonMiniGameCmd cmd;
					cmd.gameID = gameinfo.seatid;
					SEND_USER_CMD(cmd);					
					CLordStrikeManager::GetSingleton()->SetGameState(eState_Preparing); // 到准备状态
					
					LSDlg_ButtonSetEnable(ID_BUTTON_PREPARE_BASE,false);
					LSDlg_ButtonSetEnableDelay(ID_BUTTON_PREPARE_BASE,true);

					return true;
				}
				break;
			case ID_GAME_BK_BUTTON_CLOSE:
				{
					eGameState __gamestate = CLordStrikeManager::GetSingleton()->GetGameState();
					if( __gamestate == eState_eStartGame || 
					    __gamestate == eState_DealCard   ||  // 发牌
					    __gamestate == eState_DispenseCard ||// 出牌
					    __gamestate == eState_ContestPoint)// 叫分
					{
						GetGameGuiManager()->AddMessageBox("如果你现在退出游戏，将被扣掉一定积分，你确定要这样做么?",eAction_ExitLordStrikeGameIng,NULL, c_szMBOKCancel);
					}
					else 
						GetGameGuiManager()->AddMessageBox("确定退出游戏吗?",eAction_ExitLordStrikeGame,NULL, c_szMBOKCancel);
				}
				break;
			////////////////////////////////////////////////////////////////////
			// 主角叫分的界面
			case ID_BUTTON_CONTEST_POINT_BASE:   // 1亩
			case ID_BUTTON_CONTEST_POINT_BASE+1: // 2亩
			case ID_BUTTON_CONTEST_POINT_BASE+2: // 3亩 
			case ID_BUTTON_CONTEST_POINT_BASE+3: // 不叫
				{
					stPointDDZMiniGameCmd cmd;
					stGameInfo & gameInfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
					cmd.userID = CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID;
					cmd.num = nID - ID_BUTTON_CONTEST_POINT_BASE + 1;
					if(cmd.num == 4)
						cmd.num = 0;
					cmd.gameID = gameInfo.gameid;
					SEND_USER_CMD(cmd);

					ShowContestPointButtons(false);
				}
				break;
			case ID_BUTTON_DISPENSE_CARD_BASE+1:// 不出
				{
					DWORD dwMyUserID = CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID;/*CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().GetMyUserID()*/
					CLordStrikeManager::GetSingleton()->GetGame()->SendNoDispenseCard(dwMyUserID);
					CLordStrikeManager::GetSingleton()->GetGame()->ClearSelectedState();
					EnableDispenseCardButton(false);
				}
				break;
			case ID_BUTTON_DISPENSE_CARD_BASE:// 出牌 
				{		
					CLordStrikeManager::GetSingleton()->GetGame()->SendSelectedCard();
				}
				break;
			case ID_BUTTON_DISPENSE_CARD_BASE+2:// 排序
				{
					m_bSortUP = !m_bSortUP;
					CLordStrikeManager::GetSingleton()->GetGame()->GetMainPlayer()->SortPlayingCard(m_bSortUP);
				}
				break;

			case ID_BUTTON_HELP:
				{
					CLordStrikeManager::GetSingleton()->AddGameHelpDlg();
				}
				break;
			case ID_BUTTON_OPTION:
				break;
			case ID_BUTTON_GIVEUP:
				OnGuiEvent(EVENT_BUTTON_CLICKED,ID_GAME_BK_BUTTON_CLOSE,NULL);
				break;
			}
		}
		break;
	}
	return CGuiLordStrikeBaseDlg::OnGuiEvent(nEvent,nID,pControl);
}

void CGameBackGoundDlg::OnClose()
{
	// 释放所有结构
	CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk = NULL;

	if(CLordStrikeManager::GetSingleton()->GetGame())
	{
		CLordStrikeManager::GetSingleton()->GetGame()->OnWindowCloseCallBack();
	}
	
}

bool CGameBackGoundDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		{
			if(CLordStrikeManager::GetSingleton()->GetGameState() == eState_DispenseCard)
			{
				stPointI ptCursor = Engine_GetCursor()->GetPosition();
				int x = ptCursor.x - GetClientX();
				int y = ptCursor.y - GetClientY();
				if(CLordStrikeManager::GetSingleton()->GetGame()->MsgProc(WM_LBUTTONDOWN,(WPARAM)x,(WPARAM)y))
					return true;			 
			}
		}
		break;
	}
	return  CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);
}

HRESULT CGameBackGoundDlg::OnRender( float fElapsedTime )
{	
	HRESULT hr =  CGuiLordStrikeBaseDlg::OnRender(fElapsedTime);

	CLordStrikeManager::GetSingleton()->GetGame()->Draw(GetClientX(),GetClientY());

	return hr;	
}

// 显示 开始发牌 按钮
void CGameBackGoundDlg::ShowStartCardButtons(bool bShow)
{
	int nBtnID = ID_BUTTON_PREPARE_BASE;
	CGuiButton * pButton = GetButton(nBtnID);
	if(!pButton)
	{
		if(!bShow)  	return;
		stPointI pt(0,0);
		AddButton(nBtnID , "开始" , pt.x , pt.y , 32 , 32 , 0 , false , &pButton );      
	}
	if(pButton)
	{
		pButton->SetLocation(ID_BUTTON_PREPARE_BASE_X,ID_BUTTON_Y);
		pButton->SetVisible(bShow);
		pButton->SetEnabled(bShow);
	}
}

// 显示 叫分 按钮
void CGameBackGoundDlg::ShowContestPointButtons(bool bShow)
{
	FUNCTION_BEGIN;

	for(int i = 0 ; i < 4 ; i++)  // 1亩  2亩 3亩 不叫 
	{
		int nBtnID = ID_BUTTON_CONTEST_POINT_BASE + i;
		CGuiButton * pButton = GetButton(nBtnID);
		if(!pButton)
		{
			if(!bShow)  return;
			stPointI pt(0,0);
			AddButton(nBtnID , "" , pt.x , pt.y , 32 , 32 , 0 , false , &pButton );      

		}
		assert(pButton != NULL);
		if(pButton)
		{
			pButton->SetLocation(ID_BUTTON_CONTEST_POINT_BASE_X + i%4 * ID_BUTTON_CONTEST_POINT_BASE_GAP,ID_BUTTON_Y);
			pButton->SetVisible(bShow);
			pButton->SetEnabled(bShow);
		}
	}
	if(bShow)
	{
		CGuiButton * pButton = GetButton(ID_BUTTON_CONTEST_POINT_BASE);
		if(pButton)	pButton->SetText("1分");

		pButton = GetButton(ID_BUTTON_CONTEST_POINT_BASE+1);
		if(pButton)	pButton->SetText("2分");

		pButton = GetButton(ID_BUTTON_CONTEST_POINT_BASE+2);
		if(pButton)     pButton->SetText("3分");

		pButton = GetButton(ID_BUTTON_CONTEST_POINT_BASE+3);
		if(pButton)	pButton->SetText("不叫");

	}

	FUNCTION_END;
}


// 游戏进行中每轮的发牌，不出，提示 按钮
void CGameBackGoundDlg::ShowDispenseCardButtons(bool bShow)
{
	FUNCTION_BEGIN;

	for(int i = 0 ; i < 3 ; i++)  // 不出  出牌 排序 
	{
		int nBtnID = ID_BUTTON_DISPENSE_CARD_BASE + i;
		CGuiButton * pButton = GetButton(nBtnID);
		if(!pButton)
		{
			if(!bShow) return;
			stPointI pt(0,0);
			AddButton(nBtnID , "" , pt.x , pt.y , 32 , 32 , 0 , false , &pButton );
		}
		assert(pButton != NULL);
		if(pButton)
		{
			pButton->SetLocation(ID_BUTTON_DISPENSE_CARD_BASE_X + i%3 * ID_BUTTON_DISPENSE_CARD_BASE_GAP,ID_BUTTON_Y);
			pButton->SetVisible(bShow);
			pButton->SetEnabled(bShow);
		}
	}
	if(bShow)
	{
		CGuiButton * pButton = GetButton(ID_BUTTON_DISPENSE_CARD_BASE);
		if(pButton) pButton->SetText("出牌");

		pButton = GetButton(ID_BUTTON_DISPENSE_CARD_BASE+1);
		if(pButton) pButton->SetText("不出");

		pButton = GetButton(ID_BUTTON_DISPENSE_CARD_BASE+2);
		if(pButton) pButton->SetText("排序");
	}
	
	FUNCTION_END;
}

void CGameBackGoundDlg::EnableDispenseCardButton(bool bCanPut)
{
	CGuiButton * pButton = GetButton(ID_BUTTON_DISPENSE_CARD_BASE);
	if(pButton)
		pButton->SetEnabled(bCanPut);
}

void CGameBackGoundDlg::EnableNoDispenseCardButton(bool bEnable)
{
	CGuiButton * pButton = GetButton(ID_BUTTON_DISPENSE_CARD_BASE+1);
	if(pButton)
		pButton->SetEnabled(bEnable);
}

///////////////////////////////////////////////////////////////////////////////////
// 通知某个方向的人发牌
void  CGameBackGoundDlg::NotifyPutCard(int nDirection)
{
	assert( nDirection>= 1 && nDirection <=3);	
        ShowDispenseCardButtons(true/*nDirection == 3*/ );
	EnableDispenseCardButton(false);
	
}

void CGameBackGoundDlg::EndSendNoDispenseCard()
{
	//ShowDispenseCardButtons(false);
}

// 通知某个方向的人叫分
void CGameBackGoundDlg::NotifyPointDDZ(int nDirection,int maxPoint)
{
	if(nDirection>=1)
	{
		if(nDirection == 2)
		{
			ShowContestPointButtons(true);
			for(int i = 0 ; i < maxPoint; i++)
			{
				int nBtnID = ID_BUTTON_CONTEST_POINT_BASE + i;
				CGuiButton * pBtn = GetButton(nBtnID);
				if(pBtn)
				{
					pBtn->SetEnabled(false);
				}
			}
		}
		else 
		{
			ShowContestPointButtons(false);
		}
	}
}

void CGameBackGoundDlg::SendChat(std::string strChat)
{
	stChatCommonMiniGameCmd cmd;
	cmd.userID = 0 ;// noused ?
	cmd.pos = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().seatid;
	cmd.type = MCT_NORMAL;
	_snprintf(cmd.content,64,"%s",strChat.c_str());
	cmd.content[63] = 0;
	SEND_USER_CMD(cmd);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// ?￠D?????
void CGameBackGoundDlg::RefreshPlayerIconButton()
{
	if(!IsVisible())
		return ;

	struct stMPoint 
	{
		int x,y;
	};
	const stMPoint ptPlayIconButton[3] =
	{
		{51,275},   /// left up 
		{138,645},
		{660,275},     // right up 
	};

	static stMPoint s_ptName[3] = 
	{
		{48,350},      // left up 
		{120,710},      // down 
		{725,350},      // right up 
	};
	static stMPoint s_ptPrepareStatic[3] = 
	{
		{195,285},      // left up 
		{380,490},           // down 
		{580,285},             // right up 
	};

	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");
	rl.group = 132;
	for(int j = 0; j < count_of(ptPlayIconButton); j++)
	{
		int nBtnID= ID_BUTTON_PLAYICON + j;
		CGuiButton * pButton = __AddButton(nBtnID);
		int nStaticID = ID_STATIC_PLAYICON + j;


		//CGuiStatic * pStatic = __AddStatic(nStaticID);
		//nStaticID =  ID_STATIC_PREPARESTATIC + j;



		CGuiStatic * pPreStatic = __AddStatic(nStaticID);

		int nDirection = j+1;
		stGameInfo & gameinfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
		int nSeat = DirectionToSeat(nDirection,gameinfo.seatid.seat);

		CLordStrikeManager::GetSingleton()->GetGame()->UpdatePlayerName(nDirection,"");

		std :: list<MiniUserData *>::iterator iter = gameinfo.mListUserData.begin();
		for(; iter != gameinfo.mListUserData.end();iter++)
		{
			MiniUserData * puser = *iter;
			if(puser->pos.seat == nSeat)
			{
				// role icon 
				rl.group = 33;
				rl.frame = puser->face;
				if(rl.frame <= 0 || rl.frame >= 29)
					rl.frame = 1;
				__SetButtonBitmap(pButton,rl,ptPlayIconButton[j].x,ptPlayIconButton[j].y,stPointF(1.0,1.0f));

				// role name 
		/*		pStatic->SetText(puser->name);
				pStatic->SetVisible(true);
				int nOldFont = pStatic->GetFont();
				int nFontWidth = GetDevice()->GetFontWidth(nOldFont);
				int len = nFontWidth * strlen(puser->name);
				pStatic->SetSize(len+2,32);
				if(nDirection == 1 || nDirection == 2)
				{
					pStatic->SetLocation(s_ptName[j].x,s_ptName[j].y);
				}
				else 
					pStatic->SetLocation(s_ptName[j].x-len,s_ptName[j].y);*/

				CLordStrikeManager::GetSingleton()->GetGame()->UpdatePlayerName(nDirection,puser->name);

				// Prepare static 
				if(puser->state == MUS_READY)
				{
					char szTip[40];
					strcpy(szTip,"准备");
					szTip[39] = 0;
					pPreStatic->SetTextColor(COLOR_ARGB(255,74,233,29));
					pPreStatic->SetFont(2);
					int nlen = GetDevice()->GetFontWidth(pPreStatic->GetFont()) * strlen(szTip);
					pPreStatic->SetSize(nlen+2,32);
					pPreStatic->SetText(szTip);
					pPreStatic->SetVisible(true);
		
					if(nDirection == 1)
					{// left to right
						pPreStatic->SetLocation(s_ptPrepareStatic[j].x,s_ptPrepareStatic[j].y);
					}
					else if(nDirection == 2)
					{
						pPreStatic->SetLocation(s_ptPrepareStatic[j].x - nlen/2,s_ptPrepareStatic[j].y);
					}
					else if(nDirection == 3)
					{
						pPreStatic->SetLocation(s_ptPrepareStatic[j].x - nlen,s_ptPrepareStatic[j].y);
					}
					if(nDirection == 2)
					{
						ShowStartCardButtons(false);
					}
				}				
				break;
			}
		}
		if( iter !=  gameinfo.mListUserData.end())
			continue;

		for( int i = 0 ; i < gameinfo.mSeatList.size(); i++)
		{
			MiniSeatData * pseat= gameinfo.mSeatList[i];

			if( pseat->seatID.seat == nSeat)
			{
				if(pseat->state == 0)
				{ // 1?±?×′ì?
					rl.group = 132;
					rl.frame = 42;// ???1
					__SetButtonBitmap(pButton,rl,ptPlayIconButton[j].x,ptPlayIconButton[j].y,stPointF(1.0,1.0f));
				}
				else
				{
					// ′ò?a×′ì?
					rl.frame = 40;// ?êo?,??óDè?μ?ê±oò
					rl.group = 132;
					__SetButtonBitmap(pButton,rl,ptPlayIconButton[j].x,ptPlayIconButton[j].y,stPointF(1.0,1.0f));
				}
				break;
			}
		}
		if(i == gameinfo.mSeatList.size())
		{
			// ′ò?a×′ì?
			rl.frame = 40;// ?êo?,??óDè?μ?ê±oò
			rl.group = 132;
			__SetButtonBitmap(pButton,rl,ptPlayIconButton[j].x,ptPlayIconButton[j].y,stPointF(1.0,1.0f));
		}
	}

	UpdateListBox();

}

void CGameBackGoundDlg::UpdatePlayerScore(stGameResultCommonMiniGameCmd * cmd)
{
	UpdateListBox();
}

void CGameBackGoundDlg::UpdateListBox()
{

	CGuiListBox * pListBox = GetListBox(ID_LISTBOX_PLAYER);
	assert(pListBox != NULL);
	if(!pListBox)
		return;

	pListBox->RemoveAllItems();
	stGameInfo & gameInfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();

	std::list<MiniUserData *>::iterator iter = gameInfo.mListUserData.begin();
	for(; iter != gameInfo.mListUserData.end(); ++iter)
	{
		const MiniUserData * pInfo = *iter;

		pListBox->AddItem("",NULL);
		int append_index = pListBox->GetItemCount()-1;

		std::string str = pInfo->name;
		std::string str2;
		if(str.length() > 10)
			str2 = str.substr(0,10) + "...";
		else 
			str2 = str;
		pListBox->SetItemText2(append_index,0,str2.c_str(),99,GuiListBoxColFmt_Left) ; // ??é???

		pListBox->SetItemText2(append_index,1,"",75,GuiListBoxColFmt_Center);  // μè??

		char szTest[40]={0};
		_snprintf(szTest,40,"%d",pInfo->score.score);
		szTest[39] = 0;
		pListBox->SetItemText2(append_index,2,szTest,100,GuiListBoxColFmt_Center);  /// ?y·?

		pListBox->SetItemText2(append_index,3,"",65,GuiListBoxColFmt_Center); ///????

		_snprintf(szTest,40,"%d",pInfo->score.win);
		szTest[39] = 0;
		pListBox->SetItemText2(append_index,4,szTest,46,GuiListBoxColFmt_Center); ///ê¤


		_snprintf(szTest,40,"%d",pInfo->score.lose);
		szTest[39] = 0;
		pListBox->SetItemText2(append_index,5,szTest,49,GuiListBoxColFmt_Center); ///?o

		_snprintf(szTest,40,"%d",pInfo->score.draw);
		szTest[39] = 0;
		pListBox->SetItemText2(append_index,6,szTest,52,GuiListBoxColFmt_Center); ///oí

		char szOut[80] = {0};
		if(!CCountry::Instance()->GetCountryName(pInfo->countryID,szOut))
			szOut[0] = 0;
		pListBox->SetItemText2(append_index,7,szOut,70,GuiListBoxColFmt_Center); ///1ú?ò

		pListBox->SetItemText2(append_index,8,"",119,GuiListBoxColFmt_Center); ///1ù?°
	}	
}

