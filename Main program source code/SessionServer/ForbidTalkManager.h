#ifndef _FORBID_TALK_MANAGER_
#define _FORBID_TALK_MANAGER_
#include "zType.h"

struct forbidInfo
{
	char name[MAX_NAMESIZE+1];
	QWORD startTime;
	int delay;
	WORD operation;
	char reason[256];
	char gm[MAX_NAMESIZE+1];
	WORD isValid;
}__attribute__ ((packed));

class ForbidTalkManager
{
	private:
		static ForbidTalkManager *ftm;
		ForbidTalkManager();
		~ForbidTalkManager();
	public:
		static ForbidTalkManager& getMe();
		static void delMe();

		static void checkDB();
};

#endif
