/**
 * \file
 * \version  $Id: CSept.cpp $
 * \author   
 * \date 
 * \brief 实现家族管理功能
 *
 */

#include <stdarg.h>
#include "zRWLock.h"
#include "Sept.h"
#include "CSept.h"
#include "Command.h"
#include "zDBConnPool.h"
#include "SessionServer.h"
#include "Session.h"
#include "OfflineMessage.h"
#include "zTime.h"
#include "CDare.h"
#include "CUnion.h"
#include "CCityManager.h"
#include "SessionChat.h"
#include "CNpcDare.h"
using namespace SeptDef;

class CSeptSort
{
	public:
		char  septName[MAX_NAMESIZE];           // 家族名称
		DWORD dwRepute;                         // 家族声望
		DWORD dwOrder;                          // 家族排名
		DWORD dwCountryID;			// 所属国家

		CSeptSort()
		{
			dwRepute = 0;
			dwOrder  = 0;
			dwCountryID = 0;
			bzero(septName, MAX_NAMESIZE);
		}

		CSeptSort(const CSeptSort& ref)
		{
			dwRepute = ref.dwRepute;
			dwOrder = ref.dwOrder;
			dwCountryID = ref.dwCountryID;
			strncpy(septName, ref.septName, MAX_NAMESIZE);
		}

		~CSeptSort()
		{
		}
		
		CSeptSort& operator =(const CSeptSort& ref)
		{
			this->dwRepute = ref.dwRepute;
			this->dwOrder = ref.dwOrder;
			this->dwCountryID = ref.dwCountryID;
			strncpy(this->septName, ref.septName, MAX_NAMESIZE);
			return *this;
		}

		friend bool operator<(const CSeptSort& lhs, const CSeptSort& rhs)
		{
			return lhs.dwRepute > rhs.dwRepute;
		}
};

//==[CSeptMember]==================================================
/**
* \brief 家族成员构造函数，初始化基本属性
* \author fqnewman
*/
CSeptMember::CSeptMember()
{
	destroy  = false;
	//user = NULL;
}

/**
* \brief 家族成员初始化
* \param info 成员信息结构
* \author fqnewman
*/
void CSeptMember::init(const stSeptMemberInfo& info)
{
	rwlock.wrlock();

	id = info.dwCharID;
	strncpy(name,info.name,MAX_NAMESIZE);
	strncpy(aliasname, info.aliasname, MAX_NAMESIZE);
	wdOccupation = info.wdOccupation;
	byStatus = CSeptMember::Offline;                  // 会员状态
	rwlock.unlock();
}

/**
* \brief 获取成员的基本信息
* \param info 返回的信息结构
* \author fqnewman
*/
void CSeptMember::getMemberBaseInfo(struct Cmd::stSeptRember& info)
{
	rwlock.rdlock();
	strncpy(info.memberName, name, MAX_NAMESIZE);
	strncpy(info.memberAliasName, aliasname, MAX_NAMESIZE);
	
	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
	if (pUser)
	{
		info.level = pUser->level;
		info.exploit = pUser->dwExploit;
		info.grace = pUser->dwGrace;
	}
	else
	{
		info.level = 0;
		info.exploit = 0;
		info.grace = 0;
	}
	
	info.byOnline = byStatus;
	info.occupation = wdOccupation;
	rwlock.unlock();
}

/**
* \brief 发送消息给成员对应的客户端
* \param  pstrCmd 消息体
* \param  nCmdLen 消息长度
* \author fqnewman
*/
void CSeptMember::sendCmdToMe(const Cmd::stNullUserCmd *pstrCmd, const unsigned int nCmdLen)
{
	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
	if (pUser)
	{
		pUser->sendCmdToMe(pstrCmd,nCmdLen);
	}
	else
	{
		if ((pstrCmd->byCmd == Cmd::CHAT_USERCMD) && (pstrCmd->byParam == ALL_CHAT_USERCMD_PARAMETER))
		{
			Cmd::stChannelChatUserCmd *pCmd = (Cmd::stChannelChatUserCmd *)pstrCmd;
			if (Cmd::CHAT_TYPE_FAMILY_AFFICHE == pCmd->dwType)
			{
				COfflineMessage::writeOfflineMessage(pCmd->dwType, this->id, pstrCmd, nCmdLen);
			}
		}
	}
}

/**
* \brief 更新成员的数据库存档
* \author fqnewman
*/
void CSeptMember::writeDatabase()
{
	//FunctionTimes times(14,__FUNCTION__);
	static const dbCol septmember_define[] = {
		{ "`OCCUPATION`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ "ALIASNAME",		zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ NULL, 0, 0}
	};
	struct {
		WORD  wdOccupation;						// 会员的职业
		char  aliasname[MAX_NAMESIZE+1];
	} __attribute__ ((packed))
	updateseptmember_data;
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}

	rwlock.rdlock();
	
	updateseptmember_data.wdOccupation = wdOccupation;
	strncpy(updateseptmember_data.aliasname, aliasname, MAX_NAMESIZE);
	
	rwlock.unlock();
	
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "SEPTID = %u AND CHARID = %u", mySept->getID(), id);
	unsigned int retcode = SessionService::dbConnPool->exeUpdate(handle, "`SEPTMEMBER`", septmember_define, (BYTE *)(&updateseptmember_data), where);
	SessionService::dbConnPool->putHandle(handle);

	if ((DWORD)-1 == retcode)
	{
		Zebra::logger->error("CSeptMember 修改会员档案失败：SEPTID=%u CHARID=%u retcode=%u", mySept->getID(), id, retcode);
	}
}

/**
* \brief 将成员记录插入数据库
* \author fqnewman
*/
void CSeptMember::insertDatabase()
{
	//FunctionTimes times(13,__FUNCTION__);
	static const dbCol septmember_define[] = {
		{ "`SEPTID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`NAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`ALIASNAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`OCCUPATION`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ NULL, 0, 0}
	};
	struct {
		DWORD dwSeptID;						// 家族编号
		DWORD	dwCharID;						// 会员角色ID
		char  name[MAX_NAMESIZE+1];           // 族员名称
		char  aliasname[MAX_NAMESIZE+1];           // 族员别名
		WORD  wdOccupation;						// 会员角色
	} __attribute__ ((packed))
	createseptmember_data;

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
					Zebra::logger->error("不能获取数据库句柄");
					return;
	}

	//插入数据库角色信息
	bzero(&createseptmember_data, sizeof(createseptmember_data));

	rwlock.rdlock();
	
	createseptmember_data.dwSeptID = mySept->getID();
	createseptmember_data.dwCharID = id;
	strncpy(createseptmember_data.name, name, MAX_NAMESIZE);
	strncpy(createseptmember_data.aliasname, aliasname, MAX_NAMESIZE);
	createseptmember_data.wdOccupation = wdOccupation;

	rwlock.unlock();

	unsigned int retcode = SessionService::dbConnPool->exeInsert(handle, "`SEPTMEMBER`", septmember_define, (const unsigned char *)(&createseptmember_data));
		SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("插入家族成员数据库出错 %u %u", mySept->getID(), id);
	}
	return;
}

void CSeptMember::change_aliasname(const char* aliasname)
{
	rwlock.wrlock();
	strncpy(this->aliasname, aliasname, MAX_NAMESIZE);
	rwlock.unlock();

	this->online(Cmd::SEPT_MEMBER_STATUS_ALIASCHANGE);
}

void CSeptMember::update_data()
{
	Cmd::Session::t_sendUserRelationID send;
	UserSession* pUser  = UserSessionManager::getInstance()->getUserByID(id);

	if (this->mySept)
	{
		send.dwUserID = this->id;
		send.dwID = this->mySept->id;
		send.type = Cmd::Session::RELATION_TYPE_SEPT;
		bzero(send.name, sizeof(send.name));
		strncpy(send.name, this->mySept->name, sizeof(send.name));

		send.septmaster = (this == this->mySept->master);
		send.dwRepute = this->mySept->getRepute();
		send.dwSeptLevel = this->mySept->dwLevel;

		if (pUser)	pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_sendUserRelationID));
	}
	if (pUser) CNpcDareM::getMe().sendUserData(pUser);
}

void CSeptMember::update_normal_data()
{
	Cmd::Session::t_SendSeptNormal_SceneSession send;
	UserSession* pUser  = UserSessionManager::getInstance()->getUserByID(id);

	if (this->mySept)
	{
		send.dwUserID = this->id;
		send.dwRepute = this->mySept->getRepute();

		if (pUser && pUser->scene)	pUser->scene->sendCmd(&send, sizeof(send));
	}
}


/**
* \brief 析构函数，析构的时候刷新成员的数据库存档
* \author fqnewman
*/
CSeptMember::~CSeptMember()
{
//	if (!destroy) writeDatabase();
}

/**
* \brief 设置成员的所属家族对象
* \param pSept 家族对象指针
* \author fqnewman
*/
void CSeptMember::setSept(CSept * pSept)
{
 	rwlock.wrlock();
 	mySept = pSept;
 	rwlock.unlock();
}

/**
* \brief 发送聊天消息给成员的客户端
* \param type 消息类型
* \param message 消息体
* \author fqnewman
*/
void CSeptMember::sendMessageToMe(int type, const char *message)
{
	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
	if (NULL !=pUser) pUser->sendSysChat(type, message);
}

/**
* \brief 将本成员的数据库记录从库中删除
* \author fqnewman
* \return true 成功， false 失败
*/
bool CSeptMember::deleteMeFromDB()
{
	//FunctionTimes times(12,__FUNCTION__);
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "SEPTID = %u AND CHARID = %u", mySept->getID(), id);
	unsigned int retcode = SessionService::dbConnPool->exeDelete(handle, "`SEPTMEMBER`", where);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->debug("删除家族成员失败 %u", id);
		return false;
	}
	else
	{
		destroy = true;
	}
	return true;
}

/**
* \brief 解除成员与家族的关系
* \param notify 通知标志，为true表示要通知所有的在线成员，为flase表示不通知
* \author fqnewman
*/
void CSeptMember::fireMe(const bool notify, const bool checkunion)
{
	if (deleteMeFromDB())
	{
		CSeptM::getMe().removeMemberIndex(name);
		if (notify) 
		{
			mySept->notifyMemberFire(name);
			mySept->sendSeptNotify("%s 退出了家族", name);
		}
		
		CUnion* pUnion = CUnionM::getMe().getUnionByID(this->mySept->dwUnionID);
		
		if (pUnion && checkunion)
		{
			if (this->id == this->mySept->master->id)
			{
				 pUnion->fireSeptFromUnion(this->mySept->id);
			}
			else
			{
				pUnion->fireUnionMemberDirect(id, false);
			}
		}
		
		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
		if (pUser)
		{
			Cmd::Session::t_fireSeptMember_SceneSession send;
			send.dwCharID = id;
			
			send.dwMapTempID = pUser->scene->tempid;
			pUser->scene->sendCmd(&send,sizeof(Cmd::Session::t_fireSeptMember_SceneSession)); /// 通知场景服务器
			pUser->septid = 0;
			CNpcDareM::getMe().sendUserData(pUser);
		}
	}
	else
	{
		Zebra::logger->error("[家族]:开除 %s 成员失败", this->name);
	}
}

/**
* \brief 发送用户的家族成员数据
* \author fqnewman
*/
void CSeptMember::sendUserSeptData()
{
	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
	if (mySept && pUser)
	{
		mySept->sendSeptInfoToUser(pUser); // 发送家族的信息给当前成员
		mySept->sendSeptMemberList(pUser); // 发送家族的成员列表给当前成员
	}
}

/**
* \brief 判断成员是否在线
* \author fqnewman
* \return true在线  false 不在线
*/
bool CSeptMember::isOnline()
{
    return (byStatus==CSeptMember::Online)?true:false;
}

/** 
* \brief 成员上线处理
* \param status 成员的上线状态
* \author fqnewman
*/
void CSeptMember::online(const unsigned int status)
{
	Cmd::stBroadcastSeptMemberInfo send;
	
 	rwlock.wrlock();
 	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);

	if (pUser)
	{
		byStatus = CSeptMember::Online; 
		wdOccupation = pUser->occupation;
		send.level = pUser->level;
		send.exploit = pUser->dwExploit;
		send.grace = pUser->dwGrace;
	}
	
	send.byStatus = status;
	send.wdOccupation = wdOccupation;
	strncpy(send.name,name,MAX_NAMESIZE);
	strncpy(send.aliasName, aliasname, MAX_NAMESIZE);
	if (pUser) wdOccupation = pUser->occupation;
	
 	rwlock.unlock();


	if (mySept && mySept->master) 
	{
		if (mySept->master->id == this->id && pUser)
		{
			mySept->dwCountryID = pUser->country;
			mySept->writeDatabase();
		}
		mySept->sendCmdToAllMember(&send,sizeof(send));
	}
	else
	{
		Zebra::logger->error("在家族成员%s找不到自己的家族对象",send.name);
	}
}

/**
* \brief 成员下线处理
* \author fqnewman
*/
void CSeptMember::offline()
{
	Cmd::stBroadcastSeptMemberInfo send;
	
 	rwlock.wrlock();
 	byStatus = CSeptMember::Offline; 
// 	user = NULL;
	send.byStatus = Cmd::SEPT_MEMBER_STATUS_OFFLINE;
	strncpy(send.name,name,MAX_NAMESIZE);
 	rwlock.unlock();
	if (mySept) mySept->sendCmdToAllMember(&send,sizeof(send));
	else
	{
		Zebra::logger->error("在家族成员%s找不到自己的家族对象",send.name);
	}
}

/** 
* \brief 通知家族成员占领NPC的信息
* \param status 成员的上线状态
* \author fqnewman
*/
void CSeptMember::notifyNpcHoldData()
{
 	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);

	if (pUser)
	{
		CNpcDareM::getMe().sendUserData(pUser);
	}
}

void CSeptMember::sendGoldToMember(DWORD userID, DWORD num)
{
 	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);

	if (pUser)
	{
		Cmd::Session::t_notifyAddIntegral send;
		send.dwGoldUser = userID;
		send.dwUserID = id;
		send.dwNum = num;
		pUser->scene->sendCmd(&send, sizeof(send));
	}
}

//---------------------------------------new---------------------------------------------

/**
* \brief 删除实体根据名称
* \param name 实体名称
* \author fqnewman
*/
void CSept::removeEntryByName(const char * name)
{
	 zEntry *rm=getEntryByName(name);
	 removeEntry(rm);
}

/**
* \brief 家族初始化，根据传入数据初始化家族的基本信息
* \param info 家族数据结构
* \author fqnewman
*/
void CSept::init(const stSeptInfo & info)
{
	rwlock.wrlock();		
	id = info.dwSeptID;
	strncpy(name,info.name,MAX_NAMESIZE);
	byVote = info.byVote;
	tempid = info.dwCharID;
	dwCreateTime = info.dwCrTime;
	dwRepute = info.dwRepute;
	dwCountryID = info.dwCountryID;
	dwUnionID = info.dwUnionID;
	dwLevel = info.dwLevel;
	dwSpendGold = info.dwSpendGold;
	dwIsExp = info.dwIsExp;
	strncpy(note, info.note, 255);
	master = NULL;
	calltimes = info.calltimes;
	calldaytime = info.calldaytime;
	normal_exp_time = info.normalexptime;
	bossexp = info.bossexp;
	rwlock.unlock();
}

/**
* \brief 创建一个家族的帮主成员，分配权限
* \param info 家族成员数据结构
* \author fqnewman
*/
CSeptMember * CSept::addSeptMaster(const stSeptInfo& info)
{
	stSeptMemberInfo masterInfo;

	masterInfo.dwCharID = info.dwCharID;
	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(masterInfo.dwCharID);
	if (pUser) masterInfo.wdOccupation = pUser->occupation;
	strncpy(masterInfo.name,info.masterName,MAX_NAMESIZE);
	strncpy(masterInfo.aliasname, "族长", MAX_NAMESIZE);
	return addSeptMember(masterInfo);	
}

/**
* \brief 创建并初始化家族成员
* \param info 家族成员数据结构
* \author fqnewman
* \return 成功返回新创建的家族成员对象，失败返回NULL
*/
CSeptMember * CSept::addSeptMember(const stSeptMemberInfo& info)
{
	CSeptMember *pMember = NULL;
	
	pMember = new CSeptMember();
	pMember->init(info);
	if (pMember)
	{
		pMember->setSept(this);
		rwlock.wrlock();
		addEntry(pMember);
		rwlock.unlock();

		CSeptM::getMe().addMemberIndex(info.name, pMember);  // 在CSeptM中加入索引这样可以使外面的访问者通过CSeptM找到自己。
	}
	return pMember;
}

/**
* \brief 处理格式解析的宏
*/
#define getMessage(msg,msglen,pat)      \
do      \
{       \
        va_list ap;     \
        bzero(msg, msglen);     \
	va_start(ap, pat);              \
	vsnprintf(msg, msglen - 1, pat, ap);    \
	va_end(ap);     \
}while(false)

/**
* \brief 通知家族战结果
* \param  msg ... 通知内容
*/
void CSept::notifyWarResult(const char* msg, ...)
{
   char buf[1024+1];
   bzero(buf, sizeof(buf));
   
   struct findall : public execEntry<CSeptMember> 
   {
       const char* msg;

       findall(const char* pMsg)
       {
	   msg = pMsg;
       }

       ~findall(){}
       
       bool exec(CSeptMember* pMember)
       {
			UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
			if (pUser)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_SYS, msg);
			}
		   return true;
       }
   };

   getMessage(buf, 1024, msg);

   findall myList(buf);
   execEveryOne(myList);

}

/**
  * \brief 发送家族通知
  *
  * \param message 消息
  *
  * \author zjw
  */
void CSept::sendSeptNotify(const char* message, ...)
{
	if (message == NULL)
	{
		return;
	}

	char buf[1024+1];
	bzero(buf, sizeof(buf));
	getMessage(buf, 1024, message);

	Cmd::stChannelChatUserCmd send;
//	send.dwType=Cmd::CHAT_TYPE_FAMILY;
	send.dwType = Cmd::CHAT_TYPE_SYSTEM;
	send.dwSysInfoType = Cmd::INFO_TYPE_EXP;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));

	sprintf((char*)send.pstrName, "家族通知");
	sprintf((char*)send.pstrChat, "%s", buf);

	this->sendCmdToAllMember(&send, sizeof(send));

}

bool	CSept::isMember(DWORD dwUserID)
{
	CSeptMember * pMember = NULL;
	pMember = (CSeptMember *)getEntryByID(dwUserID);

	if (pMember)
	{
		return true;
	}

	return false;
}

void CSept::delSeptAllMember()
{
	rwlock.rdlock();
	
	Zebra::logger->trace("[家族]家族清空所有成员:%s", this->name);
	std::vector<DWORD> vMemberID;

	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CSeptMember *temp=(CSeptMember *)it->second;
		if (temp && temp->id != this->tempid)
		{
			vMemberID.push_back(temp->id);
		}
	}
	rwlock.unlock();

	for (std::vector<DWORD>::iterator vIter = vMemberID.begin(); vIter!=vMemberID.end(); vIter++)	
	{
		
		 CSeptM::getMe().processMemberLeave(this->id, *vIter);
	}
	
	this->writeDatabase();
}

/**
* \brief 发送命令给场景所有成员
* \param pNullCmd 进入战争状态通知消息
* \param cmdLen 消息长度
*/
void CSept::sendNpcDareCmdToScene(Cmd::Session::t_NpcDare_NotifyScene_SceneSession* pCmd, const unsigned int cmdLen)
{
    struct findall : public execEntry<CSeptMember>
    {
		Cmd::Session::t_NpcDare_NotifyScene_SceneSession* cmd;
		unsigned int cmdLen;

		findall(Cmd::Session::t_NpcDare_NotifyScene_SceneSession* ptCmd, const unsigned int nCmdLen)
		{
			cmd = ptCmd;
			cmdLen = nCmdLen;
		}

		~findall(){}
		bool exec(CSeptMember* pMember)
		{
			if (pMember)
			{
				UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
				if (pUser && pUser->scene)
				{
					cmd->dwUserID = pUser->id;
					pUser->scene->sendCmd(cmd, cmdLen);
				}
			}
			return true;
		}
	};

    findall myList(pCmd, cmdLen);
    execEveryOne(myList);
}

/**
* \brief 发送命令给场景设置所有成员的战争状态并通知客户端
* \param ptEnterWarCmd 进入战争状态通知消息
* \param cmdLen 消息长度
*/
void CSept::sendCmdToAllMemberScene(Cmd::Session::t_enterWar_SceneSession* ptEnterWarCmd, const unsigned int cmdLen)
{
    struct findall : public execEntry<CSeptMember>
    {
	Cmd::Session::t_enterWar_SceneSession* cmd;
	unsigned int cmdLen;

	findall(Cmd::Session::t_enterWar_SceneSession* ptCmd, const unsigned int nCmdLen)
	{
	    cmd = ptCmd;
	    cmdLen = nCmdLen;
	}

	~findall(){}
	bool exec(CSeptMember* pMember)
	{
		if (pMember)
		{
			UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
			if (pUser && pUser->scene)
			{
				cmd->dwUserID = pUser->id;
				cmd->dwSceneTempID = pUser->scene->tempid;

				if (cmd->dwStatus)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已进入家族对战状态。");
				}
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已退出家族对战状态。");
				}

				pUser->scene->sendCmd(cmd, cmdLen);
			}
		}

		return true;
	}
    };

    findall myList(ptEnterWarCmd, cmdLen);
    execEveryOne(myList);

}
void CSept::sendCallCmdToAllMemberScene(Cmd::Session::t_GoTo_Leader_Check_SceneSession* ptCmd, const unsigned int cmdLen,DWORD expect)
{
    struct findall : public execEntry<CSeptMember>
	{
		Cmd::Session::t_GoTo_Leader_Check_SceneSession* cmd;
		unsigned int cmdLen;
		DWORD expect;

		findall(Cmd::Session::t_GoTo_Leader_Check_SceneSession* ptCmd, const unsigned int nCmdLen,DWORD exp)
		{
			cmd = ptCmd;
			cmdLen = nCmdLen;
			expect = exp;
		}

		~findall(){}
		bool exec(CSeptMember* pMember)
		{
			if (pMember)
			{
				UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
				if (pUser && pUser->scene)
				{
					if(pUser->tempid != expect)
					{
						cmd->userTempID = pUser->tempid;
						pUser->scene->sendCmd(cmd, cmdLen);
					}
				}
			}

			return true;
		}
	};

    findall myList(ptCmd, cmdLen,expect);
    execEveryOne(myList);

}

/**
* \brief 发送命令给所有的成员（客户端）
* \param ptCmd 消息体
* \param nCmdLen 消息长度
* \author fqnewman
*/
void CSept::sendCmdToAllMember(const Cmd::stNullUserCmd * ptCmd, const unsigned int nCmdLen )
{
	struct findall : public execEntry<CSeptMember>
	{
		const Cmd::stNullUserCmd * cmd;
		unsigned int cmdLen;
		findall(const Cmd::stNullUserCmd * ptCmd, const unsigned int nCmdLen)
		{
			cmd = ptCmd;
			cmdLen = nCmdLen;
		}
		~findall(){}

		bool exec(CSeptMember *pMember)
		{
			if (pMember)
			{
				pMember->sendCmdToMe(cmd,cmdLen);
			}

			return true;
		}
	};

	if (1==byVote) return;
	findall myList(ptCmd,nCmdLen);
	execEveryOne(myList);
}

/**
* \brief 发送命令给所有的成员（客户端）
* \param ptCmd 消息体
* \param nCmdLen 消息长度
* \author fqnewman
*/
void CSept::sendCmdToAllMemberExcept(const Cmd::stNullUserCmd * ptCmd, const unsigned int nCmdLen , const char * except)
{
	struct findall : public execEntry<CSeptMember>
	{
		const Cmd::stNullUserCmd * cmd;
		unsigned int cmdLen;
		const char * except;
		findall(const Cmd::stNullUserCmd * ptCmd, const unsigned int nCmdLen, const char * e)
		{
			cmd = ptCmd;
			cmdLen = nCmdLen;
			except = e;
		}
		~findall(){}

		bool exec(CSeptMember *pMember)
		{
			if (pMember && strncmp(pMember->name, except, MAX_NAMESIZE))
			{
				pMember->sendCmdToMe(cmd,cmdLen);
			}

			return true;
		}
	};

	if (1==byVote) return;
	findall myList(ptCmd,nCmdLen, except);
	execEveryOne(myList);
}

/**
* \brief 家族经验分配结果发送到场景
* \param dwUserID 经验提供者的ID
* \param ptCmd 经验分配消息
* \author fqnewman
*/
void CSept::sendDistributeSeptExpToScene(const DWORD dwUserID, const Cmd::Session::t_distributeSeptExp_SceneSession * ptCmd)
{
	struct findAllExceptMe : public execEntry<CSeptMember>
	{
		Cmd::Session::t_distributeSeptExp_SceneSession send;
		const Cmd::Session::t_distributeSeptExp_SceneSession * cmd;
		DWORD userid;
		findAllExceptMe(const DWORD dwUserID, const Cmd::Session::t_distributeSeptExp_SceneSession * ptCmd)
		{
			cmd = ptCmd;
			userid = dwUserID;
		}
		~findAllExceptMe(){}

		bool exec(CSeptMember *pMember)
		{
			if (pMember->id != userid)
			{
				UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
				if (pUser && pUser->scene)
				{
#ifdef _DEBUGLOG
					Zebra::logger->debug("发送经验分配通知给%s经验为%u",pUser->name, cmd->dwExp);
#endif
					pUser->septExp +=cmd->dwExp;
					if (pUser->septExp >=30)
					{
						send.dwUserID = pMember->id;
						send.dwExp = pUser->septExp;
						pUser->septExp=0;
						pUser->scene->sendCmd(&send,sizeof(send));
					}
				}
			}
			return true;
		}
	};

	if (1==byVote) return;
	findAllExceptMe myList(dwUserID, ptCmd);
	execEveryOne(myList);
}


/**
* \brief 通知家族内有成员被开除
* \param pName 被开除者的名称
* \author fqnewman
*/
void CSept::notifyMemberFire(const char * pName)
{
	Cmd::stBroadcastSeptMemberInfo send;

	send.byStatus = Cmd::SEPT_MEMBER_STATUS_FIRE;
	strncpy(send.name,pName,MAX_NAMESIZE);
	sendCmdToAllMember(&send,sizeof(send));
}

/**
* \brief 发送用户的家族数据到客户端
* \param pName 接收信息的用户
* \author fqnewman
*/
void CSept::sendUserSeptData(const char *pName)
{
	CSeptMember *pMember;

	pMember = (CSeptMember *)getEntryByName(pName);
	if (pMember)
	{
		pMember->sendUserSeptData();
	}
}

/**
* \brief 发送家族信息给指定用户的客户端，用在用户上线初始化的时候。
* \param pUser 上线用户
* \author fqnewman
*/
void CSept::sendSeptInfoToUser(UserSession *pUser)
{
	Cmd::stSeptBaseInfoCmd retSept;

	rwlock.rdlock();
	strncpy(retSept.septName,name,MAX_NAMESIZE);     // 家族名称
	strncpy(retSept.master,master->name,MAX_NAMESIZE); // 家族会长
	
	if (master->id == pUser->id) 
		retSept.dwMasterTempID = pUser->tempid;
	else
		retSept.dwMasterTempID = 0;
	
	retSept.dwRepute = this->getRepute();
	retSept.dwLevel = this->dwLevel;
	strncpy(retSept.note, this->note, sizeof(retSept.note));
	rwlock.unlock();
	pUser->sendCmdToMe(&retSept,sizeof(retSept));
}

void CSept::full_SeptMember_Data(Cmd::stUnionRember*& tempPoint, unsigned int& count)
{
	rwlock.rdlock();
	for (zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CSeptMember *temp=(CSeptMember *)it->second;
		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(temp->id);

		strncpy(tempPoint->memberName, temp->name, MAX_NAMESIZE);
		strncpy(tempPoint->aliasname, temp->aliasname, MAX_NAMESIZE);
		tempPoint->byOnline = temp->byStatus;
		tempPoint->occupation = temp->wdOccupation;
		strncpy(tempPoint->septName, this->name, MAX_NAMESIZE);

		if (pUser)
		{
			tempPoint->exploit = pUser->dwExploit;
			tempPoint->level = pUser->level;
		}
		tempPoint++;
		count++;
	}

}

CSeptMember* CSept::getMemberByName(const char* pName)
{
	CSeptMember *pSeptMember=NULL;
		
	if (pName)
	{
		pSeptMember = (CSeptMember *)getEntryByName(pName);
	}

	return pSeptMember;
}

/**
* \brief 发送家族成员列表，发送有最大限制，最多只发100条家族成员记录。
* \param pUser 数据接受者对象
* \author fqnewman
*/
void CSept::sendSeptMemberList(UserSession *pUser)
{
	BYTE buf[zSocket::MAX_DATASIZE];
	unsigned int count;
	Cmd::stSeptRember *tempPoint;

	rwlock.rdlock();
	Cmd::stAllSeptMemberCmd *retCmd=(Cmd::stAllSeptMemberCmd *)buf;
	constructInPlace(retCmd);
	tempPoint = (Cmd::stSeptRember *)retCmd->memberList;
	unsigned int len = sizeof(Cmd::stSeptRember);
	count = 0;
	for (zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		struct Cmd::stSeptRember mInfo;
		CSeptMember *temp=(CSeptMember *)it->second;
		temp->getMemberBaseInfo(mInfo);
		bcopy(&mInfo,tempPoint,len);
		tempPoint++;
		count++;
		if (1000==count)
		{

			goto breakfor; // 当记录超过100的时候会超过命令发送的最大限制
		}
	}
breakfor:
	rwlock.unlock();
	retCmd->size = count;
	pUser->sendCmdToMe(retCmd,(count*sizeof(Cmd::stSeptRember)+sizeof(Cmd::stAllSeptMemberCmd)));
}

/**
* \brief 开除家族成员
* \param master 行使权利者的名称 
* \param member 被开除者的名称
* \author fqnewman
* \return 
*/
void  CSept::fireSeptMember(const char * master, const char *member)
{
	CSeptMember *pMaster = (CSeptMember *)getEntryByName(master);
	CSeptMember *pMember = (CSeptMember *)getEntryByName(member);
	if (pMaster)
	{
		if (pMember)
		{
			if (pMaster->id == pMember->id)
			{
				pMaster->sendMessageToMe(Cmd::INFO_TYPE_FAIL, "不能开除自己");
				return;
			}

			if (pMaster->id == this->master->id) //只有会长有资格
			{
				removeEntryByName(member);
				pMember->fireMe();
				SAFE_DELETE(pMember);
			}
			else
			{
				pMaster->sendMessageToMe(Cmd::INFO_TYPE_FAIL, "你没有开除成员的权力，如果你发现错误请报告给GM");
			}
		}
		else
		{
			pMaster->sendMessageToMe(Cmd::INFO_TYPE_FAIL, "你所在的家族中没有此人,请确认名字是否正确");
		}
	}
	else
	{
		Zebra::logger->debug("开除家族成员操作中出现错误的状态");
	}
}

/**
* \brief 直接开除家族成员
* \param dwSeptID 被开除的家族成员ID
* \author fqnewman
* \return 1 成功 2 失败
*/
int CSept::fireSeptMemberDirect(const DWORD dwCharID, const bool checkunion)
{
	int ret = 0;

	rwlock.wrlock();
	CSeptMember * pMember = NULL;
	pMember = (CSeptMember *)getEntryByID(dwCharID);
	removeEntry(pMember);
	rwlock.unlock();
	if (pMember)
	{
		pMember->fireMe(true, checkunion);
		SAFE_DELETE(pMember);
		ret = 1;
	}
	else
	{
		ret = 2;
	}

	return ret;
}

/**
* \brief 更新数据库记录
* \author fqnewman
*/
void CSept::writeDatabase()
{
	//FunctionTimes times(11,__FUNCTION__);
	const dbCol sept_update_define[] = {
		{ "`CHARID`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`VOTE`",			zDBConnPool::DB_BYTE,	sizeof(BYTE) }, 
		{ "`REPUTE`",                zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`COUNTRYID`",                zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`UNIONID`",                zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`LEVEL`",                zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`SPENDGOLD`",                zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`ISEXP`",                zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`NOTE`",			zDBConnPool::DB_STR,	sizeof(char[254+1])},
		{ "`CALLTIMES`",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`CALLDAYTIME`",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`NORMALEXPTIME`",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`BOSSEXP`",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },//家族BOSS经验值
		{ NULL, 0, 0}
	};
	struct {
		DWORD dwCharid;
		BYTE  byVote;
		DWORD dwRepute;
		DWORD dwCountryID;
		DWORD dwUnionID;
		DWORD dwLevel;
		DWORD dwSpendGold;
		DWORD dwIsExp;
		char  note[254+1];
		DWORD calltimes;
		DWORD calldaytime;
		DWORD normalexptime;
		DWORD bossexp;//家族BOSS经验值
	} __attribute__ ((packed))
	updatesept_data;
	char where[128];

	if (!master) return;

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}

	bzero(&updatesept_data,sizeof(updatesept_data));
	rwlock.rdlock();	

	if (master)
	{
		updatesept_data.dwCharid = master->id;
	}

	updatesept_data.dwLevel = this->dwLevel;
	updatesept_data.dwUnionID = this->dwUnionID;
	updatesept_data.dwCountryID = this->dwCountryID;
	updatesept_data.dwRepute = this->dwRepute;
	updatesept_data.byVote = byVote;
	strncpy(updatesept_data.note,note,255);
	updatesept_data.dwSpendGold = dwSpendGold;
	updatesept_data.dwIsExp = this->dwIsExp;
	updatesept_data.calltimes = this->calltimes;
	updatesept_data.calldaytime = this->calldaytime;
	updatesept_data.normalexptime = this->normal_exp_time;
	updatesept_data.bossexp = this->bossexp;
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "`SEPTID` = %u", id);
	rwlock.unlock();
	unsigned int retcode = SessionService::dbConnPool->exeUpdate(handle, "`SEPT`", sept_update_define, (BYTE *)(&updatesept_data), where);
	SessionService::dbConnPool->putHandle(handle);

	if ((DWORD)-1 == retcode)
	{
		Zebra::logger->error("CSept 修改会员档案失败：SEPTID=%u retcode=%u", id, retcode);
	}
}

/**
* \brief 构造家族，初始化变量
* \author fqnewman
*/
CSept::CSept()
{
	destroy = false;
	byVote  = 0;
	dwIsExp = 0;
	calltimes=0; 
	calldaytime=0; 
}

/**
* \brief 析构家族及其所有成员
* \author fqnewman
*/
CSept::~CSept()
{
	if (!destroy)
	{
		//writeDatabase();

		rwlock.wrlock();
		for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
		{
			CSeptMember *temp = (CSeptMember *)it->second;
			SAFE_DELETE(temp);
		}
		clear();
		rwlock.unlock();
	}
}

/**
* \brief 解散家族，将所有成员的家族关系解除包括帮主自己
* \author fqnewman
*/
void CSept::disbandSept()
{
	rwlock.wrlock();
	
	Zebra::logger->trace("[家族]:%s 家族解散", this->name);
	destroy = true;
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CSeptMember *temp=(CSeptMember *)it->second;
		if (temp)
		{
			temp->fireMe(false);
			SAFE_DELETE(temp);
		}
	}
	clear();
	deleteMeFromDB();
	rwlock.unlock();
}

/**
* \brief 删除本家族的数据库记录
* \author fqnewman
* \return true 成功 false 失败
*/
bool CSept::deleteMeFromDB()
{
	//FunctionTimes times(10,__FUNCTION__);
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "SEPTID = %u ", id);
	unsigned int retcode = SessionService::dbConnPool->exeDelete(handle, "`SEPT`", where);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("删除家族失败 %u", id);
		return false;
	}
	else
	{
		destroy = true;
	}
	return true;
}

/**
* \brief 从数据库中加载家族成员
* \author fqnewman
* \return true 加载成功， false 加载失败
*/
bool CSept::loadSeptMemberFromDB()
{
	//FunctionTimes times(9,__FUNCTION__);
	static const dbCol septmember_define[] = {
		{ "CHARID",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "NAME",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "ALIASNAME",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`OCCUPATION`",	zDBConnPool::DB_WORD,	sizeof(WORD) }, 
		{ NULL, 0, 0}
	};

	stSeptMemberInfo *memberList, *tempPoint;
	stSeptMemberInfo info;
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "SEPTID = %u", id);
	unsigned int retcode = SessionService::dbConnPool->exeSelect(handle, "`SEPTMEMBER`", septmember_define, where, NULL, (unsigned char **)&memberList);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode
			|| 0 == retcode)
	{
		Zebra::logger->debug("没有找到家族成员记录");
		return true;
	}

	if (memberList)
	{
		tempPoint = &memberList[0];
		for (unsigned int i=0; i< retcode; i++)
		{
			bcopy(tempPoint, &info, sizeof(stSeptMemberInfo));
			CSeptMember *member = addSeptMember(info);
			if (info.dwCharID == tempid) 
			{
				//Zebra::logger->trace("[家族加载]:%s(%d) 族长加载成功", member->name, this->id);
				master = member; // masterid被保存在tempid中；这里初始化master对象
			}
			tempPoint++;
		}
		SAFE_DELETE_VEC(memberList);
	}
	else
	{
		Zebra::logger->error("家族数据初始化失败，exeSelect 返回无效buf指针");
	}

	if (this->master==NULL)
	{
		this->byVote = 1;
		Zebra::logger->trace("[家族加载]:(%d) 族长加载失败", this->id);
		return false;
	}
	
	return true;
}

/**
* \brief 获取家族成员数目
* \author fqnewman
* \return 家族成员数字
*/
DWORD CSept::size()
{
	return zEntryName::size();
}

/**
* \brief 判断是否在投票状态
* \author fqnewman
* \return true 还在投票中 false 投票已结束
*/
bool CSept::isVote()
{
	return 1==byVote;
}

/**
* \brief 投票结束处理
* \author fqnewman
*/
void CSept::letVoteOver()
{
	struct findList : public execEntry<CSeptMember>
	{
		findList()
		{
		}
		~findList(){}
		bool exec(CSeptMember *pMember)
		{
			UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
			if (pUser)
			{
				//pMember->online();
				pMember->sendUserSeptData();
				pMember->update_data();
				pUser->septid = pMember->mySept->id;
			}
			return true;
		}
	};

	byVote = 0;
	findList myList;
	execEveryOne(myList);
	writeDatabase();
}

/**
* \brief 设置家族的介绍（并存库）
* \param pCmd 家族设置消息
* \author fqnewman
*/
void CSept::setNote(Cmd::stNoteSeptCmd *pCmd)
{
	rwlock.wrlock();
	strncpy(note,pCmd->noteBuf,255);
	rwlock.unlock();
	writeDatabase();
	sendCmdToAllMember(pCmd,sizeof(Cmd::stNoteSeptCmd));
}

DWORD CSept::getRepute()
{
	if (this->dwUnionID>0)
	{
		CUnion* pUnion = CUnionM::getMe().getUnionByID(this->dwUnionID);
		if (pUnion)
		{
			// TODO:判断是否是城主或国王 
			if (pUnion->master && this->master && (pUnion->master->id == this->master->id))
			{//本族族长是帮主
				if (CCityM::getMe().findByUnionID(pUnion->id) !=NULL)
				{//是国王或城主
					return (DWORD)(this->dwRepute*1.1);
				}
			}
		}
	}

	return this->dwRepute;
}

void CSept::sendSeptReputeToAll()
{
	rwlock.rdlock();
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CSeptMember *pMember=(CSeptMember *)it->second;
		if (pMember)
		{
			UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
			if (pMember->mySept && pUser)
			{
				pMember->mySept->sendSeptInfoToUser(pUser); // 发送家族的信息给当前成员
			}

			pMember->update_normal_data();
		}
		
	}
	rwlock.unlock();
}

void CSept::sendSeptInfoToAll()
{
	rwlock.rdlock();
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CSeptMember *pMember=(CSeptMember *)it->second;
		if (pMember)
		{
			pMember->sendUserSeptData();
			pMember->update_data();
		}
	}
	rwlock.unlock();
}

void CSept::changeRepute(int repute)
{
	FunctionTime func_time(0,__PRETTY_FUNCTION__,"改变一个家族声望所需时间" , 32);
	
	if (this->isVote()) return;
	if (repute>0)
	{
		this->dwRepute+=repute;
		this->sendSeptNotify("本家族声望上升 %d 点", repute);
	}
	else
	{
		this->sendSeptNotify("本家族声望降低 %d 点", abs(repute));
		
		if ((int)this->dwRepute>abs(repute))
		{
			this->dwRepute = (int)this->dwRepute - abs(repute);
		}
		else
		{
			this->dwRepute = 0;
		}
	}

	this->sendSeptReputeToAll();
	if (this->dwUnionID>0)
	{
		CUnion* pUnion = CUnionM::getMe().getUnionByID(this->dwUnionID);
		if (pUnion)
		{	
			pUnion->sendUnionManaToAll();
		}
	}

//	if (this->master) this->master->update_data();
	this->writeDatabase();
}

void CSept::changeLevel(int level)
{
	if (this->isVote()) return;
	if (level>0)
	{
		this->dwLevel+=level;
	}
	else
	{
		if ((int)this->dwLevel>abs(level))
		{
			this->dwLevel = (int)this->dwLevel - abs(level);
		}
		else
		{
			this->dwLevel = 0;
		}
	}

	this->sendSeptInfoToAll();
	this->writeDatabase();
}

/**
* \brief 通知家族成员新的NPC控制数据
* \author fqnewman
*/
void CSept::notifyNpcHoldData()
{
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CSeptMember *temp=(CSeptMember *)it->second;
		if (temp)
		{
			temp->notifyNpcHoldData();
		}
	}
}

void CSept::sendGoldToMember(DWORD userID, DWORD num)
{
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CSeptMember *temp=(CSeptMember *)it->second;
		if (temp)
		{
			temp->sendGoldToMember(userID, num);
		}
	}
}

//-------------------------------------------------------------------------------------

/**
* \brief 声明管理器实例
* \author fqnewman
*/
CSeptM *CSeptM::um(NULL);

/**
* \brief 管理器构造函数
* \author fqnewman
*/
CSeptM::CSeptM()
{
}

/**
* \brief 管理器析构函数
* \author fqnewman
*/
CSeptM::~CSeptM()
{
	rwlock.wrlock();
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CSept *temp =(CSept *)it->second;
		SAFE_DELETE(temp);
	}
	clear();
	rwlock.unlock();
}

/**
* \brief 主动释放管理器
* \author fqnewman
*/
void CSeptM::destroyMe()
{
	SAFE_DELETE(um);
}

/**
* \brief 在成员索引里面查找成员迭代
* return 迭代指针
*/
std::map<std::string, CSeptMember *>::iterator  CSeptM::findMemberIndex(const char *pName)
{
	char temp_name[MAX_NAMESIZE];
	bzero(temp_name, MAX_NAMESIZE);
	strncpy(temp_name, pName, MAX_NAMESIZE);
	return memberIndex.find(temp_name);
}

/**
* \brief 根据实体名称删除管理实体
* \param name 实体的名称
* \author fqnewman
*/
void CSeptM::removeEntryByName(const char * name)
{
	zEntry *rm=getEntryByName(name);
	removeEntry(rm);
}

/**
  * \brief 给家族全体在线成员发送消息
  *
  * \param septID 家族ID
  * \param message 消息长度
  *
  * \author fqnewman
  */
void CSeptM::sendNpcDareCmdToScene(const DWORD septID, Cmd::Session::t_NpcDare_NotifyScene_SceneSession* ptCmd, unsigned int nCmdLen)
{
	CSept *pSept = NULL;

	pSept = this->getSeptByID(septID);
	if (pSept)
	{       
		pSept->sendNpcDareCmdToScene(ptCmd, nCmdLen);
	}
	else
	{       
		Zebra::logger->error("发送命令给家族全体成员时，未找到家族:%d", septID);
	}
}

/**
  * \brief 发送家族通知
  *
  * \param septID 家族ID
  * \param message 消息长度
  *
  * \author zjw
  */
void CSeptM::sendSeptNotify(const DWORD septID, const char* message, ...)
{
	CSept *pSept = NULL;
	char buf[1024+1];
	bzero(buf, sizeof(buf));

	if (message==NULL)
	{
		return;
	}

	pSept = this->getSeptByID(septID);
	getMessage(buf, 1024, message);


	Cmd::stChannelChatUserCmd send;
	send.dwType=Cmd::CHAT_TYPE_SYSTEM;
	send.dwSysInfoType = Cmd::INFO_TYPE_EXP;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));

	sprintf((char*)send.pstrName, "家族通知");
	sprintf((char*)send.pstrChat, "%s", buf);

	if (pSept)
	{       
		pSept->sendCmdToAllMember(&send, sizeof(send));
	}
	else
	{       
		Zebra::logger->error("发送家族通知时，未找到家族:%d", septID);
	}
}

/**
* \brief 初始化家族管理器，从数据库中加载所有的家族数据
* \author fqnewman
* \return true 加载成功  false 加载失败
*/
bool CSeptM::init()
{
	//FunctionTimes times(8,__FUNCTION__);
	/*
	   Cmd::Record::t_GetSept_SeptRecord tCmd;
	   return RecordClientManager::getInstance().get().sendCmd(&tCmd, sizeof(tCmd));
	 */
	const dbCol sept_read_define[] = {
		{ "SEPTID",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "NAME",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "CHARID",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "MASTER",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`VOTE`",			zDBConnPool::DB_BYTE,	sizeof(BYTE) }, 
		{ "`CREATETIME`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ "`REPUTE`",	                zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`COUNTRYID`",                zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`UNIONID`",                  zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`LEVEL`",   		        zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`SPENDGOLD`",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`ISEXP`",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`NOTE`",			zDBConnPool::DB_STR,	sizeof(char[254+1]) },
		{ "`CALLTIMES`",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`CALLDAYTIME`",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`NORMALEXPTIME`",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`BOSSEXP`",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },//家族BOSS经验值
		{ NULL, 0, 0}
	};

	stSeptInfo *septList,*tempPoint;
	stSeptInfo info;

	septList = NULL;
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	unsigned int retcode = SessionService::dbConnPool->exeSelect(handle, "`SEPT`", sept_read_define, NULL, NULL, (BYTE **)&septList);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode
			|| 0 == retcode)
	{
		return true;
	}
	if (septList)
	{
		tempPoint = &septList[0];
		for (unsigned int i=0; i< retcode; i++)
		{
			bcopy(tempPoint, &info, sizeof(stSeptInfo));
			CSept *pSept = createSeptByDBRecord(info);
			if (!pSept->loadSeptMemberFromDB())
			{
				if (pSept->byVote && pSept->dwUnionID)
				{
					delSept(pSept->id);  // 如果是族长就解散家族
				}
			}
			//2005.11.3 modify by markzhong
			//if ((CREATE_SEPT_NEED_MAN_NUM == pSept->size()) && (pSept->isVote())) pSept->letVoteOver();
			tempPoint++;
		}
		SAFE_DELETE_VEC(septList);
		return true;
	}
	else
	{
		Zebra::logger->error("家族数据初始化失败，exeSelect 返回无效buf指针");
	}
	return false;
}

/**
* \brief 获取家族管理器的唯一实例
* \author fqnewman
* \return 家族管理器实例
*/
CSeptM &CSeptM::getMe()
{
	if(um==NULL)
	{
		um=new CSeptM();
	}
	return *um;
}

/**
* \brief 初始化家族对象并添加帮主成员 （createNewUnion()调用此方法）
* \param info 家族的数据结构
* \author fqnewman
* \return 创建并初始化好的家族对象
*/
CSept* CSeptM::createSeptAndAddMaster(const stSeptInfo & info)
{
	CSept *pSept = NULL;

	pSept = new CSept();
	pSept->init(info);
	pSept->master = pSept->addSeptMaster(info);
	pSept->master->insertDatabase();

	rwlock.wrlock();
	addEntry(pSept);
	rwlock.unlock();
	pSept->master->online(Cmd::SEPT_MEMBER_STATUS_NEWMEMBER);

	if (pSept->master)
	{
		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pSept->master->id);

		if (pUser)
		{
			pUser->septid = pSept->id;
		}

	  //soke 增加再发用户上线消息，解决不小退看不到家族...
	  //pSept->master->sendUserSeptData();
	 // pSept->master->online(Cmd::SEPT_MEMBER_STATUS_NEWMEMBER); 
		pSept->master->update_data();
	}

	Zebra::logger->trace("[家族]:%s(%u) 家族 被 %s 建立成功", pSept->name, pSept->id, pSept->master->name);
	SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pSept->dwCountryID, "恭喜%s成立了%s家族", pSept->master->name, pSept->name);
	return pSept;
}

/**
* \brief 根据数据库记录建立家族对象，系统加载的时候使用
* \param info 家族的数据结构
* \author fqnewman
* \return 创建成功的家族对象指针
*/
CSept* CSeptM::createSeptByDBRecord(const stSeptInfo & info)
{
	CSept *pSept = NULL;
	bool ret = false;

	pSept = new CSept();
	pSept->init(info);
	rwlock.wrlock();
	ret = addEntry(pSept);
	rwlock.unlock();

	if (!ret)
	{
		Zebra::logger->trace("[家族]: %s(%u) 家族添加进CSeptM管理器失败", pSept->name, pSept->id);
	}
	
	return pSept;
}

/**
* \brief 开除指定的家族成员
* \param master 行使开除权的家族成员，一般是会长
* \param member 被开除的成员
* \author fqnewman
* \return 
*/
void CSeptM::fireSeptMember(UserSession * master, const char * member)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember=NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(master->name);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	if (pSeptMember) 
	{
		CSept *pSept = pSeptMember->mySept;
		if (pSept)
		{
			pSept->fireSeptMember(master->name,member);
		}
	}
	else
	{
		master->sendSysChat(Cmd::INFO_TYPE_FAIL, "你没有加入家族");
	}
}

int CSeptM::fireSeptMember(DWORD dwUserID, bool find)
{
	struct findList : public execEntry<CSept>
	{
		DWORD _dwUserID;
		int _status;
		CSept* _pSept;

		findList(DWORD dwUserID)
		{
			_dwUserID = dwUserID;
			_status = 2;
			_pSept = NULL;
		}
		~findList(){}

		void removeSeptMember()
		{
			if (_status == 1)
			{
#ifdef _ZJW_DEBUG
				Zebra::logger->debug("%d 是族员，解除其社会关系", _dwUserID);
#endif
				if (_pSept)
				{
					_status = _pSept->fireSeptMemberDirect(_dwUserID);
				}
				else
				{
					_status = -1;
				}
			}

			if (_status == 3)
			{
				if (_pSept)
				{
					CSeptM::getMe().delSept(_pSept->id);
				}
				else
				{
					_status = -1;
				}
				
				//_status = -1;
			}
		}

		bool exec(CSept *pSept)
		{
			if (pSept)
			{
				if (pSept->master)
				{
					if (pSept->master->id == _dwUserID)
					{
#ifdef _ZJW_DEBUG
						Zebra::logger->debug("%d是族长，不能解除社会关系", _dwUserID);
#endif
						_pSept = pSept;
						_status = 3;
						return false;
					}
					else
					{
						if (pSept->isMember(_dwUserID))
						{
#ifdef _ZJW_DEBUG
							Zebra::logger->debug("%d 是 %s 族员，能解除社会关系", _dwUserID, pSept->name);
#endif
							_pSept = pSept;
							_status = 1;
							return false;
						}
						else
						{
#ifdef _ZJW_DEBUG
							Zebra::logger->debug("%d 不是 %s 的族员", _dwUserID, pSept->name);
#endif
							_status = 2;
						}
					}
				}
				else
				{
					Zebra::logger->error("%s 没有族长信息，请检查帮会信息的完整性。", pSept->name);
				}
			}

			return true;
		}
	};

	findList myList(dwUserID);
	execEveryOne(myList);

	if (!find)
	{
		myList.removeSeptMember();
	}

	return myList._status;
}

/**
* \brief 根据角色查找其所管辖的家族
* \param dwUserID 角色ID
* \author fqnewman
* \return 如果有为家族ID否则为0
*/
DWORD CSeptM::findUserSept(DWORD dwUserID)
{
	struct findList : public execEntry<CSept>
	{
		DWORD _dwUserID;
		CSept* _pSept;

		findList(DWORD dwUserID)
		{
			_dwUserID = dwUserID;
			_pSept = NULL;
		}
		~findList(){}

		bool exec(CSept *pSept)
		{
			if (pSept)
			{
				if (pSept->master)
				{
					if (pSept->master->id == _dwUserID)
					{
						_pSept = pSept;
						return false;
					}

				}
			}

			return true;
		}
	};

	findList myList(dwUserID);
	execEveryOne(myList);

	if (myList._pSept) return myList._pSept->getID();
	else return 0;
}

/**
* \brief 清除所有家族的成员
* \author zjw
*/
void CSeptM::delSeptAllMember()
{
	struct findList : public execEntry<CSept>
	{
		std::vector<CSept*> vCSept;
		
		findList()
		{
		}
		~findList(){}

		void removeAllMember()
		{
			for (std::vector<CSept*>::iterator vIter=vCSept.begin(); vIter!=vCSept.end(); vIter++)
			{
				(*vIter)->delSeptAllMember();
			}
		}
		
		bool exec(CSept *pSept)
		{
			if (pSept)
			{
				vCSept.push_back(pSept);
			}

			return true;
		}
	};

	findList myList;
	execEveryOne(myList);
	myList.removeAllMember();
}

/**
* \brief 增加新成员到家族
* \param dwSeptID 家族ID
* \param info 新成员的信息结构
* \author fqnewman
* \return true 成功 false 失败
*/
bool CSeptM::addNewMemberToSept(const DWORD dwSeptID, const stSeptMemberInfo& info)
{
	CSept * pSept = NULL;

	rwlock.rdlock();
	pSept = (CSept *)getEntryByID(dwSeptID);
	rwlock.unlock();

	if (pSept)
	{
		if (pSept->master)
		{
			/*DWORD value = pSept->master->user->level - 50;
			  value = (DWORD(value/5))*PER_LEVEL_MAN_NUM;
			  value +=10;*/
			if (pSept->size()<CREATE_SEPT_NEED_MAN_NUM)//value)
			{
				CSeptMember *pMember = pSept->addSeptMember(info);
				if (pMember)
				{
					Zebra::logger->trace("[家族]: %s 招收 %s 进入家族", pSept->name, pMember->name);
					pMember->insertDatabase();
					pMember->online(Cmd::SEPT_MEMBER_STATUS_NEWMEMBER);
					pMember->sendUserSeptData();

					UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
					if (pUser)
					{
						pUser->septid = pMember->mySept->id;

						pMember->update_data();

						if (pSept->dwUnionID>0)
						{
							CUnion* pUnion = CUnionM::getMe().getUnionByID
								(pSept->dwUnionID);

							if (pUnion)
							{
								UnionDef::stUnionMemberInfo info;
								info.dwCharID = pUser->id;
								strncpy(info.name, pUser->name, MAX_NAMESIZE);

								if (pMember->mySept && pMember->mySept->master 
										&& pMember->mySept->master->id == pUser->id)
								{       
									strncpy(info.aliasname, "族长", MAX_NAMESIZE);
								}
								else
								{       
									strncpy(info.aliasname, DEFAULTMEMBERALIAS, MAX_NAMESIZE);                                                           
								}

								SETMEMBERPOWER_WD(info.wdPower);

								info.wdOccupation = pUser->occupation;
								info.dwSeptID = pSept->id;
								CUnionM::getMe().addNewMemberToUnion(pSept->dwUnionID, info);
							}
						}
						return true;
					}
				}
			}
			else
			{
				UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pSept->master->id);
				if (pUser)	pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你目前的招收人数上限是%u人，已经满了不能再招了",CREATE_SEPT_NEED_MAN_NUM);//value);
			}
		}
		else
		{
			Zebra::logger->error("无法将成员%u加入家族%u中",info.dwCharID, dwSeptID);
		}

	}
	else
	{
		Zebra::logger->error("数据不完整家族管理器中没有%u家族，而成员%u请求加入该家族",dwSeptID,info.dwCharID);
	}
	return false;
}

/**
* \brief 用户上线处理，如果上线用户是某个家族的成员则做相应的上线初始化
* \param pUser 上线用户
* \author fqnewman
*/
void CSeptM::userOnline(UserSession * pUser)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	
	if (pSeptMember)
	{
		if (pSeptMember->mySept && pSeptMember->mySept->dwCountryID>1 && (pSeptMember->mySept->dwCountryID != pUser->country))
		{
			this->fireSeptMember(pUser->id, false);
			return;
		}

		//CDareM::getMe().userOnline(pUser);


		if (!pSeptMember->isOnline())
		{
			pSeptMember->online();

			if(pSeptMember->mySept->isVote()) return;

			pSeptMember->sendUserSeptData();
		}

		pSeptMember->update_data();
		//if ((0 == pUser->septid) && pSeptMember->user)
		if (pUser)
		{
			pUser->septid = pSeptMember->mySept->id;
			if (pSeptMember->id == pSeptMember->mySept->tempid)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "晚上七点至十点,可以带领族员,领取家族经验");
			}
		}
	}
	else
	{
		if (pUser->septid!=0)
		{
			Cmd::Session::t_fireSeptMember_SceneSession send;
			send.dwCharID = pUser->id;
			send.dwMapTempID = pUser->scene->tempid;
			pUser->scene->sendCmd(&send,sizeof(Cmd::Session::t_fireSeptMember_SceneSession)); /// 通知场景服务器
			pUser->septid = 0;
		}
	}
}

/**
* \brief 成员下线处理,将会判断指定的用户是否是家族成员，如是做下线处理
* \param pUser 下线用户
* \author fqnewman
*/
void CSeptM::userOffline(const UserSession * pUser)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	if (pSeptMember)
	{
		pSeptMember->offline();
	}
}

/**
* \brief 建立新的家族对象
* \param data 家族创建消息
* \author fqnewman
*/
void CSeptM::createNewSept(Cmd::Session::t_addSept_SceneSession *data)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(data->info.masterName);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	if (pSeptMember)
	{
		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pSeptMember->id);
		if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不能再创立家族了！");
		return;
	}

	zRTime ctv;
	data->info.dwCrTime = ctv.sec();
	if (createSeptDBRecord(data->info))  /// 家族数据库记录创建工作
	{
		/*CSept *pSept = */createSeptAndAddMaster(data->info);  /// 初始化家族管理器中的家族对象

		data->byRetcode =1;            /// 创建返回时：0 表示创建失败名称重复， 1表示成功
		SceneSession * pScene = SceneSessionManager::getInstance()->getSceneByTempID(data->dwMapTempID);
		if (pScene)
		{
			pScene->sendCmd(data,sizeof(Cmd::Session::t_addSept_SceneSession)); /// 通知场景服务器
		}
		//because vote 
		//if (pSept) pSept->sendUserSeptData(data->info.masterName);
	}
	else
	{
		data->byRetcode =0;            /// 创建返回时：0 表示创建失败名称重复， 1表示成功
		SceneSession * pScene = SceneSessionManager::getInstance()->getSceneByTempID(data->dwMapTempID);
		if (pScene)
		{
			pScene->sendCmd(data,sizeof(Cmd::Session::t_addSept_SceneSession)); /// 通知场景服务器
		}
	}
}

/**
* \brief 创建新的家族数据库记录
* \param info 新的家族结构信息
* \author fqnewman
* \return true 成功  false 失败
*/
bool CSeptM::createSeptDBRecord(stSeptInfo& info)
{
	//FunctionTimes times(7,__FUNCTION__);
	static const dbCol createsept_define[] = {
		{ "`NAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`MASTER`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`VOTE`",	zDBConnPool::DB_BYTE,	sizeof(BYTE) }, 
		{ "`CREATETIME`",zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`REPUTE`", zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`COUNTRYID`", zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`UNIONID`", zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`LEVEL`", zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`SPENDGOLD`",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`ISEXP`",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`NORMALEXPTIME`",		zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ "`NOTE`",	zDBConnPool::DB_STR,	sizeof(char[254+1]) },
		{ "`BOSSEXP`",			zDBConnPool::DB_DWORD,  sizeof(DWORD) },//家族BOSS经验值
		{ NULL, 0, 0}
	};
	struct {
		char name[MAX_NAMESIZE+1];
		DWORD charid;
		char master[MAX_NAMESIZE+1];
		BYTE vote;
		DWORD dwCrTime;
		DWORD dwRepute;
		DWORD dwCountryID;
		DWORD dwUnionID;
		DWORD dwLevel;
		DWORD dwSpendGold;
		DWORD dwIsExp;
		DWORD dwNormalExpTime;
		DWORD bossexp;//家族BOSS经验值
		char note[254+1];
	} __attribute__ ((packed))
	createsept_data;
	static const dbCol verifyname_define[] = {
		{ "`NAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ NULL, 0, 0}
	};
	
	static const dbCol verifymasterid_define[] = {
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};
	
	char strName[MAX_NAMESIZE+1];
	DWORD dwMasterID;
	char where[128];
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	//首先验证名称是否重复
	bzero(where, sizeof(where));
	std::string escapeName;
	snprintf(where, sizeof(where) - 1, "NAME = '%s'", SessionService::dbConnPool->escapeString(handle,info.name,escapeName).c_str());
	unsigned int retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`SEPT`", verifyname_define, where, NULL, 1, (BYTE *)(strName));
	if (retcode == 1)
	{
		SessionService::dbConnPool->putHandle(handle);
		return false;
	}

	retcode = 0;	
	// 验证族长是否重复
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "CHARID = '%d'", info.dwCharID);
	retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`SEPT`", verifymasterid_define, 
			where, NULL, 1, (BYTE *)&dwMasterID);

	if (retcode == 1)
	{
		SessionService::dbConnPool->putHandle(handle);
		return false;
	}

	UserSession *pUser = NULL;
	pUser = UserSessionManager::getInstance()->getUserByID(info.dwCharID);
	
	//插入数据库角色信息
	bzero(&createsept_data, sizeof(createsept_data));
	
	if (pUser) 
	{
		createsept_data.dwCountryID = pUser->country;
	}       
	else
	{
		createsept_data.dwCountryID = 0;
	}
	
	createsept_data.dwLevel = 1;
	createsept_data.dwUnionID = 0;
	createsept_data.dwRepute = info.dwRepute;
	strncpy(createsept_data.name, info.name, MAX_NAMESIZE);
	strncpy(createsept_data.master, info.masterName, MAX_NAMESIZE);
	createsept_data.vote = info.byVote;
	createsept_data.charid = info.dwCharID;
	createsept_data.dwCrTime = info.dwCrTime;
	strncpy(createsept_data.note, info.note, 255);
	createsept_data.dwIsExp = 0;
	createsept_data.dwNormalExpTime = 0;
	createsept_data.dwSpendGold = 0;
	createsept_data.bossexp = 0;//家族BOSS经验值
	retcode = SessionService::dbConnPool->exeInsert(handle, "`SEPT`", createsept_define, (const unsigned char *)(&createsept_data));
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("创建家族插入数据库出错 %u, %s", info.dwCharID, info.name);
		return false;
	}

	info.dwSeptID = retcode;
	return true;
}

/**
* \brief 添加成员索引
* \param pName 增加的成员的名称
* \param pSeptMember 成员对象
* \author fqnewman
* \return true 添加成功 false 添加失败
*/
bool CSeptM::addMemberIndex(const char *pName, CSeptMember *pSeptMember)
{
	std::pair<std::map<std::string, CSeptMember*>::iterator, bool> retval;
	char temp_name[MAX_NAMESIZE];
	bzero(temp_name, MAX_NAMESIZE);
	strncpy(temp_name, pName, MAX_NAMESIZE);

	rwlock.wrlock();
	retval = memberIndex.insert(memberIndexValueType(temp_name, pSeptMember));
	rwlock.unlock();
	
	if (!retval.second)
	{
		Zebra::logger->error("[家族]: %s 添加进memberIndex失败", pName);
		CSeptMember *pTemp = NULL;

		std::map <std::string, CSeptMember*>::iterator sIterator;
		rwlock.rdlock();
		sIterator = findMemberIndex(pName);
		if (sIterator != memberIndex.end()) pTemp = (*sIterator).second;
		rwlock.unlock();
		if (pTemp)
		{
			Zebra::logger->error("[家族]: %s 占用了 %s 的索引位置", pTemp->name, pName);
		}
		else
		{
			Zebra::logger->error("[家族]: 没有人占用,但还是添加 %s 失败了", temp_name);
		}
	}
	else
	{
#ifdef _ZJW_DEBUG		
			Zebra::logger->error("[家族]: 添加 %s 进入家族memberIndex成功", temp_name);
#endif			
	}
	
	return retval.second;
}

/**
* \brief 删除成员在索引中的记录
* \param pName 成员名称
* \author fqnewman
* \return true 删除成功 false 删除失败
*/
bool CSeptM::removeMemberIndex(const char *pName)
{
	bool ret;
	std::map <std::string, CSeptMember*>::iterator sIterator;
	rwlock.wrlock();
	ret = true;
	sIterator = findMemberIndex(pName);
	if (sIterator != memberIndex.end()) memberIndex.erase(sIterator);
	else ret = false;
	rwlock.unlock();
	return ret;
}

/**
* \brief 解散家族
* \param dwSeptID 被解散的家族的ID
* \author fqnewman
*/
void CSeptM::delSept(const DWORD dwSeptID)
{
	CSept *pSept = NULL;
	pSept = (CSept *)getEntryByID(dwSeptID);

	if (pSept)
	{
		//CDare* pDare = CDareM::getMe().findDareRecordByRelation(Cmd::SEPT_DARE, dwSeptID, 0);
		//CDare* pNpcDare = CDareM::getMe().findDareRecordByRelation(Cmd::SEPT_NPC_DARE, dwSeptID, 0);
		
		//if (pDare==NULL && pNpcDare==NULL)
		//{
			pSept->sendSeptNotify("%s 家族解散", pSept->name);
			//SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pSept->dwCountryID, 
			//		"很遗憾 %s 家族解散", pSept->name);
			
			rwlock.wrlock();
			removeEntry(pSept);
			rwlock.unlock();
			
			pSept->disbandSept();
			SAFE_DELETE(pSept);
		//}
		//else
		//{
		//	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pSept->master->id);
		//	if (pSept->master && pUser)
		//	{
		//		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "正在对战状态，不允许解散家族!");
		//	}	
		
		//}
	}
}

bool lessSeptSort(const CSeptSort& p1, const CSeptSort& p2)
{
	return p1.dwRepute > p2.dwRepute;
}

void CSeptM::processRequestSeptNormalExpMessage(UserSession* pUser, 
		const Cmd::stRequestSeptNormalExpCmd* ptCmd)
{
	CSept *pSept = NULL;
	pSept = (CSept *)getEntryByID(pUser->septid);
	struct tm tv1;
	time_t timValue = time(NULL);
	zRTime::getLocalTime(tv1, timValue);

	
	if (pSept && !pSept->isVote())
	{
		if (pSept->master && pSept->master->id == pUser->id)
		{
#ifdef _ZJW_DEBUG			
			if ((timValue - pSept->normal_exp_time)>20*60*60 && tv1.tm_hour>=9 && tv1.tm_hour<=22)
#else
			if ((timValue - pSept->normal_exp_time)>20*60*60 && tv1.tm_hour>=19 && tv1.tm_hour<=22)
#endif				
			{
				Cmd::Session::t_GetSeptNormalExp_SceneSession send;
				send.dwSeptID = pSept->id;
				send.dwUserID = pUser->id;

				if (pUser->scene)
				{
					pUser->scene->sendCmd(&send, sizeof(send));
					Zebra::logger->trace("[家族]: %s(%u) 领取 %s 普通家族经验", 
							pUser->name, pUser->id, pSept->name);
				}

				pSept->normal_exp_time = timValue;
				pSept->writeDatabase();
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "今天已领取,或不在19点到22点之间");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是族长不能领取家族经验");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是族长不能领取家族经验");
	}
}

void CSeptM::processRequestSeptExpMessage(UserSession* pUser, const Cmd::stRequestSeptExpCmd* ptCmd)
{
	CSept *pSept = NULL;
	pSept = (CSept *)getEntryByID(pUser->septid);
	
	if (pSept && !pSept->isVote())
	{
		if (pSept->master && pSept->master->id == pUser->id)
		{
			if (pSept->dwUnionID && CCityM::getMe().findByUnionID(pSept->dwUnionID) != NULL)
			{
				if (pSept->getExp())
				{
					Cmd::Session::t_GetSeptExp_SceneSession send;
					send.dwSeptID = pSept->id;
					send.dwUserID = pUser->id;
					if (pUser->scene)
					{
						pUser->scene->sendCmd(&send, sizeof(send));
						pSept->clearExp();
						Zebra::logger->trace("[家族]: %s(%u) 领取 %s 家族经验", 
								pUser->name, pUser->id, pSept->name);
					}
				}
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "今天的家族经验已经领取。");
				}
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你们家族还没加入任何帮会或所在帮会并未占领城市");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是族长不能领取家族经验");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是族长不能领取家族经验");
	}
}

void CSeptM::processSeptSortMessage(UserSession* pUser, const Cmd::stReqSeptSort* ptCmd)
{
	struct findList : public execEntry<CSept>
	{
		DWORD _dwCountry;
		DWORD _byType;
		std::vector<CSeptSort> _vSept;
		
		findList(DWORD dwCountry, BYTE byType)
		{       
			_dwCountry = dwCountry;
			_byType = byType;
		}
		~findList(){}

		void sendSeptSort(UserSession* pUser)
		{
			BYTE buf[zSocket::MAX_DATASIZE];
			Cmd::stSeptSortInfo* tempPoint;
			
			Cmd::stRtnSeptSort* retCmd = (Cmd::stRtnSeptSort*)buf;
			constructInPlace(retCmd);
			
			std::sort(_vSept.begin(), _vSept.end());
			
			tempPoint = (Cmd::stSeptSortInfo *)retCmd->data;
			retCmd->byType = _byType;

			for (DWORD i=0; i<_vSept.size(); i++)
			{
				if (i<10)
				{
					tempPoint->dwRepute = _vSept[i].dwRepute;
					strncpy(tempPoint->septName, _vSept[i].septName, MAX_NAMESIZE);
					tempPoint->dwCountryID = _vSept[i].dwCountryID;
					tempPoint++;
					retCmd->dwSize++;	
				}
				else{
					break;
				}
			}
			
			pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stSeptSortInfo)+
						sizeof(Cmd::stRtnSeptSort)));
		}
			
		
		bool exec(CSept *psept)
		{      
			if (psept && !psept->byVote && (_byType != Cmd::COUNTRY_SEPT_SORT || psept->dwCountryID==_dwCountry))
			{
				CSeptSort temp;
				strncpy(temp.septName, psept->name, MAX_NAMESIZE);

				temp.dwRepute = psept->getRepute();
				temp.dwOrder  = 0;
				temp.dwCountryID = psept->dwCountryID;
				_vSept.push_back(temp);
			}
			
			return true;
		}
	};
	
	findList myList(pUser->country, ptCmd->byType);
	execEveryOne(myList);
	myList.sendSeptSort(pUser);
}

/**
* \brief 处理成员离会
* \param dwSeptID 家族ID
* \param dwCharID 离开者的ID
* \author fqnewman
*/
void CSeptM::processMemberLeave(const DWORD dwSeptID, const DWORD dwCharID)
{
	CSept *pSept = NULL;
	pSept = (CSept *)getEntryByID(dwSeptID);
	if (pSept)
	{
		CSeptMember *pMember;
		pMember = pSept->master;
		if (pMember)
		{
			if (pMember->id != dwCharID)
			{
				pSept->fireSeptMemberDirect(dwCharID);
			}
			else
			{
				if (pSept->dwUnionID >0)
				{
					UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
					if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "你必须先退出帮会，才能解散家族");
				}
				else
				{
					delSept(dwSeptID);  // 如果是族长就解散家族
				}
			}
		}
		else
		{
			Zebra::logger->error("家族%s没有正确的会长对象，请检查数据的完整性", pSept->name);
		}
	}

}

/**
* \brief 发送家族聊天消息
* \param pUser 消息发送者
* \param pCmd 聊天消息体
* \param cmdLen 消息长度
* \author fqnewman
*/
void CSeptM::sendSeptChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *pCmd, const unsigned int cmdLen)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pCmd->pstrName);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	if (pSeptMember)
	{
		if(pSeptMember->mySept)
		{
			pSeptMember->mySept->sendCmdToAllMember(pCmd,cmdLen);
		}
	}
	else
	{
		if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "必须先加入家族才能使用家族聊天");
	}
}

/**
* \brief 发送家族私聊信息
* \param pUser 消息发送者
* \param rev 聊天消息体
* \param cmdLen 消息长度
* \author fqnewman
*/
void CSeptM::sendSeptPrivateChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *rev, const unsigned int cmdLen)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(rev->pstrName);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	if (pSeptMember)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::stChannelChatUserCmd *chatCmd;

		chatCmd = (Cmd::stChannelChatUserCmd *)buf;
		memcpy(chatCmd,rev,cmdLen);
		strncpy(chatCmd->pstrName, pUser->name,MAX_NAMESIZE);

//		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pSeptMember->id);
//		if (pUser)
//		{
			if (!pSeptMember->mySept->isVote()) pSeptMember->sendCmdToMe(chatCmd, cmdLen);
//		}
//		else
//		{
//			COfflineMessage::writeOfflineMessage(chatCmd->dwType, pSeptMember->id, chatCmd,cmdLen);
//		}
	}
}

/**
* \brief 处理 Gateway 转发过来的客户端消息
* \param pUser 消息接收者
* \param ptNullCmd 消息函数
* \param cmdLen 消息长度
* \author fqnewman
* \return true 处理成功 false 消息不在处理范围之内
*/
bool CSeptM::processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch(ptNullCmd->byCmd)
	{
		case Cmd::SEPT_USERCMD:
			{
				switch(ptNullCmd->byParam)
				{
					case Cmd::REQUEST_SEPT_NORMAL_EXP_PARA:
						{
							this->processRequestSeptNormalExpMessage(pUser, 
									(Cmd::stRequestSeptNormalExpCmd*)ptNullCmd);
							return true;
						}
						break;
					case Cmd::REQ_SEPT_NAME_LIST_PARA:
						{
							//FunctionTimes times(109, __FUNCTION__);
							Cmd::stReqSeptNameListCmd* rev = (Cmd::stReqSeptNameListCmd*)ptNullCmd;
							char buffer[zSocket::MAX_DATASIZE];
							Cmd::stRtnSeptNameListCmd *send = (Cmd::stRtnSeptNameListCmd*)buffer;
							constructInPlace(send);
							int i=0;
							while(i < rev->num && i <= 200)
							{
								CSept* pSept = CSeptM::getMe().getSeptByID(rev->dwSeptID[i]);
								if (pSept)
								{
#ifdef _ZJW_DEBUG
									Zebra::logger->debug("[优化]: %s 请求 %s 家族名称", pUser->name, pSept->name);
#endif								

									strncpy(send->list[send->num].name, pSept->name, MAX_NAMESIZE);
									send->list[send->num].dwSeptID = pSept->id;
									send->num++;
								}
								i ++;
							}
							if(send->num)
							{
								pUser->sendCmdToMe(send, sizeof(Cmd::stRtnSeptNameListCmd) + (send->num * sizeof(send->list[0])));
							}
							
							return true;
						}
						break;
					case Cmd::REQ_SEPT_NAME_PARA:
						{
							Cmd::stReqSeptNameCmd* rev = (Cmd::stReqSeptNameCmd*)ptNullCmd;
							CSept* pSept = CSeptM::getMe().getSeptByID(rev->dwSeptID);
							Cmd::stRtnSeptNameCmd send;
							//FunctionTimes times(106, __FUNCTION__);
							
							if (pSept)
							{
#ifdef _ZJW_DEBUG
								Zebra::logger->debug("[优化]: %s 请求 %s 家族名称", pUser->name, pSept->name);
#endif								

								strncpy(send.name, pSept->name, MAX_NAMESIZE);
								send.dwSeptID = rev->dwSeptID;
								pUser->sendCmdToMe(&send, sizeof(send));
							}
							
							return true;
						}
						break;
					case Cmd::REQUEST_ABJURATION_SEPTNPC_PARA:
						{
							CSept* pSept = CSeptM::getMe().getSeptByID(pUser->septid);
							if (pSept && pSept->tempid == pUser->id)
							{
								CNpcDareObj *npcdare = CNpcDareM::getMe().searchSeptHold(pUser->septid);
								if (npcdare)
								{
									npcdare->abandon_npc();
								}
								else
								{
									pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
										"贵家族还未占领任何商人");
								}
							}
							else
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
										"你不是族长，不能放弃");
							}
							
							return true;
						}
						break;
					case Cmd::REQUEST_SEPT_EXP_PARA:
						{
							Cmd::stRequestSeptExpCmd* ptCmd = 
								(Cmd::stRequestSeptExpCmd*)ptNullCmd;
							this->processRequestSeptExpMessage(pUser, ptCmd);
							return true;
						}
						break;
					case Cmd::REQ_SEPT_SORT_PARA:
						{
							Cmd::stReqSeptSort* ptCmd = 
								(Cmd::stReqSeptSort*)ptNullCmd;
							this->processSeptSortMessage(pUser, ptCmd);
							return true;
						}
						break;
					case Cmd::CHANGE_SEPT_MEMBER_ALIASNAME_PARA:
						{
							Cmd::stChangeSeptMemberAliasName* ptCmd = 
								(Cmd::stChangeSeptMemberAliasName*)ptNullCmd;
							
							change_aliasname(pUser, ptCmd);
							return true;
						}
						break;
					case Cmd::FIRE_MEMBER_FROM_SEPT_PARA:
						{
							Cmd::stFireMemberFromSeptCmd *ptCmd=(Cmd::stFireMemberFromSeptCmd *)ptNullCmd;
							fireSeptMember(pUser,ptCmd->memberName);
							return true;
						}
						break;
					case Cmd::SEPTMEMBER_LEAVE_SEPT_PARA:
						{
							//sky 国战离开家族判断
							//if (Cmd::Session::COUNTRY_FORMAL_DARE  || Cmd::Session::COUNTRY_FORMAL_ANTI_DARE || Cmd::Session::EMPEROR_DARE )
							//if (pUser->isSpecWar(Cmd::COUNTRY_FORMAL_DARE))
							CDare* pDare = CDareM::getMe().findDareRecordByID(
							Cmd::COUNTRY_FORMAL_DARE, pUser->country);
							if(pDare)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "国战期间不能离开家族!");
								return false;
							}
							else
							{
								processMemberLeave(pUser->septid, pUser->id);
								return true;
							}
						}
						break;
					case Cmd::LIST_SEPT_PARA:
						{
							sendListToUser(pUser);
							return true;
						}
						break;
					case Cmd::VOTELIST_SEPT_PARA:
						{
							/*if (pUser->level<20)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你等级不够20级不能参与投票");
								return true;
							}*/

							sendVoteListToUser(pUser);
							return true;
						}
						break;
					case Cmd::REQUEST_NPCINFO_SEPT_PARA:
						{
							if (pUser)
							{
								CNpcDareObj *npcdare = CNpcDareM::getMe().searchSeptHold(pUser->septid);
								if (npcdare)
								{
									SceneSession *scene= SceneSessionManager::getInstance()->getSceneByID((npcdare->get_country()<<16)+npcdare->get_mapid());
									if (scene)
										pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "你的家族现在控制着%s(%u,%u)处的商人！", scene->name, npcdare->get_posx(), npcdare->get_posy());
									else
										pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "无法核查你的家族控制的商人在那里");
								}
								else
								{
									pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "你的家族没有控制的商人");
								}
							}
							return true;
						}
						break;
					case Cmd::REQUEST_JOIN_SEPT_PARA:
						{
							Cmd::stRequestJoinSeptCmd *ptCmd=(Cmd::stRequestJoinSeptCmd *)ptNullCmd;
							switch(ptCmd->status)
							{
								case Cmd::REQUEST_JOIN_OK:
									{
										UserSession *master = UserSessionManager::getInstance()->getUserSessionByName(ptCmd->name);
										if (master)
										{
											strncpy(ptCmd->name, pUser->name, MAX_NAMESIZE);
											master->sendCmdToMe(ptCmd, sizeof(Cmd::stRequestJoinSeptCmd));
										}
										else
										{
											pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "族长%s目前不在线", ptCmd->name);
										}
									}
									break;
								case Cmd::REQUEST_JOIN_CANCEL:
								case Cmd::REQUEST_JOIN_TIMEOUT:
								default:
									{
										UserSession *tUser = UserSessionManager::getInstance()->getUserSessionByName(ptCmd->name);
										if (tUser)
										{
											strncpy(ptCmd->name, pUser->name, MAX_NAMESIZE);
											tUser->sendCmdToMe(ptCmd, sizeof(Cmd::stRequestJoinSeptCmd));
										}
									}
									break;
							}
						}
						break;
					case Cmd::VOTE_SEPT_PARA:
						{
							if (pUser->level<20)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你等级不够20级不能参与签名");
								return true;
							}

							Cmd::stVoteSeptCmd *ptCmd=(Cmd::stVoteSeptCmd *)ptNullCmd;
							if (ptCmd->bySuccess)
							{
								userVote(pUser, ptCmd->septName);
							}
							else
							{
								userAboutVote(pUser, ptCmd->septName);
							}
							return true;
						}
						break;
					case Cmd::NOTE_SEPT_PARA:
						{
							Cmd::stNoteSeptCmd *ptCmd=(Cmd::stNoteSeptCmd *)ptNullCmd;
							setSeptNote(pUser, ptCmd);
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
	return false;
}

bool CSeptM::processSceneSeptMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch(ptNullCmd->para)
	{
		case Cmd::Session::OP_LEVEL_PARA:
			{
				Cmd::Session::t_OpLevel_SceneSession* ptCmd=
				(Cmd::Session::t_OpLevel_SceneSession*)ptNullCmd;

				this->changeLevel(ptCmd->dwSeptID, ptCmd->dwLevel);
				return true;
			}
			break;
		case Cmd::Session::OP_REPUTE_PARA:
			{
				Cmd::Session::t_OpRepute_SceneSession* ptCmd=
					(Cmd::Session::t_OpRepute_SceneSession*)ptNullCmd;

				CSept* pSept = CSeptM::getMe().getSeptByID(ptCmd->dwSeptID);
				
				if (pSept)					
				{
					pSept->changeRepute(ptCmd->dwRepute);
				}

				return true;
			}
			break;
		default:
			break;
	}

	return false;
}
/**
* \brief 处理从场景过来的消息
* \param cmd 消息体
* \param cmdLen 消息长度
* \author fqnewman
* \return true 处理成功 false 消息不在处理范围之内
*/
bool CSeptM::processSceneMessage(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	switch(cmd->para)
	{
		case Cmd::Session::PARA_SEPT_ADDMEMBER:
			{
				Cmd::Session::t_addSeptMember_SceneSession *ptCmd=(Cmd::Session::t_addSeptMember_SceneSession *)cmd;
				std::map<std::string, CSeptMember *>::iterator sIterator;
			        CSeptMember *pSeptMember = NULL;
						                        
			        rwlock.rdlock();        
			        sIterator = findMemberIndex(ptCmd->member.name);
			        if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
			        rwlock.unlock();
				
				CSept * pSept = NULL;

				rwlock.rdlock();
				pSept = (CSept *)getEntryByID(ptCmd->dwSeptID);
				rwlock.unlock();
				if (pSept == NULL)
				{
					Zebra::logger->error("[家族]:招收新成员，但未找到指定的家族");
					return true;
				}

				if (pSeptMember != NULL)
				{
					/*if (pSeptMember->mySept && pSeptMember->user && pSeptMember->mySept->isVote())
					{
						userAboutVote(pSeptMember->user, pSeptMember->mySept->name);
					}*/
					if (!pSept || !pSept->master)
					{
						return true;
					}
					UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pSept->master->id);
					if (pSept && pSept->master && pUser)
					{

						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方已加入家族，不能招收");
					}

					pUser = UserSessionManager::getInstance()->getUserByID(pSeptMember->id);
					if (pSeptMember && pUser)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
								"你已加入家族，需放弃后才能加入另外的家族");
					}
				}
				else
				{
					if (!pSept || !pSept->master)
					{
						return true;
					}
						
					UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pSept->master->id);
					if (pSept && pSept->master && pUser)	
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "%s 接受了你的邀请加入了家族", ptCmd->member.name);
					}

					if (addNewMemberToSept(ptCmd->dwSeptID, ptCmd->member))
					{
						if (pSept) pSept->sendSeptNotify("欢迎 %s 加入家族", ptCmd->member.name);
					}
				}
					
				return true;
			}
			break;
		case Cmd::Session::PARA_SEPT_ADDSEPT:
			{
				Cmd::Session::t_addSept_SceneSession *ptCmd=(Cmd::Session::t_addSept_SceneSession *)cmd;
				createNewSept(ptCmd);
				return true;
			}
			break;
		case Cmd::Session::PARA_SEPT_EXP_DISTRIBUTE:
			{
				Cmd::Session::t_distributeSeptExp_SceneSession *ptCmd=(Cmd::Session::t_distributeSeptExp_SceneSession *)cmd;
				disributeExp(ptCmd);
				return true;
			}
			break;
		default:
			break;
	}

	return false;
}

/**
* \brief 处理从Gateway过来的消息
* \param cmd 消息体
* \param cmdLen 消息长度
* \author fqnewman
* \return true 处理成功 false 消息不在处理范围之内
*/
bool CSeptM::processGateMessage(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	switch(cmd->para)
	{
		case Cmd::Session::PARA_SEPT_DISBAND:
			{
				Cmd::Session::t_disbandSept_GateSession *ptCmd=(Cmd::Session::t_disbandSept_GateSession *)cmd;
				processMemberLeave(ptCmd->dwSeptID, ptCmd->dwCharID);
				return true;
			}
			break;
		default:
			break;
	}

	return false;
}

/**
* \brief 发送当前系统中在等待投票的家族列表给指定的用户
* \param pUser 请求投票列表者
* \author fqnewman
*/
void CSeptM::sendVoteListToUser(const UserSession *pUser)
{/*
	struct findList : public execEntry<CSept>
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::stVoteListSeptCmd *retCmd;
		Cmd::stSeptVoteRecord *tempPoint;
		std::vector<DWORD> removed;
		UserSession *user;
		findList(char *pName, UserSession *pUser)
		{
			user = pUser;
			retCmd=(Cmd::stVoteListSeptCmd *)buf;
			constructInPlace(retCmd);
			tempPoint = (Cmd::stSeptVoteRecord *)retCmd->data;
			retCmd->dwSize = 0;
			retCmd->flag =0;

			strncpy(retCmd->septName, pName, MAX_NAMESIZE);
		}
		~findList(){}

		void sendList(const UserSession *pUser)
		{
			pUser->sendCmdToMe(retCmd,(retCmd->dwSize*sizeof(Cmd::stSeptVoteRecord)+sizeof(Cmd::stVoteListSeptCmd)));
		}

		void removeInvalidSept()
		{
			std::vector<DWORD>::iterator tIterator;

			for(tIterator = removed.begin(); tIterator != removed.end(); tIterator++)
			{
				CSeptM::getMe().delSept(*tIterator);
			}
		}

		bool exec(CSept *psept)
		{
			if (psept&&psept->isVote())
			{
				zRTime ctv;
				if (ctv.sec() - psept->dwCreateTime > DESTROYTIME)
				{
					removed.push_back(psept->id);
					return true;
				}

				if (retCmd->dwSize>100)
				{
					this->sendList(user);
					char tName[MAX_NAMESIZE];
					strncpy(tName, retCmd->septName, MAX_NAMESIZE);
					tempPoint = (Cmd::stSeptVoteRecord *)retCmd->data;
					retCmd->dwSize = 0;
					retCmd->flag=1;
				}

				
				if (user && user->country == psept->dwCountryID)
				{
					strncpy(tempPoint->septName,psept->name,MAX_NAMESIZE);
					if (psept->master)
						strncpy(tempPoint->master, psept->master->name,MAX_NAMESIZE);
					else
						strncpy(tempPoint->master, "未知",MAX_NAMESIZE);
					strncpy(tempPoint->note, psept->note,255);
					tempPoint->wdVoteNumber = psept->size();
					tempPoint++;
					retCmd->dwSize++;
				}
			}
			return true;
		}
	};

	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;
	char tempName[MAX_NAMESIZE];

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	bzero(tempName, MAX_NAMESIZE);
	if (pSeptMember)
	{
		if (pSeptMember->mySept->isVote())
		{
			strncpy(tempName, pSeptMember->mySept->name, MAX_NAMESIZE);
		}
	}



	findList myList(tempName, (UserSession *)pUser);
	execEveryOne(myList);
	myList.sendList(pUser);
	myList.removeInvalidSept();
	*/
}


/**
* \brief 发送当前系统中的家族列表给指定的用户
* \param pUser 请求列表者
* \author fqnewman
*/
void CSeptM::sendListToUser(const UserSession *pUser)
{
	struct findList : public execEntry<CSept>
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::stListSeptCmd *retCmd;
		Cmd::stSeptRecord *tempPoint;
		UserSession *user;
		findList(UserSession *pUser)
		{
			user = pUser;
			retCmd=(Cmd::stListSeptCmd *)buf;
			constructInPlace(retCmd);
			tempPoint = (Cmd::stSeptRecord *)retCmd->data;
			retCmd->dwSize = 0;
			retCmd->flag =0;
		}
		~findList(){}

		void sendList(const UserSession *pUser)
		{
			pUser->sendCmdToMe(retCmd,(retCmd->dwSize*sizeof(Cmd::stSeptRecord)+sizeof(Cmd::stListSeptCmd)));
		}

		bool exec(CSept *psept)
		{
			if (psept)
			{
				if (retCmd->dwSize>100)
				{
					this->sendList(user);
					tempPoint = (Cmd::stSeptRecord *)retCmd->data;
					retCmd->dwSize = 0;
					retCmd->flag=1;
				}

				
				if (user && user->country == psept->dwCountryID)
				{
					strncpy(tempPoint->septName,psept->name,MAX_NAMESIZE);
					if (psept->master)
						strncpy(tempPoint->master, psept->master->name,MAX_NAMESIZE);
					else
						strncpy(tempPoint->master, "未知",MAX_NAMESIZE);
					strncpy(tempPoint->note, psept->note,255);
					//tempPoint->wdVoteNumber = psept->size();
					tempPoint++;
					retCmd->dwSize++;
				}
			}
			return true;
		}
	};

	findList myList((UserSession *)pUser);
	execEveryOne(myList);
	myList.sendList(pUser);
}

/**
 * \brief 发送当前系统中可以被挑战的帮会列表
 * \param pUser 请求投票者
 * \author zjw
 */
void CSeptM::sendDareListToUser(const UserSession *pUser)
{
	struct findList : public execEntry<CSept>
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::stSendDareList *send;
		Cmd::stDareList* tempPoint;
		const UserSession* _pUser;

		findList(const UserSession* pUser)
		{       
			send = (Cmd::stSendDareList *)buf;
			constructInPlace(send);
			tempPoint = (Cmd::stDareList *)send->dare_list;
			send->dwSize = 0;

			send->byType = Cmd::SEPT_DARE;
			_pUser = pUser;
		}
		~findList(){}

		void sendList()
		{       
			if (_pUser) _pUser->sendCmdToMe(send,(send->dwSize*sizeof(Cmd::stDareList)+sizeof(Cmd::stSendDareList)));
		}

		bool exec(CSept *psept)
		{      
			if (_pUser)	
			{
				if (psept && psept->master)
				{   
					UserSession *pUser = UserSessionManager::getInstance()->getUserByID(psept->master->id);
					if (pUser && pUser->country==_pUser->country
							&& psept->id!=_pUser->septid)
					{
						strncpy(tempPoint->name, psept->name, MAX_NAMESIZE);
						tempPoint++;
						send->dwSize++;
					}
				}
			}

			return true;
		}
	};


	findList myList(pUser);
	execEveryOne(myList);
	myList.sendList();
}
/**
* \brief 用户投票给指定的家族
* \param pUser 投票者
* \param pName 家族名称
* \author fqnewman
*/
void CSeptM::userVote(const UserSession *pUser, const char *pName)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	if (pSeptMember)
	{

		if (pUser)
		{
			if (pSeptMember->mySept->isVote())
			{
				if (pSeptMember == pSeptMember->mySept->master)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你是%s族的预备族长不能再签名", pSeptMember->mySept->name);
				}
				else
				{
					if (strncmp(pName,pSeptMember->mySept->name,MAX_NAMESIZE)!=0)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已对%s族签名，你之前对%s族的签名将被作废.",pName, pSeptMember->mySept->name);
						processMemberLeave(pSeptMember->mySept->id, pUser->id);
						addVoteMemberToSept(pUser,pName);
						return;
					}
				}
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已经加入其他家族了，没有签名的资格");
			}
		}
		else
		{
			Zebra::logger->error("CSeptM::userVote():一个在线的家族成员没有有效的用户指针");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你对%s族进行了签名", pName);
		addVoteMemberToSept(pUser,pName);
		return;
	}

	// 通知投票失败
	Cmd::stVoteSeptCmd send;
	strncpy(send.septName,pName,MAX_NAMESIZE);
	send.bySuccess = 0;
	pUser->sendCmdToMe(&send, sizeof(send));
}

/**
* \brief 用户放弃投票
* \param pUser 放弃投票的成员
* \param pName 被放弃家族名称
* \author fqnewman
* \return 
*/
void CSeptM::userAboutVote(const UserSession *pUser, const char *pName)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	if (pSeptMember)
	{

		if (pUser)
		{
			if (pSeptMember->mySept->isVote())
			{
				if (pSeptMember == pSeptMember->mySept->master)
				{
					
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你是%s族的预备族长,放弃签名，家族解散", 
							pSeptMember->mySept->name);

					processMemberLeave(pSeptMember->mySept->id, pUser->id);
					sendVoteListToUser(pUser);

					return;
				}
				else
				{
					if (strncmp(pName,pSeptMember->mySept->name,MAX_NAMESIZE)==0)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你放弃了%s族的票.",pName);
						processMemberLeave(pSeptMember->mySept->id, pUser->id);
						Cmd::stVoteSeptCmd send;
						strncpy(send.septName,pName,MAX_NAMESIZE);
						send.bySuccess = 2;
						pUser->sendCmdToMe(&send, sizeof(send));
						return;
					}
				}
			}
			else
			{
				Zebra::logger->error("[%s]家族已经成立，但是签名还没取消",pName);
			}
		}
		else
		{
			Zebra::logger->error("CSeptM::userVote():一个在线的家族成员没有有效的用户指针");
		}
	}

	// 通知投票失败
	Cmd::stVoteSeptCmd send;
	strncpy(send.septName,pName,MAX_NAMESIZE);
	send.bySuccess = 0;
	pUser->sendCmdToMe(&send, sizeof(send));
}

/**
* \brief 投票给指定的家族
* \param pUser 投票者
* \param pName 家族名称
* \author fqnewman
*/
void CSeptM::addVoteMemberToSept(const UserSession *pUser, const char *pName)
{
	rwlock.rdlock();
	CSept *mySept = (CSept *)getEntryByName(pName);
	rwlock.unlock();

	if (mySept)
	{
		if (mySept->isVote())
		{
			stSeptMemberInfo member;
			member.dwCharID = pUser->id;
			strncpy(member.name, pUser->name, MAX_NAMESIZE);
			member.wdOccupation = pUser->occupation;
			addNewMemberToSept(mySept->id, member);

			Cmd::stVoteSeptCmd send;
			strncpy(send.septName,pName,MAX_NAMESIZE);
			send.bySuccess = 1;
			pUser->sendCmdToMe(&send, sizeof(send));

			if (mySept->size()>=CREATE_SEPT_NEED_MAN_NUM)//10
			{
				mySept->letVoteOver();
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s族的签名期已经结束，请直接去找族长协商有关入会事宜.", pName);
			Cmd::stVoteSeptCmd send;
			strncpy(send.septName,pName,MAX_NAMESIZE);
			send.bySuccess = 0;
			pUser->sendCmdToMe(&send, sizeof(send));
		}
	}
}

/**
* \brief 家族内分配经验
* \param cmd 家族经验分配消息
* \author fqnewman
*/
void CSeptM::disributeExp(Cmd::Session::t_distributeSeptExp_SceneSession *cmd)
{
	UserSession *pUser=UserSessionManager::getInstance()->getUserByID(cmd->dwUserID);
	if (!pUser) return;

	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	if (pSeptMember)
	{
		CSept *mySept = pSeptMember->mySept;
		if (mySept)
		{
			mySept->sendDistributeSeptExpToScene(cmd->dwUserID, cmd);
		}
	}
}

/**
* \brief 根据名称获取家族对象
* \param name 家族的名称
* \author fqnewman
* \return 成功返回家族对象 失败返回NULL
*/
CSept * CSeptM::getSeptByName( const char * name)
{
	rwlock.rdlock();
	CSept *ret =(CSept *)getEntryByName(name);
	rwlock.unlock();
	return ret;
}

/**
* \brief 根据ID获取家族对象
* \param dwSeptID 家族的ID
* \author fqnewman
* \return 成功返回家族对象，失败返回 NULL
*/
CSept* CSeptM::getSeptByID(DWORD dwSeptID)
{
	rwlock.rdlock();
	CSept* ret = (CSept*)getEntryByID(dwSeptID);
	rwlock.unlock();
	return ret;
}

/**
* \brief 设置家族介绍
* \param pUser 设置者的 UserSession对象
* \param pCmd 介绍设置消息
* \author fqnewman
*/
void CSeptM::setSeptNote(UserSession *pUser, Cmd::stNoteSeptCmd *pCmd)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	if (pSeptMember)
	{
		CSept *mySept = pSeptMember->mySept;
		if (mySept && mySept->master == pSeptMember)
		{
			mySept->setNote(pCmd);
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你无法设置家族介绍");
		}
	}
}

void CSeptM::change_aliasname(UserSession* pUser, Cmd::stChangeSeptMemberAliasName* pCmd)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember=NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	
	if (pSeptMember)
	{
		CSept* pSept = pSeptMember->mySept;
		if (pSept && pSept->master == pSeptMember)
		{
			pSeptMember = NULL;
			rwlock.rdlock();
			sIterator = findMemberIndex(pCmd->name);
			if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
			rwlock.unlock();

			if (pSeptMember && pSeptMember->mySept == pSept)
			{
				pSeptMember->change_aliasname(pCmd->aliasname);
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你无权更改称号");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不属于任何家族");
	}

}

void CSeptM::changeAllRepute(DWORD countryid, int repute)
{
	FunctionTime func_time(0,__PRETTY_FUNCTION__,"遍历所有家族扣减声望所需时间" , 32);

	rwlock.rdlock();
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CSept *pSept =(CSept*)it->second;
		if (pSept && pSept->dwCountryID == countryid)
		{
			pSept->changeRepute(repute);
		}
	}
	rwlock.unlock();
}

DWORD CSeptM::getRepute(DWORD dwSeptID)
{
	CSept* pSept = this->getSeptByID(dwSeptID);
	if (pSept)
		return pSept->getRepute();

	return 0;
}

void CSeptM::changeRepute(DWORD dwSeptID, int repute)
{
	CSept* pSept = this->getSeptByID(dwSeptID);
	if (pSept)
		return pSept->changeRepute(repute);
}

void CSeptM::changeLevel(DWORD dwSeptID, int level)
{
	CSept* pSept = this->getSeptByID(dwSeptID);
	if (pSept)
		return pSept->changeLevel(level);
}
/**
 * \brief 根据角色名字获取其所属帮派名字
 *
 *
 * \param Name 角色名称
 * \return 返回帮会名称或者NULL
 */
char * CSeptM::getSeptNameByUserName(char *Name)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(Name);

	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	if (pSeptMember)
	{
		if (pSeptMember->mySept)
			return pSeptMember->mySept->name;
	}
	return NULL;
}

/**
 * \brief 根据角色名字获取其所属家族ID
 *
 *
 * \param Name 角色名称
 * \return 返回帮会名称或者NULL
 */
DWORD CSeptM::getSeptIDByUserName(char *Name)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(Name);

	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	if (pSeptMember)
	{
		if (pSeptMember->mySept)
			return pSeptMember->mySept->id;
	}
	return 0;
}

/**
 * \brief 根据家族ID来通知指定家族的NPC控制数据
 * \param septid 家族ID
 */
void CSeptM::notifyNpcHoldData(DWORD septid)
{
	CSept* pSept = this->getSeptByID(septid);
	if (pSept)
		return pSept->notifyNpcHoldData();
}

CSeptMember * CSeptM::getMemberByName(const char * pName)
{
	std::map<std::string, CSeptMember *>::iterator sIterator;
	CSeptMember *pSeptMember=NULL;

	rwlock.rdlock();
	sIterator = this->findMemberIndex(pName);
	if (sIterator != memberIndex.end()) pSeptMember = (*sIterator).second;
	rwlock.unlock();
	return pSeptMember;
}

