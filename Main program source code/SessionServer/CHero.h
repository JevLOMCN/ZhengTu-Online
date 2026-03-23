#ifndef _CLASS_HERO_H_
#define _CLASS_HERO_H_

#include <map>
#include "zNullCmd.h"
#include "Command.h"
#include "SessionCommand.h"
#include "Session.h"
#include "zTime.h"
#include "zMetaData.h"

class CHero
{
    private:
		static CHero * um;
        std::map<DWORD,std::map<DWORD,time_t> > allKillTime;

    public:
        CHero(){}
        ~CHero(){}
		static CHero &getMe();
        void addHeroKill(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen);
        RecordSet* queryHeroKill(DWORD countryid,DWORD datevalue);
        void updateDayHeroList();
        void sendHeroRewards();
};

#endif
