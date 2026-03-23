#include "public.h"
#include "miniusercommand.h"
#include "GameGuiManager.h"
#include "../Media/SoundManager.h"
#include "./Game.h"
#include ".\LordStrikeManager.h"
#include ".\LordStrikeLobby.h"
#include ".\lordstrikegame.h"
#include ".\LordStrikeGameBkDlg.h"
#include "cardpattern.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
const int  c_gap_card = 16;

#define CARD_WIDTH	68
#define CARD_HEIGHT     98
#define REVERSE_CARD_GAP	10 
const int c_up_card = 20;


#define PT_CENTER_X 340
#define PT_CENTER_Y 357

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct stMPoint
{
	int x,y;
};
const stMPoint c_PtPlayerCard[3] =
{
	 {195,234},  
	 {380,437},
	 {580,234},
};
 
const stMPoint c_PtPlayingCard[3] =
{
	{120,300},  
	{258,610},
	{650,300},
};

//const stMPoint c_PtCardNum[6] =
//{
//	{80,147},    
//	{133,580},
//	{690,147},   
//	 
//};
const stMPoint c_PtHeadCard[3] =
{
	{51,275},      
	{138,645}, 
	{660,275},
};

static stMPoint s_ptText[3] =
{
	{195,285},
	{380,490},    
	{580,285},
};


static stMPoint s_ptName[3] = 
{
	{48,350},      // left up 
	{120,710},      // down 
	{725,350},      // right up 
};

static stMPoint s_ptTimer[3] = 
{
	{50,380},      // left up 
	{100,550},           // down 
	{670,380},             // right up 

};

static stMPoint s_ptTimerNum[3] = 
{
	{50,430},      // left up 
	{150,550},           // down 
	{670,430},             // right up 
};


static stMPoint s_ptLord[3] = 
{
	{55,180},      // left up 
	{50,630},           // down 
	{656,180},             // right up 
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define REVERSE_CARD_CENTER_X		385
#define REVERSE_CARD_CENTER_Y		50

#define ID_LAND_INFO_X  112 //亩数，地皮数
#define ID_LAND_INFO_Y  106
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#define ID_CONSTEST_TIMER    30000
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void stDDZContestPoint::Draw(int nOffsetX,int nOffsetY)
{
	eGameState _state = CLordStrikeManager::GetSingleton()->GetGameState();
	if(_state == eState_ContestPoint)
	{
		if(m_nCurContestDir>=1)
		{
			int iFont = GetDevice()->SetFont(2);			
			// Draw ContestPoint Tip 
			stMPoint pt = s_ptText[m_nCurContestDir-1];
			pt.x += nOffsetX;
			pt.y += nOffsetY;
			
			
			char strTime[40] = {0};
			strcpy(strTime,"叫分");
			strTime[39] = 0;

			stRectI rcMsg;
			
			rcMsg.top = pt.y;
			rcMsg.bottom = rcMsg.top + GetDevice()->GetFontHeight(2) + 2;
			int nlen = strlen(strTime) * GetDevice()->GetFontWidth(2) ;
			if(m_nCurContestDir==3)
			{
				rcMsg.right = pt.x;
				rcMsg.left = rcMsg.right - nlen -2 ;
			}
			else if(m_nCurContestDir == 2)
			{
				rcMsg.left = pt.x - nlen/2;
				rcMsg.right = rcMsg.left + nlen + 2;
			}
			else 
			{
				rcMsg.left = pt.x;
				rcMsg.right = rcMsg.left + nlen + 2;
			}
				

			std::string strText = strTime;
			GetDevice()->DrawString(strText.c_str(),strText.length(),rcMsg,COLOR_ARGB(255,74,233,29),
				DT_VCENTER,
				FontEffect_Border);
		//	GetDevice()->SetFont(iFont);	

                        // Draw Timer Animation 
			pt = s_ptTimer[m_nCurContestDir-1];
			pt.x += nOffsetX;
			pt.y += nOffsetY;
			m_AniTimer.Render(pt.x,pt.y);

			// Draw Timer  Num 
			pt = s_ptTimerNum[m_nCurContestDir-1];
			pt.x += nOffsetX;
			pt.y += nOffsetY;

			stRectI rcMsg1(pt.x,pt.y,pt.x+30,pt.y+20);
			 
			_snprintf(strTime,40,"%d",(ID_CONSTEST_TIMER - (xtimeGetTime() - m_dwStartContestTime))/1000);
			strTime[39] = 0;
			strText = strTime;
			GetDevice()->DrawString(strText.c_str(),strText.length(),rcMsg1,COLOR_ARGB(255,74,233,29),
				DT_LEFT|DT_VCENTER,
				FontEffect_Border);
			GetDevice()->SetFont(iFont);	

		}
		if(m_nLastOKContestDir >= 1 )
		{
			////////////////////////////////////////////////////////////
			stMPoint pt = s_ptText[m_nLastOKContestDir-1];
			pt.x += nOffsetX;
			pt.y += nOffsetY;
			char strTime[40] = {0};
			if(m_nLastOKContestNum == 0 )
			{
				strcpy(strTime,"不叫");
			}
			else if(m_nLastOKContestNum == 1)
			{
				strcpy(strTime,"1分");
			}
			else if(m_nLastOKContestNum == 2)
			{
				strcpy(strTime,"2分");
			}
			else if(m_nLastOKContestNum == 3)
			{
				strcpy(strTime,"3分");
			}
			strTime[39] = 0;
			std::string strText = strTime;
			int iFont = GetDevice()->SetFont(2);

			stRectI rcMsg;
	
			rcMsg.top = pt.y;
			rcMsg.bottom = rcMsg.top + GetDevice()->GetFontHeight(2) + 2;

			int nlen = strlen(strTime) * GetDevice()->GetFontWidth(2) ;

			if(m_nLastOKContestDir==3)
			{
				rcMsg.right = pt.x;
				rcMsg.left = rcMsg.right - nlen -2 ;
			}
			else if(m_nCurContestDir == 2)
			{
				rcMsg.left = pt.x - nlen/2;
				rcMsg.right = rcMsg.left + nlen + 2;
			}
			else 
			{
				rcMsg.left = pt.x;
				rcMsg.right = rcMsg.left + nlen + 2;
			}

			GetDevice()->DrawString(strText.c_str(),strText.length(),rcMsg,COLOR_ARGB(255,74,233,29),
			DT_LEFT|DT_VCENTER,
			FontEffect_Border);
			GetDevice()->SetFont(iFont);
			////////////////////////////////////////////////////////////
		}
	}
	if(_state == eState_DispenseCard)
	{
		if(m_nDirOfLord >=1)
		{	//地主
			stMPoint pt = s_ptLord[m_nDirOfLord-1];
			pt.x += nOffsetX;
			pt.y += nOffsetY;
			m_AniLorder.Render(pt.x,pt.y);


			int iFont = GetDevice()->SetFont(1);
			pt.x  = nOffsetX + ID_LAND_INFO_X;
			pt.y  = nOffsetY + ID_LAND_INFO_Y;
		
			stRectI rcMsg(pt.x,pt.y,pt.x + 150,pt.y+20);
			char strTime[40] = {0};
			_snprintf(strTime,40,"底分:%d,倍数:%d",m_nLandNum,m_nBy);
			strTime[39] = 0;

			std::string strText = strTime;
			GetDevice()->DrawString(strText.c_str(),strText.length(),rcMsg,COLOR_ARGB(255,74,233,29),
				DT_VCENTER,
				FontEffect_Border);
			GetDevice()->SetFont(iFont);	

		}		
	}

	
}
void stDDZContestPoint::DDZPoint(stPointDDZMiniGameCmd * cmd)
{
	m_nCurContestDir = 0;
	//GetSoundManager()->Stop(m_timersound);

	stGameInfo & gameinfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
	BYTE seat = gameinfo.GetGameUserSeat(cmd->userID);
	int nDirection = 0;
	if(seat)
	{	
		nDirection = SeatToDirection(seat,gameinfo.seatid.seat);
		assert( nDirection>= 1 && nDirection <=3);
		m_nLastOKContestNum = cmd->num;
		m_nLastOKContestDir = nDirection;
	}	

	//if(cmd->num  > 0)
	//{
	m_mapDDZPoints[cmd->userID] = cmd->num;
	//}
	
	bool bLorder = false;
	if(m_mapDDZPoints[cmd->userID] >= 3 ){
		bLorder = true;
	}
	else
	{
		if(m_mapDDZPoints.size() >= gameinfo.GetUserSize())
		{
			DWORD dwMaxUserID = GetMaxPointUserID();
			seat = gameinfo.GetGameUserSeat(dwMaxUserID);
			if(seat)
			{	
				nDirection = SeatToDirection(seat,gameinfo.seatid.seat);
				assert( nDirection>= 1 && nDirection <=3);
				bLorder = true;
			}			
		}
	}
	if(bLorder)
	{
		if(nDirection)
		{	
			m_nDirOfLord = nDirection;
			m_nLandNum = cmd->num;
			m_nBy = 1;
		}
	}
}

void stDDZContestPoint::Create()
{
	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");
	rl.group = 133;
	rl.frame = 2;
	m_AniLorder.Create(&rl);

	rl.group = 133;
	rl.frame = 1;
	m_AniTimer.Create(&rl);

	m_nCurContestDir = 0;
	m_nDirOfLord = 0;

}

void stDDZContestPoint::StartTimer(int nDirection )
{
	m_nCurContestDir = nDirection;
	m_dwStartContestTime = xtimeGetTime();
	//m_timersound = GetSoundManager()->Play("sound\\ui\\91006.wav",SoundType_UI,true);
}

void stDDZContestPoint::OnContestTimer()
{
	if(CLordStrikeManager::GetSingleton()->GetGameState() == eState_ContestPoint)
	{
		if( xtimeGetTime()-m_dwStartContestTime >= ID_CONSTEST_TIMER && m_nCurContestDir >= 1)  // 40 second
		{
			PlayUISound(91007);
			if( m_nCurContestDir == 2)
			{
				CLordStrikeManager::GetSingleton()->GetGame()->SendNoContest();	
			}	
			m_dwStartContestTime = xtimeGetTime() + 0xffffff;
			m_nCurContestDir= 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
#define ID_PUTCHAR_TIMER  30000
///////////////////////////////////////////////////////////////////////////////////////////
void  stPutCardTimer::Create()
{
	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");
	rl.group = 133;
	rl.frame = 1;
	m_AniTimer.Create(&rl);

	m_dwLastCardDir = 0 ;
	m_dwLastPoint = 0 ;
}

void stPutCardTimer::StartTimer(DWORD dwUserID)//开始计时，到40秒后通知
{
	if(CLordStrikeManager::GetSingleton()->GetGameState() == eState_DispenseCard)
	{
		m_dwCurPutCardUserID = dwUserID;
		if( m_dwLastPutCardUserID == 0)
			m_dwLastPutCardUserID = m_dwCurPutCardUserID;
		m_dwStartPutCardTime = xtimeGetTime();
		stGameInfo & gameinfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
		BYTE seat = gameinfo.GetGameUserSeat(dwUserID);
		if(seat)
		{		
			int nDirection = SeatToDirection(seat,gameinfo.seatid.seat);
			m_nCurTimerDir = nDirection;
		        //if(nDirection == 2) 
			//   m_timersound = GetSoundManager()->Play("sound\\ui\\91006.wav",SoundType_UI,true);
		}
	}		
}

void  stPutCardTimer::OnPutCardTimer()
{
	if(CLordStrikeManager::GetSingleton()->GetGameState() == eState_DispenseCard)
	{
		if( xtimeGetTime()-m_dwStartPutCardTime >= ID_PUTCHAR_TIMER && m_nCurTimerDir >= 1)  // 40 second
		{
			PlayUISound(91007);
			if(CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID/*CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().GetMyUserID()*/ == m_dwCurPutCardUserID)
			{
				if(m_dwLastPutCardUserID == m_dwCurPutCardUserID)
				{// 上一个用户就是他,必须出牌,取最后一张,系统帮他出,这个他就是主角
					BYTE seat = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().GetGameUserSeat(m_dwCurPutCardUserID);
					if(seat)
					{
						stGameInfo & gameinfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
						int nDirection = SeatToDirection(seat,gameinfo.seatid.seat);
						if(nDirection == 2)
						{
							CCard *  nCard = CLordStrikeManager::GetSingleton()->GetGame()->GetMainPlayer()->GetLastPlayingCard();
							std::vector<CCard *> ccardlist;
							ccardlist.push_back(nCard);
							CLordStrikeManager::GetSingleton()->GetGame()->SendCard(ccardlist);	
						}
					}
				} 
				else 
				{  // 可以不出牌，系统自动发送不出牌消息
					CLordStrikeManager::GetSingleton()->GetGame()->SendNoDispenseCard(m_dwCurPutCardUserID);	
				}
				CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->EndSendNoDispenseCard();
			}	
			m_dwStartPutCardTime = xtimeGetTime() + 0xffff;
			m_nCurTimerDir = 0;
		}
	}
}
void  stPutCardTimer::SetLastPutCardUserID(DWORD dwUserID)
{
	m_dwLastPutCardUserID = dwUserID;
	m_nCurTimerDir = 0;
	//StopSound();
}

void stPutCardTimer::StopSound()
{
	/*if( m_timersound != 0)
	{
		GetSoundManager()->Stop(m_timersound);
		m_timersound = 0;
	}*/
}


void stPutCardTimer::SetLastCardDir(int nDirection,int nPoint)
{
	m_dwLastCardDir = nDirection;
	m_dwLastPoint = nPoint;
}

void stPutCardTimer::Draw(int offsetx,int offsety)
{
	if(CLordStrikeManager::GetSingleton()->GetGameState() == eState_DispenseCard)
	{
		if(m_nCurTimerDir >=1)
		{
			stMPoint pt = s_ptTimer[m_nCurTimerDir-1];
			pt.x += offsetx;
			pt.y += offsety;
			m_AniTimer.Render(pt.x,pt.y);

			int iFont = GetDevice()->SetFont(2);
			pt = s_ptTimerNum[m_nCurTimerDir-1];
			pt.x += offsetx;
			pt.y += offsety;
			stRectI rcMsg(pt.x,pt.y,pt.x+30,pt.y+20);

			char strTime[40] = {0};
			_snprintf(strTime,40,"%d",(ID_PUTCHAR_TIMER - (xtimeGetTime() - m_dwStartPutCardTime))/1000);
			strTime[39] = 0;
			std::string strText = strTime;
			GetDevice()->DrawString(strText.c_str(),strText.length(),rcMsg,COLOR_ARGB(255,74,233,29),
				DT_VCENTER,
				FontEffect_Border);
			GetDevice()->SetFont(iFont);
		}
		if( m_dwLastCardDir && m_dwLastPoint == 0)
		{ // 不出牌提示
			char strTime[40] = {0};
			strcpy(strTime,"不出牌");
			strTime[39] = 0;

			stMPoint pt = s_ptText[m_dwLastCardDir-1];
			pt.x += offsetx;
			pt.y += offsety;
			int iFont = GetDevice()->SetFont(2);
			stRectI rcMsg;
			

			rcMsg.top = pt.y;
			rcMsg.bottom = rcMsg.top + GetDevice()->GetFontHeight(2) + 2;
 
			int nlen = strlen(strTime) * GetDevice()->GetFontWidth(2) ;
		 

			if( m_dwLastCardDir == 3)
			{
				rcMsg.right = pt.x;
				rcMsg.left = rcMsg.right - nlen -2 ; 
			}
			else 
			{
				rcMsg.left = pt.x; 
				rcMsg.right = rcMsg.left + nlen + 2;
			}
			
			std::string strText = strTime;
			GetDevice()->DrawString(strText.c_str(),strText.length(),rcMsg,COLOR_ARGB(255,74,233,29),
				DT_VCENTER,
				FontEffect_Border);
			GetDevice()->SetFont(iFont);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void stReverseCardInfo::SetReverseCards(stReserveCardsDDZMiniGameCmd *cmd)
{
	dwOwnerID= cmd->userID;
	ClearReverseCards();
	// 显示底牌
	for(int i = 0; i < cmd->num; i++)
	{
		CCard * pCard = new CCard;
		int nCard = CardToInt(cmd->cards[i]);
		pCard->SetCardID(nCard);
		m_reverseCards.push_back(pCard);
	}
	dwStartTime = xtimeGetTime();
}
void stReverseCardInfo::ClearReverseCards()
{
	std::vector<CCard *>::iterator iter = m_reverseCards.begin();
	for( ; iter != m_reverseCards.end() ; ++iter)
	{
		CCard * pCard = *iter;
		delete pCard;
	}
	m_reverseCards.clear();
}

void stReverseCardInfo::Draw(int nOffsetX,int nOffsetY)
{
	if(m_reverseCards.size() <=0)
		return;
	if( xtimeGetTime() - dwStartTime >= 20000 )
	{
		ClearReverseCards();
		return;
	}
	int nStartx = nOffsetX + REVERSE_CARD_CENTER_X;
	int nCards=  m_reverseCards.size();
	nStartx -= (nCards/2) * (REVERSE_CARD_GAP+CARD_WIDTH);
	if( 0 == nCards%2)
		nStartx += REVERSE_CARD_GAP/2;
	else 
		nStartx -= CARD_WIDTH/2;
	std::vector<CCard *>::iterator iter = m_reverseCards.begin();
	for( int i = 0; i < m_reverseCards.size() ; ++i)
	{
		CCard * pCard = m_reverseCards[i];
		if(pCard)
			pCard->Render(nStartx + i*(REVERSE_CARD_GAP+CARD_WIDTH),nOffsetY+REVERSE_CARD_CENTER_Y);
	}
}

//////////////////////////////////////////////////////////////////////////////
void stEndTimer::EndTimer()
{
	if(m_bEndGameed)
	{
		m_bEndGameed = false;
		m_bWin = false; 
		m_bLorder = false;
		CLordStrikeManager::GetSingleton()->GetGame()->ClearPlayer();
		CLordStrikeManager::GetSingleton()->SetGameState(eState_Preparing); // 回到准备状态
	}	
}

void stEndTimer::StartTimer()
{
	m_bEndGameed = true;
	m_dwStartEndTimer = xtimeGetTime();
	if(m_bWin)
	{
		m_AniSuccess.SetStop(false);
		m_AniSuccess.SetFrame(0);
	}
	else 
	{
		m_AniFail.SetStop(false);
		m_AniFail.SetFrame(0);
	}
}

void stEndTimer::Stop()
{
	m_bEndGameed =false;
	m_bWin = false;
}

void stEndTimer::OnTimer()
{
	if ( m_bEndGameed && (xtimeGetTime() -  m_dwStartEndTimer) >= 5000 )
	{
		EndTimer();
	}
}

void stEndTimer::Draw(int nOffsetX,int nOffsetY)
{
	if(m_bEndGameed)
	{
		/*int iFont = GetDevice()->SetFont(2);
		
		stRectI rcMsg(pt.x,pt.y,pt.x+60,pt.y+20);
		char strTime[40] = {0};
		if(m_bWin)
			strcpy(strTime,"胜利");
		else 		
			strcpy(strTime,"失败");
		strTime[39] = 0;
		std::string strText = strTime;
		GetDevice()->DrawString(strText.c_str(),strText.length(),rcMsg,COLOR_ARGB(255,74,233,29),
			DT_VCENTER,
			FontEffect_Border);
		GetDevice()->SetFont(iFont);	*/
		if( m_bWin)
		{
			m_AniSuccess.Render(nOffsetX,nOffsetY);
		}
		else
		{
			m_AniFail.Render(nOffsetX,nOffsetY);
		}
	}
}


void stEndTimer::Create()
{
	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");
        rl.group = 133;
	rl.frame = 5;
	m_AniSuccess.Create(&rl);
	m_AniSuccess.SetLoopPlay(false);

	rl.frame = 6;
	m_AniFail.Create(&rl);
	m_AniFail.SetLoopPlay(false);

}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void stStartTimer::StartTimer()
{
      bStarted = true;
      dwStartTime = xtimeGetTime();
}

void stStartTimer::Create()
{
	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");
	rl.group = 133;
	rl.frame = 3;
	m_AniStart.Create(&rl);
	m_AniStart.SetLoopPlay(false);

}

void stStartTimer::EndTimer()
{
     bStarted = false;
}

void stStartTimer::OnTimer()
{
     if(bStarted)
     {
	if( xtimeGetTime() - dwStartTime  >= 3000)
	{
		EndTimer();
	}
     }
}

void stStartTimer::Draw(int nOffsetX,int nOffsetY)
{
     if(bStarted)
     {
	/*     int iFont = GetDevice()->SetFont(2);
	     stPointI pt;
	     pt.x = PT_CENTER_X; 
	     pt.y = PT_CENTER_Y;
	     pt.x += nOffsetX;
	     pt.y += nOffsetY;
	     stRectI rcMsg(pt.x,pt.y,pt.x+60,pt.y+20);
	     
	     char strTime[40] = {0};     
	     strcpy(strTime,"开始");		     
	     strTime[39] = 0;
	     std::string strText = strTime;

	     GetDevice()->DrawString(strText.c_str(),strText.length(),rcMsg,COLOR_ARGB(255,74,233,29),
		     DT_VCENTER,
		     FontEffect_Border);
	     GetDevice()->SetFont(iFont);*/
	     m_AniStart.Render(nOffsetX,nOffsetY);
     }
}

//////////////////////////////////////////////////////////////////////////////////////////
stBombEffect::stBombEffect()
{
}

void stBombEffect::Create()
{
	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");
	rl.group = 133;
	rl.frame = 4;
	m_AniBomb.Create(&rl);
	m_AniBomb.SetLoopPlay(false);
	m_AniBomb.SetStop(true);
}
void stBombEffect::OnBombed()
{
	PlayUISound(91015);
	m_AniBomb.SetStop(false);
	m_AniBomb.SetFrame(0);
	m_AniBomb.SetLoopPlay(false);
};


void stBombEffect::Draw(int nOffsetX,int OffsetY)
{
	if(m_AniBomb.IsPlaying())
		m_AniBomb.Render(nOffsetX,OffsetY);
	
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
CGame::CGame()
:m_pMainPlayer(NULL)
, m_pBlankCardBmp(NULL)
{
}

CGame::~CGame()
{
}

// 确定进入游戏消息，开始进入准备状态
void CGame::ConfirmEntryGame()
{
	CLordStrikeManager::GetSingleton()->SetGameState(eState_Preparing);
	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
		CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->ConfirmEntryGame();
}

void CGame::OnWindowCloseCallBack()
{
	m_PutCardTimer.StopSound();
}

// 退出游戏
void CGame::MeLeaveGame()
{
	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
		CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->Close();

	CLordStrikeManager::GetSingleton()->SetGameState(eState_Room); 
}


//////////////////////////////////////////////////////////////////////////////////////////
// 所有网络消息
void CGame::StartGame(stGameStartCommonMiniGameCmd *cmd)
{
    	m_gameID = cmd->gameID;
	
	m_EndTimer.Stop();
	ClearPlayer();
	InitPlayer();

	m_DDZContestPoint.Clear();
	m_DDZContestPoint.Create();

	m_PutCardTimer.Create();
	m_PutCardTimer.SetLastPutCardUserID(0);

	m_ReverseInfo.ClearReverseCards();

	CLordStrikeManager::GetSingleton()->SetGameState(eState_eStartGame);
	m_StartTimer.EndTimer();
	m_StartTimer.StartTimer();
	m_StartTimer.Create();

	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");
	rl.group = 134;
	rl.frame = 16;
	m_pBlankCardBmp = GetDevice()->FindBitmap(&rl);

	m_BombEffect.Create();
	m_EndTimer.Create();
}


void CGame::InitPlayer()
{
	m_pLastPlayer = NULL;
	m_pMainPlayer = new CMainPlayer;
	if(m_pMainPlayer)
	{
		m_pMainPlayer->SetDir(eDir_Down);
		m_vecPlayer.push_back(m_pMainPlayer);
	}	

	int nCardsPerPlayer,nReverseNum;
 	nCardsPerPlayer = 17;
	nReverseNum = 3;
	m_nPackNum = 1;
	 
	stGameInfo & gameInfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();	 
	std::vector<int> vecDir;
	std::list<MiniUserData *>::iterator iter = gameInfo.mListUserData.begin();
	for(; iter != gameInfo.mListUserData.end();iter++)
	{
		MiniUserData * pUser = *iter;
		int nDirection = SeatToDirection(pUser->pos.seat,gameInfo.seatid.seat);
		vecDir.push_back(nDirection);
		CPlayer * pPlayer = FindPlayer((eDirection)nDirection);
		if(pPlayer)
			pPlayer->SetCurNum(nCardsPerPlayer);  // 每个人初始化有这么多牌
	}
}

void CGame::ClearPlayer()
{
	for(size_t i = 0 ; i < m_vecPlayer.size(); i++)
	{
		CPlayer * pPlayer= m_vecPlayer[i];
		if(pPlayer)
		{
			if(m_pMainPlayer == pPlayer)
				m_pMainPlayer->ClearPlayingCard();
			pPlayer->ClearCard();
			delete pPlayer;
		}
	}
	m_pMainPlayer = NULL;
	m_vecPlayer.clear();  
}


void CGame::EndGame(stGameEndCommonMiniGameCmd *cmd)
{
	m_StartTimer.EndTimer();
	m_PutCardTimer.StopSound();

	bool bSomeOneExit = true;
	int dirarray[3] = {1,2,3};
	for(int i = 0 ; i < 3; i++)
	{
		int dir = dirarray[i];
		CPlayer * pPlayer = FindPlayer((eDirection)dir);
		if(pPlayer)
		{
			if( pPlayer->GetCurNum() == 0 )
			{
				bSomeOneExit = false;
				break;
			}
		}
	}
	if( bSomeOneExit)
	{ // 如果中途有人强制退出
		CLordStrikeManager::GetSingleton()->GetGame()->ClearPlayer();
		CLordStrikeManager::GetSingleton()->SetGameState(eState_Preparing); // 回到准备状态
	}
	else 
	{
		bool bLordWin = false;
		CPlayer * pLorder = FindPlayer(eDirection(m_DDZContestPoint.m_nDirOfLord));
		if(pLorder)
		{
			if(0 == pLorder->GetCurNum())
			{
				bLordWin =  true;
			}
		}
		if( m_DDZContestPoint.IsMySelfLord() )
		{
			m_EndTimer.m_bWin =   bLordWin;
		}
		else
		{
			m_EndTimer.m_bWin = !bLordWin;
		}		
		m_EndTimer.StartTimer();	
		m_EndTimer.m_bLorder = m_DDZContestPoint.IsMySelfLord();
		
	}
	assert(m_gameID  == cmd->gameID);
	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
		CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->EndGame();
}


// 画刚刚打过的牌
void CGame::DrawPlayedCard(int offsetx,int offsety)
{
	for(size_t i = 0 ; i < m_vecPlayer.size() ;i++)
	{
		CPlayer * pPlayer = m_vecPlayer[i];
		if(pPlayer)
		{
			int dir = pPlayer->GetDir();

			int startx;
			std::vector<CCard *> & cardlist = pPlayer->GetCardList();
			if( dir == 1 )
			{// 从左往右画
				startx = offsetx + c_PtPlayerCard[dir-1].x;
			}
			else if(dir == 3)
			{//从右往左画
				startx = offsetx + c_PtPlayerCard[dir-1].x - (cardlist.size() -1) * c_gap_card - CARD_WIDTH;
			}
			else if(dir ==2 )
			{ // 从中心往两边画
				startx = offsetx + c_PtPlayerCard[dir-1].x -(cardlist.size() -1)/2 * c_gap_card;
			}
			int starty = offsety + c_PtPlayerCard[dir-1].y;
			for(int j = 0 ; j < cardlist.size() ; j++)
			{
				CCard * pcard = cardlist[j];

				assert(dir>=1 && dir<=3);
				int x = startx + j * c_gap_card;
				int y = starty;
				pcard->Render(x,y);		
			}
		}
	}
}

// 画我的牌
void CGame::DrawPlayingCard(int offsetx,int offsety)
{
	if(m_pMainPlayer)
	{
		std::vector<CCard *>  cardlist = m_pMainPlayer->GetPlayingCardList();
		for(int j = 0 ; j < cardlist.size() ; j++)
		{
			CCard * pcard = cardlist[j];
			int x = offsetx + c_PtPlayingCard[1].x + j * c_gap_card;
			int y = offsety + c_PtPlayingCard[1].y;
			if(pcard)
			{
				if(pcard->IsSelectState())
				{
					y -= c_up_card;
				}
				pcard->Render(x,y);	
			}					
		}
	}

	// dir 1
	int dirarray[2] = {1,3};
	for(int i = 0 ; i< count_of(dirarray);++i)
	{
		int dir = dirarray[i];
		CPlayer * pPlayer = FindPlayer((eDirection)dir);
		int nCardNum  = pPlayer->GetCurNum();	
		int startx = offsetx + c_PtPlayingCard[dir-1].x;
		int starty = offsety + c_PtPlayingCard[dir-1].y - nCardNum/2 * c_gap_card;

		for(int j = 0 ; j < nCardNum ; j++)
		{
			int x = startx;
			if( dir == 3)
				x-=CARD_WIDTH;
			int y = starty + j * c_gap_card;
			if(m_pBlankCardBmp)
				m_pBlankCardBmp->Render(x,y);		
		}
	}
}
 
void CGame::DrawPlayerInfo(int offsetx,int offsety)
{
	for(size_t i = 0 ; i < m_vecPlayer.size() ;i++)
	{
		CPlayer * pPlayer = m_vecPlayer[i];
		if(pPlayer)
		{
			int dir = pPlayer->GetDir();
			assert(dir >=1 && dir <=3);
			if(dir >= 1 && dir <= 3)
			{
				//pPlayer->Draw(offsetx + c_PtCardNum[dir-1].x,offsety + c_PtCardNum[dir-1].y);
				pPlayer->DrawName(offsetx + s_ptName[dir-1].x,offsety + s_ptName[dir-1].y);
			}
		}
	}		
}

void CGame::Update()
{
	eGameState  __state = CLordStrikeManager::GetSingleton()->GetGameState();
	if(__state == eState_DispenseCard)
	{
		// UPDATE 
		m_PutCardTimer.OnPutCardTimer();
	}
	if(__state == eState_DispenseCard || __state == eState_ContestPoint)	
	{
		m_DDZContestPoint.OnContestTimer();
	}
	if(__state == eState_DispenseCard)
	{
		m_EndTimer.OnTimer();
	}
	if(__state == eState_eStartGame || __state == eState_ContestPoint)
	{
		m_StartTimer.OnTimer();
	}
}

void CGame::Draw(int offsetx,int offsety)
{
	eGameState  __state = CLordStrikeManager::GetSingleton()->GetGameState();
	
	if(__state == eState_DispenseCard)
	{
		// UPDATE 
		//m_PutCardTimer.OnPutCardTimer();

		// DRAW 
		DrawPlayedCard(offsetx,offsety);

		

		m_BombEffect.Draw(offsetx,offsety);
	}

	if( __state == eState_DispenseCard || __state == eState_DealCard || __state == eState_ContestPoint
		|| __state == eState_Preparing || __state == eState_eStartGame)
	{
		DrawPlayerInfo(offsetx,offsety);
	}

	 if(__state == eState_DispenseCard || __state == eState_DealCard || __state == eState_ContestPoint)
	 {
		 DrawPlayingCard(offsetx,offsety);		
	 }
		
	 if(__state == eState_DispenseCard)
	 {

	 }
	
	if(__state == eState_DispenseCard || __state == eState_ContestPoint)	
	{
		//m_DDZContestPoint.OnContestTimer();
		m_DDZContestPoint.Draw(offsetx,offsety);
	}

	if(__state == eState_DispenseCard)
	{
		m_PutCardTimer.Draw(offsetx,offsety);
	}
		
	if( __state == eState_ContestPoint || __state == eState_DispenseCard)
	{
		// 画底牌	
		m_ReverseInfo.Draw(offsetx,offsety);
	}

	if(__state == eState_DispenseCard)
	{
		m_EndTimer.Draw(offsetx,offsety);
	}
	if(__state == eState_eStartGame || __state == eState_ContestPoint)
	{
		m_StartTimer.Draw(offsetx,offsety);
	}
        
}

bool CGame::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		{
			if(m_pMainPlayer)
			{
				POINT pt;
				pt.x = (int)wParam;
				pt.y = (int)lParam;
				int nSize= GetMainPlayer()->GetPlayingCards();
				stRectI rcClip;
				for(int i = 0 ; i < nSize-1;i++)
				{
					CCard * pCard = GetMainPlayer()->GetPlayingCard(i);
					if(pCard)
					{
						rcClip.left = c_PtPlayingCard[1].x + i * c_gap_card;
						rcClip.right = rcClip.left + c_gap_card;
						rcClip.top =  c_PtPlayingCard[1].y;
						if(pCard->IsSelectState())
						{
							rcClip.top -= c_up_card;
						}
						
						rcClip.bottom = rcClip.top + CARD_HEIGHT;
						if(rcClip.PtInRect(pt))
						{
							pCard->SetSelectState(!pCard->IsSelectState());
							PlayUISound(91010);
							OnCardPatternCheck();
							return true;
						}
					}
				}
				// the last one
				rcClip.left = c_PtPlayingCard[1].x + (nSize-1) * c_gap_card;
				rcClip.right = rcClip.left + CARD_WIDTH;
				rcClip.top = c_PtPlayingCard[1].y; 
				CCard * pCard = GetMainPlayer()->GetPlayingCard(i);
				if(pCard)
				{
					if(pCard->IsSelectState())
					{
						rcClip.top -= c_up_card;
					}
					rcClip.bottom = rcClip.top + CARD_HEIGHT;
					if(rcClip.PtInRect(pt))
					{
						pCard->SetSelectState(!pCard->IsSelectState());
						PlayUISound(91010);
						OnCardPatternCheck();
						return true;
					}
				}
			}			
		}
		break;
	}
	return false;
}

void CGame::OnCardPatternCheck()
{
	if( m_PutCardTimer.m_dwCurPutCardUserID != CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID)
	{
		return;
	}
	bool bCanPut = false;

	std::vector<CCard *> ccardlist;
	GetSelectedCard(ccardlist);
	if(ccardlist.size() != 0)
	{
		if( m_PutCardTimer.IsLastPutCardUserMySelf())
		{ 
			bCanPut = true;
		}
		else 
		{
			std::vector<Card> vecCardNow;
			for(size_t i = 0 ; i < ccardlist.size() ;i++)
			{
				CCard * pCard = ccardlist[i];
				int nCardID = pCard->GetCardID();
				Card & card = IntToCard(nCardID);
				vecCardNow.push_back(card);
			}
			CardPattern patternnow;
			if(CardPattern::match_pattern(m_nPackNum, &vecCardNow[0], vecCardNow.size(),patternnow))
			{
				if(NULL != m_pLastPlayer)
				{
					std::vector<CCard *> & cardlist2 = m_pLastPlayer->GetCardList();
					if(cardlist2.size() ==0)
						return;

					std::vector<Card> vecCard2;
					for(size_t i = 0 ; i < cardlist2.size() ;i++)
					{
						CCard * pCard = cardlist2[i];
						int nCardID = pCard->GetCardID();
						Card & card = IntToCard(nCardID);
						vecCard2.push_back(card);
					}

					CardPattern patternold;
					CardPattern::match_pattern(m_nPackNum, &vecCard2[0], vecCard2.size(),patternold);

					if(patternnow > patternold)
					{
						bCanPut = true;
					}

				}
				else 
					bCanPut = true;
			}
		}

	}

	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
		CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->EnableDispenseCardButton(bCanPut);
}

CPlayer * CGame::FindPlayer(eDirection dir)
{
	for(std::vector<CPlayer *>::iterator iter = m_vecPlayer.begin(); iter != m_vecPlayer.end(); ++iter)
	{
		CPlayer * pPlayer  = *iter;
		if(pPlayer)
		{
			if(pPlayer->GetDir() == dir)
			{
				return pPlayer;
			}
		}
	}
	CPlayer * pPlayer = NULL;
	if( dir == 2)
		pPlayer = new CMainPlayer;
	else 
	{
		pPlayer = new CPlayer;
	}
	if(pPlayer)
	{
		pPlayer->SetDir(dir);
		pPlayer->ClearCard();
		if(dir == 2)
		{
			CMainPlayer * pMain = (CMainPlayer *)pPlayer;
			pMain->ClearPlayingCard();
		}
		m_vecPlayer.push_back(pPlayer);
	}
	return pPlayer;
}


//发牌
void  CGame::DealCard(stDealCardMiniGameCmd *cmd)
{
	m_pMainPlayer->ClearPlayingCard();
	m_pMainPlayer->ClearCard();
	CLordStrikeManager::GetSingleton()->SetGameState(eState_DealCard);
	for(int k = 0 ; k < cmd->num; k++)
	{
		int nCard = CardToInt(cmd->cards[k]);
		m_pMainPlayer->AddPlayingCard(nCard);
	} 
}

// 把底牌给属主
void CGame::TranslateReverseCard()
{
	std::vector<CCard *>& cardlist = m_ReverseInfo.GetReverseCards();
	if(cardlist.size() >0)
	{
		stGameInfo & gameinfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
		BYTE seat = gameinfo.GetGameUserSeat(m_ReverseInfo.dwOwnerID);
		if(seat)
		{
			int nDirection = SeatToDirection(seat,gameinfo.seatid.seat);
			CPlayer * pPlayer = FindPlayer((eDirection)nDirection);
			if(pPlayer)
			{
				int nOldCard = pPlayer->GetCurNum();
				pPlayer->SetCurNum(nOldCard+cardlist.size());  // 每个人有这么多牌
			}
			if(nDirection == 2)
			{
				for(size_t i = 0 ; i < cardlist.size() ; i++)
				{
					CCard * pcar = cardlist[i];
					GetMainPlayer()->AddPlayingCard(pcar->GetCardID());
				}
			}
		}
	}
}

// 选择"不出"
void CGame::SendNoDispenseCard(DWORD dwUserID)
{
	assert(dwUserID == CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID/*CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().GetMyUserID()*/);

	stPutCardMiniGameCmd cmd;
	cmd.userID = dwUserID;
	cmd.gameID = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().gameid;
	cmd.num = 0;
	SEND_USER_CMD(cmd);
}


// 向服务器发送我所选择的牌
void CGame::SendSelectedCard()
{
	std::vector<CCard *> ccardlist;
	GetSelectedCard(ccardlist);
	SendCard(ccardlist);
}


void CGame::SendCard(const std::vector<CCard *> & cardlist)
{
	int nCardNums = cardlist.size();

	SetFrameAllocator fa;
	DWORD dwLen = sizeof(stPutCardMiniGameCmd) + nCardNums * sizeof(Card);
	stPutCardMiniGameCmd * p = (stPutCardMiniGameCmd*)FrameAllocator::alloc(dwLen);
	constructInPlace(p);
	p->userID = CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID/*CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().GetMyUserID()*/;
	p->gameID = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().gameid;
	p->num = nCardNums;

	char * p1 = (char *)p;
	p1+= sizeof(stPutCardMiniGameCmd);
	for(int i = 0; i < nCardNums; ++i)
	{
		CCard * pCard = cardlist[i];
		if(pCard)
		{
			int nCardID = pCard->GetCardID();
			Card card = IntToCard(nCardID);
			memcpy(p1,&card,sizeof(Card));
			p1+=sizeof(Card);
		}
	}
	//*(p+dwLen) = 0;
	Client_SendCommand( p, dwLen,false );

}

//通知某人出牌
void  CGame::NotifyPutCard(stNotifyPutCardMiniGameCmd *cmd)
{
	CLordStrikeManager::GetSingleton()->SetGameState(eState_DispenseCard);
	
	PlayUISound(91011);
	m_PutCardTimer.StartTimer(cmd->userID); //开始计时，到40秒后通知
	

	CPlayer * pPlayer = FindPlayer((eDirection)m_PutCardTimer.m_nCurTimerDir);
	if(pPlayer)
	{
		pPlayer->ClearCard();
	}

	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
	{
		CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->NotifyPutCard(m_PutCardTimer.m_nCurTimerDir);
		if(m_PutCardTimer.m_nCurTimerDir == 2)
		{
			//CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->EnableDispenseCardButton(true);
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->EnableNoDispenseCardButton(true);
			OnCardPatternCheck();
			CLordStrikeManager::GetSingleton()->UpdateHideBtn();
		}
		else 
		{
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->EnableNoDispenseCardButton(false);
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->EnableDispenseCardButton(false);
		}
	}
	
}

//出牌
void  CGame::PutCard(stPutCardMiniGameCmd *cmd)
{
	if(cmd->num > 0 )
	{// 等于0,代表不出牌
		m_PutCardTimer.SetLastPutCardUserID(cmd->userID);
	}

	BYTE seat = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().GetGameUserSeat(cmd->userID);
	int nDirection  = 0;
	if(seat)
	{
		stGameInfo & gameinfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
		nDirection = SeatToDirection(seat,gameinfo.seatid.seat);
	}
	m_PutCardTimer.SetLastCardDir(nDirection,cmd->num);
 	
 
	if( nDirection )
	{
		CPlayer * pPlayer = FindPlayer((eDirection)nDirection);
		if(pPlayer)
		{
			int nOldCard = pPlayer->GetCurNum();
			assert(nOldCard>=cmd->num);
			pPlayer->SetCurNum(nOldCard-cmd->num);  // 每个人有这么多牌
			if(cmd->num > 0 )
				m_pLastPlayer = pPlayer;
			pPlayer->ClearCard();
		}

		if( nDirection == 2)
		{
			m_pMainPlayer->ClearCard();
			for(int i= 0; i < cmd->num; i++)
			{
				Card & car = cmd->cards[i];
				int nCard = CardToInt(car);
				m_pMainPlayer->AddCard(nCard);
				m_pMainPlayer->RemovePlayingCard(nCard);
			}
			std::vector<CCard *> & cardlist = m_pMainPlayer->GetCardList();
			std::sort(cardlist.begin(),cardlist.end(),CardCompare<true>());
			//////////////////////////////////////////////////////
			std::vector<Card> vecCardNow;
			for(size_t i = 0 ; i < cardlist.size() ;i++)
			{
				CCard * pCard = cardlist[i];
				int nCardID = pCard->GetCardID();
				Card & card = IntToCard(nCardID);
				vecCardNow.push_back(card);
			}
			CardPattern patternnow;
			if(CardPattern::match_pattern(m_nPackNum, &vecCardNow[0], vecCardNow.size(),patternnow))
			{
				if( patternnow.bomb)
					m_BombEffect.OnBombed();

				if(patternnow.missile || patternnow.bomb)
					m_DDZContestPoint.m_nBy +=1;
			}
			//////////////////////////////////////////////////////
			/*if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->ShowDispenseCardButtons(false);*/
		}
		else
		{
			if(pPlayer)
			{
				pPlayer->ClearCard();
				for(int i = 0; i < cmd->num; i++)
				{
					Card & ard = cmd->cards[i];
					int id = CardToInt(ard);
					pPlayer->AddCard(id);
				}
				std::vector<CCard *> & cardlist = pPlayer->GetCardList();
				//////////////////////////////////////////////////////
				//////////////////////////////////////////////////////
				std::vector<Card> vecCardNow;
				for(size_t i = 0 ; i < cardlist.size() ;i++)
				{
					CCard * pCard = cardlist[i];
					int nCardID = pCard->GetCardID();
					Card & card = IntToCard(nCardID);
					vecCardNow.push_back(card);
				}
				CardPattern patternnow;
				if(CardPattern::match_pattern(m_nPackNum, &vecCardNow[0], vecCardNow.size(),patternnow))
				{
					if( patternnow.bomb)
						m_BombEffect.OnBombed();
					if(patternnow.missile || patternnow.bomb)
						m_DDZContestPoint.m_nBy +=1;
				}
				//////////////////////////////////////////////////////
				//////////////////////////////////////////////////////
				std::sort(cardlist.begin(),cardlist.end(),CardCompare<true>());
			}
		}		
	}
}

//通知某人叫分
void  CGame::NotifyPointDDZ(stNotifyPointDDZMiniGameCmd *cmd)
{
	DWORD dwUserID =cmd->userID;
	BYTE seat = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo().GetGameUserSeat(dwUserID);
	if(seat)
	{
		CLordStrikeManager::GetSingleton()->SetGameState(eState_ContestPoint);
		stGameInfo & gameinfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
		int nDirection = SeatToDirection(seat,gameinfo.seatid.seat);
		assert( nDirection>= 1 && nDirection <=3);
		m_DDZContestPoint.StartTimer(nDirection);
		if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
               		CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->NotifyPointDDZ(nDirection,m_DDZContestPoint.GetMaxPoint());
		
	}
}

void CGame::GetSelectedCard(std::vector<CCard *> & ccardlist)
{
	ccardlist.clear();
	if(GetMainPlayer())
	{
		std::vector<CCard *> & cardlist = GetMainPlayer()-> GetPlayingCardList();
		for(int i = 0 ; i < cardlist.size() ; i++)
		{
			CCard * pcard = cardlist[i];

			if(pcard->IsSelectState())
			{
				ccardlist.push_back(pcard);
			}		
		}
	}

}

void CGame::ClearSelectedCard()
{
	if(GetMainPlayer())
	{
		std::vector<CCard *> & cardlist = GetMainPlayer()->GetPlayingCardList();
		std::vector<CCard *>::iterator iter;
		for(iter = cardlist.begin() ; iter != cardlist.end();)
		{
			CCard * pcard = *iter;
			if(pcard->IsSelectState()){
				cardlist.erase(iter);
			}
			else 
				++iter;
		}
	}
}

void CGame::ClearSelectedState()
{
	if(GetMainPlayer())
	{
		std::vector<CCard *> & cardlist = GetMainPlayer()->GetPlayingCardList();
		std::vector<CCard *>::iterator iter;
		for(iter = cardlist.begin() ; iter != cardlist.end() ;++iter)
		{
			CCard * pcard = *iter;
			pcard->SetSelectState(false);
		}
	}
}

void CGame::SetReverseCards(stReserveCardsDDZMiniGameCmd *cmd)
{
	m_ReverseInfo.SetReverseCards(cmd);
	TranslateReverseCard();
}

//斗地主叫分(服务器通知我 某人叫了多少分)
void CGame::DDZPoint(stPointDDZMiniGameCmd *cmd)
{
	if(cmd->num == 0)
		PlayUISound(91009);
	else 
		PlayUISound(91008);
        m_DDZContestPoint.DDZPoint(cmd);
}

void CGame::SendNoContest()
{
	stPointDDZMiniGameCmd cmd;
	stGameInfo & gameInfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
	cmd.userID = /*gameInfo.GetMyUserID()*/CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID;
	cmd.num = 0;
	cmd.gameID = gameInfo.gameid;
	SEND_USER_CMD(cmd);

	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
		CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->ShowContestPointButtons(false);
}

bool CGame::IsMeNow()
{
	DWORD dwMainUserID = CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID;
	return  dwMainUserID != 0 && dwMainUserID == m_PutCardTimer.m_dwCurPutCardUserID;
}

void CGame::UpdatePlayerName(int nDirection,const char * pname)
{
	CPlayer * pPlayer = FindPlayer((eDirection)nDirection);
	if( pPlayer )
	{
		pPlayer->SetName(pname);
	}
}