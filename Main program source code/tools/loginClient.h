/**
 * \file
 * \version  $Id: loginClient.h  $
 * \author  
 * \date
 * \brief 
 *
 * 
 */
#ifndef _loginClient_h_
#define _loginClient_h_
#include "zType.h"
#include "Command.h"
#include <iostream>
#include <string>
#include "zThread.h"
#include "zTCPClient.h"
#include "zTime.h"
#include "zMisc.h"


class LoginClient : public zTCPClient
{
	public:
		LoginClient(
				const std::string &name, 
				const std::string &ip, 
				const unsigned short port)
			: zTCPClient(name, ip, port , true)
			{
				bzero(pstrIP, sizeof(pstrIP));
				wdPort = 0;
				createCountry=0;
				accid=0;
				loginTempID=0;
			}
		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);
		bool msgParse_logon(const Cmd::stNullUserCmd *ptNull, const unsigned int nCmdLen);
		bool connect();
		bool versionCmd();
		bool connect(const char *ip, const unsigned short port)
		{
			this->ip = ip;
			this->port = port;
			return connect();
		}
		bool loginLoginServer(const char *name, const char *passwd);
		bool getGateInfo(char *ip , WORD &port , DWORD &acc , DWORD &tempid)
		{
			bcopy(pstrIP , ip , sizeof(pstrIP));
			port=wdPort;
			if(wdPort==0)
			{
				return false;
			}
			acc=accid;
			tempid=loginTempID;
			accid=0;
			loginTempID=0;
			close();
			bzero(pstrIP, sizeof(pstrIP));
			wdPort = 0;
			return true;
		}
		void get_key_des(char *key)
		{
			bcopy(key_des,key,sizeof(DES_cblock));
		}
	private:
		char pstrIP[MAX_IP_LENGTH];
		WORD wdPort;
		DES_cblock key_des;
		DWORD createCountry;
		DWORD accid;
		DWORD loginTempID;

};
#endif


