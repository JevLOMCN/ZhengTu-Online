#include "MiniHall.h"
#include "MiniRoom.h"
#include "MiniGame.h"
#include "MiniUser.h"
#include "MiniUserManager.h"
#include "MiniCommand.h"
#include "zXMLParser.h"
#include "MiniServer.h"
#include "zMetaData.h"

MiniHall::MiniHall()
	:roomDataCmd(0)
{}

MiniHall::~MiniHall()
{
	if (roomDataCmd)
	{
		delete[] roomDataCmd;
		roomDataCmd = 0;
	}
}

bool MiniHall::init()
{
	zXMLParser xml;
	if (!xml.initFile("miniConfig.xml"))
	{       
		Zebra::logger->error("打开Mini配置文件失败 miniConfig.xml");
		return false;
	}

	xmlNodePtr root = xml.getRootNode("Mini");
	if (!root) return false;
	xmlNodePtr gameNode = xml.getChildNode(root, "game");

	char *buf = new char[zSocket::MAX_DATASIZE];
	if (!buf) return false;
	bzero(buf, zSocket::MAX_DATASIZE);

	roomDataCmd = (Cmd::stGameListCommonMiniGameCmd *)buf;
	constructInPlace(roomDataCmd);

	std::map<BYTE, BYTE> roomIDMap;
	while (gameNode)
	{
		DWORD type=0, roomNum=0, gameNum=0, userNum=0, money=0;

		xml.getNodePropNum(gameNode, "type", &type, sizeof(type));
		xml.getNodePropNum(gameNode, "roomNum", &roomNum, sizeof(roomNum));
		xml.getNodePropNum(gameNode, "gameNum", &gameNum, sizeof(gameNum));
		xml.getNodePropNum(gameNode, "userNum", &userNum, sizeof(userNum));
		xml.getNodePropNum(gameNode, "money", &money, sizeof(money));

		for (DWORD i=1; i<=roomNum; i++)
		{
			MiniRoom * r = new MiniRoom;
			if (!r) continue;

			Cmd::MiniRoomID tempid(type, ++roomIDMap[type]);
			DWORD count = r->init(tempid, gameNum, userNum, money);
			if (!count)
			{
				Zebra::logger->error("初始化房间失败 id=%u gameNum=%u userNum=%u money=%u", tempid.id(), gameNum, userNum, money);
				delete r;
				continue;
			}
			roomList[tempid.roomID()] = r;
			Zebra::logger->debug("初始化游戏房间 roomID=%u", tempid.roomID());

			roomDataCmd->data[roomDataCmd->num].roomID = tempid;
			roomDataCmd->data[roomDataCmd->num].gameNum = count;
			roomDataCmd->data[roomDataCmd->num].oneGameUserNum = userNum;
			roomDataCmd->data[roomDataCmd->num].userNum = 0;
			roomDataCmd->data[roomDataCmd->num].money = money;
			roomDataCmd->num++;
		}
		Zebra::logger->debug("游戏类型=%u roomNum=%u gameNum=%u userNum=%u money=%u", type, roomList.size(), gameNum, userNum, money);

		gameNode = xml.getNextNode(gameNode, "game");
	}

	if (!Top100::getMe().init())
		Zebra::logger->error("Top100 初始化失败");

	return true;
}

bool MiniHall::parseUserCmd(MiniUser *u, Cmd::stMiniGameUserCmd *cmd, DWORD len)
{
	//u可能为0
	using namespace Cmd;

	switch (cmd->byParam)
	{
		case COMMON_MINI_PARA:
			{
				return parseCommonCmd(u, (Cmd::stCommonMiniGameCmd *)cmd, len);
			}
			break;
		case CARD_MINI_PARA:
			{
				stCardMiniGameCmd * rev = (stCardMiniGameCmd *)cmd;
				MiniGame *g = getGame(rev->gameID);
				if (g && MGS_PLAY==g->getState())
					return g->parseGameCmd(u, rev, len);
			}
			break;
		default:
			break;
	}
	Zebra::logger->error("MiniHall::parseUserCmd 解析指令错误 cmd=(%u,%u,%u)", cmd->byCmd, cmd->byParam, cmd->subParam);
	return false;
}

void MiniHall::userEnter(MiniUser *u)
{
	if (!u) return;
	Cmd::stLoginRetCommonMiniGameCmd send;
	send.ret = 1;
	u->sendCmdToMe(&send, sizeof(send));

	Cmd::stUserDataCommonMiniGameCmd d;
	d.data.id = u->id;
	strncpy(d.data.name, u->name, MAX_NAMESIZE-1);
	d.data.countryID = u->country;
	d.data.face = u->face;
	d.data.state = Cmd::MUS_HALL;
	d.data.score = u->getGameScore();
	u->sendCmdToMe(&d, sizeof(d));

	sendRoomData(u);


	Zebra::logger->trace("%s(%u) 进入大厅", u->name, u->id);
}

void MiniHall::userLeave(MiniUser *u)
{
	if (!u) return;

	MiniUser::game_iter it = u->gameList.begin();
	MiniUser::game_iter temp;
	for (; it!=u->gameList.end(); )
	{
		temp = it++;
		if (temp->second.state==Cmd::MUS_HALL
				|| temp->second.state==Cmd::MUS_NOTPLAY)
			continue;

		MiniRoom *r = getRoom(temp->second.pos);
		if (!r) continue;

		r->userLeave(u);
	}
	u->save();

	Zebra::logger->debug("%s(%u) 注销", u->name, u->id);

	MiniUserManager::getMe().removeUser(u);
}

MiniRoom * MiniHall::getRoom(const Cmd::MiniRoomID &id)
{
	if (roomList.end()!=roomList.find(id.roomID()))
		return roomList[id.roomID()];

	Zebra::logger->error("找不到房间！roomID=%u", id.roomID());
	return 0;
}

MiniGame * MiniHall::getGame(const Cmd::MiniGameID &id)
{
	if (roomList.end()!=roomList.find(id.roomID()))
		return roomList[id.roomID()]->getGame(id);

	return 0;
}

void MiniHall::sendRoomData(MiniUser *u)
{
	if (u && roomDataCmd)
		u->sendCmdToMe(roomDataCmd, sizeof(Cmd::stGameListCommonMiniGameCmd)+sizeof(Cmd::MiniRoomData)*roomDataCmd->num);
}

void MiniHall::updateRoomUserNum(Cmd::MiniRoomID roomID, DWORD num)
{
	for (DWORD i=0; i<roomDataCmd->num; i++)
	{
		if (roomDataCmd->data[i].roomID==roomID)
			roomDataCmd->data[i].userNum = num;
	}
}

void MiniHall::timer()
{
	for (room_iter it=roomList.begin(); it!=roomList.end(); it++)
		it->second->timer();
}

bool MiniHall::parseCommonCmd(MiniUser *u, Cmd::stCommonMiniGameCmd *cmd, DWORD len)
{
	//u可能为0
	using namespace Cmd;

	switch (cmd->subParam)
	{
		case ENTER_ROOM_COMMON_MINI_PARA:
			{
				if (!u) return false;
				stEnterRoomCommonMiniGameCmd * rev = (stEnterRoomCommonMiniGameCmd *)cmd;
				MiniRoom *r = getRoom(rev->roomID);
				if (!r)
				{
					Zebra::logger->error("ENTER_ROOM 不存在该房间 %u", rev->roomID.roomID());
					return false;
				}

				if (r->userEnter(u))
				{
					for (DWORD i=0; i<roomDataCmd->num; i++)
						if (roomDataCmd->data[i].roomID==rev->roomID)
							roomDataCmd->data[i].userNum++;
				}
				else
				{
					Zebra::logger->error("%s(%u) 进入房间失败 roomID=%u", u->name, u->id, rev->roomID.roomID());
					return false;
				}

				return true;
			}
			break;
		case ENTER_GAME_COMMON_MINI_PARA:
			{
				if (!u) return false;
				stEnterGameCommonMiniGameCmd* rev = (stEnterGameCommonMiniGameCmd *)cmd;
				MiniGame *g = getGame(rev->seatID);
				if (!g)
				{
					Zebra::logger->error("ENTER_GAME 不存在该游戏 %u", rev->seatID.id());
					return false;
				}

				g->userEnter(u, rev->seatID);

				return true;
			}
			break;
		case LEAVE_GAME_COMMON_MINI_PARA:
			{
				if (!u) return false;
				stLeaveGameCommonMiniGameCmd * rev = (stLeaveGameCommonMiniGameCmd *)cmd;
				MiniGame *g = getGame(rev->gameID);
				if (!g)
				{
					Zebra::logger->error("LEAVE_GAME 不存在该游戏 %u", rev->gameID.gameID());
					return false;
				}

				g->userLeave(u);
				return true;
			}
			break;
		case LEAVE_ROOM_COMMON_MINI_PARA:
			{
				if (!u) return false;
				stLeaveRoomCommonMiniGameCmd * rev = (stLeaveRoomCommonMiniGameCmd *)cmd;
				Cmd::MiniUserPosition pos = u->getGamePos(rev->roomID.type);

				MiniRoom *r = getRoom(pos);
				if (!r) return false;

				r->userLeave(u);
				return true;
			}
			break;
		case LOGOUT_COMMON_MINI_PARA:
			{
				if (!u) return false;

				userLeave(u);
				return true;
			}
			break;
		case TOGGLE_SEAT_COMMON_MINI_PARA:
			{
				if (!u) return false;
				stToggleSeatCommonMiniGameCmd *rev = (stToggleSeatCommonMiniGameCmd *)cmd;
				MiniGame *g = getGame(rev->seatID);
				if (!g)
				{
					Zebra::logger->error("TOGGLE_SEAT 不存在该游戏 %u", rev->seatID.gameID());
					return false;
				}

				g->toggleSeat(u, rev->seatID);
				return true;
			}
			break;
		case KICK_USER_COMMON_MINI_PARA:
			{
				if (!u) return false;
				stKickUserCommonMiniGameCmd *rev = (stKickUserCommonMiniGameCmd *)cmd;
				MiniUser *u2 = MiniUserManager::getMe().getUserByID(rev->userID);
				if (!u2) return false;

				MiniGame *g = getGame(rev->gameID);
				if (!g)
				{
					Zebra::logger->error("KICK_USER 不存在该游戏 %u", rev->gameID.gameID());
					return false;
				}

				g->kickUser(u, u2);
				return true;
			}
			break;
		case TOGGLE_READY_COMMON_MINI_PARA:
			{
				if (!u) return false;
				stToggleReadyCommonMiniGameCmd *rev = (stToggleReadyCommonMiniGameCmd *)cmd;
				MiniGame *g = getGame(rev->gameID);
				if (!g)
				{
					Zebra::logger->error("TOGGLE_READY 不存在该游戏 %u", rev->gameID.gameID());
					return false;
				}

				return g->toggleReady(u);
			}
			break;
		case CHAT_COMMON_MINI_PARA:
			{
				if (!u) return false;
				stChatCommonMiniGameCmd *rev = (stChatCommonMiniGameCmd *)cmd;
				rev->userID = u->id;

				if (rev->type==MCT_SYS)
					return u->sendCmdToMe(rev, len);

				MiniUserPosition pos = u->getGamePos(rev->pos.type);
				if (pos.game)
				{
					MiniGame *g = getGame(rev->pos);
					if (g) g->sendCmdToAll(rev, len);
				}
				else if (pos.room)
				{
					MiniRoom *r = getRoom(rev->pos);
					if (r) r->sendCmdToIdle(rev, len);
				}
				return true;
			}
			break;
		case REQ_ROOM_DATA_COMMON_MINI_PARA:
			{
				if (!u) return false;
				sendRoomData(u);
			}
			break;
		case REQ_TOP_COMMON_MINI_PARA:
			{
				if (!u) return false;
				Top100::getMe().send(u);
				return true;
			}
			break;
		case DEPOSIT_COMMON_MINI_PARA:
			{
				if (!u) return false;
				stDepositCommonMiniGameCmd *rev = (stDepositCommonMiniGameCmd *)cmd;

				if (u->getMoney()+rev->num>MAX_MONEY)
				{
					u->sendMiniInfo(Cmd::MCT_POPUP, "你的仙丹超过上限，不能再充值了");
					return true;
				}

				Cmd::Mini::t_Scene_Deposit send;
				send.userID = u->id;
				send.num = rev->num;
				u->sendCmdToScene(&send, sizeof(send));

				return true;
			}
			break;
		case DRAW_COMMON_MINI_PARA:
			{
				if (!u) return false;
				stDrawCommonMiniGameCmd *rev = (stDrawCommonMiniGameCmd *)cmd;

				for (MiniUser::game_iter it=u->gameList.begin(); it!=u->gameList.end(); it++)
				{
					if (it->second.state!=Cmd::MUS_ROOM && it->second.state!=Cmd::MUS_NOTPLAY)
					{
						u->sendMiniInfo(Cmd::MCT_POPUP, "现在不能兑换仙丹");
						return true;
					}
				}

				if (!rev->num) return true;
				if (!u->checkMoney(rev->num))
				{
					u->sendMiniInfo(Cmd::MCT_POPUP, "你的仙丹数量不足");
					return true;
				}

				Cmd::Mini::t_Scene_Check_Draw send;
				send.userID = u->id;
				send.num = rev->num;
				u->sendCmdToScene(&send, sizeof(send));

				return true;
			}
			break;
		default:
			break;
	}
	Zebra::logger->error("MiniHall::parseCommonCmd 解析指令错误 cmd=(%u,%u,%u)", cmd->byCmd, cmd->byParam, cmd->subParam);
	return false;
}

