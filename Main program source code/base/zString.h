/**
 * \file
 * \version  $Id: zString.h $
 * \author  
 * \date 
 * \brief 封装一些常用的字符串操作
 *
 * 
 */


#ifndef _zString_h_
#define _zString_h_

#include <string>
#include <algorithm>
#include <cctype>


namespace Zebra
{

	/**
	 * \brief 把字符串根据token转化为多个字符串
	 *
	 * 下面是使用例子程序：
	 *    <pre>
	 *    std::list<string> ls;
	 *    stringtok (ls, " this  \t is\t\n  a test  ");
	 *    for(std::list<string>const_iterator i = ls.begin(); i != ls.end(); ++i)
	 *        std::cerr << ':' << (*i) << ":\n";
	 *     </pre>
	 *
	 * \param container 容器，用于存放字符串
	 * \param in 输入字符串
	 * \param delimiters 分隔符号
	 * \param deep 深度，分割的深度，缺省没有限制
	 */
	template <typename Container>
		inline void
		stringtok(Container &container, std::string const &in,
				const char * const delimiters = " \t\n",
				const int deep = 0)
		{
			const std::string::size_type len = in.length();
			std::string::size_type i = 0;
			int count = 0;

			while(i < len)
			{
				i = in.find_first_not_of (delimiters, i);
				if (i == std::string::npos)
					return;   // nothing left

				// find the end of the token
				std::string::size_type j = in.find_first_of (delimiters, i);

				count++;
				// push token
				if (j == std::string::npos
						|| (deep > 0 && count > deep)) {
					container.push_back (in.substr(i));
					return;
				}
				else
					container.push_back (in.substr(i, j-i));

				// set up for next loop
				i = j + 1;

			}
		}

	/**
	 * \brief 把字符转化为小写的函数对象
	 *
	 * 例如：
	 * <pre>
	 * std::string  s ("Some Kind Of Initial Input Goes Here");
	 * std::transform (s.begin(), s.end(), s.begin(), ToLower());
	 * </pre>
	 */
	struct ToLower
	{
		char operator() (char c) const
		{
			return std::tolower(c);
		}
	};

	/**
	 * \brief 把字符串转化为小写
	 * 
	 * 把输入的字符串转化为小写
	 *
	 * \param s 需要转化的字符串
	 */
	inline void to_lower(std::string &s)
	{
		std::transform(s.begin(), s.end(), s.begin(), ToLower());
	}

	/**
	 * \brief 把字符转化为大写的函数对象
	 *
	 * 例如：
	 * <pre>
	 * std::string  s ("Some Kind Of Initial Input Goes Here");
	 * std::transform (s.begin(), s.end(), s.begin(), ToUpper());
	 * </pre>
	 */
	struct ToUpper
	{
		char operator() (char c) const
		{
			return std::toupper(c);
		}
	};

	/**
	 * \brief 把字符串转化为大写
	 * 
	 * 把输入的字符串转化为大写
	 *
	 * \param s 需要转化的字符串
	 */
	inline void to_upper(std::string &s)
	{
		std::transform(s.begin(), s.end(), s.begin(), ToUpper());
	}

}

#endif

