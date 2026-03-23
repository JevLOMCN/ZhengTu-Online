#pragma once

class CLordStrikeManager;
class CLobby;
class CGame;

class CGuiLordStrikeLobbyDlg;
class CGuiLordStrikeRoomDlg;
class CGameBackGoundDlg;

class CGuiLSScoreDlg;
class CGuiLordStrikeSortDlg;

#include "LSSCore.h"

 
 
struct stGameInfo
{
	MiniGameID    gameid;// 开始游戏后才设置,也可以由seatid得出

         int GetUserSize()
	 {
		 return mListUserData.size();
	 }
    
	std::list<MiniUserData *> mListUserData;
        MiniSeatID seatid;  // myself
	std::vector<MiniSeatData *> mSeatList;

 
	MiniUserData * GetMiniUserDataOnSeat(MiniSeatID SeatID)
	{
		std::list<MiniUserData *>::iterator iter = mListUserData.begin(); 
		for(;iter != mListUserData.end(); ++iter)
		{
			if((*iter)->pos.seat==SeatID.seat)
			{
			         return *iter;
			}
		}
		return NULL;
	}

	bool IsUserInGame(MiniUserPosition pos)
	{
		if(seatid.type == pos.type && 
			seatid.room == pos.room && 
			seatid.game == pos.game)
			return true;
		return false;
	}
	bool IsUserInGame(DWORD dwUserID)
	{
		std::list<MiniUserData *>::iterator iter =  mListUserData.begin();
		for(;iter != mListUserData.end(); ++iter)
		{
			const MiniUserData * pData = *iter;
			if(pData)
			{
				if(pData->id == dwUserID)
				{
					return true;
				}
			}
		}
		return false;
	}
	bool IsSeatInGame(MiniSeatID _seatid)  //座位在桌子中
	{
		if(seatid.type == _seatid.type && 
			seatid.room == _seatid.room && 
			seatid.game == seatid.game)
			return true;
		return false;
	}

	BYTE  GetGameUserSeat(DWORD dwUserID)
	{
		std::list<MiniUserData *>::iterator iter =  mListUserData.begin();
		for(;iter != mListUserData.end(); ++iter)
		{
			const MiniUserData * pData = *iter;
			if(pData)
			{
				if(pData->id == dwUserID)
				{
					return pData->pos.seat;
				}
			}
		}
		return 0;
	}
	//DWORD GetMyUserID()  // 不可以从rpg游戏里取
	//{
	//	std::list<MiniUserData *>::iterator iter = mListUserData.begin();
	//	for(;iter != mListUserData.end(); ++iter)
	//	{
	//		MiniUserData * pUserData = *iter;
	//		if( pUserData && pUserData->pos == seatid)
	//		{
	//			return pUserData->id;
	//		}
	//	}
	//	assert(0);
	//	return 0;
	//}
	int GetStateOKPlayerNum()
	{
		std::list<MiniUserData *>::iterator iter = mListUserData.begin();
		int nCanPlayNum = 0;
		for(; iter != mListUserData.end();iter++)
		{
			MiniUserData * puser = *iter; 
			if(puser->state == MUS_READY)
				nCanPlayNum++;
		}
		return nCanPlayNum;
	}

};

struct stRoomInfo
{
	MiniRoomID id;
	std::list<MiniUserData>  mListUserData;
	int nMaxTable;
	std::vector<MiniSeatData> mSeatList;
	int nNeedMoney;

	bool GetUserName(DWORD dwUserID,std::string & outer)
	{
		std::list<MiniUserData>::iterator iter = mListUserData.begin();
		for(;iter != mListUserData.end();++iter)
		{
			MiniUserData & user = *iter;
			if(user.id == dwUserID)
			{
				outer = user.name;
				return true;
			}
		}
		return false;
	}
};
 

class CLobby 
{
public:
	CLobby();
	~CLobby();
	
	/////////////////////////////////////////////////////////////////////
	///大厅房间列表信息,用于构造树
	void SetRoomList(stGameListCommonMiniGameCmd * pCmd);
	typedef std::list<MiniRoomData> ListMiniRoomData;
	ListMiniRoomData & GetRoomList(){
		return m_listMiniRoomData;
	}
private:
	int GetRoomMaxTableNum(MiniRoomID roomid);
	ListMiniRoomData  m_listMiniRoomData;
	/////////////////////////////////////////////////////////////////////

	
public:
	/////////////////////////////////////////////////////////////////////
	void ConfirmEntryRoom(stEnterRoomRetCommonMiniGameCmd * pCmd);
	void MeLeaveRoom();
	/////////////////////////////////////////////////////////////////////

	void ParseDrawRetCmd(stDrawRetCommonMiniGameCmd  * pCmd);

	/////////////////////////////////////////////////////////////////////
	void StartGame(stGameStartCommonMiniGameCmd * cmd);
	/////////////////////////////////////////////////////////////////////

public:
	 
	//////////////////////////////////////////////////////////////////
	void ConfirmEntryGame(stEnterGameRetCommonMiniGameCmd * pcmd);
	void MeLeaveGame();
	/////////////////////////////////////////////////////////////////

	void FindOhterUserInTable(MiniSeatID seatid, std::list<MiniUserData *>  & userlist);
	void FindOtherSeatInTable(MiniSeatID seatid, std::vector<MiniSeatData *>  & seatlist);
	/////////////////////////////////////////////////////////////////
	void UpdateSeatState(stSeatStateCommonMiniGameCmd * cmd);
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	void UpdateUserState(stUpdateUserStateCommonMiniGameCmd *cmd);
	/////////////////////////////////////////////////////////////////	
      
	////////////////////////////////////////////////////////////////////
	stGameInfo & GetGameInfo()
	{
		return m_GameInfo;
	};
 	////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////
	stRoomInfo & GetCurRommInfo()
	{
		return m_RoomInfo;
	}	
	void SetCurRoomInfo(MiniRoomID roomid,int nMaxTable)
	{
          m_RoomInfo.id = roomid;
	  m_RoomInfo.nMaxTable = nMaxTable;
	}

	void ClearAllUserFromCurRoom()
	{
		m_RoomInfo.mListUserData.clear();
		m_RoomInfo.nMaxTable = 0;
		m_RoomInfo.mSeatList.clear();
		m_RoomInfo.id = 0;
	}
	 void AddUserToCurRoom(stAddRoomUserCommonMiniGameCmd * pcmd);
	 void RemoveUserFromCurRoom(stLeaveRoomCommonMiniGameCmd * cmd);
	 void FindUserInRoom(DWORD dwUserID);
	////////////////////////////////////////////////////////////////////

	 ////////////////////////////////////////////////////////////////////
	 CLSScore & GetLSScore()
	 {
		 return m_LSScore;
	 }
	 ////////////////////////////////////////////////////////////////////

	 ////////////////////////////////////////////////////////////////////
	 void SomeChat(stChatCommonMiniGameCmd *pcmd);
	 ////////////////////////////////////////////////////////////////////

	 void AddSortList(stRetTopCommonMiniGameCmd * cmd);

	 std::list<MiniUserData> & GetSortList()
	 {
		 return m_listSort;
	 }
private:
	stGameInfo m_GameInfo;
	stRoomInfo m_RoomInfo;
	CLSScore   m_LSScore;

	std::list<MiniUserData>  m_listSort;
	//////////////////////////////////////////////////////////////

};
