#ifndef _MINI_ROOM_H_
#define _MINI_ROOM_H_

#include "MiniUserCommand.h"
#include <map>
#include <set>
#include <ext/hash_map>

class MiniGame;
class MiniUser;

class MiniRoom
{
	public:
		MiniRoom();
		DWORD init(Cmd::MiniRoomID id, DWORD gameNum, DWORD userNum, DWORD money);

		BYTE gameType() const;
		MiniGame *getGame(const Cmd::MiniGameID &id);
		DWORD userCount();

		bool userEnter(MiniUser *u);
		void userLeave(MiniUser *u);

		void sendUserToRoom(MiniUser *u);
		void sendUserStateToRoom(MiniUser *u);
		void sendRoomToUser(MiniUser *u);
		void sendCmdToAll(const void *cmd, const unsigned int len);
		void sendCmdToIdle(const void *cmd, const unsigned int len);

		void timer();
	private:
		bool full() const;
		MiniGame *createGame(Cmd::MiniGameID id, DWORD userNum, DWORD money);

	public:
		Cmd::MiniRoomID id;
	private:
		DWORD oneGameUserNum;
		__gnu_cxx::hash_map<DWORD, MiniGame *> gameList;//id-game

		std::set<MiniUser *> userList;// Íæ¼Ò-×´Ì¬

		typedef __gnu_cxx::hash_map<DWORD, MiniGame *>::iterator game_iter;
		typedef std::set<MiniUser *, Cmd::MiniUserState>::iterator user_iter;
		typedef std::set<MiniUser *, Cmd::MiniUserState>::const_iterator const_user_iter;
};

#endif
