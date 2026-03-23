/**
 * \file
 * \version  $Id: SceneClient.cpp $
 * \author  
 * \date 
 * \brief 定义场景服务器连接客户端
 *
 */

#include <unistd.h>
#include <iostream>

#include "zTCPClient.h"
#include "SceneCommand.h"
#include "SceneClient.h"
#include "SessionCommand.h"
#include "SessionClient.h"
#include "GatewayServer.h"
#include "GateUserManager.h"
#include "Zebra.h"
#include "Command.h"
#include "BillClient.h"
#include <time.h>

#if 0
SceneClientManager *SceneClientManager::instance = NULL;
#endif

/**
 * \brief 创建到场景服务器的连接
 *
 * \return 连接是否成功
 */
bool SceneClient::connectToSceneServer()
{
	//Zebra::logger->debug(__PRETTY_FUNCTION__);

	if (!zTCPClientTask::connect())
	{
		Zebra::logger->error("连接场景服务器失败");
		return false;
	}

	Cmd::Super::t_restart_ServerEntry_NotifyOther notify; 
	notify.srcID=GatewayService::getInstance().getServerID();
	notify.dstID=this->getServerID();
	GatewayService::getInstance().sendCmdToSuperServer(&notify, sizeof(notify));
	using namespace Cmd::Scene;
	t_LoginScene tCmd;
	tCmd.wdServerID = GatewayService::getInstance().getServerID();
	tCmd.wdServerType = GatewayService::getInstance().getServerType();

	return sendCmd(&tCmd, sizeof(tCmd));
}

#if 0
/**
 * \brief 把一个场景服务器从连接添加到容器中删除
 *
 * \param client 场景服务器连接
 */
void SceneClientManager::remove(SceneClient *client)
{
	//Zebra::logger->debug(__PRETTY_FUNCTION__);
	mlock.lock();
	std::vector<SceneClient *>::iterator iter = sceneClients.begin();
	for(;iter != sceneClients.end(); iter ++)
	{
		if(*iter == client)
		{
			sceneClients.erase(iter);
			mlock.unlock();
			return;
		}
	}
	mlock.unlock();
}
/**
 * \brief 把一个场景服务器的连接添加到容器中
 *
 * \param client 场景服务器连接
 */
void SceneClientManager::add(SceneClient *client)
{
	//Zebra::logger->debug(__PRETTY_FUNCTION__);
	mlock.lock();
	sceneClients.push_back(client);
	mlock.unlock();
}

/**
 * \brief 通过服务器编号获取场景服务器连接
 *
 * \param serverid 服务器编号
 * \return 场景服务器连接
 */
SceneClient *SceneClientManager::getByServerID(WORD serverid)
{
	mlock.lock();
	for(int i=0,n=sceneClients.size();i<n;i++)
	{
		if(sceneClients[i]->getServerID()==serverid)
		{
			mlock.unlock();
			return sceneClients[i];
		}
	}
	mlock.unlock();
	return NULL;
}

/**
 * \brief 关闭管理器容器，释放资源
 *
 * 将关闭所有的与场景服务器的连接
 *
 */
void SceneClientManager::final()
{
	mlock.lock();
	while(!sceneClients.empty())
	{
		Zebra::logger->debug(__PRETTY_FUNCTION__);
		std::vector<SceneClient *>::reference ref = sceneClients.back();
		sceneClients.pop_back();
		if (ref)
		{
			(ref)->final();
			(ref)->join();
			SAFE_DELETE(ref);
		}
	}
	mlock.unlock();
}
#endif 
int SceneClient::checkRebound()
{
	return 1;
}
void SceneClient::addToContainer()
{
	SceneClientManager::getInstance().add(this);
}

void SceneClient::removeFromContainer()
{
	SceneClientManager::getInstance().remove(this);
	GateUserManager::getInstance()->removeUserBySceneClient(this);
}
bool SceneClient::connect()
{
	return connectToSceneServer();
}

#if 0
/**
 * \brief 线程回调函数
 *
 */
void SceneClient::run()
{
	zTCPBufferClient::run();
	while(!GatewayService::getInstance().isTerminate())
	{
		SceneClientManager::getInstance().remove(this);
		GateUserManager::getInstance()->removeUserBySceneClient(this); 
		while(!connect())
		{
			Zebra::logger->error("连接场景服务器失败");
			zThread::msleep(1000);
		}
		Cmd::Super::t_restart_ServerEntry_NotifyOther notify;
		notify.srcID=GatewayService::getInstance().getServerID();
		notify.dstID=this->getServerID();
		GatewayService::getInstance().sendCmdToSuperServer(&notify, sizeof(notify));
		zThread::msleep(2000);
		connect();
		SceneClientManager::getInstance().add(this);
		using namespace Cmd::Scene;
		t_LoginScene tCmd;
		tCmd.wdServerID = GatewayService::getInstance().getServerID();
		tCmd.wdServerType = GatewayService::getInstance().getServerType();

		if(sendCmd(&tCmd, sizeof(tCmd)))
		{
			zTCPBufferClient::run();
		}
			// */
		zThread::msleep(1000);
	}

	//与场景服务器连接断开，关闭服务器
	GatewayService::getInstance().Terminate();
}
#endif

/**
 * \brief 解析来自场景服务器的指令
 *
 * \param ptNullCmd 待处理的指令
 * \param nCmdLen 待处理的指令长度
 * \return 解析是否成功
 */
bool SceneClient::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Scene;
	switch(ptNullCmd->cmd)
	{
		case CMD_LOGIN:
			{
				switch(ptNullCmd->para)
				{
					case PARA_CHANGE_VERIFY_VERSION:
						{
							t_ChangeVerifyVersion *rev=(t_ChangeVerifyVersion *)ptNullCmd;
							GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
							if (pUser)
							{
								switch (rev->oper)
								{
									case Cmd::Scene::CHANGECODE:
										{
											GatewayService::getInstance().verify_client_version = rev->versionCode;
											GatewayService::getInstance().notifyLoginServer();
											return true;
										}
										break;
									case Cmd::Scene::SHOWCODE:
										{
											Cmd::stChannelChatUserCmd send;
											send.dwType=Cmd::CHAT_TYPE_SYSTEM;
											send.dwSysInfoType = Cmd::INFO_TYPE_GAME;
											bzero(send.pstrName, sizeof(send.pstrName));
											bzero(send.pstrChat, sizeof(send.pstrChat));

											if (pUser)
											{
												sprintf((char*)send.pstrChat, "服务器当前版本校验码:%u",
													GatewayService::getInstance().verify_client_version);
												pUser->sendCmd(&send, sizeof(send));
											}
											return true;
										}
										break;
									default:
										break;
								}
							}
						}
						break;
					case PARA_CHANGE_COUNTRY_STATE:
						{
							t_ChangeCountryStatus *rev=(t_ChangeCountryStatus *)ptNullCmd;
							GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
							if (pUser)	GatewayService::getInstance().country_info.processChange(pUser, rev);
							return true;
						}
						break;
					case PARA_LOGIN_REFRESH:
						{
							t_Refresh_LoginScene *rev=(t_Refresh_LoginScene *)ptNullCmd;
							GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
							if(pUser)
							{
								//pUser->scene=this;
								//pUser->sceneTempID=rev->dwSceneTempID;
								pUser->playState(this ,rev->dwSceneTempID );
								Zebra::logger->trace("用户%s(%d,%d)登录成功",pUser->name,pUser->id,pUser->tempid);
							}
							else
							{
								Zebra::logger->fatal("用户id=%d已经不在了",rev->dwUserID);
								//清理Session中数据
								Cmd::Session::t_unregUser_GateSession send;
								send.dwUserID=rev->dwUserID;
								send.dwSceneTempID=rev->dwSceneTempID;
								send.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
								sessionClient->sendCmd(&send,sizeof(send));
								//清理Scene中数据
								Cmd::Scene::t_Unreg_LoginScene scnd;
								scnd.dwUserID=rev->dwUserID;
								scnd.dwSceneTempID=rev->dwSceneTempID;
								scnd.retcode=Cmd::Scene::UNREGUSER_RET_ERROR;
								sendCmd(&scnd,sizeof(scnd));
							}
							return true;
						}
						break;
					case PARA_LOGIN_UNREG:
						{
							t_Unreg_LoginScene *rev=(t_Unreg_LoginScene *)ptNullCmd;
							GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
							if(pUser)
							{
								if(rev->retcode==Cmd::Scene::UNREGUSER_RET_LOGOUT)
								{
									if(pUser->backSelect)
									{
										//pUser->final();
										Zebra::logger->trace("用户(%ld,%ld,%s)退回人物选择界面",pUser->accid,pUser->id,pUser->name);
										//pUser->initState();
										//pUser->beginSelect();
										//pUser->backSelect=false;
									}
									else
									{
										//pUser->final();
										Zebra::logger->trace("场景请求用户(%ld,%ld,%s)注销",pUser->accid,pUser->id,pUser->name);
										pUser->Terminate();
										//SAFE_DELETE(User);
									}
								}
								else if(rev->retcode==Cmd::Scene::UNREGUSER_RET_UNLOAD_SCENE)
								{
									Zebra::logger->trace("用户因卸载地图(%ld,%ld,%s)退回人物选择界面",pUser->accid,pUser->id,pUser->name);
									if(pUser && pUser->isPlayState())
									{
										pUser->backSelect=true;
										pUser->unreg();
									}
								}
								else if(rev->retcode==Cmd::Scene::UNREGUSER_RET_ERROR)
								{
									Zebra::logger->error("场景请求用户(%ld,%ld,%s)失败注销",pUser->accid,pUser->id,pUser->name);
									//pUser->final();
									pUser->Terminate();
									//SAFE_DELETE(pUser);
								}
								else if(rev->retcode==Cmd::Scene::UNREGUSER_RET_CHANGE_SCENE)
								{
									Zebra::logger->error("用户(%ld,%ld,%s)切换场景注销",pUser->accid,pUser->id,pUser->name);

									Cmd::Session::t_changeUser_GateSession send;
									send.accid = pUser->accid;
									send.dwID = pUser->id;
									send.dwTempID = pUser->tempid;
									strncpy((char *)send.byName, pUser->name, MAX_NAMESIZE);
									strncpy((char *)send.byMapFileName, (char *)rev->map, MAX_NAMESIZE);
                                    ////////////////////////////////////
									//soke 太庙资源数量显示有关
									send.scnTempID = rev->dwSceneTempID;
									/////////////////////////////////////
									sessionClient->sendCmd(&send, sizeof(send));
									pUser->waitUnregState();

									//pUser->final();
									//SAFE_DELETE(pUser);
								}
								else if(rev->retcode==Cmd::Scene::UNREGUSER_RET_KICKOUT)
								{
								}

							}
							return true;
						}
						break;
					case PARA_LOGIN_UNREG_OK:
						{
							t_Unreg_LoginScene_Ok *rev=(t_Unreg_LoginScene_Ok *)ptNullCmd;
							GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByAccID(rev->accid);
							if(pUser)
							{
								switch(rev->type)
								{
									case Cmd::Record::LOGOUT_WRITEBACK:
										{
											pUser->final();
											pUser->beginSelect();
											pUser->backSelect=false;
										}
										break;
									case Cmd::Record::CHANGE_SCENE_WRITEBACK:
										{
											pUser->playState();
										}
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
		case CMD_SCENE_GATE_BILL:
			{
				switch(ptNullCmd->para)
				{
					case PARA_REQUEST_BILL:
						{
							Cmd::Scene::t_Request_Bill *rev= (Cmd::Scene::t_Request_Bill*)ptNullCmd; 
							GateUser* pUser = (GateUser*)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
							if(pUser)
							{
								Cmd::Bill::t_Request_Card_Gold_Gateway send;
								if(pUser->getAccount())
									strncpy(send.account, pUser->getAccount(), MAX_ACCNAMESIZE);
								send.accid = pUser->accid;
								send.charid = pUser->id;
								accountClient->sendCmd(&send, sizeof(send));	
								//Zebra::logger->debug("请求查询剩余月卡和金币(accid=%d)",pUser->accid);
							}
						}
						break;
					case PARA_REQUEST_POINT:
						{
							Cmd::Scene::t_Request_Point *rev= (Cmd::Scene::t_Request_Point*)ptNullCmd; 
							GateUser* pUser = (GateUser*)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
							if(pUser)
							{
								Cmd::Bill::t_Request_Point_Gateway send;
								if(pUser->getAccount())
									strncpy(send.account, pUser->getAccount(), MAX_ACCNAMESIZE);
								send.accid = pUser->accid;
								send.charid = pUser->id;
								accountClient->sendCmd(&send, sizeof(send));	
								//Zebra::logger->debug("请求查询剩余点数(accid=%d)",pUser->accid);
							}
						}
						break;
						/*
						   case PARA_REQUEST_CARD_GOLD:
						   {
						   Cmd::Scene::t_Request_Card_Gold *rev= (Cmd::Scene::t_Request_Card_Gold*)ptNullCmd; 
						   GateUser* pUser = (GateUser*)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
						   if(pUser)
						   {
						   Cmd::Bill::t_Request_Point_Gateway send;
						   if(pUser->getAccount())
						   strncpy(send.account, pUser->getAccount(), MAX_ACCNAMESIZE);
						   send.accid = pUser->accid;
						   send.charid = pUser->id;
						   accountClient->sendCmd(&send, sizeof(send));	
						   Zebra::logger->debug("请求查询剩余点数(accid=%d)",pUser->accid);
						   }
						   }
						   break;
						// */
					case PARA_REQUEST_REDEEM_GOLD:
						{
							Cmd::Scene::t_Request_RedeemGold *rev= (Cmd::Scene::t_Request_RedeemGold*)ptNullCmd; 
							GateUser* pUser = (GateUser*)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
							if(pUser)
							{
								Cmd::Bill::t_Request_Redeem_Gold_Gateway send;
								if(pUser->getAccount())
									strncpy(send.account, pUser->getAccount(), MAX_ACCNAMESIZE);
								send.accid = pUser->accid;
								send.charid = pUser->id;
								send.point = rev->dwNum;
								accountClient->sendCmd(&send, sizeof(send));	
								//Zebra::logger->debug("请求兑换金币(accid=%d)",pUser->accid);
							}
						}
						break;
					case PARA_REQUEST_REDEEM_CARD:
						{
							Cmd::Scene::t_Request_RedeemCard *rev= (Cmd::Scene::t_Request_RedeemCard*)ptNullCmd; 
							GateUser* pUser = (GateUser*)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
							if(pUser)
							{
								Cmd::Bill::t_Request_Redeem_MonthCard_Gateway send;
								if(pUser->getAccount())
									strncpy(send.account, pUser->getAccount(), MAX_ACCNAMESIZE);
								send.accid = pUser->accid;
								send.charid = pUser->id;
								accountClient->sendCmd(&send, sizeof(send));	
								//Zebra::logger->debug("请求兑换月卡(accid=%d)",pUser->accid);
							}
						}
						break;
					default:
						break;
				}
				return true;
			}
			break;
		case CMD_SCENE:
			{
				switch(ptNullCmd->para)
				{
					case PARA_QUERY_ACCOUNT:
						{
							t_Query_AccountScene* rev = (t_Query_AccountScene*)ptNullCmd;
							GateUser* pUser = (GateUser*)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
							GateUser* pDestUser = (GateUser*)GateUserManager::getInstance()->getUserByID(rev->dwDestID);
							Cmd::stChannelChatUserCmd send;
							send.dwType=Cmd::CHAT_TYPE_SYSTEM;
							send.dwSysInfoType = Cmd::INFO_TYPE_FAIL;
							bzero(send.pstrName, sizeof(send.pstrName));
							bzero(send.pstrChat, sizeof(send.pstrChat));

							if (pUser)
							{
								if (pDestUser)
								{
									snprintf((char*)send.pstrChat, MAX_CHATINFO, "%s(%s)", pDestUser->name, pDestUser->getAccount());
								}

								pUser->sendCmd(&send, sizeof(send));
							}

							return true;
						}
						break;
					case PARA_COUNTRY_AND_SCENE:
						{
							Cmd::Scene::t_countryAndScene_GateScene* rev = (Cmd::Scene::t_countryAndScene_GateScene*)ptNullCmd;
							GateUser* pUser = (GateUser*)GateUserManager::getInstance()->getUserByID(rev->userID);
							if (pUser)
							{
								GateUserManager::getInstance()->removeCountryUser(pUser);
								pUser->countryID = rev->countryID;
								GateUserManager::getInstance()->addCountryUser(pUser);
								pUser->sceneID = rev->sceneID;
								//Zebra::logger->debug("%s 场景改变 %u", pUser->name, pUser->sceneID);
								return true;
							}
							else
							{
								Zebra::logger->debug("收到改变场景的消息但是玩家不存在 %u", rev->userID);
								return true;
							}
						}
						break;
					case PARA_FRESH_MAPINDEX:
						{
							t_fresh_MapIndex *rev=(t_fresh_MapIndex*)ptNullCmd;
							for(int i = 0 ; i < (int)rev->dwSize ; i ++)
							{
								ScreenIndex *index = new ScreenIndex(rev->mps[i].mapx , rev->mps[i].mapy);
								if(index)
								{
									Zebra::logger->debug("网关注册地图(%u,%u,%u)",rev->mps[i].maptempid ,rev->mps[i].mapx , rev->mps[i].mapy);
									mapIndex[rev->mps[i].maptempid] = index;
								}
							}

							return true;
						}
						break;
					case PARA_REMOVE_MAPINDEX:
						{
							t_Remove_MapIndex *rev=(t_Remove_MapIndex*)ptNullCmd;
							for(int i = 0 ; i < (int)rev->dwSize ; i ++)
							{
								MapIndexIter iter = mapIndex.find(rev->dwMapTempID[i]); 
								if(iter != mapIndex.end())
								{
									Zebra::logger->debug("网关注销地图(%u)",rev->dwMapTempID[i]);
									SAFE_DELETE(iter->second);
									mapIndex.erase(iter);
								}
							}
						}
						break;
					case PARA_FRESH_SCREENINDEX:
						{
							t_fresh_ScreenIndex* rev = (t_fresh_ScreenIndex*)ptNullCmd;
							GateUser* pUser = (GateUser*)GateUserManager::getInstance()->getUserByTempID(rev->dwUserTempID);
							if(pUser)
							{
								//Zebra::logger->debug("用户屏索引变化(userID=%u , oldscreen = %u , newscreen=%u)",pUser->id ,pUser->getIndexKey() , rev->dwScreen);
								if ((DWORD)-1 == rev->dwScreen)
									removeIndex(pUser, rev->dwMapTempID);
								else if(pUser->isPlayState())
									freshIndex(pUser, rev->dwMapTempID , rev->dwScreen);
							}
							return true;
						}
						break;
						//踢人
					case PARA_KICK_USER:
						{
							Cmd::Scene::t_kickUser_GateScene* rev = (Cmd::Scene::t_kickUser_GateScene*)ptNullCmd;
							GateUser* pUser = (GateUser*)GateUserManager::getInstance()->getUserByID(rev->userID);
							if (!pUser) pUser = (GateUser*)GateUserManager::getInstance()->getUserByAccID(rev->accid);
							if (pUser)
								pUser->Terminate();
							return true;
						}
						break;
					case PARA_SCENE_SYS_SETTING:
						{
							Cmd::Scene::t_sysSetting_GateScene* rev = (Cmd::Scene::t_sysSetting_GateScene*)ptNullCmd;
							GateUser* pUser = (GateUser*)GateUserManager::getInstance()->getUserByID(rev->id);
							if (pUser)
							{
								bcopy(rev->sysSetting,pUser->sysSetting,sizeof(pUser->sysSetting));
#ifdef _XWL_DEBUG
								Zebra::logger->debug("收到系统设置消息:%x %x %x %x", pUser->sysSetting[0],pUser->sysSetting[1],pUser->sysSetting[2],pUser->sysSetting[3]);
#endif
							}
							return true;
						}
						break;
						//停止金币服务
					case PARA_SERVICE_GOLD:
						{
							Cmd::Scene::t_ServiceGold_GateScene *rev=(Cmd::Scene::t_ServiceGold_GateScene *)ptNullCmd;
							{
								if(rev->byType==Cmd::Scene::SERVICE_STOP)
								{
									GatewayService::service_gold=false;
									Zebra::logger->debug("停止金币服务");
								}
								else if(rev->byType==Cmd::Scene::SERVICE_START)
								{
									GatewayService::service_gold=true;
									Zebra::logger->debug("启动金币服务");
								}
							}
							return false;
						}
						break;
						//停止股票服务
					case PARA_SERVICE_STOCK:
						{
							Cmd::Scene::t_ServiceStock_GateScene *rev=(Cmd::Scene::t_ServiceStock_GateScene *)ptNullCmd;
							{
								if(rev->byType==Cmd::Scene::SERVICE_STOP)
								{
									GatewayService::service_stock=false;
									Zebra::logger->debug("停止股票服务");
								}
								else if(rev->byType==Cmd::Scene::SERVICE_START)
								{
									GatewayService::service_stock=true;
									Zebra::logger->debug("启动股票服务");
								}
							}
							return false;
						}
						break;
					case PARA_USLEEP_TIME:
						{
							Cmd::Scene::t_Usleep_GateScene *rev = (Cmd::Scene::t_Usleep_GateScene *)ptNullCmd;
							if(rev->utask)
							{
								zTCPTaskPool::setUsleepTime(rev->utask);
								Zebra::logger->info("设置系统Task的usleep时间:%dus",rev->utask);
							}
							if(rev->uclient)
							{
								SceneClientManager::getInstance().setUsleepTime(rev->uclient);
								Zebra::logger->info("设置系统Client的usleep时间:%dus",rev->uclient);
							}
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
					case PARA_FORWARD_NINE:
						{       
							t_Nine_ForwardScene  *rev=(t_Nine_ForwardScene *)ptNullCmd;
							MapIndexIter iter = mapIndex.find(rev->maptempid); 
							if(iter != mapIndex.end())
								iter->second->sendCmdToNine(rev->screen , rev->data,rev->size);
							return true;
						}       
						break; 
					case PARA_FORWARD_NINE_DIR:
						{       
							t_Nine_dir_ForwardScene  *rev=(t_Nine_dir_ForwardScene *)ptNullCmd;
							MapIndexIter iter = mapIndex.find(rev->maptempid); 
							if(iter != mapIndex.end())
								iter->second->sendCmdToDirect(rev->screen, rev->dir, rev->data,rev->size);
							return true;
						}       
						break; 
					case PARA_FORWARD_NINE_RDIR:
						{       
							t_Nine_rdir_ForwardScene  *rev=(t_Nine_rdir_ForwardScene *)ptNullCmd;
							MapIndexIter iter = mapIndex.find(rev->maptempid); 
							if(iter != mapIndex.end())
								iter->second->sendCmdToReverseDirect(rev->screen, rev->dir, rev->data,rev->size);
							return true;
						}       
						break; 
					case PARA_FORWARD_USER:
						{
							t_User_ForwardScene *rev=(t_User_ForwardScene *)ptNullCmd;
							GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByID(rev->dwID);
							if(!pUser ||  !pUser->sendCmd(rev->data,rev->size))
							{
								if(!pUser)
								{
									//Zebra::logger->debug("用户已经不存在");
								}
								//Cmd::stNullUserCmd *cmd = (Cmd::stNullUserCmd *)rev->data;
								//Zebra::logger->debug("转发%ld的消息(%u,%u)失败",rev->dwID , cmd->byCmd , cmd->byParam);
							}
#ifdef _DEBUGLOG
							Zebra::logger->trace("场景发用户的消息已经发出[cmd=%u para=%u]", ((Cmd::stNullUserCmd *)rev->data)->byCmd, ((Cmd::stNullUserCmd *)rev->data)->byParam);
#endif
							return true;
						}
						break;
					case PARA_FORWARD_MAP:
						{       
							t_User_ForwardMap  *rev=(t_User_ForwardMap *)ptNullCmd;
							MapIndexIter iter = mapIndex.find(rev->maptempid); 
							if(iter != mapIndex.end())
								iter->second->sendCmdToAll(rev->data,rev->size);
							return true;
						}       
						break;  
					case PARA_FORWARD_SCENEUSER_TO_BILL:
						{
							t_Scene_ForwardSceneUserToBill *rev=(t_Scene_ForwardSceneUserToBill *)ptNullCmd;
							GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
							if(!pUser ||  !pUser->forwardBillScene((const Cmd::stNullUserCmd *)rev->data,rev->size))
							{
								Zebra::logger->debug("转发%ld到Bill的消息(%u,%u)失败",rev->dwUserID,rev->cmd,rev->para);
							}
							return true;
						}
						break;
					case PARA_FORWARD_SCENE_TO_BILL:
						{
							t_Scene_ForwardSceneToBill *rev=(t_Scene_ForwardSceneToBill *)ptNullCmd;
							GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByID(rev->dwUserID);
							if(!pUser ||  !pUser->forwardBill((const Cmd::stNullUserCmd *)rev->data,rev->size))
							{
								Zebra::logger->debug("转发%ld到Bill的消息(%u,%u)失败",rev->dwUserID,rev->cmd,rev->para);
							}
							return true;
						}
						break;
					case PARA_FORWARD_NINE_EXCEPTME:
						{       
							t_Nine_ExceptMe_ForwardScene  *rev=(t_Nine_ExceptMe_ForwardScene *)ptNullCmd;
							MapIndexIter iter = mapIndex.find(rev->maptempid); 
							if(iter != mapIndex.end())
								iter->second->sendCmdToNineExceptMe(rev->screen, rev->exceptme_id, rev->data,rev->size);
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
