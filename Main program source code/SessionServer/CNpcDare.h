/**
 * \file
 * \version  $Id: CNpcDare.h  $
 * \author  
 * \date 
 * \brief 定义NPC争夺战管理器
 *
 */


#ifndef _CNPCDARE_H_
#define _CNPCDARE_H_

#include "zMisc.h"
#include "zType.h"
#include "zNullCmd.h"
#include "Command.h"
#include "NpcDare.h"
#include "SessionCommand.h"
#include "zTime.h"
#include "Session.h"


class UserSession;
class CNpcDareObj;

class CNpcDareM
{
	public:
		CNpcDareM();
		bool init();
		static CNpcDareM &getMe();
		static void destroyMe();

		bool load();
		bool refreshDB();
		void timer();
		bool processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);
		bool processSceneMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen);
		void processRequest(Cmd::Session::t_NpcDare_Dare_SceneSession * rev);
		CNpcDareObj* findObject(DWORD country, DWORD mapid, DWORD npcid);
		bool searchSept(DWORD septid);
		CNpcDareObj* searchRecord(DWORD dwCountryID, DWORD dwMapID, DWORD dwNpcID);
		void doDare();
		void doResult();
		CNpcDareObj* getNpcDareObjBySept(DWORD septid);
		void processGetGold(Cmd::Session::t_NpcDare_GetGold_SceneSession *rev);
		void notifyDareReady();
		void forceProcessResult();
		void sendUserData(UserSession *pUser);
		CNpcDareObj* searchSeptHold(DWORD septid);
		template <class YourEntry>
		bool execEveryOne(execEntry<YourEntry> &exec)
		{
			std::vector<CNpcDareObj*>::iterator iter;
			for(iter = _objList.begin() ; iter != _objList.end() ; iter ++)
			{
				exec.exec((YourEntry*)*iter);
			}
			return true;
		}
	private:
		static CNpcDareM * um;
		std::vector<CNpcDareObj*> _objList;
		bool _notDare;
		bool _notifyDareMessage;
};

class CNpcDareObj
{
	public:
		void create(NpcDareDef::NpcDareRecord &);
		void writeDatabase();
		bool isMe(DWORD country, DWORD mapid, DWORD npcid);
		void dareRequest(DWORD userId);
		bool doDare();
		void processResult(DWORD septid);
		void processGetGold(UserSession *pUser, DWORD septid, DWORD dwNpcID, DWORD dwMapID, DWORD dwCountryID);
		static void itemBack(UserSession *pUser);
		void doResult();
		bool notifyDareReady();
		void forceProcessResult();
		DWORD get_country();
		DWORD get_mapid();
		DWORD get_npcid();
		DWORD get_holdseptid();
		DWORD get_dareseptid();
		DWORD get_gold();
		DWORD get_posx();
		DWORD get_posy();

		void abandon_npc();

	private:
		DWORD _dwCountry;
		DWORD _dwMapID;
		DWORD _dwNpcID;
		DWORD _dwHoldSeptID;
		DWORD _dwDareSeptID;
		DWORD _dwGold;
		DWORD _dwPosX;
		DWORD _dwPosY;
		DWORD _dwResultHold;
		DWORD _dwResultDare;
		BYTE  _dareStep;
		zRTime resultTime;
};

#endif
