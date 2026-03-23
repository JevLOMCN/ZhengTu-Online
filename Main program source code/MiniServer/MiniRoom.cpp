#include "MiniHall.h"
#include "MiniRoom.h"
#include "MiniGame.h"
#include "MiniUser.h"
#include "DDZCardGame.h"

MiniRoom::MiniRoom(){}

DWORD MiniRoom::init(Cmd::MiniRoomID i, DWORD gameNum, DWORD userNum, DWORD money)
{
	id = i;
	oneGameUserNum = userNum;

	Cmd::MiniRoomID tempid;
	for (DWORD i=1; i<=gameNum; i++)
	{
		tempid = id;
		tempid.game += i;
		MiniGame *m = createGame(tempid, oneGameUserNum, money);
		if (!m) continue;

		gameList[tempid.gameID()] = m;
	}
	return gameList.size();
}

MiniGame *MiniRoom::createGame(Cmd::MiniGameID id, DWORD oneGameUserNum, DWORD money)
{
	switch (id.type)
	{
		case Cmd::DOUDIZHU:
			{
				return new DDZCardGame(id, oneGameUserNum, money);
			}
			break;
		default:
			break;
	}
	return 0;
}

BYTE MiniRoom::gameType() const
{
	return id.type;
}

void MiniRoom::userLeave(MiniUser *u)
{
	if (!u) return;
	using namespace Cmd;

	Cmd::MiniUserPosition p = u->getGamePos(gameType());
	Cmd::MiniUserState s = u->getGameState(gameType());
	if (MUS_SEAT==s
			||MUS_READY==s
			||MUS_PLAY==s)
	{
		MiniGame *g = getGame(p);
		if (g) g->userLeave(u);
	}

	if (MUS_ROOM==u->getGameState(gameType()))
	{
		/*
		stLeaveRoomCommonMiniGameCmd send;
		send.roomID = id;
		send.userID = u->id;
		sendCmdToAll(&send, sizeof(send));
		*/

		u->setGameState(gameType(), MUS_NOTPLAY);
		u->setGamePos(gameType(), MiniUserPosition(gameType()));

		sendUserStateToRoom(u);

		userList.erase(u);

		MiniHall::getMe().updateRoomUserNum(id, userList.size());
		MiniHall::getMe().sendRoomData(u);
		Zebra::logger->debug("%s(%u) 离开房间 %u", u->name, u->id, id.id());
	}
}

bool MiniRoom::userEnter(MiniUser *u)
{
	if (!u) return false;

	Cmd::stEnterRoomRetCommonMiniGameCmd send;
	if (full())
	{
		send.ret = 2;//满
		u->sendCmdToMe(&send, sizeof(send));
		return false;
	}

	if (u->getGameState(gameType())!=Cmd::MUS_NOTPLAY)
	{
		send.ret = 3;//重复进入
		u->sendCmdToMe(&send, sizeof(send));
		return false;
	}

	u->setGamePos(gameType(), id);
	u->setGameState(gameType(), Cmd::MUS_ROOM);
	userList.insert(u);

	send.ret = 1;//成功
	send.roomID = id;
	u->sendCmdToMe(&send, sizeof(send));

	sendUserToRoom(u);
	sendRoomToUser(u);

	Zebra::logger->error("%s(%u) 进入房间 roomID=%u", u->name, u->id, id.roomID());
	return true;
}

bool MiniRoom::full() const
{
	return userList.size()>=gameList.size()*oneGameUserNum;
}

void MiniRoom::sendRoomToUser(MiniUser *u)
{
	if (!u || userList.find(u)==userList.end()) return;

	char buf[zSocket::MAX_DATASIZE];
	bzero(buf, sizeof(buf));
	Cmd::stAddRoomUserCommonMiniGameCmd * cmd = (Cmd::stAddRoomUserCommonMiniGameCmd *)buf;
	constructInPlace(cmd);
	cmd->roomID = id;

	for (user_iter it=userList.begin(); it!=userList.end(); it++)
	{
		(*it)->full_MiniUserData(gameType(), cmd->data[cmd->num]);
		cmd->num++;
		//Zebra::logger->debug("sendRoomToUser 给 %s(%u) 发送玩家信息 %s(%u)", u->name, u->id, (*it)->name, (*it)->id);
	}
	if (cmd->num)
		u->sendCmdToMe(cmd, sizeof(Cmd::stAddRoomUserCommonMiniGameCmd)+sizeof(Cmd::MiniUserData)*cmd->num);

	bzero(buf, sizeof(buf));
	Cmd::stSeatStateCommonMiniGameCmd * send = (Cmd::stSeatStateCommonMiniGameCmd *)buf;
	constructInPlace(send);

	for (game_iter it=gameList.begin(); it!=gameList.end(); it++)
		it->second->full_MiniSeatData(send);

	if (send->num)
	{
		u->sendCmdToMe(send, sizeof(Cmd::stSeatStateCommonMiniGameCmd)+sizeof(Cmd::MiniSeatData)*send->num);
		//Zebra::logger->debug("sendRoomToUser 给 %s(%u) 发送 %u 个座位信息", u->name, u->id, send->num);
	}
}

void MiniRoom::sendUserToRoom(MiniUser *u)
{
	if (!u || userList.find(u)==userList.end()) return;

	Cmd::stAddOneRoomUserCommonMiniGameCmd send;
	send.roomID = id;
	u->full_MiniUserData(gameType(), send.data);
	//Zebra::logger->debug("sendUserToRoom 广播 %s(%u) 的信息", u->name, u->id);

	sendCmdToAll(&send, sizeof(send));
}

void MiniRoom::sendUserStateToRoom(MiniUser *u)
{
	if (!u || userList.find(u)==userList.end()) return;

	Cmd::stUpdateUserStateCommonMiniGameCmd send;
	//send.roomID = id;
	//u->full_MiniUserData(gameType(), send.data);
	send.userID = u->id;
	send.state = u->getGameState(gameType());
	send.pos = u->getGamePos(gameType());
	//Zebra::logger->debug("sendUserStateToRoom %s(%u) state=%u pos=%u", u->name, u->id, send.state, send.pos.id());

	sendCmdToAll(&send, sizeof(send));
}

void MiniRoom::sendCmdToAll(const void *cmd, const unsigned int len)
{
	for (const_user_iter it=userList.begin(); it!=userList.end(); it++)
		//if ((*it)->getGameState(gameType())==Cmd::MUS_ROOM)//只发没坐下的
			(*it)->sendCmdToMe(cmd, len);
}

void MiniRoom::sendCmdToIdle(const void *cmd, const unsigned int len)
{
	for (const_user_iter it=userList.begin(); it!=userList.end(); it++)
		if ((*it)->getGameState(gameType())==Cmd::MUS_ROOM)//只发没坐下的
			(*it)->sendCmdToMe(cmd, len);
}

MiniGame *MiniRoom::getGame(const Cmd::MiniGameID &id)
{
	if (gameList.end()!=gameList.find(id.gameID()))
		return gameList[id.gameID()];

	return 0;
}

DWORD MiniRoom::userCount()
{
	return userList.size();
}

void MiniRoom::timer()
{
	for (game_iter it=gameList.begin(); it!=gameList.end(); it++)
		it->second->timer();
}
