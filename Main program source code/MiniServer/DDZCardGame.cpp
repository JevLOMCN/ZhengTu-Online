#include "DDZCardGame.h"
#include "zSocket.h"
#include "MiniUser.h"
#include "TimeTick.h"

DDZCardGame::DDZCardGame(Cmd::MiniGameID id, BYTE userNum, DWORD p)
	:MiniGame(id, userNum, p)
{
	userCardList.resize(userNum+1, CardList());
}

bool DDZCardGame::v_start()
{
	//Zebra::logger->debug("%u 斗地主 开始", id.id());
	clean();
	initCards();
	shuffle();
	deal();
	ddz_state = DDZS_POINT;

	curPointSeat = nextPointSeat();
	lastPointSeat = curPointSeat;

	Cmd::stNotifyPointDDZMiniGameCmd send;
	send.gameID = id;
	send.userID = seatList[curPointSeat].user->id;
	sendCmdToAll(&send ,sizeof(send));
	countdown = MiniTimeTick::currentTime.sec()+60;
	return true;
}

void DDZCardGame::clean()
{
	ddz_state = DDZS_POINT;
	point = 0;
	pointTime = 1;

	lordPutTime = 0;
	otherPutTime = 0;

	curPutSeat = 0;
	lastPutSeat = 0;
	lastPattern.clear();
	countdown = 0;

	lordSeat = 0;
	rCardNum = 0;
	curPointSeat = 0;
	for (BYTE i=1; i<=maxUserNum; i++)
		userCardList[i].clear();
}

void DDZCardGame::initCards()
{
	/*
#ifdef _XWL_DEBUG
	packNum = 3;
#else
*/
	packNum = 2;//几副牌
	if (curUserNum==3) packNum = 1;
	if (curUserNum==6) packNum = 3;
//#endif

	allCardList.clear();
	for (int i=0; i<packNum; i++)
	{
		allCardList.push_back(Cmd::Card::Joker+14);
		allCardList.push_back(Cmd::Card::Joker+15);
		for (BYTE n=1; n<14; n++)
			for (BYTE s=Cmd::Card::Diamond; s<Cmd::Card::Joker; s+=0x10)
				allCardList.push_back(s+n);
	}
}

void DDZCardGame::shuffle()
{
	std::random_shuffle(allCardList.begin(), allCardList.end());
}

class m_sort
{
	public:
		int operator()(const Cmd::Card &p1, const Cmd::Card &p2)
		{
			return p1>p2;
		}
};
void DDZCardGame::deal()
{
	BYTE one = 0;
	switch (curUserNum)
	{
		case 3:
			rCardNum = 3;
			one = (allCardList.size()-rCardNum)/curUserNum;
			break;
		case 4:
			rCardNum = 8;
			one = (allCardList.size()-rCardNum)/curUserNum;
			break;
		case 5:
			rCardNum = 8;
			one = (allCardList.size()-rCardNum)/curUserNum;
			break;
		case 6:
			rCardNum = 12;
			one = (allCardList.size()-rCardNum)/curUserNum;
			break;
	}

	char buf[zSocket::MAX_DATASIZE];
	bzero(buf, sizeof(buf));
	Cmd::stDealCardMiniGameCmd * cmd = (Cmd::stDealCardMiniGameCmd *)buf;
	constructInPlace(cmd);
	cmd->gameID = id;

	BYTE n=0, userCount=0;
	for (BYTE i=1; i<=maxUserNum; i++)
	{
		if (!seatList[i].user) continue;

		std::sort(&allCardList[n], &allCardList[(userCount+1)*one], m_sort());

		cmd->num = 0;
		for ( ; n<(userCount+1)*one; n++)
		{
			userCardList[i][allCardList[n]]++;
			cmd->cards[cmd->num] = allCardList[n];
			cmd->num++;
		}
		
		seatList[i].user->sendCmdToMe(cmd, sizeof(Cmd::stDealCardMiniGameCmd)+cmd->num);
		userCount++;
		Zebra::logger->debug("%u 发牌 %s(%u):\t%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u", id.id(), seatList[i].user->name, cmd->num, cmd->cards[0].number(), cmd->cards[1].number(), cmd->cards[2].number(), cmd->cards[3].number(), cmd->cards[4].number(), cmd->cards[5].number(), cmd->cards[6].number(), cmd->cards[7].number(), cmd->cards[8].number(), cmd->cards[9].number(), cmd->cards[10].number(), cmd->cards[11].number(), cmd->cards[12].number(), cmd->cards[13].number(), cmd->cards[14].number(), cmd->cards[15].number(), cmd->cards[16].number());
	}
}

bool DDZCardGame::v_parseGameCmd(MiniUser *u, Cmd::stMiniGameUserCmd *c, DWORD len)
{
	if (!u) return false;
	if (c->byParam!=CARD_MINI_PARA) return false;

	using namespace Cmd;
	stCardMiniGameCmd *cmd = (stCardMiniGameCmd *)c;

	switch (cmd->subParam)
	{
		case PUT_CARD_MINI_PARA:
			{
				if (!u) return false;
				BYTE seat = find(u);
				if (!seat) return false;

				stPutCardMiniGameCmd *rev = (stPutCardMiniGameCmd *)cmd;

				if (canPut(seat, rev, len))
				{
					putCards(seat, rev, len);
					if (userCardList[seat].empty())
						judge(seat);
				}
				//else
				//	Zebra::logger->debug("%s 无法出牌 %u", u->name, makeSeatID(seat).id());
			}
			break;
		case POINT_DDZ_MINI_PARA:
			{
				if (ddz_state!=DDZS_POINT) return false;
				stPointDDZMiniGameCmd * rev = (stPointDDZMiniGameCmd *)cmd;
				BYTE seat = find(u);
				if (!seat || seat!=curPointSeat) return false;

				countdown = MiniTimeTick::currentTime.sec()+60;

				if ((rev->num!=0 && rev->num<=point) || rev->num>3)
				{
					Cmd::stNotifyPointDDZMiniGameCmd send;
					send.gameID = id;
					send.userID = seatList[curPointSeat].user->id;
					sendCmdToAll(&send ,sizeof(send));

					return true;
				}

				if (rev->num)
				{
					point = rev->num;
					lordSeat = curPointSeat;
					Zebra::logger->debug("%s 叫 %u 分 seatID=%u", u->name, rev->num, makeSeatID(curPointSeat).id());
				}
				else
					Zebra::logger->debug("%s 不叫分 seatID=%u", u->name, makeSeatID(curPointSeat).id());

				rev->userID = u->id;
				sendCmdToAll(rev, len);
				
				BYTE newSeat = nextPointSeat();
				if (rev->num==3 || (newSeat==lastPointSeat))//叫牌完毕
				{
					if (0==point)//没人叫分
					{
						v_start();
						Zebra::logger->debug("%u 重新发牌", id.id());
					}
					else
					{
						showReserveCards();
						ddz_state = DDZS_PLAY;
						nextPutUser(lordSeat);
						Zebra::logger->debug("%s 成为地主 seatID=%u", seatList[lordSeat].user->name, makeSeatID(lordSeat).id());
					}
				}
				else
				{
					curPointSeat = newSeat;

					Cmd::stNotifyPointDDZMiniGameCmd send;
					send.gameID = id;
					send.userID = seatList[curPointSeat].user->id;
					sendCmdToAll(&send ,sizeof(send));
				}
			}
			break;
	}
	return false;
}

void DDZCardGame::nextPutUser(BYTE seat)
{
	if (seat)
		curPutSeat = seat;
	else
		curPutSeat = nextUserSeat(curPutSeat);

	if (curPutSeat==lastPutSeat)//一圈没人要
		lastPattern.clear();

	Cmd::stNotifyPutCardMiniGameCmd send;
	send.userID = seatList[curPutSeat].user->id;
	sendCmdToAll(&send, sizeof(send));

	countdown = MiniTimeTick::currentTime.sec()+60;
}

bool DDZCardGame::canPut(BYTE seat, const Cmd::stPutCardMiniGameCmd *cmd, DWORD len)
{
	if (!seat || curPutSeat!=seat) return false;

	if (0==cmd->num)
		return lastPattern.valid();

	CardPattern p;
	DWORD n = len-sizeof(Cmd::stPutCardMiniGameCmd), m=cmd->num;
	if (!CardPattern::match_pattern(packNum, cmd->cards, std::min(m, n), p)) return false;

	if (!(p>lastPattern)) return false;
	for (card_iter it=p.list.begin(); it!=p.list.end(); it++)
		if (userCardList[seat].find(it->first)==userCardList[seat].end()
				|| userCardList[seat][it->first]<it->second)
			return false;

	if (p.bomb || p.missile)
		pointTime++;
	lastPattern = p;
	return true;
}

void DDZCardGame::putCards(BYTE seat, Cmd::stPutCardMiniGameCmd *cmd, DWORD len)
{
	if (cmd->num)
	{
		lastPutSeat = seat;

		if (seat==lordSeat)
			lordPutTime++;
		else
			otherPutTime++;
		//Zebra::logger->debug("%s(%u) 出牌 seatID=%u", seatList[seat].user->name, seatList[seat].user->id, makeSeatID(seat).id());
	}
	//else
	//	Zebra::logger->debug("%s(%u) 不跟 seatID=%u", seatList[seat].user->name, seatList[seat].user->id, makeSeatID(seat).id());

	for (DWORD i=0; i<cmd->num; i++)
	{
		userCardList[seat][cmd->cards[i]]--;
		if (0==userCardList[seat][cmd->cards[i]])
			userCardList[seat].erase(cmd->cards[i]);
	}
	std::sort(&cmd->cards[0], &cmd->cards[cmd->num]);

	cmd->userID = seatList[seat].user->id;
	sendCmdToAll(cmd, len);

	nextPutUser();
}

void DDZCardGame::judge(BYTE seat)
{
	calcNormalScore(seat);

	end();
	ddz_state = DDZS_POINT;
}

void DDZCardGame::calcFleeScore(BYTE seat)
{
	Cmd::MiniGameScore s;//逃跑者得分
	s.gameType = id.type;

	Cmd::stGameResultCommonMiniGameCmd send;
	send.gameID = id;

	//逃跑者
	s.score = (point?point:1)*(bombCount()+pointTime)*3;//逃跑者扣分 = 1 * 本桌玩家手牌总炸弹数 * 3
	s.money = (s.score>30?30:s.score)*money;
	s.score = 0-s.score;
	s.money = 0-s.money;
	seatList[seat].user->addScore(s);

	send.score = s;
	seatList[seat].user->sendCmdToMe(&send, sizeof(send));
	if (s.money)
		sendInfoToAll(Cmd::MCT_POPUP, "你逃跑，被扣掉%u点积分和%u粒仙丹", 0-s.score, 0-s.money);
	else
		sendInfoToAll(Cmd::MCT_POPUP, "你逃跑，被扣掉%u点积分", 0-s.score);

	//其他人
	s.score = (point?point:1)*(bombCount()+pointTime);
	s.money = (s.score>10?10:s.score)*money;
	//sendInfoToAll(Cmd::MCT_SYS, "你逃跑，被扣掉%u点积分和%u粒仙丹", 0-s.score, 0-s.money);
	for (BYTE i=1; i<=maxUserNum; i++)
	{
		if (i!=seat && seatList[i].user)
		{
			seatList[i].user->addScore(s);
			if (s.money)
				seatList[i].user->sendMiniInfo(Cmd::MCT_POPUP, "%s逃跑，被扣掉%u点积分和%u粒仙丹，你得到%u点积分和%u粒仙丹", seatList[seat].user->name, s.score*3, s.money*3, s.score, s.money);
			else
				seatList[i].user->sendMiniInfo(Cmd::MCT_POPUP, "%s逃跑，被扣掉%u点积分，你得到%u点积分", seatList[seat].user->name, s.score*3, s.score);
		}
	}

	Zebra::logger->debug("%s 逃跑\twin:%u lose:%u score:%d", seatList[seat].user->name, s.win, s.lose, s.score);
}

void DDZCardGame::calcNormalScore(BYTE seat)
{
	Cmd::MiniGameScore s;
	s.gameType = id.type;

	Cmd::stGameResultCommonMiniGameCmd send;
	send.gameID = id;

	//地主
	if (seat==lordSeat)
	{
		s.win = 1;
		s.score = point*pointTime*(curUserNum-1);
		if (otherPutTime==0) s.score *= 2;
		s.money = (s.score>30?30:s.score)*money;
	}
	else
	{
		s.lose = 1;
		s.score = 0-point*pointTime*(curUserNum-1);
		if (lordPutTime==1) s.score *= 2;
		s.money = (s.score<-30?-30:s.score)*money;
	}
	send.userID = seatList[lordSeat].user->id;
	seatList[lordSeat].user->addScore(s);

	send.score = s;
	sendCmdToAll(&send, sizeof(send));
	Zebra::logger->debug("%s\t地主得分\twin:%u lose:%u score:%d", seatList[lordSeat].user->name, s.win, s.lose, s.score);

	//其他
	s.win = 0;
	s.lose = 0;
	if (seat==lordSeat)
	{
		s.lose = 1;
		s.score = 0-point*pointTime;
		if (otherPutTime==0) s.score *= 2;
		s.money = (s.score>15?15:s.score)*money;
	}
	else
	{
		s.win = 1;
		s.score = point*pointTime;
		if (lordPutTime==1) s.score *= 2;
		s.money = (s.score<-15?-15:s.score)*money;
	}
	send.score = s;
	for(BYTE i=1; i<=maxUserNum; i++)
	{
		if (i!=lordSeat && seatList[i].user)
		{
			send.userID = seatList[i].user->id;
			seatList[i].user->addScore(s);
			sendCmdToAll(&send, sizeof(send));
			//seatList[i].user->sendCmdToMe(&send, sizeof(send));
			Zebra::logger->debug("%s\t农民得分\twin:%u lose:%u score:%d", seatList[i].user->name, s.win, s.lose, s.score);
		}
	}
}

BYTE DDZCardGame::nextPointSeat()
{
	if (!lastPointSeat) return nextUserSeat(0);
	//if (!lastPointSeat) return nextUserSeat(hostSeat);
	if (!curPointSeat) return nextUserSeat(lastPointSeat);
	return  nextUserSeat(curPointSeat);
}

void DDZCardGame::v_userLeave(MiniUser *u)
{
	BYTE seat = find(u);

	if (state==Cmd::MGS_PLAY && seat)
		calcFleeScore(seat);

	ddz_state = DDZS_POINT;

	if (curUserNum==1)
		lastPointSeat = 0;
}

void DDZCardGame::showReserveCards()
{
	char buf[zSocket::MAX_DATASIZE];
	bzero(buf, sizeof(buf));
	Cmd::stReserveCardsDDZMiniGameCmd * cmd = (Cmd::stReserveCardsDDZMiniGameCmd *)buf;
	constructInPlace(cmd);
	cmd->gameID = id;
	cmd->userID = seatList[lordSeat].user->id;

	cmd->num = rCardNum;
	BYTE start = allCardList.size()-rCardNum;
	for (BYTE i=start; i<allCardList.size(); i++)
		userCardList[lordSeat][allCardList[i]]++;
	std::copy(&allCardList[start], &allCardList[allCardList.size()], cmd->cards);
	sendCmdToAll(cmd, sizeof(Cmd::stReserveCardsDDZMiniGameCmd)+rCardNum);
}

void DDZCardGame::v_timer()
{
	if (ddz_state==DDZS_POINT)
	{
		if (MiniTimeTick::currentTime.sec()>=countdown)
			auto_point();
	}
	if (ddz_state==DDZS_PLAY)
	{
		if (MiniTimeTick::currentTime.sec()>=countdown)
			auto_put();
	}
}

void DDZCardGame::auto_point()
{
	Cmd::stPointDDZMiniGameCmd send;
	send.gameID = id;
	send.userID = seatList[curPointSeat].user->id;
	send.num = 0;
	v_parseGameCmd(seatList[curPointSeat].user, &send, sizeof(send));
}

void DDZCardGame::auto_put()
{
	userLeave(seatList[curPutSeat].user);
	/*
	char buf[zSocket::MAX_DATASIZE];
	bzero(buf, sizeof(buf));
	Cmd::stPutCardMiniGameCmd * cmd = (Cmd::stPutCardMiniGameCmd *)buf;
	constructInPlace(cmd);
	cmd->gameID = id;
	cmd->userID = seatList[curPutSeat].user->id;

	if (!lastPattern.valid())
	{
		cmd->num = userCardList[curPutSeat].begin()->second;
		for (BYTE i=0; i<cmd->num; i++)
			cmd->cards[i] = userCardList[curPutSeat].begin()->first;
	}
	else
		cmd->num = 0;
	v_parseGameCmd(seatList[curPutSeat].user, cmd, sizeof(Cmd::stPutCardMiniGameCmd)+cmd->num);
	*/
}

BYTE DDZCardGame::bombCount()
{
	BYTE count = 0;

	BYTE joker = 0;
	for(BYTE i=1; i<=maxUserNum; i++)
	{
		joker = 0;
		for (card_iter it=userCardList[i].begin(); it!=userCardList[i].end(); it++)
		{
			if (it->second>=4)
				count++;
			if (it->first.number()>13)
				joker++;
		}
		if (joker==packNum*2)
			count++;
	}
	return count;
}

