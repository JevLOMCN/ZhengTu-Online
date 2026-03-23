/**
 * \file
 * \version  $Id: CNpcDare.cpp  $
 * \author 
 * \date 
 * \brief 定义NPC争夺战管理器
 *
 */

#include "CNpcDare.h"
#include "zDBConnPool.h"
#include "SessionServer.h"
#include "Command.h"
#include "Session.h"
#include "NpcDare.h"
#include "CSept.h"
#include "TimeTick.h"
#include "CDare.h"
#include "SessionChat.h"
#include <ctime>

using namespace NpcDareDef;

CNpcDareM *CNpcDareM::um(NULL);

/**
 * \brief 构造函数
 */
CNpcDareM::CNpcDareM()
{
	_notDare = true;
	_notifyDareMessage = true;
}

/**
 * \brief NPC争夺战管理器初始化
 * \return true 初始化成功 false初始化失败
 */
bool CNpcDareM::init()
{
	return load();
}

/**
 * \brief 获得管理器的唯一实例
 * \return 管理器的唯一实例
 */
CNpcDareM &CNpcDareM::getMe()
{
	if(um==NULL)
	{
		um=new CNpcDareM();
	}
	return *um;
}

/**
 * \brief 析构管理器
 */
void CNpcDareM::destroyMe()
{
}

/**
 * \brief 从数据库中加载争夺目标记录
 * \return true 加载成功
 */
bool CNpcDareM::load()
{
	const dbCol npcdare_read_define[] = {
		{ "`COUNTRY`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`MAPID`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`NPCID`",		zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`POSX`",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`POSY`",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`HOLDSEPTID`",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`DARESEPTID`",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`GOLD`",			zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ NULL, 0, 0}
	};

	struct NpcDareRecord *recordList,*tempPoint, info;

	recordList = NULL;
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	unsigned int retcode = SessionService::dbConnPool->exeSelect(handle, "`NPCDARE`", npcdare_read_define, NULL, NULL, (BYTE **)&recordList);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode
			|| 0 == retcode)
	{
		return true;
	}
	if (recordList)
	{
		tempPoint = &recordList[0];
		for (unsigned int i=0; i< retcode; i++)
		{
			bcopy(tempPoint, &info, sizeof(info));
			CNpcDareObj *pObject = new CNpcDareObj();
			if (pObject)
			{
				pObject->create(info);
				_objList.push_back(pObject);
			}
#ifdef _DEBUGLOG
			Zebra::logger->debug("dwCountry=%u dwMapID=%u dwNpcID=%u dwHoldSeptID=%u dwDareSeptID=%u dwGold=%u",
		info.dwCountry,
		info.dwMapID,
		info.dwNpcID,
		info.dwHoldSeptID,
		info.dwDareSeptID,
		info.dwGold);
#endif
			tempPoint++;
		}
		SAFE_DELETE_VEC(recordList);
		return true;
	}
	else
	{
		Zebra::logger->error("NPC争夺战目标数据加载失败，exeSelect 返回无效buf指针");
	}
	return false;
}

/**
 * \brief 将管理器中的数据刷新到数据库中去
 * \return true 刷新成功
 */
bool CNpcDareM::refreshDB()
{
	return true;
}

/**
 * \brief 处理开始对战
 */
void CNpcDareM::doDare()
{
	std::vector<CNpcDareObj*>::iterator vIterator;

	for(vIterator = _objList.begin(); vIterator!=_objList.end(); vIterator++)
	{
		(*vIterator)->doDare();
	}
}

/**
 * \brief 处理开始对战前的通知消息
 */
void CNpcDareM::notifyDareReady()
{
	std::vector<CNpcDareObj*>::iterator vIterator;

	for(vIterator = _objList.begin(); vIterator!=_objList.end(); vIterator++)
	{
		(*vIterator)->notifyDareReady();
	}
}

/**
 * \brief 处理对战结果
 */
void CNpcDareM::doResult()
{
	std::vector<CNpcDareObj*>::iterator vIterator;

	for(vIterator = _objList.begin(); vIterator!=_objList.end(); vIterator++)
	{
		(*vIterator)->doResult();
	}
}

/**
* \brief 强制进行对战结果计算
* \author fqnewman
*/
void CNpcDareM::forceProcessResult()
{
	std::vector<CNpcDareObj*>::iterator vIterator;

	for(vIterator = _objList.begin(); vIterator!=_objList.end(); vIterator++)
	{
		(*vIterator)->forceProcessResult();
	}
}


/**
 * \brief 获得管理器的唯一实例
 * \return 管理器的唯一实例
 */
void CNpcDareM::timer()
{
	struct tm tv1;
	time_t timValue = time(NULL);
	zRTime::getLocalTime(tv1, timValue);

#ifdef _DEBUGLOG
	Zebra::logger->debug("时间：%u:%u", tv1.tm_hour, tv1.tm_min);
#endif

	if (_notifyDareMessage && (18 == tv1.tm_hour) && (tv1.tm_min>=55))
	{
		_notifyDareMessage = false;
		notifyDareReady();
	}

	if (_notDare && (19 == tv1.tm_hour) && (tv1.tm_min <6))
	{
		_notDare = false;
		doDare();
	}

	if ((19 == tv1.tm_hour) && (tv1.tm_min >=18))//&&(!_notifyDareMessage)) 
	{
		_notDare= true;
		_notifyDareMessage = true;
		forceProcessResult();
		doResult();
	}
	if (!_notDare)
	{
		doResult();
	}
}

/**
 * \brief 查找NPC信息
 * \param country 国家
 * \param mapid 地图id
 * \param npcid npc编号
 */
CNpcDareObj* CNpcDareM::findObject(DWORD country, DWORD mapid, DWORD npcid)
{
	std::vector<CNpcDareObj*>::iterator vIterator;

	for(vIterator = _objList.begin(); vIterator!=_objList.end(); vIterator++)
	{
		if ((*vIterator)->isMe(country, mapid, npcid))
		{
			return (*vIterator);
		}
	}
	return NULL;
}

/**
 * \brief 处理对战的请求
 * \param rev 对战请求消息
 */
void CNpcDareM::processRequest(Cmd::Session::t_NpcDare_Dare_SceneSession * rev)
{
	CNpcDareObj * npcObj = findObject(rev->dwCountryID, rev->dwMapID, rev->dwNpcID);
	if (npcObj)
	{
		npcObj->dareRequest(rev->dwUserID);
	}
	else
	{
		UserSession *pUser = UserSessionManager::getInstance()->getUserByID(rev->dwUserID);
		if (pUser)
		{
//			CNpcDareObj::itemBack(pUser);
			pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "商人的思绪混乱中，无法回忆相关情况");
		}
	}

}

/**
 * \brief 处理获取保护费
 * \param pUser 收钱的角色
 * \param rev 收前的消息
 */
void CNpcDareM::processGetGold(Cmd::Session::t_NpcDare_GetGold_SceneSession *rev)
{
	DWORD septid=0;

	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(rev->dwUserID);
	if (!pUser) return;
	if ((septid=CSeptM::getMe().findUserSept(rev->dwUserID)) == 0)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "无法收取税金你不是族长！");
	}
	else
	{
		CNpcDareObj * Obj = getNpcDareObjBySept(septid);
		if (Obj)
		{
			Obj->processGetGold(pUser, septid, rev->dwNpcID, rev->dwMapID, rev->dwCountryID);
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
bool CNpcDareM::processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch(ptNullCmd->byParam)
	{
		case Cmd::NPCDARE_GETGOLD_PARA:
			{
				//Cmd::stDareNpcGetGold *rev = (Cmd::stDareNpcGetGold *)ptNullCmd;
				//processGetGold(pUser, rev);
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
bool CNpcDareM::processSceneMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch(ptNullCmd->para)
	{
		case Cmd::Session::PARA_SEPT_NPCDARE_GETGOLD:
			{
				Cmd::Session::t_NpcDare_GetGold_SceneSession *rev = (Cmd::Session::t_NpcDare_GetGold_SceneSession *)ptNullCmd;
				processGetGold(rev);
				return true;
			}
			break;
		case Cmd::Session::PARA_QUESTION_NPCDARE:
			{
				Cmd::Session::t_questionNpcDare_SceneSession* ptCmd = 
					(Cmd::Session::t_questionNpcDare_SceneSession*)ptNullCmd;

				UserSession* pUser = UserSessionManager::getInstance()->getUserByID(ptCmd->dwUserID);
				if (!pUser) return true;
				CNpcDareObj *Obj = this->searchRecord(ptCmd->dwCountryID, ptCmd->dwMapID, ptCmd->dwNpcID);
				if (Obj)
				{
					switch(ptCmd->byType)
					{
						case Cmd::QUESTION_NPCDARE_HOLD:
							{
								DWORD septid = Obj->get_holdseptid();
								if (0 == septid)
								{
									pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "目前没有家族控制该商人");
								}
								else
								{
									CSept *pSept = CSeptM::getMe().getSeptByID(septid);
									if (pSept)
									{
										pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "目前由%s家族控制",	pSept->name);
									}
									else
									{
										pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "控制该商人的家族可能已经解散");
									}
								}
							}
							break;
						case Cmd::QUESTION_NPCDARE_DARE:
							{
								DWORD septid = Obj->get_dareseptid();
								if (0 == septid)
								{
									pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "目前没有家族挑战商人的控制权");
								}
								else
								{
									CSept *pSept = CSeptM::getMe().getSeptByID(septid);
									if (pSept)
									{
										pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "目前%s家族在挑战商人的控制权",	pSept->name);
									}
									else
									{
										pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "挑战的家族可能已经解散");
									}
								}
							}
						default:
							break;
					}
				}
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "商人的思绪混乱中，无法回忆相关情况");
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SEPT_NPCDARE_DARE:
			{
				Cmd::Session::t_NpcDare_Dare_SceneSession *rev = (Cmd::Session::t_NpcDare_Dare_SceneSession *)ptNullCmd;
				processRequest(rev);
				return true;
			}
			break;
		case Cmd::Session::PARA_SEPT_NPCDARE_RESULT:
			{
				Cmd::Session::t_NpcDare_Result_SceneSession *rev = (Cmd::Session::t_NpcDare_Result_SceneSession *)ptNullCmd;
				CNpcDareObj * Obj = getNpcDareObjBySept(rev->dwSeptID);
				if (Obj)
				{
					Obj->processResult(rev->dwSeptID);
				}
				return true;
			}
			break;
		case Cmd::Session::PARA_SEPT_NPCDARE_GMCMD:
			{
				doDare();
				_notDare = false;
				return true;
			}
			break;
		default:
			break;
	}

	return false;
}


/**
* \brief 搜索该家族有否占用或者正在挑战商业NPC
* \param septid 家族id
* \author fqnewman
* \return true 成功找到该家族的信息 false 改家族无相关信息在商业NPC群组中
*/
bool CNpcDareM::searchSept(DWORD septid)
{
	std::vector<CNpcDareObj*>::iterator vIterator;

	for(vIterator = _objList.begin(); vIterator!=_objList.end(); vIterator++)
	{
		if ((*vIterator)->get_dareseptid() == septid ||
			(*vIterator)->get_holdseptid() == septid )
		{
			return true;
		}
	}
	return false;
}

/**
* \brief 搜索该家族有否占用商业NPC
* \param septid 家族id
* \author fqnewman
* \return true 成功找到该家族的信息 false 改家族无占领信息在商业NPC群组中
*/
CNpcDareObj* CNpcDareM::searchSeptHold(DWORD septid)
{
	std::vector<CNpcDareObj*>::iterator vIterator;

	for(vIterator = _objList.begin(); vIterator!=_objList.end(); vIterator++)
	{
		if ((*vIterator)->get_holdseptid() == septid )
		{
			return *vIterator;
		}
	}
	return NULL;
}

/**
* \brief 搜索该家族有否占用或者正在挑战商业NPC
* \param septid 家族id
* \author fqnewman
* \return true 成功找到该家族的信息 false 改家族无相关信息在商业NPC群组中
*/
CNpcDareObj* CNpcDareM::searchRecord(DWORD dwCountryID, DWORD dwMapID, DWORD dwNpcID)
{
	std::vector<CNpcDareObj*>::iterator vIterator;

	for(vIterator = _objList.begin(); vIterator!=_objList.end(); vIterator++)
	{
		if ((*vIterator)->get_country() == dwCountryID &&
			(*vIterator)->get_mapid() == dwMapID &&
			(*vIterator)->get_npcid() == dwNpcID)
		{
			return (*vIterator);
		}
	}
	return NULL;
}

/**
* \brief 搜索有该家族信息的挑战记录
* \param septid 家族id
* \author fqnewman
* \return true 挑战对象，找不到返回NULL
*/
CNpcDareObj* CNpcDareM::getNpcDareObjBySept(DWORD septid)
{
	std::vector<CNpcDareObj*>::iterator vIterator;

	for(vIterator = _objList.begin(); vIterator!=_objList.end(); vIterator++)
	{
		if ((*vIterator)->get_dareseptid() == septid ||
			(*vIterator)->get_holdseptid() == septid )
		{
			return (*vIterator);
		}
	}
	return NULL;
}

void CNpcDareM::sendUserData(UserSession *pUser)
{
	DWORD septid = CSeptM::getMe().getSeptIDByUserName(pUser->name);
	if (0 != septid) 
	{
		CNpcDareObj * cnd = this->searchSeptHold(septid);
		if (cnd)
		{
			Cmd::Session::t_notifyNpcHoldData send;
			send.dwUserID = pUser->id;
			send.dwMapID = cnd->get_mapid();
			send.dwPosX = cnd->get_posx();
			send.dwPosY = cnd->get_posy();
			pUser->scene->sendCmd(&send, sizeof(send));
			return ;
		}	
	}

	Cmd::Session::t_notifyNpcHoldData send;
	send.dwUserID = pUser->id;
	send.dwMapID = 0;
	send.dwPosX = 0;
	send.dwPosY = 0;
	pUser->scene->sendCmd(&send, sizeof(send));	
}

//------------------------------------------------------------------------------------------------------------

/**
* \brief 更新数据库记录
* \author fqnewman
*/
void CNpcDareObj::writeDatabase()
{
	const dbCol npcdare_update_define[] = {
		{ "`HOLDSEPTID`",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`DARESEPTID`",	zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "`GOLD`",			zDBConnPool::DB_DWORD,	sizeof(DWORD) },
		{ NULL, 0, 0}
	};

	struct {
		DWORD dwHoldSeptID;
		DWORD dwDareSeptID;
		DWORD dwGold;
	} __attribute__ ((packed))
	updatenpcdare_data;
	char where[128];

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}

	bzero(&updatenpcdare_data,sizeof(updatenpcdare_data));

	updatenpcdare_data.dwHoldSeptID = _dwHoldSeptID;
	updatenpcdare_data.dwDareSeptID = _dwDareSeptID;
	updatenpcdare_data.dwGold = _dwGold;

	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "COUNTRY = '%u' AND MAPID = '%u' AND NPCID = '%u'", _dwCountry, _dwMapID, _dwNpcID);

	unsigned int retcode = SessionService::dbConnPool->exeUpdate(handle, "`NPCDARE`", npcdare_update_define, (BYTE *)(&updatenpcdare_data), where);
	SessionService::dbConnPool->putHandle(handle);

	if (1 != retcode)
	{
		Zebra::logger->error("修改NPC对战档案失败：country=%u, mapid=%u npcid=%u", _dwCountry, _dwMapID, _dwNpcID);
	}
}


/**
* \brief 初始化建立一个NPCDare对象
* \author fqnewman
*/
void CNpcDareObj::create(NpcDareDef::NpcDareRecord &record)
{
	_dwCountry	= record.dwCountry;
	_dwMapID	= record.dwMapID;
	_dwNpcID	= record.dwNpcID;
	_dwHoldSeptID = record.dwHoldSeptID;
	_dwDareSeptID = record.dwDareSeptID;
	_dwGold		= record.dwGold;
	_dwPosX		= record.dwPosX;
	_dwPosY		= record.dwPosY;
	_dwResultHold = 0;
	_dwResultDare = 0;
	_dareStep	=	0;
}

/**
* \brief 道具物品返还
* \param pUser 角色
* \author fqnewman
*/
void CNpcDareObj::itemBack(UserSession *pUser)
{
	Cmd::Session::t_NpcDare_ItemBack_SceneSession send;
	send.dwUserID = pUser->id;
	pUser->scene->sendCmd(&send, sizeof(send));
}

/**
* \brief 对战请求
* \param userID 角色
* \author fqnewman
*/
void CNpcDareObj::dareRequest(DWORD userId)
{
	DWORD septid = 0;
	UserSession *pUser = NULL;
	pUser = UserSessionManager::getInstance()->getUserByID(userId);
	if (NULL == pUser) return;

	if ((septid=CSeptM::getMe().findUserSept(userId)) == 0)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "无法发起挑战你不是族长！");
//		itemBack(pUser);
	}
	else
	{
		if (!CNpcDareM::getMe().searchSept(septid)) //搜索改家族有否挑战或占有商业NPC
		{
			CSept *pSept = CSeptM::getMe().getSeptByID(_dwHoldSeptID);
			if (_dwHoldSeptID == 0 ||((_dwHoldSeptID !=0) && (pSept==NULL)))
			{
				_dwHoldSeptID = septid;
				itemBack(pUser);
				writeDatabase();
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你已经控制了该商人，可以每天向他收取管理费！");
				CSeptM::getMe().notifyNpcHoldData(septid);
			}
			else
			{
				if (_dwDareSeptID == 0)
				{
					// 无人挑战
					struct tm  tv1;
					time_t timValue = time(NULL);
					zRTime::getLocalTime(tv1, timValue);
					if (tv1.tm_hour <18 || tv1.tm_hour>20)
					{
						//发起挑战
						_dwDareSeptID = septid;
						writeDatabase();
						itemBack(pUser);
						pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你的挑战请求已被接受！");
						SceneSession *scene= SceneSessionManager::getInstance()->getSceneByID((this->_dwCountry<<16)+this->_dwMapID);
						if (scene)
						{
							CSeptM::getMe().sendSeptNotify(this->_dwHoldSeptID, "有家族准备向你家族在%s(%u,%u)处发起商人控制权争夺战", scene->name, this->_dwPosX, this->_dwPosY);
							CSeptM::getMe().sendSeptNotify(this->_dwDareSeptID, "家族将准备向其他家族在%s(%u,%u)处发起商人控制权争夺战", scene->name, this->_dwPosX, this->_dwPosY);
						}
						else
						{
							CSeptM::getMe().sendSeptNotify(this->_dwHoldSeptID, "有家族准备向你家族发起商人控制权争夺战，请准备！");
							CSeptM::getMe().sendSeptNotify(this->_dwDareSeptID, "家族将准备向其他家族发起商人控制权争夺战，请准备！");
						}
					}
					else
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "现在暂不受理挑战业务！");
//						itemBack(pUser);
					}
				}
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "抱歉你来晚了，今天已经有一个挑战了！");
//					itemBack(pUser);
				}
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你不能发起挑战！");
//			itemBack(pUser);
		}
	}
}

/**
* \brief 判断检查的目标是不是本对象
* \param country 国家 id
* \param mapid 地图 id
* \param npcid NPC id
* \author fqnewman
*/
bool CNpcDareObj::isMe(DWORD country, DWORD mapid, DWORD npcid)
{
	if (_dwCountry == country &&
		_dwMapID == mapid &&
		_dwNpcID == npcid)
		return true;
	else
		return false;
}

/**
* \brief 执行对战开始操作
* \author fqnewman
* \return true 成功开战 false 未有开战
*/
bool CNpcDareObj::doDare()
{
	if (this->_dwDareSeptID !=0 &&
		this->_dwHoldSeptID !=0)
	{
		_dwResultDare = 0;
		_dwResultHold = 0;
		_dareStep =0;

		std::vector<DWORD> dare_list;
		dare_list.push_back(this->_dwDareSeptID);

		Cmd::Session::t_NpcDare_NotifyScene_SceneSession send;
		send.dwCountryID = this->_dwCountry;
		send.dwMapID = this->_dwMapID;
		send.dwNpcID = this->_dwNpcID;
		send.dwPosX  = this->_dwPosX;
		send.dwPoxY  = this->_dwPosY;

		CSeptM::getMe().sendNpcDareCmdToScene(this->_dwDareSeptID, &send, sizeof(send));
		CSeptM::getMe().sendNpcDareCmdToScene(this->_dwHoldSeptID, &send, sizeof(send));
		CSeptM::getMe().sendSeptNotify(this->_dwDareSeptID, "商人争夺战开始，参战人员结束之前不要离开战区");
		CSeptM::getMe().sendSeptNotify(this->_dwHoldSeptID, "商人争夺战开始，参战人员结束之前不要离开战区");


		Cmd::Session::t_createDare_SceneSession pCmd;
#ifdef _DEBUGLOG
		pCmd.active_time	=	60;
#else
		pCmd.active_time	=	600;
#endif
		pCmd.ready_time	=	1;

		pCmd.relationID2	=	this->_dwHoldSeptID;
		pCmd.type			=	Cmd::SEPT_NPC_DARE;

		CDareM::getMe().createDare_sceneSession(&pCmd, dare_list);
		return true;
	}
	return false;
}


/**
* \brief 执行对战开始的通知
* \author fqnewman
* \return true 成功开战 false 未有开战
*/
bool CNpcDareObj::notifyDareReady()
{
	if (_dwGold< 1000000)
	{
		if (this->_dwDareSeptID ==0 &&
			this->_dwHoldSeptID ==0)
		{
			_dwGold = 0;
		}
		_dwGold += 4000;
		this->writeDatabase();
	}
	if (this->_dwDareSeptID !=0 &&
		this->_dwHoldSeptID !=0)
	{
		SceneSession *scene= SceneSessionManager::getInstance()->getSceneByID((this->_dwCountry<<16)+this->_dwMapID);
		if (scene)
		{
			CSeptM::getMe().sendSeptNotify(this->_dwDareSeptID, "5分钟以后争夺战开始，请到%s（%u,%u）坐标集合！", scene->name, this->_dwPosX, this->_dwPosY);
			CSeptM::getMe().sendSeptNotify(this->_dwHoldSeptID, "5分钟以后争夺战开始，请到%s（%u,%u）坐标集合！", scene->name, this->_dwPosX, this->_dwPosY);
		}
		else
		{
			CSeptM::getMe().sendSeptNotify(this->_dwDareSeptID, "请到商人处集合，5分钟以后商人控制权争夺战开始");
			CSeptM::getMe().sendSeptNotify(this->_dwHoldSeptID, "请到商人处集合，5分钟以后商人控制权争夺战开始");
		}
		return true;
	}
	return false;
}

/**
* \brief 收集对战结果
* \param septid 家族
* \author fqnewman
*/
void CNpcDareObj::processResult(DWORD septid)
{
	if (this->_dwDareSeptID == septid)
	{
		_dareStep = 1;
		_dwResultDare++;
#ifdef _DEBUGLOG
	Zebra::logger->debug("收到交战结果家族id=%u _dwResultDare=%u", septid, _dwResultDare);
#endif
	}
	if (this->_dwHoldSeptID == septid)
	{
		_dareStep = 1;
		_dwResultHold++;
#ifdef _DEBUGLOG
	Zebra::logger->debug("收到交战结果家族id=%u _dwResultHold=%u", septid, _dwResultHold);
#endif
	}
	resultTime = SessionTimeTick::currentTime;
}

/**
* \brief 强制进行对战结果计算
* \author fqnewman
*/
void CNpcDareObj::forceProcessResult()
{
	if (0 == _dareStep)
	{
		_dareStep=1;
		resultTime = SessionTimeTick::currentTime;
	}
}


/**
* \brief 通知对战结果
* \param septid 家族
* \author fqnewman
*/
void CNpcDareObj::doResult()
{
	if ((1==_dareStep) && SessionTimeTick::currentTime.sec() - resultTime.sec() >120)
	{
		Zebra::logger->trace("[家族]家族NPC争夺战控制者[%u]家族剩余[%d]人挑战者[%u]家族剩余[%d]人MAPID=[%u] NPCID=[%u]", this->_dwHoldSeptID, this->_dwResultHold, this->_dwDareSeptID, this->_dwResultDare, this->_dwMapID, this->_dwNpcID);
		if (this->_dwResultHold>= this->_dwResultDare)
		{
			CSeptM::getMe().changeRepute(this->_dwDareSeptID, -5);
				
			CSeptM::getMe().sendSeptNotify(this->_dwHoldSeptID, "恭喜你，你的家族保住了商人的控制权！");
			CSeptM::getMe().sendSeptNotify(this->_dwDareSeptID, "很遗憾，你的家族挑战失败！失去5点声望！");
		}
		else
		{
			CSeptM::getMe().changeRepute(this->_dwHoldSeptID, -5);
			
			CSeptM::getMe().sendSeptNotify(this->_dwDareSeptID, "恭喜你，你的家族赢得了商人的控制权！");
			CSeptM::getMe().sendSeptNotify(this->_dwHoldSeptID, 
				"很遗憾，你的家族失去了商人的控制权！失去5点声望");

			this->_dwHoldSeptID = this->_dwDareSeptID;
		}
		CSeptM::getMe().notifyNpcHoldData(this->_dwHoldSeptID);
		CSeptM::getMe().notifyNpcHoldData(this->_dwDareSeptID);
		this->_dwDareSeptID = 0;
		this->_dareStep	=	2;
		this->writeDatabase();
	}
}

/**
* \brief 处理角色收取保护费
* \author fqnewman
*/
void CNpcDareObj::processGetGold(UserSession *pUser, DWORD septid, DWORD dwNpcID, DWORD dwMapID, DWORD dwCountryID)
{
	if (this->_dwHoldSeptID == septid &&
		this->_dwMapID == dwMapID &&
		this->_dwCountry == dwCountryID &&
		this->_dwNpcID == dwNpcID)
	{
		if (this->_dwGold >0)
		{
			Cmd::Session::t_NpcDare_GetGold_SceneSession send;
			send.dwUserID = pUser->id;
			send.dwGold = _dwGold;
			pUser->scene->sendCmd(&send, sizeof(send));
			Zebra::logger->trace("[家族]角色%s领取了NPC税金%u", pUser->name, _dwGold);
			_dwGold = 0;
			this->writeDatabase();
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "今天已经收过税了，不能再次领取！");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你无法向他收取税金");
	}

}

/**
* \brief 获取npc所在国家
* \author fqnewman
*/
DWORD CNpcDareObj::get_country()
{
	return _dwCountry;
}

/**
* \brief 获取npc所在地图
* \author fqnewman
*/
DWORD CNpcDareObj::get_mapid()
{
	return _dwMapID;
}

/**
* \brief 获取npc的id
* \author fqnewman
*/
DWORD CNpcDareObj::get_npcid()
{
	return _dwNpcID;
}

/**
* \brief 获取当前控制npc的家族id
* \author fqnewman
*/
DWORD CNpcDareObj::get_holdseptid()
{
	return _dwHoldSeptID;
}

/**
* \brief 获取当前挑战npc的家族id
* \author fqnewman
*/
DWORD CNpcDareObj::get_dareseptid()
{
	return _dwDareSeptID;
}

/**
* \brief 获取npc现在的税金
* \author fqnewman
*/
DWORD CNpcDareObj::get_gold()
{
	return _dwGold;
}

/**
* \brief 获取npc的x坐标
* \author fqnewman
*/
DWORD CNpcDareObj::get_posx()
{
	return _dwPosX;
}

/**
* \brief 获取npc的y坐标
* \author fqnewman
*/
DWORD CNpcDareObj::get_posy()
{
	return _dwPosY;
}

/**
* \brief 家族放弃NPC
* \author zjw
*/
void CNpcDareObj::abandon_npc()
{
	CSeptM::getMe().sendSeptNotify(_dwHoldSeptID, "贵家族放弃商人所有权");
	_dwHoldSeptID = 0;	
	_dwGold = 0;
	this->writeDatabase();
}

