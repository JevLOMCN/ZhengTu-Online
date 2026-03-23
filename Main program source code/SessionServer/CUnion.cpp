/**
 * \file
 * \version  $Id: CUnion.cpp  $
 * \author  
 * \date  
 * \brief 实现帮会管理功能
 *
 */

#include <stdarg.h>
#include "zRWLock.h"
#include "Union.h"
#include "CUnion.h"
#include "Command.h"
#include "zDBConnPool.h"
#include "SessionServer.h"
#include "Session.h"
#include "OfflineMessage.h"
#include "SessionCommand.h"
#include "zMetaData.h"
#include "CCityManager.h"
#include "CCountryManager.h"
#include "CSept.h"
#include "CDare.h"
#include "SessionChat.h"

using namespace UnionDef;

class CUnionSort 
{               
	public: 
		char  unionName[MAX_NAMESIZE];           // 帮会名称
		DWORD dwMana;                         // 帮会威望
		DWORD dwOrder;                          // 帮会排名
		DWORD dwCountryID;                      // 所属国家

		CUnionSort()
		{               
			dwMana = 0;
			dwOrder  = 0;
			dwCountryID = 0;
			bzero(unionName, MAX_NAMESIZE);
		}

		CUnionSort(const CUnionSort& ref)
		{
			dwMana = ref.dwMana;
			dwOrder = ref.dwOrder;
			dwCountryID = ref.dwCountryID;      
			strncpy(unionName, ref.unionName, MAX_NAMESIZE);
		}

		~CUnionSort()
		{
		}

		CUnionSort& operator =(const CUnionSort& ref)
		{       
			this->dwMana = ref.dwMana;
			this->dwOrder = ref.dwOrder;
			this->dwCountryID = ref.dwCountryID;
			strncpy(this->unionName, ref.unionName, MAX_NAMESIZE);
			return *this;
		}

		friend bool operator<(const CUnionSort& lhs, const CUnionSort& rhs)
		{       
			return lhs.dwMana > rhs.dwMana;
		}
};


//==[CUnionMember]==================================================

/**
* \brief 帮会成员构造函数，初始化基本属性
* \author fqnewman
*/
CUnionMember::CUnionMember()
{
	destroy  = false;
	//user = NULL;
}

/**
* \brief 帮会成员初始化
* \param info 成员信息结构
* \author fqnewman
*/
void CUnionMember::init(const stUnionMemberInfo& info)
{
	rwlock.wrlock();
	byPower[0] = 0x00;
	byPower[1] = 0x00;
	id = info.dwCharID;
	strncpy(name,info.name,MAX_NAMESIZE);
	strncpy(aliasname,info.aliasname,MAX_NAMESIZE);
	WD2BA(info.wdPower, byPower);
	wdOccupation = info.wdOccupation;
	septid = info.dwSeptID;
	byStatus = CUnionMember::Offline;                  // 会员状态
	rwlock.unlock();
}

/**
* \brief 获取成员的基本信息
* \param info 返回的信息结构
* \author fqnewman,zjw
*/
void CUnionMember::getMemberBaseInfo(struct Cmd::stUnionRember& info)
{
	rwlock.rdlock();
	strncpy(info.memberName,name,MAX_NAMESIZE);
	strncpy(info.aliasname,aliasname,MAX_NAMESIZE);
	info.byOnline = byStatus;
	
	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
	if (pUser)
	{
		if (pUser->occupation != this->wdOccupation)
		{
			this->wdOccupation = pUser->occupation;
			this->writeDatabase();
		}
		
		info.level = pUser->level;
		info.exploit = pUser->dwExploit;
	}
	else
	{
		info.level = 0;
		info.exploit = 0;
	}
	
	info.occupation = this->wdOccupation;
	rwlock.unlock();
}

/**
* \brief 发送成员的帮会数据给客户端
* \author fqnewman
*/
void CUnionMember::sendUserUnionPrivateData()
{
	Cmd::stReturnUnionMemberInfoCmd ret;

	rwlock.rdlock();
	strncpy(ret.alias,aliasname,MAX_NAMESIZE);     // 别名
	ret.byPower[0]=byPower[0];
	ret.byPower[1]=byPower[1];
	rwlock.unlock();

	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
	if (pUser) pUser->sendCmdToMe(&ret,sizeof(ret));
}

/**
* \brief 发送消息给成员对应的客户端
* \param  pstrCmd 消息体
* \param  nCmdLen 消息长度
* \author fqnewman
*/
void CUnionMember::sendCmdToMe(const Cmd::stNullUserCmd *pstrCmd, const unsigned int nCmdLen)
{
	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
	if (pUser)
	{
		if ((Cmd::CHAT_USERCMD == pstrCmd->byCmd) && 
				(ALL_CHAT_USERCMD_PARAMETER == pstrCmd->byParam))
		{

#ifdef _ZJW_DEBJG
			Zebra::logger->debug("发送帮会公告给:%s", pUser->name);
#endif	
		}	
		pUser->sendCmdToMe(pstrCmd,nCmdLen);
	}
	else
	{
		if ((Cmd::CHAT_USERCMD == pstrCmd->byCmd) && 
				(ALL_CHAT_USERCMD_PARAMETER == pstrCmd->byParam))
		{
			Cmd::stChannelChatUserCmd *pCmd = (Cmd::stChannelChatUserCmd *)pstrCmd;
			if (Cmd::CHAT_TYPE_UNION_AFFICHE == pCmd->dwType)
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
void CUnionMember::writeDatabase()
{
	static const dbCol unionmember_define[] = {
		{ "`ALIASNAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`POWER`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ "`OCCUPATION`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ NULL, 0, 0}
	};

	struct {
		char  aliasname[MAX_NAMESIZE+1]; 		// 会员别名
		WORD  wdPower;                			// 会员权限
		WORD  wdOccupation;						// 会员的职业
	} __attribute__ ((packed))
	updateunionmember_data;
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}

	rwlock.rdlock();

	WORD wdValue;
	BA2WD(byPower, wdValue);
	updateunionmember_data.wdPower = wdValue;
	strncpy(updateunionmember_data.aliasname, aliasname, MAX_NAMESIZE);
	updateunionmember_data.wdOccupation = wdOccupation;

	rwlock.unlock();

	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "UNIONID = %u AND CHARID = %u", myUnion->getID(), id);
	unsigned int retcode = SessionService::dbConnPool->exeUpdate(handle, "`UNIONMEMBER`", unionmember_define, (BYTE *)(&updateunionmember_data), where);
	SessionService::dbConnPool->putHandle(handle);

	if ((DWORD)-1 == retcode)
	{
		Zebra::logger->error("CUnionMember 修改会员档案失败：UNIONID=%u CHARID=%u retcode=%u", myUnion->getID(), id, retcode);
	}
}

/**
* \brief 将成员记录插入数据库
* \author fqnewman
*/
void CUnionMember::insertDatabase()
{
	static const dbCol unionmember_define[] = {
		{ "`UNIONID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`NAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`ALIASNAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`POWER`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ "`OCCUPATION`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ "`SEPTID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};
	struct {
		DWORD dwUnionID;						// 帮会编号
		DWORD	dwCharID;						// 会员角色ID
		char  name[MAX_NAMESIZE+1];           // 会员名称
		char  aliasname[MAX_NAMESIZE+1]; 		// 会员别名
		WORD  wdPower;                		// 会员权限
		WORD  wdOccupation;						// 会员角色
		DWORD dwSeptID;
	} __attribute__ ((packed))
	createunionmember_data;

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}

	//插入数据库角色信息
	bzero(&createunionmember_data, sizeof(createunionmember_data));

	rwlock.rdlock();

	createunionmember_data.dwUnionID = myUnion->getID();
	createunionmember_data.dwCharID = id;
	strncpy(createunionmember_data.name, name, MAX_NAMESIZE);
	strncpy(createunionmember_data.aliasname, aliasname, MAX_NAMESIZE);
	WORD wdValue;
	BA2WD(byPower, wdValue);
	createunionmember_data.wdPower = wdValue;
	createunionmember_data.wdOccupation = wdOccupation;
	createunionmember_data.dwSeptID = this->septid;
	
	rwlock.unlock();

	unsigned int retcode = SessionService::dbConnPool->exeInsert(handle, "`UNIONMEMBER`", unionmember_define, (const unsigned char *)(&createunionmember_data));
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("插入帮会成员数据库出错 %u %u, %s", myUnion->getID(), id, aliasname);
	}
	return;
}

/**
 * \brief 更新数据到场景
 *
 *
 * \param 
 * \return 
 */
void CUnionMember::update_data()
{
	Cmd::Session::t_sendUserRelationID send;
	send.dwUserID = this->id;
	send.dwID = this->myUnion->id;
	send.type = Cmd::Session::RELATION_TYPE_UNION;

	bzero(send.name, sizeof(send.name));
	send.caption = 0;
	send.unionmaster = false;
	send.septmaster = false;
	send.king = false;
	send.emperor = false;

	UserSession* pUser  = UserSessionManager::getInstance()->getUserByID(id);
	if (pUser)
	{
		// TODO:判断是否是城主或国王
		if (this->myUnion->master && this->myUnion->master->id == pUser->id)  
		{//是帮主
			send.unionmaster = true;
			send.dwActionPoint = this->myUnion->getActionPoint();

			if (CCityM::getMe().find(pUser->country, KING_CITY_ID, this->myUnion->id) !=NULL)
			{//是国王
				CCountry* pCountry = CCountryM::getMe().find(pUser->country);
				if (pCountry) pCountry->updateKing(pUser);

				CCountry* pEmperorCountry = CCountryM::getMe().find(NEUTRAL_COUNTRY_ID);
				if (pEmperorCountry && pEmperorCountry->dwKingUnionID == pUser->unionid)
				{
					pEmperorCountry->updateKing(pUser);
					send.emperor = true;
				}
				send.king = true;
			}
		}
		
		if (CCityM::getMe().findByUnionID(this->myUnion->id) != NULL)
		{
			send.caption = CCityM::getMe().findByUnionID(this->myUnion->id)->dwCityID;
		}
		
		strncpy(send.name, this->myUnion->name, sizeof(send.name));

		pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_sendUserRelationID));
	}
}

/**
* \brief 析构函数，析构的时候刷新成员的数据库存档
* \author fqnewman
*/
CUnionMember::~CUnionMember()
{
//	if (!destroy) writeDatabase();
}

void CUnionMember::sendUnionDare(UserSession* pDareUser, const char* fromName, DWORD dwWarID)
{
	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(this->id);

	if (pUser && pDareUser)
	{//找到有效应战者,向其发送挑战询问命令
		Cmd::stActiveDareCmd send;

		send.dwMsgType = Cmd::DARE_QUESTION;
		send.dwWarID   = dwWarID;
		send.dwDareType = Cmd::UNION_DARE;

		strncpy(send.name, pDareUser->name, MAX_NAMESIZE);
		strncpy(send.fromRelationName, fromName, MAX_NAMESIZE);

		pUser->sendCmdToMe(&send, sizeof(send));
#ifdef _ZJW_DEBUG                   
		Zebra::logger->debug("向应战者发出询问。");
#endif
	}
}

/**
* \brief 设置成员的所属帮会对象
* \param pUnion 帮会对象指针
* \author fqnewman
*/
void CUnionMember::setUnion(CUnion * pUnion)
{
	rwlock.wrlock();
	myUnion = pUnion;
	rwlock.unlock();
}

/**
* \brief 检查成员是否具有某项权力
* \param power 所检查的权利项
* \author fqnewman
* \return true有权， false无权
*/
bool CUnionMember::havePower(const int power)
{
	return Cmd::isset_state(byPower,power);
}

void  CUnionMember::setPower(const int power)
{
	Cmd::set_state(byPower, power);
}

/**
* \brief 发送聊天消息给成员的客户端
* \param message 消息体
* \author fqnewman
*/
void CUnionMember::sendMessageToMe(const char *message)
{
	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
	if (NULL !=pUser) pUser->sendSysChat(Cmd::INFO_TYPE_SYS, message);
}

/**
* \brief 将本成员的数据库记录从库中删除
* \author fqnewman
* \return true 成功， false 失败
*/
bool CUnionMember::deleteMeFromDB()
{
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "UNIONID = %u AND CHARID = %u", myUnion->getID(), id);
	unsigned int retcode = SessionService::dbConnPool->exeDelete(handle, "`UNIONMEMBER`", where);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->debug("删除帮会成员失败 %u", id);
		return false;
	}
	else
	{
		destroy = true;
	}
	return true;
}

/**
* \brief 解除成员与帮会的关系
* \param notify 通知标志，为true表示要通知所有的在线成员，为flase表示不通知
* \author fqnewman
*/
void CUnionMember::fireMe(const bool notify, const bool checksept)
{
	if (deleteMeFromDB())
	{
		if (!CUnionM::getMe().removeMemberIndex(name))
		{
			Zebra::logger->error("[帮会]:%s 执行removeMemberIndex失败", name);
		}
		
		if (notify) 
		{
			myUnion->notifyMemberFire(name,aliasname);
			myUnion->sendUnionNotify("%s(%s) 离开了帮会", name, aliasname);	
		}

		CSept* pSept = CSeptM::getMe().getSeptByID(this->septid);
		if (pSept && checksept)
		{
			pSept->fireSeptMemberDirect(this->id, false);
		}
		
		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
		if (pUser)
		{
			Cmd::Session::t_fireUnionMember_SceneSession send;
			send.dwCharID = id;

			send.dwMapTempID = pUser->scene->tempid;
			pUser->scene->sendCmd(&send,sizeof(Cmd::Session::t_fireUnionMember_SceneSession)); /// 通知场景服务器
			pUser->unionid = 0;
		}
	}
}

/**
* \brief 发送用户的帮会成员数据
* \author fqnewman
*/
void CUnionMember::sendUserUnionData()
{
	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
	if (myUnion && pUser)
	{
		myUnion->sendUnionInfoToUser(pUser); // 发送帮会的信息给当前成员
		sendUserUnionPrivateData();         // 发送当前会员的个人信息给当前成员
		myUnion->sendUnionMemberList(pUser); // 发送帮会的成员列表给当前成员
	}
}

void CUnionMember::change_aliasname(const char* newaliasname)
{
	rwlock.wrlock();
	strncpy(this->aliasname, newaliasname, MAX_NAMESIZE);
	rwlock.unlock();
	this->writeDatabase();
	this->online(Cmd::UNION_MEMBER_STATUS_ALIASCHANGE);
}

/**
* \brief 判断成员是否在线
* \author fqnewman
* \return true在线  false 不在线
*/
bool CUnionMember::isOnline()
{
    return (byStatus==CUnionMember::Online)?true:false;
}

/** 
* \brief 成员上线处理
* \param status 成员的上线状态
* \author fqnewman
*/
void CUnionMember::online(const unsigned int status)
{
	Cmd::stBroadcastMemberInfo send;

	rwlock.wrlock();
	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(id);
	if (pUser)
	{
		send.exploit = pUser->dwExploit;
		byStatus = CUnionMember::Online;
		wdOccupation = pUser->occupation;
		send.level = pUser->level;
	}
	
	send.byStatus = status;
	send.wdOccupation = wdOccupation;
	strncpy(send.name,name,MAX_NAMESIZE);
	strncpy(send.aliasname, aliasname, MAX_NAMESIZE);
	if (pUser) wdOccupation = pUser->occupation;
	rwlock.unlock();
	
	CSept* pSept = CSeptM::getMe().getSeptByID(this->septid);
	if (pSept)
	{
		if (pSept->tempid == this->id)
		{
			send.bySeptMaster = 1;
		}
		else
		{
			send.bySeptMaster = 0;
		}
		strncpy(send.septName, pSept->name, MAX_NAMESIZE);
	}
	else
	{
		bzero(send.septName, MAX_NAMESIZE);
	}
	
		
			
	if (myUnion && myUnion->master) 
	{
		if (myUnion->master->id == this->id && pUser && myUnion->dwCountryID<1)
		{
			myUnion->dwCountryID = pUser->country;
			myUnion->writeDatabase();
		}
		
		myUnion->sendCmdToAllMember(&send,sizeof(send));
	}
	else
	{
		Zebra::logger->error("在帮会成员%s找不到自己的帮会对象",send.name);
	}
}

/**
* \brief 成员下线处理
* \author fqnewman
*/
void CUnionMember::offline()
{
	Cmd::stBroadcastMemberInfo send;

	rwlock.wrlock();
	byStatus = CUnionMember::Offline; 
	//user = NULL;
	send.byStatus = Cmd::UNION_MEMBER_STATUS_OFFLINE;
	strncpy(send.name,name,MAX_NAMESIZE);
	strncpy(send.aliasname, aliasname, MAX_NAMESIZE);
	rwlock.unlock();
	if (myUnion) myUnion->sendCmdToAllMember(&send,sizeof(send));
	else
	{
		Zebra::logger->error("在帮会成员%s找不到自己的帮会对象",send.name);
	}
}

//---------------------------------------new---------------------------------------------

/**
* \brief 删除实体根据名称
* \param name 实体名称
* \author fqnewman
*/
void CUnion::removeEntryByName(const char * name)
{
	zEntry *rm=getEntryByName(name);
	removeEntry(rm);
}

/**
* \brief 帮会初始化，根据传入数据初始化帮会的基本信息
* \param info 帮会数据结构
* \author fqnewman
*/
void CUnion::init(const stUnionInfo & info)
{
	rwlock.wrlock();		
	id = info.dwUnionID;
	strncpy(name,info.name,MAX_NAMESIZE);
	byVote = info.byVote;
	level = info.wdLevel;
	exp   = info.qwExp;
	dwMana = info.dwMana;
	dwCountryID = info.dwCountryID;
	tempid = info.dwCharID;
	dwCreateTime = info.dwCrTime;
	dwActionPoint = info.dwActionPoint;
	calltimes = info.calltimes;
	dwMoney	= info.dwMoney;
	strncpy(note, info.note, sizeof(note));
	master = NULL;
	rwlock.unlock();
}

/**
* \brief 创建一个帮会的帮主成员，分配权限
* \param info 帮会成员数据结构
* \author fqnewman
*/
CUnionMember * CUnion::addUnionMaster(const stUnionInfo& info)
{
	stUnionMemberInfo masterInfo;

	masterInfo.dwCharID = info.dwCharID;
	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(masterInfo.dwCharID);
	if (pUser) 
	{
		masterInfo.wdOccupation = pUser->occupation;
		masterInfo.dwSeptID = pUser->septid;
	}

	strncpy(masterInfo.name,info.masterName,MAX_NAMESIZE);
	strncpy(masterInfo.aliasname,DEFAULTMASTERALIAS,MAX_NAMESIZE);
	SETMASTERPOWER_WD(masterInfo.wdPower);
	
	return addUnionMember(masterInfo);	
}

/**
* \brief 创建并初始化帮会成员
* \param info 帮会成员数据结构
* \author fqnewman
* \return 成功返回新创建的帮会成员对象，失败返回NULL
*/
CUnionMember * CUnion::addUnionMember(const stUnionMemberInfo& info)
{
	CUnionMember *pMember = NULL;

	pMember = new CUnionMember();
	pMember->init(info);
	
	if (pMember)
	{
		pMember->setUnion(this);
		
		if (info.wdPower == 0)
		{//重新初始化帮会成员权限
			pMember->byPower[0] = 0x00;
			pMember->byPower[1] = 0x00;

			if (info.dwCharID == this->tempid)
			{//设置帮主权限
				SETMASTERPOWER(pMember->byPower);
			}
			else
			{//设置为帮众权限
				SETMEMBERPOWER(pMember->byPower);
			}
			pMember->writeDatabase();
		}
		
		rwlock.wrlock();
		if (!addEntry(pMember))
		{
			Zebra::logger->debug("[帮会]: %s 帮会添加成员 %s 进帮会管理器失败", this->name, info.name);
		}
		rwlock.unlock();
		CUnionM::getMe().addMemberIndex(info.name, pMember);  // 在CUnionM中加入索引这样可以使外面的访问者通过CUnionM找到自己。
	}
	return pMember;
}

/**
* \brief 发送命令给场景设置所有成员的战争状态并通知客户端
* \param ptEnterWarCmd 进入战争状态通知消息
* \param cmdLen 消息长度
*/
void CUnion::sendCmdToAllMemberScene(Cmd::Session::t_enterWar_SceneSession* ptEnterWarCmd, const unsigned int cmdLen)
{
	struct findall : public execEntry<CUnionMember> 
	{
		Cmd::Session::t_enterWar_SceneSession* cmd;
		unsigned int cmdLen;

		findall(Cmd::Session::t_enterWar_SceneSession* ptCmd, const unsigned int nCmdLen)
		{
			cmd = ptCmd;
			cmdLen = nCmdLen;
		}

		~findall(){}
		bool exec(CUnionMember* pMember)
		{
			UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
			if (pUser && pUser->scene)
			{
				cmd->dwUserID = pUser->id;
				cmd->dwSceneTempID = pUser->scene->tempid;
				if (cmd->dwStatus)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已进入帮会对战状态。");
				}
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已退出帮会对战状态。");
				}
				pUser->scene->sendCmd(cmd, cmdLen);
			}
			return true;
		}
	};

	findall myList(ptEnterWarCmd, cmdLen);
	execEveryOne(myList);
}
void CUnion::sendCallCmdToAllMemberScene(Cmd::Session::t_GoTo_Leader_Check_SceneSession* ptCmd, const unsigned int cmdLen,DWORD expect)
{
	struct findall : public execEntry<CUnionMember> 
	{
		Cmd::Session::t_GoTo_Leader_Check_SceneSession* cmd;
		unsigned int cmdLen;
		DWORD expect;

		findall(Cmd::Session::t_GoTo_Leader_Check_SceneSession* ptCmd, const unsigned int nCmdLen,DWORD exp)
		{
			cmd = ptCmd;
			cmdLen = nCmdLen;
			expect=exp;
		}

		~findall(){}
		bool exec(CUnionMember* pMember)
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
			return true;
		}
	};

	findall myList(ptCmd, cmdLen,expect);
	execEveryOne(myList);
}

void CUnion::fireUnionMemberLeave(DWORD dwUserID)
{
	CUnionMember * pMember = NULL;
	pMember = (CUnionMember *)getEntryByID(dwUserID);

	if (pMember)
	{
		CSept* pSept = CSeptM::getMe().getSeptByID(pMember->septid);

		if (pSept && pSept->master && pSept->master->id == dwUserID)
		{
			this->fireSeptFromUnion(pMember->septid);
		}       
		else
		{
			this->fireUnionMemberDirect(dwUserID, false);
		}
	}
}

void CUnion::sendUnionDare(UserSession* pUser, const char* fromName, DWORD dwWarID)
{
	struct findall : public execEntry<CUnionMember>
	{
		UserSession* _pUser;
		char _fromName[MAX_NAMESIZE];
		DWORD _dwWarID;
		findall(UserSession* pUser, const char* fromName, DWORD dwWarID)
		{
			_pUser = pUser;
			strncpy(_fromName, fromName, MAX_NAMESIZE);
			_dwWarID = dwWarID;
		}
		~findall(){}

		bool exec(CUnionMember *pMember)
		{
			if (pMember->havePower(Cmd::DARE))
			{
				pMember->sendUnionDare(_pUser, _fromName, _dwWarID);
			}
			return true;
		}
	};

	if (1==byVote) return;
	findall myList(pUser, fromName, dwWarID);
	execEveryOne(myList);
}

/**
* \brief 发送命令给所有的成员（客户端）
* \param ptCmd 消息体
* \param nCmdLen 消息长度
* \author fqnewman
*/
void CUnion::sendCmdToAllMember(const Cmd::stNullUserCmd * ptCmd, const unsigned int nCmdLen )
{
	struct findall : public execEntry<CUnionMember>
	{
		const Cmd::stNullUserCmd * cmd;
		unsigned int cmdLen;
		findall(const Cmd::stNullUserCmd * ptCmd, const unsigned int nCmdLen)
		{
			cmd = ptCmd;
			cmdLen = nCmdLen;
		}
		~findall(){}

		bool exec(CUnionMember *pMember)
		{
			pMember->sendCmdToMe(cmd,cmdLen);
			return true;
		}
	};

	if (1==byVote) return;
	findall myList(ptCmd,nCmdLen);
	execEveryOne(myList);
}

/**
* \brief 通知帮会内有成员被开除
* \param pName 被开除者的名称
* \param pAliasname 被开除者的别名
* \author fqnewman
*/
void CUnion::notifyMemberFire(const char * pName, const char *pAliasname)
{
	Cmd::stBroadcastMemberInfo send;

	send.byStatus = Cmd::UNION_MEMBER_STATUS_FIRE;
	strncpy(send.name,pName,MAX_NAMESIZE);
	strncpy(send.aliasname, pAliasname, MAX_NAMESIZE);
	sendCmdToAllMember(&send,sizeof(send));
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
* \brief 通知帮会战结果
* \param  msg ... 通知内容
*/
void CUnion::notifyWarResult(const char* msg, ...)
{
	char buf[1024+1];
	bzero(buf, sizeof(buf));

	struct findall : public execEntry<CUnionMember> 
	{
		const char* msg;

		findall(const char* pMsg)
		{
			msg = pMsg;
		}

		~findall(){}

		bool exec(CUnionMember* pMember)
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
* \brief 发送用户的帮会数据到客户端
* \param pName 接收信息的用户
* \author fqnewman
*/
void CUnion::sendUserUnionData(const char *pName)
{
	CUnionMember *pMember;

	pMember = (CUnionMember *)getEntryByName(pName);
	if (pMember)
	{
		pMember->sendUserUnionData();
	}
}

/**
* \brief 发送帮会信息给指定用户的客户端，用在用户上线初始化的时候。
* \param pUser 上线用户
* \author fqnewman
*/
void CUnion::sendUnionInfoToUser(UserSession *pUser)
{
	Cmd::stUnionBaseInfoCmd retUnion;

	rwlock.rdlock();
	strncpy(retUnion.unionName,name,MAX_NAMESIZE);		// 帮会名称
	if (master) strncpy(retUnion.master,master->name,MAX_NAMESIZE);	// 帮会会长
	else  strncpy(retUnion.master,"记录不完整请查证",MAX_NAMESIZE);	// 帮会会长
	retUnion.wdLevel 	= level;						// 帮会级别
	retUnion.qwExp 		= exp;							// 帮会经验
	retUnion.dwMana		= this->getMana();
	retUnion.dwAction	= this->dwActionPoint;
	strncpy(retUnion.note, this->note, sizeof(retUnion.note));
	rwlock.unlock();
	pUser->sendCmdToMe(&retUnion,sizeof(retUnion));
}

DWORD CUnion::getMana()
{
	struct findList : public execEntry<CUnionMember>
	{
		DWORD _dwMana;
		
		findList()
		{
			_dwMana = 0;
		}
		~findList(){}
		bool exec(CUnionMember *pMember)
		{
			if (pMember)
			{
				CSept* pSept = CSeptM::getMe().getSeptByID(pMember->septid);
				if (pSept && pSept->master && pSept->master->id==pMember->id)
				{
					_dwMana = _dwMana + pSept->getRepute();
				}
			}
			
			return true;
		}
	};

	findList myList;
	execEveryOne(myList);
	return (myList._dwMana+this->dwMana);

}

/**
* \brief 发送帮会成员列表，发送有最大限制，最多只发100条帮会成员记录。
* \param pUser 数据接受者对象
* \author fqnewman
*/
void CUnion::sendUnionMemberList(UserSession *pUser)
{
	BYTE buf[zSocket::MAX_DATASIZE];
	unsigned int count;
	Cmd::stUnionRember *tempPoint;

	std::list<CUnionMember *> _deleteList;
        std::list<CUnionMember *>::iterator vIterator;
	
	rwlock.rdlock();
	Cmd::stAllUnionMemberCmd *retCmd=(Cmd::stAllUnionMemberCmd *)buf;
	constructInPlace(retCmd);
	tempPoint = (Cmd::stUnionRember *)retCmd->memberList;
	unsigned int len = sizeof(Cmd::stUnionRember);
	count = 0;
	
	for (zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		struct Cmd::stUnionRember mInfo;
		CUnionMember *temp=(CUnionMember *)it->second;
		CSept* pSept = CSeptM::getMe().getSeptByID(temp->septid);

		if (pSept == NULL || pSept->master == NULL)
		{
			_deleteList.push_back(temp);
			Zebra::logger->error("[帮会]: %s 所属家族(%d)已不存在", temp->name, temp->id);
			continue;
		}

#ifdef _ZJW_DEBUG			
		Zebra::logger->debug("帮众:%s", temp->name);
#endif			
		temp->getMemberBaseInfo(mInfo);
		bcopy(&mInfo,tempPoint,len);
		strncpy(tempPoint->septName, pSept->name, MAX_NAMESIZE);
		if (pSept->master && pSept->master->id!=temp->id &&  pSept->getMemberByName(temp->name) != NULL)
		{
			strncpy(tempPoint->aliasname, pSept->getMemberByName(temp->name)->aliasname, MAX_NAMESIZE);
		}

		if (pSept->tempid == temp->id)
		{
			tempPoint->bySeptMaster = 1;
		}
		else
		{
			tempPoint->bySeptMaster = 0;
		}

		tempPoint++;
		count++;

		//pSept->full_SeptMember_Data(tempPoint, count);

		if (1000==count)
		{
			goto breakfor; // 当记录超过100的时候会超过命令发送的最大限制
		}
	}
breakfor:
	rwlock.unlock();
	
	retCmd->size = count;
	pUser->sendCmdToMe(retCmd,(count*sizeof(Cmd::stUnionRember)+sizeof(Cmd::stAllUnionMemberCmd)));
}

/**
* \brief 开除帮会成员
* \param master 行使权利者的名称 
* \param member 被开除者的名称
* \author fqnewman
* \return 
*/
void  CUnion::fireUnionMember(const char * master, const char *member)
{
	CUnionMember *pMaster = (CUnionMember *)getEntryByName(master);
	CUnionMember *pMember = (CUnionMember *)getEntryByName(member);
	
	if (pMaster)
	{
		if (pMember)
		{
			if (pMaster->id == pMember->id)
			{
				pMaster->sendMessageToMe("不能开除自己");
				return;
			}

			if (pMember->id == pMaster->myUnion->master->id)
			{
				pMaster->sendMessageToMe("不能开除帮主");
				return;
			}
			
			CUnion* pUnion = pMaster->myUnion;

			if (pUnion && (pUnion->master==pMaster || pMaster->havePower(Cmd::FIRE_MEMBER)) )
			{
				CSept* pSept = CSeptM::getMe().getSeptByID(pMember->septid);

				if (pSept && pSept->master && pSept->master->id == pMember->id)
				{
					this->fireSeptFromUnion(pMember->septid);
				}
				else
				{
					pMaster->sendMessageToMe("只能开除族长");
				}
			}
			else
			{
				pMaster->sendMessageToMe("你没有开除成员的权力，如果你发现错误请报告给GM");
			}
		}
		else
		{
			pMaster->sendMessageToMe("你所在的帮会中没有此人,请确认名字是否正确");
		}
	}
	else
	{
		Zebra::logger->debug("开除帮会成员操作中出现错误的状态");
	}
}

/**
* \brief 直接开除帮会成员
* \param dwCharID 被开除的帮会成员ID
* \author fqnewman
* \return 删除成功返回1 删除失败返回2
*/
int CUnion::fireUnionMemberDirect(const DWORD dwCharID, const bool checksept)
{
	int ret = 0;

	rwlock.wrlock();
	CUnionMember * pMember = NULL;
	pMember = (CUnionMember *)getEntryByID(dwCharID);
	removeEntry(pMember);
	rwlock.unlock();

	if (pMember)
	{
		pMember->fireMe(true, checksept);
		SAFE_DELETE(pMember);
		ret = 1;
	}
	else
	{
		ret = 2;
	}

	return ret;
}

bool  CUnion::isMember(DWORD dwUserID)
{
	CUnionMember * pMember = NULL;
	pMember = (CUnionMember *)getEntryByID(dwUserID);

	if (pMember)
	{
		return true;
	}

	return false;
}

/**
* \brief 更新数据库记录
* \author fqnewman
*/
void CUnion::writeDatabase()
{
	const dbCol union_update_define[] = {
		{ "`CHARID`",				zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`VOTE`",				zDBConnPool::DB_BYTE,	sizeof(BYTE) },
		{ "`LEVEL`",				zDBConnPool::DB_WORD,		sizeof(WORD) }, 
		{ "`EXP`",				zDBConnPool::DB_QWORD,	sizeof(QWORD) },
		{ "`MANA`",				zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`COUNTRYID`",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`ACTIONPOINT`",				zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`MONEY`",				zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`MASTER`",     	                zDBConnPool::DB_STR,    sizeof(char[MAX_NAMESIZE+1]) },
		{ "`NOTE`",				zDBConnPool::DB_STR,	sizeof(char[254+1])},
		{ "`CALLTIMES`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};
	struct {
		DWORD dwCharid;
		BYTE  byVote;
		WORD 	wdLevel;
		QWORD qwExp;
		DWORD dwMana;
		DWORD dwCountryID;
		DWORD dwActionPoint;
		DWORD dwMoney;
		char  master[MAX_NAMESIZE+1];
		char  note[254+1];
		DWORD calltimes;
	} __attribute__ ((packed))
	updateunion_data;
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}

	bzero(&updateunion_data,sizeof(updateunion_data));
	rwlock.rdlock();
	
	if (master) 
	{
		updateunion_data.dwCharid = master->id;
	}
	
	updateunion_data.dwCountryID = this->dwCountryID;
	updateunion_data.dwMana = this->dwMana;	
	updateunion_data.dwActionPoint = this->dwActionPoint;
	updateunion_data.dwMoney = this->dwMoney;
	updateunion_data.byVote = byVote;
	updateunion_data.wdLevel  = level;
	updateunion_data.qwExp 		= exp;
	updateunion_data.calltimes	= this->calltimes;
#ifdef _ZJW_DEBUG	
	if (updateunion_data.dwActionPoint>1000)
	{
		Zebra::logger->trace("[帮会]:警告,行动力过大:%d", updateunion_data.dwActionPoint);
	}
#endif	

	if (master)
	{
		strncpy(updateunion_data.master, this->master->name, MAX_NAMESIZE);
	}

	strncpy(updateunion_data.note,note, sizeof(updateunion_data.note));
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "`UNIONID` = %u", id);
	rwlock.unlock();
	unsigned int retcode = SessionService::dbConnPool->exeUpdate(handle, "`UNION`", union_update_define, (BYTE *)(&updateunion_data), where);
	SessionService::dbConnPool->putHandle(handle);

	if ((DWORD)-1 == retcode)
	{
		Zebra::logger->error("CUnion 修改会员档案失败：UNIONID=%u retcode=%u", id, retcode);
	}
}

/**
* \brief 构造帮会，初始化变量
* \author fqnewman
*/
CUnion::CUnion()
{
	destroy = false;
	master = NULL;
	byVote  = 1;
	calltimes = 0;
}

/**
* \brief 析构帮会及其所有成员
* \author fqnewman
*/
CUnion::~CUnion()
{
	if (!destroy)
	{
		writeDatabase();
		rwlock.wrlock();
		for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
		{
			CUnionMember *temp = (CUnionMember *)it->second;
			SAFE_DELETE(temp);
		}
		clear();
		rwlock.unlock();
	}
}

/**
* \brief 解散帮会，将所有成员的帮会关系解除包括帮主自己
* \author fqnewman
*/
void CUnion::disbandUnion()
{
	rwlock.wrlock();
	Zebra::logger->trace("[帮会]帮会解散%s", this->name);
	destroy = true;
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CUnionMember *temp=(CUnionMember *)it->second;
		if (temp)
		{
			CSept* pSept = CSeptM::getMe().getSeptByID(temp->septid);

			if (pSept && pSept->master && pSept->master->id == temp->id)
			{
				pSept->dwUnionID = 0;
				pSept->writeDatabase();
			}

			temp->fireMe(false, false);
			SAFE_DELETE(temp);
		}
	}

	//TODO:同时重置该帮拥有的城市城主信息
	CCity* pCity =  CCityM::getMe().findByUnionID(this->id);	
	if (pCity)
	{
		if (pCity->dwCityID == KING_CITY_ID)
		{
			CCountry* pCountry = CCountryM::getMe().find(pCity->dwCountry, this->id);
			if (pCountry)
			{
				pCountry->changeKing(NULL);
			}
		}
		
		pCity->dwUnionID = 0;
		pCity->writeDatabase();
	}
	
	clear();
	deleteMeFromDB();
	rwlock.unlock();
}

bool CUnion::isDel()
{
	DWORD septs = this->septSize();
	return septs>1?false:true;
}

DWORD CUnion::septSize()
{
	DWORD ret=0;
	
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CUnionMember *temp=(CUnionMember *)it->second;
		if (temp)
		{
			CSept* pSept = CSeptM::getMe().getSeptByID(temp->septid);
			if (pSept && pSept->master && pSept->master->id == temp->id)
			{//统计族长
				ret++;
			}
		}
	}
	
	return ret;
}

void CUnion::refreshSeptExp()
{
	if (CCityM::getMe().findByUnionID(this->id) != NULL)
	{
		for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
		{
			CUnionMember *temp=(CUnionMember *)it->second;
			if (temp)
			{
				CSept* pSept = CSeptM::getMe().getSeptByID(temp->septid);
				if (pSept && pSept->master && pSept->master->id == temp->id)
				{//设置家族经验领取标志
					Zebra::logger->trace("[家族]: %s 刷新家族经验", 
							pSept->name);
					pSept->setExp();
				}
			}
		}
	}
}

void CUnion::changeAllSeptRepute(int repute)
{
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CUnionMember *temp=(CUnionMember *)it->second;
		if (temp)
		{
			CSept* pSept = CSeptM::getMe().getSeptByID(temp->septid);
			if (pSept && pSept->master && pSept->master->id == temp->id)
			{//统计族长
				pSept->changeRepute(repute);
			}
		}
	}
}
/**
* \brief 删除本帮会的数据库记录
* \author fqnewman
* \return true 成功 false 失败
*/
bool CUnion::deleteMeFromDB()
{
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "UNIONID = %u ", id);
	unsigned int retcode = SessionService::dbConnPool->exeDelete(handle, "`UNION`", where);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("删除帮会失败 %u", id);
		return false;
	}
	else
	{
		destroy = true;
	}
	return true;
}

/**
* \brief 从数据库中加载帮会成员
* \author fqnewman
* \return true 加载成功， false 加载失败
*/
bool CUnion::loadUnionMemberFromDB()
{
	static const dbCol unionmember_define[] = {
		{ "CHARID",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "NAME",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "ALIASNAME",		zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "POWER",			zDBConnPool::DB_WORD,	sizeof(WORD) },
		{ "`OCCUPATION`",	zDBConnPool::DB_WORD,	sizeof(WORD) }, 
		{ "SEPTID",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};

	stUnionMemberInfo *memberList, *tempPoint;
	stUnionMemberInfo info;
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "UNIONID = %u", id);
	unsigned int retcode = SessionService::dbConnPool->exeSelect(handle, "`UNIONMEMBER`", unionmember_define, where, NULL, (unsigned char **)&memberList);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode
			|| 0 == retcode)
	{
		Zebra::logger->debug("没有找到帮会成员记录");
		return true;
	}

	if (memberList)
	{
		tempPoint = &memberList[0];
		for (unsigned int i=0; i< retcode; i++)
		{
			bcopy(tempPoint, &info, sizeof(stUnionMemberInfo));
			CUnionMember *member = addUnionMember(info);
			if (info.dwCharID == tempid) 
			{
				//Zebra::logger->trace("[帮会加载]:%s(%d) 帮主加载成功", member->name, this->id);
				master = member; // masterid被保存在tempid中；这里初始化master对象
			}
			
			if (member->septid>0)
			{
				CSept* pSept = (CSept*)(CSeptM::getMe().getSeptByID(member->septid));
				if (pSept==NULL && this->tempid!=member->id)
				{
					this->fireUnionMemberLeave(member->id);
					Zebra::logger->trace("[帮会]: %s 所属家族已经不存在,从帮会成员表中删除", member->name);
				}
				else
				{
					if (pSept && !pSept->isMember(member->id) && this->tempid!=member->id)
					{
						this->fireUnionMemberLeave(member->id);
						Zebra::logger->trace("[帮会]: %s 已不在对应家族中,从帮会成员表中删除", member->name);
					}
					else if (pSept && pSept->dwUnionID!=this->id && this->tempid!=member->id)
					{
						this->fireUnionMemberLeave(member->id);
						Zebra::logger->trace("[帮会]: %s 与帮主不在同一帮会,从帮会成员表中删除", 
								member->name);
					}
				}
			}
			// */

			tempPoint++;
		}
		SAFE_DELETE_VEC(memberList);
	}
	else
	{
		Zebra::logger->error("帮会数据初始化失败，exeSelect 返回无效buf指针");
	}

	if (master == NULL)
	{
		Zebra::logger->trace("[帮会加载]:%d 帮主加载失败", this->id);
	}
	
	return true;
}

/**
* \brief 发送帮会通知
*
* \param message 消息
*
* \author zjw
*/
void CUnion::sendUnionNotify(const char* message, ...)
{
	if (message == NULL)
	{
		return;
	}

	char buf[1024+1];
	bzero(buf, sizeof(buf));
	getMessage(buf, 1024, message);
	
	Cmd::stChannelChatUserCmd send;
//	send.dwType=Cmd::CHAT_TYPE_UNION;
	send.dwType = Cmd::CHAT_TYPE_SYSTEM;
	send.dwSysInfoType = Cmd::INFO_TYPE_EXP;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));
	
	sprintf((char*)send.pstrName, "帮会通知");
	sprintf((char*)send.pstrChat, "%s", buf);

	this->sendCmdToAllMember(&send, sizeof(send));
}

/**
* \brief 获取帮会成员数目
* \author fqnewman
* \return 帮会成员数字
*/
DWORD CUnion::size()
{
	return zEntryName::size();
}

/**
* \brief 判断是否在投票状态
* \author fqnewman
* \return true 还在投票中 false 投票已结束
*/
bool CUnion::isVote()
{
	return 1==byVote;
}

void CUnion::update_all_data()
{
	struct findList : public execEntry<CUnionMember>
	{
		findList()
		{
		}
		~findList(){}
		bool exec(CUnionMember *pMember)
		{
			pMember->update_data();
			return true;
		}
	};

	findList myList;
	execEveryOne(myList);
}

/**
* \brief 投票结束处理
* \author fqnewman
*/
void CUnion::letVoteOver()
{
	struct findList : public execEntry<CUnionMember>
	{
		findList()
		{
		}
		~findList(){}
		bool exec(CUnionMember *pMember)
		{
			UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
			if (pUser)
			{
				//pMember->online();
				pMember->sendUserUnionData();
				pMember->update_data();
				pUser->unionid = pMember->myUnion->id;
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
* \brief 设置帮会的介绍（并存库）
* \param pCmd 帮会设置消息
* \author fqnewman
*/
void CUnion::setNote(Cmd::stNoteUnionCmd *pCmd)
{
	rwlock.wrlock();
	strncpy(note,pCmd->noteBuf, sizeof(note));
	rwlock.unlock();
	writeDatabase();
	sendCmdToAllMember(pCmd,sizeof(Cmd::stNoteUnionCmd));
}

void  CUnion::changeMaster(CUnionMember* pMember)
{
	rwlock.wrlock();

	//UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
	
	// 暂存帮主对象
	CUnionMember* pSrcMaster = this->master;
	
	// 赋新的帮主对象
	this->master = pMember;
	this->tempid = pMember->id;

	// 修改两者的权限及称号
	strncpy(this->master->aliasname, DEFAULTMASTERALIAS, MAX_NAMESIZE);
	this->master->byPower[0] = 0x00;
	this->master->byPower[1] = 0x00;

	SETMASTERPOWER(this->master->byPower);
	
	pSrcMaster->byPower[0] = 0x00;
	pSrcMaster->byPower[1] = 0x00;	
	strncpy(pSrcMaster->aliasname, "族长", MAX_NAMESIZE);
	SETMEMBERPOWER(pSrcMaster->byPower);

	pSrcMaster->writeDatabase();
	this->master->writeDatabase();	
	rwlock.unlock();
	
	this->writeDatabase();
	
	struct findall : public execEntry<CUnionMember>
	{
		findall()
		{
		}
		~findall(){}

		bool exec(CUnionMember *pMember)
		{
			if (pMember) pMember->sendUserUnionData();
			return true;
		}
	};

	findall myList;
	execEveryOne(myList);
	this->update_all_data();
}

void CUnion::fireSeptFromUnion(const DWORD dwSeptID)
{
	struct findList : public execEntry<CUnionMember>
	{
		DWORD _dwSeptID;
		std::vector<DWORD> _removeList;
		
		findList(DWORD dwSeptID)
		{
			_dwSeptID = dwSeptID;
		}
		~findList(){}
		
		void remove(CUnion* pUnion)
		{
			for (std::vector<DWORD>::iterator vIter=_removeList.begin(); 
				vIter!=_removeList.end();vIter++)
			{
				 if (pUnion) pUnion->fireUnionMemberDirect(*vIter, false);
			}
		}
		
		bool exec(CUnionMember *pMember)
		{
			if (pMember->septid == _dwSeptID)
			{
				_removeList.push_back(pMember->id);
			}
			
			return true;
		}
	};

	findList myList(dwSeptID);
	execEveryOne(myList);
	myList.remove(this);
	writeDatabase();
	
	CSept* pSept = CSeptM::getMe().getSeptByID(dwSeptID);
	if (pSept)
	{
		pSept->clearUnion();
		pSept->writeDatabase();
	}

	if (this->isDel())
	{
		CUnionM::getMe().delUnion(this->id);
	}
}

DWORD CUnion::getActionPoint()
{
	return this->dwActionPoint;
}

bool CUnion::changeActionPoint(int repute)
{
	bool ret = false;
	
	if (repute<0)
	{
		if ((int)this->dwActionPoint>=abs(repute))
		{
			this->dwActionPoint = (int)this->dwActionPoint - abs(repute);
			ret = true;
		}
		else
		{
			ret = false;
		}
	}
	else
	{
		this->dwActionPoint += repute;
		ret = true;
	}

	if (this->master) this->master->update_data();
	this->writeDatabase();
	this->sendUnionInfoToAll();

	return ret;
}

DWORD CUnion::getMoney()
{
	return this->dwMoney;
}

bool CUnion::changeMoney(int money)
{
	bool ret = false;
	
	if (money<0)
	{
		if ((int)this->dwMoney>=abs(money))
		{
			this->dwMoney = (int)this->dwMoney - abs(money);
			ret = true;
		}
		else
		{
			ret = false;
		}
		if (this->master) this->master->update_data();
		this->writeDatabase();
		this->sendUnionInfoToAll();
	}
	else
	{
		this->dwMoney += money;
		this->writeDatabase();
		this->sendUnionInfoToAll();
		return true;
	}

	return ret;
}

void CUnion::sendUnionManaToAll()
{
	FunctionTime func_time(0,__PRETTY_FUNCTION__,"改变一个帮会威望所需时间" , 32);
	
	struct findList : public execEntry<CUnionMember>
	{
		findList()
		{
		}
		~findList(){}
		bool exec(CUnionMember *pMember)
		{
			if (pMember)
			{
				UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
				if (pMember->myUnion && pUser)
				{
					pMember->myUnion->sendUnionInfoToUser(pUser); // 发送帮会的信息给当前成员
				}
			}
			return true;
		}
	};

	findList myList;
	execEveryOne(myList);
}

void CUnion::sendUnionInfoToAll()
{
	struct findList : public execEntry<CUnionMember>
	{
		findList()
		{
		}
		~findList(){}
		bool exec(CUnionMember *pMember)
		{
			if (pMember)
			{
				pMember->sendUserUnionData();
				pMember->update_data();
			}
			return true;
		}
	};

	findList myList;
	execEveryOne(myList);
}

//-------------------------------------------------------------------------------------
/**
* \brief 管理器构造函数
* \author fqnewman
*/
CUnionM::CUnionM()
{
}

/**
* \brief 管理器析构函数
* \author fqnewman
*/
CUnionM::~CUnionM()
{
	rwlock.wrlock();
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CUnion *temp =(CUnion *)it->second;
		SAFE_DELETE(temp);
	}
	clear();
	rwlock.unlock();
}

/**
* \brief 主动释放管理器
* \author fqnewman
*/
void CUnionM::destroyMe()
{
	delMe();
}

/**
* \brief 在成员索引里面查找成员迭代
* return 迭代指针
*/
std::map<std::string, CUnionMember *>::iterator  CUnionM::findMemberIndex(const char *pName)
{
        char temp_name[MAX_NAMESIZE];
        bzero(temp_name, MAX_NAMESIZE);
        strncpy(temp_name, pName, MAX_NAMESIZE);
        return memberIndex.find(temp_name);
}

/**
* \brief 根据名称获取帮会对象
* \param pName 帮会的名称
* \author fqnewman
* \return 成功返回帮会对象 失败返回NULL
*/
CUnion* CUnionM::getUnionByName(const char* pName)
{
	CUnion *ret = NULL;
	rwlock.rdlock();
	ret =(CUnion *)getEntryByName(pName);
	rwlock.unlock();
	return ret;
}

/**
* \brief 根据ID获取帮会对象
* \param dwUnionID 帮会的ID
* \author fqnewman
* \return 成功返回帮会对象，失败返回 NULL
*/
CUnion* CUnionM::getUnionByID(DWORD dwUnionID)
{
	CUnion* ret = NULL;
	rwlock.rdlock();
	ret = (CUnion*)getEntryByID(dwUnionID);
	rwlock.unlock();
	return ret;
}

/**
* \brief 根据实体名称删除管理实体
* \param name 实体的名称
* \author fqnewman
*/
void CUnionM::removeEntryByName(const char * name)
{
	zEntry *rm=getEntryByName(name);
	removeEntry(rm);
}

/**
* \brief 初始化帮会管理器，从数据库中加载所有的帮会数据
* \author fqnewman
* \return true 加载成功  false 加载失败
*/
bool CUnionM::init()
{
	FunctionTime func_time(0,__PRETTY_FUNCTION__,"加载所有帮会所需时间" , 32);
	
	/*
	   Cmd::Record::t_GetUnion_UnionRecord tCmd;
	   return RecordClientManager::getInstance().get().sendCmd(&tCmd, sizeof(tCmd));
	 */
	const dbCol union_read_define[] = {
		{ "UNIONID",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "NAME",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "CHARID",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "MASTER",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`VOTE`",			zDBConnPool::DB_BYTE,	sizeof(BYTE) }, 
		{ "`CREATETIME`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ "LEVEL",			zDBConnPool::DB_WORD,	sizeof(WORD) }, 
		{ "EXP",			zDBConnPool::DB_QWORD,	sizeof(QWORD) },
		{ "`MANA`",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`COUNTRYID`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`ACTIONPOINT`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`MONEY`",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`NOTE`",			zDBConnPool::DB_STR,	sizeof(char[254+1]) },
		{ "`CALLTIMES`",	zDBConnPool::DB_DWORD,  sizeof(DWORD) },
		{ NULL, 0, 0}
	};

	stUnionInfo *unionList,*tempPoint;
	stUnionInfo info;

	unionList = NULL;
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	unsigned int retcode = SessionService::dbConnPool->exeSelect(handle, "`UNION`", union_read_define, NULL, NULL, (BYTE **)&unionList);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode
			|| 0 == retcode)
	{
		return true;
	}

	if (unionList)
	{
		tempPoint = &unionList[0];
		for (unsigned int i=0; i< retcode; i++)
		{
			bcopy(tempPoint, &info, sizeof(stUnionInfo));
			CUnion *pUnion = createUnionByDBRecord(info);
			pUnion->loadUnionMemberFromDB();
			//if ((CREATE_UNION_NEED_MAN_NUM <= pUnion->size()) && (pUnion->isVote())) 
			//{
				//pUnion->letVoteOver();
			//}
			tempPoint++;
		}
		SAFE_DELETE_VEC(unionList);
		return true;
	}
	else
	{
		Zebra::logger->error("帮会数据初始化失败，exeSelect 返回无效buf指针");
	}
	return false;
}

/**
* \brief 初始化帮会对象并添加帮主成员 （createNewUnion()调用此方法）
* \param info 帮会的数据结构
* \author fqnewman
* \return 创建并初始化好的帮会对象
*/
CUnion* CUnionM::createUnionAndAddMaster(const stUnionInfo & info)
{
	CUnion *pUnion = NULL;

	pUnion = new CUnion();
	pUnion->init(info);
	pUnion->master = pUnion->addUnionMaster(info);
	pUnion->master->insertDatabase();

	rwlock.wrlock();
	addEntry(pUnion);
	rwlock.unlock();

	if (pUnion->master->septid>0)
	{
		this->addNewSeptToUnion(pUnion->id, pUnion->master->septid);
	}

	pUnion->master->online(Cmd::UNION_MEMBER_STATUS_NEWMEMBER);

	if (pUnion->master)
	{   
		pUnion->master->sendUserUnionData();
		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pUnion->master->id);

		if (pUser)
		{
			pUser->unionid = pUnion->id;
		}

		pUnion->master->update_data();
	}

	Zebra::logger->trace("[帮会]:%s 帮会建立成功", pUnion->name);
	SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUnion->dwCountryID, 
			"恭喜 %s 帮会成立", pUnion->name);
	return pUnion;
}

/**
* \brief 根据数据库记录建立帮会对象，系统加载的时候使用
* \param info 帮会的数据结构
* \author fqnewman
* \return 创建成功的帮会对象指针
*/
CUnion* CUnionM::createUnionByDBRecord(const stUnionInfo & info)
{
	CUnion *pUnion = NULL;
	bool ret = false;

	pUnion = new CUnion();
	pUnion->init(info);
	rwlock.wrlock();
	ret = addEntry(pUnion);
	rwlock.unlock();
	
	if (!ret)
	{
		Zebra::logger->trace("[帮会]: %s(%u) 帮会添加进CUnionM管理器失败", pUnion->name, pUnion->id);
	}

	return pUnion;
}

/**
* \brief 开除指定的帮会成员
* \param master 行使开除权的帮会成员，一般是会长
* \param member 被开除的成员
* \author fqnewman
* \return 
*/
void CUnionM::fireUnionMember(UserSession * master, const char * member)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember=NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(master->name);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	
	if (pUnionMember) 
	{
		CUnion *pUnion = pUnionMember->myUnion;
		
		if (pUnion)
		{
			pUnion->fireUnionMember(master->name, member);
		}
	}
	else
	{
		master->sendSysChat(Cmd::INFO_TYPE_FAIL, "你没有加入帮会");
	}
}

/**
  * \brief 删除帮会成员
  *
  * 如果该角色不是帮会会长，则踢除他，如果是，则保留，不做处理
  *
  * \param dwUserID 角色ID
  *
  * \return 如果该角色不在任何帮会中，则返回2
  *         如果该角色是帮主，则返回0
  *         如果该角色是帮众，并删除成功，则返回1
  *         如果是帮主，并且删除，返回3
  *         如果该角色是帮众，但删除失败，则返回-1
  *
  */
int  CUnionM::fireUnionMember(DWORD dwUserID, bool find)
{
    struct findList : public execEntry<CUnion>
	{
		DWORD _dwUserID;
		int _status;
		CUnion* _pUnion;

		findList(DWORD dwUserID)
		{
			_dwUserID = dwUserID;
			_status = 2;
			_pUnion = NULL;
		}
		~findList(){}

		void removeUnionMember()
		{
			if (_status == 1)
			{
#ifdef _ZJW_DEBUG
				Zebra::logger->debug("%d 是帮众，解除其社会关系", _dwUserID);
#endif
				if (_pUnion)
				{
					_pUnion->fireUnionMemberLeave(_dwUserID);
				}
				else
				{
					_status = -1;
				}
			}

			if (_status == 3)
			{
				
#ifdef _ZJW_DEBUG
						Zebra::logger->debug("%d 是帮主，解除帮会", _dwUserID);
#endif
						if (_pUnion)
						{
							CUnionM::getMe().delUnion(_pUnion->id);
						}
						
				//_status = -1;
			}
		}

		bool exec(CUnion *pUnion)
		{
			if (pUnion)
			{
				if (pUnion->master)
				{
					if (pUnion->master->id == _dwUserID)
					{
#ifdef _ZJW_DEBUG
						Zebra::logger->debug("%d是帮主，解散帮会", _dwUserID);
#endif
						_pUnion = pUnion;
						_status = 3;
						return false;
					}
					else
					{
							if (pUnion->isMember(_dwUserID))
							{
#ifdef _ZJW_DEBUG
								Zebra::logger->debug("%d 是 %s 帮众，能解除社会关系", _dwUserID, pUnion->name);
#endif
								_pUnion = pUnion;
								_status = 1;
								return false;
							}
							else
							{
#ifdef _ZJW_DEBUG
								Zebra::logger->debug("%d 不是 %s 的帮众", _dwUserID, pUnion->name);
#endif
								_status = 2;
							}
					}
				}
				else
				{
					Zebra::logger->error("%s 没有帮主信息，请检查帮会信息的完整性。", pUnion->name);
				}
			}

			return true;
		}
	};

	findList myList(dwUserID);
	execEveryOne(myList);

	if (!find)
	{
		myList.removeUnionMember();
	}

	return myList._status;
}

/**
* \brief 增加新成员到帮会
* \param dwUnionID 帮会ID
* \param info 新成员的信息结构
* \author fqnewman
* \return true 成功 false 失败
*/
bool CUnionM::addNewMemberToUnion(const DWORD dwUnionID, const stUnionMemberInfo& info)
{
	CUnion * pUnion = NULL;

	rwlock.rdlock();
	pUnion = (CUnion *)getEntryByID(dwUnionID);
	rwlock.unlock();
	if (pUnion && pUnion->tempid != info.dwCharID)
	{
		CUnionMember *pMember = pUnion->addUnionMember(info);
		if (pMember)
		{
			pMember->insertDatabase();
			pMember->online(Cmd::UNION_MEMBER_STATUS_NEWMEMBER);
			UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
			if (pUser)
			{
				pUser->unionid = pMember->myUnion->id;
			}

			pMember->sendUserUnionData(); 
			pMember->update_data();

			if (pUnion) pUnion->sendUnionNotify("欢迎 %s 加入帮会", pMember->name);

			return true;
		}
		else
		{
			Zebra::logger->error("无法将成员%u加入帮会%u中",info.dwCharID, dwUnionID);
		}
	}
	else 
	{
		Zebra::logger->error("数据不完整帮会管理器中没有%u帮会，而成员%u请求加入该帮会, 或者你已经是帮主,无需再加入",dwUnionID,info.dwCharID);
	}
	return false;
}

/**
* \brief 用户上线处理，如果上线用户是某个帮会的成员则做相应的上线初始化
* \param pUser 上线用户
* \author fqnewman
*/
void CUnionM::userOnline(UserSession * pUser)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;
	
	CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);
	if (pUnion && pUnion->dwCountryID>1 && (pUnion->dwCountryID != pUser->country))
	{
		CUnionM::getMe().fireUnionMember(pUser->id, false);
		return;
	}

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();

	if (pUnionMember)
	{
		//CDareM::getMe().userOnline(pUser);

		if (!pUnionMember->isOnline())
		{
			pUnionMember->online();
			if(pUnionMember->myUnion->isVote()) return;

			pUnionMember->sendUserUnionData();
		}

		pUnionMember->update_data();
		//if ((0 == pUser->unionid) && pUnionMember->user)
		if (pUser)
		{
			pUser->unionid = pUnionMember->myUnion->id;
		}
	}
	else
	{
		if (pUser->unionid!=0)
		{
			Cmd::Session::t_fireUnionMember_SceneSession send;
			send.dwCharID = pUser->id;
			send.dwMapTempID = pUser->scene->tempid;
			pUser->scene->sendCmd(&send,sizeof(Cmd::Session::t_fireUnionMember_SceneSession)); /// 通知场景服务器
			pUser->unionid = 0;
		}
	}
}

/**
* \brief 成员下线处理,将会判断指定的用户是否是帮会成员，如是做下线处理
* \param pUser 下线用户
* \author fqnewman
*/
void CUnionM::userOffline(const UserSession * pUser)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	if (pUnionMember)		pUnionMember->offline();
}

/**
* \brief 建立新的帮会对象
* \param data 帮会创建消息
* \author fqnewman
*/
void CUnionM::createNewUnion(Cmd::Session::t_addUnion_SceneSession *data)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(data->info.masterName);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	if (pUnionMember)
	{
		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pUnionMember->id);
		if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不能再创立帮会了！");
		return;
	}

	zRTime ctv;
	data->info.dwCrTime = ctv.sec();
	if (createUnionDBRecord(data->info))  /// 帮会数据库记录创建工作
	{
		/*CUnion *pUnion = */createUnionAndAddMaster(data->info);  /// 初始化帮会管理器中的帮会对象

		data->byRetcode =1;            /// 创建返回时：0 表示创建失败名称重复， 1表示成功
		SceneSession * pScene = SceneSessionManager::getInstance()->getSceneByTempID(data->dwMapTempID);
		if (pScene)
		{
			pScene->sendCmd(data,sizeof(Cmd::Session::t_addUnion_SceneSession)); /// 通知场景服务器
		}
		//because vote
		//if (pUnion) pUnion->sendUserUnionData(data->info.masterName);
	}
	else
	{
		data->byRetcode =0;            /// 创建返回时：0 表示创建失败名称重复， 1表示成功
		SceneSession * pScene = SceneSessionManager::getInstance()->getSceneByTempID(data->dwMapTempID);
		if (pScene)
		{
			pScene->sendCmd(data,sizeof(Cmd::Session::t_addUnion_SceneSession)); /// 通知场景服务器
		}
	}
}

/**
* \brief 创建新的帮会数据库记录
* \param info 新的帮会结构信息
* \author fqnewman
* \return true 成功  false 失败
*/
bool CUnionM::createUnionDBRecord(stUnionInfo& info)
{
	static const dbCol createunion_define[] = {
		{ "`NAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`MASTER`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`VOTE`",	zDBConnPool::DB_BYTE,	sizeof(BYTE) }, 
		{ "`CREATETIME`",zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`LEVEL`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ "`EXP`", zDBConnPool::DB_QWORD, sizeof(QWORD) },
		{ "`MANA`",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`COUNTRYID`",zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`ACTIONPOINT`",zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`MONEY`",zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`NOTE`",	zDBConnPool::DB_STR,	sizeof(char[254+1]) },
		{ NULL, 0, 0}
	};
	struct {
		char name[MAX_NAMESIZE+1];
		DWORD charid;
		char master[MAX_NAMESIZE+1];
		BYTE vote;
		DWORD dwCrTime;
		WORD level;
		QWORD EXP;
		DWORD dwMana;
		DWORD dwCountryID;
		DWORD dwActionPoint;
		DWORD dwMoney;
		char note[254+1];
	} __attribute__ ((packed))
	createunion_data;
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
	std::string escapeName;
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "NAME = '%s'", SessionService::dbConnPool->escapeString(handle,info.name,escapeName).c_str());
	unsigned int retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`UNION`", verifyname_define, where, NULL, 1, (BYTE *)(strName));
	if (retcode == 1)
	{
		SessionService::dbConnPool->putHandle(handle);
		return false;
	}

	//验证是否已有帮主记录,防止记录重复
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "CHARID = %d", info.dwCharID);
	retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`UNION`", verifymasterid_define, where, NULL, 1, (BYTE *)&dwMasterID);

	if (retcode == 1)
	{
		SessionService::dbConnPool->putHandle(handle);
		return false;
	}

	UserSession *pUser = NULL;
	pUser = UserSessionManager::getInstance()->getUserByID(info.dwCharID);

	//插入数据库角色信息
	bzero(&createunion_data, sizeof(createunion_data));
	if (pUser) 
	{
		createunion_data.dwCountryID = pUser->country;
	}
	else
	{
		createunion_data.dwCountryID = 0;
	}
	
	createunion_data.dwActionPoint = info.dwActionPoint;
	createunion_data.dwMoney = info.dwMoney;
	createunion_data.dwMana = info.dwMana;
	strncpy(createunion_data.name, info.name, MAX_NAMESIZE);
	strncpy(createunion_data.master, info.masterName, MAX_NAMESIZE);
	createunion_data.vote = info.byVote;
	createunion_data.dwCrTime = info.dwCrTime;
	createunion_data.charid = info.dwCharID;
	createunion_data.level = info.wdLevel;
	createunion_data.EXP = info.qwExp;
	strncpy(createunion_data.note, info.note, sizeof(createunion_data.note));
	retcode = SessionService::dbConnPool->exeInsert(handle, "`UNION`", createunion_define, (const unsigned char *)(&createunion_data));
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("创建帮会插入数据库出错 %u, %s", info.dwCharID, info.name);
		return false;
	}

	info.dwUnionID = retcode;
	return true;
}

/**
* \brief 添加成员索引
*
* \param pName 增加的成员的名称
* \param pUnionMember 成员对象
*
* \author fqnewman
* \return true 添加成功 false 添加失败
*/
bool CUnionM::addMemberIndex(const char *pName, CUnionMember *pUnionMember)
{
	std::pair<std::map<std::string, CUnionMember*>::iterator, bool> retval;
	char temp_name[MAX_NAMESIZE];
	bzero(temp_name, MAX_NAMESIZE);
	strncpy(temp_name, pName, MAX_NAMESIZE);
	
	rwlock.wrlock();
	retval = memberIndex.insert(memberIndexValueType(temp_name, pUnionMember));
	rwlock.unlock();
	return retval.second;
}

/**
* \brief 删除成员在索引中的记录
* \param pName 成员名称
* \author fqnewman
* \return true 删除成功 false 删除失败
*/
bool CUnionM::removeMemberIndex(const char *pName)
{
	bool ret;
	std::map <std::string, CUnionMember*>::iterator sIterator;
	rwlock.wrlock();
	ret = true;
	sIterator = findMemberIndex(pName);
	if (sIterator != memberIndex.end()) memberIndex.erase(sIterator);
	else ret = false;
	rwlock.unlock();
	return ret;
}

/**
* \brief 解散帮会
* \param dwUnionID 被解散的帮会的ID
* \author fqnewman
*/
void CUnionM::delUnion(const DWORD dwUnionID)
{
	CUnion *pUnion = NULL;
	pUnion = (CUnion *)getEntryByID(dwUnionID);
	if (pUnion)
	{
		//CDare* pDare = CDareM::getMe().findDareRecordByRelation(Cmd::UNION_DARE, dwUnionID, 0);
		//CDare* pUnionCityDare = CDareM::getMe().findDareRecordByRelation(Cmd::UNION_CITY_DARE, dwUnionID, 0);
		
		//if (pDare == NULL && pUnionCityDare == NULL)
		//{
			pUnion->sendUnionNotify("%s 帮解散", pUnion->name);
			Zebra::logger->trace("[帮会]:%s 帮解散", pUnion->name);

			//SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUnion->dwCountryID, 
			//		"很遗憾 %s 帮会解散", pUnion->name);

			rwlock.wrlock();
			removeEntry(pUnion);
			rwlock.unlock();
			pUnion->disbandUnion();
			SAFE_DELETE(pUnion);
		//}
		//else
		//{
		//	UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pUnion->master->id);
		//	if (pUnion->master && pUser)
		//	{
		//		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "帮会对战状态，不允许解散帮会！");
		//	}
			
		//}
	}
}

/**
* \brief 处理成员离会
* \param dwUnionID 帮会ID
* \param dwCharID 离开者的ID
* \author fqnewman,zjw
*/
void CUnionM::processMemberLeave(const DWORD dwUnionID, const DWORD dwCharID)
{
	CUnion *pUnion = NULL;
	pUnion = (CUnion *)getEntryByID(dwUnionID);
	if (pUnion)
	{
		CUnionMember *pMember = NULL;
		pMember = pUnion->master;
		if (pMember)
		{
			if (pMember->id != dwCharID)
			{
				UserSession *pUser = UserSessionManager::getInstance()->getUserByID(dwCharID);

				if (pUser)
				{
					CSept* pSept = CSeptM::getMe().getSeptByID(pUser->septid);

					if (pSept && pSept->master && pSept->master->id == dwCharID)
					{       
						pUnion->fireSeptFromUnion(pUser->septid);
					}
					/*else
					{
						pUnion->fireUnionMemberDirect(dwCharID, false);
					}*/
				}
			}
			else
			{
				if (pUnion->isDel())	
				{
					delUnion(dwUnionID);
				}
				else
				{
					UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pMember->id);
					if (pUser)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_MSG, 
								"你必须把帮主之位转让给其它族长，才能离开");
					}
				}
			}
		}
		else
		{
			Zebra::logger->error("帮会%s没有正确的会长对象，请检查数据的完整性", pUnion->name);
		}
	}
}

void CUnionM::processCancelCityCatcherMessage(UserSession* pUser, const Cmd::stCancelCityCatcherCmd* ptCmd)
{
	CUnion* pUnion = this->getUnionByID(pUser->unionid);
	
	if (pUnion && pUnion->tempid == pUser->id)
	{
		CCity* pCity = CCityM::getMe().findByUnionID(pUser->unionid);
		if (pCity)
		{
			pCity->cancelCatcher();
			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "取消捕头成功");
		}
	}
}

void CUnionM::processAppointCityCatcherMessage(UserSession* pUser, const Cmd::stAppointCityCatcherCmd* ptCmd)
{
	CUnion* pUnion = this->getUnionByID(pUser->unionid);
	
	if (pUnion && pUnion->tempid == pUser->id)
	{
		CCity* pCity = CCityM::getMe().findByUnionID(pUser->unionid);
		if (pCity)
		{
			UserSession *pCatcher = UserSessionManager::getInstance()->getUserSessionByName(ptCmd->name);
			if (pCatcher && pCatcher->scene)
			{
				if (pCatcher->id == pUser->id)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命自己为捕头");
					return;
				}
				
				if (pCatcher->country != pUser->country)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命外国人为捕头");
					return;
				}

				if (CCityM::getMe().isCastellan(pCatcher) || CCountryM::getMe().isOfficial(pCatcher))
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能任命城主或研究员为捕头");
					return;
				}
				
				// 判断是否已被任命为国家捕头和御史大夫
				CCountry* pCountry = CCountryM::getMe().find(pUser->country);
				if (pCountry && (strncmp(pCountry->diplomatName, pCatcher->name, MAX_NAMESIZE) == 0 ||
							strncmp(pCountry->catcherName, pCatcher->name, MAX_NAMESIZE) == 0))
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方已是捕头或御史大夫不能任命");
					return;
				}

				if (CCityM::getMe().isCatcher(pUser))
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方已是捕头不能任命");
					return;
				}

				Cmd::Session::t_checkUserCity_SceneSession send;
				send.dwCheckID = pUser->id;
				send.dwCheckedID = pCatcher->id;
				pCatcher->scene->sendCmd(&send, sizeof(send));
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方不在线,不能任命");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是城主,不能使用该功能");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是城主,不能使用该功能");
	}
}

void CUnionM::processUnionSortMessage(UserSession* pUser, const Cmd::stReqUnionSort* ptCmd)
{
	struct findList : public execEntry<CUnion>
	{
		DWORD _dwCountry;
		DWORD _byType;
		std::vector<CUnionSort> _vUnion;
		
		findList(DWORD dwCountry, BYTE byType)
		{       
			_dwCountry = dwCountry;
			_byType = byType;
		}
		~findList(){}

		void sendUnionSort(UserSession* pUser)
		{
			BYTE buf[zSocket::MAX_DATASIZE];
			Cmd::stUnionSortInfo* tempPoint;
			
			Cmd::stRtnUnionSort* retCmd = (Cmd::stRtnUnionSort*)buf;
			constructInPlace(retCmd);
			
			std::stable_sort(_vUnion.begin(), _vUnion.end());
			
			tempPoint = (Cmd::stUnionSortInfo *)retCmd->data;
			retCmd->byType = _byType;

			for (DWORD i=0; i<_vUnion.size(); i++)
			{
				if (i<10)
				{
					tempPoint->dwMana = _vUnion[i].dwMana;
					strncpy(tempPoint->unionName, _vUnion[i].unionName, MAX_NAMESIZE);
					tempPoint->dwCountryID = _vUnion[i].dwCountryID;
					tempPoint++;
					retCmd->dwSize++;	
				}
				else{
					break;
				}
			}
			
			pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stUnionSortInfo)+
						sizeof(Cmd::stRtnUnionSort)));
		}
			
		
		bool exec(CUnion *pUnion)
		{      
			if (pUnion && (_byType != Cmd::COUNTRY_UNION_SORT || pUnion->dwCountryID==_dwCountry))
			{
				CUnionSort temp;
				strncpy(temp.unionName, pUnion->name, MAX_NAMESIZE);

				temp.dwMana = pUnion->getMana();
				temp.dwOrder  = 0;
				temp.dwCountryID = pUnion->dwCountryID;
				_vUnion.push_back(temp);
			}
			
			return true;
		}
	};
	
	findList myList(pUser->country, ptCmd->byType);
	execEveryOne(myList);
	myList.sendUnionSort(pUser);
}

/**
* \brief 处理权限
* \param pUser 玩家
* \param ptNullCmd 权限命令
* \author zjw
*/
void CUnionM::processMemberPower(UserSession* pUser, const Cmd::stUnionMemberPower* ptCmd)
{
	Cmd::stUnionMemberPower send;

	CUnion* pUnion = this->getUnionByID(pUser->unionid);
	
	if (pUnion)
	{
		if (pUnion->dwCountryID>1 && pUser->country!=pUnion->dwCountryID)
		{
			pUnion->fireUnionMemberDirect(pUser->id, false);
			return;
		}
			
		if (pUnion->master->id != pUser->id)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "权限操作是帮主的专利哦！");
			return;
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "老大，你连帮会都还没建呢！");
		return;
	}

	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;
	rwlock.rdlock();
	sIterator = findMemberIndex(ptCmd->name);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	strncpy(send.name, ptCmd->name, sizeof(send.name));
			
	switch (ptCmd->byType)
	{
		case Cmd::QUESTION_UNION_MEMBER_POWER:
			{
				if (pUnionMember)
				{
					 send.byPower[0]=pUnionMember->byPower[0];
					 send.byPower[1]=pUnionMember->byPower[1];
					 send.byType = Cmd::RESPOND_UNION_MEMBER_POWER;

					 if (pUser) pUser->sendCmdToMe(&send, sizeof(send));
				}
			}
			break;
		case Cmd::SET_UNION_MEMBER_POWER:
			{
				if (pUnionMember)
				{
					//pUnionMember->byPower[0] = 0x00;
					//pUnionMember->byPower[1] = 0x00;
					if (pUnionMember->id == pUnionMember->myUnion->master->id)
					{//对帮主自己无需进行权限赋值,如果要转让帮主，把某一帮众设为帮主权限既可
						return;
					}

					CSept* pSept = CSeptM::getMe().getSeptByID(pUnionMember->septid);
					if (pSept==NULL || (pSept && pSept->master && pSept->master->id != pUnionMember->id))
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你只能对族长进行操作");
					}

										
					if (isset_state(ptCmd->byPower, Cmd::MASTER) 
						&& pUnionMember->id != pUnionMember->myUnion->id)
					{//改变帮主
						pUnion->changeMaster(pUnionMember);
					}
					else
					{
						pUnionMember->byPower[0] = 0x00;
						pUnionMember->byPower[1] = 0x00;

						// 默认，都要设置GENERAL权限
						set_state(pUnionMember->byPower, Cmd::GENERAL);

						/*if (isset_state(ptCmd->byPower, Cmd::SECOND))
						{//设置副帮主权限对应的权限
							SETSECONDPOWER(pUnionMember->byPower);
						}*/

						/*if (isset_state(ptCmd->byPower, Cmd::ADD_MEMBER))
						{
							pUnionMember->sendMessageToMe("帮主赋于你招人的权限");
							set_state(pUnionMember->byPower, Cmd::ADD_MEMBER);
						}*/

						if (isset_state(ptCmd->byPower, Cmd::CHANGE_ALIAS))
						{
							pUnionMember->sendMessageToMe("帮主赋于你更改他人称号的权限");
							set_state(pUnionMember->byPower, Cmd::CHANGE_ALIAS);
						}

						/*if (isset_state(ptCmd->byPower, Cmd::FIRE_MEMBER))
						{
							pUnionMember->sendMessageToMe("帮主赋于你踢人的权限");
							set_state(pUnionMember->byPower, Cmd::FIRE_MEMBER);
						}*/

						if (isset_state(ptCmd->byPower, Cmd::NOTIFY))
						{
							pUnionMember->sendMessageToMe("帮主赋于你更改公告的权限");
							set_state(pUnionMember->byPower, Cmd::NOTIFY);
						}
						
						if (isset_state(ptCmd->byPower, Cmd::DARE))
						{
							pUnionMember->sendMessageToMe("帮主赋于你挑战和接受挑战的权限");
							set_state(pUnionMember->byPower, Cmd::DARE);
						}


						pUnionMember->writeDatabase();
						pUnionMember->sendUserUnionData();
					}
				}
			}
			break;
		default:
			{
				return;
			}
			break;
	}

	return;
}

/**
* \brief 判断权限
* \param name 玩家
*

* \author zjw
*/
bool CUnionM::havePowerByName(const char* name, const int power)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(name);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	if (pUnionMember)
	{
		return pUnionMember->havePower(power);
	}

	return false;
}



/**
* \brief 发送帮会聊天消息
* \param pUser 消息发送者
* \param pCmd 聊天消息体
* \param cmdLen 消息长度
* \author fqnewman
*/
void CUnionM::sendUnionChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *pCmd, const unsigned int cmdLen)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pCmd->pstrName);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	if (pUnionMember)
	{
		if(pUnionMember->myUnion)
		{
			pUnionMember->myUnion->sendCmdToAllMember(pCmd,cmdLen);
		}
	}
	else
	{
		if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "必须先加入帮会才能使用帮会聊天");
	}
}

/**
* \brief 发送帮会通知
*
* \param unionID 帮会ID
* \param message 消息长度
*
* \author zjw
*/
void CUnionM::sendUnionNotify(const DWORD unionID, const char* message, ...)
{
	CUnion *pUnion = NULL;
	char buf[1024+1];
	bzero(buf, sizeof(buf));
	
	if (message==NULL)
	{
		return;
	}
	
	pUnion = this->getUnionByID(unionID);
	getMessage(buf, 1024, message);
	
	Cmd::stChannelChatUserCmd send;
	send.dwType=Cmd::CHAT_TYPE_SYSTEM;
	send.dwSysInfoType = Cmd::INFO_TYPE_EXP;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));

	sprintf((char*)send.pstrName, "帮会通知");
	sprintf((char*)send.pstrChat, "%s", buf);

	if (pUnion)
	{
		pUnion->sendCmdToAllMember(&send, sizeof(send));
	}
	else
	{
		Zebra::logger->error("发送帮会通知时，未找到帮会:%d", unionID);
	}
}
/**
* \brief 发送帮会私聊信息
* \param pUser 消息发送者
* \param pCmd 聊天消息体
* \param cmdLen 消息长度
* \author fqnewman
*/
void CUnionM::sendUnionPrivateChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *pCmd, const unsigned int cmdLen)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pCmd->pstrName);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	if (pUnionMember)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::stChannelChatUserCmd *chatCmd;

		chatCmd = (Cmd::stChannelChatUserCmd *)buf;
		memcpy(chatCmd,pCmd,cmdLen);
		strncpy(chatCmd->pstrName, pUser->name,MAX_NAMESIZE);

		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pUnionMember->id);
		if (pUser)
		{
			if (!pUnionMember->myUnion->isVote()) pUser->sendCmdToMe(chatCmd, cmdLen);
		}
		else
		{
			COfflineMessage::writeOfflineMessage(chatCmd->dwType, pUnionMember->id, chatCmd,cmdLen);
		}
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
bool CUnionM::processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch(ptNullCmd->byCmd)
	{
		case Cmd::UNION_USERCMD:
			{
				switch(ptNullCmd->byParam)
				{
					case CANCEL_CITY_CATCHER_PARA:
						{
							Cmd::stCancelCityCatcherCmd* rev = (Cmd::stCancelCityCatcherCmd*)ptNullCmd;
							this->processCancelCityCatcherMessage(pUser, rev);

							return true;
						}
						break;
					case APPOINT_CITY_CATCHER_PARA:
						{
							Cmd::stAppointCityCatcherCmd* rev = (Cmd::stAppointCityCatcherCmd*)ptNullCmd;
							this->processAppointCityCatcherMessage(pUser, rev);
							return true;
						}
						break;
					case Cmd::REQ_UNION_NAME_LIST_PARA:
						{
							//FunctionTimes times(108, __FUNCTION__);
							Cmd::stReqUnionNameLlistCmd* rev = (Cmd::stReqUnionNameLlistCmd*)ptNullCmd;
							char buffer[zSocket::MAX_DATASIZE];
							Cmd::stRtnUnionNameListCmd *send = (Cmd::stRtnUnionNameListCmd*)buffer;
							constructInPlace(send);
							int i=0;
							while(i < rev->num && i <= 200)
							{
								CUnion* pUnion = this->getUnionByID(rev->dwUnionID[i]);

								if (pUnion)
								{
#ifdef _ZJW_DEBUG
									Zebra::logger->debug("[优化]: %s 请求 %s 帮会名称", pUser->name, pUnion->name);
#endif								
									strncpy(send->list[send->num].name, pUnion->name, MAX_NAMESIZE);
									send->list[send->num].dwUnionID = pUnion->id;
									send->num ++;
								}
								i ++; 
							}
							if(send->num)
							{
								pUser->sendCmdToMe(send, sizeof(Cmd::stRtnUnionNameListCmd) + (send->num * sizeof(send->list[0])));
							}
							return true;
						}
						break;
					case Cmd::REQ_UNION_NAME_PARA:
						{
							Cmd::stReqUnionNameCmd* rev = (Cmd::stReqUnionNameCmd*)ptNullCmd;
							
							Cmd::stRtnUnionNameCmd send;
							CUnion* pUnion = this->getUnionByID(rev->dwUnionID);
							//FunctionTimes times(107, __FUNCTION__);
							
							if (pUnion)
							{
#ifdef _ZJW_DEBUG
								Zebra::logger->debug("[优化]: %s 请求 %s 帮会名称", pUser->name, pUnion->name);
#endif								
								strncpy(send.name, pUnion->name, MAX_NAMESIZE);
								send.dwUnionID = rev->dwUnionID;
								pUser->sendCmdToMe(&send, sizeof(send));
							}
							
							return true;
						}
						break;
					case Cmd::REQUEST_ABJURATION_CITY_PARA:
						{
							CCity* pCity = CCityM::getMe().findByUnionID(pUser->unionid);
							CUnion* pUnion = this->getUnionByID(pUser->unionid);
							
							if (pUnion && pUnion->tempid == pUser->id)
							{
								if (pCity)
								{
									pCity->abandonCity();
								}
							}
							else
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
										"你不是帮主，不能使用该功能");
							}

							return true;
						}
						break;
					case Cmd::REQ_UNION_MONEY_PARA:
						{
							CUnion* pUnion = this->getUnionByID(pUser->unionid);
							
							if (pUnion)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_MSG, 
										"%s 帮会当前资金为:%d 文", pUnion->name,
										pUnion->dwMoney);
							}

							return true;
						}
						break;
					case Cmd::REQ_UNION_SORT_PARA:
						{
							Cmd::stReqUnionSort* ptCmd = (Cmd::stReqUnionSort*)ptNullCmd;
							processUnionSortMessage(pUser, ptCmd);
							return true;
						}
						break;
					case Cmd::UNION_MEMBER_POWER_PARA:
						{
							Cmd::stUnionMemberPower* ptCmd = (Cmd::stUnionMemberPower*)ptNullCmd;
							processMemberPower(pUser, ptCmd);

							return true;
						}
						break;
					case Cmd::FIRE_MEMBER_FROM_UNION_PARA:
						{
							Cmd::stFireMemberFromUnionCmd *ptCmd=(Cmd::stFireMemberFromUnionCmd *)ptNullCmd;
							fireUnionMember(pUser,ptCmd->memberName);
							return true;
						}
						break;
					case Cmd::UNIONMEMBER_LEAVE_UNION_PARA:
						{
							//sky 国战离开帮会判断
							//if (Cmd::Session::COUNTRY_FORMAL_DARE  || Cmd::Session::COUNTRY_FORMAL_ANTI_DARE || Cmd::Session::EMPEROR_DARE )
							CDare* pDare = CDareM::getMe().findDareRecordByID(
							Cmd::COUNTRY_FORMAL_DARE, pUser->country);
							if(pDare)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "国战期间不能离开帮会!");
								return false;
							}
							else
							{
								processMemberLeave(pUser->unionid, pUser->id);
								return true;
							}
						}
						break;
					case Cmd::LIST_UNION_PARA:
						{
							sendListToUser(pUser);
							return true;
						}
						break;
					case Cmd::REQUEST_JOIN_UNION_PARA:
						{
							Cmd::stRequestJoinUnionCmd *ptCmd=(Cmd::stRequestJoinUnionCmd *)ptNullCmd;
							switch(ptCmd->status)
							{
								case Cmd::REQUEST_JOIN_OK:
									{
										UserSession *master = UserSessionManager::getInstance()->getUserSessionByName(ptCmd->name);
										if (master)
										{
											strncpy(ptCmd->name, pUser->name, MAX_NAMESIZE);
											master->sendCmdToMe(ptCmd, sizeof(Cmd::stRequestJoinUnionCmd));
										}
										else
										{
											pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "会长%s目前不在线", ptCmd->name);
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
											tUser->sendCmdToMe(ptCmd, sizeof(Cmd::stRequestJoinUnionCmd));
										}
									}
									break;
							}

							return true;
						}
						break;
					case Cmd::VOTELIST_UNION_PARA:
						{
							/*if (pUser->level<10)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
								"你等级不够10级不能参与投票");
								return true;
							}*/

							sendVoteListToUser(pUser);
							return true;
						}
						break;
					case Cmd::VOTE_UNION_PARA:
						{
							if (pUser->level<10)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你等级不够10级不能参与签名");
								return true;
							}

							Cmd::stVoteUnionCmd *ptCmd=(Cmd::stVoteUnionCmd *)ptNullCmd;
							if (ptCmd->bySuccess)
							{
								userVote(pUser, ptCmd->unionName);
							}
							else
							{
								userAboutVote(pUser, ptCmd->unionName);
							}
							return true;
						}
						break;
					case Cmd::NOTE_UNION_PARA:
						{
							Cmd::stNoteUnionCmd *ptCmd=(Cmd::stNoteUnionCmd *)ptNullCmd;
							setUnionNote(pUser, ptCmd);
							return true;
						}
						break;
					case Cmd::CHANGE_UNION_MEMBER_ALIASNAME_PARA:
						{
							Cmd::stChangeUnionMemberAliasName* ptCmd=
								(Cmd::stChangeUnionMemberAliasName*)ptNullCmd;

							change_aliasname(pUser, ptCmd);
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
	return false;
}

/**
* \brief 处理从场景过来的消息
* \param cmd 消息体
* \param cmdLen 消息长度
* \author fqnewman
* \return true 处理成功 false 消息不在处理范围之内
*/
bool CUnionM::processSceneUnionMessage(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	switch(cmd->para)
	{
		case Cmd::Session::PARA_CHECK_USER_CITY:
			{
				Cmd::Session::t_checkUserCity_SceneSession* rev = 
					(Cmd::Session::t_checkUserCity_SceneSession*)cmd;
				UserSession* pCatcher  = UserSessionManager::getInstance()->getUserByID(rev->dwCheckedID);
				UserSession* u  = UserSessionManager::getInstance()->getUserByID(rev->dwCheckID);

				if (pCatcher && u) 
				{       
					CCity* pCity = CCityM::getMe().findByUnionID(u->unionid);

					if (pCity)
					{       
						pCity->changeCatcher(pCatcher);

						if (u)  
						{       
							u->sendSysChat(Cmd::INFO_TYPE_MSG, "成功任命 %s 为捕头", pCatcher->name);
						}       
					}      
				} 
				return true;
			}
			break;
		case Cmd::Session::OP_UNION_ACTION_PARA:
			{
				Cmd::Session::t_OpUnionAction_SceneSession* ptCmd = 
					(Cmd::Session::t_OpUnionAction_SceneSession*)cmd;

				CUnion* pUnion = this->getUnionByID(ptCmd->dwUnionID);
				if (pUnion)
				{
					pUnion->changeActionPoint(ptCmd->dwAction);
				}
				return true;
			}
			break;
		case Cmd::Session::OP_UNION_MONEY_PARA:
			{
				Cmd::Session::t_OpUnionMoney_SceneSession* ptCmd = 
					(Cmd::Session::t_OpUnionMoney_SceneSession*)cmd;

				CUnion* pUnion = this->getUnionByID(ptCmd->dwUnionID);
				if (pUnion)
				{
					pUnion->changeMoney(ptCmd->dwMoney);
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
bool CUnionM::processSceneMessage(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	switch(cmd->para)
	{
		case Cmd::Session::PARA_QUESTION_UNION_CITY:
			{
				Cmd::Session::t_questionUnionCity_SceneSession* ptCmd = 
					(Cmd::Session::t_questionUnionCity_SceneSession*)cmd;

				UserSession* pUser = UserSessionManager::getInstance()->getUserByID(ptCmd->dwUserID);
				
				if (pUser)
				{
					CCity* pCity = CCityM::getMe().find(ptCmd->dwCountryID, ptCmd->dwCityID);

					if (pCity == NULL)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "该城市未被任何帮会占领");
						return true;
					}

					CUnion* pUnion = this->getUnionByID(pCity->dwUnionID);

					if (ptCmd->byType == Cmd::QUESTION_CITY_HOLD)
					{
						if (pUser) 
						{
							if (pUnion)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "本城属于 %s 帮会", 
										pUnion->name);
							}
							else
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "目前本城还未被任何帮会占领");
							}
						}
					}
					else if (ptCmd->byType == Cmd::QUESTION_CITY_DARE)
					{
						if (pUser) 
						{
							if (pCity->dareSize()>0)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "今天夺城战挑战帮会数为: %d 个帮",
										pCity->dareSize());
							}
							else
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "今天尚未有任何帮会前来挑战");
							}
						}
					}
				}
				
				return true;
			}
			break;
		case Cmd::Session::PARA_ENTER_UNION_MANOR:
			{
				Cmd::Session::t_enterUnionManor_SceneSession* ptCmd = 
					(Cmd::Session::t_enterUnionManor_SceneSession*)cmd;
				
				Cmd::Session::t_returnEnterUnionManor_SceneSession send;
				UserSession* pUser = UserSessionManager::getInstance()->getUserByID(ptCmd->dwUserID);
				
				if (pUser)
				{
					CCity* pOwnCity = CCityM::getMe().findByUnionID(pUser->unionid);
					CCity* pCurCity = CCityM::getMe().find(ptCmd->dwCountryID, ptCmd->dwCityID);

					if (pOwnCity)
					{
						if (ptCmd->dwCountryID == pOwnCity->dwCountry
						&& ptCmd->dwCityID == pOwnCity->dwCityID)
						{// 在帮会所占城市,跳向帮会自己的密室.
							send.dwUserID = pUser->id;
							send.dwCountryID = pOwnCity->dwCountry;
						  //////////////////////////////////
						  send.dwAreaID = pOwnCity->dwCityID*10+1;
							//send.dwAreaID = 135;
						  //////////////////////////////////

							if (pUser->scene) pUser->scene->sendCmd(&send, sizeof(send));
						}
						else
						{// 不在帮会所占城市, 则判断是否与帮会所占城市ID相同.
						 // 如果是则属于攻占了国外城市
							if (ptCmd->dwCityID == pOwnCity->dwCityID)
							{
								send.dwUserID = pUser->id;
								send.dwCountryID = pOwnCity->dwCountry;
							  //////////////////////////////
							  send.dwAreaID = pOwnCity->dwCityID*10+1;
								//send.dwAreaID = 135;
							  //////////////////////////////	

								if (pUser->scene) pUser->scene->sendCmd(&send, sizeof(send));
							}
							else
							{//既不在自己所占城市,也不在自己所占国外城市相应的城市
							// 则是想跳入别人所占城市的密室,判断该城市是否在打夺城战
							// 只准参战两国的帮会成员进入,暂时未判断
								if (pCurCity==NULL)
								{
									return true;
								}

								CDare* pDare = CDareM::getMe().
							findDareRecordByID(Cmd::UNION_CITY_DARE, pCurCity->dwUnionID);

								if (pDare
										&& pDare->secondID == pCurCity->dwUnionID
										&& pDare->state == CDare::DARE_ACTIVE)
								{//对战对象存在
									send.dwUserID = pUser->id;
									send.dwCountryID = pCurCity->dwCountry;
								  ////////////////////////////
								  send.dwAreaID = pCurCity->dwCityID*10+1;
									//send.dwAreaID = 135;
								  ///////////////////////////	

									if (pUser->scene) pUser->scene->sendCmd(&send, sizeof(send));
								}
								else
								{
									pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
									"该城不属于贵帮，或未进入对战期");
								}
							}
						}
					}
					else
					{// 自己没有城,要进入当前城的密室.则判断当前该城是否在夺城战期间
						// 自己没城,则寻找有没有自己帮会参加的夺城战,如果有,则取出守方,所在城市
						// 然后,判断守方所在城市,是否为当前自己所在城市ID相符,相符,则跳入

						CDare* pDare = CDareM::getMe().
							findDareRecordByID(Cmd::UNION_CITY_DARE, pUser->unionid);

						if (pDare)
						{
							CCity* pDefCity = CCityM::getMe().findByUnionID(pDare->secondID);

							if (pDefCity && pDare->isAtt(pUser->unionid) 
									&& pDefCity->dwCityID == ptCmd->dwCityID
									&& pDare->state == CDare::DARE_ACTIVE)
							{
								send.dwUserID = pUser->id;
								send.dwCountryID = pDefCity->dwCountry;
							  //////////////////////////////
							  send.dwAreaID = pDefCity->dwCityID*10+1;
								//send.dwAreaID = 135;
							  //////////////////////////////

								if (pUser->scene) pUser->scene->sendCmd(&send, sizeof(send));
							}
							else
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
										"该城不属于贵帮，或未进入对战期");
							}
						}
					}
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_UNION_ADDMEMBER:
			{
				Cmd::Session::t_addUnionMember_SceneSession *ptCmd=(Cmd::Session::t_addUnionMember_SceneSession *)cmd;
				std::map<std::string, CUnionMember *>::iterator sIterator;
				CUnionMember *pUnionMember = NULL;

				rwlock.rdlock();        
				sIterator = findMemberIndex(ptCmd->member.name);
				if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
				rwlock.unlock();
				
				CUnion * pUnion = NULL;

				rwlock.rdlock();
				pUnion = (CUnion *)getEntryByID(ptCmd->dwUnionID);
				rwlock.unlock();
				if (pUnion == NULL)
				{
					return true;
				}

				if (pUnionMember != NULL && pUnionMember->myUnion && pUnionMember->myUnion->isVote())
				{
					UserSession *pUser = UserSessionManager::getInstance()->getUserByID(pUnion->master->id);
					if (pUnion && pUnion->master && pUser)
					{

						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方已签名，不能招收");
					}

					pUser = UserSessionManager::getInstance()->getUserByID(pUnionMember->id);
					if (pUnionMember && pUser)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已签过名，需放弃后才能加入另外的帮会");
					}
				}
				else
				{
					if (!pUnionMember)
					{
						UserSession *pUser = UserSessionManager::getInstance()->
							getUserByID(pUnion->master->id);

						UserSession* pAddUser = UserSessionManager::getInstance()->
							getUserByID(ptCmd->member.dwCharID);

						if (pUnion && pUnion->master && pUser)	
						{
							pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "%s 接受了你的邀请加入了帮会", ptCmd->member.name);
						}

						if (pAddUser)
						{
							addNewSeptToUnion(ptCmd->dwUnionID, pAddUser->septid);
						}
					}
				}

				return true;
			}
			break;
		case Cmd::Session::PARA_UNION_ADDUNION:
			{
				Cmd::Session::t_addUnion_SceneSession *ptCmd=(Cmd::Session::t_addUnion_SceneSession *)cmd;
				createNewUnion(ptCmd);
				return true;
			}
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
bool CUnionM::processGateMessage(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	switch(cmd->para)
	{
		case Cmd::Session::PARA_UNION_DISBAND:
			{
				Cmd::Session::t_disbandUnion_GateSession *ptCmd=(Cmd::Session::t_disbandUnion_GateSession *)cmd;
				processMemberLeave(ptCmd->dwUnionID, ptCmd->dwCharID);
				return true;
			}
			break;
		default:
			break;
	}

	return false;
}

/**
* \brief 发送当前系统中在等待投票的帮会列表给指定的用户
* \param pUser 请求投票列表者
* \author fqnewman
*/
void CUnionM::sendVoteListToUser(const UserSession *pUser)
{
	/*
	struct findList : public execEntry<CUnion>
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::stVoteListUnionCmd *retCmd;
		Cmd::stUnionVoteRecord *tempPoint;
		std::vector<DWORD> removed;
		UserSession *user;
		findList(char *pName,UserSession *pUser)
		{
			user = pUser;
			retCmd=(Cmd::stVoteListUnionCmd *)buf;
			constructInPlace(retCmd);
			tempPoint = (Cmd::stUnionVoteRecord *)retCmd->data;
			retCmd->dwSize = 0;
			retCmd->flag = 0;

			strncpy(retCmd->unionName,pName, MAX_NAMESIZE);
		}
		~findList(){}

		void sendList(const UserSession *pUser)
		{
			pUser->sendCmdToMe(retCmd,(retCmd->dwSize*sizeof(Cmd::stUnionVoteRecord)+sizeof(Cmd::stVoteListUnionCmd)));
		}

		void removeInvalidUnion()
		{
			std::vector<DWORD>::iterator tIterator;

			for(tIterator = removed.begin(); tIterator != removed.end(); tIterator++)
			{
				CUnionM::getMe().delUnion(*tIterator);
			}
		}

		bool exec(CUnion *punion)
		{
			if (punion && punion->isVote())
			{
				zRTime ctv;
				if (ctv.sec() - punion->dwCreateTime > DESTROYTIME)
				{
					removed.push_back(punion->id);
					return true;
				}
				

				if (retCmd->dwSize>100)
				{
					this->sendList(user);
					char tName[MAX_NAMESIZE];
					strncpy(tName, retCmd->unionName, MAX_NAMESIZE);
					tempPoint = (Cmd::stUnionVoteRecord *)retCmd->data;
					retCmd->dwSize = 0;
					retCmd->flag=1;
				}

				if (user && punion->dwCountryID == user->country)
				{// 只发送本国帮会的投票列表
					strncpy(tempPoint->unionName,punion->name,MAX_NAMESIZE);
					if (punion->master)
						strncpy(tempPoint->master, punion->master->name,MAX_NAMESIZE);
					else
						strncpy(tempPoint->master, "未知",MAX_NAMESIZE);
					tempPoint->wdVoteNumber = punion->size();
					strncpy(tempPoint->note, punion->note, 255));
					tempPoint++;
					retCmd->dwSize++;
				}
			}
			return true;
		}
	};
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;
	char tempName[MAX_NAMESIZE];

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();

	bzero(tempName, MAX_NAMESIZE);
	if (pUnionMember)
	{
		if (pUnionMember->myUnion->isVote())
		{
			strncpy(tempName, pUnionMember->myUnion->name, MAX_NAMESIZE);
		}
	}

	findList myList(tempName, (UserSession *)pUser);
	execEveryOne(myList);
	myList.sendList(pUser);
	myList.removeInvalidUnion();
	*/
}

/**
* \brief 发送当前系统中在等待投票的帮会列表给指定的用户
* \param pUser 请求投票列表者
* \author fqnewman
*/
void CUnionM::sendListToUser(const UserSession *pUser)
{
	struct findList : public execEntry<CUnion>
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::stListUnionCmd *retCmd;
		Cmd::stUnionRecord *tempPoint;
		UserSession *user;
		findList(UserSession *pUser)
		{
			user = pUser;
			retCmd=(Cmd::stListUnionCmd *)buf;
			constructInPlace(retCmd);
			tempPoint = (Cmd::stUnionRecord *)retCmd->data;
			retCmd->dwSize = 0;
			retCmd->flag = 0;
		}
		~findList(){}

		void sendList(const UserSession *pUser)
		{
			pUser->sendCmdToMe(retCmd,(retCmd->dwSize*sizeof(Cmd::stUnionRecord)+sizeof(Cmd::stListUnionCmd)));
		}

		bool exec(CUnion *punion)
		{
			if (punion)
			{
				if (retCmd->dwSize>100)
				{
					this->sendList(user);
					tempPoint = (Cmd::stUnionRecord *)retCmd->data;
					retCmd->dwSize = 0;
					retCmd->flag=1;
				}

				if (user && punion->dwCountryID == user->country)
				{// 只发送本国帮会的投票列表
					strncpy(tempPoint->unionName,punion->name,MAX_NAMESIZE);
					if (punion->master)
						strncpy(tempPoint->master, punion->master->name,MAX_NAMESIZE);
					else
						strncpy(tempPoint->master, "未知",MAX_NAMESIZE);
					//tempPoint->wdVoteNumber = punion->size();
					strncpy(tempPoint->note, punion->note, 255);
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
void CUnionM::sendDareListToUser(const UserSession *pUser)
{
	struct findList : public execEntry<CUnion>
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
			send->byType = Cmd::UNION_DARE;
			_pUser = pUser;
			
		}
		~findList(){}

		void sendList()
		{
			if (_pUser) _pUser->sendCmdToMe(send,(send->dwSize*sizeof(Cmd::stDareList)+sizeof(Cmd::stSendDareList)));
		}

		bool exec(CUnion *punion)
		{
			if (_pUser)
			{
				if (punion && punion->master)
				{
					UserSession *pUser = UserSessionManager::getInstance()->getUserByID(punion->master->id);
					if (pUser && pUser->country==_pUser->country 
							&& punion->id!=_pUser->unionid)
					{

						strncpy(tempPoint->name, punion->name, MAX_NAMESIZE);
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
* \brief 用户投票给指定的帮会
* \param pUser 投票者
* \param pName 帮会名称
* \author fqnewman
*/
void CUnionM::userVote(const UserSession *pUser, const char *pName)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	if (pUnionMember)
	{

		if (pUser)
		{
			if (pUnionMember->myUnion->isVote())
			{
				if (pUnionMember == pUnionMember->myUnion->master)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你是%s帮的预备会长不能再签名", pUnionMember->myUnion->name);
				}
				else
				{
					if (strncmp(pName,pUnionMember->myUnion->name,MAX_NAMESIZE)!=0)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已对 %s 帮签名，你之前对%s帮的签名将被作废.",pName, pUnionMember->myUnion->name);
						processMemberLeave(pUnionMember->myUnion->id, pUser->id);
						addVoteMemberToUnion(pUser,pName);
						return;
					}
				}
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已经加入其他帮会了，没有签名的资格");
			}
		}
		else
		{
			Zebra::logger->error("CUnionM::userVote():一个在线的帮会成员没有有效的用户指针");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你对%s帮进行了签名", pName);
		addVoteMemberToUnion(pUser,pName);
		return;
	}

	// 通知投票失败
	Cmd::stVoteUnionCmd send;
	strncpy(send.unionName,pName,MAX_NAMESIZE);
	send.bySuccess = 0;
	pUser->sendCmdToMe(&send, sizeof(send));
}

/**
* \brief 用户放弃投票
* \param pUser 放弃投票的成员
* \param pName 被放弃帮会名称
* \author fqnewman
* \return 
*/
void CUnionM::userAboutVote(const UserSession *pUser, const char *pName)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	if (pUnionMember)
	{

		if (pUser)
		{
			if (pUnionMember->myUnion->isVote())
			{
				if (pUnionMember == pUnionMember->myUnion->master)
				{
					
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你是%s帮的预备帮主, 放弃签名，帮会解散", 
							pUnionMember->myUnion->name);
					
					this->processMemberLeave(pUnionMember->myUnion->id, pUser->id);
					sendVoteListToUser(pUser);
					return;
				}
				else
				{
					if (strncmp(pName,pUnionMember->myUnion->name,MAX_NAMESIZE)==0)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你放弃了%s帮的签名.",pName);
						processMemberLeave(pUnionMember->myUnion->id, pUser->id);

						Cmd::stVoteUnionCmd send;
						strncpy(send.unionName,pName,MAX_NAMESIZE);
						send.bySuccess = 2;
						pUser->sendCmdToMe(&send, sizeof(send));
						return;
					}
				}
			}
			else
			{
				Zebra::logger->error("[%s]帮会已经成立，但是签名还没取消",pName);
			}
		}
		else
		{
			Zebra::logger->error("CUnionM::userVote():一个在线的帮会成员没有有效的用户指针");
		}
	}

	// 通知投票失败
	Cmd::stVoteUnionCmd send;
	strncpy(send.unionName,pName,MAX_NAMESIZE);
	send.bySuccess = 0;
	pUser->sendCmdToMe(&send, sizeof(send));
}

/**
* \brief 投票给指定的帮会
* \param pUser 投票者
* \param pName 帮会名称
* \author fqnewman
*/
void CUnionM::addVoteMemberToUnion(const UserSession *pUser, const char *pName)
{
	rwlock.rdlock();
	CUnion *myUnion = (CUnion *)getEntryByName(pName);
	rwlock.unlock();

	if (myUnion)
	{
		if (myUnion->isVote())
		{
			stUnionMemberInfo member;
			member.dwCharID = pUser->id;
			strncpy(member.name, pUser->name, MAX_NAMESIZE);
			member.wdOccupation = pUser->occupation;
			addNewMemberToUnion(myUnion->id, member);

			Cmd::stVoteUnionCmd send;
			strncpy(send.unionName,pName,MAX_NAMESIZE);
			send.bySuccess = 1;
			pUser->sendCmdToMe(&send, sizeof(send));

			//if (myUnion->size()>=CREATE_UNION_NEED_MAN_NUM) // 10
			//{
			//	myUnion->letVoteOver();
			//}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s帮的签名期已经结束，请直接去找会长协商有关入会事宜.", pName);
			Cmd::stVoteUnionCmd send;
			strncpy(send.unionName,pName,MAX_NAMESIZE);
			send.bySuccess = 0;
			pUser->sendCmdToMe(&send, sizeof(send));
		}
	}
}

/**
* \brief 设置帮会介绍
* \param pUser 设置者的 UserSession对象
* \param pCmd 介绍设置消息
* \author fqnewman
*/
void CUnionM::setUnionNote(UserSession *pUser, Cmd::stNoteUnionCmd *pCmd)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);
	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	if (pUnionMember)
	{
		CUnion *myUnion = pUnionMember->myUnion;
		if (myUnion && ((myUnion->master == pUnionMember) || pUnionMember->havePower(Cmd::NOTIFY)))
		{
			myUnion->setNote(pCmd);
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你无法设置帮会介绍");
		}
	}
}

/**
 * \brief 更改别名
 *
 *
 * \param pUser 请求更名者(目前只能是帮主)
 * \param pName 被赋别名者的名字(必须是帮会成员)
 * \param pAliasName 别名
 * \return 
 */
void CUnionM::change_aliasname(UserSession *pUser, Cmd::stChangeUnionMemberAliasName* pCmd)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(pUser->name);

	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	
	if (pUnionMember)
	{
		CUnion *myUnion = pUnionMember->myUnion;
		if (myUnion && ((myUnion->master == pUnionMember) || (pUnionMember->havePower(Cmd::CHANGE_ALIAS))))
		{
			pUnionMember = NULL;
			sIterator = findMemberIndex(pCmd->name);
			if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
			
			if (pUnionMember && pUnionMember->myUnion == myUnion)
			{
				pUnionMember->change_aliasname(pCmd->aliasname);		
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你无权更改称号");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不属于任何帮会");
	}
}

// 清除所有帮会
void CUnionM::delAllUnion(DWORD id)
{
	struct findList : public execEntry<CUnion>
	{
		std::vector<DWORD> vUnionID;
		DWORD dwUserID;
		
		findList(DWORD id)
		{
			dwUserID = id;
		}
		~findList(){}

		void removeUnion()
		{
			for (std::vector<DWORD>::iterator vIter = vUnionID.begin(); vIter!=vUnionID.end(); vIter++)
			{
				CUnionM::getMe().delUnion(*vIter);
			}
		}

		bool exec(CUnion *pUnion)
		{
			if (pUnion)
			{
				Cmd::Session::t_GetCreateUnionItem_SceneSession send;
				UserSession *pUser = UserSessionManager::getInstance()->getUserByID(dwUserID);
				
				if (pUser && pUser->scene)
				{
					send.dwUserID = pUnion->tempid;
					pUser->scene->sendCmd(&send, sizeof(send));
				}
				
				vUnionID.push_back(pUnion->id);
			}

			return true;
		}
	};

	findList myList(id);
	execEveryOne(myList);

	myList.removeUnion();
}

void CUnionM::addNewSeptToUnion(const DWORD dwUnionID, const DWORD dwSeptID)
{
	CUnion * pUnion;

	rwlock.rdlock();
	pUnion = (CUnion *)getEntryByID(dwUnionID);
	rwlock.unlock();
	
	struct findall : public execEntry<CSeptMember> 
	{                    
		DWORD _dwUnionID;

		findall(DWORD dwUnionID)
		{           
			_dwUnionID = dwUnionID;
		}

		~findall(){}

		bool exec(CSeptMember* pMember)
		{
			if (pMember)
			{
				UnionDef::stUnionMemberInfo info;

				info.dwCharID = pMember->id;
				strncpy(info.name, pMember->name, MAX_NAMESIZE);

				if (pMember->mySept && pMember->mySept->master 
						&& pMember->mySept->master->id == pMember->id)
				{
					strncpy(info.aliasname, "族长", MAX_NAMESIZE);
				}
				else
				{
					strncpy(info.aliasname, DEFAULTMEMBERALIAS, MAX_NAMESIZE);
				}

				SETMEMBERPOWER_WD(info.wdPower);

				info.wdOccupation = 0;
				info.dwSeptID = pMember->mySept->id;
				CUnionM::getMe().addNewMemberToUnion(_dwUnionID, info);
			}
			
			return true;
		}
	};

	
	DWORD max_septsize = 30;
	DWORD cur_mana = pUnion->getMana();
	
	if (cur_mana>=10000 && cur_mana<30000)
	{
		max_septsize = 40;
	}
	else if (cur_mana>=30000 && cur_mana<80000)
	{
		max_septsize = 50;
	}
	else if (cur_mana>=80000)
	{
		max_septsize = 60;
	}

	if (pUnion)
	{
		if (pUnion->septSize()<max_septsize)
		{
			findall myList(dwUnionID);
			CSept* pSept = CSeptM::getMe().getSeptByID(dwSeptID);
			if (!pSept)
				return;

			if (!pSept->dwUnionID)
			{
				pSept->dwUnionID= pUnion->id;

				pSept->execEveryOne(myList);	
				pSept->writeDatabase();
				pUnion->sendUnionNotify("欢迎 %s 家族加入本帮", pSept->name);
				Zebra::logger->trace("[帮会]: %s 家族加入 %s 帮会成功", pSept->name, pUnion->name);
			}
			else
			{
				Zebra::logger->trace("[帮会]: 加入帮会失败 %s 家族已加入其它帮会.", 
						pSept->name);
			}
		}
	}
}

/**
 * \brief 时间回调函数
 */
void CUnionM::timer()
{
	time_t timValue = time(NULL);
	struct tm tmValue;
	zRTime::getLocalTime(tmValue, timValue);
	struct findList : public execEntry<CUnion>
	{
		findList()
		{
		}
		~findList(){}

		bool exec(CUnion *pUnion)
		{
			if (pUnion)
			{
				pUnion->dwActionPoint = pUnion->getMana()/100;
				Zebra::logger->trace("[帮会]: %s 刷新行动力:(%d,%d)", pUnion->name, pUnion->getMana(), 
						pUnion->dwActionPoint);
				pUnion->sendUnionInfoToAll();
				pUnion->calltimes=0;
				pUnion->writeDatabase();
				pUnion->refreshSeptExp();
				
			}

			return true;
		}
	};

	if (tmValue.tm_hour==0 && tmValue.tm_min==0 && tmValue.tm_sec==0)
	{
		findList myList;
		execEveryOne(myList);
	}
}

/**
 * \brief 根据角色名字获取其所属帮派名字
 *
 *
 * \param Name 角色名称
 * \return 返回帮会名称或者NULL
 */
char * CUnionM::getUnionNameByUserName(char *Name)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(Name);

	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	if (pUnionMember)
	{
		if (pUnionMember->myUnion)
			return pUnionMember->myUnion->name;
	}
	return NULL;
}

/**
 * \brief 根据角色名字获取其所属帮派名字
 *
 *
 * \param Name 角色名称
 * \return 角色所处的国家编号
 */
DWORD CUnionM::getCountryIDByUserName(char *Name)
{
	std::map<std::string, CUnionMember *>::iterator sIterator;
	CUnionMember *pUnionMember = NULL;

	rwlock.rdlock();
	sIterator = findMemberIndex(Name);

	if (sIterator != memberIndex.end()) pUnionMember = (*sIterator).second;
	rwlock.unlock();
	if (pUnionMember)
	{
		if (pUnionMember->myUnion)
			return pUnionMember->myUnion->dwCountryID;
	}
	return 0;
}
