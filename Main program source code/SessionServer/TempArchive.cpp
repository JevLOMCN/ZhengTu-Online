/**
 * \file
 * \version  $Id: TempArchive.cpp $
 * \author  
 * \date 
 * \brief 临时档案存储
 *
 * 
 */

#include "TempArchive.h"
#include "zMutex.h"


GlobalTempArchiveIndex *GlobalTempArchiveIndex::_instance = NULL;
GlobalTempArchiveIndex::GlobalTempArchiveIndex()
{
}
GlobalTempArchiveIndex::~GlobalTempArchiveIndex()
{
}
/**
 * \brief 得到唯一实例
 *
 *
 * \return 
 */
GlobalTempArchiveIndex *GlobalTempArchiveIndex::getInstance()
{
	if(_instance == NULL)
	{
		_instance = new GlobalTempArchiveIndex; 
	}
	return _instance;
}
/**
 * \brief 删除唯一实例
 *
 *
 * \return 
 */
void GlobalTempArchiveIndex::delInstance()
{
	SAFE_DELETE(_instance);
}
/**
 * \brief 读取一个用户的临时档案
 *
 *
 * \param id: 用户id
 * \param out: 输出buf
 * \param outSize: 输出数据大小
 * \return 读取是否成功
 */
bool GlobalTempArchiveIndex::readTempArchive(DWORD id , char *out , DWORD &outSize)
{
	checkOverdue();
	mlock.lock();
	TempArchive_iterator iter = tempArchive.find(id);
	if(iter == tempArchive.end())
	{
		outSize = 0;
		mlock.unlock();
		return false;
	}
	if(outSize < iter->second->dwSize || !out)
	{
		outSize = 0 ;
		mlock.unlock();
		return false;
	}
	outSize = iter->second->dwSize;
	//Zebra::logger->debug("临时读档数据内容大小%u" , outSize);
	bcopy(iter->second->data , out , outSize);
	remove(iter);
	mlock.unlock();
	return true;
}
/**
 * \brief 检查是否有过期的档案
 *
 *
 * \return 
 */
void GlobalTempArchiveIndex::checkOverdue()
{
	TempArchive_iterator del_iter;
	zRTime ctv;
	mlock.lock();
	for(TempArchive_iterator iter = tempArchive.begin() ; iter != tempArchive.end() ;)
	{
		//Zebra::logger->debug("临时存档数据离失效时间还剩%u秒",ctv.sec() - iter->second->createtime.sec());
		if(ctv.sec() - iter->second->createtime.sec() > 120)
		{
			//Zebra::logger->trace("临时存档数据过期失效%u" , iter->second->id);
			del_iter = iter;
			iter ++;
			remove(del_iter);
			continue;
		}
		iter ++;
	}
	mlock.unlock();
}
/**
 * \brief 保存一个用户的临时档案
 *
 *
 * \param id: 用户id
 * \param data: 输出buf
 * \param dwSize: 输出数据大小
 * \return 读取是否成功
 */
bool GlobalTempArchiveIndex::writeTempArchive(DWORD id , char *data , DWORD  dwSize)
{
	//Zebra::logger->debug("临时存档数据内容大小%u" , dwSize);
	checkOverdue();
	
	mlock.lock();
	TempArchive_iterator iter = tempArchive.find(id);
	if(iter != tempArchive.end())
	{
		remove(iter);
	}
	char *buf = new char[sizeof(TempArchive) + dwSize];
	if(!buf)
	{
		mlock.unlock();
		return false;
	}
	TempArchive *ta = (TempArchive *)buf;
	ta->id = id ;
	zRTime ctv;
	ta->createtime = ctv;
	ta->dwSize = dwSize;
	bcopy(data , ta->data , dwSize);
	bool inserted = tempArchive.insert(TempArchive_value_type(ta->id , ta)).second;
	mlock.unlock();
	if(!inserted)
	{
		SAFE_DELETE_VEC(buf);
	}

	return inserted;
}
/**
 * \brief 删除一个用户档案所占用的空间
 *
 *
 * \param del_iter: 需要删除的迭代
 * \return 
 */
void GlobalTempArchiveIndex::remove(TempArchive_iterator del_iter)
{
	char *tmp = (char*)del_iter->second;
	SAFE_DELETE_VEC(tmp);
	tempArchive.erase(del_iter);
}
