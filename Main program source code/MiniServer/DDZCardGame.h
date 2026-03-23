#pragma once
#include "MiniGame.h"
#include <map>
#include <vector>
#include "CardPattern.h"

class DDZCardGame : public MiniGame
{
	public:
		DDZCardGame(Cmd::MiniGameID id, BYTE userNum, DWORD money);
		bool v_start();
		void v_userLeave(MiniUser *u);
		bool v_parseGameCmd(MiniUser *u, Cmd::stMiniGameUserCmd *cmd, DWORD len);
		void v_timer();

	private:
		enum DDZ_State
		{
			DDZS_POINT,//叫分
			DDZS_PLAY//开始
		}ddz_state;

		std::vector<Cmd::Card> allCardList;
		std::vector<CardList> userCardList;

		BYTE packNum;//几副牌
		BYTE point;//分数
		BYTE pointTime;//倍率
		BYTE rCardNum;//保留几张底牌
		BYTE lordSeat;//地主的位置

		BYTE lordPutTime;//地主出过几次牌
		BYTE otherPutTime;//其他人出过几次牌

		BYTE curPutSeat;//当前出牌的人
		BYTE lastPutSeat;//最后一个出牌的人
		CardPattern lastPattern;//最后出的牌

		BYTE curPointSeat;//当前叫牌的人
		BYTE lastPointSeat;//上一局先叫牌的人

		DWORD countdown;//计时

		void clean();
		void shuffle();
		void deal();
		void initCards();
		void showReserveCards();//显示底牌
		BYTE nextPointSeat();
		void nextPutUser(BYTE seat=0);
		void judge(BYTE seat);

		void auto_put();
		void auto_point();

		void calcNormalScore(BYTE seat);//计算得分
		void calcFleeScore(BYTE seat);//玩家逃跑计分

		bool canPut(BYTE seat, const Cmd::stPutCardMiniGameCmd *cmd, DWORD len);
		//int compare(const std::list<Cmd::Card> &list1, const std::list<Cmd::Card> &list2);
		void putCards(BYTE seat, Cmd::stPutCardMiniGameCmd *cmd, DWORD len);

		BYTE bombCount();
};
