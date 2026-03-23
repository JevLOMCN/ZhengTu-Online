/**
 * \file
 * \version  $Id: MiniClient.cpp  $
 * \author  
 * \date 
 * \brief 定义计费服务器连接客户端
 *
 */

#include <unistd.h>
#include <iostream>

#include "zTCPClient.h"
#include "MiniCommand.h"
#include "MiniClient.h"
#include "ScenesServer.h"
//#include "SceneClient.h"
#include "SceneUserManager.h"
#include "Zebra.h"
//#include "LoginSessionManager.h"
#include "SceneTaskManager.h"

/**
 * \brief 计费服务器连接客户端
 *
 * 一个区中只有一个计费服务器，所以这里只需要保留一个指针，不需要连接管理器之类的东东
 *
 */
MiniClient *miniClient = NULL;

/**
 * \brief 建立到Mini服务器的连接
 *
 * \return 连接是否成功
 */
bool MiniClient::connectToMiniServer()
{
	if (!connect())
	{
		Zebra::logger->error("连接Mini服务器失败");
		return false;
	}

	using namespace Cmd::Mini;
	t_LoginMini tCmd;
	tCmd.wdServerID = ScenesService::getInstance().getServerID();
	tCmd.wdServerType = ScenesService::getInstance().getServerType();

	return sendCmd(&tCmd, sizeof(tCmd));
}

/**
 * \brief 重载zThread中的纯虚函数，是线程的主回调函数，用于处理接收到的指令
 *
 */
void MiniClient::run()
{
	zTCPBufferClient::run();

	while(!ScenesService::getInstance().isTerminate())
	{
		while(!connect())
		{
			Zebra::logger->error("连接小游戏服务器失败");
			zThread::msleep(1000);
		}
		Cmd::Super::t_restart_ServerEntry_NotifyOther notify;
		notify.srcID=ScenesService::getInstance().getServerID();
		notify.dstID=this->getServerID();
		ScenesService::getInstance().sendCmdToSuperServer(&notify, sizeof(notify));
		zThread::msleep(2000);
		connect();
		using namespace Cmd::Mini;
		t_LoginMini tCmd;
		tCmd.wdServerID = ScenesService::getInstance().getServerID();
		tCmd.wdServerType = ScenesService::getInstance().getServerType();

		if(sendCmd(&tCmd, sizeof(tCmd)))
		{
			zTCPBufferClient::run();
		}
			// */
		zThread::msleep(1000);
	}
	//与Mini之间的连接断开，不需要关闭服务器
	//ScenesService::getInstance().Terminate();
}

struct Capacity : public PackageCallback
{       
	public: 
		Capacity(SceneUser* user, DWORD id, DWORD level) : _user(user), _id(id), _level(level), _num(0)
		{ }

		bool exec(zObject* o)
		{
			if (o && o->data.dwObjectID==_id && o->data.upgrade==_level && o->data.dwNum<o->base->maxnum)
				_num += o->base->maxnum-o->data.dwNum;
			return true;
		}

		DWORD num() const
		{
			return _num;
		}

	private:
		SceneUser* _user;
		DWORD _id;
		DWORD _level;
		DWORD _num;
};

struct DrawCombin : public PackageCallback
{       
	public: 
		DrawCombin(SceneUser* user, DWORD id, DWORD level, DWORD &num) : _user(user), _id(id), _level(level), _num(num)
		{ }

		bool exec(zObject* o)
		{
			if (o && o->data.dwObjectID==_id && o->data.upgrade==_level && o->data.dwNum<o->base->maxnum)
			{
				DWORD n = (_num>o->base->maxnum-o->data.dwNum)?(o->base->maxnum-o->data.dwNum):_num;

				o->data.dwNum += n;
				Cmd::stRefCountObjectPropertyUserCmd status;
				status.qwThisID = o->data.qwThisID;
				status.dwNum = o->data.dwNum;
				_user->sendCmdToMe(&status, sizeof(status));

				_num -= n;
			}
			return _num>0;
		}

		DWORD num() const
		{
			return _num;
		}

	private:
		SceneUser* _user;
		DWORD _id;
		DWORD _level;
		DWORD &_num;
};
/**
 * \brief 解析来自Mini服务器的所有指令
 *
 * \param ptNullCmd 待解析的指令
 * \param nCmdLen 待解析的指令长度
 * \return 解析是否成功
 */
bool MiniClient::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Mini;
	using namespace Cmd;

	switch (ptNullCmd->para)
	{
		case PARA_SCENE_CHECK_DRAW:
			{
				t_Scene_Check_Draw *rev = (t_Scene_Check_Draw *)ptNullCmd;
				SceneUser *u = SceneUserManager::getMe().getUserByID(rev->userID);
				if (!u) return false;

				zObjectB *base = objectbm.get(584);
				if (!base) return false;

				Capacity c(u, 584, 0);
				u->packs.main.execEvery(c);
				if ((c.num()+u->packs.main.space()*base->maxnum)<rev->num)
				{
					stDrawRetCommonMiniGameCmd send;
					send.ret = 3;//包裹满
					u->sendCmdToMe(&send, sizeof(send));
					return true;
				}
				return sendCmd(rev, nCmdLen);
			}
			break;
		case PARA_SCENE_DRAW_RET:
			{
				t_Scene_Draw_Ret *rev = (t_Scene_Draw_Ret *)ptNullCmd;
				SceneUser *u = SceneUserManager::getMe().getUserByID(rev->userID);
				if (!u)
				{
					rev->ret = 2;//不在线
					sendCmd(rev, nCmdLen);
					return true;
				}

				zObjectB *base = objectbm.get(584);
				if (!base)
				{
					rev->ret = 0;//失败
					sendCmd(rev, nCmdLen);
					return false;
				}

				//合并
				DrawCombin dc(u, 584, 0, rev->num);
				u->packs.main.execEvery(dc);

				//添加包裹
				for (DWORD i=rev->num; i!=0;)
				{
					DWORD num = (i>base->maxnum)?base->maxnum:i;

					zObject* o = zObject::create(base, num, 0);
					if (o && u->packs.addObject(o, true, AUTO_PACK))
					{
						rev->num -= num;

						Cmd::stAddObjectPropertyUserCmd send;
						bcopy(&o->data, &send.object, sizeof(t_Object)); 
						u->sendCmdToMe(&send, sizeof(send));
						Channel::sendSys(u, Cmd::INFO_TYPE_GAME, "得到%s%ld个",o->name,o->data.dwNum);
					}

					i -= num;
				}

				rev->ret = rev->num?3:1;//添加包裹失败
				sendCmd(rev, nCmdLen);
				return true;
			}
			break;
		case PARA_SCENE_DEPOSIT:
			{
				t_Scene_Deposit *rev = (t_Scene_Deposit *)ptNullCmd;
				SceneUser *u = SceneUserManager::getMe().getUserByID(rev->userID);
				if (!u) return false;

				if (!u->packs.checkMoney(rev->num*100) || !u->packs.removeMoney(rev->num*100, "小游戏充值"))
				{
					Channel::sendSys(u, Cmd::INFO_TYPE_FAIL, "你的金钱不足");
					return true;
				}
				return sendCmd(rev, nCmdLen);
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

