/**
 * \file
 * \version  $Id: SessionClient.cpp  $
 * \author
 * \date 
 * \brief 定义Session服务器连接客户端
 *
 */

#include <unistd.h>
#include <iostream>

#include "zTCPClient.h"
#include "SessionCommand.h"
#include "SessionClient.h"
#include "GatewayServer.h"
#include "SceneClient.h"
#include "GateUserManager.h"
#include "Zebra.h"
#include "RecordClient.h"
#include "zSocket.h"

/**
 * \brief Session服务器连接客户端
 *
 * 一个区中只有一个Session服务器，所以这里只需要保留一个指针，不需要连接管理器之类的东东
 *
 */
SessionClient *sessionClient = NULL;

/**
 * \brief 建立到Session服务器的连接
 *
 * \return 连接是否成功
 */
bool SessionClient::connectToSessionServer()
{
	if (!connect())
	{
		Zebra::logger->error("连接Session服务器失败");
		return false;
	}

	using namespace Cmd::Session;
	t_LoginSession tCmd;
	tCmd.wdServerID = GatewayService::getInstance().getServerID();
	tCmd.wdServerType = GatewayService::getInstance().getServerType();

	return sendCmd(&tCmd, sizeof(tCmd));
}

/**
 * \brief 重载zThread中的纯虚函数，是线程的主回调函数，用于处理接收到的指令
 *
 */
void SessionClient::run()
{
	zTCPBufferClient::run();

	//与Session之间的连接断开，需要关闭服务器
	GatewayService::getInstance().Terminate();
	while(!GatewayService::getInstance().isSequeueTerminate())
	{
		zThread::msleep(10);
	}
	// */
}

/**
 * \brief 解析来自Session服务器的所有指令
 *
 * \param ptNullCmd 待解析的指令
 * \param nCmdLen 待解析的指令长度
 * \return 解析是否成功
 */
bool SessionClient::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Session;
//	if(ptNullCmd->cmd!=CMD_GATE) return false; 方强在此添加了 转发消息，所以将此句注释
	switch(ptNullCmd->cmd)
	{
		case CMD_GATE:
		{
			switch(ptNullCmd->para)
			{
				//*
				case PARA_GATE_UNREGUSER:
					{
						t_unregUser_GateSession *rev=(t_unregUser_GateSession *)ptNullCmd;
						GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
						if(pUser)
						{
							if(rev->retcode==UNREGUSER_RET_LOGOUT)
							{
								if(pUser->backSelect)
								{
									pUser->final();
									Zebra::logger->trace("用户(%ld,%ld,%s)退回人物选择界面",pUser->accid,pUser->id,pUser->name);
									pUser->initState();
									pUser->beginSelect();
									pUser->backSelect=false;
								}
								else
								{
									//pUser->final();
									Zebra::logger->trace("Session请求用户(%ld,%ld,%s)注销",pUser->accid,pUser->id,pUser->name);
									pUser->Terminate();
									//SAFE_DELETE(pUser);
								}
								return true;
							}
							else if(rev->retcode==UNREGUSER_RET_ERROR)
							{
								Zebra::logger->error("Session请求用户%s注册失败注销",pUser->name);
							/////////////////假人开始	
								//pUser->final();
								
								// //sky 假人
								// pUser->initState();
								// pUser->beginSelect();
								
								pUser->Terminate();
								//SAFE_DELETE(pUser);
								return true;
							}
						}
						  /////////////////////假人结束
						else
						{
							Zebra::logger->warn("注销时无此用户(%ld),scenetempid=%ld,注销类型%d",
									rev->dwUserID,rev->dwSceneTempID,rev->retcode);
							return true;
						}
					}
					break;
				case PARA_GATE_DELCHAR:
					{
						Cmd::Session::t_DelChar_GateSession *rev=(Cmd::Session::t_DelChar_GateSession *)ptNullCmd;

#ifdef _ZJW_DEBUG
						Zebra::logger->debug("删除角色状态为:%d", rev->status);
#endif						
						if (rev->status == 0)
						{
							Cmd::Record::t_DelChar_GateRecord send;
							send.accid=rev->accid;
							send.id=rev->id;
							strncpy(send.name, rev->name, MAX_NAMESIZE);
							recordClient->sendCmd(&send,sizeof(send));
						}
						else
						{
							Cmd::stReturnDeleteSelectUserCmd send;
							strncpy(send.name, rev->name, MAX_NAMESIZE);
							send.err_code = rev->status;

							GateUser* pUser = GateUserManager::getInstance()->
								getUserByAccID(rev->accid);

							if (pUser)
							{
								pUser->sendCmd(&send, sizeof(Cmd::stReturnDeleteSelectUserCmd));
							}
						}

						return true;
					}
					break;
				case PARA_GATE_COUNTRY_ORDER:
					{
						using namespace Cmd::Session;
						Cmd::Session::t_order_Country_GateSession* cmd=(t_order_Country_GateSession *)ptNullCmd; 
#define country_info GatewayService::getInstance().country_info
						country_info.setCountryOrder(&cmd->order);
						return true;
					}
					break;
				case PARA_EXIT_QUIZ:
					{
						Cmd::Session::t_exitQuiz_GateSession* rev = 
							(Cmd::Session::t_exitQuiz_GateSession*)ptNullCmd;

						GateUser *pUser=(GateUser *)GateUserManager::getInstance()->
							getUserByID(rev->dwUserID);		

						if (pUser)
						{
							if (rev->type == 1)
							{
								pUser->quizState();
							}
							else
							{
								pUser->clearQuizState();
							}
						}
						
						return true;
					}
					break;
				default:
					break;
			}
		}
		break;
	case CMD_FORWARD:
		{
			switch(ptNullCmd->para)
			{
				case PARA_FORWARD_USER:
					{
						t_Session_ForwardUser *rev=(t_Session_ForwardUser *)ptNullCmd;
						GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByID(rev->dwID);
						if(!pUser ||  !pUser->sendCmd(rev->data,rev->size))
						{
							//Zebra::logger->debug("转发会话服务器帐号%ld的%u %u消息失败",rev->dwID,((Cmd::stNullUserCmd *)rev->data)->byCmd,((Cmd::stNullUserCmd *)rev->data)->byParam);
						}
						else
						{
							//Zebra::logger->debug("转发会话服务器帐号%ld的%u %u消息成功",rev->dwID,((Cmd::stNullUserCmd *)rev->data)->byCmd,((Cmd::stNullUserCmd *)rev->data)->byParam);
						}
						return true;
					}
					break;
				case PARA_FORWARD_WORLD:
					{
						using namespace Cmd::Session;
						t_Session_ForwardWorld *rev=(t_Session_ForwardWorld *)ptNullCmd;
						struct WorldExec :public execEntry<GateUser>
						{
							unsigned char *_data;
							DWORD _len;
							DWORD _type;
							char _name[MAX_NAMESIZE];
							int _sendLen;
							t_StackCmdQueue cmd_queue;
							WorldExec(unsigned char *data, DWORD len):_data(data),_len(len),_type(0)
							{
								bzero(_name,sizeof(_name));
								GateUser::cmdFilter((Cmd::stNullUserCmd *)_data,_type,_name,_len);
								_sendLen = zSocket::packetPackZip(_data,_len,cmd_queue); 
							}
							bool exec(GateUser *u)
							{
								u->sendCmd(cmd_queue.rd_buf() , _sendLen,_type,_name,true);
								return true;
							}
						};
						WorldExec exec(rev->data,rev->size);
						GateUserManager::getInstance()->execEveryUser(exec);
						return true;
					}
					break;
				case PARA_FORWARD_COUNTRY:
					{
						using namespace Cmd::Session;
						t_Session_ForwardCountry *rev=(t_Session_ForwardCountry *)ptNullCmd;
						GateUserManager::getInstance()->sendCmdToCountry(rev->dwCountry,rev->data,rev->size); 
						return true;
					}
					break;
				default:
					break;
			}
		}
		break;
	case CMD_SESSION:
		{
			switch(ptNullCmd->para)
			{
				case HANDLE_BLACK_LIST_PARA:
					{
						t_Session_HandleBlackList *rev = (t_Session_HandleBlackList *)ptNullCmd;
						GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByID(rev->dwID);
						if (pUser)
						{
							switch(rev->byOper)
							{
								case Cmd::Session::BLACK_LIST_ADD:
									{
										pUser->addBlackList(rev->name);
									}
									break;
								case Cmd::Session::BLACK_LIST_REMOVE:
									{
										pUser->removeBlackList(rev->name);
									}
									break;
								default:
									break;
							}
						}
						return true;
					}
					break;
				default:
					break;
			}
		}
		break;
	default:
		break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

