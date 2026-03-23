/**
 * \file
 * \version  $Id: MessageSystem.cpp  $
 * \author  
 * \date 
 * \brief 系统提示信息
 *
 * 
 */

#include <time.h>
#include <vector>
#include "zType.h"
#include "Command.h"
#include "zRWLock.h"
#include "SceneUser.h"
#include "zXMLParser.h"
#include "MessageSystem.h"
#include "Zebra.h"
#include "zMisc.h"
#include "Chat.h"

MessageSystem *MessageSystem::instance = NULL;

/**
 * \brief 加载消息系统的配置文件
 *
 *
 * \return 加载是否成功
 */
bool MessageSystem::init()
{
	final();

	rwlock.wrlock();
	zXMLParser xml;
	if (!xml.initFile(Zebra::global["messageSystemFile"]))
	{
		Zebra::logger->error("加载消息系统配置文件 %s 失败", Zebra::global["messageSystemFile"].c_str());
		return false;
	}

	xmlNodePtr root = xml.getRootNode("messageSystem");
	if (root)
	{
		xmlNodePtr node = xml.getChildNode(root, "messages");
		while(node)
		{
			if (strcmp((char *)node->name, "messages") == 0)
			{
				t_Message mm;
				char strtime[64];
				struct tm tm;
				int login = 0;

				bzero(&mm, sizeof(mm));
				constructInPlace(&mm.cmd);
				mm.endtime=-1;
				xml.getNodePropNum(node, "login", &login, sizeof(login));
				if (login)
					mm.login = true;
				else
					mm.login = false;
				xml.getNodePropNum(node, "interval", &mm.interval, sizeof(mm.interval));
				if (xml.getNodePropStr(node, "starttime", strtime, sizeof(strtime)))
				{
					if(strptime(strtime,"%Y/%m/%d %H:%M:%S", &tm)!=NULL)
					{
						time_t t=mktime(&tm);
						if(t!=(time_t)-1) mm.starttime=t;
					}
				}
				if (xml.getNodePropStr(node, "endtime", strtime, sizeof(strtime)))
				{
					if(strptime(strtime,"%Y/%m/%d %H:%M:%S", &tm)!=NULL)
					{
						time_t t=mktime(&tm);
						if(t!=(time_t)-1) mm.endtime=t;
					}
				}
				mm.count=xml.getChildNodeNum(node,"message");
				//是否顺序发送
				int order=0;
				if(xml.getNodePropNum(node,"order",&order,sizeof(order)) && order)
					mm.order=0;//从零开始,顺序发送
				else
					mm.order=mm.count;

				bzero(mm.cmd.pstrName, sizeof(mm.cmd.pstrName));
				mm.cmd.dwType = Cmd::CHAT_TYPE_SYTEM;

				xmlNodePtr snode = xml.getChildNode(node, "message");
				while(snode)
				{
					if (strcmp((char *)snode->name, "message") == 0)
					{
						xml.getNodeContentStr(snode, mm.cmd.pstrChat, sizeof(mm.cmd.pstrChat));
						//Zebra::logger->debug("%s", mm.cmd.pstrChat);
					}
					messages.push_back(mm);

					snode = xml.getNextNode(snode, NULL);
				}

			}

			node = xml.getNextNode(node, NULL);
		}
		rwlock.unlock();
		Zebra::logger->info("初始化Message消息系统成功");
		return true;
	}
	rwlock.unlock();

	Zebra::logger->error("加载Message消息系统配置文件 %s 失败", Zebra::global["messageSystemFile"].c_str());
	return false;
}

/**
 * \brief 卸载消息系统
 *
 *
 */
void MessageSystem::final()
{
	rwlock.wrlock();
	while(!messages.empty())
	{
		messages.pop_back();
	}
	rwlock.unlock();
}

/**
 * \brief 给指令用户发送指定类型的消息
 *
 *
 * \param sceneUser: 用户
 * \param init: 是否是刚刚登陆
 */
void MessageSystem::check(SceneUser *sceneUser, const bool init)
{
	rwlock.rdlock();
	if (!messages.empty())
	{
		if (init)
		{
			int count=0;
			time_t t=time(NULL);
			while(count<(int)messages.size() && messages[count].count!=0)
			{
				if(messages[count].login &&//登陆发送
						t>=messages[count].starttime &&//已开始
						(messages[count].endtime==-1 || t<=messages[count].endtime))//未结束
				{
					//Zebra::logger->debug("发送：%s", messages[count].cmd.pstrChat);
					sceneUser->sendCmdToMe(&messages[count].cmd, sizeof(messages[count].cmd));
				}
				//Zebra::logger->debug("%s", messages[count].cmd.pstrChat);
				count++;
			}

		}
		else
		{
			int count=0;
			time_t t=time(NULL);
			while(count<(int)messages.size() && messages[count].count!=0)
			{
				if( (messages[count].interval==0 && t==messages[count].starttime) ||//一次性发送
						(messages[count].interval!=0 && t%messages[count].interval==0 &&//定时发送
						 t>=messages[count].starttime &&//已开始
						 (messages[count].endtime==-1 || t<=messages[count].endtime)//未结束
						))
				{
					int i;
					if(messages[count].count==messages[count].order)
						i=zMisc::randBetween(count,count+messages[count].count-1);//随机找一条发给用户
					else
					{
						i=count+sceneUser->messageOrder;
						if(messages[count].count-1<=sceneUser->messageOrder)
							sceneUser->messageOrder=0;
						else
							sceneUser->messageOrder++;
					}
					sceneUser->sendCmdToMe(&messages[i].cmd, sizeof(messages[i].cmd));
				}
				count+=messages[count].count;
			}
		}
	}
	rwlock.unlock();
}

