#include "public.h"
#include "miniusercommand.h"
#include "../Media/SoundManager.h"
#include "../Engine/include/Animation.h"
#include "Maincharacter.h"
#include "GameScene.h"
#include ".\lordstrikeManager.h"
#include ".\lordstrikeLobby.h"
#include "LordStrikeGame.h"

bool ParseLordLobbyMessage(stCommonMiniGameCmd* pCmd,size_t size)
{
	switch(pCmd->subParam)
	{
	case GAME_LIST_COMMON_MINI_PARA://房间列表
		{
			std::string strName = GetScene()->GetMainCharacter()->GetName();
			int Level = GetScene()->GetMainCharacter()->GetLevel();
			CLordStrikeManager::GetSingleton()->SetMainUserInfo(Level,strName);

			TRACE("---mini Game-- 收到大厅房间列表信息--\n");
			stGameListCommonMiniGameCmd * __pCmd = (stGameListCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->SetRoomList(__pCmd);
		}
		break;
	case ADD_ROOM_USER_COMMON_MINI_PARA:
		{
			TRACE("有人进入房间或者刷新房间玩家数据\n");
			stAddRoomUserCommonMiniGameCmd * __pcmd = (stAddRoomUserCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->AddUserToCurRoom(__pcmd);
		}
		break;
	case ENTER_ROOM_RET_COMMON_MINI_PARA:
		{
			TRACE("进入游戏房间确认信息\n");
			stEnterRoomRetCommonMiniGameCmd * __pCmd = (stEnterRoomRetCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->ConfirmEntryRoom(__pCmd);
		}
		break;
	case LOGIN_RET_COMMON_MINI_PARA:  // 进入大厅确认消息
		{
			TRACE("进入大厅确认消息");
			stLoginRetCommonMiniGameCmd * __pCmd = (stLoginRetCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->EntryLobby(__pCmd);
		}
		break;
		//case LEAVE_ROOM_COMMON_MINI_PARA:
		//	{
		//		TRACE("有人离开房间");
		//		stLeaveRoomCommonMiniGameCmd * __cmd = (stLeaveRoomCommonMiniGameCmd *)pCmd;
		//		CLordStrikeManager::GetSingleton()->GetLobby()->RemoveUserFromCurRoom(__cmd);

		//	}
		break;
	case ENTER_GAME_RET_COMMON_MINI_PARA:
		{
			TRACE("进入桌子确认信息"); // 在桌子上坐下
			stEnterGameRetCommonMiniGameCmd * cmd = (stEnterGameRetCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->ConfirmEntryGame(cmd);
		}	
		break;
		//case HOST_GAME_COMMON_MINI_PARA:  
		//	{
		//		TRACE("设定桌主"); // 在桌子上坐下
		//		stHostGameCommonMiniGameCmd * cmd = (stHostGameCommonMiniGameCmd *)pCmd;
		//		CLordStrikeManager::GetSingleton()->GetLobby()->SetTabler(cmd);
		//	}
		//	break;
	case SEAT_STATE_COMMON_MINI_PARA:
		{
			TRACE("刷新桌子开关状态，包括桌主信息");
			stSeatStateCommonMiniGameCmd * cmd = (stSeatStateCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->UpdateSeatState(cmd);
		}
		break;
		//case ADD_GAME_USER_COMMON_MINI_PARA:
		//	{
		//		TRACE("添加|更新游戏玩家列表");
		//		stAddGameUserCommonMiniGameCmd * cmd = (stAddGameUserCommonMiniGameCmd *)pCmd;
		//		CLordStrikeManager::GetSingleton()->GetLobby()->AddGameUser(cmd); 
		//	}
		//	break;
	case GAME_END_COMMON_MINI_PARA:
		{
			TRACE("游戏结束了");
			stGameEndCommonMiniGameCmd * cmd = (stGameEndCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetGame()->EndGame(cmd); 
		}
		break;
	case UPDATE_USER_STATE_COMMON_MINI_PARA:
		{
			TRACE("刷新玩家状态");
			stUpdateUserStateCommonMiniGameCmd * cmd = (stUpdateUserStateCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->UpdateUserState(cmd);
		}
		break;
	case GAME_START_COMMON_MINI_PARA:
		{
			TRACE("开始比赛");
			stGameStartCommonMiniGameCmd * cmd= (stGameStartCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->StartGame(cmd);
		}
		break;
	case USER_SCORE_COMMON_MINI_PARA:  // 登陆房间，服务器发玩家自己的分数过来
		{
			TRACE("服务器刷新玩家总分数");
			stUserScoreCommonMiniGameCmd * cmd= (stUserScoreCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->GetLSScore().UpdateTotalPlayInfo(cmd);
		}
		break;
	case GAME_RESULT_COMMON_MINI_PARA:  // 每局的分数
		{
			TRACE("每局的分数");
			stGameResultCommonMiniGameCmd * pcmd = (stGameResultCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->GetLSScore().UpdatePlayerPerInfo(pcmd);
		}
		break;
	case CHAT_COMMON_MINI_PARA:
		{  // chat 
			stChatCommonMiniGameCmd * pcmd = (stChatCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->SomeChat(pcmd);
		}
		break;
	case RET_TOP_COMMON_MINI_PARA:
		{
			stRetTopCommonMiniGameCmd * __pcmd = (stRetTopCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->AddSortList(__pcmd);
		}
		break;
	case DRAW_RET_COMMON_MINI_PARA:
		{
			stDrawRetCommonMiniGameCmd  * __pcmd = (stDrawRetCommonMiniGameCmd *)pCmd;
			CLordStrikeManager::GetSingleton()->GetLobby()->ParseDrawRetCmd(__pcmd);
		}
		break;
	case USER_DATA_COMMON_MINI_PARA:
		{
                        stUserDataCommonMiniGameCmd  * __pcmd = (stUserDataCommonMiniGameCmd  *)pCmd;
			CLordStrikeManager::GetSingleton()->UpdateMainUserInfo(__pcmd);
		}
		break;
	}
	return false;
}


// 所有游戏消息必须填上gameid
bool ParseLordGameMessage(stCardMiniGameCmd * pcmd,size_t size)
{
	switch(pcmd->subParam)
	{
	case DEAL_CARD_MINI_PARA: // 发牌
		{
			stDealCardMiniGameCmd  * cmd = (stDealCardMiniGameCmd *)pcmd;
			CLordStrikeManager::GetSingleton()->GetGame()->DealCard(cmd);
		}
		break;
	case NOTIFY_PUT_CARD_MINI_PARA:  // //通知某人出牌
		{
			stNotifyPutCardMiniGameCmd   * cmd = (stNotifyPutCardMiniGameCmd  *)pcmd;
			CLordStrikeManager::GetSingleton()->GetGame()->NotifyPutCard(cmd);
		}
		break;
	case PUT_CARD_MINI_PARA: // 出牌
		{
			stPutCardMiniGameCmd   * cmd = (stPutCardMiniGameCmd  *)pcmd;
			CLordStrikeManager::GetSingleton()->GetGame()->PutCard(cmd);
		}
		break;
	case NOTIFY_POINT_DDZ_MINI_PARA:  // //通知某人叫分
		{
			stNotifyPointDDZMiniGameCmd   * cmd = (stNotifyPointDDZMiniGameCmd  *)pcmd;
			CLordStrikeManager::GetSingleton()->GetGame()->NotifyPointDDZ(cmd);
		}
		break;
	case POINT_DDZ_MINI_PARA:// 他叫了多少分
		{
			stPointDDZMiniGameCmd * cmd = (stPointDDZMiniGameCmd *)pcmd;
			CLordStrikeManager::GetSingleton()->GetGame()->DDZPoint(cmd);
		}
		break;
	case RESERVE_CARDS_DDZ_MINI_PARA: // 底牌发给谁
		{
			stReserveCardsDDZMiniGameCmd * cmd = (stReserveCardsDDZMiniGameCmd *)pcmd;
			CLordStrikeManager::GetSingleton()->GetGame()->SetReverseCards(cmd);
		}
		break;
	}
	return false;
}