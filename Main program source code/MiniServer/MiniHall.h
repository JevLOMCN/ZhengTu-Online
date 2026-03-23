#ifndef _MINI_HALL_H_
#define _MINI_HALL_H_

#include "zSingleton.h"
#include "MiniUserCommand.h"
#include <map>
#include <list>
#include "Top100.h"

class MiniRoom;
class MiniGame;
class MiniUser;

class Top100;

class MiniHall : public Singleton<MiniHall>
{
	friend class MiniService;

	public:
		MiniHall();
		~MiniHall();

		bool parseUserCmd(MiniUser *u, Cmd::stMiniGameUserCmd *cmd, DWORD len);

		MiniRoom * getRoom(const Cmd::MiniRoomID &id);
		MiniGame * getGame(const Cmd::MiniGameID &id);

		void userEnter(MiniUser *u);
		void userLeave(MiniUser *u);

		void sendRoomData(MiniUser *u);
		void updateRoomUserNum(Cmd::MiniRoomID roomID, DWORD num);

		void timer();
	private:
		bool init();
		bool parseCommonCmd(MiniUser *u, Cmd::stCommonMiniGameCmd *cmd, DWORD len);

	private:
		std::map<unsigned int, MiniRoom *> roomList;
		typedef std::map<unsigned int, MiniRoom *>::iterator room_iter;
		Cmd::stGameListCommonMiniGameCmd *roomDataCmd;
};

#endif
