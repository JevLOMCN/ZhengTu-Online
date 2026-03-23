/**
 * \file
 * \version  $Id: zMysqlDBConnPool.cpp  $
 * \author  
 * \date 
 * \brief 实现Mysql连接池
 *
 * 
 */

#include <mysql.h>
#include <pthread.h>
#include <unistd.h>
#include <zlib.h>
#include <iostream>
#include <ext/hash_map>
#include <sstream>

#include "zDBConnPool.h"
#include "zType.h"
#include "Zebra.h"
#include "zMutex.h"
#include "zNoncopyable.h"
#include "zTime.h"
#include "zMisc.h"
#include "zMetaData.h"

using namespace Zebra;

enum handleState
{
	MYSQLCLIENT_HANDLE_INVALID  = 1,   //无效的
	MYSQLCLIENT_HANDLE_VALID    = 2,   //有效的
	MYSQLCLIENT_HANDLE_USED     = 3,   //被使用
};


/**
 * \brief Mysql连接句柄类
 * 封装了大部分的mysql操作
 */
class MysqlClientHandle : private zNoncopyable
{

	private:

		const connHandleID id;

		const UrlInfo url;

		unsigned int getedCount;
		zTime lifeTime;
		MYSQL *mysql; 

		bool initMysql();
		void finalHandle();

		static connHandleID HandleID_generator;

	public:

		handleState state; ///handle状态
		pthread_t getedThread;
		zTime useTime;
		std::string my_sql;

		MysqlClientHandle(const UrlInfo &url)
			: id(++HandleID_generator), url(url), lifeTime(), useTime()
			{
				state=MYSQLCLIENT_HANDLE_INVALID;
				getedCount=0;
				getedThread=0;
				mysql=NULL;
			}

		~MysqlClientHandle()
		{
			finalHandle();
		}

		const connHandleID &getID() const
		{
			return id;
		}

		const unsigned int hashcode() const
		{
			return url.hashcode;
		}

		bool isSupportTransactions() const
		{
			return url.supportTransactions;
		}

		bool initHandle();
		bool setHandle();
		void unsetHandle();

		bool setTransactions(bool supportTransactions);
		int execSql(const char *sql,unsigned int sqllen);
		unsigned int execSelectSql(const char *sql,unsigned int sqllen ,const dbCol *column,unsigned int limit,unsigned char *data);
		unsigned int fullSelectDataByRow(MYSQL_ROW row,unsigned long *lengths,const dbCol *temp,unsigned char *tempData);
		int fetchSelectSql(const char* tableName,const dbCol *column, const char *where,const char *order, unsigned int limit = 0, unsigned int limit_from = 0);
		unsigned int exeSelect(const char* tableName,const dbCol *column, const char *where,const char *order ,unsigned char **data);
		RecordSet* exeSelect(FieldSet* table, Record* column, Record* where, Record* order = NULL, 
				unsigned int limit=0,
				Record* groupby = NULL, Record* having = NULL);

		unsigned int exeSelectLimit(const char* tableName, const dbCol *column, const char *where,const char *order ,unsigned int limit,unsigned char *data, unsigned int limit_from);
		unsigned int exeInsert(const char *tableName,const dbCol *column,const unsigned char *data);
		unsigned int exeInsert(FieldSet* table, Record* rec);
		unsigned int exeDelete(const char *tableName, const char *where);
		unsigned int exeDelete(FieldSet* table, Record* where);
		unsigned int exeUpdate(const char *tableName,const dbCol *column,const unsigned char *data, const char *where);

		unsigned int exeUpdate(FieldSet* table, Record* data, Record* where);

		char * escapeString(const char *src,char *dest,unsigned int size);
		std::string& escapeString(const std::string &src,std::string &dest);
		void updateDatatimeCol(const char* tableName, const char *colName);
		unsigned int getCount(const char* tableName, const char *where);

};

connHandleID MysqlClientHandle::HandleID_generator = 0;

typedef __gnu_cxx::hash_multimap<unsigned int,MysqlClientHandle *> handlesPool;
typedef __gnu_cxx::hash_map<unsigned int,UrlInfo> urlsPool;
typedef __gnu_cxx::hash_map<connHandleID,MysqlClientHandle *> handlesIDMap;

/**
 * \brief MysqlClient默认HashCode函数,始终返回0
 * \param anyArg 任意参数
 * \return 始终返回0
 */
unsigned int defaultHashCode(const void *anyArg)
{
	return 0;
}

class zMysqlDBConnPool : public zDBConnPool
{

	private:

#ifdef _ZJW_DEBUG
		static const unsigned int maxHandleBuf = 5;
#else		
		static const unsigned int maxHandleBuf = 64;
#endif		

		zMutex mlock;
		handlesPool handles;
		urlsPool urls;
		handlesIDMap idmaps;
		hashCodeFunc hashCode;

	public:

		zMysqlDBConnPool(hashCodeFunc hashfunc)
		{
			if(hashfunc!=NULL)
				hashCode=hashfunc;
			else
				hashCode=defaultHashCode;
			logger->info("Version of the mysql libs is %s" ,mysql_get_client_info());
			if(!mysql_thread_safe())
			{
				logger->warn("The mysql libs is not thread safe...");
			}
		}

		~zMysqlDBConnPool()
		{
			mlock.lock();
			if (!handles.empty())
			{
				for(handlesPool::iterator it = handles.begin(); it != handles.end(); ++it)
				{
					MysqlClientHandle *tempHandle=(*it).second;
					SAFE_DELETE(tempHandle);
				}
			}
			handles.clear();
			urls.clear();
			hashCode=defaultHashCode;
			mlock.unlock();
		}

		bool putURL(unsigned int hashcode,const char *url,bool supportTransactions)
		{
			UrlInfo ui(hashcode,url,supportTransactions);
			MysqlClientHandle *handle=new MysqlClientHandle(ui);
			if (handle==NULL)
				return false;
			if(handle->initHandle())
			{
				mlock.lock();
				handles.insert(handlesPool::value_type(hashcode, handle));
				urls.insert(urlsPool::value_type(hashcode, ui));
				idmaps.insert(handlesIDMap::value_type(handle->getID(), handle));
				mlock.unlock();
				return true;
			}
			else
			{
				SAFE_DELETE(handle);
				return false;
			}
		}

		connHandleID getHandle(const void *hashData)
		{
			unsigned int hashcode=0;
			if(hashData!=NULL)
				hashcode=hashCode(hashData);
			MysqlClientHandle* handle = getHandleByHashcode(hashcode);
			if(handle!=NULL)
				return handle->getID();
			else
				return (connHandleID)-1;
		}

		connHandleID getNextHandle(connHandleID handleID)
		{
			Zebra::logger->error("%s 没有实现", __PRETTY_FUNCTION__);
			// /*
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				urlsPool::iterator rt=urls.find(handle->hashcode());
				if(rt!=urls.end())
				{
					rt++;
					if(rt!=urls.end())
					{
						MysqlClientHandle* nextHandle=getHandleByHashcode((*rt).first);
						if(nextHandle!=NULL) return nextHandle->getID();
					}
				}
			}//*/
			return (connHandleID)-1;
		}

		void putHandle(const connHandleID handleID)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				handle->unsetHandle();
			}
		}

		bool commit(connHandleID handleID)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return (0 == handle->execSql("COMMIT",strlen("COMMIT")));
			}
			else
				return false;
		}

		bool rollback(const connHandleID handleID)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return (0 == handle->execSql("ROLLBACK",strlen("ROLLBACK")));
			}
			else
				return false;
		}

		bool setTransactions(const connHandleID handleID, bool supportTransactions)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return handle->setTransactions(supportTransactions);
			}
			else
				return false;
		}

		bool supportTransactions(connHandleID handleID)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
				return handle->isSupportTransactions();
			else
				return false;
		}

		int execSql(connHandleID handleID, const char *sql,unsigned int sqllen)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return handle->execSql(sql,sqllen);
			}
			else
				return -1;
		}
		unsigned int execSelectSql(connHandleID handleID, const char *sql,unsigned int sqllen ,const dbCol *column,unsigned int limit,unsigned char *data)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return handle->execSelectSql(sql,sqllen ,column, limit,data);
			}
			else
				return (unsigned int)-1;
		}

		unsigned int exeSelect(connHandleID handleID, const char* tableName,const dbCol *column, const char *where,const char *order ,unsigned char **data)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return handle->exeSelect(tableName,column,where,order,data);
			}
			else
				return (unsigned int)-1;
		}

		RecordSet* exeSelect(connHandleID handleID, FieldSet* table, Record* column,
				Record* where,  Record* order = NULL, unsigned int limit=0,
				Record* groupby = NULL, Record* having = NULL)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);

			if (handle!=NULL)
			{
				return handle->exeSelect(table, column, where, order, limit, groupby, having);
			}
			return NULL;
		}

		unsigned int exeSelectLimit(connHandleID handleID, const char* tableName,
				const dbCol *column, const char *where,const char *order ,unsigned int limit,unsigned char *data, unsigned int limit_from = 0)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return handle->exeSelectLimit(tableName,column,where,order,limit,data,limit_from);
			}
			else
				return (unsigned int)-1;
		}

		unsigned int exeInsert(connHandleID handleID, const char *tableName,const dbCol *column,const unsigned char *data)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return handle->exeInsert(tableName,column,data);
			}
			else
				return (unsigned int)-1;
		}

		unsigned int exeInsert(connHandleID handleID, FieldSet* table, Record* rec)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			
			if (handle!=NULL)
			{
				return handle->exeInsert(table, rec);
			}
			else
				return (unsigned int)-1;
		}


		unsigned int exeDelete(connHandleID handleID, const char *tableName, const char *where)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return handle->exeDelete(tableName,where);
			}
			else
				return (unsigned int)-1;
		}

		unsigned int exeDelete(connHandleID handleID, FieldSet* table, Record* where)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);

			if (handle!=NULL)
			{
				return handle->exeDelete(table, where);
			}
			else
				return (unsigned int)-1;
		}

		unsigned int exeUpdate(connHandleID handleID, const char *tableName,const dbCol *column,const unsigned char *data, const char *where)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return handle->exeUpdate(tableName,column,data,where);
			}
			else
				return (unsigned int)-1;
		}

		unsigned int exeUpdate(connHandleID handleID, FieldSet* table, Record* data, Record* where)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);

			if (handle!=NULL)
			{
				return handle->exeUpdate(table, data, where);
			}
			else
				return (unsigned int)-1;
		}

		char * escapeString(connHandleID handleID, const char *src,char *dest,unsigned int size)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return handle->escapeString(src,dest,size);
			}
			else
				return dest;
		}

		std::string& escapeString(connHandleID handleID, const std::string &src,std::string &dest)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return handle->escapeString(src,dest);
			}
			else
				return dest;
		}

		void updateDatatimeCol(connHandleID handleID, const char* tableName, const char *colName)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				handle->updateDatatimeCol(tableName, colName);
			}
		}

		unsigned int getCount(connHandleID handleID, const char* tableName, const char *where)
		{
			MysqlClientHandle* handle = getHandleByID(handleID);
			if(handle!=NULL)
			{
				return handle->getCount(tableName,where);
			}
			else
				return (unsigned int)-1;
		}

	private:

		MysqlClientHandle* getHandleByHashcode(unsigned int hashcode)
		{
			while(true)
			{
				MysqlClientHandle* invalidHandle=NULL;
				mlock.lock();
				__gnu_cxx::pair<handlesPool::iterator,handlesPool::iterator> hps = handles.equal_range(hashcode);
				for(handlesPool::iterator it = hps.first; it != hps.second; ++it)
				{
					MysqlClientHandle* tempHandle=(*it).second;
					switch(tempHandle->state)
					{
						case MYSQLCLIENT_HANDLE_INVALID:
							//handle无效，如果没有找到可用的，需要初始化为可用
							if(invalidHandle==NULL)
								invalidHandle=tempHandle;
							break;
						case MYSQLCLIENT_HANDLE_VALID:
							//handle可用
							if(tempHandle->setHandle())
							{
								mlock.unlock();
								return tempHandle;
							}
							break;
						case MYSQLCLIENT_HANDLE_USED:
							//handle正在使用中
							if(tempHandle->useTime.elapse()>10)
							{
								//使用时间过长，是否程序存在问题
								logger->warn("The handle(%u) timeout %lus by thread %u",
										tempHandle->getID(),tempHandle->useTime.elapse(),tempHandle->getedThread);
								logger->warn("The handle sql is : %s" , tempHandle->my_sql.c_str());
							}
							break;
					}
				}
				if(urls.find(hashcode)==urls.end() || urls[hashcode].url.size()==0)
				{
					mlock.unlock();
					return NULL;
				}
				if(invalidHandle!=NULL)
				{
					if(invalidHandle->initHandle())
					{
						if(invalidHandle->setHandle())
						{
							mlock.unlock();
							return invalidHandle;
						}
					}
				}
				else if(handles.count(hashcode) < maxHandleBuf)
				{
					MysqlClientHandle *handle=new MysqlClientHandle(urls[hashcode]);
					if (NULL==handle)
					{
						mlock.unlock();
						logger->fatal("not enough memory to allocate handle");
						return handle;
					}
					if(handle->initHandle())
					{
						handles.insert(handlesPool::value_type(hashcode, handle));
						idmaps.insert(handlesIDMap::value_type(handle->getID(), handle));

						if(handle->setHandle())
						{
							mlock.unlock();
							return handle;
						}
					}
				}
				mlock.unlock();

				logger->debug("usleep(10000) with getHandleByHashcode");
				usleep(10000);
			}
		}

		MysqlClientHandle* getHandleByID(connHandleID handleID)
		{
			mlock.lock();
			if (!idmaps.empty())
			{
				handlesIDMap::iterator it = idmaps.find(handleID);
				if (it != idmaps.end())
				{
					mlock.unlock();
					return (*it).second;
				}
			}
			/*
			   for(handlesPool::iterator it = handles.begin(); it != handles.end(); ++it)
			   {
			   MysqlClientHandle *tempHandle=(*it).second;
			   if(tempHandle->getID()==handleID)
			   {
			   mlock.unlock();
			   return tempHandle;
			   }
			   }*/
			mlock.unlock();
			return NULL;
		}

};

/* ********************************* *
 * MysqlClientHandle类函数实现       *
 * ********************************* */
bool MysqlClientHandle::initMysql()
{
	if (mysql)
	{
		logger->info("initMysql():The mysql connect will been closed...");
		mysql_close(mysql);
		mysql=NULL;
	}
	mysql=mysql_init(NULL);
	if(mysql==NULL)
	{
		logger->error("initMysql():Initiate mysql error...");
		return false;
	}

	if(mysql_real_connect(mysql,url.host,url.user,url.passwd,url.dbName,url.port,NULL,CLIENT_COMPRESS|CLIENT_INTERACTIVE)==NULL)
	{
		logger->error("initMysql():connect mysql://%s:%u/%s failed...",url.host,url.port,url.dbName);
		//logger->error("reason: %s",mysql_error(mysql));
		return false;
	}
	if (!setTransactions(url.supportTransactions))
	{
		return false;
	}
	logger->info("initMysql():connect mysql://%s:%u/%s successful...",url.host,url.port,url.dbName);
	state=MYSQLCLIENT_HANDLE_VALID;
	lifeTime.now();
	getedCount=0;
	return true;
}

bool MysqlClientHandle::initHandle()
{
	if(!initMysql())
	{
		finalHandle();
		return false;
	}
	return true;
}

void MysqlClientHandle::finalHandle()
{
	if (mysql)
	{
		logger->info("finalHandle():The mysql connect will been closed...");
		mysql_close(mysql);
		mysql=NULL;
	}
	state=MYSQLCLIENT_HANDLE_INVALID;
	getedCount=0;
	getedThread=(pthread_t)0;
	my_sql="";
}

bool MysqlClientHandle::setHandle()
{
	//无效连接,句柄被使用超过1800次或生成超过半小时,重连
	if(getedCount>3600 || lifeTime.elapse()>1800 || mysql_ping(mysql)!=0)
	{
		if(!initMysql())
		{
			finalHandle();
			return false;
		}
	}
	state=MYSQLCLIENT_HANDLE_USED;
	getedCount++;
	useTime.now();
	getedThread=pthread_self();
	return true;
}

void MysqlClientHandle::unsetHandle()
{
	state=MYSQLCLIENT_HANDLE_VALID;
	useTime.now();
	getedThread=0;
}

bool MysqlClientHandle::setTransactions(bool supportTransactions)
{
	if(supportTransactions)
		return (0 == execSql("SET AUTOCOMMIT=0",strlen("SET AUTOCOMMIT=0")));
	else
		return (0 == execSql("SET AUTOCOMMIT=1",strlen("SET AUTOCOMMIT=1")));
}

int MysqlClientHandle::execSql(const char *sql,unsigned int sqllen)
{
	FunctionTime func_time(100000,__PRETTY_FUNCTION__,sql , 32); 
	if(sql==NULL || sqllen==0 || mysql==NULL)
	{
		logger->error("invalid mysql handle or sql statement.");
		return -1;
	}
#ifdef _XWL_DEBUG
	//Zebra::logger->debug("execSql: %s", sql);
#endif
	my_sql=sql;
	int ret=mysql_real_query(mysql,sql,sqllen);
	if(ret)
	{
		logger->error(mysql_error(mysql));
		logger->error_out(sql);
	}
	return ret;
}

unsigned int MysqlClientHandle::execSelectSql(const char *sql,unsigned int sqllen , const dbCol *column, unsigned int limit , unsigned char *data)
{
	FunctionTime func_time(100000,__PRETTY_FUNCTION__,sql , 32); 
	unsigned int reterror = (unsigned int) -1;
	if(sql==NULL || sqllen==0 || mysql==NULL)
	{
		logger->error("invalid mysql handle or sql statement.");
		return reterror;
	}
	my_sql=sql;
	int ret=mysql_real_query(mysql,sql,sqllen);
	if(ret)
	{
		logger->error(mysql_error(mysql));
		logger->error(sql);
	}
	if(ret != 0) return reterror;
	unsigned int retCount=0;
	MYSQL_RES *result=NULL;
	{
		result=mysql_store_result(mysql);
		if(result==NULL)
		{
			logger->error(mysql_error(mysql));
			return reterror;
		}
		retCount =mysql_num_rows(result);
		if(retCount==0)
		{
			mysql_free_result(result);
			return retCount;
		}
	}

	MYSQL_ROW row;
	unsigned char *tempData=data;
	unsigned int count=0;
	while ((row = mysql_fetch_row(result)) && count <limit)
	{
		unsigned long *lengths= mysql_fetch_lengths(result);
		unsigned int fullsize=fullSelectDataByRow(row,lengths,column,tempData);
		if(fullsize==0)
		{
			mysql_free_result(result);
			return count;
		}
		tempData+=fullsize;
		count++;
	}
	mysql_free_result(result);
	return count;
}


unsigned int MysqlClientHandle::fullSelectDataByRow(
		MYSQL_ROW row,unsigned long *lengths,const dbCol *temp,unsigned char *tempData)
{
	int offset=0;
	int i=0;
	while(temp->name)
	{
		if(strlen(temp->name)!=0)
		{
			switch(temp->type)
			{
				case zDBConnPool::DB_CHAR:
					if (row[i])
						*(char *)(tempData+offset)=strtoul(row[i],(char **)NULL,10);
					else
						*(char *)(tempData+offset)=0;
				case zDBConnPool::DB_BYTE:
					if (row[i]) 
						*(BYTE *)(tempData+offset)=strtoul(row[i],(char **)NULL,10);
					else
						*(BYTE *)(tempData+offset)=0;
					break;
				case zDBConnPool::DB_WORD:
					if (row[i]) 
						*(WORD *)(tempData+offset)=strtoul(row[i],(char **)NULL,10);
					else
						*(WORD *)(tempData+offset)=0;
					break;
				case zDBConnPool::DB_DWORD:
					if (row[i]) 
						*(DWORD *)(tempData+offset)=strtoul(row[i],(char **)NULL,10);
					else
						*(DWORD *)(tempData+offset)=0L;
					break;
				case zDBConnPool::DB_QWORD:
					if (row[i]) 
						*(QWORD *)(tempData+offset)=strtoull(row[i],(char **)NULL,10);
					else
						*(QWORD *)(tempData+offset)=0LL;
					break;
				case zDBConnPool::DB_STR:
				case zDBConnPool::DB_BIN:
					bzero(tempData+offset,temp->size);
					if (row[i])
						bcopy(row[i],tempData+offset,temp->size>lengths[i]?lengths[i]:temp->size);
					break;
				case zDBConnPool::DB_BIN2:
					bzero(tempData+offset,sizeof(DWORD));
					if (row[i])
					{
						DWORD bin2size=*((DWORD *)row[i])+sizeof(DWORD);
						bzero(tempData+offset,bin2size);
						bcopy(row[i],tempData+offset,bin2size>lengths[i]?lengths[i]:bin2size);
					}
					break;
				case zDBConnPool::DB_ZIP:
				case zDBConnPool::DB_ZIP2:
					{
						if(temp->size==0)
							bzero(tempData+offset,sizeof(DWORD));
						else
							bzero(tempData+offset,temp->size);
						if (row[i])
						{
							int retcode;
							uLong destLen = temp->size;
							retcode = uncompress(tempData+offset, &destLen, (Bytef *)row[i], lengths[i]);
							switch(retcode) {
								case Z_OK:
									break;
								case Z_MEM_ERROR:
									bzero(tempData+offset,temp->size);
									break;
								case Z_BUF_ERROR:
									bzero(tempData+offset,temp->size);
									break;
								case Z_DATA_ERROR:
									bzero(tempData+offset,temp->size);
									break;
							}
						}
					} 
					break;
				default:
					logger->error("invalid zebra mysql type(%d). ->%s)",temp->type,temp->name);
					return 0;
			}       
			i++;    
		}   
		offset+=temp->size==0?*((DWORD *)tempData+offset):temp->size;
		temp++;
	}   
	return offset;
}

int MysqlClientHandle::fetchSelectSql(const char* tableName,const dbCol *column, const char *where,const char *order, unsigned int limit, unsigned int limit_from)
{
	unsigned int retsize=0;
	const dbCol *temp;
	bool first=true;
	std::string sql;

	sql+="SELECT ";
	temp = column;
	while(temp->name) 
	{   
		/*
		   if(temp->size==0)
		   {
		   logger->error("invalid column data size.");
		   return -1;
		   }
		// */
		retsize+=temp->size;
		if(strlen(temp->name) > 0)
		{
			if(first)
				first=false;
			else
				sql+=", ";
			sql+=temp->name;
		}
		temp++;
	}
	if(strlen(tableName)>0)
	{
		sql+=" FROM ";
		sql+=tableName;
	}
	if(where!=NULL && strlen(where)>0)
	{
		sql+=" WHERE ";
		sql+=where;
	}
	if(order!=NULL && strlen(order)>0)
	{
		sql+=" ORDER BY ";
		sql+=order;
	}
	if (limit_from)
	{
		char tmp[32];
		snprintf(tmp, sizeof(tmp) - 1, "%u , %u", limit_from,limit);
		sql+=" LIMIT ";
		sql+=tmp;
	}
	else if(limit)
	{
		char tmp[32];
		snprintf(tmp, sizeof(tmp) - 1, "%u", limit);
		sql+=" LIMIT ";
		sql+=tmp;
	}
#ifdef _ZJW_DEBUG	
	logger->debug("(%d)%s",sql.length(),sql.c_str());
#endif	
	if(0 == execSql(sql.c_str(),sql.length()))
		return retsize;
	else
		return -1;

}

unsigned int MysqlClientHandle::exeSelect(const char* tableName,const dbCol *column, const char *where,const char *order ,unsigned char **data)
{
	int retval=-1;
	unsigned int retsize=0;

	*data=NULL;
	if(tableName==NULL || column==NULL || mysql==NULL)
	{
		logger->error("null pointer error. ---- %s",__PRETTY_FUNCTION__);
		return retval;
	}
	retsize=fetchSelectSql(tableName,column,where,order);
	if(retsize<1) return retval;
	unsigned int retCount=0;
	MYSQL_RES *result=NULL;
	{
		result=mysql_store_result(mysql);
		if(result==NULL)
		{
			logger->error(mysql_error(mysql));
			return retval;
		}
		retCount =mysql_num_rows(result);
		if(retCount==0)
		{
			mysql_free_result(result);
			return retCount;
		}
	}
	*data =new unsigned char[retCount*retsize];
	if(data==NULL)
	{
		logger->error("malloc mem error %s",__PRETTY_FUNCTION__);
		mysql_free_result(result);
		return retval;
	}
	bzero(*data,retCount*retsize);

	MYSQL_ROW row;
	unsigned char *tempData=*data;
	while ((row = mysql_fetch_row(result)))
	{
		unsigned long *lengths= mysql_fetch_lengths(result);
		unsigned int fullsize=fullSelectDataByRow(row,lengths,column,tempData);
		if(fullsize==0)
		{
			SAFE_DELETE_VEC(*data);
			mysql_free_result(result);
			return retval;
		}
		tempData+=fullsize;
	}

	mysql_free_result(result);
	return retCount;
}

RecordSet* MysqlClientHandle::exeSelect(FieldSet* table, Record* column, Record* where, Record* order,
		unsigned int limit,	
		Record* groupby, Record* having)
{
	using namespace std;

	RecordSet* ret_set = NULL;
	bool first=true;
	ostringstream query_string;
	
	if (table==NULL || mysql==NULL)
	{
		logger->error("null pointer error. ---- %s",__PRETTY_FUNCTION__);
		return NULL;
	}

	unsigned int num_field = table->size();

	query_string << "SELECT ";

	if (column)
	{
		const char* fieldvalue = NULL;
		if (column->size() > 0)
		{

			if (column->find("*"))
			{
				first=false;

				if ((fieldvalue = (*column)["*"]) == NULL
						|| strlen((*column)["*"])==0)
				{
					query_string << "*";
				}
				else
				{
					query_string << fieldvalue;
				}

			}

			for (unsigned int i=0; i<num_field; i++)
			{

				Field* field = NULL;
				field = table->getField(i);

				if (!column->find(field->name))
				{
					continue;
				}

				if (first)
					first=false;
				else
				{
					query_string <<  " , ";
				}

				if ((fieldvalue = (*column)[field->name]) == NULL
						|| strlen((*column)[field->name])==0)
				{
					query_string << field->name;
				}
				else
				{
					query_string << fieldvalue;
				}
			}
		}
		else
		{
			query_string << " * ";
		}
	}
	else
	{
		query_string << " * ";
	}

	query_string << "  FROM " << " `" << table->getTableName() << "` ";

	first = true;

	if (where)
	{
		for (unsigned int i=0; i<num_field; i++)
		{
			const char *fieldvalue = NULL;
			Field* field = NULL;
			field = table->getField(i);

			if ((fieldvalue = (*where)[field->name]) == NULL)
			{
				continue;
			}

			if (first)
			{
				query_string << " WHERE ";
				first=false;
			}
			else
			{
				query_string <<  " AND ";
			}

			//	query_string << field->name;

			switch (field->type)
			{
				case FIELD_TYPE_STRING:
				case FIELD_TYPE_VAR_STRING:
				case FIELD_TYPE_DATE:
				case FIELD_TYPE_TIME:
				case FIELD_TYPE_DATETIME:
				case FIELD_TYPE_YEAR:
				case FIELD_TYPE_BLOB:
					{
						//char strData[strlen(fieldvalue) * 2 + 1];
						//mysql_real_escape_string(mysql, strData, fieldvalue, strlen(fieldvalue));
						query_string << fieldvalue;
					}

					break;
				default:
					query_string << fieldvalue;
					break;
			}

		}
	}

	first = true;

	if (groupby)
	{
		for (unsigned int i=0; i<num_field; i++)
		{
			const char *fieldvalue = NULL;
			Field* field = NULL;
			field = table->getField(i);

			if (!groupby->find(field->name))
			{
				continue;
			}

			if (first)
			{
				query_string << " GROUP BY  ";
				first=false;
			}
			else
			{
				query_string <<  ",  ";
			}

			if ((fieldvalue = (*groupby)[field->name]) == NULL
					|| strlen((*groupby)[field->name])==0)
			{
				query_string << field->name;
			}
			else
			{
				query_string << field->name << " " << fieldvalue;
			}

		}

	}


	first = true;

	if (having)
	{
		for (unsigned int i=0; i<num_field; i++)
		{
			const char *fieldvalue = NULL;
			Field* field = NULL;
			field = table->getField(i);

			if ((fieldvalue = (*having)[field->name]) == NULL)
			{
				continue;
			}

			if (first)
			{
				query_string << " HAVING ";
				first=false;
			}
			else
			{
				query_string <<  " AND ";
			}

			//	query_string << field->name;

			switch (field->type)
			{
				case FIELD_TYPE_STRING:
				case FIELD_TYPE_VAR_STRING:
				case FIELD_TYPE_DATE:
				case FIELD_TYPE_TIME:
				case FIELD_TYPE_DATETIME:
				case FIELD_TYPE_YEAR:
				case FIELD_TYPE_BLOB:
					{
						//char strData[strlen(fieldvalue) * 2 + 1];
						//mysql_real_escape_string(mysql, strData, fieldvalue, strlen(fieldvalue));
						query_string << fieldvalue;
					}

					break;
				default:
					query_string << fieldvalue;
					break;
			}

		}
	}
	first = true;
	if (order)
	{
		for (unsigned int i=0; i<num_field; i++)
		{
			const char *fieldvalue = NULL;
			Field* field = NULL;
			field = table->getField(i);

			if (!order->find(field->name))
			{
				continue;
			}

			if (first)
			{
				query_string << " ORDER BY  ";
				first=false;
			}
			else
			{
				query_string <<  ",  ";
			}

			if ((fieldvalue = (*order)[field->name]) == NULL
					|| strlen((*order)[field->name])==0)
			{
				query_string << field->name;
			}
			else
			{
				query_string << field->name << " " << fieldvalue;
			}

		}

	}

	if (limit)
	{
		query_string << " LIMIT " << limit;
	}

#ifdef _ZJW_DEBUG
	logger->debug("sql:%s", query_string.str().c_str());
#endif 	

	if(0 == execSql(query_string.str().c_str(), query_string.str().size()))
	{
	}
	else
	{
		logger->error(mysql_error(mysql));
		return NULL;
	}


	unsigned int retCount=0;
	MYSQL_RES *result=NULL;

	{
		result=mysql_store_result(mysql);
		if(result==NULL)
		{
			logger->error(mysql_error(mysql));
			return NULL;
		}
		retCount =mysql_num_rows(result);

		if(retCount==0)
		{
			mysql_free_result(result);
			return NULL;
		}
	}

	MYSQL_ROW row;
	num_field = mysql_num_fields(result);
	MYSQL_FIELD* mysql_fields = NULL;
	mysql_fields = mysql_fetch_fields(result);

	if (mysql_num_rows(result) > 0)
	{
		ret_set = new RecordSet();

		if (ret_set)
		{

			while ((row = mysql_fetch_row(result)))
			{
				Record* rec = new Record();
				if (rec)
				{

					for (unsigned int i=0; i<num_field; i++)
					{
						if (row[i] != NULL)
						{
							rec->put(mysql_fields[i].name, row[i]);
#ifdef _ZJW_DEBUG							
							Zebra::logger->debug("%s:%s", mysql_fields[i].name, row[i]);
#endif							
						}
					}
				}
				else
				{
					continue;
				}

				rec->fields = table;
				ret_set->put(rec);
			}
		}
	}

	mysql_free_result(result);

	if (ret_set && ret_set->size() ==0 )
	{
		SAFE_DELETE(ret_set);
	}

	return ret_set;
}

unsigned int MysqlClientHandle::exeSelectLimit(const char* tableName, const dbCol *column, const char *where,const char *order ,unsigned int limit,unsigned char *data ,unsigned int limit_from = 0)
{
	unsigned int errorret=(unsigned int)-1;
	unsigned int retsize;

	if(tableName==NULL || column==NULL || mysql==NULL)
	{
		logger->error("null pointer error. ---- %s",__PRETTY_FUNCTION__);
		return errorret;
	}
	retsize=fetchSelectSql(tableName,column,where,order,limit,limit_from);
	if(retsize<1) return errorret;
	unsigned int retCount=0;
	MYSQL_RES *result=NULL;
	{
		result=mysql_store_result(mysql);
		if(result==NULL)
		{
			logger->error(mysql_error(mysql));
			return errorret;
		}
		retCount =mysql_num_rows(result);
		if(retCount==0)
		{
			mysql_free_result(result);
			return retCount;
		}
	}

	MYSQL_ROW row;
	unsigned char *tempData=data;
	unsigned int count=0;
	while ((row = mysql_fetch_row(result)) && count <limit)
	{
		unsigned long *lengths= mysql_fetch_lengths(result);
		unsigned int fullsize=fullSelectDataByRow(row,lengths,column,tempData);
		if(fullsize==0)
		{
			mysql_free_result(result);
			return count;
		}
		tempData+=fullsize;
		count++;
	}
	mysql_free_result(result);
	return count;
}

unsigned int MysqlClientHandle::exeInsert(const char *tableName,const dbCol *column,const unsigned char *data)
{
	const dbCol *temp;
	if(tableName == NULL || data==NULL || column==NULL || mysql==NULL)
	{
		logger->error("null pointer error. ---- %s",__PRETTY_FUNCTION__);
		return (unsigned int)-1;
	}
	std::ostringstream strSql;
	strSql << "INSERT INTO ";
	strSql << tableName;
	strSql << " ( ";
	temp = column; 
	bool first=true;
	while(temp->name) 
	{
		int len = strlen(temp->name);
		if(len > 0)
		{       
			if(first)
				first=false;
			else
				strSql << ", ";
			strSql << temp->name;
		}       
		/*
		   if(temp->size==0)
		   {       
		   logger->error("invalid column data size.");
		   return (unsigned int)-1;
		   } 
		// */
		temp++; 
	}
	strSql << ") VALUES( ";

	first=true;
	temp = column; 
	int offset=0;
	while(temp->name) 
	{
		if(strlen(temp->name)!=0)
		{       
			if(first)
				first=false;
			else
				strSql << ", ";
			switch(temp->type)
			{
				case zDBConnPool::DB_CHAR:
					{
						short temp = 0x00ff & (*(char *)(data+offset));
						strSql << temp;
					}
					break;
				case zDBConnPool::DB_BYTE:
					{
						unsigned short temp = 0x00ff & (*(BYTE *)(data+offset));
						strSql << temp;
					}
					break;
				case zDBConnPool::DB_WORD:
					strSql << *(WORD *)(data+offset);
					break;
				case zDBConnPool::DB_DWORD:
					strSql << *(DWORD *)(data+offset);
					break;
				case zDBConnPool::DB_QWORD:
					strSql << *(QWORD *)(data+offset);
					break;
				case zDBConnPool::DB_STR:
					{
						unsigned int len=strlen((char *)(data+offset));
						len = (len < (unsigned int)temp->size ? len : (unsigned int)temp->size);
						//You must allocate the to buffer to be at least length*2+1 bytes long.
						char strData[len * 2 + 1];
						mysql_real_escape_string(mysql, strData,(char *)(data+offset),len);
						strSql << "\'" << strData << "\'";
					}
					break;
				case zDBConnPool::DB_BIN:
					{
						//You must allocate the to buffer to be at least length*2+1 bytes long.
						char strData[temp->size * 2 + 1];
						mysql_real_escape_string(mysql, strData,(char *)(data+offset),temp->size);
						strSql << "\'" << strData << "\'";
					}
					break;
				case zDBConnPool::DB_ZIP:
					{
						uLong destLen = temp->size * 120 / 100 + 12;
						Bytef destBuffer[destLen * sizeof(Bytef)];
						int retcode = compress(destBuffer, &destLen, (Bytef *)(data+offset),temp->size);
						switch(retcode)
						{
							case Z_OK:
								{
									//You must allocate the to buffer to be at least length*2+1 bytes long.
									char strData[destLen * 2 + 1];
									mysql_real_escape_string(mysql,strData,(char *)destBuffer,destLen);
									strSql << "\'" << strData << "\'";
								}
								break; 
							case Z_MEM_ERROR:
								logger->error("Not enough memory, NULL value instead.... %s",__PRETTY_FUNCTION__);
								strSql << "\'\'";
								break;
							case Z_BUF_ERROR:
								logger->error("Not enough memory, NULL value instead.... %s",__PRETTY_FUNCTION__);
								strSql << "\'\'";
								break;
						}
					}
					break;
				case zDBConnPool::DB_BIN2:
					{
						unsigned int size = *((DWORD *)(data+offset));
						size += sizeof(DWORD);
						//You must allocate the to buffer to be at least length*2+1 bytes long.
						char strData[size * 2 + 1];
						mysql_real_escape_string(mysql, strData,(char *)(data+offset),size);
						strSql << "\'" << strData << "\'";
					}
					break;
				case zDBConnPool::DB_ZIP2:
					{
						unsigned int size = *((DWORD *)(data+offset));
						size += sizeof(DWORD);
						uLong destLen = size * 120 / 100 + 12;
						Bytef destBuffer[destLen * sizeof(Bytef)];
						int retcode = compress(destBuffer, &destLen, (Bytef *)(data+offset),size);
						switch(retcode)
						{
							case Z_OK:
								{
									//You must allocate the to buffer to be at least length*2+1 bytes long.
									char strData[destLen * 2 + 1];
									mysql_real_escape_string(mysql,strData,(char *)destBuffer,destLen);
									strSql << "\'" << strData << "\'";
								}
								break; 
							case Z_MEM_ERROR:
								logger->error("Not enough memory, NULL value instead.... %s",__PRETTY_FUNCTION__);
								strSql << "\'\'";
								break;
							case Z_BUF_ERROR:
								logger->error("Not enough memory, NULL value instead.... %s",__PRETTY_FUNCTION__);
								strSql << "\'\'";
								break;
						}
					}
					break;
				default:
					logger->error("invalid zebra mysql type.");
					return (unsigned int)-1;
			}
		}
		if(temp->size==0)
			offset+=(*((DWORD *)(data+offset)) + sizeof(DWORD));
		else
			offset+=temp->size;
		temp++;
	}   
	strSql << ")";
	//logger->debug("(%d)%s",strSql.length(),strSql.c_str());
	if(0 == execSql(strSql.str().c_str(),strSql.str().size()))
		return (unsigned int)mysql_insert_id(mysql);
	else
		return (unsigned int)-1;
}

unsigned int MysqlClientHandle::exeInsert(FieldSet* table, Record* rec)
{
	using namespace std;


	ostringstream query_string;
	ostringstream value_string;

	if(table == NULL || rec==NULL || mysql==NULL)
	{
		logger->error("null pointer error. ---- %s",__PRETTY_FUNCTION__);
		return (unsigned int)-1;
	}


	unsigned int num_field = table->size();


	query_string << "INSERT INTO ";
	query_string << "`" << table->getTableName() << "`";
	query_string << " ( ";

	value_string << " VALUES( ";

	bool first=true;
	for (unsigned int i=0; i<num_field; i++)
	{
		const char *fieldvalue = NULL;
		Field* field = NULL;


		field = table->getField(i);

		if ((fieldvalue = (*rec)[field->name]) == NULL
				|| strlen((*rec)[field->name]) == 0)
		{
			continue;
		}



		if (first)
			first=false;
		else
		{
			query_string <<  " , ";
			value_string <<  " , ";
		}

		query_string << "`" << field->name << "`";

		switch (field->type)
		{
			case FIELD_TYPE_STRING:
			case FIELD_TYPE_VAR_STRING:
			case FIELD_TYPE_DATE:
			case FIELD_TYPE_TIME:
			case FIELD_TYPE_DATETIME:
			case FIELD_TYPE_YEAR:
			case FIELD_TYPE_BLOB:
				{
					char strData[strlen(fieldvalue) * 2 + 1];
					mysql_real_escape_string(mysql, strData, fieldvalue, strlen(fieldvalue));
					value_string << "\'" << strData << "\'";
				}

				break;
			default:
				value_string << fieldvalue;
				break;
		}
	}

	query_string << ")" << value_string.str() << ")";

#ifdef _ZJW_DEBUG
	logger->debug("sql:%s", query_string.str().c_str());	
#endif	

	if (0 == execSql(query_string.str().c_str(), query_string.str().size()))
	{
		return (unsigned int)mysql_insert_id(mysql);
	}
	else
	{
		return (unsigned int)-1;
	}
}

unsigned int MysqlClientHandle::exeDelete(const char *tableName, const char *where)
{
	if(tableName==NULL || mysql==NULL)
	{
		logger->error("null pointer error. ---- %s",__PRETTY_FUNCTION__);
		return (unsigned int)-1;
	}
	std::string strSql="DELETE FROM ";
	strSql+=tableName;
	if(where)
	{
		strSql+=" WHERE ";
		strSql+=where;
	}
	//logger->debug("(%d)%s",strSql.length(),strSql.c_str());
	if (0 == execSql(strSql.c_str(),strSql.length()))
		return (unsigned int)mysql_affected_rows(mysql);
	else
		return (unsigned int)-1;
}

unsigned int MysqlClientHandle::exeDelete(FieldSet* table, Record* where)
{
	using namespace std;
	ostringstream query_string;

	if(table==NULL || mysql==NULL)
	{
		logger->error("null pointer error. ---- %s",__PRETTY_FUNCTION__);
		return (unsigned int)-1;
	}

	unsigned int num_field = table->size();

	query_string << "DELETE FROM ";
	query_string << " `" << table->getTableName() << "` ";

	bool first=true;

	if (where && where->size()>0)
	{
		for (unsigned int i=0; i<num_field; i++)
		{
			const char *fieldvalue = NULL;
			Field* field = NULL;
			field = table->getField(i);

			if ((fieldvalue = (*where)[field->name]) == NULL
					|| strlen((*where)[field->name])==0)
			{
				continue;
			}

			if (first)
			{
				query_string << " WHERE ";
				first=false;
			}
			else
			{
				query_string <<  " AND ";
			}

			//query_string << field->name;

			switch (field->type)
			{
				case FIELD_TYPE_STRING:
				case FIELD_TYPE_VAR_STRING:
				case FIELD_TYPE_DATE:
				case FIELD_TYPE_TIME:
				case FIELD_TYPE_DATETIME:
				case FIELD_TYPE_YEAR:
				case FIELD_TYPE_BLOB:
					{
						//char strData[strlen(fieldvalue) * 2 + 1];
						//mysql_real_escape_string(mysql, strData, fieldvalue, strlen(fieldvalue));
						query_string << fieldvalue;
					}

					break;
				default:
					query_string << fieldvalue;
					break;
			}

		}

	}

#ifdef _ZJW_DEBUG
	logger->debug("sql:%s", query_string.str().c_str());	
#endif	

	if (0 == execSql(query_string.str().c_str(), query_string.str().size()))
	{
		return (unsigned int)mysql_affected_rows(mysql);
	}
	else
	{
		return (unsigned int)-1;
	}
}

unsigned int MysqlClientHandle::exeUpdate(const char *tableName,const dbCol *column,const unsigned char *data, const char *where)
{
	std::ostringstream out_sql;
	const dbCol *temp;
	if(tableName==NULL || column==NULL || data==NULL || mysql==NULL)
	{
		logger->error("null pointer error. ---- %s",__PRETTY_FUNCTION__);
		return (unsigned int)-1;
	}
	out_sql << "UPDATE " << tableName << " SET ";
	temp = column;
	bool first=true;
	int offset=0;
	while(temp->name)
	{
		/*
		   if(temp->size==0)
		   {
		   logger->error("invalid column data size.");
		   return (unsigned int)-1;
		   }
		// */
		int len = strlen(temp->name);
		if(len > 0)
		{
			if(first)
				first=false;
			else
				out_sql << ", ";
			out_sql << temp->name << "=";
			switch(temp->type)
			{
				case zDBConnPool::DB_CHAR:
					{
						short temp = 0x00ff & (*(char *)(data+offset));
						out_sql << temp;
					}
					break;
				case zDBConnPool::DB_BYTE:
					{
						unsigned short temp = 0x00ff & (*(BYTE *)(data+offset));
						out_sql << temp;
					}
					break;
				case zDBConnPool::DB_WORD:
					out_sql << *(WORD *)(data+offset);
					break;
				case zDBConnPool::DB_DWORD:
					out_sql << *(DWORD *)(data+offset);
					break;
				case zDBConnPool::DB_QWORD:
					out_sql << *(QWORD *)(data+offset);
					break;
				case zDBConnPool::DB_STR:
					{
						unsigned int temp_len=strlen((char *)(data+offset));
						temp_len = (temp_len < (unsigned int)temp->size ? temp_len : (unsigned int)temp->size);
						//You must allocate the to buffer to be at least length*2+1 bytes long.
						char buffer[temp_len * 2 + 1];
						mysql_real_escape_string(mysql, buffer,(char *)(data+offset),temp_len);
						out_sql << "\'" << buffer << "\'";
					}
					break;
				case zDBConnPool::DB_BIN:
					{
						//You must allocate the to buffer to be at least length*2+1 bytes long.
						char buffer[temp->size * 2 + 1];
						mysql_real_escape_string(mysql, buffer,(char *)(data+offset),temp->size);
						out_sql << "\'" << buffer << "\'";
					}
					break;
				case zDBConnPool::DB_ZIP:
					{
						uLong destLen = temp->size * 120 / 100 + 12;
						Bytef destBuffer[destLen * sizeof(Bytef)];
						int retcode = compress(destBuffer, &destLen, (Bytef *)(data+offset),temp->size);
						switch(retcode)
						{
							case Z_OK:
								{
									//You must allocate the to buffer to be at least length*2+1 bytes long.
									char buffer[destLen * 2 + 1];
									mysql_real_escape_string(mysql,buffer,(char *)destBuffer,destLen);
									out_sql << "\'" << buffer << "\'";
								}
								break; 
							case Z_MEM_ERROR:
								logger->error("(%s)Z_MEM_ERROR, NULL value instead.",__PRETTY_FUNCTION__);
								out_sql << "\'\'";
								break;
							case Z_BUF_ERROR:
								logger->error("(%s)Z_BUF_ERROR, NULL value instead.",__PRETTY_FUNCTION__);
								out_sql << "\'\'";
								break;
						}
					}
					break;
				case zDBConnPool::DB_BIN2:
					{
						unsigned int size = *((DWORD *)(data+offset));
						size += sizeof(DWORD);
						//You must allocate the to buffer to be at least length*2+1 bytes long.
						char buffer[size * 2 + 1];
						mysql_real_escape_string(mysql, buffer,(char *)(data+offset),size);
						out_sql << "\'" << buffer << "\'";
					}
					break;
				case zDBConnPool::DB_ZIP2:
					{
						unsigned int size = *((DWORD *)(data+offset));
						size += sizeof(DWORD);
						size = (size < (unsigned int)temp->size ? size : (unsigned int)temp->size);
						uLong destLen = size * 120 / 100 + 12;
						Bytef destBuffer[destLen * sizeof(Bytef)];
						int retcode = compress(destBuffer, &destLen, (Bytef *)(data+offset),size);
						switch(retcode)
						{
							case Z_OK:
								{
									//You must allocate the to buffer to be at least length*2+1 bytes long.
									char buffer[destLen * 2 + 1];
									mysql_real_escape_string(mysql,buffer,(char *)destBuffer,destLen);
									out_sql << "\'" << buffer << "\'";
								}
								break; 
							case Z_MEM_ERROR:
								logger->error("(%s)Z_MEM_ERROR, NULL value instead.",__PRETTY_FUNCTION__);
								out_sql << "\'\'";
								break;
							case Z_BUF_ERROR:
								logger->error("(%s)Z_BUF_ERROR, NULL value instead.",__PRETTY_FUNCTION__);
								out_sql << "\'\'";
								break;
						}
					}
					break;
				default:
					logger->error("invalid zebra mysql type.");
					return (unsigned int)-1;
			}
		}
		if(temp->size==0)
			offset+=(*((DWORD *)(data+offset)) + sizeof(DWORD));
		else
			offset+=temp->size;
		temp++;
	}
	if(where!=NULL)
	{
		out_sql << " WHERE " << where;
	}
	//logger->debug("(%d)%s",strSql.length(),strSql.c_str());
	if (0 == execSql(out_sql.str().c_str(),out_sql.str().length()))
		return (unsigned int)mysql_affected_rows(mysql);
	else
		return (unsigned int)-1;
}


unsigned int MysqlClientHandle::exeUpdate(FieldSet* table, Record* data, Record* where)
{
	using namespace std;


	ostringstream query_string;
	ostringstream where_string;

	if (table==NULL || data==NULL || mysql==NULL)
	{
		logger->error("null pointer error. ---- %s",__PRETTY_FUNCTION__);
		return (unsigned int)-1;
	}

	unsigned int num_field = table->size();

	query_string << "UPDATE ";
	query_string << "`" << table->getTableName() << "`";
	query_string << " SET ";

	bool first=true;

	for (unsigned int i=0; i<num_field; i++)
	{
		const char *fieldvalue = NULL;

		Field* field = NULL;
		field = table->getField(i);

		if ((fieldvalue = (*data)[field->name]) == NULL)
		{
			continue;
		}

		if (first)
			first=false;
		else
		{
			query_string <<  " ,";
		}

		query_string << "`" << field->name << "`" << " = ";

		switch (field->type)
		{
			case FIELD_TYPE_STRING:
			case FIELD_TYPE_VAR_STRING:
			case FIELD_TYPE_DATE:
			case FIELD_TYPE_TIME:
			case FIELD_TYPE_DATETIME:
			case FIELD_TYPE_YEAR:
			case FIELD_TYPE_BLOB:
				{
					char strData[strlen(fieldvalue) * 2 + 1];
					mysql_real_escape_string(mysql, strData, fieldvalue, strlen(fieldvalue));
					query_string << "\'" << strData << "\'";
				}

				break;
			default:
				query_string << fieldvalue;
				break;
		}
	}

	first = true;

	if (where && where->size()>0)
	{
		for (unsigned int i=0; i<num_field; i++)
		{
			const char *fieldvalue = NULL;

			Field* field = NULL;
			field = table->getField(i);

			if ((fieldvalue = (*where)[field->name]) == NULL
					|| strlen((*where)[field->name])==0)
			{
				continue;
			}

			if (first)
			{
				query_string << " WHERE ";
				first=false;
			}
			else
			{
				query_string <<  " AND ";
			}

			//query_string << field->name;

			switch (field->type)
			{
				case FIELD_TYPE_STRING:
				case FIELD_TYPE_VAR_STRING:
				case FIELD_TYPE_DATE:
				case FIELD_TYPE_TIME:
				case FIELD_TYPE_DATETIME:
				case FIELD_TYPE_YEAR:
				case FIELD_TYPE_BLOB:
					{
						//char strData[strlen(fieldvalue) * 2 + 1];
						//mysql_real_escape_string(mysql, strData, fieldvalue, strlen(fieldvalue));
						query_string << fieldvalue;
					}

					break;
				default:
					query_string << fieldvalue;
					break;
			}
		}
	}

#ifdef _ZJW_DEBUG
	logger->debug("sql:%s", query_string.str().c_str());	
#endif	

	if (0 == execSql(query_string.str().c_str(), query_string.str().size()))
	{
		return (unsigned int)mysql_affected_rows(mysql);
	}
	else
	{
		return (unsigned int)-1;
	}
}

char * MysqlClientHandle::escapeString(const char *src,char *dest,unsigned int size)
{
	if(src==NULL || dest==NULL || mysql==NULL) return NULL;
	char *end=dest;
	mysql_real_escape_string(mysql, end,src,size==0?strlen(src):size);
	return dest;
}

std::string& MysqlClientHandle::escapeString(const std::string &src,std::string &dest)
{
	if(mysql==NULL) return dest;
	char buff[2 * src.length() + 1];
	bzero(buff, sizeof(buff));
	mysql_real_escape_string(mysql, buff,src.c_str(),src.length());
	dest = buff;
	return dest;
}

void MysqlClientHandle::updateDatatimeCol(const char* tableName, const char *colName)
{
	if (tableName
			&& strlen(tableName) > 0
			&& colName
			&& strlen(colName) > 0)
	{
		std::string sql = "UPDATE ";
		sql += tableName;
		sql += " SET ";
		sql += colName;
		sql += " = NOW()";
		execSql(sql.c_str(), sql.length());
	}
}

unsigned int MysqlClientHandle::getCount(const char* tableName, const char *where)
{
	dbCol mycountCol_define[]=
	{
		{"COUNT(*)",zDBConnPool::DB_DWORD,sizeof(DWORD)},
		{NULL, 0, 0}
	};
	DWORD count=0;
	unsigned int retval = 0;
	int i = exeSelectLimit(tableName, mycountCol_define, where, NULL ,1, (unsigned char *)(&count));
	if (1 == i)
		retval = count;
	else
		retval=(unsigned int)-1;
	return retval;
}


/* ************************* *
 * zDBConnPool类静态函数实现 *
 * ************************* */
zDBConnPool *zDBConnPool::newInstance(hashCodeFunc hashfunc)
{
	return new zMysqlDBConnPool(hashfunc);
}

void zDBConnPool::delInstance(zDBConnPool **delThisClass)
{
	if(*delThisClass==NULL) return;
	SAFE_DELETE(*delThisClass);
}

unsigned int zDBConnPool::getColSize(dbCol* column)
{
	unsigned int retval = 0;
	if(column==NULL) return retval;
	const dbCol *temp;
	temp = column; 
	while(temp->name)
	{
		retval += temp->size;
		temp++; 
	}
	return retval; 
}

const char *zDBConnPool::getTypeString(int type)
{ 
	char *retval = "DB_NONE";

	switch(type)
	{
		case DB_BYTE:
			retval = "DB_BYTE";
			break;  
		case DB_CHAR:
			retval = "DB_CHAR";
			break;  
		case DB_WORD:
			retval = "DB_WORD";
			break;  
		case DB_DWORD:
			retval = "DB_DWORD";
			break;  
		case DB_QWORD:
			retval = "DB_QWORD";
			break;
		case DB_STR:
			retval = "DB_STR";
			break;
		case DB_BIN:
			retval = "DB_BIN";
			break;
		case DB_ZIP:
			retval = "DB_ZIP";
			break;
		case DB_BIN2:
			retval = "DB_BIN2";
			break;
		case DB_ZIP2:
			retval = "DB_ZIP2";
			break;
	}

	return retval;
}

void zDBConnPool::dumpCol(const dbCol *column)
{
	const dbCol *temp;
	temp = column;
	while(temp->name)
	{
		logger->info("%s(%s)->%d", temp->name ,getTypeString(temp->type),temp->size);
		temp++;
	}
}
