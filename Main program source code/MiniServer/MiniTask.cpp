/**
 * \file
 * \version  $Id: MiniTask.cpp  $
 * \author  
 * \date 
 * \brief 实现读档连接类
 *
 * 
 */


#include "MiniTask.h"
#include "MiniCommand.h"
#include "MiniServer.h"
#include "MiniTaskManager.h"
#include "MiniUserManager.h"
#include "MiniUser.h"
#include "MiniHall.h"
#include "zMetaData.h"


/**
 * \brief 验证登陆档案服务器的连接指令
 *
 * 如果验证不通过直接断开连接
 *
 * \param ptCmd 登陆指令
 * \return 验证是否成功
 */
bool MiniTask::verifyLogin(const Cmd::Mini::t_LoginMini *ptCmd)
{
	using namespace Cmd::Mini;

	if (CMD_LOGIN == ptCmd->cmd
			&& PARA_LOGIN == ptCmd->para)
	{
		const Cmd::Super::ServerEntry *entry = MiniService::getInstance().getServerEntry(ptCmd->wdServerID);
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

/**
 * \brief 等待接受验证指令并进行验证
 *
 * 实现虚函数<code>zTCPTask::verifyConn</code>
 *
 * \return 验证是否成功，或者超时
 */
int MiniTask::verifyConn()
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
			using namespace Cmd::Mini;
			if (verifyLogin((t_LoginMini *)pstrCmd))
			{
				Zebra::logger->debug("客户端连接通过验证");
				veriry_ok=true;
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

bool MiniTask::checkRecycle()
{
	if(recycle_state == 0)
	{
		return false;
	}
	if(recycle_state == 1)
	{
		MiniUserManager::getInstance()->removeUserByGatewayID(this);
		recycle_state=2;
		return true;
	}
	return true;
}
/**
 * \brief 确认一个服务器连接的状态是可以回收的
 *
 * 当一个连接状态是可以回收的状态，那么意味着这个连接的整个生命周期结束，可以从内存中安全的删除了：）<br>
 * 实现了虚函数<code>zTCPTask::recycleConn</code>
 *
 * \return 是否可以回收
 */
int MiniTask::recycleConn()
{
	if(veriry_ok)
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
	}
	return 1;
}
/*
{

		MiniUserManager::getInstance()->removeUserByGatewayID(wdServerID);
	return 1;
}
// */

bool MiniTask::uniqueAdd()
{
	return MiniTaskManager::getInstance().uniqueAdd(this);
}

bool MiniTask::uniqueRemove()
{
	return MiniTaskManager::getInstance().uniqueRemove(this);
}

/**
 * \brief 解析来自各个服务器连接的指令
 *
 * \param ptNullCmd 待处理的指令
 * \param nCmdLen 指令长度
 * \return 处理是否成功
 */
bool MiniTask::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	return MessageQueue::msgParse(ptNullCmd , nCmdLen);
}


bool MiniTask::cmdMsgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Mini;

	switch(ptNullCmd->cmd)
	{
		case CMD_GATE:
			{
				return parseGateMsg(ptNullCmd, nCmdLen);
			}
			break;
		case CMD_FORWARD:
			{
				return parseForwardMsg(ptNullCmd, nCmdLen);
			}
			break;
		case CMD_SCENE:
			{
				return parseSceneMsg(ptNullCmd, nCmdLen);
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

bool MiniTask::parseGateMsg(const Cmd::t_NullCmd * cmd, const unsigned int len)
{
	using namespace Cmd::Mini;

	if (!cmd || cmd->cmd!=CMD_GATE) return false;

	switch (cmd->para)
	{
		case PARA_GATE_USER_LOGIN:
			{
				t_UserLogin_Gateway *rev = (t_UserLogin_Gateway *)cmd;
				MiniUser * user = MiniUserManager::getInstance()->newUser(rev);
				if (!user)
				{
					Zebra::logger->error("用户登录失败！id=%u name=%s", rev->userID, rev->name);
					return false;
				}
				MiniHall::getMe().userEnter(user);
			}
			break;
		case PARA_GATE_USER_LOGOUT:
			{
				t_UserLogout_Gateway *rev = (t_UserLogout_Gateway *)cmd;
				MiniUser *u = MiniUserManager::getMe().getUserByID(rev->userID);
				if (u)
					MiniHall::getMe().userLeave(u);
				return true;
			}
			break;
		case PARA_GATE_USER_DELETE:
			{
				t_UserDelete_Gateway *rev = (t_UserDelete_Gateway *)cmd;

				FieldSet* fs = MiniService::metaData->getFields("MINIGAME");

				if (fs)
				{
					connHandleID handle = MiniService::dbConnPool->getHandle();

					if ((connHandleID)-1 == handle)
					{   
						Zebra::logger->error("newUser()不能获取数据库句柄");
						return false;
					}

					Record where;
					char w[32];
					bzero(w, sizeof(w));
					snprintf(w, sizeof(w)-1, "`CHARID`=%u", rev->userID);
					where.put("charid", w);

					if (MiniService::dbConnPool->exeDelete(handle, fs, &where))
						Zebra::logger->trace("玩家删号，清除得分记录 id=%u", rev->userID);
					MiniService::dbConnPool->putHandle(handle);
				}
			}
			break;
		default:
			break;
	}

	return false;
}

bool MiniTask::parseForwardMsg(const Cmd::t_NullCmd * cmd, const unsigned int len)
{
	using namespace Cmd::Mini;

	if (!cmd || cmd->cmd!=CMD_FORWARD) return false;

	switch (cmd->para)
	{
		/*
		case PARA_FORWARD_SCENE_TO_MINI:
			{
				t_Scene_ForwardMini *rev = (t_Scene_ForwardMini *)cmd;
				return true;
			}
			break;
			*/
		case PARA_USER_FORWARD_MINI:
			{
				t_Mini_UserForwardMini *rev = (t_Mini_UserForwardMini *)cmd;
				MiniUser *u = MiniUserManager::getMe().getUserByID(rev->id);
				//if (!u) return false;
				Cmd::stMiniGameUserCmd *msg = (Cmd::stMiniGameUserCmd *)rev->data;
				if (msg->byCmd!=Cmd::MINIGAME_USERCMD) break;

				return MiniHall::getMe().parseUserCmd(u, msg, rev->size);
			}
			break;
		default:
			break;
	}

	return false;
}

bool MiniTask::parseSceneMsg(const Cmd::t_NullCmd * cmd, const unsigned int len)
{  
	using namespace Cmd::Mini;
	using namespace Cmd;

	if (!cmd || cmd->cmd!=CMD_SCENE) return false;

	switch (cmd->para)
	{
		case PARA_SCENE_SET_SCENE:
			{
				t_Scene_SetScene *rev = (t_Scene_SetScene *)cmd;
				MiniUser *u = MiniUserManager::getMe().getUserByID(rev->userID);
				if (!u) return true;

				Zebra::logger->trace("%s(%u) 切换场景服务器 %u", u->name, u->id, rev->serverID);
				MiniTask *t = MiniTaskManager::getInstance().getTaskByID(rev->serverID);
				if (!t)
				{
					//MiniHall::getMe().userLeave(u);
					return false;
				}

				u->setScene(t);
				return true;
			}
		case PARA_SCENE_DEPOSIT:
			{
				t_Scene_Deposit *rev = (t_Scene_Deposit *)cmd;
				MiniUser *u = MiniUserManager::getMe().getUserByID(rev->userID);
				if (!u)
				{
					if (addDBMoney(rev->userID, rev->num))
					{
						Zebra::logger->trace("玩家 userID=%u 直接写数据库充值 money=%u 成功", rev->userID, rev->num);
						return true;
					}
					else
						return false;
				}

				u->addMoney(rev->num);
				u->save();
				u->sendMiniInfo(Cmd::MCT_POPUP, "购买成功！");
				Zebra::logger->trace("%s(%u) 充值 %u，现有 %u", u->name, u->id, rev->num, u->getMoney());
			}
			break;
		case PARA_SCENE_CHECK_DRAW:
			{
				t_Scene_Check_Draw *rev = (t_Scene_Check_Draw *)cmd;
				MiniUser *u = MiniUserManager::getMe().getUserByID(rev->userID);
				if (!u) return false;

				if (!u->checkMoney(rev->num))
				{
					stDrawRetCommonMiniGameCmd send;
					send.ret = 2;//仙丹不足
					u->sendCmdToMe(&send, sizeof(send));
					return true;
				}

				t_Scene_Draw_Ret send;
				send.userID = u->id;
				send.num = rev->num;
				send.ret = 0;
				if (!u->sendCmdToScene(&send, sizeof(send)))
					return false;

				u->removeMoney(rev->num);

				if (u->save())
					Zebra::logger->trace("%s(%u) 兑换银两 %u，现有 %u", u->name, u->id, rev->num, u->getMoney());
			}
			break;
		case PARA_SCENE_DRAW_RET:
			{
				t_Scene_Draw_Ret *rev = (t_Scene_Draw_Ret *)cmd;
				MiniUser *u = MiniUserManager::getMe().getUserByID(rev->userID);
				if (rev->ret==1)
				{
					stDrawRetCommonMiniGameCmd send;
					send.ret = 1;//成功
					u->sendCmdToMe(&send, sizeof(send));
					Zebra::logger->trace("兑换银两完成 userID=%u rev->num=%u", rev->userID, rev->num);
					return true;
				}
				else
				{
					if (u)
					{
						u->addMoney(rev->num);
						Zebra::logger->trace("%s(%u) 兑换仙丹失败，补偿成功 num=%u 现有 %u", u->name, u->id, rev->num, u->getMoney());
						return true;
					}
					else
					{
						if (addDBMoney(rev->userID, rev->num))
						{
							Zebra::logger->trace("兑换仙丹失败，补偿成功 userID=%u money=%u ret=%u", rev->userID, rev->num, rev->ret);
							return true;
						}
						else
						{
							Zebra::logger->error("兑换仙丹失败，补偿也失败 userID=%u money=%u ret=%u", rev->userID, rev->num, rev->ret);
							stDrawRetCommonMiniGameCmd send;
							send.ret = 0;//失败
							u->sendCmdToMe(&send, sizeof(send));
							return true;
						}
					}
				}
			}
			break;
		default:
			break;
	}

	return false;
}

/**
 * \brief 发送命令给场景用户
 *
 * \param id 用户id
 * \param pstrCmd 命令指令
 * \param nCmdLen 命令长度
 */
bool MiniTask::sendCmdToScene(DWORD id, const void *pstrCmd, const unsigned int nCmdLen)
{
	/*
	using namespace Cmd::Mini;
	using namespace Cmd;
	
	BYTE buf[zSocket::MAX_DATASIZE];
	t_Mini_ForwardMiniToScene *scmd=(t_Mini_ForwardMiniToScene *)buf;
	constructInPlace(scmd);
	
	scmd->id=id;
	scmd->size=nCmdLen;
	bcopy(pstrCmd,scmd->data,nCmdLen);
	return sendCmd(scmd, sizeof(t_Mini_ForwardMiniToScene)+nCmdLen);
	*/
	return true;
}

/**
 * \brief 发送命令给用户
 *
 * \param id 用户id
 * \param pstrCmd 命令指令
 * \param nCmdLen 命令长度
 */
bool MiniTask::sendCmdToUser(DWORD id, const void *pstrCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Mini;
	using namespace Cmd;
	
	BYTE buf[zSocket::MAX_DATASIZE];
	t_Mini_ForwardUser *scmd=(t_Mini_ForwardUser *)buf;
	constructInPlace(scmd);
	
	scmd->id = id;
	scmd->size = nCmdLen;
	bcopy(pstrCmd, scmd->data, nCmdLen);
	return sendCmd(scmd, sizeof(t_Mini_ForwardUser)+nCmdLen);
}

bool MiniTask::addDBMoney(DWORD userID, DWORD num)
{
	FieldSet* fs = MiniService::metaData->getFields("MINIGAME");

	if (fs)
	{
		connHandleID handle = MiniService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{   
			Zebra::logger->error("addDBMoney()不能获取数据库句柄");
			return false;
		}   

		Record where;
		char w[32];
		bzero(w, sizeof(w));
		snprintf(w, sizeof(w)-1, "`CHARID`=%u", userID);
		where.put("charid", w);

		RecordSet *recordset = MiniService::dbConnPool->exeSelect(handle, fs, NULL, &where);

		if (recordset)
		{
			Record* rec = recordset->get(0);
			DWORD money = rec->get("money");

			if (money+num>MAX_MONEY)
				money = MAX_MONEY;
			else
				money += num;

			Record r;
			r.put("money", money);

			if ((DWORD)-1==MiniService::dbConnPool->exeUpdate(handle, fs, &r, &where))
			{
				Zebra::logger->error("往数据库增加金钱时写数据库失败! userID=%u money=%u", userID, num);
				return false;
			}

			SAFE_DELETE(recordset);
			MiniService::dbConnPool->putHandle(handle);
			return true;
		}
		else
			Zebra::logger->error("往数据库增加金钱时没找到记录 userID=%u money=%u", userID, num);

		MiniService::dbConnPool->putHandle(handle);
	}
	return false;
}
