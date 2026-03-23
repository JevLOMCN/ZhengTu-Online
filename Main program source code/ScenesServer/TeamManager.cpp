
#include "SceneUser.h"
#include "Scene.h"
#include "TeamManager.h"
#include "Chat.h"
#include "SessionClient.h"

/**
 * \brief  遍历组队将指定新增队员user的数据发送给队伍所有成员的客户端
 */
struct SendTeamDataExec : public TeamMemExec
{

	/// 队长
	SceneUser *leader;

	/// 增加组队成员消息定义
	Cmd::stFreshTeamMemberUserCmd ret;

	/**
	 * \brief  构造函数初始化初始化将要发送的消息
	 * \param  l 队长的指针
	 * \param  user 新增队员的对象指针
	 * \return 
	 */
	SendTeamDataExec(SceneUser * l , SceneUser *user)
	{
		leader = l; 
		ret.dwTempID = user->tempid;
		ret.dwMaxHealth = user->charstate.maxhp;
		ret.dwHealth = user->charbase.hp;
		ret.dwMaxMp = user->charstate.maxmp;
		ret.dwMp = user->charbase.mp;
	}

	/**
	 * \brief  回调方法
	 * \param  member 遍历成员
	 * \return false 处理终止 true 处理继续
	 */
	bool exec(TeamMember &member)
	{
		SceneUser *pUser = SceneUserManager::getMe().
						getUserByTempID(member.tempid);
		if(pUser)
		{
			if(ret.dwTempID != pUser->tempid)
				pUser->sendCmdToMe(&ret , sizeof(ret));
			return true;
		}
		return false;
	}
};

/**
 * \brief  发送组队指定成员数据
 * \param  leader 队长
 * \param  user 队员
 */
void TeamManager::sendtoTeamCharData(SceneUser *leader , SceneUser *user)
{
		if(leader)
		{
			SendTeamDataExec exec(leader , user);
			team.execEvery(exec);
		}
}

/**
 * \brief  发送组队指定成员数据
 * \param  user 需要发送的成员对象
 */
void TeamManager::sendtoTeamCharData(SceneUser *user)
{
	if(user->team.IsTeamed())
	{
		SceneUser *leader = SceneUserManager::getMe().getUserByTempID(user->team.getLeader());
		if(leader)
		{
			leader->team.sendtoTeamCharData(leader , user);
		}
	}
}

/**
 * \brief  遍历结构，用来给组队中所有队员广播消息
 */
struct SendCmdExec : public TeamMemExec
{
	/// 需要广播的消息
	void *cmd;

	/// 消息长度
	DWORD cmdLen;

	/**
	 * \brief  构造初始化属性
	 * \param  data 消息体
	 * \param  dataLen 消息长度
	 */
	SendCmdExec(void *data , DWORD dataLen)
	{
		cmd = data;
		cmdLen = dataLen;
	}

	/**
	 * \brief  回调方法
	 * \param  member 成员
	 * \return false 终止遍历 true 继续遍历
	 */
	bool exec(TeamMember &member)
	{
		SceneUser *pUser = SceneUserManager::getMe().
						getUserByTempID(member.tempid);
		if(pUser)
		{
			pUser->sendCmdToMe(cmd , cmdLen);
			return true;
		}
		return true;
	}
};

/**
 * \brief  发送消息给组队
 * \param  user   消息发送者
 * \param  cmd    消息体
 * \param  cmdLen 消息长度
 */
void TeamManager::sendCmdToTeam(SceneUser *user , void *cmd , DWORD cmdLen)
{
	if(user->team.IsTeamed())
	{
		SceneUser *leader = SceneUserManager::getMe().getUserByTempID(user->team.getLeader());
		if(leader)
		{
			SendCmdExec exec(cmd , cmdLen);
			leader->team.execEveryOne(exec);
		}
	}
}

/**
 * \brief  用来检查所有队员是否都在一屏内
 */
struct CheckAllInOneScreenExec : public TeamMemExec
{
	bool isOk;
	SceneUser *leader;

	/**
	 * \brief  构造初始化属性
	 * \param  data 消息体
	 * \param  dataLen 消息长度
	 */
	CheckAllInOneScreenExec(SceneUser *pUser)
	{
		isOk = true;
		leader = pUser;
	}

	/**
	 * \brief  回调方法
	 * \param  member 成员
	 * \return false 终止遍历 true 继续遍历
	 */
	bool exec(TeamMember &member)
	{
		SceneUser *pUser = SceneUserManager::getMe().
						getUserByTempID(member.tempid);
		if(pUser)
		{
			if (!(pUser->scene == leader->scene && pUser->scene->zPosShortRange(pUser->getPos() , leader->getPos() , 11, 11)))
			{
				isOk = false;
			}
			return true;
		}
		else
		{
			isOk = false;
		}
		return true;
	}
};


bool TeamManager::canPutSkill()
{
	if(me && me->team.IsTeamed())
	{
		SceneUser *leader = SceneUserManager::getMe().getUserByTempID(me->team.getLeader());
		if(leader)
		{
			CheckAllInOneScreenExec exec(leader);
			leader->team.execEveryOne(exec);
			if (!exec.isOk)
			{
				if (!giveupstatus)
				{
					giveupstatus = true;
					giveuptime.now();
					giveuptime.addDelay(120000);
					return true;
				}
				else
				{
					if (SceneTimeTick::currentTime.msecs() > giveuptime.msecs())
					{
						giveupstatus = false;///将状态初始化回去。
						return false;
					}
					else
						return true;
				}
			}
			else
			{
				giveupstatus = false;
				return true;
			}
		}
	}
	return false;
}

/**
 * \brief   增加一个新的队员处理
 */
struct AddNewMemberExec : public TeamMemExec
{
	/// 队长
	SceneUser *leader;

	/// 新成员
	SceneUser *nm;

	/// 队员新增消息实例1
	Cmd::stAddTeamMemberUserCmd ret_1;

	/// 队员新增消息实例2
	Cmd::stAddTeamMemberUserCmd ret_2;

	/**
	 * \brief  构造初始化消息实例
	 * \param  u 队长角色对象
	 * \param n 新增队员角色对象
	 */
	AddNewMemberExec(SceneUser *u , SceneUser *n)
	{
		leader = u;
		nm = n;
		ret_1.dwTeamID = u->team.getLeader();
		ret_2.dwTeamID = u->team.getLeader();
		ret_2.data.dwTempID = nm->tempid;
		ret_2.data.dwMaxHealth = nm->charstate.maxhp;
		ret_2.data.dwHealth = nm->charbase.hp;
		ret_2.data.dwMaxMp = nm->charstate.maxmp;
		ret_2.data.dwMp = nm->charbase.mp;
		ret_2.data.wdFace = nm->charbase.face;
		strncpy(ret_2.data.pstrName , nm->name , MAX_NAMESIZE);
		ret_2.data.byHead = false;
	}

	/**
	 * \brief  回调方法将新成员发送给每个队员，将每个队员发送给新成员
	 * \param  member 队员
	 * \return false 终止遍历 true 继续遍历
	 */
	bool exec(TeamMember &member)
	{
		SceneUser *pUser = SceneUserManager::getMe().
						getUserByTempID(member.tempid);
		if(pUser)
		{
			ret_1.dwTeamID = pUser->team.getLeader();
			strncpy(ret_1.data.pstrName , pUser->name , MAX_NAMESIZE);
			if(leader->tempid == pUser->tempid)
			{
				ret_1.data.byHead = true;
			}
			else
			{
				ret_1.data.byHead = false;
			}
			ret_1.data.dwTempID = pUser->tempid;
			ret_1.data.dwMaxHealth = pUser->charstate.maxhp;
			ret_1.data.dwHealth = pUser->charbase.hp;
			ret_1.data.dwMaxMp = pUser->charstate.maxmp;
			ret_1.data.dwMp = pUser->charbase.mp;
			ret_1.data.wdFace = pUser->charbase.face;
			strncpy(ret_1.data.pstrName , pUser->name , MAX_NAMESIZE);
			pUser->sendCmdToMe(&ret_2 , sizeof(ret_2));
			Channel::sendSys(pUser , Cmd::INFO_TYPE_GAME, "%s加入队伍" , ret_2.data.pstrName);
			nm->sendCmdToMe(&ret_1 , sizeof(ret_1));
			//Zebra::logger->debug("队伍(%ld)发出添加队员指令(%s , %ld)" , ret_2.dwTeamID , nm->name , nm->id);
			return true;
		}
		return true;
	}
};

/**
 * \brief  判断角色是否是自己的队友
 * \param  pUser 被判断对象
 * \return true 是 false 否
 */
bool TeamManager::IsOurTeam(SceneUser *pUser)
{
	if(!team.leader)
	{
		return false;
	}
	else
	{
		return team.leader == pUser->team.getLeader();
	}
}

/**
 * \brief  判断角色是否是自己的队友
 * \param  dwID 被判断对象的id
 * \return true 是 false 否
 */
bool TeamManager::IsOurTeam(DWORD dwID)
{
	SceneUser *pUser = SceneUserManager::getMe().
		getUserByID(dwID);
	if(pUser)
	{
		return IsOurTeam(pUser);
	}
	return false;
}

/**
 * \brief  创建一个新的队伍，产生一个队长
 * \param  pUser 准队长
 * \return true 成功 false 失败
 */
bool TeamManager::addNewLeader(SceneUser *pUser)
{
	if(team.getSize() == 0)
	{
		/*		
				if(addNewMember(pUser , pUser))
				{
				Zebra::logger->debug("产生队伍%ld,组长(%s , %ld)" , pUser->tempid , pUser->name , pUser->id);
				return true;
				}
		// */
		if(addMemberByTempID(pUser , pUser->tempid))
		{
			pUser->team.setLeader(pUser->tempid);
			Cmd::stAddTeamMemberUserCmd ret;
			ret.data.dwTempID = pUser->tempid;
			ret.data.dwMaxHealth = pUser->charstate.maxhp;
			ret.data.dwHealth = pUser->charbase.hp;
			ret.data.dwMaxMp = pUser->charstate.maxmp;
			ret.data.dwMp = pUser->charbase.mp;
			ret.data.wdFace = pUser->charbase.face;
			strncpy(ret.data.pstrName , pUser->name , MAX_NAMESIZE);
			ret.data.byHead = true;
			setLeader(pUser->tempid);
			pUser->sendCmdToMe(&ret , sizeof(ret));
			if(pUser->team_mode == Cmd::TEAM_HONOR)
			{
				Channel::sendSys(pUser , Cmd::INFO_TYPE_GAME, "%s以荣誉模式组建队伍" , ret.data.pstrName);
			}
			else
			{
				Channel::sendSys(pUser , Cmd::INFO_TYPE_GAME, "%s以普通模式组建队伍" , ret.data.pstrName);
			}
			//Zebra::logger->trace("产生队伍%ld,组长(%s , %ld)" , pUser->tempid , pUser->name , pUser->id);

			pUser->reSendMyMapData();
			//session队伍
			addMemberToSession(pUser->id , pUser->id);
			return true;
		}
	}
	return false;
}

/**
 * \brief  增加一个新的队员
 * \param  pUser 准队员
 * \param  rev 组队邀请应答消息
 * \return true 加入成功 false 加入失败
 */
bool TeamManager::addNewMember(SceneUser *pUser , Cmd::stAnswerTeamUserCmd *rev)
{
		SceneUser *nm = SceneUserManager::getMe().
						getUserByTempID(rev->dwAnswerUserID);
	if(nm)
	{
		if (nm->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
				||  nm->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
		{
			return false;
		}
		nm->team.setLeader(pUser->tempid);
		if(addMemberByTempID(pUser , rev->dwAnswerUserID))
		{
			Cmd::stTeamModeUserCmd tmu; 
			tmu.byType=pUser->team_mode;
			nm->sendCmdToMe(&tmu , sizeof(tmu));
			nm->team.setObjMode(pUser->team.getObjMode());
			Cmd::stObjModeTeamMemberUserCmd objmode; 
			objmode.byType=pUser->team.getObjMode();
			nm->sendCmdToMe(&objmode , sizeof(objmode));
			nm->team.setExpMode(pUser->team.getExpMode());
			Cmd::stExpModeTeamMemberUserCmd expmode; 
			expmode.byType=pUser->team.getExpMode();
			nm->sendCmdToMe(&expmode , sizeof(expmode));
			AddNewMemberExec add(pUser , nm);
			team.execEvery(add);
			//FunctionTimes times(103,__FUNCTION__);
			pUser->reSendMyMapData();

			addMemberToSession(pUser->id , nm->id);
			return true;

		}
	}
	return false;
}

/**
 * \brief  增加一个新的队员
 * \param  leader 队长
 * \param  pUser 准队员
 * \return true 增加成功  false 增加失败
 */
bool TeamManager::addNewMember(SceneUser *leader , SceneUser *pUser)
{
	if (pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_TIGER)
			||  pUser->issetUState(Cmd::USTATE_TOGETHER_WITH_DRAGON))
	{
		return false;
	}

	pUser->team.setLeader(leader->tempid);
	///Zebra::logger->debug("队员(%u)加入队伍:%u",pUser->tempid , leader->tempid);
	AddNewMemberExec add(leader , pUser);
	if(addMemberByTempID(leader , pUser->tempid))
	{
		Cmd::stTeamModeUserCmd tmu; 
		tmu.byType=leader->team_mode;
		pUser->sendCmdToMe(&tmu , sizeof(tmu));
		pUser->team.setObjMode(leader->team.getObjMode());
		Cmd::stObjModeTeamMemberUserCmd objmode; 
		objmode.byType=leader->team.getObjMode();
		pUser->sendCmdToMe(&objmode , sizeof(objmode));
		pUser->team.setExpMode(leader->team.getExpMode());
		Cmd::stExpModeTeamMemberUserCmd expmode; 
		expmode.byType=leader->team.getExpMode();
		pUser->sendCmdToMe(&expmode , sizeof(expmode));
		team.execEvery(add);
		//FunctionTimes times(102,__FUNCTION__);
		pUser->reSendMyMapData();

		addMemberToSession(leader->id , pUser->id);
		return true;

	}

	return false;
}

/**
 * \brief 获取队伍人数
 */
int TeamManager::getSize()
{
	if(me && me->team.IsTeamed())
	{
		if (me->tempid == me->team.getLeader())
		{
			return team.getSize();
		}
		else
		{
			SceneUser *leader = SceneUserManager::getMe().getUserByTempID(me->team.getLeader());
			if(leader)
			{
				return leader->team.getSize();
			}
		}
	}
	return 0;
}

/**
 * \brief  删除组队
 * \param  pUser 队长
 * \param  tempid 队长的临时id
 */
void TeamManager::removeTeam(SceneUser *pUser , DWORD tempid)
{
	if(pUser->team.getLeader() == tempid)
	{
		//changeLeader();
		pUser->team.setLeader(0);
		removeMemberByTempID(tempid);
	}
}

/**
 * \brief  队伍解散遍历处理每个队员的退出事宜
 */
struct DeleteTeamExec : public TeamMemExec
{
	/// 队长
	SceneUser *leader;

	/// 删除组队消息
	Cmd::stRemoveTeamUserCmd ret;

	/**
	 * \brief  构造初始化
	 * \param  u 队长
	 */
	DeleteTeamExec(SceneUser *u)
	{
		leader = u;
	}

	/**
	 * \brief  回调方法，发送队伍解散消息给每个成员
	 * \param  member 队员
	 * \return true 继续遍历 false 终止遍历
	 */
	bool exec(TeamMember &member)
	{
		SceneUser *pUser = SceneUserManager::getMe().
						getUserByTempID(member.tempid);
		if(pUser)
		{
			//FunctionTimes times(100,"队伍解散");
			pUser->team.setLeader(0);
			pUser->sendCmdToMe(&ret , sizeof(ret));
			pUser->reSendMyMapData();
			Channel::sendSys(pUser , Cmd::INFO_TYPE_GAME, "队伍解散");
		}
		return true;
	}

};

/**
 * \brief  删除组队
 * \param  leader 队长
 */
void TeamManager::deleteTeam(SceneUser *leader)
{
	//清除队伍标志
	setLeader(0);
	DeleteTeamExec del(leader);
	team.execEvery(del);
	team.Clear();
	// Session队伍
	delMemberToSession(leader->id ,leader->id);
}

/**
 * \brief  删除成员遍历通知所有队伍成员
 */
struct RemoveMemberExec : public TeamMemExec
{
	/// 队长
	SceneUser *leader;

	/// 删除成员消息
	Cmd::stRemoveTeamMemberUserCmd ret;


	/**
	 * \brief  构造函数初始化成员删除消息
	 * \param  u 队长
	 * \param  rem  队员删除通知消息
	 */
	RemoveMemberExec(SceneUser *u , const Cmd::stRemoveTeamMemberUserCmd *rem)
	{
		leader = u;
		ret.dwTeamID = rem->dwTeamID;
		//strncpy(ret.pstrName , rem->pstrName , MAX_NAMESIZE);
		ret.dwTempID = rem->dwTempID;
	}

	/**
	 * \brief  回调方法通知所有队员有成员离开
	 * \param  member 队员
	 * \return true 继续遍历 false 终止遍历
	 */
	bool exec(TeamMember &member)
	{
		SceneUser *pUser = SceneUserManager::getMe().
			getUserByTempID(member.tempid);
		if(pUser)
		{
			pUser->sendCmdToMe(&ret , sizeof(ret));
			//Channel::sendSys(pUser , Cmd::INFO_TYPE_GAME, "%s离开队伍" , ret.pstrName);
		}
		return true;
	}

};

/**
 * \brief  在队伍中踢人
 * \param  pUser 队长
 * \param rev 队员删除消息
 */
void TeamManager::kickoutMember(SceneUser *pUser , Cmd::stRemoveTeamMemberUserCmd *rev)
{
	RemoveMemberExec rem(pUser , rev);
	team.execEvery(rem);
	//DWORD tempid = team.removeMemberByName(rev->pstrName);
	team.removeMemberByTempID(rev->dwTempID);
	if(rev->dwTempID)
	{
		SceneUser *u = SceneUserManager::getMe().
						getUserByTempID(rev->dwTempID);
		if(u)
		{
			u->team.setLeader(0);
			u->reSendMyMapData();
			// Session队伍
			delMemberToSession(pUser->id ,u->id);
		}
	}
}

/**
 * \brief  响应消息删除队伍成员
 * \param  pUser 被删除的成员
 * \param  rev 删除队伍成员消息
 */
void TeamManager::removeMember(SceneUser *pUser , Cmd::stRemoveTeamMemberUserCmd *rev)
{
	RemoveMemberExec rem(pUser , rev);
	team.execEvery(rem);
	//DWORD tempid = team.removeMemberByName(rev->pstrName);
	team.removeMemberByTempID(rev->dwTempID);
	if(rev->dwTempID)
	{
		SceneUser *u = SceneUserManager::getMe().
						getUserByTempID(rev->dwTempID);
		if(u)
		{
			//FunctionTimes times(101,__FUNCTION__);
			u->team.setLeader(0);
			u->reSendMyMapData();
			// Session队伍
			delMemberToSession(pUser->id ,u->id);
		}
	}
}

/**
 * \brief  删除队伍成员
 * \param  pUser 队长
 * \param  mem 被删除成员
 */
void TeamManager::removeMember(SceneUser *pUser , SceneUser *mem)
{
	//TODO 删除队友
	Cmd::stRemoveTeamMemberUserCmd ret;
	ret.dwTeamID = pUser->tempid;
	//strncpy(ret.pstrName , mem->name , MAX_NAMESIZE);
	ret.dwTempID=mem->tempid;
	removeMember(pUser , &ret);
	// Session队伍
	delMemberToSession(pUser->id ,mem->id);
}				

/**
 * \brief  遍历
 * \param  callback 回调方法
 */
void TeamManager::execEveryOne(TeamMemExec &callback)
{
	team.execEvery(callback);
}

/**
 * \brief  遍历除了特定成员以外的所有队员
 * \param  callback 回调方法
 * \param  tempid 被排除队员的 id
 */
void TeamManager::execEveryOneExceptMe(TeamMemExec &callback , DWORD tempid)
{
	team.execEveryExceptMe(callback , tempid);
}

/**
 * \brief  根据id增加一个新的成员
 * \param  pUser 角色指针，无特定要求
 * \param  id 新增成员的 id
 * \return true 增加成功 false 增加失败
 */
bool TeamManager::addMemberByID(SceneUser *pUser , DWORD id)
{
	//在地层做判断
	/*
	if(IsFull())
	{
		//TODO 提示满
		return false;
	}
	// */
	if(pUser)
	{
		SceneUser *u = SceneUserManager::getMe().getUserByID(id);
		if(u)
		{
			if(team.addMember(id , u->tempid , u->name))
			{
				return true;
			}
		}
	}
	return false;
}

/**
 * \brief  遍历检查离线成员状态，未离线成员计算离线时间，超过规定时间的离线成员从队伍中删除
 */
struct CheckOfflineExec : public TeamMemExec
{
	/// 队长
	SceneUser *leader;

	/// 类型定义
	typedef std::vector<Cmd::stRemoveTeamMemberUserCmd> Remove_vec;

	/// 类型定义
	typedef Remove_vec::iterator Remove_vec_iterator;

	/// 清除管理器
	Remove_vec del_vec;

	/// 类型定义
	typedef std::vector<DWORD> Online_vec;

	/// 定义迭代指针
	typedef Online_vec::iterator Online_vec_iterator;

	/// 重新上线管理器
	Online_vec add_vec;

	/**
	 * \brief  构造函数初始化属性
	 * \param  u 队长
	 */
	CheckOfflineExec(SceneUser *u)
	{
		leader = u;
	}

	/**
	 * \brief  遍历处理离线成员
	 * \param  member 成员
	 * \return true 继续遍历 false 终止遍历
	 */
	bool exec(TeamMember &member)
	{
		SceneUser *pUser = SceneUserManager::getMe().
			getUserByID(member.id);
		if(pUser)
		{
			if(!pUser->team.IsTeamed())
			{
				add_vec.push_back(pUser->id);
			}
			/*
			//刚刚上线
			if(member.offtime != 0)
			{
			if(!pUser->team.IsTeamed())
			{
			add_vec.push_back(pUser->id);
			}
			//leader->team.addNewMember(leader , pUser);
			}
			// */
		}
		else
		{
			member.offtime += 10;
			if(member.offtime >= 120)
			{
				//TODO 删除队友
				Cmd::stRemoveTeamMemberUserCmd ret;
				ret.dwTeamID = leader->tempid;
				//strncpy(ret.pstrName , member.name , MAX_NAMESIZE);
				ret.dwTempID=member.tempid;
				del_vec.push_back(ret);
			}
		}
		return true;
	}
};

/**
 * \brief  检查离线成员状态，删除超时的，恢复上线的
 * \param  pUser 队长
 */
void TeamManager::checkOffline(SceneUser *pUser)
{
	CheckOfflineExec exec(pUser);
	team.execEvery(exec);
	if(!exec.del_vec.empty())
	{
		for(CheckOfflineExec::Remove_vec_iterator del_iter=exec.del_vec.begin();del_iter != exec.del_vec.end();del_iter ++)
		{
			pUser->team.removeMember(pUser , &*del_iter);
		}
		exec.del_vec.clear();
		if(pUser->team.getSize() == 1)
		{
			pUser->team.deleteTeam(pUser);
		}
	}
	if(!exec.add_vec.empty())
	{
		for(CheckOfflineExec::Online_vec_iterator add_iter=exec.add_vec.begin();add_iter !=exec.add_vec.end();add_iter ++)
		{
			SceneUser *tuser = SceneUserManager::getMe().
				getUserByID(*add_iter);
			if(tuser)
			{
				pUser->team.addNewMember(pUser , tuser);
			}
		}
		exec.add_vec.clear();
	}
}

/**
 * \brief  遍历将队伍中所有的成员转移到新队长名下
 */
struct ChangeLeaderExec : public TeamMemExec
{
	/// 现任队长
	SceneUser *leader;

	/// 新队长
	SceneUser *newleader;

	/**
	 * \brief  构造初始化属性
	 * \param  u 现在任队长
	 * \param  n 未来队长
	 */
	ChangeLeaderExec(SceneUser *u , SceneUser *n)
	{
		leader = u;
		newleader = n;
	}

	/**
	 * \brief  遍历将所有队员转移到新队长名下
	 * \param  member 成员
	 * \return  true 继续遍历 false 终止遍历
	 */
	bool exec(TeamMember &member)
	{
		if(member.id != newleader->tempid)
		{
			SceneUser *user = SceneUserManager::getMe().
				getUserByTempID(member.tempid);
			if(user)
			{
				newleader->team.addNewMember(newleader , user);
			}
		}
		return true;
	}
};

/**
 * \brief  遍历队伍查找一个现任队长以外的成员使其成为新队长
 */
struct FindLeaderExec : public TeamMemExec
{
	/// 现任队长
	SceneUser *leader;

	/// 新队长
	SceneUser *newleader;

	/**
	 * \brief  构造函数初始化变量
	 * \param  u 当前队长
	 */
	FindLeaderExec(SceneUser *u)
	{
		leader = u;
		newleader = NULL;
	}

	/**
	 * \brief  遍历查找新的队长
	 * \param  member 成员
	 * \return true 表示继续遍历 false 终止遍历
	 */
	bool exec(TeamMember &member)
	{
		SceneUser *pUser = SceneUserManager::getMe().
			getUserByTempID(member.tempid);
		if(pUser && pUser->tempid != leader->tempid)
		{
			newleader = pUser;
			return true;
		}
		else
		{
			return false;
		}
		return true;
	}
};

/**
 * \brief  查找一个新的队长
 * \param  pUser 当前队长
 * \return 新的队长对象角色指针
 */
SceneUser *TeamManager::findNewLeader(SceneUser *pUser)
{
	FindLeaderExec exec(pUser);
	team.execEvery(exec);
	return exec.newleader;
}

/**
 * \brief  改变队长
 * \param  pUser 当前队长
 * \return true 改变成功  false 改变失败
 */
bool TeamManager::changeLeader(SceneUser *pUser)
{
	SceneUser *newleader = findNewLeader(pUser);
	if(newleader)
	{
		if(newleader->team.addNewLeader(newleader))
		{
			ChangeLeaderExec exec(pUser , newleader);
			team.execEvery(exec);
		}
		return true;
	}
	return false;
}

/**
 * \brief  根据临时id增加队伍成员
 * \param  pUser 未使用
 * \param tempid 新增加成员的临时id
 * \return true 增加成功 false 增加失败
 */
bool TeamManager::addMemberByTempID(SceneUser *pUser , DWORD tempid)
{
	//在地层做判断
	/*
	if(IsFull())
	{
		//TODO 提示满
		return false;
	}
	// */
	if(pUser)
	{
		SceneUser *u = SceneUserManager::getMe().
						getUserByTempID(tempid);
		if(u )
		{
			if(team.addMember(u->id , tempid , u->name))
			{
				return true;
			}
		}
	}
	return false;
}

/**
 * \brief  根据id 删除成员
 * \param  id 角色的id
 */
void TeamManager::removeMemberByID(DWORD id)
{
	team.removeMemberByID(id);
}

/**
 * \brief 根据临时id删除成员 
 * \param  tempid 临时id
 */
void TeamManager::removeMemberByTempID(DWORD tempid)
{
	team.removeMemberByTempID(tempid);
}

/**
 * \brief 请求友好度关系
 */
void TeamManager::requestFriendDegree()
{
	team.requestFriendDegree();
}

/**
 * \brief  发送当前友好度关系列表到会话存档
 */
void TeamManager::sendFriendDegreeToSession()
{
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::Session::stCountMember * temp = NULL;

	Cmd::Session::t_CountFriendDegree_SceneSession *retCmd=(Cmd::Session::t_CountFriendDegree_SceneSession *)buf;
	strncpy(retCmd->name, me->name, MAX_NAMESIZE);
	constructInPlace(retCmd);
	retCmd->size = 0;
	temp = retCmd->namelist;

	if (!friendList.empty())
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		for(sIterator = friendList.begin(); sIterator!= friendList.end(); sIterator++)
		{
			temp->dwUserID = sIterator->second.dwUserID;
			temp->wdDegree = sIterator->second.wdDegree;
			temp->byType   = Cmd::RELATION_TYPE_FRIEND;
			retCmd->size++;
			temp++;
		}
	}

	if (!consortList.empty())
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		for(sIterator = consortList.begin(); sIterator!= consortList.end(); sIterator++)
		{
			temp->dwUserID = sIterator->second.dwUserID;
			temp->wdDegree = sIterator->second.wdDegree;
			temp->byType   = Cmd::RELATION_TYPE_LOVE;
			retCmd->size++;
			temp++;
		}
	}

	if (!teacherList.empty())
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		for(sIterator = teacherList.begin(); sIterator!= teacherList.end(); sIterator++)
		{
			temp->dwUserID = sIterator->second.dwUserID;
			temp->wdDegree = sIterator->second.wdDegree;
			temp->byType   = Cmd::RELATION_TYPE_TEACHER;
			retCmd->size++;
			temp++;
		}
	}

	if (!prenticeList.empty())
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		for(sIterator = prenticeList.begin(); sIterator!= prenticeList.end(); sIterator++)
		{
			temp->dwUserID = sIterator->second.dwUserID;
			temp->wdDegree = sIterator->second.wdDegree;
			temp->byType   = Cmd::RELATION_TYPE_TEACHER;
			retCmd->size++;
			temp++;
		}
	}
	if (0 != retCmd->size) sessionClient->sendCmd(retCmd, sizeof(Cmd::Session::t_CountFriendDegree_SceneSession)+retCmd->size*sizeof(Cmd::Session::stCountMember));
}

/**
 * \brief  设置友好度流程把当前的友好度值替换到发过来的关系清单中，因为当前值是最新的，
		   然后清空之前的关系列表，再把收到的关系清单插入到关系列表中
 * \param  rev 友好度关系列表消息
 */
void TeamManager::setFriendDegree(Cmd::Session::t_ReturnFriendDegree_SceneSession *rev)
{
	relationlock.wrlock();

	if (NULL == rev)
	{
		friendList.clear();   // 朋友列表
		consortList.clear();  // 夫妻列表
		teacherList.clear();  // 师傅列表
		prenticeList.clear(); // 徒弟列表

	}
	else if (0 == rev->size)
	{
		friendList.clear();   // 朋友列表
		consortList.clear();  // 夫妻列表
		teacherList.clear();  // 师傅列表
		prenticeList.clear(); // 徒弟列表
	}
	else
	{
		std::list<stTempDegreeMember> templist;

		for(int i=0; i<rev->size; i++)
		{
			stTempDegreeMember unit;
			unit.dwUserID = rev->memberlist[i].dwUserID;
			unit.wdDegree = rev->memberlist[i].wdDegree;
			unit.byType   = rev->memberlist[i].byType;
			unit.wdTime   = 0;
			templist.push_back(unit);
		}

		if (!friendList.empty())   // 朋友列表
		{
			std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
			for(sIterator = friendList.begin(); sIterator!= friendList.end(); sIterator++)
			{
				std::list<stTempDegreeMember>::iterator tIterator;
				for(tIterator = templist.begin(); tIterator!=templist.end(); tIterator++)
				{
					if ((sIterator->second.dwUserID == tIterator->dwUserID) &&
						(Cmd::Session::TYPE_FRIEND == tIterator->byType))
					{
						tIterator->wdDegree = sIterator->second.wdDegree;
						tIterator->wdTime		= sIterator->second.wdTime;
					}
				}
			}
		}
		if (!consortList.empty())  // 夫妻列表
		{
			std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
			for(sIterator = consortList.begin(); sIterator!= consortList.end(); sIterator++)
			{
				std::list<stTempDegreeMember>::iterator tIterator;
				for(tIterator = templist.begin(); tIterator!=templist.end(); tIterator++)
				{
					if ((sIterator->second.dwUserID == tIterator->dwUserID) &&
						(Cmd::Session::TYPE_CONSORT == tIterator->byType))
					{
						tIterator->wdDegree = sIterator->second.wdDegree;
						tIterator->wdTime		= sIterator->second.wdTime;
					}
				}
			}
		}
		if (!teacherList.empty())  // 师傅列表
		{
			std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
			for(sIterator = teacherList.begin(); sIterator!= teacherList.end(); sIterator++)
			{
				std::list<stTempDegreeMember>::iterator tIterator;
				for(tIterator = templist.begin(); tIterator!=templist.end(); tIterator++)
				{
					if ((sIterator->second.dwUserID == tIterator->dwUserID) &&
						(Cmd::Session::TYPE_TEACHER == tIterator->byType))
					{
						tIterator->wdDegree = sIterator->second.wdDegree;
						tIterator->wdTime		= sIterator->second.wdTime;
					}
				}
			}
		}
		if (!prenticeList.empty()) // 徒弟列表
		{
			std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
			for(sIterator = prenticeList.begin(); sIterator!= prenticeList.end(); sIterator++)
			{
				std::list<stTempDegreeMember>::iterator tIterator;
				for(tIterator = templist.begin(); tIterator!=templist.end(); tIterator++)
				{
					if ((sIterator->second.dwUserID == tIterator->dwUserID) &&
						(Cmd::Session::TYPE_PRENTICE == tIterator->byType))
					{
						tIterator->wdDegree = sIterator->second.wdDegree;
						tIterator->wdTime		= sIterator->second.wdTime;
					}
				}
			}
		}

		friendList.clear();   // 朋友列表
		consortList.clear();  // 夫妻列表
		teacherList.clear();  // 师傅列表
		prenticeList.clear(); // 徒弟列表

		std::list<stTempDegreeMember>::iterator tIterator;
		for(tIterator = templist.begin(); tIterator!=templist.end(); tIterator++)
		{
			stDegreeMember member;
	
			member.dwUserID = tIterator->dwUserID;
			member.wdDegree = tIterator->wdDegree;
			member.wdTime   = tIterator->wdTime;
			switch(tIterator->byType)
			{
				case Cmd::Session::TYPE_FRIEND:
					{
						friendList.insert(insValueType(member.wdDegree, member));
					}
					break;
				case Cmd::Session::TYPE_CONSORT:
					{
						consortList.insert(insValueType(member.wdDegree, member));
					}
					break;
				case Cmd::Session::TYPE_TEACHER:
					{
						teacherList.insert(insValueType(member.wdDegree, member));
					}
					break;
				case Cmd::Session::TYPE_PRENTICE:
					{
						prenticeList.insert(insValueType(member.wdDegree, member));
					}
					break;
				default:
					break;
			}
		}
	}
	relationlock.unlock();
}

/**
 * \brief  额外金钱奖励
 * \param  wdMoney 奖励参照额度
 */
void TeamManager::putMoneyPlus(WORD wdMoney)
{
	if (0 == team.leader) return;

	relationlock.rdlock();
	if (!teacherList.empty())
	{
		if (me && (me->charbase.level <40))
		{
			SceneUser * pMaster = NULL;
			pMaster = SceneUserManager::getMe().getUserByID(teacherList.begin()->second.dwUserID);
			if (pMaster)
			{
				pMaster->packs.addMoney((WORD)(wdMoney*(FRIENDDEGREE_MONEYPLUS_RATE/100.0f)), "队伍额外奖励","获得额外金钱奖励");
			}
		}
	}
	relationlock.unlock();
}


/**
 * \brief  获取友好度相关附加经验值
 * \param  wdExp 奖励参照额度
 */
DWORD TeamManager::getExpPlus(WORD wdExp)
{
	DWORD Exp = 0;
	if (0 == team.leader) return 0;

	relationlock.rdlock();
	if (!teacherList.empty())
	{
		if (me && (me->charbase.level <40))
		{
			SceneUser * pMaster = NULL, *pLeader = NULL;
			pMaster = SceneUserManager::getMe().getUserByID(teacherList.begin()->second.dwUserID);
			pLeader = SceneUserManager::getMe().getUserByTempID(team.leader);
			if (pMaster && pLeader)
			{
				// TODO 把下面的计算公式改成一个宏
				Exp = (DWORD)((wdExp * (((pMaster->charbase.level / 30.0f)+1.0f) / pLeader->team.getAverageExp()) + 0.9)*(FRIENDDEGREE_EXPPLUS_RATE/100.0f));
				if (0 == Exp) Exp = 1;
			}
		}
	}
	relationlock.unlock();
	return Exp;
}

/**
 * \brief  获取友好度攻击附加值
 * \return 附加值
 */
DWORD TeamManager::getAttPlus()
{
	DWORD teacherAttPlus = 0;
	DWORD prenticeID = 0;
	DWORD friendAttPlus = 0;
	DWORD friendID = 0;

	if (0 == team.leader) return 0;

	relationlock.rdlock();

	if (!prenticeList.empty())
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		for(sIterator = prenticeList.begin(); sIterator!= prenticeList.end(); sIterator++)
		{
			SceneUser * otherUser = NULL;
			otherUser = SceneUserManager::getMe().getUserByID(sIterator->second.dwUserID);
			if (otherUser)
			{
				if (otherUser->scene->zPosShortRange(me->getPos(), otherUser->getPos(),FRIENDDEGREE_VALIDATION_DISTANCE_WIDE, FRIENDDEGREE_VALIDATION_DISTANCE_HEIGHT))
				{
					if ((sIterator->second.wdDegree>= FRIENDDEGREE_RANGE_BTM) &&(sIterator->second.wdDegree<FRIENDDEGREE_RANGE_MED))
					{
						if (zMisc::selectByPercent(FRIENDDEGREE_TEACHER_RATE_BTM))
						{
							teacherAttPlus =  FRIENDDEGREE_TEACHER_ATTPLUS_RATE_BTM;
							prenticeID = sIterator->second.dwUserID;
						}
						break;
					}
					else if ((sIterator->second.wdDegree>= FRIENDDEGREE_RANGE_MED) &&(sIterator->second.wdDegree<FRIENDDEGREE_RANGE_TOP))
					{
						if (zMisc::selectByPercent(FRIENDDEGREE_TEACHER_RATE_MED))
						{
							teacherAttPlus =  FRIENDDEGREE_TEACHER_ATTPLUS_RATE_MED;
							prenticeID = sIterator->second.dwUserID;
						}
						break;
					}
					else if (sIterator->second.wdDegree >= FRIENDDEGREE_RANGE_TOP)
					{
						if (zMisc::selectByPercent(FRIENDDEGREE_TEACHER_RATE_TOP))
						{
							teacherAttPlus =  FRIENDDEGREE_TEACHER_ATTPLUS_RATE_TOP;
							prenticeID = sIterator->second.dwUserID;
						}
						break;
					}
				}
			}
		}
	}

	if (!friendList.empty())
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		for(sIterator = friendList.begin(); sIterator!= friendList.end(); sIterator++)
		{
			SceneUser * otherUser = NULL;
			otherUser = SceneUserManager::getMe().getUserByID(sIterator->second.dwUserID);
			if (otherUser)
			{
				if (otherUser->scene->zPosShortRange(me->getPos(), otherUser->getPos(),FRIENDDEGREE_VALIDATION_DISTANCE_WIDE,FRIENDDEGREE_VALIDATION_DISTANCE_HEIGHT))
				{
					if ((sIterator->second.wdDegree>= FRIENDDEGREE_RANGE_BTM) &&(sIterator->second.wdDegree<FRIENDDEGREE_RANGE_MED))
					{
						if (zMisc::selectByPercent(FRIENDDEGREE_FRIEND_RATE_BTM))
						{
							friendAttPlus =  FRIENDDEGREE_FRIEND_ATTPLUS_RATE_BTM;
							friendID = sIterator->second.dwUserID;
						}
						break;
					}
					else if ((sIterator->second.wdDegree>= FRIENDDEGREE_RANGE_MED) &&(sIterator->second.wdDegree<FRIENDDEGREE_RANGE_TOP))
					{
						if (zMisc::selectByPercent(FRIENDDEGREE_FRIEND_RATE_MED))
						{
							friendAttPlus =  FRIENDDEGREE_FRIEND_ATTPLUS_RATE_MED;
							friendID = sIterator->second.dwUserID;
						}
						break;
					}
					else if (sIterator->second.wdDegree >= FRIENDDEGREE_RANGE_TOP)
					{
						if (zMisc::selectByPercent(FRIENDDEGREE_FRIEND_RATE_TOP))
						{
							friendAttPlus =  FRIENDDEGREE_FRIEND_ATTPLUS_RATE_TOP;
							friendID = sIterator->second.dwUserID;
						}
						break;
					}
				}
			}
		}
	}

	relationlock.unlock();

	if ((0 == teacherAttPlus) && (0 == friendAttPlus))
	{
		return 0;
	}
	else if ((0 != teacherAttPlus) && (0 == friendAttPlus))
	{
		return teacherAttPlus;
	}
	else if ((0 == teacherAttPlus) && (0 != friendAttPlus))
	{
		return friendAttPlus;
	}
	else if ((0 != teacherAttPlus) && (0 != friendAttPlus))
	{
		if (prenticeID == friendID)
		{
			if (teacherAttPlus > friendAttPlus)
			{
				return teacherAttPlus;
			}
			else
			{
				return friendAttPlus;
			}
		}
		else
		{
			return friendAttPlus+teacherAttPlus;
		}
	}
	return 0;
}

/**
 * \brief  获取友好度防御附加值
 * \return 附加值
 */
DWORD TeamManager::getDefPlus()
{
	DWORD prenticeAttPlus = 0;
	DWORD teacherID = 0;
	DWORD consortAttPlus = 0;
	DWORD consortID = 0;

	if (0 == team.leader) return 0;

	relationlock.rdlock();

	if (!teacherList.empty())
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		sIterator = teacherList.begin();
		SceneUser * otherUser = NULL;
		otherUser = SceneUserManager::getMe().getUserByID(sIterator->second.dwUserID);
		if (otherUser)
		{
			if (otherUser->scene->zPosShortRange(me->getPos(), otherUser->getPos(),FRIENDDEGREE_VALIDATION_DISTANCE_WIDE, FRIENDDEGREE_VALIDATION_DISTANCE_HEIGHT))
			{
				if ((sIterator->second.wdDegree>= FRIENDDEGREE_RANGE_BTM) &&(sIterator->second.wdDegree<FRIENDDEGREE_RANGE_MED))
				{
					if (zMisc::selectByPercent(FRIENDDEGREE_TEACHER_RATE_BTM))
					{
						prenticeAttPlus =  FRIENDDEGREE_PRENTICE_DEFPLUS_RATE_BTM;
						teacherID = sIterator->second.dwUserID;
					}
				}
				else if ((sIterator->second.wdDegree>= FRIENDDEGREE_RANGE_MED) &&(sIterator->second.wdDegree<FRIENDDEGREE_RANGE_TOP))
				{
					if (zMisc::selectByPercent(FRIENDDEGREE_TEACHER_RATE_MED))
					{
						prenticeAttPlus =  FRIENDDEGREE_PRENTICE_DEFPLUS_RATE_MED;
						teacherID = sIterator->second.dwUserID;
					}
				}
				else if (sIterator->second.wdDegree >= FRIENDDEGREE_RANGE_TOP)
				{
					if (zMisc::selectByPercent(FRIENDDEGREE_TEACHER_RATE_TOP))
					{
						prenticeAttPlus =  FRIENDDEGREE_PRENTICE_DEFPLUS_RATE_TOP;
						teacherID = sIterator->second.dwUserID;
					}
				}
			}
		}
	}

	if (!consortList.empty())
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		sIterator = consortList.begin();

		SceneUser * otherUser = NULL;
		otherUser = SceneUserManager::getMe().getUserByID(sIterator->second.dwUserID);
		if (otherUser)
		{
			if (me->scene->zPosShortRange(me->getPos(), otherUser->getPos(),FRIENDDEGREE_VALIDATION_DISTANCE_WIDE, FRIENDDEGREE_VALIDATION_DISTANCE_HEIGHT))
			{
				if ((sIterator->second.wdDegree>= FRIENDDEGREE_RANGE_BTM) &&(sIterator->second.wdDegree<FRIENDDEGREE_RANGE_MED))
				{
					if (zMisc::selectByPercent(FRIENDDEGREE_CONSORT_RATE_BTM))
					{
						consortAttPlus =  FRIENDDEGREE_CONSORT_DEFPLUS_RATE_BTM;
						consortID = sIterator->second.dwUserID;
					}
				}
				else if ((sIterator->second.wdDegree>= FRIENDDEGREE_RANGE_MED) &&(sIterator->second.wdDegree<FRIENDDEGREE_RANGE_TOP))
				{
					if (zMisc::selectByPercent(FRIENDDEGREE_CONSORT_RATE_MED))
					{
						consortAttPlus =  FRIENDDEGREE_CONSORT_DEFPLUS_RATE_MED;
						consortID = sIterator->second.dwUserID;
					}
				}
				else if (sIterator->second.wdDegree >= FRIENDDEGREE_RANGE_TOP)
				{
					if (zMisc::selectByPercent(FRIENDDEGREE_CONSORT_RATE_TOP))
					{
						consortAttPlus =  FRIENDDEGREE_CONSORT_DEFPLUS_RATE_TOP;
						consortID = sIterator->second.dwUserID;
					}
				}
			}
		}
	}

	relationlock.unlock();

	if ((0 == prenticeAttPlus) && (0 == consortAttPlus))
	{
		return 0;
	}
	else if ((0 != prenticeAttPlus) && (0 == consortAttPlus))
	{
		return prenticeAttPlus;
	}
	else if ((0 == prenticeAttPlus) && (0 != consortAttPlus))
	{
		return consortAttPlus;
	}
	else if ((0 != prenticeAttPlus) && (0 != consortAttPlus))
	{
		if (teacherID == consortID)
		{
			if (prenticeAttPlus > consortAttPlus)
			{
				return prenticeAttPlus;
			}
			else
			{
				return consortAttPlus;
			}
		}
		else
		{
			return consortAttPlus+prenticeAttPlus;
		}
	}
	return 0;
}

/**
 * \brief  实时累加计算所有关系的友好度关系
 */
void TeamManager::countFriendDegree()
{
	if (0 == team.leader) return;

	bool oper = false;

	relationlock.rdlock();
	if (!friendList.empty())   // 朋友列表
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		for(sIterator = friendList.begin(); sIterator!= friendList.end(); sIterator++)
		{
			sIterator->second.wdTime++;
			if (60 <= sIterator->second.wdTime)
			{
				sIterator->second.wdTime=0;
				sIterator->second.wdDegree+=60;
				oper = true;
			}
		}
	}
	if (!consortList.empty())  // 夫妻列表
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		for(sIterator = consortList.begin(); sIterator!= consortList.end(); sIterator++)
		{
			sIterator->second.wdTime++;
			if (60 <= sIterator->second.wdTime)
			{
				sIterator->second.wdTime=0;
				sIterator->second.wdDegree+=60;
				oper = true;
			}
		}
	}
	if (!teacherList.empty())  // 师傅列表
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		for(sIterator = teacherList.begin(); sIterator!= teacherList.end(); sIterator++)
		{
			sIterator->second.wdTime++;
			if (60 <= sIterator->second.wdTime)
			{
				sIterator->second.wdTime=0;
				sIterator->second.wdDegree+=60;
				oper = true;
			}
		}
	}
	if (!prenticeList.empty()) // 徒弟列表
	{
		std::map<WORD, struct stDegreeMember,ltword>::iterator sIterator;
		for(sIterator = prenticeList.begin(); sIterator!= prenticeList.end(); sIterator++)
		{
			sIterator->second.wdTime++;
			if (60 <= sIterator->second.wdTime)
			{
				sIterator->second.wdTime=0;
				sIterator->second.wdDegree+=60;
				oper = true;
			}
		}
	}
	if (oper) sendFriendDegreeToSession();
	relationlock.unlock();
}

/**
 * \brief  根据临时存档数据加载队伍
 * \param  leader 队长
 * \param  data 临时存档对象
 * \return true 加载成功  false 加载失败
 */
bool TeamManager::loadTeam(SceneUser *leader , TempArchiveMember *data)
{
	//本地图不能组队
	if(leader->scene->noTeam())
	{
		return false;
	}
	if(!leader->team.addNewLeader(leader))
	{
		return false;
	}
	int size = data->size - sizeof(DWORD) - sizeof(DWORD) - sizeof(BYTE) - sizeof(BYTE);
	leader->team_mode = *(DWORD*)&data->data[size];
	Cmd::stTeamModeUserCmd tmu; 
	tmu.byType=leader->team_mode;
	leader->sendCmdToMe(&tmu , sizeof(tmu));
	leader->team.setNextObjOwnerID(*(DWORD*)&data->data[size+sizeof(DWORD)]);
	leader->team.setObjMode(*(BYTE*)&data->data[size+sizeof(DWORD)+sizeof(DWORD)]);
	leader->team.setExpMode(*(BYTE*)&data->data[size+sizeof(DWORD)+sizeof(DWORD)+sizeof(BYTE)]);
	struct PairIDAndName
	{
		DWORD id;
		char name[MAX_NAMESIZE];
	};
	PairIDAndName *pair = NULL;
	while((int)size > 0)
	{
		pair = (PairIDAndName*)&data->data[data->size -sizeof(DWORD) - sizeof(DWORD) - sizeof(BYTE) - sizeof(BYTE) - size]; 
		size -= sizeof(PairIDAndName);
		SceneUser *u = SceneUserManager::getMe().
			getUserByID(pair->id);
		if(u)
		{
			if(!u->team.IsTeamed())
			{
				//Zebra::logger->debug("队伍%u恢复成员找到成员,id=%u , tempid=%u , name=%s" , leader->tempid , u->id , u->tempid , u->name);
				if(leader->team.IsFull())
				{
					break;
				}
				addNewMember(leader , u);
			}
			//team.addMember(u->id , u->tempid , u->name);
		}
		else if(pair->id)
		{
			team.addMember(pair->id , 0 , pair->name);
		}
		//Zebra::logger->debug("恢复队伍成员,id=%u , name=%s" , pair->id , pair->name);
		//size -= (int)((size - sizeof(DWORD)) > 0) ? size - sizeof(DWORD) : 0;
	}
	if(leader->team.getSize() == 1)
	{
		leader->team.deleteTeam(leader);
	}
	//Zebra::logger->debug("load队伍成员数量=%u" , leader->team.getSize());
	return true;
}

/**
 * \brief  增加一个成员到会话
 * \param  leaderid 队长 id
 * \param  userid 增加的成员的 id
 * \return true 为消息发送成功 false 消息发送失败
 */
bool TeamManager::addMemberToSession(DWORD leaderid , DWORD userid)
{
	Cmd::Session::t_Team_AddMember add;
	add.dwLeaderID = leaderid;
	add.dwMemberID = userid;
	sessionClient->sendCmd(&add , sizeof(Cmd::Session::t_Team_AddMember));
	return true;
}

/**
 * \brief  向会话发送成员删除消息
 * \param  leaderid 队长的id
 * \param  userid 被删除角色的id
 * \return true 为消息发送成功 false 消息发送失败
 */
bool TeamManager::delMemberToSession(DWORD leaderid , DWORD userid)
{
	Cmd::Session::t_Team_DelMember del;
	del.dwLeaderID = leaderid;
	del.dwMemberID = userid;
	sessionClient->sendCmd(&del , sizeof(Cmd::Session::t_Team_DelMember));
	return true;
}
void TeamManager::decreaseAverageExp(SceneUser *leader , SceneUser *pUser)
{
	team.decreaseAverageExp(pUser);
}
void TeamManager::decreaseAverageExp(DWORD leaderid , SceneUser *pUser)
{
	SceneUser *leader = SceneUserManager::getMe().
		getUserByID(leaderid);
	if(leader)
	{
		leader->team.decreaseAverageExp(leader , pUser);
	}
}
struct ExpSizeExec : public TeamMemExec
{
	int size;
	zPosI pos;
	DWORD sceneid;
	ExpSizeExec(zPosI p,DWORD s)
	{
		pos = p;
		size = 0;
		sceneid = s;
	}
	bool exec(TeamMember &member)
	{
		SceneUser *pUser = SceneUserManager::getMe().
						getUserByTempID(member.tempid);
		if(pUser)
		{
			if(pUser->scene->id == sceneid && pUser->scene->checkTwoPosIInNine(pUser->getPosI(),pos))
			{
				//Zebra::logger->debug("检查9屏成员坐标成功(%d,%d)(%d,%d)",pos,pUser->getPosI(),pUser->getPos().x,pUser->getPos().y);
				size++;
			}
			else
			{
				//Zebra::logger->debug("检查9屏成员坐标失败(%d,%d)(%d,%d)",pos,pUser->getPosI(),pUser->getPos().x,pUser->getPos().y);
			}
		}
		return true;
	}
};
int TeamManager::getExpSize(zPosI pos , DWORD sceneid)
{
	ExpSizeExec exec(pos,sceneid);
	team.execEvery(exec);
	return exec.size;
}
void TeamManager::calAverageExp(zPosI pos,DWORD sceneid)
{
	team.calAverageExp(pos,sceneid);
}
