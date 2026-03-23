/**
 * \file
 * \version  $Id: CRelationManager.cpp  $
 * \author  
 * \date 
 * \brief 定义好友关系管理类
 *
 */
#include <vector>
#include "Zebra.h"
#include "CRelationManager.h"
#include "zRWLock.h"
#include "zDBConnPool.h"
#include "Session.h"
#include "SessionTask.h"
#include "SessionManager.h"
#include "SessionServer.h"
#include "OfflineMessage.h"
#include "CUnion.h"

/**
* \brief 关系成员构造函数
* \author fqnewman
*/
CRelation::CRelation()
{
	online = false;
}

/**
* \brief 发送通知给场景，通知更新相关的社会关系字段以及在场景做写对应的处理
* \author fqnewman
*/
void CRelation::sendNotifyToScene()
{
	//rwlock.rdlock();
	UserSession*    pUser = NULL;
        pUser = UserSessionManager::getInstance()->getUserByID(this->charid);
	
	if (pUser)
	{
		if (pUser->scene)
		{
			Cmd::Session::t_sendUserRelationID send;
			send.dwUserID = pUser->id;
			send.dwID = 0;
			send.type = Cmd::Session::RELATION_TYPE_NOTIFY;
			if (pUser->scene) pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_sendUserRelationID));
		}
	}

	//rwlock.unlock();
}

/**
* \brief 成员是否在线
* \author fqnewman
* \return true 在线  false 不在线
*/
bool CRelation::isOnline()
{
	UserSession*    pUser = NULL;
        pUser = UserSessionManager::getInstance()->getUserByID(this->id);
	return pUser==NULL?false:true;
}

/**
* \brief 析构函数
* \author fqnewman
*/
CRelationManager::~CRelationManager()
{
	std::list<CRelation *> _deleteList;
	std::list<CRelation *>::iterator vIterator;

	CRelation *temp= NULL;
	//rwlock.wrlock();
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		temp = (CRelation *)it->second;
		if (temp)
		{
			//updateDBRecord(temp); //更新数据
			UserSession*	pUser = NULL;
			pUser = UserSessionManager::getInstance()->getUserByID(temp->id);

			if (temp->type!=3 && pUser && pUser->id)
			{
				if (temp->type!=3 && user && pUser)
					pUser->relationManager.offline(user->id, user->name);
			}

			_deleteList.push_back(temp);
		}
	}
	clear();

	for(vIterator = _deleteList.begin(); vIterator != _deleteList.end(); vIterator++)
	{
		SAFE_DELETE(*vIterator);
	}
	//rwlock.unlock();
}

/**
* \brief 设置关联的用户，简单关系管理器的拥有者
* \param pUser 所有者
* \author fqnewman
*/
void CRelationManager::setUser(UserSession *pUser)
{
	//rwlock.wrlock();
	user = pUser;
	//rwlock.unlock();
}

/**
* \brief 管理器初始化，由于这事情总是发生在角色上线的时候所以1加载数据库，2发送关系列表给客户端，3发送黑名单给网关
* \author fqnewman
*/
void CRelationManager::init()
{
	if (this->size() == 0)
	{
		loadFromDatabase();
		sendRelationList();
		sendAllBlackListToGateway();
	}
}

/**
* \brief 发送简单社会关系列表
* \author fqnewman
*/
void CRelationManager::sendRelationList()
{
	BYTE buf[zSocket::MAX_DATASIZE];
	unsigned int count;
	Cmd::stRelation *tempPoint;

	//rwlock.rdlock();
	Cmd::stSendRelationListCmd *retCmd=(Cmd::stSendRelationListCmd *)buf;
	constructInPlace(retCmd);
	tempPoint = (Cmd::stRelation *)retCmd->member;
	count = 0;
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CRelation *temp=(CRelation *)it->second;
		tempPoint->level = temp->level;

		if (temp->online)
		{
			UserSession *pUser = UserSessionManager::getInstance()->getUserByID(temp->id);
			if (pUser) 
			{
				tempPoint->user_level = pUser->level;
				tempPoint->exploit = pUser->dwExploit;
				tempPoint->country = pUser->country;
				CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);

				if (pUnion)
				{
					strncpy(tempPoint->unionName, pUnion->name, MAX_NAMESIZE);
				}
				else
				{
					bzero(tempPoint->unionName, MAX_NAMESIZE);
				}
			}
		}
		else
		{
			bzero(tempPoint->unionName, MAX_NAMESIZE);
			tempPoint->user_level = 0;
			tempPoint->exploit = 0;
			tempPoint->country = 0;
		}

		tempPoint->online = temp->isOnline();
		tempPoint->type = temp->type;
		tempPoint->occupation = temp->occupation;
		strncpy(tempPoint->name, temp->name, MAX_NAMESIZE);

		tempPoint++;
		count++;
		if (400==count)
		{
			goto breakRation; // 当记录超过100的时候会超过命令发送的最大限制
		}
	}
breakRation:
	//rwlock.unlock();
	retCmd->size = count;
	user->sendCmdToMe(retCmd,(count*sizeof(Cmd::stRelation)+sizeof(Cmd::stSendRelationListCmd)));
}

/**
* \brief 在线处理
* \param dwID 上线角色的 id
* \author fqnewman
*/
void CRelationManager::online(const DWORD dwID)
{
	CRelation *relation;
	//rwlock.rdlock();
	relation = (CRelation *)getEntryByID(dwID);
	if (relation)
	{
		relation->online = true;
		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(dwID);		

		if (pUser && relation->type!=Cmd::RELATION_TYPE_BAD)
		{
			if (relation->occupation != pUser->occupation)
			{
				relation->occupation = pUser->occupation;
				this->updateDBRecord(relation);
			}
				
			sendStatusChange(relation,Cmd::RELATION_ONLINE);

			if (this->user)
			{
				switch(relation->type)
				{
					case Cmd::RELATION_TYPE_LOVE:
						{
							this->user->sendSysChat(Cmd::INFO_TYPE_SYS, "配偶　%s　上线了", pUser->name);
						}
						break;
					case Cmd::RELATION_TYPE_BAD:
						{
							this->user->sendSysChat(Cmd::INFO_TYPE_SYS, "坏蛋　%s　上线了", pUser->name);
						}
						break;
					case Cmd::RELATION_TYPE_FRIEND:
						{
							this->user->sendSysChat(Cmd::INFO_TYPE_SYS, "好友　%s　上线了", pUser->name);
						}
						break;
					case Cmd::RELATION_TYPE_ENEMY:
						{
							this->user->sendSysChat(Cmd::INFO_TYPE_SYS, "仇人　%s　上线了", pUser->name);
						}
						break;
					default:
						{
							this->user->sendSysChat(Cmd::INFO_TYPE_SYS, "%s　上线了", pUser->name);
						}
						break;
				}
			}
		}

	}
	//rwlock.unlock();
}

/**
* \brief 下线处理
* \param dwID 下线角色的 id
* \author fqnewman
*/
void CRelationManager::offline(const DWORD dwID, const char* name)
{
	CRelation *relation = NULL;
	//rwlock.rdlock();

	relation = (CRelation *)getEntryByID(dwID);
	
	if (relation)
	{
		if (relation->type!=Cmd::RELATION_TYPE_BAD)
		{
			sendStatusChange(relation,Cmd::RELATION_OFFLINE);

			if (relation->online && this->user)
			{
				switch(relation->type)
				{
					case Cmd::RELATION_TYPE_LOVE:
						{
							this->user->sendSysChat(Cmd::INFO_TYPE_SYS, "配偶　%s　下线了", name);
						}
						break;
					case Cmd::RELATION_TYPE_BAD:
						{
							this->user->sendSysChat(Cmd::INFO_TYPE_SYS, "坏蛋　%s　下线了", name);
						}
						break;
					case Cmd::RELATION_TYPE_FRIEND:
						{
							this->user->sendSysChat(Cmd::INFO_TYPE_SYS, "好友　%s　下线了", name);
						}
						break;
					case Cmd::RELATION_TYPE_ENEMY:
						{
							this->user->sendSysChat(Cmd::INFO_TYPE_SYS, "仇人　%s　下线了", name);
						}
						break;
					default:
						{
							this->user->sendSysChat(Cmd::INFO_TYPE_SYS, "%s　下线了", name);
						}
						break;
				}
			}
		}

		relation->online = false;
	}
	
	
	//rwlock.unlock();
}

/**
* \brief 从数据库加载本角色所有的社会关系ID
* \author fqnewman
*/
void CRelationManager::loadFromDatabase()
{
	//FunctionTimes times(55,__FUNCTION__);
	static const dbCol relation_define[] = {
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`RELATIONID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`RELATIONNAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`TYPE`", zDBConnPool::DB_BYTE, sizeof(BYTE) }, 
		{ "`DEGREE`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ "`LASTTIME`", zDBConnPool::DB_DWORD, sizeof(DWORD) },
		{ "`OCCUPATION`", zDBConnPool::DB_WORD, sizeof(WORD) },
		{ NULL, 0, 0}
	};
	struct {
		DWORD	dwCharID;						// 会员编号
		DWORD	dwRelationID;					// 关系ID
		char	relationName[MAX_NAMESIZE+1];	// 关系名称
		BYTE	byType;							// 关系类型
		WORD	wdDegree;                		// 友好度
		DWORD	dwLastTime;						// 最后组队时间
		WORD	wdOccupation;					// 职业
	} __attribute__ ((packed))
	* relationList, *tempPoint;
	
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}

	bzero(where, sizeof(where));
	
	snprintf(where, sizeof(where) - 1, "CHARID = %u", user->id);
	unsigned int retcode = SessionService::dbConnPool->exeSelect(handle, "`SAMPLERELATION`", relation_define, where, NULL, (unsigned char **)&relationList);
	SessionService::dbConnPool->putHandle(handle);
	
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("查询错误:%d，加载简单社会关系失败。", retcode);
		return;
	}

	if (relationList)
	{
		tempPoint = &relationList[0];
		for (unsigned int i=0; i< retcode; i++)
		{
			switch(tempPoint->byType)
			{
				case Cmd::RELATION_TYPE_LOVE:
				case Cmd::RELATION_TYPE_BAD:
				case Cmd::RELATION_TYPE_FRIEND:
				case Cmd::RELATION_TYPE_ENEMY:
					{
						UserSession *pUser = NULL;
						CRelation *relation = NULL;
						relation = new CRelation();
						if (relation)
						{
							pUser = UserSessionManager::getInstance()->getUserByID(tempPoint->dwRelationID);
							if (pUser)
							{
								//if (user && pUser->country != user->country)
								//{
								//	deleteDBRecord(tempPoint->dwRelationID);
								//}
								//else
								//{
									relation->occupation = pUser->occupation;
									relation->online = true;
									pUser->relationManager.online(tempPoint->dwCharID);
								//}
							}
							else
							{
								relation->occupation = tempPoint->wdOccupation;
							}

							relation->charid = tempPoint->dwCharID;
							relation->id = tempPoint->dwRelationID;
							relation->type = tempPoint->byType;
							relation->level = tempPoint->wdDegree;
							relation->lasttime = tempPoint->dwLastTime;
							strncpy(relation->name,tempPoint->relationName,MAX_NAMESIZE);

							//rwlock.wrlock();
							if (!addEntry(relation))
							{
								Zebra::logger->error("添加[%u:%s]社会关系[%u:%s]进入管理器失败!",user->id, user->name, relation->id,relation->name);
							}
								

							// 对最后组队时间超过最大间隔的关系进行友好度扣减
							zRTime ctv;
							DWORD curTime = ctv.sec();

							if (curTime - relation->lasttime >= MAX_GROUP_TIME_GAP)
							{
								if (relation->level - DEDUCT_POINT >=0)
									relation->level-=DEDUCT_POINT;
								else
									relation->level = 0;

								relation->lasttime = curTime;

								UserSession *otherUser = NULL;
								otherUser = UserSessionManager::getInstance()->getUserByID(relation->id);
								if (otherUser)
								{
									otherUser->relationManager.setFriendDegreeByOtherUser(user->id, relation->level, curTime);
								}
								else
								{
									updateOtherOfflineUserDBRecord(relation);
									this->updateDBRecord(relation);
								}
							}
							//rwlock.unlock();
							/////////////////////////////////////////////////
						}
						else
						{
							Zebra::logger->error("严重错误在装载好友列表的时候无法分配出内存");
						}
					}
					break;
				case Cmd::RELATION_TYPE_OVER:
					{
						deleteDBRecord(tempPoint->dwRelationID);
						user->sendSysChat(Cmd::INFO_TYPE_BREAKFRIEND, "%s已经与你恩断义绝",tempPoint->relationName);
					}
					break;
				default:
					Zebra::logger->error("%s的简单社会关系%s类型不正确type=%u",user->name,tempPoint->relationName, tempPoint->byType);
					break;

			}
			tempPoint++;
		}
		SAFE_DELETE_VEC(relationList);
	}
	return;
}

/**
* \brief  删除指定角色的数据库记录
* \param dwID 被删除的角色的ID
* \author fqnewman
*/
void CRelationManager::deleteDBRecord(const DWORD dwID)
{
	//FunctionTimes times(54,__FUNCTION__);
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}

	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "CHARID = %u AND RELATIONID = %u ", user->id,dwID);
	unsigned int retcode = SessionService::dbConnPool->exeDelete(handle, "`SAMPLERELATION`", where);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->debug("%s删除关系记录失败 %u", user->name,dwID);
		return;
	}
	return;
}

/**
* \brief 插入一个新的简单社会关系记录
* \param relation 简单社会关系对象
* \author fqnewman
*/
bool CRelationManager::insertDBRecord(const CRelation *relation)
{
	//FunctionTimes times(53,__FUNCTION__);
	static const dbCol relation_define[] = {
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`RELATIONID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`RELATIONNAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`TYPE`", zDBConnPool::DB_BYTE, sizeof(BYTE) }, 
		{ "`DEGREE`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ "`LASTTIME`", zDBConnPool::DB_DWORD, sizeof(DWORD) },
		{ "`OCCUPATION`", zDBConnPool::DB_WORD, sizeof(WORD) },
		{ NULL, 0, 0}
	};
	struct {
		DWORD	dwCharID;						// 会员编号
		DWORD	dwRelationID;					// 关系ID
		char	relationName[MAX_NAMESIZE+1];	// 关系名称
		BYTE	byType;							// 关系类型
		WORD	wdDegree;                		// 友好度
		DWORD	dwLastTime;						// 最后组队时间
		WORD	wdOccupation;					// 职业
	} __attribute__ ((packed))
	createrelation_data;

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	//插入数据库角色信息
	bzero(&createrelation_data, sizeof(createrelation_data));

	//rwlock.rdlock();

	createrelation_data.dwCharID		= user->id;
	createrelation_data.dwRelationID	= relation->id;
	strncpy(createrelation_data.relationName, relation->name, MAX_NAMESIZE);
	createrelation_data.byType			= relation->type;
	createrelation_data.wdDegree		= relation->level;
	createrelation_data.dwLastTime		= relation->lasttime;
	createrelation_data.wdOccupation	= relation->occupation;

	//rwlock.unlock();

	unsigned int retcode = SessionService::dbConnPool->exeInsert(handle, "`SAMPLERELATION`", relation_define, (const unsigned char *)(&createrelation_data));
	SessionService::dbConnPool->putHandle(handle);

	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("增加好友记录数据库出错");
		return false;
	}
	
	return true;
}

/**
* \brief 更新简单社会关系记录
* \param relation 简单社会关系对象
* \author fqnewman
*/
void CRelationManager::updateDBRecord(const CRelation *relation)
{
	//FunctionTimes times(52,__FUNCTION__);
	static const dbCol relation_define[] = {
		{ "`TYPE`", zDBConnPool::DB_BYTE, sizeof(BYTE) }, 
		{ "`DEGREE`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ "`LASTTIME`", zDBConnPool::DB_DWORD, sizeof(DWORD) },
		{ "`OCCUPATION`", zDBConnPool::DB_WORD, sizeof(WORD) },
		{ NULL, 0, 0}
	};
	struct {
		BYTE	byType;							// 关系类型
		WORD	wdDegree;                		// 友好度
		DWORD	dwLastTime;						// 最后组队时间
		WORD	wdOccupation;					// 职业
	} __attribute__ ((packed))
	update_data;
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}

	bzero(&update_data,sizeof(update_data));
	update_data.byType = relation->type;
	update_data.wdDegree  = relation->level;
	update_data.dwLastTime = relation->lasttime;
	update_data.wdOccupation = relation->occupation;
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "CHARID = %u AND RELATIONID = %u", user->id, relation->id);
	unsigned int retcode = SessionService::dbConnPool->exeUpdate(handle, "`SAMPLERELATION`", relation_define, (BYTE *)(&update_data), where);
	SessionService::dbConnPool->putHandle(handle);

	if ((DWORD)-1 == retcode)
	{
		Zebra::logger->error("CRelationManager::updateDBRecord() 修改好友档案失败CHARID = %u RELATIONID = %u retcode =%u", user->id, relation->id, retcode);
	}
}

/**
* \brief 更新指定社会关系角色的对端角色的数据库记录
* \param relation 社会关系对象
* \author fqnewman
*/
void CRelationManager::updateOtherOfflineUserDBRecord(const CRelation *relation)
{
	//FunctionTimes times(51,__FUNCTION__);
	static const dbCol relation_define[] = {
		{ "`TYPE`", zDBConnPool::DB_BYTE, sizeof(BYTE) }, 
		{ "`DEGREE`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
		{ "`LASTTIME`", zDBConnPool::DB_DWORD, sizeof(DWORD) },
		{ "`OCCUPATION`", zDBConnPool::DB_WORD, sizeof(WORD) },
		{ NULL, 0, 0}
	};
	struct {
		BYTE	byType;							// 关系类型
		WORD	wdDegree;                		// 友好度
		DWORD	dwLastTime;						// 最后组队时间
		WORD	wdOccupation;					// 职业
	} __attribute__ ((packed))
	update_data;
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}

	bzero(&update_data,sizeof(update_data));
	update_data.byType = relation->type;
	update_data.wdDegree  = relation->level;
	update_data.dwLastTime = relation->lasttime;
	update_data.wdOccupation = user->occupation;
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "CHARID = %u AND RELATIONID = %u", relation->id, user->id);
	SessionService::dbConnPool->exeUpdate(handle, "`SAMPLERELATION`", relation_define, (BYTE *)(&update_data), where);
	SessionService::dbConnPool->putHandle(handle);
}

/**
* \brief 写离线开除通知， 将数据库中的对方记录写成RELATION_TYPE_OVER下次用户上线的时候将会进行通知
* \param relation 被处理的社会关系对象
* \author fqnewman
*/
void CRelationManager::writeOfflineNotify(const CRelation *relation)
{
	//FunctionTimes times(50,__FUNCTION__);
	static const dbCol relation_define[] = {
		{ "`TYPE`", zDBConnPool::DB_BYTE, sizeof(BYTE) }, 
		{ NULL, 0, 0}
	};
	BYTE	byType;
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}

	byType = Cmd::RELATION_TYPE_OVER;
	bzero(where, sizeof(where));
	//rwlock.rdlock();
	snprintf(where, sizeof(where) - 1, "CHARID = %u AND RELATIONID = %u", relation->id, user->id);
	//rwlock.unlock();
	SessionService::dbConnPool->exeUpdate(handle, "`SAMPLERELATION`", relation_define, (BYTE *)(&byType), where);
	SessionService::dbConnPool->putHandle(handle);
}

/**
* \brief 发送状态改变消息给客户端
* \param relation 社会关系对象
* \param byState 当前状态
* \author fqnewman
*/
void CRelationManager::sendStatusChange(const CRelation *relation, const BYTE byState)
{
	Cmd::stRelationStatusCmd ret;

	ret.type = relation->type;
	strncpy(ret.name,relation->name, MAX_NAMESIZE);
	ret.level = relation->level;
	ret.byState = byState;
	ret.occupation = relation->occupation;
	ret.user_level = 0;
	bzero(ret.unionName, MAX_NAMESIZE);
	ret.exploit = 0;
	ret.country = 0;
	ret.user_level = 0;
	

	if (relation)	
	{
		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(relation->id);
		if (pUser) 
		{
			ret.country = pUser->country;
			if (relation->online)
			{
				CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);

				if (pUnion)
				{
					strncpy(ret.unionName, pUnion->name, MAX_NAMESIZE);
				}

				ret.exploit = pUser->dwExploit;
				ret.user_level = pUser->level;
			}
		}
	}
		
	if (user)
	{
		user->sendCmdToMe(&ret,sizeof(Cmd::stRelationStatusCmd));
	}
}

/**
* \brief 删除简单社会关系
* \param name 被删除者的名字
* \author fqnewman
*/
void CRelationManager::removeRelation(const char * name)
{
	CRelation *pRelation = NULL;

	//rwlock.rdlock();
	pRelation = (CRelation *)getEntryByName(name);
	//rwlock.unlock();
	if (pRelation)
	{
		//rwlock.wrlock();
		deleteDBRecord(pRelation->id);
		removeEntry(pRelation);
		//rwlock.unlock();
		sendStatusChange(pRelation,Cmd::RELATION_REMOVE);
		if (Cmd::RELATION_TYPE_BAD == pRelation->type) sendBlackListToGateway(name,Cmd::Session::BLACK_LIST_REMOVE);
		if (Cmd::RELATION_TYPE_BAD != pRelation->type) pRelation->sendNotifyToScene();
		SAFE_DELETE(pRelation);
	}
}

CRelation* CRelationManager::getMarryRelation()
{
	struct findall : public execEntry<CRelation>
	{
		CRelation* _pRelation;

		findall()
		{
			_pRelation = NULL;
		}

		~findall(){}

		bool exec(CRelation* pRelation)
		{
			if (pRelation && pRelation->type == Cmd::RELATION_TYPE_LOVE)
			{
				_pRelation = pRelation;
				return false;
			}

			return true;
		}
	};

	findall find_marry;
	execEveryOne(find_marry);

	return find_marry._pRelation;

}

/**
* \brief 增加一个黑名单类型的关系
* \param name 上黑名单者
* \author fqnewman
*/
void CRelationManager::addBadRelation(const char *name)
{
	CRelation *relation = NULL;
	relation = (CRelation *)getEntryByName(name);

	if (relation)
	{
		if (Cmd::RELATION_TYPE_BAD == relation->type)
		{
			user->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方已经在黑名单列表中");
		}
		else
		{
			if(Cmd::RELATION_TYPE_LOVE == relation->type)
			{
				user->sendSysChat(Cmd::INFO_TYPE_FAIL, "必须先离婚才能将其加入黑名单！");
				return;
			}
			UserSession *otherUser = NULL;
			otherUser = UserSessionManager::getInstance()->getUserSessionByName(name);
			if (otherUser) 
			{
				otherUser->relationManager.removeRelation(user->name);
				otherUser->sendSysChat(Cmd::INFO_TYPE_BREAKFRIEND, "%s选择和你恩断义绝",user->name);
			}
			else
			{
				CRelation *relation = NULL;
				relation = (CRelation *)getEntryByName(name);
				if (relation) writeOfflineNotify(relation);
			}
			changeRelationType(name, Cmd::RELATION_TYPE_BAD);
		}
	}
	else
	{
		UserSession *otherUser = NULL;
		otherUser = UserSessionManager::getInstance()->getUserSessionByName(name);
		if (otherUser)
		{
			CRelation *relation = NULL;
			zRTime ctv;
			relation = new CRelation();
			if (relation)
			{
				relation->id = otherUser->id;
				relation->level = 0;
				strncpy(relation->name,otherUser->name,MAX_NAMESIZE);
				relation->type = Cmd::RELATION_TYPE_BAD;
				relation->lasttime = ctv.sec();
				relation->occupation = otherUser->occupation;
				//if (insertDBRecord(relation))
				//{
					//rwlock.wrlock();
					addEntry(relation);
					//rwlock.unlock();
					insertDBRecord(relation);

					sendStatusChange(relation,Cmd::RELATION_ADD);
					sendBlackListToGateway(name, Cmd::Session::BLACK_LIST_ADD);
				//}
				//else
				//{

				//	if (user) user->sendSysChat(Cmd::INFO_TYPE_GAME, "将 %s 加入黑名单失败", name);
				//	SAFE_DELETE(relation);
				//}
			}

			if (user) user->sendSysChat(Cmd::INFO_TYPE_GAME, "将 %s 加入了黑名单", name);
		}
		else
		{
			if (user) user->sendSysChat(Cmd::INFO_TYPE_FAIL, "人不在线无法确认");
		}
	}
}

/**
* \brief 增加一个仇人到社会关系中
* \param name 上仇人名单者
* \author fqnewman
*/

/**
* \brief 增加一个黑名单类型的关系
* \param name 上黑名单者
* \author fqnewman
*/
void CRelationManager::addEnemyRelation(const char *name)
{
	int iCount=0;
	CRelation *lastPoint = NULL;
	zRTime ctv;
	DWORD dwLastTime = ctv.sec();

	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CRelation *temp = (CRelation *)it->second;

		if (temp && (temp->type == Cmd::RELATION_TYPE_ENEMY))
		{
			iCount++;
			if (dwLastTime >= temp->lasttime)
			{
				dwLastTime = temp->lasttime;
				lastPoint = temp;
			}
		}
	}

	if (iCount>=5 && lastPoint)
	{
		this->deleteDBRecord(lastPoint->id);
		this->removeEntry(lastPoint);
	}

	CRelation *relation = NULL;
	relation = (CRelation *)getEntryByName(name);

	if (relation)
	{
	}
	else
	{
		UserSession *otherUser = NULL;
		otherUser = UserSessionManager::getInstance()->getUserSessionByName(name);
		if (otherUser)
		{
			CRelation *relation = NULL;
			zRTime ctv;
			relation = new CRelation();
			if (relation)
			{
				relation->id = otherUser->id;
				relation->level = 0;
				strncpy(relation->name,otherUser->name,MAX_NAMESIZE);
				relation->type = Cmd::RELATION_TYPE_ENEMY;
				relation->lasttime = ctv.sec();
				relation->occupation = otherUser->occupation;
				//rwlock.wrlock();
				addEntry(relation);
				//rwlock.unlock();
				insertDBRecord(relation);
				sendStatusChange(relation,Cmd::RELATION_ADD);
			}

			if (user) user->sendSysChat(Cmd::INFO_TYPE_GAME, "将 %s 加入了仇人列表", name);
		}
	}
}

/**
* \brief 改变社会关系类型，发送通知并更新数据库记录
* \param name 对端名称
* \param type 社会关系类型
* \author fqnewman
*/
void CRelationManager::changeRelationType(const char * name, const BYTE type)
{
	CRelation *relation = NULL;
	relation = (CRelation *)getEntryByName(name);
	if (relation)
	{
		zRTime ctv;
		//rwlock.wrlock();
		if (Cmd::RELATION_TYPE_BAD == relation->type) 
		{
			sendBlackListToGateway(name, Cmd::Session::BLACK_LIST_REMOVE);
			relation->level = 0;
		}
		if (Cmd::RELATION_TYPE_BAD == type)
		{
			sendBlackListToGateway(name, Cmd::Session::BLACK_LIST_ADD);
			relation->level = 0;
			relation->lasttime = ctv.sec();
		}
		relation->type = type;
		relation->sendNotifyToScene();
		sendStatusChange(relation,Cmd::RELATION_TYPECHANGE);
		updateDBRecord(relation);
		//rwlock.unlock();
	}
}

/**
* \brief 增加一个新的社会关系类型
* \param dwID 对端角色id
* \param type 社会关系类型
* \author fqnewman
*/
void CRelationManager::addRelation(const DWORD dwID, const BYTE type)
{
	UserSession *otherUser = NULL;

	otherUser = UserSessionManager::getInstance()->getUserByID(dwID);
	if (otherUser)
	{
		CRelation *relation = NULL;
		relation = (CRelation *)getEntryByID(dwID);
		if (relation)
		{
			if (relation->type == type)
			{
				user->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家已经在列表中");
			}
			else
			{
				changeRelationType(otherUser->name, type);
				relation->online = true;
			}
		}
		else
		{
			zRTime ctv;
			relation = new CRelation();
			if (relation)
			{
				relation->online = true;
				relation->id = otherUser->id;
				relation->level = 0;
				strncpy(relation->name,otherUser->name,MAX_NAMESIZE);
				relation->type = type;
				relation->lasttime = ctv.sec();
				relation->occupation = otherUser->occupation;

				//if (insertDBRecord(relation))
				//{
					//rwlock.wrlock();
					addEntry(relation);
					//rwlock.unlock();
					insertDBRecord(relation);
					sendStatusChange(relation,Cmd::RELATION_ADD);
					relation->sendNotifyToScene();
				//}
				//else
				//{
				//	SAFE_DELETE(relation);
				//}
			}
		}
	}
	else
	{
		if (user) user->sendSysChat(Cmd::INFO_TYPE_FAIL, "人不在线无法确认");
	}
}

/**
* \brief 处理Gateway转发过来的客户端消息
* \param ptNullCmd 消息体
* \param cmdLen 消息长度
* \author fqnewman
* \return true 处理完毕， false 不在处理范围之中
*/
bool CRelationManager::processUserMessage(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch(ptNullCmd->byCmd)
	{
		case Cmd::RELATION_USERCMD:
			{
				switch(ptNullCmd->byParam)
				{
					case UNMARRY_PARA:
						{
							CRelation* relation = NULL;
							relation = getMarryRelation();

							if (relation)
							{
								UserSession *otherUser = NULL;
								otherUser = UserSessionManager::getInstance()->getUserSessionByName(relation->name);
								if (otherUser) 
								{
									removeRelation(relation->name);
									otherUser->relationManager.removeRelation(user->name);
									otherUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s选择和你离婚",user->name);
									otherUser->updateConsort();
								}
								else
								{
									CRelation *tRelation = NULL;

									tRelation = (CRelation *)getEntryByName(relation->name);
									if (tRelation) writeOfflineNotify(tRelation);
									removeRelation(relation->name);
								}
								user->updateConsort();
							}
						}
						break;
					case Cmd::RELATION_STATUS_PARA:
						{
							Cmd::stRelationStatusCmd *rev = (Cmd::stRelationStatusCmd *)ptNullCmd;
							switch(rev->byState)
							{
								case Cmd::RELATION_ADD:
									{
										if(!strncmp(rev->name,user->name, MAX_NAMESIZE))
										{
											user->sendSysChat(Cmd::INFO_TYPE_FAIL, "不能把自己加入名单中?");
											return true;
										}

										if (300>size())
										{
											if (rev->type == Cmd::RELATION_TYPE_BAD)
											{
												addBadRelation(rev->name);
											}
											else
											{
												addEnemyRelation(rev->name);
											}
										}
										else
										{
											user->sendSysChat(Cmd::INFO_TYPE_FAIL, "名单列表已满！");
										}
										return true;
									}
									break;
								case Cmd::RELATION_ANSWER_NO:
									{
										UserSession *otherUser = NULL;
										otherUser = UserSessionManager::getInstance()->getUserByID(rev->userid);
										if (otherUser)
										{
											switch(rev->type)
											{
												case Cmd::RELATION_TYPE_FRIEND:
													{
														otherUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方不同意与你结为好友");
														return true;
													}
													break;
												case Cmd::RELATION_TYPE_LOVE:
													{
														otherUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方不同意与你结为夫妻");
													}
													break;
												default:
													break;
											}
										}
									}
									break;
								case Cmd::RELATION_ANSWER_YES:
									{
										UserSession *otherUser = NULL;
										otherUser = UserSessionManager::getInstance()->getUserByID(rev->userid);
										if (otherUser)
										{
											if (300>otherUser->relationManager.size()|| rev->type == Cmd::RELATION_TYPE_LOVE)
											{
												if (300>size() || rev->type == Cmd::RELATION_TYPE_LOVE)
												{
													addRelation(rev->userid, rev->type);
													otherUser->relationManager.addRelation(user->id, rev->type);
													if (rev->type != Cmd::RELATION_TYPE_LOVE)
													{
														otherUser->sendSysChat(Cmd::INFO_TYPE_ADDFRIEND, "你与 %s 义结金兰，成为好友", user->name);
														user->sendSysChat(Cmd::INFO_TYPE_ADDFRIEND, "你与 %s 义结金兰，成为好友", otherUser->name);
													}
												}
												else
												{
													user->sendSysChat(Cmd::INFO_TYPE_FAIL, "我的好友列表已满");
													otherUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方好友列表已满");
												}
											}
											else
											{
												user->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方好友列表已满");
												otherUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你的好友列表已满");
											}
										}
									}
									break;
								case Cmd::RELATION_QUESTION:
									{
										if(!strncmp(rev->name,user->name, MAX_NAMESIZE))
										{
											user->sendSysChat(Cmd::INFO_TYPE_FAIL, "你在开玩笑吗？加自己为好友！");
											return true;
										}
										CRelation *relation = NULL;
										relation = (CRelation *)getEntryByName(rev->name);
										if (relation)
										{
											if (!strncmp(rev->name,relation->name, MAX_NAMESIZE) && Cmd::RELATION_TYPE_BAD != relation->type)
											{
												user->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方已经在你的好友列表中了，无需再添加！");
												return true;
											}
										}

										UserSession *otherUser = NULL;
										otherUser = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
										if (otherUser)
										{
											if (isset_state(otherUser->sysSetting , Cmd::USER_SETTING_FRIEND))
											{

												user->sendSysChat(Cmd::INFO_TYPE_GAME, "好友请求已发送，等待对方应答!");
												rev->userid = user->id;
												strncpy(rev->name,user->name,MAX_NAMESIZE);
												otherUser->sendCmdToMe(rev,sizeof(Cmd::stRelationStatusCmd));
											}
											else
												user->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方添加好友未开启");
										}
										else
										{
											user->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方不在线不能响应你的邀请");
										}

									}
									break;
								case Cmd::RELATION_REMOVE:
									{
										CRelation *rel = NULL;
										rel = (CRelation *)getEntryByName(rev->name);
										if (!rel) return true;
										int type = rel->type;
								
										if (Cmd::RELATION_TYPE_BAD == type || Cmd::RELATION_TYPE_ENEMY == type)
										{
											removeRelation(rev->name); // 删除黑名单成员
										}
										else
										{
											if(Cmd::RELATION_TYPE_LOVE == type)
											{
												user->sendSysChat(Cmd::INFO_TYPE_FAIL, "你必须到民政官那里去办理离婚手续！");
												return true;
											}
											UserSession *otherUser = NULL;
											otherUser = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
											if (otherUser) 
											{
												removeRelation(rev->name);
												otherUser->relationManager.removeRelation(user->name);
												otherUser->sendSysChat(Cmd::INFO_TYPE_BREAKFRIEND, "%s选择与你割席断交",user->name);
												user->sendSysChat(Cmd::INFO_TYPE_BREAKFRIEND, "你选择与 %s 割席断交", otherUser->name);
												otherUser->updateConsort();
											}
											else
											{
												user->sendSysChat(Cmd::INFO_TYPE_BREAKFRIEND, "你选择与 %s 割席断交", rev->name);
												CRelation *relation = NULL;

												relation = (CRelation *)getEntryByName(rev->name);
												if (relation) writeOfflineNotify(relation);
												removeRelation(rev->name);
											}
											user->updateConsort();
										}
									}
									break;
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
	//	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->byCmd, ptNullCmd->byParam, cmdLen);
	return false;
}

/**
* \brief 发送聊天消息给自己的所有简单社会关系，如果不在线则存为离线消息
* \param pCmd 聊天消息
* \param cmdLen 消息长度
* \author fqnewman
*/
void CRelationManager::sendChatToMyFriend(const Cmd::stChannelChatUserCmd *pCmd, const unsigned int cmdLen)
{
	//rwlock.rdlock();
	user->sendCmdToMe(pCmd, cmdLen);  // 转发一条消息给自己，以免看不到自己的聊天记录
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CRelation *temp = (CRelation *)it->second;

		if (temp && (temp->type != Cmd::RELATION_TYPE_BAD)&& (temp->type != Cmd::RELATION_TYPE_ENEMY))
		{
			if (temp->online)
			{
				UserSession *pUser = UserSessionManager::getInstance()->getUserByID(temp->id);
				if (pUser) pUser->sendCmdToMe(pCmd, cmdLen);
			}
			else
			{
				if (Cmd::CHAT_TYPE_FRIEND_AFFICHE == pCmd->dwChannelID)
				{
					COfflineMessage::writeOfflineMessage(pCmd->dwType, user->id, pCmd,cmdLen);
				}
			}
		}
	}
	//rwlock.unlock();
}

/**
* \brief 发送消息给自己的所有简单社会关系
* \param pCmd 消息
* \param cmdLen 消息长度
* \param sendMe 是否发给自己
* \author xwl
*/
void CRelationManager::sendCmdToMyFriend(const void *pCmd, const DWORD cmdLen, bool sendMe)
{
	//rwlock.rdlock();
	if (sendMe)
		user->sendCmdToMe(pCmd, cmdLen);  // 转发一条消息给自己
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CRelation *temp = (CRelation *)it->second;

		if (temp && (temp->type != Cmd::RELATION_TYPE_BAD) && (temp->type != Cmd::RELATION_TYPE_ENEMY))
		{
			if (temp->online)
			{
				UserSession *pUser = UserSessionManager::getInstance()->getUserByID(temp->id);
				if (pUser)
				{
					pUser->sendCmdToMe(pCmd, cmdLen);
				}
			}
		}
	}
	//rwlock.unlock();
}

/**
* \brief 发送消息给自己的所有简单社会关系
* \param pCmd 消息
* \param cmdLen 消息长度
* \param sendMe 是否发给自己
* \author xwl
*/
void CRelationManager::sendCmdToMyFriendExcept(const void *pCmd, const DWORD cmdLen, bool sendMe, const char * except)
{
	//rwlock.rdlock();
	if (sendMe)
		user->sendCmdToMe(pCmd, cmdLen);  // 转发一条消息给自己
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CRelation *temp = (CRelation *)it->second;

		if (temp && (temp->type != Cmd::RELATION_TYPE_BAD) && (temp->type != Cmd::RELATION_TYPE_ENEMY))
		{
			if (temp->online)
			{
				UserSession *pUser = UserSessionManager::getInstance()->getUserByID(temp->id);
				if (pUser && strncmp(pUser->name, except, MAX_NAMESIZE))
				{
					pUser->sendCmdToMe(pCmd, cmdLen);
				}
			}
		}
	}
	//rwlock.unlock();
}

/**
* \brief 发送私聊消息给好友，如果对方不在则存为离线消息
* \param  pCmd 聊天消息
* \param cmdLen 消息长度
* \author fqnewman
* \return 
*/
void CRelationManager::sendPrivateChatToFriend(const Cmd::stChannelChatUserCmd *pCmd, const unsigned int cmdLen)
{
	//rwlock.rdlock();

	CRelation *rel = (CRelation *)getEntryByName(pCmd->pstrName);
	if (rel)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::stChannelChatUserCmd *chatCmd;

		chatCmd = (Cmd::stChannelChatUserCmd *)buf;
		memcpy(chatCmd,pCmd,cmdLen);
		strncpy(chatCmd->pstrName, user->name,MAX_NAMESIZE);

		if (rel->isOnline())
		{
			UserSession *pUser = UserSessionManager::getInstance()->getUserByID(rel->id);
			if (pUser) pUser->sendCmdToMe(chatCmd, cmdLen);
		}
		else
		{
			COfflineMessage::writeOfflineMessage(chatCmd->dwType, rel->id, chatCmd,cmdLen);
		}
	}
	//rwlock.unlock();
}

/**
* \brief  发送黑名单操作消息到GateWay
* \param name 操作的角色名称
* \param oper 操作类型 Cmd::Session::BLACK_LIST_ADD， Cmd::Session::BLACK_LIST_REMOVE
* \author fqnewman
*/
void CRelationManager::sendBlackListToGateway(const char *name, const BYTE oper)
{
	if (user) 
	{
		Cmd::Session::t_Session_HandleBlackList send;
		strncpy(send.name, name, MAX_NAMESIZE);
		send.byOper = oper;
		send.dwID = user->id;
		user->sendCmd(&send, sizeof(Cmd::Session::t_Session_HandleBlackList));
	}
}

/**
* \brief  发送所有的黑名单列表到Gateway
* \author fqnewman
*/
void CRelationManager::sendAllBlackListToGateway()
{
	//rwlock.rdlock();
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CRelation *temp = (CRelation *)it->second;
		if (temp)
		{
			if (Cmd::RELATION_TYPE_BAD == temp->type)
			{
				sendBlackListToGateway(temp->name, Cmd::Session::BLACK_LIST_ADD);
			}
		}
	}
	//rwlock.unlock();
}

/**
* \brief 根据对端关系名获取关系对象
* \param name 对端关系名
* \author fqnewman
* \return 简单社会关系对象
*/
CRelation * CRelationManager::getRelationByName(const char *name)
{
	return (CRelation *)getEntryByName(name);
}

/**
* \brief 根据对端关系ID获取关系对象
* \param  dwRelationID 对端关系ID
* \author fqnewman
* \return 简单社会关系对象
*/
CRelation* CRelationManager::getRelationByID(DWORD dwRelationID)
{
	return (CRelation*)getEntryByID(dwRelationID);
}

/**
* \brief 设置友好度
* \param rev 友好度设置请求消息
* \author fqnewman
*/
void CRelationManager::setFriendDegree(Cmd::Session::t_CountFriendDegree_SceneSession *rev)
{
	//rwlock.wrlock();
	for (int i=0; i<rev->size; i++)
	{
		CRelation *rel = NULL;
		rel = (CRelation *)getEntryByID(rev->namelist[i].dwUserID);
		if (rel)
		{
#ifdef _DEBUGLOG
			Zebra::logger->info("设置友好度：设置%s 与 %s 的 %u 友好度为 %d", this->user->name, rel->name, rel->type, rev->namelist[i].wdDegree);
#endif
			if (rel->type == rev->namelist[i].byType)
			{
				if (rel->level < rev->namelist[i].wdDegree)
				{
#ifdef _DEBUGLOG
				Zebra::logger->info("设置友好度：成功设置");
#endif
					UserSession *otherUser = NULL;
					zRTime ctv;

					rel->level = rev->namelist[i].wdDegree;
					rel->lasttime = ctv.sec();
					this->updateDBRecord(rel);
					otherUser = UserSessionManager::getInstance()->getUserByID(rel->id);
					if (otherUser) 
					{
						otherUser->relationManager.setFriendDegreeByOtherUser(user->id, rel->level, ctv.sec());
					}
					else
					{
						updateOtherOfflineUserDBRecord(rel);
					}
				}
			}
		}
	}
	//rwlock.unlock();
}

/**
* \brief 设置对端友好度，主要用来做双边友好度同步
* \param dwUserID 对端角色id
* \param wdDegree 友好度
* \param currTime 当前时间
* \author fqnewman
*/
void CRelationManager::setFriendDegreeByOtherUser(const DWORD dwUserID, const WORD wdDegree, const DWORD currTime)
{
	CRelation *rel = NULL;

	//rwlock.wrlock();
	rel = (CRelation *)getEntryByID(dwUserID);
	if (rel)
	{
		if (rel->level < wdDegree)
		{
			rel->level = wdDegree;
			rel->lasttime = currTime;
			this->updateDBRecord(rel);
		}
	}
	//rwlock.unlock();
}

//Zzise 升级好友祝贺
void CRelationManager::FriendCongratulate(DWORD playerid,const char *name,DWORD playerlevel,DWORD type)
{
	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CRelation *temp=(CRelation *)it->second;

		if (temp->online)
		{
			UserSession *pUser = UserSessionManager::getInstance()->getUserByID(temp->id);
			if (pUser) 
			{
				Cmd::stFriendCongratulateUserCmd retCmd;
				retCmd.playerid = playerid;
				retCmd.dwLevel = playerlevel;
				retCmd.dwType = type;
				strncpy(retCmd.name, name, sizeof(retCmd.name));
				pUser->sendCmdToMe(&retCmd, sizeof(Cmd::stFriendCongratulateUserCmd));
			}
		}
		
	}
}

