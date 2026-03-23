/**
 * \file
 * \version  $Id: MySqlTest.cpp  $
 * \author  
 * \date
 * \brief 测试类zService
 *
 * 
 */

#include <iostream>
#include <string>
#include <ext/numeric>

#include "zType.h"
#include "zDBConnPool.h"
#include "zService.h"
#include "zMisc.h"
#include "Zebra.h"
#include "zTime.h"

class MySqlTestService : public zService
{

	public:

		static MySqlTestService &getInstance()
		{
			if (NULL == instance)
				instance = new MySqlTestService("服务程序");

			return *instance;
		}

		/**
		 * \brief 释放类的唯一实例
		 *
		 */
		static void delInstance()
		{
			if (instance)
			{
				delete instance;
				instance = NULL;
			}
		}

	private:

		static MySqlTestService *instance;

		MySqlTestService(const std::string &name) : zService(name)
		{
			count = 0;
		}

		bool init();
		bool serviceCallback();
		void final();

		unsigned long long count;
		zTime start;
};

MySqlTestService *MySqlTestService::instance = NULL;

zDBConnPool *dbConnPool=NULL;

bool MySqlTestService::init()
{
	if (!zService::init())
		return false;

	Zebra::logger->debug(__PRETTY_FUNCTION__);

	dbConnPool = zDBConnPool::newInstance(NULL);
	if (NULL == dbConnPool)
		return false;

	if (!dbConnPool->putURL(0, "mysql://Zebra:Zebra@192.168.2.11:3306/Zebra", false))
		return false;


	connHandleID handle = dbConnPool->getHandle();
	if ((connHandleID)-1 != handle)
	{
		//简历表个
		char *drop_sql = "DROP TABLE IF EXISTS MYTEST";
		dbConnPool->execSql(handle, drop_sql, strlen(drop_sql));
		char *create_sql = "CREATE TABLE `MYTEST` (\
			`ZIP_DATA` blob NOT NULL,\
			`BIN_DATA` blob NOT NULL,\
			`ZIP2_DATA` blob NOT NULL,\
			`BIN2_DATA` blob NOT NULL,\
			`T1` tinyint(4) unsigned NOT NULL default '0',\
			`T2` smallint(6) unsigned NOT NULL default '0',\
			`T3` mediumint(9) unsigned NOT NULL default '0',\
			`T4` int(11) unsigned NOT NULL default '0',\
			`T5` bigint(20) unsigned NOT NULL default '0',\
			`NAME` varchar(100) NOT NULL default ''\
			) TYPE=MyISAM; ";
		dbConnPool->execSql(handle, create_sql, strlen(create_sql));

		//产生初始数据
		char test_data[32] = "UPDATE";
		static const dbCol col_define[]=
		{
			{"NAME",zDBConnPool::DB_STR,32},
			{NULL, 0, 0}
		};

		dbConnPool->exeInsert(handle, "MYTEST", col_define, (const unsigned char *)test_data);

		dbConnPool->putHandle(handle);
	}
	else
	{
		Zebra::logger->error("不能获取句柄");
		return false;
	}

	return true;
}

bool MySqlTestService::serviceCallback()
{
	connHandleID handle = dbConnPool->getHandle();

	if ((connHandleID)-1 == handle)
		return false;

	//生成测试数据
	static const dbCol col_define[]=
	{
		{"NAME",zDBConnPool::DB_STR,32},
		{"T1",zDBConnPool::DB_BYTE,sizeof(char)},
		{"T2",zDBConnPool::DB_CHAR,sizeof(BYTE)},
		{"T3",zDBConnPool::DB_WORD,sizeof(WORD)},
		{"T4",zDBConnPool::DB_DWORD,sizeof(DWORD)},
		{"T5",zDBConnPool::DB_QWORD,sizeof(QWORD)},
		{"BIN_DATA",zDBConnPool::DB_BIN,4096},
		{"ZIP_DATA",zDBConnPool::DB_ZIP,4096},
		{"BIN2_DATA",zDBConnPool::DB_BIN2,sizeof(DWORD)+4096},
		{"ZIP2_DATA",zDBConnPool::DB_ZIP2,sizeof(DWORD)+4096},
		{NULL, 0, 0}
	};
	union{
		struct {
			char name[32];
			char t1;
			BYTE t2;
			WORD t3;
			DWORD t4;
			QWORD t5;
			BYTE bin_data[4096];
			BYTE zip_data[4096];
			DWORD bin2_len;
			BYTE bin2_data[4096];
			DWORD zip2_len;
			BYTE zip2_data[4096];
		} s;
		char v[0];
	} __attribute__ ((packed))
	test_data, *test_data1 = NULL;

	{
		std::generate(test_data.v, test_data.v + sizeof(test_data), std::rand);
		strcpy(test_data.s.name, "INSERT");
		test_data.s.bin2_len = zMisc::randBetween(0, 4096);
		test_data.s.zip2_len = zMisc::randBetween(0, 4096);
		//std::copy(test_data.v, test_data.v + sizeof(test_data), std::ostream_iterator<int>(std::cout, " "));
		//std::cout << std::endl; 

		//插入数据
		dbConnPool->exeInsert(handle, "MYTEST", col_define, (const unsigned char *)&test_data);

		//查询数据并对比
		dbConnPool->exeSelect(handle, "MYTEST", col_define, "NAME='INSERT'",NULL,(unsigned char **)&test_data1);
		if (test_data1)
		{
			bool equal_data = false;
			if (strcmp(test_data.s.name, test_data1->s.name))
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.t1 != test_data1->s.t1)
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.t2 != test_data1->s.t2)
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.t3 != test_data1->s.t3)
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.t4 != test_data1->s.t4)
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.t5 != test_data1->s.t5)
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			for(int i = 0; i < 4096; i++)
			{
				if (test_data.s.bin_data[i] != test_data1->s.bin_data[i])
				{
					Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
					equal_data = true;
				}
			}
			for(int i = 0; i < 4096; i++)
			{
				if (test_data.s.zip_data[i] != test_data1->s.zip_data[i])
				{
					Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
					equal_data = true;
				}
			}
			if (test_data.s.bin2_len == test_data1->s.bin2_len)
			{
				for(DWORD i = 0; i < test_data.s.bin2_len; i++)
				{
					if (test_data.s.bin2_data[i] != test_data1->s.bin2_data[i])
					{
						Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
						equal_data = true;
					}
				}
			}
			else
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.zip2_len == test_data1->s.zip2_len)
			{
				for(DWORD i = 0; i < test_data.s.zip2_len; i++)
				{
					if (test_data.s.zip_data[i] != test_data1->s.zip_data[i])
					{
						Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
						equal_data = true;
					}
				}
			}
			else
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (equal_data)
			{
				std::cout << "插入数据再查询出来对比数据出错" << std::endl;
				std::copy(test_data.v + 32, test_data.v + sizeof(test_data), std::ostream_iterator<int>(std::cout, " "));
				std::cout << test_data.s.name << std::endl; 
				std::copy(test_data1->v + 32, test_data1->v + sizeof(test_data), std::ostream_iterator<int>(std::cout, " "));
				std::cout << test_data1->s.name << std::endl; 
				std::cout << std::endl; 
			}
			delete[] test_data1;
		}

		//再删除刚才插入的数据
		dbConnPool->exeDelete(handle, "MYTEST", "NAME='INSERT'");
	}

	{
		std::generate(test_data.v, test_data.v + sizeof(test_data), std::rand);
		strcpy(test_data.s.name, "UPDATE");
		test_data.s.bin2_len = zMisc::randBetween(0, 4096);
		test_data.s.zip2_len = zMisc::randBetween(0, 4096);
		//std::copy(test_data.v, test_data.v + sizeof(test_data), std::ostream_iterator<int>(std::cout, " "));
		//std::cout << std::endl; 

		//更新已有数据
		dbConnPool->exeUpdate(handle, "MYTEST", col_define, (const unsigned char *)&test_data, "NAME='UPDATE'");

		//查询数据并对比
		dbConnPool->exeSelect(handle, "MYTEST", col_define, "NAME='UPDATE'",NULL,(unsigned char **)&test_data1);
		if (test_data1)
		{
			bool equal_data = false;
			if (strcmp(test_data.s.name, test_data1->s.name))
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.t1 != test_data1->s.t1)
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.t2 != test_data1->s.t2)
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.t3 != test_data1->s.t3)
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.t4 != test_data1->s.t4)
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.t5 != test_data1->s.t5)
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			for(int i = 0; i < 4096; i++)
			{
				if (test_data.s.bin_data[i] != test_data1->s.bin_data[i])
				{
					Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
					equal_data = true;
				}
			}
			for(int i = 0; i < 4096; i++)
			{
				if (test_data.s.zip_data[i] != test_data1->s.zip_data[i])
				{
					Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
					equal_data = true;
				}
			}
			if (test_data.s.bin2_len == test_data1->s.bin2_len)
			{
				for(DWORD i = 0; i < test_data.s.bin2_len; i++)
				{
					if (test_data.s.bin2_data[i] != test_data1->s.bin2_data[i])
					{
						Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
						equal_data = true;
					}
				}
			}
			else
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (test_data.s.zip2_len == test_data1->s.zip2_len)
			{
				for(DWORD i = 0; i < test_data.s.zip2_len; i++)
				{
					if (test_data.s.zip2_data[i] != test_data1->s.zip2_data[i])
					{
						Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
						equal_data = true;
					}
				}
			}
			else
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}
			if (equal_data)
			{
				std::cout << "更新已有数据再查询出来对比数据出错" << std::endl;
				std::copy(test_data.v + 32, test_data.v + sizeof(test_data), std::ostream_iterator<int>(std::cout, " "));
				std::cout << test_data.s.name << std::endl; 
				std::copy(test_data1->v + 32, test_data1->v + sizeof(test_data), std::ostream_iterator<int>(std::cout, " "));
				std::cout << test_data1->s.name << std::endl; 
				std::cout << std::endl; 
			}
			delete[] test_data1;
		}
	}

	dbConnPool->putHandle(handle);

	count++;

	//zThread::sleep(1);
	return true;
}

void MySqlTestService::final()
{
	zDBConnPool::delInstance(&dbConnPool);
	Zebra::logger->debug("%s", __PRETTY_FUNCTION__);
	Zebra::logger->debug("total count: %llu", count);
	Zebra::logger->debug("time elapsed: %llu", start.elapse());
}

int main(int argc, char **argv)
{
	Zebra::logger=new zLogger();

	MySqlTestService::getInstance().main();
	MySqlTestService::delInstance();

	return EXIT_SUCCESS;
}

