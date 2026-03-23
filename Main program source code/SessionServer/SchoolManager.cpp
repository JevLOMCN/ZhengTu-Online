/**
 * \file
 * \version  $Id: SchoolManager.cpp  $
 * \author
 * \date 
 * \brief 实现师门门派管理器
 *
 * 
 */

#include <stdarg.h>
#include "zRWLock.h"
#include "SchoolManager.h"
#include "Command.h"
#include "zDBConnPool.h"
#include "SessionServer.h"
#include "Session.h"
#include "zTime.h"
#include "OfflineMessage.h"
#include "CDare.h"
#include "CUnion.h"
#include "CSept.h"

/**
 * \brief 取得师门成员的回调函数
 * \param member 成员指针
 */
void CSchoolMemberListCallback::exec(CSchoolMember *member)
{
	memberList.push_back(member);
}

/**
 * \brief 删除成员并清除他和其他成员的关系
 */
void CSchoolMemberListCallback::clearInValidNodeRelation()
{
	std::list<CSchoolMember *>::iterator tIterator;

	for(tIterator = memberList.begin(); tIterator != memberList.end(); tIterator++)
	{
		CSchoolMember *member = (*tIterator);
		if (member)
		{
			if (!member->isValid()||member->isClean())
			{
				CSchoolMember *preNode = member->getPreLevelNode();
				if (preNode) preNode->directRemovePrentice(member);
				member->clearAllNextLevelRelation();
			}
		}
	}
}

/**
 * \brief 向一个成员发送其他成员的信息
 */
void CSchoolMemberListCallback::sendNotifyToMember()
{
	std::list<CSchoolMember *>::iterator tIterator;

	clearInValidNodeRelation();

	for(tIterator = memberList.begin(); tIterator != memberList.end(); tIterator++)
	{
		if ((*tIterator))
		{
			if (!(*tIterator)->isValid()||(*tIterator)->isClean())
			{
				(*tIterator)->deleteMe();
			}
			else
			{
				(*tIterator)->updateRecord();
				(*tIterator)->sendInfomationToMe();
			}
		}
	}
}

/**
 * \brief 把要处理的成员添加进列表
 * sendNotifyToMember和clearInValidNodeRelation处理列表里的成员
 * \param member 成员指针
 * \param tag 标志
 */
void CSendSchoolCallback::exec(CSchoolMember *member, const BYTE tag)
{
	Cmd::stSchoolMember temp;

	strncpy(temp.name, member->name, MAX_NAMESIZE);
	temp.tag = tag;
	temp.online = member->isOnline();
	temp.occupation = member->getOccupation();
	temp.level = member->getLevel();
	memberList.push_back(temp);
	//	Zebra::logger->debug("tag = [%u] name = [%s] online = [%u]", temp.tag, temp.name, temp.online);
}

/**
 * \brief 向成员发送成员列表
 *
 * \param member 成员指针
 */
void CSendSchoolCallback::sendListToMember(CSchoolMember *member)
{
	BYTE buf[zSocket::MAX_DATASIZE];
	unsigned int count;
	Cmd::stSchoolMember *tempPoint;
	std::list<struct Cmd::stSchoolMember>::iterator tIterator;

	Cmd::stSendListSchoolCmd *retCmd=(Cmd::stSendListSchoolCmd *)buf;
	constructInPlace(retCmd);
	tempPoint = (Cmd::stSchoolMember *)retCmd->list;
	count = 0;
	retCmd->byPackageTag=Cmd::SCHOOL_PACKAGE_HEAD;

	CSchool *tempSchool = member->getSchool();
	DWORD masterid = 0;
	if (tempSchool)	 masterid = tempSchool->getMasterNode()->getCharID();
	if (member->getCharID() == masterid)
	{
		UserSession *master = NULL;
		master = UserSessionManager::getInstance()->getUserByID(masterid);
		if (master)
			retCmd->dwMasterTempID = master->tempid;
		else
			retCmd->dwMasterTempID = 0;
	}
	else
	{
		retCmd->dwMasterTempID = 0;
	}

	tIterator = memberList.begin();
	while(tIterator != memberList.end())
	{
		if (count > 1500)
		{
			retCmd->size = count;
			member->sendCmdToMe(retCmd,(count*sizeof(Cmd::stSchoolMember)+sizeof(Cmd::stSendListSchoolCmd)));

			tempPoint = (Cmd::stSchoolMember *)retCmd->list;
			count = 0;
			retCmd->byPackageTag=Cmd::SCHOOL_PACKAGE_BODY;
		}
		strncpy(tempPoint->name, tIterator->name, MAX_NAMESIZE);
		tempPoint->tag = tIterator->tag;
		tempPoint->online = tIterator->online;
		tempPoint->occupation = tIterator->occupation;
		tempPoint->level  = tIterator->level;

		count++;
		tempPoint++;
		tIterator++;
	}

	if (count>0)
	{
		if (retCmd->byPackageTag == Cmd::SCHOOL_PACKAGE_HEAD)
		{
			retCmd->byPackageTag=Cmd::SCHOOL_PACKAGE_HEAD|Cmd::SCHOOL_PACKAGE_TAIL;
		}
		else
		{
			retCmd->byPackageTag = Cmd::SCHOOL_PACKAGE_TAIL;
		}
		retCmd->size = count;
		member->sendCmdToMe(retCmd,(count*sizeof(Cmd::stSchoolMember)+sizeof(Cmd::stSendListSchoolCmd)));
	}
}

//---------------------------------------------------------------------

/**
 * \brief 门派管理器构造函数
 */
CSchoolM *CSchoolM::sm(NULL);

/**
 * \brief 析构函数
 */
CSchoolM::~CSchoolM()
{
	rwlock.wrlock();
	for(zEntryID::hashmap::iterator it=zEntryID::ets.begin();it!=zEntryID::ets.end();it++)
	{
		CSchool *temp =(CSchool *)it->second;
		SAFE_DELETE(temp);
	}
	clear();
	rwlock.unlock();
}

/**
 * \brief 初始化管理器
 * \return 初始化是否成功
 */
bool CSchoolM::init()
{
	CSchool *pSchool = new CSchool();
	if (pSchool)
	{
		pSchool->initToNoneSchool();
		rwlock.wrlock();
		addEntry(pSchool);
		rwlock.unlock();
	}
	else
	{
		Zebra::logger->error("不能生成师门管理器对象！");
		return false;
	}


	Zebra::logger->debug("loadSchoolFromDB()");
	if (!loadSchoolFromDB()) return false;
	Zebra::logger->debug("loadSchoolMemberFromDB()");
	if (!loadSchoolMemberFromDB()) return false;
	return true;
}

/**
 * \brief 得到管理器实例
 * \return 门派管理器实例
 */
CSchoolM &CSchoolM::getMe()
{
	if(sm==NULL)
	{
		sm=new CSchoolM();
	}
	return *sm;
}

/**
 * \brief 删除管理器实例
 */
void CSchoolM::destroyMe()
{
	SAFE_DELETE(sm);
}

/**
* \brief 在成员索引里面查找成员迭代
* return 迭代指针
*/
std::map<std::string, CSchoolMember *>::iterator  CSchoolM::findMemberIndex(const char *pName)
{
        char temp_name[MAX_NAMESIZE];
        bzero(temp_name, MAX_NAMESIZE);
        strncpy(temp_name, pName, MAX_NAMESIZE);
        return memberIndex.find(temp_name);
}


/**
 * \brief 根据名字得到门派指针
 * \param name 门派名字
 * \return 门派指针
 */
CSchool * CSchoolM::getSchoolByName( const char * name)
{
	rwlock.rdlock();
	CSchool *ret =(CSchool *)getEntryByName(name);
	rwlock.unlock();
	return ret;
}

/**
 * \brief 从数据库读取门派信息
 * \return 是否读取成功
 */
bool CSchoolM::loadSchoolFromDB()
{
	//FunctionTimes times(1,__FUNCTION__);
	const dbCol school_read_define[] = {
		{ "SCHOOLID",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "NAME",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "MASTERSERIAL",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};

	stSchoolInfo *schoolList,*tempPoint;
	char *point;

	schoolList = NULL;
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	unsigned int retcode = SessionService::dbConnPool->exeSelect(handle, "`SCHOOL`", school_read_define, NULL, NULL, (BYTE **)&schoolList);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode
			|| 0 == retcode)
	{
		return true;
	}

	if (schoolList)
	{
		tempPoint = &schoolList[0];
		point = (char *)&schoolList[0];
		for (unsigned int i=0; i< retcode; i++)
		{
			if (!createSchoolFromDB(*tempPoint))
			{
				Zebra::logger->error("无法根据门派记录创建门派管理器");
				SAFE_DELETE_VEC(schoolList);
				return false;
			}
			point +=sizeof(stSchoolInfo)+tempPoint->size;//MAX_BULLETIN_CHAR_NUMBER;
			tempPoint = (stSchoolInfo *)point;
		}
		SAFE_DELETE_VEC(schoolList);
		return true;
	}
	else
	{
		Zebra::logger->error("门派数据初始化失败，exeSelect 返回无效buf指针");
	}
	return false;
}

/**
 * \brief 从数据库读取门派成员
 * \return 是否成功
 */
bool CSchoolM::loadSchoolMemberFromDB()
{
	bool ret = true;

	for(zEntryName::hashmap::iterator it=zEntryName::ets.begin();it!=zEntryName::ets.end();it++)
	{
		CSchool *temp =(CSchool *)it->second;
		if (!temp->loadSchoolMemberFromDB()) ret = false;
	}
	return ret;
}

/**
 * \brief 根据数据库里的信息创建门派对象
 * \param info 门派信息
 * \return  是否成功
 */
bool CSchoolM::createSchoolFromDB(const stSchoolInfo &info)
{
	CSchool *pSchool = new CSchool();
	if (pSchool)
	{
		//		Zebra::logger->debug("创建门派对象id=[%u]name=[%s]",info.dwSchoolID, info.name);
		pSchool->initSchool(info);
		rwlock.wrlock();
		addEntry(pSchool);
		rwlock.unlock();
		return true;
	}
	else
	{
		Zebra::logger->error("建立门派对象的时候无法分配内存");
		return false;
	}
}

/**
 * \brief 处理成员上线
 *
 * \param pUser 上线的成员
 */
void CSchoolM::userOnline(UserSession *pUser)
{
	std::map <std::string, CSchoolMember*>::iterator sIterator;

	rwlock.wrlock();
	sIterator = findMemberIndex(pUser->name);
	rwlock.unlock();
	if (sIterator != memberIndex.end())
	{
		if (sIterator->second) sIterator->second->online(pUser);
	}
	else
	{
		Cmd::stMemberStatusSchool ret;
		ret.byStatus = Cmd::SCHOOL_STATUS_NONE;
		pUser->sendCmdToMe(&ret, sizeof(ret));
		if (pUser->schoolid != 0)
		{
			pUser->schoolid = 0;
			Cmd::Session::t_sendUserRelationID send;
			send.dwUserID = pUser->id;
			send.caption = 0;
			send.dwID = 0;
			send.type = Cmd::Session::RELATION_TYPE_SCHOOL;
			pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_sendUserRelationID));
		}
	}
}

/**
 * \brief 处理角色离线，向其他成发送信息
 *
 * \param pUser 下线的成员
 */
void CSchoolM::userOffline(UserSession *pUser)
{
	std::map <std::string, CSchoolMember*>::iterator sIterator;

	rwlock.wrlock();
	sIterator = findMemberIndex(pUser->name);
	rwlock.unlock();
	if (sIterator != memberIndex.end())
	{
		if (sIterator->second) sIterator->second->offline();
	}
}

/**
 * \brief 添加一条门派成员索引
 * \param pName 门派名字
 * \param pSchoolMember 成员指针
 * \return 是否成功
 */
bool CSchoolM::addMemberIndex(const char *pName, CSchoolMember *pSchoolMember)
{
	std::pair<std::map<std::string, CSchoolMember*>::iterator, bool> retval;
        char temp_name[MAX_NAMESIZE];
        bzero(temp_name, MAX_NAMESIZE);
        strncpy(temp_name, pName, MAX_NAMESIZE);

	rwlock.wrlock();
	retval = memberIndex.insert(memberIndexValueType(temp_name,pSchoolMember));
	rwlock.unlock();
	return retval.second;
}

/**
 * \brief 从映射中删除一个成员 
 * \param pName 要删除的名字
 * \return 删除是否成功 
 */
bool CSchoolM::removeMemberIndex(const char *pName)
{
	bool ret;
	std::map <std::string, CSchoolMember*>::iterator sIterator;
	rwlock.wrlock();
	ret = true;
	sIterator = findMemberIndex(pName);
	if (sIterator != memberIndex.end()) memberIndex.erase(sIterator);
	else ret = false;
	rwlock.unlock();
	return ret;
}

/**
 * \brief 解析门派相关的消息
 *
 * \param pUser 玩家会话对象指针 
 * \param ptNullCmd 待解析的消息
 * \param cmdLen 消息长度
 * \return 解析是否成功
 */
bool CSchoolM::processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch(ptNullCmd->byCmd)
	{
		case Cmd::SCHOOL_USERCMD:
			{
				switch(ptNullCmd->byParam)
				{
					case REQ_MASTER_BOUNTY_PARA:
						{
							CSchoolMember *member = (CSchoolMember *)getMember(pUser->name);
							
							if (!member)
								return true;
							
							Cmd::stReqMasterBountyUserCmd* cmd = (Cmd::stReqMasterBountyUserCmd*)ptNullCmd;

							if (cmd->byState == Cmd::QUERY_SCHOOL_BOUNTY)
							{
								Cmd::stRtnMasterBountyUserCmd send;
								send.dwBalance = member->master_balance;
								send.dwTotal = member->master_total;
								pUser->sendCmdToMe(&send, sizeof(send));
							}
							else if (cmd->byState == Cmd::GET_SCHOOL_BOUNTY)
							{
								Cmd::Session::t_PickupMaster_SceneSession send;

								if (member->master_balance>0)
								{
									send.dwUserID = pUser->id;
									send.dwMoney = member->master_balance;
									pUser->scene->sendCmd(&send, sizeof(send));
									member->master_total += member->master_balance;
									member->master_balance = 0;
									member->updateRecord();
								}
							}

							return true;
						}
						break;
					case REQ_PRENTICE_BOUNTY_PARA:
						{
							CSchoolMember *member = (CSchoolMember *)getMember(pUser->name);
							if (!member || !member->getTeacher())
								return true;
							
							Cmd::stReqPrenticeBountyUserCmd* cmd = 
								(Cmd::stReqPrenticeBountyUserCmd*)ptNullCmd;

							if (cmd->byState == Cmd::QUERY_SCHOOL_BOUNTY)
							{
								Cmd::stRtnPrenticeBountyUserCmd send;
								send.dwBalance = member->queryBounty();
								send.dwTotal = member->prentice_total;
								send.dwLastLevel = member->prentice_total;
								strncpy(send.name, member->getTeacher()->name, MAX_NAMESIZE);
								pUser->sendCmdToMe(&send, sizeof(send));
							}
							else if (cmd->byState == Cmd::PUT_SCHOOL_BOUNTY)
							{
								if (member->putBounty())
								{
									pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "进贡成功");
								}
							}
							
							return true;
						}
						break;
					case Cmd::ADD_MEMBER_TO_SCHOOL_PARA: // 招收徒弟
						{
							Cmd::stAddMemberToSchoolCmd *ptCmd=(Cmd::stAddMemberToSchoolCmd *)ptNullCmd;
							switch(ptCmd->byState)
							{
								case Cmd::TEACHER_QUESTION:
									{
										if (getUserPrenticeInfo(pUser->name,ptCmd->schoolName))
										{
											UserSession *otherUser = NULL;
											otherUser = UserSessionManager::getInstance()->getUserSessionByName(ptCmd->memberName);
											if (otherUser)
											{
												if (!isset_state(otherUser->sysSetting , Cmd::USER_SETTING_SCHOOL))
												{
													pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,"对方加入师门未开启");
													return true;
												}
												if (otherUser->level >=10)
												{
													if (isClean(otherUser->name))
													{
														ptCmd->memberID = pUser->tempid;
														strncpy(ptCmd->memberName, pUser->name, MAX_NAMESIZE);
														otherUser->sendCmdToMe(ptCmd, cmdLen);
													}
													else
													{
														pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不能招他为徒，此人已经存在师徒关系");
													}
												}
												else
												{
													pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不能招他为徒，他还没到10级！");
												}
											}
											else
											{
												pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "玩家不在线，无法回应邀请");
											}
										}
										else
										{
											pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "人数已满，不能再招收了");
										}
										return true;
									}
									break;
								case Cmd::TEACHER_ANSWER_YES:
									{
										UserSession *master = NULL;
										master = UserSessionManager::getInstance()->getUserByTempID(ptCmd->memberID);
										if (isClean(pUser->name))
										{
											if (master)
											{
												Zebra::logger->trace("[师徒]收到%s招收%s的指令",master->name, pUser->name);
												addMember(master,pUser);
											}
										}
										else
										{
											if (master) master->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不能招他为徒，此人已经存在师徒关系");
										}
										return true;
									}
									break;
								case Cmd::TEACHER_ANSWER_NO:
									{
										UserSession *master = NULL;
										master = UserSessionManager::getInstance()->getUserByTempID(ptCmd->memberID);
										if (master) master->sendSysChat(Cmd::INFO_TYPE_FAIL, "他拒绝了你的提议！");
										return true;
									}
									break;
								default:
									break;
							}
							return true;
						}
						break;
					case Cmd::FIRE_MEMBER_FROM_SCHOOL_PARA: //开除某人
						{
							Cmd::stFireMemberFromSchoolCmd *ptCmd=(Cmd::stFireMemberFromSchoolCmd *)ptNullCmd;
							Zebra::logger->trace("[师徒]收到%s开除%s的指令",pUser->name, ptCmd->memberName);
							frieMember(pUser, ptCmd->memberName);
							return true;
						}
						break;
					case Cmd::SCHOOL_STATUS_CHECK_PARA: //门派创建前的条件检查
						{
							if (checkSchoolCreateCondition(pUser))
							{
								Cmd::stSchoolStatusCheckCmd ret;
								pUser->sendCmdToMe(&ret, sizeof(ret));
							}
							return true;
						}
						break;
					case Cmd::CREATE_SCHOOL_PARA: // 门派创建
						{
							Cmd::stCreateSchoolCmd *ptCmd=(Cmd::stCreateSchoolCmd *)ptNullCmd;
							if (checkSchoolCreateCondition(pUser))
							{
								if (createNewSchool(pUser->name, ptCmd->SchoolName))
								{
									Cmd::Session::t_SchoolCreateSuccess_SceneSession send;
									send.dwID = pUser->id;
									send.dwSchoolID = pUser->schoolid;
									strncpy(send.schoolName, ptCmd->SchoolName, MAX_NAMESIZE);
									pUser->scene->sendCmd(&send, sizeof(send));
								}
								else
								{
									pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "遗憾的通知你门派名称已被占用，请换个名字再试试!");
								}
							}
							return true;
						}
						break;
					case Cmd::SCHOOL_BULLETIN__PARA: // 门派公告
						{
							Cmd::stSchoolBulletinCmd *ptCmd=(Cmd::stSchoolBulletinCmd *)ptNullCmd;
							processBulletin(pUser, ptCmd);
							return true;
						}
						break;
					case Cmd::SCHOOLMEMBER_LEAVE_SCHOOL_PARA: // 成员申请脱离关系
						{
							processLeaveGroup(pUser);
							return true;
						}
						break;
					case Cmd::DESTROY_SCHOOL_PARA: // 解散门派
						{
							if (!destroySchool(pUser->name))
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "解散无效!");
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
	return false;
}

/**
 * \brief 解析场景的消息
 * 一定失败
 *
 * \param ptNullCmd 待解析的消息
 * \param cmdLen 消息长度
 * \return 解析是否成功
 */
bool CSchoolM::processSceneMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
	return false;
}

/**
 * \brief 为玩家发送师门聊天消息
 *
 * \param pUser 发消息的玩家
 * \param rev 消息
 * \param cmdLen 消息长度
 * \return 
 */
void CSchoolM::sendSchoolChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *rev, const unsigned int cmdLen)
{
	CSchoolMember *member = getMember(pUser->name);
	if (member) 
		member->sendChatMessages(rev, cmdLen);
	else
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "没有加入师门或者门派不能使用此频道");
}

/**
 * \brief 为玩家发送师门私聊消息
 *
 * \param pUser 发消息的玩家
 * \param rev 消息
 * \param cmdLen 消息长度
 */
void CSchoolM::sendSchoolPrivateChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *rev, const unsigned int cmdLen)
{
	rwlock.rdlock();

	CSchoolMember *member = (CSchoolMember *)getMember(rev->pstrName);
	if (member)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::stChannelChatUserCmd *chatCmd;

		chatCmd = (Cmd::stChannelChatUserCmd *)buf;
		memcpy(chatCmd,rev,cmdLen);
		strncpy(chatCmd->pstrName, pUser->name,MAX_NAMESIZE);

		if (member->getUser())
		{
			member->getUser()->sendCmdToMe(chatCmd, cmdLen);
		}
		else
		{
			COfflineMessage::writeOfflineMessage(chatCmd->dwType, member->id, chatCmd,cmdLen);
		}
	}
	rwlock.unlock();
}

/**
 * \brief 处理玩家离开门派
 *
 * \param pUser 脱离门派的玩家
 */
void CSchoolM::processLeaveGroup(UserSession *pUser)
{
	CSchoolMember *member = NULL;
	member = getMember(pUser->name);
	if (member)
	{
		CSchool *school = NULL;
		school = member->getSchool();
		if (school)
		{
			CSchoolMember *teacher = member->getTeacher();
			if (school->processLeaveSchool(member))
			{
				if (school->id >0)
				{
					Cmd::stChannelChatUserCmd send;
					send.dwType = Cmd::CHAT_TYPE_SYSTEM;
					send.dwChannelID = 0;
					strncpy(send.pstrName,pUser->name,MAX_NAMESIZE);
					snprintf(send.pstrChat, sizeof(send.pstrChat) - 1, "%s选择了离开门派", pUser->name);
					school->sendCmdToSchool(&send, sizeof(send));

					/// 开始清楚相关关系ID
					pUser->schoolid = 0;
					Cmd::Session::t_sendUserRelationID sendrl;
					sendrl.dwUserID = pUser->id;
					sendrl.caption = 0;
					sendrl.dwID = 0;
					sendrl.type = Cmd::Session::RELATION_TYPE_SCHOOL;
					pUser->scene->sendCmd(&sendrl, sizeof(Cmd::Session::t_sendUserRelationID));
				}
				else
				{
					if(teacher) teacher->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s选择了离开师门!",pUser->name);
				}
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不能使用此功能，抱歉！");
			}
		}
		else
		{
			Zebra::logger->error("CSchoolM::processLeaveGroup(): %s对应节点无法取到节点管理器的对象", member->name);
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你并没有加入任何的师门或者门派啊？！");
	}

}

/**
 * \brief 处理玩家离开门派
 *
 * \param roleName 离开门派的玩家姓名
 */
void CSchoolM::processLeaveGroupDirect(const char* roleName)
{
	CSchoolMember *member = NULL;
	member = getMember(roleName);
	
	if (member)
	{
		CSchool *school = NULL;
		school = member->getSchool();
		if (school)
		{
			CSchoolMember *teacher = member->getTeacher();
			if (school->processLeaveSchool(member))
			{
				if (school->id >0)
				{
					Cmd::stChannelChatUserCmd send;
					send.dwType = Cmd::CHAT_TYPE_SYSTEM;
					send.dwChannelID = 0;
					strncpy(send.pstrName, roleName,MAX_NAMESIZE);
					snprintf(send.pstrChat, sizeof(send.pstrChat) - 1, "%s选择了离开门派", roleName);
					school->sendCmdToSchool(&send, sizeof(send));
				}
				else
				{
					if(teacher) teacher->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s选择了离开师门!", roleName);
				}
			}
		}
		else
		{
			Zebra::logger->error("CSchoolM::processLeaveGroup(): %s对应节点无法取到节点管理器的对象", member->name);
		}
	}
}
/**
 * \brief 创建门派
 *
 * \param  userName 创建者名字
 * \param schoolName 门派名字
 * \return 是否创建成功
 */
bool CSchoolM::createNewSchool(const char *userName, const char *schoolName)
{
	std::map<std::string, CSchoolMember*>::iterator tIterator;
	rwlock.rdlock();
	tIterator = findMemberIndex(userName);
	rwlock.unlock();
	if (tIterator != memberIndex.end())
	{
		CSchool *pSchool = new CSchool();
		if (pSchool)
		{
			pSchool->setSchoolName(schoolName);
			pSchool->setMasterSerialID(tIterator->second->getSerialID());
			if (pSchool->insertSchoolToDB())
			{
				rwlock.rdlock();
				CSchool *nonSchool = (CSchool *)getEntryByID(0); //获得师门管理器
				rwlock.unlock();
				if (nonSchool)
				{
					if (nonSchool->moveMemberToNewSchool(userName,pSchool))
					{
						pSchool->getMasterNode()->getUser()->schoolid = pSchool->id; // 初始化本地Session的schoolid
						rwlock.wrlock();
						addEntry(pSchool);
						rwlock.unlock();
						return true;
					}
				}
				else
				{
					Zebra::logger->error("CSchoolM::createNewSchool 无法取得门派管理器");
				}
			}
			SAFE_DELETE(pSchool);
		}
		else
		{
			Zebra::logger->error("建立门派对象的时候无法分配内存");
		}
	}
	return false;
}

/**
 * \brief 解散门派
 *
 *
 * \param pName 要解散的门派名字
 * \return 是否成功
 */
bool CSchoolM::destroySchool(const char *pName)
{
	std::map<std::string, CSchoolMember*>::iterator tIterator;
	rwlock.rdlock();
	tIterator = findMemberIndex(pName);
	rwlock.unlock();
	if (tIterator != memberIndex.end())
	{
		CSchoolMember *member = tIterator->second;
		if (member)
		{
			if (member->getSchoolID() > 0)
			{
				CSchool *school = NULL;
				school = member->getSchool();
				if (school)
				{
					if (school->getMasterNode() == member)
					{
						if (school->getMasterNode()
								&&  CDareM::getMe().findDareRecordByID
								(Cmd::SCHOOL_DARE, school->getID()) == NULL)
						{
							UserSession* pUser = school->getMasterNode()->getUser();
							if (school->moveMemberToTeacherGroup())
							{
								//school->getMasterNode()->getUser()->schoolid = 0;
								school->deleteSchoolFromDB();
								SAFE_DELETE(school);
								if (pUser)
								{
									pUser->sendSysChat(Cmd::INFO_TYPE_GAME,
											"师门已解散");
								}

								return true;
							}
						}
						else
						{
							if (school->getMasterNode() && school->getMasterNode()->getUser())	
							{
								school->getMasterNode()->getUser()->sendSysChat(Cmd::INFO_TYPE_FAIL,
										"师门对战状态，不允许解散师门");
							}

						}
					}
				}
			}
		}
	}
	return false;
}

/**
 * \brief 检查角色是否可以创建门派
 *
 * \param pUser 想创建的人
 * \return 是否可以创建
 */
bool CSchoolM::checkSchoolCreateCondition(const UserSession *pUser)
{
	std::map<std::string, CSchoolMember*>::iterator tIterator;
	rwlock.rdlock();
	tIterator = findMemberIndex(pUser->name);
	rwlock.unlock();
	if (tIterator != memberIndex.end())
	{
		CSchoolMember *me = tIterator->second;
		if (!me->havePreNode())
		{
			if (me->getSchoolID() == 0)
			{
				if (me->checkSchoolCreateCondition())
				{
					return true;
				}
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "要成立门派你徒弟和徒弟的徒弟都必须收满%u人，并且徒弟等级要全部达到%u级, 徒孙等级要全部达到%u级, 你去检查一下再来吧",
							MAX_PRENTICE, 
							FIRST_LAYER_PRENTICE_REQUEST_LEVEL, 
							SECOND_LAYER_PRENTICE_REQUEST_LEVEL);
				}
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已经是师尊了，如果你愿意就把门派解散了重新来过吧!");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你有师傅不能成立门派，先和你的师傅解除关系再来吧!");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你还没有徒弟，等你有了徒孙再来吧!");
	}
	return false;
}

/**
 * \brief 判断用户的徒弟是否满
 *
 * \param master 被检查的角色名称
 * param schoolName 门派名称，如果此人的徒弟未满且此人是有门派的那么门派名称通过这里返回
 * \return 返回true表示master的徒弟没收满， 返回false表示master的徒弟已经招收满了
 */
bool CSchoolM::getUserPrenticeInfo(const char *master,char *schoolName)
{
	std::map<std::string, CSchoolMember*>::iterator tIterator;
	bool ret;

	rwlock.rdlock();
	tIterator = findMemberIndex(master);
	if (tIterator != memberIndex.end())
	{
		if (tIterator->second->getPrenticeCount() <MAX_PRENTICE)
		{
			ret = true;
			char *point = tIterator->second->getSchoolName();
			if (point)
			{
				strncpy(schoolName, point, MAX_NAMESIZE);
			}
		}
		else
			ret = false;
	}
	else
	{
		ret = true;
	}
	rwlock.unlock();
	return ret;
}

/**
 * \brief 判断某个人是否为干净的，意思是他没有师傅和徒弟的关系，即他不在此管理器中
 *
 * \param name 被检查的角色的名称
 * \return true为干净的，false 为已经有师或徒弟关系
 */
bool CSchoolM::isClean(const char *name)
{
	std::map<std::string, CSchoolMember*>::iterator tIterator;
	rwlock.rdlock();
	tIterator = findMemberIndex(name);
	rwlock.unlock();
	if (tIterator == memberIndex.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * \brief 为 master 增加一个徒弟 prentice
 * 在函数中会根据两人的情况进行具体的操作，如果不成功会给对应角色发送消息
 *
 * \param master 师傅
 * \param prentice 徒弟
 */
void CSchoolM::addMember(UserSession *master, UserSession *prentice)
{
	std::map<std::string, CSchoolMember*>::iterator tIterator;
	CSchoolMember *pPrentice = NULL;
	rwlock.rdlock();
	tIterator = findMemberIndex(master->name);
	rwlock.unlock();
	if (tIterator != memberIndex.end())
	{
		CSchoolMember *pMember = tIterator->second;
		if (NULL != (pPrentice = pMember->addPrentice(prentice)))
		{
			pPrentice->notifyNewMemberAdd(); // 通知周边新成员加入
			if (prentice)
			{
				prentice->schoolid = master->schoolid;
				Cmd::Session::t_sendUserRelationID send;
				send.dwUserID = prentice->id;
				send.caption = master->id;
				send.dwID = master->schoolid;
				send.type = Cmd::Session::RELATION_TYPE_SCHOOL;
				prentice->scene->sendCmd(&send, sizeof(Cmd::Session::t_sendUserRelationID));
			}
		}
		else
		{
			master->sendSysChat(Cmd::INFO_TYPE_FAIL, "请通知GM查证，暂时无法为你办理徒弟招收事宜");
		}
	}
	else
	{
		CSchool *noneSchool = (CSchool *)getEntryByID(0); // 取得师门关系管理器
		CSchoolMember *masterMember = NULL;
		masterMember = noneSchool->addTeacher(master);
		if (masterMember)
		{
			if (NULL == (pPrentice = masterMember->addPrentice(prentice)))
			{
				Zebra::logger->trace("[师徒]增加徒弟节点[%s]失败删除师傅节点[%s]", master->name, prentice->name);
				masterMember->deleteMe();
				SAFE_DELETE(masterMember);
				return;
			}
			else
			{
				if (masterMember)	masterMember->sendInfomationToMe();
				if (pPrentice)
				{
					pPrentice->notifyNewMemberAdd();
					pPrentice->sendNotifyToScene();
				}
			}
		}
		else
		{
			master->sendSysChat(Cmd::INFO_TYPE_FAIL, "请通知GM查证，暂时无法为你办理徒弟招收事宜");
			Zebra::logger->error("无法在School模块中再添加节点addMember方法失败");
			return;
		}
	}
}
/**
 * \brief 开除门派成员
 *
 *
 * \param roleName 成员名字
 * \param find 如果只是查找，则不开除成员
 * \return 1:成功 0：不可以解除社会关系 -1：没找到门派 2：不是该门派的成员
 */
int CSchoolM::fireSchoolMember(const char* roleName, bool find)
{
	struct findList : public execEntry<CSchool>
	{
		const char* _roleName;
		int _status;
		CSchool* _pSchool;

		findList(const char* roleName)
		{
			_roleName = roleName;
			_status = 2;
			_pSchool = NULL;
		}
		~findList(){}

		void removeSchoolMember()
		{
			if (_status == 1)
			{
				Zebra::logger->trace("%s 是师门成员，解除其社会关系", _roleName);

				if (_pSchool)
				{

					CSchoolMember *pMember = _pSchool->getMember(_roleName);
					if (pMember)
					{
						if (pMember->getTeacher())
						{
							_pSchool->processLeaveSchool(pMember);
						}
						else
						{
							pMember->fireAllPrentice();
							if (pMember->isClean())
							{
								pMember->sendDestroyNotifyToMe();
								pMember->deleteMe();
								SAFE_DELETE(pMember);
							}
						}
					}
					
					/*if (CSchoolM::getMe().processLeaveGroupDirect(_roleName))
					{
						_status = 1;
					}
					else
					{
						_status = -1;
					}
					CSchoolM::getMe().processLeaveGroupDirect(_roleName);*/
					_status = 1;
				}
				else
				{
					_status = -1;
				}
			}

			if (_status == 3)
			{
				Zebra::logger->debug("%s 是师尊，解除其社会关系", _roleName);

				if (_pSchool)
				{
					CSchoolMember *pMember = _pSchool->getMember(_roleName);

					if (pMember)
					{
						pMember->fireAllPrentice();
					}

					CSchoolM::getMe().destroySchool(_roleName);
				
					if (pMember)
						pMember->deleteMe();
				}
			}
		}

		bool exec(CSchool *pSchool)
		{
			if (pSchool)
			{
				if (pSchool->getMasterNode() && pSchool->getMasterNode()->isMe(_roleName))
				{
#ifdef _ZJW_DEBUG
					Zebra::logger->debug("%s是师尊，解除社会关系", _roleName);
#endif
					_pSchool = pSchool;
					_status = 3;
					return false;
				}
				else
				{
					if (pSchool->isMember(_roleName))
					{
#ifdef _ZJW_DEBUG
						Zebra::logger->debug("%s 是 %s 师门成员，能解除社会关系", _roleName, pSchool->getSchoolName());
#endif
						_pSchool = pSchool;
						_status = 1;
						return false;
					}
					else
					{
#ifdef _ZJW_DEBUG
						Zebra::logger->debug("%s 不是 %s 的师门成员", _roleName, pSchool->getSchoolName());
#endif
						_status = 2;
					}
				}
			}

			return true;
		}
	};

	findList myList(roleName);
	execEveryOne(myList);

	if (!find)
	{
		myList.removeSchoolMember();
	}

	return myList._status;
}

/**
 * \brief master 开除一个徒弟 prentice
 * 在函数中会根据两人的情况进行具体的操作，如果不成功会给对应角色发送消息
 *
 * \param master 师傅
 * \param prentice 徒弟
 * \return 
 */
void CSchoolM::frieMember(UserSession *master,  const char *prentice)
{
	std::map<std::string, CSchoolMember*>::iterator tIterator;
	//	CSchoolMember *pPrentice = NULL;
	rwlock.rdlock();
	tIterator = findMemberIndex(master->name);
	rwlock.unlock();
	if (tIterator != memberIndex.end())
	{
		if (tIterator->second->firePrentice(prentice))
		{
			UserSession *pUser = UserSessionManager::getInstance()->getUserSessionByName(prentice);
			if (pUser)
			{
				pUser->schoolid = 0;
				Cmd::Session::t_sendUserRelationID send;
				send.dwUserID = pUser->id;
				send.caption = 0;
				send.dwID = 0;
				send.type = Cmd::Session::RELATION_TYPE_SCHOOL;
				pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_sendUserRelationID));
			}
			if ((tIterator->second->getSchoolID() == 0) &&(tIterator->second->isClean()))
			{
				CSchoolMember *pMember = tIterator->second;
				pMember->sendDestroyNotifyToMe();
				pMember->deleteMe();
				SAFE_DELETE(pMember);
			}
		}
		else
		{
			master->sendSysChat(Cmd::INFO_TYPE_FAIL, "你无法开除这个人");
		}
	}
	else
	{
		master->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是师傅你不能开除任何人!");
	}
}

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
 * \brief 向所有成员发送战争结果
 *
 * \param msg 要发的消息
 */
void CSchool::notifyWarResult(const char* msg, ...)
{
	char buf[1024+1];
	bzero(buf, sizeof(buf));

	struct findall : public execEntry<CSchoolMember> 
	{
		const char* msg;

		findall(const char* pMsg)
		{
			msg = pMsg;
		}

		~findall(){}

		bool exec(CSchoolMember* pMember)
		{
			if (pMember)
			{
				if (pMember->getUser())
				{
					pMember->getUser()->sendSysChat(Cmd::INFO_TYPE_SYS, msg);
				}
			}

			return true;
		}
	};

	getMessage(buf, 1024, msg);

	findall myList(buf);
	execEveryOne(myList);

}

/**
 * \brief 向所有成员发送对战状态消息
 *
 * \param ptEnterWarCmd 对战状态消息
 * \param cmdLen 消息长度
 */
void CSchool::sendCmdToAllMemberScene(Cmd::Session::t_enterWar_SceneSession* ptEnterWarCmd, const unsigned int cmdLen)
{

	struct findall : public execEntry<CSchoolMember>
	{
		Cmd::Session::t_enterWar_SceneSession* cmd;
		unsigned int cmdLen;

		findall(Cmd::Session::t_enterWar_SceneSession* ptCmd, const unsigned int nCmdLen)
		{
			cmd = ptCmd;
			cmdLen = nCmdLen;
		}

		~findall(){}

		bool exec(CSchoolMember* pMember)
		{
			if (pMember)
			{
				UserSession* pUser = pMember->getUser();
				if (pUser && pUser->scene)
				{
					cmd->dwUserID = pUser->id;
					cmd->dwSceneTempID = pUser->scene->tempid;
					if (cmd->dwStatus)
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已进入师门对战状态。");
					}
					else
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你已退出师门对战状态。");
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

/**
 * \brief 响应公告功能设置公告或者读取公告
 *
 * \param pUser 公告者
 * \param rev 公告消息
 */
void CSchoolM::processBulletin(const UserSession *pUser, const Cmd::stSchoolBulletinCmd *rev)
{
	std::map<std::string, CSchoolMember*>::iterator tIterator;
	CSchoolMember *me = NULL;
	rwlock.rdlock();
	tIterator = findMemberIndex(pUser->name);
	rwlock.unlock();
	if (tIterator != memberIndex.end())
	{
		me = tIterator->second;
		if (me)
		{
			if (me->getSchoolID() >0)
			{
				switch(rev->byState)
				{
					case Cmd::SCHOOL_BULLETIN_GET:
						{
							me->sendBulletinToMe();
						}
						break;
					case Cmd::SCHOOL_BULLETIN_SET:
						{
							char buf[zSocket::MAX_DATASIZE];

							bzero(buf, zSocket::MAX_DATASIZE);
							strncpy(buf, rev->data, rev->wdSize);
							me->setBulletin(buf);
						}
						break;
				}
				return;
			}
		}
	}

	pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你没有加入门派！无权使用公告相关功能！");
}

/**
 * \brief 根据名字得到玩家对象
 *
 *
 * \param pName 玩家名字
 * \return 找到的对象，失败返回0
 */
CSchoolMember *CSchoolM::getMember(const char *pName)
{
	std::map<std::string, CSchoolMember*>::iterator tIterator;
	CSchoolMember *member = NULL;
	rwlock.rdlock();
	tIterator = findMemberIndex(pName);
	if (tIterator != memberIndex.end()) member = tIterator->second;
	rwlock.unlock();
	return member;
}

/**
 * \brief 根据id得到门派对象
 *
 * \param id 门派id
 * \return 找到的对象，失败返回0
 */
CSchool * CSchoolM::getSchool(DWORD id)
{
	CSchool *school = NULL;
	rwlock.rdlock();
	school = (CSchool *)getEntryByID(id);
	rwlock.unlock();
	return school;
}


/**
 * \brief 设置玩家的级别
 *
 * \param pName 玩家名字
 * \param level 级别
 */
void CSchoolM::setUserLevel(const char *pName, const WORD &level)
{
	CSchoolMember *member = getMember(pName);

	if (!member)
		return;

	if (member)
	{
		member->setLevel(level);
	}

	if (member->getTeacher()!=NULL)
	{
		if ((abs((int)member->getTeacher()->getLevel() - (int)member->getLevel()) >= 20)  || (member->getLevel()>=80))
		{//师傅和徒弟等级相差20级, 或徒弟自己上升到80级,则解除师徒关系
			CSchoolMember* pTeacher = member->getTeacher();
			if (pTeacher->firePrentice(pName))
			{
				UserSession *pUser = UserSessionManager::getInstance()->getUserSessionByName(pName);
				if (pUser)
				{
					pUser->schoolid = 0;
					Cmd::Session::t_sendUserRelationID send;
					send.dwUserID = pUser->id;
					send.caption = 0;
					send.dwID = 0;
					send.type = Cmd::Session::RELATION_TYPE_SCHOOL;
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_sendUserRelationID));
				}

				if ((pTeacher->getSchoolID() == 0) && (pTeacher->isClean()))
				{
					pTeacher->sendDestroyNotifyToMe();
					pTeacher->deleteMe();
					SAFE_DELETE(pTeacher);
				}
			}
		}
	}
}

//--[CSchool]----------------------------------------------------------

/**
 * \brief 门派构造函数
 */
CSchool::CSchool()
{
	destroy = false;
}

/**
 * \brief 析构函数
 */
CSchool::~CSchool()
{
	if (!destroy)
	{
		updateSchoolInDB();
		rwlock.wrlock();
		for(zEntryID::hashmap::iterator it=zEntryID::ets.begin();it!=zEntryID::ets.end();it++)
		{
			CSchoolMember *temp = (CSchoolMember *)it->second;
			SAFE_DELETE(temp);
		}
		clear();
		rwlock.unlock();
	}
}

/**
 * \brief 设置门派名字
 *
 * \param pName 名字
 */
void CSchool::setSchoolName(const char *pName)
{
	strncpy(name,pName,MAX_NAMESIZE);
}

/**
 * \brief 得到门派名字
 *
 * \return 门派名字
 */
char *CSchool::getSchoolName()
{
	return name;
}

/**
 * \brief 设置师尊的节点id
 *
 * \param id 师尊的节点ID
 */
void CSchool::setMasterSerialID(const DWORD &id)
{
	dwMasterSerialID = id;
}

/**
 * \brief 将管理器初始化成师门关系管理器
 *
 * \return 返回NULL表示添加徒弟失败，否则返回徒弟的节点对象
 */
void CSchool::initToNoneSchool()
{
	id =0;
	bzero(name,sizeof(name));
}

/**
 * \brief 初始化管理器，根据信息结构，从数据库初始化的时候用
 *
 * \param info 管理器信息结构
 */
void CSchool::initSchool(const stSchoolInfo &info)
{
	rwlock.wrlock();
	id = info.dwSchoolID;
	strncpy(name,info.name, MAX_NAMESIZE);
	dwMasterSerialID = info.dwMasterSerialID;
	//bulletin = info.data;
	rwlock.unlock();
}

bool CSchool::addNode(const stSchoolMemberInfo &info)
{
	CSchoolMember *member = new CSchoolMember(this, NULL);

	if (member)
	{
		member->initByDBRecord(info);
		if (member->havePreNode())
		{
			CSchoolMember *pPreNode = (CSchoolMember *)getEntryByID(info.dwPreSerialID);
			if (pPreNode)
			{
				if (addMember(member))
				{
					if (pPreNode->addNextLevelNode(member))
					{
						member->setPreLevelNode(pPreNode);
					}
					else
					{
						removeMember(member);
						SAFE_DELETE(member);
					}
					return true;
				}
				else
				{
					SAFE_DELETE(member);
					Zebra::logger->error("%s节点管理器加入%s节点失败id为%u",name, info.name, info.dwSerialID);
				}
			}
			else
			{
				SAFE_DELETE(member);
				Zebra::logger->error("%s节点管理器中%s节点寻找师傅节点%u失败",name, info.name, info.dwMasterID);
			}
		}
		else
		{
			if (addMember(member))
			{
				return true;
			}
			else
			{
				SAFE_DELETE(member);
				Zebra::logger->error("%s节点管理器加入%s节点失败id为%u",name, info.name, info.dwSerialID);
			}
		}
	}
	else
	{
		Zebra::logger->error("初始化的时候内存分配失败 CSchool::addNode");
	}
	return false;
}

bool CSchool::updateSchoolInDB()
{
	/*
	   const dbCol school_update_define[] = {
	   { "BULLETIN",		zDBConnPool::DB_BIN2,	0},//MAX_BULLETIN_CHAR_NUMBER+2 }, 
	   { NULL, 0, 0}
	   };

	   BYTE buf[zSocket::MAX_DATASIZE];
	   struct stSchoolWrite
	   {
	   DWORD  size;
	   char data[0];
	   } __attribute__ ((packed))
	 * schooldata;
	 char where[128];

	 bzero(buf, zSocket::MAX_DATASIZE);
	 schooldata = (stSchoolWrite *)buf;
	 connHandleID handle = SessionService::dbConnPool->getHandle();
	 if ((connHandleID)-1 == handle)
	 {
	 Zebra::logger->error("不能获取数据库句柄");
	 return false;
	 }

	 rwlock.rdlock();

	 schooldata->size = bulletin.size();
	 if (schooldata->size>0) strncpy(schooldata->data, bulletin.data(), schooldata->size);

	 rwlock.unlock();

	 bzero(where, sizeof(where));
	 snprintf(where, sizeof(where) - 1, "SCHOOLID = %u", id);
	 SessionService::dbConnPool->exeUpdate(handle, "`SCHOOL`", school_update_define, (BYTE *)(schooldata), where);
	 SessionService::dbConnPool->putHandle(handle);
	 */
return true;
}


bool CSchool::insertSchoolToDB()
{
	//FunctionTimes times(2,__FUNCTION__);
	const dbCol school_write_define[] = {
		{ "NAME",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "MASTERSERIAL",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};
	static const dbCol verifyname_define[] = {
		{ "`NAME`", zDBConnPool::DB_STR, sizeof(char[MAX_NAMESIZE+1]) }, 
		{ NULL, 0, 0}
	};

	//	BYTE buf[zSocket::MAX_DATASIZE];
	char where[128];
	char strName[MAX_NAMESIZE+1];

	struct stSchoolWrite
	{
		char  name[MAX_NAMESIZE+1];
		DWORD dwMasterSerialID;
		//		WORD  size;
		//		char data[0];
	} __attribute__ ((packed))
	schooldata;

	//	bzero(buf, zSocket::MAX_DATASIZE);
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	//首先验证名称是否重复
	std::string escapeName;
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "NAME = '%s'", SessionService::dbConnPool->escapeString(handle,name,escapeName).c_str());
	unsigned int retcode = SessionService::dbConnPool->exeSelectLimit(handle, "`SCHOOL`", verifyname_define, where, NULL, 1, (BYTE *)(strName));
	if (retcode == 1)
	{
		SessionService::dbConnPool->putHandle(handle);
		return false;
	}

	rwlock.rdlock();

	strncpy(schooldata.name, name, MAX_NAMESIZE);
	schooldata.dwMasterSerialID = dwMasterSerialID;
	//	schooldata->size = bulletin.size();
	//	if (schooldata->size>0) strncpy(schooldata->data, bulletin.c_str(), schooldata->size);

	rwlock.unlock();

	retcode = SessionService::dbConnPool->exeInsert(handle, "`SCHOOL`", school_write_define, (const unsigned char *)(&schooldata));
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("插入SCHOOL表数据库出错 name = %s",schooldata.name);
		return false;
	}
	else
	{
		id = retcode;
	}
	return true;
}

bool CSchool::deleteSchoolFromDB()
{
	//FunctionTimes times(3,__FUNCTION__);
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "SCHOOLID = %u ", id);
	unsigned int retcode = SessionService::dbConnPool->exeDelete(handle, "`SCHOOL`", where);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("删除SCHOOL记录失败 SCHOOLID=%u", id);
		return false;
	}
	destroy = true;
	return true;
}

bool CSchool::loadSchoolMemberFromDB()
{
	//FunctionTimes times(4,__FUNCTION__);
	const dbCol schoolmember_read_define[] = {
		{ "SERIALID",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "MASTERID",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "PRESERIALID",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "CHARID",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "NAME",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "LEVEL",			zDBConnPool::DB_WORD,	sizeof(WORD) }, 
		{ "JOINTIME",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "DEGREE",			zDBConnPool::DB_WORD,	sizeof(WORD) },
		{ "LASTTIME",		zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ "SCHOOLID",		zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ "TAG",			zDBConnPool::DB_BYTE,	sizeof(BYTE) },
		{ "`OCCUPATION`",	zDBConnPool::DB_WORD,	sizeof(WORD) },
		{ "MASTERTOTAL",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "MASTERBALANCE",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "PRENTICELASTLVL",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "PRENTICETOTAL",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};

	stSchoolMemberInfo *memberList,*tempPoint;
	char where[128];

	memberList = NULL;
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	snprintf(where, sizeof(where) - 1, "SCHOOLID = %u ", id);
	unsigned int retcode = SessionService::dbConnPool->exeSelect(handle, "`SCHOOLMEMBER`", schoolmember_read_define, where, "SERIALID ASC", (BYTE **)&memberList);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode
			|| 0 == retcode)
	{
		return true;
	}

	bool ret = true;
	if (memberList)
	{
		tempPoint = &memberList[0];
		for (unsigned int i=0; i< retcode; i++)
		{
			if (!addNode(*tempPoint)) ret = false;
			tempPoint++;
		}
		SAFE_DELETE_VEC(memberList);
	}
	else
	{
		Zebra::logger->error("门派数据初始化失败，exeSelect 返回无效buf指针");
		ret = false;
	}
	return true; //不处理返回值了
}

// 加一个师傅节点，此人以前从来没有招收过徒弟，也没有被人收为徒弟，加一个师傅节点，其是非门派节点
CSchoolMember * CSchool::addTeacher(UserSession *master)
{
	if (id != 0) return NULL; // 如果这是一个门派管理器则不能使用此方法
	CSchoolMember *member = new CSchoolMember(this, master);
	if (member)
	{
		member->initRootMember();
		if (member->insertRecord())
		{
			rwlock.wrlock();
			addEntry(member);
			rwlock.unlock();
			CSchoolM::getMe().addMemberIndex(master->name, member);
		}
		else
		{
			SAFE_DELETE(member);
			master->sendSysChat(Cmd::INFO_TYPE_FAIL, "暂时无法为你招收徒弟，请通知GM查证");
		}
	}
	else
	{
		Zebra::logger->error("CSchool::addTeacher():内存分配失败，无法创建新的成员节点");
	}
	return member;
}

bool CSchool::addMember(CSchoolMember *member)
{
	bool ret = false;
	rwlock.wrlock();
	ret = addEntry(member);
	rwlock.unlock();
	if (ret) CSchoolM::getMe().addMemberIndex(member->name, member);
	return ret;
}

void CSchool::directAdddMember(CSchoolMember *member)
{
	rwlock.wrlock();
	addEntry(member);
	rwlock.unlock();
}

void CSchool::removeMember(CSchoolMember *member)
{
	CSchoolM::getMe().removeMemberIndex(member->name);
	rwlock.wrlock();
	removeEntry(member);
	rwlock.unlock();
}

void CSchool::directRemoveMember(CSchoolMember *member)
{
	rwlock.wrlock();
	removeEntry(member);
	rwlock.unlock();
}

void CSchool::sendCmdToSchool(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	rwlock.rdlock();
	for(zEntryID::hashmap::iterator it=zEntryID::ets.begin();it!=zEntryID::ets.end();it++)
	{
		CSchoolMember *temp =(CSchoolMember *)it->second;
		temp->sendCmdToMe(ptNullCmd, cmdLen);
	}
	rwlock.unlock();
}

bool CSchool::moveMemberToNewSchool(const char *userName, CSchool *pSchool)
{
	bool ret = true;
	if (id != 0) return false; // 门派管理器不能使用此方法
	rwlock.wrlock();
	CSchoolMember *master = getMember(userName);
	rwlock.unlock();
	if (master)
	{
		master->moveAllToSchool(pSchool);
		CSendSchoolCallback sscallback;
		master->getSchoolTree(sscallback);
		master->notifyAllReSendInitData(&sscallback);
		ret = true;
	}
	else
	{
		ret = false;
	}
	return ret;
}

bool CSchool::moveMemberToTeacherGroup()
{
	CSchoolMember *master = getMasterNode();
	CSchool *nonSchool = (CSchool *)CSchoolM::getMe().getSchool(0);
	if (nonSchool)
	{
		if (master)
		{
			CSchoolMemberListCallback memberSet;
			master->moveAllToSchool(nonSchool, &memberSet);
			memberSet.sendNotifyToMember();
			return true;
		}
	}
	return false;
}

CSchoolMember * CSchool::getMasterNode()
{
	CSchoolMember *member = NULL;
	rwlock.rdlock();
	member = (CSchoolMember *)getEntryByID(dwMasterSerialID);
	rwlock.unlock();
	return member;
}

void CSchool::setBulletin(const char *buf)
{
	rwlock.wrlock();
	bulletin = buf;
	rwlock.unlock();
	updateSchoolInDB();
}

const char * CSchool::getBulletin()
{
	return bulletin.data();
}

CSchoolMember *CSchool::getMember(const char *pName)
{
	CSchoolMember *member = NULL;

	member = CSchoolM::getMe().getMember(pName);
	if (member)
	{
		if (member->getSchool() != this) member = NULL;
	}
	return member;
}

bool	CSchool::isMember(const char* memberName)
{
	CSchoolMember *member = NULL;
	member = this->getMember(memberName);

	if (member == NULL)
	{
		return false;
	}

	return true;
}

bool CSchool::processLeaveSchool(CSchoolMember * member, bool deleteTeacher)
{
	if (member)
	{
		if (member->getSchool() != this) return false;
		if (member->getSchoolID() == 0)
		{
			Zebra::logger->trace("[师徒]processLeaveSchool(%s)", member->name);
			CSchoolMember *teacher = NULL;
			teacher = member->getTeacher();
			if (teacher)
			{
				teacher->directRemovePrentice(member);
				member->clearTeacherRelation();
				if (member->isClean())
				{
					member->sendDestroyNotifyToMe();
					member->deleteMe();
					SAFE_DELETE(member);
				}
				else
				{
					member->sendInfomationToMe();
					member->updateRecord();
				}
				if (teacher->isClean())
				{
					if (deleteTeacher)
					{
						teacher->sendDestroyNotifyToMe();
						teacher->deleteMe();
						SAFE_DELETE(teacher);
					}
				}
				else
				{
					teacher->notifyTeacherGroup();
					teacher->updateRecord();
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (member->getSerialID() == dwMasterSerialID) return false;
			if (member->getNextLevelNodeCount()>0)
			{
				member->notifySchoolMemberRemove();
				member->sendDestroyNotifyToMe();
				member->setInValid();
				member->updateRecord();
			}
			else
			{
				CSchoolMember *master = member->getPreLevelNode();
				member->notifySchoolMemberRemove();
				member->sendDestroyNotifyToMe();
				if (master) master->directRemovePrentice(member);
				member->deleteMe();
				SAFE_DELETE(member);

				// 下面的部分清除被删除节点以上无效节点直到遇到有效节点为止
				CSchoolMember *curMember = master;  
				CSchoolMember *preMember = NULL;

				while((!curMember->isValid())&&(curMember->getNextLevelNodeCount()==0))
				{
					preMember = curMember->getPreLevelNode();
					if (preMember) preMember->directRemovePrentice(curMember);
					curMember->deleteMe();
					SAFE_DELETE(curMember);
					if (preMember)
					{
						curMember = preMember;
					}
					else
					{
						break;
					}
				}
				if ((curMember != master) && (NULL != curMember))
				{
					curMember->updateRecord();
				}
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

//--[CSchoolMember]----------------------------------------------------
CSchoolMember::CSchoolMember(CSchool *pSchool, UserSession *pUser)
{
	school = pSchool;
	user = pUser;
	preLevelNode = NULL;		// 师傅
	id				= 0;
	dwMasterID		= 0;
	dwPreSerialID	= 0;
	dwCharID		= 0;
	bzero(name,sizeof(name));
	wdLevel			= 0;
	dwJoinTime		= 0;
	wdDegree		= 0;
	dwLastTime		= 0;
	dwSchoolID		= 0;
	byTag			= 0;
	wdOccupation	= 0;

	destroy = false;
}

DWORD CSchoolMember::getCharID()
{
	return dwCharID;
}

DWORD CSchoolMember::getSerialID()
{
	return id;
}

DWORD CSchoolMember::getSchoolID()
{
	return dwSchoolID;
}

char *CSchoolMember::getSchoolName()
{
	if (school)
	{
		return school->getSchoolName();
	}
	else
	{
		return NULL;
	}
}

CSchool * CSchoolMember::getSchool()
{
	return school;
}

BYTE CSchoolMember::getPrenticeCount()
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;
	BYTE count = 0;

	rwlock.rdlock();
	for(tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		if (tIterator->second->isValid()) count++;
	}
	rwlock.unlock();
	return count;
}

BYTE CSchoolMember::getNextLevelNodeCount()
{
	BYTE count =0;
	rwlock.rdlock();
	count = prenticeList.size();
	rwlock.unlock();
	return count;
}


WORD CSchoolMember::getDegree()
{
	return wdDegree;
}

DWORD CSchoolMember::getJoinTime()
{
	return dwJoinTime;
}

DWORD CSchoolMember::getLastTime()
{
	return dwLastTime;
}

WORD CSchoolMember::getLevel()
{
	return wdLevel;
}

CSchoolMember *CSchoolMember::addPrentice(UserSession *pUser)
{
	CSchoolMember *returnMember = NULL;
	std::pair<std::map<DWORD, CSchoolMember*, ltword>::iterator, bool> retval;

	if (getPrenticeCount() == MAX_PRENTICE) return NULL;
	if (prenticeList.size() > getPrenticeCount())
	{
		CSchoolMember *member = getFirstInValideNode();
		if (member)
		{
			member->setUser(pUser);
			member->initInValidNode(this);
			if (member->updateRecord())
			{
				directRemovePrentice(member);
				rwlock.wrlock();
				retval = prenticeList.insert(prenticeListValueType(member->getJoinTime(),member));
				rwlock.unlock();
				if (retval.second == false)
				{
					Zebra::logger->error("[师徒]: %s 添加 %s 徒弟节点失败(%u)--A", this->name, member->name, member->getJoinTime());
				}
				CSchoolM::getMe().addMemberIndex(member->name,member);
				returnMember = member;
			}
			else
			{
				member->setInValid();
			}
		}
		else
		{
			Zebra::logger->error("CSchoolMember::addPrentice():%s的无效下级节点存在问题，请查证",name);
		}
	}
	else
	{
		CSchoolMember *member = new CSchoolMember(school, pUser);

		if (member)
		{
			member->initGeneralMember(this);
			if (member->insertRecord())
			{
				rwlock.wrlock();
				retval = prenticeList.insert(prenticeListValueType(member->getJoinTime(),member));
				rwlock.unlock();
				if (retval.second == false)
				{
					Zebra::logger->error("[师徒]: %s 添加 %s 徒弟节点失败(%u)--B", this->name, member->name, member->getJoinTime());
					member->deleteRecord();
					SAFE_DELETE(member);
				}
				else
				{
					member->setPreLevelNode(this);
					school->addMember(member);
					returnMember = member;
				}
			}
			else
			{
				SAFE_DELETE(member);
			}
		}
		else
		{
			Zebra::logger->error("CSchoolMember::addPrentice():内存分配失败，无法创建新的成员节点");
		}
	}
	if (returnMember) updateRecord();
	return returnMember;
}

bool CSchoolMember::isClean()
{
	bool ret = true;

	if (dwMasterID >0) ret = false;
	if (getPrenticeCount()>0) ret = false;
	return ret;
}

void CSchoolMember::deleteMe()
{
	Zebra::logger->trace("[师徒]节点被移除数据库记录被删掉 Name=[%s] CharID=[%u] MasterID=[%u] PreSerialID=[%u]",this->name, this->dwCharID, this->dwMasterID, this->dwPreSerialID);
	school->removeMember(this);
	deleteRecord();
	destroy = true;
}

void CSchoolMember::online(UserSession *pUser)
{
	zRTime ctv;

	rwlock.wrlock();
	user = pUser;
	wdLevel = pUser->level;
	wdOccupation = pUser->occupation;

	//if (pUser->schoolid != school->id)
	//{
		pUser->schoolid = dwSchoolID;;
		Cmd::Session::t_sendUserRelationID send;
		send.dwUserID = pUser->id;
		CSchoolMember * pTeacher = this->getTeacher();
		if (pTeacher)
			send.caption = pTeacher->getCharID();
		else
			send.caption = 0;
		send.dwID = dwSchoolID;
		send.type = Cmd::Session::RELATION_TYPE_SCHOOL;
		pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_sendUserRelationID));
	//}

	DWORD curTime = ctv.sec();

	if (curTime - dwLastTime >= MAX_GROUP_TIME_GAP)
	{
		if (wdDegree - DEDUCT_POINT >=0)
			wdDegree-=DEDUCT_POINT;
		else
			wdDegree = 0;
		dwLastTime = curTime;
	}
	rwlock.unlock();
	sendInfomationToMe();
	sendOnlineStatusMessage(Cmd::SCHOOL_MEMBER_ONLINE);
}

void CSchoolMember::sendOnlineStatusMessage(BYTE onlineStatus)
{
	Cmd::stSchoolMemberStatusCmd send;
	send.byState = onlineStatus;
	send.level = wdLevel;	
	strncpy(send.name, name, MAX_NAMESIZE);


	char *tempStr = CUnionM::getMe().getUnionNameByUserName(name);
	if (tempStr) strncpy(send.unionName, tempStr, MAX_NAMESIZE);
	else strncpy(send.unionName, "无", MAX_NAMESIZE);
	tempStr = NULL;
	tempStr = CSeptM::getMe().getSeptNameByUserName(name);
	if (tempStr) strncpy(send.septName, tempStr, MAX_NAMESIZE);
	else strncpy(send.septName, "无", MAX_NAMESIZE);
	send.country = CUnionM::getMe().getCountryIDByUserName(name);
	if (send.country == 0)
	{
		if (this->user) send.country = user->country;
	}

	if (dwSchoolID >0)
	{
		if (school) school->sendCmdToSchool(&send, sizeof(send));
	}
	else
	{
		if (preLevelNode) preLevelNode->sendCmdToTeacherGroup(&send, sizeof(send));
		sendCmdToTeacherGroup(&send, sizeof(send), true);
	}
}

void CSchoolMember::prenticeRequestMemberInfo(unsigned int &count, Cmd::stTeacherMember *point, CSchoolMember *me)
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;
	bool brother = true;

	if (byTag)
	{
		point->degree = wdDegree;
		strncpy(point->name, name, MAX_NAMESIZE);

		char *tempStr = CUnionM::getMe().getUnionNameByUserName(name);
		if (tempStr) strncpy(point->unionName, tempStr, MAX_NAMESIZE);
		else strncpy(point->unionName, "无", MAX_NAMESIZE);
		tempStr = NULL;
		tempStr = CSeptM::getMe().getSeptNameByUserName(name);
		if (tempStr) strncpy(point->septName, tempStr, MAX_NAMESIZE);
		else strncpy(point->septName, "无", MAX_NAMESIZE);
		point->country = CUnionM::getMe().getCountryIDByUserName(name);
		if (point->country == 0)
		{
			if (this->user) point->country = user->country;
		}

		point->tag = Cmd::TEACHER;
		point->online = isOnline();
		point->occupation = wdOccupation;
		point->level = wdLevel;
		count++;
		point++;
	}

	for(tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		if (tIterator->second->byTag == 0) continue;
		if (tIterator->second == me)
		{
			brother = false;
			continue;
		}

		char *tempStr = CUnionM::getMe().getUnionNameByUserName(tIterator->second->name);
		if (tempStr) strncpy(point->unionName, tempStr, MAX_NAMESIZE);
		else strncpy(point->unionName, "无", MAX_NAMESIZE);
		tempStr = NULL;
		tempStr = CSeptM::getMe().getSeptNameByUserName(tIterator->second->name);
		if (tempStr) strncpy(point->septName, tempStr, MAX_NAMESIZE);
		else strncpy(point->septName, "无", MAX_NAMESIZE);
		point->country = CUnionM::getMe().getCountryIDByUserName(tIterator->second->name);
		if (point->country == 0)
		{
			if (tIterator->second->user) point->country = tIterator->second->user->country;
		}

		point->degree = tIterator->second->getDegree();
		strncpy(point->name, tIterator->second->name, MAX_NAMESIZE);
		if (brother)
			point->tag = Cmd::BIGBROTHER;
		else
			point->tag = Cmd::LITTLEBROTHER;
		point->online = tIterator->second->isOnline();
		point->occupation = tIterator->second->getOccupation();
		count++;
		point++;
	}
}

void CSchoolMember::sendInfomationToMe(CSendSchoolCallback *callback)
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;
	Cmd::stMemberStatusSchool ret;

	if (NULL == user && byTag) return;
	//--[向客户端发送类型初始化消息]------------------------
	if (0 == dwSchoolID)
	{
		ret.byStatus = Cmd::SCHOOL_STATUS_TEACHER;
	}
	else
	{
		ret.byStatus = Cmd::SCHOOL_STATUS_SCHOOL;
		strncpy(ret.schoolName, school->name, MAX_NAMESIZE);
	}
	user->sendCmdToMe(&ret, sizeof(ret));
	////////////////////////////////////////////////////////

	BYTE buf[zSocket::MAX_DATASIZE];
	unsigned int count;
	Cmd::stTeacherMember *tempPoint;

	Cmd::stSendMemberInfoCmd *retCmd=(Cmd::stSendMemberInfoCmd *)buf;
	constructInPlace(retCmd);
	tempPoint = (Cmd::stTeacherMember *)retCmd->list;
	count = 0;
	if (preLevelNode)
	{
		preLevelNode->prenticeRequestMemberInfo(count, tempPoint, this);
		tempPoint+=count;
	}

	rwlock.rdlock();
	for(tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		if (tIterator->second->byTag == 0) continue;

		char *tempStr = CUnionM::getMe().getUnionNameByUserName(tIterator->second->name);
		if (tempStr) strncpy(tempPoint->unionName, tempStr, MAX_NAMESIZE);
		else strncpy(tempPoint->unionName, "无", MAX_NAMESIZE);
		tempStr = NULL;
		tempStr = CSeptM::getMe().getSeptNameByUserName(tIterator->second->name);
		if (tempStr) strncpy(tempPoint->septName, tempStr, MAX_NAMESIZE);
		else strncpy(tempPoint->septName, "无", MAX_NAMESIZE);
		tempPoint->country = CUnionM::getMe().getCountryIDByUserName(tIterator->second->name);
		if (tempPoint->country == 0)
		{
			if (tIterator->second->user) tempPoint->country = tIterator->second->user->country;
		}

		tempPoint->degree = tIterator->second->getDegree();
		strncpy(tempPoint->name, tIterator->second->name, MAX_NAMESIZE);
		tempPoint->online = tIterator->second->isOnline();
		tempPoint->occupation = tIterator->second->getOccupation();
		tempPoint->level = tIterator->second->getLevel();
		tempPoint->tag = Cmd::PRENTICE;
		count++;
		tempPoint++;
	}

	rwlock.unlock();
	retCmd->size = count;
	user->sendCmdToMe(retCmd,(count*sizeof(Cmd::stTeacherMember)+sizeof(Cmd::stSendMemberInfoCmd)));

	if (0 != dwSchoolID)
	{
		if (callback == NULL)
		{
			CSchoolMember * master = school->getMasterNode();
			if (master)
			{
				CSendSchoolCallback sscallback;
				master->getSchoolTree(sscallback);
				sscallback.sendListToMember(this);
			}
		}
		else
		{
			callback->sendListToMember(this);
		}
	}
}

void CSchoolMember::offline()
{
	user = NULL;
	sendOnlineStatusMessage(Cmd::SCHOOL_MEMBER_OFFLINE);
}

void CSchoolMember::getMyBigBrother(CSchoolMember *member, char *pName)
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;
	CSchoolMember *brother = NULL;

	for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		if (!tIterator->second->isValid()) continue;
		if (tIterator->second == member)
		{
			break;
		}
		else
		{
			brother = tIterator->second;
		}
	}
	if (tIterator == prenticeList.end()) brother = NULL;
	if (brother)
	{
		strncpy(pName,brother->name,MAX_NAMESIZE);
	}
	else
	{
		bzero(pName,MAX_NAMESIZE);
	}
}

void CSchoolMember::notifyNewMemberAdd()
{
	if (dwSchoolID == 0)
	{// 在师门中的处理方法
		if (preLevelNode) preLevelNode->notifyTeacherGroup();

		CSchoolMember *myTeacher = getTeacher();
		if (myTeacher)
		{
			char buf[MAX_CHATINFO];
			Cmd::stChannelChatUserCmd send;
			send.dwType=Cmd::CHAT_TYPE_SYSTEM;
			send.dwSysInfoType = Cmd::INFO_TYPE_GAME;
			bzero(send.pstrName, sizeof(send.pstrName));
			bzero(send.pstrChat, sizeof(send.pstrChat));
			strncpy(send.pstrName,"公告",MAX_NAMESIZE);
			sprintf(buf, "你成功招收%s为徒弟", this->name);
			strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);
			myTeacher->sendCmdToMe(&send, sizeof(send));
			sprintf(buf, "你成为了%s的徒弟", myTeacher->name);
			strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);
			sendCmdToMe(&send, sizeof(send));
		}
	}
	else
	{// 在门派中的处理方法
		Cmd::stAddMemberToSchoolCmd ret;
		strncpy(ret.memberName, name, MAX_NAMESIZE);
		ret.byState = Cmd::TEACHER_ANSWER_YES;
		ret.degree = 0;
		ret.wdOccupation = wdOccupation;
		if (user)
		{
			ret.level = user->level;
		}
		
		bzero(ret.prename, sizeof(ret.prename));
		if (preLevelNode)
		{
			preLevelNode->getMyBigBrother(this, ret.prename);
			preLevelNode->getMyLayer(ret.degree);
		}
		school->sendCmdToSchool(&ret, sizeof(ret));
		if (preLevelNode) 
			preLevelNode->notifyTeacherGroup();
		else 
			sendInfomationToMe();
	}
}

void CSchoolMember::initInValidNode(CSchoolMember * master)
{
	zRTime ctv;
	rwlock.wrlock();
	dwMasterID = master->getCharID();			// 师傅的ID
	dwPreSerialID = master->getSerialID();		// 前一个节点的ID
	dwCharID = user->id;						// 自己的角色ID
	strncpy(name, user->name, MAX_NAMESIZE);	// 自己的角色名字
	wdLevel = user->level;						// 角色的当前级别
	dwJoinTime = ctv.sec();						// 节点创建时间，跟如果有师兄弟这个就是排位的关键
	wdDegree = 0;								// 友好度为零，这里友好度是指本人与自己师傅的友好度
	dwLastTime = 0;								// 最后组队时间，默认设置为0
	dwSchoolID = master->getSchoolID();			// 拿取师傅的门派信息
	byTag = 1;									// 节点是有效的，这个标志只有在门派中才有意义
	wdOccupation = 0;							// 角色的职业
	rwlock.unlock();
}

void CSchoolMember::initGeneralMember(CSchoolMember * master)
{
	zRTime ctv;
	rwlock.wrlock();
	id = 0;										// 本节点的ID
	dwMasterID = master->getCharID();			// 师傅的ID
	dwPreSerialID = master->getSerialID();		// 前一个节点的ID
	dwCharID = user->id;						// 自己的角色ID
	strncpy(name, user->name, MAX_NAMESIZE);	// 自己的角色名字
	wdLevel = user->level;						// 角色的当前级别
	dwJoinTime = ctv.sec();						// 节点创建时间，跟如果有师兄弟这个就是排位的关键
	wdDegree = 0;								// 友好度为零，这里友好度是指本人与自己师傅的友好度
	dwLastTime = 0;								// 最后组队时间，默认设置为0
	dwSchoolID = master->getSchoolID();			// 拿取师傅的门派信息
	byTag = 1;									// 节点是有效的，这个标志只有在门派中才有意义
	wdOccupation = user->occupation;			// 角色的职业
	rwlock.unlock();
}

void CSchoolMember::initRootMember()
{
	zRTime ctv;
	rwlock.wrlock();
	id = 0;											// 本节点的ID
	dwMasterID = 0;									// 师傅的ID
	dwPreSerialID = 0;								// 前一个节点的ID
	dwCharID = user->id;							// 自己的角色ID
	strncpy(name, user->name, MAX_NAMESIZE);		// 自己的角色名字
	wdLevel = user->level;							// 角色的当前级别
	dwJoinTime = ctv.sec();							// 节点创建时间，跟如果有师兄弟这个就是排位的关键
	wdDegree = 0;									// 友好度为零，这里友好度是指本人与自己师傅的友好度，根节点的友好度无用处
	dwLastTime = 0;									// 最后组队时间，默认设置为0
	dwSchoolID = 0;									// 表示无门派
	byTag = 1;										// 节点是有效的，这个标志只有在门派中才有意义
	wdOccupation = user->occupation;				// 角色的职业
	rwlock.unlock();
}

bool CSchoolMember::insertRecord()
{
	//FunctionTimes times(4,__FUNCTION__);
	static const dbCol createschoolmember_define[] = {
		{ "`MASTERID`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`PRESERIALID`",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`CHARID`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`NAME`",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`LEVEL`",		zDBConnPool::DB_WORD,	sizeof(WORD) },
		{ "`JOINTIME`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ "`DEGREE`",		zDBConnPool::DB_WORD,	sizeof(WORD) },
		{ "LASTTIME",		zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ "`SCHOOLID`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ "`TAG`",			zDBConnPool::DB_BYTE,	sizeof(BYTE) },
		{ "`OCCUPATION`",		zDBConnPool::DB_WORD,	sizeof(WORD) },
		{ "MASTERTOTAL",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "MASTERBALANCE",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "PRENTICELASTLVL",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "PRENTICETOTAL",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};
	struct {
		DWORD dwMasterID;
		DWORD dwPreSerialID;
		DWORD dwCharID;
		char  name[MAX_NAMESIZE+1];
		WORD  wdLevel;
		DWORD dwJoinTime;
		WORD  wdDegree;
		DWORD dwLastTime;
		DWORD dwSchoolID;
		BYTE  byTag;
		WORD  wdOccupation;
		DWORD master_total;
		DWORD master_balance;
		DWORD prentice_lastlevel;
		DWORD prentice_total;
	} __attribute__ ((packed))
	createschoolmember_data;

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}
	//插入数据库角色信息
	bzero(&createschoolmember_data, sizeof(createschoolmember_data));

	rwlock.wrlock();
	createschoolmember_data.dwMasterID		= dwMasterID;
	createschoolmember_data.dwPreSerialID	= dwPreSerialID;
	createschoolmember_data.dwCharID		= dwCharID;
	strncpy(createschoolmember_data.name, name, MAX_NAMESIZE);
	createschoolmember_data.wdLevel			= wdLevel;
	createschoolmember_data.dwJoinTime		= dwJoinTime;
	createschoolmember_data.wdDegree		= wdDegree;
	createschoolmember_data.dwLastTime		= dwLastTime;
	createschoolmember_data.dwSchoolID		= dwSchoolID;
	createschoolmember_data.byTag			= byTag;
	createschoolmember_data.wdOccupation	= wdOccupation;

	createschoolmember_data.master_total	= master_total;
	createschoolmember_data.master_balance	= master_balance;
	createschoolmember_data.prentice_lastlevel	= prentice_lastlevel;
	createschoolmember_data.prentice_total	= prentice_total;
	rwlock.unlock();

	unsigned int retcode = SessionService::dbConnPool->exeInsert(handle, "`SCHOOLMEMBER`", createschoolmember_define, (const unsigned char *)(&createschoolmember_data));
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("插入SCHOOLMEMBER数据库出错 %u, %s", dwCharID, name);
		return false;
	}

	id = retcode;
	return true;
}

bool CSchoolMember::updateRecord(bool locked)
{
	//FunctionTimes times(5,__FUNCTION__);
	static const dbCol updatechoolmember_define[] = {
		{ "`MASTERID`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`PRESERIALID`",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`CHARID`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`NAME`",			zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ "`LEVEL`",		zDBConnPool::DB_WORD,	sizeof(WORD) },
		{ "`JOINTIME`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ "`DEGREE`",		zDBConnPool::DB_WORD,	sizeof(WORD) },
		{ "LASTTIME",		zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ "`SCHOOLID`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ "`TAG`",			zDBConnPool::DB_BYTE,	sizeof(BYTE) },
		{ "`OCCUPATION`",	zDBConnPool::DB_WORD,	sizeof(WORD) },
		{ "MASTERTOTAL",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "MASTERBALANCE",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "PRENTICELASTLVL",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "PRENTICETOTAL",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ NULL, 0, 0}
	};
	struct {
		DWORD dwMasterID;
		DWORD dwPreSerialID;
		DWORD dwCharID;
		char  name[MAX_NAMESIZE+1];
		WORD  wdLevel;
		DWORD dwJoinTime;
		WORD  wdDegree;
		DWORD dwLastTime;
		DWORD dwSchoolID;
		BYTE  byTag;
		WORD  wdOccupation;
		DWORD master_total;
		DWORD master_balance;
		DWORD prentice_lastlevel;
		DWORD prentice_total;
	} __attribute__ ((packed))
	updateschoolmember_data;
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	bzero(&updateschoolmember_data,sizeof(updateschoolmember_data));

	if (!locked) rwlock.rdlock();	

	updateschoolmember_data.dwMasterID		= dwMasterID;
	updateschoolmember_data.dwPreSerialID	= dwPreSerialID;
	updateschoolmember_data.dwCharID		= dwCharID;
	strncpy(updateschoolmember_data.name, name, MAX_NAMESIZE);
	updateschoolmember_data.wdLevel			= wdLevel;
	updateschoolmember_data.dwJoinTime		= dwJoinTime;
	updateschoolmember_data.wdDegree		= wdDegree;
	updateschoolmember_data.dwLastTime		= dwLastTime;
	updateschoolmember_data.dwSchoolID		= dwSchoolID;
	updateschoolmember_data.byTag			= byTag;
	updateschoolmember_data.wdOccupation	= wdOccupation;
	
	updateschoolmember_data.master_total	= master_total;
	updateschoolmember_data.master_balance	= master_balance;
	updateschoolmember_data.prentice_lastlevel	= prentice_lastlevel;
	updateschoolmember_data.prentice_total	= prentice_total;

	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "`SERIALID` = %u", id);

	if (!locked) rwlock.unlock();

	unsigned int retcode = SessionService::dbConnPool->exeUpdate(handle, "`SCHOOLMEMBER`", updatechoolmember_define, (BYTE *)(&updateschoolmember_data), where);
	SessionService::dbConnPool->putHandle(handle);

	if (retcode > 1)
	{
		Zebra::logger->error("修改门派成员档案失败：serialid = %u, retcode = %u", id, retcode);
		return false;
	}
	return true;
}

bool CSchoolMember::deleteRecord()
{
	//FunctionTimes times(6,__FUNCTION__);
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "SERIALID = %u ", id);
	unsigned int retcode = SessionService::dbConnPool->exeDelete(handle, "`SCHOOLMEMBER`", where);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("删除门派SCHOOLMEMBER记录失败 SERIALID=%u", id);
		return false;
	}
	return true;
}

void CSchoolMember::initByDBRecord(const stSchoolMemberInfo &info)
{
	id				= info.dwSerialID;
	dwMasterID		= info.dwMasterID;
	dwPreSerialID	= info.dwPreSerialID;
	dwCharID		= info.dwCharID;
	strncpy(name, info.name, MAX_NAMESIZE);
	wdLevel			= info.wdLevel;
	dwJoinTime		= info.dwJoinTime;
	wdDegree		= info.wdDegree;
	dwLastTime		= info.dwLastTime;
	dwSchoolID		= info.dwSchoolID;
	byTag			= info.byTag;
	wdOccupation	= info.wdOccupation;
}

bool CSchoolMember::havePreNode()
{
	return (dwPreSerialID !=0 );
}

bool CSchoolMember::haveTeacher()
{
	return (dwMasterID != 0);
}

bool CSchoolMember::addNextLevelNode(CSchoolMember *member)
{
	std::pair<std::map<DWORD, CSchoolMember*, ltword>::iterator, bool> retval;
	retval = prenticeList.insert(prenticeListValueType(member->getJoinTime(), member));

	if (retval.second == false)
	{
		Zebra::logger->error("[师徒]: %s 添加 %s 徒弟节点失败(%u)", this->name, member->name, member->getJoinTime());
	}
	
	return retval.second;
}

void CSchoolMember::setPreLevelNode(CSchoolMember *pPreNode)
{
	preLevelNode = pPreNode;
}

bool CSchoolMember::fireAllPrentice()
{
	bool ret = false;
	std::map<DWORD, CSchoolMember*,ltword> tmp_bak = prenticeList;
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;

	for (tIterator = tmp_bak.begin(); tIterator != tmp_bak.end(); tIterator++)
	{
		if(school->processLeaveSchool(tIterator->second, false))
		{
		}
	}
	return ret;
}

bool CSchoolMember::firePrentice(const char *prenticeName)
{
	bool ret = false;

	if (dwSchoolID == 0)
	{
		CSchoolMember * member = school->getMember(prenticeName);
		if (member)
		{
			UserSession * pUser = member->getUser();
			if (member->getTeacher() == this) // 如果此人的师傅是本节点
			{
				if(school->processLeaveSchool(member, false))
				{
					if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你被你的师傅%s逐出师门!",name);
					Cmd::stChannelChatUserCmd send;
					send.dwType = Cmd::CHAT_TYPE_SYSTEM;
					send.dwChannelID = 0;
					strncpy(send.pstrName,name,MAX_NAMESIZE);
					snprintf(send.pstrChat, sizeof(send.pstrChat) - 1, "%s被逐出了师门", prenticeName);
					this->sendCmdToTeacherGroup(&send,sizeof(send));
					//school->sendCmdToSchool(&send, sizeof(send));
					Zebra::logger->trace("[师徒]%s被%s逐出师门,剩余徒弟数Count=[%u] Size=[%u]", prenticeName,name,
								this->getPrenticeCount(), prenticeList.size());
				}
			}
			else
			{
				if (user) user->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是他师傅不能开除他！");
			}

			ret = true;
		}
	}
	else
	{
		CSchoolMember *prentice = school->getMember(prenticeName);
		if (prentice)
		{
			UserSession * pUser = prentice->getUser();
			if (prentice->getTeacher() == this) // 如果此人的师傅是本节点
			{
				if (school->processLeaveSchool(prentice))
				{
					if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你被你的师傅%s逐出门派!",name);
				}
				ret = true;
			}
			else if (school->getMasterNode() == this) // 如果本节点是师尊
			{
				if (school->processLeaveSchool(prentice))
				{
					if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你被你的师尊%s逐出门派!",name);
				}
				ret = true;
			}
		}
	}

	if (ret) updateRecord();
	return ret;
}

bool CSchoolMember::isMe(const char *pName)
{
	return (strncmp(pName, name, MAX_NAMESIZE) == 0);
}

bool CSchoolMember::isOnline()
{
	return (NULL != user);
}

bool CSchoolMember::isValid()
{
	return (byTag == 1);
}

void CSchoolMember::getMyLayer(DWORD &layer)
{
	if (preLevelNode)
	{
		preLevelNode->getMyLayer(layer);
	}
	layer++;
}

void CSchoolMember::notifyTeacherGroup()
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;
	sendInfomationToMe();
	rwlock.rdlock();
	for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		tIterator->second->sendInfomationToMe();
	}
	rwlock.unlock();
}

void CSchoolMember::sendCmdToTeacherGroup(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen, bool exceptMe)
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;

	if (!exceptMe) sendCmdToMe(ptNullCmd, cmdLen);
	rwlock.rdlock();
	for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		tIterator->second->sendCmdToMe(ptNullCmd, cmdLen);
	}
	rwlock.unlock();
}

void CSchoolMember::sendCmdToMe(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	if (1==byTag)
	{
		if (user)
		{
			user->sendCmdToMe(ptNullCmd, cmdLen);
		}
		else
		{
			if (ptNullCmd->byCmd == Cmd::CHAT_USERCMD && ptNullCmd->byParam == ALL_CHAT_USERCMD_PARAMETER)
			{
				Cmd::stChannelChatUserCmd *pCmd = (Cmd::stChannelChatUserCmd *)ptNullCmd;
				if (Cmd::CHAT_TYPE_OVERMAN_AFFICHE == pCmd->dwType)
				{
					COfflineMessage::writeOfflineMessage(pCmd->dwType, this->id, ptNullCmd, cmdLen);
				}
			}
		}
	}
}

#define getMessage(msg,msglen,pat)	\
do	\
{	\
	va_list ap;	\
		bzero(msg, msglen);	\
		va_start(ap, pat);		\
		vsnprintf(msg, msglen - 1, pat, ap);	\
		va_end(ap);	\
}while(false)

void CSchoolMember::sendSysChat(int type, const char *pattern, ...)
{
	if (user && (byTag == 1))
	{
		char buf[MAX_CHATINFO];
		getMessage(buf,MAX_CHATINFO,pattern);
		user->sendSysChat(type, buf);
	}
}

void CSchoolMember::clearTeacherRelation()
{
	rwlock.wrlock();
	preLevelNode = NULL;
	dwMasterID = 0;
	if (dwSchoolID == 0) dwPreSerialID =0;
	rwlock.unlock();
}

bool CSchoolMember::checkSchoolCreateCondition()
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;
	bool ret = true;
	int count = 0;

	rwlock.rdlock();
	for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		count++;
		if (!tIterator->second->checkMeAndPrenticeNumberAndLevel()) ret = false;
	}
	if (count<MAX_PRENTICE) ret = false;
	rwlock.unlock();
	return ret;
}

bool CSchoolMember::checkMeAndPrenticeNumberAndLevel()
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;
	bool ret = true;
	int count = 0;

	rwlock.rdlock();
	if (wdLevel < FIRST_LAYER_PRENTICE_REQUEST_LEVEL) ret = false;
	for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		count++;
		if (tIterator->second->getLevel() < SECOND_LAYER_PRENTICE_REQUEST_LEVEL) ret = false;
	}
	if (count<MAX_PRENTICE) ret = false;
	rwlock.unlock();
	return ret;
}

void CSchoolMember::moveAllToSchool(CSchool *pSchool, CSchoolMemberListCallback *memberSet)
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;

	rwlock.rdlock();
	for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		tIterator->second->moveAllToSchool(pSchool, memberSet);
	}

	pSchool->directAdddMember(this);
	school->directRemoveMember(this);
	school = pSchool;
	dwSchoolID = pSchool->id;
	if (this->user)
	{
		user->schoolid = pSchool->id;
		Cmd::Session::t_sendUserRelationID send;
		send.dwUserID = user->id;
		CSchoolMember *pTeacher = this->getTeacher();
		if (pTeacher)
		{
			send.caption = pTeacher->getCharID();
		}
		else
		{
			send.caption = 0;
		}
		send.dwID = pSchool->id;
		send.type = Cmd::Session::RELATION_TYPE_SCHOOL;
		user->scene->sendCmd(&send, sizeof(Cmd::Session::t_sendUserRelationID));
	}
	updateRecord();
	if (memberSet) memberSet->exec(this);
	rwlock.unlock();
}

void CSchoolMember::notifyAllReSendInitData(CSendSchoolCallback *callback)
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;

	rwlock.rdlock();
	for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		tIterator->second->notifyAllReSendInitData(callback);
	}
	rwlock.unlock();
	sendInfomationToMe(callback);
}

void CSchoolMember::getSchoolTree(CSendSchoolCallback &callback)
{
	DWORD curLevel = 0;
	bool condition;
	BYTE tag;

	do {
		condition = false;
		tag = Cmd::SCHOOL_NEWLAYER;
		schoolTreeCallback(0, curLevel, condition, callback, tag);
		curLevel++;
	}while(condition);
}

void CSchoolMember::schoolTreeCallback(DWORD level, DWORD tgLevel, bool &condition, CSendSchoolCallback &callback, BYTE &tag)
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;

	if (level >= tgLevel)
	{
		if (!prenticeList.empty()) condition = true;
		callback.exec(this, tag);
	}
	else
	{
		level++;
		for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
		{
			tIterator->second->schoolTreeCallback(level, tgLevel, condition, callback, tag);
			if (level == tgLevel) tag = Cmd::SCHOOL_NONE;
		}
		if (!prenticeList.empty()&&(level == tgLevel))tag = Cmd::SCHOOL_NEWLINE;
	}
}

void CSchoolMember::sendBulletinToMe()
{
	if (dwSchoolID <0) return;
	if (school)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		const char *point;

		point = school->getBulletin();

		bzero(buf, zSocket::MAX_DATASIZE);
		Cmd::stSchoolBulletinCmd *retCmd=(Cmd::stSchoolBulletinCmd *)buf;
		constructInPlace(retCmd);
		retCmd->byState = Cmd::SCHOOL_BULLETIN_GET;
		if (point)
		{
			retCmd->wdSize = strlen(point);
			strncpy(retCmd->data, point, retCmd->wdSize);
		}
		else
		{
			retCmd->wdSize = 0;
		}
		if (user) user->sendCmdToMe(retCmd, sizeof(retCmd));
	}
}

void CSchoolMember::setBulletin(const char *buf)
{
	if (dwSchoolID <0) return;
	if (school)
	{
		if (school->getMasterNode() == this)
		{
			school->setBulletin(buf);
		}
		else
		{
			if (user) user->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是师尊不能书写公告!");
		}
	}
}

CSchoolMember * CSchoolMember::getPreLevelNode()
{
	return preLevelNode;
}

CSchoolMember * CSchoolMember::getTeacher()
{
	if (preLevelNode)
	{
		if (preLevelNode->getCharID() == dwMasterID)
		{
			return preLevelNode;
		}
	}
	return NULL;
}

void CSchoolMember::setInValid()
{
	if (dwSchoolID > 0)
	{
		std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;

		CSchoolM::getMe().removeMemberIndex(name);

		rwlock.wrlock();
		dwMasterID =0;
		dwCharID = 0;
		user = NULL;
		byTag = 0;
		//		dwJoinTime = (DWORD)-1;  在新的人员被招收进来之前不改变其位置。
		bzero(name, MAX_NAMESIZE);
		wdLevel = 0;
		wdDegree = 0;
		dwLastTime = 0;
		wdOccupation = 0;
		for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
		{
			if (tIterator->second->isValid())
			{
				tIterator->second->clearMaster();
				tIterator->second->updateRecord();
			}
		}
		rwlock.unlock();
	}
}

void CSchoolMember::clearMaster()
{
	dwMasterID = 0;
}

void CSchoolMember::sendDestroyNotifyToMe()
{
	Cmd::stMemberStatusSchool ret;
	ret.byStatus = Cmd::SCHOOL_STATUS_NONE;
	if (user) user->sendCmdToMe(&ret, sizeof(ret));
}

void CSchoolMember::directRemovePrentice(const CSchoolMember *member)
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;

	rwlock.wrlock();
	for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		if (tIterator->second == member)
		{
			prenticeList.erase(tIterator);
			break;
		}
	}
	rwlock.unlock();
}

void CSchoolMember::notifySchoolMemberRemove()
{
	Cmd::stFireMemberFromSchoolCmd ret;
	strncpy(ret.memberName, name, MAX_NAMESIZE);
	school->sendCmdToSchool(&ret, sizeof(ret));
}

CSchoolMember* CSchoolMember::getFirstInValideNode()
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;
	CSchoolMember *member = NULL;

	rwlock.rdlock();
	for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		if (!tIterator->second->isValid())
		{
			member = tIterator->second;
			break;
		}
	}
	rwlock.unlock();
	return member;
}

void CSchoolMember::setUser(UserSession *pUser)
{
	user = pUser;
}

void CSchoolMember::clearPreLevelNode()
{
	rwlock.wrlock();
	dwPreSerialID = 0;
	preLevelNode = NULL;
	rwlock.unlock();
}

void CSchoolMember::clearAllNextLevelRelation()
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;

	rwlock.rdlock();
	for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
	{
		tIterator->second->clearPreLevelNode();
	}
	prenticeList.clear();
	rwlock.unlock();
}

void CSchoolMember::setFriendDegree(const Cmd::Session::t_CountFriendDegree_SceneSession *rev)
{
	std::map<DWORD, CSchoolMember*,ltword>::iterator tIterator;
	zRTime ctv;

	rwlock.wrlock();
	for (int i=0; i<rev->size; i++)
	{
		if (rev->namelist[i].byType == Cmd::RELATION_TYPE_TEACHER)
		{
			if (preLevelNode)
			{
				if (preLevelNode->getCharID() == rev->namelist[i].dwUserID)
				{
					if (wdDegree < rev->namelist[i].wdDegree)
					{
						wdDegree = rev->namelist[i].wdDegree;
						dwLastTime = ctv.sec();
						updateRecord(true);
					}
				}
			}
			for (tIterator = prenticeList.begin(); tIterator != prenticeList.end(); tIterator++)
			{
				if (tIterator->second->getCharID() == rev->namelist[i].dwUserID)
				{
					if (tIterator->second->getDegree() < rev->namelist[i].wdDegree)
					{
						tIterator->second->setDegree(rev->namelist[i].wdDegree);
						tIterator->second->setLastTime(ctv.sec());
						tIterator->second->updateRecord(true);
					}
				}
			}
		}
	}
	rwlock.unlock();
}

void CSchoolMember::setDegree(const WORD &degree)
{
	wdDegree = degree;
}

void CSchoolMember::setLastTime(const DWORD &lasttime)
{
	dwLastTime =lasttime;
}

void CSchoolMember::sendChatMessages(const Cmd::stChannelChatUserCmd *rev, const unsigned int cmdLen)
{
	if (dwSchoolID == 0)
	{
		CSchoolMember *myTeacher = getTeacher();
		if (rev->dwType == Cmd::CHAT_TYPE_OVERMAN_AFFICHE)
		{
			sendCmdToTeacherGroup(rev, cmdLen);
		}
		else
		{
			if (myTeacher)
			{
				//myTeacher->sendCmdToTeacherGroup(rev, cmdLen);  新版本聊天消息不发给自己的师兄弟只发给自己的师傅自己的徒弟
				//sendCmdToTeacherGroup(rev, cmdLen,true);
				myTeacher->sendCmdToMe(rev, cmdLen);
				sendCmdToTeacherGroup(rev, cmdLen);
			}
			else
			{
				sendCmdToTeacherGroup(rev, cmdLen);
			}
		}
	}
	else
	{
		if (school) school->sendCmdToSchool(rev, cmdLen);
	}
}

CSchoolMember::~CSchoolMember()
{
	if (!destroy) updateRecord();
}

UserSession *CSchoolMember::getUser()
{
	return user;
}

void CSchoolMember::setLevel(const WORD level)
{
	this->wdLevel = level;
	updateRecord();
}

WORD CSchoolMember::getOccupation()
{
	return wdOccupation;
}

void CSchoolMember::sendNotifyToScene()
{
	if (user)
	{
		Cmd::Session::t_sendUserRelationID send;
		send.dwUserID = user->id;
		CSchoolMember * pTeacher = this->getTeacher();
		if (pTeacher)
			send.caption = pTeacher->getCharID();
		else
			send.caption = 0;
		send.dwID = 0;
		send.type = Cmd::Session::RELATION_TYPE_SCHOOL;
		if(user->scene) user->scene->sendCmd(&send, sizeof(Cmd::Session::t_sendUserRelationID));
	}
}

DWORD CSchoolMember::queryBounty()
{
	double ret = 0;
	if (getUser() && getUser()->level>=10)
	{
		if (this->prentice_lastlevel==0)
		{
			ret = (0.1*(getUser()->level*getUser()->level - 19*getUser()->level + 90)) * 100;
			if (ret < 0) ret = 0;
		}
		else
		{
			ret = (0.1 * (getUser()->level*getUser()->level - 19*getUser()->level - this->prentice_lastlevel*this->prentice_lastlevel + 19*this->prentice_lastlevel)) * 100;
			
		}
	}
	
	return 0;
}

bool CSchoolMember::putBounty()
{
	if (getUser()!=NULL && getTeacher()!=NULL)
	{
		DWORD bounty = this->queryBounty();

		getTeacher()->master_balance += bounty;
		getTeacher()->updateRecord();
		
		this->prentice_lastlevel = getUser()->level;
		this->prentice_total += bounty;
		
		this->updateRecord();
		return true;
	}

	return false;
}
