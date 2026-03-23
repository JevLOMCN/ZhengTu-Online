/**
 * \file zMetaData.cpp
 * \version  $Id: zMetaData.cpp  $
 * \author  
 * \date 
 * \brief 表结构管理器及相关类的实现
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

#include "zMetaData.h"
//MetaData* MetaData::instance = NULL;
using namespace Zebra;


/**
  * \brief 解析函数
  *
  */
MetaData:: ~MetaData()
{
	TableMember it;
	for (it=tables.begin(); it!=tables.end(); it++)
	{
		FieldSet* temp = it->second;
		SAFE_DELETE(temp);
	}	
}

class MySQLMetaData : public MetaData
{
	/**
	  * \brief 初始化表结构
	  *
	  * 建立数据库连接，并取得该数据库中所有表的表结构
	  *  
	  * \param url:  数据库连接串
	  */
	bool init(const std::string& url)
	{
		UrlInfo urlInfo(0, url, false);	
		if (!this->loadMetaDataFromDB(urlInfo))
		{
			return false;
		}

		// TODO:其它一些需要初始化的代码写在这里
		return true;
	}	

	/**
	  * \brief 通过指定的连接，载入数据表结构
	  *
	  */
	bool loadMetaDataFromDB(const UrlInfo& url)
	{
		MYSQL* mysql_conn = NULL;
		MYSQL_RES* table_res = NULL;

		mysql_conn=mysql_init(NULL);

		if(mysql_conn==NULL)
		{
			logger->error("Initiate mysql error...");
			return false;
		}

		if(mysql_real_connect(mysql_conn,url.host,url.user,url.passwd,url.dbName,url.port,NULL,CLIENT_COMPRESS|CLIENT_INTERACTIVE)==NULL)
		{
			logger->error("loadMetaDataFromDB():connect mysql://%s:%u/%s failed...",url.host,url.port,url.dbName);
			logger->error("loadMetaDataFromDB():reason: %s",mysql_error(mysql_conn));
			return false;
		}

		logger->info("loadMetaDataFromDB():connect mysql://%s:%u/%s successful...",url.host,url.port,url.dbName);

		if (mysql_conn)
		{
			if ((table_res = mysql_list_tables(mysql_conn, NULL)) == NULL)
			{
				logger->error("loadMetaDataFromDB():mysql_list_tables fail.");
				logger->error("loadMetaDataFromDB():reason: %s",mysql_error(mysql_conn));
				if (table_res) mysql_free_result(table_res);
				if (mysql_conn) mysql_close(mysql_conn);
				return false;
			}
			MYSQL_ROW row;

			while ((row=mysql_fetch_row(table_res)))
			{
				this->addNewTable(mysql_conn, row[0]);
			}

			mysql_free_result(table_res);
		}

		if (mysql_conn) mysql_close(mysql_conn);

		return true;
	}
	
	/**
	  * \brief 加入一个新表
	  */
	bool addNewTable(MYSQL* mysql_conn, const char* tableName)
	{
		MYSQL_RES* field_res = NULL;

		/*char query_string[20+strlen(tableName)+1];
		bzero(query_string, sizeof(query_string));

		snprintf(query_string, sizeof(query_string), "SELECT * FROM  `%s`", tableName);

		//mysql_real_escape_string();
		if (mysql_real_query(mysql_conn, query_string, strlen(query_string)) != 0)
		{
			logger->error("查询%s失败", tableName);
			return false;
		}

		field_res = mysql_store_result(mysql_conn);
		*/
		field_res = mysql_list_fields(mysql_conn, tableName, NULL);

		if (field_res)
		{
			unsigned int num_fields = mysql_num_fields(field_res);
			MYSQL_FIELD* mysql_fields = NULL;
			mysql_fields = mysql_fetch_fields(field_res);

			FieldSet *fields = new FieldSet(tableName);

			if (!fields)
			{
				mysql_free_result(field_res);
				return false;
			}

			for (unsigned int i=0; i<num_fields; i++)
			{
				if (!fields->addField(mysql_fields[i].type, mysql_fields[i].name))
				{
					mysql_free_result(field_res);
					return false;
				}
			}

			tables.insert(valueType(tableName, fields));
			mysql_free_result(field_res);
		}
		else
		{
			return false;
		}

		return true;	
	}

};

/**
  * \brief 取字段个数
  *
  * \return 返回字段个数
  */	  
unsigned int FieldSet::size()
{
	return fields.size();
}

/**
  * \brief 重载operator[]运算符
  *
  * \param pos： 指定随机访问某个字段的位置 
  *
  * \return 如果找到该字段则返回该字段的指针，如果没找到，则返回NULL
  */
Field* FieldSet::operator[] (unsigned int pos)
{
	if (pos<0 || pos>=fields.size())
	{
		return NULL;
	}

	return fields[pos];
}

/**
  * \brief 重载operator[]运算符
  *
  * \param pos： 指定随机访问某个字段的名称
  *
  * \return 如果找到该字段则返回该字段的指针，如果没找到，则返回NULL
  */
Field* FieldSet::operator[](const std::string& name)
{
	for (unsigned int i=0; i<fields.size(); i++)
	{
		Field* ret = fields.at(i);
		
		if (ret)
		{
			if (ret->name == name)
			{
				return ret;
			}
		}
	}
	
	return NULL;
}


/**
  * \brief 加入新的字段
  *
  *  字段类型目前支持以下类型:
  *
  *  FIELD_TYPE_TINY TINYINT field
  *  FIELD_TYPE_SHORT SMALLINT field
  *  FIELD_TYPE_LONG INTEGER field
  *  FIELD_TYPE_INT24 MEDIUMINT field
  *  FIELD_TYPE_LONGLONG BIGINT field
  *  FIELD_TYPE_DECIMAL DECIMAL or NUMERIC field
  *  FIELD_TYPE_FLOAT FLOAT field
  *  FIELD_TYPE_DOUBLE DOUBLE or REAL field
  *  FIELD_TYPE_TIMESTAMP TIMESTAMP field
  *  FIELD_TYPE_DATE DATE field
  *  FIELD_TYPE_TIME TIME field
  *  FIELD_TYPE_DATETIME DATETIME field
  *  FIELD_TYPE_YEAR YEAR field
  *  FIELD_TYPE_STRING CHAR field
  *  FIELD_TYPE_VAR_STRING VARCHAR field
  *  FIELD_TYPE_BLOB BLOB or TEXT field 
  *  FIELD_TYPE_SET SET field
  *  FIELD_TYPE_ENUM ENUM field
  *  FIELD_TYPE_NULL NULL-type field
  *  FIELD_TYPE_CHAR Deprecated; use FIELD_TYPE_TINY instead
  *
  * \param fieldType: 字段类型
  * \param fieldName: 字段名称
  *
  *
  */
bool FieldSet::addField(int fieldType, const std::string& fieldName)
{
	std::string tempname = fieldName;

	transform(tempname.begin(), tempname.end(),
			tempname.begin(),
			toupper);

	Field* field = new Field(fieldType, tempname);

	if (field)
	{
		fields.push_back(field);
		return true;
	}

	return false;
}

/**
  * \brief 提供另一种添加字段的方法
  *
  *  重载addField
  */
bool FieldSet::addField(Field* field)
{
	if (!field)
	{
		fields.push_back(field);
		return true;
	}

	return false;
}

/**
  * \brief 解析函数
  *
  * 释放空间
  */
FieldSet::~FieldSet()
{
 	unsigned int num_field = fields.size(); 

	for (unsigned int i=0; i<num_field; i++)
	{
		SAFE_DELETE(fields[i]);
	}
   	
}
/**
  * \brief 通过指定表名，获取该表的表结构
  *
  * \param tableName: 表名
  *
  * \return 如果找到该表，返回表结构指针,否则，返回为空
  */
FieldSet* MetaData::getFields(const std::string& tableName)
{
	std::string tempname = tableName;

	//transform(tempname.begin(), tempname.end(),
	//		tempname.begin(),
	//		toupper);

	TableMember tm = tables.find(tempname);	
	if (tm!=tables.end())
	{
		return (FieldSet*)(tm->second);
	}
	
	return NULL;
}

/**
  * \brief 重载operator[]运算符
  *
  * 可通过指定字段名，获取其该字段的值。
  * 如果该字段类型为数值型，通过该函数也可返回其值，应用程序员需要自己调用相应函数进行转换
  * 或者显式调用与该类型匹配的get函数
  *
  * \param name: 字段名。不区分大小写
  * 
  * \return 如果该字段存在，则返回其值。如果不存在，则返回为NULL
  */
VarType Record::operator[](const std::string& name)
{
/*	std::string tempname = name;

	transform(tempname.begin(), tempname.end(),
			tempname.begin(),
			toupper);

	field_it it = field.find(tempname);
	
	
	if (it != field.end())
	{
		return it->second.c_str();
	}
	
	return NULL;*/
	return this->get(name);
}

/**
  * \brief 重载operator[]运算符
  *
  * 通过指定列的位置获取其值，不推荐在对位置有依赖的代码中使用，因为列的位置不一定是固定的。
  * 
  * \param idx: 指定的位置
  *
  * \return 如果指定的列有值，则返回其值，否则，返回为NULL
  */
const char* Record::operator[](unsigned int idx)
{
	field_it it;
	unsigned int i=0;

	for (it = field.begin(); it!=field.end(); i++, it++)
	{
		if (idx == i)
		{
			return it->second.c_str();	
		}
	}

	return NULL;
}

/**
  * \brief 添加列
  *
  * \param fieldName: 字段名称
  * \param value: 字段值
  * 
  */
void Record::put(const char* fieldName)
{
	if (fieldName == NULL)
	{
		return;
	}
	   
	std::string tempname = fieldName;

	transform(tempname.begin(), tempname.end(),
			tempname.begin(),
			toupper);
	
	
	field.insert(valType(tempname, ""));
	
}

/**
  * \brief 获取指定字段的值的通用方法
  * 
  * 可获得所有字段类型的值，皆以字符串的形式返回其值。
  * 如需按字段类型获得其值，请调用相应的get方法
  */
VarType Record::get(const std::string& fieldName)
{
	std::string tempname = fieldName;
	VarType ret;

	transform(tempname.begin(), tempname.end(),
			tempname.begin(),
			toupper);

	field_it it = field.find(tempname);

	if (it == field.end())
	{
		return ret;
	}
	
	if (fields)
	{
		Field* fl = (*fields)[tempname];

		if (fl)
		{
			ret.setValid(true);

			switch (fl->type)
			{
				case FIELD_TYPE_TINY:
				case FIELD_TYPE_SHORT:
				case FIELD_TYPE_LONG:
				case FIELD_TYPE_INT24:
				case FIELD_TYPE_LONGLONG:
				case FIELD_TYPE_DECIMAL:
					{// 所有整型在这里处理
						ret.val_us = atoi(it->second.c_str());
						ret.val_short = atoi(it->second.c_str());
						ret.val_int = atoi(it->second.c_str());
						ret.val_dword = atoi(it->second.c_str());
						ret.val_ul = strtoul(it->second.c_str(), NULL, 10);
						ret.val_qword = strtoull(it->second.c_str(), NULL, 10);
						ret.val_sqword = strtoll(it->second.c_str(), NULL, 10);
						ret.val_long = atol(it->second.c_str());
						ret.val_byte = atoi(it->second.c_str());
						break;
					}
				case FIELD_TYPE_FLOAT:
				case FIELD_TYPE_DOUBLE:
					{//所有浮点型在这里处理`
						ret.val_float = atof(it->second.c_str());
						ret.val_double = atof(it->second.c_str());
						break;
					}
				default:
					{// 其它所有类型按字符串处理
						ret.val_pstr = it->second.c_str();
					}
			}
		}
	}
	else
	{
		ret.setValid(true);
		ret.val_pstr = it->second.c_str();
	}

	return ret;
}

/**
 * \brief 判断某个字段是否有效 
 *
 * \param fieldName: 字段名称

 * \return 如果该记录包含该字段，返回TRUE,否则为FALSE
 */
bool Record::find(const std::string& fieldName)
{
	std::string tempname = fieldName;

	transform(tempname.begin(), tempname.end(),
			tempname.begin(),
			toupper);

	if (field.find(tempname) == field.end())
	{
		return false;
	}

	return true;

}

/**
 * \brief 解析方法
 */
RecordSet::~RecordSet()
{
//	unsigned int num_record = recordSet.size();
//	std::cout << "~RecordSet" << std::endl;
	for (std::vector<Record*>::iterator pos = recordSet.begin(); pos!=recordSet.end(); pos++)
	{
		SAFE_DELETE(*pos);
	}
}

/**
 * \brief 重载operator[]运算符
 *
 * 通过指定的行数，获取相应的记录
 *
 * \param idx:指定的行数
 *
 * \return 如果指定的行数有效，则返回相应的记录指针，如果无效，则返回NULL
 */
Record* RecordSet::operator[](unsigned int idx)
{
	return this->get(idx);
}

/**
 * \brief 获取记录数
 *
 * \return 返回记录数，如果没有记录，返回为0
 */
unsigned int RecordSet::size()
{
	return recordSet.size();
}

/**
 * \brief 添加记录
 *
 */
void RecordSet::put(Record* rec)
{
	recordSet.push_back(rec);
}

/**
 * \brief 获取指定的行
 *
 * 功能与重载的operator[]运算符相同。
 */
Record* RecordSet::get(unsigned int idx)
{
	return recordSet[idx];
}

/**
 * \brief builder方法，通过传入的类型描述，生成对应的实例
 *
 * \param 数据库类型，目前只支持MYSQL，传入时可以为空。也可以是"MYSQL"
 * 
 * \return  返回基类指针
 */
MetaData* MetaData::newInstance(const char* type)
{
	if (type == NULL || !strcmp(type, "MYSQL"))
		return (new MySQLMetaData());
	else 
		return (new MySQLMetaData());
}

