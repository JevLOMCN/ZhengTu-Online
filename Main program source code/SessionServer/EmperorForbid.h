#ifndef _EMPERORFORBID_H_
#define _EMPERORFORBID_H_

#include "zMisc.h"
#include "zSingleton.h"
#include <vector>

class EmperorForbid : public Singleton<EmperorForbid>
{
	friend class SingletonFactory<EmperorForbid>;
	private:
		std::vector<DWORD> list;

		void loadDB();
		void writeDB();

		EmperorForbid();

		void clear();
		bool find(DWORD);
	public:
		~EmperorForbid();

		void timer();
		DWORD count();
		bool add(DWORD);
};

#endif
