/**
 * \file
 * \version  $Id: NewMySqlTest.cpp $
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
#include "zMetaData.h"
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
MetaData*    metaData = NULL;

bool MySqlTestService::init()
{
	if (!zService::init())
		return false;

	Zebra::logger->debug(__PRETTY_FUNCTION__);

	 struct tm tv1;
	 struct tm tv2;

         time_t timValue = time(NULL);
         zRTime::getLocalTime(tv1, timValue);
	 Zebra::logger->debug("hour:%d min:%d(%d-%d-%d)", tv1.tm_hour, tv1.tm_min, 
			 tv1.tm_year+1900, tv1.tm_mon+1, tv1.tm_mday);

	 struct timeval tval;
	 struct timezone zone;

	 gettimeofday(&tval, &zone);
	 long sec = tval.tv_sec + abs(zone.tz_minuteswest)*60;
	 tv2 = *gmtime(&sec);

	 Zebra::logger->debug("mytest: hour:%d min:%d(%d-%d-%d)", tv2.tm_hour, tv2.tm_min, tv2.tm_year+1900, 
			 tv2.tm_mon+1, tv2.tm_mday);

	 

	dbConnPool = zDBConnPool::newInstance(NULL);
	if (NULL == dbConnPool)
		return false;
	if (!dbConnPool->putURL(0, "mysql://zebra1:zebra1@192.168.2.16:3306/zebra1", false))
		return false;

	metaData = MetaData::newInstance("");
	if (!metaData)
	{
		return false;	
	}

	if (!metaData->init("mysql://zebra1:zebra1@192.168.2.16:3306/zebra1"))
	{
		return false;
	}

	connHandleID handle = dbConnPool->getHandle();
	if ((connHandleID)-1 != handle)
	{
		//简历表个
		char *drop_sql = "DROP TABLE IF EXISTS MYTEST";
		dbConnPool->execSql(handle, drop_sql, strlen(drop_sql));

		char *create_sql = "CREATE TABLE `MYTEST` (\
			`ZIP_DATA` blob ,\
			`BIN_DATA` blob ,\
			`ZIP2_DATA` blob ,\
			`BIN2_DATA` blob ,\
			`T1` int(10) unsigned  auto_increment,\
			`T2` smallint(6) unsigned  default '0',\
			`T3` mediumint(9) unsigned  default '0',\
			`T4` int(11) unsigned default '0',\
			`t5` bigint(20) default '0',\
			`NAME` varchar(100)  default '',\
			 PRIMARY KEY  (`T1`) \
			) TYPE=MyISAM; ";
		dbConnPool->execSql(handle, create_sql, strlen(create_sql));

		//产生初始数据
		FieldSet* mytest = metaData->getFields("MYTEST");

		Record rec;

		Zebra::logger->error("DEBUGS");
		std::string namevalue = "INSERT";
		rec.put("name", namevalue);
		
		rec.put("t2", 2);
		rec.put("t3", 3);
		rec.put("t4", 4);
		rec.put("t5", 5);

		dbConnPool->exeInsert(handle, mytest, &rec);

		Zebra::logger->error("DEBUGE");
		//dbConnPool->exeInsert(handle, "MYTEST", col_define, (const unsigned char *)test_data);

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
	FieldSet* mytest = metaData->getFields("MYTEST");
	Record  rec, column, where;
	std::string* str_value = new std::string("INSERT");

	rec.put("name", static_cast<std::string>(*str_value));
	
	rec.put("t2", 2);
	rec.put("t3", 3);
	rec.put("t4", 4);
	rec.put("t5", 5);
	dbConnPool->exeInsert(handle, mytest, &rec);


	//查询数据并对比
	where.put("name", "name='INSERT'");
//	column.put("name");
	column.put("t5", " max(t5) t5");

	RecordSet* result=dbConnPool->exeSelect(handle, mytest, &column, NULL);
	static long long t5 = 0;	
	if (result)
	{
		Record* record = result->get(0);
		if (record)
		{

/*			if (strcmp((*record)["name"], "INSERT"))
			{
				Zebra::logger->warn("%s, %u", __FUNCTION__, __LINE__);
				equal_data = true;
			}*/

			t5 = (int)record->get("t5");
			Zebra::logger->debug("t5:%d", t5);
		}
	}

	rec.put("name", "UPDATE");

	dbConnPool->exeUpdate(handle, mytest, &rec, &where);

	//再删除刚才插入的数据
	where.put("name", "name='UPDATE'");
	dbConnPool->exeDelete(handle, mytest, &where);



	/*
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
}*/

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

	for (int i=0; i<20; i++)
	{
		Zebra::logger->debug("%d\n", zMisc::randBetween(1,1000));
	}

	return EXIT_SUCCESS;
}

