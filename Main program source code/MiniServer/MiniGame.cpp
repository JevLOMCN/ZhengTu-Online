//#include "MiniUserCommand.h"
#include "MiniHall.h"
#include "MiniRoom.h"
#include "MiniGame.h"
#include "MiniUser.h"
#include <stdarg.h>

MiniGame::MiniGame(Cmd::MiniGameID i, BYTE n, DWORD m)
	:id(i),state(Cmd::MGS_PREPARE),minUserNum(3),maxUserNum(n),curUserNum(0),money(m)
{
	seatList.resize(n+1, Seat());
}

bool MiniGame::userEnter(MiniUser *u, Cmd::MiniUserPosition seatID)
{
	if (!u) return false;

	Cmd::stEnterGameRetCommonMiniGameCmd send;
	if (money && !u->checkMoney(money*30))
	{
		send.ret = 5;//钱不足
		u->sendCmdToMe(&send, sizeof(send));
		return false;
	}
	if (full())
	{
		send.ret = 2;//满
		u->sendCmdToMe(&send, sizeof(send));
		return false;
	}
	if (Cmd::MGS_PREPARE!=state)
	{
		send.ret = 3;//已开始
		u->sendCmdToMe(&send, sizeof(send));
		return false;
	}
	if (seatList[seatID.seat].user)
	{
		send.ret = 4;//已有人
		u->sendCmdToMe(&send, sizeof(send));
		return false;
	}
	if (find(u) || seatID.seat>maxUserNum
			|| u->getGameState(id.type)!=Cmd::MUS_ROOM
			|| seatList[seatID.seat].open!=1)
	{
		send.ret = 0;//失败
		u->sendCmdToMe(&send, sizeof(send));
		return false;
	}

	seatList[seatID.seat].user = u;

	u->setGameState(id.type, Cmd::MUS_SEAT);
	u->setGamePos(id.type, seatID);

	send.ret = 1;//成功
	send.seatID = seatID;
	u->sendCmdToMe(&send, sizeof(send));

	//sendUserToGame(u);
	//sendGameToUser(u);

	getRoom()->sendUserStateToRoom(u);
	curUserNum++;

	if (curUserNum==1)
		setHost(seatID.seat);

	v_userEnter(u, seatID);
	Zebra::logger->debug("%s(%u) 坐下 seatID=%u", u->name, u->id, seatID.id());
	return true;
}

void MiniGame::userLeave(MiniUser *u)
{
	BYTE seat=find(u);
	if (!u || !seat) return;

	v_userLeave(u);

	if (Cmd::MGS_PLAY==state)
		end();

	//if (Cmd::MGS_PREPARE==state)
	{
		seatList[seat].user = 0;
		u->setGameState(id.type, Cmd::MUS_ROOM);
		u->setGamePos(id.type, Cmd::MiniRoomID(id.type, id.room));

		/*
		Cmd::stLeaveGameCommonMiniGameCmd send;
		send.gameID = id;
		send.userID = u->id;
		sendCmdToAll(&send ,sizeof(send));
		*/
	}

	/*
	if (seat==hostSeat)
	{
		BYTE i = nextUserSeat(seat);
		setHost(i);
		hostSeat = i;
	}
	*/

	//getRoom()->sendUserToRoom(u);
	//getRoom()->sendRoomToUser(u);
	getRoom()->sendUserStateToRoom(u);
	u->sendGameScore();

	curUserNum--;
	if (empty())
	{
		//hostSeat = 0;
		enableAllSeats();
	}

	Zebra::logger->debug("%s(%u) 离开桌子 %u", u->name, u->id, makeSeatID(seat).id());
}

//返回座位号
BYTE MiniGame::find(MiniUser *u)
{
	if (!u) return 0;
	for (BYTE i=1; i<=maxUserNum; i++)
		if (seatList[i].user==u)
			return i;

	return 0;
}

bool MiniGame::empty()
{
	return curUserNum==0;
}

bool MiniGame::full()
{
	return curUserNum>=maxUserNum;
}

/*
void MiniGame::sendUserToGame(MiniUser *u)
{
	if (!u || !find(u)) return;

	char buf[zSocket::MAX_DATASIZE];
	bzero(buf, sizeof(buf));
	Cmd::stAddGameUserCommonMiniGameCmd * cmd = (Cmd::stAddGameUserCommonMiniGameCmd *)buf;
	constructInPlace(cmd);
	cmd->gameID = id;

	cmd->num = 1;
	u->full_MiniUserData(id.type, cmd->data[0]);

	sendCmdToAll(cmd, sizeof(Cmd::stAddGameUserCommonMiniGameCmd)+sizeof(Cmd::MiniUserData));

	Zebra::logger->debug("sendUserToGame 广播 %s(%u) 的信息", u->name, u->id);
}

void MiniGame::sendGameToUser(MiniUser *u)
{
	if (!u || !find(u)) return;

	char buf[zSocket::MAX_DATASIZE];
	bzero(buf, sizeof(buf));
	Cmd::stAddGameUserCommonMiniGameCmd * cmd = (Cmd::stAddGameUserCommonMiniGameCmd *)buf;
	constructInPlace(cmd);
	cmd->gameID = id;

	for (BYTE i=1; i<=maxUserNum; i++)
	{
		if (seatList[i].user)
		{
			seatList[i].user->full_MiniUserData(id.type, cmd->data[cmd->num]);
			cmd->num++;
		}
	}

	if (cmd->num)
	{
		u->sendCmdToMe(cmd, sizeof(Cmd::stAddGameUserCommonMiniGameCmd)+sizeof(Cmd::MiniUserData));
		Zebra::logger->debug("sendGameToUser 给 %s(%u) 发送 %u 个玩家信息", u->name, u->id, cmd->num);
	}
}
*/

void MiniGame::sendCmdToAll(const void *cmd, const unsigned int len) const
{
	if (!cmd || !len) return;

	for (BYTE i=1; i<=maxUserNum; i++)
		if (seatList[i].user)
			seatList[i].user->sendCmdToMe(cmd, len);
}

void MiniGame::setHost(BYTE seat)
{
	/*
	if (seat>maxUserNum)
	{
		Zebra::logger->error("gameID=%u 设置房间主人错误 seat=%u", id.id(), seat);
		return;
	}

	if (hostSeat)
	{
		Cmd::stOneSeatStateCommonMiniGameCmd send;
		send.data.seatID = makeSeatID(hostSeat);
		send.data.state = seatList[hostSeat].open;
		send.data.isHost = 0;
		getRoom()->sendCmdToAll(&send, sizeof(send));

		Zebra::logger->debug("清除桌主 gameID=%u seat=%u", id.id(), hostSeat);
	}

	hostSeat = seat;
	if (hostSeat)
	{
		seatList[hostSeat].user->setGameState(id.type, Cmd::MUS_SEAT);
		getRoom()->sendUserStateToRoom(seatList[hostSeat].user);

		Cmd::stOneSeatStateCommonMiniGameCmd send;
		send.data.seatID = makeSeatID(hostSeat);
		send.data.state = seatList[hostSeat].open;
		send.data.isHost = 1;
		getRoom()->sendCmdToAll(&send, sizeof(send));

		Zebra::logger->debug("设置桌主 gameID=%u seat=%u", id.id(), seat);
	}
	*/
}

void MiniGame::toggleSeat(MiniUser *host, Cmd::MiniSeatID seatID)
{
	/*
	using namespace Cmd;

	if (Cmd::MGS_PREPARE!=state) return;
	if (!host || host!=seatList[hostSeat].user) return;
	if (!seatID.seat || seatID.seat>maxUserNum || seatList[seatID.seat].user) return;

	BYTE c=0;
	for (BYTE i=1; i<=maxUserNum; i++) if (seatList[i].open) c++;
	if (c<=minUserNum) return;

	seatList[seatID.seat].open = seatList[seatID.seat].open?0:1;

	Cmd::stOneSeatStateCommonMiniGameCmd send;
	send.data.seatID = seatID;
	send.data.state = seatList[seatID.seat].open;
	send.data.isHost = 0;
	sendCmdToAll(&send, sizeof(send));

	getRoom()->sendCmdToAll(&send, sizeof(send));

	Zebra::logger->debug("%s(%u) 设置座位状态 %u state=%u", host->name, host->id, seatID.id(), seatList[seatID.seat].open);
	*/
}

void MiniGame::kickUser(MiniUser *host, MiniUser *u)
{
	/*
	if (Cmd::MGS_PREPARE!=state) return;
	if (!host || !u) return;

	BYTE seat = find(u);
	if (!seat) return;
	if (seat!=hostSeat) return;

	userLeave(u);

	Zebra::logger->debug("%s 被 %s 踢出游戏，进入房间 %u", u->name, host->name, id.roomID());
	*/
}

void MiniGame::full_MiniSeatData(Cmd::stSeatStateCommonMiniGameCmd *cmd)
{
	for (BYTE i=1; i<=maxUserNum; i++)
	{
		if (0==seatList[i].open)
		{
			cmd->data[cmd->num].seatID = makeSeatID(i);
			cmd->data[cmd->num].state = seatList[i].open;
			cmd->num++;
		}
	}

	/*//带桌主的代码
	for (BYTE i=1; i<=maxUserNum; i++)
	{
		if (0==seatList[i].open || i==hostSeat)
		{
			cmd->data[cmd->num].seatID = makeSeatID(i);
			cmd->data[cmd->num].state = seatList[i].open;
			cmd->data[cmd->num].isHost = i==hostSeat?1:0;
			cmd->num++;
		}
	}
	*/
}

Cmd::MiniGameState MiniGame::getState()
{
	return state;
}

bool MiniGame::toggleReady(MiniUser *u)
{
	BYTE seat = find(u);
	if (!seat) return false;

	/* //带桌主的代码
	if (seat==hostSeat)
	{
		if (canStart())
		{
			start();
			return true;
		}
		u->sendSys(Cmd::INFO_TYPE_FAIL, "所有人都准备好才能开始游戏");
		return false;
	}
	*/

	if (u->getGameState(id.type)!=Cmd::MUS_READY)
		u->setGameState(id.type, Cmd::MUS_READY);
	else
		u->setGameState(id.type, Cmd::MUS_SEAT);

	getRoom()->sendUserStateToRoom(u);

	Zebra::logger->debug("%s(%u) %s %u", u->name, u->id, u->getGameState(id.type)==Cmd::MUS_READY?"准备":"取消准备", makeSeatID(seat).id());

	if (canStart())
		start();

	return true;
}

bool MiniGame::canStart()
{
	BYTE count = 0;
	for (BYTE i=1; i<=maxUserNum; i++)
	{
		//if (i==hostSeat) continue;
		if (seatList[i].user)
			if (Cmd::MUS_READY!=seatList[i].user->getGameState(id.type))
				return false;
			else
				count++;
	}
	return count>=minUserNum;
}

bool MiniGame::start()
{

	Cmd::stGameStartCommonMiniGameCmd s;
	s.gameID = id;
	sendCmdToAll(&s, sizeof(s));

	for (BYTE i=1; i<=maxUserNum; i++)
	{
		if (seatList[i].user)
		{
			seatList[i].user->setGameState(id.type, Cmd::MUS_PLAY);
			getRoom()->sendUserStateToRoom(seatList[i].user);
		}
	}

	state = Cmd::MGS_PLAY;

	if (!v_start()) return false;

	Zebra::logger->debug("%u 游戏开始", id.gameID());
	return true;
}

bool MiniGame::end()
{
	v_end();

	Cmd::stGameEndCommonMiniGameCmd e;
	e.gameID = id;
	sendCmdToAll(&e, sizeof(e));

	for (BYTE i=1; i<=maxUserNum; i++)
	{
		if (seatList[i].user)
		{
			seatList[i].user->setGameState(id.type, Cmd::MUS_SEAT);
			seatList[i].user->sendGameScore();
			getRoom()->sendUserStateToRoom(seatList[i].user);
		}
	}

	state = Cmd::MGS_PREPARE;

	//钱不够的踢出去
	for (BYTE i=1; i<=maxUserNum; i++)
		if (seatList[i].user && !seatList[i].user->checkMoney(money*30))
		{
			seatList[i].user->sendMiniInfo(Cmd::MCT_POPUP, "对不起，你的仙丹储备不足 %u 粒，不能再继续游戏", money*30);
			Zebra::logger->trace("%s(%u) 金钱不足被踢出游戏 gameID=%u", seatList[i].user->name, seatList[i].user->id, id.id());
			userLeave(seatList[i].user);
		}

	Zebra::logger->debug("%02u%02u%02u%02u 游戏结束", id.type, id.room, id.game, id.seat);
	return true;
}

MiniRoom *MiniGame::getRoom()
{
	return MiniHall::getMe().getRoom(id);
}

bool MiniGame::parseGameCmd(MiniUser *u, Cmd::stMiniGameUserCmd *cmd, DWORD len)
{
	if (Cmd::MGS_PLAY!=state) return false;
	if (cmd->byParam==COMMON_MINI_PARA) return false;
	return (v_parseGameCmd(u, cmd, len));
}

BYTE MiniGame::nextUserSeat(BYTE from)
{
	BYTE ret = from%maxUserNum+1;
	while (!seatList[ret].user && ret!=from)
		ret = ret%maxUserNum+1;

	if (!seatList[ret].user) return 0;

	return ret;
}

void MiniGame::enableAllSeats()
{
	char buf[zSocket::MAX_DATASIZE];
	bzero(buf, sizeof(buf));
	Cmd::stSeatStateCommonMiniGameCmd * cmd = (Cmd::stSeatStateCommonMiniGameCmd *)buf;
	constructInPlace(cmd);

	for (BYTE i=1; i<=maxUserNum; i++)
		if (0==seatList[i].open)
		{
			seatList[i].open = 1;

			cmd->data[cmd->num].seatID = makeSeatID(i);
			cmd->data[cmd->num].state = 1;
			cmd->data[cmd->num].isHost = 0;
			cmd->num++;
		}
	if (cmd->num)
		getRoom()->sendCmdToAll(cmd, sizeof(Cmd::stSeatStateCommonMiniGameCmd)+sizeof(Cmd::MiniSeatData)*cmd->num);
}

void MiniGame::timer()
{
	if (state==Cmd::MGS_PLAY)
		v_timer();
}
#define getMessage(msg,msglen,pat)  \
	do  \
{   \
	va_list ap; \
	bzero(msg, msglen); \
	va_start(ap, pat);      \
	vsnprintf(msg, msglen - 1, pat, ap);    \
	va_end(ap); \
}while(false)
void MiniGame::sendInfoToAll(const int type, const char *info, ...) const
{
	using namespace Cmd;

	stChatCommonMiniGameCmd send;
	send.type = MCT_SYS;
	send.pos = id;
	getMessage(send.content, sizeof(send.content), info);

	for (BYTE i=1; i<=maxUserNum; i++)
		if (seatList[i].user)
			seatList[i].user->sendCmdToMe(&send, sizeof(send));
}

