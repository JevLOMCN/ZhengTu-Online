#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "../Media/SoundManager.h"
#include "Game.h"
#include ".\lordstrikeManager.h"
#include ".\lordstrikeLobby.h"
#include "LordStrikeRoomDlg.h"
#include "LordStrikeLobbyDlg.h"
#include "LordStrikeGameBkDlg.h"
#include "LordStrikeGame.h"
#include "LSScoreDlg.h"


#define  CHAT_NORMAL_COLOR    COLOR_ARGB(255,255,0,255)
#define  MSG_SYSTEM_COLOR   COLOR_ARGB(255,255,0,6)
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
CLobby::CLobby()
{
}

CLobby::~CLobby()
{
}
 
// 大厅房间列表
void CLobby::SetRoomList(stGameListCommonMiniGameCmd * pCmd)
{
	m_listMiniRoomData.clear();
	for(int i = 0 ; i < pCmd->num; i++)
	{
		m_listMiniRoomData.push_back(pCmd->data[i]);
	}

	if(pCmd->num!=0)
	{
		// CLordStrikeManager::GetSingleton()->AddLordStrikeLobbyDlg();
		if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeLobby)
		{
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeLobby->RefreshRoom();
		}
	}
}

int CLobby::GetRoomMaxTableNum(MiniRoomID roomid)
{
	std::list<MiniRoomData> & roomlist = GetRoomList();
	std::list<MiniRoomData>::iterator iter = roomlist.begin();
	for( ; iter != roomlist.end(); iter++)
	{
		MiniRoomData & room = *iter;
		if(room.roomID == roomid)
		{
			return room.gameNum;
		}
	}
	assert(0);
	return 0;
}

//进入房间确认消息
void CLobby::ConfirmEntryRoom(stEnterRoomRetCommonMiniGameCmd * pCmd)
{

      //BYTE ret;//1成功 0失败 2房间已满 3已经参加该类型的游戏
      if(pCmd->ret == 1)  // 
      { 
	       ClearAllUserFromCurRoom();
	       //GetCurRommInfo().id = pCmd->roomID;
	       GetCurRommInfo().mListUserData.clear();
	       GetCurRommInfo().mSeatList.clear();

	       // 
	       if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeLobby)
	       {
		       int nNeedMoney = CLordStrikeManager::GetSingleton()->m_guiLordStrikeLobby->GetRoomNeedMoney(pCmd->roomID);
                       GetCurRommInfo().nNeedMoney = nNeedMoney;
	       }
	   
	       int nMaxTable = GetRoomMaxTableNum(pCmd->roomID);
	       SetCurRoomInfo(pCmd->roomID,nMaxTable);


               CLordStrikeManager::GetSingleton()->AddLordStrikeRoomDlg();
              
	       //进入房间,先new好空房间
	       if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
	       {
		       int nMaxTable = GetCurRommInfo().nMaxTable;
		       CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->NewTable(nMaxTable);
		       stMainUserInfo& info = CLordStrikeManager::GetSingleton()->GetMainUserInfo ();
		       CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->UpdatePlayerRoleNameAndLevel(info.strRoleName,info.nlevel);
		       GetGuiManager()->RequestFocus(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom);
	       }

	       CLordStrikeManager::GetSingleton()->SetGameState(eState_Room);     

      }
      else if(pCmd->ret == 0)
      {
	      GameMessageBox("登陆该房间失败");
      }
      else if(pCmd->ret == 2)
      {
		GameMessageBox("房间已满");
      }
      else if(pCmd->ret == 3)
      {
	      GameMessageBox("已经参加该类型的游戏");
      }
}

void CLobby::MeLeaveRoom()
{
	stLeaveRoomCommonMiniGameCmd cmd;
	cmd.roomID = GetCurRommInfo().id;
	SEND_USER_CMD(cmd);

	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
		CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->Close();
}


// 添加或者删除大厅人物信息，游戏里也用大厅里的数据，所以游戏里的玩家不会再专门定消息
// 来添加游戏里的人物
void CLobby::AddUserToCurRoom(stAddRoomUserCommonMiniGameCmd * pcmd)
{
	//assert(pcmd->roomID == m_RoomInfo.id);
	for(int i = 0 ; i < pcmd->num; i++)
	{
		MiniUserData & user = pcmd->data[i];
		std::list<MiniUserData>::iterator iter = GetCurRommInfo().mListUserData.begin();
		for(; iter != GetCurRommInfo().mListUserData.end();++iter)
		{
			MiniUserData & userdata = *iter;
			stMainUserInfo & MainInfo = CLordStrikeManager::GetSingleton()->GetMainUserInfo();
			if(MainInfo.dwMainUserID == 0)
			{
				if(0 == strcmp(userdata.name,MainInfo.strRoleName.c_str()))
				{
					MainInfo.dwMainUserID = userdata.id;
					MainInfo.userdata.score = userdata.score;
				}
			}
			if(userdata.id == user.id)
			{
				userdata = user;
				break;
			}
		}
		if( iter == GetCurRommInfo().mListUserData.end())
		{
			GetCurRommInfo().mListUserData.push_back(user);
			char szTip[100];
			_snprintf(szTip,100,"%s进入了房间",user.name);
			szTip[99] = 0;
			if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
				CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->AddChat(szTip,CHAT_NORMAL_COLOR);
		}

		if(user.id == CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID)
		{
			CLordStrikeManager::GetSingleton()->GetMainUserInfo().userdata.score = user.score;
			if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
				CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->UpdateMainPlayerScore(user.score);
			if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
				CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->UpdateMainPlayerScore(user.score);
		}
	}
        if(NULL == CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
		CLordStrikeManager::GetSingleton()->AddLordStrikeRoomDlg();

	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
              CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->UpdateTable();
}

//void CLobby::RemoveUserFromCurRoom(stLeaveRoomCommonMiniGameCmd * cmd)
//{
//	//assert(pcmd->roomID == m_RoomInfo.id);
//	 
//	//std::list<MiniUserData *>::iterator iter = m_GameInfo.mListUserData.begin();
//	//for(; iter != m_GameInfo.mListUserData.end();++iter)
//	//{
//	//	MiniUserData * puserdata = *iter;
//	//	if(puserdata->id == cmd->userID)
//	//	{
//	//		m_GameInfo.mListUserData.erase(iter);
//	//		break;
//	//	}
//	//}
//	//if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
//	//	CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->RefreshPlayerIconButton();
//
//	std::list<MiniUserData>::iterator iter = GetCurRommInfo().mListUserData.begin();
//	for(; iter != GetCurRommInfo().mListUserData.end();++iter)
//	{
//		MiniUserData & userdata = *iter;
//		/*if(userdata.id == cmd->userID)
//		{
//			GetCurRommInfo().mListUserData.erase(iter);
//			break;
//		}*/
//	}
//	
//	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
//		CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->UpdateTable();
//
//
//}

// 找到一个seatid所属的桌子的其他玩家
void CLobby::FindOhterUserInTable(MiniSeatID seatid, std::list<MiniUserData *>  & userlist)
{
	std::list<MiniUserData>::iterator iter = GetCurRommInfo().mListUserData.begin();
	for(; iter != GetCurRommInfo().mListUserData.end();++iter)
	{
		MiniUserData & userdata = *iter;
		if(userdata.pos.type == seatid.type
			&& userdata.pos.room == seatid.room
			&& userdata.pos.game == seatid.game)
		{
			userlist.push_back(&userdata);
		}
	}
}

// 找到一个seatid所属的桌子的其他方位信息
void CLobby::FindOtherSeatInTable(MiniSeatID seatid, std::vector<MiniSeatData *>  & seatlist)
{
	std::vector<MiniSeatData>::iterator iter = GetCurRommInfo().mSeatList.begin();
	for(; iter != GetCurRommInfo().mSeatList.end();++iter)
	{
		MiniSeatData & seatdata = *iter;
		if(seatdata.seatID.type == seatid.type
			&& seatdata.seatID.room == seatid.room
			&& seatdata.seatID.game == seatid.game)
		{
			seatlist.push_back(&seatdata);
		}
	}
}

// 主角玩家进入游戏
void CLobby::ConfirmEntryGame(stEnterGameRetCommonMiniGameCmd * pcmd)
{
	if( pcmd->ret == 1)
	{
		m_GameInfo.seatid = pcmd->seatID;

		std :: list<MiniUserData>::iterator iter;
		for(iter = m_RoomInfo.mListUserData.begin(); iter != m_RoomInfo.mListUserData.end();++iter)
		{
			MiniUserData & user = *iter;
			if(user.pos.type == pcmd->seatID.type
				&& user.pos.room == pcmd->seatID.room)
			{
				user.pos = pcmd->seatID;
				break;
			}
		}
		assert(iter != m_RoomInfo.mListUserData.end());
		/*if(iter == m_RoomInfo.mListUserData.end)
		{
			m_RoomInfo.mListUserData.push_back(pcmd->seatid);
		}*/

		m_GameInfo.mListUserData.clear();
		FindOhterUserInTable(pcmd->seatID,m_GameInfo.mListUserData);
		m_GameInfo.mSeatList.clear();
		FindOtherSeatInTable(pcmd->seatID,m_GameInfo.mSeatList);

		if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->SetVisible(false);
		CLordStrikeManager::GetSingleton()->AddLordStrikeGameBkDlg();

		CLordStrikeManager::GetSingleton()->GetLobby()->GetLSScore().BeginNewTurn();



		stMainUserInfo& info = CLordStrikeManager::GetSingleton()->GetMainUserInfo();
		if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
		{
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->UpdatePlayerRoleNameAndLevel(info.strRoleName,info.nlevel);
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->UpdateMainPlayerScore(info.userdata.score);
		}
		CLordStrikeManager::GetSingleton()->GetGame()->ConfirmEntryGame();

		PlayUISound(91000);

		
	}	
	else 
	{
		if(pcmd->ret == 0 )
			GameMessageBox("失败");
		else if(pcmd->ret == 2)
		{
			GameMessageBox("人数已满");
		}
		else if(pcmd->ret == 3)
		{
			GameMessageBox("游戏已开始");
		}
		else if(pcmd->ret == 4)
		{
                         GameMessageBox("座位已有人");
		}
		else if(pcmd->ret == 5)
		{
			GameMessageBox("豆子不足");
		}
	}

}
// 主角离开游戏
void CLobby::MeLeaveGame()
{
	stLeaveGameCommonMiniGameCmd cmd;
	cmd.gameID = m_GameInfo.seatid;
	//cmd.userID = //自己退出不用
	SEND_USER_CMD(cmd);

	CLordStrikeManager::GetSingleton()->GetGame()->MeLeaveGame();

	CLordStrikeManager::GetSingleton()->GetLobby()->GetLSScore().EndNewTurn();

	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
		CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->SetVisible(true);

	

}

// 刷新状态应该只按方位
void CLobby::UpdateSeatState(stSeatStateCommonMiniGameCmd * cmd)
{
	for(int i = 0 ; i < cmd->num; i++)
	{
		MiniSeatData & seatdata = cmd->data[i];
 	
	 	// 刷新大厅里的
		std::vector<MiniSeatData>::iterator iter;
		for(iter = m_RoomInfo.mSeatList.begin(); iter != m_RoomInfo.mSeatList.end();++iter)
		{
			MiniSeatData & data = *iter;
			if(data.seatID == seatdata.seatID)
			{
			         data = seatdata;
				 break;
			}
		}
		if(m_RoomInfo.mSeatList.end() == iter)
		{
			m_RoomInfo.mSeatList.push_back(seatdata);
		}

		if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->UpdateTable();

		//如果在游戏中，也要刷新游戏里的
		stGameInfo & gameinfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
		if(gameinfo.IsSeatInGame(seatdata.seatID))
		{
                        if(seatdata.state == 1)  
			{
				PlayUISound(91004);
			}
			else 
				PlayUISound(91003);
                	//m_GameInfo.AddSeatToGame(&seatdata);
			m_GameInfo.mSeatList.clear();
			FindOtherSeatInTable(seatdata.seatID,m_GameInfo.mSeatList);
			if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
				CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->RefreshPlayerIconButton();
		}
	}
}

// 刷新玩家的状态
void CLobby::UpdateUserState(stUpdateUserStateCommonMiniGameCmd *cmd)
{
	// 刷新大厅里的玩家数据的state
	MiniUserData oldUserData;
	bool bFinded = true;
	std::list<MiniUserData>::iterator iter;
	for(iter = m_RoomInfo.mListUserData.begin(); iter != m_RoomInfo.mListUserData.end();++iter)
	{
		MiniUserData & user = *iter;
		if(user.id == cmd->userID)
		{
			oldUserData = user;
			bFinded = true;
			user.state = cmd->state;
			user.pos = cmd->pos;
			break;
		}
	}
	assert(iter != m_RoomInfo.mListUserData.end() );

	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
		CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->UpdateTable();

        bool bErasedFromRoom = false;
	if(bFinded && oldUserData.state == MUS_ROOM && (cmd->state == MUS_HALL || cmd->state == MUS_NOTPLAY ))
	{
		char szTip[100];
		_snprintf(szTip,100,"%s离开了房间",oldUserData.name);
		szTip[99] = 0;
		if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->AddChat(szTip,CHAT_NORMAL_COLOR);
		m_RoomInfo.mListUserData.erase(iter);  // 必须清除掉
		bErasedFromRoom = true; 
          
	}
	 
        //#################################################################################
	// 一个人在房间被踢出,但是因为服务器会发退出游戏消息,所以这种状态的游戏变化就不管了
	// 某人原来在房间
	if( bFinded && m_GameInfo.IsUserInGame( oldUserData.pos))
	{
		if( (oldUserData.state==MUS_READY || oldUserData.state == MUS_PLAY)
			&& cmd->state == MUS_SEAT)
		{
			if(cmd->userID == CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID/*mm_GameInfo.GetMyUserID()*/)
			{ // 主角所在的游戏退出了
				if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
				{
					CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->ShowStartCardButtons(true);
					CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->ShowContestPointButtons(false);
					CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->ShowDispenseCardButtons(false);
				}
			}			         
		}

		bool bSomeOtherOneExit = false;
		if(cmd->state == MUS_ROOM)
		{
			if(cmd->userID == CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID/*m_GameInfo.GetMyUserID()*/)
			{
				if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
					CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->Close();
				if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
				{
					CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->SetVisible(true);
					CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->UpdateTable();
				}
			}
			else
			{// 其他玩家的界面处理服务器会发送endgame消息，这里我们给出提示
				bSomeOtherOneExit = true;
			}
		}
		if(bSomeOtherOneExit)
		{
						
			eGameState __gamestate = CLordStrikeManager::GetSingleton()->GetGameState();
			if(__gamestate != eState_Room && __gamestate != eState_Lobby)   
			{
				char szTip[120];
				_snprintf(szTip,120,"%s中途离开了牌桌,牌局结束!",oldUserData.name);
				szTip[119] = 0;

				if(__gamestate == eState_Preparing)
				{
					if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
						CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->AddChat(szTip,CHAT_NORMAL_COLOR);        
				}
				else
				{
					GameMessageBox(szTip);
				}

				m_GameInfo.mListUserData.clear();
				FindOhterUserInTable(oldUserData.pos,m_GameInfo.mListUserData);
				if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
					CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->RefreshPlayerIconButton();

				CLordStrikeManager::GetSingleton()->GetLobby()->GetLSScore().BeginNewTurn();
			}			
			return;
		}
		//else
		//{
		//	// 如果这个家伙仍在房间,那么后面的函数会作出处理
		//}
	}
	
	//#################################################################################
	
	if(!bErasedFromRoom)
	{
		if( m_GameInfo.IsUserInGame(  (*iter).pos ))
		{// 某人现在在房间了
			m_GameInfo.mListUserData.clear();
			FindOhterUserInTable((*iter).pos,m_GameInfo.mListUserData);
			if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
				CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->RefreshPlayerIconButton();
		}
	}	
}

void CLobby::StartGame(stGameStartCommonMiniGameCmd * cmd)
{
	m_GameInfo.gameid = cmd->gameID;

	PlayUISound(91005);
	CLordStrikeManager::GetSingleton()->GetGame()->StartGame(cmd);

	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
		CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->StartGame();
}

void CLobby::ParseDrawRetCmd(stDrawRetCommonMiniGameCmd  * pCmd)
{
        if( pCmd->ret == 0)
	{
		GameMessageBox("失败");
	}
	else if(pCmd->ret == 1 )
	{
		GameMessageBox("成功");
	}
	else if(pCmd->ret == 2 )
	{
		GameMessageBox("仙丹不足");
	}
	else if(pCmd->ret == 3)
	{
		GameMessageBox("包裹满");
	}
}

//// 某个人离开了正在游戏中的
//void  CLobby::OneMenLeftGame(DWORD dwUserID)
//{
//	std :: list<MiniUserData *>::iterator iter = m_GameInfo.mListUserData.begin();
//	for(iter != m_GameInfo.mListUserData.end() ;++iter)
//	{
//              MiniUserData & user = *iter;
//	      if(user.id == dwUserID)
//	      {
//		      if(user.pos == seatid)
//		      {// 桌主离开了(桌子非主动离开???)
//			      if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
//				      CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->MeLeaveGame();
//		      }
//		      else 
//		      {
//			      if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
//				      CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->RefreshPlayerIconButton();
//		      }
//		      break;
//	      }
//	}	
//}

void  CLobby::SomeChat(stChatCommonMiniGameCmd *pcmd)
{
 	 //char name[40];
	 std::string name;
         GetCurRommInfo().GetUserName(pcmd->userID,name);
	 //name[39] = 0;
	
	 std::string strchat;
         if(pcmd->type != MCT_SYS)
		 strchat = name +"说:";
	 strchat += pcmd->content;
	 
	 DWORD clr;

	 if( pcmd->type == MCT_NORMAL)
		clr = CHAT_NORMAL_COLOR;
	 else if(pcmd->type == MCT_SYS)
		 clr = MSG_SYSTEM_COLOR;
	 else if( pcmd->type == MCT_POPUP)
	 {
		 GameMessageBox(strchat.c_str());
		 return;
	 }
	 else 
		 clr = CHAT_NORMAL_COLOR;

	 if(  GetGameInfo().IsUserInGame(pcmd->userID))
	 {
		 if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->AddChat(strchat.c_str());
	 }
	 else 
	 {
		 if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
			 CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->AddChat(strchat.c_str());
	 }
}

void CLobby::AddSortList( stRetTopCommonMiniGameCmd * cmd)
{
	m_listSort.clear();
	for(int i = 0 ; i< cmd->num; i++)
	{
              m_listSort.push_back(cmd->data[i]);
	}
	CLordStrikeManager::GetSingleton()->AddLordStrikeSortDlg();
	if(CLordStrikeManager::GetSingleton()->m_guiSortDlg)
		CLordStrikeManager::GetSingleton()->m_guiSortDlg->UpdateList();
}
