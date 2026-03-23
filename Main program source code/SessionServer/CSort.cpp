/**
 * \file
 * \version  $Id: CSort.cpp $
 * \author  
 * \date 
 * \brief 实现等级排序功能
 *
 */

#include "CSort.h"
#include "Session.h"
#include "CharBase.h"
#include "zDBConnPool.h"
#include "SessionServer.h"

CSortM *CSortM::csm(NULL);

/*
* \brief 管理器构造函数
* \author fqnewman
*/
CSortM::CSortM()
{
}

/**
* \brief 管理器析构函数
* \author fqnewman
*/
CSortM::~CSortM()
{
	clearDBTable();
	createDBRecord();
}

/**
* \brief 主动释放管理器
* \author fqnewman
*/
void CSortM::destroyMe()
{
	if (csm) SAFE_DELETE(csm);
}

/**
* \brief 获取唯一对象实例
* \author fqnewman
*/
CSortM& CSortM::getMe()
{
	if (!csm)
		csm = new CSortM();
	return *csm;
}

/**
* \brief 初始化排队系统
* \author fqnewman
*/
bool CSortM::init()
{
	//FunctionTimes times(69,__FUNCTION__);
	//bzero(leveltable, sizeof(leveltable));
	const dbCol sortlist_read_define[] = {
		{ "CHARID",			zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "LEVEL",			zDBConnPool::DB_WORD,	sizeof(WORD) }, 
		{ "EXP",			zDBConnPool::DB_QWORD,	sizeof(QWORD) }, 
		{ NULL, 0, 0}
	};

	struct 											// 数据库读取结构，请不要随意修改，修改注意更新所有代码
	{
		DWORD		dwCharID;  			// 角色ID
		WORD		wdLevel;   			// 人物等级
		QWORD 		qwExp;				// 人物经验
	}__attribute__ ((packed))	*recordList,*tempPoint;

	recordList = NULL;
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	unsigned int retcode = SessionService::dbConnPool->exeSelect(handle, "`SORTLIST`", sortlist_read_define, NULL, NULL, (BYTE **)&recordList);
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
			onlineCount(tempPoint->dwCharID, tempPoint->wdLevel, tempPoint->qwExp);
			tempPoint++;
		}
		SAFE_DELETE_VEC(recordList);
		return true;
	}
	else
	{
		Zebra::logger->error("角色排序数据初始化失败，exeSelect 返回无效buf指针");
	}
	return false;
}

/**
* \brief 清空角色排序表记录
* \author fqnewman
* \return true 成功  false 失败
*/
bool CSortM::clearDBTable()
{
	//FunctionTimes times(68,__FUNCTION__);
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}
	unsigned int retcode = SessionService::dbConnPool->exeDelete(handle, "`SORTLIST`", NULL);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("清除角色排序表错误");
		return false;
	}
	return true;
}

/**
* \brief 建立排队记录
* \param dwCharID 角色ID
* \param wdLevel  角色等级
* \param qwExp    角色经验
* \author fqnewman
* \return true 成功  false 失败
*/
bool CSortM::createDBRecord()
{
	//FunctionTimes times(67,__FUNCTION__);
	static const dbCol createsortlist_define[] = {
		{ "`CHARID`", zDBConnPool::DB_DWORD, sizeof(DWORD) }, 
		{ "`LEVEL`", zDBConnPool::DB_WORD,  sizeof(WORD) },
		{ "`EXP`", zDBConnPool::DB_QWORD,  sizeof(QWORD) },
		{ NULL, 0, 0}
	};
	struct {
		DWORD dwCharID;
		WORD  wdLevel;
		QWORD qwExp;
	} __attribute__ ((packed))
	createsortlist_data;
	
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	std::map<QWORD,DWORD>::iterator sIterator;
	for(sIterator = _sortKey.begin(); sIterator != _sortKey.end(); sIterator++)
	{
		createsortlist_data.dwCharID = sIterator->second;
		createsortlist_data.wdLevel = (WORD)(sIterator->first/(WORD)100000000);
		createsortlist_data.qwExp = sIterator->first - createsortlist_data.wdLevel*(WORD)100000000;
		SessionService::dbConnPool->exeInsert(handle, "`SORTLIST`", createsortlist_define, (const unsigned char *)(&createsortlist_data));
	}
	SessionService::dbConnPool->putHandle(handle);
	return true;
}


/**
* \brief 角色上线处理
* \param pUser 当前角色
* \author fqnewman
*/
void CSortM::onlineCount(UserSession *pUser)
{
	if (pUser->id <=100) return;
	onlineCount(pUser->id, pUser->level, pUser->qwExp);
}

/**
* \brief 角色上线处理
* \param dwCharID 角色ID
* \param wdLevel  角色等级
* \param qwExp    角色经验
* \author fqnewman
*/
void CSortM::onlineCount(DWORD dwCharID, WORD wdLevel, QWORD qwExp)
{
	//int level = (int)pUser->level;
	//if (level >0 && level <= MAX_LEVEL)
	//{
	//	if (leveltable[level] <65535) leveltable[level] = leveltable[level]+1;
	//}
	if (dwCharID <=100) return;
	
	std::map<DWORD,QWORD>::iterator tIterator;
	std::map<QWORD,DWORD>::iterator sIterator;

	tIterator = _sortMap.find(dwCharID);
	if (tIterator == _sortMap.end()) // 之前不处于排名系统中才处理
	{
		QWORD key = (WORD)(wdLevel*(WORD)100000000) + qwExp;

		_sortKey.insert(keyValueType(key, dwCharID));
		_sortMap.insert(mapValueType(dwCharID, key));

		int count = _sortKey.size();
		if (count>5000)
		{
			count-=5000;
			for(int i=0; i<count; i++)
			{
				sIterator = _sortKey.end();
				sIterator--;
				_sortMap.erase(sIterator->second);
				_sortKey.erase(sIterator);
			}
		}
	}
	else
	{
		_sortKey.erase(tIterator->second);
		_sortMap.erase(tIterator);
		this->onlineCount(dwCharID, wdLevel, qwExp);
	}
}

/**
* \brief 角色离线处理
* \param pUser 当前角色
* \author fqnewman
*/
void CSortM::offlineCount(UserSession *pUser)
{
//	int level = (int)pUser->level;
//	if (level >0 && level <= MAX_LEVEL)
//	{
//		if ((int)(leveltable[level])>0) leveltable[level] = leveltable[level]-1;
//	}
}

/**
* \brief 角色升级处理
* \param pUser 当前角色
* \author fqnewman
*/
void CSortM::upLevel(UserSession *pUser)
{
//	int level = (int)pUser->level;
//	if (level >0 && level <= MAX_LEVEL)
//	{
//		leveltable[level] = leveltable[level]+1;
//		if (level - 1 >0)
//		{
//			level--;
//			if ((int)(leveltable[level])>0) leveltable[level] = leveltable[level]-1;
//		}
//	}
	if (pUser->id <=100) return;

	std::map<DWORD,QWORD>::iterator tIterator;
	std::map<QWORD,DWORD>::iterator sIterator;

	tIterator = _sortMap.find(pUser->id);
	if (tIterator != _sortMap.end())
	{
		_sortKey.erase(tIterator->second);
		_sortMap.erase(tIterator);
	}
	this->onlineCount(pUser);
}

/**
* \brief 获得当前排名
* \param pUser 当前角色
* \author fqnewman
* \return 当前排名
*/
WORD CSortM::getLevelDegree(UserSession *pUser)
{
//	int level = (int)pUser->level;
//	if (level <10) return 0;
//	WORD degree =1;
//	if (level >0 && level <= MAX_LEVEL)
//	{
//		for(int i=MAX_LEVEL; i>level; i--)
//		{
//			degree+=leveltable[i];
//			if (degree>5000) break;
//		}
//	}
//	if (degree >5000) degree=0;
//	return degree;
	if (pUser->id <=100) return 0;

	WORD degree=0;
	std::map<DWORD,QWORD>::iterator tIterator;
	std::map<QWORD,DWORD>::iterator sIterator;

	tIterator = _sortMap.find(pUser->id);
	if (tIterator != _sortMap.end())
	{
		for (sIterator = _sortKey.begin(); sIterator != _sortKey.end(); sIterator++)
		{
			degree++;
			if (sIterator->second == pUser->id) break;
		}
		return degree;
	}
	return 0;
}

