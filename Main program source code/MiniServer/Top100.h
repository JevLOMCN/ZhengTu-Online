#pragma once
#include "zSingleton.h"
#include "MiniUserCommand.h"
#include <list>

class MiniUser;

class Top100 : public Singleton<Top100>
{
	public:
		bool init();
		void calculate(MiniUser *u);
		void send(MiniUser *u);
		void remove(DWORD id);
	private:
		std::list<Cmd::MiniUserData> top100;
		typedef std::list<Cmd::MiniUserData>::iterator top_iter;
};
