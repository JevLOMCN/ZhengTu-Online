/**
 * \file zMetaData.h
 * \version  $Id: zMetaData.h  $
 * \author  
 * \date 
 * \brief 数据库表结构管理器及相关类的定义
 *
 * 
 */

#ifndef _METADATA_H_
#define _METADATA_H_
#include <mysql.h>
#include <pthread.h>
#include <unistd.h>
#include <zlib.h>
#include <iostream>
#include <ext/hash_map>
#include <sstream>
#include <string>
#include <map>
#include "zDBConnPool.h"


/**
  * \brief 字段信息类
  *
  * 维护字段名称及类型，目前系统只能处理MYSQL的所有标准类型，
  *
  * 还不支持Zebra中定义的ZIP,ZIP2两种类型
  * 如需支持更多的字段类型，请修改zMysqlDBConnPool::exeSelect, exeUpdate,exeInsert,exeDelete四个方法
  *
  */
  
class Field
{
	public:
		/**
		  * \brief 构造方法
		  *
		  *
		  */
		Field(int fieldType, const std::string& fieldName) 
			: type(fieldType), name(fieldName)
		{
		}
		
		/**
		  * \ brief 解析方法
		  *
		  */
		~Field(){}

		/// 字段类型
		int type;

		/// 字段名称
		std::string name;
};


/**
  * \brief 字段容器
  *
  * 维护一个表的所有字段
  *
  */
class FieldSet
{
	public:
		/**
		  * \brief 默认构造方法
		  *
		  */
	       	FieldSet(){}

		
		/**
		  * \brief 初始化表名的构造方法
		  *
		  * 不支持显式类型转换
		  */
		explicit FieldSet(const std::string& tbn) : tableName(tbn)
		{
		}
		

		/**
		  * \brief 解析函数
		  *
		  */
		virtual ~FieldSet();

		/**
	  	  * \brief 取字段个数
		  *
		  * \return 返回字段个数
	          */	  
		unsigned int size();
		

		/**
		  * \brief 重载operator[]运算符
		  *
		  * \param pos： 指定随机访问某个字段的位置 
		  *
		  * \return 如果找到该字段则返回该字段的指针，如果没找到，则返回NULL
		  */
		Field* operator[](unsigned int pos);


		/**
		  * \brief 重载operator[]运算符
		  *
		  * \param name： 指定随机访问某个字段的名称
		  *
		  * \return 如果找到该字段则返回该字段的指针，如果没找到，则返回NULL
		  */
		Field* operator[](const std::string& name);


	
		/**
		  * \brief 取指定位置的字段的方法
		  *
		  * \param pos: 指定随机访问某个字段的位置
		  *
		  * \return 如果找到该字段则返回该字段的指针，如果没找到，则返回NULL
		  */
		Field* getField(unsigned int pos)
		{
			return fields[pos];
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
		bool addField(int fieldType, const std::string& fieldName);

	        /**
		  * \brief 提供另一种添加字段的方法
		  *
		  *  重载addField
		  */
		bool addField(Field* field);

		/**
		  * \brief 设置该字段集与之关联的表名
		  *
		  * 如通过默认构造函数生成的对象，则必须显式调用该函数进行设置
		  *
		  * \param name：表名
		  */
		void setTableName(const std::string& name);
		
		/**
		  * \brief 得到表名
		  *
		  * \return 返回表名
		  */
		const char* getTableName() const
		{
			return tableName.c_str();	
		}
	 	
	protected:
		/// 字段容器
		std::vector<Field*> fields;

		/// 表名
		std::string tableName;	
};


/**
  * \brief 表结构管理器
  *
  * 该类采用Builder模式，以支持不同数据库的表结构管理
  *
  */
class MetaData
{
	public:
		/**
		  * \brief builder方法，通过传入的类型描述，生成对应的实例
		  *
		  * \param type 数据库类型，目前只支持MYSQL，传入时可以为空。也可以是"MYSQL"
		  * 
		  * \return  返回基类指针
		  */
		static MetaData* newInstance(const char* type);
		
		/**
		  * \brief 默认构造函数
		  *
		  */
		MetaData()
		{
		}

		/**
		  * \brief 解析函数
		  *
		  */
		virtual ~MetaData();
		
		/**
		  * \brief 初始化表结构
		  *
		  * 建立数据库连接，并取得该数据库中所有表的表结构
		  *  
		  * \param url:  数据库连接串
		  */
		virtual bool init(const std::string& url) = 0;
		
		/**
		  * \brief 通过指定表名，获取该表的表结构
		  *
		  * \param tableName: 表名
		  *
		  * \return 如果找到该表，返回表结构指针,否则，返回为空
		  */
		FieldSet* getFields(const std::string& tableName);

	protected:
		
		typedef std::map<std::string, FieldSet*> TableManager;
		typedef TableManager::iterator TableMember;
		typedef TableManager::value_type valueType;

		/// 内部表结构管理器
		TableManager tables;
};


class VarType
{
	public:
		VarType()
		{
			val_us = 0;
			val_ul = 0;
			val_short = 0;
			val_int = 0;
			val_dword = 0;
			val_qword = 0;
			val_sqword = 0;
			val_long = 0;
			
			val_float = 0.0;
			val_double = 0.0;
			val_byte = 0;
		
		        val_pstr = NULL;
			valid = false;
		}
		
		operator unsigned short() const
		{
			return val_us;
		}

		operator short() const
		{
			return val_short;
		}
		
		operator int() const
		{
			return val_int;
		}

		operator unsigned int() const
		{
			return val_dword;
		}

		operator unsigned long() const
		{
			return val_ul;
		}

		operator unsigned long long() const
		{
			return val_qword;
		}

		operator long long() const
		{
			return val_sqword;
		}

		operator long() const
		{
			return val_long;
		}

		operator float() const
		{
			return val_float;
		}

		operator double() const
		{
			return val_double;
		}


		operator const char*() const
		{
			return val_pstr;
		}
		
		operator unsigned char() const
		{
			return val_byte;
		}

		void setValid(bool value)
		{
			valid = value;
		}

		bool isValid()
		{
			return valid;
		}
	
		unsigned short val_us;
		short val_short;
		int val_int;
		unsigned int val_dword;
		unsigned long val_ul;
		unsigned long long val_qword;
		long long val_sqword;
		long val_long;
		float val_float;
		double val_double;
		unsigned char val_byte;
		
	        const char* val_pstr;
		//const unsigned char* val_ustr;

		bool valid;
};
/**
  * \brief 记录类
  *
  * 维护一条数据库记录
  */
class Record
{
	public:
		/**
		  * \brief 默认构造方法
		  */
		Record()
		{
//			std::cout << "Record" << std::endl;
			fields = NULL;
		}

		/**
		  * \brief 解析方法
		  */
		virtual ~Record()
		{
//			std::cout << "~Record" << std::endl;
			field.clear();
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
		VarType operator[](const std::string& name);

		

		/**
		  * \brief 重载operator[]运算符
		  *
		  * 通过指定列的位置获取其值，不推荐在对位置有依赖的代码中使用，因为列的位置不一定是固定的。
		  * 
		  * \param idx: 指定的位置
		  *
		  * \return 如果指定的列有值，则返回其值，否则，返回为NULL
		  */
		const char* operator[](unsigned int idx);
	  
		/**
		  * \brief 添加列
		  *
		  * 注意：第二个参数，绝不允许为NULL值，否则会导致程序崩溃
		  *
		  * \param fieldName: 字段名称
		  * \param value: 字段值
		  * 
		  */
		template <typename X>
		void put(const char* fieldName, const X& value, bool binary = false)
		{
			if (fieldName == NULL)
			{
				return;
			}

			std::ostringstream oss;
			std::string tempname = fieldName;
			
			std::transform(tempname.begin(), tempname.end(), 
				tempname.begin(),
				::toupper);

			if (binary)
			{
				
			}
			else
			{
				oss << value;
			}

			field_it member = field.find(tempname);

			if (member==field.end())
			{
				field.insert(valType(tempname, oss.str()));
			}
			else
			{
				field.erase(member);
				field.insert(valType(tempname, oss.str()));
			}

		}
		
		/**
		  * \brief 添加列
		  *
		  * 注意：第二个参数，绝不允许为NULL值，否则会导致程序崩溃
		  *
		  * \param fieldName: 字段名称
		  * \param value: 字段值
		  * 
		  */
		/*template <> void put<const char*>(const char* fieldName, const char* value)
		{
			if (fieldName == NULL)
			{
				return;
			}

			std::ostringstream oss;
			std::string tempname = fieldName;
			
			std::transform(tempname.begin(), tempname.end(), 
				tempname.begin(),
				::toupper);

			if (value)
			{
				oss << value;
			}

			field_it member = field.find(tempname);

			if (member==field.end())
			{
				field.insert(valType(tempname, oss.str()));
			}
			else
			{
				field.erase(member);
				field.insert(valType(tempname, oss.str()));
			}

		}*/

		/**
		  * \brief 添加列
		  *
		  *  主要用于当这个Record用做,SELECT时的column,groupby子句。
		  *  添加一个列，但这个列的值为空
		  *
		  * \param fieldName: 字段名称
		  * 
		  */
		void put(const char* fieldName);

		/**
		  * \brief 清空所有列
		  *
		  */
		void clear()
		{
			field.clear();
		}

		/**
		  * \brief 获取指定字段的值的通用方法
		  * 
		  * 可获得所有字段类型的值，皆以字符串的形式返回其值。
		  * 如需按字段类型获得其值，请调用相应的get方法
		  */
		VarType get(const std::string& fieldName);
		

		//bool getBool(const std::string& fieldName);
		//double getDouble(const std::string& fieldName);
		//int    getInt(const std::string& fieldName);

		/**
		  * \brief 判断某个字段是否有效 
		  *
		  * \param fieldName: 字段名称

		  * \return 如果该记录包含该字段，返回TRUE,否则为FALSE
		  */
		bool find(const std::string& fieldName);
		
		/**
		  * \brief 获取该记录的列数
		  *
		  * \return 返回该记录的列数，为0表示没有列。
		  */
		unsigned int size()
		{
			return field.size();
		}


		FieldSet* fields;

	private:
		typedef std::map<std::string, std::string> FIELD;
		typedef FIELD::value_type valType;
		typedef FIELD::iterator field_it;

		/// 字段-值对
		FIELD field;
};


class RecordSet
{
	public:
		/**
		  * \brief 默认构造函数
		  *
		  */
		RecordSet()
		{
//			std::cout << "RecordSet" << std::endl;
		}

		/**
		  * \brief 解析函数
		  *
		  */
		virtual ~RecordSet();
		
		/**
		  * \brief 重载operator[]运算符
		  *
		  * 通过指定的行数，获取相应的记录
		  *
		  * \param idx:指定的行数
		  *
		  * \return 如果指定的行数有效，则返回相应的记录指针，如果无效，则返回NULL
		  */
		Record* operator[](unsigned int idx);
		
		/**
		  * \brief 获取记录数
		  *
		  * \return 返回记录数，如果没有记录，返回为0
		  */
		unsigned int size();

		/**
		  * \brief 获取记录数
		  *
		  * \return 返回记录数，如果没有记录，返回为0
		  */
		bool empty(){return recordSet.empty();}

		/**
		  * \brief 添加记录
		  *
		  */
		void put(Record* rec);


		/**
		  * \brief 清空所有记录
		  *
		  */
		void clear()
		{
			recordSet.clear();
		}
		
		/**
		  * \brief 获取指定的行
		  *
		  * 功能与重载的operator[]运算符相同。
		  */
		Record* get(unsigned int idx);

	private:
		/// 记录集
		std::vector<Record*>    recordSet;
};

#endif

