/**
 * \file
 * \version  $Id: SessionTask.cpp $
 * \author  
 * \date 
 * \brief 定义登陆连接任务
 *
 */

//#include <iostream>
//#include <vector>
//#include <list>
//#include <iterator>
//#include <ext/hash_map>

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "zMisc.h"
#include "SessionTask.h"
#include "GmToolCommand.h"
#include "zDBConnPool.h"
#include "zMetaData.h"
#include "SessionServer.h"
#include "SessionCommand.h"
#include "SessionTaskManager.h"
#include "CUnion.h"
#include "CSept.h"
#include "SchoolManager.h"
#include "OfflineMessage.h"
#include "TempArchive.h"
#include "CharBase.h"
#include "Team.h"
#include "Session.h"
//#include "RecordCommand.h"
//`#include "RecordClient.h"
#include "CDare.h"
#include "CQuiz.h"
#include "CNpcDare.h"
#include "SchoolManager.h"
#include "SessionChat.h"
#include "CCountryManager.h"
#include "CCityManager.h"
#include "MailService.h"
#include "AuctionService.h"
#include "CartoonPetService.h"
#include "CSort.h"
#include "CVote.h"
#include "CArmy.h"
#include "CGem.h"
#include "Gift.h"
#include "TimeTick.h"
#include "Ally.h"
#include "RecommendManager.h"
#include "GameConfigMgrX.h"

#include "CHero.h"

/**
 * \brief 遍历每个用户会话给同一国家的角色发送聊天消息
 */
struct EveryUserSessionAction: public execEntry<UserSession>
{
	DWORD country;
	unsigned int cmdLen;
	Cmd::stChannelChatUserCmd * revCmd;
	bool init(Cmd::stChannelChatUserCmd * rev, unsigned int len)
	{
		UserSession *pUser = UserSessionManager::getInstance()->getUserSessionByName(rev->pstrName);
		revCmd = rev;
		cmdLen = len;
		if (pUser) 
		{
			country = pUser->country;
			return true;
		}
		return false;
	}

	/**
	 * \brief 遍历每个用户会话给同一国家的角色发送聊天消息
	 * \param su 用户会话
	 * \return true 成功 false 失败
	 */
	bool exec(UserSession *su)
	{
		if (country == su->country)
			su->sendCmdToMe(revCmd, cmdLen);
		return true;
	}
};


/**
 * \brief 遍历每个用户会话给同一国家的角色发送用户命令
 */
struct OneCountryUserSessionAction: public execEntry<UserSession>
{
	DWORD country;
	unsigned int cmdLen;
	Cmd::stNullUserCmd* sendCmd;

	void init(Cmd::stNullUserCmd * rev, unsigned int len, DWORD countryID)
	{
		sendCmd = rev;
		cmdLen = len;

		country = countryID;
	}

	/**
	 * \brief 遍历每个用户会话给同一国家的角色发送聊天消息
	 * \param su 用户会话
	 * \return true 成功 false 失败
	 */
	bool exec(UserSession *su)
	{
		if (country == su->country)
			su->sendCmdToMe(sendCmd, cmdLen);
		return true;
	}
};

/**
 * \brief 遍历每个用户会话给角色发送聊天消息
 */
struct broadcastToEveryUser: public execEntry<UserSession>
{
	unsigned int cmdLen;
	Cmd::stChannelChatUserCmd * revCmd;
	bool init(Cmd::stChannelChatUserCmd * rev, unsigned int len)
	{
		if (0==rev) return false;
		revCmd = rev;
		cmdLen = len;
		return true;
	}

	/**
	 * \brief 遍历
	 * \param su 用户会话
	 * \return true 成功 false 失败
	 */
	bool exec(UserSession *su)
	{
		if (su) su->sendCmdToMe(revCmd, cmdLen);
		return true;
	}
};

/**
 * \brief 广播通知给每个角色
 */
struct broadcastRushToEveryUser: public execEntry<UserSession>
{
	char * pContent;

	bool init(char * content)
	{
		pContent = content;
		return true;
	}
	/**
	 * \brief 遍历发送消息
	 * \param su 用户会话
	 * \return true 成功 false 失败
	 */
	bool exec(UserSession *su)
	{
		su->sendSysChat(Cmd::INFO_TYPE_GAME, pContent);
		return true;
	}
};

/**
 * \brief 验证登陆Session服务器的连接指令
 *
 * 如果验证不通过直接断开连接
 *
 * \param ptCmd 登陆指令
 * \return 验证是否成功
 */
bool SessionTask::verifyLogin(const Cmd::Session::t_LoginSession *ptCmd)
{
	using namespace Cmd::Session;

	if (CMD_LOGIN == ptCmd->cmd
			&& PARA_LOGIN == ptCmd->para
			&& (SCENESSERVER == ptCmd->wdServerType || GATEWAYSERVER == ptCmd->wdServerType))
	{
		const Cmd::Super::ServerEntry *entry = SessionService::getInstance().getServerEntry(ptCmd->wdServerID);
		char strIP[32];
		strncpy(strIP, getIP(), 31);
		if (entry
				&& ptCmd->wdServerType == entry->wdServerType
				&& 0 == strcmp(strIP, entry->pstrIP))
		{
			wdServerID = ptCmd->wdServerID;
			wdServerType = ptCmd->wdServerType;
			return true;
		}
	}

	return false;
}

int SessionTask::verifyConn()
{
	int retcode = mSocket.recvToBuf_NoPoll();
	if (retcode > 0)
	{
		unsigned char pstrCmd[zSocket::MAX_DATASIZE];
		int nCmdLen = mSocket.recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
		if (nCmdLen <= 0)
			//这里只是从缓冲取数据包，所以不会出错，没有数据直接返回
			return 0;
		else
		{
			using namespace Cmd::Session;
			if (verifyLogin((t_LoginSession *)pstrCmd))
			{
				Zebra::logger->debug("客户端连接通过验证");
				return 1;
			}
			else
			{
				Zebra::logger->error("客户端连接验证失败");
				return -1;
			}
		}
	}
	else
		return retcode;
}

bool SessionTask::checkRecycle()
{
	if(recycle_state == 0)
	{
		return false;
	}
	if(recycle_state == 1)
	{
		//清理已经注册的用户
		UserSessionManager::getInstance()->removeAllUserByTask(this);
		//注销已经注册的地图
		SceneSessionManager::getInstance()->removeAllSceneByTask(this);
		recycle_state=2;
		return true;
	}
	return true;
}
int SessionTask::recycleConn()
{
	switch(recycle_state)
	{
		case 0:
			{
				recycle_state=1;
				return 0;
			}
			break;
		case 1:
			{
				return 0;
			}
			break;
		case 2:
			{
				return 1;
			}
			break;
	}
	return 1;
}

void SessionTask::addToContainer()
{
	SessionTaskManager::getInstance().addSessionTask(this);
}

void SessionTask::removeFromContainer()
{
	SessionTaskManager::getInstance().removeSessionTask(this);
}

bool SessionTask::uniqueAdd()
{
	return SessionTaskManager::getInstance().uniqueAdd(this);
}

bool SessionTask::uniqueRemove()
{
	return SessionTaskManager::getInstance().uniqueRemove(this);
}

/**
 * \brief 更换国籍
 *
 * \param dwUserID : 更换国籍的用户ID
 *
 */
bool SessionTask::change_country(const Cmd::Session::t_changeCountry_SceneSession* cmd)
{
	CUnionM::getMe().fireUnionMember(cmd->dwUserID, false);
	CSeptM::getMe().fireSeptMember(cmd->dwUserID, false); 

	//FieldSet* samplerelation = SessionService::metaData->getFields("SAMPLERELATION");
	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(cmd->dwUserID);
	if (!pUser) return true;
	CSchoolM::getMe().fireSchoolMember(pUser->name, false);
	CCountry* pCountry = CCountryM::getMe().find(pUser->country);

	if (pCountry)
	{
		//soke 2016 - 3 - 11 外交官（停用）
		if (strncmp(pCountry->diplomatName, pUser->name, MAX_NAMESIZE) == 0)
		{
			pCountry->cancelDiplomat();
		}
		//soke 2016 - 3 - 11 御史大夫（右上）
		if (strncmp(pCountry->censorName, pUser->name, MAX_NAMESIZE) == 0)
		{
			pCountry->cancelCensor();
		}
		//soke 2016 - 3 - 11 御史大夫（右下）
		if (strncmp(pCountry->censosName, pUser->name, MAX_NAMESIZE) == 0)
		{
			pCountry->cancelCensos();
		}
        //soke 巡捕（左）
		if (strncmp(pCountry->catcherName, pUser->name, MAX_NAMESIZE) == 0)
		{
			pCountry->cancelCatcher();
		}
		//soke 巡捕（右）
		if (strncmp(pCountry->catchexName, pUser->name, MAX_NAMESIZE) == 0)
		{
			pCountry->cancelCatchex();
		}
		//soke 元帅
		if (strncmp(pCountry->yuanshuaiName, pUser->name, MAX_NAMESIZE) == 0)
		{
			pCountry->cancelYuanshuai();
		}
		//soke 宰相
		if (strncmp(pCountry->zaixiangName, pUser->name, MAX_NAMESIZE) == 0)
		{
			pCountry->cancelZaixiang();
		}
	}
	
	Cmd::stUnmarryCmd unmarry;
	pUser->relationManager.processUserMessage(&unmarry, sizeof(unmarry));

	if (pUser)
	{
		pUser->relationManager.sendRelationList();
		pUser->country = cmd->dwToCountryID; 
	}

	Cmd::Session::t_returnChangeCountry_SceneSession send;
	send.dwUserID = cmd->dwUserID;
	if (pUser && pUser->scene) pUser->scene->sendCmd(&send, sizeof(send));

	return true;
}



/**
 * \brief 删除角色
 *
 * \param cmd : 删除角色命令
 * \param cmdLen: 命令长度
 *
 *
 */
bool SessionTask::del_role(const Cmd::t_NullCmd* cmd, const unsigned int cmdLen)
{
	Cmd::Session::t_DelChar_GateSession *rev=(Cmd::Session::t_DelChar_GateSession *)cmd;
	using namespace std;

	MailService::getMe().delMailByNameAndID(rev->name, rev->id);
	AuctionService::getMe().delAuctionRecordByName(rev->name);
	CartoonPetService::getMe().delPetRecordByID(rev->id);

	int retUnion	= 0;
	int retSept     = 0;
	int retSchool	= 0;

	rev->status = 0;

	retUnion  = CUnionM::getMe().fireUnionMember(rev->id, false);
	retSept   = CSeptM::getMe().fireSeptMember(rev->id, false); 
	retSchool = CSchoolM::getMe().fireSchoolMember(rev->name, false);
	RecommendM::getMe().fireRecommendSub(rev->id);

	//	if ( (retUnion > 0 )
	//	  && (retSept>0)
	//	  && (retSchool>0)
	//		)
	{// 没有帮主、师尊、族长的情况。皆能正常退出社会关系。则再进行相应的处理
		// 只要有一个社会关系不能退出，则取消操作(现在直接解除社会关系，不再判断了)
		//int ret = 0;

		/*ret = CUnionM::getMe().fireUnionMember(rev->id, false);

		  if (ret<=0)
		  {
		  rev->status = 4;
		  }

		  ret = CSeptM::getMe().fireSeptMember(rev->id, false);

		  if (ret<=0)
		  {
		  rev->status = 4;
		  }

		  ret = CSchoolM::getMe().fireSchoolMember(rev->name, false);

		  if (ret<=0)
		  {
		  rev->status = 4;
		  }*/

		// 从数据库中读取我的社会关系，并判断对方是否在线，如果不在线，则直接删除数据库记录，并在用户上线处理中
		// 把删除关系的命令发送给场景，让场景更新数据
		// 如果在线，则删除我自己的数据库记录，并调用对方的relationManager->removeRelation(我的角色名称)
		FieldSet* samplerelation = SessionService::metaData->getFields("SAMPLERELATION");

		Record where;
		std::ostringstream oss;
		oss << "charid='" << rev->id << "'";
		where.put("charid", oss.str());

		if (samplerelation)
		{
			//FunctionTimes times(56,"samplerelation");
			connHandleID handle = SessionService::dbConnPool->getHandle();

			RecordSet* recordset = NULL;

			if ((connHandleID)-1 != handle)
			{
				recordset = SessionService::dbConnPool->exeSelect(handle, samplerelation, NULL, &where);
			}

			SessionService::dbConnPool->putHandle(handle);

			if (recordset)
			{//清除我的所有关系
				for (unsigned int i=0; i<recordset->size(); i++)
				{
					Record* rec = recordset->get(i);

					UserSession *pOtherUser = NULL;

					if (rec)
					{
						pOtherUser = UserSessionManager::getInstance()->getUserSessionByName
							(rec->get("relationname"));
					}

					if (pOtherUser)
					{
						pOtherUser->relationManager.removeRelation(rev->name);
					}
					else
					{
						connHandleID handle = SessionService::dbConnPool->getHandle();

						where.clear();
						oss.str("");
						oss << "relationid='" << rev->id << "'";
						where.put("relationid", oss.str());

						if ((connHandleID)-1 != handle)
						{
							SessionService::dbConnPool->exeDelete(handle, samplerelation, &where);
						}

						SessionService::dbConnPool->putHandle(handle);
					}

					where.clear();
					oss.str("");
					oss << "charid='" << rev->id << "'";
					where.put("charid", oss.str());
					connHandleID handle = SessionService::dbConnPool->getHandle();

					if ((connHandleID)-1 != handle)
					{
						SessionService::dbConnPool->exeDelete(handle, samplerelation, &where);
					}
					SessionService::dbConnPool->putHandle(handle);
				}
			}


			// ---------------清除与我有关的所有社会关系记录---------------
			SAFE_DELETE(recordset);

			where.clear();
			oss.str("");
			oss << "relationid='" << rev->id << "'";
			where.put("relationid", oss.str());
			handle = SessionService::dbConnPool->getHandle();


			if ((connHandleID)-1 != handle)
			{
				recordset = SessionService::dbConnPool->exeSelect(handle, samplerelation, NULL, &where);
			}

			SessionService::dbConnPool->putHandle(handle);

			if (recordset)
			{//清除我的所有关系
				for (unsigned int i=0; i<recordset->size(); i++)
				{
					Record* rec = recordset->get(i);
					UserSession *pOtherUser = NULL;
					DWORD charid = 0;

					if (rec)
					{
						charid = rec->get("charid");
#ifdef _ZJW_DEBUG
						Zebra::logger->debug("charid:%d", charid);			
#endif							
						pOtherUser = UserSessionManager::getInstance()->getUserByID
							(charid);
					}

					if (pOtherUser)
					{
						pOtherUser->relationManager.removeRelation(rev->name);
					}
					else
					{
						connHandleID handle = SessionService::dbConnPool->getHandle();

						where.clear();
						oss.str("");
						oss << "relationid='" << rev->id << "'";
						where.put("relationid", oss.str());

						oss.str("");
						if (charid)
						{
							oss << "charid='" << charid << "'";
							where.put("charid", oss.str());
						}

						if ((connHandleID)-1 != handle)
						{
							SessionService::dbConnPool->exeDelete(handle, samplerelation, &where);
						}

						SessionService::dbConnPool->putHandle(handle);
					}
				}
				SAFE_DELETE(recordset)
			}
		}
	}

	if (rev->status == 0)
	{
		Zebra::logger->info("已解除社会关系，继续删除角色的处理");
	}
	else
	{
		Zebra::logger->info("存在不能解除的社会关系，删除角色操作被取消");
	}

	this->sendCmd(rev, sizeof(Cmd::Session::t_DelChar_GateSession));

	return true;
}

bool SessionTask::msgParse_Scene(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	if (CUnionM::getMe().processSceneMessage(cmd, cmdLen)) return true; //帮会的场景服务器消息处理
	if (CSchoolM::getMe().processSceneMessage(cmd, cmdLen)) return true;
	if (CSeptM::getMe().processSceneMessage(cmd, cmdLen)) return true;
	if (CQuizM::getMe().processSceneMessage(cmd, cmdLen)) return true;
	if (CNpcDareM::getMe().processSceneMessage(cmd, cmdLen)) return true;
	if (MailService::getMe().doMailCmd(cmd, cmdLen)) return true;

	switch (cmd->para)
	{
	///////////////////////////////////////////////////////
		//soke 升级好友祝贺
	case Cmd::Session::PARA_SCENE_LEVELUP:
		{
			Cmd::Session::t_levelup_SceneSession *rev = (Cmd::Session::t_levelup_SceneSession *)cmd;
			UserSession *pUser=UserSessionManager::getInstance()->getUserByID(rev->dwID);
			if(pUser)
			{										
				pUser->relationManager.FriendCongratulate(rev->dwID,rev->byName,rev->dwLevel,rev->dwType);
			}
		}
		break;
		//////////////////////////////////////////////////////////////////////////
		case Cmd::Session::PARA_DEBUG_COUNTRYPOWER:
			{
				time_t timValue = time(NULL);
				struct tm tmValue;
				zRTime::getLocalTime(tmValue, timValue);
				SessionService::getInstance().checkCountry(tmValue, true);
				return true;
			}
			break;
		case Cmd::Session::PARA_CLOSE_NPC:
			{
				SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_SEND_CMD:
			{
				Cmd::Session::t_sendCmd_SceneSession *rev = (Cmd::Session::t_sendCmd_SceneSession *)cmd;
				SceneSession * s = SceneSessionManager::getInstance()->getSceneByID(rev->mapID);
				if (!s) return true;

				return s->sendCmd(rev, sizeof(Cmd::Session::t_sendCmd_SceneSession)+rev->len);
			}
			break;
		case Cmd::Session::PARA_SET_SERVICE:
			{
				Cmd::Session::t_SetService_SceneSession *rev = (Cmd::Session::t_SetService_SceneSession *)cmd;

				char buf[32];
				bzero(buf, sizeof(buf));
				snprintf(buf, 32, "%u", rev->flag);
				Zebra::global["service_flag"] = buf;

				SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::PARA2_SET_SERVICE: //soke 封印等级(未转生)
			{
				Cmd::Session::t_SetFylevel_SceneSession *rev = (Cmd::Session::t_SetFylevel_SceneSession *)cmd;

				char buf[32];
				bzero(buf, sizeof(buf));
				snprintf(buf, 32, "%u", rev->flag);
				Zebra::global["serverfy_level"] = buf;

				SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::PARA3_SET_SERVICE: //soke 服务器最高等级(未转生)
			{
				Cmd::Session::t_SetMaxlevel_SceneSession *rev = (Cmd::Session::t_SetMaxlevel_SceneSession *)cmd;

				char buf[32];
				bzero(buf, sizeof(buf));
				snprintf(buf, 32, "%u", rev->flag);
				Zebra::global["servermax_level"] = buf;

				SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::PARA4_SET_SERVICE: //soke封印等级(转生)
			{
				Cmd::Session::t_SetTrunFylevel_SceneSession *rev = (Cmd::Session::t_SetTrunFylevel_SceneSession *)cmd;

				char buf[32];
				bzero(buf, sizeof(buf));
				snprintf(buf, 32, "%u", rev->flag);
				Zebra::global["trunfy_level"] = buf;

				SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::PARA5_SET_SERVICE: //soke 服务器最高等级(转生)
			{
				Cmd::Session::t_SetTrunMaxlevel_SceneSession *rev = (Cmd::Session::t_SetTrunMaxlevel_SceneSession *)cmd;

				char buf[32];
				bzero(buf, sizeof(buf));
				snprintf(buf, 32, "%u", rev->flag);
				Zebra::global["maxtrun_level"] = buf;

				SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::PARA_ADD_RELATION_ENEMY:
			{
				Cmd::Session::t_addRelationEnemy *rev = (Cmd::Session::t_addRelationEnemy *)cmd;
				UserSession* pUser = UserSessionManager::getInstance()->getUserByID(rev->dwUserID);
				if (!pUser) return false;
				pUser->relationManager.addEnemyRelation(rev->name);
				return true;
			}
			break;
		case Cmd::Session::PARA_GXJCKJ://功勋竞猜开奖
			{
				Cmd::Session::t_Gxjckj_SceneSession *rev = (Cmd::Session::t_Gxjckj_SceneSession *)cmd;
				connHandleID handle = SessionService::dbConnPool->getHandle();
						if ((connHandleID)-1 == handle)
						{               
							Zebra::logger->error("[功勋竞猜]: 得到数据库句柄失败");
							return false;
						}
						RecordSet* recordset = NULL;
						FieldSet* city = SessionService::metaData->getFields("CHARBASE");
						Record order;
						order.put("LEVEL","desc");
						if ((connHandleID)-1 != handle)
						{
							recordset = SessionService::dbConnPool->exeSelect(handle, city, NULL, NULL,&order);
						}
						SessionService::dbConnPool->putHandle(handle);
						if (recordset)
						{
							for (unsigned int i=0; i<recordset->size(); i++)
							{
								Record* rec = recordset->get(i);
								int chid=rec->get("CHARID");
								Zebra::logger->info("[功勋竞猜]: 开始对%d号玩家竞猜信息校验",chid);
								int objectid=0;
								int objectnum=0;
								int objectlevel=0;
								switch (rev->index)
								{
								case 0:
									{
										objectid=rec->get("JINGCAI0_OBJECTID");
										objectnum=rec->get("JINGCAI0_OBJECTNUM");
										objectlevel=rec->get("JINGCAI0_OBJECTLEVEL");
									}
									break;
								case 1:
									{
										objectid=rec->get("JINGCAI1_OBJECTID");
										objectnum=rec->get("JINGCAI1_OBJECTNUM");
										objectlevel=rec->get("JINGCAI1_OBJECTLEVEL");
									}
									break;
								case 2:
									{
										objectid=rec->get("JINGCAI2_OBJECTID");
										objectnum=rec->get("JINGCAI2_OBJECTNUM");
										objectlevel=rec->get("JINGCAI2_OBJECTLEVEL");
									}
									break;
								case 3:
									{
										objectid=rec->get("JINGCAI3_OBJECTID");
										objectnum=rec->get("JINGCAI3_OBJECTNUM");
										objectlevel=rec->get("JINGCAI3_OBJECTLEVEL");
									}
									break;
								case 4:
									{
										objectid=rec->get("JINGCAI4_OBJECTID");
										objectnum=rec->get("JINGCAI4_OBJECTNUM");
										objectlevel=rec->get("JINGCAI4_OBJECTLEVEL");
									}
									break;
								case 5:
									{
										objectid=rec->get("JINGCAI5_OBJECTID");
										objectnum=rec->get("JINGCAI5_OBJECTNUM");
										objectlevel=rec->get("JINGCAI5_OBJECTLEVEL");
									}
									break;
								case 6:
									{
										objectid=rec->get("JINGCAI6_OBJECTID");
										objectnum=rec->get("JINGCAI6_OBJECTNUM");
										objectlevel=rec->get("JINGCAI6_OBJECTLEVEL");
									}
									break;
								case 7:
									{
										objectid=rec->get("JINGCAI7_OBJECTID");
										objectnum=rec->get("JINGCAI7_OBJECTNUM");
										objectlevel=rec->get("JINGCAI7_OBJECTLEVEL");
									}
									break;
								case 8:
									{
										objectid=rec->get("JINGCAI8_OBJECTID");
										objectnum=rec->get("JINGCAI8_OBJECTNUM");
										objectlevel=rec->get("JINGCAI8_OBJECTLEVEL");
									}
									break;
								case 9:
									{
										objectid=rec->get("JINGCAI9_OBJECTID");
										objectnum=rec->get("JINGCAI9_OBJECTNUM");
										objectlevel=rec->get("JINGCAI9_OBJECTLEVEL");
									}
									break;
								
								default:
									break;
								}

							
								

								Zebra::logger->info("[功勋竞猜]: 校验完毕，ID:%d NUM:%d LEVEL:%d",objectid,objectnum,objectlevel);
								if(objectid!=0 && objectnum!=0)
								{
									Cmd::Session::t_sendGongxunRankReward pSend;
									pSend.charID = chid;
									Zebra::logger->debug("charID:%d",pSend.charID);
									bcopy(rec->get("NAME"), pSend.name, MAX_NAMESIZE+1 );
									Zebra::logger->debug("name:%s", pSend.name);
									pSend.itemId =objectid;
									Zebra::logger->debug("itemId:%d", pSend.itemId);
									pSend.itemNum = objectnum;
									Zebra::logger->debug("itemNum:%d", pSend.itemNum);
									pSend.itemLevel = objectlevel;
									Zebra::logger->debug("itemLevel:%d", pSend.itemLevel);
									pSend.beishu = atoi(Zebra::global["gongxun_big"].c_str());;//单中箱子倍数
									SessionTaskManager::getInstance().broadcastScene(&pSend, sizeof(pSend));
								}
								
								//开始判断大小箱子
								int objectid10=0;
								int objectnum10=0;
								int objectlevel10=0;
								int objectid11=0;
								int objectnum11=0;
								int objectlevel11=0;
								objectid10=rec->get("JINGCAI10_OBJECTID");
								objectnum10=rec->get("JINGCAI10_OBJECTNUM");
								objectlevel10=rec->get("JINGCAI10_OBJECTLEVEL");
								objectid11=rec->get("JINGCAI11_OBJECTID");
								objectnum11=rec->get("JINGCAI11_OBJECTNUM");
								objectlevel11=rec->get("JINGCAI11_OBJECTLEVEL");
								Zebra::logger->info("[功勋竞猜]: 小号箱子校验完毕，ID:%d NUM:%d LEVEL:%d",objectid10,objectnum10,objectlevel10);
								Zebra::logger->info("[功勋竞猜]: 大号箱子校验完毕，ID:%d NUM:%d LEVEL:%d",objectid11,objectnum11,objectlevel11);
					
								if(rev->index==0 ||rev->index==1 || rev->index==2 || rev->index==3 || rev->index==4)
								{
									if(objectid10!=0 && objectnum10!=0 ) //小号箱子中了
									{
										Cmd::Session::t_sendGongxunRankReward pSend;
										pSend.charID = chid;
										Zebra::logger->debug("charID:%d",pSend.charID);
										bcopy(rec->get("NAME"), pSend.name, MAX_NAMESIZE+1 );
										Zebra::logger->debug("name:%s", pSend.name);
										pSend.itemId =objectid10;
										Zebra::logger->debug("itemId:%d", pSend.itemId);
										pSend.itemNum = objectnum10;
										Zebra::logger->debug("itemNum:%d", pSend.itemNum);
										pSend.itemLevel = objectlevel10;
										Zebra::logger->debug("itemLevel:%d", pSend.itemLevel);
										pSend.beishu = atoi(Zebra::global["gongxun_smile"].c_str());;//单中箱子倍数
										SessionTaskManager::getInstance().broadcastScene(&pSend, sizeof(pSend));	
									}
								}
								if(rev->index==5 ||rev->index==6 || rev->index==7 || rev->index==8 || rev->index==9)
								{
									if(objectid11!=0 && objectnum11!=0 ) //大号箱子中了
									{
										Cmd::Session::t_sendGongxunRankReward pSend;
										pSend.charID = chid;
										Zebra::logger->debug("charID:%d",pSend.charID);
										bcopy(rec->get("NAME"), pSend.name, MAX_NAMESIZE+1 );
										Zebra::logger->debug("name:%s", pSend.name);
										pSend.itemId =objectid11;
										Zebra::logger->debug("itemId:%d", pSend.itemId);
										pSend.itemNum = objectnum11;
										Zebra::logger->debug("itemNum:%d", pSend.itemNum);
										pSend.itemLevel = objectlevel11;
										Zebra::logger->debug("itemLevel:%d", pSend.itemLevel);
										pSend.beishu = atoi(Zebra::global["gongxun_smile"].c_str());;//单中箱子倍数
										SessionTaskManager::getInstance().broadcastScene(&pSend, sizeof(pSend));
									}
								}
								
								//全部发奖完毕
								Cmd::Session::t_sendGongxunRankReward2 pSend;
								pSend.charID = chid;
								SessionTaskManager::getInstance().broadcastScene(&pSend, sizeof(pSend));
								Zebra::logger->info("[功勋竞猜]: 执行Sc清零 ID:%d ",chid);
								
									
							}

							Record recs,where;
							std::ostringstream oss;
							recs.put("JINGCAI0_OBJECTID", 0);
							recs.put("JINGCAI0_OBJECTNUM", 0);
							recs.put("JINGCAI0_OBJECTLEVEL",0);
							recs.put("JINGCAI1_OBJECTID", 0);
							recs.put("JINGCAI1_OBJECTNUM", 0);
							recs.put("JINGCAI1_OBJECTLEVEL",0);
							recs.put("JINGCAI2_OBJECTID", 0);
							recs.put("JINGCAI2_OBJECTNUM", 0);
							recs.put("JINGCAI2_OBJECTLEVEL",0);
							recs.put("JINGCAI3_OBJECTID", 0);
							recs.put("JINGCAI3_OBJECTNUM", 0);
							recs.put("JINGCAI3_OBJECTLEVEL",0);
							recs.put("JINGCAI4_OBJECTID", 0);
							recs.put("JINGCAI4_OBJECTNUM", 0);
							recs.put("JINGCAI4_OBJECTLEVEL",0);
							recs.put("JINGCAI5_OBJECTID", 0);
							recs.put("JINGCAI5_OBJECTNUM", 0);
							recs.put("JINGCAI5_OBJECTLEVEL",0);
							recs.put("JINGCAI6_OBJECTID", 0);
							recs.put("JINGCAI6_OBJECTNUM", 0);
							recs.put("JINGCAI6_OBJECTLEVEL",0);
							recs.put("JINGCAI7_OBJECTID", 0);
							recs.put("JINGCAI7_OBJECTNUM", 0);
							recs.put("JINGCAI7_OBJECTLEVEL",0);
							recs.put("JINGCAI8_OBJECTID", 0);
							recs.put("JINGCAI8_OBJECTNUM", 0);
							recs.put("JINGCAI8_OBJECTLEVEL",0);
							recs.put("JINGCAI9_OBJECTID", 0);
							recs.put("JINGCAI9_OBJECTNUM", 0);
							recs.put("JINGCAI9_OBJECTLEVEL",0);
							recs.put("JINGCAI10_OBJECTID", 0);
							recs.put("JINGCAI10_OBJECTNUM", 0);
							recs.put("JINGCAI10_OBJECTLEVEL",0);
							recs.put("JINGCAI11_OBJECTID", 0);
							recs.put("JINGCAI11_OBJECTNUM", 0);
							recs.put("JINGCAI11_OBJECTLEVEL",0);
							FieldSet* ally = SessionService::metaData->getFields("CHARBASE");

							if (ally)
							{       
								connHandleID handle = SessionService::dbConnPool->getHandle();

								if ((connHandleID)-1 == handle)
								{
									Zebra::logger->error("不能获取数据库句柄");
									return false;
								}

								if ((connHandleID)-1 != handle)
								{
									SessionService::dbConnPool->exeUpdate(handle, ally, &recs, NULL);
								}

								SessionService::dbConnPool->putHandle(handle);
							}
							else
							{
								Zebra::logger->error("功勋竞猜初始化失败");
								return false;
							}
						}
				
				return true;
			}
			break;
		case Cmd::Session::PARA_JIAZUBOSS://家族BOSS信息

			{
				Cmd::Session::t_JiazuBoss_SceneSession *rev = (Cmd::Session::t_JiazuBoss_SceneSession *)cmd;
				UserSession* pUser = UserSessionManager::getInstance()->getUserByID(rev->userID);
				if (!pUser) return false;

				CSept * sept = CSeptM::getMe().getSeptByID(pUser->septid);
				if (sept)
				{
					int exp = 0;
					int level = 1;
					exp = sept->bossexp;
					level = (exp/1000)+1;
					if(level>=10)
					{
						level = 10;
					}

					exp = exp - (1000*(level-1));

					Cmd::stJiazuBossInfoReturnCmd send;
					if (pUser)
					{
						send.level = level;
						send.exp = exp;
						pUser->sendCmdToMe(&send, sizeof(Cmd::stJiazuBossInfoReturnCmd));
					}
				}
				
				return true;
			}
			break;
		case Cmd::Session::PARA_WEIYANGJIAZUBOSS://家族BOSS喂养
			{
				Cmd::Session::t_WeiyangJiazuBoss_SceneSession *rev = (Cmd::Session::t_WeiyangJiazuBoss_SceneSession *)cmd;
				UserSession* pUser = UserSessionManager::getInstance()->getUserByID(rev->userID);
				if (!pUser) return false;

				CSept * sept = CSeptM::getMe().getSeptByID(pUser->septid);
				if (sept)
				{	
					sept->bossexp+=10;	
					Cmd::stJiazuBossWeiyangReturnCmd send;
					if (pUser)
					{
						send.state = 1;
						pUser->sendCmdToMe(&send, sizeof(Cmd::stJiazuBossWeiyangReturnCmd));
					}
				}
				
				return true;
			}
			break;
		case Cmd::Session::PARA_ZHAOHUANJIAZUBOSS://家族BOSS召唤

			{
				Cmd::Session::t_ZhaoHuanJiazuBoss_SceneSession *rev = (Cmd::Session::t_ZhaoHuanJiazuBoss_SceneSession *)cmd;
				UserSession* pUser = UserSessionManager::getInstance()->getUserByID(rev->userID);
				if (!pUser) return false;

				CSept * sept = CSeptM::getMe().getSeptByID(pUser->septid);
				if (sept)
				{	
					int exp=0;
					exp = sept->bossexp;
					int level = (exp/1000)+1;
					if(level>=10)
					{
						level = 10;
					}

					if(level<=1)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "BOSS等级太低无法召唤，请先喂养");
						return true;	
					}
					if(sept->master->id != pUser->id)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "只有家族族长才有权限召唤家族BOSS");
						return true;
					}

					//发给场景 召唤BOSS
					sept->bossexp = 0;

					Cmd::Session::t_sendZhaoHuanJiazuBossRankReward pSend;
					pSend.charID = rev->userID;
					pSend.level = level;
					SessionTaskManager::getInstance().broadcastScene(&pSend, sizeof(pSend));

				}
				
				return true;
			}
			break;
		
		case Cmd::Session::PARA_ZHANCHELIST://查询 战车列表
			{
				Cmd::Session::t_ZhancheList_SceneSession *rev = (Cmd::Session::t_ZhancheList_SceneSession *)cmd;
				UserSession* pUser = UserSessionManager::getInstance()->getUserByID(rev->dwUserID);
				if (!pUser) return false;
				connHandleID handle = SessionService::dbConnPool->getHandle();
						if ((connHandleID)-1 == handle)
						{               
							Zebra::logger->error("[战车系统]: 得到数据库句柄失败");
							return false;
						}
						RecordSet* recordset = NULL;
						FieldSet* city = SessionService::metaData->getFields("TANKINFO");
						std::ostringstream oss; 
						Record where;                           
				
						oss << "SEPTID = " << rev->septid;
						where.put("SEPTID",oss.str());
						if ((connHandleID)-1 != handle)
						{
							recordset = SessionService::dbConnPool->exeSelect(handle, city, NULL,&where);
						}
						SessionService::dbConnPool->putHandle(handle);
						if (recordset)
						{
							BUFFER_CMD(Cmd::stZhancheListReturnCmd ,pSend , zSocket::MAX_USERDATASIZE);
							int pnum=0;
							for (unsigned int i=0; i<20; i++)
							{
								if(i>=recordset->size())
								{
									(pSend->zhanche + pnum )->id = 0;
									(pSend->zhanche + pnum )->septid = 0;
									bcopy("", (pSend->zhanche + pnum)->name, MAX_NAMESIZE);
									(pSend->zhanche + pnum )->type = 0;
									(pSend->zhanche + pnum )->lingyongid = 0;
									bcopy("", (pSend->zhanche + pnum)->lingyongname, MAX_NAMESIZE);
									pnum+=1;
								}
								else{
									Record* rec = recordset->get(i);
									(pSend->zhanche + pnum )->id = rec->get("ID");
									(pSend->zhanche + pnum )->septid = rec->get("SEPTID");
									bcopy((const char*)rec->get("NAME"), (pSend->zhanche + pnum)->name, MAX_NAMESIZE);
									(pSend->zhanche + pnum )->type = rec->get("TYPE");
									(pSend->zhanche + pnum )->lingyongid = rec->get("LINGYONGID");
									bcopy((const char*)rec->get("LINGYONGNAME"), (pSend->zhanche + pnum)->lingyongname, MAX_NAMESIZE);
									pnum+=1;
								}
								
							}

							pUser->sendCmdToMe(pSend, sizeof(Cmd::stZhancheListReturnCmd) + pnum * sizeof(pSend->zhanche[0]));
						}					
						return true;	
			}
			break;
		case Cmd::Session::PARA_ZHANCHEGUIHUAN://战车 归还
			{
					Cmd::Session::t_ZhancheGuiHuan_SceneSession *rev = (Cmd::Session::t_ZhancheGuiHuan_SceneSession *)cmd;
					UserSession* pUser = UserSessionManager::getInstance()->getUserByID(rev->dwUserID);
					if (!pUser) return false;
					connHandleID handle = SessionService::dbConnPool->getHandle();
							if ((connHandleID)-1 == handle)
							{               
								Zebra::logger->error("[战车系统]: 得到数据库句柄失败");
								return false;
							}
							RecordSet* recordset = NULL;
							FieldSet* city = SessionService::metaData->getFields("TANKINFO");
							std::ostringstream oss; 
							Record where;                           
					
							oss << "ID = " << rev->zhancheid;
							where.put("id",oss.str());
							if ((connHandleID)-1 != handle)
							{
								recordset = SessionService::dbConnPool->exeSelect(handle, city, NULL,&where);
							}
							SessionService::dbConnPool->putHandle(handle);
							if (recordset)
							{
								if(recordset->size()==1)
								{
									Record* rec = recordset->get(0);
									DWORD zhancheid = rec->get("ID");
									DWORD septid = rec->get("SEPTID");
									DWORD type = rec->get("TYPE");
									DWORD lingyongid = rec->get("LINGYONGID");
									if(lingyongid!=rev->dwUserID)
									{
										pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "此战车不是你在领用");
										return true;	
									}
									
									Record recs,wheres;
									std::ostringstream oss;
									recs.put("LINGYONGID", 0);
									recs.put("LINGYONGNAME", "");
									oss << "ID='" << zhancheid << "'";
									wheres.put("ID", oss.str());
									FieldSet* ally = SessionService::metaData->getFields("TANKINFO");
									if (ally)
									{       
									 	handle = SessionService::dbConnPool->getHandle();
										if ((connHandleID)-1 == handle)
										{
											Zebra::logger->error("不能获取数据库句柄");
											return false;
										}
										if ((connHandleID)-1 != handle)
										{
											SessionService::dbConnPool->exeUpdate(handle, ally, &recs, &wheres);
											//归还战车
											Cmd::Session::t_sendZhancheGuihuanRankReward pSend;
											pSend.charID = rev->dwUserID;
											pSend.zhancheid = zhancheid;
											SessionTaskManager::getInstance().broadcastScene(&pSend, sizeof(pSend));
											Zebra::logger->info("[战车系统]: 归还战车 ID:%d ",zhancheid);
										}
										SessionService::dbConnPool->putHandle(handle);
									}
									else
									{
										Zebra::logger->error("[战车系统]战车领用失败，数据库执行失败");
										return false;
									}
								}
								else{
									pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "未查询到你要领取的战车信息");
									return true;
								}
							}					
							return true;	
			}
			break;
		case Cmd::Session::PARA_ZHANCHELINGYONG://领用 战车列表
			{
					Cmd::Session::t_ZhancheLingYong_SceneSession *rev = (Cmd::Session::t_ZhancheLingYong_SceneSession *)cmd;
					UserSession* pUser = UserSessionManager::getInstance()->getUserByID(rev->dwUserID);
					if (!pUser) return false;
					connHandleID handle = SessionService::dbConnPool->getHandle();
							if ((connHandleID)-1 == handle)
							{               
								Zebra::logger->error("[战车系统]: 得到数据库句柄失败");
								return false;
							}
							RecordSet* recordset = NULL;
							FieldSet* city = SessionService::metaData->getFields("TANKINFO");
							std::ostringstream oss; 
							Record where;                           
					
							oss << "ID = " << rev->zhancheid;
							where.put("id",oss.str());
							if ((connHandleID)-1 != handle)
							{
								recordset = SessionService::dbConnPool->exeSelect(handle, city, NULL,&where);
							}
							SessionService::dbConnPool->putHandle(handle);
							if (recordset)
							{
								if(recordset->size()==1)
								{
									Record* rec = recordset->get(0);
									DWORD zhancheid = rec->get("ID");
									DWORD septid = rec->get("SEPTID");
									DWORD type = rec->get("TYPE");
									DWORD lingyongid = rec->get("LINGYONGID");
									if(lingyongid!=0)
									{
										pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "此战车已被他人领用");
										return true;	
									}
									
									Record recs,wheres;
									std::ostringstream oss;
									recs.put("LINGYONGID", rev->dwUserID);
									recs.put("LINGYONGNAME", pUser->name);
									oss << "ID='" << zhancheid << "'";
									wheres.put("ID", oss.str());
									FieldSet* ally = SessionService::metaData->getFields("TANKINFO");
									if (ally)
									{       
									 	handle = SessionService::dbConnPool->getHandle();
										if ((connHandleID)-1 == handle)
										{
											Zebra::logger->error("不能获取数据库句柄");
											return false;
										}
										if ((connHandleID)-1 != handle)
										{
											SessionService::dbConnPool->exeUpdate(handle, ally, &recs, &wheres);
											//领用战车
											Cmd::Session::t_sendZhancheLingYongRankReward pSend;
											pSend.charID = rev->dwUserID;
											pSend.zhancheid = zhancheid;
											pSend.zhanchetype = type;
											pSend.state = 1;
											SessionTaskManager::getInstance().broadcastScene(&pSend, sizeof(pSend));
											Zebra::logger->info("[战车系统]: 领用战车 ID:%d ",zhancheid);
										}
										SessionService::dbConnPool->putHandle(handle);
									}
									else
									{
										Zebra::logger->error("[战车系统]战车领用失败，数据库执行失败");
										return false;
									}
								}
								else{
									pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "未查询到你要领取的战车信息");
									return true;
								}
							}					
							return true;	
			}
			break;

		case Cmd::Session::PARA_ZLFF://战力榜离线发放
			{
				Cmd::Session::t_Zhanlifafang_SceneSession *rev = (Cmd::Session::t_Zhanlifafang_SceneSession *)cmd;
				connHandleID handle = SessionService::dbConnPool->getHandle();
						if ((connHandleID)-1 == handle)
						{               
							Zebra::logger->error("[排行榜发放]: 得到数据库句柄失败");
							return false;
						}
						RecordSet* recordset = NULL;
						FieldSet* city = SessionService::metaData->getFields("CHARBASE");
					
						Record recs,where;
						std::ostringstream oss;
						recs.put("ZHANLICOUNT", rev->index);
						oss << "CHARID='" << rev->charid << "'";
						where.put("CHARID", oss.str());
						FieldSet* ally = SessionService::metaData->getFields("CHARBASE");
						if (ally)
						{       
							connHandleID handle = SessionService::dbConnPool->getHandle();
							if ((connHandleID)-1 == handle)
							{
								Zebra::logger->error("不能获取数据库句柄");
								return false;
							}
							if ((connHandleID)-1 != handle)
							{
								SessionService::dbConnPool->exeUpdate(handle, ally, &recs, &where);
							}
							SessionService::dbConnPool->putHandle(handle);
						}
						else
						{
							Zebra::logger->error("排行榜发放失败");
							return false;
						}	
			}
			break;	

		case Cmd::Session::PARA_HGFF://护国榜离线发放
			{
				Cmd::Session::t_Huguofafang_SceneSession *rev = (Cmd::Session::t_Huguofafang_SceneSession *)cmd;
				connHandleID handle = SessionService::dbConnPool->getHandle();
						if ((connHandleID)-1 == handle)
						{               
							Zebra::logger->error("[排行榜发放]: 得到数据库句柄失败");
							return false;
						}
						RecordSet* recordset = NULL;
						FieldSet* city = SessionService::metaData->getFields("CHARBASE");
					
						Record recs,where;
						std::ostringstream oss;
						recs.put("HEROCOUNT", rev->index);
						oss << "CHARID='" << rev->charid << "'";
						where.put("CHARID", oss.str());
						FieldSet* ally = SessionService::metaData->getFields("CHARBASE");
						if (ally)
						{       
							connHandleID handle = SessionService::dbConnPool->getHandle();
							if ((connHandleID)-1 == handle)
							{
								Zebra::logger->error("不能获取数据库句柄");
								return false;
							}
							if ((connHandleID)-1 != handle)
							{
								SessionService::dbConnPool->exeUpdate(handle, ally, &recs, &where);
							}
							SessionService::dbConnPool->putHandle(handle);
						}
						else
						{
							Zebra::logger->error("排行榜发放失败");
							return false;
						}	
			}
			break;

			case Cmd::Session::PARA_XHFF://鲜花榜离线发放
			{
				Cmd::Session::t_Xianhuafafang_SceneSession *rev = (Cmd::Session::t_Xianhuafafang_SceneSession *)cmd;
				connHandleID handle = SessionService::dbConnPool->getHandle();
						if ((connHandleID)-1 == handle)
						{               
							Zebra::logger->error("[排行榜发放]: 得到数据库句柄失败");
							return false;
						}
						RecordSet* recordset = NULL;
						FieldSet* city = SessionService::metaData->getFields("CHARBASE");
					
						Record recs,where;
						std::ostringstream oss;
						recs.put("XHBBCOUNT", rev->index);
						oss << "CHARID='" << rev->charid << "'";
						where.put("CHARID", oss.str());
						FieldSet* ally = SessionService::metaData->getFields("CHARBASE");
						if (ally)
						{       
							connHandleID handle = SessionService::dbConnPool->getHandle();
							if ((connHandleID)-1 == handle)
							{
								Zebra::logger->error("不能获取数据库句柄");
								return false;
							}
							if ((connHandleID)-1 != handle)
							{
								SessionService::dbConnPool->exeUpdate(handle, ally, &recs, &where);
							}
							SessionService::dbConnPool->putHandle(handle);
						}
						else
						{
							Zebra::logger->error("排行榜发放失败");
							return false;
						}	
			}
			break;	
		case Cmd::Session::PARA_SPEND_GOLD:
			{
				Cmd::Session::t_SpendGold_SceneSession *rev = (Cmd::Session::t_SpendGold_SceneSession *)cmd;
				UserSession* pUser = UserSessionManager::getInstance()->getUserByID(rev->userID);
				if (!pUser) return false;

				CSept * sept = CSeptM::getMe().getSeptByID(pUser->septid);
				if (sept)
				{
					DWORD num = ((DWORD)(rev->gold/100))*2;
					sept->sendGoldToMember(rev->userID, num);
				}
				/*
				CSept * sept = CSeptM::getMe().getSeptByID(pUser->septid);
				if (sept)
				{
					DWORD m = sept->dwSpendGold/10;
					sept->dwSpendGold += rev->gold;
					DWORD n = sept->dwSpendGold/10;
					if (n-m)
					{
						sept->dwRepute += n-m;
						sept->sendSeptNotify("家族成员 %s 消费金币,家族声望提高了 %u 点", pUser->name, n-m);
					}
				}
				*/
				return true;
			}
			break;
		//soke 2016 - 3 - 10 师徒给银子
		case Cmd::Session::OVERMAN_MONEY_ADD:
			{
				Cmd::Session::t_OvermanMoneyAdd *command = (Cmd::Session::t_OvermanMoneyAdd*)cmd;
				UserSession* pUser = UserSessionManager::getInstance()->getUserByID(command->id);
				if(pUser)
				{
					Cmd::Session::t_OvermanMoneyAdd add;
					add.id=command->id;
					add.money=command->money;
					strncpy(add.name,command->name,MAX_NAMESIZE);
					pUser->scene->sendCmd(&add,sizeof(Cmd::Session::t_OvermanMoneyAdd));
				}
				return true;
			}
			break;

		case Cmd::Session::QUEST_BULLETIN_USERCMD_PARA:
			{
				Cmd::Session::t_QuestBulletinUserCmd* command = (Cmd::Session::t_QuestBulletinUserCmd*)cmd;
				if (command->kind == 1) //soke 发送帮会公告
				{
					CUnionM::getMe().sendUnionNotify(command->id, command->content);
					return true;
				}

				if (command->kind == 2) //soke 发送家族公告
				{
					CSeptM::getMe().sendSeptNotify(command->id, command->content);
					return true;
				}
			}
			break;

		case Cmd::Session::QUEST_CHANGE_AP:
			{
				Cmd::Session::t_ChangeAP* command = (Cmd::Session::t_ChangeAP*) cmd;
				CUnion* u = CUnionM::getMe().getUnionByID(command->id);
				if (u) {
					u->changeActionPoint(command->point);
				}
			}
			break;			

		case Cmd::Session::PARA_CHANGE_USER_DATA:
			{
				Cmd::Session::t_changeUserData_SceneSession* rev = 
					(Cmd::Session::t_changeUserData_SceneSession*)cmd;

				UserSession* pUser = UserSessionManager::getInstance()->getUserByID(rev->dwUserID);
				if (pUser)
				{
					pUser->level = rev->wdLevel;
					pUser->dwExploit = rev->dwExploit;
					pUser->dwGrace = rev->dwGrace;
				}

				return true;
			}
			break;

		case Cmd::Session::PARA_AUCTION_CMD:
			{
				Cmd::Session::t_AuctionCmd * rev = (Cmd::Session::t_AuctionCmd *)cmd;
				return AuctionService::getMe().doAuctionCmd(rev, cmdLen);
			}
			break;
		case Cmd::Session::PARA_CARTOON_CMD:
			{
				Cmd::Session::t_CartoonCmd * rev = (Cmd::Session::t_CartoonCmd *)cmd;
				return CartoonPetService::getMe().doCartoonCmd(rev, cmdLen);
			}
			break;
		case Cmd::Session::PARA_SERVER_NOTIFY://soke 发送世界信息
			{
				Cmd::Session::t_serverNotify_SceneSession* rev = 
					(Cmd::Session::t_serverNotify_SceneSession*)cmd;

				SessionChannel::sendAllInfo(rev->infoType, rev->info);
				return true;
			}
			break;
		case Cmd::Session::PARA_COUNTRY_NOTIFY://soke 发送国家信息
			{
				Cmd::Session::t_countryNotify_SceneSession* rev = 
					(Cmd::Session::t_countryNotify_SceneSession*)cmd;

				SessionChannel::sendCountryInfo(rev->infoType, 
						rev->dwCountryID, "%s", rev->info);
				return true;
			}
			break;
		case Cmd::Session::PARA_CHANGE_COUNTRY:
			{
				Cmd::Session::t_changeCountry_SceneSession* rev = 
					(Cmd::Session::t_changeCountry_SceneSession*)cmd;
				this->change_country(rev);

				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_FORWARD_USER:
			{
				Cmd::Session::t_forwardUser_SceneSession * rev = (Cmd::Session::t_forwardUser_SceneSession *)cmd;

				UserSession* pUser = 0;
				if (rev->id)
					pUser = UserSessionManager::getInstance()->getUserByID(rev->id);
				if (!pUser && rev->tempid)
					pUser = UserSessionManager::getInstance()->getUserByTempID(rev->id);
				if (!pUser && !strncmp("", rev->name, MAX_NAMESIZE))
					pUser = UserSessionManager::getInstance()->getUserSessionByName(rev->name);

				if (pUser)
					pUser->sendCmdToMe(rev->cmd, rev->cmd_len);

				return true;
			}
			break;
		case Cmd::Session::PARA_RETURN_OBJECT:
			{
				Cmd::Session::t_returnObject_SceneSession* rev = (Cmd::Session::t_returnObject_SceneSession*)cmd;
				UserSession* pUser = UserSessionManager::getInstance()->getUserSessionByName(rev->to_name);
				Cmd::stReturnQuestionObject send;

				if (pUser)
				{
					strncpy(send.name, rev->from_name, MAX_NAMESIZE);
					memcpy(&send.object, &rev->object, sizeof(t_Object));
					pUser->sendCmdToMe(&send, sizeof(Cmd::stReturnQuestionObject));
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_REGSCENE:
			{
				Cmd::Session::t_regScene_SceneSession *reg=(Cmd::Session::t_regScene_SceneSession *)cmd;
				SceneSession *scene=new SceneSession(this);
				Cmd::Session::t_regScene_ret_SceneSession ret;
				ret.dwTempID=reg->dwTempID;
				if(scene->reg(reg) && SceneSessionManager::getInstance()->addScene(scene))
				{
					ret.byValue=Cmd::Session::REGSCENE_RET_REGOK;
					CCountryM::getMe().refreshTax();
					CCountryM::getMe().refreshTech(this, reg->dwCountryID);
					if (KING_CITY_ID==(reg->dwID&0x0000ffff))
						CCountryM::getMe().refreshGeneral(reg->dwCountryID);
					CAllyM::getMe().refreshAlly(this);

					Zebra::logger->debug("注册地图%u(%s %s) 成功",reg->dwID , reg->byName, reg->fileName);
					CCityM::getMe().refreshUnion(reg->dwCountryID, reg->dwID & 0x0FFF);

					return true;
				}
				else
				{
					ret.byValue=Cmd::Session::REGSCENE_RET_REGERR;
					Zebra::logger->error("注册地图%u(%s %s)失败",reg->dwID , reg->byName, reg->fileName);
				}
				sendCmd(&ret,sizeof(ret));
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_UNREGUSER:
			{
				Cmd::Session::t_unregUser_SceneSession *unreg=(Cmd::Session::t_unregUser_SceneSession *)cmd;
				UserSession *pUser=UserSessionManager::getInstance()->getUserByID(unreg->dwUserID);
				if(pUser)
				{
					CSortM::getMe().offlineCount(pUser);
					CUnionM::getMe().userOffline(pUser); // 用于处理帮会成员下线
					CSchoolM::getMe().userOffline(pUser);
					CSeptM::getMe().userOffline(pUser);
					CQuizM::getMe().userOffline(pUser);
					CGemM::getMe().userOffline(pUser);

					UserSessionManager::getInstance()->removeUser(pUser);
					if(unreg->retcode==Cmd::Session::UNREGUSER_RET_ERROR)
						Zebra::logger->debug("ScenesServer错误，注销%s(%ld)",pUser->name,pUser->id);
					else
						Zebra::logger->debug("场景请求用户%s(%ld)注销",pUser->name,pUser->id);
					SAFE_DELETE(pUser);
				}
				return true;
			}
		case Cmd::Session::PARA_SCENE_REGUSERSUCCESS:
			{
				Cmd::Session::t_regUserSuccess_SceneSession *regsuccess=(Cmd::Session::t_regUserSuccess_SceneSession *)cmd;
				UserSession *pUser=UserSessionManager::getInstance()->getUserByID(regsuccess->dwUserID);
				if(pUser)
				{
					pUser->dwExploit = regsuccess->dwExploit;
					pUser->dwGrace = regsuccess->dwGrace;
					pUser->qwExp = regsuccess->qwExp;
					pUser->setRelationData(regsuccess);
					pUser->relationManager.init();		// 初始化这个用户的好友列表

					pUser->updateConsort();			// 更新配偶状态到场景

					if (pUser->level >= 50 && SessionService::getInstance().uncheckCountryProcess)
					{
						typedef std::map<DWORD,BYTE>::value_type ValueType;
						std::pair<std::map<DWORD, BYTE>::iterator, bool> retval;
						retval = SessionService::userMap.insert(ValueType(pUser->id, pUser->level));
						if (retval.second) SessionService::getInstance().countryLevel[pUser->country]+=pUser->level;
					}
					CNpcDareM::getMe().sendUserData(pUser);
					CSortM::getMe().onlineCount(pUser);
					CUnionM::getMe().userOnline(pUser); // 当用户RecordServer读档完毕以后再通知上线
					CSchoolM::getMe().userOnline(pUser);
					CSeptM::getMe().userOnline(pUser);
					CQuizM::getMe().userOnline(pUser);
					CCountryM::getMe().userOnline(pUser);
					CArmyM::getMe().userOnline(pUser);
					CGemM::getMe().userOnline(pUser);
					CAllyM::getMe().userOnline(pUser);
					CDareM::getMe().userOnline(pUser);

					WORD degree = CSortM::getMe().getLevelDegree(pUser);
					Cmd::stLevelDegreeDataUserCmd send;
					send.degree = degree;
					pUser->sendCmdToMe(&send, sizeof(send));

					COfflineMessage::getOfflineMessage(pUser);
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_TAXADD_COUNTRY:
			{
				Cmd::Session::t_taxAddCountry_SceneSession *rev=(Cmd::Session::t_taxAddCountry_SceneSession *)cmd;
				CCountry *pCountry = CCountryM::getMe().find(rev->dwCountryID);
				if (pCountry)
				{
					pCountry->addTaxMoney(rev->qwTaxMoney);
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_FRIENDDEGREE_REQUEST:
			{
				Cmd::Session::t_RequestFriendDegree_SceneSession *rev=(Cmd::Session::t_RequestFriendDegree_SceneSession *)cmd;

				for (int i=0; i< rev->size; i++)
				{
					UserSession *pUser=UserSessionManager::getInstance()->getUserSessionByName(rev->namelist[i].name);
					if(pUser)
					{
						pUser->sendFriendDegree(rev);
					}
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_FRIENDDEGREE_COUNT:
			{
				Cmd::Session::t_CountFriendDegree_SceneSession *rev=(Cmd::Session::t_CountFriendDegree_SceneSession *)cmd;
				UserSession *pMainUser=UserSessionManager::getInstance()->getUserSessionByName(rev->name);
				if (pMainUser)
				{
					pMainUser->setFriendDegree(rev);
				}
				CSchoolMember *member = CSchoolM::getMe().getMember(rev->name);
				if (member) member->setFriendDegree(rev);
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_LEVELUPNOTIFY:
			{
				Cmd::Session::t_levelupNotify_SceneSession *rev=(Cmd::Session::t_levelupNotify_SceneSession *)cmd;
				UserSession *pUser=UserSessionManager::getInstance()->getUserByID(rev->dwUserID);
				if (pUser)
				{
					pUser->level = rev->level;
					pUser->qwExp = rev->qwExp;

					CSortM::getMe().upLevel(pUser); //角色排名系统刷新
					WORD degree = CSortM::getMe().getLevelDegree(pUser);
					Cmd::stLevelDegreeDataUserCmd send;
					send.degree = degree;
					pUser->sendCmdToMe(&send, sizeof(send));

					CSchoolM::getMe().setUserLevel(pUser->name, rev->level);
					CartoonPetService::getMe().userLevelUp(pUser->id, rev->level);

					/*
					if (Zebra::global["sn"]=="true")
					{
						//活动，给玩家发送一个序列号
						if (10==pUser->level||20==pUser->level||30==pUser->level)
						{
							struct snInfo
							{
								char sn[16+1];
							}__attribute__ ((packed));
							const dbCol sn_define[] = {
								{ "SN",		zDBConnPool::DB_STR,    sizeof(char[16+1]) },
								{ NULL, 0, 0}
							};

							connHandleID handle = SessionService::dbConnPool->getHandle();
							if ((connHandleID)-1 == handle)
							{
								Zebra::logger->error("[活动]得到数据库句柄失败");
								return true;
							}

							char where[128];
							bzero(where, sizeof(where));
							if (pUser->level==10)
								snprintf(where, sizeof(where) - 1, "SN < '2000000000000000'");
							else
								snprintf(where, sizeof(where) - 1, "SN > '2000000000000000'");

							snInfo snList;
							unsigned int retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`SN`", sn_define, where, 0, 1, (BYTE *)&snList);
							//SessionService::dbConnPool->putHandle(handle);
							if ((DWORD)-1 == retcode)
							{
								Zebra::logger->error("[活动]查询错误: retcode=%d", retcode);
								SessionService::dbConnPool->putHandle(handle);
								return true;
							}

							if (0 == retcode)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_SYS, "对不起，本日发放的10万个抽奖机会已经全部放出，请明日继续努力！", pUser->level);
								SessionService::dbConnPool->putHandle(handle);
								return true;
							}

							char text[1024];
							bzero(text, sizeof(text));
							snprintf(text, sizeof(text) - 1, "亲爱的玩家：\n\t 恭喜你升到%d级，请使用下面的16位序列号到官网参加抽奖活动，加油!\n\t\t%s\n\t\t\t\t\t\t征途活动中心 ", pUser->level, snList.sn);
							if (MailService::getMe().sendTextMail("征途活动中心", pUser->name, text, (DWORD)handle, Cmd::Session::MAIL_TYPE_ACTIVITY))
							{
								std::string escapeSN;
								bzero(where, sizeof(where));
								snprintf(where, sizeof(where) - 1, "SN='%s'", SessionService::dbConnPool->escapeString(handle,snList.sn,escapeSN).c_str());
								retcode = SessionService::dbConnPool->exeDelete(handle, "`SN`", where);
								pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "欢迎参加《征途》百万实物大奖抽奖活动，请到传递员处查收你的信件，并到官网活动页面参加抽奖。祝你好运！");
								Zebra::logger->trace("[活动]玩家 %s 到达 %u 级，获得抽奖号码 %s", pUser->name, pUser->level, snList.sn);
								//SessionService::snCount++;//计数
							}
							SessionService::dbConnPool->putHandle(handle);
						}
					}
					*/
				}
				return true;
			}
			break;
			//队伍信息
		case Cmd::Session::PARA_USER_TEAM_ADDMEMBER:
			{
				Cmd::Session::t_Team_AddMember *rev = (Cmd::Session::t_Team_AddMember *)cmd;
				GlobalTeamIndex::getInstance()->addMember(rev->dwLeaderID , rev->dwMemberID);
				return true;
			}
			break;
		case Cmd::Session::PARA_USER_TEAM_DELMEMBER:
			{
				Cmd::Session::t_Team_DelMember *rev = (Cmd::Session::t_Team_DelMember *)cmd;
				GlobalTeamIndex::getInstance()->delMember(rev->dwLeaderID , rev->dwMemberID);
				return true;
			}
			break;
			//请求读临时档案
		case Cmd::Session::PARA_USER_ARCHIVE_REQ:
			{
				//TODO
				Cmd::Session::t_ReqUser_SceneArchive *rev=(Cmd::Session::t_ReqUser_SceneArchive *)cmd;
				SceneSession *scene= SceneSessionManager::getInstance()->getSceneByTempID(rev->dwMapTempID);
				if(!scene)
				{
					return true;
				}
				char buf[sizeof(Cmd::Session::t_ReadUser_SceneArchive) + MAX_TEMPARCHIVE_SIZE];
				if(buf)
				{
					Cmd::Session::t_ReadUser_SceneArchive *ret = (Cmd::Session::t_ReadUser_SceneArchive *)buf;
					constructInPlace(ret);
					ret->id = rev->id;
					ret->dwMapTempID = rev->dwMapTempID;
					ret->dwSize = MAX_TEMPARCHIVE_SIZE;
					if(GlobalTempArchiveIndex::getInstance()->readTempArchive(ret->id , ret->data , ret->dwSize))
					{
						scene->sendCmd(ret , sizeof(Cmd::Session::t_ReadUser_SceneArchive) + ret->dwSize);
						Zebra::logger->trace("发送临时读档数据包%u" , sizeof(Cmd::Session::t_ReadUser_SceneArchive) + ret->dwSize);
					}
				}
				return true;
			}
			break;
			//请求写临时档案
		case Cmd::Session::PARA_USER_ARCHIVE_WRITE:
			{
				//TODO
				Cmd::Session::t_WriteUser_SceneArchive *rev=(Cmd::Session::t_WriteUser_SceneArchive *)cmd;
				//TempArchiveMember *temp = (TempArchiveMember *)(rev->data + rev->dwSize);
				//Zebra::logger->debug("temp->dwSize=%u" , temp->size);
				//Zebra::logger->debug("rev->dwSize=%u" , rev->dwSize);
				//Zebra::logger->debug("收到临时存档数据包%u" , cmdLen);
				//UserSession *pUser=UserSessionManager::getInstance()->getUserByID(rev->id);
				//if (pUser)
				//{
				GlobalTempArchiveIndex::getInstance()->writeTempArchive(rev->id , rev->data , rev->dwSize);
				//}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_CHANEG_SCENE:
			{
				Cmd::Session::t_changeScene_SceneSession *rev=(Cmd::Session::t_changeScene_SceneSession *)cmd;
				SceneSession* scene = NULL;
				if ((char)rev->map_file[0]) {
					scene = SceneSessionManager::getInstance()->getSceneByFile((char*) rev->map_file);
				}else if(rev->map_id){
					scene = SceneSessionManager::getInstance()->getSceneByID(rev->map_id);
					//Zebra::logger->debug("地图id=%u" , rev->map_id);
				}else{
					scene = SceneSessionManager::getInstance()->getSceneByName((char*) rev->map_name);
					//Zebra::logger->debug("地图名称%s" , rev->map_name);
				}
				if (!scene) return true;

				if (scene->level>0)
				{
					UserSession *pUser = UserSessionManager::getInstance()->getUserByID(rev->id);
					if (pUser)
					{
						if (pUser->level < scene->level)
						{
							pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "%s不对等级低于%d级的玩家开放！", scene->name, scene->level);
							Zebra::logger->trace("[GOMAP]玩家%s等级不够跳到地图[%s]失败!" , pUser->name, scene->name);
							return true;
						}
					}
					//else return true;
				}


				//Zebra::logger->debug("场景信息%s , %u, %u" , scene->name , scene->id , scene->tempid);
				//Zebra::logger->debug("场景信息%s , %s , %u" , rev->map_file , rev->map_name , rev->map_id);
				Cmd::Session::t_changeScene_SceneSession ret;
				ret.id = rev->id;
				ret.x = rev->x;
				ret.y = rev->y;
				ret.map_id = rev->map_id;

				if (scene) 
                {
					ret.temp_id = scene->tempid;
					strncpy((char *)ret.map_file, scene->file.c_str(), MAX_NAMESIZE);
					strncpy((char *)ret.map_name, scene->name, MAX_NAMESIZE);
				}
                else 
                {
					ret.temp_id = (DWORD)-1;
				}
				sendCmd(&ret, sizeof(ret));
				return true;
				/*
				   Cmd::Session::t_changeScene_SceneSession *rev=(Cmd::Session::t_changeScene_SceneSession *)cmd;
				   SceneSession* scene = SceneSessionManager::getInstance()->getSceneByFile((char*) rev->map_file);
				   if (0==scene)
				   scene = SceneSessionManager::getInstance()->getSceneByName((char*) rev->map_name);

				   Cmd::Session::t_changeScene_SceneSession ret;
				   ret.id = rev->id;
				   ret.x = rev->x;
				   ret.y = rev->y;
				   if (scene) {
				   strncpy((char *)ret.map_file, scene->file.c_str(), MAX_NAMESIZE);
				   ret.temp_id = scene->tempid;
				   strncpy((char *)ret.map_name, scene->name, MAX_NAMESIZE);
				   }else {
				   ret.temp_id = (DWORD)-1;
				   }
				   sendCmd(&ret, sizeof(ret));
				   return true;
				 */
			}
			break;
		case Cmd::Session::PARA_SCENE_GM_COMMAND:
			{
				Cmd::Session::t_gmCommand_SceneSession * rev = (Cmd::Session::t_gmCommand_SceneSession *)cmd;

				switch (rev->gm_cmd)
				{
						case Cmd::Session::GM_COMMAND_LOAD_GIFT:
								return Gift::getMe().init();
								break;
						case Cmd::Session::GM_COMMAND_LOAD_PROCESS:
                        case Cmd::Session::GM_COMMAND_LOADMESSAGE:  //soke 刷新Message
						case Cmd::Session::GM_COMMAND_LOADQUEST:    //soke 刷新任务
					    case Cmd::Session::GM_COMMAND_LOADGIFTBOX:  //soke 刷新宝盒
						case Cmd::Session::GM_COMMAND_LOADNEWQUEST: //soke 刷新lua
								return SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
                                break;
						case Cmd::Session::GM_COMMAND_LOADNPCTRADE://soke 刷新npctrade
						case Cmd::Session::GM_COMMAND_LOADTBL:     //soke 刷新tbl
								return SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
                                break;
						/////////////////////////////////////////////////////		
                        case Cmd::Session::GM_COMMAND_LOAD_GAME_CONFIG:
                            {//soke 刷新温泉脚本
                                GameConfigMgrX::getMe().init();
                                return SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
                            }
                            break;
						//////////////////////////////////////////////////////							
						///////////////////////开始//////////////////////////////		
                        /*
						case Cmd::Session::GM_COMMAND_LOAD_FUBEN_CONFIG:
                            {//soke 刷新副本脚本
                                FuBenConfigMgrX::getMe().init();
                                return SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
                            }
                            break;
						*/
						//////////////////////结束////////////////////////////////							
                        case Cmd::Session::GM_COMMAND_NEWZONE:
                            return SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
                            break;
                        case Cmd::Session::GM_COMMAND_REFRESH_GENERAL:
                            CCountryM::getMe().refreshGeneral(0);
                            return true;
                        default:
                            break;
                }

				UserSession * pUser = UserSessionManager::getInstance()->getUserSessionByName((char *)rev->dst_name);
				if (0==pUser)
				{
					Cmd::Session::t_gmCommand_SceneSession ret;
					ret.gm_cmd = rev->gm_cmd;
					strncpy((char *)ret.dst_name, (char *)rev->src_name, MAX_NAMESIZE);
					strncpy((char *)ret.src_name, (char *)rev->dst_name, MAX_NAMESIZE);
					ret.err_code = Cmd::Session::GM_COMMAND_ERR_NOUSER;

					ret.cmd_state = Cmd::Session::GM_COMMAND_STATE_RET;
					sendCmd(&ret, sizeof(ret));
					return true;
				}

				pUser->scene->sendCmd(rev, sizeof(Cmd::Session::t_gmCommand_SceneSession));
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_CREATE_SCHOOL:
			{
				Cmd::Session::t_createSchool_SceneSession * rev = (Cmd::Session::t_createSchool_SceneSession *)cmd;

				UserSession * pUser = UserSessionManager::getInstance()->getUserSessionByName((char *)rev->masterName);
				if (!pUser) return false;
				if (CSchoolM::getMe().createNewSchool(pUser->name, (char *)rev->schoolName))
				{
					Cmd::Session::t_SchoolCreateSuccess_SceneSession send;
					send.dwID = pUser->id;
					send.dwSchoolID = pUser->schoolid;
					strncpy(send.schoolName, rev->schoolName, MAX_NAMESIZE);
					pUser->scene->sendCmd(&send, sizeof(send));
				}
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "遗憾的通知你门派名称已被占用，请换个名字再试试!");
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_PRIVATE_CHAT:
			{
				Cmd::Session::t_privateChat_SceneSession * rev = (Cmd::Session::t_privateChat_SceneSession *)cmd;
				UserSession * pUser = UserSessionManager::getInstance()->getUserSessionByName((char *)rev->dst_name);
				if (0==pUser)
				{
					if (strstr((char *)rev->dst_name, "GM")
							|| strstr((char *)rev->dst_name, "gm"))  return true;//发给GM的私聊不返回

					Cmd::Session::t_privateChat_SceneSession ret;
					ret.err_code = Cmd::Session::PRIVATE_CHAT_ERR_NOUSER;
					strncpy((char *)ret.src_name, (char *)rev->dst_name, MAX_NAMESIZE);
					strncpy((char *)ret.dst_name, (char *)rev->src_name, MAX_NAMESIZE);
					//bcopy(rev->chat_cmd, ret.chat_cmd, rev->cmd_size);
					//sendCmd(&ret, sizeof(Cmd::Session::t_privateChat_SceneSession)+rev->cmd_size);
					sendCmd(&ret, sizeof(ret));
					return true;
				}

				pUser->scene->sendCmd(rev, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_SYS_SETTING:
			{
				Cmd::Session::t_sysSetting_SceneSession * rev = (Cmd::Session::t_sysSetting_SceneSession *)cmd;
				UserSession * pUser = UserSessionManager::getInstance()->getUserSessionByName((char *)rev->name);
				if (pUser)
				{
					bcopy(rev->sysSetting, pUser->sysSetting, sizeof(pUser->sysSetting));
					pUser->face = rev->face;
					return true;
				}
			}
			break;
		case Cmd::Session::PARA_SCENE_CITY_RUSH:
			{
				//Zebra::logger->debug("收到rush公告");
				Cmd::Session::t_cityRush_SceneSession * rev = (Cmd::Session::t_cityRush_SceneSession *)cmd;
				char content[MAX_CHATINFO];
				sprintf(content, "BOSS %s 被杀死了,他的灵魂得到了魔神的救赎，将在 %d 分钟后对 %s 发起 %s", rev->bossName, rev->delay/60, rev->mapName, rev->rushName);
				SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, rev->countryID, content);
				/*
				   broadcastRushToEveryUser b;
				   if (b.init(content))
				   UserSessionManager::getInstance()->execEveryUser(b);
				 */
				return true;
			}
			break;
		// case Cmd::Session::PARA_SCENE_CITY_RUSH_CUST: //桃子 这里好像是怪物攻城notify的提示地址
		// 	{
		// 		Cmd::Session::t_cityRushCust_SceneSession * rev = (Cmd::Session::t_cityRushCust_SceneSession *)cmd;
		// if (strncmp("  ", rev->text, 128))
		// 		SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME6, rev->countryID, rev->text); //桃子 这里好像是怪物攻城notify的提示地址
		// 		return true;
		// 	}
		// 	break;
		case Cmd::Session::PARA_SCENE_REMOVE_SCENE:
			{
				Cmd::Session::t_removeScene_SceneSession *rev = (Cmd::Session::t_removeScene_SceneSession*)cmd;
				SceneSession *scene= SceneSessionManager::getInstance()->getSceneByID(rev->map_id);
				if(scene)
				{
					SceneSessionManager::getInstance()->removeScene(scene);
					Zebra::logger->info("卸载地图%u(%s) 成功",scene->id , scene->name);
					SAFE_DELETE(scene);
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_REQ_ADD_SCENE:
			{
				Cmd::Session::t_reqAddScene_SceneSession *rev = (Cmd::Session::t_reqAddScene_SceneSession*)cmd;
				Zebra::logger->info("转发加载地图消息(%u,%u,%u)",rev->dwServerID ,rev->dwCountryID , rev->dwMapID);
				SessionTaskManager::getInstance().broadcastByID(rev->dwServerID , 
						rev , sizeof(Cmd::Session::t_reqAddScene_SceneSession));
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_UNLOAD_SCENE:
			{
				Cmd::Session::t_unloadScene_SceneSession *rev = (Cmd::Session::t_unloadScene_SceneSession*)cmd;
				SceneSession *scene= SceneSessionManager::getInstance()->getSceneByID(rev->map_id);
				if(scene)
				{
					//TODO
					//设置不可注册标志
					scene->setRunState(SCENE_RUN_STATE_UNLOAD);
					scene->sendCmd(rev,sizeof(Cmd::Session::t_unloadScene_SceneSession));
					Zebra::logger->info("地图%s因卸载地图停止注册",scene->name);
					/*
					   SceneSessionManager::getInstance()->removeScene(scene);
					   struct UnloadSceneSessionExec :public execEntry<UserSession>
					   {
					   SceneSession *scene;
					   std::vector<DWORD> del_vec;
					   UnloadSceneSessionExec(SceneSession *s):scene(s)
					   {
					   }
					   bool exec(UserSession *u)
					   {
					   if(u->scene->id == scene->id)
					   {
					   del_vec.push_back(u->id);
					   }
					   return true;
					   }
					   };
					   UnloadSceneSessionExec exec(scene);
					   UserSessionManager::getInstance()->execEveryUser(exec);
					   for(std::vector<DWORD>::iterator iter = exec.del_vec.begin() ; iter != exec.del_vec.end() ; iter ++)
					   {
					   UserSession *pUser=UserSessionManager::getInstance()->getUserByID(*iter);
					   if(pUser)
					   {
					   Zebra::logger->trace("用户%s(%ld)因卸载地图注销",pUser->name,pUser->id);
					   UserSessionManager::getInstance() ->removeUser(pUser);
					   SAFE_DELETE(pUser);
					   }
					   }
					   Zebra::logger->debug("卸载地图%u(%s) 成功",scene->id , scene->name);
					   SAFE_DELETE(scene);
					// */
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SCENE_GUARD_FAIL:
			{
				Cmd::Session::t_guardFail_SceneSession * rev = (Cmd::Session::t_guardFail_SceneSession *)cmd;
				UserSession *pUser=UserSessionManager::getInstance()->getUserByTempID(rev->userID);
				if (pUser)
					pUser->scene->sendCmd(rev, cmdLen);
				return true;
			}
			break;
			/*
		case Cmd::Session::PARA_SCENE_LOAD_PROCESS:
			{
				SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
				return true;
			}
			break;
			*/
		case Cmd::Session::PARA_HEOR_KILL_UESR:
			{
				//护国榜
				//Cmd::Session::t_heorKill_SceneSession * rev = (Cmd::Session::t_heorKill_SceneSession *)cmd;
				//Zebra::logger->debug("名称:%s,ID:%d,国家:%d,击杀ID:%d",rev->name,rev->charID,rev->country,rev->kCharID);
				CHero::getMe().addHeroKill(cmd, cmdLen);
				return true;
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, cmd->cmd, cmd->para, cmdLen);
	return false;
}

bool SessionTask::msgParse_Gate(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	switch(cmd->para)
	{
		case Cmd::Session::PARA_GATE_REGUSER:
			{
				Cmd::Session::t_regUser_GateSession *reg=(Cmd::Session::t_regUser_GateSession *)cmd;
				SceneSession *scene= SceneSessionManager::getInstance()->getSceneByName((char *)reg->byMapName);

				if(!scene||(reg->wdLevel<scene->level))
				{       
					char map[MAX_NAMESIZE+1];
					bzero(map,sizeof(map));
					bcopy(reg->byMapName,map,6);
					bcopy("清源村",&map[6],6);
					scene=SceneSessionManager::getInstance()->getSceneByName(map);
				} 
				if(scene)
				{
					if(scene->getRunState() == SCENE_RUN_STATE_NORMAL)
					{
						UserSession *pUser=UserSessionManager::getInstance()->getUserByID(reg->dwID);
						if(!pUser)
						{
							pUser=new UserSession(this);
							if(pUser && pUser->reg(reg))
							{
								//Zebra::logger->debug("创建用户Session成功");
								pUser->scene=scene;

								if(UserSessionManager::getInstance()->addUser(pUser))
								{
									//场景读档案
									Cmd::Session::t_regUser_SceneSession reginscene;
									reginscene.accid=reg->accid;
									reginscene.dwID=reg->dwID;
									reginscene.dwTempID=reg->dwTempID;
									reginscene.dwMapID=reg->dwMapID;
									bcopy(reg->byName,reginscene.byName,MAX_NAMESIZE+1);
									bcopy(reg->byMapName,reginscene.byMapName,MAX_NAMESIZE+1);
									reginscene.dwGatewayServerID=pUser->getTask()->getID();
									scene->sendCmd(&reginscene,sizeof(reginscene));

									Zebra::logger->trace("用户%s(%ld)注册成功",pUser->name,pUser->id);

									CartoonPetService::getMe().userOnline(pUser);
									return true;
								}
								else
								{
									UserSession *pUser=UserSessionManager::getInstance()->getUserByID(reg->dwID);
									if(pUser)
									{
										Zebra::logger->debug("角色id重复(id=%u,name=%s,tempid=%u" 
												, pUser->id ,pUser->name ,pUser->tempid);
									}
									pUser=UserSessionManager::getInstance()->getUserByTempID(reg->dwTempID);
									if(pUser)
									{
										Zebra::logger->debug("角色tempid重复(id=%u,name=%s,tempid=%u" 
												, pUser->id ,pUser->name ,pUser->tempid);
									}
									Zebra::logger->error("添加角色失败，可能是角色重复登陆");
								}
								SAFE_DELETE(pUser);
							}
							else
								Zebra::logger->fatal("注册用户时候分配内存失败(%u, %u, %x)",reg->accid,reg->dwID,reg->byName);
						}
						else
						{
							CSortM::getMe().offlineCount(pUser);
							CUnionM::getMe().userOffline(pUser); // 用于处理帮会成员下线
							CSchoolM::getMe().userOffline(pUser);
							CSeptM::getMe().userOffline(pUser);
							CQuizM::getMe().userOffline(pUser);
							CGemM::getMe().userOffline(pUser);
							UserSessionManager::getInstance()->removeUser(pUser);

							//通知网关错误注销
							Cmd::Session::t_unregUser_GateSession ret;
							ret.dwUserID=reg->dwID;
							ret.dwSceneTempID=scene->tempid;
							ret.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
							sendCmd(&ret,sizeof(ret));

							//通知场景错误注销
							Cmd::Session::t_unregUser_SceneSession send;
							send.dwUserID=reg->dwID;
							send.dwSceneTempID=scene->tempid;
							send.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
							scene->sendCmd(&send,sizeof(send));
							Zebra::logger->trace("发现重复用户注销%s(%ld)注销",pUser->name,pUser->id);
							SAFE_DELETE(pUser);
							return true;
						}
					}
					else
					{
						Zebra::logger->trace("场景%s现在不可以注册用户",(char *)reg->byMapName);
					}
				}
				else
					Zebra::logger->error("未找到角色所在地图 %s",(char *)reg->byMapName);
				//通知网关注册失败
				Zebra::logger->error("用户(%lu,%lu,%s,%lu)注册失败",reg->accid,reg->dwID,reg->byName,reg->dwTempID);
				Cmd::Session::t_unregUser_GateSession ret;
				ret.dwUserID=reg->dwID;
				if(scene)
					ret.dwSceneTempID=scene->tempid;
				else
					ret.dwSceneTempID=0;
				ret.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
				sendCmd(&ret,sizeof(ret));
				return true;
			}
			break;
			//请求国家在线排序
		case Cmd::Session::REQUEST_GATE_COUNTRY_ORDER:
			{
				char Buf[200];
				bzero(Buf , sizeof(Buf));
				Cmd::Session::t_order_Country_GateSession *ret_gate = 
					(Cmd::Session::t_order_Country_GateSession*)Buf;
				constructInPlace(ret_gate);
				ret_gate->order.size = UserSession::country_map.size();
				for(std::map<DWORD  , DWORD>::iterator iter = 
						UserSession::country_map.begin() ; iter != UserSession::country_map.end() ;iter ++)
				{
					DWORD temp = iter->second;
					DWORD cn = iter->first;
					for(int i=ret_gate->order.size -1 ; i>=0; i--)
					{
						if(ret_gate->order.order[i].count <= temp)
						{
							DWORD temp_1 = ret_gate->order.order[i].count;
							DWORD cn_1 = ret_gate->order.order[i].country;
							ret_gate->order.order[i].count = temp;
							ret_gate->order.order[i].country = cn; 
							temp = temp_1;
							cn = cn_1;
						}
					}
				}
				for(int i = 0 ; i < (int)ret_gate->order.size ; i ++)
				{
					Zebra::logger->debug("国家:%d,在线人数:%d",ret_gate->order.order[i].country , ret_gate->order.order[i].count);
				}
				sendCmd(ret_gate , sizeof(Cmd::Session::t_order_Country_GateSession) 
						+ sizeof(ret_gate->order.order[0]) * ret_gate->order.size); 
				return true;
			}
			break;
		case Cmd::Session::PARA_GATE_CHANGE_SCENE_USER:
			{
				Cmd::Session::t_changeUser_GateSession *reg=(Cmd::Session::t_changeUser_GateSession *)cmd;
				//SceneSession *scene= SceneSessionManager::getInstance()->getSceneByFile((char *)reg->byMapFileName);

                //soke 这里是显示太庙资源数量
				SceneSession *scene = NULL;
                if ( reg->scnTempID )
                {
                    scene = SceneSessionManager::getInstance()->getSceneByTempID( reg->scnTempID );
                }
                else if ( reg->byMapFileName )
                {
				    scene= SceneSessionManager::getInstance()->getSceneByFile((char *)reg->byMapFileName);
                }

				if(scene)  
                {
					UserSession *pUser=UserSessionManager::getInstance()->getUserByID(reg->dwID);
					if(pUser) 	
                    {
						//Zebra::logger->debug("创建用户Session成功");
						pUser->scene=scene;

						//场景读档案
						Cmd::Session::t_regUser_SceneSession reginscene;
						reginscene.accid=reg->accid;
						reginscene.dwID=reg->dwID;
						reginscene.dwTempID=reg->dwTempID;
						reginscene.dwMapID=scene->id;
						bcopy(reg->byName,reginscene.byName,MAX_NAMESIZE+1);
						bcopy(scene->name,reginscene.byMapName,MAX_NAMESIZE+1);
						reginscene.dwGatewayServerID=pUser->getTask()->getID();
						scene->sendCmd(&reginscene,sizeof(reginscene));

						Zebra::logger->trace("用户%s(%ld)切换场景成功",pUser->name,pUser->id);
						return true;
					}
                    else 
                    {
						Zebra::logger->error("未找到用户%s(%ld)", (char*) reg->byName, reg->dwID);
					}
				}
				else
                {
					Zebra::logger->error("未找到角色所在地图 %s",(char *)reg->byMapFileName);
                }
				//通知网关注册失败
				Zebra::logger->error("用户(%lu,%lu,%s,%lu)注册失败",reg->accid,reg->dwID,reg->byName,reg->dwTempID);
				Cmd::Session::t_unregUser_GateSession ret;
				ret.dwUserID=reg->dwID;
				if(scene)
					ret.dwSceneTempID=scene->tempid;
				else
					ret.dwSceneTempID=0;
				ret.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
				sendCmd(&ret,sizeof(ret));
				return true;
			}
			break;

		case Cmd::Session::PARA_GATE_UNREGUSER:
			{
				Cmd::Session::t_unregUser_GateSession *reg=(Cmd::Session::t_unregUser_GateSession *)cmd;
				UserSession *pUser=UserSessionManager::getInstance()->getUserByID(reg->dwUserID);
				SceneSession *scene=SceneSessionManager::getInstance()->getSceneByTempID(reg->dwSceneTempID);

				if(pUser)
				{
					CSortM::getMe().offlineCount(pUser);
					CUnionM::getMe().userOffline(pUser); // 用于处理帮会成员下线
					CSchoolM::getMe().userOffline(pUser);
					CSeptM::getMe().userOffline(pUser);
					CQuizM::getMe().userOffline(pUser);
					CGemM::getMe().userOffline(pUser);
					UserSessionManager::getInstance()->removeUser(pUser);

					if(reg->retcode==Cmd::Session::UNREGUSER_RET_ERROR)
					{
						Zebra::logger->error("用户%s(%ld)因网关错误注销",pUser->name,pUser->id);
						SAFE_DELETE(pUser);
						return true;
					}
					else if(reg->retcode==Cmd::Session::UNREGUSER_RET_LOGOUT)
					{
						if(scene)
						{
							Cmd::Session::t_unregUser_SceneSession send;
							send.dwUserID=reg->dwUserID;
							send.dwSceneTempID=reg->dwSceneTempID;
							send.retcode=Cmd::Session::UNREGUSER_RET_LOGOUT;
							scene->sendCmd(&send,sizeof(send));
							Zebra::logger->trace("网关请求用户%s(%ld)注销",pUser->name,pUser->id);
							SAFE_DELETE(pUser);
							return true;
						}
						else
						{
							Cmd::Session::t_unregUser_SceneSession send;
							send.dwUserID=reg->dwUserID;
							send.dwSceneTempID=reg->dwSceneTempID;
							send.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
							SessionTaskManager::getInstance().broadcastScene(&send,sizeof(send));
							Zebra::logger->error("用户%s注销时发生错误,发送广播消息注销场景用户",pUser->name);
						}
					}
					SAFE_DELETE(pUser);
				}
				else
					Zebra::logger->error("注销时未找到用户%ld",reg->dwUserID);

				// 更改流程后注销失败无需通知网关
				/*
				   if(reg->retcode==Cmd::Session::UNREGUSER_RET_LOGOUT)
				   {
				   Cmd::Session::t_unregUser_GateSession ret;
				   ret.dwUserID=reg->dwUserID;
				   ret.dwSceneTempID=reg->dwSceneTempID;
				   ret.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
				   sendCmd(&ret,sizeof(ret));
				   }
				// */
				return true;
			}
			break;
		case Cmd::Session::PARA_UNION_DISBAND:
			{
				CUnionM::getMe().processGateMessage(cmd,cmdLen);
				return true;
			}
			break;
		case Cmd::Session::PARA_SEPT_DISBAND:
			{
				CSeptM::getMe().processGateMessage(cmd,cmdLen);
				return true;
			}
			break;
		case Cmd::Session::PARA_GATE_DELCHAR:
			{
				this->del_role(cmd ,cmdLen);
				return true;

			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, cmd->cmd, cmd->para, cmdLen);
	return false;
}

bool SessionTask::msgParse_Forward(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
    if(ptNullCmd->cmd==Cmd::Session::CMD_FORWARD && ptNullCmd->para==Cmd::Session::PARA_FORWARD_USER)
    {
        Cmd::Session::t_Session_ForwardUser *rev=(Cmd::Session::t_Session_ForwardUser *)ptNullCmd;
        UserSession *pUser=UserSessionManager::getInstance()->getUserByID(rev->dwID);
        Cmd::stNullUserCmd * cmd = (Cmd::stNullUserCmd *)rev->data;
        if(pUser)
        {
            switch(cmd->byCmd)
            {
            case Cmd::GIFT_USERCMD:
                {
                    return Gift::getMe().doGiftCmd(pUser, (Cmd::stNullUserCmd*)rev->data, rev->size);
                }
                break;
            case Cmd::NPCDARE_USERCMD:
                {
                    return CNpcDareM::getMe().processUserMessage(pUser, (Cmd::stNullUserCmd*)rev->data, 
                        rev->size);
                }
                break;
            case Cmd::QUIZ_USERCMD:
                {
                    return CQuizM::getMe().processUserMessage(pUser, (Cmd::stNullUserCmd*)rev->data, 
                        rev->size);
                }
                break;
            case Cmd::DARE_USERCMD:
                {
                    return CDareM::getMe().processUserMessage(pUser,(Cmd::stNullUserCmd *)rev->data, rev->size);
                }
                break;
            case Cmd::SCHOOL_USERCMD:
                {
                    return CSchoolM::getMe().processUserMessage(pUser,(Cmd::stNullUserCmd *)rev->data, rev->size);
                }
                break;
            case Cmd::UNION_USERCMD:
                {
                    return CUnionM::getMe().processUserMessage(pUser,(Cmd::stNullUserCmd *)rev->data,rev->size);
                }
                break;
            case Cmd::RELATION_USERCMD:
                {
                    return pUser->relationManager.processUserMessage((Cmd::stNullUserCmd *)rev->data,rev->size);
                }
                break;
            case Cmd::SEPT_USERCMD:
                {
                    return CSeptM::getMe().processUserMessage(pUser,(Cmd::stNullUserCmd *)rev->data,rev->size);
                }
                break;
            case Cmd::COUNTRY_USERCMD:
                {
                    return CCountryM::getMe().processUserMessage(pUser, (Cmd::stNullUserCmd*)rev->data, rev->size);
                }
                break;
            case Cmd::ARMY_USERCMD:
                {
                    return CArmyM::getMe().processUserMessage(pUser, (Cmd::stNullUserCmd*)rev->data, rev->size);
                }
                break;
            case Cmd::RECOMMEND_USERCMD:
                {
                    return RecommendM::getMe().processUserMessage(pUser, (Cmd::stNullUserCmd*)rev->data, rev->size);
                }
            case Cmd::ALLY_USERCMD:
                {
                    return CAllyM::getMe().processUserMessage(pUser, (Cmd::stNullUserCmd*)rev->data, rev->size);
                }
                break;
            case Cmd::GEM_USERCMD:
                {
                    return CGemM::getMe().processUserMessage(pUser, (Cmd::stNullUserCmd*)rev->data, rev->size);
                }
                break;
            case Cmd::VOTE_USERCMD:
                {
                    return CVoteM::getMe().processUserMessage(pUser, (Cmd::stNullUserCmd*)rev->data, rev->size);
                }
                break;
            case Cmd::DATA_USERCMD:
                {
                    WORD degree = CSortM::getMe().getLevelDegree(pUser);
                    Cmd::stLevelDegreeDataUserCmd send;
                    send.degree = degree;
                    pUser->sendCmdToMe(&send, sizeof(send));
                    return true;
                }
                break;
            case Cmd::CHAT_USERCMD:
                {
                    switch (cmd->byParam)
                    {
                    case REQUEST_COUNTRY_HELP_USERCMD_PARA:
                    case KILL_FOREIGNER_USERCMD_PARA:
                    case REFRESH_BOSS_USERCMD_PARA:
                    case KILL_BOSS_USERCMD_PARA:
                        {
                            SessionTaskManager::getInstance().sendCmdToCountry(pUser->country,cmd,cmdLen);
                        }
                        break;
                    case QUESTION_OBJECT_USERCMD_PARA:
                        {
                            Cmd::stQuestionObject* questionCmd = (Cmd::stQuestionObject*)cmd;
#ifdef _ZJW_DEBUG
                            Zebra::logger->debug("收到物品查询命令");
#endif							
                            if (questionCmd)
                            {
                                UserSession* pFromUser = UserSessionManager::getInstance()->
                                    getUserSessionByName(questionCmd->name);
                                Cmd::Session::t_questionObject_SceneSession send;


                                if (pFromUser && pFromUser->scene && pUser)
                                {
                                    strncpy(send.from_name, questionCmd->name, MAX_NAMESIZE);
                                    strncpy(send.to_name, pUser->name, MAX_NAMESIZE);

                                    send.dwObjectTempID = questionCmd->dwObjectTempID;

                                    pFromUser->scene->sendCmd(&send,sizeof(Cmd::Session::t_questionObject_SceneSession));
                                }
                                else
                                {
                                    if (pUser)
                                    {
                                        pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
                                            "对方已不在线");
                                    }
                                }
                            }

                            return true;
                        }
                        break;
                    case CREATE_CHANNEL_USERCMD_PARAMETER:
                        {
                            Cmd::stCreateChannelUserCmd *create=(Cmd::stCreateChannelUserCmd *)cmd;
                            SessionChannel * sc = new SessionChannel(pUser);
                            if (!sc) return false;
                            if (!SessionChannelManager::getMe().add(sc))
                            {
                                pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,"你只能创建一个频道");
                                SAFE_DELETE(sc);
                                return true;
                            }
                            Cmd::stCreateChannelUserCmd ret;
                            ret.dwChannelID=sc->tempid;
                            ret.dwClientID=create->dwClientID;
                            strncpy(ret.name, create->name,MAX_NAMESIZE);
                            pUser->sendCmdToMe(&ret,sizeof(ret));
                            sc->add(pUser);

                            UserSession * us1 = UserSessionManager::getInstance()->getUserSessionByName(create->name);
                            sc->add(us1);
                            UserSession * us = UserSessionManager::getInstance()->getUserSessionByName(create->name2);
                            if (us)
                            {
                                Cmd::stInvite_ChannelUserCmd inv;
                                inv.dwChannelID=sc->tempid;
                                inv.dwCharType = pUser->face;
                                strncpy(inv.name, pUser->name, MAX_NAMESIZE);
                                us->sendCmdToMe(&inv, sizeof(inv));
                            }
                            return true;
                        }
                        break;
                    case INVITE_CHANNEL_USERCMD_PARAMETER:
                        {
                            Cmd::stInvite_ChannelUserCmd *invite=(Cmd::stInvite_ChannelUserCmd *)cmd;
                            //SceneUser *pUser=SceneUserManager::getMe().getUserByName(invite->name);
                            UserSession * us = UserSessionManager::getInstance()->getUserSessionByName(invite->name);
                            if (us)
                            {
                                SessionChannel *cl=SessionChannelManager::getMe().get(invite->dwChannelID);
                                if(cl)
                                {
                                    if(strncmp(pUser->name, cl->name, MAX_NAMESIZE)!=0)
                                    {
                                        pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不能邀请用户");
                                        return true;
                                    }
                                    if(cl->has(us->tempid))
                                    {
                                        pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方已经在频道里了");
                                        return true;
                                    }
                                    if (cl->count()>=20)
                                    {
                                        pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "频道人数已满");
                                        return true;
                                    }
                                }
                                else
                                {
                                    pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "聊天频道不存在");
                                    return true;
                                }

                                Cmd::stInvite_ChannelUserCmd inv;
                                inv.dwChannelID=invite->dwChannelID;
                                inv.dwCharType = pUser->face;
                                strncpy(inv.name, pUser->name, MAX_NAMESIZE);
                                us->sendCmdToMe(&inv, sizeof(inv));
                            }
                            else
                                pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家 %s 不在线", invite->name);
                            return true;
                        }
                        break;
                    case JOIN_CHANNEL_USERCMD_PARAMETER:
                        {
                            Cmd::stJoin_ChannelUserCmd *join=(Cmd::stJoin_ChannelUserCmd *)cmd;
                            //UserSession *pHost=UserSessionManager::getMe().getUserSessionByName(join->host_name);
                            //if (pHost)
                            {
                                SessionChannel *cl = SessionChannelManager::getMe().get(join->dwChannelID);
                                if(cl)
                                {       
                                    cl->add(pUser);
                                }
                                else    
                                    pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不存在此聊天频道");
                            }
                            /*
                            else
                            {       
                            Cmd::Session::t_privateChat_SceneSession cmd;
                            bzero(cmd.src_name,MAX_NAMESIZE);
                            bzero(cmd.dst_name,MAX_NAMESIZE);
                            cmd.act = Cmd::Session::PRIVATE_CHAT_ACT_JOIN;
                            cmd.err_code = 0;
                            cmd.cmd_size = sizeof(stJoin_ChannelUserCmd);
                            strncpy((char *)&cmd.src_name, name, MAX_NAMESIZE);
                            strncpy((char *)&cmd.dst_name, join->host_name, MAX_NAMESIZE);
                            bcopy(rev, cmd.chat_cmd, sizeof(stJoin_ChannelUserCmd));
                            return sessionClient->sendCmd(&cmd, sizeof(Cmd::Session::t_privateChat_SceneSession)+sizeof(stJoin_ChannelUserCmd));                
                            }
                            */
                        }
                        break;
                    case LEAVE_CHANNEL_USERCMD_PARAMETER:
                        {
                            Cmd::stLeave_ChannelUserCmd *leave=(Cmd::stLeave_ChannelUserCmd *)cmd;
                            //SceneUser *pHost=SceneUserManager::getMe().getUserByName(leave->host_name);
                            //if (pHost)
                            {       
                                SessionChannel *cl=SessionChannelManager::getMe().get(leave->dwChannelID);
                                if(cl)
                                {       
                                    if(!cl->remove(pUser->tempid))
                                    {       
                                        SessionChannelManager::getMe().remove(cl->tempid);
                                    }
                                }
                                else    
                                    pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不存在此聊天频道");
                            }
                        }
                        break;
                    default:
                        break;
                    }

                    Cmd::stChannelChatUserCmd * chatCmd = (Cmd::stChannelChatUserCmd *)cmd;
                    switch(chatCmd->dwType)
                    {
                    case Cmd::CHAT_TYPE_FRIEND_AFFICHE:
                    case Cmd::CHAT_TYPE_FRIEND:				/// 好友频道
                        {
                            pUser->relationManager.sendChatToMyFriend(chatCmd, rev->size);
                            return true;
                        }
                        break;
                    case Cmd::CHAT_TYPE_FRIEND_PRIVATE:			/// 好友私聊
                        {
                            pUser->relationManager.sendPrivateChatToFriend(chatCmd, rev->size);
                            return true;
                        }
                        break;
                    case Cmd::CHAT_TYPE_UNION_AFFICHE:		/// 帮会公告
                    case Cmd::CHAT_TYPE_UNION:				/// 帮会频道
                        {
                            CUnionM::getMe().sendUnionChatMessages(pUser, chatCmd, rev->size);
                            return true;
                        }
                        break;
                    case Cmd::CHAT_TYPE_UNION_PRIVATE:			/// 帮会私聊
                        {
                            CUnionM::getMe().sendUnionPrivateChatMessages(pUser, chatCmd, rev->size);
                            return true;
                        }
                        break;
                    case Cmd::CHAT_TYPE_COUNTRY_MARRY:
							case Cmd::CHAT_TYPE_COUNTRY_PK:
							case Cmd::CHAT_TYPE_COUNTRY:			/// 国家频道
								{
									UserSession *pUser = UserSessionManager::getInstance()->getUserSessionByName(chatCmd->pstrName);

                            if (pUser && pUser->unionid>0 && chatCmd->dwType != Cmd::CHAT_TYPE_COUNTRY_PK && chatCmd->dwType != Cmd::CHAT_TYPE_COUNTRY_MARRY)
                            {
							    if (chatCmd->dwSysInfoType == Cmd::INFO_TYPE_KING || chatCmd->dwSysInfoType == Cmd::INFO_TYPE_CASTELLAN)
                                {
                                    chatCmd->dwSysInfoType = 0;
                                }

                                CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);

                                if (pUnion && pUnion->master && pUnion->master->id == pUser->id)
                                {//是帮主
                                    if (CCityM::getMe().findByUnionID(pUser->unionid) != NULL)
                                    {//城主
                                        CCity* pCity = CCityM::getMe().findByUnionID(pUser->unionid);
                                        SceneSession * pScene = SceneSessionManager::getInstance()->getSceneByID((pCity->dwCountry<<16)+pCity->dwCityID);
                                        if (pScene)
										{
                                            chatCmd->dwSysInfoType = Cmd::INFO_TYPE_CASTELLAN;

                                        }
                                    }

                                    if (CCityM::getMe().find(pUser->country, KING_CITY_ID, pUser->unionid) !=NULL)
                                    {//是国王
                                        chatCmd->dwSysInfoType = Cmd::INFO_TYPE_KING;
                                        CCountry* pEmperor = CCountryM::getMe().find(NEUTRAL_COUNTRY_ID);
                                        if (pEmperor && pEmperor->dwKingUnionID == pUser->unionid)
                                        {
                                            chatCmd->dwSysInfoType = Cmd::INFO_TYPE_EMPEROR;
                                        }
                                    }
                                }
                            }

                            if (pUser)
                            {
                                if (chatCmd->dwSysInfoType == Cmd::INFO_TYPE_EXP &&
                                    chatCmd->dwType == Cmd::CHAT_TYPE_COUNTRY)
                                {
                                    Zebra::logger->error("怀疑玩家%s使用外挂利用自定义消息刷屏", pUser->name);
                                }
                                SessionChannel::sendCountry(pUser->country, chatCmd, rev->size);
                                BYTE buf[zSocket::MAX_DATASIZE];
                                Cmd::GmTool::t_Chat_GmTool *cmd=(Cmd::GmTool::t_Chat_GmTool *)buf;
                                bzero(buf, sizeof(buf));
                                constructInPlace(cmd);

                                strncpy(cmd->userName, pUser->name, MAX_NAMESIZE);
                                cmd->countryID = pUser->country;
                                cmd->sceneID = pUser->scene->id;
                                cmd->dwType = chatCmd->dwType;
                                strncpy(cmd->content, chatCmd->pstrChat, 255);
                                cmd->size = chatCmd->size;
                                if (cmd->size)
                                    bcopy(chatCmd->tobject_array, cmd->tobject_array, cmd->size*sizeof(Cmd::stTradeObject));
                                SessionService::getInstance().sendCmdToSuperServer(cmd, sizeof(Cmd::GmTool::t_Chat_GmTool)+cmd->size*sizeof(Cmd::stTradeObject));
                            }
                            return true;
                        }
                        break;
                    case Cmd::CHAT_TYPE_OVERMAN_AFFICHE:	/// 师门公告
                    case Cmd::CHAT_TYPE_OVERMAN:			/// 师门频道
                        {
                            CSchoolM::getMe().sendSchoolChatMessages(pUser, chatCmd, rev->size);
                            return true;
                        }
                        break;
                    case Cmd::CHAT_TYPE_OVERMAN_PRIVATE:			/// 师门私聊
                        {
                            CSchoolM::getMe().sendSchoolPrivateChatMessages(pUser, chatCmd, rev->size);
                            return true;
                        }
                        break;
                    case Cmd::CHAT_TYPE_FAMILY_AFFICHE:		/// 家族公告
                    case Cmd::CHAT_TYPE_FAMILY:				/// 家族频道
                        {
                            CSeptM::getMe().sendSeptChatMessages(pUser, chatCmd, rev->size);
                            return true;
                        }
                        break;
                    case Cmd::CHAT_TYPE_FAMILY_PRIVATE:			/// 家族私聊
                        {
                            CSeptM::getMe().sendSeptPrivateChatMessages(pUser, chatCmd, rev->size);
                            return true;
                        }
                        break;
                    case Cmd::CHAT_TYPE_GM:
							case Cmd::CHAT_TYPE_SYTEM:
                        chatCmd->dwType = Cmd::CHAT_TYPE_SYSTEM;
                        chatCmd->dwSysInfoType = Cmd::INFO_TYPE_SCROLL;
                    case Cmd::CHAT_TYPE_WORLD:
                        {
                            CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);

                            if (pUnion && pUnion->master && pUnion->master->id == pUser->id)
                            {//是帮主

                                if (CCityM::getMe().find(pUser->country, KING_CITY_ID, pUser->unionid) !=NULL)
                                {//是国王   //soke 皇帝世界说话
                                    CCountry* pEmperor = CCountryM::getMe().find(NEUTRAL_COUNTRY_ID);
                                    if (pEmperor && pEmperor->dwKingUnionID == pUser->unionid)
                                    {
                                        chatCmd->dwSysInfoType = Cmd::INFO_TYPE_EMPEROR;
                                    }
                                }
                            }
                            SessionTaskManager::getInstance().sendCmdToWorld(chatCmd,rev->size);
                            BYTE buf[zSocket::MAX_DATASIZE];
                            Cmd::GmTool::t_Chat_GmTool *cmd=(Cmd::GmTool::t_Chat_GmTool *)buf;
                            bzero(buf, sizeof(buf));
                            constructInPlace(cmd);

                            strncpy(cmd->userName, pUser->name, MAX_NAMESIZE);
                            cmd->countryID = pUser->country;
                            cmd->sceneID = pUser->scene->id;
                            cmd->dwType = chatCmd->dwType;
                            strncpy(cmd->content, chatCmd->pstrChat, 255);


                            cmd->size = chatCmd->size;
                            if (cmd->size)
                                bcopy(chatCmd->tobject_array, cmd->tobject_array, cmd->size*sizeof(Cmd::stTradeObject));
                            SessionService::getInstance().sendCmdToSuperServer(cmd, sizeof(Cmd::GmTool::t_Chat_GmTool)+cmd->size*sizeof(Cmd::stTradeObject));
                            //}
                            return true;
                        }
                        break;
                    case Cmd::CHAT_TYPE_PERSON:
                        {
                            SessionChannel *cl=SessionChannelManager::getMe().get(chatCmd->dwChannelID);
                            if(cl)                                  
                            {
                                strncpy(chatCmd->pstrName, pUser->name, MAX_NAMESIZE);
                                cl->sendToOthers(pUser, chatCmd, rev->size);
                            }               
                            else                    
                                pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不存在此聊天频道");
                            return true;
                        }
                        break;
                    case Cmd::CHAT_TYPE_BLESS_MSG:
                        {//soke 送花祝福
                            UserSession *otherUser = UserSessionManager::getInstance()->getUserSessionByName(chatCmd->pstrName);
                            if (otherUser)
                            {
                                if (strncmp(pUser->name, chatCmd->pstrName, MAX_NAMESIZE)!=0)
                                {
                                    if (SessionTimeTick::currentTime<pUser->nextBlessTime)
                                    {
                                        pUser->sendSysChat(Cmd::INFO_TYPE_MSG,"对不起两分钟内你只能发送一次祝福现在距离下次可发送时间还有%u秒",(pUser->nextBlessTime.msecs()-SessionTimeTick::currentTime.msecs())/1000);
                                        return true;
                                    }
                                    else
                                    {
                                        pUser->nextBlessTime.now();
                                        pUser->nextBlessTime.addDelay(120000); //soke 时间间隔120秒
                                        strncpy(chatCmd->pstrName, pUser->name, MAX_NAMESIZE);
                                        otherUser->sendCmdToMe(chatCmd, cmdLen);
                                        pUser->sendSysChat(Cmd::INFO_TYPE_MSG,"祝福已送出");
                                        return true;
                                    }
                                }
                                else
                                {
                                    pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "这是你的名字");
                                }
                            }
                            else
                            {
                                pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "输入的名称不正确或指定的玩家不在线");
                            }
                            return true;
                        }
                        break;
                    default:
                        break;
                    }
                    return true;
                }
                break;
				case Cmd::BOSSINFO_USERCMD: //世界BOSS模块信息
				{
					return CCountryM::getMe().ProBossInfoSystem(pUser, (Cmd::stNullUserCmd*)rev->data, rev->size);
				}
				break;				
            case Cmd::LOONG_USERCMD:
                {
                    return CCountryM::getMe().processLoong(pUser, (Cmd::stNullUserCmd*)rev->data, rev->size);
                }
                break;
                /*
                case Cmd::GMTOOL_USERCMD:
                {
                switch (cmd->byParam)
                {
                case Cmd::MSG_GMTOOL_PARA:
                {
                Cmd::stMsgGmTool * rev = (Cmd::stMsgGmTool *)cmd;
                Cmd::GmTool::t_Msg_GmTool send;
                strncpy(send.userName, rev->userName, MAX_NAMESIZE);
                send.accid = pUser->accid;
                strncpy(send.country, (char *)pUser->countryName, MAX_NAMESIZE);
                send.type = rev->type;
                strncpy(send.content, rev->content, 256);
                send.contact = rev->contact;
                strncpy(send.tele, rev->tele, 101);

                SessionService::getInstance().sendCmdToSuperServer(&send, sizeof(send));
                return true;
                }
                break;
                default:
                return true;
                }
                }
                break;
                case Cmd::MAIL_USERCMD:
                {
                }
                break;
                */
            default:
                break;
            }
        }
        else
        {
            switch(cmd->byCmd)
            {
            case Cmd::CHAT_USERCMD:
                {
                    switch (cmd->byParam)
                    {
                    case REFRESH_BOSS_USERCMD_PARA:
                        {
                            Cmd::stRefreshBossUserCmd * msg = (Cmd::stRefreshBossUserCmd *)cmd;
                            SessionTaskManager::getInstance().sendCmdToCountry(msg->country, cmd, cmdLen);
                        }
                        break;
                    default:
                        break;
                    }
                }
                break;
            default:
                Zebra::logger->error("处理用户指(%d,%d)令时,未找到角色为%ld的用户",ptNullCmd->cmd , ptNullCmd->para , rev->dwID);
                break;
            }
            return true;
        }
    }
    Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, cmdLen);
    return false;
}

bool SessionTask::msgParse_Scene_BossInfoSystem(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	switch (cmd->para)
	{
        case Cmd::Session::PARA_NPC_BOSSDIE_INFO: //soke 场景请求写入BOSS死亡信息
		{
			Cmd::Session::t_BossDieInfo_SceneSession *rev = (Cmd::Session::t_BossDieInfo_SceneSession *)cmd;
			//soke 打开BOSS信息数据库表
			FieldSet* fs = SessionService::metaData->getFields("BOSSINFO");
			if (fs)
			{
				connHandleID handle = SessionService::dbConnPool->getHandle();
				if ((connHandleID)-1 == handle)
				{
					Zebra::logger->error("不能获取数据库句柄");
					return false;
				}
				RecordSet* recordset = NULL;
				Record rec,where;   
				std::ostringstream oss; 

				oss << "NPCID='" << rev->NpcID << "'";
				where.put("NPCID", oss.str());
				oss.str("");
				oss << "COUNTRYID='" << rev->CountryID << "'";
				where.put("COUNTRYID", oss.str());
				oss.str("");
		
				if ((connHandleID)-1 != handle)
				{
					recordset = SessionService::dbConnPool->exeSelect(handle, fs, NULL, &where);
				}

				if (recordset) //库内已经存在BOSS信息 不新增 只修改
				{
					rec.put("MAPNAME",rev->MapName);
					rec.put("KILLSTATE",rev->KillState); 
                    rec.put("REVIVETIME",rev->ReviveTime); 
            
					if ((connHandleID)-1 != handle)
					{
						SessionService::dbConnPool->exeUpdate(handle, fs, &rec, &where);
					}
				}
				SessionService::dbConnPool->putHandle(handle);
		    }
			return true;
        }
        break;
	}
    Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, cmd->cmd, cmd->para, cmdLen);
	return false;
}

bool SessionTask::msgParse(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	return MessageQueue::msgParse(cmd , cmdLen);
}
bool SessionTask::cmdMsgParse(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	switch(cmd->cmd)
	{
		case Cmd::CMD_NULL:
			break;
		case Cmd::Session::CMD_SCENE:
			if (msgParse_Scene(cmd, cmdLen))
			{
				return true;
			}
			break;
		case Cmd::Session::CMD_GATE:
			if (msgParse_Gate(cmd, cmdLen))
			{
				return true;
			}
			break;
		case Cmd::Session::CMD_FORWARD:
			if (msgParse_Forward(cmd, cmdLen))
			{
				return true;
			}
			break;
		case Cmd::Session::CMD_SCENE_SHUTDOWN:
			{
				switch(cmd->para)
				{
					case Cmd::Session::PARA_SHUTDOWN:
						{
							Cmd::Session::t_shutdown_SceneSession *sss = (Cmd::Session::t_shutdown_SceneSession*)cmd; 
							struct tm tm_1;
							time_t timval=time(NULL);
							//tm_1=*localtime(&timval);
							zRTime::getLocalTime(tm_1, timval);
							Zebra::logger->info("系统当前时间%d年%d月%d日%d时%d分%d秒",tm_1.tm_year+1900,tm_1.tm_mon+1,tm_1.tm_mday,tm_1.tm_hour,tm_1.tm_min,tm_1.tm_sec);
							if(sss->time)
							{
								SessionChannel::sendAllInfo(Cmd::INFO_TYPE_SCROLL, "系统当前时间%d年%d月%d日%d时%d分%d秒",tm_1.tm_year+1900,tm_1.tm_mon+1,tm_1.tm_mday,tm_1.tm_hour,tm_1.tm_min,tm_1.tm_sec);
								//tm_1=*localtime(&sss->time);
								zRTime::getLocalTime(tm_1, sss->time);
								SessionChannel::sendAllInfo(Cmd::INFO_TYPE_SCROLL, "系统将于%d年%d月%d日%d时%d分%d秒停机维护",tm_1.tm_year+1900,tm_1.tm_mon+1,tm_1.tm_mday,tm_1.tm_hour,tm_1.tm_min,tm_1.tm_sec);
								Zebra::logger->debug("系统将于%d年%d月%d日%d时%d分%d秒停机维护",tm_1.tm_year+1900,tm_1.tm_mon+1,tm_1.tm_mday,tm_1.tm_hour,tm_1.tm_min,tm_1.tm_sec);
								if(strlen(sss->info)>0)
								{
									SessionChannel::sendAllInfo(Cmd::INFO_TYPE_SCROLL, "%s",sss->info);
								}
								SessionService::getInstance().shutdown_time=*sss;
							}
							else
							{
								Zebra::logger->debug("取消停机维护");
								SessionChannel::sendAllInfo(Cmd::INFO_TYPE_SCROLL, "取消停机维护");
								SessionService::getInstance().shutdown_time=*sss;
				Cmd::Session::t_SetService_SceneSession send;
				send.flag |= Cmd::Session::SERVICE_MAIL;
				send.flag |= Cmd::Session::SERVICE_AUCTION;
				SessionTaskManager::getInstance().broadcastScene(&send, sizeof(send));

				SessionChannel::sendAllInfo(Cmd::INFO_TYPE_SYS, "邮件系统和拍卖系统已经启动，可以正常使用了");
				Zebra::logger->trace("取消停机，开启邮件和拍卖服务");
							}
							return true;
						}
						break;
				}
			}
			break;
		case Cmd::Session::CMD_SCENE_SEPT:
			{
				CSeptM::getMe().processSceneSeptMessage(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::CMD_SCENE_UNION:
			{
				CUnionM::getMe().processSceneUnionMessage(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::CMD_SCENE_COUNTRY:
			{
				CCountryM::getMe().processSceneMessage(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::CMD_SCENE_DARE:
			{
				CDareM::getMe().processSceneMessage(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::CMD_SCENE_ARMY:
			{
				CArmyM::getMe().processSceneMessage(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::CMD_SCENE_GEM:
			{
				CGemM::getMe().processSceneMessage(cmd, cmdLen);
				return true;
			}
			break;
		case Cmd::Session::CMD_SCENE_TMP:
			{
				switch(cmd->para)
				{
					case Cmd::Session::CLEARRELATION_PARA:
						{
							Cmd::Session::t_ClearRelation_SceneSession* rev=
								(Cmd::Session::t_ClearRelation_SceneSession*)cmd;

							//CSeptM::getMe().delSeptAllMember();
							CUnionM::getMe().delAllUnion(rev->dwUserID);
							return true;
						}
						break;
					case Cmd::Session::RETURN_CREATE_UNION_ITEM_PARA:
						{//给指定ID的用户添加两封邮件
							Cmd::Session::t_ReturnCreateUnionItem_SceneSession* rev=
								(Cmd::Session::t_ReturnCreateUnionItem_SceneSession*)cmd;

							Cmd::Session::t_sendMail_SceneSession sm;

							sm.mail.state = Cmd::Session::MAIL_STATE_NEW;
							strncpy(sm.mail.fromName, "系统", MAX_NAMESIZE);
							sm.mail.toID = rev->dwUserID;
							strncpy(sm.mail.title, "天羽令", MAX_NAMESIZE);
							sm.mail.type = Cmd::Session::MAIL_TYPE_MAIL;
							sm.mail.createTime = rev->item.createtime;
							sm.mail.delTime = sm.mail.createTime + 60*60*24*7;
							sm.mail.accessory = 1;
							sm.mail.itemGot = 0;
							snprintf(sm.mail.text, 255-1, "%s", "建立帮会所用道具");
							sm.mail.sendMoney = 0;
							sm.mail.recvMoney = 0;
							bcopy(&rev->item, &sm.item, sizeof(sm.item));

							MailService::getMe().sendMail(sm);
							MailService::getMe().sendMoneyMail("系统", 0, "", rev->dwUserID, 
									UnionDef::CREATE_UNION_NEED_PRICE_GOLD,
									"建立帮会所需银两", (DWORD)-1, 
									Cmd::Session::MAIL_TYPE_MAIL);

							return true;
						}
						break;
				}
			}
			break;
			case Cmd::Session::CMD_SCENE_BOSSINFO:
			if (msgParse_Scene_BossInfoSystem(cmd, cmdLen))
			{
				return true;
			}
			break;			
		case Cmd::Session::CMD_SCENE_FUBEN: //副本新
			{
                SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);
				return true;
			}
			break;
		default:
			break;
	}				
	//Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, cmd->cmd, cmd->para, cmdLen);
	return false;
}

SessionTask::~SessionTask()
{
}

