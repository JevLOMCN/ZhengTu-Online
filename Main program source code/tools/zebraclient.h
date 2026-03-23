#ifndef _SKILL_TEST_CLIENT_
#define _SKILL_TEST_CLIENT_


#include <iostream>
#include <string>

#include "zType.h"
#include "Command.h"
#include "zThread.h"
#include "zTCPClient.h"
#include "zTime.h"
#include "zMisc.h"
#include "ArrayTable.h"
#include "zAStar.h"
#include <map>
#include <vector>
#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "LoadMap.h"

class DelayTimer
{
	public:
		DelayTimer(int how_long) : _long(how_long*1000), _timer(_long)
		{

		}

		bool operator() (zRTime& current)
		{
			if (_timer <= current) {
				_timer = current;
				_timer.addDelay(_long);
				return true;
			}

			return false;
		}
	private:
		int _long;
		zRTime _timer;
};
class zebraClient : public zTCPClientTask , zAStar<>, public zAStar<2>
{

	public:

		zebraClient(
				const std::string &ip, 
				const unsigned short port
				)
			:  zTCPClientTask(ip, port , true) , _one_sec(1) , _five_sec(5) , _ten_sec(10) , _one_min(60)
			{
				moveto_x = 0;
				moveto_y = 0;
				x = 0;
				y = 0;
				firstaction = true;
				pk_mode_ok = false;
				accid = 0;
				loginTempID = 0;
				bzero(pstrIP, sizeof(pstrIP));
				wdPort = 0;
				qwGameTime = 0;
				targetid = 0;
				targettype = (DWORD)-1;
				isPlayState = true;
				waitingCharInfo = false;
				roleNotExists = false;
				waitingRoleLogon = false;
				bzero(charInfo, sizeof(charInfo));
				role_num=0;
				locked = false;
				dwUserTempID = 0;
				moveReturned = false;;
				death = false;
				createCountry = 0;
				dir = 0;
				first=true; 
			}
		zebraClient(
				const std::string &name, 
				const std::string &ip, 
				const unsigned short port,
				const std::string u
				)
			: zTCPClientTask(ip, port , true) , _one_sec(1) , _five_sec(5) , _ten_sec(10) , _one_min(60),user(u)
			{
				moveto_x = 0;
				moveto_y = 0;
				x = 0;
				y = 0;
				firstaction = true;
				pk_mode_ok = false;
				accid = 0;
				loginTempID = 0;
				bzero(pstrIP, sizeof(pstrIP));
				wdPort = 0;
				qwGameTime = 0;
				targetid = 0;
				targettype = (DWORD)-1;
				isPlayState = true;
				waitingCharInfo = false;
				roleNotExists = false;
				waitingRoleLogon = false;
				bzero(charInfo, sizeof(charInfo));
				role_num=0;
				locked = false;
				dwUserTempID = 0;
				moveReturned = false;;
				death = false;
				createCountry = 0;
				dir = 0;
				first=true; 
				dwGameRunTime=0; 
			}

		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);
		bool sendCmd(const void *pstrCmd, const int nCmdLen)
		{
			if (NULL == pSocket) 
				return false;
			else
			{
				Cmd::stNullUserCmd  * cmd = (Cmd::stNullUserCmd  *)pstrCmd; 
				zRTime ct;
				ct.now();
				cmd->dwTimestamp = (DWORD)(ct.msecs() - dwGameRunTime);
				return pSocket->sendCmd(pstrCmd, nCmdLen);
			}
		}

		bool versionCmd();
		bool createAcc(const char *name, const char *passwd);
		bool loginLoginServer(const char *name, const char *passwd);
		bool loginGatewayServer();
		bool changePasswd(const char *name, const char *passwd, const char *newPasswd);
		bool createRole(const char *name);
		bool logonRole(const int i);
		bool timeAction();
		bool init(DWORD acc ,DWORD tempid , DES_cblock key);
		int checkRebound();
		void addToContainer();
		void removeFromContainer();
		bool connect();
		/*
		bool connect(const char *ip, const unsigned short port)
		{
			this->ip = ip;
			this->port = port;
			return connect();
		}
		// */
		char pstrIP[MAX_IP_LENGTH];
		WORD wdPort;
		bool firstaction;
		bool isPlayState;
		bool pk_mode_ok;
		bool locked;
		bool firstRun();
		//地图信息
		static zTiles allTiles;
		static zPos sceneWH;

	private:
		QWORD dwGameRunTime;
		bool first;
		DES_cblock key_des;
		const bool zPosValidate(const zPos &pos) const
		{
				return pos.x < sceneWH.x && pos.y < sceneWH.y;
		}
		void setBlock(const zPos &pos, const BYTE block)
		{
			if (zPosValidate(pos))
			{
				DWORD index = pos.y * sceneWH.x + pos.x;
				allTiles[index].flags |= block;
			}
		}
		void clearBlock(const zPos &pos, const BYTE block)
		{
			if (zPosValidate(pos))
			{
				DWORD index = pos.y * sceneWH.x + pos.x;
				allTiles[index].flags &= ~block;
			}
		}
		const bool checkBlock(const zPos &pos, const BYTE block) const
		{
			if (zPosValidate(pos))
				return (0 != (allTiles[pos.y * sceneWH.x + pos.x].flags & block));
			else
				return true;
		}
		const bool checkBlock(const zPos &pos) const
		{
			return checkBlock(pos, TILE_BLOCK | TILE_ENTRY_BLOCK);
		}
		bool moveable(const zPos &tempPos, const zPos &destPos, const int radius)
		{
			return ((!checkBlock(tempPos) //目标点可达，或者是最终目标点
						|| tempPos == destPos));
		}
		bool move(const int direct, const int step);
		void zebraClient::attack(DWORD tempid , DWORD xx , DWORD yy , BYTE byDir);
		bool gotoFindPath(const zPos &srcPos, const zPos &destPos)
		{
			return zAStar<>::gotoFindPath(srcPos, destPos);
		}



		Cmd::stMainUserDataUserCmd charbase;

		DWORD accid;
		DWORD loginTempID;
		DWORD moveto_x;
		DWORD moveto_y;
		DWORD x;
		DWORD y;
		DWORD dir;
		DWORD skillid;

		//攻击目标的id
		DWORD targetid;
		DWORD targettype;

		QWORD qwGameTime;
		zRTime startTime;
		bool waitingCharInfo;
		bool roleNotExists;
		bool waitingRoleLogon;
		Cmd::SelectUserInfo charInfo[Cmd::MAX_CHARINFO];
		DWORD role_num;

		DWORD dwUserTempID;
		bool moveReturned;
		DWORD createCountry;
		zRTime lastMoveTime;
		bool death;
		zRTime lastReliveTime;

		//9屏用户和npc
		typedef std::map<DWORD , Cmd::stNpcPosition> MapEntry;
		typedef MapEntry::iterator MapEntry_interator;
		typedef MapEntry::value_type MapEntry_value_type;
		MapEntry map_user;
		MapEntry map_object;
		MapEntry map_npc;
		typedef std::vector<zPos> VecNpcPos;
		typedef VecNpcPos::iterator VecNpcPos_iterator;
		typedef VecNpcPos::value_type VecNpcPos_value_type;
		VecNpcPos function_npc_pos;
		bool msgParse_select(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen);
		bool msgParse_relation(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen);
		bool msgParse_time(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen);
		bool msgParse_data(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen);
		bool msgParse_mapscreen(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen);
		bool msgParse_move(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen);
		DWORD getDir(zPos pos_1, zPos pos_2);
		bool msgParse_relive(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen);
		bool msgParse_magic(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen);
		bool msgParse_chat(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen);
		bool msgParse_logon(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen);

		void action();
		void operation(DWORD timer);
		bool one_sec_action();
		bool five_sec_action();
		bool ten_sec_action();
		bool one_min_action();
		void firstAction();
		void gm(const char *data , const unsigned int size);

		int getType(DWORD type)
		{
			int ret = 0;
			while(int(type - 1) > 0)
			{
				type = type >> 1;
				ret ++;
			}
			return ret;
		}
		DelayTimer _one_sec;
		DelayTimer _five_sec;
		DelayTimer _ten_sec;
		DelayTimer _one_min;
		std::string user;
};


#endif
