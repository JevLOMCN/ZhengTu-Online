#pragma once

#include ".\LordStrikePlayer.h"
#include ".\Lordstrikecard.h"



// 叫分管理器
struct stDDZContestPoint
{
	stDDZContestPoint()
	{
		Clear();
	}
	void Clear()
	{
		m_nCurContestDir = 0;
		m_mapDDZPoints.clear();
		m_nDirOfLord = 0;
		m_nLastOKContestDir=0 ;
		m_nLastOKContestNum=0 ;
	}
	void Create();

	void DDZPoint(stPointDDZMiniGameCmd * cmd);
	void Draw(int nScreenOffsetX,int nScreenOffsetY);
	std::map<DWORD,DWORD> m_mapDDZPoints;

	int GetMaxPoint()
	{
		std::map<DWORD,DWORD>::iterator iter = m_mapDDZPoints.begin();
		DWORD dwMax = 0;
		for(;iter != m_mapDDZPoints.end(); ++iter)
		{
                       if( iter->second > dwMax)
			       dwMax = iter->second;
		}
		return dwMax;
	}

	int GetMaxPointUserID()
	{
		std::map<DWORD,DWORD>::iterator iter = m_mapDDZPoints.begin();
		DWORD dwMax = 0;
		DWORD dwMaxUserID = 0;
		for(;iter != m_mapDDZPoints.end(); ++iter)
		{
			if( iter->second > dwMax)
			{
				dwMax = iter->second;
				dwMaxUserID = iter->first;
			}
		}
		return dwMaxUserID;
	}

	int m_nCurContestDir;
	int m_nDirOfLord;
	CAnimation m_AniLorder;

	int m_nLastOKContestDir ;  // 上一次叫好分的方向,用于提示
	int m_nLastOKContestNum;
	
	int m_nLandNum; //地皮数
	int m_nBy; // 倍数
	////////////////////////////////////////
	void StartTimer(int nDirection);
	void OnContestTimer();

	bool IsMySelfLord()
	{
		return (m_nDirOfLord == 2);
	}
	////////////////////////////////////////

	CAnimation m_AniTimer;  // 叫分定时器
	DWORD  m_dwLastUserID;        // 上一次谁出牌
	DWORD  m_dwCurContestUserID;   
	DWORD  m_dwStartContestTime;    
	SoundId m_timersound;
	////////////////////////////////////////
};

// 开局特效
struct  stStartTimer
{
	void StartTimer();
	void OnTimer();
	void EndTimer();
	void Draw(int nOffsetX,int nOffsetY);
	bool bStarted;
	DWORD dwStartTime;
	void Create();
	CAnimation m_AniStart;
};

// 发牌系统定时器(超时自动发牌)
struct stPutCardTimer
{
	stPutCardTimer()
	{
		m_nCurTimerDir = 0;
		m_dwLastCardDir = 0 ;
		m_dwLastPoint = 0 ;
	}
	void  Create();
	void  Draw(int x,int y);
        void  StartTimer(DWORD dwUserID);    //开始计时,到40秒后通知
        void  SetLastPutCardUserID(DWORD dwUserID);
	void  OnPutCardTimer();

	DWORD  m_dwLastPutCardUserID;        // 上一次谁出牌
	DWORD  m_dwCurPutCardUserID;         // 当前谁在出牌中
	DWORD  m_dwStartPutCardTime;       // 开始出牌时间   

	CAnimation m_AniTimer;
	int m_nCurTimerDir;
	SoundId m_timersound;

	int m_dwLastCardDir ;
	int m_dwLastPoint ;
	
	void StopSound();

	bool IsLastPutCardUserMySelf()
	{
		return m_dwLastPutCardUserID == m_dwCurPutCardUserID;
	}

	void SetLastCardDir(int nDirection,int nPoint);
};

// 底牌管理器
struct stReverseCardInfo
{
	void SetReverseCards(stReserveCardsDDZMiniGameCmd *cmd);
	void ClearReverseCards();
	void Draw(int nOffsetX,int nOffsetY);
	std::vector<CCard *>& GetReverseCards()
	{
		return m_reverseCards;
	}
	std::vector<CCard *> m_reverseCards;

	DWORD dwOwnerID;
	DWORD dwStartTime;
};

struct stEndTimer
{
	stEndTimer()
	{
		m_bEndGameed = false;
	}
	bool  m_bEndGameed;
	DWORD  m_dwStartEndTimer;

	bool IsFromEndTime()
	{
		return m_bEndGameed;
	}
	void StartTimer(); 
	void Stop();
	void OnTimer();
	void EndTimer();
	void Create();

	CAnimation m_AniSuccess,m_AniFail;

	void Draw(int nOffsetX,int nOffsetY);

	bool m_bWin;
	bool m_bLorder;
	
};

struct stBombEffect
{
	stBombEffect();
	void Create();
	void OnBombed();
	void Draw(int nOffsetX,int nOffsetY);
	CAnimation m_AniBomb;
};

// 游戏类
class CGame
{
public:
	CGame();
	~CGame();

	//////////////////////////////////////////////////
	void Draw(int offsetx,int offsety);
	void Update();
private:
	void DrawPlayerInfo(int nOffsetX,int nOffsetY);
	void DrawPlayingCard(int nOffsetx,int nOffsety);
	void DrawPlayedCard(int nOffsetx,int noffsety);

public:
	bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam );


	CPlayer * FindPlayer(eDirection dir);
	CMainPlayer * GetMainPlayer()
	{
		return m_pMainPlayer;
	}
	void ClearMainPlayer()
	{
		m_pMainPlayer = NULL;
	};

	///////////////////////////////////////////
	// start/finish game 
	void StartGame(stGameStartCommonMiniGameCmd *cmd);	
	void EndGame(stGameEndCommonMiniGameCmd *cmd);


	void ClearPlayer();
private:
	// init 
	void InitPlayer();
	
	////////////////////////////////////////////
public:
	/////////////////////////////////////////////
	// entry/leave game 
	void ConfirmEntryGame();
	void MeLeaveGame();
	/////////////////////////////////////////////

	/////////////////////////////////////////////
	void GetSelectedCard(std::vector<CCard *> & ccardlist);
	void ClearSelectedCard();
	void ClearSelectedState();
	void SendSelectedCard();
	//////////////////////////////////////////////
	
	///////////////////////////////////////////////
	// 把底牌给属主
	void TranslateReverseCard();
	void SetReverseCards(stReserveCardsDDZMiniGameCmd *cmd);
	/////////////////////////////////////////////////
private:
	MiniGameID  m_gameID;

	int  m_nPackNum;
	CPlayer * m_pLastPlayer;     
	CMainPlayer   * m_pMainPlayer;
     	std::vector<CPlayer *>  m_vecPlayer;      // 其它玩家

	eDirection  m_eCurDir; 
	////////////////////////////////////////////////////////
	//  叫分管理器
	stDDZContestPoint m_DDZContestPoint;

        //  计时管理器
	stPutCardTimer     m_PutCardTimer;
	
	// 底牌管理器
	stReverseCardInfo m_ReverseInfo;


	stBombEffect  m_BombEffect;

	void OnCardPatternCheck();

	stEndTimer m_EndTimer;

	stStartTimer m_StartTimer;

	IBitmap * m_pBlankCardBmp ;
public:
	////////////////////////////////////////////////////////////
	// 网络消息处理接口
	//发牌
	void  DealCard(stDealCardMiniGameCmd *cmd); 
	
	//通知某人出牌
	void  NotifyPutCard(stNotifyPutCardMiniGameCmd *cmd);

	//出牌
	void  PutCard(stPutCardMiniGameCmd *cmd);

	//通知某人叫分
	void  NotifyPointDDZ(stNotifyPointDDZMiniGameCmd *cmd);

	// 某人叫分
	void DDZPoint(stPointDDZMiniGameCmd *cmd);

	// 发送card
	void SendNoDispenseCard(DWORD dwUserID);
	void SendCard(const std::vector<CCard *> & cardlist);

	void SendNoContest();

///////////////////////////////////////////////////////////////////////

	void OnWindowCloseCallBack();

	// 是否轮到我出牌
	bool IsMeNow();


	void UpdatePlayerName(int nDirection,const char * pname);

};
