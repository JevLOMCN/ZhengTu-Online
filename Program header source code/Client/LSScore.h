#pragma once 

class CLSScore
{
public:
	CLSScore(){};
	~CLSScore(){};

	//////////////////////////////////////////////////////////////////////
	void UpdateTotalPlayInfo(stUserScoreCommonMiniGameCmd * pCmd);
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	void UpdatePlayerPerInfo(stGameResultCommonMiniGameCmd *cmd);
	////////////////////////////////////////////////////////////////////

	struct stGameScorePer
	{
		MiniGameID gameID;
		DWORD userID;
		MiniGameScore score;
		void operator= (const stGameScorePer & info)
		{
			gameID = info.gameID;
			userID = info.userID;
			score = info.score;
		}
	};
	typedef std::map<DWORD,stGameScorePer> MapPlayerInfoPer;
	typedef std::map<DWORD,stGameScorePer>::iterator IterPlayerInfoPer;

	MapPlayerInfoPer & GetMapPerGameScore()
	{
		return m_MapPerGameScore;
	}

	void BeginNewTurn()
	{
		 m_MapPerGameScore.clear();
	}
	void EndNewTurn()
	{
                m_MapPerGameScore.clear();
	}
private:
        // 用于记录每一轮玩家的分数
	std::map<DWORD,stGameScorePer> m_MapPerGameScore;
};


