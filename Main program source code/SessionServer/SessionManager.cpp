/**
 * \file
 * \version  $Id: SessionManager.cpp  $
 * \author  
 * \date 
 * \brief 实现会话管理器
 *
 * 
 */

#include "SessionManager.h"
#include "Zebra.h"
#include "CUnion.h"
#include "CSept.h"
#include "SchoolManager.h"
#include "CQuiz.h"
#include "SchoolManager.h"
#include "SessionCommand.h"
#include "CCountryManager.h"
#include "CArmy.h"
#include "CCityManager.h"

///用户会话管理器实例
UserSessionManager * UserSessionManager::sm(NULL);
DWORD UserSession::user_count=0;
//国家排序
std::map<DWORD , DWORD> UserSession::country_map;
class CGraceUser
{
	public:
		DWORD dwUserID;
		DWORD dwGrace;
		DWORD dwExploit;
		char  szName[MAX_NAMESIZE];

		CGraceUser()
		{
			dwUserID = 0;
			dwGrace = 0;
			dwExploit = 0;
			bzero(szName, MAX_NAMESIZE);
		}

		CGraceUser(const CGraceUser& ref)
		{
			dwUserID = ref.dwUserID;
			dwGrace = ref.dwGrace;
			dwExploit = ref.dwExploit;
			strncpy(szName, ref.szName, MAX_NAMESIZE);
		}

		~CGraceUser()
		{
		}
		
		CGraceUser& operator =(const CGraceUser& ref)
		{
			this->dwUserID = ref.dwUserID;
			this->dwGrace = ref.dwGrace;
			this->dwExploit = ref.dwExploit;
			strncpy(this->szName, ref.szName, MAX_NAMESIZE);
			return *this;
		}

};

bool lessGraceScore(const CGraceUser& p1, const CGraceUser& p2)
{
	return p1.dwGrace > p2.dwGrace;
}

bool lessExploit(const CGraceUser& p1, const CGraceUser& p2)
{
	return p1.dwExploit > p2.dwExploit;
}

/**
 * \brief 构造函数
 */
UserSessionManager::UserSessionManager():zUserManager()
{
	inited=false;
}

/**
 * \brief 析构函数
 */
UserSessionManager::~UserSessionManager()
{
	final();
}

/**
 * \brief 取得管理器实例
 * 如果还没有初始化就在这里进行
 * \return 管理器实例指针
 */
UserSessionManager *UserSessionManager::getInstance()
{
	if(sm==NULL)
		sm=new UserSessionManager();
	return sm;
}

/**
 * \brief 删除管理器实例
 */
void UserSessionManager::delInstance()
{
	SAFE_DELETE(sm);
}

/**
 * \brief 获取一个唯一ID
 * \return 是否成功
 */
bool UserSessionManager::getUniqeID(DWORD &tempid)
{
	return true;
}

/**
 * \brief 释放一个唯一ID
 * \return 
 */
void UserSessionManager::putUniqeID(const DWORD &tempid)
{
}

/**
 * \brief 初始化用户管理器
 * \return 初始化是否成功
 */
bool UserSessionManager::init()
{
	if(inited)
	{
		Zebra::logger->warn("User Session管理器已经被初始化过...");
		return true;
	}
	inited=true;
	Zebra::logger->info("初始化User Session管理器成功...");
	return inited;
}

/**
 * \brief 结束用户管理器
 */
void UserSessionManager::final()
{
	if(inited)
	{
		inited=false;
	}
}

/**
 * \brief 根据名字得到用户会话的指针
 * \param name 用户名字
 * \return 找到的会话指针，失败返回0
 */
UserSession * UserSessionManager::getUserSessionByName( const char * name)
{
	UserSession *ret =(UserSession *)getUserByName(name);
	/*
	mlock.lock();
	UserSession *ret =(UserSession *)getEntryByName(name);
	mlock.unlock();
	// */
	return ret;
}

/**
 * \brief 根据临时ID得到用户会话的指针
 * \param tempid 用户临时id
 * \return 找到的会话指针，失败返回0
 */
UserSession *UserSessionManager::getUserByTempID(DWORD tempid)
{
	return (UserSession *)zUserManager::getUserByTempID(tempid);
}

/**
 * \brief 根据ID得到用户会话的指针
 * \param id 用户id
 * \return 找到的会话指针，失败返回0
 */
UserSession *UserSessionManager::getUserByID(DWORD id)
{
	return (UserSession *)zUserManager::getUserByID(id);
}

void UserSessionManager::sendGraceSort(UserSession* pUser)
{
	struct sortAllUser: public execEntry<UserSession>
	{
		std::vector<CGraceUser> vGraceList;
		DWORD _dwCountryID;

		sortAllUser(DWORD dwCountryID)
		{
			_dwCountryID = dwCountryID;
		}

		~sortAllUser()
		{
		}

		void sendSort(UserSession* user)
		{
			BYTE buf[zSocket::MAX_DATASIZE];
			Cmd::stWaitOfficialItem* tempPoint;
			Cmd::stRtnWaitOfficialUserCmd* retCmd = (Cmd::stRtnWaitOfficialUserCmd*)buf;
			constructInPlace(retCmd);
			std::sort(vGraceList.begin(), vGraceList.end(), lessGraceScore);
			tempPoint = (Cmd::stWaitOfficialItem *)retCmd->data;

			for (DWORD i=0; i<vGraceList.size(); i++)
			{
				if (i<10)
				{
					tempPoint->dwCharID = vGraceList[i].dwUserID;
					strncpy(tempPoint->szName, vGraceList[i].szName, MAX_NAMESIZE);
					tempPoint++;
					retCmd->dwSize++;	
				}
				else{
					break;
				}
			}
			
			user->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stWaitOfficialItem)+sizeof(Cmd::stRtnWaitOfficialUserCmd)));
		}
		
		bool exec(UserSession* user)
		{
			if (user && user->dwGrace>0 && !CCountryM::getMe().isKing(user)
					&& user->country == _dwCountryID && !CCountryM::getMe().isOfficial(user))
			{
				CGraceUser temp;
				temp.dwUserID = user->id;
				temp.dwGrace = user->dwGrace;
				strncpy(temp.szName, user->name, MAX_NAMESIZE);
				vGraceList.push_back(temp);
			}
			
			return true;
		}
	};

	struct sortAllUser allUser(pUser->country);
	execEveryUser(allUser);
	allUser.sendSort(pUser);
}

void UserSessionManager::sendExploitSort(UserSession* pUser)
{
	struct sortAllUser: public execEntry<UserSession>
	{
		std::vector<CGraceUser> vExploitList;
		DWORD _dwCountryID;

		sortAllUser(DWORD dwCountryID)
		{
			_dwCountryID = dwCountryID;
		}

		~sortAllUser()
		{
		}

		void sendSort(UserSession* user)
		{
			BYTE buf[zSocket::MAX_DATASIZE];
			Cmd::stWaitGenItem* tempPoint;
			Cmd::stRtnWaitGenUserCmd* retCmd = (Cmd::stRtnWaitGenUserCmd*)buf;
			constructInPlace(retCmd);
			
			std::sort(vExploitList.begin(), vExploitList.end(), lessExploit);
			
			tempPoint = (Cmd::stWaitGenItem *)retCmd->data;

			for (DWORD i=0; i<vExploitList.size(); i++)
			{
				if (i<10)
				{
					tempPoint->dwCharID = vExploitList[i].dwUserID;
					strncpy(tempPoint->szName, vExploitList[i].szName, MAX_NAMESIZE);
					tempPoint++;
					retCmd->dwSize++;	
				}
				else{
					break;
				}
			}
		
			if (retCmd->dwSize > 0)
			{
				user->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stWaitGenItem)+
							sizeof(Cmd::stRtnWaitGenUserCmd)));
				
				user->sendSysChat(Cmd::INFO_TYPE_FAIL, "当前没有可任命的待选将军。请稍后再试");
			}
		}
		
		bool exec(UserSession* user)
		{
			if (user && user->dwExploit>0 && !CCountryM::getMe().isKing(user)  
					&& !CCityM::getMe().isCastellan(user)
					&& !CArmyM::getMe().isCaptain(user->id) && user->country == _dwCountryID)
			{
				CGraceUser temp;
				temp.dwUserID = user->id;
				temp.dwGrace = user->dwGrace;
				temp.dwExploit = user->dwExploit;
				strncpy(temp.szName, user->name, MAX_NAMESIZE);
				vExploitList.push_back(temp);
			}
			
			return true;
		}
	};

	struct sortAllUser allUser(pUser->country);
	execEveryUser(allUser);
	allUser.sendSort(pUser);
}

void UserSessionManager::sendCmdByCondition(const Cmd::stNullUserCmd *pstrCmd, const unsigned int nCmdLen, Compare* compare)
{
	struct sendCmdToAllUser	: public execEntry<UserSession>
	{
		const Cmd::stNullUserCmd* cmd;
		unsigned int cmdLen;
		Compare* compare;
		
		sendCmdToAllUser(const Cmd::stNullUserCmd *pstrCmd, const unsigned int nCmdLen, Compare* pfCompare)
		{
			cmd = pstrCmd;
			cmdLen = nCmdLen;
			compare = pfCompare;
		}

		~sendCmdToAllUser()
		{
		}

		bool exec(UserSession* user)
		{
			if (user && (*compare)(user))
				user->sendCmdToMe(cmd, cmdLen);
			return true;
		}
		
	};

	struct sendCmdToAllUser allUser(pstrCmd, nCmdLen, compare);
	execEveryUser(allUser);
}

/**
 * \brief 删除一个task上的所有用户会话
 * 一个task有管理多个连接
 * \param task task指针
 * \return 
 */
void UserSessionManager::removeAllUserByTask(SessionTask *task)
{
	struct offlineAllUserBySessionTask: public execEntry<UserSession>
	{
		SessionTask *task;
		bool exec(UserSession *su)
		{
			if(su->getTask()==task || (su->scene!=NULL && su->scene->getTask()==task))
			{
				CUnionM::getMe().userOffline(su); // 用于处理帮会成员下线
				CSchoolM::getMe().userOffline(su);
				CSeptM::getMe().userOffline(su);
				CQuizM::getMe().userOffline(su);
			}

			return true;
		}
	};
	offlineAllUserBySessionTask offline;
	struct removeAllUserBySessionTask :public removeEntry_Pred<UserSession>
	{
		SessionTask *task;
		bool isIt(UserSession *us)
		{
			if(us->getTask()==task || (us->scene!=NULL && us->scene->getTask()==task))
			{
				return true;
			}
			else
				return false;
		}
	};
	removeAllUserBySessionTask rust;
	if(task==NULL) return;

	if(task->getType()==GATEWAYSERVER || task->getType()==SCENESSERVER)
	{
		Zebra::logger->debug(" 清理此服务器(%ld,%ld)的所有用户信息",task->getID(),task->getType());
		rust.task=task;
		offline.task=task;
	}
	else
	{
		Zebra::logger->debug(" 未知服务器(%ld,%ld)",task->getID(),task->getType());
		rust.task=NULL;
		offline.task=NULL;
	}

	if(offline.task!=NULL)
	{
		execEveryUser(offline);
	}
	if(rust.task!=NULL)
	{
		removeUser_if(rust);
		for(unsigned int i=0,n=rust.removed.size();i<n;i++)
		{
			SAFE_DELETE(rust.removed[i]);
		}
	}
}

///场景会话管理器实例
SceneSessionManager * SceneSessionManager::sm(NULL);

/**
 * \brief 构造函数
 */
SceneSessionManager::SceneSessionManager():zSceneManager()
{
	inited=false;
}

/**
 * \brief 析构函数
 */
SceneSessionManager::~SceneSessionManager()
{
	final();
}

/**
 * \brief 得到场景会话管理器实例
 * \return 场景会话管理器实例指针
 */
SceneSessionManager *SceneSessionManager::getInstance()
{
	if(sm==NULL)
		sm=new SceneSessionManager();
	return sm;
}

/**
 * \brief 删除场景会话管理器实例
 */
void SceneSessionManager::delInstance()
{
	SAFE_DELETE(sm);
}

/**
 * \brief 得到一个唯一id
 * \param tempid 输出，得到的id
 * \return 是否成功
 */
bool SceneSessionManager::getUniqeID(DWORD &tempid)
{
	return true;
}

/**
 * \brief 释放一个唯一的id
 * \param tempid 要释放的id
 */
void SceneSessionManager::putUniqeID(const DWORD &tempid)
{
}

/**
 * \brief 初始化场景会话管理器
 * \return 是否成功
 */
bool SceneSessionManager::init()
{
	if(inited)
	{
		Zebra::logger->warn("Scene Session管理器已经被初始化过...");
		return true;
	}
	inited=true;
	Zebra::logger->info("初始化Scene Session管理器成功...");
	return inited;
}

/**
 * \brief 结束场景管理器
 */
void SceneSessionManager::final()
{
	if(inited)
	{
		inited=false;
	}
}

/**
 * \brief 向管理器中添加一个场景
 * \param scene 要添加的场景
 * \return 是否添加成功
 */
bool SceneSessionManager::addScene(SceneSession *scene)
{
	rwlock.wrlock();
	bool ret= addEntry(scene);
	rwlock.unlock();
	return ret;
}

/**
 * \brief 根据名字得到场景指针
 * \param name 场景名字
 * \return 场景指针，失败返回0
 */
SceneSession * SceneSessionManager::getSceneByName(const char *name)
{
	return (SceneSession *)zSceneManager::getSceneByName(name);
}

/**
 * \brief 根据临时id得到场景指针
 * \param tempid 场景临时id
 * \return 场景指针，失败返回0
 */
SceneSession * SceneSessionManager::getSceneByTempID(DWORD tempid)
{
	return (SceneSession *)zSceneManager::getSceneByTempID(tempid);
}

/**
 * \brief 根据id得到场景指针
 * \param id 场景id
 * \return 场景指针，失败返回0
 */
SceneSession * SceneSessionManager::getSceneByID(DWORD id)
{
	return (SceneSession *)zSceneManager::getSceneByID(id);
}

/**
 * \brief 从管理器移除一个场景
 * \param scene 要移除的场景
 */
void SceneSessionManager::removeScene(SceneSession *scene)
{
	rwlock.wrlock();
	removeEntry(scene);
	rwlock.unlock();
}
/**
 * \brief 场景服务器关闭，需要注销一些地图
 * \param task 该场景服务器连接的SessionTask
 */
void SceneSessionManager::removeAllSceneByTask(SessionTask *task)
{
	struct removeAllSceneByTask :public removeEntry_Pred<SceneSession>
	{
		SessionTask *task;
		bool isIt(SceneSession *ss)
		{
			return (ss->getTask()==task);
		}
	};
	if(task==NULL || task->getType()!=SCENESSERVER) return;
	removeAllSceneByTask rust;
	rust.task=task;
	removeScene_if(rust);
	for(unsigned int i=0,n=rust.removed.size();i<n;i++)
	{
		SAFE_DELETE(rust.removed[i]);
	}
}

/**
 * \brief 根据文件名字得到场景指针
 * \param name 文件名字
 * \return 找到的场景指针，失败返回0
 */
SceneSession * SceneSessionManager::getSceneByFile(const char *name)
{
	struct GetSceneByFileName: public execEntry<SceneSession>
	{
		SceneSession *ret;
		const char *_name;

		GetSceneByFileName(const char* name) : ret(NULL), _name(name)
		{ }
		
		bool exec(SceneSession *scene)
		{
			if(strncmp(scene->file.c_str(), _name,MAX_NAMESIZE)==0)
			{
				ret = scene;
				return false;
			}
			else
				return true;
		}
	};

	GetSceneByFileName gsfn(name);
	execEveryScene(gsfn);
	return gsfn.ret;
}

/**
 * \brief 设置国家的税率
 * \param byTax 税率
 */
void SceneSessionManager::notifyCountryTax(DWORD dwCountry,  BYTE byTax)
{
	struct findall : public execEntry<SceneSession> 
	{
		Cmd::Session::t_taxCountry_SceneSession send;
		findall(DWORD dwCountry, BYTE byTax)
		{
			send.dwCountryID = dwCountry;
			send.byTax = byTax;
		}

		~findall(){}

		bool exec(SceneSession *scene)
		{
			if (scene)
			{
				send.dwTempID = scene->tempid;
				scene->sendCmd(&send, sizeof(send));
			}
			return true;
		}
	};

	findall myList(dwCountry, byTax);
	execEveryScene(myList);
}
