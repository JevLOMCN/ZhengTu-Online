#ifndef _ZDBCONNPOOL_H_
#define _ZDBCONNPOOL_H_

#include "zType.h"
#include <string.h>
#define MAX_HOSTSIZE	32
#define MAX_USERSIZE	32
#define MAX_DBSIZE		32

/**
 *
 * \brief 数据库字段描述结构类型定义
 * 
 * 本结构描述了要操作的数据的数据库类型字段,描述的数组必须以{NULL,0，0}作为描述结束标记。
 *
 * 注意：如果type是DB_BIN2或者DB_ZIP2，那么大小必须是缓冲的最大大小。
 * 
 * 例子：
 *
 *	dbCol mycountCol_define[]=
 *
 *	{
 *
 *		{"COUNT(*)",DB_DWORD,sizeof(DWORD)},
 *
 *		{NULL, 0, 0}
 *
 *	};
 */
typedef struct
{
	const char *name;	/**< 字段名字 */
	int type;			/**< ZEBRA数据类型 */
	unsigned int size;	/**< 数据大小 */
} dbCol;

class Record;
class FieldSet;
class RecordSet;
/**
 * \brief 哈希代码函数类型定义
 * 
 * 用户可以根据自己的需要写自己的哈希函数，以便对相对应用户定义的数据库进行操作。
 */
typedef unsigned int(* hashCodeFunc)(const void *data);

/**
 * \brief 连接句柄,用户调用使用,只能从链接池中得到
 */
typedef unsigned int connHandleID;

/**
 * \brief 数据链接池接口定义
 *
 * 本类提供了对数据库的简单的基本访问,比如UPDATE,INSERT,SELECT,DELETE,执行SQL语句等.
 *
 *  用户只需要定义要操作的数据库数据，即可访问。
 *
 * 如果选用不同数据库,必须实现这个接口,目前提供了Mysql的实现.
 */
class zDBConnPool
{
	public:
		/**
		 * \brief 数据库支持的数据类型
		 */
		enum
		{
			DB_BYTE,		/**< BYTE类型 1字节长度 */
			DB_CHAR,		/**< CHAR类型 1字节长度 */
			DB_WORD,		/**< WORD类型 2字节长度 */
			DB_DWORD,		/**< DWORD类型 4字节长度 */
			DB_QWORD,		/**< QWORD类型 8字节长度 */
			DB_STR,			/**< 字符串类型 */
			DB_BIN,			/**< 二进制数据类型 */
			DB_ZIP,			/**< zip压缩数据类型 */
			DB_BIN2,		/**< 扩展二进制数据类型 */
			DB_ZIP2			/**< 扩展zip压缩数据类型 */
		};

		/**
		 * \brief 新建立一个链接池的实例
		 *
		 * 本接口没有默认的构造函数，为了使用者无需关心底层数据库的差异。提供此接口。
		 * \param hashfunc 哈希函数指针，如果为NULL，接口实现者应该提供默认函数。
		 * \return 返回这个接口的一个实例，错误返回NULL。
		 */
		static zDBConnPool *newInstance(hashCodeFunc hashfunc);

		/**
		 * \brief 回收一个链接池的实例
		 *
		 * 本接口没有默认的析构函数，为了使用者无需关心底层数据库的差异。提供此接口。
		 * \param delThisClass 要回收的链接池实例。
		 */
		static void delInstance(zDBConnPool **delThisClass);

		/**
		 * \brief 根据数据字段描述计算数据字段的大小
		 *
		 * \param column 数据字段描述指针。
		 * \return 数据字段大小。
		 */
		static unsigned int getColSize(dbCol* column);

		/**
		 * \brief 得到数据字段的类型字符串 
		 * \param type 类型
		 * \return 类型字符串
		 */
		static const char *getTypeString(int type);

		/**
		 * \brief 打印出数据类型定义描述  
		 * \param column 数据定义指针
		 */
		static void dumpCol(const dbCol *column);

		/**
		 * \brief 接口析构虚函数
		 */
		virtual ~zDBConnPool(){};

		/**
		 * \brief 向连接池中添加数据库连接URL，并设置此连接是否支持事务
		 *
		 * \param hashcode 此连接所对应的哈希代码，使用者需要指定。
		 * \param url 数据库联接的url
		 * \param supportTransactions 此连接是否只是事务
		 * \return 成功返回true，否则返回false
		 */
		virtual bool putURL(unsigned int hashcode,const char *url,bool supportTransactions) =0;

		/**
		 * \brief 根据hashData得到连接Handle
		 * 
		 * \param hashData 链接池用此参数作为调用hashCodeFunc的参数，计算hashcode，用来得到相应的数据库联接。
		 * \return 数据库联接句柄,-1表示无效句柄
		 */
		virtual connHandleID getHandle(const void *hashData=NULL) =0;

		/**
		 * \brief 根据当前Handle得到下一个Handle用来遍历所有不同URL的db连接
		 *
		 * \param handleID 当前的链接句柄
		 * \return 下一个链接句柄，-1表示没有不同连接句柄了
		 */
		virtual connHandleID getNextHandle(connHandleID handleID) =0;

		/**
		 * \brief 将Handle放回连接池
		 *
		 * 用户在使用完数据库联接句柄后，应该将其放回链接池，已备下次使用。
		 * \param handleID 放回链接池的链接句柄
		 */
		virtual void putHandle(connHandleID handleID) =0;

		/**
		 * \brief 执行Sql语句,返回db_real_query的返回结果
		 *
		 * 为了提供更灵活的数据库操作，提供了本函数
		 * \param handleID 操作的链接句柄
		 * \param sql 要执行的SQL语句
		 * \param sqllen SQL语句的长度
		 * \return 返回执行数据语句后的代码，根具体的数据库的返回值有关
		 */
		virtual int execSql(connHandleID handleID, const char *sql,unsigned int sqllen) =0;

		/**
		 * \brief 执行SELECT SQL
		 *
		 * \param handleID 操作的链接句柄
		 * \param tableName 要操作的表名
		 * \param column 要操作的数据字段描述，以{NULL,0,0}为结尾标记
		 * \param where SQL的where表达式,没有时用NULL
		 * \param order SQL的order表达式,没有时用NULL
		 * \param data SELECT后的结果数据存储的位置，如果返回值大于0，调用者应该释放*data内存空间
		 * \return 返回值为结果的个数，如果错误返回-1
		 */
		virtual unsigned int exeSelect(connHandleID handleID, const char* tableName,
				const dbCol *column, const char *where,const char *order ,unsigned char **data) =0;
		
		/**
		 * \brief 执行SELECT SQL
		 *
		 *  使用方法可参见test/NewMySQLTest中的使用代码例程 
		 *
		 * \param handleID 操作的链接句柄
		 * \param table 要操作的表结构对象，通过MetaData.getFields取得
		 * \param column 要操作的数据字段描述，不指定时为返回所有字段"*" 
		 * \param where SQL的where描述,没有时用NULL
		 * \param order SQL的order描述,没有时用NULL，可不填写
		 * \param limit 返回结果的最大限制，为0时，为不限制，或可不填写
		 * \param groupby SQL中的GROUPBY子句描述，未有时，可不填写。也可填为NULL
		 * \param having SQL中的HAVING子句描述，未有时，可不填写，也可填为NULL
		 *
		 * \return 返回结果集
		 *
		 * \author zjw
		 */

		virtual RecordSet* exeSelect(connHandleID handleID, FieldSet* table, Record* column,
				Record* where, Record* order = NULL, 
				unsigned int limit=0,
				Record* groupby = NULL, Record* having = NULL) = 0;
		
		/*virtual unsigned int  exeSelect(connHandleID handleID, RecordSet* result, FieldSet* table, Record* column,
				Record* where, Record* order = NULL, 
				unsigned int limit=0,
				Record* groupby = NULL, Record* having = NULL) = 0;*/


		
		/**
		 * \brief 执行SELECT SQL,并限制返回结果的个数
		 * 
		 * \param handleID 操作的链接句柄
		 * \param tableName 要操作的表名
		 * \param column 要操作的数据字段描述，以{NULL,0,0}为结尾标记
		 * \param where SQL的where表达式,没有时用NULL
		 * \param order SQL的order表达式,没有时用NULL
		 * \param limit 返回结果的最大限制
		 * \param data SELECT后的结果数据存储的位置,data应该有足够的空间存储返回的结果
		 * \return 返回值为结果的个数，如果错误返回-1
		 */
		virtual unsigned int exeSelectLimit(connHandleID handleID, const char* tableName,
				const dbCol *column, const char *where,const char *order ,unsigned int limit,unsigned char *data ,unsigned int limit_from = 0) =0;

		/**
		 * \brief 执行SELECT SQL,并限制返回结果的个数
		 * 
		 * \param handleID 操作的链接句柄
		 * \param sql 标准查询sql语句
		 * \param sqlen sql长度
		 * \param cloumn 需尧返回的列结构
		 * \param limit 返回结果的最大限制
		 * \param data SELECT后的结果数据存储的位置,data应该有足够的空间存储返回的结果
		 * \return 返回值为结果的个数，如果错误返回-1
		 */
		virtual unsigned int execSelectSql(connHandleID handleID, const char *sql,
				unsigned int sqllen ,const dbCol *column,unsigned int limit,unsigned char *data)=0;
		/**
		 * \brief 将data添加进数据库
		 *
		 * 本函数保证是原子操作
		 *
		 * \param handleID 操作的链接句柄
		 * \param tableName 要操作的表名
		 * \param column 要操作的数据字段描述，以{NULL,0,0}为结尾标记
		 * \param data 要操作的数据字段
		 * \return 插入数据的描述返回值为插入语句执行后，为AUTO_INCREMENT column所产生的ID,如果为-1表示函数错误
		 * 显然如果本次插入没有AUTO_INCREMENT column,返回值大于等于0是没有意义的
		 */
		virtual unsigned int exeInsert(connHandleID handleID, const char *tableName,const dbCol *column,const unsigned char *data) =0;

		/**
		 * \brief 插入一条记录
		 *
		 * 本函数保证是原子操作
		 *
		 * \param handleID 操作的链接句柄
		 * \param table 要操作的表结构，通过MetaData::getFields获得
		 * \param rec 要操作的数据字段
		 *
		 * \return 插入数据的描述返回值为插入语句执行后，为AUTO_INCREMENT column所产生的ID,如果为-1表示函数错误
		 * 显然如果本次插入没有AUTO_INCREMENT column,返回值大于等于0是没有意义的
		 */
		virtual unsigned int exeInsert(connHandleID handleID, FieldSet* table, Record* rec) = 0;
		
		/**
		 * \brief 执行删除操作
		 *
		 * \param handleID 操作的链接句柄
		 * \param tableName 要操作的表名
		 * \param where 删除的条件
		 * \return 返回受影响的记录数,返回-1表示有错误发生
		 */
		virtual unsigned int exeDelete(connHandleID handleID, const char *tableName, const char *where) =0;
		
		/**
		 * \brief 执行删除操作
		 *
		 * \param handleID 操作的链接句柄
		 * \param table 要操作的表结构，通过MetaData::getFields获得
		 * \param where 删除的条件
		 *
		 * \return 返回受影响的记录数,返回-1表示有错误发生
		 */
		virtual unsigned int exeDelete(connHandleID handleID, FieldSet* table, Record* where) = 0;

		/**
		 * \brief 更新数据
		 * \param handleID 操作的链接句柄
		 * \param tableName 要操作的表名
		 * \param column 要操作的数据字段描述，以{NULL,0,0}为结尾标记
		 * \param data 要操作的数据字段
		 * \param where 更新条件
		 * \return 返回受影响的记录数,返回-1表示有错误发生
		 */
		virtual unsigned int exeUpdate(connHandleID handleID, const char *tableName,const dbCol *column,const unsigned char *data, const char *where) =0;
		
		/**
		 * \brief 更新数据
		 * \param handleID 操作的链接句柄
		 * \param table 要操作的表结构，通过MetaData::getFields获得
		 * \param data  要更新的字段及值
		 * \param where 更新条件描述
		 *
		 * \return 返回受影响的记录数,返回-1表示有错误发生
		 */
		virtual unsigned int exeUpdate(connHandleID handleID, FieldSet* table, Record* data, Record* where) = 0;

		/**
		 * \brief 转化字符串为有效的db字符串
		 * \param handleID 操作的链接句柄
		 * \param src 操作源数据
		 * \param dest 转换后字符串所存放的空间,为了程序的安全你应该为dest分配(size==0?strlen(src):size)*2+1的空间
		 * \param size 如果size>0,表示转化指定长度的字符串，用于二进制数据的转化，如果为0表示一般字符串的转户
		 * \return 失败返回NULL,成功返回dest
		 */
		virtual char * escapeString(connHandleID handleID, const char *src,char *dest,unsigned int size) =0;

		/**
		 * \brief 转化字符串为有效的db字符串
		 * \param handleID 操作的链接句柄
		 * \param src 操作源数据
		 * \param dest 转换后字符串所存放的空间,为了程序的安全你应该为dest分配(size==0?strlen(src):size)*2+1的空间
		 * \param size 如果size>0,表示转化指定长度的字符串，用于二进制数据的转化，如果为0表示一般字符串的转户
		 * \return 失败返回NULL,成功返回dest
		 */
		virtual std::string& escapeString(connHandleID handleID, const std::string &src, std::string &dest) =0;

		/**
		 * \brief 获取表中记录个数
		 * \param handleID 操作的链接句柄
		 * \param tableName 要操作的表名
		 * \param where 计数条件
		 * \return 返回计数结果
		 */
		virtual unsigned int getCount(connHandleID handleID, const char* tableName, const char *where) =0;

		/**
		 * \brief 把表中某个时间字段更新到最新时间
		 * \param handleID 操作的链接句柄
		 * \param tableName 要操作的表名
		 * \param colName 要操作的字段名
		 */
		virtual void updateDatatimeCol(connHandleID handleID, const char* tableName, const char *colName) =0;

		/**
		 * \brief 事务提交
		 * \param handleID 操作的链接句柄
		 * \return 成功返回true，失败返回false
		 */
		virtual bool commit(connHandleID handleID) =0;


		/**
		 * \brief 事务回滚
		 * \param handleID 操作的链接句柄
		 * \return 成功返回true，失败返回false
		 */
		virtual bool rollback(connHandleID handleID) =0;

		/**
		 * \brief 设置此链接是否支持事务
		 * \param handleID 操作的链接句柄
		 * \param supportTransactions  是否支持事务
		 * \return 成功返回true，失败返回false
		 */
		virtual bool setTransactions(connHandleID handleID, bool supportTransactions) =0;

		/**
		 * \brief 检查此链接是否支持事务
		 * \param handleID 操作的链接句柄
		 * \return 支持返回true，否则返回false
		 */
		virtual bool supportTransactions(connHandleID handleID) =0;
};

struct UrlInfo
{
	const unsigned int hashcode;
	const std::string url;
	const bool supportTransactions;

	char host[MAX_HOSTSIZE];
	char user[MAX_USERSIZE];
	char passwd[MAX_PASSWORD];
	unsigned int port;
	char dbName[MAX_DBSIZE];

	UrlInfo()
		: hashcode(0),url(),supportTransactions(false) {};
	UrlInfo(const unsigned int hashcode,const std::string &url,const bool supportTransactions)
		: hashcode(hashcode),url(url),supportTransactions(supportTransactions)
		{
			parseMySQLURLString();
		}
	UrlInfo(const UrlInfo &ui)
		: hashcode(ui.hashcode),url(ui.url),supportTransactions(ui.supportTransactions)
		{
			parseMySQLURLString();
		}

	private:
	void parseMySQLURLString()
	{
		bzero(host,sizeof(host));
		bzero(user,sizeof(user));
		bzero(passwd,sizeof(passwd));
		port=3306;
		bzero(dbName,sizeof(dbName));

		char strPort[16] = "";
		int  j, k;
		size_t i;
		const char *connString = url.c_str();
		if (0 == strncmp(connString, "mysql://", strlen("mysql://")))
		{
			i = 0; j = 0; k = 0;
			for(i = strlen("mysql://"); i < strlen(connString) + 1; i++)
			{
				switch(j)
				{
					case 0:
						if (connString[i] == ':')
						{
							user[k] = '\0'; j++; k = 0;
						}
						else
							user[k++] = connString[i];
						break;
					case 1:
						if (connString[i] == '@')
						{
							passwd[k] = '\0'; j++; k = 0;
						}
						else
							passwd[k++] = connString[i];
						break;
					case 2:
						if (connString[i] == ':')
						{
							host[k] = '\0'; j++; k = 0;
						}
						else
							host[k++] = connString[i];
						break;
					case 3:
						if (connString[i] == '/')
						{
							strPort[k] = '\0'; j++; k = 0;
						}
						else
							strPort[k++] = connString[i];
						break;
					case 4:
						if (connString[i] == '\0')
						{
							dbName[k] = '\0'; j++; k = 0;
						}
						else
							dbName[k++] = connString[i];
						break;
					default:
						break;
				}
			}
		}
		port=atoi(strPort);
	}
};

#endif
