#ifndef _MINIUSER_H_
#define _MINIUSER_H_

//#include "Zebra.h"
#include "MiniTask.h"
#include "zEntry.h"
#include "MiniUserCommand.h"
#include <map>

#define MAX_MONEY 100000

struct oneGameState
{
	Cmd::MiniUserPosition pos;
	Cmd::MiniUserState state;
	//Cmd::MiniGameScore score;
	bool isNew;
	bool needSave;

	oneGameState()
		:state(Cmd::MUS_NOTPLAY),isNew(true),needSave(false){}
};

class MiniUser :public zEntry 
{
	public:
		WORD face;
		WORD country;

		MiniUser(DWORD i, char *n, WORD c, WORD f, MiniTask *gate, MiniTask *scene);
		void setScene(MiniTask *scene);

		bool sendCmdToMe(const void *pstrCmd, const int nCmdLen) const;
		bool sendCmdToScene(const void *pstrCmd, const int nCmdLen) const;
		void sendSys(int type, const char *pattern, ...) const;
		void sendMiniInfo(int type, const char *pattern, ...) const;

		Cmd::MiniUserState getGameState(const BYTE &type);
		void setGameState(const BYTE &type, const Cmd::MiniUserState &s);
		Cmd::MiniUserPosition getGamePos(const BYTE &type);
		void setGamePos(const BYTE &type, const Cmd::MiniUserPosition &pos);

		void full_MiniUserData(const BYTE &type, Cmd::MiniUserData &data);

		void addScore(Cmd::MiniGameScore score, bool isNew=true);
		Cmd::MiniGameScore getGameScore();
		void sendGameScore(MiniUser *u=0);

		bool save();

		friend class MiniUserManager;
		std::map<BYTE, oneGameState> gameList;
		typedef std::map<BYTE, oneGameState>::iterator game_iter;

		int getMoney();
		bool addMoney(int num);
		bool checkMoney(int num);
		bool removeMoney(int num);
	private:
		MiniTask *minitask;
		MiniTask *scene;

		Cmd::MiniGameScore score;
		bool needSave;
};

#endif
