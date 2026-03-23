/**
 * \file
 * \version  $Id: zProperties.h  $
 * \author  
 * \date 
 * \brief 定义zProperties属性关联类
 *
 * 
 */


#ifndef _zProperties_h_
#define _zProperties_h_

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <ext/hash_map>
#include <string.h>

#include "zString.h"

/**
 * \brief 属性关联类容器，所有属性关键字和值都使用字符串代表，关键字不区分大小写
 *
 */
class zProperties
{

	public:

		/**
		 * \brief 获取一个属性值
		 *
		 * \param key 关键字
		 * \return 返回与关键字对应的属性值
		 */
		const std::string &getProperty(const std::string &key)
		{
			return properties[key];
		}

		/**
		 * \brief 设置一个属性
		 *
		 * \param key 关键字
		 * \param value 关键字对应的属性
		 */
		void setProperty(const std::string &key, const std::string &value)
		{
			properties[key] = value;
		}

		/**
		 * \brief 重载操作符，返回与关键字对应的属性值
		 *
		 * \param key 关键字
		 * \return 属性值
		 */
		std::string & operator[] (const std::string &key)
		{
			return properties[key];
		}

		/**
		 * \brief 输出存储的所有属性值
		 *
		 */
		void dump(std::ostream &out)
		{
			property_hashtype::const_iterator it;
			for(it = properties.begin(); it != properties.end(); it++)
				out << it->first << " = " << it->second << std::endl;
		}

		unsigned int parseCmdLine(const std::string &cmdLine);
		unsigned int parseCmdLine(const char *cmdLine);

	protected:

		/**
		 * \brief hash函数
		 *
		 */
		struct key_hash : public std::unary_function<const std::string, size_t>
		{
			size_t operator()(const std::string &x) const
			{
				std::string s = x;
				__gnu_cxx::hash<const char *> H;
				//std::cout << __PRETTY_FUNCTION__ << "    " << s << std::endl;
				//转化字符串为小写
				Zebra::to_lower(s);
				//std::cout << __PRETTY_FUNCTION__ << "    " << s << std::endl;
				return H(s.c_str());
			}
		};

		/**
		 * \brief 判断两个字符串是否相等
		 *
		 */
		struct key_equal : public std::binary_function<const std::string, const std::string, bool>
		{
			bool operator()(const std::string &s1, const std::string &s2) const
			{
				//std::cout << __PRETTY_FUNCTION__ << "    " << s1 << "    " << s2 << std::endl;
				return strcasecmp(s1.c_str(), s2.c_str()) == 0;
			}
		};

		/**
		 * \brief 字符串的hash_map
		 *
		 */
		typedef __gnu_cxx::hash_map<std::string, std::string, key_hash, key_equal> property_hashtype;

		property_hashtype properties;			/**< 保存属性的键值对 */

};

#endif

