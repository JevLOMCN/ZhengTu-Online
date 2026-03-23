#include "public.h"
#include "miniusercommand.h"
#include "GameGuiManager.h"
#include "Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "LordStrikeGameBkDlg.h"
#include "LordStrikeRoomDlg.h"
#include "LSScoreDlg.h"
//#include "LordStrikeGame.h"
//#include ".\lordstrikecard.h"
//#include ".\lordstrikePlayer.h"
#include "LSScore.h"

// 右上角的总分统计
void CLSScore::UpdateTotalPlayInfo(stUserScoreCommonMiniGameCmd * cmd)
{
	stMainUserInfo& mainuserinfo = CLordStrikeManager::GetSingleton()->GetMainUserInfo();
	assert(cmd->userID == mainuserinfo.dwMainUserID);

	if(cmd->userID == mainuserinfo.dwMainUserID)
	{
		mainuserinfo.userdata.score = cmd->score;
		if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->UpdateMainPlayerScore(cmd->score);
		if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
			CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->UpdateMainPlayerScore(cmd->score);
	}
}

//通知游戏结果/分数(每局的)
void CLSScore::UpdatePlayerPerInfo(stGameResultCommonMiniGameCmd *cmd)
{
	 MiniGameScore MinitotalScore;
         IterPlayerInfoPer iter = m_MapPerGameScore.begin();
	 iter = m_MapPerGameScore.find(cmd->userID);
	 if(iter != m_MapPerGameScore.end())
	 {
		stGameScorePer & info = iter->second;
		assert(info.gameID == cmd->gameID);
		info.score += cmd->score;
		MinitotalScore = info.score;
	 }
	 else 
	 {
		 stGameScorePer info;
		 info.gameID = cmd->gameID;
		 info.score = cmd->score;
		 info.userID = cmd->userID;
		 m_MapPerGameScore[cmd->userID] = info; 
		 MinitotalScore = info.score;
	 }

	 if(CLordStrikeManager::GetSingleton()->GetMainUserInfo().dwMainUserID == cmd->userID)
	 { // 如果是主角,则更新右上角主角分数情况
		stMainUserInfo& mainUserInfo = CLordStrikeManager::GetSingleton()->GetMainUserInfo();
		mainUserInfo.userdata.score = MinitotalScore;
		if(cmd->score.score >0 )
			PlayUISound(91012);
		else if(cmd->score.score < 0)
			PlayUISound(91013);

		if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
			 CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->UpdateMainPlayerScore(mainUserInfo.userdata.score);
		if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
				 CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->UpdateMainPlayerScore(mainUserInfo.userdata.score);
	 }
	
	 stGameInfo & gameInfo = CLordStrikeManager::GetSingleton()->GetLobby()->GetGameInfo();
	 if(gameInfo.IsUserInGame(cmd->userID))
	 {
		 // 弹出本次统计框
		 CLordStrikeManager::GetSingleton()->AddLordStrikeLSScoreDlg();
		 if(CLordStrikeManager::GetSingleton()->m_guiLSScoreDlg)
		 {
			 CLordStrikeManager::GetSingleton()->m_guiLSScoreDlg->UpdataList(cmd->userID,cmd->score,MinitotalScore);
		 }
	 }
	 
	 // 更新列表框中的数据
	 if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
		 CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom->UpdatePlayerScore(cmd);

	 if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk  && CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->IsVisible())
		CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk->UpdatePlayerScore(cmd);
}

