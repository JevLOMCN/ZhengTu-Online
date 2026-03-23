/**
 * \file
 * \version  $Id: zUrl.cpp  $
 * \author  
 * \date 
 * \brief 封装一些url操作函数
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "zUrl.h"

namespace Zebra
{
	/// 十六进制对应的字符
	static unsigned char hexchars[] = "0123456789ABCDEF";

	/**
	 * \brief 对url特殊字符进行编码
	 * \param s 输入字符串
	 * \param len 输入字符串长度
	 * \param new_length 输出字符串长度
	 * \return 输出编码后的url字符串，这段内存再使用完成以后需要释放
	 */
	char *url_encode(const char *s, int len, int *new_length)
	{
		register int x, y;
		unsigned char *str;

		str = (unsigned char *)malloc(3 * len + 1);
		for(x = 0, y = 0; len--; x++, y++)
		{
			str[y] = (unsigned char) s[x];
			if (str[y] == ' ')
			{
				str[y] = '+';
			}
			else if ((str[y] < '0' && str[y] != '-' && str[y] != '.')
					|| (str[y] < 'A' && str[y] > '9')
					|| (str[y] > 'Z' && str[y] < 'a' && str[y] != '_')
					|| (str[y] > 'z'))
			{
				str[y++] = '%';
				str[y++] = hexchars[(unsigned char) s[x] >> 4];
				str[y] = hexchars[(unsigned char) s[x] & 15];
			}
		}
		str[y] = '\0';
		if (new_length) {
			*new_length = y;
		}
		return ((char *) str);
	}

	void url_encode(std::string &s)
	{
		char *buf = url_encode(s.c_str(), s.length(), NULL);
		if (buf)
		{
			s = buf;
			free(buf);
		}
	}

	static inline int htoi(char *s)
	{
		int value;
		int c;

		c = ((unsigned char *)s)[0];
		if (isupper(c))
			c = tolower(c);
		value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

		c = ((unsigned char *)s)[1];
		if (isupper(c))
			c = tolower(c);
		value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

		return (value);
	}

	/**
	 * \brief url字符串解码
	 * \param str 待解码的字符串，同时也作为输出
	 * \param len 待解码字符串的长度
	 * \return 解码以后的字符串长度
	 */
	int url_decode(char *str, int len)
	{
		char *dest = str;
		char *data = str;

		while (len--) {
			if (*data == '+')
				*dest = ' ';
			else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2))) {
				*dest = (char) htoi(data + 1);
				data += 2;
				len -= 2;
			} else
				*dest = *data;
			data++;
			dest++;
		}
		*dest = '\0';
		return dest - str;
	}

	/**
	 * \brief url字符串解码
	 * \param str 待解码的字符串，同时也作为输出
	 */
	void url_decode(std::string &str)
	{
		char buf[str.length() + 1];
		strcpy(buf, str.c_str());
		url_decode(buf, str.length());
		str = buf;
	}
};

